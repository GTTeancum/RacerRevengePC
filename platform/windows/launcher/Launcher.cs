using System;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;

static class Program {
    [STAThread] static void Main(string[] args){
        Application.EnableVisualStyles();Application.SetCompatibleTextRenderingDefault(false);
        try { Application.Run(new LauncherForm(args.Length==1 && args[0]=="--test-play-capture")); }
        catch(Exception e){MessageBox.Show(e.Message,"Racer Revenge",MessageBoxButtons.OK,MessageBoxIcon.Error);}
    }
}

sealed class LauncherForm:Form {
    readonly TableLayoutPanel page=new TableLayoutPanel();
    static readonly Color Night=Color.FromArgb(17,24,34), PanelColor=Color.FromArgb(29,40,54),
        Ink=Color.FromArgb(239,243,247), Muted=Color.FromArgb(187,202,215), Gold=Color.FromArgb(255,193,68);
    TableLayoutPanel section;
    sealed class Masthead:Panel {
        readonly string subtitle;
        readonly Icon badge;
        readonly Bitmap panorama;
        Point drag;
        public Masthead(string subtitle,Icon badge){
            this.subtitle=subtitle;this.badge=badge;DoubleBuffered=true;
            Dock=DockStyle.Top;Height=176;Margin=new Padding(0,0,0,14);AccessibleName="Racer Revenge "+subtitle;
            using(var stream=typeof(LauncherForm).Assembly.GetManifestResourceStream("banner.jpg"))
                if(stream!=null)using(var image=Image.FromStream(stream))panorama=new Bitmap(image);
            var chrome=new FlowLayoutPanel{AutoSize=true,AutoSizeMode=AutoSizeMode.GrowAndShrink,WrapContents=false,BackColor=Night,Margin=Padding.Empty};
            foreach(string name in new[]{"Minimize","Close"}){
                var button=new Button{AccessibleName=name,Text=name=="Close"?"×":"–",Width=36,Height=26,
                    BackColor=Night,ForeColor=Ink,FlatStyle=FlatStyle.Flat,Margin=Padding.Empty};
                button.FlatAppearance.BorderSize=0;
                button.Click+=(s,e)=>{var window=FindForm();if(name=="Close")window.Close();else window.WindowState=FormWindowState.Minimized;};
                chrome.Controls.Add(button);
            }
            Controls.Add(chrome);Layout+=(s,e)=>chrome.Location=new Point(Width-chrome.Width,0);
            MouseDown+=(s,e)=>{if(e.Button==MouseButtons.Left){drag=e.Location;Capture=true;}};
            MouseMove+=(s,e)=>{if(Capture&&e.Button==MouseButtons.Left){var window=FindForm();window.Left+=e.X-drag.X;window.Top+=e.Y-drag.Y;}};
            MouseUp+=(s,e)=>Capture=false;
        }
        protected override void Dispose(bool disposing){if(disposing&&panorama!=null)panorama.Dispose();base.Dispose(disposing);}
        protected override void OnPaint(PaintEventArgs e){
            base.OnPaint(e);var g=e.Graphics;float scale=g.DpiX/96f;
            g.ScaleTransform(scale,scale);float w=ClientSize.Width/scale,h=ClientSize.Height/scale;
            g.Clear(Night);g.SmoothingMode=SmoothingMode.AntiAlias;
            if(panorama!=null){
                float width=h*panorama.Width/panorama.Height;
                g.DrawImage(panorama,w-width,0,width,h);
                using(var fade=new LinearGradientBrush(new PointF(w-width-1,0),new PointF(w-width+width*.6f,0),Night,Color.FromArgb(0,Night)))
                    g.FillRectangle(fade,w-width-1,0,width*.6f+1,h);
            }else if(badge!=null)g.DrawIcon(badge,new Rectangle((int)w-155,22,128,128));
            using(var brand=new Font("Segoe UI",12,FontStyle.Bold,GraphicsUnit.Pixel))
            using(var title=new Font("Impact",42,FontStyle.Regular,GraphicsUnit.Pixel))
            using(var sub=new Font("Segoe UI",16,FontStyle.Regular,GraphicsUnit.Pixel))
            using(var accent=new SolidBrush(Gold)){
                g.DrawString("S T A R   W A R S",brand,accent,18,h/2-54);
                g.DrawString("RACER REVENGE",title,Brushes.White,15,h/2-37);
                g.DrawString(subtitle,sub,Brushes.White,18,h/2+20);
                g.FillRectangle(accent,0,h-3,w*.40f,3);
                for(int i=0;i<3;i++){float x=w*.40f+5+i*9;g.FillPolygon(accent,new[]{new PointF(x,h-3),new PointF(x+6,h-3),new PointF(x+3,h),new PointF(x-3,h)});}
            }
        }
    }
    sealed class Switch:CheckBox {
        public Switch(){AutoSize=false;Height=30;Dock=DockStyle.Top;FlatStyle=FlatStyle.Flat;DoubleBuffered=true;}
        protected override void OnPaint(PaintEventArgs e){
            var g=e.Graphics;float k=g.DpiX/96f;g.Clear(BackColor);g.SmoothingMode=SmoothingMode.AntiAlias;
            float h=14*k,y=(Height-h)/2;
            using(var fill=new SolidBrush(Checked?Gold:Color.FromArgb(89,109,128))){
                g.FillEllipse(fill,0,y,h,h);g.FillRectangle(fill,h/2,y,17*k,h);g.FillEllipse(fill,17*k,y,h,h);
            }
            g.FillEllipse(Checked?Brushes.Black:Brushes.White,(Checked?19:2)*k,y+2*k,10*k,10*k);
            TextRenderer.DrawText(g,Text,Font,new Rectangle((int)(40*k),0,Width-(int)(40*k),Height),ForeColor,TextFormatFlags.Left|TextFormatFlags.VerticalCenter);
            if(Focused&&ShowFocusCues)ControlPaint.DrawFocusRectangle(g,ClientRectangle,Ink,BackColor);
        }
    }
    sealed class RaceProgress:ProgressBar {
        public RaceProgress(){SetStyle(ControlStyles.UserPaint|ControlStyles.OptimizedDoubleBuffer,true);}
        protected override void OnPaint(PaintEventArgs e){
            var g=e.Graphics;g.Clear(PanelColor);
            using(var bar=new SolidBrush(Gold))g.FillRectangle(bar,0,0,Width*Value/Maximum,Height);
            using(var seam=new Pen(Night,2))for(int x=10;x<Width;x+=12)g.DrawLine(seam,x,0,x-4,Height);
        }
    }
    string installed; bool busy;
    readonly bool testPlayCapture;
    public LauncherForm():this(false){}
    public LauncherForm(bool testPlayCapture){
        this.testPlayCapture=testPlayCapture;
        Text="Racer Revenge";Font=new Font("Segoe UI",10);AutoScaleMode=AutoScaleMode.Dpi;
        StartPosition=FormStartPosition.CenterScreen;FormBorderStyle=FormBorderStyle.None;ClientSize=new Size(780,664);MinimumSize=new Size(710,520);BackColor=Night;ForeColor=Ink;
        Icon=Icon.ExtractAssociatedIcon(Application.ExecutablePath);
        page.Dock=DockStyle.Fill;page.Padding=new Padding(12);page.ColumnCount=1;page.AutoScroll=true;
        page.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,100));Controls.Add(page);
        installed=Install.Current();if(installed==null)SetupPage();else OptionsPage();
        FormClosing+=(s,e)=>{if(busy)e.Cancel=true;};
    }
    void ResetPage(){section=null;while(page.Controls.Count!=0){var old=page.Controls[0];page.Controls.Remove(old);old.Dispose();}page.RowStyles.Clear();page.RowCount=0;AcceptButton=null;}
    void Add(Control c){
        var target=section??page;int i=target.RowCount++;
        target.RowStyles.Add(new RowStyle(SizeType.AutoSize));target.Controls.Add(c,0,i);
    }
    Label Caption(string text){return new Label{Text=text,ForeColor=Muted,AutoSize=true,Anchor=AnchorStyles.Left,Margin=new Padding(2,6,2,3)};}
    void Title(string text){Add(new Masthead(text,Icon));}
    TableLayoutPanel Card(string name){
        var card=new TableLayoutPanel{BackColor=PanelColor,ForeColor=Ink,Dock=DockStyle.Fill,AutoSize=true,ColumnCount=1,Padding=new Padding(16),Margin=new Padding(0)};
        card.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,100));
        var heading=Caption(name);heading.Font=new Font(Font,FontStyle.Bold);heading.ForeColor=Gold;
        heading.Margin=new Padding(2,0,2,10);card.Controls.Add(heading,0,0);card.RowCount=1;
        card.RowStyles.Add(new RowStyle(SizeType.AutoSize));return card;
    }
    Button ActionButton(string text,EventHandler action){
        bool primary=text=="Install"||text=="Play";
        var b=new Button{Text=text,AutoSize=true,AutoSizeMode=AutoSizeMode.GrowAndShrink,Padding=new Padding(12,4,12,4),
            FlatStyle=FlatStyle.Flat,ForeColor=primary?Night:Ink,BackColor=primary?Gold:PanelColor,UseVisualStyleBackColor=false};
        b.FlatAppearance.BorderColor=primary?Gold:Color.FromArgb(91,113,132);
        b.FlatAppearance.MouseOverBackColor=primary?Color.FromArgb(255,215,128):Color.FromArgb(55,76,94);
        b.Click+=action;return b;
    }
    static void SelectFile(TextBox box,string title,string filter){
        using(var dialog=new OpenFileDialog{Title=title,Filter=filter,CheckFileExists=true,RestoreDirectory=true})
            if(dialog.ShowDialog()==DialogResult.OK)box.Text=dialog.FileName;
    }
    static void SelectFolder(TextBox box,string title,bool parent){
        using(var dialog=new FolderBrowserDialog{Description=title,ShowNewFolderButton=parent})
            if(dialog.ShowDialog()==DialogResult.OK)box.Text=parent?Path.Combine(dialog.SelectedPath,"Racer Revenge PC"):dialog.SelectedPath;
    }
    TextBox PathRow(string label,string filter,bool folders,bool parent){
        Add(Caption(label));
        var row=new TableLayoutPanel{Dock=DockStyle.Top,AutoSize=true,RowCount=1,ColumnCount=1};
        row.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,100));
        var box=new TextBox{BackColor=PanelColor,ForeColor=Ink,BorderStyle=BorderStyle.FixedSingle,Dock=DockStyle.Fill,Anchor=AnchorStyles.Left|AnchorStyles.Right,AccessibleName=label};
        row.Controls.Add(box,0,0);
        Action<Button> append=b=>{int i=row.ColumnCount++;row.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));row.Controls.Add(b,i,0);};
        if(filter!=null)append(ActionButton(folders?"Image…":"Browse…",(s,e)=>SelectFile(box,label,filter)));
        if(folders)append(ActionButton("Folder…",(s,e)=>SelectFolder(box,label,parent)));
        Add(row);return box;
    }
    void SetupPage(){
        ResetPage();Title("Installation");
        var disc=PathRow("Racer Revenge · PS2 disc image or extracted folder","PS2 images and archives (*.iso;*.cue;*.bin;*.chd;*.7z;*.zip)|*.iso;*.cue;*.bin;*.chd;*.7z;*.zip|All files (*.*)|*.*",true,false);
        var full=new Switch{Text="Include Arcade + Nintendo 64 content (both ROMs required)",Checked=false};
        Add(full);
        var arcade=PathRow("Racer Arcade · chip-set ZIP","Racer Arcade (*.zip)|*.zip",false,false);
        var n64=PathRow("Episode I Racer · USA Nintendo 64 ROM","Nintendo 64 ROM (*.z64;*.v64;*.n64)|*.z64;*.v64;*.n64",false,false);
        Action updateMode=()=>{arcade.Parent.Enabled=full.Checked;n64.Parent.Enabled=full.Checked;};
        full.CheckedChanged+=(s,e)=>updateMode();
        updateMode();
        var dest=PathRow("Install into a new folder",null,true,true);
        dest.Text=Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),"Racer Revenge PC");
        Add(Caption("Pugwis is ready to race in both modes. An editable copy is included in Templates."));
        var progress=new RaceProgress{Dock=DockStyle.Top,Height=12};
        var status=Caption("");Add(progress);Add(status);
        var install=ActionButton("Install",async(s,e)=>{
            string d=disc.Text.Trim(),a=full.Checked?arcade.Text.Trim():"",n=full.Checked?n64.Text.Trim():"",to=dest.Text.Trim();
            if(full.Checked&&(a.Length==0||n.Length==0)){status.Text="Select both ROMs for the full installation.";return;}
            full.Enabled=false;busy=true;foreach(Control field in page.Controls)if(field is TableLayoutPanel||field is Button)field.Enabled=false;UseWaitCursor=true;
            try{
                var reporter=new Progress<Tuple<int,string>>(pair=>{progress.Value=pair.Item1;progress.Invalidate();status.Text=pair.Item2+"  ·  "+pair.Item1+"%";});
                installed=await Task.Run(()=>Install.Run(d,a,n,to,(v,msg)=>((IProgress<Tuple<int,string>>)reporter).Report(Tuple.Create(v,msg))));
                OptionsPage();
            }catch(Exception error){status.Text="Installation stopped.";ShowError(error);}
            finally{busy=false;foreach(Control field in page.Controls)field.Enabled=true;if(!full.IsDisposed){full.Enabled=true;updateMode();}UseWaitCursor=false;}
        });
        Add(install);AcceptButton=install;
    }
    void ShowError(Exception e){MessageBox.Show(this,e.Message,Text,MessageBoxButtons.OK,MessageBoxIcon.Error);}
    static ComboBox List(string[] items,int selected){
        var c=new ComboBox{DropDownStyle=ComboBoxStyle.DropDownList,Width=242,FlatStyle=FlatStyle.Flat,BackColor=Night,ForeColor=Ink};c.DrawMode=DrawMode.OwnerDrawFixed;
        c.DrawItem+=(sender,e)=>{
            if(e.Index<0)return;
            bool highlighted=(e.State&DrawItemState.Selected)!=0;
            using(var bg=new SolidBrush(highlighted?PanelColor:Night))e.Graphics.FillRectangle(bg,e.Bounds);
            TextRenderer.DrawText(e.Graphics,items[e.Index],c.Font,e.Bounds,Ink,TextFormatFlags.Left|TextFormatFlags.VerticalCenter);
            e.DrawFocusRectangle();
        };c.Items.AddRange(items);c.SelectedIndex=selected;return c;
    }
    void Row(string label,Control control){
        Add(Caption(label));control.AccessibleName=label;Add(control);
    }
    static NumericUpDown Number(int value,int low,int high){return new NumericUpDown{BackColor=Night,ForeColor=Ink,BorderStyle=BorderStyle.FixedSingle,Minimum=low,Maximum=high,Value=value,Width=87};}
    float CreateGraphicsDpiScale(){using(var g=CreateGraphics())return g.DpiX/96f;}
    void OptionsPage(){
        ResetPage();ClientSize=new Size(ClientSize.Width,(int)(650*CreateGraphicsDpiScale()));Title("Controls & video");var s=Settings.Read(installed);
        var cards=new TableLayoutPanel{AutoSize=true,Dock=DockStyle.Top,ColumnCount=2,RowCount=1,Margin=new Padding(0,0,0,12)};
        cards.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,50));cards.ColumnStyles.Add(new ColumnStyle(SizeType.Percent,50));
        var controls=Card("Controls");var video=Card("Video");controls.Margin=new Padding(0,0,6,0);video.Margin=new Padding(6,0,0,0);
        cards.Controls.Add(controls,0,0);cards.Controls.Add(video,1,0);Add(cards);section=controls;
        var device=List(new[]{"Use menu input device","Keyboard","Controller"},s.Device=="Auto"?0:s.Device=="Keyboard"?1:2);
        string[] schemes={"Use in-game selection","Standard 1","Standard 2","Standard 3","Standard 4","Twin engine 1","Twin engine 2","Twin engine 3","Twin engine 4"};
        var p1=List(schemes,s.P1+1);var p2=List(schemes,s.P2+1);
        Row("Player 1 device",device);Row("Player 1 scheme",p1);Row("Player 2 scheme",p2);
        var editors=new FlowLayoutPanel{AutoSize=true,Dock=DockStyle.Top,Margin=Padding.Empty};
        editors.Controls.Add(ActionButton("Keyboard bindings…",(sender,e)=>{
            using(var editor=new BindingEditor(s.Keyboard,p1.SelectedIndex>=5)){
                if(editor.ShowDialog(this)==DialogResult.OK)s.Keyboard=editor.LayoutDraft;
            }
        }));
        editors.Controls.Add(ActionButton("Controller bindings…",(sender,e)=>{
            using(var editor=new PadEditor(s.Controllers,Math.Max(0,p1.SelectedIndex-1),Math.Max(0,p2.SelectedIndex-1))){
                if(editor.ShowDialog(this)==DialogResult.OK)s.Controllers=editor.Draft;
            }
        }));Add(editors);
        section=video;
        var max=new Switch{Text="Start maximized",Checked=s.Maximized};Add(max);
        var dimensions=new FlowLayoutPanel{AutoSize=true,WrapContents=false};
        var w=Number(s.Width,640,3840);var h=Number(s.Height,448,2160);
        w.AccessibleName="Window width";h.AccessibleName="Window height";
        dimensions.Controls.Add(w);dimensions.Controls.Add(Caption("×"));dimensions.Controls.Add(h);Row("Window size",dimensions);
        dimensions.Enabled=!max.Checked;max.CheckedChanged+=(sender,e)=>dimensions.Enabled=!max.Checked;
        var wide=new Switch{Text="Widescreen",Checked=s.Widescreen};
        var fxaa=new Switch{Text="FXAA edge smoothing",Checked=s.Fxaa};Add(wide);Add(fxaa);
        section=null;var notice=Caption("Settings apply the next time the game starts.");Add(notice);
        Action save=()=>{s.Device=new[]{"Auto","Keyboard","Controller"}[device.SelectedIndex];s.P1=p1.SelectedIndex-1;s.P2=p2.SelectedIndex-1;
            s.Width=(int)w.Value;s.Height=(int)h.Value;s.Maximized=max.Checked;s.Widescreen=wide.Checked;s.Fxaa=fxaa.Checked;s.Save(installed);};
        var buttons=new FlowLayoutPanel{AutoSize=true,Dock=DockStyle.Top,Margin=new Padding(0,10,0,0)};
        buttons.Controls.Add(ActionButton("Save settings",(sender,e)=>{try{save();notice.Text="Saved. Settings apply on the next launch.";}catch(Exception err){ShowError(err);}}));
        var play=ActionButton("Play",(sender,e)=>{try{save();Process.Start(new ProcessStartInfo(Path.Combine(installed,Install.Game)){Arguments=testPlayCapture?"--assets .":"",WorkingDirectory=installed,UseShellExecute=false,CreateNoWindow=true});Close();}catch(Exception err){ShowError(err);}});
        buttons.Controls.Add(play);
        if(testPlayCapture)Shown+=(sender,e)=>BeginInvoke(new Action(play.PerformClick));
        buttons.Controls.Add(ActionButton("Open game folder",(sender,e)=>{try{Process.Start("explorer.exe","\""+installed+"\"");}catch(Exception err){ShowError(err);}}));
        Add(buttons);
    }
}

