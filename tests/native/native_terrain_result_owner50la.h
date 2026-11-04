#pragma once
#include "static_type2_producer49x.h"
#include <array>
#include "../../src/game/original_wreck_type2_owners50la.h"

namespace ra_source_camera50LA {class OwnershipBridge;}
namespace rr::game {class OriginalRacerTerrainQueryOwner50LA;}
namespace native_terrain_result50LA {
//Separate native representation of the one-pair11C3F0 result90 allocation.
//NOT a spring detector result, emulated guest pointer, or initialized history.
//The source capacity is three contacts per ray. Unwritten scalar/metadata
//backing is value-initialized only for C++ safety and is not readable authority.
class Owner {
    friend class ra_source_camera50LA::OwnershipBridge;
    friend class rr::game::OriginalRacerTerrainQueryOwner50LA;
    friend class native_repulsor_payload50LA::Owner;
    //Source-only borrow. Fixed legacy arrays below are NEVER constructed or
    //read on this route. The counted payload owns every result allocation.
    const rr::game::OriginalWreckType2Owners50LA::Result* sourceResult90_=nullptr;
    bool sourceWithdrawn_=false;
    rr::game::OriginalWreckType2Owners50LA::Result* nativeResult90_=nullptr;
    const native_repulsor_payload50LA::Owner* nativePayload_=nullptr;
    bool legacyWithdrawn_=false,selectedHitKnown_=false;
    //Native diagnostics only, not a duplicate Result vector/fraction/flags lane.
    struct NativeDiagnostics {std::uint32_t candidateIdentity=0,face=0;std::uint16_t queryTag=0;};
    NativeDiagnostics nativeDiagnostics_;
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
        if(constructed_||revoked_||sourceResult90_||sourceWithdrawn_||nativePayload_||legacyWithdrawn_)return false;
        const Vec unit{{0.f,0.f,0.f,1.f}};
        selected_.first=selected_.second=selected_.normal=
            selected_.surfacePoint=selected_.correction=unit;
        endpoints50_.fill(unit);normals54_.fill(unit);surfacePoints58_.fill(unit);
        constructed_=true;return true;
    }
    //Direct typed borrows: native replacement never reads/copies fixed backing.
    //These are private to the already-validated same invocation consumers.
    Vec& first()noexcept{return nativeResult90_?nativeResult90_->first00:selected_.first;}
    Vec& second()noexcept{return nativeResult90_?nativeResult90_->second10:selected_.second;}
    Vec& normal()noexcept{return nativeResult90_?nativeResult90_->normal20:selected_.normal;}
    const Vec& normal()const noexcept{return nativeResult90_?nativeResult90_->normal20:selected_.normal;}
    Vec& point()noexcept{return nativeResult90_?nativeResult90_->point30:selected_.surfacePoint;}
    const Vec& point()const noexcept{return nativeResult90_?nativeResult90_->point30:selected_.surfacePoint;}
    Vec& correction()noexcept{return nativeResult90_?nativeResult90_->correction40:selected_.correction;}
    float& fraction()noexcept{return nativeResult90_?nativeResult90_->selectedFraction74:selected_.fraction;}
    std::uint32_t& hits()noexcept{return nativeResult90_?nativeResult90_->hits60:count60_;}
    const std::uint32_t& hits()const noexcept{return nativeResult90_?nativeResult90_->hits60:count60_;}
    Vec& endpoint(std::size_t i)noexcept{return nativeResult90_?
        (i%2?nativeResult90_->pairs50[i/2].second:nativeResult90_->pairs50[i/2].first):endpoints50_[i];}
    Vec& contactNormal(std::size_t i)noexcept{return nativeResult90_?nativeResult90_->normals54[i]:normals54_[i];}
    Vec& contactPoint(std::size_t i)noexcept{return nativeResult90_?nativeResult90_->points58[i]:surfacePoints58_[i];}
    std::uint32_t& contactWord(std::size_t i)noexcept{return nativeResult90_?nativeResult90_->words64[i]:words64_[i];}
    std::uint16_t& contactTag(std::size_t i)noexcept{return nativeResult90_?nativeResult90_->tags70[i]:tags70_[i];}
    float contactFraction(std::size_t i)const noexcept {
        if(nativeResult90_)return nativeResult90_->fractions6C[i];
        float value;std::memcpy(&value,&words6C_[i],sizeof(value));return value;
    }
    void contactFraction(std::size_t i,float value)noexcept {
        if(nativeResult90_)nativeResult90_->fractions6C[i]=value;
        else std::memcpy(&words6C_[i],&value,sizeof(value));
    }
    void withdrawLegacy()noexcept {
        //No unknown count/contact read, and no claim of original11B600.
        constructed_=false;legacyWithdrawn_=true;resetKnown_=false;selectedHitKnown_=false;
    }
    void revoke()noexcept {revoked_=true;}
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
};
}
