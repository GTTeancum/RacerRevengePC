"""Read original assets/source to preserve evidence for the native-PC plan.

No game/tool binaries run; no original input files are modified.
"""
from pathlib import Path
import collections
import hashlib
import importlib.util
import json
import struct
import sys

sys.dont_write_bytecode = True

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'evidence/native-plan48A'


def digest(data):
    return hashlib.sha256(data).hexdigest()


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    decoder = ROOT / 'work/native/project/scripts/validate_ui_resources07.py'
    spec = importlib.util.spec_from_file_location('original_res_decoder', decoder)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    report = {'date': '2026-10-03', 'scope': 'read-only planning evidence; no native implementation',
              'decoder': str(decoder.relative_to(ROOT)), 'decoder_sha256': digest(decoder.read_bytes()),
              'containers': [], 'source_anchors': []}
    containers = ['TRACKS/TA.RES', 'PODS/POD01/POD01.RES', 'UI/PODPHYS.RES']
    for relative in containers:
        path = ROOT / 'work/native/project/disc' / relative
        blob = path.read_bytes()
        records, blocks = module.decode(blob)
        logical = b''.join(b['data'] for b in blocks)
        formats = collections.Counter()
        members = []
        for record in records:
            start, size = record['logical_offset'], record['bytes']
            data = logical[start:start + size]
            item = dict(record, sha256=digest(data))
            if record['name'].lower().endswith('.psg'):
                assert len(data) >= 8
                item['magic_hex'] = data[:4].hex()
                item['version_le32'] = struct.unpack_from('<I', data, 4)[0]
                formats[(item['magic_hex'], item['version_le32'])] += 1
            if record['name'].replace('\\', '/').lower() in {
                    'geometry/zone01groundstrips16.psg', 'geometry/ta_bigrocka.psg',
                    'textures/ta_sand8.psm', 'textures/ta_sand8.pst',
                    'tracks/ta.tob', 'tracks/ta.scb', 'geometry/zone9_col_floor.col',
                    'pods/pod01/englt1b.psg', 'pods/pod01/englt1.psg',
                    'pods/pod01/pod01.phy'}:
                # Basenames here come from the fixed allowlist above, never arbitrary archive paths.
                sample = OUT / (path.stem + '-' + record['name'].replace('\\', '/').split('/')[-1])
                sample.write_bytes(data)
                item['evidence_sample'] = sample.relative_to(ROOT).as_posix()
                if sample.suffix == '.psm':
                    item['material_text'] = data.decode('ascii')
            members.append(item)
        report['containers'].append({'path': path.relative_to(ROOT).as_posix(),
            'bytes': len(blob), 'sha256': digest(blob), 'members': members,
            'psg_headers': [{'magic_hex': key[0], 'version': key[1], 'count': count}
                            for key, count in sorted(formats.items())]})
    for name, spans in [
            ('FUN_00219810_0x219810.cpp', [(536, 590), (640, 680), (855, 885), (3590, 3820), (3960, 4010)]),
            ('entry_001a8060_0x1a8060.cpp', [(1320, 1465), (1500, 1670), (1770, 1825)]),
            ('FUN_001a0570_0x1a0570.cpp', [(82, 160), (800, 845)]),
            ('FUN_001af330_0x1af330.cpp', [(429, 510)])]:
        path = ROOT / 'work/native/project/generated' / name
        data = path.read_bytes()
        lines = data.decode('utf-8').splitlines()
        evidence = OUT / (name + '.excerpts.txt')
        chunks = [f'{i}: {lines[i-1]}' for begin, end in spans
                  for i in range(begin, min(end, len(lines)) + 1)]
        evidence.write_text('\n'.join(chunks) + '\n', encoding='utf-8')
        report['source_anchors'].append({'path': path.relative_to(ROOT).as_posix(),
            'sha256': digest(data), 'lines': len(lines), 'spans': spans,
            'excerpt': evidence.relative_to(ROOT).as_posix()})
    elf = ROOT / 'work/native/project/disc/SLUS_202.68'
    data = elf.read_bytes()
    marker = b'PodVehicle::RunSimulation\x00'
    locations = []
    cursor = 0
    while (offset := data.find(marker, cursor)) >= 0:
        locations.append(offset)
        cursor = offset + len(marker)
    assert data[:6] == b'\x7fELF\x01\x01', 'Expected original ELF32 little endian'
    phoff = struct.unpack_from('<I', data, 28)[0]
    phsize, phnum = struct.unpack_from('<HH', data, 42)
    mappings = []
    for index in range(phnum):
        typ, off, va, _, size = struct.unpack_from('<IIIII', data, phoff + index * phsize)
        if typ == 1:
            for location in locations:
                if off <= location < off + size:
                    mappings.append({'file_offset': location, 'virtual_address': hex(va + location - off)})
    report['elf'] = {'path': elf.relative_to(ROOT).as_posix(), 'sha256': digest(data),
                     'run_simulation_string_file_offsets': locations, 'load_mappings': mappings,
                     'note': 'String/function identity is static evidence; live semantics require tracing.'}
    (OUT / 'assessment.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'containers': [{'path': c['path'], 'sha256': c['sha256'],
        'members': len(c['members']), 'psg_headers': c['psg_headers']} for c in report['containers']],
        'source_anchors': len(report['source_anchors']), 'elf_string_file_offsets': locations}, indent=2))


if __name__ == '__main__':
    main()
