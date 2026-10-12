#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::game {
//The ONE actual115B60/115DE0 camera collection for this controller. This is
//outside racer snapshots. Follow64, attached6C and procedural7C bindings all
//borrow this same owner; they must not keep independent selected34 copies.
struct OriginalCameraChildRegistry {
    std::uint64_t sessionGeneration=0,controllerKey=0,sceneKey=0;
    const std::uint64_t* registered38=nullptr;
    std::size_t count30=0,selected34=0;
    bool bound=false;
};
//Cold projection after the actual1752D0/115DE0 ordered registrations finish.
//The supplied keys must identify those real owners in their retained append
//order; this function cannot create/register a missing camera. Source current
//index starts0. Repeated identities are retained, matching115E60 first match.
inline bool bindFreshOriginalCameraChildRegistry(OriginalCameraChildRegistry& out,
    std::uint64_t session,std::uint64_t controller,std::uint64_t scene,
    const std::uint64_t* actualOrder,std::size_t count) noexcept {
    if(out.bound||!session||!controller||!scene||!actualOrder||!count||count>64)return false;
    for(std::size_t i=0;i<count;++i)if(!actualOrder[i])return false;
    out={session,controller,scene,actualOrder,count,0,true};return true;
}
}
