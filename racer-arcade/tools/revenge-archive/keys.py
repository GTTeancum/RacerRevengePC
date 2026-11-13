import sys,re,struct;sys.path.insert(0,'.')
import res
e,d=res.load(sys.argv[1])
for n,lo,sz in e:
    if n.lower().endswith(sys.argv[2]):
        b=d[lo:lo+sz]; assert b[:7]==b'BINARY\0'
        cnt,=struct.unpack_from('<I',b,11); off=15; names=[]
        for i in range(cnt):
            l,=struct.unpack_from('<I',b,off); off+=4; names.append(b[off:off+l-1].decode('latin1')); off+=l
        c={}
        for x in names:
            k=re.sub(r'\d+','#',x); c[k]=c.get(k,0)+1
        print(n,cnt,'string-table keys:',' '.join(f'{k}x{v}' if v>1 else k for k,v in c.items()))
