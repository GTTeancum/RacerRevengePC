"""Freeze exact original orientation instruction words and independent oracle."""
from pathlib import Path
import hashlib, json, re, struct
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/bodyScene48G'
OUT.mkdir(parents=True,exist_ok=True)
elfpath=ROOT/'work/native/project/disc/SLUS_202.68'
elf=elfpath.read_bytes()
assert hashlib.sha256(elf).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
blocks=[('quaternion_matrix',0x14f4f0,0x14f620),('matrix_quaternion',0x14f950,0x14fc00),
        ('quaternion_angles',0x14fc00,0x14fcf4),('atan2',0x13c1f0,0x13c550)]
header=['#pragma once','#include <cstdint>','// Test-only exact words; see evidence/bodyScene48G/recovery.json.']
proof=[]
for name,start,end in blocks:
    data=elf[start-0x100000+0x80:end-0x100000+0x80]
    words=struct.unpack('<'+'I'*(len(data)//4),data)
    (OUT/(name+'.bin')).write_bytes(data)
    header.extend([f'inline constexpr std::uint32_t rr_{name}_48g_start=0x{start:x};',
                   f'inline constexpr std::uint32_t rr_{name}_48g[]={{'])
    for i in range(0,len(words),8):header.append('    '+','.join(f'0x{w:08x}u' for w in words[i:i+8])+',')
    header.append('};')
    instructions={}
    # Generated source is explanatory evidence, never a substitute for ELF bytes.
    for p in (ROOT/'work/native/project/generated').glob('*.cpp'):
        match=re.search(r'_0x([0-9a-f]+)\.cpp$',p.name)
        if not match or not start<=int(match[1],16)<end:continue
        text=p.read_text()
        for line in text.splitlines():
            m=re.search(r'// 0x([0-9a-f]+): 0x([0-9a-f]+)',line)
            if m and start<=int(m[1],16)<end:
                pc=int(m[1],16);assert words[(pc-start)//4]==int(m[2],16)
                instructions[pc]=line.strip()
    (OUT/(name+'.instructions.txt')).write_text('\n'.join(instructions[a] for a in sorted(instructions))+'\n')
    proof.append(dict(name=name,start=hex(start),end_exclusive=hex(end),bytes=len(data),sha256=hashlib.sha256(data).hexdigest(),comment_instructions_verified=len(instructions)))
ram_path=ROOT/'work/native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-181624-483/ee.bin'
ram=ram_path.read_bytes();assert hashlib.sha256(ram).hexdigest()=='38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
for name,address,size in [('saved_quaternion',0xd110a0+0x1f0,16),('saved_matrix',0xcfd8f0+0x100,64)]:
    words=struct.unpack('<'+'I'*(size//4),ram[address:address+size])
    header.append(f'inline constexpr std::uint32_t rr_{name}_48g[]={{'+','.join(f'0x{w:08x}u' for w in words)+'};')
(ROOT/'tools/gameplay/body_scene_words48g.h').write_text('\n'.join(header)+'\n')
oracle=(ROOT/'tools/gameplay/aero_oracle48e.h').read_text().replace('struct Original48E','struct OriginalScene48G')
oracle=oracle.replace('case 4: f[fd]=Ps2EeSqrt14(f[rd],flags);','case 4: f[fd]=Ps2EeSqrt14(f[rt],flags);')
oracle=oracle.replace('#include "aero_words48e.h"','#include "aero_words48e.h"\n#include "body_scene_words48g.h"')
oracle=oracle.replace('unsigned steps=0;','unsigned steps=0;\n    std::array<bool,4> quaternionBranches{};\n    std::array<bool,12> angleBranches{};')
insertion='\n#define SCENE_BLOCK(name) if(pc>=rr_##name##_48g_start && pc-rr_##name##_48g_start<sizeof(rr_##name##_48g)) return rr_##name##_48g[(pc-rr_##name##_48g_start)/4]\n'
insertion+='        SCENE_BLOCK(quaternion_matrix);SCENE_BLOCK(matrix_quaternion);SCENE_BLOCK(quaternion_angles);SCENE_BLOCK(atan2);\n#undef SCENE_BLOCK\n'
oracle=oracle.replace('        throw std::runtime_error("oracle PC outside original blocks:',insertion+'        throw std::runtime_error("oracle PC outside original blocks:')
oracle=oracle.replace('        currentPc=pc;', '''        currentPc=pc;
        const std::uint32_t qb[]={0x14fa04,0x14fa90,0x14fb10,0x14fb78};
        const std::uint32_t ab[]={0x13c228,0x13c234,0x13c29c,0x13c2d8,0x13c33c,0x13c378,0x13c3f4,0x13c430,0x13c494,0x13c4d0,0x13c514,0x13c538};
        for(unsigned i=0;i<4;++i)if(pc==qb[i])quaternionBranches[i]=true;
        for(unsigned i=0;i<12;++i)if(pc==ab[i])angleBranches[i]=true;''')
(ROOT/'tools/gameplay/body_scene_oracle48g.h').write_text(oracle)
(OUT/'recovery.json').write_text(json.dumps(dict(elf_sha256=hashlib.sha256(elf).hexdigest(),ram_sha256=hashlib.sha256(ram).hexdigest(),blocks=proof,scope='Original finite orientation helpers; test-only instruction interpreter; no full body wrapper claim'),indent=2)+'\n')
prior_sqrt=[]
for name in ['physics_words48c.h','forces_words48d.h','aero_words48e.h']:
    p=ROOT/'tools/gameplay'/name;text=p.read_text()
    for m in re.finditer(r'(rr_\w+_48[cde])\[\]\s*=\s*\{([^}]*)\}',text):
        start=re.search(r'\b'+m[1]+r'_start\s*=\s*(0x[0-9a-fA-F]+)',text)
        if not start:continue
        for i,w in enumerate(re.findall(r'0x[0-9a-fA-F]+',m[2])):
            word=int(w,16)
            if word>>26==0x11 and (word>>21)&31==16 and word&63==4:
                fs=(word>>11)&31;ft=(word>>16)&31
                assert fs==ft, 'Prior executed SQRT source audit requires investigation'
                prior_sqrt.append(dict(header=name,pc=hex(int(start[1],16)+i*4),word=w,fs=fs,ft=ft))
assert prior_sqrt
pcsx=ROOT/'work/reference/pcsx2-master/pcsx2/FPU.cpp';text=pcsx.read_text()
excerpt=text[text.index('void SQRT_S()'):text.index('void SUB_S()')]
(OUT/'pcsx2-sqrt-source.txt').write_text(excerpt)
(OUT/'oracle-sqrt-discovery.json').write_text(json.dumps(dict(
    failed_build='evidence/nativePc48G-build-20261003-152624-335',
    failure='New matrix-to-quaternion differential failed at check22; inherited test-only SQRT case read Fs instead of original EE Ft.',
    correction='New 48G oracle reads Ft. Production orientation code was unchanged. Frozen earlier oracle files remain intact.',
    pcsx2_path=pcsx.relative_to(ROOT).as_posix(),pcsx2_sha256=hashlib.sha256(pcsx.read_bytes()).hexdigest(),
    prior_instruction_blocks=prior_sqrt,prior_sqrt_all_equal_operands=True),indent=2)+'\n')
print(json.dumps(proof))
