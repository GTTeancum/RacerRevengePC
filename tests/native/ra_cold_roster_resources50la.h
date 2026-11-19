#pragma once
#include "ra_cold_roster_storage50la.h"
#include "ra_cold_wreck_backing50la.h"
namespace original_results_roster50LA { class Read; }
#include "native_racer_rig50hw_fixture.h"
#include "native_racer_collision50hw_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "original_disc_effects50lb.h"
#include "../../src/assets/original_ui_archive50kg.h"
#include <array>
#include <memory>
#include <string>

namespace ra_cold_aggregate50LA { class Owner; }
namespace ra_cold_roster50LA {
class Resources final {
    friend class FreshExecutionArena;
    friend class ::original_results_roster50LA::Read;
    friend class Operation;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    friend class ra_cold_aggregate50LA::Owner;
    std::shared_ptr<SharedOwners> owners_;
    const std::array<std::unique_ptr<native_racer50HW::Rig>,8>& rigs_;
    const std::array<std::unique_ptr<native_racer_collision50HW::Owner>,8>& collisions_;
    std::array<std::vector<std::uint8_t>,8> aiSources_;
    // SAME authenticated full archives, retained without a second reopen.
    std::array<std::vector<std::uint8_t>,8> rawRes_;
    // Input custody only, not12CEA0/1ADF80 issuance. The source archive catalog
    // currently admits gameplay POD01 only. Other human IDs stay absent.
    std::shared_ptr<const a::OriginalUiArchive50KG> sourceHumanPod01Archive_;
    std::shared_ptr<WreckBacking> wreckBacking_;
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
            result->wreckBacking_=std::make_shared<WreckBacking>();
            result->humanCount_=humanCount;result->racerCount_=racerCount;
            const auto& rigs=result->rigs_;const auto& collisions=result->collisions_;
            for(std::size_t slot=0;slot<racerCount;++slot){
                const auto* rig=rigs[slot].get();const auto* collision=collisions[slot].get();
                if(!rig||!collision||!collision->prepared()||(rig->modelCount!=3u&&rig->modelCount!=5u))
                    throw std::runtime_error("actual original roster owners or bounded model roles absent");
                for(std::size_t old=0;old<slot;++old)if(rigs[old]->podId==rig->podId)
                    throw std::runtime_error("duplicate actual roster identity");
                const auto* selected=g::original_racer_selection50IJ::findAnyByPodId(rig->podId);
                const auto* pin=selected?original_player_identity50IK::find(selected->racer60):nullptr;
                if(!pin)throw std::runtime_error("original roster source pin absent");
                auto bytes=runtime_startup50fl::readBytes(rig->resourceFile);
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
                if(slot==0&&humanCount==1u&&pin->racer60==1u){
                    // Authenticate from the SAME already-read, hash-pinned
                    // input before rawRes publication. Never reopen a file or
                    // promote native mount/Player readiness into source IO.
                    auto sourceInput=std::make_shared<a::OriginalUiArchive50KG>();
                    a::OriginalUiArchiveDiagnostic50KG sourceInputDiagnostic;
                    if(!sourceInput->load(a::OriginalUiArchiveId50KG::sourcePod01,bytes,sourceInputDiagnostic))
                        throw std::runtime_error("actual human POD01 encoded source custody rejected");
                    // Existing const-input archive API owns an encoded copy
                    // and authenticated decoded/member/block tables. Keep the
                    // original rawRes allocation for existing wreck consumers.
                    result->sourceHumanPod01Archive_=std::move(sourceInput);
                }
                result->rawRes_[slot]=std::move(bytes);
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
