#!/usr/bin/env python3
"""Bridge exact original RA/RC through the bounded native extraction-only helper.
Source-only candidate. Builds nothing; launches only the explicitly supplied helper.
RA/RC are exact source codes; this does not choose a course or claim gameplay.
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
import hashlib
import stat


def need(ok, why):
    if not ok:
        raise ValueError(why)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def safe(name):
    need(name and "\\" not in name and "\0" not in name and
         all(part not in ('', '.', '..') and ':' not in part for part in name.split('/')),
         'Unsafe retained original member path: '+repr(name))
    return Path(name)


def regular(path):
    need(stat.S_ISREG(path.lstat().st_mode) and not path.is_symlink() and
         not any(parent.is_symlink() for parent in path.parents),
         'Expected nonsymlink regular file: '+str(path))


PROFILES = {
    'RA': dict(archive_bytes=4604561, archive_entries=1010,
               archive_sha256='1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2',
               reference_sha256='42520c6334dfa53b79f75ae2db855432d23b49a82fc524301f3c717a20d3924d'),
    'RC': dict(archive_bytes=4655141, archive_entries=1121,
               archive_sha256='ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b',
               reference_sha256='01ef7eb45385a33a98a1e265108c7c52e1deecda40ddeccb1ceca0f81252a1ba'),
}
# Root must review and pin the new bridge/helper as part of its source packet.
# These are the existing native decoder and JSON dependencies, not gameplay.
DEPENDENCIES = {
    'src/assets/import_decode48c.cpp': '99feffba580821d63187dd12f7a61ed1685b9c9ded0bf962c120f1f4cb5c53f2',
    'src/assets/import_decode48c.h': 'b334a60a0a5794cdb3ac5e7492e06045f93b40927ce3b6cecfbf7291abde2230',
    'third_party/native-json/json.hpp': '9bea4c8066ef4a1c206b2be5a36302f8926f7fdc6087af5d20b417d0cf103ea6',
}

FULL_SHA = '60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c'
GENERATION = '3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39'
GENERATION_PATH = 'evidence/importFull48F-20261003/native-cache/generations/'+GENERATION+'.json'



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
    parser.add_argument('--track', choices=tuple(PROFILES), required=True)
    parser.add_argument('--extractor-sha256', required=True, help='Root-reviewed native helper binary identity')
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--archive', type=Path, required=True, help='Exact original RA/RC.RES')
    parser.add_argument('--full48f', type=Path, required=True)
    parser.add_argument('--extractor', type=Path, required=True,
                        help='Root-built extract_ry_members from this reviewed source packet')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--execute-preparation', action='store_true', required=True)
    args = parser.parse_args()
    os.umask(0o077)
    repo = args.repo.resolve(strict=True)
    tool_root = Path(__file__).resolve().parent
    track = args.track
    cfg = PROFILES[track]
    reference_path = 'evidence/worldImport48F/'+track.lower()+'-tracks_'+track.lower()+'.reference.json'
    used = {}
    for relative, digest in DEPENDENCIES.items():
        path = repo/safe(relative)
        need(sha(read_file(path, 16*1024*1024)) == digest,
             'Native extraction dependency differs: '+str(path))
        used[str(path)] = digest
    for path in (Path(__file__).resolve(), tool_root/'extract_ry_members.cpp'):
        used[str(path)] = sha(read_file(path, 16*1024*1024))
    archive_bytes = read_file(args.archive, cfg['archive_bytes'])
    need(len(archive_bytes) == cfg['archive_bytes'] and sha(archive_bytes) == cfg['archive_sha256'],
         'Exact original RA/RC archive differs')
    full = read_file(args.full48f, 300*1024*1024)
    need(sha(full) == FULL_SHA, 'Exact retained Full48F archive differs')
    # Read from the authenticated in-memory bytes, not a reopened mutable path.
    import io
    with zipfile.ZipFile(io.BytesIO(full)) as archive:
        generation_bytes = unique_zip_member(archive, GENERATION_PATH, 64*1024*1024)
        need(sha(generation_bytes) == GENERATION, 'Retained generation differs')
        generation = json.loads(generation_bytes)
        reference = unique_zip_member(archive, reference_path, 8*1024*1024)
    need(sha(reference) == cfg['reference_sha256'], 'Retained RA/RC world reference differs')
    containers = [row for row in generation['containers'] if row['name'] == 'TRACKS/'+track+'.RES']
    need(len(containers) == 1 and containers[0]['bytes'] == cfg['archive_bytes'] and
         containers[0]['sha256'] == cfg['archive_sha256'] and
         containers[0]['entries'] == cfg['archive_entries'], 'Retained RA/RC container differs')
    members = []
    seen = set()
    for row in generation['members']:
        if row['container'] != 'TRACKS/'+track+'.RES':
            continue
        name = row['member']
        path = name.replace('\\', '/')
        safe(path)
        need(path.casefold() not in seen, 'Duplicate retained RA/RC member')
        seen.add(path.casefold())
        members.append(dict(name=name, path=path, bytes=row['source_bytes'],
                            logical_offset=row['logical_offset'], sha256=row['source_sha256'],
                            **{'48F_status': row['status']}))
    need(len(members) == cfg['archive_entries'], 'Retained RA/RC member count differs')
    for row in members:
        parent = Path(row['path']).parent
        while str(parent) != '.':
            need(parent.as_posix().casefold() not in seen, 'Retained file/directory collision')
            parent = parent.parent
    manifest = canonical(dict(schema='RR_ORIGINAL_RYLOTH_MEMBER_INVENTORY_V1', track=track,
        archive_sha256=cfg['archive_sha256'], archive_bytes=cfg['archive_bytes'],
        source_archive_sha256=FULL_SHA,
        source_generation_sha256=GENERATION, members=members))
    # Manifest is derived only from the hash-pinned retained generation above.
    extractor_bytes = read_file(args.extractor, 128*1024*1024)
    extractor_sha = sha(extractor_bytes)
    need(extractor_sha == args.extractor_sha256, 'Root-reviewed native extractor identity differs')
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
        fresh(out/'original'/(track+'.RES'), archive_bytes)
        with (out/'extraction.log').open('xb') as log:
            subprocess.run([str(extractor), track, str(out/'original'/(track+'.RES')), str(out/'extracted')],
                           stdout=log, stderr=subprocess.STDOUT, timeout=1800, check=True)
        need(sha(read_file(extractor, 128*1024*1024)) == extractor_sha, 'Extractor changed during preparation')
        extracted_bytes = read_file(out/'extracted/extraction-index.json', 4*1024*1024)
        extracted = json.loads(extracted_bytes)
        need(extracted['schema'] == 'RR_ORIGINAL_RYLOTH_EXTRACT_V1' and extracted['track'] == track, 'Native extraction schema differs')
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
                 sha(data) == expected['sha256'], 'Original extracted RA/RC member differs: '+expected['name'])
            authenticated.append((expected['path'], data))
        for name, data in authenticated:
            fresh(out/'members'/safe(name), data)
        for path, digest in used.items():
            need(sha(read_file(Path(path), 16*1024*1024)) == digest, 'Preparation source changed: '+path)
        fresh(out/'manifest.json', manifest)
        fresh(out/'retained48F-world.reference.json', reference)
        fresh(out/'INTAKE-COMPLETE.json', canonical(dict(status='authenticated_ryloth_intake_v1', track=track,
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
