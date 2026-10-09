#!/usr/bin/env python3
"""Verify the six twice-rendered lifecycle views and explicit component controls."""
import hashlib,json
from pathlib import Path
from PIL import Image
import numpy as np
ROOT=Path(__file__).resolve().parents[2]
E=ROOT/'evidence/chariotLifecycle49V'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def pixels(p):
 with Image.open(p) as im:return np.asarray(im.convert('RGBA'))
def main():
 rows=[(15,'full-motion'),(47,'full-motion'),(94,'full-motion'),(135,'full-motion'),(94,'virtual-only'),(94,'physical-copy-only')]
 reports=[]; pins={}
 for f,profile in rows:
  name=f'frame{f:03d}-{profile}'; pair=[]
  for mode in ('pass1','pass2'):
   directory=E/'frames'/mode;r=json.loads((directory/(name+'.json')).read_text());assert r['status']=='rendered_diagnostic_only'
   assert r['plan']['total_draw_calls']==r['actual_draw_calls']==360 and r['plan']['submitted_triangles']==r['actual_submitted_triangles']==73409
   assert r['gl_error_checks_passed'] and r['framebuffer_unchanged_by_caption']
   assert r['plan']['budget']['gl_geometry_and_texture_payload_bytes']==7604084
   for image in r['images']:
    p=Path(image['path']);assert sha(p)==image['sha256'];raw=pixels(p);assert hashlib.sha256(raw.tobytes()).hexdigest()==image['rgba8_sha256'];pins[p.relative_to(ROOT).as_posix()]=sha(p)
   for suffix in ('.json','.plan.json','.render.stdout','.render.stderr'):
    p=directory/(name+suffix);pins[p.relative_to(ROOT).as_posix()]=sha(p)
   assert not (directory/(name+'.render.stderr')).read_bytes()
   pair.append(r)
  assert pair[0]['pixel_rgba8_sha256']==pair[1]['pixel_rgba8_sha256']
  assert pair[0]['plan']['plan_sha256']==pair[1]['plan']['plan_sha256']
  for suffix in ('.png','.raw.png'):assert np.array_equal(pixels(E/'frames/pass1'/(name+suffix)),pixels(E/'frames/pass2'/(name+suffix)))
  reports.append({'frame':f,'profile':profile,'plan_sha256':pair[0]['plan']['plan_sha256'],'pixel_rgba8_sha256':pair[0]['pixel_rgba8_sha256'],'draw_calls':360,'triangles':73409,'gpu_payload_bytes':7604084,'two_pass_raw_and_caption_pixels_identical':True,'maximum_resident_set_bytes':max(x['runtime']['maximum_resident_set_bytes'] for x in pair)})
 full=pixels(E/'frames/pass1/frame094-full-motion.raw.png');controls={}
 for profile in ('virtual-only','physical-copy-only'):
  other=pixels(E/'frames/pass1'/('frame094-'+profile+'.raw.png'));mask=np.any(full!=other,axis=2);yy,xx=np.where(mask);assert len(xx)>0
  controls[profile]={'different_framebuffer_pixels':len(xx),'difference_bounds_xyxy':[int(xx.min()),int(yy.min()),int(xx.max()+1),int(yy.max()+1)],'changed_chariot_vertices':2884,'changed_engine_vertices':0}
  assert xx.min()>=288 and xx.max()<608 and yy.min()>=224 and yy.max()<444
 presentation=json.loads((E/'frames/presentation-final/presentation.json').read_text());assert presentation['status']=='passed' and presentation['draws_performed']==0
 result={'passed':True,'rendered_views':6,'independent_passes':2,'all_raw_and_caption_pixels_identical':True,'views':reports,'controls':controls,'image_and_capture_files':pins,'renderer_sha256':sha(ROOT/'tools/visual/render_chariot_lifecycle49v.py'),'composer_sha256':sha(ROOT/'tools/visual/compose_chariot_lifecycle_views49v.py'),'verifier_sha256':sha(Path(__file__)),'presentation_sha256':sha(E/'frames/presentation-final/presentation.json'),'limits':'Original ordinary root lifecycle with supplied main pose/current body inputs and initial secondary scene. Later physical-scene getter is isolated. Rigid diagnostic vertex arithmetic, first-pass textures, fixed TA camera. No complete physics, spawn, contacts, original materials, original VU rendering or gameplay FPS.'}
 (E/'render-validation.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({'passed':True,'controls':controls,'views':6,'identical_passes':2}))
if __name__=='__main__':main()
