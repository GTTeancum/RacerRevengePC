#pragma once
#include "original_water_topology_request50la.h"
#include <cstddef>
#include <cstring>
#include <optional>

namespace rr::game {
//Separate process BSS from the provider initializer's 3D66F0/443EE8 pair.
//World cancellation must not reset it. Unjoined writers invalidate knownness.
struct OriginalWaterTopologyGlobals50LA {
    std::uint8_t guard3D670C=0;
    std::uint32_t assertion443FC0=0;
    bool known=true;
};
struct OriginalWaterTopologyInitialization50LA {
    enum class Status {refused,alreadyAttempted,invalidGlobals,invalidBacking,initialized};
    Status status=Status::refused;
    std::uint32_t pc=0;
    std::optional<std::uint8_t> guardBefore;
    bool assertionStored=false,guardStored=false,assertionCalled=false,zeroFillReturned=false;
    std::optional<std::uint32_t> assertionRead,assertionReturn;
    std::size_t zeroedBytes=0;
    bool operator==(const OriginalWaterTopologyInitialization50LA&)const noexcept=default;
};

//Native equivalent of the bounded nonnull arm at2BAB60..2BABE0. The caller
//authenticates the allocation role, lifetime, exact extent and owner thread.
//No guest pointer bits, allocator return, index allocation or loader return.
inline OriginalWaterTopologyInitialization50LA initializeOriginalWaterTopology50LA(
    OriginalWaterTopologyGlobals50LA& globals,void* storage,std::size_t extent,
    std::uint32_t count)noexcept {
    using R=OriginalWaterTopologyInitialization50LA;R r;
    const auto plan=originalWaterTopologyRequest50LA(count,16384u,1048576u);
    if(!storage||plan.admission!=OriginalWaterTopologyRequest50LA::Admission::accepted||
       extent!=plan.fullBytes){r.status=R::Status::invalidBacking;return r;}
    r.pc=0x2bab60u;
    if(!globals.known||globals.guard3D670C>1u||globals.assertion443FC0>1u||
       globals.guard3D670C!=globals.assertion443FC0){r.status=R::Status::invalidGlobals;return r;}
    r.guardBefore=globals.guard3D670C;
    if(!*r.guardBefore){
        r.pc=0x2bab74u;globals.assertion443FC0=1u;r.assertionStored=true;
        r.pc=0x2bab78u;globals.guard3D670C=1u;r.guardStored=true;
    }
    r.pc=0x2bab80u;r.assertionRead=globals.assertion443FC0;
    //Authenticated storage is nonnull. Both debug36C100 branches of133660
    //return0 without diagnostics in this arm; no debug value is fabricated.
    r.pc=0x2bab98u;r.assertionCalled=true;r.assertionReturn=0u;
    //306478 writes the supplied byte over exactly the supplied extent for
    //every alignment (including zero length), then returns the same role.
    r.pc=0x2babdcu;std::memset(storage,0,extent);
    r.zeroedBytes=extent;r.zeroFillReturned=true;r.pc=0x2babe4u;
    r.status=R::Status::initialized;return r;
}
} //namespace rr::game
