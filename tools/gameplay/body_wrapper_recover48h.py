"""Freeze original full-body and root-scene instruction evidence for 48H."""
from pathlib import Path
import hashlib,json,re,struct
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/bodyWrapper48H';OUT.mkdir(parents=True,exist_ok=True)
ep=ROOT/'work/native/project/disc/SLUS_202.68';elf=ep.read_bytes()
assert hashlib.sha256(elf).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
rp=ROOT/'work/native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-181624-483/ee.bin';ram=rp.read_bytes()
assert hashlib.sha256(ram).hexdigest()=='38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
blocks=[('body_wrapper',0x167c40,0x16836c),('set_orientation',0x289b20,0x289d4c),
 ('rotate_axis',0x28b840,0x28b8e4),('rotate_matrix',0x112b20,0x112c4c),
 ('sin',0x13bad0,0x13bba0),('cos',0x13bba0,0x13bca0),
 ('scene_refresh',0x28ea20,0x28ea74),('base_update',0x141ed0,0x141f90),
 ('body_callback',0x1654b0,0x1654b8)]
comments={};sources=[]
for p in (ROOT/'work/native/project/generated').glob('*.cpp'):
 m=re.search(r'_0x([0-9a-f]+)\.cpp$',p.name)
 if not m or not any(a<=int(m[1],16)<b for _,a,b in blocks):continue
 for line in p.read_text().splitlines():
  m=re.search(r'// 0x([0-9a-f]+): 0x([0-9a-f]+)',line)
  if m:
   pc=int(m[1],16)
   if any(a<=pc<b for _,a,b in blocks):
    assert struct.unpack_from('<I',elf,pc-0x100000+0x80)[0]==int(m[2],16)
    comments[pc]=line.strip()
 sources.append(dict(path=p.relative_to(ROOT).as_posix(),sha256=hashlib.sha256(p.read_bytes()).hexdigest()))
