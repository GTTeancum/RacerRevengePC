"""Preserve 47G, then invalidate idle proofs across nested execution/mutation."""
from pathlib import Path
import hashlib
import json
import shutil
root=Path(__file__).resolve().parents[1]
source=root/'build/iop47G'
out=root/'build/iop47H'
assert not out.exists()
shutil.copytree(source,out)
p=out/'src/emulator/iop_emulator.cpp'
text=p.read_text()
before=hashlib.sha256(p.read_bytes()).hexdigest()
anchor='        rr::IdleWindow idleWindow;'
assert text.count(anchor)==1
text=text.replace(anchor,'''        // Public operations and nested guest execution may create an idle proof
        // and then mutate state after that inner runCycles returns. A proof is
        // never allowed to escape such a mutation scope.
        struct IdleMutationScope {
            rr::IdleWindow& window;
            explicit IdleMutationScope(rr::IdleWindow& w):window(w){window.invalidate();}
            ~IdleMutationScope(){window.invalidate();}
        };
        IopThread* mutableRpcThread(int id) {
            idleWindow.invalidate();
            return kernel.rpcThread(id);
        }
''' + anchor)
# Do not replace the helper's own implementation.
assert text.count('kernel.rpcThread(threadId)')==3
text=text.replace('kernel.rpcThread(threadId)', 'mutableRpcThread(threadId)')
anchor='''        uint32_t runCpu(CpuState &cpu, uint32_t instructionBudget)
        {'''
assert text.count(anchor)==1
text=text.replace(anchor,anchor+'\n            IdleMutationScope idleMutation(idleWindow);')
anchor='''        uint32_t executeRpcFunction(int threadId,uint32_t address,uint32_t a0,uint32_t a1,uint32_t a2,uint32_t gp) override
        {'''
assert text.count(anchor)==1
text=text.replace(anchor,anchor+'\n            IdleMutationScope idleMutation(idleWindow);')
assert text.count('        m_impl->idleWindow.invalidate();')==10
text=text.replace('        m_impl->idleWindow.invalidate();', '        Impl::IdleMutationScope idleMutation(m_impl->idleWindow);')
p.write_text(text,encoding='utf-8',newline='\n')
(out/'idle-provenance47h.json').write_text(json.dumps(dict(
    source=str(source/'src/emulator/iop_emulator.cpp'), before_sha256=before,
    after_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),
    reason='Native fixed VBlank 500 differs with idle batching; nested RPC mutation can retain a stale idle proof.',
    status='Hypothesis candidate, requires original-state comparison', reference_modified=False),indent=2))
cmake=(root/'tools/IopCandidate47G.cmake').read_text().replace('47g','47h').replace('47G','47H')
(root/'tools/IopCandidate47H.cmake').write_text(cmake,encoding='utf-8',newline='\n')
shutil.copy2(out/'differential47g.cpp',out/'differential47h.cpp')
# Keep established scheduler test source; only its linked candidate changes.
cmake=(root/'tools/IopCandidate47H.cmake').read_text().replace('tests/iop_scheduler47h.cpp','tests/iop_scheduler47g.cpp')
(root/'tools/IopCandidate47H.cmake').write_text(cmake,encoding='utf-8',newline='\n')
(root/'tools/LegacyHook47H.cmake').write_text('include("${RR_SHARED_ROOT}/tools/LegacyHook47F.cmake")\ncmake_language(DEFER CALL include "${RR_SHARED_ROOT}/tools/IopCandidate47H.cmake")\n')
evidence=root/'evidence/47H'
evidence.mkdir(exist_ok=False)
cmd=(root/'evidence/47G/build.cmd').read_text().replace('47g','47h').replace('47G','47H')
(evidence/'build.cmd').write_text(cmd)
runner=(root/'tools/Run-Native47G.ps1').read_text()
# Keep the actual unchanged ready-cache control environment variable names.
runner=runner.replace('47G','47H').replace('47g','47h').replace('RR_IOP_READY_CACHE47H','RR_IOP_READY_CACHE47G').replace('RR_IOP_READY_STATS47H','RR_IOP_READY_STATS47G')
(root/'tools/Run-Native47H.ps1').write_text(runner,encoding='utf-8',newline='\n')
print(out)
