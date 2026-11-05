import struct
import unittest

from trace_water_topology import ee_instruction, file_bytes_at, inspect


def elf_fixture():
    data = bytearray(128)
    data[:7] = b'\x7fELF\x01\x01\x01'
    struct.pack_into('<H', data, 18, 8)
    struct.pack_into('<I', data, 28, 52)
    struct.pack_into('<HH', data, 42, 32, 1)
    struct.pack_into('<8I', data, 52, 1, 96, 0x1000, 0, 16, 32, 5, 16)
    data[96:112] = bytes(range(16))
    return data


class TraceTests(unittest.TestCase):
    def test_maps_segment_instead_of_assuming_retail_offset(self):
        self.assertEqual(file_bytes_at(elf_fixture(), 0x1004, 4), b'\x04\x05\x06\x07')

    def test_does_not_fabricate_bss_bytes(self):
        with self.assertRaises(ValueError):
            file_bytes_at(elf_fixture(), 0x1010, 4)

    def test_rejects_truncated_segment(self):
        with self.assertRaises(ValueError):
            file_bytes_at(elf_fixture()[:104], 0x1000, 4)

    def test_rejects_wrong_machine(self):
        data = elf_fixture()
        struct.pack_into('<H', data, 18, 3)
        with self.assertRaises(ValueError):
            file_bytes_at(data, 0x1000, 4)

    def test_rejects_wrong_original_identity(self):
        with self.assertRaisesRegex(ValueError, 'SHA256'):
            inspect(elf_fixture())

    def test_ee_instructions_are_not_modern_mips_dsp(self):
        self.assertEqual(ee_instruction(0x7FBF0040), 'sq $ra, 0x40($sp)')
        self.assertEqual(ee_instruction(0x7BBF0040), 'lq $ra, 0x40($sp)')
        self.assertEqual(ee_instruction(0x70809E28), 'por $s3, $a0, $zero')
        self.assertEqual(ee_instruction(0x70000000), 'EE MMI (not decoded)')
        self.assertEqual(ee_instruction(0xD8E40000), 'lqc2 $vf4, 0x0($a3)')
        self.assertEqual(ee_instruction(0xF8660000), 'sqc2 $vf6, 0x0($v1)')
        self.assertEqual(ee_instruction(0xD8E4FFF0), 'lqc2 $vf4, -0x10($a3)')
        self.assertEqual(ee_instruction(0x4BC4216A), 'EE COP2 (not decoded)')
        self.assertIsNone(ee_instruction(0x0C0C15C0))


if __name__ == '__main__':
    unittest.main()
