using System;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

// Presents ISO9660's 2048-byte logical blocks regardless of the CD sector framing.
sealed class DiscImage : IDisposable {
    const int Block = 2048, CacheSectors = 64;
    readonly FileStream file;
    readonly byte[] cache = new byte[CacheSectors * 2352];
    long first, cached = -1;
    int stride, payload;
    public long Length { get; private set; }

    static readonly Regex CueFile = new Regex("^FILE\\s+(?:\"([^\"]+)\"|([^\"\\s]+))\\s+BINARY$", RegexOptions.IgnoreCase);
    static readonly Regex CueTrack = new Regex(@"^TRACK\s+\d+\s+(MODE[12]/(?:2048|2336|2352))$", RegexOptions.IgnoreCase);
    static readonly Regex CueIndex = new Regex(@"^INDEX\s+01\s+(\d{1,3}):([0-5]\d):([0-7]\d)$", RegexOptions.IgnoreCase);

    static void ReadCue(string cue, out string path, out string mode, out long start) {
        path = null; mode = null; start = -1;
        if (new FileInfo(cue).Length > 1024*1024) throw new InvalidDataException("The CUE sheet is too large.");
        foreach (string line in File.ReadLines(cue)) {
            string s = line.Trim();
            string keyword = s.Split(new[]{' ', '\t'}, 2)[0].ToUpperInvariant();
            if (keyword == "FILE") {
                Match m = CueFile.Match(s);
                if (!m.Success || path != null) throw new InvalidDataException("Choose a single-track BINARY CUE for Racer Revenge.");
                string name = m.Groups[1].Success ? m.Groups[1].Value : m.Groups[2].Value;
                string parent = Path.GetDirectoryName(Path.GetFullPath(cue));
                path = Disc.ChildPath(parent, name);
            } else if (keyword == "TRACK") {
                Match m = CueTrack.Match(s);
                if (path == null || mode != null || !m.Success) throw new InvalidDataException("Expected one PS2 CD data track in the CUE sheet.");
                mode = m.Groups[1].Value.ToUpperInvariant();
            } else if (Regex.IsMatch(s, @"^INDEX\s+01\b", RegexOptions.IgnoreCase)) {
                Match m = CueIndex.Match(s);
                if (!m.Success || mode == null || start >= 0 || Int32.Parse(m.Groups[3].Value) >= 75)
                    throw new InvalidDataException("Invalid INDEX 01 in the CUE sheet.");
                start = (Int32.Parse(m.Groups[1].Value)*60L + Int32.Parse(m.Groups[2].Value))*75 + Int32.Parse(m.Groups[3].Value);
            }
        }
        if (start < 0) throw new InvalidDataException("The data track has no INDEX 01 in its CUE sheet.");
    }

    static void Fill(Stream source, byte[] buffer, int at, int count) {
        while (count > 0) {
            int n = source.Read(buffer, at, count);
            if (n == 0) throw new EndOfStreamException("The PS2 image ends inside a data sector.");
            at += n; count -= n;
        }
    }

    public DiscImage(string path) {
        string mode = null;
        long start = 0;
        if (String.Equals(Path.GetExtension(path), ".cue", StringComparison.OrdinalIgnoreCase))
            ReadCue(path, out path, out mode, out start);
        file = File.OpenRead(path);
        try {
            // Some standalone BINs store the standard 150-sector pregap in the file.
            foreach (long index in mode == null ? new long[]{0,150} : new long[]{start}) {
                if ((mode == null || mode.EndsWith("/2048")) && Probe(index, 2048, 0)) return;
                if ((mode == null || mode == "MODE2/2352") && Probe(index, 2352, 24)) return;
                if ((mode == null || mode == "MODE1/2352") && Probe(index, 2352, 16)) return;
                if ((mode == null || mode == "MODE2/2336") && Probe(index, 2336, 8)) return;
            }
            throw new InvalidDataException("No ISO9660 PS2 data track found. For a BIN with a CUE sheet, select the CUE.");
        } catch { file.Dispose(); throw; }
    }

    bool Probe(long index, int physical, int data) {
        long position = (index + 16) * physical;
        if (position + physical > file.Length) return false;
        file.Position = position;
        Fill(file, cache, 0, physical);
        if (Encoding.ASCII.GetString(cache, data+1, 5) != "CD001" || cache[data+6] != 1) return false;
        first = index; stride = physical; payload = data;
        CheckSector(0);
        Length = (file.Length / stride - first) * Block;
        return true;
    }

    void CheckSector(int at) {
        if (stride == Block) return;
        if (stride == 2352) {
            if (cache[at]!=0 || cache[at+11]!=0 || cache[at+15]!=(payload==16?1:2))
                throw new InvalidDataException("Invalid raw CD sector header.");
            for (int i=1; i<=10; i++) if (cache[at+i]!=255) throw new InvalidDataException("Invalid raw CD sync.");
        }
        if (payload == 16) return;
        int sub = at + payload - 8;
        for (int i=0; i<4; i++) if (cache[sub+i] != cache[sub+4+i])
            throw new InvalidDataException("Invalid Mode 2 XA subheader.");
        if ((cache[sub+2]&0x20)!=0) throw new InvalidDataException("Expected a Mode 2 Form 1 filesystem sector.");
    }

    public void Read(long position, byte[] output, int offset, int count) {
        if (position < 0 || count < 0 || position > Length-count) throw new InvalidDataException("Truncated PS2 disc extent.");
        if (stride == Block) {
            file.Position = first*stride + position;
            Fill(file, output, offset, count);
            return;
        }
        while (count > 0) {
            long sector = position / Block;
            if (cached < 0 || sector < cached || sector >= cached+CacheSectors) {
                cached = sector;
                file.Position = (first+cached)*stride;
                int sectors = (int)Math.Min(CacheSectors, Length/Block-cached);
                Fill(file, cache, 0, sectors*stride);
            }
            int at = (int)(sector-cached)*stride;
            CheckSector(at);
            int inside = (int)(position%Block);
            int n = Math.Min(count, Block-inside);
            Buffer.BlockCopy(cache, at+payload+inside, output, offset, n);
            position += n; offset += n; count -= n;
        }
    }

    public void Dispose() { file.Dispose(); }
}
