#include "../../src/game/original_water_topology_initialization50la.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <vector>

int main(){
    using namespace rr::game;
    using S=OriginalWaterTopologyInitialization50LA::Status;
    for(const auto count:std::array<std::uint32_t,6>{0,1,2,3,4,16384}){
        for(std::size_t alignment=0;alignment<16;++alignment){
            const std::size_t bytes=std::size_t(count)*20;
            std::vector<unsigned char> memory(bytes+48,0xa5);
            auto* storage=memory.data()+16+alignment;
            OriginalWaterTopologyGlobals50LA globals;
            const auto first=initializeOriginalWaterTopology50LA(globals,storage,bytes,count);
            assert(first.status==S::initialized&&first.guardBefore==0&&first.assertionStored&&first.guardStored);
            assert(first.assertionRead==1u&&first.assertionReturn==0u&&first.assertionCalled&&first.zeroFillReturned);
            assert(first.zeroedBytes==bytes&&first.pc==0x2babe4);
            assert(globals.guard3D670C==1&&globals.assertion443FC0==1);
            for(std::size_t i=0;i<memory.size();++i)
                assert(memory[i]==(i>=16+alignment&&i<16+alignment+bytes?0:0xa5));
            std::memset(storage,0x5a,bytes);
            const auto next=initializeOriginalWaterTopology50LA(globals,storage,bytes,count);
            assert(next.status==S::initialized&&next.guardBefore==1&&!next.assertionStored&&!next.guardStored);
            for(std::size_t i=0;i<bytes;++i)assert(storage[i]==0);
        }
    }
    std::array<unsigned char,20> memory;memory.fill(0x5a);
    for(const auto invalid:std::array<OriginalWaterTopologyGlobals50LA,4>{{
        {0,0,false},{1,0,true},{0,1,true},{2,2,true}}}){
        auto globals=invalid;
        const auto result=initializeOriginalWaterTopology50LA(globals,memory.data(),memory.size(),1);
        assert(result.status==S::invalidGlobals&&!result.guardBefore&&!result.zeroFillReturned);
        assert(globals.guard3D670C==invalid.guard3D670C&&globals.assertion443FC0==invalid.assertion443FC0&&globals.known==invalid.known);
        for(const auto value:memory)assert(value==0x5a);
    }
    OriginalWaterTopologyGlobals50LA globals;
    assert(initializeOriginalWaterTopology50LA(globals,nullptr,0,0).status==S::invalidBacking);
    assert(initializeOriginalWaterTopology50LA(globals,memory.data(),19,1).status==S::invalidBacking);
    assert(initializeOriginalWaterTopology50LA(globals,memory.data(),21,1).status==S::invalidBacking);
    assert(initializeOriginalWaterTopology50LA(globals,memory.data(),20,0x40000001).status==S::invalidBacking);
    assert(!globals.guard3D670C&&!globals.assertion443FC0);
    for(const auto value:memory)assert(value==0x5a);
    std::puts("Water topology initialization: 96 extent/alignment cases, repeat-state preservation and atomic refusals passed");
}
