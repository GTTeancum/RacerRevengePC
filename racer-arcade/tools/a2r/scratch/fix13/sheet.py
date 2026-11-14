import sys,glob,os,re
from PIL import Image
ev=r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\silo\tree\evidence'
run=sys.argv[1]; pick=[int(x) for x in sys.argv[2].split(',')] if len(sys.argv)>2 else None
fs=sorted(glob.glob(os.path.join(ev,run,'capture','race-session-*','track-*.ppm')),key=lambda f:int(re.findall(r'track-(\d+)',f)[0]))
if pick: fs=[f for f in fs if int(re.findall(r'track-(\d+)',f)[0]) in pick]
ims=[Image.open(f).convert('RGB') for f in fs]
w,h=ims[0].size; s=float(sys.argv[3]) if len(sys.argv)>3 else 0.5; cols=2
out=Image.new('RGB',(int(w*s)*cols,int(h*s)*((len(ims)+cols-1)//cols)))
for i,im in enumerate(ims): out.paste(im.resize((int(w*s),int(h*s))),((i%cols)*int(w*s),(i//cols)*int(h*s)))
p=os.path.join(os.path.dirname(os.path.abspath(__file__)),'sheet_%s.png'%run); out.save(p); print(p,[os.path.basename(f) for f in fs])
