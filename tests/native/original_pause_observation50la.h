#pragma once
#include "original_ui_main_entry50kg.h"
#include "../../src/game/original_pause_storage50la.h"

namespace original_pause_observation50LA {
// Serial observation only. Application must outlive this borrow; caller unbinds
// before Application destruction. All concrete/profile ownership is weak.
// This facet cannot supply arbitrary frames, selection, controller or settings.
class Owner final {
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
    void unbind()noexcept {application_=nullptr;pause_.reset();controls_.reset();settings_.reset();}
};
}
