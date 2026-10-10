"""Bind 48J terrain recovery to the parent's final verified native build."""
import argparse,hashlib,json
from query_extract48h import ROOT
OUT=ROOT/'evidence/query48J'
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',required=True);ap.add_argument('--tests',type=int,required=True);args=ap.parse_args()
    build=ROOT/args.build
    compiled=['src/game/terrain_query48j.h','src/game/terrain_query48j.cpp','tests/native/terrain_query48j.cpp','tools/gameplay/terrain_query_fixtures48j.h']
    files={ROOT/x for x in compiled};files.update((ROOT/'tools/gameplay').glob('terrain_query*48j.py'));files.add(ROOT/'docs/terrain-query48j.md')
    expected='PASS 5143 checks; 124 original-word limited-list queries and contact-state transitions, hits=55, misses=69; actual imported meshes and source instance placements; unsupported paths transactional'
    validation=[]
    for arch in ('x64','x86'):
        manifest=build/f'{arch}-source-hashes.json';rows=json.loads(manifest.read_text())
        for rel in compiled:
            p=(ROOT/rel).resolve();row=next(r for r in rows if r['Path'].lower()==str(p).lower());assert row['Hash'].lower()==digest(p),rel
        log=build/f'{arch}-tests.log';text=log.read_text();assert text.count('Test Passed.')==args.tests and expected in text
        exe=build/arch/'racer_native_terrain_query_check.exe';assert exe.exists()
        validation.append({'architecture':arch,'result':'PASS','checks':5143,'original_queries_and_state_transitions':124,'hits':55,'misses':69,
          'full_native_meshes':2,'source_instance_placements':2,'all_project_tests':args.tests,'compiled_files_verified':len(compiled),
          'log':log.relative_to(ROOT).as_posix(),'log_sha256':digest(log),'executable':exe.relative_to(ROOT).as_posix(),'executable_sha256':digest(exe)})
        files.update((manifest,log,exe))
    sparse=json.loads((OUT/'sparse-replay.json').read_text());assert sparse['result']=='PASS' and sparse['queries']==124
    fixture=json.loads((OUT/'original-data.json').read_text());raw=(OUT/'original-data.bin').read_bytes();assert len(raw)==1640 and hashlib.sha256(raw).hexdigest()==fixture['fixture_sha256']
    for r in fixture['slices']:
        part=raw[r['file_offset']:r['file_offset']+r['bytes']];assert hashlib.sha256(part).hexdigest()==r['sha256']
    (OUT/'validation.json').write_text(json.dumps({'native':validation,'guarded_original_sparse_replay':sparse},indent=2)+'\n')
    dependencies=['src/game/collision_tree48i.h','src/game/collision_tree48i.cpp','src/game/collision_query48h.h','src/game/collision_query48h.cpp',
      'src/game/vehicle_contact48g.h','src/assets/collision_mesh48i.h','src/assets/collision_mesh48i.cpp',
      'tools/gameplay/query_binding48i.h','tools/gameplay/query_extract48h.py','tools/gameplay/query_oracle48h.py',
      'evidence/query48H/original-words.json','evidence/query48H/source-anchors.txt',
      'evidence/query48I/cache-binding.json','evidence/query48I/17-ta_colcavetop.rrcol','evidence/query48I/18-zone04_col_floor.rrcol']
    files.update(ROOT/p for p in dependencies)
    files.update(p for p in OUT.rglob('*') if p.is_file() and p.name!='checkpoint-manifest.json')
    records=[{'path':p.relative_to(ROOT).as_posix(),'bytes':p.stat().st_size,'sha256':digest(p)}for p in sorted(files)]
    dest=OUT/'checkpoint-manifest.json'
    dest.write_text(json.dumps({'checkpoint':'48J native prepared limited-list query to terrain surface',
      'original_ELF_sha256':'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30',
      'original_RAM_sha256':'38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48','files':records},indent=2)+'\n')
    for r in json.loads(dest.read_text())['files']:
        p=ROOT/r['path'];assert p.stat().st_size==r['bytes'] and digest(p)==r['sha256']
    print(json.dumps({'verified_files':len(records),'manifest_sha256':digest(dest),'validation':validation}))

if __name__=='__main__':main()
