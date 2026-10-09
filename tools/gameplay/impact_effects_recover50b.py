"""Original223540/helper/getter/lookup/RNG proof; explicit effect output boundary."""
from pathlib import Path
import argparse,hashlib,json,random,struct
from spring_fixture48j import SpringOriginal
from query_oracle48h import bits,fp,f32,signed
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'work/local50B/impact-effects'
BLOCKS={'effects':(0x223540,0x224368),'position':(0x28b1a0,0x28b2b0),'refresh':(0x2897b0,0x28993c),'product':(0x289940,0x2899b4),'lookup':(0x28c590,0x28c6e0),'rng':(0x308030,0x308060),'name-compare':(0x308f00,0x308fb4)}
class EffectOriginal(SpringOriginal):
    def __init__(self,words):super().__init__(words=words);self.events=[];self.effect_ids={};self.libcalls=0
    def execute(self,pc):
        w=self.words[pc]
        if w>>26==0 and w&63 in (0x2d,0x0a): # Original DADDU/MOVZ, full low64.
            self.pc=pc;self.coverage.add(pc);self.steps+=1;self.annul=False
            rs=(w>>21)&31;rt=(w>>16)&31;rd=(w>>11)&31
            a=self.g[rs][0]|(self.g[rs][1]<<32);b=self.g[rt][0]|(self.g[rt][1]<<32)
            if w&63==0x2d:
                value=(a+b)&0xffffffffffffffff;self.g[rd][0]=value&0xffffffff;self.g[rd][1]=value>>32
            elif b==0:self.g[rd][:2]=self.g[rs][:2]
            return None
        if w>>26==0 and w&63==0x18: # Actual EE three-operand signedMULT; lowword rd.
            self.pc=pc;self.coverage.add(pc);self.steps+=1;self.annul=False
            self.g[(w>>11)&31][0]=(signed(self.g[(w>>21)&31][0])*signed(self.g[(w>>16)&31][0]))&0xffffffff;return None
        return super().execute(pc)
    def run_effects(self):
        pc=0x223540;self.g[29][0]=0x1ff0000;self.g[31][0]=0xfffffff0
        while pc!=0xfffffff0:
            if pc==0x28c590 and not self.g[4][0]:return 3 # Actual missing outerdamage00 anchor.
            if pc==0x308f00:self.libcalls+=1 # Count entry; execute every original word below.
            if pc in (0x1f00000,0x1f00004,0x1f00008,0x15acb0,0x19a810,0x15fbf0):
                a=self.g[4][0];target=self.effect_ids[a];caller=self.g[31][0]-8;values=[0]*4;arg=0
                kind={0x1f00000:0,0x1f00004:1,0x15acb0:2,0x19a810:3,0x15fbf0:4,0x1f00008:5}[pc]
                if kind in (2,4):values=[self.read(self.g[5][0]+j*4) for j in range(4)]
                if kind==3:values[0]=bits(self.f[12])
                if kind==5:arg=self.g[5][0]
                self.events +=[kind,caller,target,arg]+values;pc=self.g[31][0];continue
            target=self.execute(pc)
            if target is None:pc+=4
            else:
                if not self.annul:assert self.execute(pc+4) is None
                pc=target
        return 0
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--elf',type=Path,required=True);a=ap.parse_args();data=a.elf.read_bytes()
    assert hashlib.sha256(data).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
    words={};blocks=[];OUT.mkdir(parents=True,exist_ok=True)
    for name,(lo,hi) in BLOCKS.items():
        raw=data[lo-0x100000+0x80:hi-0x100000+0x80];rows=[dict(pc=p,word=struct.unpack_from('<I',raw,p-lo)[0]) for p in range(lo,hi,4)]
        words.update({r['pc']:r['word'] for r in rows});blocks.append(dict(name=name,sha256=hashlib.sha256(raw).hexdigest(),instructions=rows))
    (OUT/'original-words.json').write_text(json.dumps(dict(elf_sha256=hashlib.sha256(data).hexdigest(),blocks=blocks),indent=2)+'\n')
    table=data[0x3c6311-0x100000+0x80:0x3c6411-0x100000+0x80]
    (OUT/'name-compare-table.bin').write_bytes(table)
    comparisons=[]
    for name in (b'damage00',b'DAMAGE00',b'Damage00',b'DaMaGe00',b'damage01',b'DAMAGE01',b'damage00x',b'',b'other'):
        c=EffectOriginal(words)
        for j,x in enumerate(table):c.write(0x3c6311+j,x,1)
        for address,string in ((0x10000,name),(0x11000,b'damage00')):
            for j,x in enumerate(string+b'\0'):c.write(address+j,x,1)
        c.g[4][0]=0x10000;c.g[5][0]=0x11000;c.run(0x308f00)
        comparisons.append(dict(name=name.decode(),original_result=c.g[2][0],literal_match=name==b'damage00',steps=c.steps))
    assert all(r['original_result']==0 for r in comparisons[:4])
    assert all(r['original_result']!=0 for r in comparisons[4:])
    (OUT/'name-compare-controls.json').write_text(json.dumps(dict(table_base='3C6311',table_sha256=hashlib.sha256(table).hexdigest(),cases=comparisons),indent=2)+'\n')
    rng=random.Random(0x22354050);records=[];coverage=set();branches={};steps=libcalls=0;maxevents=0;previousrng=None;name_records=[]
    for k in range(1024):
        ins=[];names=[0]*12;nodes=[]
        for j in range(4):
            model=j*3 if rng.randrange(8) else 0xffffffff;flags=rng.randrange(8)
            effects=sum(([j*3+t+1 if rng.randrange(5) else 0,rng.randrange(4)] for t in range(3)),[])
            ins += [model,flags]+effects
            names[j*3+rng.choice([0,1,2])]=1
            for t in range(3):
                local=[1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,float(j+1),float(t+2),float(k%17),1.]
                world=[x*2 for x in local];world[15]=1.
                parent=0xffffffff if t==0 else j*3;child=j*3+1 if t==0 else 0xffffffff;sibling=j*3+2 if t==1 else 0xffffffff
                nodes += [bits(x) for x in local+world]+[parent,child,sibling,rng.randrange(4)]
        damages=[bits(f32(rng.choice([.1,.4,.5,.7,.8,.99]))) for j in range(4)]
        ins +=damages+[bits(f32(rng.choice([0.,-.1,.1,.25]))) for j in range(5)]+[bits(float(j+k%7)) for j in range(4)]+[13 if k%3 else 0]
        if k%16==0:ins[:32]=sum(([j*3,0,j*3+1,0,j*3+2,0,j*3+3,0] for j in range(4)),[]);ins[32:36]=[bits(.8)]*4;ins[36:40]=[bits(.1)]*4
        if k%16==1:names=[0]*12
        if k%16==2:ins[32:36]=[bits(fp(0x3ecccccc))]*4
        if k%16==3:ins[32:36]=[bits(fp(0x3eccccce))]*4
        rawrng=rng.randrange(0x100000000) if k%4==0 or previousrng is None else previousrng
        original=EffectOriginal(words);o=original;v=0x10000;body=0x18000
        for j,x in enumerate(table):o.write(0x3c6311+j,x,1)
        def address(index):return 0 if index==0xffffffff else 0x20000+index*0x200
        fixture_names=[]
        for j in range(12):
            a=address(j);row=nodes[j*36:(j+1)*36];o.vector(a+0xc0,[fp(w) for w in row[:16]]);o.vector(a+0x100,[fp(w) for w in row[16:32]])
            for off,index in zip([0x180,0x184,0x188],row[32:35]):o.write(a+off,address(index))
            o.write(a+0x190,row[35],1)
            name=(b'damage00',b'DAMAGE00',b'Damage00',b'DaMaGe00')[(k//16+j)%4] if names[j] else (b'other',b'DAMAGE01',b'damage00x',b'')[(k+j)%4]
            fixture_names.append(name.decode())
            for n,x in enumerate(name+b'\0'):o.write(a+0x38+n,x,1)
        for j,off in enumerate([0x1bc,0x1b8,0x1c4,0x1c0]):
            m=ins[j*8:(j+1)*8];o.write(v+off,address(m[0]))
            if m[0]!=0xffffffff:o.write(address(m[0])+3,m[1],1)
            for t,array in enumerate([0x800,0x804,0x80c]):
                aid=0x40000+t*0x100;eid=m[2+t*2];eaddr=0x50000+eid*0x200 if eid else 0
                o.write(v+array,aid);o.write(aid+j*4,eaddr)
                if eid:o.effect_ids[eaddr]=eid;o.write(eaddr+0x1d4,m[3+t*2],1);o.write(eaddr+8,0x60000)
        for off,w in zip(list(range(0x7d0,0x7e0,4))+list(range(0x820,0x834,4)),ins[32:41]):o.write(v+off,w)
        o.write(v+0x340,body)
        for j,w in enumerate(ins[41:45]):o.write(body+0x70+j*4,w)
        o.write(v+0xd40,0x70000 if ins[45] else 0);o.effect_ids[0x70000]=13;o.write(0x70008,0x60000)
        for off,target in [(0x80,0x1f00000),(0x84,0x1f00004),(0xa8,0x1f00008)]:o.write(0x60000+off,target)
        for j,x in enumerate(b'damage00\0'):o.write(0x3b2d80+j,x,1)
        o.write(0x38544c,0x71000);o.write(0x71058,rawrng);o.g[4][0]=v
        name_records.append(dict(row=k,names=fixture_names,source_matches=names,literal_matches=[int(n=='damage00') for n in fixture_names]))
        status=o.run_effects();assert not o.source_reads
        resultnodes=[]
        for j in range(12):
            a=address(j);resultnodes += [o.read(a+0xc0+z*4) for z in range(16)]+[o.read(a+0x100+z*4) for z in range(16)]+nodes[j*36+32:j*36+35]+[o.read(a+0x190,1)]
        nextrng=o.read(0x71058);ev=o.events
        if status:resultnodes=nodes;nextrng=rawrng;ev=[]
        previousrng=nextrng;maxevents=max(maxevents,len(ev)//8)
        record=ins+names+[rawrng]+nodes+[status,nextrng]+resultnodes+[len(ev)//8]+ev+[0]*(328-len(ev));assert len(record)==1254;records.append(record)
        coverage.update(o.coverage);steps+=o.steps;libcalls+=o.libcalls
        for pc,t in o.branches.items():branches.setdefault(pc,set()).update(t)
    assert any(r['source_matches']!=r['literal_matches'] for r in name_records)
    (OUT/'fixture-names.json').write_text(json.dumps(name_records,indent=2)+'\n')
    raw=struct.pack('<II',len(records),1254)+b''.join(struct.pack('<1254I',*r) for r in records);(OUT/'fixtures.bin').write_bytes(raw)
    report=dict(cases=len(records),record_words=1254,steps=steps,name_compare_source_calls=libcalls,strcmp_primitive_calls=0,mixed_case_fixture_names=True,maximum_events=maxevents,source_reads=0,fixture_sha256=hashlib.sha256(raw).hexdigest(),coverage=[hex(p) for p in sorted(coverage)],branches={hex(p):sorted(v) for p,v in sorted(branches.items())},scope='Original223540/224170/224250/2242D0,28C590,28B1A0,2897B0,289940 308030 and308F00 plus original case-table bytes. Only actualbackendoutputcalls are explicit boundaries; scene lookup/cache/RNG are not substituted. Missing outerdamage00 anchor rejected.')
    (OUT/'fixtures.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')}))
if __name__=='__main__':main()
