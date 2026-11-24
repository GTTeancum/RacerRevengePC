#pragma once
// Read-only native-host scheduling adapter for the actual packed racer bank.
// No timing owner, host standings, driver dispatch, new clock or AI retagging.
#include "original_camera_session50kx.h"
#include "native_ai_session50iv_fixture.h"
#include "lap_drive50gj_fixture.h"
#include "../../src/game/original_time_stats50la.h"

namespace original_time_stats_adapter50LA {
class Owner final {
    using Camera=original_camera_session50KX::Session;
    using Bank=native_racer50HN::Bank;
    template<class T> using Read=Bank::ReadHandle<T>;
    const Camera* camera_=nullptr;
    const native_ai50IV::Session* manager_=nullptr;
    const std::uint16_t* settings1125C_=nullptr;
    std::uint64_t generation_=0;
    Read<float> timeCB0_,bestLapCB4_,timeCB8_,valueCBC_;
    Read<std::uint32_t> lapCC8_,vehicle40_;
    Read<std::uint8_t> flagsCD8_;
    bool bound_=false;
public:
    enum class Status : std::uint8_t {complete,alreadyBound,owner,phase,identity,read};
    struct Diagnostic {Status status=Status::owner;std::size_t slot=0;};
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    // Call once after actual SourceCamera::prepare and Bank::seal, outside
    // every Bank visit/visitMany/step callback. Owners outlive this adapter.
    // continuity and originalAiSettings1125C are the actual existing locals
    // passed by the native terrain root, never reconstructed HUD settings.
    bool bind(const Camera& camera,const native_ai50IV::Session& manager,
        const lap_drive50GJ::Continuity& continuity,const std::uint16_t& originalAiSettings1125C,
        Diagnostic& d)noexcept {
        d={};if(bound_){d.status=Status::alreadyBound;return false;}
        try {
            if(!camera.coldBound_||!camera.readsBound_||camera.closed_||camera.abandoned_||
               !camera.generation_||!camera.hudCameraBound(0)||
               &camera.actual_.all8Manager!=&manager.manager||
               &camera.actual_.racerTiming!=&continuity.timing||(!camera.actual_.bank.count()||camera.actual_.bank.count()>8))
                return false;
            if(camera.frameStage_!=Camera::FrameStage::idle&&camera.frameStage_!=Camera::FrameStage::published){
                d.status=Status::phase;return false;
            }
            if(!camera.identitiesCurrent()){d.status=Status::identity;return false;}
            auto& bank=camera.actual_.bank;
            // bindRead verifies each scalar lies inside an actual sealed
            // Bank owner span and refuses an active/read-blocked lease.
            const auto cb0=bank.bindRead(camera.actual_.racerTiming.timeCB0);
            const auto cb4=bank.bindRead(camera.actual_.racerTiming.bestLapCB4);
            const auto cb8=bank.bindRead(camera.actual_.racerTiming.timeCB8);
            const auto cbc=bank.bindRead(camera.actual_.racerTiming.valueCBC);
            const auto cc8=bank.bindRead(continuity.state.vehicleCC8);
            const auto cd8=bank.bindRead(camera.actual_.arena.fields.impactFlagsCD8);
            const auto state=bank.bindRead(camera.actual_.arena.bodies.main.vehicleState40);
            timeCB0_=cb0;bestLapCB4_=cb4;timeCB8_=cb8;valueCBC_=cbc;
            lapCC8_=cc8;flagsCD8_=cd8;vehicle40_=state;
            camera_=&camera;manager_=&manager;settings1125C_=&originalAiSettings1125C;
            generation_=camera.generation_;bound_=true;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
    // Synchronous serial snapshot only, after the actual manager gap writes
    // and a completed camera publication, outside all Bank leases. Output
    // is unchanged on rejection. Borrowed viewport/preferences live only as
    // long as the actual Session/configuration owners; consume before another
    // world/camera update or teardown, not from an asynchronous renderer.
    bool capture(rr::game::OriginalTimeStatsNativeFrame50LA& output,Diagnostic& d,unsigned view=0)const noexcept {
        d={};if(!bound_||!camera_||!manager_||!settings1125C_)return false;
        try {
            const auto& camera=*camera_;const auto& manager=*manager_;const auto& bank=camera.actual_.bank;
            const auto* viewport=camera.hudViewport(view);
            if(camera.closed_||camera.abandoned_||!camera.coldBound_||!camera.readsBound_||
               camera.generation_!=generation_||camera.frameStage_!=Camera::FrameStage::published||
               !camera.publicationCount_||!camera.completedFrame_||
               !viewport||!viewport->prepared||!viewport->matricesReady||
               viewport->sessionGeneration!=generation_){d.status=Status::phase;return false;}
            if(!camera.hudCameraBound(view)||(!bank.count()||bank.count()>8)||
               &camera.actual_.all8Manager!=&manager.manager||!manager.managerBound||!manager.rankBound||
               !manager.retired40.bound||manager.retired40.activeCount50>bank.count()||
               (camera.raceFlags1B0_&3u)!=(camera.actual_.humanCount34==2u?2u:1u)||!camera.identitiesCurrent()){
                d.status=Status::identity;return false;
            }
            const auto publication=camera.publicationCount_;
            rr::game::OriginalTimeStatsNativeFrame50LA next;
            next.sessionGeneration=generation_;next.cameraPublication=publication;
            next.viewport=viewport;next.preferences=&camera.actual_.preferences;
            next.count=static_cast<std::uint32_t>(bank.count());
            next.selectedSlot=camera.identity_[view].managerIndex;
            if(next.selectedSlot>=next.count){d.status=Status::identity;return false;}
            // The represented Session has exactly its real camera0, proven
            // by the binding/low2 check above; no human-count inference.
            next.viewIndex=view;next.subtype11258=camera.actual_.launchKind11258;
            next.activeCount50=manager.retired40.activeCount50;
            next.raceMode8C=camera.raceMode8C_;
            next.raceFlags1B0=camera.raceFlags1B0_;next.raceEntityFlags29=camera.raceEntityFlags29_;
            next.uiFlags1E4=camera.uiFlags1E4_;next.managerFlags58=manager.manager.flags58;
            next.lapCount1125C=static_cast<std::uint8_t>(*settings1125C_);
            for(std::size_t slot=0;slot<next.count;++slot){
                d.slot=slot;auto& record=next.records[slot];
                record.timeCB0=bank.read(slot,timeCB0_);record.bestLapCB4=bank.read(slot,bestLapCB4_);
                record.timeCB8=bank.read(slot,timeCB8_);record.valueCBC=bank.read(slot,valueCBC_);
                record.lapCC8=bank.read(slot,lapCC8_);record.flagsCD8=bank.read(slot,flagsCD8_);
                record.vehicleState40=bank.read(slot,vehicle40_);
                record.placeCCC=manager.rank[slot].placeCCC;
            }
            if(camera.frameStage_!=Camera::FrameStage::published||camera.generation_!=generation_||
               camera.publicationCount_!=publication){d.status=Status::phase;return false;}
            output=next;d.status=Status::complete;return true;
        }catch(...){d.status=Status::read;return false;}
    }
};
} // namespace original_time_stats_adapter50LA
