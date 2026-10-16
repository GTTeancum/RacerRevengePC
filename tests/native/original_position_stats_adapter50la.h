#pragma once
// Actual ALL8 view0 numeric observation. No scheduling or drawing is implied.
namespace original_main_entry50KG { struct Application; }
#include "original_camera_session50kx.h"
#include "native_ai_session50iv_fixture.h"
#include "../../src/game/original_position_stats_frame50la.h"
#include <array>
#include <cmath>
namespace original_position_stats_adapter50LA {
class Owner final {
    friend struct original_main_entry50KG::Application;
    using Camera=original_camera_session50KX::Session;
    // Only Application supplies these identities, taken from its real display
    // and canonical owners. Equality of camera/display counters is not proof.
    static bool belongsToApplication(const Camera& camera,
        const rr::game::OriginalApplicationEntities50LA* entities,
        const rr::assets::OriginalStandardMaterialTable50LA* materials,
        const std::string* directory,const rr::platform::VehicleWindowGl50DS* renderer,
        const rr::game::OriginalRendererDispatchFields* dispatch)noexcept;

    using Manager=native_ai50IV::Session;
    using Bank=native_racer50HN::Bank;
    const Camera* camera_=nullptr;
    const Manager* manager_=nullptr;
    std::uint64_t generation_=0;
    Bank::ReadHandle<std::uint32_t> state40_,countCD0_;
    bool identityCurrent()const {
        const auto& c=*camera_;const auto& m=*manager_;
        return !c.closed_&&!c.abandoned_&&c.coldBound_&&c.readsBound_&&
            c.generation_==generation_&&c.actual_.humanCount34==0&&c.actual_.bank.count()==8&&
            &c.actual_.all8Manager==&m.manager&&m.managerBound&&m.rankBound&&
            m.retired40.bound&&m.retired40.activeCount50<=8&&
            c.controller_.bound&&c.controller_.binding.sessionGeneration==generation_&&
            c.controller_.binding.bootstrap==rr::game::CameraBootstrap::native_all_ai&&
            c.controller_.binding.racerClass==rr::game::CameraRacerClass::ai&&
            c.controller_.binding.managerIndex==0&&c.identity_[0].managerIndex==0&&
            c.controller_.binding.racerKey==Camera::key(c.identity_[0].racerTag)&&
            c.controller_.binding.mainBodyKey==Camera::key(c.identity_[0].mainBodyTag)&&
            c.controller_.binding.sceneKey==Camera::key(c.scene_)&&
            (c.raceFlags1B0_&3u)==1u&&c.identitiesCurrent();
    }
public:
    enum class Status:std::uint8_t {complete,owner,alreadyBound,phase,identity,read};
    struct Diagnostic {Status status=Status::owner;};
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // Actual Camera and Manager must outlive this nonmoving borrow. Bind outside
    // every Bank lease. No caller provides a row, racer override or ready flag.
    bool bind(const Camera& camera,const Manager& manager,Diagnostic& d)noexcept {
        d={};if(camera_){d.status=Status::alreadyBound;return false;}
        try {
            if(!camera.generation_||camera.closed_||camera.abandoned_||
               !camera.coldBound_||!camera.readsBound_||camera.actual_.humanCount34||
               camera.actual_.bank.count()!=8||&camera.actual_.all8Manager!=&manager.manager||
               !camera.identitiesCurrent())return false;
            if(camera.frameStage_!=Camera::FrameStage::idle&&camera.frameStage_!=Camera::FrameStage::published){
                d.status=Status::phase;return false;}
            const auto state=camera.actual_.bank.bindRead(camera.actual_.arena.bodies.main.vehicleState40);
            const auto count=camera.actual_.bank.bindRead(camera.actual_.racerTiming.countCD0);
            state40_=state;countCD0_=count;generation_=camera.generation_;
            camera_=&camera;manager_=&manager;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
    // Source1A73DC/1A73F0 passes the SAME manager-loop index to1750A0, which
    // stores its low nibble at175144. The retained native binding mirrors that
    // exact view0 call argument. This is not a followed-racer override; no other
    // camera bootstrap, nonzero view, rebinding or arbitrary index is admitted.
    // Consume synchronously after real rank/camera publication and before the
    // next world tick; publication is an observation, not an async lifetime pin.
    bool capture(rr::game::OriginalPositionStatsNumbers50LA& output,Diagnostic& d)const noexcept {
        d={};if(!camera_||!manager_)return false;
        try {
            const auto& c=*camera_;const auto& m=*manager_;
            if(c.frameStage_!=Camera::FrameStage::published||!c.publicationCount_||
               !c.completedFrame_||!c.viewport_.prepared||!c.viewport_.matricesReady||
               c.viewport_.sessionGeneration!=generation_){d.status=Status::phase;return false;}
            if(!identityCurrent()){d.status=Status::identity;return false;}
            const auto index=c.controller_.binding.managerIndex;
            // Actual native manager3C row identity is manager order, not ranking
            // order. Its original rank pass publishes rank[index].placeCCC.
            if(!m.observed[index].bound||m.observed[index].managerIndex!=index||
               !m.fields[index].bound||m.fields[index].placeCCC!=m.rank[index].placeCCC){
                d.status=Status::identity;return false;}
            rr::game::OriginalPositionStatsNumbers50LA next;
            next.sessionGeneration=generation_;next.cameraPublication=c.publicationCount_;
            next.viewIndex=0;next.cameraManagerIndex=index;next.managerCount30=8;
            next.placeCCC=m.rank[index].placeCCC;next.activeCount50=m.retired40.activeCount50;
            next.viewState40=c.actual_.bank.read(0,state40_);
            next.viewCountCD0=c.actual_.bank.read(0,countCD0_);
            next.preference10F9C=c.actual_.preferences.slots[0].field10F9C;
            next.subtype11258=c.actual_.launchKind11258;next.raceMode8C=c.raceMode8C_;
            next.raceFlags1B0=c.raceFlags1B0_;next.raceEntityFlags29=c.raceEntityFlags29_;
            next.uiFlags1E4=c.uiFlags1E4_;
            if(c.frameStage_!=Camera::FrameStage::published||c.publicationCount_!=next.cameraPublication||
               !identityCurrent()){d.status=Status::phase;return false;}
            output=next;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
private:
    struct RadarObservation {
        const rr::game::TrackNavigation50EU* map=nullptr;
        std::array<float,3> center{};
        float headingE4=0;
        std::array<std::array<float,3>,8> racerCenters{};
        std::array<std::uint32_t,8> racerStates{};
        std::uint64_t publication=0;
    };
    //1924C0 resolves managerB0[Application268], body340->scene288 and E4.
    //Use that same current view0 racer. The bank visit performs the original
    //lazy world-cache refresh; its canonical cache effects are not rolled back
    //if a later numeric admission fails. No local/world or camera-pose shortcut.
    bool captureRadar1924C0(RadarObservation& output,Diagnostic& d)const noexcept {
        rr::game::OriginalPositionStatsNumbers50LA observation;
        if(!capture(observation,d))return false;
        const auto& c=*camera_;
        if(!c.actual_.actualRadarMapB8||!c.actual_.actualRadarMapB8->gatesPrepared){
            d.status=Status::owner;return false;}
        try {
            RadarObservation next;next.map=c.actual_.actualRadarMapB8;
            next.publication=observation.cameraPublication;
            for(std::size_t slot=0;slot<8;++slot){
            next.racerStates[slot]=c.actual_.bank.read(slot,c.reads_.state40);
            if(slot!=observation.viewIndex&&next.racerStates[slot]!=0)continue;
            const bool read=c.actual_.bank.visit(slot,[&]()noexcept {
                auto& arena=c.actual_.arena;
                const auto index=c.actual_.bindings.animation.bodyScene288;
                const auto node=static_cast<std::int32_t>(index);
                if(node<0||static_cast<std::uint32_t>(node)>=arena.sceneNodeCount)return false;
                if(!rr::game::refreshSceneWorld49P({arena.nodes,arena.sceneNodeCount},index))return false;
                const auto& world=arena.nodes[static_cast<std::size_t>(node)].world100;
                for(std::size_t axis=0;axis<3;++axis)next.racerCenters[slot][axis]=world.v[12+axis];
                if(slot==observation.viewIndex){next.center=next.racerCenters[slot];
                    next.headingE4=arena.bodies.main.cachedAnglesE0.v[1];}
                for(float value:next.racerCenters[slot])if(!std::isfinite(value))return false;
                return std::isfinite(next.headingE4);
            });
            if(!read){d.status=Status::read;return false;}
            }
            if(c.frameStage_!=Camera::FrameStage::published||c.publicationCount_!=next.publication||
               !identityCurrent()){d.status=Status::phase;return false;}
            output=next;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
};
inline bool Owner::belongsToApplication(const Camera& camera,
    const rr::game::OriginalApplicationEntities50LA* entities,
    const rr::assets::OriginalStandardMaterialTable50LA* materials,
    const std::string* directory,const rr::platform::VehicleWindowGl50DS* renderer,
    const rr::game::OriginalRendererDispatchFields* dispatch)noexcept {
    return entities&&materials&&directory&&renderer&&dispatch&&camera.displayInputs_&&
        camera.actual_.applicationEntities.get()==entities&&
        camera.actual_.applicationMaterials.get()==materials&&
        camera.actual_.actualMaterialDirectory36B970==directory&&
        &camera.displayInputs_->renderer==renderer&&
        &camera.displayInputs_->rendererDispatch==dispatch;
}
}
