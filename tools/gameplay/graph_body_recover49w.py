#!/usr/bin/env python3
"""Authenticate and reproduce graph-aware original167C40 realreturn proof.

Default output is temporary. Only --output-dir persists evidence. No RAM read,
old RAM-dependent script, game/PC runtime, install, or gameplay target hook.
"""
import argparse,hashlib,json,os,re,struct,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SOURCE=ROOT/'evidence/frameBridge49W/body/source'
TEST=ROOT/'tests/native/graph_body49w.cpp'
TRACE=SOURCE/'trace.h'
SOURCES=['graph_body49w','body_integration48c','body_scene48g','scene_graph49p','chariot_scene49u']
FLAGS=['-ffp-contract=off','-fno-fast-math','-Wall','-Wextra','-Werror','-Wno-misleading-indentation']
SOURCE_MUTANTS=[
 ('previous_full_vector_copy_omitted',0x167c84,0),
 ('position_derivative_x_stale',0x167eb4,0),
 ('linear_current_derivative_x_stale',0x167de4,0),
 ('angular_current_derivative_x_stale',0x167fdc,0),
 ('angular_acceleration_W_increment_omitted',0x167f04,0),
 ('point_W_producer_omitted',0x28bb78,0),
 ('local_velocity_full_copy_omitted',0x1682e4,0),
 ('world_angular_full_copy_omitted',0x1682f8,0),
 ('force_full_clear_omitted',0x168304,0),
 ('accumulator1C0_clear_omitted',0x16831c,0),
 ('rotation_correction_clear_omitted',0x168340,0),
 ('callback238_clear_omitted',0x1654b4,0),
 ('axis_translation_restore_omitted',0x112c40,0),
]
NATIVE_MUTANTS=[
 ('setter_before_first_point','graph_body49w','        V point{};','        V point{};\n        if (!setParentlessWorldMatrix49P(staged, binding, node(staged, binding).localC0)) return fail(D::Arithmetic);'),
 ('eager_world_equals_local','graph_body49w','        V point{};','        V point{};\n        node(staged, binding).world100 = node(staged, binding).localC0;'),
 ('retain_old_quaternion_translation','graph_body49w','!setParentlessWorldMatrix49P(staged, binding, orientation)','!(orientation.v[12] = node(staged, binding).localC0.v[12], setParentlessWorldMatrix49P(staged, binding, orientation))'),
 ('missing_observer_rejection','graph_body49w','if (active && domain.rootObserverPresent19C)','if (false && active && domain.rootObserverPresent19C)'),
 ('scalar_zero_rotation_not_skipped','graph_body49w','if (length != 0.0f) {','if (true) {'),
 ('previous90W_forced_one','graph_body49w','std::memcpy(&s.previousVelocity90W, &lanes.velocity70W, sizeof(float));','s.previousVelocity90W = 1.0f;'),
 ('position_derivative_not_continued','graph_body49w','s.positionDerivative2B8 = b.body.velocity;','/* mutant leaves current position derivative stale */'),
 ('angular_D0W_forced_one','graph_body49w','s.angularAccelerationD0W = 2.0f;','s.angularAccelerationD0W = 1.0f;'),
 ('supplement_reset_on_skip','graph_body49w','    clear(w);','    if (!active) w.supplement = {};\n    clear(w);'),
 ('only_root_graph_commit','graph_body49w','if (graph.count) std::memcpy(graph.nodes, w.nodes, graph.count * sizeof(SceneNode49P));','if (graph.count && binding != I::none) graph.nodes[static_cast<unsigned>(binding)] = w.nodes[static_cast<unsigned>(binding)];'),
('blanket_descendant_invalidation', 'scene_graph49p', '    if (node.nextSibling188 != none) invalidate(graph, node.nextSibling188);\n    if ((node.sceneFlags190 & 1u) == 0) return;', '    if (node.nextSibling188 != none) invalidate(graph, node.nextSibling188);\n    /* mutant recursively clears even dirty-node descendants */'),
]
def sha(b):return hashlib.sha256(b).hexdigest()
def authenticate():
 raw=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();m=json.loads((SOURCE/'source-manifest.json').read_text());assert sha(raw)==m['elf_sha256']
 po=struct.unpack_from('<I',raw,28)[0];ps,pn=struct.unpack_from('<HH',raw,42);segments=[struct.unpack_from('<8I',raw,po+i*ps)for i in range(pn)]
 assert [list(s)for s in segments]==m['elf_load_segments']
 def take(a,z):return next(raw[o+a-v:o+z-v]for k,o,v,_,n,*_ in segments if k==1 and v<=a<z<=v+n)
 header=(SOURCE/'body_words.h').read_text();words={}
 for b in m['blocks']:
  a,z=int(b['start'],16),int(b['end_exclusive'],16);data=take(a,z);assert sha(data)==b['sha256'] and data==(SOURCE/(b['name']+'.bin')).read_bytes()
  match=re.search(r'\b'+b['name']+r'\[\]=\{(.*?)\};',header,re.S);assert match
  values=[int(w,16)for w in re.findall(r'0x([0-9a-f]+)u',match[1])];assert len(values)==b['words'];assert f'{{\"{b["name"]}\",0x{b["start"]}u,{b["words"]},{b["index"]},{b["name"]}}},' in header;assert struct.pack('<%dI'%len(values),*values)==data
  rows=[tuple(int(x,16)for x in row.split())for row in(SOURCE/(b['name']+'.txt')).read_text().splitlines()];assert rows==[(a+4*i,w)for i,w in enumerate(values)]
  for pc,w in rows:assert pc not in words;words[pc]=w
 for b in m['static']:
  data=take(int(b['start'],16),int(b['end_exclusive'],16));assert sha(data)==b['sha256'] and data==(SOURCE/(b['name']+'.bin')).read_bytes()
 assert struct.unpack('<I',(SOURCE/'rigid_callback_slot.bin').read_bytes())[0]==0x1654b0
 a,z=0x3df590,0x3df5b0;assert any(k==1 and v+n<=a<z<=v+size for k,_,v,_,n,size,*_ in segments)
 original=(ROOT/m['retained_machine']).read_text();assert sha(original.encode())==m['retained_machine_sha256']
 expected=original.replace('"lifecycle_words.h"','"body_words.h"').replace('chariot49v_words','graph49w_words').replace('chariot49v_original','graph49w_original')
 assert (SOURCE/'body_machine.h').read_text()==expected,'Independent original-word interpreter changed beyond namespace/word-table binding'
 assert len(words)==m['words'];return m,words

