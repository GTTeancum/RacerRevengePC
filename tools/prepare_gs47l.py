"""Isolate raw-GIF batch elision; keep the canonical GS frontend unchanged."""
from pathlib import Path
import hashlib
import json
root=Path(__file__).resolve().parents[1]
source=root/'work/native/sources/src/PS2Recomp/ps2xRuntime/src/lib/gs/gs_frontend.cpp'
assert hashlib.sha256(source.read_bytes()).hexdigest()=='071c7ebeefb7684c28781579435f1a94b046ba6fa2e564fc2dc2523944c5ddb8'
dest=root/'build/gs47L';dest.mkdir(exist_ok=False)
text=source.read_text()
anchor='    static constexpr uint32_t kHostFrameWidth = 640u;'
text=text.replace(anchor,anchor+'''
    // 0: original path; 1: no unused raw-GIF draw batches, full history;
    // 2: same batching change, native duplicate history disabled explicitly.
    // The currently supported raw-GIF backend owns pixels and ignores Submit.
    int rrRawMode47L() {
        static const int mode=[] {
            const char* value=std::getenv("RR_RAW_GS_FAST47L");
            return value && value[0] && !value[1] && value[0]>='1' && value[0]<='2'
                ? value[0]-'0' : 0;
        }();
        return mode;
    }
''',1)
# Copy the entire existing preferred-source predicate, changing only the source
# of its values. This preserves the native presentation observer even in raw mode.
begin=text.index('void GS::updatePreferredDisplaySourceForDraw(const GSPrimitiveBatch &batch)')
end=text.index('\nvoid GS::notifyVsync()',begin)
function=text[begin:end]
body=function[function.index('{')+1:function.rfind('}')]
body=body.replace('    const GSDrawState &state = batch.state;\n    const GSContext &ctx = state.context;',
                  '    const GSContext &ctx = m_ctx[m_prim.ctxt ? 1 : 0];')
body=body.replace('state.prim','m_prim').replace('batch.vertexCount','needed').replace('batch.vertices','m_vtxQueue')
assert 'state.' not in body and 'batch.' not in body
old='''        GSPrimitiveBatch batch = buildDrawBatch(needed);
        updatePreferredDisplaySourceForDraw(batch);
        m_backend->Submit(batch);
        recordDrawDebugEventUnlocked(needed);'''
assert text.count(old)==1
new='''        if (rrRawMode47L() && m_backend->SupportsRawGIF()) {
            // A raw backend already received the original GIF. Retain queue
            // progression, preferred display tracking and full debug values.
            auto updateRawPreferred = [&]() {'''+body+'''
            };
            updateRawPreferred();
        } else {
            GSPrimitiveBatch batch = buildDrawBatch(needed);
            updatePreferredDisplaySourceForDraw(batch);
            m_backend->Submit(batch);
        }
        recordDrawDebugEventUnlocked(needed);'''
text=text.replace(old,new)
assert text.count('    if (m_debugHistoryPaused)')==6
text=text.replace('    if (m_debugHistoryPaused)',
    '    if (m_debugHistoryPaused || (rrRawMode47L() == 2 && m_backend && m_backend->SupportsRawGIF()))')
(dest/'gs_frontend.cpp').write_text(text)
(dest/'provenance.json').write_text(json.dumps(dict(original=str(source.relative_to(root)),
    original_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),
    candidate_sha256=hashlib.sha256((dest/'gs_frontend.cpp').read_bytes()).hexdigest(),
    mode0='Original draws and history',mode1='No unused raw-backend draw batch; retain full history',
    mode2='Batch elision plus explicit duplicate-history reduction; not an instruction-execution speed claim'),indent=2))
print('Prepared isolated GS47L candidate; original frontend unchanged.')
