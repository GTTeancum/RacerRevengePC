#!/usr/bin/env python3
"""One root-owned COLD preparation command. Never launches a game or emulator.
UNEXECUTED reconstruction: review and run only in the centralized authorized slot.
Failure leaves a quarantined partial output with FAILURE.json; never resumes it.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import stat
import subprocess
import sys
import zipfile
sys.dont_write_bytecode = True
GC_SHA = '69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9'
FULL = 'RacerRevenge-Checkpoint48F-NativeWorldCacheAndRendering-Evidence.zip'
GEN = '3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39'
ARCHIVES = {
 'RacerRevenge-50HS-Race-LinuxRuntime-20261009.zip': '8d7da47006c2ba4900ea9558b9874c617d1fc166de9a1d2640ba85cc634db49c',
 'RacerRevenge-original-AUDIO.zip': 'c246ad58020726d92a65a56e09e4cd9b68ba60cdcc9aa697d2cb48e51275aafb',
 'RacerRevenge-original-GC-track.zip': '4b24307f08ce6a73a169d73425f03be11c7c82782bb8f77205d95a81bb87562c',
 'racer-revenge-original-pods-03-04-07-08-09-10-11.zip': 'd844ce6548265ebff52eeaef0332b46b67898d0cb4c8cc7c6c6f0e14f7c28d82',
 FULL: '60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c',
}

def need(ok, why):
    if not ok:
        raise ValueError(why)

def sha(b):
    return hashlib.sha256(b).hexdigest()

def file_sha(p):
    h = hashlib.sha256()
    with p.open('rb') as f:
        for b in iter(lambda: f.read(1024*1024), b''):
            h.update(b)
    return h.hexdigest()

def safe(name):
    need('\\' not in name and '\0' not in name, 'Unsafe path')
    parts = name.split('/')
    need(all(p not in ('', '.', '..') and ':' not in p for p in parts), 'Unsafe path components')
    return PurePosixPath(name)

def regular(p):
    need(stat.S_ISREG(p.lstat().st_mode), 'Expected non-symlink regular file: '+str(p))
    need(not any(q.is_symlink() for q in p.parents), 'Symlink ancestor: '+str(p))

def fresh(p, b):
    p.parent.mkdir(parents=True, exist_ok=True)
    with p.open('xb') as f:
        f.write(b)
    p.chmod(0o600)

def write_json(p, value):
    fresh(p, (json.dumps(value, indent=2, sort_keys=True)+'\n').encode())

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--repo', required=True, type=Path)
    p.add_argument('--inputs', required=True, type=Path)
    p.add_argument('--output', required=True, type=Path)
    p.add_argument('--compiler', default='g++')
    p.add_argument('--execute-preparation', required=True, action='store_true')
    args = p.parse_args()
    os.umask(0o077)
    repo, inputs = args.repo.resolve(strict=True), args.inputs.resolve(strict=True)
    # Reject symlink spelling before resolving the new destination.
    requested = args.output.absolute()
    need(not requested.exists() and not requested.is_symlink(), 'Output already exists')
    need(requested.parent.is_dir() and not any(q.is_symlink() for q in (requested.parent, *requested.parents)), 'Output parent must be real and existing')
    out = requested.parent.resolve()/requested.name
    need(out != repo and repo not in out.parents and out != inputs and inputs not in out.parents, 'Output cannot be within source or recovered inputs')
    compiler = shutil.which(args.compiler)
    need(compiler is not None and shutil.which('ffmpeg') is not None, 'Compiler and FFmpeg must already be installed')
    here = Path(__file__).resolve().parent
    # Freeze and authenticate source dependencies against the delivered manifest.
    pins = {}
    for line in (here/'REPOSITORY-SOURCES.sha256').read_text().splitlines():
        digest, name = line.split('  ', 1)
        relative = safe(name); source = repo/relative
        regular(source); need(file_sha(source) == digest, 'Source pin mismatch: '+name); pins[name] = digest
    for name, digest in ARCHIVES.items():
        archive = inputs/name; regular(archive)
        need(file_sha(archive) == digest, 'Recovered archive identity mismatch: '+name)
    with zipfile.ZipFile(inputs/FULL) as z:
        generation = 'evidence/importFull48F-20261003/native-cache/generations/'+GEN+'.json'
        need(z.namelist().count(generation) == 1 and sha(z.read(generation)) == GEN, 'Pinned native cache generation missing or changed')
    out.mkdir(mode=0o700)
    stage = 'stage-authenticated-original-data'
    commands = []
    try:
        # Only inventoried, previously recovered data files are admitted. Compare
        # each to its authenticated ZIP member; never use old scripts/binaries.
        inventory_path = inputs/'extracted-inventory.json'; regular(inventory_path)
        rows = json.loads(inventory_path.read_bytes())
        need(isinstance(rows, list) and len(rows) <= 10000, 'Recovered inventory bounds')
        archives = {}; seen_sources = set(); destinations = {}; total = 0
        try:
            for row in rows:
                archive, member = row['archive'], row['member']; safe(member)
                need(archive in ARCHIVES and archive != FULL, 'Unexpected extracted source archive')
                key = (archive, member); need(key not in seen_sources, 'Duplicate recovered inventory row'); seen_sources.add(key)
                target = None
                if archive.startswith('RacerRevenge-50HS-'):
                    for prefix in ('RacerRevenge50HS/evidence/', 'RacerRevenge50HS/work/'):
                        if member.startswith(prefix): target = member[len('RacerRevenge50HS/'):]
                elif archive == 'RacerRevenge-original-AUDIO.zip' and member.startswith('AUDIO/'):
                    target = 'work/native/project/disc/'+member
                elif archive == 'RacerRevenge-original-GC-track.zip' and member == 'TRACKS/GC.RES':
                    target = 'work/native/project/disc/'+member
                elif archive.startswith('racer-revenge-original-pods-') and member.startswith('PODS/'):
                    target = 'work/native/project/disc/'+member
                if target is None: continue
                safe(target)
                need(Path(target).suffix.lower() not in ('.py', '.sh', '.exe', '.so', '.dll', '.bat', '.cmd', '.ps1'), 'Executable/script data rejected')
                if archive not in archives:
                    archives[archive] = zipfile.ZipFile(inputs/archive)
                z = archives[archive]
                need(z.namelist().count(member) == 1, 'Duplicate or missing archive member')
                info = z.getinfo(member)
                need(not stat.S_ISLNK(info.external_attr >> 16) and info.file_size <= 128*1024*1024, 'Unsafe archive entry')
                data = z.read(info); total += len(data)
                need(total <= 512*1024*1024, 'Staging aggregate limit')
                need(len(data) == row['size'] and sha(data) == row['sha256'], 'Recovered inventory differs from authenticated archive')
                original_elf_data = (target == 'work/native/project/disc/SLUS_202.68' and
                    len(data) == 2972720 and sha(data) == 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30')
                need(original_elf_data or not data.startswith((b'\x7fELF', b'MZ', b'#!')), 'Executable payload rejected')
                # Supplied local paths are not trusted as filesystem destinations.
                relative = Path('extracted')/Path(archive).stem/PurePosixPath(member)
                recovered = inputs/relative; regular(recovered)
                need(recovered.read_bytes() == data, 'Extracted source differs from original ZIP')
                folded = target.casefold()
                if folded in destinations:
                    old = destinations[folded]
                    need(old == target and (out/old).read_bytes() == data, 'Staging path/byte collision')
                    continue
                destinations[folded] = target; fresh(out/target, data)
        finally:
            for z in archives.values(): z.close()
        disc = out/'work/native/project/disc'
        gc = disc/'TRACKS/GC.RES'
        need(gc.stat().st_size == 4205067 and file_sha(gc) == GC_SHA, 'Exact original GC identity')
        for pod in (1, 3, 4, 7, 8, 9, 10, 11):
            regular(disc/f'PODS/POD{pod:02}/POD{pod:02}.RES')
        for reserved in ('evidence/originalRoster50HW', 'evidence/visualGC', 'evidence/texturesGC', 'evidence/skyGC', 'gc-intake', 'preparation-tools', 'preparation-logs', 'tests'):
            need(not (out/reserved).exists(), 'Prepared output collision: '+reserved)
        (out/'preparation-tools').mkdir(); (out/'preparation-logs').mkdir(); (out/'tests/native').mkdir(parents=True)
        def run(label, command):
            nonlocal stage
            stage = label; commands.append([str(x) for x in command])
            with (out/'preparation-logs'/f'{len(commands):02}-{label}.log').open('xb') as log:
                subprocess.run(commands[-1], cwd=out, env=dict(os.environ, PYTHONDONTWRITEBYTECODE='1'), stdout=log, stderr=subprocess.STDOUT, check=True, timeout=1800)
        flags = ['-std=c++17', '-O2', '-ffp-contract=off', '-I'+str(repo/'src'), '-I'+str(repo/'third_party')]
        importer = out/'preparation-tools/import_original_pods'
        extractor = out/'preparation-tools/extract_gc_members'
        deps = ['import_decode48c', 'import_scene48d', 'import_texture48e', 'scene_asset48d', 'native_asset48b', 'pod_physics49n']
        run('build-original-importer', [compiler, *flags, repo/'tools/assets/original_roster/import_original_pods.cpp', *[repo/f'src/assets/{s}.cpp' for s in deps], '-lz', '-o', importer])
        run('build-cold-extractor', [compiler, *flags, here/'extract_gc_members.cpp', repo/'src/assets/import_decode48c.cpp', '-lz', '-o', extractor])
        intake = out/'gc-intake'; intake.mkdir(); (intake/'original').mkdir()
        fresh(intake/'original/GC.RES', gc.read_bytes())
        run('extract-gc', [extractor, intake/'original/GC.RES', intake/'intake'])
        extraction = json.loads((intake/'intake/extraction-index.json').read_bytes())
        for row in extraction['members']:
            data = (intake/'intake/original-members'/safe(row['path'])).read_bytes()
            need(len(data) == row['bytes'], 'Extracted member size changed'); row['sha256'] = sha(data)
        extraction['source_container_sha256'] = GC_SHA
        write_json(intake/'intake/archive-inventory.json', extraction)
        run('import-gc-native', [importer, intake/'original/GC.RES', intake/'native-import'])
        script = lambda name: [sys.executable, '-B', repo/'tools/assets'/name]
        roster = out/'evidence/originalRoster50HW'
        run('prepare-original-roster', [*script('original_roster/prepare_original_roster.py'), '--importer', importer, '--disc', disc, '--output', roster/'raw-imports'])
        run('select-original-visuals', [*script('original_roster/select_original_roster.py'), '--archive', inputs/FULL, '--output', roster/'original-eight-visuals', '--audio-root', disc/'AUDIO', '--raw-imports', roster/'raw-imports'])
        for name in ('assemble_gc_visual.py', 'recover_gc_sky.py', 'inventory_gc_atm_frames.py'):
            run(Path(name).stem, [*script(name), '--repo', repo, '--intake', intake, '--output-root', out])
        stage = 'authenticate-generated-indexes'
        indexes = ('source_texture_gc_index.inc', 'sky_gc_index.inc', 'gc_atm_animation_index.inc', 'gc_atm_frame_index.inc')
        for name in indexes:
            need((out/'tests/native'/name).read_bytes() == (repo/'tests/native'/name).read_bytes(), 'Generated index differs from retained fixture: '+name)
        for name, digest in pins.items():
            need(file_sha(repo/name) == digest, 'Source changed during preparation: '+name)
        need(file_sha(gc) == GC_SHA, 'Staged original GC changed')
        write_json(out/'PREPARATION-COMPLETE.json', dict(status='cold_preparation_only', archive_sha256=ARCHIVES, source_sha256=pins, commands=commands, retained_indexes=list(indexes), gameplay_validated=False, limitations=['No runtime build or gameplay checks run.', 'GC incomplete-world limitations remain; no guest execution.']))
        print(str(out/'PREPARATION-COMPLETE.json'))
    except BaseException as error:
        write_json(out/'FAILURE.json', dict(stage=stage, error=str(error), commands=commands, status='quarantined_partial_preparation_do_not_launch'))
        raise

if __name__ == '__main__':
    main()
