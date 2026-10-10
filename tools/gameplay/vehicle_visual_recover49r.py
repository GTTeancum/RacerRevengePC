#!/usr/bin/env python3
"""Verify/reproduce complete admitted21C120 word differential proof.

Default output lives only in a temporary directory. Evidence persistence needs
--output-dir; test coverage output requires RR_VISUAL49R_COVERAGE. No RAM files,
original game launch, repository installation, old-source writes, or network.
"""
import argparse,hashlib,json,os,re,struct,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SOURCE=ROOT/'evidence/vehicleVisual49R/source'
PRODUCTION=['src/game/vehicle_visual49r.cpp','src/game/visual_container49r.cpp','src/game/visual_response49r.cpp','src/game/scene_graph49p.cpp','src/game/body_scene48g.cpp','src/game/gradient_noise49m.cpp','src/assets/vehicle_visual_authoring49r.cpp','src/assets/pod_physics49n.cpp','src/assets/model_geometry49o.cpp']
FLAGS=['-DRR_VISUAL49R_ENABLE_STAGE_TRACE=1','-ffp-contract=off','-fno-fast-math','-Wall','-Wextra','-Werror','-Wno-misleading-indentation']
MUTATIONS=[
 ('bit1_reads_bit0',0x21d82c,0x000217fc),
 ('right_binder_sign',0x21e110,0x46000006),
 ('left_binder_torque_sign',0x21ea44,0x46000006),
 ('left_reads_right_clock',0x21f0dc,0xc6210900),
 ('right_targets_left_root',0x21e9f0,0x8e2401bc),
 ('right_y_clamp_feedback_omitted',0x21e814,0),
 ('right_y_clamp_velocity_omitted',0x21e81c,0),
 ('empty_tail_global_store_omitted',0x2205b4,0),
 ('noise_rng_increment_changed',0x308050,0x2463303a),
 ('parent_conversion_wrong_axis',0x289df8,0x4beb30be),
 ('quaternion_add_wrong_sign',0x14f348,0x46000801),
]
NATIVE_MUTATIONS=[
 ('recompute_left_binder_after_right', '        V force = shared.zero434630, torque = shared.zero434630;', '''        if (!right) {
            const V changed = f.difference(s.right.position, s.left.position);
            binder = f.vuNormalize(changed);
            const float changedScalar = f.mul(p.binderSpringA10, f.sub(f.scalarRoot(f.squared(changed, false)), restLength));
            for (unsigned i = 0; i < 3; ++i) binder.v[i] = f.mul(binder.v[i], changedScalar);
        }
        V force = shared.zero434630, torque = shared.zero434630;'''),
 ('left_integrates_before_right','const bool right = side == 0;','const bool right = side == 1;'),
 ('scalar_rsqrt_replaced_with_vu_root_divide','volatile float x = static_cast<float>(1.0 / std::sqrt(static_cast<double>(a)));','volatile float root = std::sqrt(a); volatile float x = 1.0f / root;'),
]
def sha(data):return hashlib.sha256(data).hexdigest()
def verify():
 raw=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();m=json.loads((SOURCE/'source-manifest.json').read_text());assert sha(raw)==m['elf_sha256']
 po=struct.unpack_from('<I',raw,28)[0];ps,pn=struct.unpack_from('<HH',raw,42);segments=[struct.unpack_from('<8I',raw,po+i*ps)for i in range(pn)]
 def take(a,z):
  return next(raw[o+a-v:o+z-v]for k,o,v,_,n,*_ in segments if k==1 and v<=a<z<=v+n)
 header=(SOURCE/'visual_words.h').read_text();words={}
 for b in m['blocks']:
  a,z=int(b['start'],16),int(b['end_exclusive'],16);data=take(a,z);assert sha(data)==b['sha256'];assert data==(SOURCE/(b['name']+'.bin')).read_bytes()
  numeric=re.search(r'\b'+b['name']+r'\[\]=\{(.*?)\};',header,re.S);assert numeric
  values=[int(x,16)for x in re.findall(r'0x([0-9a-f]+)u',numeric[1])];assert len(values)==b['words'];assert struct.pack('<%dI'%len(values),*values)==data
  listing=[tuple(int(x,16)for x in line.split()[:2])for line in (SOURCE/(b['name']+'.txt')).read_text().splitlines()];assert listing==[(a+4*i,x)for i,x in enumerate(values)]
  for pc,w in listing:assert pc not in words;words[pc]=w
 for b in m['data']:
  a,z=int(b['start'],16),int(b['end_exclusive'],16);data=take(a,z);assert sha(data)==b['sha256'];assert data==(SOURCE/(b['name']+'.bin')).read_bytes()
 assert len(words)==m['word_count'];return m,words

