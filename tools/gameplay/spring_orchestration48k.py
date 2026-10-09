"""Bounded original spring coordinator recovery; no production edits/game run."""
from pathlib import Path
import argparse,hashlib,json,re,struct
from spring_fixture48j import SpringOriginal
from query_oracle48h import f32,bits
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/springOrchestration48K'
RAM=ROOT/'work/native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-181624-483/ee.bin'
ELF=ROOT/'work/native/project/disc/SLUS_202.68'
RANGES=[('spring_query_and_response',0x295630,0x296c10),('contact_velocity_and_spring_force',0x296f10,0x297390),('world_point',0x28bab0,0x28bbb0)]
def sha(raw):return hashlib.sha256(raw).hexdigest()
def extract():
    OUT.mkdir(parents=True,exist_ok=True);elf=ELF.read_bytes();assert sha(elf)=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
    comments={};sources=[]
    for p in (ROOT/'work/native/project/generated').glob('*_0x*.cpp'):
        start=int(p.stem.split('_0x')[-1],16)
        if not any(lo<=start<hi for _,lo,hi in RANGES):continue
        data=p.read_bytes()
        for line,text in enumerate(data.decode().splitlines(),1):
            match=re.search(r'// 0x([0-9a-f]+): 0x([0-9a-f]+)\s+(.*)',text)
            if not match:continue
            pc=int(match[1],16);word=int(match[2],16)
            if not any(lo<=pc<hi for _,lo,hi in RANGES):continue
            assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==word
            comments[pc]={'pc':pc,'word':word,'assembly':match[3],'source':p.relative_to(ROOT).as_posix(),'line':line}
        sources.append({'path':p.relative_to(ROOT).as_posix(),'sha256':sha(data)})
    blocks=[]
    for name,lo,hi in RANGES:
        raw=elf[lo-0x100000+0x80:hi-0x100000+0x80]
        rows=[comments.get(pc,{'pc':pc,'word':struct.unpack_from('<I',raw,pc-lo)[0],'assembly':'[no annotation]'}) for pc in range(lo,hi,4)]
        (OUT/(name+'.bin')).write_bytes(raw)
        (OUT/(name+'.txt')).write_text('\n'.join(f"{r['pc']:08X} {r['word']:08X} {r['assembly']} | {r.get('source','')}:{r.get('line','')}" for r in rows)+'\n')
        blocks.append({'name':name,'start':lo,'end':hi,'sha256':sha(raw),'instructions':rows})
    proof={'elf_sha256':sha(elf),'blocks':blocks,'sources':sources};(OUT/'original-words.json').write_text(json.dumps(proof,indent=2)+'\n')
    return proof
