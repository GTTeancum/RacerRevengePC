"""Compare saved native runs stopped at the same guest VBlank boundary."""
from pathlib import Path
import argparse
import hashlib
import json
import re

root = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument('reference', type=Path)
p.add_argument('candidate', type=Path)
p.add_argument('--output', required=True, type=Path)
args = p.parse_args()

def load(run):
    run = run.resolve()
    log = (run / 'native.log').read_text(errors='replace')
    stops = re.findall(r'\[rr:fixed-stop47f\] target=(\d+) actual=(\d+) ee_cycles=(\d+)', log)
    state_prefixes = ('[rr:iop]', '[rr:iop-diagnostic] module ', '[rr:iop-diagnostic] thread ',
                      '[rr:iop-service]', '[rr:gs-status]', '[rr:terminal-dma32r]',
                      '[probe] ee_cycle=', '[probe] thread=')
    state = [line for line in log.splitlines() if line.startswith(state_prefixes)]
    launch = json.loads((run / 'launch.json').read_text(encoding='utf-8-sig'))
    cards = [(run / name).read_text(encoding='utf-8-sig').strip() for name in ('cards-before.json', 'cards-after.json')]
    return dict(path=str(run), stop=stops, state=state, launch=launch, cards=cards,
                exit=int((run / 'native.exit').read_text(encoding='utf-8-sig')))

a, b = load(args.reference), load(args.candidate)
report = dict(reference=a, candidate=b, files={})
for name in ('ee.bin', 'iop.bin', 'mixed.s16le', 'final.ppm', 'buttons.txt', 'racer_pcsx2_bridge44r.dll'):
    x, y = Path(a['path']) / name, Path(b['path']) / name
    record = dict(reference_exists=x.exists(), candidate_exists=y.exists())
    if x.exists() and y.exists():
        xb, yb = x.read_bytes(), y.read_bytes()
        record.update(reference_bytes=len(xb), candidate_bytes=len(yb),
                      reference_sha256=hashlib.sha256(xb).hexdigest(),
                      candidate_sha256=hashlib.sha256(yb).hexdigest(), equal=xb == yb)
        if xb != yb and name in ('ee.bin', 'iop.bin'):
            pages = [i // 4096 for i in range(0, max(len(xb), len(yb)), 4096) if xb[i:i+4096] != yb[i:i+4096]]
            record.update(differing_4k_pages=len(pages), first_differing_pages=pages[:32])
    report['files'][name] = record
report['matching_exact_stop'] = len(a['stop']) == 1 and a['stop'] == b['stop'] and a['stop'][0][0] == a['stop'][0][1]
report['terminal_state_equal'] = a['state'] == b['state']
report['cards_unchanged_and_equal'] = len(set(a['cards'] + b['cards'])) == 1
report['saved_state_equivalent'] = (a['exit'] == b['exit'] == 0 and report['matching_exact_stop']
    and report['terminal_state_equal'] and report['cards_unchanged_and_equal']
    and all(report['files'][n].get('equal', False) for n in ('ee.bin', 'iop.bin', 'buttons.txt', 'racer_pcsx2_bridge44r.dll')))
report['limitation'] = 'Only saved RAM, reported terminal state and this boundary; no complete CPU/GS hidden-state or full gameplay proof.'
assert not args.output.exists(), 'Preserve earlier comparison evidence'
args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
print(json.dumps({k:v for k,v in report.items() if k not in ('reference','candidate')}, indent=2))
raise SystemExit(0 if report['saved_state_equivalent'] else 1)
