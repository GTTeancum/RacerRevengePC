#include "ps2_runtime.h"
#include "runtime/ee_scheduler.h"
#include "ps2_runtime_macros.h"
#include <atomic>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
namespace {
constexpr uint32_t entry=0x100008,alarmEntry=0x100100,gsEntry=0x100200,vifEntry=0x100300;
unsigned calls=0,alarms=0,gsCalls=0,vifCalls=0;std::vector<uint64_t> trace;
std::atomic<bool> request{false},posted{false};
void alarm(uint8_t*,R5900Context* c,PS2Runtime* r){++alarms;trace.push_back(0xa000000000000000ull|r->eeScheduler().observerEeCycle33h());SET_GPR_U64(c,2,0);c->pc=0;}
void gs(uint8_t*,R5900Context* c,PS2Runtime* r){++gsCalls;trace.push_back(0xb000000000000000ull|r->eeScheduler().observerEeCycle33h());r->memory().gs().csr.fetch_and(~1ull);SET_GPR_U64(c,2,0);c->pc=0;}
void vif(uint8_t*,R5900Context* c,PS2Runtime* r){++vifCalls;trace.push_back(0xc000000000000000ull|r->eeScheduler().observerEeCycle33h());r->memory().vif1_regs.stat&=~(1u<<11);SET_GPR_U64(c,2,0);c->pc=0;}
void mainGuest(uint8_t*,R5900Context* c,PS2Runtime* r){
    ++calls;auto& s=r->eeScheduler();
    if(calls==4)s.setIrqCauseEnabled(false,0,true);
    if(calls==16)s.setIrqCauseEnabled(false,5,true);
    s.accountCycles(256);
    SET_GPR_U64(c,8,calls);c->pc=entry;
    if(calls==2000){request.store(true,std::memory_order_release);while(!posted.load(std::memory_order_acquire))std::this_thread::yield();}
    if(calls>2000)throw std::runtime_error("posted stop was lost");
}
}
int main(){try{
    PS2Runtime runtime;if(!runtime.memory().initialize()||!runtime.syncCoreSubsystems())throw std::runtime_error("core initialization");
    runtime.setMissingFunctionPolicy(PS2Runtime::MissingFunctionPolicy::Stop);
    for(const auto& p:std::vector<std::pair<uint32_t,PS2Runtime::RecompiledFunction>>{{entry,mainGuest},{alarmEntry,alarm},{gsEntry,gs},{vifEntry,vif}})
        if(!runtime.registerFunction(p.first,p.second))throw std::runtime_error("fixture registration");
    auto& s=runtime.eeScheduler();R5900Context initial{};initial.pc=entry;SET_GPR_U32(&initial,29,0x1ff0000);s.reset(runtime.memory().getRDRAM(),initial);
    const int g=s.addIrqHandler(false,0,gsEntry,true,0,0,0x1fe0000),v=s.addIrqHandler(false,5,vifEntry,true,0,0,0x1fe0000);
    if(g<0||v<0)throw std::runtime_error("IRQ registration");s.setIrqHandlerEnabled(false,g,true);s.setIrqHandlerEnabled(false,v,true);
    s.setIrqCauseEnabled(false,0,false);s.setIrqCauseEnabled(false,5,false);
    runtime.memory().gs().imr=0;runtime.memory().gs().csr.fetch_or(1ull);runtime.memory().vif1_regs.stat|=1u<<11;
    if(s.setAlarm(0,alarmEntry,0,0,0x1fe0000)<0||s.setAlarm(1,alarmEntry,0,0,0x1fe0000)<0)throw std::runtime_error("alarm registration");
    std::jthread poster([&](std::stop_token stop){while(!request.load(std::memory_order_acquire)){if(stop.stop_requested())return;std::this_thread::yield();}s.postEvent({EeEventType::Stop,0,0});posted.store(true,std::memory_order_release);});
    s.run();poster.join();
    if(calls!=2000||alarms!=2||gsCalls!=1||vifCalls!=1)throw std::runtime_error("event count mismatch");
    std::cout<<"EVENT47S calls="<<calls<<" alarm="<<alarms<<" gs="<<gsCalls<<" vif="<<vifCalls<<" cycles="<<s.observerEeCycle33h()<<" trace=";
    for(auto e:trace)std::cout<<std::hex<<e<<',';std::cout<<std::dec<<'\n';return 0;
}catch(const std::exception& e){std::cerr<<"fixture failure: "<<e.what()<<'\n';return 1;}}
