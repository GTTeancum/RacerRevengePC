"""Freeze ELF-checked original texture/material instruction and table evidence."""
from pathlib import Path
import csv,json,re,sys
sys.dont_write_bytecode=True
from verify_texture48d import ROOT,Elf
from decode_static48b import sha

def main():
    out=ROOT/'evidence/textures48D/source-evidence';out.mkdir(parents=True,exist_ok=True)
    ep=ROOT/'work/native/project/disc/SLUS_202.68';elf=Elf(ep)
    entries={0x11a630,0x24e880,0x24ed50,0x24d3e0,0x24d470,0x24d6f0,0x250580,
             0x250900,0x257b50,0x257e60,0x242080,0x242360,0x247110,0x26e640}
    records=[];unique=set()
    for row in csv.DictReader((ROOT/'work/native/project/analysis/function-ranges.csv').open()):
        entry,start,end=[int(row[k],16) for k in ['Entry','Start','End']]
        if entry not in entries:continue
        name=f'{row["Name"]}_0x{entry:x}.cpp' if start==entry else f'entry_{start:08x}_0x{start:x}.cpp'
        path=ROOT/'work/native/project/generated'/name;raw=path.read_bytes();instructions={}
        for line in raw.decode().splitlines():
            m=re.match(r'\s*// (0x[0-9a-f]+): (0x[0-9a-f]+)\s+',line)
            if not m:continue
            va,value=[int(x,16) for x in m.groups()]
            if not start<=va<end:continue
            if elf.words(va,1)[0]!=value:raise ValueError('ELF instruction mismatch '+hex(va))
            instructions[va]=line.strip();unique.add(va)
        target=out/(name+'.instructions.txt');target.write_text('\n'.join(instructions.values())+'\n')
        records.append({'source':str(path.relative_to(ROOT)),'source_sha256':sha(raw),'original_entry':hex(entry),
                        'original_range':[hex(start),hex(end)],'verified_opcodes':len(instructions),'evidence':target.name})
    tables=[]
    for va,count,label in [(0x382fc8,1,'fast_upload_enabled'),(0x382fd0,128,'original_byte_permutation'),
                           (0x3831d0,32,'original_input_block_order'),(0x383250,32,'original_output_block_order')]:
        data=elf.read(va,count*4);target=out/(label+'.bin');target.write_bytes(data)
        tables.append({'VA':hex(va),'uint32_count':count,'sha256':sha(data),'values':elf.words(va,count),'evidence':target.name})
    references=[];base=ROOT/'work/reference/pcsx2-master/pcsx2/GS'
    for name,start,end in [('GSTables.cpp',21,146),('GSTables.cpp',215,233),('GSClut.cpp',227,240),
                           ('GSClut.cpp',552,615),('GSClut.cpp',662,696),('GSLocalMemory.h',95,110),
                           ('GSLocalMemory.h',625,646),('GSLocalMemory.h',958,1045)]:
        path=base/name;data=path.read_bytes();lines=data.decode().splitlines();target=out/f'{name}.{start}-{end}.txt'
        target.write_text('\n'.join(f'{i+1}: {lines[i]}' for i in range(start-1,min(end,len(lines))))+'\n')
        references.append({'source':str(path.relative_to(ROOT)),'source_sha256':sha(data),'lines':[start,end],'evidence':target.name})
    result={'ELF':{'path':str(ep.relative_to(ROOT)),'bytes':len(elf.data),'sha256':sha(elf.data)},
            'unique_verified_opcodes':len(unique),'instructions':records,'original_tables':tables,'PCSX2_reference':references,
            'anchors':[
              {'VA':['24E880','24EA24','11A630'],'fact':'PST revision1 type1 palette read uses256 raw4-byte entries.'},
              {'VA':['24EE14','24EE38','24F014','24F044'],'fact':'Raw width*height 8-bit indices are read; optional24D6F0 conversion sets the fast-upload flag.'},
              {'VA':['250680','250698','2506B0','2506D8'],'fact':'PSMT8 buffer width is at least2 GS units and rounded even.'},
              {'VA':['250CA0','250CC8','250CDC','250CE4','250CF0'],'fact':'Fast upload halves width,height,buffer width and uploads32-bit; otherwise original8-bit dimensions.'},
              {'VA':['250D58','250D68','250D6C','257CB0','257CB8','257D0C','257D34','257D74'],'fact':'Original palette data pointer is uploaded unchanged as16x16 PSMCT32.'},
              {'VA':['242150','242154','242274','242284','242288','24228C','242294'],'fact':'8-bit texture bind selectsPSM19,CPSM0,CSM0,CSA0,CLD1.'},
              {'VA':['2427E8','2427F8','2427FC','242814','242820','242828','242834','242840'],'fact':'Descriptor bytes A/B/C/D pack intoTEX0 bits51/55/56/61, confirmingCLUT field identities.'},
              {'VA':['24719C','247214','247248','247260','2472CC','2472F0','2472F8'],'fact':'Material loader reads microProgramName,totalTextureMaps,TextureMap_N, resolves named textures and stores binding handles.'},
              {'VA':['26E670','26E6A4'],'fact':'PSG material table is read ascount plus64-byte material names.'}],
            'limitations':['Offline static instruction verification and literal scalar translation, not an executed original loader fixture.',
                           'Final original shader arithmetic, alpha normalization, filtering and mip selection are not claimed.']}
    (out/'anchors.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'ELF_verified_opcodes':len(unique),'instruction_sources':len(records),'data_tables':len(tables),'PCSX2_excerpts':len(references)}))

if __name__=='__main__':main()
