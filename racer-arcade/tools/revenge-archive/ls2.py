import sys,re,collections;sys.path.insert(0,'.')
import res
R=r'C:/Games/Star Wars Racer Revenge/TRACKS/'
for t in sys.argv[1:]:
    e,d=res.load(R+t+'.RES')
    c=collections.Counter(); nonimg=[]
    BS=chr(92)
    for n,lo,sz in e:
        ext=n.rsplit('.',1)[-1].lower(); folder=n.split(BS)[0].lower() if BS in n else '-'
        c[(folder,ext)]+=1
        if ext not in ('psg','pst','psm','tga','col','emt','atm','mot','vut','mcf','vag'): nonimg.append(n+':'+str(sz))
    print(t,len(e),dict(sorted(c.items())))
    print('  ',' '.join(nonimg))
