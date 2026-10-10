"""Exercise the actual built-in importer, including cancellation and cache recovery."""
from pathlib import Path
import argparse
import ctypes
from ctypes import wintypes
import hashlib
import json
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
if not __debug__:
    raise RuntimeError('Importer verification requires normal Python execution')
PY_SHA = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--architecture', choices=['x64', 'x86'], required=True)
    args = parser.parse_args()
    exe = ROOT / f'build/native-pc48C-{args.architecture}/racer_native_pc.exe'
    out = Path(tempfile.mkdtemp(prefix=f'import48C-{args.architecture}-', dir=ROOT/'evidence'))
    cache = out/'native-cache'
    source = ROOT/'work/native/project/disc'
    iso = ROOT/'work/native/project/original/RacerRevenge.iso'
    source_hashes = {str(p): PY_SHA(p) for p in [source/'TRACKS/TA.RES', source/'SLUS_202.68']}
    results = []

    def run(label, src=source, dst=cache, extra=(), expected=0):
        command = [str(exe), '--import', '--source', str(src), '--cache', str(dst), '--container', 'TRACKS/TA.RES', *extra]
        result = subprocess.run(command, capture_output=True, encoding='utf-8', timeout=120)
        (out/(label+'.stdout.json')).write_text(result.stdout, encoding='utf-8')
        (out/(label+'.stderr.log')).write_text(result.stderr, encoding='utf-8')
        record = json.loads(result.stdout)
        if result.returncode != expected:
            raise RuntimeError(f'{label}: exit {result.returncode}, expected {expected}: {record}')
        results.append(dict(label=label, arguments=command, exit=result.returncode, result=record))
        return record

    first = run('01-cancel-new', extra=['--cancel-after', '3'], expected=130)
    assert first['cancelled'] and not (cache/'current.json').exists()
    resumed = run('02-resume')
    assert resumed['success'] and resumed['converted'] + resumed['reused'] == 140 and resumed['reused'] >= 3
    current = (cache/'current.json').read_bytes()
    manifest = json.loads(Path(resumed['manifest_path']).read_text())
    assert PY_SHA(Path(resumed['manifest_path'])) == resumed['generation']
    candidates = [m for m in manifest['members'] if m['status'] == 'converted_geometry']
    assert len(candidates) == 140
    # Compare every mesh with the frozen independent Python decoder's inventory.
    frozen = json.loads((ROOT/'evidence/assets48B-inventory/inventory.json').read_text())
    expected_meshes = {}
    for item in frozen['accepted_candidates']:
        expected_meshes[item['name']] = item
    comparisons = []
    for member in candidates:
        native = member['native']; mesh = cache/'objects'/(native['key']+'.rrmesh')
        assert PY_SHA(mesh) == native['mesh_sha256']
        item = expected_meshes[member['member']]
        # Ordinal is the deterministic original TA PSG index used by the inventory.
        independent = ROOT/'evidence/assets48B-inventory/decoded'/f"{item['ordinal']:03d}.rrmesh"
        assert mesh.read_bytes() == independent.read_bytes(), member['member']
        comparisons.append(dict(member=member['member'], sha256=PY_SHA(mesh)))
    reused = run('03-reuse')
    assert reused['converted'] == 0 and reused['reused'] == 140
    assert (cache/'current.json').read_bytes() == current
    victim = cache/'objects'/(candidates[0]['native']['key']+'.rrmesh')
    original = victim.read_bytes(); damaged = bytearray(original); damaged[-1] ^= 0x80; victim.write_bytes(damaged)
    repaired = run('04-corrupt-object-repair')
    assert repaired['converted'] >= 1 and victim.read_bytes() == original
    assert (cache/'current.json').read_bytes() == current
    metadata = victim.with_suffix('.json'); altered = json.loads(metadata.read_text()); altered['converter'] = 'obsolete'
    metadata.write_text(json.dumps(altered))
    invalidated = run('05-converter-version-repair')
    assert invalidated['converted'] >= 1 and json.loads(metadata.read_text())['converter'] == 'rr-native-static-48C-v1'
    altered = json.loads(metadata.read_text()); altered['world_space'] = not altered['world_space']
    metadata.write_text(json.dumps(altered))
    semantic_repair = run('05b-semantic-metadata-repair')
    assert semantic_repair['converted'] >= 1
    assert json.loads(metadata.read_text())['world_space'] == candidates[0]['native']['world_space']
    run('06-cancel-preserves-active', extra=['--cancel-after', '5'], expected=130)
    assert (cache/'current.json').read_bytes() == current
    iso_result = run('07-original-iso', src=iso)
    assert iso_result['converted'] == 0 and iso_result['reused'] == 140
    iso_manifest = json.loads(Path(iso_result['manifest_path']).read_text())
    assert iso_manifest['source']['image_sha256'] == PY_SHA(iso)
    assert iso_manifest['members'] == manifest['members'] and iso_manifest['containers'] == manifest['containers']
    active = (cache/'current.json').read_bytes()
    invalid_iso = out/'truncated.iso'; invalid_iso.write_bytes(b'bad ISO')
    run('08-invalid-source-preserves-active', src=invalid_iso, expected=1)
    assert (cache/'current.json').read_bytes() == active
    unrelated = out/'unrelated'; unrelated.mkdir(); (unrelated/'keep.txt').write_text('keep')
    run('09-unrelated-cache-rejected', dst=unrelated, expected=1)
    assert sorted(p.name for p in unrelated.iterdir()) == ['keep.txt']
    run('10-cache-in-original-rejected', dst=source/'native-cache-test48C', expected=1)
    assert not (source/'native-cache-test48C').exists()
    # Corrupt original RES in a disposable fixture; do not modify original game data.
    fixture = out/'source-fixture'; (fixture/'TRACKS').mkdir(parents=True)
    shutil.copyfile(source/'SLUS_202.68', fixture/'SLUS_202.68')
    bad = bytearray((source/'TRACKS/TA.RES').read_bytes()); bad[-1] ^= 1
    (fixture/'TRACKS/TA.RES').write_bytes(bad)
    run('11-corrupt-container-preserves-active', src=fixture, expected=1)
    assert (cache/'current.json').read_bytes() == active
    shutil.copyfile(source/'TRACKS/TA.RES', fixture/'TRACKS/TA.RES')
    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel.CreateFileW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD, ctypes.c_void_p,
                                  wintypes.DWORD, wintypes.DWORD, wintypes.HANDLE]
    kernel.CreateFileW.restype = wintypes.HANDLE
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    # A pre-existing writer prevents the importer from accepting a mixed snapshot.
    writer = kernel.CreateFileW(str(fixture/'TRACKS/TA.RES'), 0x40000000, 7, None, 3, 0x80, None)
    assert writer not in (None, ctypes.c_void_p(-1).value)
    try:
        blocked = run('12-active-source-writer-rejected', src=fixture, expected=1)
        assert 'unchanged' in blocked['error']
    finally:
        kernel.CloseHandle(writer)
    assert (cache/'current.json').read_bytes() == active
    lock = kernel.CreateFileW(str(cache/'import.lock'), 0xC0000000, 0, None, 3, 0x80, None)
    assert lock not in (None, ctypes.c_void_p(-1).value)
    try:
        blocked = run('13-concurrent-cache-import-rejected', expected=1)
        assert 'already using' in blocked['error']
    finally:
        kernel.CloseHandle(lock)
    assert (cache/'current.json').read_bytes() == active
    for path, digest in source_hashes.items():
        assert PY_SHA(Path(path)) == digest
    report = dict(architecture=args.architecture, executable_sha256=PY_SHA(exe), passed=True,
        native_invocations=len(results), original_meshes_byte_identical=len(comparisons),
        source_hashes_unchanged=source_hashes, mesh_comparisons=comparisons, runs=results)
    (out/'checks.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({'output':str(out),'passed':True,'native_invocations':len(results),'meshes_compared':len(comparisons)}))


if __name__ == '__main__':
    main()
