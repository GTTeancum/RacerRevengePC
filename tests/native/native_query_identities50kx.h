#pragma once
#include <cstdint>

// Opaque native identities for the actual query owners in one active racer
// arena. Follow/death are unbanked and join bank0; POD694 must remain distinct
// from death120 even before that camera's query is attached. These values are
// neither guest addresses nor collection refs, and do not construct owners.
namespace native_query_identities50KX {
inline constexpr std::uint32_t movement7A4=0xf0000001u;
inline constexpr std::uint32_t secondary364=0xf0000002u;
inline constexpr std::uint32_t terrain348=0xf0000003u;
inline constexpr std::uint32_t followSphere24=0xf0000004u;
inline constexpr std::uint32_t followRay190=0xf0000005u;
inline constexpr std::uint32_t death120=0xf0000006u;
inline constexpr std::uint32_t pod694=0xf0000007u;
static_assert([] {
    constexpr std::uint32_t ids[]{movement7A4,secondary364,terrain348,
        followSphere24,followRay190,death120,pod694};
    for(unsigned i=0;i<sizeof ids/sizeof ids[0];++i){
        if(!ids[i])return false;
        for(unsigned j=0;j<i;++j)if(ids[i]==ids[j])return false;
    }
    return true;
}(),"actual native query owners must have distinct nonzero identities");
}
