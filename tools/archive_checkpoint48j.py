"""Verify compiled48J state, freeze source/evidence, reopen and hash every member."""
from pathlib import Path
import argparse,datetime,hashlib,json,zipfile
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'outputs'
TAG='RacerRevenge-Checkpoint48J-NativeTerrainAndFriction'
def sha(p):
 with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def js(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,required=True);a=ap.parse_args();build=a.build.resolve()
 assert __debug__ and not list(OUT.glob(TAG+'-*.zip'))
 owned=ROOT/'evidence/query48J/checkpoint-manifest.json'
 assert sha(owned)=='d03bba5441de0b606974e51022e102cf8f4fe036e7b8006ac0e2bdb0ac187db9'
 for r in js(owned)['files']:assert sha(ROOT/r['path'])==r['sha256'],r['path']
 friction=js(ROOT/'evidence/spring48J/friction-fixtures.json')
 assert friction['cases']==1024 and friction['steps']==396096 and friction['source_reads']==0
 assert sha(ROOT/'evidence/spring48J/friction-fixtures.bin')==friction['fixture_sha256']=='c8ef93ae47db9393a9bac6c82bdbbbc35dbacd5ea25badba0211ab8bf391b3b8'
 assert sha(ROOT/'evidence/spring48J/original-words.json')==friction['original_words_sha256']
 impulse=js(ROOT/'evidence/spring48J/impulse-saved-replay.json')
 assert impulse['sparse_replay_passed'] and len(impulse['cases'])==8 and impulse['sparse_bytes']==711
 assert sha(ROOT/'evidence/spring48J/impulse-saved-data.bin')==impulse['sparse_sha256']
 assert impulse['original_words_sha256']==friction['original_words_sha256']
 builds=[]
 for arch in ('x64','x86'):
  assert (build/(arch+'.exit')).read_text().strip()=='0'
  log=(build/(arch+'-tests.log')).read_text(encoding='utf-8-sig')
  assert log.count('Test Passed.')==20 and 'spring48J 16392 checks, 0 failures' in log
  assert 'PASS 5143 checks; 124 original-word limited-list queries' in log
  for r in js(build/(arch+'-source-hashes.json')):assert sha(Path(r['Path']))==r['Hash'].lower(),r['Path']
  for r in js(build/(arch+'-hashes.json')):assert sha(build/arch/Path(r['Path']).name)==r['Hash'].lower()
  mapping=(build/(arch+'-native.map')).read_text(errors='replace').lower()
  assert not any(s in mapping for s in ('ps2runtime','dispatchguest','pcsx2'))
  builds.append(dict(architecture=arch,tests_passed=20,spring_checks=16392,terrain_checks=5143,
   application_sha256=sha(build/arch/'racer_native_pc.exe'),spring_test_sha256=sha(build/arch/'racer_native_spring_contact_check.exe'),
   terrain_test_sha256=sha(build/arch/'racer_native_terrain_query_check.exe')))
 dependencies=[]
 for label,digest in [('Source','0eff7a5f2dff438a808c2eeac59f72f18b0db60d9c912e735d7f8f882ee81932'),('Evidence','69e0b653e46a34d7dac9ca6174aad8eec87884b089799483f6529b2b0d7673ff')]:
  p=OUT/f'RacerRevenge-Checkpoint48I-NativeOrderedCollision-{label}.zip';assert sha(p)==digest
  dependencies.append(dict(path=p.name,bytes=p.stat().st_size,sha256=digest))
 source=[ROOT/p for p in ['README.md','CMakeLists.txt','.gitignore','.gitattributes']]
 allowed={'.cpp','.c','.h','.hpp','.inl','.py','.ps1','.cmake','.md','.json','.toml','.s','.asm'}
 for d in ['src','tests','platform','tools','docs','cmake']:
  source.extend(p for p in (ROOT/d).rglob('*') if p.is_file() and (p.suffix.lower() in allowed or p.name=='Makefile'))
 for d in ['third_party/native-zlib','third_party/native-json']:source.extend(p for p in (ROOT/d).rglob('*') if p.is_file())
 source.extend([ROOT/'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h',OUT/(TAG+'.md')])
 evidence=[OUT/'RacerRevenge-Checkpoint48I-NativeOrderedCollision-Manifest.json']
 for d in (ROOT/'evidence').iterdir():
  if d.is_dir() and '48j' in d.name.lower():evidence.extend(p for p in d.rglob('*') if p.is_file())
 audit=dict(passed=True,build=build.relative_to(ROOT).as_posix(),builds=builds,
  terrain_manifest_sha256=sha(owned),friction_fixture_sha256=friction['fixture_sha256'],
  normal_impulse_native_implemented=False,native_races_verified=False,native_racing_fps_measured=False,xbox_execution_verified=False)
 auditpath=ROOT/'evidence/spring48J/audit.json';auditpath.write_text(json.dumps(audit,indent=2)+'\n');evidence.append(auditpath)
 manifest=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,audit=audit,
  goal_status='active',native_gameplay_complete=False,
  scope='Prepared native terrain filtering/contact state and spring friction; original normal-impact recovery',
  blockers=['Native normal impulse and surrounding spring orchestration remain incomplete.',
   'Live query preparation, world metadata/enumeration, auxiliary/group/callback paths remain incomplete.',
   'Full native simulation, races, AI/camera/audio/progression, final rendering and60-FPS full-race validation remain incomplete.',
   'Xbox target execution and performance remain unverified.'],all_members_reopened_sha256_verified=False)
 for label,files in [('Source',source),('Evidence',evidence)]:
  p=OUT/(TAG+'-'+label+'.zip');records=[]
  with zipfile.ZipFile(p,'x',zipfile.ZIP_DEFLATED,compresslevel=1) as z:
   for item in sorted(set(files)):
    name=item.relative_to(ROOT).as_posix();digest=sha(item);z.write(item,name);assert sha(item)==digest
    records.append(dict(path=name,bytes=item.stat().st_size,sha256=digest))
  with zipfile.ZipFile(p) as z:
   assert len(z.namelist())==len(records)
   for r in records:
    with z.open(r['path']) as stream:assert hashlib.file_digest(stream,'sha256').hexdigest()==r['sha256'],r['path']
  manifest[label]=dict(path=p.name,bytes=p.stat().st_size,sha256=sha(p),files=records)
  print(label,manifest[label]['sha256'],len(records),flush=True)
 manifest['all_members_reopened_sha256_verified']=True
 (OUT/(TAG+'-Manifest.json')).write_text(json.dumps(manifest,indent=2)+'\n')
 print('All archive members reopened and verified. Full native game goal remains active.',flush=True)
if __name__=='__main__':main()