header=['#pragma once','#include <cstdint>'];proof=[]
for name,a,b in blocks:
 data=elf[a-0x100000+0x80:b-0x100000+0x80];(OUT/(name+'.bin')).write_bytes(data)
 words=struct.unpack('<'+'I'*(len(data)//4),data)
 header+=[f'inline constexpr std::uint32_t rr_{name}_48h_start=0x{a:x};',f'inline constexpr std::uint32_t rr_{name}_48h[]={{']
 header+=['    '+','.join(f'0x{x:08x}u' for x in words[i:i+8])+',' for i in range(0,len(words),8)];header+=['};']
 (OUT/(name+'.txt')).write_text('\n'.join(comments[pc] for pc in sorted(comments) if a<=pc<b)+'\n')
 proof.append(dict(name=name,start=hex(a),end_exclusive=hex(b),sha256=hashlib.sha256(data).hexdigest(),matched_comments=sum(a<=pc<b for pc in comments)))
body=0xd110a0;scene=0xcfd8f0
u=lambda a:struct.unpack_from('<I',ram,a)[0]
saved=dict(body=hex(body),scene=hex(scene),body_vtable=hex(u(body+8)),body_callback_9c=hex(u(u(body+8)+0x9c)),body_child_18=hex(u(body+0x18)),body_flags24=hex(u(body+0x24)),scene_parent180=hex(u(scene+0x180)),scene_children184=hex(u(scene+0x184)),scene_observer19c=hex(u(scene+0x19c)))
for name,address,size in [('body',body,0x340),('scene',scene,0x1a0)]:
 data=ram[address:address+size];(OUT/('saved-'+name+'.bin')).write_bytes(data)
 words=struct.unpack('<'+'I'*(size//4),data)
 header+=[f'inline constexpr std::uint32_t rr_saved_{name}_48h[]={{'+','.join(f'0x{x:08x}u' for x in words)+'};']
(ROOT/'tools/gameplay/body_wrapper_words48h.h').write_text('\n'.join(header)+'\n')
oracle=(ROOT/'tools/gameplay/contact_oracle48g.h').read_text()
oracle=re.sub(r'struct Original\w+', 'struct OriginalWrapper48H', oracle, count=1)
oracle=oracle.replace('#include "contact_words48g.h"','#include "contact_words48g.h"\n#include "body_scene_words48g.h"\n#include "body_wrapper_words48h.h"')
constants=struct.unpack_from('<8I',ram,0x3df590)
oracle=oracle.replace('        if(a==0x433c10)', '        if(a>=0x3df590&&a<0x3df5b0&&!(a&3)){static constexpr std::uint32_t data[]={'+','.join(hex(x)+'u' for x in constants)+'};return data[(a-0x3df590)/4];}\n        if(a==0x433c10)')
insertion='\n#define WRAP_BLOCK(name) if(pc>=rr_##name##_48h_start&&pc-rr_##name##_48h_start<sizeof(rr_##name##_48h))return rr_##name##_48h[(pc-rr_##name##_48h_start)/4]\n'
insertion+='        '+''.join('WRAP_BLOCK('+name+');' for name,_,_ in blocks)+'\n#undef WRAP_BLOCK\n'
insertion+='#define SCENE_BLOCK(name) if(pc>=rr_##name##_48g_start&&pc-rr_##name##_48g_start<sizeof(rr_##name##_48g))return rr_##name##_48g[(pc-rr_##name##_48g_start)/4]\n        SCENE_BLOCK(quaternion_matrix);SCENE_BLOCK(matrix_quaternion);\n#undef SCENE_BLOCK\n'
oracle=oracle.replace('        throw std::runtime_error("oracle PC outside original blocks:',insertion+'        throw std::runtime_error("oracle PC outside original blocks:')
oracle=oracle.replace('if(op==0x19)', 'if(op==8)g[rt][0]=g[rs][0]+imm;\n        else if(op==0x15){const bool take=g[rs][0]!=g[rt][0];annulDelay=!take;return branch(take);}\n        else if(op==0x19)')
oracle=oracle.replace('            if((w&0x7ffu)==0x2feu)', '''            const auto fn=w&63u,fd=(w>>6)&31u,mask=(w>>21)&15u,special=w&0x7ffu;
            if(rs>=16&&(fn<16||(fn>=0x18&&fn<=0x1b)||fn==0x28||fn==0x2c)) {
                auto dest=v[fd];
                for(unsigned i=0;i<4;++i)if(mask&(8u>>i)) {
                    const float a=v[rd][i],b=v[rt][fn==0x28||fn==0x2c?i:(fn&3)];
                    if(fn<4||fn==0x28)dest[i]=add(a,b);
                    else if(fn<8||fn==0x2c)dest[i]=sub(a,b);
                    else if(fn<12)dest[i]=add(vacc[i],mul(a,b));
                    else if(fn<16)dest[i]=sub(vacc[i],mul(a,b));
                    else dest[i]=mul(a,b);
                }v[fd]=dest;
            } else if(rs>=16&&((special>=0x1bc&&special<=0x1bf)||(special>=0xbc&&special<=0xbf))) {
                for(unsigned i=0;i<4;++i)if(mask&(8u>>i)){const float product=mul(v[rd][i],v[rt][special&3]);vacc[i]=special>=0x1bc?product:add(vacc[i],product);}
            } else if(w==0x4a6203beu){const float d=v[2][0];if(d==0)q=fp((bits(d)&0x80000000u)|0x7f7fffffu);else{volatile float t=std::sqrt(std::fabs(d));q=divide(1,t);}}
            else if(w==0x4bc008dcu){for(unsigned i=0;i<3;++i)v[3][i]=mul(v[1][i],q);}
            else if(w==0x4bc108aau){for(unsigned i=0;i<3;++i)v[2][i]=mul(v[1][i],v[1][i]);}
            else if((w&0x7ffu)==0x2feu)''')
oracle=oracle.replace('    unsigned steps=0;', '    unsigned steps=0;\n    std::array<bool,8> wrapperCoverage{};')
oracle=oracle.replace('        currentPc=pc;annulDelay=false;', '''        currentPc=pc;annulDelay=false;
        const std::uint32_t gates[]={0x167c7c,0x16819c,0x1681f8,0x168228,0x168234,0x1682fc,0x1654b0,0x168354};
        for(unsigned i=0;i<8;++i)if(pc==gates[i])wrapperCoverage[i]=true;''')
(ROOT/'tools/gameplay/body_wrapper_oracle48h.h').write_text(oracle)
(OUT/'recovery.json').write_text(json.dumps(dict(elf_sha256=hashlib.sha256(elf).hexdigest(),ram_sha256=hashlib.sha256(ram).hexdigest(),blocks=proof,sources=sources,saved=saved),indent=2)+'\n')
print(json.dumps(saved))
