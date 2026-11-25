using System;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Text;

enum DiscContainer { Plain, Archive, Chd }

static class DiscTool {
    public static DiscContainer Format(string path) {
        if (Directory.Exists(path)) return DiscContainer.Plain;
        byte[] h = new byte[8];
        using (var source = File.OpenRead(path)) {
            if (source.Read(h, 0, h.Length) < h.Length) return DiscContainer.Plain;
        }
        if (Encoding.ASCII.GetString(h) == "MComprHD") return DiscContainer.Chd;
        if ((h[0] == 0x37 && h[1] == 0x7a && h[2] == 0xbc && h[3] == 0xaf && h[4] == 0x27 && h[5] == 0x1c) ||
            (h[0] == 'P' && h[1] == 'K' && h[2] == 3 && h[3] == 4)) return DiscContainer.Archive;
        return DiscContainer.Plain;
    }

    public static void Unpack(ZipArchive bundle, string work, string executable) {
        Directory.CreateDirectory(work);
        using (var input = bundle.GetEntry(executable).Open())
        using (var output = File.Create(Path.Combine(work, executable))) input.CopyTo(output);
    }

    public static void Run(string work, string executable, string arguments, Action<Stream> read, bool allowWarning = false) {
        var start = new ProcessStartInfo(Path.Combine(work, executable), arguments) {
            WorkingDirectory = work, UseShellExecute = false, CreateNoWindow = true,
            RedirectStandardOutput = true, RedirectStandardError = true, RedirectStandardInput = true
        };
        start.EnvironmentVariables["TMP"] = start.EnvironmentVariables["TEMP"] = work;
        string detail = "";
        using (var worker = new Process { StartInfo = start }) {
            worker.ErrorDataReceived += (sender, e) => {
                if (!String.IsNullOrWhiteSpace(e.Data)) detail = e.Data.Substring(0, Math.Min(e.Data.Length, 512));
            };
            worker.Start();
            worker.StandardInput.Close();
            worker.BeginErrorReadLine();
            try { read(worker.StandardOutput.BaseStream); }
            catch {
                try { if (!worker.HasExited) worker.Kill(); } catch (InvalidOperationException) {}
                worker.WaitForExit();
                throw;
            }
            worker.WaitForExit();
            if (worker.ExitCode != 0 && !(allowWarning && worker.ExitCode == 1))
                throw new InvalidDataException(executable + " could not read the disc image. " + detail);
        }
    }
}
