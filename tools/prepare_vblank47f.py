"""Create a diagnostic scheduler copy; leave the legacy reference unchanged."""
from pathlib import Path
import hashlib
import json
root=Path(__file__).resolve().parents[1]
source=root/'work/native/sources/src/PS2Recomp/ps2xRuntime/src/lib/Kernel/EeScheduler.cpp'
out=root/'build/fixed47F'
assert not out.exists(), 'Keep diagnostic source candidates immutable'
out.mkdir(parents=True)
text=source.read_text()
for before,after in [
    ('#include "rr_gs_capture.h"','#include "rr_gs_capture.h"\n#include "vblank_stop47f.h"'),
    ('    RrGsCaptureSequence rrGsCaptures;','    RrGsCaptureSequence rrGsCaptures;\n    RrVblankStop47F rrFixedStop(this);'),
    ('        dispatchIrq(false, 2u);\n        break;',
     '        dispatchIrq(false, 2u);\n        if (RrVblankStop47F::reached(this,m_vsyncTick,m_eeCycle)) requestStop();\n        break;')]:
    assert text.count(before)==1,before
    text=text.replace(before,after)
target=out/'EeScheduler.cpp'
target.write_text(text,encoding='utf-8',newline='\n')
(out/'provenance.json').write_text(json.dumps({'reference_modified':False,
    'source':source.relative_to(root).as_posix(),'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),
    'candidate_sha256':hashlib.sha256(target.read_bytes()).hexdigest(),
    'purpose':'Optional cooperative stop after an original VBlank event; same guest execution until that event.'},indent=2))
print(target)
