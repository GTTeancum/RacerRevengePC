using System;
using System.IO;
using System.IO.Compression;
using System.Text.RegularExpressions;

static class Chd {
    static readonly Regex Metadata = new Regex(@"^Metadata:\s+Tag='(CHCD|CHTR|CHT2|DVD )'");

    static string Run(string work, string arguments) {
        string media = null;
        DiscTool.Run(work, "chdman.exe", arguments, stream => {
            var reader = new StreamReader(stream);
            string line;
            while ((line = reader.ReadLine()) != null) {
                Match m = Metadata.Match(line);
                if (m.Success) media = m.Groups[1].Value;
            }
        });
        return media;
    }

    public static string Extract(ZipArchive bundle, string path, string work) {
        DiscTool.Unpack(bundle, work, "chdman.exe");
        string inputArg = " -i \"" + Path.GetFullPath(path) + "\"";
        string media = Run(work, "info" + inputArg);
        if (media == null) throw new InvalidDataException("Choose a self-contained PS2 CD or DVD CHD.");
        if (media == "DVD ") {
            Run(work, "extractdvd" + inputArg + " -o image.iso");
            return Path.Combine(work, "image.iso");
        }
        Run(work, "extractcd" + inputArg + " -o image.cue -ob image.bin");
        return Path.Combine(work, "image.cue");
    }
}
