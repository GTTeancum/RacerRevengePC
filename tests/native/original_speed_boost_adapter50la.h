#pragma once
#include "original_camera_session50kx.h"
#include "../../src/game/original_speed_boost_frame50la.h"
namespace original_speed_boost_adapter50LA {
class Owner final {
    using Camera=original_camera_session50KX::Session;
    using Bank=native_racer50HN::Bank;
    template<class T>using Read=Bank::ReadHandle<T>;
    const Camera* camera_=nullptr;
    std::uint64_t generation_=0;
    Read<float> speed34_,heat7C_;
    Read<std::uint32_t> boost64_,vehicle40_;
public:
    enum class Status:std::uint8_t {complete,owner,phase,identity,read};
    struct Diagnostic {Status status=Status::owner;};
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool bind(const Camera& camera,Diagnostic& d)noexcept {
        d={};if(camera_)return false;
        try {
            if(!camera.coldBound_||!camera.readsBound_||camera.closed_||camera.abandoned_||
               !camera.generation_||(!camera.actual_.bank.count()||camera.actual_.bank.count()>8)||!camera.hudCameraBound(0))
                return false;
            if((camera.frameStage_!=Camera::FrameStage::idle&&camera.frameStage_!=Camera::FrameStage::published)||
               !camera.identitiesCurrent()){d.status=Status::phase;return false;}
            const auto& bank=camera.actual_.bank;
            speed34_=bank.bindRead(camera.actual_.arena.bodies.main.cachedSpeed34);
            heat7C_=bank.bindRead(camera.actual_.boost.heat7C);
            boost64_=bank.bindRead(camera.actual_.boost.vehicle64);
            vehicle40_=bank.bindRead(camera.actual_.arena.bodies.main.vehicleState40);
            generation_=camera.generation_;camera_=&camera;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
    // Use after the actual world/manager/camera publication, outside Bank
    // leases. Consume synchronously before another world tick or teardown.
    bool capture(rr::game::OriginalSpeedBoostNativeFrame50LA& output,Diagnostic& d,unsigned view=0)const noexcept {
        d={};if(!camera_)return false;
        try {
            const auto& c=*camera_;const auto& bank=c.actual_.bank;
            if(!c.hudCameraBound(view)||!c.identitiesCurrent()){d.status=Status::identity;return false;}
            const auto& viewport=view?c.secondViewport_:c.viewport_;
            if(c.closed_||c.abandoned_||!c.coldBound_||!c.readsBound_||!c.displayInputs_||
               c.generation_!=generation_||c.frameStage_!=Camera::FrameStage::published||
               !c.publicationCount_||!c.completedFrame_||!viewport.prepared||!viewport.matricesReady||
               viewport.sessionGeneration!=generation_){d.status=Status::phase;return false;}
            rr::game::OriginalSpeedBoostNativeFrame50LA next;
            next.sessionGeneration=generation_;next.cameraPublication=c.publicationCount_;
            next.viewport=&viewport;next.preferences=&c.actual_.preferences;
            next.viewIndex=view;next.selectedSlot=c.identity_[view].managerIndex;
            if(next.selectedSlot>=bank.count()){d.status=Status::identity;return false;}
            next.selectedBodySpeed34=bank.read(next.selectedSlot,speed34_);
            next.selectedHeat7C=bank.read(next.selectedSlot,heat7C_);
            next.selectedBoost64=bank.read(next.selectedSlot,boost64_);
            // Original draw leaf gates manager slot=view independently of
            // the selected racer used by predraw/heat. Preserve both identities.
            next.viewVehicleState40=bank.read(next.viewIndex,vehicle40_);
            next.raceBlinkD8=c.raceHudBlink_.d8[next.viewIndex];
            next.raceMode8C=c.raceMode8C_;next.raceFlags1B0=c.raceFlags1B0_;
            next.raceEntityFlags29=c.raceEntityFlags29_;next.uiFlags1E4=c.uiFlags1E4_;
            if(c.frameStage_!=Camera::FrameStage::published||c.generation_!=generation_||
               c.publicationCount_!=next.cameraPublication){d.status=Status::phase;return false;}
            output=next;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
};
}
