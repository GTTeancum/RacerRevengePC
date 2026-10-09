"""Freeze 48I source/evidence, verifying every reopened archive member."""
from pathlib import Path
import argparse,datetime,hashlib,json,zipfile
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'outputs'
TAG='RacerRevenge-Checkpoint48I-NativeOrderedCollision'
def sha(p):
 with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def js(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,required=True);a=ap.parse_args();build=a.build.resolve()
 assert __debug__ and not list(OUT.glob(TAG+'-*.zip'))
 audit=js(ROOT/'evidence/collisionMesh48I/audit.json');assert audit['passed'] and audit['build']==build.relative_to(ROOT).as_posix()
 for r in audit['files']:assert sha(ROOT/r['path'])==r['sha256']
 query_manifest=ROOT/'evidence/query48I/checkpoint-manifest.json'
 assert sha(query_manifest)=='549c5133e6c2906baaa7945a9675c29fb55990e8a16fe09445544625d11fdcf6'
 for r in js(query_manifest)['files']:assert sha(ROOT/r['path'])==r['sha256'],r['path']
 dependencies=[]
 for label,digest in [('Source','ebc7ac5f896d04ae2b0c0e8a8520abf417c7850e8367663ceb5a457f377bc344'),('Evidence','9607f4535e07f56d88bdcd91c22a4eee7756a953adbc7438cbe869ccbfd122ff')]:
  p=OUT/f'RacerRevenge-Checkpoint48H-NativeBodyAndQueryRecovery-{label}.zip';assert sha(p)==digest
  dependencies.append(dict(path=p.name,bytes=p.stat().st_size,sha256=digest))
 for arch in ('x64','x86'):
  assert (build/(arch+'.exit')).read_text().strip()=='0'
  log=(build/(arch+'-tests.log')).read_text(encoding='utf-8-sig');assert log.count('Test Passed.')==18
  assert 'collision normal packing checks passed' in log and '347396 root body wrapper checks passed' in log
  for r in js(build/(arch+'-source-hashes.json')):assert sha(Path(r['Path']))==r['Hash'].lower(),r['Path']
  for r in js(build/(arch+'-hashes.json')):assert sha(build/arch/Path(r['Path']).name)==r['Hash'].lower()
 source=[ROOT/p for p in ['README.md','CMakeLists.txt','.gitignore','.gitattributes']]
 allowed={'.cpp','.c','.h','.hpp','.inl','.py','.ps1','.cmake','.md','.json','.toml','.s','.asm'}
 for d in ['src','tests','platform','tools','docs','cmake']:
  source.extend(p for p in (ROOT/d).rglob('*') if p.is_file() and (p.suffix.lower() in allowed or p.name=='Makefile'))
 for d in ['third_party/native-zlib','third_party/native-json']:source.extend(p for p in (ROOT/d).rglob('*') if p.is_file())
 source.extend([ROOT/'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h',OUT/(TAG+'.md')])
 evidence=[OUT/'RacerRevenge-Checkpoint48H-NativeBodyAndQueryRecovery-Manifest.json']
 for d in (ROOT/'evidence').iterdir():
  if d.is_dir() and '48i' in d.name.lower():evidence.extend(p for p in d.rglob('*') if p.is_file())
 manifest=dict(client_checkpoint_date='2026-10-03',created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),dependencies=dependencies,
  goal_status='active',native_gameplay_complete=False,native_racing_fps_measured=False,xbox_execution_verified=False,
  scope='Compact collision runtime, original placements and ordered native terrain queries',audit=audit,
  blockers=['Full spatial candidate/category/callback integration, material selection and spring impulse/friction remain incomplete.',
   'Second body, scene notifications, full live vehicle scheduling and gameplay remain incomplete.',
   'Final renderer, complete native races, 60 presented FPS benchmark and Xbox target validation remain incomplete.'],
  all_members_reopened_sha256_verified=False)
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
