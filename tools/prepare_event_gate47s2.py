"""Conservative variant: a zero deadline marker takes original processing."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1];out=root/'build/event47S2';out.mkdir(exist_ok=True)
source=root/'build/event47S/EeScheduler.cpp';t=source.read_text();needle='(next==0u || m_eeCycle<next)'
assert t.count(needle)==1
t=t.replace(needle,'(next!=0u && m_eeCycle<next)')
t=t.replace('RR_EE_EVENT_GATE47S','RR_EE_EVENT_GATE47S2').replace('[rr:event47s]','[rr:event47s2]')
(out/'EeScheduler.cpp').write_text(t)
(out/'provenance.json').write_text(json.dumps(dict(original=str(source.relative_to(root)),original_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),candidate_sha256=hashlib.sha256((out/'EeScheduler.cpp').read_bytes()).hexdigest(),scope='Zero next-deadline marker always uses original event processing; all other 47S guards and cycle charges retained.'),indent=2))
t=(root/'build/event47S/Probe.cmake').read_text().replace('racer_event47s_','racer_event47s2_').replace('build/event47S/','build/event47S2/')
(out/'Probe.cmake').write_text(t)
t=(root/'tools/Event47S.cmake').read_text().replace('racer_event47s_','racer_event47s2_').replace('build/event47S/','build/event47S2/')
(out/'Event.cmake').write_text(t)
t=(root/'tools/Run-Native47S.ps1').read_text().replace('native-run47s-','native-run47s2-').replace('racer_event47s_','racer_event47s2_').replace('evidence/47S/','evidence/47S2/').replace('RR_EE_EVENT_GATE47S','RR_EE_EVENT_GATE47S2')
t=t.replace('release-opt-47q-iop-diagnostic-cache','release-opt-47s2-zero-safe-event-gate')
(root/'tools/Run-Native47S2.ps1').write_text(t)
print('Prepared conservative zero-deadline event gate; original candidate retained.')
