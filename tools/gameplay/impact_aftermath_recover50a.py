"""Independent original224370 repair proof and authoredCF4 source bindings."""
from pathlib import Path
import argparse,hashlib,json,random,struct
from spring_fixture48j import SpringOriginal
from query_oracle48h import bits,fp,f32
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'work/local50A/impact-aftermath'
BLOCKS={'repair224370':(0x224370,0x22471c),'repair-authoring-loader':(0x215dec,0x215efc),'repair-CF4-selection':(0x215f3c,0x215f6c),'repair-profile-override':(0x2163b0,0x2165b4)}
OFFSETS=[0x7d0,0x7d4,0x7d8,0x7dc,0x7e4,0xd38,0xd3c]
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--elf',type=Path,required=True);a=ap.parse_args()
    data=a.elf.read_bytes();assert hashlib.sha256(data).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
    words={};blocks=[];OUT.mkdir(parents=True,exist_ok=True)
    for name,(lo,hi) in BLOCKS.items():
        raw=data[lo-0x100000+0x80:hi-0x100000+0x80];rows=[dict(pc=p,word=struct.unpack_from('<I',raw,p-lo)[0]) for p in range(lo,hi,4)]
        words.update({r['pc']:r['word'] for r in rows});blocks.append(dict(name=name,sha256=hashlib.sha256(raw).hexdigest(),instructions=rows))
    (OUT/'original-words.json').write_text(json.dumps(dict(elf_sha256=hashlib.sha256(data).hexdigest(),blocks=blocks),indent=2)+'\n')
    # Actual source selection, authoredPod01 min/max + base/max ability, no liveprofile substitution.
    o=SpringOriginal(words=words);o.g[29][0]=0x18000;o.g[21][0]=0x10000
    for offset,value in [(0x490,.0055),(0x494,.014),(0x498,1),(0x49c,1),(0x3f0,.5)]:o.storef(0x18000+offset,value)
    for pc in range(0x215f3c,0x215f6c,4):assert o.execute(pc) is None
    authored=o.read(0x10000+0xcf4)
    pod=(ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy').read_bytes();assert hashlib.sha256(pod).hexdigest()=='47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b'
    (OUT/'Pod01.phy').write_bytes(pod)
    (OUT/'provenance.json').write_text(json.dumps(dict(pod01_sha256=hashlib.sha256(pod).hexdigest(),authoredCF4word=hex(authored),loader_stack={'490':'DamageRepair.MinBaseRepairAmount=.0055','494':'DamageRepair.MaxBaseRepairAmount=.014','498':'BaseAbilities.BaseRepairAmount=1','49C':'MaxAbilities.BaseRepairAmount=1'},selection='215F3C..215F68 original instructions, both ability endpoints1 so interpolation fraction does not change outcome; optional216040 profile override must be caller-owned, not assumed absent.'),indent=2)+'\n')
    rng=random.Random(0x22437050);records=[];coverage=set();branches={};steps=0;previous=None
    for k in range(4096):
        ins=[2,rng.randrange(16),bits(f32(rng.choice([0,1/60,.1]))),authored if k<1024 else bits(f32(rng.uniform(-.02,.25)))]
        state=[bits(f32(rng.uniform(-.2,1.2))) for _ in range(4)]+[bits(rng.choice([0.,-.1,.1,.001]))]+[bits(f32(rng.uniform(-.2,1.2))) for _ in range(2)]
        if k%32==0:ins[0]=0
        if k%32 in (1,2,3,4,5):
            ins[1]=15;ins[3]=bits(.125);state[4]=0
            state[:4]=[bits(.5),bits(fp(bits(.75)+k%32-3)),bits(.6),bits(.5)]
        if k%32==6:state[:4]=[bits(-0.)]*4;state[4]=0;ins[1]=15;ins[3]=bits(0.)
        if k%32==7:state[:4]=[bits(.4),bits(.4),bits(.6),bits(.5)];state[4]=0;ins[1]=15;ins[3]=bits(.125)
        if k%32==8:state[:4]=[bits(.4),bits(.4),bits(.5),bits(.6)];state[4]=0;ins[1]=15;ins[3]=bits(.125)
        if k%32>=24 and previous is not None:state=previous.copy();ins[1]=15
        o=SpringOriginal(words=words);v=0x10000;o.write(v+0xb9c,ins[0],1)
        for j in range(4):o.write(v+0x1b8+4*j,(ins[1]>>j)&1)
        o.write(v+0xcf4,ins[3]);o.f[12]=fp(ins[2]);o.g[4][0]=v
        for off,w in zip(OFFSETS,state):o.write(v+off,w)
        o.run(0x224370);assert not o.source_reads
        result=[o.read(v+off) for off in OFFSETS];previous=result.copy()
        records.append(ins+state+[0 if ins[0]&2 else 1]+result);coverage.update(o.coverage);steps+=o.steps
        for pc,t in o.branches.items():branches.setdefault(pc,set()).update(t)
    writes=[p for p,w in words.items() if 0x224370<=p<0x22471c and w>>26 in (0x2b,0x39) and (w>>21)&31==4]
    assert all(p in coverage for p in writes)
    raw=struct.pack('<II',len(records),19)+b''.join(struct.pack('<19I',*r) for r in records);(OUT/'repair-fixtures.bin').write_bytes(raw)
    report=dict(cases=len(records),steps=steps,source_reads=0,fixture_sha256=hashlib.sha256(raw).hexdigest(),authored_CF4_cases=1024,chained_state_cases=1024,source_vehicle_write_pcs=[hex(p) for p in writes],coverage=[hex(p) for p in sorted(coverage)],branches={hex(p):sorted(v) for p,v in sorted(branches.items())})
    (OUT/'repair-fixtures.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')}))
if __name__=='__main__':main()
