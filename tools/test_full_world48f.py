"""Regression: unsupported local texture must never borrow a different track variant."""
from pathlib import Path
import hashlib,json,subprocess
ROOT=Path(__file__).resolve().parents[1]
def js(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def main():
    cache=ROOT/'evidence/importFull48F-20261003/native-cache';out=ROOT/'evidence/native48F-parent'
    current=js(cache/'current.json');manifest=js(cache/'generations'/(current['generation']+'.json'))
    candidates=[m for m in manifest['members'] if m['member'].replace(chr(92),'/').lower()=='textures/cord.pst']
    local=next(m for m in candidates if m['container']=='TRACKS/TA.RES');assert local['status']=='unsupported'
    alternatives=[m for m in candidates if m['status']=='converted_texture'];assert alternatives and all(m['source_sha256']!=local['source_sha256'] for m in alternatives)
    runs=[]
    for arch in ('x64','x86'):
        exe=ROOT/f'build/native-pc48F-{arch}/racer_native_pc.exe';report=out/f'full-world-fixed-{arch}.json';capture=out/f'full-world-fixed-{arch}.ppm'
        assert not report.exists() and not capture.exists()
        args=[str(exe),'--world','--cache',str(cache),'--container','TRACKS/TA.RES','--member','tracks/ta.scb','--frames','3','--capture',str(capture),'--report',str(report)]
        p=subprocess.run(args,capture_output=True,text=True,timeout=180)
        (out/f'full-world-fixed-{arch}.stdout').write_text(p.stdout);(out/f'full-world-fixed-{arch}.stderr').write_text(p.stderr)
        assert p.returncode==0,(p.returncode,p.stdout,p.stderr)
        r=js(report);assert r['success'] and r['bound_native_textures']==21 and r['textured_spans_per_frame']==279
        assert 'Unresolved TEXTURES/CORD.PST (original resource unsupported)' in r['binding_warnings']
        assert r['world_placed_mesh_sha256']=='4b38721b18705c459da282fe7736e3f3d45f3c7a0c21daeca033ca96833415d7'
        digest=hashlib.sha256(capture.read_bytes()).hexdigest();assert digest=='dcf6b6c4929180b54aef63ea192344dafe2394ef6654e9700c33d6d1d0489063'
        runs.append(dict(architecture=arch,arguments=args,exit=p.returncode,executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),capture_sha256=digest,world_cache_load_ms=r['world_cache_load_ms']))
    result=dict(passed=True,generation=current['generation'],original_local=local,conflicting_alternatives=alternatives,runs=runs,
        scope='Native full-cache local-original precedence regression and indexed startup measurements; not gameplayFPS')
    (out/'full-world-binding-check.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(dict(passed=True,runs=runs)))
if __name__=='__main__':main()
