"""Run the canonical GS and isolated candidate with raw/non-raw backends."""
from pathlib import Path
import hashlib
import json
import os
import subprocess
root=Path(__file__).resolve().parents[1]
out=root/'evidence/47L/differential-v3';out.mkdir(exist_ok=False)
build=root/'work/native/ninja-build-release-opt'
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
records=[]
for raw in (0,1):
    reference=None
    for variant,mode in [('original',0),('candidate',0),('candidate',1),('candidate',2)]:
        dest=out/f'{raw}-{variant}-{mode}'
        env=os.environ.copy();env['RR_RAW_GS_FAST47L']=str(mode)
        result=subprocess.run([str(build/f'racer_gs47l_{variant}_check.exe'),str(dest),str(raw)],
            env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=180)
        (dest/'run.log').write_bytes(result.stdout);(dest/'run.exit').write_text(str(result.returncode))
        assert result.returncode==0,result.stdout[-1500:]
        backend=json.loads((dest/'backend.json').read_text())
        record=dict(raw=raw,variant=variant,mode=mode,exit=result.returncode,backend=backend)
        if reference is None:
            reference=dest
            assert backend['submits']>0 and backend['preferred_hits']>0 and backend['checks']==504
        else:
            expected=json.loads((reference/'backend.json').read_text())
            if raw and mode:
                assert backend['submits']==0
                expected['submits']=0
            assert backend==expected,(record,expected)
            assert (dest/'state.txt').read_bytes()==(reference/'state.txt').read_bytes()
            frames=sorted(reference.glob('frame-*.json'));assert len(frames)==42
            for frame in frames:
                before=json.loads(frame.read_text());after=json.loads((dest/frame.name).read_text())
                if raw and mode==2:
                    assert before['history'] and after['history']==[]
                    before['history']=[]
                assert before==after,(record,frame.name)
                for suffix in ('.ppm','.vram'):
                    assert sha(frame.with_suffix(suffix))==sha((dest/frame.name).with_suffix(suffix))
            record.update(canonical_snapshots=42,state_observations_equal=True,pixels_and_vram_equal=True,
                history_equal=not(raw and mode==2),expected_history_reduction=bool(raw and mode==2))
        records.append(record)
        print(json.dumps(record),flush=True)
(out/'report.json').write_text(json.dumps(dict(success=True,tests=records,
    scope='Actual original/candidate GS frontends, deterministic raw/non-raw backend; not a replacement for native-game or actual-renderer validation.'),indent=2))
