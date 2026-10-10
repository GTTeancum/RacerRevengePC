// Diagnostic-only Windows executable import shim. No environment values logged.
// DLL imports and original source are untouched. Not a performance candidate.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

namespace rr47u {
using Getter=char* (__cdecl*)(const char*);
Getter original() {
    static Getter fn=[] {
        auto p=reinterpret_cast<Getter>(GetProcAddress(GetModuleHandleW(L"ucrtbase.dll"),"getenv"));
        if(!p)std::abort();
        return p;
    }();
    return fn;
}
struct Config {
    char prefix[2048]{};
    uintptr_t base=0;
    Config() {
        const char* p=original()("RR_ENV_ATTRIBUTION47U");
        if(p && *p && std::strlen(p)<sizeof(prefix))std::strcpy(prefix,p);
        base=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
    }
};
// POD output settings outlive worker TLS and the main-thread local scopes.
Config& config(){static Config cfg;return cfg;}
struct Site {uintptr_t caller=0;uint64_t calls=0;char name[128]{};};
struct Counts {
    Site sites[128]{};
    unsigned used=0;
    uint64_t overflow=0;
    bool flushed=false;
    void add(uintptr_t caller,const char* name) {
        if(!name)return;
        for(unsigned i=0;i<used;++i) {
            if(sites[i].caller==caller && std::strcmp(sites[i].name,name)==0){++sites[i].calls;return;}
        }
        if(used==128 || std::strlen(name)>=sizeof(sites[0].name)){++overflow;return;}
        auto& s=sites[used++];s.caller=caller;s.calls=1;std::strcpy(s.name,name);
        for(char* c=s.name;*c;++c)if(*c=='\t'||*c=='\r'||*c=='\n')*c='?';
    }
    void flush() {
        if(flushed)return;flushed=true;
        auto& cfg=config();if(!cfg.prefix[0])return;
        char path[2200];std::snprintf(path,sizeof(path),"%s-%lu.tsv",cfg.prefix,GetCurrentThreadId());
        FILE* f=std::fopen(path,"wb");
        if(!f){std::fprintf(stderr,"[rr:env47u] output failed\n");std::abort();}
        std::fprintf(f,"caller_rva\tcalls\tname\n");
        for(unsigned i=0;i<used;++i)std::fprintf(f,"%llx\t%llu\t%s\n",
            static_cast<unsigned long long>(sites[i].caller-cfg.base),
            static_cast<unsigned long long>(sites[i].calls),sites[i].name);
        std::fprintf(f,"0\t%llu\t<overflow>\n",static_cast<unsigned long long>(overflow));
        if(std::fclose(f)!=0)std::abort();
    }
    ~Counts(){flush();}
};
Counts& counts(){static thread_local Counts c;return c;}
void flushCurrent(){counts().flush();}
}

extern "C" __declspec(noinline) char* __cdecl rr47uGetenv(const char* name) {
    const DWORD incomingError=GetLastError();const int incomingErrno=errno;
    auto real=rr47u::original();errno=incomingErrno;SetLastError(incomingError);
    char* result=real(name);
    const DWORD savedError=GetLastError();const int savedErrno=errno;
    if(rr47u::config().prefix[0])rr47u::counts().add(reinterpret_cast<uintptr_t>(_ReturnAddress()),name);
    errno=savedErrno;SetLastError(savedError);
    return result;
}
// MSVC /MD object calls use this import pointer. A local definition resolves it
// before the CRT import library. The original getter is resolved independently.
extern "C" { rr47u::Getter __imp_getenv=rr47uGetenv; }
