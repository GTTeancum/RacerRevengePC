using System;
using System.IO;
using System.Text;
using System.Collections.Generic;

// PS2 DVD ISO9660 primary volume, 2048-byte logical sectors. Only manifest
// paths leave the reader, irrespective of what other files are on the disc.
sealed class Disc : IDisposable {
    readonly FileStream image;
    readonly string folder;
    readonly Dictionary<string,Extent> files = new Dictionary<string,Extent>(StringComparer.OrdinalIgnoreCase);
    struct Extent { public long offset, size; }
    public Disc(string path) {
        if (Directory.Exists(path)) { folder = Path.GetFullPath(path); return; }
        image = File.OpenRead(path);
        try {
            for (int sector = 16; sector < 128; sector++) {
                byte[] block = Read(sector * 2048L, 2048);
                if (Encoding.ASCII.GetString(block,1,5) != "CD001" || block[6] != 1) break;
                if (block[0] == 255) break;
                if (block[0] != 1) continue;
                if (BitConverter.ToUInt16(block,128) != 2048) break;
                Walk("", Record(block,156),new HashSet<long>(),0);
                return;
            }
            throw new InvalidDataException("Select a 2048-byte-sector Racer Revenge DVD ISO or an extracted disc folder.");
        } catch { image.Dispose(); throw; }
    }
    byte[] Read(long at,int length) {
        if (at < 0 || length < 0 || at > image.Length-length) throw new InvalidDataException("Truncated DVD image.");
        image.Position = at;
        using (var r = new BinaryReader(image,Encoding.ASCII,true)) return r.ReadBytes(length);
    }
    Extent Record(byte[] b,int i) {
        long offset = ((long)BitConverter.ToUInt32(b,i+2) + b[i+1])*2048;
        long size = BitConverter.ToUInt32(b,i+10);
        if (offset > image.Length-size || (b[i+25]&128)!=0 || b[i+26]!=0 || b[i+27]!=0)
            throw new InvalidDataException("Unsupported or damaged DVD extent.");
        return new Extent { offset=offset, size=size };
    }
    void Walk(string prefix,Extent dir,HashSet<long> visited,int depth) {
        if (depth>20 || dir.size>16*1024*1024 || !visited.Add(dir.offset))
            throw new InvalidDataException("Invalid DVD directory.");
        byte[] data=Read(dir.offset,(int)dir.size);
        for (int i=0;i<data.Length;) {
            int n=data[i];
            if(n==0){i=(i/2048+1)*2048;continue;}
            if(n<34 || i+n>data.Length || data[i+32]>n-33)throw new InvalidDataException("Invalid DVD record.");
            if(data[i+32]!=1 || data[i+33]>1){
                string name=Encoding.ASCII.GetString(data,i+33,data[i+32]).Split(';')[0].TrimEnd('.');
                if(name.Length==0 || name.IndexOfAny(Path.GetInvalidFileNameChars())>=0)throw new InvalidDataException("Invalid DVD filename.");
                Extent e=Record(data,i);
                string path=prefix+name;
                if((data[i+25]&2)!=0)Walk(path+"/",e,visited,depth+1);
                else files.Add(path,e);
            }
            i+=n;
        }
    }
    public void Copy(string relative,string output) {
        if(folder!=null){File.Copy(Path.Combine(folder,relative),output);return;}
        Extent e;
        if(!files.TryGetValue(relative,out e))throw new FileNotFoundException("Missing DVD file: "+relative);
        image.Position=e.offset;
        using(var dest=File.Create(output)){
            byte[] b=new byte[65536];
            for(long left=e.size;left>0;){
                int n=image.Read(b,0,(int)Math.Min(b.Length,left));
                if(n==0)throw new EndOfStreamException();
                dest.Write(b,0,n);left-=n;
            }
        }
    }
    public void Dispose(){if(image!=null)image.Dispose();}
}