def exclusion(pc,block):
 if block=='complete_visual':
  ranges=[(0x21d960,0x21d970,'Optional1C0 angle snapshot excluded by absent root'),(0x21d97c,0x21d98c,'Optional1C4 angle snapshot excluded by absent root'),(0x21db3c,0x21de80,'Four-engine binder excluded by absent optional roots'),(0x21e02c,0x21e040,'No-binder fallback excluded by both primary roots present'),(0x21e0bc,0x21e0fc,'Optional1C4 right partner excluded'),(0x21f32c,0x21fc70,'Optional1C0 response excluded'),(0x21fc7c,0x2205a0,'Optional1C4 response excluded'),(0x2205c8,0x2205fc,'Rotating-node body excluded by explicitB54=0'),(0x220610,0x22085c,'Nonzero834 route excluded; normal restoresRA in22060C delay'),(0x21c6d8,0x21c6e0,'Turn ratio is strictly positive after absB5C>0.05; ascending interpolation cannot undershoot start'),(0x21c708,0x21c70c,'Positive turn ratio cannot overshoot descending start'),(0x21c78c,0x21c794,'Positive turn ratio cannot undershoot ascending time start'),(0x21c7bc,0x21c7c0,'Positive turn ratio cannot overshoot descending time start')]
  for a,z,why in ranges:
   if a<=pc<z:return 'excluded_or_unreachable',why
  raise AssertionError(f'unaccounted complete-call word {pc:08x}')
 if block=='shared_vector_initializer':return 'provenance_only','Static zero-vector provenance is frozen, not executed by21C120; source registration helper is not part of this call.'
 if block=='noise_initializer':return 'excluded_or_unreachable','RNG masks result with7FFFFFFF; signed-negative remainder corrections are unreachable for every seed.'
 if block=='world_refresh':return 'excluded_or_unreachable','21C120 getters inline their dirty refresh; reached2897B0 calls see already-ready parentless main after prefix getter.'
 if block=='mark_ancestors':return 'excluded_or_unreachable','Updated engine roots are parentless and194 has only parentless main; no third ancestor.'
 if block=='world_product':return 'excluded_or_unreachable','Full retained helper range contains a separate trailing scene function/alignment after executed2899B0 return.'
 if block=='orientation_setter':return 'excluded_or_unreachable','Selected28A090 arguments are relative=null,t0=0,t1=1,t2=0 with canonical parented194; alternate orientations/relative routes excluded.'
 if block in ('position_getter','axes_getter','inverse_direction'):return 'excluded_or_unreachable','Only selected null-relative/getter and ready main direction paths execute; other relative/parent refresh branches are outside call admission.'
 if block=='original_angle':return 'excluded_or_unreachable','Unreachable duplicate setup after unconditional branch or alignment; all finite axis/quadrant cases exercised.'
 if block in ('invalidate_descendants','local_matrix_setter'):return 'excluded_or_unreachable','Alignment after real return/delay; all selected dirty/ready sibling/child branches exercised.'
 # These complete helpers execute original words with no replacements. Their
 # unvisited per-PC branches are retained and reported, not falsely counted as
 # executed or proven unreachable over all possible finite inputs.
 return 'unexercised_helper_branch','Full original software-double noise helper; this PC was not reached by the explicit bounded seeds/positive finite norms. No universal library-branch coverage claim.'

