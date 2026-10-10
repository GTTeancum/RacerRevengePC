"""Clone the IOP before adding an opt-in exact ready-selection cache."""
from pathlib import Path
import hashlib
import json
import shutil

root=Path(__file__).resolve().parents[1]
original=root/'work/native/sources/src/PS2Recomp/ps2xIOP'
out=root/'build/iop47G'
assert not out.exists(), 'Preserve previous source candidates'
shutil.copytree(original,out)
shutil.copy2(root/'build/legacy-iop47B2/iop_emulator.cpp',out/'src/emulator/iop_emulator.cpp')
def edit(relative,changes):
    path=out/relative
    text=path.read_text()
    before=hashlib.sha256(path.read_bytes()).hexdigest()
    for old,new in changes:
        assert text.count(old)==1,(relative,text.count(old),old[:100])
        text=text.replace(old,new)
    path.write_text(text,encoding='utf-8',newline='\n')
    return {'path':relative,'before_sha256':before,'after_sha256':hashlib.sha256(path.read_bytes()).hexdigest()}
changes=[]
changes.append(edit('src/emulator/core/iop_kernel.h',[
('        void reset();','''        void reset();
        // Mutable RPC references are refreshed after every scheduled interval
        // by the existing caller. Reacquire/invalidate before a deferred write.
        void invalidateScheduling() noexcept { m_readyValid=false; m_checkDead=true; }
        struct SchedulingStats { uint64_t hits, scans, deadScans; };
        SchedulingStats schedulingStats() const noexcept { return {m_readyHits,m_readyScans,m_deadScans}; }
        bool schedulingCacheEnabled() const noexcept { return m_readyEnabled; }'''),
('''        IopThread* rpcThread(int id) noexcept {
            auto i=''','''        IopThread* rpcThread(int id) noexcept {
            invalidateScheduling();
            auto i='''),
('''        void pauseRpcDispatch(IopCpuState& cpu) noexcept {
            if''','''        void pauseRpcDispatch(IopCpuState& cpu) noexcept {
            invalidateScheduling();
            if'''),
('        IopMemory &m_memory;','''        bool m_readyEnabled=false, m_readyValid=false, m_checkDead=true;
        IopThread* m_readyThread=nullptr;
        uint64_t m_readyUntil=UINT64_MAX;
        uint64_t m_readyHits=0, m_readyScans=0, m_deadScans=0;
        IopMemory &m_memory;''')]))
cpp=[('#include <algorithm>','#include <algorithm>\n#include <cstdlib>'),
('''    void IopKernel::reset()
    {''','''    void IopKernel::reset()
    {
        const char* flag=std::getenv("RR_IOP_READY_CACHE47G");
        m_readyEnabled=flag && flag[0]=='1';
        m_readyValid=false; m_checkDead=true; m_readyThread=nullptr; m_readyUntil=UINT64_MAX;
        m_readyHits=m_readyScans=m_deadScans=0;''')]
for signature in [
 'bool IopKernel::dispatchThreadImport(uint16_t ordinal, IopCpuState &cpu, uint64_t currentCycle)',
 'bool IopKernel::dispatchSemaphoreImport(uint16_t ordinal, IopCpuState &cpu)',
 'bool IopKernel::dispatchEventImport(uint16_t ordinal, IopCpuState &cpu)',
 'int IopKernel::createInternalEventFlag(uint32_t attr, uint32_t option, uint32_t bits)',
 'bool IopKernel::setInternalEventFlag(int id, uint32_t bits)',
 'void IopKernel::wakeOneSemaphore(int id)',
 'void IopKernel::wakeEventWaiters(EventFlag &event)',
 'void IopKernel::sleepCurrent(IopCpuState &cpu)',
 'void IopKernel::delayCurrentUntil(uint64_t wakeCycle, IopCpuState &cpu)',
 'void IopKernel::terminateThreadsInRange(uint32_t base, uint32_t size)']:
    old='    '+signature+'\n    {'
    cpp.append((old,old+'\n        invalidateScheduling();'))
old='''        IopThread *next = nullptr;
        for (auto &[id, thread] : m_threads)
        {
            if (thread.state == IopThreadState::Delay && thread.wakeCycle <= currentCycle)
                thread.state = IopThreadState::Ready;

            if (thread.state != IopThreadState::Ready)
                continue;
            if (next == nullptr || thread.priority < next->priority ||
                (thread.priority == next->priority && thread.id < next->id))
                next = &thread;
        }'''
