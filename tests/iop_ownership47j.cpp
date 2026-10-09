#include "emulator/core/iop_memory.h"
#include "memory_reference47j.h"
#include <array>
#include <iostream>
#include <memory>
#include <vector>
#include <type_traits>
using namespace ps2x::iop::detail;
#ifdef RR_OWNERSHIP_LIFETIME_GUARD
static_assert(!std::is_copy_constructible_v<IopMemory> && !std::is_move_constructible_v<IopMemory>);
#endif
int main() {
    auto original=std::make_unique<OriginalIopMemory47J>();
    auto candidate=std::make_unique<IopMemory>();
    auto& a=*original; auto& b=*candidate;
    constexpr uint32_t bytes=IopMemory::RamSize;
    uint32_t seed=0x914ace37;
    auto random=[&] {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;};
    unsigned queries=0;
    auto query=[&](uint32_t address,size_t size) {
        ++queries;
        if(a.ownsRamRange(address,size)==b.ownsRamRange(address,size)) return true;
        std::cerr<<"ownership differs at "<<std::hex<<address<<std::dec<<" size="<<size<<"\n";
        return false;
    };
    if(a.ownershipStorageBytes()!=bytes || b.ownershipStorageBytes()!=bytes/8) return 1;
    for(unsigned n=0;n<12000;++n) {
        const uint32_t p=random()%(bytes-256), value=random();
        const size_t size=random()%257;
        std::array<uint8_t,256> block{};
        for(unsigned i=0;i<block.size();++i) block[i]=uint8_t(value+i);
        switch(n%8) {
        case 0:a.write8(p,uint8_t(value));b.write8(p,uint8_t(value));break;
        case 1:a.write16(p,uint16_t(value));b.write16(p,uint16_t(value));break;
        case 2:a.write32(p,value);b.write32(p,value);break;
        case 3:if(a.writeRam(p,block.data(),size)!=b.writeRam(p,block.data(),size))return 2;break;
        case 4:if(a.zeroRam(p,size)!=b.zeroRam(p,size))return 3;break;
        case 5:{const auto x=a.allocate(31+(value&63u)),y=b.allocate(31+(value&63u));
            if(x!=y || !query(x,31+(value&63u)))return 4;
            if(x && (a.freeAllocation(x)!=b.freeAllocation(y)))return 5;
            if(!query(x,31+(value&63u)))return 6;break;}
        case 6:a.write32(0xa0000000u|p,value);b.write32(0xa0000000u|p,value);break;
        case 7:{auto dma=[&](auto& m){m.write32(0x1f9001a8,0);m.write32(0x1f8010c0,p&~3u);m.write32(0x1f8010c4,1);m.write32(0x1f8010c8,0x01000000);};dma(a);dma(b);break;}
        }
        if(!query(p,size) || !query(0x80000000u|p,size) || !query(p,1) || !query(p+size,0))return 7;
        const uint32_t q=random()%(bytes-256);
        if(!query(q,random()%257))return 8;
    }
    for(uint32_t p=0;p<bytes;++p) if(!query(p,1))return 9;
    for(uint32_t p=0;p<bytes;p+=4096) if(!query(p,4096))return 10;
    if(!query(bytes,0) || !query(bytes,1) || !query(bytes+1,0) || !query(1,SIZE_MAX))return 11;
    std::vector<uint8_t> ra(bytes),rb(bytes);
    if(!a.readRam(0,ra.data(),bytes) || !b.readRam(0,rb.data(),bytes) || ra!=rb)return 12;
    a.reset();b.reset();
    for(uint32_t p=0;p<bytes;p+=32) if(!query(p,32))return 13;
    if(!a.zeroRam(0,bytes) || !b.zeroRam(0,bytes))return 14;
    if(!query(0,bytes) || !b.ownsRamRange(0,bytes))return 14;
    std::cout<<"memory-ownership queries="<<queries<<" mismatches=0 ram_bytes="<<bytes
             <<" original_storage="<<a.ownershipStorageBytes()<<" compact_storage="<<b.ownershipStorageBytes()
             <<" saved_bytes="<<a.ownershipStorageBytes()-b.ownershipStorageBytes()<<"\n";
}
