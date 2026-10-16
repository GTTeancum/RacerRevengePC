#!/usr/bin/env python3
"""Bridge exact original SA through the bounded native extraction-only helper.
Source-only candidate. Builds nothing; launches only the explicitly supplied helper.
Run only under root's authorized cold-preparation operation, into a new root.
"""
import argparse
import json
import os
from pathlib import Path
import zipfile
import sys
import subprocess
sys.dont_write_bytecode = True
from ma_sa_asset_sources import authenticated_inventory, config, need, regular, safe, sha

FULL_SHA = '60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c'
GENERATION = '3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39'
GENERATION_PATH = 'evidence/importFull48F-20261003/native-cache/generations/'+GENERATION+'.json'
REFERENCE_PATH = 'evidence/worldImport48F/sa-tracks_sa.reference.json'


def canonical(value):
    return (json.dumps(value, sort_keys=True, indent=2, ensure_ascii=True,
                       allow_nan=False)+'\n').encode('ascii')


def read_file(path, maximum):
    regular(path)
    need(path.stat().st_size <= maximum, 'Input exceeds byte bound: '+str(path))
    return path.read_bytes()


def fresh(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as stream:
        stream.write(data)


def unique_zip_member(archive, name, maximum):
    matches = [entry for entry in archive.infolist() if entry.filename == name]
    need(len(matches) == 1 and not matches[0].is_dir() and matches[0].file_size <= maximum,
         'Missing, duplicated or oversized retained metadata: '+name)
    return archive.read(matches[0])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--archive', type=Path, required=True, help='Exact original SA.RES')
    parser.add_argument('--full48f', type=Path, required=True)
    parser.add_argument('--extractor', type=Path, required=True,
                        help='Root-built extract_sa_members from this reviewed source packet')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--execute-preparation', action='store_true', required=True)
    args = parser.parse_args()
    os.umask(0o077)
    repo = args.repo.resolve(strict=True)
    tool_root = Path(__file__).resolve().parent
    pins = json.loads(read_file(tool_root/'ma-sa-preparation-source-pins.json', 1024*1024))
    used = {}
    for row in pins['sources']:
        need(row['base'] in ('repo', 'candidate'), 'Unexpected source pin base')
        path = (tool_root if row['base'] == 'candidate' else repo)/safe(row['path'])
        need(sha(read_file(path, 16*1024*1024)) == row['sha256'], 'Preparation source pin differs: '+str(path))
        used[str(path)] = row['sha256']
    cfg = config('SA')
    archive_bytes = read_file(args.archive, cfg['archive_bytes'])
    need(len(archive_bytes) == cfg['archive_bytes'] and sha(archive_bytes) == cfg['archive_sha256'],
         'Exact original SA archive differs')
    full = read_file(args.full48f, 300*1024*1024)
    need(sha(full) == FULL_SHA, 'Exact retained Full48F archive differs')
    # Read from the authenticated in-memory bytes, not a reopened mutable path.
    import io
    with zipfile.ZipFile(io.BytesIO(full)) as archive:
        generation_bytes = unique_zip_member(archive, GENERATION_PATH, 64*1024*1024)
        need(sha(generation_bytes) == GENERATION, 'Retained generation differs')
        generation = json.loads(generation_bytes)
        reference = unique_zip_member(archive, REFERENCE_PATH, 8*1024*1024)
    need(sha(reference) == cfg['reference_sha256'], 'Retained SA world reference differs')
    containers = [row for row in generation['containers'] if row['name'] == 'TRACKS/SA.RES']
    need(len(containers) == 1 and containers[0]['bytes'] == cfg['archive_bytes'] and
         containers[0]['sha256'] == cfg['archive_sha256'] and
         containers[0]['entries'] == cfg['archive_entries'], 'Retained SA container differs')
    members = []
    seen = set()
    for row in generation['members']:
        if row['container'] != 'TRACKS/SA.RES':
            continue
        name = row['member']
        path = name.replace('\\', '/')
        safe(path)
        need(path.casefold() not in seen, 'Duplicate retained SA member')
        seen.add(path.casefold())
        members.append(dict(name=name, path=path, bytes=row['source_bytes'],
                            logical_offset=row['logical_offset'], sha256=row['source_sha256'],
                            **{'48F_status': row['status']}))
    need(len(members) == cfg['archive_entries'], 'Retained SA member count differs')
    for row in members:
        parent = Path(row['path']).parent
        while str(parent) != '.':
            need(parent.as_posix().casefold() not in seen, 'Retained file/directory collision')
            parent = parent.parent
    manifest = canonical(dict(schema='RR_ORIGINAL_SA_MEMBER_INVENTORY_V2',
        archive_sha256=cfg['archive_sha256'], archive_bytes=cfg['archive_bytes'],
        archive_decoded_bytes=cfg['archive_decoded_bytes'], source_archive_sha256=FULL_SHA,
        source_generation_sha256=GENERATION, members=members))
    authenticated_inventory(manifest, cfg)  # Exact reviewed bytes, never a newly trusted arbitrary manifest.
    extractor_bytes = read_file(args.extractor, 128*1024*1024)
    extractor_sha = sha(extractor_bytes)
    extractor = args.extractor.resolve(strict=True)
    requested = args.output.absolute()
    need(not requested.exists() and not requested.is_symlink(), 'Output must be new')
    need(requested.parent.is_dir() and not any(p.is_symlink() for p in requested.parents),
         'Output parent must be a real existing directory')
    out = requested.parent.resolve()/requested.name
    # The small native helper uses main(char**) on Windows. Do not allow the
    # platform ANSI conversion to silently change any path passed to it.
    need(os.name != 'nt' or (str(out).isascii() and str(extractor).isascii()),
         'This Windows cold extractor requires ASCII output and executable paths')
    need(out != tool_root and tool_root not in out.parents and out != repo and repo not in out.parents,
         'Output cannot be within source tools/repository')
    out.mkdir(mode=0o700)
    try:
        fresh(out/'original/SA.RES', archive_bytes)
        with (out/'extraction.log').open('xb') as log:
            subprocess.run([str(extractor), str(out/'original/SA.RES'), str(out/'extracted')],
                           stdout=log, stderr=subprocess.STDOUT, timeout=1800, check=True)
        need(sha(read_file(extractor, 128*1024*1024)) == extractor_sha, 'Extractor changed during preparation')
        extracted_bytes = read_file(out/'extracted/extraction-index.json', 4*1024*1024)
        extracted = json.loads(extracted_bytes)
        need(extracted['schema'] == 'RR_ORIGINAL_SA_EXTRACT_V2', 'Native extraction schema differs')
        rows = extracted['members']
        need(len(rows) == len(members), 'Native extracted member count differs')
        by_name = {}
        for index, actual in enumerate(rows):
            need(actual['source_index'] == index and actual['file'] == str(index)+'.bin' and
                 actual['name'] not in by_name, 'Native extraction row/index duplicate or mismatch')
            by_name[actual['name']] = actual
        authenticated = []
        total = 0
        for expected in members:
            actual = by_name.get(expected['name'])
            need(actual is not None and
                 actual['logical_offset'] == expected['logical_offset'] and
                 actual['bytes'] == expected['bytes'],
                 'Native extraction source row differs')
            data = read_file(out/'extracted'/actual['file'], 8*1024*1024)
            total += len(data)
            need(total <= 128*1024*1024 and len(data) == expected['bytes'] and
                 sha(data) == expected['sha256'], 'Original extracted SA member differs: '+expected['name'])
            authenticated.append((expected['path'], data))
        for name, data in authenticated:
            fresh(out/'members'/safe(name), data)
        for path, digest in used.items():
            need(sha(read_file(Path(path), 16*1024*1024)) == digest, 'Preparation source changed: '+path)
        fresh(out/'manifest.json', manifest)
        fresh(out/'retained48F-world.reference.json', reference)
        fresh(out/'INTAKE-COMPLETE.json', canonical(dict(status='authenticated_sa_intake_v2',
            manifest_sha256=sha(manifest), original_archive_sha256=cfg['archive_sha256'],
            reference_sha256=sha(reference), source_archive_sha256=FULL_SHA,
            source_generation_sha256=GENERATION, extraction_index_sha256=sha(extracted_bytes),
            extractor_sha256=extractor_sha, source_sha256=used, member_count=len(members), member_bytes=total,
            game_launched=False, native_res_extraction_only=True)))
    except BaseException as error:
        fresh(out/'FAILURE.json', canonical(dict(status='quarantined_partial_do_not_use', error=str(error))))
        raise
    print(str(out/'INTAKE-COMPLETE.json'))


if __name__ == '__main__':
    main()