def coverage(path,manifest,words):
 rows={int(pc,16):(int(h),int(t),int(n))for pc,h,t,n in (line.split()for line in path.read_text().splitlines())};assert set(rows)==set(words)
 blocks=[];counts={};root_unaccounted=0
 for b in manifest['blocks']:
  out=[];a,z=int(b['start'],16),int(b['end_exclusive'],16)
  for pc in range(a,z,4):
   h,t,n=rows[pc];row=dict(pc=f'{pc:08x}',word=f'{words[pc]:08x}',hits=h,taken=t,not_taken=n)
   if not h:
    kind,why=exclusion(pc,b['name']);row.update(coverage_class=kind,reason=why);counts[kind]=counts.get(kind,0)+1
   elif t+n:
    assert t+n==h;row['branch_directions']='both'if t and n else'single';op=words[pc]>>26;rs=words[pc]>>21&31;rt=words[pc]>>16&31
    if not(t and n):row['single_direction_context']='Unconditional BEQ with identical registers'if op==4 and rs==rt else'Actual observed direction under declared call/fixture domain; alternate direction is not claimed exercised.'
   out.append(row)
  blocks.append(dict(name=b['name'],pcs=out))
 return dict(frozen_words=len(words),executed_unique_words=sum(h>0 for h,t,n in rows.values()),dynamic_instructions=sum(h for h,t,n in rows.values()),uncovered_classes=counts,complete_root_unaccounted_words=root_unaccounted,unknown_executed_words=0,blocks=blocks)

