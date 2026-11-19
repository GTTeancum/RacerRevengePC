#pragma once
#include "native_racer_rig50hw_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "original_racer_manager_entity50la.h"
#include "../../src/game/dlc_rider_entries50dl.h"
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>
#include "../../src/game/original_application_entities50la.h"
#include <array>
#include <memory>
#include <exception>

namespace original_subtype3_record {
namespace g=rr::game;
// Distinct manager40 lifetime for a fresh, no-playback native time-trial.
//1AD70C always calls1AE810: this is NOT bank[0], an absent-record sentinel,
//or a boolean standing in for an unconstructed actor. The independently
//loaded source Rig and its body/model graph are owned here until teardown.
//This is the bounded native no-record actor representation, NOT complete
//1A02C0/1A0570 Player construction, collision-exclusion or replay parity.
//Persisted replay loading/recording is deliberately a separate future owner.
class Owner final {
    std::uint64_t generation_=0;
    std::uint32_t rider_=0;
    std::shared_ptr<g::OriginalApplicationEntities50LA> application_;
    g::OriginalWreckComponent50LA entity_;
    original_racer_manager_entity::Owner* manager_=nullptr;
    bool enrolled_=false;
    std::unique_ptr<native_racer50HW::Rig> rig_;
    std::unique_ptr<g::VehicleFrameState49W> body_;
    g::VehicleFrameBindings49W bindings_{};
    g::ChariotIkPair49T hands_{};
    bool published_=false,closed_=false;
    static std::uint64_t key(const void* p)noexcept{return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));}
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    ~Owner();
    // originalRankZeroPose must come from this course's2A2880 rank0 and
    //its existing cold-placement owner. Never pass the player's rank4 pose.
    // The only live-arena borrow is the process-shared animation lane, not
    //player nodes, body fields, racer identity, collision or input state.
    bool constructFreshNoRecord(std::uint64_t generation,std::uint16_t packed1125C,
        const std::filesystem::path& originalPodArchive,const g::BodyMatrix48G& originalRankZeroPose,
        std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        g::VehicleFrameState49W& actualSharedArena,
        original_racer_manager_entity::Owner& actualRacerManager,std::string& error) {
        if(generation_||closed_||!generation||!application||!application->application10()||
           application->attachmentLock423F58()||actualRacerManager.application()!=application||
           actualRacerManager.generation()!=generation) {error="Time-trial record actor requires a fresh real application owner";return false;}
        const auto rider=std::uint32_t((packed1125C>>11u)&31u);
        // The packed rider is the profile's stock rider; a latched free-play
        // add-on races in its place (PORT EXTENSION), so its row is checked.
        const auto* identity=original_player_identity50IK::find(rr::game::dlc50DL::racedRider(rider));
        if(!identity){error="Time-trial record actor source rider is invalid";return false;}
        //Reject numeric-domain failures before allocating the persistent actor.
        for(const auto value:originalRankZeroPose.v)if(!std::isfinite(value)){
            error="Time-trial record actor rank-zero pose is nonfinite";return false;}
        try {
            application_=std::move(application);generation_=generation;rider_=rider;manager_=&actualRacerManager;
            const auto created=g::initializeOriginalWreckComponent50LA(entity_,
                g::OriginalWreckComponentKind50LA::entity141300,generation_,key(this),true,
                application_->objectCount423F60());
            if(created.status!=g::OriginalWreckComponentStatus50LA::complete){
                error="Time-trial record actor Entity constructor failed";return false;}
            if(!manager_->enrollRecord(entity_)){error="Time-trial record actor authority enrollment failed";return false;}
            enrolled_=true;
            rig_=std::make_unique<native_racer50HW::Rig>(".",originalPodArchive);
            if(!rig_->directOriginal||rig_->podId!=identity->podId){error="Time-trial record actor archive does not match packed1125D rider";return false;}
            body_=std::make_unique<g::VehicleFrameState49W>();
            using I=g::SceneNodeIndex49P;
            //Four independent native cold SceneObjects. Rig::install builds
            //all actual authored model hierarchies onto these owned roots.
            bindings_.lifecycle.main190=static_cast<I>(0);
            bindings_.lifecycle.physical19C=static_cast<I>(1);
            bindings_.lifecycle.container194=static_cast<I>(2);
            bindings_.lifecycle.hidden198=static_cast<I>(3);
            for(unsigned i=0;i<4;++i){auto& n=body_->nodes[i];
                n.localC0=n.world100=i<2?originalRankZeroPose:runtime_startup50fl::identity();
                n.parent180=n.firstChild184=n.nextSibling188=I::none;n.sceneFlags190=0x80u;}
            body_->sceneNodeCount=bindings_.sceneNodeCount=4;
            body_->animationShared.motion=actualSharedArena.animationShared.motion;
            rig_->install(*body_,bindings_,hands_,nullptr,nullptr,nullptr,nullptr,nullptr,true);
            //Preserve the exact shared motion chronology consumed by this
            //new constructor without copying any mutable player body state.
            actualSharedArena.animationShared.motion=body_->animationShared.motion;
            const g::SceneGraph49P graph{body_->nodes,body_->sceneNodeCount};
            if(!g::refreshSceneWorld49P(graph,bindings_.animation.bodyScene288)){
                error="Time-trial record actor body scene refresh failed";return false;}
            //Actual1AEA1C attaches this distinct actor to the genuine1AD440
            //racer manager, mask2, before disable and manager40 publication.
            if(!manager_->attachRecord(entity_)){error="Time-trial record actor manager attachment failed";return false;}
            g::OriginalWreckComponent50LA* objects[]={&entity_};
            const g::OriginalWreckComponentDomain50LA domain{generation_,objects,1};
            //1AEA24 invokes actual Player slot18=141520, before manager40
            //publication. The no-playback native owner never activates it.
            const auto hidden=g::setOriginalWreckBaseActivity50LA(domain,entity_,false);
            if(hidden.status!=g::OriginalWreckComponentStatus50LA::complete){
                error="Time-trial record actor source disable failed";return false;}
            if(!manager_->publishRecord(entity_)){error="Time-trial record actor manager40 publication failed";return false;}
            published_=true;return true;
        }catch(const std::exception& e){error=e.what();return false;}
    }
    bool radarObservation(std::uint64_t generation,std::uint8_t& flags29,
        std::array<float,3>& worldPosition)const noexcept {
        if(!published_||closed_||generation!=generation_||!application_||!rig_||!body_||
           entity_.sessionGeneration!=generation_||entity_.objectKey!=key(this)||entity_.reference00!=1||
           !manager_||!manager_->ownsRecord(entity_,generation_)||
           manager_->recordEntity()!=&entity_||entity_.firstChild18)return false;
        flags29=entity_.flags29;
        //192A84 branches before reading body340/scene288 for a hidden actor.
        //Do not fabricate a position, issue a world refresh, or borrow player0.
        if(!(flags29&1u))return true;
        (void)worldPosition;
        //No activation API exists until a real retained replay producer owns
        //the actor's actual world updates. A corrupt visible bit is not proof.
        return false;
    }
    bool close()noexcept {
        if(closed_)return true;
        if(entity_.kind!=g::OriginalWreckComponentKind50LA::unbound){
            if(!application_||application_->attachmentLock423F58()||entity_.reference00!=1||
               entity_.firstChild18||
               !application_->objectCount423F60())return false;
            if(enrolled_){
                if(!manager_||!manager_->retireRecord(entity_))return false;
                enrolled_=false;
            }
            if(entity_.parent1C||entity_.previous10||entity_.next14)return false;
            --entity_.reference00;--application_->objectCount423F60();
        }
        published_=false;closed_=true;body_.reset();rig_.reset();manager_=nullptr;application_.reset();return true;
    }
};
}
