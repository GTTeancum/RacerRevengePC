"""Verify final compiled source, native tests and application evidence for48I."""
from pathlib import Path
import argparse
import hashlib
import json
import re

ROOT = Path(__file__).resolve().parents[1]
def sha(path):
    with path.open('rb') as stream: return hashlib.file_digest(stream, 'sha256').hexdigest()
def js(path): return json.loads(path.read_text(encoding='utf-8-sig'))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build', type=Path, required=True)
    ap.add_argument('--x64-import', type=Path, required=True)
    ap.add_argument('--x86-import', type=Path, required=True)
    args = ap.parse_args(); build = args.build.resolve()
    records = []
    for arch in ('x64', 'x86'):
        assert (build / (arch + '.exit')).read_text().strip() == '0'
        log = (build / (arch + '-tests.log')).read_text(encoding='utf-8-sig')
        assert log.count('Test Passed.') == 18
        assert '61759 native collision mesh checks passed; 38 assets; 14586 faces' in log
        match = re.search(r'PASS (\d+) checks; (\d+) original-word ordered queries, hits=(\d+), misses=(\d+), exact visits=(\d+)', log)
        assert match and tuple(int(match[i]) for i in range(1,6)) == (5351,43,15,28,1195)
        for row in js(build / (arch + '-source-hashes.json')):
            assert sha(Path(row['Path'])) == row['Hash'].lower(), row['Path']
        for row in js(build / (arch + '-hashes.json')):
            assert sha(build / arch / Path(row['Path']).name) == row['Hash'].lower(), row['Path']
        mapping = (build / (arch + '-native.map')).read_text(errors='replace').lower()
        assert 'ps2runtime' not in mapping and 'dispatchguest' not in mapping and 'pcsx2' not in mapping
        path = getattr(args, arch + '_import').resolve()
        checks = js(path / 'checks.json'); world = js(path / 'world.json')
        assert checks['passed'] and checks['architecture'] == arch and checks['native_invocations'] == 7
        assert checks['executable_sha256'] == sha(build / arch / 'racer_native_pc.exe')
        assert world['success'] and world['native_collision_world_loaded']
        assert world['collision_runtime_format'] == 'native48I compact records'
        assert world['collision_placement_arithmetic'] == 'original-order binary32'
        assert (world['collision_placed_instances'], world['collision_unique_assets'], world['collision_absent_original_members']) == (37, 36, 281)
        assert world['frames_submitted'] == 3 and world['frames_presented'] == 0
        assert sha(path / 'world.ppm') == checks['world_capture_sha256'] == 'dcf6b6c4929180b54aef63ea192344dafe2394ef6654e9700c33d6d1d0489063'
        records.append(dict(architecture=arch, ctests=18, tree_checks=int(match[1]), query_fixtures=int(match[2]),
            exact_query_visits=int(match[5]), application_sha256=checks['executable_sha256'],
            application_evidence=path.relative_to(ROOT).as_posix(), collision_payload_bytes=world['collision_retained_payload_bytes']))
    proof = js(ROOT / 'evidence/collisionMesh48I/placement-recovery.json')
    assert proof['fixture_cases'] == 558 and proof['instruction_words'] == 56
    assert sha(ROOT / 'tools/assets/collision_placement_fixtures48i.h') == proof['fixture_sha256']
    assert sha(ROOT / 'evidence/collisionMesh48I/placement-words.bin') == proof['words_sha256']
    for row in proof['input_sources']: assert sha(ROOT / row['path']) == row['sha256']
    files = [p for directory in ('src','tests','tools','platform','cmake','docs') for p in (ROOT/directory).rglob('*')
             if p.is_file() and '48i' in p.name.lower() and p.suffix != '.pyc']
    files += [ROOT/p for p in ['README.md','CMakeLists.txt','platform/windows/native/main48b.cpp','platform/windows/native/import_service48c.cpp']]
    audit = dict(passed=True, build=build.relative_to(ROOT).as_posix(), builds=records,
        scope='Compact collision runtime, original placement and ordered native endpoint-pair/tree queries',
        full_vehicle_substep_verified=False, native_races_verified=False, native_racing_fps_measured=False, xbox_execution_verified=False,
        files=[dict(path=p.relative_to(ROOT).as_posix(),bytes=p.stat().st_size,sha256=sha(p)) for p in sorted(set(files))])
    (ROOT/'evidence/collisionMesh48I/audit.json').write_text(json.dumps(audit,indent=2)+'\n')
    print(json.dumps(dict(passed=True,builds=records)))

if __name__ == '__main__': main()
