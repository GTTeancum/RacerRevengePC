using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;

sealed class PadEditor:Form {
    static readonly Color Night=Color.FromArgb(17,24,34), Panel=Color.FromArgb(29,40,54), Ink=Color.WhiteSmoke, Gold=Color.FromArgb(255,193,68);
    static readonly string[] Buttons={"Y / Triangle","B / Circle","A / Cross","X / Square","LT / L2","RT / R2","LB / L1","RB / R1","Back / Select","Start","Right stick click","Left stick click","D-pad up","D-pad right","D-pad down","D-pad left"};
    static readonly string[] Axes={"Left stick X","Left stick Y","Right stick X","Right stick Y"};
    static readonly int[][] Presets=PadProfiles.ReadPresetRows();
    public readonly PadProfiles Draft;
    readonly TableLayoutPanel groups=new TableLayoutPanel();
    readonly List<Action> refresh=new List<Action>();
    readonly Label status=new Label();
    readonly ComboBox players=List("Player"), presets=List("Preset to edit");
    readonly int[] viewed;
    bool updating;
    int Player{get{return players.SelectedIndex;}}
    int Preset{get{return presets.SelectedIndex;}}
    static ComboBox List(string name){return new ComboBox{AccessibleName=name,DropDownStyle=ComboBoxStyle.DropDownList,FlatStyle=FlatStyle.Flat,BackColor=Night,ForeColor=Ink,Width=160};}
    static Label TextLabel(string text,float size,Color color,bool bold=false){
        return new Label{Text=text,AutoSize=true,Font=new Font("Segoe UI",size,bold?FontStyle.Bold:FontStyle.Regular),ForeColor=color,Anchor=AnchorStyles.Left,Margin=new Padding(2,4,2,5)};
    }
    static Button MakeButton(string text,Action action){
        var b=new Button{Text=text,AutoSize=true,FlatStyle=FlatStyle.Flat,Padding=new Padding(9,3,9,3),BackColor=Panel,ForeColor=Ink};
        b.FlatAppearance.BorderColor=Color.SlateGray;b.Click+=(s,e)=>action();return b;
    }
    public PadEditor(PadProfiles original,int p1,int p2){
        Draft=original.Copy();viewed=new[]{p1,p2};
        Text="Controller bindings";Font=new Font("Segoe UI",10);ForeColor=Ink;BackColor=Night;Padding=new Padding(18);
        FormBorderStyle=FormBorderStyle.None;StartPosition=FormStartPosition.CenterParent;AutoScaleMode=AutoScaleMode.Dpi;ClientSize=new Size(808,545);
        var page=new TableLayoutPanel{Dock=DockStyle.Fill,ColumnCount=1,RowCount=7};
        page.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,100));
        for(int i=0;i<7;i++)page.RowStyles.Add(i==4?new RowStyle(SizeType.Percent,100):new RowStyle(SizeType.AutoSize));
        Controls.Add(page);page.Controls.Add(TextLabel("RACE CONTROLS",9,Gold,true),0,0);
        page.Controls.Add(TextLabel(Text,23,Ink,true),0,1);
        page.Controls.Add(TextLabel("Choose the physical inputs for each race action. In-game prompts show the original layout.",10,Ink),0,2);
        var nav=new FlowLayoutPanel{AutoSize=true,Dock=DockStyle.Top,Margin=new Padding(0,4,0,12)};
        players.Items.AddRange(new[]{"Player 1","Player 2"});players.SelectedIndex=0;
        for(int i=0;i<8;i++)presets.Items.Add((i<4?"Standard ":"Twin engine ")+(i%4+1));presets.SelectedIndex=p1;
        nav.Controls.Add(players);nav.Controls.Add(TextLabel("Preset to edit",10,Ink));nav.Controls.Add(presets);page.Controls.Add(nav,0,3);
        groups.Dock=DockStyle.Fill;groups.ColumnCount=2;groups.RowCount=1;
        for(int i=0;i<2;i++)groups.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,50));page.Controls.Add(groups,0,4);
        status.Dock=DockStyle.Fill;status.TextAlign=ContentAlignment.MiddleLeft;status.Height=36;status.AccessibleRole=AccessibleRole.StatusBar;
        page.Controls.Add(status,0,5);
        var footer=new TableLayoutPanel{AutoSize=true,Dock=DockStyle.Fill,ColumnCount=4};
        footer.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));footer.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,100));
        footer.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));footer.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
        footer.Controls.Add(MakeButton("Restore preset defaults",()=>{Draft.Items[Player,Preset]=new PadLayout();Render();}),0,0);
        var cancel=MakeButton("Cancel",()=>DialogResult=DialogResult.Cancel);CancelButton=cancel;footer.Controls.Add(cancel,2,0);
        var done=MakeButton("Use bindings",()=>DialogResult=DialogResult.OK);done.BackColor=Gold;done.ForeColor=Night;footer.Controls.Add(done,3,0);page.Controls.Add(footer,0,6);
        players.SelectedIndexChanged+=(s,e)=>{updating=true;presets.SelectedIndex=viewed[Player];updating=false;Render();};
        presets.SelectedIndexChanged+=(s,e)=>{if(!updating){viewed[Player]=Preset;Render();}};Render();
    }
    TableLayoutPanel Card(string title,int side){
        var card=new TableLayoutPanel{AutoScroll=true,Dock=DockStyle.Fill,ColumnCount=2,Padding=new Padding(11),BackColor=Panel,Margin=new Padding(side==0?0:5,0,side==0?5:0,0)};
        card.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,100));card.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
        var header=TextLabel(title,12,Gold,true);card.Controls.Add(header,0,0);card.SetColumnSpan(header,2);card.RowCount=1;
        card.RowStyles.Add(new RowStyle(SizeType.AutoSize));groups.Controls.Add(card,side,0);return card;
    }
    void Row(TableLayoutPanel card,string action,int target,bool axis=false){
        var map=axis?Draft.Items[Player,Preset].Axes:Draft.Items[Player,Preset].Buttons;
        var labels=axis?Axes:Buttons;var ids=Enumerable.Range(0,labels.Length).Where(i=>axis||i!=9).ToList();
        var field=List(action);field.Items.Add("Unbound");foreach(int id in ids)field.Items.Add(labels[id]);
        int row=card.RowCount++;card.RowStyles.Add(new RowStyle(SizeType.AutoSize));
        card.Controls.Add(TextLabel(action,10,Ink),0,row);card.Controls.Add(field,1,row);
        refresh.Add(()=>{field.SelectedIndex=ids.IndexOf(map[target])+1;field.ForeColor=map[target]==target?Ink:Gold;});
        field.SelectedIndexChanged+=(s,e)=>{
            if(updating)return;
            int value=field.SelectedIndex==0?-1:ids[field.SelectedIndex-1];
            int other=PadLayout.Assign(map,target,value);
            Sync();status.Text=other>=0&&other!=target?"Assignments swapped. Each physical input has one job in this preset.":"Binding updated for "+players.Text+" · "+presets.Text+".";
        };
    }
    void Sync(){updating=true;foreach(var update in refresh)update();updating=false;}
    void Render(){
        refresh.Clear();while(groups.Controls.Count>0)groups.Controls[0].Dispose();
        var driving=Card("Driving",0);var actions=Card("Race actions",1);var p=Presets[Preset];
        if(Preset<4){Row(driving,"Throttle",p[1]);Row(driving,"Brake",p[0]);Row(driving,"Steering",0,true);Row(driving,"Pitch",1,true);}
        else{Row(driving,"Left engine",1,true);Row(driving,"Right engine",3,true);}
        if(Preset==0&&Player==0)Row(driving,"Zero-G lean",2,true);
        Row(driving,"D-pad left",15);Row(driving,"D-pad right",13);
        Row(actions,"Boost",p[2]);Row(actions,"Repair",p[3]);Row(actions,"Powerslide",p[9]);
        if(Player==0){Row(actions,"Cycle camera",p[5]);Row(actions,"Look left",p[6]);Row(actions,"Look right",p[8]);Row(actions,"Reset pod",p[7]);}
        foreach(var card in new[]{driving,actions}){card.RowCount++;card.RowStyles.Add(new RowStyle(SizeType.Percent,100));}
        Sync();status.Text="Start pauses the race. Menu navigation uses the original controls.";
    }
}
