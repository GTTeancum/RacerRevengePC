"""Original GC ATM production metadata: actual indexed PST frames, no generated pixels."""
from pathlib import Path
import argparse,json,hashlib,sys,struct
sys.dont_write_bytecode=True
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--repo',type=Path,default=Path(__file__).resolve().parents[2])
parser.add_argument('--intake',type=Path,required=True)
parser.add_argument('--output-root',type=Path,required=True)
args=parser.parse_args()
repo=args.repo
sys.path.insert(0,str(repo/'tools/assets'))
from decode_source_textures50fc import parse_resource,decode_indexed
intake=args.intake/'native-import'
out=args.output_root
(out/'tests/native').mkdir(parents=True,exist_ok=True)
(out/'evidence').mkdir(parents=True,exist_ok=True)
index=json.loads((intake/'import.json').read_bytes())
members={r['source_member'].replace('\\','/').lower():r for r in index['members']}
raw=args.intake/'intake/original-members'
sha=lambda b:hashlib.sha256(b).hexdigest()
rows=[];animations=[];frame_rows=[];first=0
for name in ['arrowred','s_flaga','s_flagb','s_flagc']:
 original=(raw/'textures'/(name+'.atm')).read_bytes();frames=parse_resource(name+'.atm',original)
 material=members['textures/'+name+'.psm'];psm=(intake/material['source_file']).read_bytes()
 fields={f['name']:f['value'] for f in material['passes'][0]['fields']['authored']}
 initial=int(fields.get('animatedMapInitialFrame','0'));fps=int(fields['animatedMapFramesPerSecond'])
 record=dict(name=name,source_sha256=sha(original),source_bytes=len(original),material_sha256=sha(psm),material_bytes=len(psm),frame_count=len(frames),initial_frame=initial,fps=fps,first_texture_index=first,frames=[])
 for fi,f in enumerate(frames):
  if f['kind']!=1:raise ValueError('This native48E bank admits original indexed PST frames only')
  mip=f['mips'][-1];rgba=decode_indexed(mip['data'],f['palette']);rrtex=struct.pack('<8sII',b'RRTEX001',mip['width'],mip['height'])+rgba
  entry=dict(frame=fi,start=f['start'],bytes=f['end']-f['start'],pst_sha256=sha(original[f['start']:f['end']]),native_sha256=sha(rrtex),width=mip['width'],height=mip['height'],mip=mip['level'])
  record['frames'].append(entry)
  frame_rows.append('    {%du,%du,%du,%du,%du,"%s","%s"},' %(entry['start'],entry['bytes'],entry['width'],entry['height'],entry['mip'],entry['pst_sha256'],entry['native_sha256']))
 animations.append('    {"%s","%s","%s",%du,%du,%du,%du,%du,%d.f},' %(name,sha(original),sha(psm),len(original),len(psm),len(frames),first,initial,fps));first+=len(frames);rows.append(record)
(out/'tests/native/gc_atm_animation_index.inc').write_text('// Original GC ATM and PSM identities; first frame-bank indices preserve stored order.\n'+'\n'.join(animations)+'\n')
(out/'tests/native/gc_atm_frame_index.inc').write_text('// Embedded original indexed PST extents, hashes and native48E base hashes.\n'+'\n'.join(frame_rows)+'\n')
(out/'evidence/original-atm-frames.json').write_text(json.dumps(dict(archive_sha256='69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9',animations=rows,frames=first,rgba_bytes=sum(f['width']*f['height']*4 for r in rows for f in r['frames'])),indent=2)+'\n')
print(json.dumps({'frames':first,'rgba_bytes':sum(f['width']*f['height']*4 for r in rows for f in r['frames']),'initial_frames':[r['initial_frame'] for r in rows],'rates':[r['fps'] for r in rows]}))
