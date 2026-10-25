#pragma once
// Native user-data IO only. This is not the original memory-card format.
#include <cstdint>
#include <atomic>
#include <cstdlib>
#include <cerrno>
#include <filesystem>
#include <vector>
#include <string>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#endif
namespace rr::platform::native_beta50kg {
enum class Result {ok,missing,invalid,ioError,unavailable,durabilityUnknown};
using Bytes=std::vector<std::uint8_t>;
#ifdef _WIN32
// Unicode environment access, including explicitly empty overrides. No fallback
// to the executable directory (which can be read-only or shared across users).
inline std::wstring environment(const wchar_t* name,bool& present) {
    ::SetLastError(ERROR_SUCCESS);
    const DWORD count=::GetEnvironmentVariableW(name,nullptr,0);
    present=count!=0||::GetLastError()!=ERROR_ENVVAR_NOT_FOUND;
    if(!count)return {};
    std::wstring value(count,L'\0');
    const DWORD copied=::GetEnvironmentVariableW(name,value.data(),count);
    if(!copied||copied>=count)return {};
    value.resize(copied);return value;
}
#endif
inline std::filesystem::path defaultDirectory() {
#ifdef _WIN32
    bool present=false;auto value=environment(L"RR_NATIVE_SAVE_DIR",present);
    if(present)return std::filesystem::path(value);
    value=environment(L"LOCALAPPDATA",present);
    if(present&&!value.empty()&&std::filesystem::path(value).is_absolute())
        return std::filesystem::path(value)/L"RacerRevengePC"/L"Saves";
    return {};
#else
    if(const auto* p=std::getenv("RR_NATIVE_SAVE_DIR"))return *p?std::filesystem::path(p):std::filesystem::path{};
    if(const auto* p=std::getenv("XDG_DATA_HOME"))if(*p&&std::filesystem::path(p).is_absolute())return std::filesystem::path(p)/"racer-revenge";
    if(const auto* p=std::getenv("HOME"))if(*p)return std::filesystem::path(p)/".local/share/racer-revenge";
    return {};
#endif
}
#ifndef _WIN32
struct Fd {int value=-1;explicit Fd(int v):value(v){} ~Fd(){if(value>=0)::close(value);} Fd(const Fd&)=delete;Fd& operator=(const Fd&)=delete;};
inline Result readFile(const std::filesystem::path& path,Bytes& out) noexcept {
 try {
    Fd fd(::open(path.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK));
    if(fd.value<0)return errno==ENOENT?Result::missing:Result::ioError;
    struct stat st{};if(::fstat(fd.value,&st)||!S_ISREG(st.st_mode)||st.st_size<0||st.st_size>1024*1024)return Result::invalid;
    Bytes next(static_cast<std::size_t>(st.st_size));std::size_t at=0;
    while(at<next.size()){auto n=::read(fd.value,next.data()+at,next.size()-at);if(n<0&&errno==EINTR)continue;if(n<=0)return Result::ioError;at+=static_cast<std::size_t>(n);}
    std::uint8_t extra;ssize_t n;do{n=::read(fd.value,&extra,1);}while(n<0&&errno==EINTR);
    if(n!=0)return Result::invalid;
    out.swap(next);return Result::ok;
 }catch(...){return Result::ioError;}
}
inline Result writeFile(const std::filesystem::path& path,const Bytes& bytes) noexcept {
 std::string temporary;bool renamed=false;
 try {
    if(bytes.size()>1024*1024||path.filename().empty())return Result::invalid;
    const auto directory=path.parent_path();if(directory.empty())return Result::unavailable;
    std::error_code ec;std::filesystem::create_directories(directory,ec);if(ec)return Result::ioError;
    Fd dir(::open(directory.c_str(),O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW));if(dir.value<0)return Result::ioError;
    const auto name=path.filename().string();struct stat current{};
    if(::fstatat(dir.value,name.c_str(),&current,AT_SYMLINK_NOFOLLOW)==0){if(!S_ISREG(current.st_mode))return Result::invalid;}
    else if(errno!=ENOENT)return Result::ioError;
    // O_EXCL prevents following a stale/malicious temporary-file symlink.
    static std::atomic<std::uint64_t> serial{0};int raw=-1;
    for(unsigned i=0;i<64&&raw<0;++i){temporary=name+".tmp."+std::to_string(::getpid())+"."+std::to_string(++serial);raw=::openat(dir.value,temporary.c_str(),O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);if(raw<0&&errno!=EEXIST)return Result::ioError;}
    if(raw<0)return Result::ioError;Fd fd(raw);
    const auto cleanup=[&](){::unlinkat(dir.value,temporary.c_str(),0);};
    std::size_t at=0;while(at<bytes.size()){auto n=::write(fd.value,bytes.data()+at,bytes.size()-at);if(n<0&&errno==EINTR)continue;if(n<=0){cleanup();return Result::ioError;}at+=static_cast<std::size_t>(n);}
    if(::fsync(fd.value)){cleanup();return Result::ioError;}
    if(::close(fd.value)){fd.value=-1;cleanup();return Result::ioError;}fd.value=-1;
    if(::renameat(dir.value,temporary.c_str(),dir.value,name.c_str())){cleanup();return Result::ioError;}
    renamed=true;
    return ::fsync(dir.value)==0?Result::ok:Result::durabilityUnknown;
 }catch(...){return renamed?Result::durabilityUnknown:Result::ioError;}
}
#else
struct Handle final {
    HANDLE value=INVALID_HANDLE_VALUE;
    explicit Handle(HANDLE v=INVALID_HANDLE_VALUE):value(v){}
    ~Handle(){if(value!=INVALID_HANDLE_VALUE)::CloseHandle(value);}
    Handle(const Handle&)=delete;Handle& operator=(const Handle&)=delete;
};
inline bool regularFile(HANDLE handle) noexcept {
    BY_HANDLE_FILE_INFORMATION info{};
    return ::GetFileType(handle)==FILE_TYPE_DISK&&::GetFileInformationByHandle(handle,&info)&&
        !(info.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT));
}
inline Result readFile(const std::filesystem::path& path,Bytes& out) noexcept {
 try {
    // Deny writers/deletion for the duration of validation and bounded reading.
    Handle file(::CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,
        OPEN_EXISTING,FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_SEQUENTIAL_SCAN,nullptr));
    if(file.value==INVALID_HANDLE_VALUE){const auto error=::GetLastError();
        return error==ERROR_FILE_NOT_FOUND||error==ERROR_PATH_NOT_FOUND?Result::missing:Result::ioError;}
    if(!regularFile(file.value))return Result::invalid;
    LARGE_INTEGER size{};
    if(!::GetFileSizeEx(file.value,&size))return Result::ioError;
    if(size.QuadPart<0||size.QuadPart>1024*1024)return Result::invalid;
    Bytes next(static_cast<std::size_t>(size.QuadPart));std::size_t at=0;
    while(at<next.size()) {DWORD n=0;
        if(!::ReadFile(file.value,next.data()+at,static_cast<DWORD>(next.size()-at),&n,nullptr)||!n)return Result::ioError;
        at+=n;
    }
    std::uint8_t extra=0;DWORD n=0;
    if(!::ReadFile(file.value,&extra,1,&n,nullptr))return Result::ioError;
    if(n)return Result::invalid;
    out.swap(next);return Result::ok;
 }catch(...){return Result::ioError;}
}
inline Result writeFile(const std::filesystem::path& path,const Bytes& bytes) noexcept {
 // Temporary cleanup runs after its handle closes, including exception paths.
 struct Temporary {std::filesystem::path path;bool owned=false;
     ~Temporary(){if(owned)::DeleteFileW(path.c_str());}} temporary;
 bool replaced=false;
 try {
    if(bytes.size()>1024*1024||path.filename().empty())return Result::invalid;
    const auto directory=path.parent_path();if(directory.empty())return Result::unavailable;
    std::error_code ec;std::filesystem::create_directories(directory,ec);if(ec)return Result::ioError;
    Handle dir(::CreateFileW(directory.c_str(),FILE_READ_ATTRIBUTES,FILE_SHARE_READ|FILE_SHARE_WRITE,
        nullptr,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OPEN_REPARSE_POINT,nullptr));
    BY_HANDLE_FILE_INFORMATION info{};
    if(dir.value==INVALID_HANDLE_VALUE||!::GetFileInformationByHandle(dir.value,&info))return Result::ioError;
    if(!(info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)||(info.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT))return Result::invalid;
    const DWORD attributes=::GetFileAttributesW(path.c_str());
    if(attributes!=INVALID_FILE_ATTRIBUTES){
        if(attributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))return Result::invalid;
    }else if(::GetLastError()!=ERROR_FILE_NOT_FOUND)return Result::ioError;
    static std::atomic<std::uint64_t> serial{0};Handle file;
    for(unsigned i=0;i<64;++i){
        temporary.path=directory/(path.filename().wstring()+L".tmp."+
            std::to_wstring(::GetCurrentProcessId())+L"."+std::to_wstring(++serial));
        file.value=::CreateFileW(temporary.path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OPEN_REPARSE_POINT,nullptr);
        if(file.value!=INVALID_HANDLE_VALUE){temporary.owned=true;break;}
        if(::GetLastError()!=ERROR_FILE_EXISTS&&::GetLastError()!=ERROR_ALREADY_EXISTS)return Result::ioError;
    }
    if(file.value==INVALID_HANDLE_VALUE)return Result::ioError;
    std::size_t at=0;while(at<bytes.size()){DWORD n=0;
        if(!::WriteFile(file.value,bytes.data()+at,static_cast<DWORD>(bytes.size()-at),&n,nullptr)||!n)return Result::ioError;
        at+=n;
    }
    if(!::FlushFileBuffers(file.value))return Result::ioError;
    if(!::CloseHandle(file.value))return Result::ioError;
    file.value=INVALID_HANDLE_VALUE;
    // One same-directory replacement, never delete the destination first and
    // never permit a cross-volume copy/delete fallback. The session lock is held.
    if(!::MoveFileExW(temporary.path.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))return Result::ioError;
    replaced=true;temporary.owned=false;
    // Windows success contract: file data flushed and same-volume replacement
    // completed with WRITE_THROUGH. This is not a claim of POSIX directory-fsync
    // equivalence or a guarantee against all device/filesystem power-loss modes.
    return Result::ok;
 }catch(...){return replaced?Result::durabilityUnknown:Result::ioError;}
}
#endif
// Separate sentinel file, held for the store lifetime. Never remove a live lock
// file: another process must contend for the same file identity.
class ProcessLock final {
public:
    ProcessLock()=default;
    ProcessLock(const ProcessLock&)=delete;ProcessLock& operator=(const ProcessLock&)=delete;
    ~ProcessLock(){
#ifdef _WIN32
        if(file_!=INVALID_HANDLE_VALUE){if(held_){OVERLAPPED offset{};::UnlockFileEx(file_,0,1,0,&offset);}::CloseHandle(file_);}
#else
        if(file_>=0)::close(file_);
#endif
    }
    bool held()const noexcept{return held_;}
    Result acquire(const std::filesystem::path& path) noexcept {
        if(held_)return Result::ok;
#ifdef _WIN32
        if(file_==INVALID_HANDLE_VALUE)file_=::CreateFileW(path.c_str(),GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_ALWAYS,FILE_FLAG_OPEN_REPARSE_POINT,nullptr);
        if(file_==INVALID_HANDLE_VALUE)return Result::ioError;
        OVERLAPPED offset{};
        if(!regularFile(file_)||!::LockFileEx(file_,LOCKFILE_EXCLUSIVE_LOCK|LOCKFILE_FAIL_IMMEDIATELY,0,1,0,&offset))return Result::unavailable;
#else
        if(file_<0)file_=::open(path.c_str(),O_RDWR|O_CREAT|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK,0600);
        if(file_<0)return Result::ioError;
        struct stat info{};
        if(::fstat(file_,&info)||!S_ISREG(info.st_mode)||::flock(file_,LOCK_EX|LOCK_NB))return Result::unavailable;
#endif
        held_=true;return Result::ok;
    }
private:
#ifdef _WIN32
    HANDLE file_=INVALID_HANDLE_VALUE;
#else
    int file_=-1;
#endif
    bool held_=false;
};
// Stable little-endian field encoding, no object padding or pointers.
inline void put32(Bytes& b,std::uint32_t n){for(unsigned i=0;i<4;++i)b.push_back(static_cast<std::uint8_t>(n>>(8*i)));}
inline std::uint32_t checksum(const Bytes& b,std::size_t begin=0)noexcept{
 std::uint32_t crc=0xffffffffu;for(std::size_t i=begin;i<b.size();++i){crc^=b[i];for(unsigned j=0;j<8;++j)crc=(crc>>1)^(0xedb88320u&(~(crc&1u)+1u));}return ~crc;
}
}
