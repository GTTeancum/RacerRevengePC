"""Verify frozen 48I query sources against both compiled builds and manifest."""
from pathlib import Path
import hashlib,json
from query_extract48h import ROOT

OUT=ROOT/'evidence/query48I'
BUILD=ROOT/'evidence/nativePc48I-build-20261003-170723-706'
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    compiled=['src/game/collision_tree48i.h','src/game/collision_tree48i.cpp',
      'tests/native/collision_tree48i.cpp','tools/gameplay/query_fixture48i.h',
      'tools/gameplay/query_binding48i.h']
    files={ROOT/p for p in compiled}
    files.update((ROOT/'tools/gameplay').glob('query*48i.py'))
    files.add(ROOT/'docs/terrain-query48i.md')
    validation=[]
    expected='PASS 5351 checks; 43 original-word ordered queries, hits=15, misses=28, exact visits=1195; 2 complete native cache meshes and source instance placements used'
    for arch in ('x64','x86'):
        manifest=BUILD/f'{arch}-source-hashes.json'
        records=json.loads(manifest.read_text())
        for rel in compiled:
            path=(ROOT/rel).resolve()
            row=next(r for r in records if r['Path'].lower()==str(path).lower())
            assert row['Hash'].lower()==digest(path),rel
        log=BUILD/f'{arch}-tests.log'
        text=log.read_text();assert text.count('Test Passed.')==18 and expected in text
        exe=BUILD/arch/'racer_native_collision_tree_check.exe';assert exe.exists()
        validation.append({'architecture':arch,'result':'PASS','checks':5351,
          'original_word_queries':43,'hits':15,'misses':28,'exact_ordered_visits':1195,
          'full_native_cache_meshes':2,'exact_source_instance_placements':2,
          'all_project_tests':18,'compiled_source_files_verified':len(compiled),
          'log':log.relative_to(ROOT).as_posix(),'log_sha256':digest(log),
          'executable':exe.relative_to(ROOT).as_posix(),'executable_sha256':digest(exe)})
        files.update((manifest,log,exe))
    (OUT/'validation.json').write_text(json.dumps(validation,indent=2)+'\n')
    # Preserve dependency hashes without changing immutable earlier checkpoints.
    dependencies=['tools/gameplay/query_extract48h.py','tools/gameplay/query_oracle48h.py',
      'evidence/query48H/original-words.json','evidence/query48H/source-anchors.txt',
      'src/game/collision_query48h.cpp','src/game/collision_query48h.h',
      'src/assets/collision_mesh48i.cpp','src/assets/collision_mesh48i.h',
      'src/assets/collision_runtime48h.cpp','src/assets/collision_runtime48h.h',
      'evidence/nativePc48I-build-20261003-170000-689/x64.log']
    binding=json.loads((OUT/'cache-binding.json').read_text())
    for row in binding:
        for name in ('source_col','source_reference','world_reference'):
            path=ROOT/row[name];assert digest(path)==row[name+'_sha256'];files.add(path)
        assert digest(ROOT/row['native_cache'])==row['native_cache_sha256']
    files.update(ROOT/p for p in dependencies)
    files.update(p for p in OUT.rglob('*') if p.is_file() and p.name!='checkpoint-manifest.json')
    records=[{'path':p.relative_to(ROOT).as_posix(),'bytes':p.stat().st_size,'sha256':digest(p)} for p in sorted(files)]
    path=OUT/'checkpoint-manifest.json'
    path.write_text(json.dumps({'checkpoint':'48I native ordered mesh query over retained collision caches',
      'original_ELF_sha256':'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30',
      'original_RAM_sha256':'38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48',
      'files':records},indent=2)+'\n')
    for row in json.loads(path.read_text())['files']:
        p=ROOT/row['path'];assert p.stat().st_size==row['bytes'] and digest(p)==row['sha256']
    print(json.dumps({'verified_files':len(records),'manifest_sha256':digest(path),'validation':validation}))

if __name__=='__main__':main()
