"""Bounded offline original-data queries; no source or executable mutation."""
from pathlib import Path
import csv,json,struct,hashlib,re
from recover48e import ROOT,ELF_SHA,RAM_PATH,RAM_SHA
def sha(b):return hashlib.sha256(b).hexdigest()
def main():
    elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();ram=(ROOT/RAM_PATH).read_bytes()
    assert sha(elf)==ELF_SHA and sha(ram)==RAM_SHA
    ranges=[]
    with (ROOT/'work/native/project/analysis/function-ranges.csv').open() as f:
        for r in csv.DictReader(f):ranges.append((int(r['Start'],16),int(r['End'],16),r['Name'],r['Entry']))
    def owners(a):return [{'name':name,'entry':entry} for start,end,name,entry in ranges if start<=a<end]
    def u(a):return struct.unpack_from('<I',ram,a)[0]
    def e(a):return struct.unpack_from('<I',elf,a-0x100000+0x80)[0]
    def string(a):
        start=a-0x100000+0x80;return elf[start:elf.index(b'\0',start)].decode('ascii')
    # 2.84MiB original load segment only; scan exact instruction encodings,
    # not the enormous generated tree. Offset+38 is not itself a type proof.
    hits=[]
    for o in range(0x80,0x80+2972160,4):
        w=struct.unpack_from('<I',elf,o)[0]
        if w==0x8f390038 or w==(0x0c000000 | (0x167b80>>2)):
            a=o-0x80+0x100000
            hits.append({'pc':hex(a),'word':hex(w),'owners':owners(a),
                'nearby_words':[hex(e(x)) for x in range(a-16,a+16,4)]})
    body=u(0xc5b0b0+0x340)
    evidence={'elf_sha256':ELF_SHA,'ram_sha256':RAM_SHA,'bounded_scan_bytes':2972160,
        'slot38_or_direct167b80_candidates':hits,'warning':'Slot offset matches are candidates; only the generic child-list path is examined. No claim every candidate is PhysicsRigidBody.',
        'body_fields':{hex(x):hex(u(body+x)) for x in [0x8,0xc,0x10,0x14,0x18,0x20,0x24,0x28,0x320]},
        'source_extracts':[]}
    keys=[]
    for pc in range(0x214bcc,0x214dd8,4):
        if e(pc)!=(0x0c000000|(0x158a70>>2)):continue
        keyWord=e(pc-8);destWord=e(pc-4)
        assert keyWord>>16==0x2445 and destWord>>16==0x27a6
        va=0x3b0000+(keyWord&65535);stack=destWord&65535;field=0x5c+stack-0x100
        setter=0x1004bc+(field-0x5c)*2
        assert e(setter)==(0xe4800000|field)
        assert e(setter-4)==(0xc4a00000|(stack-0x100))
        keys.append({'key':string(va),'string_va':hex(va),'reader_pc':hex(pc),'stack_offset':hex(stack),
                     'setter_store_pc':hex(setter),'aero_offset':hex(field)})
    for pc in range(0x214df8,0x214ed4,0x18):
        assert e(pc)==(0x0c000000|(0x158a70>>2))
        keyWord=e(pc-8);destWord=e(pc+4)
        assert keyWord>>16==0x2445 and destWord>>16==0x2466
        va=0x3b0000+(keyWord&65535)
        keys.append({'key':string(va),'string_va':hex(va),'reader_pc':hex(pc),'aero_offset':hex(destWord&65535)})
    (ROOT/'evidence/gameplay48E/tuning-key-chains.json').write_text(json.dumps({'chains':keys,
        'note':'Literal->reader destination->setter store checked against ELF. Steering +100 is separately selected/interpolated in 214Fxx..215078; runtime parameters remain the API input.'},indent=2)+'\n')
    out=ROOT/'evidence/gameplay48E'
    for name in ['FUN_00141ed0_0x141ed0.cpp','entry_00141efc_0x141efc.cpp','entry_00141f68_0x141f68.cpp',
                 'entry_00214f6c_0x214f6c.cpp','entry_00214a18_0x214a18.cpp','FUN_00100430_0x100430.cpp',
                 'FUN_001004b0_0x1004b0.cpp','FUN_00217de0_0x217de0.cpp','FUN_001a7b40_0x1a7b40.cpp',
                 'entry_001a8060_0x1a8060.cpp']:
        p=ROOT/'work/native/project/generated'/name
        if not p.exists():continue
        rows=[];seen=set()
        for n,line in enumerate(p.read_text().splitlines(),1):
            m=re.match(r'\s*// (0x[0-9a-f]+): (0x[0-9a-f]+)\s+(.*)',line)
            if m:
                a,w=int(m[1],16),int(m[2],16);assert e(a)==w
                if a not in seen:rows.append(f'{n}: {line.strip()}');seen.add(a)
        q=out/(name+'.instructions.txt');q.write_text('\n'.join(rows)+'\n')
        evidence['source_extracts'].append({'source':p.relative_to(ROOT).as_posix(),'sha256':sha(p.read_bytes()),'words_verified':len(seen)})
    (out/'dependency-queries.json').write_text(json.dumps(evidence,indent=2)+'\n')
    print(json.dumps({'slot38_candidates':len(hits),'body320':hex(u(body+0x320)),'tuning_keys':len(keys),
        'key_fields':{x['aero_offset']:x['key'] for x in keys}}))
if __name__=='__main__':main()
