#include "memory_env47w.h"
#include <cstdio>
#include <cstring>
const char* present(){return RR47W_MEMORY_ENV("RR_TRACE_TEST47W_PRESENT");}
const char* absent(){return RR47W_MEMORY_ENV("RR_TRACE_TEST47W_ABSENT");}
int main(int argc,char** argv){
    if(argc!=2)return 1;const bool cached=argv[1][0]=='1';
    if(rr47w::cacheDiagnostics()!=cached)return 2;
    if(_putenv_s("RR_TRACE_TEST47W_PRESENT","first") || _putenv_s("RR_TRACE_TEST47W_ABSENT",""))return 3;
    const char* retained=present();if(!retained || std::strcmp(retained,"first") || absent())return 4;
    for(unsigned i=0;i<100;++i)if(_putenv_s("RR_TRACE_TEST47W_PRESENT",i&1?"second":"third"))return 5;
    if(_putenv_s("RR_TRACE_TEST47W_ABSENT","added"))return 6;
    if(std::strcmp(present(),cached?"first":"second"))return 7;
    if(cached){if(absent() || std::strcmp(retained,"first"))return 8;}
    else if(!absent() || std::strcmp(absent(),"added"))return 9;
    std::printf("MEMORY47W mode=%d owned-present and cached-absence checks passed\n",cached);
    return 0;
}