def main():
 p=argparse.ArgumentParser();p.add_argument('--output-dir',type=Path);p.add_argument('--build-dir',type=Path);p.add_argument('--skip-sanitized',action='store_true');p.add_argument('--skip-mutations',action='store_true');args=p.parse_args();manifest,words=verify()
 with tempfile.TemporaryDirectory(prefix='visual49r-')as temporary:
  build=args.build_dir.resolve()if args.build_dir else Path(temporary)/'build';build.mkdir(parents=True,exist_ok=True)
  out=args.output_dir.resolve()if args.output_dir else Path(temporary)/'evidence';out.mkdir(parents=True,exist_ok=True)
  def save(name,value):(out/name).write_text(json.dumps(value,indent=2)+'\n')
  def run(command,name,env=None,expect=True):
   result=subprocess.run([str(x)for x in command],cwd=ROOT,capture_output=True,text=True,env=env);(out/(name+'.stdout')).write_text(result.stdout);(out/(name+'.stderr')).write_text(result.stderr)
   if expect:assert result.returncode==0,f'{name}: {result.stdout}\n{result.stderr}'
   return result
  cxx=os.environ.get('CXX','c++');compiler=run([cxx,'--version'],'compiler').stdout.splitlines()[0]
  def compile_set(sanitized=False,traced=True):
   tag='san'if sanitized else'main'if traced else'plain';selected_flags=FLAGS if traced else [x for x in FLAGS if not x.startswith('-DRR_VISUAL49R_ENABLE_STAGE_TRACE=')];forced_include=['-include',SOURCE/'stage_trace.h']if traced else[];flags=selected_flags+(['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie']if sanitized else['-O2','-g']);objs=[]
   for file in PRODUCTION:
    stem=Path(file).stem;obj=build/(stem+'-'+tag+'.o')
    run([cxx,'-std=c++17',*flags,*forced_include,'-c',ROOT/file,'-o',obj],'build-'+stem+'-'+tag)
    objs.append(obj)
   exe=build/('vehicle_visual49r-'+tag);run([cxx,'-std=c++20',*flags,*( ['-no-pie']if sanitized else[]),*forced_include,ROOT/'tests/native/vehicle_visual49r.cpp',*objs,'-o',exe],'link-'+tag);return exe,objs
  exe,objs=compile_set();env=dict(os.environ,RR_VISUAL49R_COVERAGE=str(out/'coverage.tsv'))
  normal=json.loads(run([exe,ROOT],'normal',env).stdout);assert normal['all_match']and normal['native_allocations']==0;save('normal.json',normal);save('coverage-accounting.json',coverage(out/'coverage.tsv',manifest,words))
  coverage_hash=sha((out/'coverage.tsv').read_bytes());clean=dict(os.environ);clean.pop('RR_VISUAL49R_COVERAGE',None)
  readonly=json.loads(run([exe,ROOT],'default-read-only',clean).stdout);assert readonly==normal;assert sha((out/'coverage.tsv').read_bytes())==coverage_hash
  save('read-only-default.json',dict(default_run_passed=True,coverage_unchanged=True,argv1='Optional repository root',evidence_output='Only explicit--output-dir or RR_VISUAL49R_COVERAGE; temporary files otherwise'))
  plain_exe,_=compile_set(traced=False)
  plain=json.loads(run([plain_exe,ROOT],'plain-production',clean).stdout)
  assert {k:v for k,v in plain.items()if k!='checks'}=={k:v for k,v in normal.items()if k!='checks'}
  assert normal['checks']-plain['checks']==11*normal['native_calls']
  save('plain-production.json',dict(result=plain,stage_trace_enabled=False,numeric_state_and_original_instruction_checks_unchanged=True,omitted_assertions_per_call=11))
  mutations=[]
  if not args.skip_mutations:
   for name,pc,w in MUTATIONS:
    result=run([exe,ROOT,f'{pc:x}',f'{w:x}'],'mutation-'+name,clean,False);assert result.returncode!=0 and'mismatch'in result.stderr,(name,result.stderr)
    mutations.append(dict(name=name,type='original_word_substitution',pc=f'{pc:08x}',replacement=f'{w:08x}',actual_mismatch=result.stderr.strip()))
   native=ROOT/'src/game/visual_response49r.cpp';original=native.read_text();index=PRODUCTION.index('src/game/visual_response49r.cpp')
   for name,before,after in NATIVE_MUTATIONS:
    assert original.count(before)==1,(name,original.count(before));file=build/(name+'.cpp');file.write_text(original.replace(before,after));obj=build/(name+'.o')
    run([cxx,'-std=c++17','-O2',*FLAGS,'-I',ROOT/'src/game','-include',SOURCE/'stage_trace.h','-c',file,'-o',obj],'mutation-build-'+name)
    mutant_objs=list(objs);mutant_objs[index]=obj;binary=build/name
    run([cxx,'-std=c++20','-O2',*FLAGS,'-include',SOURCE/'stage_trace.h',ROOT/'tests/native/vehicle_visual49r.cpp',*mutant_objs,'-o',binary],'mutation-link-'+name)
    result=run([binary,ROOT],'mutation-'+name,clean,False);assert result.returncode!=0 and'mismatch'in result.stderr,(name,result.stderr)
    mutations.append(dict(name=name,type='isolated_native_source_mutation',before=before,after=after,actual_mismatch=result.stderr.strip()))
  save('mutation-controls.json',dict(mutations=mutations,actual_mismatches=len(mutations),setup_failures_counted=0,working_tree_production_mutated=False))
  sanitized=None
  if not args.skip_sanitized:
   binary,_=compile_set(True);envsan=dict(clean,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
   sanitized=json.loads(run([binary,ROOT],'asan-ubsan',envsan).stdout);assert sanitized==normal;save('sanitized.json',sanitized)
  artifacts=[ROOT/'tests/native/vehicle_visual49r.cpp',Path(__file__),*[ROOT/f for f in PRODUCTION],*[x for x in SOURCE.iterdir()if x.is_file()]]
  validation=dict(status='passed',scope='Complete admitted21C120 through220894 return and220898 delay, every reached helper executed',compiler=compiler,production_language='C++17',test_language='C++20',flags=FLAGS,normal=normal,plain_production=plain,sanitized=sanitized,leak_sanitizer='disabled, not claimed',mutations=len(mutations),stage_trace='Ten logical source-PC stages per complete call; not all individual native scalar stores',native_no_allocation=True,source_elf_sha256=manifest['elf_sha256'],files={str(x.relative_to(ROOT)):sha(x.read_bytes())for x in artifacts})
  save('validation.json',validation);print(json.dumps(dict(status='passed',cases=normal['cases'],checks=normal['checks'],sanitized=sanitized is not None,mutations=len(mutations),output=str(out)if args.output_dir else'temporary only')))
if __name__=='__main__':main()
