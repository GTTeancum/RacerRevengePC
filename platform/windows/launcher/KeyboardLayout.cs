using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

sealed class BindingAction {
    public string Family, Id, Label;
    public int Original;
}

sealed class KeyboardLayout {
    public static readonly BindingAction[] Actions=ReadActions();
    readonly Dictionary<string,int>[] keys={new Dictionary<string,int>(),new Dictionary<string,int>()};
    static BindingAction[] ReadActions(){
        var rows=new List<BindingAction>();
        using(var stream=typeof(KeyboardLayout).Assembly.GetManifestResourceStream("keyboard-actions"))
        using(var reader=new StreamReader(stream)){
            string line;
            while((line=reader.ReadLine())!=null){
                var m=Regex.Match(line,"^RR_BIND\\((\\w+),(\\w+),(\\d+),\"([^\"]+)\"\\)$");
                if(!m.Success)throw new InvalidDataException("Invalid keyboard action definition.");
                rows.Add(new BindingAction{Family=m.Groups[1].Value,Id=m.Groups[2].Value,
                    Original=int.Parse(m.Groups[3].Value),Label=m.Groups[4].Value});
            }
        }
        return rows.ToArray();
    }
    public static IEnumerable<BindingAction> For(int family){
        return Actions.Where(a=>a.Family=="Both"||a.Family==(family==0?"Standard":"Twin"));
    }
    public KeyboardLayout(){Reset(0);Reset(1);}
    public void Reset(int family){foreach(var a in For(family))keys[family][a.Id]=a.Original;}
    public int Get(int f,BindingAction a){return keys[f][a.Id];}
    public void Set(int f,BindingAction a,int code){keys[f][a.Id]=code;}
    public KeyboardLayout Copy(){
        var result=new KeyboardLayout();
        for(int f=0;f<2;f++)foreach(var a in For(f))result.Set(f,a,Get(f,a));
        return result;
    }
    static string Section(int f){return f==0?"KeyboardStandard":"KeyboardTwin";}
    public void Load(Func<string,string,string> read){
        for(int f=0;f<2;f++){
            var used=new HashSet<int>();
            foreach(var a in For(f)){
                string raw=read(Section(f),a.Id);int key=a.Original;
                if((raw.Length!=0&&!int.TryParse(raw,out key))||key<0||key>=512||
                    key==41||key==42||key==62||key==69||(key!=0&&!used.Add(key))){Reset(f);break;}
                Set(f,a,key);
            }
        }
    }
    public void Write(Action<string,string,string> put){
        for(int f=0;f<2;f++)foreach(var a in For(f))put(Section(f),a.Id,Get(f,a).ToString());
    }
}
