using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

sealed class BindingEditor:Form,IMessageFilter {
    static readonly Color Night=Color.FromArgb(17,24,34), PanelColor=Color.FromArgb(29,40,54),
        Ink=Color.FromArgb(239,243,247), Muted=Color.FromArgb(185,199,210), Gold=Color.FromArgb(255,193,68);
    public readonly KeyboardLayout LayoutDraft;
    readonly TableLayoutPanel body=new TableLayoutPanel();
    readonly Label status=new Label();
    readonly Dictionary<BindingAction,Button> buttons=new Dictionary<BindingAction,Button>();
    readonly Button[] tabs=new Button[2];
    BindingAction listening;
    int family;

    static Button ButtonFor(string text,Action action){
        var b=new Button{Text=text,AutoSize=true,FlatStyle=FlatStyle.Flat,BackColor=PanelColor,ForeColor=Ink,
            Padding=new Padding(8,2,8,2),Margin=new Padding(3)};
        b.FlatAppearance.BorderColor=Color.FromArgb(76,96,113);b.Click+=(s,e)=>action();return b;
    }
    static Label LabelFor(string text,float size,Color color,bool bold){
        return new Label{Text=text,ForeColor=color,AutoSize=true,Font=new Font("Segoe UI",size,bold?FontStyle.Bold:FontStyle.Regular),
            Margin=new Padding(2,3,2,5),Anchor=AnchorStyles.Left};
    }
    public BindingEditor(KeyboardLayout original,bool twin){
        LayoutDraft=original.Copy();family=twin?1:0;
        Text="Keyboard bindings";Font=new Font("Segoe UI",10);BackColor=Night;ForeColor=Ink;
        AutoScaleMode=AutoScaleMode.Dpi;FormBorderStyle=FormBorderStyle.None;StartPosition=FormStartPosition.CenterParent;
        ClientSize=new Size(792,620);Padding=new Padding(18);
        var page=new TableLayoutPanel{Dock=DockStyle.Fill,ColumnCount=1,RowCount=6};
        page.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,100));
        for(int i=0;i<6;i++)page.RowStyles.Add(i==3?new RowStyle(SizeType.Percent,100):new RowStyle(SizeType.AutoSize));
        Controls.Add(page);
        page.Controls.Add(LabelFor("PLAYER 1   /   CONTROLS",9,Gold,true),0,0);
        page.Controls.Add(LabelFor("Keyboard bindings",23,Ink,true),0,1);
        var nav=new FlowLayoutPanel{AutoSize=true,Dock=DockStyle.Top,Margin=new Padding(0,3,0,10)};
        tabs[0]=ButtonFor("Standard",()=>SwitchFamily(0));tabs[1]=ButtonFor("Twin engine",()=>SwitchFamily(1));
        nav.Controls.AddRange(tabs);page.Controls.Add(nav,0,2);
        body.Dock=DockStyle.Fill;body.ColumnCount=2;body.RowCount=1;body.Margin=Padding.Empty;
        body.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,50));body.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,50));
        page.Controls.Add(body,0,3);
        status.Dock=DockStyle.Fill;status.Height=34;status.ForeColor=Muted;status.TextAlign=ContentAlignment.MiddleLeft;
        status.AccessibleRole=AccessibleRole.StatusBar;page.Controls.Add(status,0,4);
        var footer=new TableLayoutPanel{Dock=DockStyle.Top,AutoSize=true,ColumnCount=4,Margin=Padding.Empty};
        footer.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));footer.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,100));
        footer.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));footer.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
        footer.Controls.Add(ButtonFor("Restore layout defaults",()=>{LayoutDraft.Reset(family);SwitchFamily(family);}),0,0);
        var cancel=ButtonFor("Cancel",()=>{DialogResult=DialogResult.Cancel;});CancelButton=cancel;footer.Controls.Add(cancel,2,0);
        var done=ButtonFor("Use bindings",()=>{DialogResult=DialogResult.OK;});done.BackColor=Gold;done.ForeColor=Night;
        footer.Controls.Add(done,3,0);page.Controls.Add(footer,0,5);SwitchFamily(family);
    }
    void SwitchFamily(int next){
        family=next;listening=null;buttons.Clear();
        while(body.Controls.Count>0)body.Controls[0].Dispose();
        for(int i=0;i<2;i++){tabs[i].BackColor=i==family?Gold:PanelColor;tabs[i].ForeColor=i==family?Night:Ink;}
        for(int col=0;col<2;col++){
            var card=new TableLayoutPanel{ColumnCount=2,Dock=DockStyle.Fill,Padding=new Padding(12),BackColor=PanelColor,
                Margin=new Padding(col==0?0:5,0,col==0?5:0,0)};
            card.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,100));card.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            var heading=LabelFor(col==0?"Driving":"Race actions",12,Gold,true);card.Controls.Add(heading,0,0);card.SetColumnSpan(heading,2);
            card.RowStyles.Add(new RowStyle(SizeType.AutoSize));int row=1;
            foreach(var a in KeyboardLayout.For(family).Where(a=>(a.Family=="Both")== (col==1))){
                var label=LabelFor(a.Label+(a.Id.StartsWith("Lean")?" ¹":""),10,Ink,false);card.Controls.Add(label,0,row);
                var key=ButtonFor("",()=>Listen(a));key.AutoSize=false;key.Size=new Size(105,29);key.Padding=Padding.Empty;
                key.AccessibleName="Rebind "+a.Label;buttons.Add(a,key);card.Controls.Add(key,1,row++);
                card.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            }
            if(col==0&&family==0){
                var foot=LabelFor("¹ Available with Standard 1.",9,Muted,false);card.Controls.Add(foot,0,row++);card.SetColumnSpan(foot,2);
                card.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            }
            card.RowCount=row+1;card.RowStyles.Add(new RowStyle(SizeType.Percent,100));body.Controls.Add(card,col,0);
        }
        RefreshKeys();status.Text="Select a key to change its binding.";
    }
    void RefreshKeys(){
        foreach(var item in buttons){
            bool active=item.Key==listening;var b=item.Value;
            b.BackColor=active?Gold:Night;b.ForeColor=active?Night:Ink;
            b.Text=active?"Press a key…":PhysicalKeys.Name(LayoutDraft.Get(family,item.Key));
        }
    }
    void Listen(BindingAction a){listening=a;RefreshKeys();status.Text="Press a key for "+a.Label+".   Delete clears the binding.   Esc cancels.";}
    internal void CaptureScan(int scan){
        if(listening==null)return;
        if(scan==1){listening=null;RefreshKeys();status.Text="Binding unchanged.";return;}
        int key=scan==0x153?0:PhysicalKeys.Code(scan);
        if(key<0){status.Text="That key is unavailable for driving. Choose another key or press Esc.";return;}
        var used=KeyboardLayout.For(family).FirstOrDefault(a=>a!=listening&&key!=0&&LayoutDraft.Get(family,a)==key);
        if(used!=null){status.Text=PhysicalKeys.Name(key)+" is assigned to "+used.Label+". Clear that binding first or choose another key.";return;}
        var changed=listening;LayoutDraft.Set(family,changed,key);listening=null;RefreshKeys();
        status.Text=changed.Label+"   ·   "+PhysicalKeys.Name(key);
    }
    protected override void OnShown(EventArgs e){base.OnShown(e);Application.AddMessageFilter(this);}
    protected override void OnFormClosed(FormClosedEventArgs e){Application.RemoveMessageFilter(this);base.OnFormClosed(e);}
    public bool PreFilterMessage(ref Message m){
        if(listening==null||!ContainsFocus||(m.Msg!=0x100&&m.Msg!=0x104))return false;
        long bits=m.LParam.ToInt64();
        if((bits&(1L<<30))==0)CaptureScan((int)((bits>>16)&0x1ff));
        return true;
    }
}

