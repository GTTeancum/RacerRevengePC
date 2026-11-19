# Find 32-bit word occurrences in SLUS_202.68 (virtual addresses). python -I -B findword.py 0x2702e0
import struct, sys
d = open(r'C:\Games\Star Wars Racer Revenge\SLUS_202.68', 'rb').read()
phoff = struct.unpack_from('<I', d, 0x1c)[0]; n = struct.unpack_from('<H', d, 0x2c)[0]
w = struct.pack('<I', int(sys.argv[1], 16))
for i in range(n):
    t, off, va, pa, fs, ms = struct.unpack_from('<6I', d, phoff + i * 32)
    seg = d[off:off + fs]; k = seg.find(w)
    while k >= 0:
        if k % 4 == 0: print(hex(va + k))
        k = seg.find(w, k + 1)
