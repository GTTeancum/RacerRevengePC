import struct, sys, os
E = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'img', 'eprom.bin'), 'rb').read()
BASE = 0x0C00FC00
for t in sys.argv[1:]:
    v = int(t, 16); pat = struct.pack('<I', v)
    hits = []; i = E.find(pat, 0, 0x300000)
    while i >= 0:
        hits.append('%08X' % (i + BASE)); i = E.find(pat, i + 1, 0x300000)
    print(t, len(hits), hits[:40])
