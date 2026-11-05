import struct
import unittest
import zlib
from inspect_track_water_selectors import BLOCK, archive, environment_sections, member


def scene(fields):
    blocks = []
    for entries in (fields, [('duplicate', 'a'), ('duplicate', 'b')]):
        values = bytearray(); table = bytearray()
        for key, value in entries:
            raw = value.encode()+b'\0'
            table.extend(key.encode().ljust(35, b'\0'))
            table.extend(struct.pack('<II', len(entries)*43+len(values), len(raw)))
            values.extend(raw)
        blocks.append(struct.pack('<I', len(entries))+table+values)
    header = bytearray(b'BINARY\0'+struct.pack('<II', 1, 2))
    for name in (b'environment\0', b'zone1\0'):
        header.extend(struct.pack('<I', len(name))+name)
    offset = len(header)+16
    for block in blocks:
        header.extend(struct.pack('<II', offset, len(block)-4));offset += len(block)
    return bytes(header)+b''.join(blocks)


def container(raw, compressed):
    data = raw.ljust(BLOCK, b'\0');encoded = zlib.compress(data) if compressed else data
    name = b'tracks\\test.scb'
    index = struct.pack('<I', len(name))+name+struct.pack('<II', 0, len(raw))
    return (struct.pack('<5I', 3, 16, 0x340, 0, 4)+struct.pack('<HH', len(encoded), 0)+
            struct.pack('<II', 1, len(index))+index+encoded+b'\xff')


class SelectorTests(unittest.TestCase):
    def test_complete_environment_with_duplicate_keys_elsewhere(self):
        result = environment_sections(scene([('skyfile', 'gc'), ('waterfile', 'gc.wtt')]))
        self.assertEqual([f['value'] for f in result['environment']['fields'] if f['name']=='waterfile'], ['gc.wtt'])
        absent = environment_sections(scene([('skyfile', 'ga')]))
        self.assertFalse(any(f['name']=='waterfile' for f in absent['environment']['fields']))

    def test_retains_duplicate_selector_evidence(self):
        result = environment_sections(scene([('waterfile', 'a'), ('waterfile', 'b')]))
        self.assertEqual([f['value'] for f in result['environment']['fields']], ['a', 'b'])

    def test_compressed_and_raw_archive_blocks(self):
        raw = scene([('waterfile', 'gc.wtt')])
        for compressed in (False, True):
            blob = container(raw, compressed);entries, offsets = archive(blob)
            self.assertEqual(member(blob, offsets, entries[0][1], entries[0][2]), raw)
            with self.assertRaises(ValueError):
                archive(blob[:-1]+b'\x00')
            with self.assertRaises(ValueError):
                archive(blob+b'\x00')

    def test_truncated_scene_and_outside_value_refuse(self):
        data = scene([('waterfile', 'gc.wtt')])
        with self.assertRaises(ValueError):
            environment_sections(data[:-1])
        result = environment_sections(data)
        entry = result['environment']['fields'][0]['entry_offset']
        corrupt = bytearray(data);struct.pack_into('<I', corrupt, entry+35, 0xffffffff)
        with self.assertRaises(ValueError):
            environment_sections(corrupt)


if __name__ == '__main__':
    unittest.main()
