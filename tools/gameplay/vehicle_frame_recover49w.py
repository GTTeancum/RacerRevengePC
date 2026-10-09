#!/usr/bin/env python3
"""Reproduce bounded staged49W graphics/physics differential; no game/RAM/PC use.
Default reports/builds are temporary; --output-dir explicitly persists reports.
No historical recovery, capacity-alias diagnostic, install or ownership repair.
"""
import argparse,concurrent.futures,hashlib,importlib.util,json,os,re,struct,subprocess,sys,tempfile
from pathlib import Path
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[2]
E=ROOT/'evidence/frameBridge49W/frame'
SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
SPEC=importlib.util.spec_from_file_location('retained49v',ROOT/'tools/visual/run_chariot_lifecycle49v.py');OLD=importlib.util.module_from_spec(SPEC);SPEC.loader.exec_module(OLD)
SOURCES=list(dict.fromkeys(OLD.SOURCES[1:]+['src/game/vehicle_frame49w.cpp','src/game/graph_body49w.cpp','src/game/pod_shape49w.cpp','src/assets/pod_shape49w.cpp','src/assets/import_collision48g.cpp','src/game/player_input48x.cpp','src/game/advancement_selectors49k.cpp','src/game/secondary_body49m.cpp','src/game/vehicle_forces48d.cpp','src/game/vehicle_aero48e.cpp','src/game/body_integration48c.cpp','src/game/body_wrapper48h.cpp']))
FLAGS=['-ffp-contract=off','-fno-fast-math','-frounding-math','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-Wno-unused-function']
MUTANTS=[
 ('secondary_uses_duplicate_gravity','s.fields.gravity378:s.fields.gravity37C','x.gravity378:x.gravity37C'),
 ('fresh_speed_before_graphics','const float dt=m.frameFactor;','const float dt=m.frameFactor;(void)refreshCachedSpeed48E(c.root.body.velocity,m.cachedSpeed34);'),
 ('reset_impact_before_graphics','const float dt=m.frameFactor;','const float dt=m.frameFactor;r.fields.impactBA0=r.visualShared.zero434630;'),
 ('omit_postgraphics_impact_reset','r.fields.impactBA0=r.visualShared.zero434630;','/* omitted reset */'),
 ('lose_secondary_derivative_buffer','z.phase=SecondaryPhase49M::Ready;r.bodies.phase=','r.secondarySupplement.positionDerivative2B8={};z.phase=SecondaryPhase49M::Ready;r.bodies.phase='),
 ('main_clear20c_omitted','clearMainBodyPost20C49K(c.body20C);','/* omitted clear */'),
 ('query_uses_previous_position','node(r,b.lifecycle.main190).world100.v[12+i]','node(r,b.lifecycle.main190).localC0.v[i]'),
 ('query_forces_w','for(unsigned i=0;i<3;++i)m.currentPoint7C0[i]=','m.currentPoint7C0[3]=1;for(unsigned i=0;i<3;++i)m.currentPoint7C0[i]='),
 ('feedback_omitted','m.components.feedback=selectLateFeedback49K(m.components.probe348.field104,m.response354_3C,m.response354_44);','/* omitted feedback */'),
 ('late_shape_omitted','if(!transformPodShape49W(p.shape,node(r,b.lifecycle.main190).world100,r.shape))','if((static_cast<void>(p),false))'),
 ('secondary_double_gravity','f.mul(f.neg(par.body.mass),g)','f.mul(f.neg(par.body.mass),f.mul(2,g))'),
 ('secondary_early_refresh','RR_VEHICLE_FRAME49W_STAGE(0x218a3c);','(void)refreshSceneWorld49P(graph(s),b.lifecycle.physical19C);RR_VEHICLE_FRAME49W_STAGE(0x218a3c);'),
 ('reader_uses_duplicate_source','c.source7D0=s.fields.damage7D0;','c.source7D0=x.context.source7D0;'),
]
def sha(b):return hashlib.sha256(b).hexdigest()
def authenticate():
 elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(elf)==SHA
 po=struct.unpack_from('<I',elf,28)[0];ps,pn=struct.unpack_from('<HH',elf,42);segs=[struct.unpack_from('<8I',elf,po+i*ps)for i in range(pn)]
 def take(a,z):return next(elf[o+a-v:o+z-v]for k,o,v,_,n,*_ in segs if k==1 and v<=a<z<=v+n)
 d=ROOT/'evidence/frameBridge49W/independent-review';m=json.loads((d/'source-authentication.json').read_text());header=(d/'frame_order_words49w.h').read_text();count=0
 for name,b in m['blocks'].items():
  a,z=int(b['start'],0),int(b['end_exclusive'],0);data=take(a,z);assert sha(data)==b['sha256']
  block=re.search(r'\b'+name+r'\[\]=\{(.*?)\};',header,re.S);assert block,name
  words=[int(x,16)for x in re.findall(r'0x([0-9a-f]+)u',block[1])];assert struct.pack('<%dI'%len(words),*words)==data;count+=len(words)
 shape_header=ROOT/'evidence/frameBridge49W/connected/shape_original49w.h';shape_text=shape_header.read_text();shape_count=0
 for address,count_words,index,name in re.findall(r'\{0x([0-9a-f]+)u,(\d+)u,(\d+)u,(\w+)\}',shape_text):
  block=re.search(r'\b'+name+r'\[\]=\{(.*?)\};',shape_text,re.S);assert block,name
  words=[int(x,16)for x in re.findall(r'0x([0-9a-f]+)u',block[1])];assert len(words)==int(count_words)
  data=struct.pack('<%dI'%len(words),*words);a=int(address,16);assert data==take(a,a+len(data)),name;shape_count+=len(words)
 assert shape_count==155
 # Freeze identity of retained APIs; this driver executes only its new test.
 retained=[*ROOT.glob('src/game/*49v.*'),*ROOT.glob('src/game/*49m.*'),*ROOT.glob('src/game/*49k.*')]
 return {'elf_sha256':SHA,'fresh_caller_secondary_words':count,'shape_original_words':shape_count,'shape_original_header_sha256':sha(shape_header.read_bytes()),'retained_files':{str(p.relative_to(ROOT)):sha(p.read_bytes())for p in retained}}
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--output-dir',type=Path);ap.add_argument('--skip-sanitized',action='store_true');ap.add_argument('--skip-mutations',action='store_true');args=ap.parse_args();source=authenticate()
 with tempfile.TemporaryDirectory(prefix='vehicle-frame49w-')as temp:
  build=Path(temp)/'build';build.mkdir();out=args.output_dir.resolve()if args.output_dir else Path(temp)/'reports';out.mkdir(parents=True,exist_ok=True)
  def save(name,obj):(out/name).write_text(json.dumps(obj,indent=2)+'\n')
  def run(cmd,name,env=None,expect=True):
   r=subprocess.run([str(x)for x in cmd],cwd=ROOT,text=True,capture_output=True,env=env);(out/(name+'.stdout')).write_text(r.stdout);(out/(name+'.stderr')).write_text(r.stderr)
   if expect and r.returncode:raise RuntimeError(name+': '+r.stdout[-2000:]+r.stderr[-5000:])
   return r
  cxx=os.environ.get('CXX','c++');compiler=run([cxx,'--version'],'compiler').stdout.splitlines()[0]
  def flags(traced,san):
   f=FLAGS+(['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie']if san else['-O2','-g'])
   if traced:f+=['-include',E/'frame_stage_trace.h']
   return f
  def compile_set(tag,traced=True,san=False):
   f=flags(traced,san);objects=[]
   def one(src):
    obj=build/(src.replace('/','_')+'-'+tag+'.o');run([cxx,'-std=c++17',*f,'-c',ROOT/src,'-o',obj],'compile-'+obj.stem);return obj
   with concurrent.futures.ThreadPoolExecutor(max_workers=3)as pool:objects=list(pool.map(one,SOURCES))
   test=build/('test-'+tag+'.o');run([cxx,'-std=c++20',*f,'-c',ROOT/'tests/native/vehicle_frame49w.cpp','-o',test],'compile-test-'+tag)
   exe=build/('frame-'+tag);run([cxx,*f,*(['-no-pie']if san else[]),test,*objects,'-o',exe],'link-'+tag);return exe,test,objects,f
  clean=dict(os.environ);clean.pop('RR_VEHICLE_FRAME49W_COVERAGE',None)
  exe,test,objects,fl=compile_set('normal');normal=json.loads(run([exe,ROOT],'normal',clean).stdout);assert normal['passed']and normal['native_allocations']==0 and normal['shape_original_comparisons']==normal['substeps'];save('normal.json',normal)
  plain,_,_,_=compile_set('plain',False);plain_result=json.loads(run([plain,ROOT],'plain',clean).stdout);assert {k:v for k,v in normal.items()if k not in('checks','trace_checks')}=={k:v for k,v in plain_result.items()if k not in('checks','trace_checks')};save('plain.json',plain_result)
  mutations=[]
  if not args.skip_mutations:
   src=(ROOT/'src/game/vehicle_frame49w.cpp').read_text();where=SOURCES.index('src/game/vehicle_frame49w.cpp')
   for name,before,after in MUTANTS:
    assert src.count(before)==1,(name,src.count(before));p=build/(name+'.cpp');p.write_text(src.replace(before,after));obj=build/(name+'.o');r=run([cxx,'-std=c++17',*fl,'-I',ROOT/'src/game','-c',p,'-o',obj],'mutation-build-'+name,expect=False)
    if r.returncode:raise RuntimeError('Mutation didnot compile: '+name+' '+r.stderr)
    obs=list(objects);obs[where]=obj;binary=build/name;run([cxx,*fl,test,*obs,'-o',binary],'mutation-link-'+name)
    result=run([binary,ROOT],'mutation-'+name,clean,False);assert result.returncode!=0 and'FAIL frame49W:'in result.stderr,(name,result.stdout,result.stderr)
    mutations.append({'name':name,'kind':'isolated_native_semantic','diagnostic':result.stderr.strip()})
  save('mutations.json',{'count':len(mutations),'detected':mutations,'setup_failures_counted':0,'production_edited':False})
  sanitized=None
  if not args.skip_sanitized:
   binary,_,_,_=compile_set('sanitized',True,True);env=dict(clean,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1');sanitized=json.loads(run([binary,ROOT],'asan-ubsan',env).stdout);assert sanitized==normal;save('sanitized.json',sanitized)
  files=[ROOT/'src/game/vehicle_frame49w.h',ROOT/'src/game/vehicle_frame49w.cpp',ROOT/'tests/native/vehicle_frame49w.cpp',Path(__file__),*sorted(E.glob('*.h')),ROOT/'evidence/frameBridge49W/connected/shape_original49w.h']
  summary={'passed':True,'scope':'Bounded single-vehicle staged advancing graphics/physics diagnostic with actual53node forest and supplied preserving opaque/world/contact/query cuts; not completeoriginalframe','source':source,'compiler':compiler,'production_standard':'C++17','test_standard':'C++20','flags':FLAGS,'normal':normal,'plain':plain_result,'sanitized':sanitized,'mutants':len(mutations),'source_substitutions':0,'leak_sanitizer':'disabled; noLSan claim','files':{str(p.relative_to(ROOT)):sha(p.read_bytes())for p in files}}
  save('validation.json',summary);print(json.dumps({'passed':True,'frames':normal['frames'],'substeps':normal['substeps'],'mutants':len(mutations),'sanitized':sanitized is not None,'output':str(out)if args.output_dir else'temporary'}))
if __name__=='__main__':main()
