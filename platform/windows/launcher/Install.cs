using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;

static class Install {
    public const string Game="RacerRevengePC.exe", Launcher="RacerRevengeLauncher.exe";
    const string Marker=".rrpc-install";
    public static bool Ready(string dir) {return !String.IsNullOrEmpty(dir)&&File.Exists(Path.Combine(dir,Marker))&&File.Exists(Path.Combine(dir,Game));}
    public static string Current(){
        string own=AppDomain.CurrentDomain.BaseDirectory;
        if(Ready(own))return own;
        string link=Path.Combine(own,"RacerRevenge.install-path");
        if(File.Exists(link)){string dir=File.ReadAllText(link).Trim();if(Ready(dir))return dir;}
        return null;
    }
    static string Digest(Stream s){using(var h=SHA256.Create())return BitConverter.ToString(h.ComputeHash(s)).Replace("-","").ToLowerInvariant();}
    static Dictionary<string,string> Rows(ZipArchive bundle,string name){
        var rows=new Dictionary<string,string>(StringComparer.OrdinalIgnoreCase);
        using(var r=new StreamReader(bundle.GetEntry(name+".tsv").Open())){
            string line;
            while((line=r.ReadLine())!=null){string[] pair=line.Split('\t');rows.Add(pair[1],pair[0]);}
        }
        return rows;
    }
    static string Safe(string root,string relative){
        string result=Path.GetFullPath(Path.Combine(root,relative));
        if(!result.StartsWith(root+Path.DirectorySeparatorChar,StringComparison.OrdinalIgnoreCase))throw new InvalidDataException("Invalid package path.");
        return result;
    }
    static void VerifyArcade(string path,Dictionary<string,string> files){
        using(var input=File.OpenRead(path))using(var zip=new ZipArchive(input)){
            var chips=zip.Entries.Where(e=>e.Name.Length>0 && files.ContainsKey(e.Name)).ToDictionary(e=>e.Name,StringComparer.OrdinalIgnoreCase);
            foreach(var pair in files){
                ZipArchiveEntry e;
                if(!chips.TryGetValue(pair.Key,out e))throw new InvalidDataException("Racer Arcade ROM set is missing "+pair.Key);
                using(var s=e.Open())if(Digest(s)!=pair.Value)throw new InvalidDataException("Racer Arcade ROM mismatch: "+pair.Key);
            }
        }
    }
    static void VerifyN64(string path,string expected){
        var info=new FileInfo(path);
        if(info.Length<4||info.Length>64*1024*1024||(info.Length%4)!=0)throw new InvalidDataException("Invalid N64 ROM size.");
        byte[] data=File.ReadAllBytes(path);
        if(data[0]==0x37&&data[1]==0x80){
            for(int i=0;i<data.Length;i+=2){byte b=data[i];data[i]=data[i+1];data[i+1]=b;}
        }else if(data[0]==0x40&&data[1]==0x12){
            for(int i=0;i<data.Length;i+=4)Array.Reverse(data,i,4);
        }
        using(var s=new MemoryStream(data))if(Digest(s)!=expected)throw new InvalidDataException("Select the supported USA Episode I Racer N64 ROM (.z64, .v64 or .n64).");
    }
#if PUBLIC_RELEASE
    static void ConvertRoms(ZipArchive bundle,string stage,string arcade,string n64,Action<int,string> status){
        string work=Path.Combine(stage,"Import");
        Directory.CreateDirectory(work);
        string archive=Path.Combine(work,"converter.zip");
        using(var input=bundle.GetEntry("converter.zip").Open())
        using(var output=File.Create(archive))input.CopyTo(output);
        using(var file=File.OpenRead(archive))
        using(var kit=new ZipArchive(file)){
            foreach(var e in kit.Entries){
                string target=Safe(work,e.FullName);
                Directory.CreateDirectory(Path.GetDirectoryName(target));
                using(var input=e.Open())using(var output=File.Create(target))input.CopyTo(output);
            }
        }
        File.Delete(archive);
        var start=new ProcessStartInfo(Path.Combine(work,"runtime","python.exe"),"-B -u convert.py"){
            WorkingDirectory=work,UseShellExecute=false,CreateNoWindow=true,
            RedirectStandardOutput=true,RedirectStandardError=true
        };
        start.EnvironmentVariables["RR_CONVERT_ROOT"]=work;
        start.EnvironmentVariables["RR_CONVERT_STOCK"]=stage;
        start.EnvironmentVariables["RR_IMPORT_ARCADE"]=arcade;
        start.EnvironmentVariables["RR_IMPORT_N64"]=n64;
        start.EnvironmentVariables["OPENBLAS_NUM_THREADS"]="1";
        start.EnvironmentVariables["PYTHONIOENCODING"]="utf-8";
        start.EnvironmentVariables["TMP"]=work;
        start.EnvironmentVariables["TEMP"]=work;
        string lastError="";
        using(var worker=new Process()){
            worker.StartInfo=start;
            worker.ErrorDataReceived+=(sender,e)=>{if(!String.IsNullOrWhiteSpace(e.Data))lastError=e.Data;};
            worker.Start();worker.BeginErrorReadLine();
            string line;
            while((line=worker.StandardOutput.ReadLine())!=null){
                if(line.StartsWith("Importing ")||line.StartsWith("Packaging "))status(92,line);
            }
            worker.WaitForExit();
            if(worker.ExitCode!=0)throw new IOException("ROM conversion failed. "+lastError);
        }
        Directory.Delete(work,true);
    }
#endif
    public static string Run(string ps2,string arcade,string n64,string destination,Action<int,string> status){
        if(String.IsNullOrWhiteSpace(ps2)||String.IsNullOrWhiteSpace(destination))throw new ArgumentException("Choose the PS2 game and installation folder.");
        if(String.IsNullOrWhiteSpace(arcade)!=String.IsNullOrWhiteSpace(n64))
            throw new ArgumentException("The full installation requires both Racer Arcade and Episode I Racer.");
        string dir=Path.GetFullPath(destination).TrimEnd(Path.DirectorySeparatorChar);
        string parent=Path.GetDirectoryName(dir);
        if(String.IsNullOrEmpty(parent)||Directory.Exists(dir)||File.Exists(dir))throw new IOException("Choose a new installation folder. Existing folders are preserved.");
        string source=Path.GetFullPath(ps2).TrimEnd(Path.DirectorySeparatorChar);
        if(Directory.Exists(source)&&dir.StartsWith(source+Path.DirectorySeparatorChar,StringComparison.OrdinalIgnoreCase))throw new IOException("Choose an installation outside the source disc folder.");
        Directory.CreateDirectory(parent);
        string stage=Path.Combine(parent,".rrpc-"+Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(stage);
        try{
            using(var resource=Assembly.GetExecutingAssembly().GetManifestResourceStream("content.zip"))
            using(var bundle=new ZipArchive(resource)){
                if(!String.IsNullOrEmpty(arcade)){status(0,"Verifying Racer Arcade");VerifyArcade(arcade,Rows(bundle,"arcade"));}
                if(!String.IsNullOrEmpty(n64)){status(1,"Verifying Episode I Racer");VerifyN64(n64,Rows(bundle,"n64")["USA"]);}
                var files=Rows(bundle,"disc");int done=0,previous=-1;
                using(var disc=new Disc(ps2))foreach(var pair in files){
                    int percent=2+86*done++/files.Count;
                    if(percent!=previous){status(percent,"Installing Racer Revenge");previous=percent;}
                    string dest=Safe(stage,pair.Key);Directory.CreateDirectory(Path.GetDirectoryName(dest));
                    disc.Copy(pair.Key,dest);
                    using(var s=File.OpenRead(dest))if(Digest(s)!=pair.Value)throw new InvalidDataException("The Racer Revenge USA disc does not match at "+pair.Key);
                }
                status(89,"Installing the game and selected content");
                foreach(var e in bundle.Entries){
                    int slash=e.FullName.IndexOf('/');if(slash<0)continue;
                    string group=e.FullName.Substring(0,slash);
                    if(group!="base"&&!(group=="arcade"&&!String.IsNullOrEmpty(arcade))&&!(group=="n64"&&!String.IsNullOrEmpty(n64)))continue;
                    string target=Safe(stage,e.FullName.Substring(slash+1));
                    Directory.CreateDirectory(Path.GetDirectoryName(target));
                    using(var stream=e.Open())using(var output=File.Create(target))stream.CopyTo(output);
                    const string templatePrefix="base/Templates/demolition-pugwis/";
                    if(e.FullName.StartsWith(templatePrefix,StringComparison.Ordinal)){
                        string active=Safe(stage,"DLC/demolition-pugwis/"+e.FullName.Substring(templatePrefix.Length));
                        Directory.CreateDirectory(Path.GetDirectoryName(active));
                        File.Copy(target,active);
                    }
                }
#if PUBLIC_RELEASE
                if(!String.IsNullOrEmpty(arcade)||!String.IsNullOrEmpty(n64)){
                    status(90,"Preparing offline conversion tools");
                    ConvertRoms(bundle,stage,arcade,n64,status);
                }
#endif
            }
            Directory.CreateDirectory(Path.Combine(stage,"DLC"));
            File.Copy(Assembly.GetExecutingAssembly().Location,Path.Combine(stage,Launcher));
            File.WriteAllText(Path.Combine(stage,Marker),"1\n");
            Directory.Move(stage,dir);
        }finally{if(Directory.Exists(stage))Directory.Delete(stage,true);}
        // The copy inside the installation always locates the game beside itself.
        try{File.WriteAllText(Path.Combine(AppDomain.CurrentDomain.BaseDirectory,"RacerRevenge.install-path"),dir);}
        catch(UnauthorizedAccessException){}catch(IOException){}
        status(100,"Installed");return dir;
    }
}
