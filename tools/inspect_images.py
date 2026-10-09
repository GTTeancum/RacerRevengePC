"""Record PE/XBE headers without treating file size as runtime memory usage."""
import argparse
import hashlib
import json
from pathlib import Path
import struct

parser = argparse.ArgumentParser()
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('images', nargs='+', type=Path)
args = parser.parse_args()
records = []
for path in args.images:
    data = path.read_bytes()
    r = {'path': path.as_posix(), 'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
    if data[:4] == b'XBEH':
        base, headers, image, image_headers, timestamp = struct.unpack_from('<5I', data, 0x104)
        nsec, table = struct.unpack_from('<2I', data, 0x11c)
        r.update(format='XBE', image_base=base, header_bytes=headers, image_bytes=image,
                 section_count=nsec, execution_verified=False)
        assert table >= base and table - base + nsec * 56 <= len(data)
    elif data[:2] == b'MZ':
        off = struct.unpack_from('<I', data, 0x3c)[0]
        assert data[off:off+4] == b'PE\0\0'
        machine, count = struct.unpack_from('<HH', data, off+4)
        optsize = struct.unpack_from('<H', data, off+20)[0]
        optional = off + 24
        r.update(format='PE', machine=hex(machine), image_bytes=struct.unpack_from('<I', data, optional+56)[0])
        r['sections'] = []
        for i in range(count):
            pos = optional + optsize + i * 40
            name = data[pos:pos+8].rstrip(b'\0').decode('ascii')
            virtual, address, raw = struct.unpack_from('<III', data, pos+8)
            r['sections'].append({'name': name, 'virtual_bytes': virtual, 'raw_bytes': raw})
    else:
        raise ValueError(f'Unrecognized image: {path}')
    records.append(r)
with args.output.open('x', encoding='utf-8') as f:
    json.dump({'warning': 'Header sizes are not resident/peak working memory; interpret image content and execution evidence separately for each target.', 'images': records}, f, indent=2)
print(json.dumps(records, indent=2))
