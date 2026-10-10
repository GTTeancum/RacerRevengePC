#include <cstdlib>
#include <cstring>
#include <thread>
#include <cstdio>
namespace rr47u {void flushCurrent();}
__declspec(noinline) int query(unsigned count) {
    for(unsigned i=0;i<count;++i) {
        const char* p=std::getenv("RR_ENV47U_TEST_VALUE");
        if(!p || std::strcmp(p,"preserved"))return 1;
        if(std::getenv("RR_ENV47U_TEST_ABSENT")!=nullptr)return 2;
    }
    return 0;
}
int main() {
    if(_putenv_s("RR_ENV47U_TEST_VALUE","preserved") || _putenv_s("RR_ENV47U_TEST_ABSENT",""))return 3;
    int child=9;
    std::thread t([&]{child=query(300);});
    int mainResult=query(700);t.join();
    rr47u::flushCurrent();
    if(child || mainResult)return 4;
    std::puts("ENV47U original return values preserved across 2000 lookups / two threads");
    return 0;
}
