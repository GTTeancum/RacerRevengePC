"""Bind complete saved meshes to retained native caches and source instances."""
import json,struct,hashlib
from pathlib import Path
from query_extract48h import ROOT
from query_oracle48h import bits,f32
OUT=ROOT/'evidence/query48I'

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    saved=json.loads((OUT/'meshes.json').read_text());worldpath=ROOT/'evidence/worldImport48F/ta-tracks_ta.reference.json'
    world=json.loads(worldpath.read_text());report=[];parameters=[]
    for i,name in enumerate(['17-ta_colcavetop','18-zone04_col_floor']):
        sourcepath=ROOT/('evidence/importCollision48G/'+name+'.reference.json');source=json.loads(sourcepath.read_text())
        mesh=saved[i];assert source['vertices']==mesh['vertices'] and source['vertex_normals']==mesh['normals']
        assert len(source['nodes'])==len(mesh['nodes']) and len(source['faces'])==len(mesh['faces'])
        for a,b in zip(source['nodes'],mesh['nodes']):
            assert all(a[k]==b[k] for k in ['volume','center','half','children'])
        for a,b in zip(source['faces'],mesh['faces']):
            packed=[((int(f32(f32(32767.*v)+.5))+32768)%65536)-32768 for v in a['normal']]
            assert a['d']==b['d'] and a['vertices']==b['vertices'] and packed==b['normal']
        wanted='ta_colcavetop.slt' if i==0 else 'zone04_col_floor.slt'
        instance=next(x for x in world['instances'] if x['original_parameters'].get('slt','').lower()==wanted)
        fields=instance['original_parameters'];values=[]
        for k in ['position','lookvector','upvector']:values += [f32(float(x)) for x in fields[k].split(',')]
        parameters.append(values)
        cache=ROOT/('evidence/nativePc48H-build-20261003-163334-944/x64-collision-fixtures/'+name+'.rrcol')
        destination=OUT/(name+'.rrcol');destination.write_bytes(cache.read_bytes());assert sha(destination)==sha(cache)
        report.append({'saved_shape':f"{mesh['source_shape']:08X}",'mesh_index':i,'native_cache':destination.relative_to(ROOT).as_posix(),'native_cache_sha256':sha(destination),'source_cache':cache.relative_to(ROOT).as_posix(),'source_col':('evidence/importCollision48G/'+name+'.col'),'source_col_sha256':sha(ROOT/('evidence/importCollision48G/'+name+'.col')),'all_saved_typed_records_match_source':True,'source_reference':sourcepath.relative_to(ROOT).as_posix(),'source_reference_sha256':sha(sourcepath),'instance_section':instance['section'],'original_parameters':fields,'source_block_offset':instance['source_block_offset'],'world_reference':worldpath.relative_to(ROOT).as_posix(),'world_reference_sha256':sha(worldpath),'native_placement_input_words':[f'{bits(v):08X}' for v in values],'saved_world_words':[f'{bits(v):08X}' for v in mesh['world']]})
    (OUT/'cache-binding.json').write_text(json.dumps(report,indent=2)+'\n')
    lines=['#pragma once','#include <cstdint>','static const std::uint32_t queryCachePlacement48I[2][9] = {']
    lines += ['{'+','.join(f'0x{bits(v):08x}u' for v in row)+'},' for row in parameters];lines.append('};')
    (ROOT/'tools/gameplay/query_binding48i.h').write_text('\n'.join(lines)+'\n')
    print(json.dumps({'bound_meshes':2,'full_saved_records_match':True,'instance_sections':[r['instance_section'] for r in report],'cache_sha256':[r['native_cache_sha256'] for r in report]}))

if __name__=='__main__':main()
