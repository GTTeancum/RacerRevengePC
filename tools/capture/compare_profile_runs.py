"""Compare logs-only performance runs from run_windows_takeover.py.

For each evidence folder: measured gameplay FPS (capture/track-fps-baseline.json)
and the last recorded per-step wall time of selected simulation subphases
(RacerRevenge.log). Subphases are nested scopes; never sum them.
Usage: python -B compare_profile_runs.py EVIDENCE_DIR [EVIDENCE_DIR ...]
"""
import json, pathlib, re, sys

PHASES = ('scheduled_total', 'racer_physics', 'racer_contacts', 'racer_children',
          'sample_race_field', 'race_presentation')

def summarize(folder):
    folder = pathlib.Path(folder)
    execution = json.loads((folder / 'execution.json').read_text(encoding='utf-8-sig'))
    fps = None
    for name in ('track-fps-baseline.json',):
        for path in (folder / 'capture').rglob(name):
            fps = json.loads(path.read_text(encoding='utf-8-sig')).get('fps')
    last = {}
    pattern = re.compile(r'^simulation_subphase (\S+) .*?calls=(\d+) wall_ns=(\d+)')
    for line in (folder / 'RacerRevenge.log').read_text(encoding='utf-8', errors='replace').splitlines():
        match = pattern.match(line)
        if match and match[1] in PHASES:
            last[match[1]] = (int(match[2]), int(match[3]))
    return {
        'run': folder.name, 'exit': execution.get('exitCode'), 'routeCompleted': execution.get('routeCompleted'),
        'noImagesWritten': execution.get('noImagesWritten'), 'executableSha256': execution.get('executableSha256'),
        'fps': fps,
        'msPerStep': {k: round(w / c / 1e6, 4) for k, (c, w) in last.items() if c},
        'steps': last.get('scheduled_total', (0, 0))[0],
    }

if __name__ == '__main__':
    rows = [summarize(p) for p in sys.argv[1:]]
    print(json.dumps(rows, indent=2))
