#!/usr/bin/env python3
"""Root-owned cold preparation for original GA/Watchtower Run.

UNEXECUTED source candidate. This command compiles/runs the existing native
asset importer and source texture decoders. It must only run in an authorized
central slot. It never launches gameplay or claims complete track admission.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import stat
import subprocess
import sys
sys.dont_write_bytecode = True
GA_SHA = '063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9'


def need(ok, why):
    if not ok:
        raise ValueError(why)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def regular(path):
    need(stat.S_ISREG(path.lstat().st_mode) and not any(p.is_symlink() for p in path.parents),
         'Expected readable nonsymlink regular file: '+str(path))


def safe(path):
    need(path and '\\' not in path and '\0' not in path and
         all(p not in ('', '.', '..') and ':' not in p for p in path.split('/')),
         'Unsafe relative input path')
    return Path(path)


def fresh(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as stream:
        stream.write(data)


def write_json(path, value):
    fresh(path, (json.dumps(value, sort_keys=True, indent=2)+'\n').encode())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--intake', type=Path, required=True,
                        help='Verified source directory with original/GA.RES and intake/archive-inventory.json')
    parser.add_argument('--output', type=Path, required=True, help='New private output root')
    parser.add_argument('--compiler', default='g++')
    parser.add_argument('--execute-preparation', action='store_true', required=True)
    args = parser.parse_args()
    os.umask(0o077)
    repo, intake = args.repo.resolve(strict=True), args.intake.resolve(strict=True)
    requested = args.output.absolute()
    need(not requested.exists() and not requested.is_symlink(), 'Output must be new')
    need(requested.parent.is_dir() and not any(p.is_symlink() for p in requested.parents),
         'Output parent must be a real existing directory')
    out = requested.parent.resolve()/requested.name
    need(repo not in out.parents and intake not in out.parents and out not in (repo, intake),
         'Output must be separate from source and input directories')
    here = Path(__file__).resolve().parent
    pins = json.loads((here/'ga-preparation-source-pins.json').read_bytes())
    used = {}
    for row in pins['sources']:
        base = here if row['base'] == 'candidate' else repo
        path = base/safe(row['path'])
        regular(path)
        need(sha(path.read_bytes()) == row['sha256'], 'Preparation source pin differs: '+str(path))
        used[str(path)] = row['sha256']
    archive = intake/'original/GA.RES'; regular(archive)
    archive_bytes = archive.read_bytes()
    need(len(archive_bytes) == 2834942 and sha(archive_bytes) == GA_SHA, 'Original GA archive differs')
    manifest_path = intake/'intake/archive-inventory.json'; regular(manifest_path)
    manifest_bytes = manifest_path.read_bytes()
    need(sha(manifest_bytes) == pins['inventory_sha256'], 'Reviewed original source inventory differs')
    inventory = json.loads(manifest_bytes)
    need(inventory['archive_sha256'] == GA_SHA and len(inventory['members']) == 970,
         'Original GA inventory extent differs')
    compiler = shutil.which(args.compiler)
    need(compiler and shutil.which('ffmpeg'), 'Installed compiler and FFmpeg are required')
    out.mkdir(mode=0o700)
    commands, stage = [], 'stage-authenticated-original-members'
    try:
        staged = out/'ga-intake'
        fresh(staged/'original/GA.RES', archive_bytes)
        seen = set()
        for row in inventory['members']:
            relative = safe(row['path'])
            need(str(relative).casefold() not in seen, 'Duplicate original member path')
            seen.add(str(relative).casefold())
            source = intake/'intake/original-members'/relative; regular(source)
            data = source.read_bytes()
            need(len(data) == row['bytes'] and sha(data) == row['sha256'],
                 'Original GA member differs: '+str(relative))
            fresh(staged/'intake/original-members'/relative, data)
        fresh(staged/'intake/archive-inventory.json', manifest_bytes)
        fresh(out/'work/native/project/disc/TRACKS/GA.RES', archive_bytes)
        (out/'preparation-tools').mkdir(); (out/'preparation-logs').mkdir()
        (out/'tests/native').mkdir(parents=True)

        def run(label, argv):
            nonlocal stage
            stage = label
            command = [str(x) for x in argv]; commands.append(command)
            with (out/'preparation-logs'/f'{len(commands):02}-{label}.log').open('xb') as log:
                subprocess.run(command, cwd=out, env=dict(os.environ, PYTHONDONTWRITEBYTECODE='1'),
                               stdout=log, stderr=subprocess.STDOUT, timeout=1800, check=True)

        native_importer = out/'preparation-tools/import_original_ga'
        deps = ['import_decode48c', 'import_scene48d', 'import_texture48e',
                'scene_asset48d', 'native_asset48b', 'pod_physics49n']
        run('build-original-native-importer', [compiler, '-std=c++17', '-O2', '-ffp-contract=off',
            '-I'+str(repo/'src'), '-I'+str(repo/'third_party'),
            repo/'tools/assets/original_roster/import_original_pods.cpp',
            *[repo/f'src/assets/{name}.cpp' for name in deps], '-lz', '-o', native_importer])
        run('import-original-ga', [native_importer, staged/'original/GA.RES', staged/'native-import'])
        for name in ('assemble_ga_visual.py', 'recover_ga_sky.py', 'inventory_ga_atm_frames.py'):
            run(Path(name).stem, [sys.executable, '-B', here/name, '--repo', repo,
                '--intake', staged, '--output-root', out])
        generated = {}
        for name in ('ga_visual_profile.inc', 'source_texture_ga_index.inc', 'sky_ga_index.inc',
                     'ga_atm_animation_index.inc', 'ga_atm_frame_index.inc'):
            path = out/'tests/native'/name; regular(path)
            generated[name] = sha(path.read_bytes())
        for path, digest in used.items():
            need(sha(Path(path).read_bytes()) == digest, 'Preparation source changed while running: '+path)
        write_json(out/'PREPARATION-COMPLETE.json', dict(status='original_ga_static_assets_cold_prepared',
            archive_sha256=GA_SHA, input_inventory_sha256=sha(manifest_bytes), source_sha256=used,
            commands=commands, generated_index_sha256=generated, gameplay_validated=False,
            limitations=['No gameplay or renderer was launched.',
                         'Generated profiles must be independently reviewed before client integration.',
                         'Dynamic Gate, LostAI triggers, environment and source lighting are separate owners.',
                         'Static source admission is not a claim that the complete Watchtower Run can be played.']))
        print(str(out/'PREPARATION-COMPLETE.json'))
    except BaseException as error:
        write_json(out/'FAILURE.json', dict(status='quarantined_partial_do_not_launch', stage=stage,
                                           error=str(error), commands=commands))
        raise


if __name__ == '__main__':
    main()
