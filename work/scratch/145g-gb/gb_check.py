# Static check for helper 145g task 4 (GB through the 145c common readers).
# Mirrors src/assets/original_particle_emitters50pe.h, the new descriptor/definition
# mapping in tests/native/gb_particle_authoring50gb.h, and
# src/assets/original_positional_sounds50sr.h + positional_sound_rows50sr.h over the
# stock GB.RES; compares with the OLD pinned GB tables (removed in this change).
#   python -I gb_check.py > gb_check.txt
import sys,struct
sys.path.insert(0,r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-ae92ecc69f2d930ba\work\scratch\145c')
from survey import doc,load_res,T
def f32(x):return struct.unpack('<f',struct.pack('<f',float(x)))[0]
def int158(s):
    c=s[:1]
    if c in('T','t'):return 1
    if c in('F','f'):return 0
    if s.upper()=='ON':return 1
    if s.upper()=='OFF':return 0
    if c in('Y','y'):return 1
    if c in('N','n'):return 0
    return int(s,0)
m=load_res(T+r'\GB.RES');tob=doc(m['tracks\\gb.tob'][1]);by={n:f for n,f in tob}
ok=True
# ---- particles
n=int158(dict(by['particleemitters'])['numberofemitters'])
rows=[]
for i in range(1,n+1):
    b=dict(by['particleemitter%d'%i])
    rows.append(dict(zone=int158(b.get('startingzone','0')),type=int158(b.get('typeid','0')),attach=b.get('attachtosoultree','NONE'),
        child=b.get('childobject','NONE'),off=tuple(f32(x) for x in b['offsetfromsoultree'].split(',')),
        inst=int158(b.get('numberofinstances','1')),file=b.get('emitterfilename','NONE'),starton=int158(b.get('starton','1')),authStart='starton' in b))
attach=["gam_temple.slt","stone_head.slt","stone_head.slt","pig_head.slt","pig_head.slt","gam_temple.slt","shade.slt","startgate.slt","startgate.slt","startgate.slt","startgate.slt","startgate.slt"]
names=["gbtemplefire.emt"]*6+["genratorfire.emt","gbfireworks.emt","gbfireworks.emt","gbredfireworks.emt","gbredfireworks.emt","gbredfireworks.emt"]
counts=[1,4,4,3,3,1,46,1,1,1,1,1];defs_old=[0,0,0,0,0,0,1,2,2,3,3,3]
offs=[(-271,190,-150),(69.10,36.2,-76.4),(-69.10,36.2,-76.4),(139.9,72.5,-152.9),(-139.9,72.5,-152.9),(260,190,-150),(-4.3,46.0,0),(-154.8,111.6,36.5),(154.8,111.6,36.5),(234.5,248.1,126.3),(-234.5,248.1,126.3),(-3.8,207.3,181.8)]
dn=[];defs_new=[]
for r in rows:
    k=[i for i,x in enumerate(dn) if x.lower()==r['file'].lower()]
    if not k:dn.append(r['file']);k=[len(dn)-1]
    defs_new.append(k[0])
pe=len(rows)==12
for i,r in enumerate(rows):
    pe&=r['zone']==1 and r['type']==(8 if i<7 else 11) and r['attach']==attach[i] and r['child']=='NONE' and r['inst']==counts[i] \
        and r['file']==names[i] and r['off']==tuple(f32(x) for x in offs[i]) and r['starton']==(1 if i<7 else 0) and r['authStart']==(i>=6)
pe&=defs_new==defs_old and [x for x in dn]==["gbtemplefire.emt","genratorfire.emt","gbfireworks.emt","gbredfireworks.emt"]
print('particle_emitters site=GB authored=%d read=%d types=%s'%(n,len(rows),','.join(str(r['type']) for r in rows)))
print('GB emitters: reader table equals old pinned descriptors (type/zone/soultree/instances/filename/offset bits/StartOn) and definition index map:',pe)
print('  instances total',sum(r['inst'] for r in rows),'definitions',dn)
ok&=pe
# SCB environment atlas name (by-name members)
scbs=[k for k in m if k.lower().endswith('.scb')]
atlas=None
for k in scbs:
    try:
        for nme,fl in doc(m[k][1]):
            if nme=='environment':
                for a,v in fl:
                    if a=='particletexturemapname':atlas=v
    except Exception as e:pass
print('SCB environment ParticleTextureMapName:',atlas,'members present:',[x for x in ('particle\\demoprt.prt','particle\\demoprt.psm','particle\\demoprt.pst') if x in m])
ok&=atlas is not None and atlas.lower().split('.')[0]=='demoprt'
# ---- sounds
aub=doc(m['tracks\\gb.aub'][1]);ab={n:f for n,f in aub}
def num(b,k,d):
    v=dict(b).get(k);return d if v is None else float(v)
def ival(b,k,d):
    v=dict(b).get(k);return d if v is None else int158(v)
snd=dict(ab['sounds'])
ns=int158(snd.get('numsoultreesounds','0'));na=int158(snd.get('numambientsounds','0'))
new=[];old=[];reqn=[];reqo=[]
def oflag(b,k):
    v=dict(b).get(k)
    if v is None:return False
    assert v in('T','t','1','F','f','0');return v in('T','t','1')
for i in range(1,ns+1):
    b=ab['soultreesound%d'%i];d=dict(b)
    assert 'attachtoobject' not in d and not ival(b,'force2d',0) and not ival(b,'stereo',0) and not ival(b,'startdisabled',0) and 'position' in d
    name=d['soundresourcefile'].replace('\\','/')+'.vag';pos=tuple(f32(x) for x in d['position'].split(','))
    new.append((name,pos,num(b,'mindistance',0),num(b,'maxdistance',0),num(b,'oneshotdistance',20),num(b,'randomtriggerpercent',100),
        ival(b,'looping',0)!=0,ival(b,'oneshot',0)!=0,ival(b,'resetwheninaudible',0)!=0))
    old.append((name,pos,num(b,'mindistance',0),num(b,'maxdistance',0),num(b,'oneshotdistance',20),num(b,'randomtriggerpercent',100),
        oflag(b,'looping'),oflag(b,'oneshot'),oflag(b,'resetwheninaudible')))
    if name not in reqn:reqn.append(name)
amb=[]
for i in range(1,na+1):
    b=ab['ambientsound%d'%i];d=dict(b)
    amb.append((d['soundresourcefile'].replace('\\','/')+'.vag',num(b,'cycletime',0),num(b,'randomtriggerpercent',100)))
oldamb=[("sfx/race/rain.vag",0,100),("sfx/race/thunder1.vag",20,35),("sfx/race/thunder2.vag",18,45),("sfx/race/thunder3.vag",23,40)]
so=new==old and ns==49 and len(reqn)==4 and amb==oldamb
print('positional_sounds site=positional_GB soultree_read=%d ambient_read=%d pcm=%d'%(ns,na,len(reqn)))
print('GB sounds: reader projection equals old GB parse (49 rows) and old pinned ambient rows:',so)
ok&=so
print('ALL GB EQUAL' if ok else 'MISMATCH')
