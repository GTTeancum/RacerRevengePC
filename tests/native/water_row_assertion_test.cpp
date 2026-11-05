#include "../../src/game/original_water_row_assertion50la.h"
#include "../../src/game/original_water_topology_initialization50la.h"
#include <array>
#include <cassert>
#include <cstdio>
int main(){
    using namespace rr::game;using R=OriginalWaterRowAssertion50LA;
    std::array<unsigned char,32> storage;storage.fill(0x5a);
    OriginalWaterRowGlobals50LA globals;
    auto null=admitOriginalWaterRowOutput50LA(globals,nullptr);
    assert(null==R{}&&!globals.guard3D6644&&!globals.assertion443638);
    const auto first=admitOriginalWaterRowOutput50LA(globals,storage.data());
    assert(first.status==R::Status::accepted&&first.guardBefore==0&&first.pc==0x2994bcu);
    assert(first.assertionStored&&first.guardStored&&first.assertionCalled&&first.assertionRead==1u&&first.assertionReturn==0u);
    assert(globals.guard3D6644==1&&globals.assertion443638==1);
    const auto second=admitOriginalWaterRowOutput50LA(globals,storage.data());
    assert(second.status==R::Status::accepted&&second.guardBefore==1&&!second.assertionStored&&!second.guardStored);
    assert(second.assertionCalled&&second.assertionReturn==0u);
    //The distinct topology globals remain untouched by row initialization.
    OriginalWaterTopologyGlobals50LA topology;
    assert(!topology.guard3D670C&&!topology.assertion443FC0);
    for(auto byte:storage)assert(byte==0x5a); //Assertion helper is not zero-fill.
    for(unsigned guard=0;guard<4u;++guard)for(unsigned word=0;word<4u;++word){
        if(guard==word&&guard<=1u)continue;
        OriginalWaterRowGlobals50LA bad{static_cast<std::uint8_t>(guard),word,true};
        const auto result=admitOriginalWaterRowOutput50LA(bad,storage.data());
        assert(result.status==R::Status::invalidGlobals&&!result.guardBefore&&!result.assertionCalled);
        assert(!result.assertionStored&&!result.guardStored&&bad.guard3D6644==guard&&bad.assertion443638==word);
    }
    globals.known=false;
    const auto unknown=admitOriginalWaterRowOutput50LA(globals,storage.data());
    assert(unknown.status==R::Status::invalidGlobals&&!unknown.assertionCalled&&!globals.known);
    assert(globals.guard3D6644==1&&globals.assertion443638==1);
    std::puts("Row assertion: first/repeated initialization, null refusal, distinct pair, poisoned/inconsistent state and unchanged storage passed");
}
