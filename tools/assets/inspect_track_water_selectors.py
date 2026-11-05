"""Read-only inventory of installed track scene WaterFile selectors and pool members.

No generated asset bytes are published; reports stay in private repo evidence.
This reads archive directories and only decompresses the selected scene blocks.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys
import zlib
sys.dont_write_bytecode = True

ROOT = Path(__file__).resolve().parents[2]
BLOCK = 24576


def archive(blob):
    def u32(at):
        if at < 0 or at + 4 > len(blob):
            raise ValueError('truncated archive integer')
        return struct.unpack_from('<I', blob, at)[0]
    if (u32(0), u32(8), u32(12)) != (3, 0x340, 0):
        raise ValueError('unsupported archive header')
    table_bytes = u32(16)
    if not 2 <= table_bytes <= 16384 or table_bytes % 2 or 20 + table_bytes != 8 + u32(4):
        raise ValueError('invalid block table extent')
    sizes = struct.unpack_from('<' + 'H' * (table_bytes // 2), blob, 20)
    if sizes[-1] or not all(0 < size <= BLOCK for size in sizes[:-1]):
        raise ValueError('invalid block sizes/sentinel')
    at = 8 + u32(4); count, index_bytes = u32(at), u32(at+4); at += 8
    end = at + index_bytes
    if not 0 < count < 100000 or end > len(blob):
        raise ValueError('invalid directory extent')
    entries = []
    for _ in range(count):
        length = u32(at); at += 4
        if not 0 < length <= 1024 or at + length + 8 > end:
            raise ValueError('invalid member name extent')
        name = blob[at:at+length].decode('ascii');at += length
        start, extent = u32(at), u32(at+4);at += 8
        if '\0' in name or start + extent > (len(sizes)-1)*BLOCK:
            raise ValueError('invalid logical member extent')
        entries.append((name, start, extent))
    if at != end:
        raise ValueError('directory suffix')
    offsets = []
    for size in sizes[:-1]:
        if at + size >= len(blob) or blob[at+size] != 255:
            raise ValueError('invalid block marker/extent')
        offsets.append((at, size));at += size+1
    if at != len(blob):
        raise ValueError('archive suffix')
    return entries, offsets


def member(blob, offsets, start, length):
    output = bytearray()
    while length:
        block, within = divmod(start, BLOCK)
        at, size = offsets[block]
        encoded = blob[at:at+size]
        if size == BLOCK:
            data = encoded
        else:
            decoder = zlib.decompressobj();data = decoder.decompress(encoded, BLOCK+1)
            if not decoder.eof or decoder.unused_data or decoder.unconsumed_tail or len(data) != BLOCK:
                raise ValueError('invalid decompressed block')
        take = min(length, BLOCK-within)
        output.extend(data[within:within+take]);start += take;length -= take
    return bytes(output)


def environment_sections(data):
    #Read every label in each complete Environment block. Other scene blocks
    #may legally contain repeated keys; this inventory does not normalize them.
    def u32(at):
        if at < 0 or at+4 > len(data):
            raise ValueError('truncated scene integer')
        return struct.unpack_from('<I', data, at)[0]
    def text(at, length):
        if length <= 0 or at < 0 or at+length > len(data):
            raise ValueError('scene string extent')
        raw = data[at:at+length]
        if b'\0' not in raw:
            raise ValueError('scene string terminator')
        head, suffix = raw.split(b'\0', 1)
        if any(suffix):
            raise ValueError('scene string suffix')
        return head.decode('ascii')
    if data[:7] != b'BINARY\0' or u32(7) != 1:
        raise ValueError('unsupported scene header')
    count = u32(11)
    if not 0 < count <= 8192:
        raise ValueError('scene block count')
    at = 15; names = []
    for _ in range(count):
        length = u32(at);at += 4;names.append(text(at, length));at += length
    if at+8*count > len(data) or len(set(names)) != count:
        raise ValueError('scene block table')
    result = {}
    for i, name in enumerate(names):
        start, size = u32(at+i*8), u32(at+i*8+4)
        if start < at+8*count or start+4+size > len(data):
            raise ValueError('scene block extent')
        if 'environment' not in name.lower():
            continue
        number = u32(start);base = start+4
        if not 0 < number <= 1024 or number*43 > size:
            raise ValueError('environment label table')
        fields = []
        for j in range(number):
            entry = base+j*43;key = text(entry, 35)
            offset, length = u32(entry+35), u32(entry+39)
            if offset < number*43 or offset+length > size:
                raise ValueError('environment value extent')
            fields.append(dict(name=key, value=text(base+offset, length),
                               entry_offset=entry, value_offset=base+offset, value_bytes=length))
        result[name] = dict(offset=start, size_without_count=size, fields=fields)
    if not result:
        raise ValueError('no Environment block found')
    return result


def inspect(path, scene):
    blob = path.read_bytes();entries, offsets = archive(blob)
    report = dict(archive=path.name, archiveSha256=hashlib.sha256(blob).hexdigest(),
                  memberCount=len(entries), slxMembers=[n for n, _, _ in entries if n.lower().endswith('.slx')])
    if scene is None:
        return report
    matches = [e for e in entries if e[0].lower() == scene]
    if len(matches) != 1:
        raise ValueError('scene must occur exactly once')
    name, start, length = matches[0];data = member(blob, offsets, start, length)
    environments = environment_sections(data)
    report.update(scene=name, sceneSha256=hashlib.sha256(data).hexdigest(), sceneBytes=len(data),
                  environmentSections={key: [f for f in section['fields'] if 'water' in f['name']]
                                       for key, section in environments.items()},
                  environmentWaterFields=[dict(section=key, **f) for key, section in environments.items()
                                  for f in section['fields'] if 'water' in f['name']])
    selectors = [field['value'] for field in report['environmentWaterFields'] if field['name'].lower() == 'waterfile']
    configs = []
    for selector in selectors:
        wanted = 'tracks\\'+selector.lower()
        found = [entry for entry in entries if entry[0].lower() == wanted]
        if len(found) != 1:
            raise ValueError('selected WaterFile must occur exactly once')
        config_name, config_start, config_length = found[0]
        config = member(blob, offsets, config_start, config_length)
        section = ''; assignments = []
        for number, line in enumerate(config.decode('ascii').splitlines(), 1):
            line = line.strip()
            if not line or line.startswith(';'):
                continue
            if line.startswith('[') and line.endswith(']'):
                section = line[1:-1].strip().lower()
            elif section == 'general information' and '=' in line:
                key, value = line.split('=', 1)
                assignments.append(dict(key=key.strip().lower(), value=value.strip(), line=number))
        configs.append(dict(name=config_name, sha256=hashlib.sha256(config).hexdigest(),
                            bytes=len(config), generalAssignments=assignments))
    report['waterConfigs'] = configs
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game-root', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args();output = args.output.resolve();evidence = (ROOT/'evidence').resolve()
    if output == evidence or not output.is_relative_to(evidence):
        parser.error('output must be a fresh repository evidence subdirectory')
    reports = [inspect(p, 'tracks\\'+p.stem.lower()+'.scb') for p in sorted((args.game_root/'TRACKS').glob('??.RES'))]
    reports += [inspect(args.game_root/'APPSTART.RES', None)]
    output.mkdir(parents=True, exist_ok=False)
    (output/'inventory.json').write_text(json.dumps(reports, indent=2)+'\n', encoding='utf-8')
    for report in reports:
        print(report['archive'], 'SLX:', len(report['slxMembers']),
              'environments:', report.get('environmentSections'), 'water fields:', len(report.get('environmentWaterFields', [])))


if __name__ == '__main__':
    main()
