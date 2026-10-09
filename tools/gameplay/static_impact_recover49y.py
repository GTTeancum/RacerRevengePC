"""ELF-word differential proof for ordinary223030, no RAM or native oracle."""
from pathlib import Path
import argparse,hashlib,json,math,random,struct
from query_oracle48h import Original,bits,fp,f32
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'work/local49Y/static-impact'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS={'impact223030':(0x223030,0x2234dc),'static222770':(0x222770,0x2228c8),'inverse28bbb0':(0x28bbb0,0x28bd48)}
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--elf',type=Path,required=True);a=ap.parse_args()
    data=a.elf.read_bytes();assert hashlib.sha256(data).hexdigest()==ELF_SHA
    phoff=struct.unpack_from('<I',data,28)[0];phsize,phcount=struct.unpack_from('<HH',data,42)
    segments=[struct.unpack_from('<8I',data,phoff+i*phsize) for i in range(phcount)]
    words={};blocks=[];OUT.mkdir(parents=True,exist_ok=True)
    for name,(lo,hi) in BLOCKS.items():
        seg=next(s for s in segments if s[0]==1 and s[2]<=lo<hi<=s[2]+s[4]);raw=data[seg[1]+lo-seg[2]:seg[1]+hi-seg[2]]
        rows=[{'pc':pc,'word':struct.unpack_from('<I',raw,pc-lo)[0]} for pc in range(lo,hi,4)]
        words.update({r['pc']:r['word'] for r in rows});blocks.append(dict(name=name,start=lo,end=hi,sha256=hashlib.sha256(raw).hexdigest(),instructions=rows))
    (OUT/'original-words.json').write_text(json.dumps(dict(elf_sha256=ELF_SHA,blocks=blocks),indent=2)+'\n')
    rng=random.Random(0x22303049);records=[];coverage=set();branches={};steps=0;prior_state=None;prior_w=None
    def rand(scale=1):return f32(rng.uniform(-scale,scale))
    for i in range(2048):
        angle=rand(3);c=f32(math.cos(angle));s=f32(math.sin(angle));m=[c,0,-s,0,0,1,0,0,s,0,c,0,rand(100),rand(100),rand(100),1]
        if i%4==0:m=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
        if i%4==1:
            for j in (0,1,2,4,5,6,8,9,10):m[j]=f32(m[j]+rand(.15))
        impulse=[rand(20),rand(20),rand(20),rand()]
        thresholds=[20.,10.,5.,2.,0.];values=[2.,1.,.5,.25,.1]
        reduction=rand(2);state=[rand(2) for _ in range(8)]+[rand(10),rand(10),rand(10),fp(0x7fc12345)]
        if i<512:
            # Exact TA EnvDamage numbers and Pod01 base Defense, source-bound in
            # separate provenance. Profile override remains caller-supplied.
            thresholds=[1050.,787.5,440.,175.,50.];values=[14.,12.,8.,5.,.5];reduction=f32(.8)
            impulse=[rand(1200),rand(1200),rand(1200),rand()]
        mode=0;flag=0
        if i%32==0:impulse=[0.,-0.,0.,fp(0x7fc12345)]
        if i%32 in (1,2,3):impulse=[fp(0x3dcccccD+i%32-2),0,0,1];m=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
        if i%32 in (4,5,6):impulse=[-fp(0x3dcccccD+i%32-5),0,0,1];m=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
        if i%32 in (7,8,9,10,11):impulse=[thresholds[i%32-7],0,0,1]
        if i%32==12:thresholds=[100,100,100,100,100]
        if i%32==13:thresholds=[2,50,1,100,0]
        if i%32==14:reduction=1.
        if i%32==15:reduction=0.;values=[.1]*5
        if i%32 in (16,17,18):
            impulse=[f32(math.sqrt(7)/4),0.,-fp(0x3f400000+i%32-17),1.];m=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1];reduction=0.;thresholds=[0]*5;values=[1]*5
        if i%32==19:state[8:11]=impulse[:3]
        if i%32==20:state[8:11]=[-x for x in impulse[:3]]
        if i%32==21:mode=3
        if i%32==22:mode=4
        if i%32==23:flag=1
        w=rand(20)
        if i%32>=24:state=prior_state;w=prior_w
        o=Original(words=words);vehicle=0x10000;body=0x12000;scene=0x13000;tuning=0x14000
        o.write(vehicle+0xcd8,flag,1);o.write(vehicle+0x40,mode);o.write(vehicle+0x58,0);o.write(vehicle+0x340,body);o.write(vehicle+0x190,scene)
        o.write(scene+0x190,1,1);o.vector(scene+0x100,m);o.write(body+0x20c,0);o.vector(body+0x1c0,impulse)
        o.write(0x382b80,0x15000);o.write(0x15000+0x28c,0x16000);o.write(0x16000+0xc0,tuning);o.vector(tuning,thresholds+values);o.storef(vehicle+0xcf0,reduction)
        o.vector(vehicle+0xd08,state[:8]);o.vector(vehicle+0xba0,state[8:]);o.g[4][0]=vehicle;o.v[9][3]=w
        o.run(0x223030);assert not o.source_reads
        inputs=[mode,0,0,flag,1]+[bits(x) for x in m+impulse+[reduction]+thresholds+values+[w]+state]
        outputs=[o.read(vehicle+0xd08+j*4) for j in range(8)]+[o.read(vehicle+0xba0+j*4) for j in range(4)]+[bits(o.v[9][3])]
        prior_state=[fp(x) for x in outputs[:12]];prior_w=fp(outputs[12])
        assert len(inputs)==49 and len(outputs)==13
        records.append(inputs+outputs);coverage.update(o.coverage);steps+=o.steps
        for pc,taken in o.branches.items():branches.setdefault(pc,set()).update(taken)
    # Every direct ordinary-route state write and each numeric decision branch
    # must be reached; excluded vehicle58 and partner-list gates are one-sided.
    state_writes=[0x223164,0x2231a0,0x223234,0x223274,0x2232bc,0x2232fc,0x2233ac,0x2233ec,0x223434,0x22347c,0x2234c4]
    assert all(pc in coverage for pc in state_writes)
    decisions=[0x2227a4,0x2227c0,0x2227dc,0x2227f8,0x222814,0x223048,0x223058,0x223060,0x223144,0x223158,0x223184,0x223198,0x2231bc,0x22320c,0x223228,0x223254,0x22326c,0x223294,0x2232b0,0x2232dc,0x2232f4,0x22331c,0x22333c,0x223358,0x2233cc,0x2233e4,0x223424,0x22346c,0x2234b4]
    assert all(branches.get(pc)=={False,True} for pc in decisions),[hex(pc) for pc in decisions if branches.get(pc)!={False,True}]
    raw=struct.pack('<II',len(records),62)+b''.join(struct.pack('<62I',*r) for r in records);(OUT/'fixtures.bin').write_bytes(raw)
    report=dict(cases=len(records),input_words=49,output_words=13,steps=steps,elf_sha256=ELF_SHA,fixture_sha256=hashlib.sha256(raw).hexdigest(),source_reads=0,coverage=[f'{pc:08x}' for pc in sorted(coverage)],branches={f'{pc:08x}':sorted(v) for pc,v in sorted(branches.items())},scope='Full reached original223030 and222770 and ready28BBB0, ordinary vehicle58=0 and body20C=0. Individually rounded IEEE binary32. No physical EE exceptional-mode claim; no RAM; no substituted calls.')
    report.update(authored_tuning_cases=512,chained_state_cases=512,all_ordinary_state_write_pcs=[hex(p) for p in state_writes],two_sided_decision_pcs=[hex(p) for p in decisions])
    (OUT/'fixtures.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches')}))
if __name__=='__main__':main()
