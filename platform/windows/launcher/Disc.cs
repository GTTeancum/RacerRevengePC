using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Collections.Generic;

sealed class Disc : IDisposable {
    static readonly string[] Trees = {"AUDIO","PODS","TRACKS","UI"};
    static readonly string[] Core = {"APPSTART.RES","LOC.RES","UI/UIFONTS.RES","UI/UIMAIN/UIMAIN.RES",
        "UI/UICOURSE/UICOURSE.RES","UI/UIRIDER/UIRIDER.RES","AUDIO/ANNOUNCE.RES"};
    static readonly string[] Courses = "TA TB TC RC MA MB MC RA GA GB GC SA SC".Split(' ');
    static readonly string[] Pods = "01 02 03 04 07 08 09 10 11 12 14 15 16 17 18 19 20 21 22 23 24 25 26".Split(' ');
    static readonly Regex Serial = new Regex(@"^[A-Z]{4}_[0-9]{3}\.[0-9]{2}$",RegexOptions.IgnoreCase);
    readonly Dictionary<string,Extent> files = new Dictionary<string,Extent>(StringComparer.OrdinalIgnoreCase);
    readonly HashSet<long> directories = new HashSet<long>();
    readonly byte[] buffer = new byte[65536];
    readonly DiscImage image;
    readonly string folder;
    readonly string boot;
    struct Extent { public long offset, size; }

    internal static string ChildPath(string root,string relative) {
        string[] parts=relative.Replace('\\','/').Split('/').Where(part=>part!=".").ToArray();
        if(parts.Length==0)throw new InvalidDataException("Empty disc path.");
        foreach(string part in parts) CheckName(part);
        return Path.Combine(root,String.Join(Path.DirectorySeparatorChar.ToString(),parts));
    }
    static void CheckName(string name) {
        if(name.Length==0 || name=="." || name==".." || name.EndsWith(".") || name.EndsWith(" ") ||
           name.IndexOfAny(Path.GetInvalidFileNameChars())>=0 || name.Contains("/") || name.Contains("\\") ||
           Regex.IsMatch(name,@"^(?:CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\.|$)",RegexOptions.IgnoreCase))
            throw new InvalidDataException("Invalid path component on the disc: "+name);
    }
    static bool AssetTree(string name) { return Trees.Contains(name,StringComparer.OrdinalIgnoreCase); }
    static bool BootName(string name) {return Serial.IsMatch(name) || name.EndsWith(".ELF",StringComparison.OrdinalIgnoreCase);}
    static bool RootFile(string name) {
        return BootName(name) || new[]{"APPSTART.RES","LOC.RES","SYSTEM.CNF"}.Contains(name,StringComparer.OrdinalIgnoreCase);
    }

    public Disc(string path) {
        try {
            if(Directory.Exists(path)) {
                folder=Path.GetFullPath(path); WalkFolder("",0);
            } else {
                image=new DiscImage(path); IndexImage();
            }
            boot=FindBoot();
            byte[] elf=Prefix(boot,52);
            if(elf[0]!=0x7f || elf[1]!='E' || elf[2]!='L' || elf[3]!='F' || elf[4]!=1 || elf[5]!=1 ||
               BitConverter.ToUInt16(elf,16)!=2 || BitConverter.ToUInt16(elf,18)!=8)
                throw new InvalidDataException("The disc boot file is not a PS2 MIPS executable: "+boot);
            foreach(string file in Core) CheckArchive(file);
            foreach(string course in Courses) CheckArchive("TRACKS/"+course+".RES");
            foreach(string pod in Pods) {
                string baseName="PODS/POD"+pod+"/POD"+pod;
                CheckArchive(baseName+".RES");
                if(Require(baseName+".CAM").size==0)throw new InvalidDataException("Empty Racer Revenge pod camera: "+baseName);
            }
            foreach(string name in new[]{"AUDIO/RACEMUS.JUK","AUDIO/RACEMUS.JUB"})
                if(Require(name).size==0)throw new InvalidDataException("Empty Racer Revenge music file: "+name);
        } catch { Dispose(); throw; }
    }

    void Add(string name, Extent extent) {
        if(files.Count>=65536 || files.ContainsKey(name)) throw new InvalidDataException("Duplicate path or too many files on the disc: "+name);
        files.Add(name,extent);
    }

    void WalkFolder(string prefix,int depth) {
        if(depth>20)throw new InvalidDataException("Disc directory nesting is too deep.");
        string dir=prefix.Length==0?folder:ChildPath(folder,prefix);
        foreach(string entry in Directory.EnumerateFileSystemEntries(dir)) {
            string name=Path.GetFileName(entry);
            var attr=File.GetAttributes(entry);
            bool directory=(attr&FileAttributes.Directory)!=0;
            if(prefix.Length==0 && !(directory?AssetTree(name):RootFile(name)))continue;
            CheckName(name);
            if((attr&FileAttributes.ReparsePoint)!=0) throw new InvalidDataException("Linked disc entry: "+name);
            string key=prefix.Length==0?name:prefix+"/"+name;
            if(directory)WalkFolder(key,depth+1);
            else Add(key,new Extent{size=new FileInfo(entry).Length});
        }
    }

