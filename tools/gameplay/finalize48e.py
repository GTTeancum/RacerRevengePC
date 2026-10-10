"""Verify final built inputs and freeze this agent's bounded 48E evidence."""
from pathlib import Path
import hashlib,json,re,shutil
ROOT=Path(__file__).resolve().parents[2]
BUILD=ROOT/'evidence/nativePc48E-build-20261003-141953-482'
OUT=ROOT/'evidence/gameplay48E'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    inputs=['src/game/vehicle_aero48e.h','src/game/vehicle_aero48e.cpp','tests/native/physics48e.cpp',
            'tools/gameplay/aero_words48e.h','tools/gameplay/aero_oracle48e.h']
    validation={'scope':'Complete ready-scene aerodynamic numerical producer; connected engine/gravity/aero/body partial step. Not a full original substep or gameplay FPS measurement.',
        'build_evidence':BUILD.relative_to(ROOT).as_posix(),'architectures':{}}
    for arch in ['x64','x86']:
        source=json.loads((BUILD/f'{arch}-source-hashes.json').read_text(encoding='utf-8-sig'))
        indexed={str(Path(r['Path'])).lower():r['Hash'].lower() for r in source}
        verified=[]
        for rel in inputs:
            p=ROOT/rel;digest=sha(p);assert indexed[str(p).lower()]==digest,(arch,rel)
            verified.append({'path':rel,'sha256':digest})
        log=BUILD/f'{arch}-tests.log';text=log.read_text();m=re.search(r'physics48e: (\d+) checks passed',text)
        assert m and int(m[1])==354461 and text.count('Test Passed.')==9
        shutil.copyfile(log,OUT/f'validation-{arch}.log')
        exe=BUILD/arch/'racer_native_aero_check.exe';digest=sha(exe)
        expected=json.loads((BUILD/f'{arch}-hashes.json').read_text(encoding='utf-8-sig'))
        assert [x['Hash'].lower() for x in expected if Path(x['Path']).name=='racer_native_aero_check.exe']==[digest]
        validation['architectures'][arch]={'passed':True,'checks':int(m[1]),'project_ctests_passed':9,
            'test_log_sha256':sha(log),'frozen_executable':exe.relative_to(ROOT).as_posix(),'executable_sha256':digest,
            'verified_compiled_inputs':verified}
    shutil.copyfile(ROOT/'evidence/nativePc48E-build-20261003-141343-104/x64.log',OUT/'initial-integrated-failure.log')
    (OUT/'validation.json').write_text(json.dumps(validation,indent=2)+'\n')
    files=[ROOT/x for x in ['src/game/vehicle_aero48e.h','src/game/vehicle_aero48e.cpp','tests/native/physics48e.cpp','docs/vehicle-aero48e.md']]
    files.extend(p for p in (ROOT/'tools/gameplay').glob('*48e*') if p.is_file())
    files.extend(p for p in OUT.rglob('*') if p.is_file() and p.name!='checkpoint-manifest.json')
    records=[{'path':p.relative_to(ROOT).as_posix(),'bytes':p.stat().st_size,'sha256':sha(p)} for p in sorted(set(files))]
    manifest={'scope':'Frozen 48E gameplay source/evidence. Parent checkpoint owns complete project/source archives. No full-vehicle claim.',
              'files':records}
    target=OUT/'checkpoint-manifest.json';target.write_text(json.dumps(manifest,indent=2)+'\n')
    for r in records:
        p=ROOT/r['path'];assert p.stat().st_size==r['bytes'] and sha(p)==r['sha256']
    print(json.dumps({'owned_files_reopened_verified':len(records),'manifest_sha256':sha(target),
        'source_sha256':sha(ROOT/'src/game/vehicle_aero48e.cpp'),'test_sha256':sha(ROOT/'tests/native/physics48e.cpp'),
        'architectures':{a:v['executable_sha256'] for a,v in validation['architectures'].items()}}))
if __name__=='__main__':main()
