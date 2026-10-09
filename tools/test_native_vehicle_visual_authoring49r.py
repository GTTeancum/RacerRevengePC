#!/usr/bin/env python3
"""Run bounded49R authoring/constructor proof and isolated semantic mutants."""
import sys
sys.dont_write_bytecode=True
import argparse,hashlib,json,os,re,struct,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
EVIDENCE=ROOT/'evidence/vehicleVisual49R/authoring'
SOURCES=['src/assets/vehicle_visual_authoring49r.cpp','src/assets/pod_physics49n.cpp','src/assets/model_geometry49o.cpp','src/game/scene_graph49p.cpp']
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build-dir',type=Path);ap.add_argument('--sanitize',action='store_true');ap.add_argument('--mutations',action='store_true');ap.add_argument('--verify-only',action='store_true');args=ap.parse_args()
 build=args.build_dir or Path(tempfile.mkdtemp(prefix='rr49r-authoring-'));build.mkdir(parents=True,exist_ok=True);build=build.resolve()
 manifest=json.loads((EVIDENCE/'source-manifest.json').read_text())
 elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
 assert hashlib.sha256(elf).hexdigest()==manifest['elf_sha256']
 phoff=struct.unpack_from('<I',elf,28)[0];phsize,phcount=struct.unpack_from('<HH',elf,42)
 segments=[struct.unpack_from('<8I',elf,phoff+i*phsize) for i in range(phcount)]
 header=(EVIDENCE/'authoring_words49r.h').read_text()
 for category in ['blocks','data']:
  for row in manifest[category]:
   a,z=int(row['start'],16),int(row['end_exclusive'],16)
   source=None
   for kind,offset,virtual,_,size,_,_,_ in segments:
    if kind==1 and virtual<=a and z<=virtual+size:source=elf[offset+a-virtual:offset+z-virtual];break
   assert source is not None and source==(EVIDENCE/(row['name']+'.bin')).read_bytes(),row['name']
   found=re.search(r'\b'+row['name']+r'\[\]=\{([^}]+)\}',header);assert found,row['name']
   encoded=[int(v.strip().rstrip('u'),0) for v in found.group(1).split(',')]
   compiled=struct.pack('<'+'I'*len(encoded),*encoded) if category=='blocks' else bytes(encoded)
   assert compiled==source,'Compiled original-word header differs: '+row['name']
 for row in manifest['blocks']+manifest['data']:
  assert hashlib.sha256((EVIDENCE/(row['name']+'.bin')).read_bytes()).hexdigest()==row['sha256'],row['name']
 for row in manifest['assets']:assert hashlib.sha256((ROOT/row['path']).read_bytes()).hexdigest()==row['sha256'],row['path']
 assert hashlib.sha256((ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy').read_bytes()).hexdigest()==manifest['phy_sha256']
 if args.verify_only:
  print(json.dumps({'verified':True,'elf_sha256':manifest['elf_sha256'],'compiled_original_words':sum((int(r['end_exclusive'],16)-int(r['start'],16))//4 for r in manifest['blocks']),'assets':len(manifest['assets'])}));return 0
 env=dict(os.environ);env['ASAN_OPTIONS']='detect_leaks=0:halt_on_error=1';env['UBSAN_OPTIONS']='halt_on_error=1:print_stacktrace=1'
 flags=['-std=c++17','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off','-frounding-math','-I',str(ROOT/'src/assets')]
 flags+=['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie'] if args.sanitize else ['-O2']
 test=str(ROOT/'tests/native/vehicle_visual_authoring49r.cpp')
 def compile_run(name,source=None):
  binary=build/name;cmd=['g++',*flags,test,*[str(source) if i==0 and source else str(ROOT/s) for i,s in enumerate(SOURCES)],'-o',str(binary)]
  p=subprocess.run(cmd,text=True,capture_output=True,env=env);(build/(name+'-compile.stdout')).write_text(p.stdout);(build/(name+'-compile.stderr')).write_text(p.stderr)
  if p.returncode:raise RuntimeError('Compile failed: '+p.stderr)
  p=subprocess.run([str(binary),str(ROOT),str(build/(name+"-coverage.json"))],text=True,capture_output=True,env=env);(build/(name+'.stdout')).write_text(p.stdout);(build/(name+'.stderr')).write_text(p.stderr);return p
 p=compile_run('vehicle_visual_authoring49r');assert p.returncode==0,p.stderr
 result=json.loads(p.stdout);result['sanitizers']='ASan+UBSan; leak detection disabled' if args.sanitize else 'none'
 result['command_flags']=flags
 if args.mutations:
  original=(ROOT/SOURCES[0]).read_text()
  mutations={
   'wrong_scalar_conversion':('present[id]=true; audit.tokens[id]=token;','result.values[id]=std::nextafter(result.values[id],0.0f); present[id]=true; audit.tokens[id]=token;'),
   'wrong_speed_conversion_constant':('0x3fbbbb84u','0x3fbbbbbbu'),
   'wrong_three_multiply_order':('p.effectiveSpeedSquaredA54=multiply(k,second);','p.effectiveSpeedSquaredA54=multiply(first,first);'),
   'wrong_local_offset_shortcut':('nodes[1].world100.v[12],nodes[1].world100.v[13],nodes[1].world100.v[14]','node.local[12],node.local[13],node.local[14]'),
   'wrong_constructor_W':('zero={{0,0,0,1}},up={{0,1,0,1}}','zero={{0,0,0,0}},up={{0,1,0,1}}'),
  }
  outcomes={}
  for name,(old,new) in mutations.items():
   assert original.count(old)==1,(name,original.count(old));src=build/(name+'.cpp');src.write_text(original.replace(old,new));m=compile_run(name,src)
   assert m.returncode!=0,'Semantic mutant survived: '+name
   assert 'vehicle_visual_authoring49r:' in m.stderr,'Mutant did not fail through a proof assertion: '+name
   outcomes[name]={'detected':True,'diagnostic':m.stderr.strip()}
  result['mutations']=outcomes
 (build/'result.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result));return 0
if __name__=='__main__':sys.exit(main())
