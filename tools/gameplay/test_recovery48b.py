"""Offline malformed-input and provenance checks for gameplay recovery."""
import math
from pathlib import Path
import struct
import unittest
import zlib
import recover48b as r


def fixture(text=b'[Joystick]\nXAxisRateLimit=4\n', logical_offset=0):
    record = b'pods\\pod01\\Pod01.phy'
    index = struct.pack('<I', len(record)) + record + struct.pack('<II', logical_offset, len(text))
    payload = zlib.compress(text + bytes(r.BLOCK - len(text)))
    return (struct.pack('<5I', 3, 16, 0x340, 0, 4) + struct.pack('<2H', len(payload), 0) +
            struct.pack('<2I', 1, len(index)) + index + payload + b'\xff')


class RecoveryTests(unittest.TestCase):
    def test_positive_resource(self):
        text = b'[Joystick]\r\r\nXAxisRateLimit = 4\r\r\n'
        records, blocks, decoded = r.unpack_resource(fixture(text))
        self.assertEqual(decoded[:records[0]['bytes']], text)
        self.assertEqual(len(blocks), 1)
        self.assertEqual(records[0]['logical_offset'], 0)

    def test_truncated_resource(self):
        blob = fixture()
        for n in (0, 19, 23, 31, len(blob) - 1, len(blob) - 8):
            with self.subTest(length=n), self.assertRaises((ValueError, zlib.error)):
                r.unpack_resource(blob[:n])

    def test_wrong_header(self):
        for offset in (0, 8, 12, 16):
            data = bytearray(fixture()); data[offset] ^= 1
            with self.subTest(offset=offset), self.assertRaises(ValueError):
                r.unpack_resource(data)

    def test_no_container_suffix(self):
        with self.assertRaises(ValueError):
            r.unpack_resource(fixture() + b'junk')

    def test_record_out_of_virtual_bounds(self):
        with self.assertRaises(ValueError):
            r.unpack_resource(fixture(logical_offset=r.BLOCK))

    def test_boundary_marker(self):
        data = bytearray(fixture()); data[-1] = 0
        with self.assertRaises(ValueError):
            r.unpack_resource(data)

    def test_preserve_duplicates_and_values(self):
        p = r.parse_phy(b';comment\n[A]\nx=1\nx=2\n[B]\nx="three"\n')
        self.assertEqual([(e['section'], e['key'], e['raw_value']) for e in p['entries']],
                         [('A', 'x', '1'), ('A', 'x', '2'), ('B', 'x', '"three"')])

    def test_do_not_guess_phy_lines(self):
        for data in (b'x=1', b'[A]\nnot an assignment', b'[A]\n=2', b'[A]\nx=1\0'):
            with self.subTest(data=data), self.assertRaises(ValueError):
                r.parse_phy(data)

    def test_signed_conversion(self):
        cases = {0: 0, 0x80000000: 0, 0x3fc00000: 1, 0xbfc00000: -1,
                 0x4f000000: 2147483647, 0xcf000000: -2147483648,
                 0x7fc00000: 2147483647, 0xffc00000: -2147483648,
                 1: 0, 0x80000001: 0}
        for bits, expected in cases.items():
            with self.subTest(bits=hex(bits)):
                self.assertEqual(r.cvt_bits(bits), expected)

    def test_float_step_threshold(self):
        threshold = r.fbits(1 / 60)
        self.assertEqual(r.substep_formula(r.frombits(threshold))[0], 2)
        self.assertEqual(r.substep_formula(r.frombits(threshold - 1))[0], 1)
        self.assertEqual(r.substep_formula(0)[0], 1)

    def test_wrong_elf_rejected(self):
        with self.assertRaises(ValueError):
            r.Elf(b'not the original executable')

    def test_original_hash_and_substep_interval(self):
        root = Path(__file__).resolve().parents[2]
        elf = r.Elf((root / r.PROJECT / 'disc/SLUS_202.68').read_bytes())
        self.assertEqual(elf.string(0x3b2ca0), 'PodVehicle::RunSimulation')
        self.assertEqual(r.execute_substep_words(elf, r.float32(1 / 60))[0], 2)
        self.assertEqual(len(r.tuning_field_proof(elf)), 6)
        self.assertEqual(elf.file_offset(0x3b2ca0), 0x2b2d20)


if __name__ == '__main__':
    unittest.main(verbosity=2)