def probe(proof):
    ram=RAM.read_bytes();assert sha(ram)=='38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
    u=lambda a:struct.unpack_from('<I',ram,a)[0]
    vehicle=0xc5b0b0;spring=u(vehicle+0x354);detection=u(vehicle+0x358);body=u(spring+0x30)
    assert (spring,detection,body,u(detection+0x30))==(0xd3c570,0x95d0a0,0xd110a0,spring)
    wrong=u(vehicle+0x344)
    withdrawal={'status':'WITHDRAWN: 48J saved primary-body interpretation invalid',
      'affected_report':'evidence/spring48J/impulse-saved-replay.json',
      'affected_report_sha256':sha((ROOT/'evidence/spring48J/impulse-saved-replay.json').read_bytes()),
      'wrong_spring':wrong,'wrong_primary':u(wrong+0x30),
      'failure':'Prior replay selected vehicle+344 instead of actual SpringCollisionModel+354; bogus primary pointer1 caused low-memory accesses. Guarded replay equality did not prove correct object selection.',
      'correct_chain':{'vehicle':vehicle,'spring_slot':0x354,'spring':spring,'primary':body,'detection_slot':0x358,'detection':detection,'detection_spring':u(detection+0x30)},
      'unaffected':'Independent explicitly initialized 48J friction/point-force fixtures; separate terrain query proof. Frozen archives preserved.'}
    (OUT/'48j-saved-replay-withdrawal.json').write_text(json.dumps(withdrawal,indent=2)+'\n')
    words={r['pc']:r['word'] for b in proof['blocks']for r in b['instructions']}
    for b in json.loads((ROOT/'evidence/spring48J/original-words.json').read_text())['blocks']:
        for r in b['instructions']:words[r['pc']]=r['word']
    for b in json.loads((ROOT/'evidence/query48H/original-words.json').read_text())['ranges']:
        for r in b['words']:words.setdefault(r['pc'],r['word'])
    fields={'worldVelocity':0x70,'localAngularVelocity':0xb0,'force':0x180,'torque':0x190,'positionCorrection':0x1a0,'rotationCorrection':0x1b0,'velocityChange':0x1c0,'angularVelocityChange':0x1d0}
    class Trace(SpringOriginal):
        def __init__(self,data,allowed=None):super().__init__(data,words);self.allowed_source_addresses=allowed;self.calls=[];self.records=[]
        def execute(self,pc):
            if pc==0x296978:
                record=self.g[17][0];sp=self.g[29][0]
                self.records.append({'address':record,'penetration':self.f[20],'penetration_word':bits(self.f[20]),
                  'worldPoint':self.vec(sp+0x50,4),'localPoint':self.vec(record+0x10,4),'normal':self.vec(record+0x20,4),
                  'surfacePoint':self.vec(record+0x30,4),'scene':self.read(record+0x40),'otherBody':self.read(record+0x44),'material':self.read(record+0x4c)})
            if pc in (0x296f10,0x297610,0x297390,0x297190,0x2983c0):
                self.calls.append({'pc':f'{pc:08X}','spring':self.g[4][0],'primary':self.read(self.g[4][0]+0x30),
                  'args':[self.g[i][0] for i in range(5,8)],'f12':self.f[12],
                  'worldCenter110':self.vec(spring+0x110,4),'contactVelocity120':self.vec(spring+0x120,4),
                  'vectors':self.vec(self.g[5][0],4) if self.g[5][0] else None,
                  'point':self.vec(self.g[6][0],4) if pc!=0x296f10 and self.g[6][0] else None})
            return super().execute(pc)
    def execute(data,variant,allowed=None):
        m=Trace(data,allowed);m.g[28][0]=0x3dd970;m.g[4][0]=spring
        record=u(spring+0x8c)
        if variant!='saved':
            # Establish the current transformed record point by original words,
            # then explicitly author a positive-depth test input around it.
            preparation=Trace(data,allowed);preparation.g[28][0]=0x3dd970;preparation.g[4][0]=spring
            preparation.g[29][0]=0x1ff0000;preparation.g[31][0]=0xfffffff0;pc=0x2964c0
            while pc!=0x296978:
                target=preparation.execute(pc)
                if target is not None:
                    if not preparation.annul:assert preparation.execute(pc+4) is None
                    pc=target
                else:pc+=4
            point=preparation.vec(preparation.g[29][0]+0x50);normal=preparation.vec(record+0x20)
            m.source_reads.update(preparation.source_reads)
            m.vector(record+0x30,[f32(p+f32(n*.5))for p,n in zip(point,normal)]+[1.])
            if variant=='positive-two-records':
                for off in range(0,0x60,4):m.write(record+0x60+off,m.read(record+off))
                m.write(spring+0x84,2)
            if variant=='positive-other-body':m.write(record+0x44,0xd117b0)
        bodies=[body]+([0xd117b0]if variant=='positive-other-body' else [])
        before={f'{b:08X}':{name:[m.read(b+offset+i*4)for i in range(3)]for name,offset in fields.items()}for b in bodies}
        result=m.run(0x2964c0)
        after={f'{b:08X}':{name:[m.read(b+offset+i*4)for i in range(3)]for name,offset in fields.items()}for b in bodies}
        output={'variant':variant,'return':result,'steps':m.steps,'calls':m.calls,'records':m.records,'before':before,'after':after,
          'spring_flags':{f'{offset:X}':m.read(spring+offset)for offset in (0x38,0x3c,0x40,0x44,0x74,0x78,0x84,0x130)},
          'worldCenter110':m.vec(spring+0x110,4),'contactVelocity120':m.vec(spring+0x120,4),
          'record_flags50':m.read(u(spring+0x8c)+0x50,1),'coverage':[f'{pc:08X}'for pc in sorted(m.coverage)]}
        return m,output
    runs=[];reads=set()
    for variant in ('saved','positive-contact','positive-two-records','positive-other-body'):
        try:m,output=execute(ram,variant)
        except Exception as e:
            (OUT/'coordinator-replay-failure.json').write_text(json.dumps({'variant':variant,'error':str(e),'spring':spring,'primary':body,'no_original_words_patched':True},indent=2)+'\n');raise
        runs.append(output);reads.update(m.source_reads)
    addresses=sorted(reads);spans=[]
    for a in addresses:
        if spans and a==spans[-1][1]:spans[-1][1]+=1
        else:spans.append([a,a+1])
    raw=bytearray();layout=[];sparse=bytearray(len(ram))
    for lo,hi in spans:
        data=ram[lo:hi];layout.append({'address':lo,'bytes':hi-lo,'offset':len(raw),'sha256':sha(data)});raw.extend(data);sparse[lo:hi]=data
    for output in runs:
        _,replayed=execute(sparse,output['variant'],set(addresses));assert replayed==output
    (OUT/'coordinator-saved-data.bin').write_bytes(raw)
    result={'ram_sha256':sha(ram),'chain':withdrawal['correct_chain'],'source_bytes':len(raw),'source_slices':layout,'source_sha256':sha(raw),'sparse_replay':'PASS with reads guarded','runs':runs,
      'variant_scope':'saved is untouched input; positive cases author record surfacePoint as float32(worldPoint + float32(normal*0.5)); two-record case duplicates the record; other-body case supplies real saved D117B0. No original instructions or helpers patched.'}
    (OUT/'coordinator-saved-replay.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'coordinator':'PASS','source_bytes':len(raw),'runs':[{'variant':r['variant'],'steps':r['steps'],'calls':[(x['pc'],x['f12'])for x in r['calls']],'spring110':r['worldCenter110'],'spring120':r['contactVelocity120']}for r in runs]}))
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--show',nargs=2,type=lambda s:int(s,16));ap.add_argument('--probe',action='store_true');args=ap.parse_args();proof=extract()
    if args.probe:probe(proof);return
    if args.show:
        for b in proof['blocks']:
            for r in b['instructions']:
                if args.show[0]<=r['pc']<args.show[1]:print(f"{r['pc']:08X} {r['assembly']}")
    else:print(json.dumps({'words':sum(len(b['instructions'])for b in proof['blocks']),'sources':len(proof['sources'])}))
if __name__=='__main__':main()
