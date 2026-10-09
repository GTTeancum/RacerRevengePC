"""Generate a bounded real-game leaf differential and opt-in native adapter."""
from pathlib import Path
import json,hashlib
root=Path(__file__).resolve().parents[1]
out=root/'build/leaf47M';out.mkdir(exist_ok=True)
manifest=json.loads((root/'evidence/47M/leaf-eligibility.json').read_text())
deferred=[e for e in manifest['functions'] if set(e['operations']) & {'MOVN','MOVZ'}]
entries=[e for e in manifest['functions'] if not set(e['operations']) & {'MOVN','MOVZ'}]
(out/'selection.json').write_text(json.dumps(dict(eligible=len(entries),deferred=deferred,reason='Legacy generated MOVN/MOVZ writes 128 bits; supplied PCSX2 handlers preserve upper 64 bits. Keep these functions on the original path pending separate semantics correction.'),indent=2))
lines=['#include "compact_leaf_native47m.h"','#include <cstdio>','#include <cstdlib>']
for i,e in enumerate(entries):
    assert hashlib.sha256((root/e['path']).read_bytes()).hexdigest()==e['source_sha256']
    lines.append(f'extern void {e["symbol"]}(uint8_t*,R5900Context*,PS2Runtime*);')
    lines.append(f'static const uint32_t words{i}[]={{'+','.join(hex(w)+'u' for w in e['words'])+'};')
lines.append('static const rr_leaf47m::Entry entries[]={')
for i,e in enumerate(entries):
    lines.append('{{'+f'{hex(e["entry"])}u,words{i},{len(e["words"])}'+'},&'+e['symbol']+','+str(e['original_sets_delay_state']).lower()+'},')
lines.append('};')
(out/'entries.inc').write_text('\n'.join(lines)+'\n')
check=r'''#include "entries.inc"
#include <type_traits>
static_assert(std::is_trivially_copyable_v<R5900Context>);
static uint32_t next(uint32_t& s){s^=s<<13;s^=s>>17;s^=s<<5;return s;}
int main(){
    uint32_t seed=0x47dfe012,digest=2166136261u;unsigned checks=0,failures=0;
    const uint64_t edges[]={0,1,UINT64_MAX,0x80000000u,0x7fffffffu,0x8000000000000000ull,0x7fffffffffffffffull,0x5555aaaaffff0000ull};
    for(const auto& entry:entries)for(unsigned round=0;round<256;++round){
        R5900Context input;
        // Fully initialized object representation permits a whole-context comparison,
        // including fields outside the supported instruction subset and padding.
        auto* bytes=reinterpret_cast<unsigned char*>(&input);
        for(size_t i=0;i<sizeof input;++i)bytes[i]=static_cast<unsigned char>(next(seed));
        input.in_delay_slot=(round&1)!=0;
        for(unsigned r=0;r<32;++r){uint64_t lanes[2];for(unsigned n=0;n<2;++n)lanes[n]=round<64?edges[(round+r+n)%8]:(uint64_t(next(seed))<<32)|next(seed);std::memcpy(input.r+r,lanes,16);}
        input.pc=entry.leaf.entry;
        R5900Context original,candidate;std::memcpy(&original,&input,sizeof input);std::memcpy(&candidate,&input,sizeof input);
        entry.original(nullptr,&original,nullptr);
        bool ok=rr_leaf47m::execute(candidate,entry);++checks;
        if(!ok || std::memcmp(&original,&candidate,sizeof input)){
            ++failures;if(failures<=12){size_t offset=0;auto* a=reinterpret_cast<unsigned char*>(&original);auto* b=reinterpret_cast<unsigned char*>(&candidate);while(offset<sizeof input&&a[offset]==b[offset])++offset;
            std::printf("Mismatch entry=%08x round=%u offset=%zu ok=%u expected=%02x actual=%02x\n",entry.leaf.entry,round,offset,unsigned(ok),offset<sizeof input?a[offset]:0,offset<sizeof input?b[offset]:0);}
        }
        for(size_t i=0;i<sizeof candidate;++i)digest=(digest^reinterpret_cast<unsigned char*>(&candidate)[i])*16777619u;
    }
    std::printf("NativeLeaf47M functions=%zu cases=%u failures=%u digest=%08x\n",sizeof(entries)/sizeof(entries[0]),checks,failures,digest);
    return failures?1:0;
}
'''
(out/'check.cpp').write_text(check)
adapter=['#include "entries.inc"','namespace rr_leaf47m {','static uint64_t calls=0,fallbacks=0;','struct Report {~Report(){std::fprintf(stderr,"[rr:leaf47m] calls=%llu fallbacks=%llu\\n",(unsigned long long)calls,(unsigned long long)fallbacks);}};static Report report;']
for i,e in enumerate(entries):
    adapter.append(f'static void wrap{i}(uint8_t* ram,R5900Context* ctx,PS2Runtime* runtime){{++calls;if(!execute(*ctx,entries[{i}])){{++fallbacks;entries[{i}].original(ram,ctx,runtime);}}}}')
adapter.append('bool install(PS2Runtime& runtime){unsigned installed=0,skipped=0;')
for i,e in enumerate(entries):
    adapter.append(f'if(runtime.lookupFunction(entries[{i}].leaf.entry)==entries[{i}].original){{if(!runtime.replaceFunction(entries[{i}].leaf.entry,wrap{i}))return false;++installed;}}else ++skipped;')
adapter+=['std::fprintf(stderr,"[rr:leaf47m] installed=%u skipped_owner=%u\\n",installed,skipped);return installed!=0;}','}']
(out/'adapter.cpp').write_text('\n'.join(adapter)+'\n')
probe=(root/'work/native/project/src/probe.cpp').read_text()
needle='        if(std::getenv("RR_CAPTURE_FRAME") || std::getenv("RR_CAPTURE_SEQUENCE"))'
assert probe.count(needle)==1
probe=probe.replace('#include "ps2_runtime.h"','#include "ps2_runtime.h"\nnamespace rr_leaf47m {bool install(PS2Runtime&);}')
probe=probe.replace(needle,'        if(!rr_leaf47m::install(runtime))return 21;\n'+needle)
(out/'probe.cpp').write_text(probe)
print('Prepared',len(entries),'ELF-verified native leaf comparisons and wrappers.')
