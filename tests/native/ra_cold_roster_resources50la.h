#pragma once
#include "native_racer_rig50hw_fixture.h"
#include "native_racer_collision50hw_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "original_disc_effects50lb.h"
#include <array>
#include <memory>
#include <string>

namespace ra_cold_aggregate50LA { class Owner; }
namespace ra_cold_roster50LA {
namespace g=rr::game;namespace a=rr::assets;
class Operation;
// One-time adoption of the EXISTING active roster objects (capacity eight), before world reservation.
// Unique-pointer movement changes no Rig/COL address. No public array getter:
// the private stable array is the future aggregate grid/camera borrow.
struct SharedOwners final {
    std::array<std::unique_ptr<native_racer50HW::Rig>,8> rigs;
    std::array<std::unique_ptr<native_racer_collision50HW::Owner>,8> collisions;
};
class Resources final {
    friend class Operation;
    friend class ra_cold_aggregate50LA::Owner;
    std::shared_ptr<SharedOwners> owners_;
    const std::array<std::unique_ptr<native_racer50HW::Rig>,8>& rigs_;
    const std::array<std::unique_ptr<native_racer_collision50HW::Owner>,8>& collisions_;
    std::array<std::vector<std::uint8_t>,8> aiSources_;
    std::array<std::vector<std::uint8_t>,6> effects_;
    std::uint32_t humanCount_=0;
    std::size_t racerCount_=8;
    explicit Resources(std::shared_ptr<SharedOwners> owners)
        :owners_(std::move(owners)),rigs_(owners_->rigs),collisions_(owners_->collisions){}
public:
    Resources(const Resources&)=delete;Resources& operator=(const Resources&)=delete;
    static std::shared_ptr<const Resources> adopt(
        std::shared_ptr<SharedOwners> owners,std::string& error,std::uint32_t humanCount=0,std::size_t racerCount=8) noexcept {
        try {
            if(!racerCount||racerCount>8u||humanCount>1u||humanCount>racerCount)throw std::runtime_error("RA roster requires zero or one actual human");
            if(!owners)throw std::runtime_error("actual retained roster storage absent");
            auto result=std::shared_ptr<Resources>(new Resources(std::move(owners)));
            result->humanCount_=humanCount;result->racerCount_=racerCount;
            const auto& rigs=result->rigs_;const auto& collisions=result->collisions_;
            for(std::size_t slot=0;slot<racerCount;++slot){
                const auto* rig=rigs[slot].get();const auto* collision=collisions[slot].get();
                if(!rig||!collision||!collision->prepared()||(rig->modelCount!=3u&&rig->modelCount!=5u))
                    throw std::runtime_error("actual original roster owners or bounded model roles absent");
                for(std::size_t old=0;old<slot;++old)if(rigs[old]->podId==rig->podId)
                    throw std::runtime_error("duplicate actual roster identity");
                const auto* selected=g::original_racer_selection50IJ::findByPodId(rig->podId);
                const auto* pin=selected?original_player_identity50IK::find(selected->racer60):nullptr;
                if(!pin)throw std::runtime_error("original roster source pin absent");
                const auto bytes=runtime_startup50fl::readBytes(rig->resourceFile);
                if(bytes.size()!=pin->archiveBytes||pod_materials50ed_fixture::detail::sha256(bytes)!=pin->archiveSha256||
                   pod_materials50ed_fixture::detail::sha256(rig->phy)!=pin->phySha256||
                   collision->receipt().archiveBytes!=bytes.size())
                    throw std::runtime_error("actual roster archive/PHY identity differs");
                a::ResArchive48C archive;a::DecodeError48C d;std::vector<std::uint8_t> member;
                if(!a::decodeResource48C(bytes,archive,d))throw std::runtime_error("original pod archive framing rejected");
                for(unsigned role=0;role<rig->modelCount;++role){
                    if(!a::extractResMember48C(archive,rig->psgMembers[role],member,d)||member!=rig->psg[role]||
                       !a::extractResMember48C(archive,rig->mcfMembers[role],member,d)||member!=rig->mcf[role])
                        throw std::runtime_error("retained model source differs from authentic pod");
                }
                for(unsigned motion=0;motion<native_racer50HW::Rig::MotionCount;++motion){
                    if(rig->mot[motion].empty())continue;
                    if(!a::extractResMember48C(archive,rig->motMembers[motion],member,d)||member!=rig->mot[motion])
                        throw std::runtime_error("retained motion source differs from authentic pod");
                }
                const auto prefix=std::string("pods\\")+native_racer50HW::rig_detail::lower(rig->podId)+"\\";
                constexpr const char* names[]={"podboxm.col","chariotm.col","podboxf.col","chariotf.col"};
                for(unsigned i=0;i<4;++i)if(!a::extractResMember48C(archive,prefix+names[i],member,d)||
                    member!=collision->storage_->raw[i])throw std::runtime_error("retained collision bytes differ from original pod");
                if(!a::extractResMember48C(archive,prefix+native_racer50HW::rig_detail::lower(rig->podId)+".ai",
                    result->aiSources_[slot],d))throw std::runtime_error("actual selected pod AI source unavailable");
            }
            // Existing fixed corpus identities, retained here instead of trusting
            // arbitrary supplied arrays. Reading happens only when adopted.
            constexpr const char* names[]={"podLeftEngineDust.emt","podRightEngineDust.emt",
                "podLeftEngineSmoke.emt","podRightEngineSmoke.emt","podSparks.emt","podFire.emt"};
            constexpr const char* pins[]={
                "0e18ac3a94b8d4abaec38f9288a96b39b800ac58b17ba499ffc33f247995bb66",
                "9e5b26cf362e261239a0c6f7ff499f4dea285862286aa8dae65611529e2b5f7f",
                "03f835faf0501dd2a0761f5d1e1996d44bb634fa636242ff94341beaec7008c0",
                "2e89d1d1c6d44804dc25687d837699ab3207e8a800ee3756875e5760226957e2",
                "0a8e6c18e9f98992770f7046eea11a7541f286c387977aedcbbf277026d180d8",
                "b122aca0eb7779d48f5f90c93f2ab2d624ee38d2f824b1843ae62e790a5e44f8"};
            result->effects_=original_disc_effects50LB::load();
            for(unsigned i=0;i<6;++i){
                if(pod_materials50ed_fixture::detail::sha256(result->effects_[i])!=pins[i])
                    throw std::runtime_error("original shared effect identity differs");
            }
            // Retain the SAME arrays and object addresses used by the driver.
            // Construction is complete; no pointer is moved or reconstructed.
            return result;
        }catch(const std::exception& e){try{error=e.what();}catch(...){}return {};}
        catch(...){try{error="actual roster adoption failed";}catch(...){}return {};}
    }
};
} // namespace ra_cold_roster50LA
