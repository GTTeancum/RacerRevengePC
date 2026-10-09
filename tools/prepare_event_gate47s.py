"""Clone the fixed scheduler; retain full processing whenever work may exist."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1];out=root/'build/event47S';out.mkdir(exist_ok=True)
source=root/'build/fixed47F/EeScheduler.cpp';t=source.read_text()
needle='void EeScheduler::processPendingEvents()\n{\n    assertExecutor();\n'
assert t.count(needle)==1
gate='''    struct EventCounts47s {uint64_t fast=0,full=0;~EventCounts47s(){
        std::fprintf(stderr,"[rr:event47s] fast=%llu full=%llu\\n",
            static_cast<unsigned long long>(fast),static_cast<unsigned long long>(full));}};
    static thread_local EventCounts47s counts47s;
    // The original path samples levels even without queued events, so the
    // fast return must include GS/VIF unmask and timer checks. Never clear the
    // pending flag here: a concurrent post must remain visible next iteration.
    static const bool fastEnabled=[] {const char* p=std::getenv("RR_EE_EVENT_GATE47S");return p&&p[0]=='1'&&p[1]=='\\0';}();
    if(fastEnabled && !m_checkpointPending.load(std::memory_order_acquire)
       && !m_stopRequested.load(std::memory_order_acquire) && m_pendingEeTimerInterrupts==0u){
        const uint64_t next=m_nextDeadlineCycle.load(std::memory_order_acquire);
        const auto& regs=m_runtime.memory().gs();
        const bool gsLevel=(m_enabledIntcMask&1u)!=0u
            && (regs.csr.load(std::memory_order_acquire)&~(regs.imr>>8u)&0x1full)!=0u;
        const bool vifLevel=(m_enabledIntcMask&(1u<<5))!=0u
            && (m_runtime.memory().vif1_regs.stat&(1u<<11))!=0u;
        if((next==0u || m_eeCycle<next) && !gsLevel && !vifLevel){
            ++counts47s.fast;
            applyPendingPreemption();
            return;
        }
    }
    ++counts47s.full;
'''
t='#include <cstdio>\n'+t.replace(needle,needle+gate);(out/'EeScheduler.cpp').write_text(t)
(out/'provenance.json').write_text(json.dumps(dict(original=str(source.relative_to(root)),original_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),candidate_sha256=hashlib.sha256((out/'EeScheduler.cpp').read_bytes()).hexdigest(),scope='Only no-work processPendingEvents fast return; original processing retained on pending event/stop/timer/due deadline/GS/VIF level. Original cycle charges and preemption retained.'),indent=2))
t=(root/'build/iop47Q/Probe.cmake').read_text().replace('racer_launch47q_fixed_probe','racer_event47s_fixed_probe').replace('build/fixed47F/EeScheduler.cpp','build/event47S/EeScheduler.cpp')
(out/'Probe.cmake').write_text(t)
t=(root/'tools/Run-Native47Q.ps1').read_text().replace('    [switch]$DisableGsProfile,',"    [switch]$DisableGsProfile,\n    [ValidateSet('0','1')][string]$EventGate='1',")
t=t.replace('$Mode=$DiagnosticCache+',"$env:RR_EE_EVENT_GATE47S=$EventGate\n$Mode=$EventGate+'-'+$DiagnosticCache+")
t=t.replace('native-run47q-','native-run47s-').replace('racer_launch47q_fixed_probe.exe','racer_event47s_fixed_probe.exe').replace('evidence/47Q/build-gate.json','evidence/47S/build-gate.json').replace('@{diagnostic_cache47q=', '@{event_gate47s=$EventGate;diagnostic_cache47q=')
(root/'tools/Run-Native47S.ps1').write_text(t)
print('Prepared isolated scheduler event gate and same-binary control mode.')