static class PhysicalKeys {
    // Windows physical make codes (including the extended bit) to SDL USB positions.
    static readonly Dictionary<int,int> scans=new Dictionary<int,int>();
    static readonly Dictionary<int,string> names=new Dictionary<int,string>();
    [DllImport("user32.dll",CharSet=CharSet.Unicode)]static extern int GetKeyNameText(int lParam,StringBuilder text,int count);
    static void Add(int scan,int code,string fallback){
        scans.Add(scan,code);var name=new StringBuilder(64);
        names.Add(code,GetKeyNameText(scan<<16,name,name.Capacity)>0?name.ToString():fallback);
    }
    static void Letters(int start,string letters){foreach(char c in letters)Add(start++,4+c-'A',c.ToString());}
    static PhysicalKeys(){
        Letters(0x10,"QWERTYUIOP");Letters(0x1e,"ASDFGHJKL");Letters(0x2c,"ZXCVBNM");
        for(int i=0;i<10;i++)Add(2+i,30+i,((i+1)%10).ToString());
        int[,] pairs={{12,45},{13,46},{15,43},{26,47},{27,48},{28,40},{29,224},{39,51},{40,52},{41,53},{42,225},{43,49},{51,54},{52,55},{53,56},{54,229},{55,85},{57,44},{71,95},{72,96},{73,97},{74,86},{75,92},{76,93},{77,94},{78,87},{79,89},{80,90},{81,91},{82,98},{83,99},{86,100},{0x11c,88},{0x11d,228},{0x135,84},{0x147,74},{0x148,82},{0x149,75},{0x14b,80},{0x14d,79},{0x14f,77},{0x150,81},{0x151,78},{0x152,73}};
        for(int i=0;i<pairs.GetLength(0);i++)Add(pairs[i,0],pairs[i,1],"Key "+pairs[i,1]);
        for(int i=0;i<10;i++)if(i!=4)Add(59+i,58+i,"F"+(i+1));Add(87,68,"F11");
        names[225]="Left Shift";names[229]="Right Shift";names[224]="Left Ctrl";names[228]="Right Ctrl";
    }
    public static int Code(int scan){int value;return scans.TryGetValue(scan,out value)?value:-1;}
    public static string Name(int key){string label;return key==0?"Unbound":names.TryGetValue(key,out label)?label:"Scan "+key;}
}
