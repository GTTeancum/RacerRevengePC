"""Original ELF consumer proof. Explicit memory fixtures only, never captured RAM."""
from pathlib import Path
import argparse, hashlib, json, random, struct
from spring_fixture48j import SpringOriginal
from query_oracle48h import bits, fp, f32
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'work/local49Z/impact-consumers'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={'dispatch22888c':(0x22888c,0x2288bc),'consumer222b20':(0x222b20,0x222efc),'consume2228d0':(0x2228d0,0x222b18),'damage221710':(0x221710,0x2218e4),'periodic221590':(0x221590,0x22170c),'predicate188ec0':(0x188ec0,0x188f04),'authored-loader':(0x1aa908,0x1aa938)}
OFFSETS=list(range(0xd08,0xd28,4))+list(range(0x820,0x834,4))+list(range(0x7d0,0x7e0,4))+[0x7e0,0x7e8,0xcfc,0xd00,0xd04]+list(range(0x2000,0x2014,4))+[0xd38,0xd3c]
class ConsumerOriginal(SpringOriginal):
    def __init__(self,words):
        super().__init__(words=words);self.events=[]
    def run_consumer(self,player):
        pc=0x22888c;self.g[29][0]=0x1ff0000
        while pc not in (0x2288bc,0x228b40):
            if pc in (0x1f00000,0x1f00004):
                # Explicit caller-owned output boundary. No substituted gameplay
                # instructions: record original call arguments and return only.
                slot=0x40 if pc==0x1f00000 else 0x44
                assert self.g[4][0]==0x1a000 and self.g[5][0]==2
                self.events += [self.g[31][0]-8,slot,player,self.g[5][0],0 if slot==0x40 else self.g[6][0],0 if slot==0x40 else bits(self.f[12])]
                pc=self.g[31][0];continue
            if pc in (0x1af330,0x22ade0):return 2
            target=self.execute(pc)
            if target is None:pc+=4
            else:
                if not self.annul:assert self.execute(pc+4) is None
                pc=target
        return 1 if pc==0x228b40 or 0x222b54 not in self.coverage else 0
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--elf',type=Path,required=True);args=ap.parse_args()
    data=args.elf.read_bytes();assert hashlib.sha256(data).hexdigest()==ELF_SHA
    phoff=struct.unpack_from('<I',data,28)[0];sz,n=struct.unpack_from('<HH',data,42)
    segments=[struct.unpack_from('<8I',data,phoff+j*sz) for j in range(n)]
    def extract(lo,hi):
        s=next(s for s in segments if s[0]==1 and s[2]<=lo<hi<=s[2]+s[4]);return data[s[1]+lo-s[2]:s[1]+hi-s[2]]
    words={};blocks=[];OUT.mkdir(parents=True,exist_ok=True)
    for name,(lo,hi) in BLOCKS.items():
        raw=extract(lo,hi);rows=[dict(pc=p,word=struct.unpack_from('<I',raw,p-lo)[0]) for p in range(lo,hi,4)]
        words.update({r['pc']:r['word'] for r in rows});blocks.append(dict(name=name,start=lo,end=hi,sha256=hashlib.sha256(raw).hexdigest(),instructions=rows))
    (OUT/'original-words.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,blocks=blocks),indent=2)+'\n')
    # Loader a1 points at actual string; a2 points into tuning64/68.
    strings={hex(a):extract(a,a+64).split(b'\0')[0].decode() for a in (0x3aaee0,0x3aaf00)}
    ta=(ROOT/'work/local49Y/static-impact/provenance/ta.tun').read_bytes()
    assert hashlib.sha256(ta).hexdigest()=='6cba8a2ea570e6985fc73a7f0399e07360b96315f5a190886e4d3edbe793c243'
    (OUT/'ta.tun').write_bytes(ta)
    (OUT/'provenance.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,strings=strings,loader_pcs=['1AA918 -> tuning64','1AA930 -> tuning68'],ta_sha256=hashlib.sha256(ta).hexdigest(),authored={'FinalDamageThreshold':1.0,'FinalDamageIncrement':0.0},constant_words={'damageThreshold':0x3dcccccd,'lateralTimerIncrement':0x3ccccccd,'periodicDamage':0x3ca3d70a,'controllerScale':0x3ba3d70a},data_source='Original ELF and retained authored TA member; no RAM.'),indent=2)+'\n')
    rng=random.Random(0x222b2049);records=[];coverage=set();branches={};steps=0;rejects=0;chains=0;previous=None;eventcases=0
    for k in range(4096):
        # Inputs: 11 integer words, then factor and actual tuning64/68.
        ins=[2,0,rng.randrange(3),0,rng.randrange(3),rng.randrange(4),rng.randrange(4),rng.randrange(16),rng.randrange(4),rng.randrange(2),rng.randrange(8),bits(f32(rng.choice([0,1/60,.1,.5]))),bits(1.),bits(0.)]
        if k<2048:ins[2]=0;ins[4]=0
        vals=[f32(rng.uniform(-.2,22)) for _ in range(8)]+[f32(rng.uniform(-.05,.8)) for _ in range(5)]+[f32(rng.uniform(-.2,1.2)) for _ in range(4)]+[rng.choice([0.,-.1,.001,.5]),rng.choice([0.,-.1,.001,.5])]
        state=[bits(x) for x in vals]+[rng.randrange(4),rng.randrange(4),rng.randrange(10),0xffffffff,rng.randrange(3),bits(rng.choice([0.,.5,.50000006])),bits(rng.choice([0.,.1,.2])),bits(.15),bits(.2),bits(.3)]
        if k%32==0:ins[0]=0
        if k%32==1:ins[1]=1
        if k%32==2:ins[3]=3
        if k%32 in (3,4,5):state[22]=2;state[24]=bits(0.);state[25]=bits([.099,.1,.2][k%32-3])
        if k%32 in (6,7,8):
            boundary=0x3dcccccd+k%32-7
            state[:8]=[boundary]*8;state[17]=bits(0.)
        if k%32==9:state[23]=0;state[17]=bits(0.);ins[7]=15;state[:8]=[bits(10.)]*8
        if k%32==10:state[23]=1;state[17]=bits(0.);ins[7]=15;state[13:17]=[bits(1.)]*4;ins[13]=bits(1.)
        if k%32==11:state[:8]=[bits(-0.)]*8;state[13:17]=[bits(-0.)]*4
        if k%32==12:ins[5]=2;state[18]=bits(0.);ins[7]=15
        if k%32>=24 and previous is not None:
            state=previous.copy();state[0]=bits(f32(rng.uniform(.1,3)));state[6]=bits(2.);ins[0]=2;ins[1]=0;ins[2]=0;ins[4]=0;chains+=1
        assert len(state)==29 and len(ins)==14
        o=ConsumerOriginal(words);vehicle=0x10000;partner=vehicle+0x2000;game=0x14000;manager=0x15000;tuning=0x16000;profile=0x30000
        for off,x in zip(OFFSETS,state):o.write(vehicle+off,x)
        for off,x in zip([0xcd8,0x3c,0x40,0x58,0x64,0x5c],ins[1:7]):o.write(vehicle+off,x,1 if off==0xcd8 else 4)
        for j in range(4):o.write(vehicle+0x1b8+j*4,(ins[7]>>j)&1)
        o.write(vehicle+0xd34,partner);o.write(0x382b80,game);o.write(game+0x28c,manager);o.write(manager+0xc0,tuning);o.write(manager+0xb0,0x19000);o.write(manager+0xb4,0x17000);o.write(0x17058,ins[0],1)
        o.write(game+0x2dc,profile);o.write(profile+0x34,ins[8]);o.write(profile+0x10fac,ins[9]);o.write(profile+0x11050,ins[10],1)
        o.write(tuning+0x64,ins[12]);o.write(tuning+0x68,ins[13]);o.storef(tuning+0x48,7.)
        o.write(game+4,0x18000);o.write(0x18010+ins[6]*4,0x1a000);o.write(0x1a018,0x1b000);o.write(0x1b040,0x1f00000);o.write(0x1b044,0x1f00004)
        o.g[16][0]=vehicle;o.f[20]=fp(ins[11]);status=o.run_consumer(ins[6]);assert not o.source_reads
        result=[o.read(vehicle+off) for off in OFFSETS] if status!=2 else state
        if status==2:rejects+=1;previous=None
        else:previous=result.copy()
        events=o.events if status!=2 else [];eventcases+=bool(events)
        rec=ins+state+[status]+result+[len(events)//6]+events+[0]*(12-len(events));assert len(rec)==86;records.append(rec)
        coverage.update(o.coverage);steps+=o.steps
        for pc,taken in o.branches.items():branches.setdefault(pc,set()).update(taken)
    raw=struct.pack('<II',len(records),86)+b''.join(struct.pack('<86I',*r) for r in records);(OUT/'fixtures.bin').write_bytes(raw)
    report=dict(cases=len(records),input_words=14,state_words=29,record_words=86,steps=steps,unsupported_partner_cases=rejects,chained_state_cases=chains,controller_event_cases=eventcases,source_reads=0,fixture_sha256=hashlib.sha256(raw).hexdigest(),coverage=[hex(p) for p in sorted(coverage)],branches={hex(p):sorted(v) for p,v in sorted(branches.items())},scope='Original manager dispatch,222B20,2228D0,221710,221590,188EC0. Controller calls recorded at typed external boundary and returned without game-state mutation; real backend binding remains integration dependency. Dynamic partner route rejected at first reached call. Individually rounded IEEE ordinary scalar model, not all physical EE modes.')
    (OUT/'fixtures.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')}))
if __name__=='__main__':main()