new='''        IopThread *next = nullptr;
        if (m_readyEnabled && m_readyValid && currentCycle < m_readyUntil &&
            m_currentThread == nullptr && (!m_readyThread || m_readyThread->state == IopThreadState::Ready))
        {
            ++m_readyHits;
            next=m_readyThread;
        }
        else
        {
            ++m_readyScans;
            uint64_t nextWake=UINT64_MAX;
            for (auto &[id, thread] : m_threads)
            {
                if (thread.state == IopThreadState::Delay && thread.wakeCycle <= currentCycle)
                    thread.state = IopThreadState::Ready;
                if (thread.state == IopThreadState::Delay)
                    nextWake=std::min(nextWake,thread.wakeCycle);
                if (thread.state != IopThreadState::Ready) continue;
                if (next == nullptr || thread.priority < next->priority ||
                    (thread.priority == next->priority && thread.id < next->id)) next=&thread;
            }
            m_readyThread=next; m_readyUntil=nextWake; m_readyValid=true;
        }'''
cpp.append((old,new))
cpp.append(('''        if (thread.cpu.pc == returnSentinel || thread.cpu.stopped)
            thread.state = IopThreadState::Dormant;''','''        if (thread.cpu.pc == returnSentinel || thread.cpu.stopped)
        {
            thread.state = IopThreadState::Dormant;
            invalidateScheduling();
        }'''))
cpp.append(('''        m_currentThread = nullptr;
        cleanupDeadThreads();''','''        m_currentThread = nullptr;
        if (!m_readyEnabled || m_checkDead) cleanupDeadThreads();'''))
cpp.append(('''    void IopKernel::cleanupDeadThreads()
    {''','''    void IopKernel::cleanupDeadThreads()
    {
        ++m_deadScans;
        m_checkDead=false;'''))
cpp.append(('''            thread = m_threads.erase(thread);''','''            m_readyValid=false;
            m_readyThread=nullptr;
            thread = m_threads.erase(thread);'''))
changes.append(edit('src/emulator/core/iop_kernel.cpp',cpp))
changes.append(edit('src/emulator/iop_emulator.cpp',[
('''        const char* idleStats = std::getenv("RR_IOP_IDLE_STATS");''','''        const char* readyStats = std::getenv("RR_IOP_READY_STATS47G");
        if (readyStats && readyStats[0]=='1') {
            const auto stats=m_impl->kernel.schedulingStats();
            std::ostringstream out;
            out<<"ready-cache enabled="<<m_impl->kernel.schedulingCacheEnabled()
               <<" hits="<<stats.hits<<" scans="<<stats.scans<<" dead_scans="<<stats.deadScans;
            result.push_back(out.str());
        }
        const char* idleStats = std::getenv("RR_IOP_IDLE_STATS");''')]))

# Use the established physical-module differential with both idle windows on,
# isolating the new selection cache. Original-source reference stays separate.
test=(root/'tests/legacy_iop_idle47b.cpp').read_text()
test=test.replace('return s.rfind("idle-batch ",0)==0;','return s.rfind("idle-batch ",0)==0 || s.rfind("ready-cache ",0)==0;')
test=test.replace('_putenv_s("RR_IOP_IDLE_STATS","1");','_putenv_s("RR_IOP_IDLE_STATS","1");\n    _putenv_s("RR_IOP_READY_STATS47G","1");')
test=test.replace('_putenv_s("RR_IOP_IDLE_BATCH","0");','_putenv_s("RR_IOP_IDLE_BATCH","1");\n        _putenv_s("RR_IOP_READY_CACHE47G","0");')
test=test.replace('        Emulator candidate(b);','        _putenv_s("RR_IOP_READY_CACHE47G","1");\n        Emulator candidate(b);')
test=test.replace('''            if(line.rfind("idle-batch ",0)==0) {''','''            if(line.rfind("ready-cache ",0)==0) std::cout<<line<<"\\n";
            if(line.rfind("idle-batch ",0)==0) {''')
(out/'differential47g.cpp').write_text(test,encoding='utf-8',newline='\n')
manifest={'reference_modified':False,'base':'Original IOP tree plus validated 47B2 idle adapter',
    'changes':changes,'enabled_by':'RR_IOP_READY_CACHE47G=1','statistics':'RR_IOP_READY_STATS47G=1',
    'assumption':'Internal rpcThread callers reacquire the mutable pointer after every runCycles interval before writing; all current call sites audited.',
    'status':'prepared; not built or validated yet'}
(out/'provenance.json').write_text(json.dumps(manifest,indent=2))
print(out)
