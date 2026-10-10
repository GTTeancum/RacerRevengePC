#!/usr/bin/env python3
"""Hash original source, freeze bounded palette words and verify source/native behavior.
No game/RAM/PC input or original lifecycle execution. Existing files stay frozen.
"""
import argparse,hashlib,json,os,struct,subprocess,sys,tempfile
from pathlib import Path
sys.dont_write_bytecode=True
from authored_parameters_recover49n import decode
ROOT=Path(__file__).resolve().parents[2]
SOURCE=ROOT/'evidence/modelPalette49S/source'
SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={'root_ready_gate':(0x2412e4,0x2412fc),'root_copy_and_palette':(0x2414f8,0x2415a4),'lazy_world_refresh':(0x2897b0,0x289940),'world_matrix_product':(0x289940,0x2899b4)}
ASSETS={'43-engrt1.psg':'7b1e5d4548e60ccb52cee5be243e663523fa9201ea858f425229697200b4e5bf','48-englt1.psg':'75effabdf530151f4698dd174983298c535f99cdce040904ec34e71b28891b3b','52-chariot.psg':'203232e291b5098ddfeb437d59748105f33a4432bad1a9359d96681fc5d03b59'}
SOURCE_MUTATIONS=[('palette_source_index_shift','241548','00031800'),('palette_copies_local','241564','7a4100c0'),('omit_final_matrix_row','241580','00000000'),('omit_empty_node_getter','24155c','00000000'),('reverse_root_ready_gate','2412ec','10400003'),('world_product_subtract','289964','4be5224f')]
def sha(b):return hashlib.sha256(b).hexdigest()
def generated():
 raw=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(raw)==SHA
 phoff=struct.unpack_from('<I',raw,28)[0];psz,pnum=struct.unpack_from('<HH',raw,42);segs=[struct.unpack_from('<8I',raw,phoff+i*psz) for i in range(pnum)]
 def take(a,z):
  for kind,off,va,_,n,_,_,_ in segs:
   if kind==1 and va<=a<z<=va+n:return raw[off+a-va:off+z-va],off+a-va
  raise AssertionError('range not file-backed')
 files={};total=0;header=['#pragma once','#include <cstdint>','#include <cstddef>','namespace palette49s_words {'];manifest={'elf_sha256':SHA,'blocks':[],'scope':'Selected root ready gate2412E4..2412FC and one nonnull source-index palette2414F8..2415A4; source2897B0/289940. Not whole241280, VU rendering or visibility/material recovery.'}
 for name,(a,z) in BLOCKS.items():
  b,off=take(a,z);ws=struct.unpack('<%dI'%(len(b)//4),b);files[name+'.bin']=b
  listing='\n'.join(f'{a+i*4:08x} {w:08x} {decode(a+i*4,w)}' for i,w in enumerate(ws))+'\n';files[name+'.txt']=listing.encode()
  manifest['blocks'].append({'name':name,'start':hex(a),'end_exclusive':hex(z),'elf_offset':hex(off),'words':len(ws),'index':total,'sha256':sha(b)})
  total+=len(ws);header.append(f'inline constexpr std::uint32_t {name}[]={{'+','.join(f'0x{w:08x}u' for w in ws)+'};')
 header+=['struct Block {const char* name;std::uint32_t start;const std::uint32_t* words;std::size_t count,index;};','inline constexpr Block blocks[]={']
 for row in manifest['blocks']:header.append('{"%s",%su,%s,sizeof(%s)/4,%d},'%(row['name'],row['start'],row['name'],row['name'],row['index']))
 header+=['};',f'inline constexpr std::size_t total_words={total};','}'];files['palette_words49s.h']=('\n'.join(header)+'\n').encode()
 old=ROOT/'evidence/sceneProjection49P/source/scene_oracle.h';s=old.read_text();manifest['oracle_parent']={'path':str(old.relative_to(ROOT)),'sha256':sha(old.read_bytes())}
 s=s.replace('#include "scene_words.h"','#include "palette_words49s.h"').replace('scene49p_oracle','palette49s_oracle').replace('scene49p_words','palette49s_words')
 s=s.replace('U pc=0,flags=0;','bool annulDelay=false; U pc=0,flags=0;')
 s=s.replace('std::vector<Store> stores; std::vector<U> trace;', 'std::vector<Store> stores; std::vector<U> trace,explicitGetters;')
 s=s.replace('pc=p;++steps;', 'pc=p;if(p==0x2897b0&&(g[31][0]==0x2412fc||g[31][0]==0x241564))explicitGetters.push_back(g[4][0]);++steps;')
 s=s.replace('else if(op==5)return branch(g[rs][0]!=g[rt][0]);','else if(op==5)return branch(g[rs][0]!=g[rt][0]);\n  else if(op==0x14){const bool take=g[rs][0]==g[rt][0];annulDelay=!take;return branch(take);}')
 oldrun=''' void run(U entry){
  U p=entry;callSteps=0;g[31][0]=stop;g[29][0]=stack;v[0]={0,0,0,1};
  while(p!=stop){U next=0,unused=0;if(execute(p,next)){if(execute(p+4,unused))throw std::runtime_error("branch in original delay slot");p=next;}else p+=4;}
 }'''
 newrun=''' void runUntil(U entry,U end){
  U p=entry;callSteps=0;annulDelay=false;g[31][0]=stop;g[29][0]=stack;v[0]={0,0,0,1};
  while(p!=end){U next=0,unused=0;if(execute(p,next)){const bool skip=annulDelay;annulDelay=false;if(!skip&&execute(p+4,unused))throw std::runtime_error("branch in original delay slot");p=next;}else p+=4;}
 }
 void run(U entry){runUntil(entry,stop);}'''
 assert oldrun in s;s=s.replace(oldrun,newrun);files['palette_oracle49s.h']=s.encode();manifest['word_count']=total
 manifest['oracle_adaptations']=['Uses only four selected original-word blocks and renamed namespace','Adds BEQL delay-slot annulment, required by original nonnull-node branch','Adds runUntil for explicit selected source-block continuations; no instruction replaced by a native formula','Inherits independent exact instruction decoder and synthetic-memory source from49P; every simulated byte is initialized to sentinel then explicit fixture inputs']
 files['source-manifest.json']=(json.dumps(manifest,indent=2)+'\n').encode();return files,manifest

def verify(freeze=False):
 for name,digest in ASSETS.items():assert sha((ROOT/'evidence/authoredGeometry49O/asset-reader/fixtures'/name).read_bytes())==digest,'original PSG changed: '+name
 files,m=generated()
 for name,data in files.items():
  p=SOURCE/name
  if freeze and not p.exists():p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
  assert p.read_bytes()==data,'frozen source differs: '+name
 return m

def account_coverage(path):
 report=json.loads(path.read_text());assert report['ordered_trace'] is True
 exclusions={0x241558:'Null-source BEQL taken delay slot is outside the nonnull source-reference admission; on every admitted entry the delay slot is annulled.',0x28981c:'Unreachable duplicate setup after the unconditional source branch289814.',0x2898e4:'Unreachable duplicate setup after the unconditional source branch2898DC.',0x28993c:'Alignment word after the getter return and its delay slot.'}
 single={0x241554:'All admitted source references are nonnull; null skips output copy and is not implemented by this bounded projection.'}
 reached=both=branches=0
 for row in report['pcs']:
  pc,w=row['pc'],row['word'];op,rs,rt=w>>26,(w>>21)&31,(w>>16)&31
  if not row['hits']:assert pc in exclusions,hex(pc);row['unreached_reason']=exclusions[pc];continue
  reached+=1
  # Include BGTZ: original289840/289908 implement the two root-copy loops.
  # Every other opcode must have zero branch counters, so a new branch family
  # cannot silently disappear from accounting again.
  if op not in (4,5,7,0x14):
   assert not (row['taken'] or row['not_taken']),f'unclassified branch counters at {pc:#x}, opcode {op:#x}'
   continue
  branches+=1;assert row['taken']+row['not_taken']==row['hits']
  if row['taken'] and row['not_taken']:both+=1
  elif op==4 and rs==rt:row['single_direction_reason']='Original unconditional equal-register BEQ.'
  else:assert pc in single,hex(pc);row['single_direction_reason']=single[pc]
 report['accounting']={'source_words':len(report['pcs']),'reached_words':reached,'excluded_words':len(report['pcs'])-reached,'reached_branch_pcs':branches,'both_directions':both,'single_direction_with_reason':branches-both,'unaccounted_words_or_branches':0}
 path.write_text(json.dumps(report,indent=2)+'\n');return report

def execute(cmd,out,name,env=None,success=True):
 p=subprocess.run([str(c) for c in cmd],cwd=ROOT,env=env,text=True,capture_output=True)
 (out/(name+'.stdout')).write_text(p.stdout);(out/(name+'.stderr')).write_text(p.stderr)
 if success:assert p.returncode==0,name+': '+p.stdout+p.stderr
 return p

def run(args):
 m=verify();build=args.build_dir.resolve();out=(args.output_dir or build).resolve();build.mkdir(parents=True,exist_ok=True);out.mkdir(parents=True,exist_ok=True)
 flags=['-std=c++17','-O2','-g','-Wall','-Wextra','-Werror','-pedantic','-ffp-contract=off','-fno-fast-math']
 if args.sanitizers:flags+=['-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie']
 env=dict(os.environ);env['RR_PALETTE49S_COVERAGE']=str(out/'coverage.json')
 if args.sanitizers:env['ASAN_OPTIONS']='detect_leaks=0:halt_on_error=1';env['UBSAN_OPTIONS']='halt_on_error=1:print_stacktrace=1'
 test=ROOT/'tests/native/model_palette49s.cpp';scene=ROOT/'src/game/scene_graph49p.cpp';native=ROOT/'src/game/model_palette49s.cpp';exe=build/'model_palette49s'
 cmd=[os.environ.get('CXX','g++'),*flags,'-include',SOURCE/'trace49s.h',test,scene,native,'-o',exe];execute(cmd,out,'compile')
 p=execute([exe],out,'test',env);print(p.stdout.strip())
 coverage=account_coverage(out/'coverage.json');mutations=[];menv=dict(env);menv.pop('RR_PALETTE49S_COVERAGE')
 for name,pc,word in SOURCE_MUTATIONS:
  r=execute([exe,pc,word],out,'source-control-'+name,menv,False)
  assert r.returncode==1 and ('mismatch' in r.stderr or 'outside' in r.stderr or 'unsupported' in r.stderr),name+': '+r.stderr
  mutations.append({'name':name,'kind':'original-word substitution','pc':pc,'word':word,'detected':True,'diagnostic':r.stderr.strip()})
 s=native.read_text();controls=[
 ('use_palette_slot_as_source','input.sourceNodes[input.paletteIds[entry]]','input.sourceNodes[entry % input.sourceNodeCount]'),
 ('reverse_palette_order','input.sourceNodes[input.paletteIds[entry]]','input.sourceNodes[input.paletteIds[input.paletteCount - entry - 1]]'),
 ('omit_empty_source_node','if (!getWorld(staged, node))','if (input.paletteIds[entry] != 1u && !getWorld(staged, node))'),
 ('copy_local_instead_of_world','&staged.nodes[static_cast<std::uint32_t>(node)].world100','&staged.nodes[static_cast<std::uint32_t>(node)].localC0'),
 ('omit_graph_cache_commit','std::memcpy(graph.nodes, workspace.nodes, graph.count * sizeof(SceneNode49P));','/* selected mutation: no graph cache commit */'),
 ('ready_root_getter_added','(staged.nodes[root].sceneFlags190 & 1u) == 0 &&','true &&'),
 ('drop_fourth_row','input.paletteCount * sizeof(BodyMatrix48G));','input.paletteCount * sizeof(BodyMatrix48G) - 16);'),
 ]
 for name,before,after in controls:
  assert s.count(before)==1,name+' anchor';mut=build/(name+'.cpp');mut.write_text(s.replace(before,after));mx=build/name
  mc=[os.environ.get('CXX','g++'),*flags,'-I',ROOT/'src/game','-include',SOURCE/'trace49s.h',test,scene,mut,'-o',mx];execute(mc,out,'compile-'+name)
  r=execute([mx],out,'native-control-'+name,menv,False);assert r.returncode==1 and 'mismatch' in r.stderr,name+': '+r.stderr
  mutations.append({'name':name,'kind':'native semantic control','detected':True,'diagnostic':r.stderr.strip()})
 report={'status':'pass','mode':'ASan+UBSan; LeakSanitizer disabled' if args.sanitizers else 'normal','elf_sha256':SHA,'source_words':m['word_count'],'psg_inputs':ASSETS,'result':p.stdout.strip(),'coverage':coverage,'mutations':mutations,'command':[str(c) for c in cmd]}
 (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('All '+str(len(mutations))+' source/native semantic controls detected.')

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--freeze',action='store_true');p.add_argument('--check',action='store_true');p.add_argument('--build-dir',type=Path);p.add_argument('--output-dir',type=Path);p.add_argument('--sanitizers',action='store_true');a=p.parse_args()
 if a.freeze or a.check:print(json.dumps(verify(a.freeze)));return
 if not a.build_dir:
  with tempfile.TemporaryDirectory(prefix='palette49s-') as d:a.build_dir=Path(d);run(a)
 else:run(a)
if __name__=='__main__':main()
