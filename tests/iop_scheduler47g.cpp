#include "emulator/core/iop_kernel.h"
#include "emulator/core/iop_memory.h"
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <vector>
using namespace ps2x::iop::detail;

bool sameCpu(const IopCpuState& a,const IopCpuState& b) {
    return std::tie(a.gpr,a.hi,a.lo,a.pc,a.cop0,a.pendingLoadReg,a.pendingLoadValue,a.pendingLoad,
        a.branchPending,a.branchTarget,a.stopped,a.yielded,a.exception)==
        std::tie(b.gpr,b.hi,b.lo,b.pc,b.cop0,b.pendingLoadReg,b.pendingLoadValue,b.pendingLoad,
        b.branchPending,b.branchTarget,b.stopped,b.yielded,b.exception);
}
bool sameThreads(const IopKernel& a,const IopKernel& b) {
    const auto& aa=a.debugThreads(); const auto& bb=b.debugThreads();
    if(aa.size()!=bb.size() || a.currentThreadId()!=b.currentThreadId()) return false;
    for(const auto& [id,x]:aa) {
        auto i=bb.find(id); if(i==bb.end()) return false;
        const auto& y=i->second;
        if(!sameCpu(x.cpu,y.cpu) ||
           std::tie(x.id,x.state,x.entry,x.stackBase,x.stackSize,x.priority,x.initialPriority,x.option,
               x.attr,x.wakeCycle,x.waitId,x.waitBits,x.waitMode,x.waitResultAddress,x.wakeupCount)!=
           std::tie(y.id,y.state,y.entry,y.stackBase,y.stackSize,y.priority,y.initialPriority,y.option,
               y.attr,y.wakeCycle,y.waitId,y.waitBits,y.waitMode,y.waitResultAddress,y.wakeupCount)) return false;
    }
    return true;
}
int main() {
    IopMemory ma,mb;
    IopKernel a(ma),b(mb);
    _putenv_s("RR_IOP_READY_CACHE47G","0"); a.reset();
    _putenv_s("RR_IOP_READY_CACHE47G","1"); b.reset();
    unsigned created=0,active=0;
    auto command=[&](uint16_t ordinal,uint32_t id,uint32_t value,uint64_t cycle) {
        IopCpuState ca{},cb{}; ca.gpr[4]=cb.gpr[4]=id; ca.gpr[5]=cb.gpr[5]=value;
        return a.dispatchThreadImport(ordinal,ca,cycle)==b.dispatchThreadImport(ordinal,cb,cycle) && sameCpu(ca,cb);
    };
    auto create=[&]() {
        ++created;
        for(auto* memory:{&ma,&mb}) {
            memory->write32(0x1000,0); memory->write32(0x1004,0);
            memory->write32(0x1008,0x2000+created*0x100);
            memory->write32(0x100c,0x200); memory->write32(0x1010,1+(created%17));
        }
        IopCpuState ca{},cb{}; ca.gpr[4]=cb.gpr[4]=0x1000;
        if(!a.dispatchThreadImport(4,ca,0) || !b.dispatchThreadImport(4,cb,0) ||
            !sameCpu(ca,cb) || int32_t(ca.gpr[2])<=0) return false;
        return command(6,ca.gpr[2],0,0);
    };
    for(unsigned n=0;n<69;++n) if(!create()) return 1;
    const int event=a.createInternalEventFlag(0,0,0);
    if(event<=0 || b.createInternalEventFlag(0,0,0)!=event) return 13;
    for(auto* memory:{&ma,&mb}) {
        memory->write32(0x1200,0); memory->write32(0x1204,0);
        memory->write32(0x1208,0); memory->write32(0x120c,1);
    }
    IopCpuState semaA{},semaB{}; semaA.gpr[4]=semaB.gpr[4]=0x1200;
    if(!a.dispatchSemaphoreImport(4,semaA) || !b.dispatchSemaphoreImport(4,semaB) || !sameCpu(semaA,semaB)) return 14;
    const uint32_t semaphore=semaA.gpr[2];
    unsigned eventWaits=0,semaphoreWaits=0;
    for(uint64_t n=0;n<200000;++n) {
        if(n%257==0 && (a.setInternalEventFlag(event,1)!=b.setInternalEventFlag(event,1))) return 15;
        if(n%127==0) {
            IopCpuState ca{},cb{}; ca.gpr[4]=cb.gpr[4]=semaphore;
            if(!a.dispatchSemaphoreImport(6,ca) || !b.dispatchSemaphoreImport(6,cb) || !sameCpu(ca,cb)) return 16;
        }
        const uint32_t id=uint32_t((n/97)%created)+1;
        if(n%97==0 && !command(25,id,0,n)) return 2; // Wake or increment wake count.
        if(n%503==0 && !command(14,id,1+uint32_t(n%126),n)) return 3;
        if(n%4096==0 && !command(30,id,0,n)) return 4;
        if(n%4096==1 && !command(32,id,0,n)) return 5;
        auto* x=a.beginNextReady(n); auto* y=b.beginNextReady(n);
        if((x?x->id:0)!=(y?y->id:0) || a.nextWakeCycle(UINT64_MAX)!=b.nextWakeCycle(UINT64_MAX)) return 6;
        bool replenish=false;
        if(x) {
            ++active;
            x->cpu.gpr[8]^=uint32_t(n); y->cpu.gpr[8]^=uint32_t(n);
            x->cpu.pc+=4; y->cpu.pc+=4;
            if(n%997==0) { a.terminateThreadsInRange(x->cpu.pc,4); b.terminateThreadsInRange(y->cpu.pc,4); replenish=true; }
            else if(n%431==0) {
                IopCpuState ca{},cb{}; ca.gpr[4]=cb.gpr[4]=event;
                if(!a.dispatchEventImport(8,ca) || !b.dispatchEventImport(8,cb) || !sameCpu(ca,cb)) return 17;
                x->cpu.gpr[4]=y->cpu.gpr[4]=event; x->cpu.gpr[5]=y->cpu.gpr[5]=1;
                x->cpu.gpr[6]=y->cpu.gpr[6]=0; x->cpu.gpr[7]=y->cpu.gpr[7]=0x1800;
                if(!a.dispatchEventImport(10,x->cpu) || !b.dispatchEventImport(10,y->cpu)) return 18;
                eventWaits+=x->state==IopThreadState::EventFlag;
            }
            else if(n%733==0) {
                IopCpuState ca{},cb{}; ca.gpr[4]=cb.gpr[4]=semaphore;
                if(!a.dispatchSemaphoreImport(9,ca) || !b.dispatchSemaphoreImport(9,cb) || !sameCpu(ca,cb)) return 19;
                x->cpu.gpr[4]=y->cpu.gpr[4]=semaphore;
                if(!a.dispatchSemaphoreImport(8,x->cpu) || !b.dispatchSemaphoreImport(8,y->cpu)) return 20;
                semaphoreWaits+=x->state==IopThreadState::Semaphore;
            }
            else if(n%101==0) { a.delayCurrentUntil(n+19,x->cpu); b.delayCurrentUntil(n+19,y->cpu); }
            else if(n%211==0) { a.sleepCurrent(x->cpu); b.sleepCurrent(y->cpu); }
            a.endTimeslice(*x,0xfffffff0); b.endTimeslice(*y,0xfffffff0);
        }
        if(replenish && !create()) return 7;
        if(!sameThreads(a,b)) { std::cerr<<"Thread state differs at step "<<n<<"\n"; return 8; }
    }
    // Mutable RPC references are reacquired before updates, matching the audited caller.
    const int last=a.debugThreads().begin()->first;
    auto* x=a.rpcThread(last); auto* y=b.rpcThread(last);
    x->priority=y->priority=1; x->state=y->state=IopThreadState::Ready;
    auto* nx=a.beginNextReady(200001); auto* ny=b.beginNextReady(200001);
    if(!nx || !ny || nx->id!=ny->id) return 9;
    a.endTimeslice(*nx,0xfffffff0); b.endTimeslice(*ny,0xfffffff0);
    if(!sameThreads(a,b)) return 10;
    std::vector<uint8_t> ra(IopMemory::RamSize),rb(ra.size());
    if(!ma.readRam(0,ra.data(),ra.size()) || !mb.readRam(0,rb.data(),rb.size()) || ra!=rb) return 11;
    const auto sa=a.schedulingStats(),sb=b.schedulingStats();
    if(active<100000 || sb.hits<100000 || sb.scans>=sa.scans/10 || sb.deadScans>=sa.deadScans/10 ||
       eventWaits<100 || semaphoreWaits<100) return 12;
    std::cout<<"scheduler-kernel decisions=200001 active="<<active<<" full_state_mismatches=0 ram_bytes="<<ra.size()
        <<" baseline_scans="<<sa.scans<<" cached_scans="<<sb.scans<<" hits="<<sb.hits
        <<" baseline_dead_scans="<<sa.deadScans<<" cached_dead_scans="<<sb.deadScans<<"\n";
    std::cout<<"blocking event_waits="<<eventWaits<<" semaphore_waits="<<semaphoreWaits<<"\n";
}
