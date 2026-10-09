"""Offline bounded original-word interpreter for terrain-query evidence only.

Every fetched word comes from the hash-verified ELF extraction. Data reads come
from an immutable supplied snapshot or explicit fixtures; writes are an overlay.
Floating operations model individually rounded host IEEE binary32, ordinary
finite inputs. This is not proof of every physical EE/VU exceptional mode.
"""
import json
import math
import struct
from query_extract48h import ROOT, OUT

def fp(i): return struct.unpack('<f',struct.pack('<I',i&0xffffffff))[0]
def bits(x):
    try:return struct.unpack('<I',struct.pack('<f',x))[0]
    except OverflowError:return 0xff800000 if x<0 else 0x7f800000
def f32(x):return fp(bits(x))
def signed(x):return x-0x100000000 if x&0x80000000 else x
def div(a,b):
    if b==0:
        if a==0:return float('nan')
        return fp(((bits(a)^bits(b))&0x80000000)|0x7f800000)
    return f32(a/b)
def add(a,b):return f32(a+b)
def mul(a,b):return f32(a*b)
def sub(a,b):return f32(a-b)
def vu_minmax(x,y,maximum):
    """Finite VU lane selection, including -0 < +0; not a host float tie.

    Independently checked against PCSX2 VUops.cpp fp_min/fp_max/applyMinMax.
    Exceptional EE/VU arithmetic modes remain outside this bounded model.
    """
    a,b=signed(bits(x)),signed(bits(y))
    integer_max=maximum!=(a<0 and b<0)
    return fp((max(a,b) if integer_max else min(a,b))&0xffffffff)

