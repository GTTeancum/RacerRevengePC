#!/usr/bin/env python3
"""Reconstructed native mutation gate; disposable builds with durable logs/hashes."""
from pathlib import Path
import argparse,hashlib,json,os,shutil,subprocess,time,sys
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser();p.add_argument('--baseline',type=Path,required=True);p.add_argument('--output-dir',type=Path,required=True);a=p.parse_args();base=a.baseline.resolve();out=a.output_dir.resolve();out.mkdir(parents=True,exist_ok=False)
 flags=['g++','-std=c++17','-O2','-fno-fast-math','-frounding-math','-ffp-contract=off','-I'+str(ROOT/'src/game'),'-I'+str(base/'src/game'),'-I'+str(base/'src/assets'),'-I'+str(base/'third_party/native-json')]
 logs=[];retired=[]
 def call(cmd,name):
  assert shutil.disk_usage(out).free>1024**3,'disk guard'
  t=time.monotonic();r=subprocess.run(cmd,cwd=ROOT,capture_output=True,text=True,timeout=300);(out/(name+'.stdout')).write_text(r.stdout);(out/(name+'.stderr')).write_text(r.stderr);logs.append({'name':name,'command':list(map(str,cmd)),'returncode':r.returncode,'seconds':time.monotonic()-t});return r
 def retire(path):
  retired.append({'file':path.name,'bytes':path.stat().st_size,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()});path.unlink()
 common=[]
 sources=[ROOT/'tests/native/static_type2_producer49x.cpp',ROOT/'src/game/type2_resource49x.cpp',ROOT/'src/game/type2_triangle_axes49x.cpp']+[base/'src/game'/n for n in ['collision_query48h.cpp','spring_callback48n.cpp','spring_records48n.cpp']]
 for i,s in enumerate(sources):
  obj=out/(str(i)+'.o');r=call(flags+['-c',str(s),'-o',str(obj)],'build-'+str(i));assert r.returncode==0,'base build';common.append(str(obj))
 source=(ROOT/'src/game/static_type2_producer49x.cpp').read_text()
 changes={
  'per_pair_non_strict':('hit.fraction<w.metadata[pair].fraction','hit.fraction<=w.metadata[pair].fraction'),
  'global_strict':('hit.fraction<=result.selected.fraction','hit.fraction<result.selected.fraction'),
  'final_only_callback':('if(query.callbackEnabled){','if(query.callbackEnabled&&candidateIndex+1==count){'),
  'query_children_reversed':('w.stack[used++]={n.children[1],item.targetTag};w.stack[used++]={n.children[0],item.targetTag};','w.stack[used++]={n.children[0],item.targetTag};w.stack[used++]={n.children[1],item.targetTag};'),
  'target_children_reversed':('w.stack[used++]={item.queryTag,tag16(n.children[1])};w.stack[used++]={item.queryTag,tag16(n.children[0])};','w.stack[used++]={item.queryTag,tag16(n.children[0])};w.stack[used++]={item.queryTag,tag16(n.children[1])};'),
  'volume_equality_query_first':('splitQuery=qnode.volume>tnode.volume;','splitQuery=qnode.volume>=tnode.volume;'),
  'one_ray_looping':('std::size_t used=1,visits=0;w.stack[0]={resource.root,0};','std::size_t used=resource.rayCount,visits=0;if(used>w.stackCapacity)return Status::InsufficientCapacity;for(std::size_t r=0;r<used;++r)w.stack[r]={static_cast<std::uint16_t>(leaf|(used-r-1)),0};'),
  'mode1_smoothing':('triangle.hasVertexNormals=false;','triangle.hasVertexNormals=!mesh.vertex_normals.empty();for(unsigned k=0;k<3&&triangle.hasVertexNormals;++k)triangle.vertexNormals[k]=vector(mesh.vertex_normals[face.vertices[k]]);'),
  'node_tangent_inclusive':('return !(mul(radius,radius)<=squared);','return !(mul(radius,radius)<squared);'),
  'clear_stale_pairs':('std::memcpy(w.pairs,initial.pairs,initial.pairSlots*sizeof(*w.pairs));','std::memset(w.pairs,0,initial.pairSlots*sizeof(*w.pairs));'),
  'invent_endpoint_w':('std::memcpy(&p.first,&ray.first,sizeof(p.first));std::memcpy(&p.second,&ray.second,sizeof(p.second));','std::memcpy(&p.first,&ray.first,sizeof(p.first));std::memcpy(&p.second,&ray.second,sizeof(p.second));p.first.v[3]=p.second.v[3]=1.f;'),
  'selected_correction_w2':('worldDirection(result.selected.correction,c.geometry.worldMatrix,3.f);','worldDirection(result.selected.correction,c.geometry.worldMatrix,2.f);'),
  'callback_per_pair_last_only':('w.pairs,result.pairCount,c.filter.identity','w.pairs+result.pairCount-1,1,c.filter.identity')}
 quiet='auto quiet49x=[](float& value){std::uint32_t word;std::memcpy(&word,&value,4);if((word&0x7f800000u)==0x7f800000u&&(word&0x003fffffu)&&!(word&0x00400000u)){word|=0x00400000u;std::memcpy(&value,&word,4);}};'
 endpoint='std::memcpy(&p.first,&ray.first,sizeof(p.first));std::memcpy(&p.second,&ray.second,sizeof(p.second));'
 initial='std::memcpy(&result.selected,&initial.selected,sizeof(result.selected));'
 changes['quiet_signaling_endpoint']=(endpoint,endpoint+quiet+'quiet49x(p.first.v[3]);quiet49x(p.second.v[3]);')
 changes['quiet_signaling_stale_selected']=(initial,initial+quiet+'for(unsigned i=0;i<4;++i){quiet49x(result.selected.normal.v[i]);quiet49x(result.selected.surfacePoint.v[i]);quiet49x(result.selected.correction.v[i]);}')
 fixture=ROOT/'evidence/staticType2Producer49X/original-word/native-fixtures.json'
 opaque=ROOT/'evidence/staticType2Producer49X/original-word/opaque-snan-fixtures.json' 
 report={'fresh_after_recovery':True,'source_sha256':hashlib.sha256(source.encode()).hexdigest(),'fixture_sha256':hashlib.sha256(fixture.read_bytes()).hexdigest(),'test_sha256':hashlib.sha256(sources[0].read_bytes()).hexdigest(),'opaque_fixture_sha256':hashlib.sha256(opaque.read_bytes()).hexdigest(),'controls':{},'logs':logs,'retired_rebuildable_products':retired}
 for name,(before,after) in changes.items():
  assert source.count(before)==1,name;mutant=out/(name+'.cpp');mutant.write_text(source.replace(before,after));binary=out/name
  assert call(flags+[str(mutant)]+common+['-o',str(binary)],'build-'+name).returncode==0,'compile '+name
  prior=None
  if name.startswith('quiet_signaling_'):
   prior=call([str(binary),str(fixture)],'quiet-only-corpus-'+name);assert prior.returncode==0,'control should distinguish new SNaN coverage'
  r=call([str(binary),str(opaque if prior is not None else fixture)],'run-'+name);report['controls'][name]={'detected':r.returncode==1,'returncode':r.returncode,'reason':r.stderr.strip(),'sha256':hashlib.sha256(mutant.read_bytes()).hexdigest(),'old_quiet_only_corpus_passed':prior.returncode==0 if prior is not None else None};retire(binary);(out/'report.json').write_text(json.dumps(report,indent=2))
 for path in common:retire(Path(path))
 report['status']='passed' if all(c['detected'] for c in report['controls'].values()) else 'failed';(out/'report.json').write_text(json.dumps(report,indent=2));print(json.dumps({'status':report['status'],'controls':len(report['controls'])}));assert report['status']=='passed'
if __name__=='__main__':main()
