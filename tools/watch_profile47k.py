"""Save bounded cumulative profile snapshots during the fixed game run.

The sampling categories overlap; they are elapsed time, not CPU time. These are
observations near capture boundaries, not an exact guest-range profiler.
"""
from pathlib import Path
import argparse
import datetime
import json
import time

p=argparse.ArgumentParser()
p.add_argument('run',type=Path)
p.add_argument('output',type=Path)
a=p.parse_args()
a.output.mkdir(parents=True,exist_ok=True)
remaining=[6000,6900]
while remaining and not (a.run/'native.exit').exists():
    frames=sorted(a.run.glob('frame-*.json'),key=lambda p:int(p.stem.split('-')[1]),reverse=True)
    if frames:
        try:
            frame=json.loads(frames[0].read_text())
            profile=(a.run/'host-profile.csv.live').read_bytes()
            # A writer may be mid-snapshot; all phase rows and a newline are
            # required. Guest rows are retained as observed, without completeness claims.
            complete=profile.endswith(b'\n') and b'nested,dma-service-inclusive,' in profile
            for threshold in list(remaining):
                if complete and frame['vsync']>=threshold:
                    (a.output/f'near-{threshold}.csv').write_bytes(profile)
                    (a.output/f'near-{threshold}.json').write_text(json.dumps(dict(
                        observed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                        frame=frames[0].name,vsync=frame['vsync'],ee_cycles=frame['cycles'],
                        caveat='Cumulative profiler snapshot and periodic frame are not simultaneous'),indent=2))
                    remaining.remove(threshold)
                    print('Saved cumulative profile near',threshold,'at capture',frame['vsync'],flush=True)
        except (OSError,ValueError,KeyError):
            pass
    time.sleep(2)
print('Observer done; thresholds not observed:',remaining,flush=True)