def account(path,m,words):
 counts={int(pc,16):(int(h),int(t),int(n))for pc,h,t,n in(line.split()for line in path.read_text().splitlines())};assert counts.keys()==words.keys()
 categories={'base_component':'Nonempty component routing excluded by explicit component context. Both empty routes execute.', 'observer':'Nonzero observer requires a different unrecovered bounds/observer transaction and is rejected atomically.', 'root_translation':'Only parentless target and relative=null are admitted.', 'root_setter':'Only parentless target and relative=null are admitted.'}
 result=[]
 for b in m['blocks']:
  pcs=range(int(b['start'],16),int(b['end_exclusive'],16),4);executed=[pc for pc in pcs if counts[pc][0]];missed=[f'{pc:08x}'for pc in pcs if not counts[pc][0]]
  result.append(dict(name=b['name'],words=b['words'],executed_words=len(executed),unexecuted=missed,unexecuted_scope=categories.get(b['name'],'Alignment/delay slots, alternative finite branches, or non-root helper routes outside this corpus; no universal instruction/branch coverage claim.')))
 assert all(counts[pc][0]for pc in [0x167c40,0x141ed0,0x167c7c,0x168194,0x1681f0,0x16822c,0x168234,0x1682c4,0x1682fc,0x1654b4,0x168364,0x168368])
 for pc in [0x167c68,0x167c74,0x167ce0,0x168194,0x1681f0,0x28bad8]:
  if pc in counts and (words[pc]>>26 in [4,5,0x14,0x15] or words[pc]>>26==0x11 and words[pc]>>21&31==8):assert counts[pc][1] and counts[pc][2],hex(pc)
 return dict(blocks=result,total_words=len(words),executed_words=sum(bool(x[0])for x in counts.values()),executed_instructions=sum(x[0]for x in counts.values()),call_target_substitutions=0,real_return=True)

