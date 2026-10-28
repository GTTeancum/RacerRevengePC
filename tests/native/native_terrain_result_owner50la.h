#pragma once
#include "static_type2_producer49x.h"
#include <array>

namespace ra_source_camera50LA {class OwnershipBridge;}
namespace native_terrain_result50LA {
//Separate native representation of the one-pair11C3F0 result90 allocation.
//NOT a spring detector result, emulated guest pointer, or initialized history.
//The source capacity is three contacts per ray. Unwritten scalar/metadata
//backing is value-initialized only for C++ safety and is not readable authority.
class Owner {
    friend class ra_source_camera50LA::OwnershipBridge;
    using Vec=rr::game::SpringVector4_48N;
    rr::game::Type2Selected49X selected_{};
    std::array<Vec,6> endpoints50_{};
    std::array<Vec,3> normals54_{},surfacePoints58_{};
    std::array<std::uint32_t,3> words64_{},words68_{},words6C_{};
    std::array<std::uint16_t,3> tags70_{};
    std::uint32_t count60_=0;
    bool constructed_=false,revoked_=false,resetKnown_=false;
    //No constructor inference for selected.fraction/flags/identity/face/tags,
    //count60, or any per-contact scalar. Only a reached reset authorizes count
    //and fraction; no consumer may read an unappended contact slot.
    bool construct()noexcept {
        if(constructed_||revoked_)return false;
        const Vec unit{{0.f,0.f,0.f,1.f}};
        selected_.first=selected_.second=selected_.normal=
            selected_.surfacePoint=selected_.correction=unit;
        endpoints50_.fill(unit);normals54_.fill(unit);surfacePoints58_.fill(unit);
        constructed_=true;return true;
    }
    void revoke()noexcept {revoked_=true;}
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
};
}
