# Print 32-bit words of SLUS_202.68 at virtual addresses. python -I -B vword.py 0x3d52b0 [n]
import struct, sys
d = open(r'C:\Games\Star Wars Racer Revenge\SLUS_202.68', 'rb').read()
phoff = struct.unpack_from('<I', d, 0x1c)[0]; n = struct.unpack_from('<H', d, 0x2c)[0]
a = int(sys.argv[1], 16); cnt = int(sys.argv[2]) if len(sys.argv) > 2 else 1
for i in range(n):
    t, off, va, pa, fs, ms = struct.unpack_from('<6I', d, phoff + i * 32)
    if va <= a < va + fs:
        for k in range(cnt):
            print(hex(a + 4 * k), hex(struct.unpack_from('<I', d, off + a - va + 4 * k)[0]))
