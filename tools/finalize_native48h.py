"""Audit the actual 48H native build and preserve exact component evidence."""
from pathlib import Path
import argparse,hashlib,json,re
ROOT=Path(__file__).resolve().parents[1]
def sha(p):
 with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def js(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,required=True);a=ap.parse_args();build=a.build.resolve()
 records=[]
 for arch in ('x64','x86'):
  assert (build/(arch+'.exit')).read_text().strip()=='0'
  log=(build/(arch+'-tests.log')).read_text(encoding='utf-8-sig')
  count=log.count('Test Passed.');assert count==16
  match=re.search(r'(\d+) root body wrapper checks passed; (\d+) original-word calls; 8 branch gates; four rounding modes',log)
  assert match and int(match[1])==347396 and int(match[2])==2221
  assert '133735 collision normal packing checks passed; 14586 original faces; four rounding modes' in log
  assert 'PASS 4025 checks; 710 original-word leaf fixtures, hits=434, misses=261, explicit interpolation boundaries=15' in log
  for r in js(build/(arch+'-source-hashes.json')):assert sha(Path(r['Path']))==r['Hash'].lower(),r['Path']
  for r in js(build/(arch+'-hashes.json')):assert sha(build/arch/Path(r['Path']).name)==r['Hash'].lower()
  mapping=(build/(arch+'-native.map')).read_text(errors='replace').lower()
  assert 'ps2runtime' not in mapping and 'dispatchguest' not in mapping and 'pcsx2' not in mapping
  records.append(dict(architecture=arch,ctests=count,wrapper_checks=int(match[1]),original_word_calls=int(match[2]),normal_packing_checks=133735,triangle_query_checks=4025,triangle_query_fixtures=710,
   wrapper_executable_sha256=sha(build/arch/'racer_native_body_wrapper_check.exe'),application_sha256=sha(build/arch/'racer_native_pc.exe')))
 proof=js(ROOT/'evidence/bodyWrapper48H/recovery.json')
 for r in proof['sources']:assert sha(ROOT/r['path'])==r['sha256']
 for r in proof['blocks']:assert sha(ROOT/'evidence/bodyWrapper48H'/(r['name']+'.bin'))==r['sha256']
 files=[ROOT/'src/game/body_wrapper48h.h',ROOT/'src/game/body_wrapper48h.cpp',ROOT/'tests/native/body_wrapper48h.cpp',
        ROOT/'tools/gameplay/body_wrapper_recover48h.py',ROOT/'tools/gameplay/body_wrapper_oracle48h.h',ROOT/'tools/gameplay/body_wrapper_words48h.h',ROOT/'docs/body-wrapper48h.md']
 files.extend(p for p in (ROOT/'evidence/bodyWrapper48H').rglob('*') if p.is_file() and p.name!='audit.json')
 files.extend([ROOT/p for p in ['src/assets/collision_runtime48h.h','src/assets/collision_runtime48h.cpp','tests/native/collision_runtime48h.cpp','tools/assets/collision_runtime_recover48h.py','tools/assets/collision_runtime_words48h.h']])
 files.extend(p for p in (ROOT/'evidence/collisionRuntime48H').rglob('*') if p.is_file())
 query=js(ROOT/'evidence/query48H/leaf-fixtures.json');assert len(query['fixtures'])==710
 assert sum(f['expected_status']==1 for f in query['fixtures'])==434
 assert sum(f['expected_status']==3 for f in query['fixtures'])==15
 assert any(r['matches_saved_contact'] is True and r['hit']==1 for r in query['saved47W_replay'])
 replay=js(ROOT/'evidence/query48H/saved-query-replay.json')
 assert replay['data_bytes_preserved']==1908 and replay['data_slices']==116
 assert any(r['entry']=='00122660' and r['return']==1 and r['steps']==9553 and r['matches_saved_contact'] is True for r in replay['runs'])
 assert sha(ROOT/'evidence/query48H/saved-query-data.bin')=='926805985c79af131f6e93c160d042f9d79defa2a6d01ef11ac1848d0d794f00'
 for r in js(ROOT/'evidence/query48H/checkpoint-manifest.json')['files']:assert sha(ROOT/r['path'])==r['sha256'],r['path']
 out=dict(passed=True,build=build.relative_to(ROOT).as_posix(),builds=records,
          persistent_original_chain_calls=180,scope='Native root-body numerical wrapper and final observed callback, original face-normal packing and triangle leaf; scene render-child notifications and full native query traversal excluded.',
          full_vehicle_substep_verified=False,native_races_verified=False,native_racing_fps_measured=False,xbox_execution_verified=False,
          files=[dict(path=p.relative_to(ROOT).as_posix(),bytes=p.stat().st_size,sha256=sha(p)) for p in sorted(set(files))])
 (ROOT/'evidence/bodyWrapper48H/audit.json').write_text(json.dumps(out,indent=2)+'\n')
 print(json.dumps(dict(passed=True,builds=records)))
if __name__=='__main__':main()
