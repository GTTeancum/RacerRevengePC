#pragma once
#include "original_pause_entity50la.h"
#include "original_camera_session50kx.h"
#include "ra_cold_sources50la.h"
#include "../../src/game/original_pause_storage50la.h"
#include "../../src/game/original_pause_layout50la.h"
#include "../../src/game/original_pause_native_draw50la.h"
#include "../../src/game/original_pause_viewport_scope50la.h"

namespace original_pause_observation50LA {
// Serial actual-owner observation and bounded navigation. Application must outlive this borrow; caller unbinds
// before Application destruction. All concrete/profile ownership is weak.
// This facet cannot supply arbitrary frames, selection, controller or settings.
class Owner final {
    using Camera=original_camera_session50KX::Session;
    using Track=ra_cold_sources50LA::DecodedSources;
    std::weak_ptr<Camera> camera_;
    std::weak_ptr<const Track> track_;
    std::uint64_t cameraGeneration_=0;
    const original_main_entry50KG::Application* application_=nullptr;
    std::weak_ptr<rr::game::OriginalPauseStorage50LA> pause_;
    std::weak_ptr<rr::game::ControlPresets49D> controls_;
    std::weak_ptr<rr::game::OriginalUiGameSettingsProfile50KG> settings_;
public:
    using Diagnostic=rr::game::OriginalPauseStorage50LA::Diagnostic;
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool bind(const original_main_entry50KG::Application& application,
        const std::shared_ptr<rr::game::OriginalPauseStorage50LA>& pause)noexcept {
        if(application_||!pause||pause->stage_!=rr::game::OriginalPauseStorage50LA::Stage::derivedResources||
           !pause->context_||pause->context_->materialApplication()!=application.materialApplication||
           !application.controlsProfile||!application.gameSettingsProfile||
           !rr::game::validOriginalUiGameSettingsProfile50KG(*application.gameSettingsProfile))return false;
        application_=&application;pause_=pause;controls_=application.controlsProfile;
        settings_=application.gameSettingsProfile;return true;
    }
    bool captureSettings(Diagnostic& d)const noexcept {
        d={};if(!application_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();const auto settings=settings_.lock();
        if(!pause||!controls||!settings||!pause->context_||
           pause->context_->materialApplication()!=application_->materialApplication||
           application_->controlsProfile!=controls||
           application_->gameSettingsProfile!=settings)return false;
        return pause->refreshSettings1BB590(*controls,*settings,d);
    }
    // RA-only current native source join. The shared source is the aggregate's
    // actual loaded-track borrow, not SessionAssets or a host track label.
    bool bindRace(const std::shared_ptr<Camera>& camera,
        const std::shared_ptr<const Track>& track)noexcept {
        if(cameraGeneration_||!application_||!camera||!track)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        if(!pause||!controls||!raceIdentity(*camera,*track,*pause,*controls))return false;
        camera_=camera;track_=track;cameraGeneration_=camera->generation_;return true;
    }
    bool captureTitle(Diagnostic& d)const noexcept {
        d={};if(!application_||!cameraGeneration_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto camera=camera_.lock();const auto track=track_.lock();
        if(!pause||!controls||!camera||!track||camera->generation_!=cameraGeneration_||
           application_->controlsProfile!=controls||!raceIdentity(*camera,*track,*pause,*controls))return false;
        // Original descriptor17EA64/6C: trackID7's1127C slot is localization108.
        // The camera is proven to use this SAME RA map and selected metadata.
        return pause->refreshTitle1BB010(camera->raceFlags1B0_,108,d);
    }
    // Inactive CPU preparation only. Caller must first obtain actual parent
    // and last-camera admission; this does not dispatch1B9220 or a GPU draw.
    bool captureLayout(rr::game::OriginalPauseLayout50LA::Commands& out,
        Diagnostic& observation,rr::game::OriginalPauseLayout50LA::Diagnostic& layout)const noexcept {
        out={};observation={};layout={};
        if(!application_||!cameraGeneration_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto settings=settings_.lock();const auto camera=camera_.lock();const auto track=track_.lock();
        if(!pause||!controls||!settings||!camera||!track||camera->generation_!=cameraGeneration_||
           application_->controlsProfile!=controls||application_->gameSettingsProfile!=settings||
           !raceIdentity(*camera,*track,*pause,*controls))return false;
        // Source gating precedes title/settings observation. Existing standalone
        // captureTitle/captureSettings remain observations, not callback calls.
        if(!rr::game::OriginalPauseLayout50LA::visible(*pause))
            return rr::game::OriginalPauseLayout50LA::prepare1BB010(*pause,out,layout);
        if(!pause->refreshTitle1BB010(camera->raceFlags1B0_,108,observation)||
           !pause->refreshSettings1BB590(*controls,*settings,observation))return false;
        return rr::game::OriginalPauseLayout50LA::prepare1BB010(*pause,out,layout);
    }
    // Inactive outer one-view render preparation. Actual shared profile1125C
    // determines count; the ALL8 manager human count is unrelated.
    bool captureOuterLayout(rr::game::OriginalPauseLayout50LA::Commands& out,
        Diagnostic& observation,rr::game::OriginalPauseLayout50LA::Diagnostic& layout)const noexcept {
        out={};observation={};layout={};
        if(!application_||!cameraGeneration_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto camera=camera_.lock();const auto track=track_.lock();
        const auto profile=application_->profile;
        if(!pause||!controls||!camera||!track||!profile||camera->generation_!=cameraGeneration_||
           application_->controlsProfile!=controls||!raceIdentity(*camera,*track,*pause,*controls))return false;
        const auto count=(profile->playerBits1125C>>5)&3u;
        if(count>=2||pause->mode1F0_!=0){layout.sourcePC=count>=2?0x1B9DB0:0x1B9C24;return false;}
        rr::game::OriginalPauseLayout50LA::Commands derived;
        if(count&&!captureLayout(derived,observation,layout))return false;
        return rr::game::OriginalPauseLayout50LA::prepareOuterTail1B9D84(
            *pause,derived,profile->playerBits1125C,out,layout);
    }

    // Native draw-tail preparation under the actual retained camera's115A60
    // scope. This is not last-camera admission, a timer tick or full1B9220.
    bool captureOuterNativeFrame(rr::game::OriginalPauseNativeDraw50LA& native,
        Diagnostic& observation,rr::game::OriginalPauseLayout50LA::Diagnostic& layout,
        rr::game::OriginalPauseNativeResources50LA::Diagnostic& draw)const noexcept {
        observation={};layout={};draw={};
        if(native.frame_)native.frame_->prepared=false;
        if(!application_||!cameraGeneration_)return false;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto camera=camera_.lock();const auto track=track_.lock();const auto profile=application_->profile;
        if(!pause||!controls||!camera||!track||!profile||native.owner_!=pause||!native.current()||
           camera->generation_!=cameraGeneration_||!camera->publicationCount_||
           application_->controlsProfile!=controls||!raceIdentity(*camera,*track,*pause,*controls))return false;
        if(pause->mode1F0_!=0){layout.sourcePC=0x1B9C24;return false;}
        const auto count=(profile->playerBits1125C>>5)&3u;
        rr::game::OriginalPauseViewportScope50LA viewport(camera->viewport_,count>=2);
        if(!viewport.valid_){layout.sourcePC=0x115A60;return false;}
        rr::game::OriginalPauseLayout50LA::Commands derived,outer;
        if(count&&!captureLayout(derived,observation,layout))return false;
        if(!rr::game::OriginalPauseLayout50LA::prepareOuterTail1B9D84(
            *pause,derived,profile->playerBits1125C,outer,layout))return false;
        // Private concrete join can consume a multiview-tagged command list;
        // public prepareOuter cannot bypass this real viewport mutation/restore.
        return native.prepareCommands(outer,camera->viewport_,camera->publicationCount_,draw);
    }

    struct NavigationResult {
        enum class Status : std::uint8_t {complete,owner,parent,actionUnavailable,audioUnavailable,childRoute};
        Status status=Status::owner;
        std::uint32_t sourcePC=0,originalReturn=0;
        bool selectionChanged=false;
        original_camera_cycle_audio50KX::Owner::Result audio=
            original_camera_cycle_audio50KX::Owner::Result::unprepared;
    };
    // Bounded actual1BD7C0 up/down and ignored-event routes only. Parent
    // admission remains mandatory; there is no host selection setter. Confirm
    // and left/right settings actions reject before effects until joined.
    NavigationResult pressNavigation1BD7C0(rr::game::InputEvent48Z& event,
        rr::game::DeviceButtonRecord48Y& record)const noexcept {
        using Result=NavigationResult;using Status=Result::Status;
        Result result;
        if(!application_||!cameraGeneration_)return result;
        const auto pause=pause_.lock();const auto controls=controls_.lock();
        const auto camera=camera_.lock();const auto track=track_.lock();
        if(!pause||!controls||!camera||!track||camera->generation_!=cameraGeneration_||
           application_->controlsProfile!=controls||!raceIdentity(*camera,*track,*pause,*controls)||
           !pause->entity_||!pause->entity_->initialized||pause->entity_->releasing||
           pause->entity_->application!=application_->entities()||
           pause->entity_->component.sessionGeneration!=cameraGeneration_)return result;
        const auto& component=pause->entity_->component;
        // Cannot activate the current detached cold prefix. Only an actual
        // future race sibling-chain attachment can supply the exact App10 parent,
        // source1402 mask and enabled flag (1A7818, then17D6A0).
        if(component.parent1C!=pause->entity_->application->application10()||
           component.mask20!=0x1402u||!(component.flags29&1u)||(component.flags29&8u)||
           component.firstChild18||component.descendants24){
            result.status=Status::parent;result.sourcePC=0x142350;return result;
        }
        if(!pause->mode1F0_&&event.deviceId==pause->controller884_&&event.category==2u){
            //1277D0 compares code/category. Its context query is category0
            //only; the source0x80000000 argument does not filter category2.
            if(event.code==12u||event.code==14u){
                if(pause->selection880_>=11)return result;
                if(event.code==12u)pause->selection880_=pause->selection880_?pause->selection880_-1u:10u;
                else pause->selection880_=pause->selection880_==10u?0u:pause->selection880_+1u;
                result.selectionChanged=true;
                result.sourcePC=event.code==12u?0x1BD844:0x1BD8A0;
                // Source mutation precedes audio. On native rejection retain
                // the mutation and report partial progress; do not retry it.
                result.audio=camera->actual_.cycleAudio.dispatchPauseFeedback(event.code==12u?3u:4u);
                using A=original_camera_cycle_audio50KX::Owner::Result;
                if(result.audio!=A::started&&result.audio!=A::unavailable&&result.audio!=A::muted){
                    result.status=Status::audioUnavailable;return result;
                }
            }else if(event.code==2u||event.code==15u||event.code==13u){
                result.status=Status::actionUnavailable;result.sourcePC=0x1BD8BC;return result;
            }
        }
        // This concrete owner has no Entity children. Use the genuine actual
        // descendant mask rather than inventing a successful child callback.
        const rr::game::EventChildRouter48Z children{component.descendants24,
            rr::game::noEventChild48Z,nullptr,0};
        const auto routed=rr::game::routeChildInputEvent48Z(children,true,event,record,nullptr,nullptr,0);
        result.status=routed.status==rr::game::EventRouteStatus48Z::Complete?Status::complete:Status::childRoute;
        result.sourcePC=0x1BE80C;result.originalReturn=routed.handled?1u:0u;return result;
    }
    void unbind()noexcept {
        camera_.reset();track_.reset();cameraGeneration_=0;
        application_=nullptr;pause_.reset();controls_.reset();settings_.reset();
    }
private:
    bool raceIdentity(const Camera& c,const Track& track,
        const rr::game::OriginalPauseStorage50LA& p,const rr::game::ControlPresets49D& controls)const noexcept {
        const auto& selection=track.freshSelection;
        return !c.closed_&&!c.abandoned_&&c.coldBound_&&c.generation_&&
            p.stage_==rr::game::OriginalPauseStorage50LA::Stage::derivedResources&&
            p.context_&&p.context_->live()&&p.context_->materialApplication()==application_->materialApplication&&
            c.actual_.applicationMaterials==p.domain_->materials382B38()&&
            &c.actual_.preferences==&controls&&c.actual_.actualRadarMapB8==&track.map&&
            selection.bound&&selection.selectedTrackId1125F==7&&selection.selectedOrder187B10==4&&
            selection.freshCourseRecordPod11280==9&&selection.actualRaceClass==rr::game::AuthoredClass49N::A&&
            c.actual_.launchKind11258==static_cast<std::uint32_t>(p.subtype11258_);
    }
};
}
