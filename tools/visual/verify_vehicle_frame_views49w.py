#!/usr/bin/env python3
"""Verify both render passes, profile isolation and lossless 49W presentation."""
import copy
import hashlib
import json
from pathlib import Path
from PIL import Image
import numpy as np
import render_vehicle_frame49w as renderer
ROOT = Path(__file__).resolve().parents[2]
E = ROOT / 'evidence/frameBridge49W'
FRAMES = (0, 31, 95, 143)
COUNTS = {'chariot':2884, 'left_engine':1411, 'right_engine':1460}
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def need(value, why):
    if not value: raise ValueError(why)
def pixels(p):
    with Image.open(p) as image: return np.asarray(image.convert('RGBA'))
def difference(a, b):
    yy, xx = np.where(np.any(a != b, axis=2))
    return {'different_framebuffer_pixels':int(len(xx)),
            'difference_bounds_xyxy':[int(xx.min()),int(yy.min()),int(xx.max()+1),int(yy.max()+1)] if len(xx) else None}
def main():
    rows = [('support', f, 'full-motion') for f in FRAMES]
    rows += [('support',31,'physical-copy-only'), ('support',31,'frozen-roots')]
    rows += [('no-support', f, 'full-motion') for f in FRAMES]
    files, views, reports = {}, [], {}
    renderer_sha = sha(Path(renderer.__file__))
    for scenario, frame, profile in rows:
        name = f'{scenario}-frame{frame:03d}-{profile}'
        pair = []
        for mode in ('pass1', 'pass2'):
            directory = E/'views'/mode
            report = json.loads((directory/(name+'.json')).read_text())
            plan = report['plan']
            need(report['schema']=='advancing49w.visual-report.v1' and report['status']=='rendered_diagnostic_only', 'Render did not pass')
            need(plan==json.loads((directory/(name+'.plan.json')).read_text()), 'Planned/executed data differ')
            without_digest = {k:v for k,v in plan.items() if k!='plan_sha256'}
            need(hashlib.sha256(renderer.base.json_bytes(without_digest)).hexdigest()==plan['plan_sha256'], 'Plan digest differs')
            need(plan['renderer49W_sha256']==renderer_sha, 'Renderer source changed after capture')
            need(plan['scenario']==('synthetic-support' if scenario=='support' else 'no-support'), 'Scenario differs')
            native = plan['native_export']; row = native['selected']
            need(sha(Path(native['path']))==native['sha256'], 'Independent report changed')
            need((row['frame'],row['label'])==(frame,profile), 'Wrong frame/profile')
            need(plan['total_draw_calls']==report['actual_draw_calls']==360, 'Draw total changed')
            need(plan['submitted_triangles']==report['actual_submitted_triangles']==73409, 'Triangle total changed')
            need(plan['budget']['gl_geometry_and_texture_payload_bytes']==7604084, 'GPU payload changed')
            need(report['gl_error_checks_passed'] and report['framebuffer_unchanged_by_caption'], 'GL/caption check failed')
            need(plan['camera']['sha256']==renderer.old.AUTHORED_HASHES['camera'], 'Preserved49H camera changed')
            geometry = plan['uploaded_vehicle_geometry']
            need(geometry['uv_color_indices_unchanged'], 'Mesh attributes changed')
            expected = None if profile=='full-motion' else dict(COUNTS) if profile=='frozen-roots' else dict(chariot=2884,left_engine=0,right_engine=0)
            need(geometry['control_changed_component_vertices_vs_full']==expected, 'Component isolation differs')
            for image in report['images']:
                p = Path(image['path'])
                need(sha(p)==image['sha256'], 'Image bytes changed')
                decoded = pixels(p)
                need(hashlib.sha256(decoded.tobytes()).hexdigest()==image['rgba8_sha256'], 'Image pixels changed')
                files[p.relative_to(ROOT).as_posix()] = sha(p)
            raw = pixels(directory/(name+'.raw.png')); caption = pixels(directory/(name+'.png'))
            need(raw.shape==(480,640,4) and caption.shape==(568,640,4), 'Capture size differs')
            need(np.array_equal(raw,caption[:480]), 'Caption altered original framebuffer pixels')
            need(hashlib.sha256(raw.tobytes()).hexdigest()==report['pixel_rgba8_sha256'], 'Framebuffer digest differs')
            for suffix in ('.json','.plan.json','.plan.stdout','.plan.stderr','.render.stdout','.render.stderr'):
                p = directory/(name+suffix); files[p.relative_to(ROOT).as_posix()] = sha(p)
            need(not (directory/(name+'.plan.stderr')).read_bytes() and not (directory/(name+'.render.stderr')).read_bytes(), 'Capture stderr not empty')
            pair.append(report)
        need(pair[0]['pixel_rgba8_sha256']==pair[1]['pixel_rgba8_sha256'], 'Independent framebuffer pixels differ')
        need(pair[0]['plan']['plan_sha256']==pair[1]['plan']['plan_sha256'], 'Independent plans differ')
        for suffix in ('.png','.raw.png'):
            need(np.array_equal(pixels(E/'views/pass1'/(name+suffix)),pixels(E/'views/pass2'/(name+suffix))), 'Independent PNG pixels differ')
        reports[(scenario,frame,profile)] = pair[0]
        views.append({'scenario':scenario, 'frame':frame, 'profile':profile,
                      'plan_sha256':pair[0]['plan']['plan_sha256'], 'pixel_rgba8_sha256':pair[0]['pixel_rgba8_sha256'],
                      'draw_calls':360, 'triangles':73409, 'gpu_payload_bytes':7604084,
                      'two_pass_raw_and_caption_pixels_identical':True,
                      'maximum_resident_set_bytes':max(r['runtime']['maximum_resident_set_bytes'] for r in pair)})
    def raw(s, f, p='full-motion'): return pixels(E/'views/pass1'/f'{s}-frame{f:03d}-{p}.raw.png')
    full = raw('support',31); controls = {}
    for profile in ('physical-copy-only','frozen-roots'):
        delta = difference(full, raw('support',31,profile))
        need(delta['different_framebuffer_pixels']>0, 'Display control lacks a visible effect')
        delta['changed_component_vertices'] = reports[('support',31,profile)]['plan']['uploaded_vehicle_geometry']['control_changed_component_vertices_vs_full']
        controls[profile] = delta
    comparisons = {str(f):difference(raw('support',f),raw('no-support',f)) for f in FRAMES}
    need(comparisons['0']['different_framebuffer_pixels']==0, 'Initial scenario frame differs')
    need(all(comparisons[str(f)]['different_framebuffer_pixels']>0 for f in (31,95,143)), 'Later scenarios do not visibly differ')
    need(all(np.array_equal(raw('no-support',31),raw('no-support',f)) for f in (95,143)), 'Expected depth-hidden no-support pixels differ')
    progression = {f'{a}_to_{b}':difference(raw('support',a),raw('support',b)) for a,b in zip(FRAMES,FRAMES[1:])}
    need(all(d['different_framebuffer_pixels']>0 for d in progression.values()), 'Supported sampled frames must all differ')
    # Read-only adversarial consumer checks preserve the capture sources.
    self_test = renderer.self_test(np)
    report_path = E/'connected/export-report.json'
    native = json.loads((E/'connected/export-report.native.json').read_text())
    native['frames'] = json.loads(report_path.read_text())['frames']
    renderer.report_rows(native)
    rejected = []
    for mutation in ('duplicate','nonfinite-time','wrong-pose','wrong-vertices','physical-engine-change','frozen-engine-unchanged','unknown-scenario','missing-row'):
        bad = copy.deepcopy(native)
        if mutation=='duplicate': bad['frames'][1] = copy.deepcopy(bad['frames'][0])
        elif mutation=='nonfinite-time': bad['frames'][0]['elapsed_seconds']=float('nan')
        elif mutation=='wrong-pose': bad['frames'][0]['pose_source']='postphysics-copy'
        elif mutation=='wrong-vertices': bad['frames'][0]['vertices']=5756
        elif mutation=='physical-engine-change': next(r for r in bad['frames'] if r['label']=='physical-copy-only')['changed_vertices_vs_full']['left_engine']=1
        elif mutation=='frozen-engine-unchanged': next(r for r in bad['frames'] if r['label']=='frozen-roots')['changed_vertices_vs_full']['left_engine']=0
        elif mutation=='unknown-scenario': bad['scenario']='authentic-racing'
        else: bad['frames'].pop()
        try: renderer.report_rows(bad)
        except (ValueError,KeyError,TypeError): rejected.append(mutation)
        else: raise ValueError('Adversarial report was accepted: '+mutation)
    for path in ('../export-report.json','/tmp/not-allowed','bad\\path'):
        try: renderer.relative_file(E,path)
        except (ValueError,OSError): rejected.append('path:'+path)
        else: raise ValueError('Unbounded input path accepted')
    presentation_path = E/'presentation-final/presentation.json'
    presentation = json.loads(presentation_path.read_text())
    need(presentation['status']=='passed' and presentation['draws_performed']==0 and not presentation['camera_changed'] and not presentation['poses_changed'], 'Presentation altered diagnostic state')
    for path, expected in presentation['source_pngs'].items(): need(sha(ROOT/path)==expected, 'Presentation source changed')
    for entry in presentation['images']:
        p=ROOT/entry['path'];need(sha(p)==entry['sha256'], 'Presentation image changed')
        need(hashlib.sha256(pixels(p).tobytes()).hexdigest()==entry['rgba8_sha256'], 'Presentation image pixels changed')
    animation = presentation['animation'];need(sha(ROOT/animation['path'])==animation['sha256'], 'Animation changed')
    with Image.open(ROOT/animation['path']) as decoded:
        need(decoded.n_frames==4, 'Loop frame count differs')
        for i, frame in enumerate(FRAMES):
            decoded.seek(i); data=np.asarray(decoded.convert('RGBA'))
            need(hashlib.sha256(data.tobytes()).hexdigest()==animation['decoded_frame_rgba8_sha256'][i], 'Decoded loop hash differs')
            for j, scenario in enumerate(('support','no-support')):
                need(np.array_equal(data[64:632,j*640:(j+1)*640],pixels(E/'views/pass1'/f'{scenario}-frame{frame:03d}-full-motion.png')), 'Loop tile differs from native capture')
    result = {'schema':'advancing49w.visual-verification.v1', 'passed':True, 'rendered_views':10,
              'independent_passes':2, 'total_actual_draws':7200, 'all_raw_and_caption_pixels_identical':True,
              'views':views, 'controls':controls, 'scenario_comparisons':comparisons, 'support_progression':progression,
              'no_support_late_framebuffers_identical':True,
              'visibility_limit':'No-support meshes retain advancing positions but are not visible in preserved49H depth-tested TA view from frame31; this explicitly demonstrates supplied-no-hit fallthrough. Supported late frames are distant.',
              'bounded_consumer_checks':{'mesh_self_test':self_test,'rejected_invalid_reports_and_paths':rejected,'all_no_draw':True},
              'image_and_capture_files':files, 'renderer_sha256':renderer_sha,
              'composer_sha256':sha(ROOT/'tools/visual/compose_vehicle_frame_views49w.py'),
              'verifier_sha256':sha(Path(__file__)), 'presentation_sha256':sha(presentation_path),
              'limits':presentation['limits']}
    target=E/'views/visual-verification.json'
    with target.open('x') as handle: handle.write(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'passed':True,'rendered_views':10,'independent_passes':2,'controls':controls,
                      'support_progression':progression,'scenario_comparisons':comparisons,
                      'maximum_resident_set_bytes':max(r['maximum_resident_set_bytes'] for r in views),
                      'verification':str(target.relative_to(ROOT))}))
if __name__=='__main__': main()
