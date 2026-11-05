#include "../../src/game/original_water_topology_request50la.h"
#include <cassert>
#include <cstdint>
#include <cstdio>

int main(){
    using rr::game::originalWaterTopologyRequest50LA;
    using A=rr::game::OriginalWaterTopologyRequest50LA::Admission;
    //Zero remains a raw zero request, not scalar-new's one-byte coercion.
    const auto zero=originalWaterTopologyRequest50LA(0,16384,1048576);
    assert(zero.fullBytes==0&&zero.sourceRequest==0&&zero.admission==A::accepted);
    for(std::uint32_t count=1;count<=16384;++count){
        const auto p=originalWaterTopologyRequest50LA(count,16384,1048576);
        assert(p.fullBytes==std::uint64_t(count)*20&&p.sourceRequest==count*20&&p.admission==A::accepted);
    }
    assert(originalWaterTopologyRequest50LA(16385,16384,1048576).admission==A::countCapacity);
    assert(originalWaterTopologyRequest50LA(100,100,1999).admission==A::byteCapacity);
    assert(originalWaterTopologyRequest50LA(100,100,2000).admission==A::accepted);
    const auto wrapped=originalWaterTopologyRequest50LA(0x40000000,0xffffffff,0xffffffff);
    assert(wrapped.sourceRequest==0&&wrapped.fullBytes==0x500000000ull&&wrapped.admission==A::byteCapacity);
    //Relaxed test capacities exercise negative source-word and sum-overflow
    //refusal independently of the production count/byte limits.
    assert(originalWaterTopologyRequest50LA(0x08000000,0xffffffff,0xffffffff).admission==A::requestMismatch);
    assert(originalWaterTopologyRequest50LA(107374182,0xffffffff,0xffffffff).admission==A::requestMismatch);
    const auto negative=originalWaterTopologyRequest50LA(0xffffffff,16384,1048576);
    assert(negative.sourceRequest==0xffffffec&&negative.admission==A::countCapacity);
    std::puts("Water topology request: zero, 16384 positive counts, capacities and wrapped/signed domains passed");
}
