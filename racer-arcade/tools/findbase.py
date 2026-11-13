# Find load base: count aligned 32-bit words v where v-B is the start of a known string.
import sys, numpy as np, collections
img=open(sys.argv[1],'rb').read()
strs=set(int(l.split()[0],16) for l in open(sys.argv[2]) if l.strip())
w=np.frombuffer(img[:len(img)//4*4],dtype='<u4')
vals=set(w.tolist())
top=collections.Counter((v>>20) for v in w.tolist() if 0x0C000000<=v<0x0E000000)
print("high-nibble hist of 0x0C-0x0D words:", [(hex(k<<20),c) for k,c in top.most_common(12)])
res=[]
for B in range(0x0BF00000,0x0C200000,0x400):
    c=sum(1 for s in strs if (B+s) in vals)
    res.append((c,B))
res.sort(reverse=True)
for c,B in res[:10]: print(hex(B),c)
