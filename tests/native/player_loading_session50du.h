#pragma once
#include <chrono>
#include <cstdio>
#include <optional>
#include <stdexcept>
#include "../../platform/linux/native/loading_progress50du.h"
#include "authored_loading_presenter50du.h"

struct PlayerLoadingCancelled50DU final {};

// Root scope only. The original Application remains the sole Video owner;
// this adapter neither opens/replaces a window nor retains a Display lease.
class PlayerLoadingSession50DU final {
    using Clock=std::chrono::steady_clock;
    const original_main_entry50KG::Application& application_;
    rr::platform::loading_progress50DU::Observer previous_{};
    const char* stage_;
    const char* screenCode_;
    Clock::time_point nextPoll_{};
    bool installed_=false,finished_=false,quit_=false,failed_=false;
    std::optional<rr::platform::PlayerLoadingPresenter50DU> presenter_;
    inline static thread_local PlayerLoadingSession50DU* active_=nullptr;
    PlayerLoadingSession50DU* previousActive_=nullptr;
    void pump(const char* stage)noexcept {
        if(finished_)return;
        const bool changed=stage&&stage!=stage_;
        if(stage)stage_=stage;
        const auto now=Clock::now();
        if(!changed&&now<nextPoll_)return;
        nextPoll_=now+std::chrono::milliseconds(100);
        const auto video=application_.playerVideo50DU();
        if(!video||!video->host.created())return;
        rr::platform::GameHostDiagnostic50ER diagnostic;
        bool close=false;
        if(!video->host.pollLoading50DU(close,diagnostic)){
            if(!failed_)std::fprintf(stderr,"player_loading_event_failure operation=%s error=%u\n",
                diagnostic.operation?diagnostic.operation:"unknown",static_cast<unsigned>(diagnostic.error));
            failed_=true;return;
        }
        quit_=quit_||close;
        if(!presenter_)presenter_.emplace(video->host,application_.assetRoot,screenCode_,application_.riderProfile);
        if(!presenter_->draw(stage_,quit_)){
            if(!failed_)std::fprintf(stderr,"player_loading_present_failure\n");
            failed_=true;
        }
    }
public:
    PlayerLoadingSession50DU(const original_main_entry50KG::Application& application,const char* stage,const char* screenCode)
        :application_(application),stage_(stage),screenCode_(screenCode) {
        if(!application.retainsPlayerWindow50DU())return;
        const auto video=application.playerVideo50DU();
        if(video&&video->host.created()){
            std::fprintf(stderr,"native_player_window phase=retained sdl_id=%u video=%p host=%p\n",
                static_cast<unsigned>(video->host.nativeWindowId50DU()),
                static_cast<void*>(video.get()),static_cast<void*>(&video->host));
            rr::platform::GameHostDiagnostic50ER diagnostic;
            if(!video->host.prepareRetainedPlayerSession50DU(diagnostic))
                throw std::runtime_error("retained player session GL/window preparation rejected");
        }
        previous_=rr::platform::loading_progress50DU::observer;
        previousActive_=active_;active_=this;installed_=true;
        rr::platform::loading_progress50DU::observer={this,
            [](void* context,const char* label)noexcept{static_cast<PlayerLoadingSession50DU*>(context)->pump(label);},
            [](void* context)noexcept{
                auto& self=*static_cast<PlayerLoadingSession50DU*>(context);
                self.nextPoll_={};self.pump(nullptr);self.finished_=true;
                const auto video=self.application_.playerVideo50DU();
                if(video&&video->host.created()&&!self.quit_&&!self.failed_){
                    rr::platform::GameHostDiagnostic50ER diagnostic;
                    if(!video->host.setWindowTitle("Racer Revenge",diagnostic))self.failed_=true;
                    self.application_.showBetaPersistenceStatus();
                }
            }};
        pump(stage_);
    }
    PlayerLoadingSession50DU(const PlayerLoadingSession50DU&)=delete;
    PlayerLoadingSession50DU& operator=(const PlayerLoadingSession50DU&)=delete;
    static void checkStop() {
        if(!active_)return;
        if(active_->failed_)throw std::runtime_error("native player loading presentation/event pump failed");
        if(active_->quit_)throw PlayerLoadingCancelled50DU{};
    }
    void end()noexcept {
        if(!installed_)return;
        rr::platform::loading_progress50DU::observer=previous_;
        active_=previousActive_;installed_=false;
    }
    ~PlayerLoadingSession50DU()noexcept{end();}
};
