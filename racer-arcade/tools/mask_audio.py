# SPSD stream helpers for mask audio banks (0x05000000-0x0AEE15EC).
# SPSD header 0x40 bytes: "SPSD" 01 01 00 04, +0x08 codec (1 = 8-bit PCM, 3 = AICA 4-bit ADPCM),
# +0x0C data bytes (LE32), +0x2A sample rate (LE16, 44100). Data follows the header.
#   python -I tools/mask_audio.py list
#   python -I tools/mask_audio.py wav <spsd_off> <seconds> out.wav      (decode start of a stream to WAV)
#   python -I tools/mask_audio.py spec <spsd_off> <seconds> out.png     (spectrogram, to eyeball)
import sys, os, struct, wave, numpy as np
MASK = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'img', 'mask.bin')
SCALE = [0x0E6, 0x0E6, 0x0E6, 0x0E6, 0x133, 0x199, 0x200, 0x266]
def adpcm(b):
    out = np.empty(b.size*2, np.int16); hist, step = 0, 0x7F; k = 0
    for byte in b.tolist():
        for n in (byte & 15, byte >> 4):
            d = n & 7; diff = ((1 + d*2) * step) >> 3
            hist = max(-32768, min(32767, hist - diff if n & 8 else hist + diff))
            step = max(0x7F, min(0x6000, (step * SCALE[d]) >> 8)); out[k] = hist; k += 1
    return out
def read(off, secs):
    with open(MASK, 'rb') as f:
        f.seek(off); h = f.read(0x40); codec = h[8]; rate = struct.unpack('<H', h[0x2A:0x2C])[0]
        n = int(rate*secs) // (2 if codec == 3 else 1)
        b = np.frombuffer(f.read(n), np.uint8)
    s = adpcm(b) if codec == 3 else (b.view(np.int8).astype(np.int16) << 8)
    return s, rate, codec
if __name__ == '__main__':
    if sys.argv[1] == 'list':
        data = open(MASK, 'rb').read(); i = data.find(b'SPSD')
        while i >= 0:
            sz = struct.unpack('<I', data[i+12:i+16])[0]; c = data[i+8]
            print('%08X codec %d bytes %X (%.1f s)' % (i, c, sz, sz*(2 if c == 3 else 1)/44100)); i = data.find(b'SPSD', i+4)
    elif sys.argv[1] == 'wav':
        s, rate, _ = read(int(sys.argv[2], 0), float(sys.argv[3]))
        w = wave.open(sys.argv[4], 'wb'); w.setnchannels(1); w.setsampwidth(2); w.setframerate(rate); w.writeframes(s.tobytes()); w.close()
    elif sys.argv[1] == 'spec':
        from PIL import Image
        s, rate, codec = read(int(sys.argv[2], 0), float(sys.argv[3]))
        N = 1024; fr = s[:s.size//N*N].reshape(-1, N).astype(float) * np.hanning(N)
        sp = 20*np.log10(np.abs(np.fft.rfft(fr, axis=1)) + 1e-3); sp = sp[:, :N//2].T[::-1]
        sp = np.clip((sp - sp.max() + 90) / 90 * 255, 0, 255).astype(np.uint8)
        Image.fromarray(sp).resize((min(800, sp.shape[1]*2), 256)).save(sys.argv[4])
