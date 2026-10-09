"""Compare the same captured guest-VBlank window; never call it unique FPS."""
from pathlib import Path
import argparse
import bisect
import hashlib
import json
import re

parser=argparse.ArgumentParser()
parser.add_argument('--root',type=Path,default=Path(__file__).resolve().parents[1])
parser.add_argument('--from-vblank',type=int,default=6000)
parser.add_argument('--to-vblank',type=int,default=6900)
parser.add_argument('--baseline',type=Path,required=True)
parser.add_argument('--candidate',type=Path)
parser.add_argument('--expected-sha256',required=True)
parser.add_argument('--output',type=Path,required=True)
args=parser.parse_args()
assert args.from_vblank<args.to_vblank
def sha(p):
    with p.open('rb') as f: return hashlib.file_digest(f,'sha256').hexdigest()
def snapshot(mode):
    run=args.baseline if mode=='baseline' else args.candidate
    if run is None: return {'status':'not started'}
    run=run.resolve()
    assert sha(run/'racer_native_probe.exe')==args.expected_sha256.lower(), 'Frozen executable hash differs'
    points=[]
    for p in run.glob('frame-*.json'):
        match=re.fullmatch(r'frame-(\d+)-(\d+)\.json',p.name)
        if not match: continue
        try: data=json.loads(p.read_text())
        except json.JSONDecodeError: continue # A currently writing capture is not final evidence.
        points.append({'milliseconds':int(match[2]),'vsync':data['vsync'],'cycles':data['cycles'],
                       'frame':p.name,'has_frame':bool(data['has_frame'])})
    points.sort(key=lambda p:p['milliseconds'])
    assert all(a['vsync']<=b['vsync'] and a['cycles']<=b['cycles'] for a,b in zip(points,points[1:])), 'Nonmonotone guest state'
    result={'run':str(run),'status':'running','captures':len(points),'latest':points[-1] if points else None,
            'binary_sha256':sha(run/'racer_native_probe.exe'),'bridge_sha256':sha(run/'racer_pcsx2_bridge44r.dll'),
            'buttons_sha256':sha(run/'buttons.txt'),'cards_before':json.loads((run/'cards-before.json').read_text(encoding='utf-8-sig')),
            'launch':json.loads((run/'launch.json').read_text(encoding='utf-8-sig'))}
    validation=run/'fixed-stop-validation.json'
    if validation.exists(): result['fixed_stop']=json.loads(validation.read_text(encoding='utf-8-sig'))
    exit_file=run/'native.exit'
    if exit_file.exists(): result.update(status='terminal',exit=exit_file.read_text(encoding='utf-8-sig').strip())
    after=run/'cards-after.json'
    if after.exists(): result['cards_unchanged']=result['cards_before']==json.loads(after.read_text(encoding='utf-8-sig'))
    def boundary(v):
        at=bisect.bisect_left([p['vsync'] for p in points],v)
        if at==len(points): return None
        if points[at]['vsync']==v: return {'milliseconds':points[at]['milliseconds'],'bracket':[points[at],points[at]]}
        if at==0: return None
        a,b=points[at-1:at+1]
        t=a['milliseconds']+(v-a['vsync'])*(b['milliseconds']-a['milliseconds'])/(b['vsync']-a['vsync'])
        return {'milliseconds':t,'bracket':[a,b]}
    start,end=boundary(args.from_vblank),boundary(args.to_vblank)
    if start and end:
        delta=args.to_vblank-args.from_vblank
        elapsed=(end['milliseconds']-start['milliseconds'])/1000
        minimum=(end['bracket'][0]['milliseconds']-start['bracket'][1]['milliseconds'])/1000
        maximum=(end['bracket'][1]['milliseconds']-start['bracket'][0]['milliseconds'])/1000
        sample=[p for p in points if args.from_vblank<=p['vsync']<=args.to_vblank]
        hashes=[sha(run/Path(p['frame']).with_suffix('.ppm')) for p in sample if p['has_frame']]
        result['window']={'vblank_start':args.from_vblank,'vblank_end':args.to_vblank,
            'elapsed_host_seconds_interpolated':elapsed,'vblanks_per_host_second':delta/elapsed,
            'rate_bounds_from_capture_brackets':[delta/maximum,delta/minimum] if minimum>0 else None,
            'start':start,'end':end,'sampled_pictures':len(hashes),'unique_sampled_pictures':len(set(hashes)),
            'warning':'Pictures are sampled every ~10 host seconds; unique sampled pictures are not a frame-rate measurement.'}
    return result

report={'metric':'emulated capture VBlank events per host second, not distinct rendered FPS',
        'window':[args.from_vblank,args.to_vblank],'baseline':snapshot('baseline'),'candidate':snapshot('candidate')}
a,b=report['baseline'],report['candidate']
if b['status']!='not started':
    report['same_binary_bridge_input_cards']=all(a[k]==b[k] for k in ['binary_sha256','bridge_sha256','buttons_sha256','cards_before'])
    assert report['same_binary_bridge_input_cards'], 'Experiment inputs differ'
if 'window' in a and 'window' in b:
    report['throughput_ratio']=b['window']['vblanks_per_host_second']/a['window']['vblanks_per_host_second']
report['terminal_pair_verified']=all(r.get('status')=='terminal' and r.get('exit')=='0' and r.get('cards_unchanged') and (r.get('fixed_stop') or {}).get('verified') for r in [a,b])
report['game_behavior_equivalence_verified']=False
report['caveat']='No whole-race, real Xbox, unique-image FPS or guest-state equivalence acceptance from this timing report alone.'
args.output.parent.mkdir(parents=True,exist_ok=True)
with args.output.open('x',encoding='utf-8') as f: json.dump(report,f,indent=2)
print(json.dumps({k:report[k] for k in ['metric','window','terminal_pair_verified']},indent=2))
for mode in ['baseline','candidate']:
    r=report[mode]
    print(mode,r['status'],'latest_vblank=',(r.get('latest') or {}).get('vsync'),
          'rate=',(r.get('window') or {}).get('vblanks_per_host_second'))