class Original:
    def __init__(self, data=b'', words=None, *, legacy_vu_minmax=False):
        self.data=data
        # Audit-only reproduction of the archived pre-48P VU selection/VF0 bug.
        # Scalar EE MIN.S/MAX.S in SpringOriginal is unaffected by this switch.
        self.legacy_vu_minmax=legacy_vu_minmax
        self.overlay={}
        self.words=words or {w['pc']:w['word'] for r in json.loads((OUT/'original-words.json').read_text())['ranges'] for w in r['words']}
        self.g=[[0]*4 for _ in range(32)]
        self.f=[0.]*32
        self.v=[[0.]*4 for _ in range(32)]
        self.v[0][3]=1.
        self.acc=0.
        self.vacc=[0.]*4
        self.q=0.
        self.condition=False
        self.pc=0
        self.coverage=set()
        self.branches={}
        self.steps=0
        self.annul=False
        self.source_reads=set()
        self.allowed_source_addresses=None
    def read(self,a,size=4):
        if a<0 or a+size>0x2000000:raise ValueError(f'out-of-range data read {a:08x} pc={self.pc:08x}')
        v=0
        for i in range(size):
            b=self.overlay.get(a+i)
            if b is None:
                if a+i>=len(self.data):raise ValueError(f'uninitialized fixture read {a+i:08x} pc={self.pc:08x}')
                if self.allowed_source_addresses is not None and a+i not in self.allowed_source_addresses:
                    raise ValueError(f'read outside preserved data slices {a+i:08x} pc={self.pc:08x}')
                b=self.data[a+i]
                self.source_reads.add(a+i)
            v|=b<<(8*i)
        return v
    def write(self,a,x,size=4):
        if a<0 or a+size>0x2000000:raise ValueError(f'out-of-range data write {a:08x}')
        for i in range(size):self.overlay[a+i]=(x>>(8*i))&255
    def loadf(self,a):return fp(self.read(a))
    def storef(self,a,x):self.write(a,bits(x))
    def vector(self,a,x):
        for i,v in enumerate(x):self.storef(a+4*i,v)
    def vec(self,a,n=3):return [self.loadf(a+4*i) for i in range(n)]
    def execute(self,pc):
        self.pc=pc;self.steps+=1;self.coverage.add(pc);self.annul=False
        if self.steps>1000000:raise ValueError('instruction budget')
        if pc not in self.words:raise ValueError(f'PC outside extracted words {pc:08x}')
        w=self.words[pc];op=w>>26;rs=(w>>21)&31;rt=(w>>16)&31;rd=(w>>11)&31;fd=(w>>6)&31;fn=w&63
        imm=w&65535;imm=imm-65536 if imm&32768 else imm
        a=(self.g[rs][0]+imm)&0xffffffff
        def branch(take,likely=False):
            self.branches.setdefault(pc,set()).add(bool(take))
            self.annul=likely and not take
            return pc+4+imm*4 if take else pc+8
        if w==0:pass
        elif op in (9,0x19):self.g[rt][0]=a
        elif op==15:self.g[rt][0]=(w&65535)<<16
        elif op==12:self.g[rt][0]=self.g[rs][0]&(w&65535)
        elif op==13:self.g[rt][0]=self.g[rs][0]|(w&65535)
        elif op==11:self.g[rt][0]=int(self.g[rs][0]<(imm&0xffffffff))
        elif op==10:self.g[rt][0]=int(signed(self.g[rs][0])<imm)
        elif op==0x23:self.g[rt][0]=self.read(a)
        elif op in (0x21,0x25):
            z=self.read(a,2);self.g[rt][0]=(z-65536 if op==0x21 and z&32768 else z)&0xffffffff
        elif op in (0x20,0x24):
            z=self.read(a,1);self.g[rt][0]=(z-256 if op==0x20 and z&128 else z)&0xffffffff
        elif op in (0x28,0x29,0x2b):self.write(a,self.g[rt][0],{0x28:1,0x29:2,0x2b:4}[op])
        elif op==0x1e:self.g[rt]=[self.read(a+4*i) for i in range(4)]
        elif op==0x1f:
            for i in range(4):self.write(a+4*i,self.g[rt][i])
        elif op==0x31:self.f[rt]=self.loadf(a)
        elif op==0x39:self.storef(a,self.f[rt])
        elif op==0x36:self.v[rt]=self.vec(a,4)
        elif op==0x3e:self.vector(a,self.v[rt])
        elif op in (4,5,0x14,0x15):return branch((self.g[rs][0]==self.g[rt][0])==(op in (4,0x14)),op>=0x14)
        elif op==6:return branch(signed(self.g[rs][0])<=0)
        elif op==7:return branch(signed(self.g[rs][0])>0)
        elif op==3:self.g[31][0]=pc+8;return (w&0x3ffffff)*4
        elif op==2:return (w&0x3ffffff)*4
        elif op==0:
            if fn==0:self.g[rd][0]=self.g[rt][0]<<fd
            elif fn==2:self.g[rd][0]=self.g[rt][0]>>fd
            elif fn==3:self.g[rd][0]=signed(self.g[rt][0])>>fd
            elif fn==0x21:self.g[rd][0]=self.g[rs][0]+self.g[rt][0]
            elif fn==0x23:self.g[rd][0]=self.g[rs][0]-self.g[rt][0]
            elif fn==0x24:self.g[rd][0]=self.g[rs][0]&self.g[rt][0]
            elif fn==0x25:self.g[rd][0]=self.g[rs][0]|self.g[rt][0]
            elif fn==0x2a:self.g[rd][0]=int(signed(self.g[rs][0])<signed(self.g[rt][0]))
            elif fn in (0x3c,0x3e):
                value=self.g[rt][0]|(self.g[rt][1]<<32)
                value=((value<<(fd+32))&0xffffffffffffffff) if fn==0x3c else value>>(fd+32)
                self.g[rd][0]=value&0xffffffff;self.g[rd][1]=value>>32
            elif fn==8:return self.g[rs][0]
            elif fn==9:
                target=self.g[rs][0];self.g[rd][0]=pc+8;return target
            else:raise ValueError(f'unsupported SPECIAL {w:08x} pc={pc:08x}')
            self.g[rd][0]&=0xffffffff
        elif (w&0xfc0007ff)==0x70000628 and rt==0:self.g[rd]=self.g[rs].copy()
        elif op==0x11:
            if rs==4:self.f[rd]=fp(self.g[rt][0])
            elif rs==0:self.g[rt][0]=bits(self.f[rd])
            elif rs==8:return branch(self.condition==bool(rt&1),bool(rt&2))
            elif rs==20 and fn==0x20:self.f[fd]=f32(signed(bits(self.f[rd])))
            elif rs==16:
                x,y=self.f[rd],self.f[rt]
                if fn==0:self.f[fd]=add(x,y)
                elif fn==1:self.f[fd]=sub(x,y)
                elif fn==2:self.f[fd]=mul(x,y)
                elif fn==3:self.f[fd]=div(x,y)
                elif fn==4:self.f[fd]=f32(math.sqrt(abs(y)))
                elif fn==5:self.f[fd]=abs(x)
                elif fn==6:self.f[fd]=x
                elif fn==7:self.f[fd]=-x
                elif fn==0x16:self.f[fd]=f32(x/math.sqrt(abs(y))) if y else fp(0x7f7fffff|(bits(y)&0x80000000))
                elif fn==0x18:self.acc=add(x,y)
                elif fn==0x19:self.acc=sub(x,y)
                elif fn==0x1a:self.acc=mul(x,y)
                elif fn==0x1c:self.f[fd]=add(self.acc,mul(x,y))
                elif fn==0x1d:self.f[fd]=sub(self.acc,mul(x,y))
                elif fn==0x32:self.condition=x==y
                elif fn==0x34:self.condition=x<y
                elif fn==0x36:self.condition=x<=y
                else:raise ValueError(f'unsupported FPU {w:08x} pc={pc:08x}')
            else:raise ValueError(f'unsupported COP1 {w:08x} pc={pc:08x}')
        elif op==0x12:
            if rs==1:self.g[rt]=[bits(x) for x in self.v[rd]]
            elif rs>=16:
                mask=rs&15;x=self.v[rd].copy();y=self.v[rt].copy();result=self.v[fd].copy()
                def active(i):return mask&(8>>i)
                if (w&0x7ff)==0x1fd:
                    for i in range(4):
                        if active(i):self.v[rt][i]=abs(x[i])
                elif (w&0x7ff)==0x2fe:
                    for i in range(3):self.vacc[i]=mul(x[(i+1)%3],y[(i+2)%3])
                elif fn==0x2e:
                    for i in range(3):result[i]=sub(self.vacc[i],mul(x[(i+1)%3],y[(i+2)%3]))
                    self.v[fd]=result
                elif w==0x4a6503be:self.q=div(1.,f32(math.sqrt(abs(self.v[5][0])))) if self.v[5][0] else fp(0x7f7fffff|(bits(self.v[5][0])&0x80000000))
                elif w==0x4a0003bf:pass
                elif (w&0x7ff) in (0x1bc,0x1bd,0x1be,0x1bf,0x0bd,0x0bc,0x0be,0x0bf,0x03c,0x03d,0x03e,0x03f):
                    low=w&0x7ff
                    for i in range(4):
                        if active(i):
                            if low&0x180==0x180:self.vacc[i]=mul(x[i],y[low&3])
                            elif low&0x80:self.vacc[i]=add(self.vacc[i],mul(x[i],y[low&3]))
                            else:self.vacc[i]=add(x[i],y[low&3])
                else:
                    for i in range(4):
                        if not active(i):continue
                        if fn<=3:result[i]=add(x[i],y[fn])
                        elif 4<=fn<=7:result[i]=sub(x[i],y[fn-4])
                        elif 8<=fn<=11:result[i]=add(self.vacc[i],mul(x[i],y[fn-8]))
                        elif 24<=fn<=27:result[i]=mul(x[i],y[fn-24])
                        elif fn==0x1c:result[i]=mul(x[i],self.q)
                        elif fn==0x28:result[i]=add(x[i],y[i])
                        elif fn==0x2a:result[i]=mul(x[i],y[i])
                        elif fn==0x2b:result[i]=max(x[i],y[i]) if self.legacy_vu_minmax else vu_minmax(x[i],y[i],True)
                        elif fn==0x2c:result[i]=sub(x[i],y[i])
                        elif fn==0x2f:result[i]=min(x[i],y[i]) if self.legacy_vu_minmax else vu_minmax(x[i],y[i],False)
                        else:raise ValueError(f'unsupported VU {w:08x} pc={pc:08x}')
                    if fd or fn not in (0x2b,0x2f) or self.legacy_vu_minmax:self.v[fd]=result
            else:raise ValueError(f'unsupported COP2 {w:08x} pc={pc:08x}')
        else:raise ValueError(f'unsupported word {w:08x} pc={pc:08x}')
        self.g[0]=[0]*4
        return None
    def run(self,pc):
        self.g[29][0]=0x1ff0000;self.g[31][0]=0xfffffff0
        while pc!=0xfffffff0:
            target=self.execute(pc)
            if target is not None:
                if not self.annul:
                    other=self.execute(pc+4)
                    if other is not None:raise ValueError('branch in delay slot')
                pc=target
            else:pc+=4
        return self.g[2][0]
