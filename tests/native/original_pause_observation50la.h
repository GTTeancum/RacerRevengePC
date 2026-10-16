#pragma once
#include "original_ui_main_entry50kg.h"
#include "original_camera_session50kx.h"
#include "ra_cold_sources50la.h"
#include "../../src/game/original_pause_storage50la.h"

namespace original_pause_observation50LA {
// Serial observation only. Application must outlive this borrow; caller unbinds
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
