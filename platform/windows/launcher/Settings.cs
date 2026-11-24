using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

sealed class Settings {
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode)]
    static extern int GetPrivateProfileString(string section,string key,string fallback,StringBuilder result,int count,string file);
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode, SetLastError=true)]
    static extern bool WritePrivateProfileString(string section,string key,string value,string file);
    public int Width=1280, Height=896, P1=-1, P2=-1;
    public bool Maximized=true,Widescreen=true,Fxaa=true;
    public string Device="Auto";
    public KeyboardLayout Keyboard=new KeyboardLayout();
    public PadProfiles Controllers=new PadProfiles();
    static string Get(string file,string section,string key,string fallback){
        var result=new StringBuilder(256);
        GetPrivateProfileString(section,key,fallback,result,result.Capacity,file);
        return result.ToString();
    }
    static int Num(string file,string section,string key,int fallback,int min,int max){
        int n;return int.TryParse(Get(file,section,key,""),out n)&&n>=min&&n<=max?n:fallback;
    }
    static string Ini(string dir){return Path.Combine(dir,"RacerRevengePC.ini");}
    public static Settings Read(string dir){
        string f=Ini(dir);
        var s=new Settings();
        s.Width=Num(f,"Video","WindowWidth",1280,640,3840);
        s.Height=Num(f,"Video","WindowHeight",896,448,2160);
        s.Maximized=Num(f,"Video","Maximized",1,0,1)==1;
        s.Widescreen=Num(f,"Video","Widescreen",1,0,1)==1;
        s.Fxaa=Num(f,"Video","FXAA",1,0,1)==1;
        s.P1=Num(f,"Controls","Player1Preset",-1,-1,7);
        s.P2=Num(f,"Controls","Player2Preset",-1,-1,7);
        string device=Get(f,"Controls","Device","Auto");
        if(device.Equals("keyboard",StringComparison.OrdinalIgnoreCase))s.Device="Keyboard";
        if(device.Equals("controller",StringComparison.OrdinalIgnoreCase))s.Device="Controller";
        s.Keyboard.Load((section,key)=>Get(f,section,key,""));
        s.Controllers.Load((section,key)=>Get(f,section,key,""));
        return s;
    }
    public void Save(string dir){
        string f=Ini(dir), tmp=Path.Combine(dir,Guid.NewGuid().ToString("N")+".ini");
        try{
            if(File.Exists(f))File.Copy(f,tmp);
            Action<string,string,string> put=(sec,key,val)=>{
                if(!WritePrivateProfileString(sec,key,val,tmp))throw new IOException("Could not save "+key);
            };
            put("Video","WindowWidth",Width.ToString());put("Video","WindowHeight",Height.ToString());
            put("Video","Maximized",Maximized?"1":"0");put("Video","Widescreen",Widescreen?"1":"0");put("Video","FXAA",Fxaa?"1":"0");
            put("Controls","Device",Device);put("Controls","Player1Preset",P1.ToString());put("Controls","Player2Preset",P2.ToString());
            Keyboard.Write(put);
            Controllers.Write(put);
            if(File.Exists(f))File.Replace(tmp,f,null);else File.Move(tmp,f);
        }finally{if(File.Exists(tmp))File.Delete(tmp);}
    }
}
