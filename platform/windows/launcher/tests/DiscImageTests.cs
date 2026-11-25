using System;
using System.IO;
using System.Linq;
using System.Text;

static class DiscImageTests {
    const int Block = 2048;

    // Generate only synthetic filesystem sectors. Sector 16 carries the PVD signature.
    static byte[] LogicalImage() {
        byte[] data = new byte[131 * Block];
        new Random(211).NextBytes(data);
        int descriptor = 16 * Block;
        data[descriptor] = 1;
        Encoding.ASCII.GetBytes("CD001").CopyTo(data, descriptor + 1);
        data[descriptor + 6] = 1;
        return data;
    }

    static void WriteTrack(string path, byte[] data, int stride, int payload, int pregap) {
        using (var file = File.Create(path)) {
            file.SetLength((long)pregap * stride);
            file.Position = file.Length;
            byte[] sector = new byte[stride];
            if (stride == 2352) {
                for (int i = 1; i <= 10; i++) sector[i] = 255;
                sector[15] = (byte)(payload == 16 ? 1 : 2);
            }
            if (payload == 24 || payload == 8) sector[payload-6] = sector[payload-2] = 8;
            for (int i = 0; i < data.Length; i += Block) {
                Buffer.BlockCopy(data, i, sector, payload, Block);
                file.Write(sector, 0, sector.Length);
            }
        }
    }

    static void Compare(string path, byte[] data) {
        using (var image = new DiscImage(path)) {
            if (image.Length != data.Length) throw new Exception("Wrong logical track size");
            byte[] actual = new byte[data.Length + 2];
            image.Read(0, actual, 1, data.Length);
            if (!data.SequenceEqual(actual.Skip(1).Take(data.Length))) throw new Exception("Incorrect payload in " + path);
            int start = 63 * Block - 3, length = 4 * Block + 15;
            image.Read(start, actual, 2, length);
            if (!data.Skip(start).Take(length).SequenceEqual(actual.Skip(2).Take(length)))
                throw new Exception("Incorrect partial or cross-sector read in " + path);
        }
    }

    static void Rejected(Action operation) {
        try { operation(); } catch (InvalidDataException) { return; }
        throw new Exception("Invalid image was accepted");
    }

    public static void Main(string[] args) {
        string root = Directory.CreateDirectory(args[0]).FullName;
        string bin = Path.Combine(root, "image with spaces.track"), cue = Path.Combine(root, "game.cue");
        byte[] data = LogicalImage();
        foreach (var encoding in new[] {
            new { Mode="MODE1/2048", Stride=2048, Offset=0 }, new { Mode="MODE1/2352", Stride=2352, Offset=16 },
            new { Mode="MODE2/2336", Stride=2336, Offset=8 }, new { Mode="MODE2/2352", Stride=2352, Offset=24 }
        }) {
            foreach (int pregap in new[] {0, 75, 150}) {
                WriteTrack(bin, data, encoding.Stride, encoding.Offset, pregap);
                File.WriteAllText(cue, "FILE \".\\image with spaces.track\" BINARY\r\n TRACK 01 " + encoding.Mode +
                    "\r\n INDEX 01 00:0" + (pregap/75) + ":00\r\n");
                Compare(cue, data);
                if (pregap != 75) Compare(bin, data);
            }
        }
        WriteTrack(bin, data, 2352, 24, 0);
        using (var file = File.OpenWrite(bin)) {
            file.Position = 35 * 2352 + 18; file.WriteByte(0x28);
            file.Position += 3; file.WriteByte(0x28);
        }
        Rejected(() => Compare(bin, data));
        File.WriteAllText(cue, "FILE \"../other.bin\" BINARY\n TRACK 01 MODE2/2352\n INDEX 01 00:00:00\n");
        Rejected(() => Compare(cue, data));
        Console.WriteLine("Passed cooked and raw sector layouts, CUE offsets, partial reads and invalid input checks.");
    }
}
