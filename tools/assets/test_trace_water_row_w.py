import unittest
from trace_water_row_w import ADD_ZERO_W, SUB_SELF_W, analyze, block, inspect

#Use the actual memory and masked arithmetic encodings. XYZ arithmetic is
#irrelevant here; these fixtures exercise which W source each operation uses.
MAIN = [0xD8E40000, ADD_ZERO_W, 0xF8E60000,
        0xD8C40000, SUB_SELF_W, 0xF8660000,
        0xD8640000, ADD_ZERO_W, SUB_SELF_W, 0xF8E60000]
TAIL = [0xD8C40000, 0xD8E50000, SUB_SELF_W, 0xF8660000,
        0xD8640000, ADD_ZERO_W, SUB_SELF_W, 0xF8E60000]


class WKnownnessTests(unittest.TestCase):
    def test_unknown_self_subtraction_is_not_invented_zero(self):
        registers = {4: None, 5: None, 6: None}
        block([SUB_SELF_W], registers, {})
        self.assertIsNone(registers[6])
        registers[6] = 1
        block([SUB_SELF_W], registers, {})
        self.assertEqual(registers[6], 0)

    def test_two_entries_preserve_unknown_entry_register(self):
        self.assertEqual(analyze(2, MAIN, [TAIL, TAIL]), [None, None])

    def test_main_loop_establishes_w_despite_unknown_normal_padding(self):
        for count in (3, 4, 17, 16384):
            self.assertEqual(analyze(count, MAIN, [TAIL, TAIL]), [0] * count)

    def test_refuses_unmodeled_words_counts_and_wrong_elf(self):
        for count in (0, 1, 16385):
            with self.assertRaises(ValueError):
                analyze(count, MAIN, [TAIL, TAIL])
        for word in (0, 0x4A000000, 0xD8E40004, 0xD8E60000):
            with self.assertRaises(ValueError):
                block([word], {4: None, 5: None, 6: None}, {'output': 0})
        with self.assertRaisesRegex(ValueError, 'SHA256'):
            inspect(bytes(64))


if __name__ == '__main__':
    unittest.main()