def main():
 p=argparse.ArgumentParser();p.add_argument('--output-dir',type=Path);p.add_argument('--skip-sanitized',action='store_true');p.add_argument('--skip-mutations',action='store_true');a=p.parse_args();m,words=authenticate()
 with tempfile.TemporaryDirectory(prefix='graph-body49w-')as tmp:
  temp=Path(tmp);out=a.output_dir.resolve()if a.output_dir else temp/'evidence';out.mkdir(parents=True,exist_ok=True);build=temp/'build';build.mkdir()
  def save(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n')
  def run(cmd,name,env=None,success=True):
   r=subprocess.run([str(x)for x in cmd],cwd=ROOT,text=True,capture_output=True,env=env);(out/(name+'.stdout')).write_text(r.stdout);(out/(name+'.stderr')).write_text(r.stderr)
   if success:assert r.returncode==0,f'{name}: {r.stdout}\n{r.stderr}'
   return r
  cxx=os.environ.get('CXX','c++');compiler=run([cxx,'--version'],'compiler').stdout.splitlines()[0]
  def compile_set(tag,traced=True,sanitizer=False):
   flags=FLAGS+(['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie']if sanitizer else ['-O2','-g'])+(['-include',TRACE]if traced else [])
   objs={}
   for source in SOURCES:
    obj=build/(source+'-'+tag+'.o');run([cxx,'-std=c++17',*flags,'-fstack-usage','-c',ROOT/'src/game'/(source+'.cpp'),'-o',obj],'build-'+source+'-'+tag);objs[source]=obj
    if tag=='plain':(out/(source+'.su')).write_text(obj.with_suffix('.su').read_text())
   test=build/('test-'+tag+'.o');run([cxx,'-std=c++20',*flags,'-c',TEST,'-o',test],'build-test-'+tag)
   exe=build/tag;run([cxx,*flags,*(['-no-pie']if sanitizer else []),test,*objs.values(),'-o',exe],'link-'+tag);return exe,objs,test,flags
  clean=dict(os.environ);clean.pop('RR_GRAPH_BODY49W_COVERAGE',None)
  exe,objs,test,flags=compile_set('normal');normal=json.loads(run([exe],'normal',dict(clean,RR_GRAPH_BODY49W_COVERAGE=str(out/'coverage.tsv'))).stdout);assert normal['passed'] and normal['native_allocations']==0;save('normal.json',normal)
  save('coverage-accounting.json',account(out/'coverage.tsv',m,words));before=sha((out/'coverage.tsv').read_bytes());readonly=json.loads(run([exe],'read-only-default',clean).stdout);assert readonly==normal and before==sha((out/'coverage.tsv').read_bytes());save('read-only-default.json',dict(passed=True,evidence_unchanged=True,explicit_output_only=True))
  plain,plain_objs,_,_=compile_set('plain',False);plain_result=json.loads(run([plain],'plain',clean).stdout)
  for k in ['cases','rejections','leaf_cases','original_instructions','rotation_calls','scalar_zero_skips','native_allocations']:assert plain_result[k]==normal[k]
  save('plain.json',plain_result)
  undefined=run(['nm','-u',plain_objs['graph_body49w']],'native-undefined').stdout;assert not any(x in undefined for x in ['_Znwm','_Znam','malloc','calloc','realloc','pthread'])
  mutants=[]
  if not a.skip_mutations:
   for name,pc,word in SOURCE_MUTANTS:
    assert pc in words and word!=words[pc];r=run([exe,f'{pc:x}',f'{word:x}'],'mutant-'+name,clean,False);assert r.returncode!=0 and 'mismatch' in r.stderr,(name,r.stderr);mutants.append(dict(name=name,kind='original_word',pc=f'{pc:08x}',word=f'{word:08x}',mismatch=r.stderr.strip()))
   for name,source,before,after in NATIVE_MUTANTS:
    original=(ROOT/'src/game'/(source+'.cpp')).read_text();assert original.count(before)==1,(name,original.count(before));file=build/(name+'.cpp');file.write_text(original.replace(before,after));obj=build/(name+'.o')
    run([cxx,'-std=c++17',*flags,'-I',ROOT/'src/game','-c',file,'-o',obj],'mutant-build-'+name);binary=build/name;run([cxx,*flags,test,*[obj if k==source else v for k,v in objs.items()],'-o',binary],'mutant-link-'+name);r=run([binary],'mutant-'+name,clean,False);assert r.returncode!=0 and 'mismatch' in r.stderr,(name,r.stderr);mutants.append(dict(name=name,kind='isolated_native_source',mismatch=r.stderr.strip()))
  save('mutations.json',dict(count=len(mutants),detected=mutants,production_edited=False,setup_failures_counted=0))
  sanitized=None
  if not a.skip_sanitized:
   san,_,_,_=compile_set('sanitized',True,True);sanitized=json.loads(run([san],'asan-ubsan',dict(clean,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')).stdout);assert sanitized==normal;save('sanitized.json',sanitized)
  inputs=[Path(__file__),TEST,TRACE,SOURCE/'body_machine.h',SOURCE/'body_fixture.h',SOURCE/'body_words.h',SOURCE/'source-manifest.json',SOURCE/'freeze_source.py']+[ROOT/'src/game'/(s+'.cpp')for s in SOURCES]+[ROOT/'src/game/graph_body49w.h']
  save('validation.json',dict(passed=True,scope='Complete admitted original167C40 through actual168364/368 return, with actual empty141ED0 route, explicit root observer absent, original14EE40 integrator and1654B0 callback; synthetic real190->194 forest and persistent canonical body.',elf_sha256=m['elf_sha256'],normal=normal,plain=plain_result,sanitized=sanitized,compiler=compiler,flags=FLAGS,production_language='C++17',test_language='C++20',native_call_target_substitutions=0,RAM_reads=0,old_files_edited=False,mutations=len(mutants),leak_sanitizer='disabled; no LSan claim',template_domain=m['template_domain'],hashes={str(p.relative_to(ROOT)):sha(p.read_bytes())for p in inputs}))
  print(json.dumps(dict(passed=True,cases=normal['cases'],leaf_cases=normal['leaf_cases'],mutations=len(mutants),sanitized=sanitized is not None,output=str(out)if a.output_dir else 'temporary')))
if __name__=='__main__':main()
