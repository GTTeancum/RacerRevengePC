"""Focused original-data, malformed-input, and transform regressions for 48E."""
from pathlib import Path
import copy
import json
import math
import struct
import sys
sys.dont_write_bytecode=True
from decode_world48e import (ROOT, decode_parameter_blocks, encode_parameter_blocks,
    decode_world, placement_matrix, transform, FormatError, sha, values)
from decode_scene48c import load_container

checks=0
def check(ok,label):
    global checks
    checks+=1
    if not ok:raise AssertionError(label)

def rejected(data,label,fn=decode_parameter_blocks):
    try:fn(data)
    except FormatError:return check(True,label)
    raise AssertionError('Accepted '+label)

def put32(data,at,value):
    out=bytearray(data);struct.pack_into('<I',out,at,value);return bytes(out)

def main():
    members,container=load_container(ROOT/'work/native/project/disc/TRACKS/TA.RES')
    for member,expected in [(r'tracks\ta.scb',378),(r'tracks\ta.tob',44)]:
        data=members[member][0];parsed=decode_parameter_blocks(data)
        check(len(parsed['sections'])==expected,member+' original section count')
        check(encode_parameter_blocks(parsed)==data,member+' byte-exact reversible parse')
        # Structural boundaries and sampled interior truncations; no arbitrary
        # byte prefix is treated as a complete parameter document.
        prefixes={0,6,7,10,11,14,len(data)-1,parsed['header_table_offset']}
        prefixes.update(range(0,len(data),max(1,len(data)//41)))
        prefixes.update(s['offset'] for s in parsed['sections'].values())
        for length in sorted(prefixes):rejected(data[:length],member+' strict prefix')
        rejected(put32(data,7,2),'version')
        rejected(put32(data,11,0xffffffff),'section cap')
        rejected(put32(data,15,0xffffffff),'name extent')
        table=parsed['header_table_offset']
        rejected(put32(data,table,0xffffffff),'block pointer extent')
        rejected(put32(data,table+4,0xffffffff),'block size extent')
        start=next(iter(parsed['sections'].values()))['offset']
        rejected(put32(data,start,0xffffffff),'label count cap')
        rejected(put32(data,start+4+35,0),'label value aliases label table')
        rejected(put32(data,start+4+39,0xffffffff),'label value extent')
        changed=bytearray(data);changed[start+4+34]=1;rejected(bytes(changed),'nonzero fixed label suffix')
        rejected(data+b'\0','trailing byte')
    data=members[r'tracks\ta.scb'][0];world=decode_world(data)
    check(len(world['instances'])==318,'318 authored scene entries')
    check([z['models'] for z in world['zones']]==[148,21,6,6,29,3,30,14,61],'all original zone counts')
    check(len(world['portals'])==10,'10 original portals')
    for instance in world['instances']:
        row=instance['original_parameters'];mat=instance['matrix']
        check(row['slt'].lower().replace('.slt','.psg')==instance['geometry_member'].split('\\')[-1],'asset reference mapping')
        check(transform((0.,0.,0.),mat)==tuple(instance['position']),'original translation exact')
        axes=[mat[i:i+3] for i in (0,4,8)]
        check(all(abs(sum(v*v for v in axis)-1)<2e-14 for axis in axes),'normalized original axes')
        check(all(abs(sum(axes[i][k]*axes[j][k] for k in range(3)))<2e-14 for i,j in [(0,1),(0,2),(1,2)]),'orthogonal original axes')
    mat=placement_matrix((10.,20.,30.),(0.,0.,1.),(0.,1.,0.))
    check(transform((1.,2.,3.),mat)==(11.,22.,33.),'identity placement analytical')
    mat=placement_matrix((10.,20.,30.),(1.,0.,0.),(0.,1.,0.))
    check(transform((1.,2.,3.),mat)==(13.,22.,29.),'row-vector orientation and handedness analytical')
    try:placement_matrix((0.,0.,0.),(0.,1.,0.),(0.,1.,0.))
    except FormatError:check(True,'parallel vectors rejected')
    else:raise AssertionError('parallel vectors accepted')
    parsed=decode_parameter_blocks(data)
    # Semantic errors encoded through the reversible independent container.
    def alter(section,key,newvalue):
        changed=copy.deepcopy(parsed)
        field=next(f for f in changed['sections'][section]['fields'] if f['name']==key)
        field['value']=newvalue
        rejected(encode_parameter_blocks(changed),section+'/'+key,decode_world)
    for args in [('sceneinfo','numzones','10'),('zone1','numberofstaticmodels','147'),
                 ('zone1-model1','slt','..\\ta_bigrocka.slt'),('zone1-model1','position','nan,0,0'),
                 ('zone1-model1','position','1e999,0,0'),('zone1-model1','lookvector','0,1,0'),
                 ('zone1-model1','uselighting','maybe'),('portal1','connect1','10')]:alter(*args)
    # Validate exported evidence without accepting a render as topology proof.
    report=json.loads((ROOT/'evidence/worldScene48E/world.json').read_text())
    expected=dict(visual_instances=284,collision_only_instances=34,unique_visual_members=228,
        root_matrices_replaced=42,world_quantized_instances=31,local_instances=253,vertices=110196,triangles=69838)
    for key,value in expected.items():check(report['summary'][key]==value,'accepted original '+key)
    binary=(ROOT/'evidence/worldScene48E/ta-static-placed-neutral.rrmesh').read_bytes()
    check(sha(binary)==report['summary']['mesh_sha256'],'export hash')
    nv,ni=struct.unpack_from('<II',binary,8)
    check(len(binary)==16+24*nv+4*ni and nv==110196 and ni==69838*3,'export exact extent/count')
    offset=16+nv*24
    for span in report['surface_spans']:
        indices=struct.unpack_from('<'+str(span['index_count'])+'I',binary,offset+span['index_start']*4)
        check(all(span['vertex_start']<=i<span['vertex_start']+span['vertex_count'] for i in indices),'instance/surface index isolation')
    check(len(report['unhandled_animations'])==2,'moving animations explicitly separated')
    result=dict(passed=True,checks=checks,container_sha256=container['sha256'],mesh_sha256=sha(binary),
        scope='Reversible original SCB/TOB, exact counts and references, bounded malformed input, analytical placement and original output invariants; no VU/gameplay equivalence claim')
    (ROOT/'evidence/worldScene48E/tests.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result))

if __name__=='__main__':main()
