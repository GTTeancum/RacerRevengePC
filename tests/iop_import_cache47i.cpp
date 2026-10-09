#include "emulator/core/iop_memory.h"
#include "emulator/imports/iop_imports.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>
using namespace ps2x::iop::detail;

int main() {
    auto a=std::make_unique<IopMemory>(),b=std::make_unique<IopMemory>();
    IopImportRegistry original(*a),cached(*b);
    _putenv_s("RR_IOP_IMPORT_CACHE47I","0"); original.reset();
    _putenv_s("RR_IOP_IMPORT_CACHE47I","1"); cached.reset();
    unsigned checks=0;
    auto compare=[&](uint32_t pc) {
        ++checks;
        auto x=original.decode(pc),y=cached.decode(pc);
        const bool equal=bool(x)==bool(y) && (!x || (x->library==y->library && x->ordinal==y->ordinal && x->version==y->version));
        if(!equal) std::cerr<<"decode mismatch pc="<<std::hex<<pc<<std::dec<<" check="<<checks<<"\n";
        return equal;
    };
    auto table=[&](uint32_t address,unsigned count,const char* name) {
        for(auto* m:{a.get(),b.get()}) {
            m->write32(address,0x41e00000);
            m->write32(address+4,0);
            m->write32(address+8,0x102);
            char bytes[8]{};
            std::memcpy(bytes,name,std::min<size_t>(8,std::strlen(name)));
            if(!m->writeRam(address+12,bytes,8)) std::abort();
            for(unsigned n=0;n<count;++n) {
                m->write32(address+20+n*8,0x03e00008);
                m->write32(address+24+n*8,0x24000000 | (n+10));
            }
            m->write32(address+20+count*8,0);
            m->write32(address+24+count*8,0);
        }
    };
    constexpr uint32_t base=0x18000,pc=base+20+6*8;
    table(base,8,"thbase");
    for(unsigned n=0;n<1024;++n) if(!compare(pc)) return 1;
    if(cached.decodeStats().hits<1023) return 2;
    if(!compare(0x80000000u|pc) || !compare(0xa0000000u|pc)) return 3;
    // Every public RAM write route, including alias writes and page crossings.
    for(auto* m:{a.get(),b.get()}) m->write8(base+12,'x');
    if(!compare(pc)) return 4;
    for(auto* m:{a.get(),b.get()}) m->write16(base+8,0x203);
    if(!compare(pc)) return 5;
    for(auto* m:{a.get(),b.get()}) m->write32(0xa0000000u| (pc+4),0x24001234);
    if(!compare(pc)) return 6;
    const char replacement[8]={'s','i','f','c','m','d',0,0};
    for(auto* m:{a.get(),b.get()}) if(!m->writeRam(base+12,replacement,8)) return 7;
    if(!compare(pc)) return 8;
    const auto hits=cached.decodeStats().hits;
    for(auto* m:{a.get(),b.get()}) m->write32(0x120000,0xdeadbeef);
    if(!compare(pc) || cached.decodeStats().hits!=hits+1) return 9;
    // A newly inserted nearer header must supersede the previously cached one.
    table(base+0x20,4,"nearer");
    if(!compare(pc)) return 10;
    const auto nearer=cached.decode(pc);
    if(!nearer || nearer->library!="nearer") return 10;
    table(base,8,"thbase");
    if(!compare(pc)) return 11;
    for(auto* m:{a.get(),b.get()}) if(!m->zeroRam(base+20,8)) return 12;
    if(!compare(pc) || cached.decode(pc)) return 13;
    // A cache collision must neither reuse another PC nor drop old semantics.
    table(base,8,"thbase"); table(base+0x800,8,"intrman");
    for(unsigned n=0;n<64;++n) if(!compare(pc) || !compare(pc+0x800)) return 14;
    table(0x20ff0,40,"crossing");
    const uint32_t crossing=0x20ff0+20+39*8;
    if(!compare(crossing)) return 15;
    for(auto* m:{a.get(),b.get()}) m->write16(0x20fff,0);
    if(!compare(crossing)) return 16;
    // Direct device-to-RAM DMA also passes through the write watcher.
    table(base,8,"thbase");
    if(!compare(pc)) return 17;
    for(auto* m:{a.get(),b.get()}) {
        m->write32(0x1f9001a8,0);
        m->write32(0x1f8010c0,base);
        m->write32(0x1f8010c4,1);
        m->write32(0x1f8010c8,0x01000000);
    }
    if(!compare(pc) || cached.decode(pc)) return 18;
    // Do not reset the registries: memory reset alone must invalidate cached data.
    table(base,8,"thbase");
    if(!compare(pc)) return 19;
    a->reset(); b->reset();
    if(!compare(pc) || cached.decode(pc)) return 20;
    table(base,8,"thbase");
    for(unsigned n=0;n<4096;++n) {
        const uint32_t address=(n%3==0)?base+8:(n%3==1)?pc+4:0x130000;
        const uint32_t value=address==pc+4?0x24000000u|(n&0xffffu):n;
        a->write32(address,value); b->write32(address,value);
        if(!compare(pc) || !compare(pc)) return 21;
    }
    std::vector<uint8_t> ra(IopMemory::RamSize),rb(ra.size());
    if(!a->readRam(0,ra.data(),ra.size()) || !b->readRam(0,rb.data(),rb.size()) || ra!=rb) return 22;
    const auto x=original.decodeStats(),y=cached.decodeStats();
    if(!y.hits || y.searchWords>=x.searchWords) return 23;
    std::cout<<"import-cache checks="<<checks<<" mismatches=0 ram_bytes="<<ra.size()
             <<" hits="<<y.hits<<" original_search_words="<<x.searchWords
             <<" cached_search_words="<<y.searchWords<<" cache_bytes="<<cached.decodeCacheBytes()<<"\n";
}
