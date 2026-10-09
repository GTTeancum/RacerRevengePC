"""Independently replay frozen 48H inputs without opening the full RAM file."""
import json
from query_extract48h import OUT, digest
from query_probe48h import execute


def main():
    metadata = json.loads((OUT / 'saved-query-data.json').read_text())
    raw = (OUT / 'saved-query-data.bin').read_bytes()
    assert digest(raw) == metadata['raw_sha256']
    data = bytearray(0x2000000)
    allowed = set()
    for span in metadata['slices']:
        start, size, offset = span['address'], span['bytes'], span['file_offset']
        piece = raw[offset:offset + size]
        assert len(piece) == size and digest(piece) == span['sha256']
        assert not allowed.intersection(range(start, start + size))
        data[start:start + size] = piece
        allowed.update(range(start, start + size))
    proof = json.loads((OUT / 'saved-query-replay.json').read_text())
    for expected in proof['runs']:
        candidate = int(expected['candidate'], 16) if expected['candidate'] else None
        _, result = execute(data, int(expected['entry'], 16), candidate, allowed)
        assert result == expected
    report = dict(passed=True, runs=len(proof['runs']), bytes=len(raw),
                  slices=len(metadata['slices']), full_ram_opened=False,
                  data_sha256=digest(raw))
    (OUT / 'independent-sparse-verification.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report))


if __name__ == '__main__':
    main()
