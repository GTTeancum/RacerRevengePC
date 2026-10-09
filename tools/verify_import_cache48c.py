"""Independently rederive every imported mesh from original bytes with frozen Python decoding."""
from pathlib import Path
import argparse
import hashlib
import json
import sys

ROOT = Path(__file__).resolve().parents[1]
if not __debug__:
    raise RuntimeError('Cache verification requires normal Python execution')
sys.dont_write_bytecode = True
sys.path.insert(0, str(ROOT/'tools/assets'))
from decode_static48b import decode_psg3, mesh_bytes
sys.path.insert(0, str(ROOT/'work/native/project/scripts'))
from validate_ui_resources07 import decode


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--cache', type=Path, required=True)
    ap.add_argument('--source', type=Path, required=True)
    ap.add_argument('--output', type=Path, required=True)
    args = ap.parse_args()
    current = json.loads((args.cache/'current.json').read_text())
    path = args.cache/'generations'/(current['generation']+'.json')
    assert sha(path.read_bytes()) == current['manifest_sha256'] == current['generation']
    manifest = json.loads(path.read_text())
    by_container = {}
    for member in manifest['members']:
        by_container.setdefault(member['container'], []).append(member)
    checked, reused_derivations = [], {}
    total_members = 0
    for container in manifest['containers']:
        raw = (args.source/container['name']).read_bytes()
        assert sha(raw) == container['sha256']
        if container['status'] != 'decoded':
            continue
        rows, blocks = decode(raw)
        flat = b''.join(b['data'] for b in blocks)
        indexed = {(r['name'],r['logical_offset'],r['bytes']) for r in rows}
        for member in by_container[container['name']]:
            key = (member['member'], member['logical_offset'], member['source_bytes'])
            assert key in indexed
            data = flat[key[1]:key[1]+key[2]]
            assert sha(data) == member['source_sha256']
            total_members += 1
            if member['status'] != 'converted_geometry':
                continue
            record = member['native']
            fields = dict(record); digest = fields.pop('record_sha256')
            assert sha(json.dumps(fields,sort_keys=True,separators=(',',':'),ensure_ascii=False).encode()) == digest
            if member['source_sha256'] not in reused_derivations:
                vertices, indices, report = decode_psg3(data)
                expected = mesh_bytes(vertices, indices)
                reused_derivations[member['source_sha256']] = (expected, report)
            expected, report = reused_derivations[member['source_sha256']]
            cached = (args.cache/'objects'/(record['key']+'.rrmesh')).read_bytes()
            assert cached == expected and sha(cached) == record['mesh_sha256']
            assert record['world_space'] == bool(report['quantization'][7])
            assert record['material'] == report['material']
            checked.append(dict(container=container['name'], member=member['member'],
                                source_sha256=sha(data), mesh_sha256=sha(cached)))
    result = dict(passed=True, scope='byte/metadata equivalence to independently executed frozen48B decoder; not material fidelity',
        current_generation=current['generation'], containers=len(manifest['containers']), original_members_verified=total_members,
        imported_members_byte_identical=len(checked), unique_original_derivations=len(reused_derivations),
        original_source=str(args.source.resolve()), cache=str(args.cache.resolve()), meshes=checked)
    args.output.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='meshes'}))


if __name__ == '__main__':
    main()
