"""Create/reopen exact owned-file manifest; optional verified integration records."""
from pathlib import Path
import argparse,hashlib,json,shutil
from recover48g import ROOT
def sha(b):return hashlib.sha256(b).hexdigest()
def main():
 parser=argparse.ArgumentParser();parser.add_argument('--integrated');args=parser.parse_args()
 out=ROOT/'evidence/gameplay48G';validation={'component':'original terrain-following response48G','standalone':{
  'architecture':'x64','checks':241921,'original_word_calls':5900,'branch_gates':12,'host_rounding_modes':4,
  'log':'evidence/gameplay48G/iteration-03.log','result':'PASS'},'integrated':[],
  'limitations':['Not a full vehicle/substep or gameplay performance test.','Surface query and SpringCollisionModel remain external.',
  'Original root-scene numeric operations compared; child notifications excluded.','Normal finite/zero arithmetic tested; not all PS2 exception/denormal modes.']}
 assert '241921 checks passed' in (out/'iteration-03.log').read_text()
 files=[ROOT/'src/game/vehicle_contact48g.h',ROOT/'src/game/vehicle_contact48g.cpp',ROOT/'tests/native/physics48g.cpp',
  ROOT/'tools/gameplay/recover48g.py',ROOT/'tools/gameplay/dependencies48g.py',ROOT/'tools/gameplay/finalize48g.py',
  ROOT/'tools/gameplay/contact_words48g.h',ROOT/'tools/gameplay/contact_oracle48g.h',ROOT/'docs/vehicle-contact48g.md']
 if args.integrated:
  build=(ROOT/args.integrated).resolve();assert build.is_relative_to(ROOT)
  for arch in ['x64','x86']:
   log=build/f'{arch}-tests.log';t=log.read_text()
   assert 'native_terrain_following' in t and '241921 checks passed' in t and 'Test Failed.' not in t
   assert t.count('Test Passed.')==13
   candidates=[build/arch/'racer_native_contact_check.exe',build/arch/'frozenbinary'/'racer_native_contact_check.exe']
   exe=next((p for p in candidates if p.exists()),None)
   record={'architecture':arch,'result':'PASS','log':log.relative_to(ROOT).as_posix(),'log_sha256':sha(log.read_bytes())}
   files.append(log)
   build_sources=build/f'{arch}-source-hashes.json';rows=json.loads(build_sources.read_text())
   for relative in ['src/game/vehicle_contact48g.h','src/game/vehicle_contact48g.cpp','tests/native/physics48g.cpp',
                    'tools/gameplay/contact_words48g.h','tools/gameplay/contact_oracle48g.h']:
    wanted=(ROOT/relative).resolve();entry=next(row for row in rows if Path(row['Path']).resolve()==wanted)
    assert entry['Hash'].lower()==sha(wanted.read_bytes())
   files.append(build_sources);record['compiled_source_hashes']=build_sources.relative_to(ROOT).as_posix()
   record['compiled_contact_files_verified']=5
   if exe is not None:record['executable']=exe.relative_to(ROOT).as_posix();record['executable_sha256']=sha(exe.read_bytes());files.append(exe)
   validation['integrated'].append(record)
 (out/'validation.json').write_text(json.dumps(validation,indent=2)+'\n')
 files.extend(p for p in out.rglob('*') if p.is_file() and p.name!='checkpoint-manifest.json')
 records=[{'path':p.relative_to(ROOT).as_posix(),'bytes':p.stat().st_size,'sha256':sha(p.read_bytes())} for p in sorted(set(files))]
 manifest={'checkpoint':'48G terrain-following response','scope':'Owned source, original evidence and validation records; parent checkpoint archives project integration.',
  'files':records}
 destination=out/'checkpoint-manifest.json';destination.write_text(json.dumps(manifest,indent=2)+'\n')
 reopened=json.loads(destination.read_text())
 for record in reopened['files']:
  b=(ROOT/record['path']).read_bytes();assert len(b)==record['bytes'] and sha(b)==record['sha256']
 print(json.dumps({'verified_files':len(records),'manifest_sha256':sha(destination.read_bytes()),'integrated_architectures':[x['architecture'] for x in validation['integrated']]}))
if __name__=='__main__':main()
