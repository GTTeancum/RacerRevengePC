import struct,zlib,sys
def load(p):
    b=open(p,'rb').read()
    assert struct.unpack_from('<I',b,0)[0]==3
    meta,=struct.unpack_from('<I',b,4); tb,=struct.unpack_from('<I',b,16)
    n=tb//2-1; sizes=struct.unpack_from('<%dH'%n,b,20)
    off=20+tb; cnt,ib=struct.unpack_from('<II',b,off); off+=8
    ents=[]
    for i in range(cnt):
        l,=struct.unpack_from('<I',b,off); off+=4
        name=b[off:off+l].decode(); off+=l
        lo,sz=struct.unpack_from('<II',b,off); off+=8
        ents.append((name,lo,sz))
    out=bytearray()
    for s in sizes:
        out+=zlib.decompress(b[off:off+s]); off+=s+1
    return ents,bytes(out)
def member(p,name):
    e,d=load(p)
    for n,lo,sz in e:
        if n.lower()==name.lower(): return d[lo:lo+sz]
if __name__=='__main__':
    e,d=load(sys.argv[1])
    for n,lo,sz in e: print(sz,n)
