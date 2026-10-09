"""Restore verified 49W files without deleting local files or replacing Git metadata."""
from pathlib import Path
import datetime
import hashlib
import json
import shutil
import zipfile

ROOT = Path(__file__).resolve().parents[1]
HANDOFF = ROOT / 'work/handoff49w'
SEALED = HANDOFF / 'sealed49w'

def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()

def main():
    receipt = json.loads((HANDOFF / 'receipt49w.json').read_text())
    assert digest(Path(receipt['retained_archive'])) == receipt['archive_sha256']
    out = HANDOFF / 'integration49w'
    out.mkdir(exist_ok=False)
    changes = []
    same = 0
    for row in receipt['files']:
        parts = Path(row['path']).parts
        if parts[0] != 'RacerRevengePC' or parts[1] in ('.git', 'build'):
            continue
        rel = Path(*parts[1:])
        source, target = SEALED / row['path'], ROOT / rel
        assert digest(source) == row['sha256'], str(source)
        before = digest(target) if target.is_file() else None
        if before == row['sha256']:
            same += 1
            continue
        assert not target.exists() or target.is_file(), str(target)
        changes.append({'path': rel.as_posix(), 'before_sha256': before,
                        'after_sha256': row['sha256'], 'bytes': row['bytes']})
    plan = {'baseline': '49W', 'archive_sha256': receipt['archive_sha256'],
            'unchanged_files': same, 'changes': changes,
            'excluded': ['incoming Git metadata', 'incoming build directory'],
            'created_utc': datetime.datetime.now(datetime.timezone.utc).isoformat()}
    (out / 'plan.json').write_text(json.dumps(plan, indent=2) + '\n')
    backup = out / 'previous-overwritten-files.zip'
    with zipfile.ZipFile(backup, 'x', zipfile.ZIP_DEFLATED, compresslevel=6) as z:
        z.writestr('RESTORE-MANIFEST.json', json.dumps(plan, indent=2))
        for row in changes:
            if row['before_sha256']:
                z.write(ROOT / row['path'], 'previous/' + row['path'])
    with zipfile.ZipFile(backup) as z:
        for row in changes:
            if row['before_sha256']:
                assert hashlib.sha256(z.read('previous/' + row['path'])).hexdigest() == row['before_sha256']
    print('Verified backup; applying', len(changes), 'file updates', flush=True)
    for index, row in enumerate(changes):
        target = ROOT / row['path']
        if row['before_sha256']:
            assert digest(target) == row['before_sha256'], 'Concurrent edit: ' + str(target)
        else:
            assert not target.exists(), 'Concurrent creation: ' + str(target)
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(SEALED / 'RacerRevengePC' / row['path'], target)
        assert digest(target) == row['after_sha256']
        if index % 4000 == 0:
            print('Applied', index + 1, 'of', len(changes), flush=True)
    report = {k: v for k, v in plan.items() if k != 'changes'}
    report.update(status='verified', updated_files=len(changes),
                  replaced_files=sum(bool(r['before_sha256']) for r in changes),
                  backup=str(backup), backup_sha256=digest(backup),
                  completed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
    (out / 'result.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2), flush=True)

if __name__ == '__main__':
    main()
