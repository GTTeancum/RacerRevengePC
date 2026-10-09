"""Original225C50 first-call/no-call-return proof, original ELF only."""
from pathlib import Path
import argparse,hashlib,json,random,struct,re
from spring_fixture48j import SpringOriginal
from query_oracle48h import bits,fp,f32
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'work/local50C/impact-transitions'
BLOCKS={'decision225C50':(0x225c50,0x2261b0),'handler88':(0x2261b0,0x226750),'handler8C':(0x226750,0x227370),'handler90':(0x227370,0x227a40),'handler94':(0x227a40,0x228110),'outer-base-tail':(0x22888c,0x228f20),'plain-tail228390':(0x228390,0x228620),'tail2291D0':(0x2291d0,0x229770),'attached-tail22A620':(0x22a620,0x22addc),'optional2179D0':(0x2179d0,0x217de0)}
TARGETS={0x2261b0:0x88,0x226750:0x8c,0x227370:0x90,0x227a40:0x94}
class TransitionOriginal(SpringOriginal):
    def run_decision(self):
        self.g[29][0]=0x1ff0000;self.g[31][0]=0xfffffff0;pc=0x225c50
        while pc!=0xfffffff0:
            if pc in TARGETS:return [1,self.g[31][0]-8,TARGETS[pc],pc]
            target=self.execute(pc)
            if target is None:pc+=4
            else:
                if not self.annul:assert self.execute(pc+4) is None
                pc=target
        return [0,0x2261a4,0,0]
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--elf',type=Path,required=True);ap.add_argument('--generated',type=Path,required=True);a=ap.parse_args();data=a.elf.read_bytes()
    assert hashlib.sha256(data).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
    OUT.mkdir(parents=True,exist_ok=True);words={};annotations={};blocks=[]
    for prefix in ('217','225','226','227','228','229','22a'):
        for p in a.generated.glob('*0x'+prefix+'*.cpp'):
            for m in re.finditer(r'// 0x([0-9a-fA-F]+): 0x([0-9a-fA-F]+)\s+([^\r\n]+)',p.read_text()):
                pc=int(m[1],16)
                if any(lo<=pc<hi for lo,hi in BLOCKS.values()):
                    assert struct.unpack_from('<I',data,pc-0x100000+0x80)[0]==int(m[2],16);annotations[pc]=m[3]
    for name,(lo,hi) in BLOCKS.items():
        raw=data[lo-0x100000+0x80:hi-0x100000+0x80];rows=[]
        for pc in range(lo,hi,4):
            w=struct.unpack_from('<I',raw,pc-lo)[0];words[pc]=w;rows.append(dict(pc=pc,word=w,annotation=annotations.get(pc,'')))
        calls=[dict(pc=hex(r['pc']),target=hex((r['word']&0x3ffffff)*4)) for r in rows if r['word']>>26==3]
        indirect=[hex(r['pc']) for r in rows if r['word']>>26==0 and r['word']&63==9]
        stores=[dict(pc=hex(r['pc']),base=(r['word']>>21)&31,offset=hex(r['word']&65535),annotation=r['annotation']) for r in rows if r['word']>>26 in (0x28,0x29,0x2b,0x39,0x1f,0x3e)]
        blocks.append(dict(name=name,sha256=hashlib.sha256(raw).hexdigest(),instructions=rows,direct_calls=calls,indirect_call_pcs=indirect,stores=stores))
        (OUT/(name+'.txt')).write_text(''.join(f"{r['pc']:08X} {r['word']:08X} {r['annotation']}\n" for r in rows))
    vtable={slot:struct.unpack_from('<I',data,0x3d1790+slot-0x100000+0x80)[0] for slot in [0x88,0x8c,0x90,0x94]};assert all(vtable[s]==t for t,s in TARGETS.items())
    (OUT/'original-words.json').write_text(json.dumps(dict(elf_sha256=hashlib.sha256(data).hexdigest(),vtable_base='3D1790',vtable=vtable,blocks=blocks),indent=2)+'\n')
    rng=random.Random(0x225c5050);values=[bits(0.),0x3f666665,0x3f666666,0x3f666667,0x3f7c28f5,0x3f7c28f6,0x3f7c28f7,bits(1.)];records=[];coverage=set();branches={};steps=0;results={}
    for k in range(8192):
        ins=[values[(k>>(j*3))&7] for j in range(4)]+[0,bits(1.),0]
        if k>=4096:
            ins[:4]=[bits(f32(rng.choice([fp(rng.choice(values)),rng.uniform(-1.,2.)]))) for j in range(4)]
            ins[4:]=[bits(f32(rng.choice([-1.,0.,1.,.5]))),bits(f32(rng.choice([-1.,0.,.5,1.]))),rng.choice([0,0,0,1,2,255])]
        o=TransitionOriginal(words=words);v=0x10000;o.g[4][0]=v;o.write(v+8,0x18000)
        for slot,t in vtable.items():o.write(0x18000+slot,t)
        for j,w in enumerate(ins[:4]):o.write(v+0x7d0+j*4,w)
        o.write(v+0xba8,ins[4]);o.write(v+0x50,ins[5]);o.write(v+0xb9d,ins[6],1)
        before=o.overlay.copy();result=o.run_decision();assert not o.source_reads
        assert all(o.overlay[a]==b for a,b in before.items()),'no gameplay writes before first call/return'
        assert all(a in before or 0x1feff90<=a<0x1ff0000 for a in o.overlay),'only source stack writes'
        records.append(ins+result);coverage.update(o.coverage);steps+=o.steps;results[hex(result[1])]=results.get(hex(result[1]),0)+1
        for pc,t in o.branches.items():branches.setdefault(pc,set()).update(t)
    assert set(results)=={'0x2261a4','0x225d10','0x225d9c','0x225e28','0x225e7c','0x225ec0'}
    raw=struct.pack('<II',len(records),11)+b''.join(struct.pack('<11I',*r) for r in records);(OUT/'fixtures.bin').write_bytes(raw)
    report=dict(cases=len(records),steps=steps,source_reads=0,gameplay_writes=0,fixture_sha256=hashlib.sha256(raw).hexdigest(),selected=results,coverage=[hex(p) for p in sorted(coverage)],branches={hex(p):sorted(t) for p,t in sorted(branches.items())})
    (OUT/'fixtures.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')}))
if __name__=='__main__':main()
