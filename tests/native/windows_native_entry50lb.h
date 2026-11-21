#pragma once
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "native_last_failure50pd.h"
#include "common_race_log50cr.h"   // common_run_summary at the clean exit
#include <exception>
#include <io.h>
#include <fcntl.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Normal Windows player entry (no arguments or --assets) uses one native log.
// Other explicit diagnostic modes retain their previous output/entry behavior.
namespace windows_native_entry50LB {
inline bool automaticEntryActive=false;
inline bool active() noexcept {return automaticEntryActive;}
namespace fs=std::filesystem;
#ifndef RR_WINDOWS_BUILD_ID
#define RR_WINDOWS_BUILD_ID "native-player-next-unversioned"
#endif
inline HANDLE faultLog=INVALID_HANDLE_VALUE;
inline LPTOP_LEVEL_EXCEPTION_FILTER previousFilter=nullptr;
inline LONG WINAPI nativeFaultFilter(EXCEPTION_POINTERS* fault) noexcept {
    // Fixed stack storage and Win32 writes only; no streams, heap, symbols,
    // dumps or attempts to continue execution after a native fault.
    char text[512]{};std::size_t length=0;
    auto literal=[&](const char* value){while(*value&&length+1<sizeof(text))text[length++]=*value++;};
    auto hex=[&](std::uintptr_t value){
        literal("0x");bool emitted=false;
        for(int shift=int(sizeof(value)*8)-4;shift>=0;shift-=4){
            const unsigned digit=unsigned((value>>shift)&15u);
            if(digit||emitted||shift==0){if(length+1<sizeof(text))text[length++]="0123456789abcdef"[digit];emitted=true;}
        }
    };
    literal("\nNATIVE_UNHANDLED_EXCEPTION code=");
    const auto* record=fault?fault->ExceptionRecord:nullptr;
    hex(record?record->ExceptionCode:0);literal(" address=");
    const auto address=record?reinterpret_cast<std::uintptr_t>(record->ExceptionAddress):0;
    hex(address);MEMORY_BASIC_INFORMATION memory{};
    if(address&&VirtualQuery(reinterpret_cast<const void*>(address),&memory,sizeof(memory))==sizeof(memory)&&memory.Type==MEM_IMAGE){
        const auto base=reinterpret_cast<std::uintptr_t>(memory.AllocationBase);
        literal(" image_base=");hex(base);literal(" image_offset=");hex(address-base);
    }
    literal(" thread_id=");hex(GetCurrentThreadId());
    literal(" disposition=EXCEPTION_CONTINUE_SEARCH\n");
    if(faultLog!=INVALID_HANDLE_VALUE){DWORD written=0;WriteFile(faultLog,text,static_cast<DWORD>(length),&written,nullptr);FlushFileBuffers(faultLog);}
    return EXCEPTION_CONTINUE_SEARCH;
}
struct FaultLogLifetime {
    ~FaultLogLifetime(){
        if(faultLog!=INVALID_HANDLE_VALUE){SetUnhandledExceptionFilter(previousFilter);CloseHandle(faultLog);faultLog=INVALID_HANDLE_VALUE;}
    }
};
inline std::string utf8Path(const fs::path& path) {
    const auto bytes=path.u8string();
    return std::string(reinterpret_cast<const char*>(bytes.data()),bytes.size());
}
inline fs::path executableDirectory() {
    std::vector<wchar_t> path(32768);
    const DWORD size=GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()));
    if(!size||size>=path.size())throw std::runtime_error("Cannot determine executable directory");
    const fs::path file(std::wstring(path.data(),size));
    if(!file.is_absolute()||file.parent_path().empty())
        throw std::runtime_error("Executable directory is not absolute");
    return file.parent_path();
}
inline void cleanInheritedOverrides() {
    // Snapshot before editing the environment. _wputenv_s updates CRT and
    // Windows copies together; no registry/system/user settings are changed.
    wchar_t* block=GetEnvironmentStringsW();
    if(!block)throw std::runtime_error("Cannot inspect inherited launch environment");
    std::vector<std::wstring> names;
    try {
        for(const wchar_t* at=block;*at;at+=std::wcslen(at)+1){
            const std::wstring item(at);const auto equal=item.find(L'=');
            if(equal==std::wstring::npos||equal==0)continue;
            const auto name=item.substr(0,equal);
            if(_wcsnicmp(name.c_str(),L"RR_",3)==0||_wcsnicmp(name.c_str(),L"SDL_",4)==0||
               _wcsnicmp(name.c_str(),L"MESA_",5)==0||_wcsnicmp(name.c_str(),L"LIBGL_",6)==0)
                names.push_back(name);
        }
    }catch(...){FreeEnvironmentStringsW(block);throw;}
    FreeEnvironmentStringsW(block);
    for(const auto& name:names)
        if(_wputenv_s(name.c_str(),L"")!=0)throw std::runtime_error("Cannot clear inherited diagnostic override");
    if(_wputenv_s(L"RR_NATIVE_FIXED_WINDOW",L"0")!=0||
       _wputenv_s(L"RR_ORIGINAL_RIDER_PRESENTATION",L"allow-native-pc-deadline60hz")!=0)
        throw std::runtime_error("Cannot set normal native presentation policy");
}
inline void redirectToLog(const fs::path& path) {
    // OPEN_ALWAYS never truncates the user's previous sessions. FILE_SHARE_READ
    // permits readers but denies another writer for this process's lifetime.
    HANDLE file=CreateFileW(path.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,
        OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file==INVALID_HANDLE_VALUE)
        throw std::runtime_error("Cannot open RacerRevenge.log in the game folder. The folder may be read-only, or another game/log writer may already have it open. No alternate log location was used");
    LARGE_INTEGER zero{};
    if(!SetFilePointerEx(file,zero,nullptr,FILE_END)){
        CloseHandle(file);throw std::runtime_error("Cannot append to RacerRevenge.log");
    }
    // GUI-subsystem startup may expose invalid CRT descriptors. Reopen the
    // actual CRT streams on NUL first, without creating or attaching a console.
    if(!_wfreopen(L"NUL",L"wb",stdout)||!_wfreopen(L"NUL",L"wb",stderr)){
        CloseHandle(file);throw std::runtime_error("Cannot initialize GUI stdout/stderr streams");
    }
    HANDLE crashHandle=INVALID_HANDLE_VALUE;
    if(!DuplicateHandle(GetCurrentProcess(),file,GetCurrentProcess(),&crashHandle,0,FALSE,DUPLICATE_SAME_ACCESS)){
        CloseHandle(file);throw std::runtime_error("Cannot retain native diagnostic handle");
    }
    const int descriptor=_open_osfhandle(reinterpret_cast<std::intptr_t>(file),_O_WRONLY|_O_BINARY|_O_APPEND);
    if(descriptor<0){CloseHandle(crashHandle);CloseHandle(file);throw std::runtime_error("Cannot attach native log handle");}
    // _dup2 uses duplicate handles to the SAME file object/current position.
    // Both standard streams share the writer lock and append destination.
    std::fflush(stdout);std::fflush(stderr);
    const bool okay=_dup2(descriptor,_fileno(stdout))==0&&_dup2(descriptor,_fileno(stderr))==0;
    _close(descriptor);
    if(!okay){CloseHandle(crashHandle);throw std::runtime_error("Cannot redirect stdout and stderr to RacerRevenge.log");}
    // Windows-level writers and the CRT now address the same locked log.
    if(!SetStdHandle(STD_OUTPUT_HANDLE,reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(stdout))))||
       !SetStdHandle(STD_ERROR_HANDLE,reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(stderr))))){
        CloseHandle(crashHandle);throw std::runtime_error("Cannot publish native standard log handles");
    }
    faultLog=crashHandle;previousFilter=SetUnhandledExceptionFilter(nativeFaultFilter);
    std::setvbuf(stdout,nullptr,_IONBF,0);std::setvbuf(stderr,nullptr,_IONBF,0);
    std::cout.clear();std::cerr.clear();
    std::cout.setf(std::ios::unitbuf);std::cerr.setf(std::ios::unitbuf);
}
inline void sessionMarker(const char* stage) {
    SYSTEMTIME now{};GetSystemTime(&now);
    std::fprintf(stderr,"\n========== RacerRevenge %s UTC %04u-%02u-%02u %02u:%02u:%02u.%03u PID %lu ==========\n",
        stage,unsigned(now.wYear),unsigned(now.wMonth),unsigned(now.wDay),unsigned(now.wHour),
        unsigned(now.wMinute),unsigned(now.wSecond),unsigned(now.wMilliseconds),
        static_cast<unsigned long>(GetCurrentProcessId()));
}
inline void visibleFailure(const std::wstring& detail,const fs::path& log) noexcept {
    // Explicit process-local test mode reports failure through the log and exit code.
    const char* unattended=std::getenv("RR_UNATTENDED_CAPTURE");
    if(unattended&&std::strcmp(unattended,"1")==0){
        std::fprintf(stderr,"native_failure_dialog_suppressed=unattended_capture\n");
        return;
    }
    try {
        std::wstring message=detail;
        if(!log.empty())message+=L"\n\nLog file: RacerRevenge.log\nIn the same folder as the game EXE (if startup could open it)."
            L"\n\nClose the game before opening or sharing the log. Nothing is uploaded automatically.";
        MessageBoxW(nullptr,message.c_str(),L"Star Wars: Racer Revenge - startup / session error",MB_OK|MB_ICONERROR|MB_SETFOREGROUND);
    }catch(...){MessageBoxW(nullptr,L"Star Wars: Racer Revenge could not finish startup. Check the original files and RacerRevenge.log in the game folder.",
        L"Star Wars: Racer Revenge",MB_OK|MB_ICONERROR);}
}
inline std::wstring readableError(const char* text) {
    if(!text)return L"Unknown native startup error.";
    const int count=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,-1,nullptr,0);
    if(count<=0)return L"Native startup failed; see RacerRevenge.log for details.";
    std::wstring result(static_cast<std::size_t>(count),L'\0');
    MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,-1,result.data(),count);
    result.resize(static_cast<std::size_t>(count)-1);return result;
}
// Player video settings: RacerRevengePC.ini beside the EXE. Written with
// defaults when absent; read before any window or renderer exists and
// published as process settings. A value already set for this process (an
// explicit diagnostic/test launch) is kept, never overridden by the file.
inline void applyVideoSettings(const fs::path& directory) {
    const auto ini=directory/L"RacerRevengePC.ini";
    std::error_code error;bool created=false;
    if(!fs::exists(ini,error)&&!error){
        std::ofstream out(ini,std::ios::binary|std::ios::trunc);
        out<<"; Racer Revenge PC video settings. Edit while the game is closed.\r\n"
             "; Delete this file to restore the defaults.\r\n"
             "[Video]\r\n"
             "; 1 = open the window maximized. 0 = open it at WindowWidth x WindowHeight.\r\n"
             "Maximized=1\r\n"
             "; Window size when not maximized (640-3840 x 448-2160).\r\n"
             "WindowWidth=1280\r\n"
             "WindowHeight=896\r\n"
             "; 1 = widescreen: a wider window shows more to the sides. 0 = original 4:3 picture.\r\n"
             "Widescreen=1\r\n"
             "; 1 = FXAA edge smoothing on the 3D scene (HUD and menus stay sharp). 0 = off.\r\n"
             "FXAA=1\r\n";
        out.close();created=!out.fail();
        if(!created)std::cerr<<"video_settings_write_failed file="<<utf8Path(ini)<<" (defaults used)\n";
    }
    const auto read=[&](const wchar_t* key,const char* name,int fallback,int low,int high){
        const int value=static_cast<int>(GetPrivateProfileIntW(L"Video",key,fallback,ini.c_str()));
        if(value<low||value>high){
            std::cerr<<"video_settings_invalid key="<<name<<" value="<<value<<" (default used)\n";
            return fallback;
        }
        return value;
    };
    const int maximized=read(L"Maximized","Maximized",1,0,1),
        width=read(L"WindowWidth","WindowWidth",1280,640,3840),
        height=read(L"WindowHeight","WindowHeight",896,448,2160),
        widescreen=read(L"Widescreen","Widescreen",1,0,1),fxaa=read(L"FXAA","FXAA",1,0,1);
    const auto publish=[&](const char* name,const std::string& value){
        if(std::getenv(name))return;
        if(_putenv_s(name,value.c_str())!=0)throw std::runtime_error("Cannot apply video settings");
    };
    publish("RR_VIDEO_MAXIMIZED",maximized?"1":"0");
    publish("RR_VIDEO_WINDOW_SIZE",std::to_string(width)+"x"+std::to_string(height));
    if(!widescreen)publish("RR_ORIGINAL_ASPECT","1");
    publish("RR_VIDEO_FXAA",fxaa?"1":"0");
    const auto value=[](const char* name){const char* v=std::getenv(name);return std::string(v?v:"");};
    std::cerr<<"video_settings file="<<utf8Path(ini)<<" created="<<created
        <<" maximized="<<value("RR_VIDEO_MAXIMIZED")<<" window="<<value("RR_VIDEO_WINDOW_SIZE")
        <<" widescreen="<<(value("RR_ORIGINAL_ASPECT")=="1"?0:1)<<" fxaa="<<value("RR_VIDEO_FXAA")<<'\n';
}
// PC enhancement (NOT ORIGINAL): DLC switches from RacerRevengePC.ini [DLC].
// Read only when the keys are present; nothing is ever written for DLC, so a
// file without a [DLC] section behaves exactly as before (DLC on, replacements
// on). Enabled=0 ignores the DLC folder; Replacements=0 turns every flagged
// replacement off at once (the master switch). Process values win.
inline void applyDlcSettings(const fs::path& directory) {
    const auto ini=directory/L"RacerRevengePC.ini";
    std::error_code error;
    if(!fs::is_regular_file(ini,error))return;
    const auto publish=[&](const wchar_t* key,const char* name){
        const int value=static_cast<int>(GetPrivateProfileIntW(L"DLC",key,-1,ini.c_str()));
        if(value==-1||std::getenv(name))return;
        if(value!=0&&value!=1){std::cerr<<"dlc_settings_invalid key="<<name<<" value="<<value<<" (ignored)\n";return;}
        if(_putenv_s(name,value?"1":"0")!=0)throw std::runtime_error("Cannot apply DLC settings");
        std::cerr<<"dlc_settings "<<name<<"="<<value<<'\n';
    };
    publish(L"Enabled","RR_DLC");
    publish(L"Replacements","RR_DLC_REPLACEMENTS");
}
inline fs::path& terminateLogPath()noexcept{static fs::path path;return path;}
// A failure that ends in std::terminate (strict retained-owner guards during
// unwinding) reports its last recorded reason instead of closing silently.
[[noreturn]] inline void reportTerminate()noexcept {
    const char* reason=native_last_failure50PD::last();
    std::fprintf(stderr,"native_terminate last_failure=%s\n",reason?reason:"(none recorded)");
    {   // Return addresses as module offsets (resolve with the link map).
        void* frames[48]{};const auto count=CaptureStackBackTrace(0,48,frames,nullptr);
        const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
        std::fprintf(stderr,"native_terminate_stack base=%p",reinterpret_cast<void*>(base));
        for(unsigned i=0;i<count;++i)std::fprintf(stderr," +%llx",
            static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(frames[i])-base));
        std::fputc('\n',stderr);
    }
    sessionMarker("SESSION END exit=terminate");std::fflush(stderr);
    try {
        visibleFailure(L"The game stopped while loading or running a race:\n"+
            (reason?readableError(reason):std::wstring(L"unknown native failure"))+
            L"\n\nRacerRevenge.log contains the details.",terminateLogPath());
    }catch(...){}
    std::abort();
}
template<class Entry> int run(Entry entry,int argc,char** argv,bool automatic) {
    fs::path log;bool logging=false;FaultLogLifetime faultLifetime;
    try {
        const auto assets=executableDirectory();log=assets/L"RacerRevenge.log";
        redirectToLog(log);logging=true;sessionMarker("SESSION START");
        terminateLogPath()=log;std::set_terminate(&reportTerminate);
        std::cerr<<"build_id="<<RR_WINDOWS_BUILD_ID<<" compiled="<<__DATE__<<" "<<__TIME__<<'\n';
        std::cerr<<"native_player_entry="<<(automatic?"doubleclick":"explicit_assets")<<" executable_directory="<<utf8Path(assets)
            <<" window=1280x896 source_canvas=640x448 telemetry=false\n";
        if(automatic)cleanInheritedOverrides();
        else {
            if(!std::getenv("RR_ORIGINAL_RIDER_PRESENTATION")&&
               _putenv_s("RR_ORIGINAL_RIDER_PRESENTATION","allow-native-pc-deadline60hz")!=0)
                throw std::runtime_error("Cannot set normal Rider presentation policy");
        }
#if defined(RR_WINDOWS_EMBEDDED_ICON)
        // SDL reads these native resource IDs before creating its one window.
        // The same compiled ICO also serves Explorer's executable icon.
        if(_putenv_s("SDL_WINDOWS_INTRESOURCE_ICON","101")!=0||
           _putenv_s("SDL_WINDOWS_INTRESOURCE_ICON_SMALL","101")!=0)
            throw std::runtime_error("Cannot select embedded application icon");
        std::cerr<<"native_application_icon_resource=101 embedded=true\n";
#endif
        std::cerr<<"rider_presentation="<<std::getenv("RR_ORIGINAL_RIDER_PRESENTATION")<<'\n';
        applyVideoSettings(assets);
        applyDlcSettings(assets);
        // Exact wide directory only, never cwd/PATH search or fallback assets.
        // ASCII relative roots avoid the legacy narrow argv/environment seam.
        if(automatic&&!SetCurrentDirectoryW(assets.c_str()))throw std::runtime_error("Cannot open executable directory as original asset root");
        if(automatic)for(const wchar_t* relative:{L"SLUS_202.68",L"APPSTART.RES",L"LOC.RES",L"UI/UIFONTS.RES",L"UI/UIMAIN/UIMAIN.RES"}){
            std::error_code error;
            if(!fs::is_regular_file(assets/relative,error)){
                std::cerr<<"native_player_missing_original_asset="<<utf8Path(fs::path(relative))<<'\n';
                sessionMarker("SESSION END exit=1 missing-assets");
                visibleFailure(L"An original disc file is missing or unreadable:\n"+std::wstring(relative)+
                    L"\n\nPlace this native EXE and its required DLLs in the folder containing your intact original disc files (including SLUS_202.68, APPSTART.RES, LOC.RES, UI and AUDIO). Do not execute or rename SLUS_202.68. No replacement assets are downloaded or substituted.",log);
                return 1;
            }
        }
        automaticEntryActive=automatic;
        char program[]="RacerRevengePC.exe",option[]="--assets",root[]=".";
        char* arguments[]={program,option,root,nullptr};
        std::cerr<<"native_player_entering_normal_original_menu\n";
        const int result=automatic?entry(3,arguments):entry(argc,argv);automaticEntryActive=false;
        std::cerr<<"native_exit_code="<<result<<'\n';
        // Test logging (common rewrite R11, PLAN 4/5.5): the one common_run_summary line, at the
        // clean process exit only (a terminate or startup error never reaches here). result= and
        // first_refusal= come from the run's refusal record (any common_load/teardown refusal
        // makes it fail). Course, mode, pods, laps, routes and FPS are per race and stay on the
        // race's own lines (common_race_start, common_lap, common_race_end, race_fps_summary);
        // this line names none of them. It was defined but never emitted (cr-17).
        if(result==0)common_race_log50CR::runSummary({.course="-",.mode="-",.routes="-"});
        sessionMarker("SESSION END");
        std::fflush(stdout);std::fflush(stderr);
        if(result!=0)visibleFailure(L"The native game stopped with exit code "+std::to_wstring(result)+
            L". RacerRevenge.log contains startup, selection, loading and error diagnostics.",log);
        return result;
    }catch(const std::exception& error){
        automaticEntryActive=false;
        if(logging){std::cerr<<"native_player_failure: "<<error.what()<<'\n';sessionMarker("SESSION END exit=1 startup-error");}
        visibleFailure(readableError(error.what()),log);return 1;
    }catch(...){
        automaticEntryActive=false;
        if(logging){std::cerr<<"native_player_failure: non-standard exception\n";sessionMarker("SESSION END exit=1 startup-error");}
        visibleFailure(L"An unexpected native startup failure occurred.",log);return 1;
    }
}
} // namespace windows_native_entry50LB
#endif
