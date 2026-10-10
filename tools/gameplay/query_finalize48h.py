"""Verify owned source against compiled artifacts and write exact manifest."""
from query_extract48h import ROOT, OUT, digest
import json

def main():
    build=ROOT/'evidence/nativePc48H-build-20261003-163334-944'
    compiled=['src/game/collision_query48h.h','src/game/collision_query48h.cpp','tests/native/collision_query48h.cpp','tools/gameplay/query_fixtures48h.h']
    files=[ROOT/p for p in compiled]+list((ROOT/'tools/gameplay').glob('query*48h.py'))+[ROOT/'docs/terrain-query48h.md']
    validation=[]
    for arch in ('x64','x86'):
        hashes=build/f'{arch}-source-hashes.json';rows=json.loads(hashes.read_text())
        for rel in compiled:
            p=(ROOT/rel).resolve();row=next(r for r in rows if str(p).lower()==str(r['Path']).lower())
            assert row['Hash'].lower()==digest(p.read_bytes()),rel
        log=build/f'{arch}-tests.log';s=log.read_text();assert s.count('Test Passed.')==16 and 'PASS 4025 checks' in s
        exe=build/arch/'racer_native_collision_query_check.exe'
        if not exe.exists():exe=build/arch/'frozenbinary/racer_native_collision_query_check.exe'
        assert exe.exists()
        validation.append({'architecture':arch,'result':'PASS','checks':4025,'original_word_fixtures':710,'hits':434,'misses':261,'explicit_interpolation_boundaries':15,'all_project_tests':16,'log':log.relative_to(ROOT).as_posix(),'log_sha256':digest(log.read_bytes()),'exe':exe.relative_to(ROOT).as_posix(),'exe_sha256':digest(exe.read_bytes()),'compiled_source_files_verified':len(compiled)})
        files += [hashes,log,exe]
    (OUT/'validation.json').write_text(json.dumps(validation,indent=2)+'\n')
    files += [p for p in OUT.rglob('*') if p.is_file() and p.name!='checkpoint-manifest.json']
    records=[{'path':p.relative_to(ROOT).as_posix(),'bytes':p.stat().st_size,'sha256':digest(p.read_bytes())} for p in sorted(set(files))]
    dest=OUT/'checkpoint-manifest.json';dest.write_text(json.dumps({'checkpoint':'48H native terrain triangle leaf and original saved query recovery','files':records},indent=2)+'\n')
    for r in json.loads(dest.read_text())['files']:
        b=(ROOT/r['path']).read_bytes();assert len(b)==r['bytes'] and digest(b)==r['sha256']
    print(json.dumps({'verified_files':len(records),'manifest_sha256':digest(dest.read_bytes()),'architectures':[v['architecture'] for v in validation]}))

if __name__=='__main__':main()
