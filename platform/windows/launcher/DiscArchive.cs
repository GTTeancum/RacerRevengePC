using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;

static class DiscArchive {
    const string Tool = "7za.exe";
    const long MaxDiscBytes = 9L*1024*1024*1024;
    static readonly string[] Extensions = { ".cue", ".bin", ".iso", ".chd" };
    sealed class Member {
        public string name, path, extension;
        public long size = -1;
    }

    static List<Member> ListImages(string work, string source) {
        var images = new List<Member>();
        DiscTool.Run(work, Tool, "l -ba -slt -bd -sccUTF-8 -p- -- \"" + source + "\"", stream => {
            var reader = new StreamReader(stream, Encoding.UTF8);
            string line;
            Member member = null;
            while ((line = reader.ReadLine()) != null) {
                if (line.StartsWith("Path = ", StringComparison.Ordinal)) {
                    string name = line.Substring(7);
                    string ext = Path.GetExtension(name).ToLowerInvariant();
                    member = null;
                    if (!Extensions.Contains(ext)) continue;
                    if (images.Count == 2) throw new InvalidDataException("Choose an archive containing one PS2 disc image or BIN/CUE pair.");
                    member = new Member { name = name, extension = ext, path = Disc.ChildPath(Path.Combine(work, "media"), name) };
                    images.Add(member);
                } else if (member != null && line.StartsWith("Size = ", StringComparison.Ordinal)) {
                    if (!Int64.TryParse(line.Substring(7), NumberStyles.None, CultureInfo.InvariantCulture, out member.size))
                        throw new InvalidDataException("Invalid disc image size in the archive.");
                } else if (member != null && line == "Encrypted = +") {
                    throw new InvalidDataException("Choose an unencrypted archive or the extracted PS2 disc image.");
                }
            }
        }, true);
        return images;
    }

    public static string Extract(ZipArchive bundle, string path, string work) {
        DiscTool.Unpack(bundle, work, Tool);
        string source = Path.GetFullPath(path);
        var images = ListImages(work, source);
        var cue = images.Find(m => m.extension == ".cue");
        if (images.Count == 0 || (cue != null && !images.Exists(m => m.extension == ".bin")) ||
            (images.Count > 1 && cue == null))
            throw new InvalidDataException("Choose an archive containing one ISO, BIN, CHD, or BIN/CUE pair.");

        foreach (Member member in images) {
            long limit = member == cue ? 1024*1024 : MaxDiscBytes;
            if (member.size <= 0 || member.size > limit) throw new InvalidDataException("Invalid archived image size: " + member.name);
            Directory.CreateDirectory(Path.GetDirectoryName(member.path));
            // Only the selected member's bytes go to the validated staging path.
            string args = "x -so -bd -bb0 -spd -y -p- \"-i!" + member.name + "\" -- \"" + source + "\"";
            using (var file = new FileStream(member.path, FileMode.CreateNew, FileAccess.Write)) {
                DiscTool.Run(work, Tool, args, stream => {
                    byte[] buffer = new byte[65536];
                    int n;
                    while ((n = stream.Read(buffer, 0, buffer.Length)) != 0) {
                        if (n > member.size-file.Position) throw new InvalidDataException("Archived image exceeds its declared size.");
                        file.Write(buffer, 0, n);
                    }
                    if (file.Position != member.size) throw new InvalidDataException("Incomplete archived image: " + member.name);
                }, true);
            }
        }
        return (cue ?? images[0]).path;
    }
}
