using System;
using System.IO;
using System.Linq;

sealed class PadLayout {
    public readonly int[] Buttons=Enumerable.Range(0,16).ToArray(), Axes=Enumerable.Range(0,4).ToArray();
    public static int Assign(int[] map,int target,int source){
        int other=source<0?-1:Array.IndexOf(map,source);
        if(other>=0)map[other]=map[target];
        map[target]=source;return other;
    }
    public static bool Read(string text,int[] map){
        if(String.IsNullOrEmpty(text))return true;
        var fields=text.Split(',');if(fields.Length!=map.Length)return false;
        var parsed=new int[map.Length];var used=new bool[map.Length];
        for(int i=0;i<map.Length;i++){
            int v;if(!int.TryParse(fields[i],out v)||v< -1||v>=map.Length||
                (map.Length==16&&((v==9)!=(i==9)))||(v>=0&&used[v]))return false;
            parsed[i]=v;if(v>=0)used[v]=true;
        }
        parsed.CopyTo(map,0);return true;
    }
    public static string Encode(int[] map){return map.Where((v,i)=>v!=i).Any()?String.Join(",",map):null;}
}
sealed class PadProfiles {
    public readonly PadLayout[,] Items=new PadLayout[2,8];
    public PadProfiles(){for(int p=0;p<2;p++)for(int s=0;s<8;s++)Items[p,s]=new PadLayout();}
    static string Section(int player,int preset){return "Controller"+(player+1)+"Preset"+(preset+1);}
    public PadProfiles Copy(){
        var copy=new PadProfiles();
        for(int p=0;p<2;p++)for(int s=0;s<8;s++){
            Items[p,s].Buttons.CopyTo(copy.Items[p,s].Buttons,0);Items[p,s].Axes.CopyTo(copy.Items[p,s].Axes,0);
        }
        return copy;
    }
    public void Load(Func<string,string,string> read){
        for(int p=0;p<2;p++)for(int s=0;s<8;s++){
            var section=Section(p,s);var layout=new PadLayout();
            if(PadLayout.Read(read(section,"Buttons"),layout.Buttons)&&PadLayout.Read(read(section,"Axes"),layout.Axes))Items[p,s]=layout;
        }
    }
    public void Write(Action<string,string,string> write){
        for(int p=0;p<2;p++)for(int s=0;s<8;s++){
            var section=Section(p,s);write(section,"Buttons",PadLayout.Encode(Items[p,s].Buttons));
            write(section,"Axes",PadLayout.Encode(Items[p,s].Axes));
        }
    }
    public static int[][] ReadPresetRows(){
        using(var reader=new StreamReader(typeof(PadProfiles).Assembly.GetManifestResourceStream("pad-presets"))){
            var rows=reader.ReadToEnd().Split(new[]{'\r','\n'},StringSplitOptions.RemoveEmptyEntries);
            return rows.Select(row=>row.Trim().TrimEnd(',').Trim('{','}').Split(',').Select(int.Parse).ToArray()).ToArray();
        }
    }
}
