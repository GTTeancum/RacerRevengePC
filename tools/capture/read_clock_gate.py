#!/usr/bin/env python3
"""Read-only central rolling-hour gate. Never reserves, builds, or launches."""
import datetime as dt
import json
from pathlib import Path


LEDGER = Path('/workspace/shared/rr-last-central-test.json')
ACTIVE = {'running', 'launching', 'reserved', 'building', 'ready', 'active'}
TERMINAL = {'completed', 'complete', 'passed', 'failed', 'stopped', 'aborted', 'done',
            'passed_focused_menu_clegg_capture'}


def aware(value):
    result = dt.datetime.fromisoformat(value.replace('Z', '+00:00'))
    if result.tzinfo is None:
        raise ValueError('Ledger timestamps require an explicit timezone')
    return result.astimezone(dt.timezone.utc)


def main():
    ledger = json.loads(LEDGER.read_bytes())
    status = ledger.get('status')
    if ledger.get('owner') != 'root' or status not in ACTIVE | TERMINAL:
        raise RuntimeError('Unknown central owner/status; root must reconcile the ledger')
    started = aware(ledger['started_utc'])
    earliest = max(started + dt.timedelta(hours=1),
                   aware(ledger.get('next_earliest_utc', (started + dt.timedelta(hours=1)).isoformat())))
    now = dt.datetime.now(dt.timezone.utc)
    if started > now:
        raise RuntimeError('Future central start timestamp')
    eligible = status in TERMINAL and now >= earliest
    print(json.dumps({'read_only': True, 'ledger': str(LEDGER), 'now_utc': now.isoformat(),
        'previous_started_utc': ledger['started_utc'], 'previous_status': status,
        'next_earliest_utc': earliest.isoformat(), 'seconds_remaining': max(0, (earliest - now).total_seconds()),
        'eligible_for_root_to_reserve_new_slot': eligible,
        'runtime_launch_authorized': False,
        'note': 'Root must reserve the sole new build/runtime slot after this gate; this command changes nothing.'}, indent=2))
    return 0 if eligible else 2


if __name__ == '__main__':
    raise SystemExit(main())
