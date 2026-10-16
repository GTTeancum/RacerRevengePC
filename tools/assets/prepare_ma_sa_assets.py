#!/usr/bin/env python3
"""Root-owned cold preparation of exact original MA or SA assets.

UNEXECUTED source candidate. Builds/runs the established native importer and
source image decoders only after explicit central-slot authorization. An
unsupported present PSG is fatal, never relabeled absent or silently skipped.
"""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
sys.dont_write_bytecode = True
from ma_sa_asset_sources import authenticated_inventory, config, need, regular, safe, sha


def fresh(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as stream:
        stream.write(data)


def write_json(path, value):
    fresh(path, (json.dumps(value, sort_keys=True, indent=2)+'\n').encode())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--track', choices=('MA', 'SA'), required=True)
    parser.add_argument('--archive', type=Path, required=True, help='Exact original TRACKS/MA.RES or TRACKS/SA.RES')
    parser.add_argument('--intake', type=Path, required=True, help='Reviewed MA or SA intake containing manifest.json and members/')
    parser.add_argument('--output', type=Path, required=True, help='New private output root, separate from repository and inputs')
    parser.add_argument('--compiler', default='g++')
    parser.add_argument('--execute-preparation', action='store_true', required=True)
    args = parser.parse_args()
    os.umask(0o077)
    track, cfg = args.track, config(args.track)
    repo, intake = args.repo.resolve(strict=True), args.intake.resolve(strict=True)
    requested = args.output.absolute()
    need(not requested.exists() and not requested.is_symlink(), 'Output must be new')
    need(requested.parent.is_dir() and not any(p.is_symlink() for p in requested.parents),
         'Output parent must be a real existing directory')
    out = requested.parent.resolve()/requested.name
    need(repo not in out.parents and intake not in out.parents and out not in (repo, intake),
         'Output must be separate from source and inputs')
    here = Path(__file__).resolve().parent
    pins = json.loads((here/'ma-sa-preparation-source-pins.json').read_bytes())
    used = {}
    for row in pins['sources']:
        base = here if row['base'] == 'candidate' else repo
        path = base/safe(row['path']); regular(path)
        need(sha(path.read_bytes()) == row['sha256'], 'Preparation source pin differs: '+str(path))
        used[str(path)] = row['sha256']
    regular(args.archive)
    archive = args.archive.read_bytes()
    need(len(archive) == cfg['archive_bytes'] and sha(archive) == cfg['archive_sha256'], 'Exact original archive differs')
    regular(intake/'manifest.json')
    manifest_bytes = (intake/'manifest.json').read_bytes()
    inventory = authenticated_inventory(manifest_bytes, cfg)
    regular(intake/'retained48F-world.reference.json')
    reference = (intake/'retained48F-world.reference.json').read_bytes()
    need(sha(reference) == cfg['reference_sha256'], 'Reviewed original world reference differs')
    compiler = shutil.which(args.compiler)
    need(compiler and shutil.which('ffmpeg'), 'Installed compiler and FFmpeg are required')
    out.mkdir(mode=0o700)
    commands, stage = [], 'stage-authenticated-original-members'
    try:
        staged = out/(track.lower()+'-intake')
        fresh(staged/'original'/(track+'.RES'), archive)
        seen = set()
        for row in inventory['members']:
            relative = safe(row['path'])
            need(str(relative).casefold() not in seen, 'Duplicate original member path')
            seen.add(str(relative).casefold())
            source = intake/'members'/relative; regular(source)
            data = source.read_bytes()
            need(len(data) == row['bytes'] and sha(data) == row['sha256'], 'Original member differs: '+str(relative))
            fresh(staged/'intake/original-members'/relative, data)
        fresh(staged/'intake/archive-inventory.json', manifest_bytes)
        fresh(staged/'intake/retained48F-world.reference.json', reference)
        fresh(out/'work/native/project/disc/TRACKS'/(track+'.RES'), archive)
        (out/'preparation-tools').mkdir(); (out/'preparation-logs').mkdir()
        (out/'tests/native').mkdir(parents=True)

        def run(label, argv):
            nonlocal stage
            stage = label
            command = [str(x) for x in argv]; commands.append(command)
            with (out/'preparation-logs'/f'{len(commands):02}-{label}.log').open('xb') as log:
                subprocess.run(command, cwd=out, env=dict(os.environ, PYTHONDONTWRITEBYTECODE='1'),
                               stdout=log, stderr=subprocess.STDOUT, timeout=1800, check=True)

        native_importer = out/'preparation-tools/import_original_track'
        deps = ['import_decode48c', 'import_scene48d', 'import_texture48e',
                'scene_asset48d', 'native_asset48b', 'pod_physics49n']
        run('build-original-native-importer', [compiler, '-std=c++17', '-O2', '-ffp-contract=off',
            '-I'+str(repo/'src'), '-I'+str(repo/'third_party'),
            repo/'tools/assets/original_roster/import_original_pods.cpp',
            *[repo/f'src/assets/{name}.cpp' for name in deps], '-lz', '-o', native_importer])
        run('import-original-'+track.lower(), [native_importer, staged/'original'/(track+'.RES'), staged/'native-import',
            *(['--authenticated-ma-compressed'] if track == 'MA' else [])])
        for name in ('assemble_ma_sa_visual.py', 'recover_ma_sa_sky.py', 'inventory_ma_sa_atm_frames.py'):
            run(Path(name).stem, [sys.executable, '-B', here/name, '--repo', repo,
                '--track', track, '--intake', staged, '--output-root', out])
        generated = {}
        for name in (track.lower()+'_visual_profile.inc', 'source_texture_'+track.lower()+'_index.inc',
                     'sky_'+track.lower()+'_index.inc', track.lower()+'_atm_animation_index.inc',
                     track.lower()+'_atm_frame_index.inc', track.lower()+'_atm_source_index.inc'):
            path = out/'tests/native'/name; regular(path)
            generated[name] = sha(path.read_bytes())
        for path, digest in used.items():
            need(sha(Path(path).read_bytes()) == digest, 'Preparation source changed during run: '+path)
        write_json(out/'PREPARATION-COMPLETE.json', dict(status='original_'+track.lower()+'_static_assets_cold_prepared',
            archive_sha256=cfg['archive_sha256'], input_inventory_sha256=sha(manifest_bytes), source_sha256=used,
            commands=commands, generated_index_sha256=generated, gameplay_validated=False,
            limitations=['No gameplay, test, shader or renderer was launched.',
                         'Generated native asset profiles need independent review before client integration.',
                         'All original ATM frame images are retained; live animation is a separate owner.',
                         'SA authored cube selection is zone-specific and must be honored by its runtime owner.',
                         'Authentic asset closure is not complete-course gameplay admission.']))
        print(str(out/'PREPARATION-COMPLETE.json'))
    except BaseException as error:
        write_json(out/'FAILURE.json', dict(status='quarantined_partial_do_not_launch', stage=stage,
                                           error=str(error), commands=commands))
        raise


if __name__ == '__main__':
    main()
