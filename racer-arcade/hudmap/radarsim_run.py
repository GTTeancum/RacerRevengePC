import sys,re,json,math,glob
sys.path.insert(0,r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/hudmap')
exec(open(r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/hudmap/splplot.py').read().split('srcs=')[0]); srcs={'orig':r'C:/Games/Star Wars Racer Revenge/TRACKS/SC.RES','eb':r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/silo/assets/TRACKS/SC.RES','lc':r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/silo/assets-lc/TRACKS/SC.RES'}
from PIL import Image,ImageDraw
EV=r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/silo/tree/evidence/'
SC=4*45/720/7
def track(run):
    rows=[]
    for l in open(EV+run+'/RacerRevenge.log',errors='replace'):
        if l.startswith('race '):
            s=float(re.search(r'seconds=([0-9.]+)',l).group(1)); v=re.search(r'racer1\[[^]]*\]=([^ ]*)',l).group(1).split(',')
            rows.append((s,float(v[3]),float(v[4])))
    return rows
def at(rows,t):
    for a,b in zip(rows,rows[1:]):
        if a[0]<=t<=b[0] and b[0]>a[0]:
            f=(t-a[0])/(b[0]-a[0]); return (a[1]+f*(b[1]-a[1]),a[2]+f*(b[2]-a[2])),(b[1]-a[1],b[2]-a[2])
    return None
def draw(d,n,cx,cz,hx,hz,col,ox,oy,k,mirror):
    L=math.hypot(hx,hz) or 1; fx,fz=hx/L,hz/L   # forward
    rx,rz=fz,-fx                                   # right (x,z) ; mirror flips
    if mirror: rx,rz=-rx,-rz
    def pr(p):
        dx,dz=p[0]-cx,p[2]-cz
        return ox+k*SC*(dx*rx+dz*rz), oy-k*SC*(dx*fx+dz*fz)
    for i,(p,nx) in n.items():
        for j in nx:
            d.line([pr(p),pr(n[j][0])],fill=col,width=3)
def run(name,src,mirror):
    rows=track(name); fs=sorted(glob.glob(EV+name+'/capture/race-session-2/track-*.ppm'),key=lambda s:int(re.findall(r'(\d+)\.ppm',s)[0]))
    orig=nodes(srcs['orig']); conv=nodes(srcs[src]); k=3.0
    W=270;H=260; sheet=Image.new('RGB',(W*len(fs),H*3),'black'); d=ImageDraw.Draw(sheet)
    for i,f in enumerate(fs):
        t=json.load(open(f[:-4]+'.json'))['race_seconds']
        crop=Image.open(f).crop((1018,150,1198,326)).resize((W,int(176*W/180)))
        sheet.paste(crop,(W*i,0))
        r=at(rows,t)
        for row,(nn,col) in enumerate([(conv,'cyan'),(orig,'red')]):
            pn=Image.new('RGB',(W,H),(20,20,50)); pd=ImageDraw.Draw(pn)
            if r: draw(pd,nn,r[0][0],r[0][1],r[1][0],r[1][1],col,W/2,H*0.72,k,mirror)
            pd.polygon([(W/2,H*0.72-8),(W/2-6,H*0.72+6),(W/2+6,H*0.72+6)],outline='yellow'); pd.rectangle([0,0,W-1,H-1],outline='gray')
            sheet.paste(pn,(W*i,H*(row+1)))
        d.text((W*i+4,4),'%.0fs'%t,fill='white')
    d.text((4,H+4),'converted msc.spl (sim)',fill='white'); d.text((4,2*H+4),'original SC msc.spl (sim)',fill='white')
    sheet.save('radar_compare_%s%s.png'%(name,'_m' if mirror else ''))
for m in ():
    run('silo-eb-frames-01','eb',m); run('silo-lc-frames-01','lc',m)
run('silo-eb-radar-01','eb',0)