    void IndexImage() {
        byte[] block=new byte[2048];
        for(int sector=16;sector<128;sector++) {
            image.Read(sector*2048L,block,0,block.Length);
            if(Encoding.ASCII.GetString(block,1,5)!="CD001" || block[6]!=1 || block[0]==255) break;
            if(block[0]!=1)continue;
            if(BitConverter.ToUInt16(block,128)!=2048 || block[156]<34)break;
            WalkImage("",Record(block,156),0);return;
        }
        throw new InvalidDataException("No supported ISO9660 primary volume on this disc.");
    }
    Extent Record(byte[] b,int i) {
        long offset=((long)BitConverter.ToUInt32(b,i+2)+b[i+1])*2048;
        long size=BitConverter.ToUInt32(b,i+10);
        if(offset>image.Length-size || (b[i+25]&128)!=0 || b[i+26]!=0 || b[i+27]!=0)
            throw new InvalidDataException("Unsupported or damaged ISO9660 extent.");
        return new Extent{offset=offset,size=size};
    }
    void WalkImage(string prefix,Extent dir,int depth) {
        if(depth>20 || dir.size>16*1024*1024 || !directories.Add(dir.offset))throw new InvalidDataException("Invalid ISO9660 directory.");
        byte[] data=new byte[(int)dir.size]; image.Read(dir.offset,data,0,data.Length);
        for(int i=0;i<data.Length;) {
            int n=data[i];
            if(n==0){i=(i/2048+1)*2048;continue;}
            if(n<34 || i+n>data.Length || data[i+32]>n-33)throw new InvalidDataException("Invalid ISO9660 directory record.");
            if(data[i+32]!=1 || data[i+33]>1){
                string name=Encoding.ASCII.GetString(data,i+33,data[i+32]).Split(';')[0].TrimEnd('.');
                bool directory=(data[i+25]&2)!=0;
                if(prefix.Length!=0 || (directory?AssetTree(name):RootFile(name))) {
                    CheckName(name);
                    string key=prefix+name;
                    Extent e=Record(data,i);
                    if(directory)WalkImage(key+"/",e,depth+1);else Add(key,e);
                }
            }
            i+=n;
        }
    }
    Extent Require(string name) {
        Extent e;
        if(!files.TryGetValue(name,out e))throw new InvalidDataException("Missing Racer Revenge disc file: "+name);
        return e;
    }
    void CheckArchive(string name) {
        byte[] header=Prefix(name,20);
        long table=BitConverter.ToUInt32(header,16);
        if(BitConverter.ToUInt32(header,0)!=3 || BitConverter.ToUInt32(header,8)!=0x340 ||
           BitConverter.ToUInt32(header,12)!=0 || table<2 || table%2!=0 ||
           BitConverter.ToUInt32(header,4)!=12+table || 28+table>Require(name).size)
            throw new InvalidDataException("Invalid Racer Revenge resource archive: "+name);
    }
    byte[] Prefix(string name,int length) {
        Extent e=Require(name);
        if(length>e.size)throw new InvalidDataException("Truncated Racer Revenge disc file: "+name);
        byte[] data=new byte[length];
        if(image!=null)image.Read(e.offset,data,0,length);
        else using(var f=File.OpenRead(ChildPath(folder,name))) {
            int done=0;
            while(done<length){int n=f.Read(data,done,length-done);if(n==0)throw new EndOfStreamException();done+=n;}
        }
        return data;
    }
    string FindBoot() {
        Extent cnf;
        if(files.TryGetValue("SYSTEM.CNF",out cnf)) {
            if(cnf.size>65536)throw new InvalidDataException("Invalid PS2 SYSTEM.CNF.");
            string config=Encoding.ASCII.GetString(Prefix("SYSTEM.CNF",(int)cnf.size)).TrimEnd('\0');
            Match m=Regex.Match(config,@"^[ \t]*BOOT2[ \t]*=[ \t]*cdrom0?:[\\/]+([^\\/;:\r\n]+?)(?:;1)?[ \t]*\r?$",
                                 RegexOptions.Multiline|RegexOptions.IgnoreCase);
            if(!m.Success || !BootName(m.Groups[1].Value))throw new InvalidDataException("The PS2 SYSTEM.CNF has no supported boot executable.");
            string name=m.Groups[1].Value; Require(name);return name;
        }
        // Previously extracted installations may omit SYSTEM.CNF.
        var candidates=files.Keys.Where(k=>k.IndexOf('/')<0 && Serial.IsMatch(k)).ToArray();
        if(candidates.Length!=1)throw new InvalidDataException("Choose a Racer Revenge disc root with SYSTEM.CNF or one PS2 boot executable.");
        return candidates[0];
    }
    public IEnumerable<string> GameFiles() {
        return files.Keys.Where(k=>k.IndexOf('/')>=0 || !BootName(k) || String.Equals(k,boot,StringComparison.OrdinalIgnoreCase)).OrderBy(k=>k,StringComparer.OrdinalIgnoreCase);
    }
    public void Copy(string name,string output) {
        if(folder!=null){File.Copy(ChildPath(folder,name),output);return;}
        Extent e=Require(name);
        using(var dest=File.Create(output)) {
            for(long done=0;done<e.size;) {
                int n=(int)Math.Min(buffer.Length,e.size-done);
                image.Read(e.offset+done,buffer,0,n);dest.Write(buffer,0,n);done+=n;
            }
        }
    }
    public void Dispose(){if(image!=null)image.Dispose();}
}
