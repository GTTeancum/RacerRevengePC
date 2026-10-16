#!/usr/bin/env python3
"""Authenticate GA intake against retained Full48F original-member identities.
Cold preparation only. No game, guest code, or renderer is launched.
The supplied extractor is built centrally from extract_ga_members.cpp.
"""
import argparse
import hashlib
import io
import json
import os
from pathlib import Path
import stat
import subprocess
import sys
import zipfile
sys.dont_write_bytecode = True
GA_SHA = '063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9'
FULL_SHA = '60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c'
GEN = '3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39'

def need(ok, why):
    if not ok:
        raise ValueError(why)

def sha(data):
    return hashlib.sha256(data).hexdigest()

def read(path, maximum):
    need(stat.S_ISREG(path.lstat().st_mode) and not any(p.is_symlink() for p in path.parents),
         'Expected nonsymlink regular input: '+str(path))
    need(path.stat().st_size <= maximum, 'Input byte bound: '+str(path))
    return path.read_bytes()

def safe(name):
    name = name.replace('\\', '/').lower()
    need(name and all(p not in ('', '.', '..') for p in name.split('/')) and
         all(c in 'abcdefghijklmnopqrstuvwxyz0123456789/_-.' for c in name), 'Unsafe member path')
    return name

def fresh(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as stream:
        stream.write(data)

def canonical(value):
    return (json.dumps(value, sort_keys=True, indent=2, allow_nan=False)+'\n').encode()

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, required=True)
    parser.add_argument('--full48f', type=Path, required=True)
    parser.add_argument('--extractor', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--execute-preparation', action='store_true', required=True)
    args = parser.parse_args()
    original = read(args.archive, 2834942)
    need(len(original) == 2834942 and sha(original) == GA_SHA, 'Exact original GA archive differs')
    retained = read(args.full48f, 300*1024*1024)
    need(sha(retained) == FULL_SHA, 'Retained Full48F archive differs')
    name = 'evidence/importFull48F-20261003/native-cache/generations/'+GEN+'.json'
    with zipfile.ZipFile(io.BytesIO(retained)) as archive:
        entries = [e for e in archive.infolist() if e.filename == name]
        need(len(entries) == 1 and not entries[0].is_dir() and entries[0].file_size <= 64*1024*1024,
             'Retained generation missing, duplicated or oversized')
        generation_bytes = archive.read(entries[0])
    need(sha(generation_bytes) == GEN, 'Retained generation differs')
    generation = json.loads(generation_bytes)
    containers = [r for r in generation['containers'] if r['name'] == 'TRACKS/GA.RES']
    need(len(containers) == 1 and containers[0]['sha256'] == GA_SHA and
         containers[0]['bytes'] == 2834942 and containers[0]['entries'] == 970, 'Retained GA container differs')
    members, paths = [], set()
    for row in generation['members']:
        if row['container'] != 'TRACKS/GA.RES':
            continue
        path = safe(row['member'])
        need(path not in paths and 0 <= row['source_bytes'] <= 8*1024*1024, 'Duplicate or oversized GA member')
        paths.add(path)
        members.append(dict(name=row['member'], path=path, bytes=row['source_bytes'],
                            logical_offset=row['logical_offset'], sha256=row['source_sha256']))
    need(len(members) == 970, 'Retained GA member count differs')
    for path in paths:
        parent = Path(path).parent
        while str(parent) != '.':
            need(parent.as_posix() not in paths, 'Member file/directory collision')
            parent = parent.parent
    extractor = args.extractor.resolve(strict=True)
    extractor_sha = sha(read(extractor, 128*1024*1024))
    out = args.output.absolute()
    need(not out.exists() and not out.is_symlink() and out.parent.is_dir() and
         not any(p.is_symlink() for p in out.parents), 'Output must be new beneath a real parent')
    need(os.name != 'nt' or (str(out).isascii() and str(extractor).isascii()), 'Windows extractor requires ASCII paths')
    out.mkdir()
    try:
        fresh(out/'original/GA.RES', original)
        with (out/'extraction.log').open('xb') as log:
            subprocess.run([str(extractor), str(out/'original/GA.RES'), str(out/'extracted')],
                           stdout=log, stderr=subprocess.STDOUT, timeout=1800, check=True)
        need(sha(read(extractor, 128*1024*1024)) == extractor_sha, 'Extractor changed during preparation')
        index_bytes = read(out/'extracted/extraction-index.json', 4*1024*1024)
        index = json.loads(index_bytes)
        need(index['schema'] == 'RR_ORIGINAL_GA_EXTRACT_V1' and len(index['members']) == 970,
             'Native GA extraction extent/schema differs')
        actuals = {}
        for i, row in enumerate(index['members']):
            need(row['source_index'] == i and row['file'] == str(i)+'.bin' and row['name'] not in actuals,
                 'Native extraction ordinal differs or duplicates a member')
            actuals[row['name']] = row
        verified, total = [], 0
        for expected in members:
            row = actuals.get(expected['name'])
            need(row is not None and row['bytes'] == expected['bytes'] and
                 row['logical_offset'] == expected['logical_offset'], 'Native member identity differs')
            data = read(out/'extracted'/row['file'], 8*1024*1024)
            total += len(data)
            need(total <= 128*1024*1024 and len(data) == expected['bytes'] and sha(data) == expected['sha256'],
                 'Native member bytes differ: '+expected['name'])
            verified.append((expected['path'], data))
        for path, data in verified:
            fresh(out/'intake/original-members'/path, data)
        manifest = canonical(dict(schema='RR_ORIGINAL_GA_RETAINED_INVENTORY_V1', archive_sha256=GA_SHA,
            source_archive_sha256=FULL_SHA, source_generation_sha256=GEN, members=members))
        fresh(out/'intake/archive-inventory.json', manifest)
        fresh(out/'INTAKE-COMPLETE.json', canonical(dict(status='authenticated_ga_intake',
            archive_sha256=GA_SHA, inventory_sha256=sha(manifest), source_generation_sha256=GEN,
            extractor_sha256=extractor_sha, extraction_index_sha256=sha(index_bytes),
            members=970, member_bytes=total, game_launched=False)))
    except BaseException as error:
        fresh(out/'FAILURE.json', canonical(dict(status='quarantined_partial_do_not_use', error=str(error))))
        raise
    print(str(out/'INTAKE-COMPLETE.json'))

if __name__ == '__main__':
    main()
