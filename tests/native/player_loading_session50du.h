#pragma once
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include "../../platform/linux/native/loading_progress50du.h"
#include "../../platform/linux/native/loading_progress_step113a.h"
#include "authored_loading_presenter50du.h"

struct PlayerLoadingCancelled50DU final {};

// Root scope only. The original Application remains the sole Video owner;
// this adapter neither opens/replaces a window nor retains a Display lease.
class PlayerLoadingSession50DU final {
    using Clock=std::chrono::steady_clock;
    // Every original 1CC900(loadscreen,delta) call of one load, in original
    // execution order, keyed by its jal PC. Race: 1A6AA0 and its 1A9EE0 track
    // load. Menu: 1B7300. Each runs once per load in the original.
    struct OriginalStep {std::uint32_t pc;float delta;};
    static constexpr OriginalStep raceSteps_[]={{0x1a6cec,.1f},{0x1aa1b8,.1f},{0x1aa23c,.1f},{0x1aa27c,.1f},
        {0x1a6dec,.1f},{0x1a6ec4,.1f},{0x1a7164,.1f},{0x1a7450,.1f},{0x1a77a4,.1f},{0x1a78dc,.1f}};
    static constexpr OriginalStep menuSteps_[]={{0x1b73a0,.1f},{0x1b7450,.1f},{0x1b74e0,.1f},{0x1b7618,.1f},
        {0x1b7860,.2f},{0x1b7b54,.1f},{0x1b7ce8,.2f},{0x1b7d98,.1f}};
    const original_main_entry50KG::Application& application_;
    rr::platform::loading_progress50DU::Observer previous_{};
    rr::platform::loading_progress50DU::StepObserver previousStep_{};
    const char* stage_;
    const char* screenCode_;
    std::uint64_t sessionIndex_;
    Clock::time_point nextPoll_{};
    bool installed_=false,finished_=false,quit_=false,failed_=false;
    float progress3C_=0.f; // Loading-screen +3C: the running sum of deltas.
    std::uint32_t appliedSteps_=0,stepCount_=0;
    std::optional<rr::platform::PlayerLoadingPresenter50DU> presenter_;
    inline static thread_local PlayerLoadingSession50DU* active_=nullptr;
    PlayerLoadingSession50DU* previousActive_=nullptr;
    bool menuLoad()const noexcept {return screenCode_&&std::string(screenCode_)=="UI";}
    void applyStep(std::size_t index,bool late)noexcept {
        const auto& s=(menuLoad()?menuSteps_:raceSteps_)[index];
        appliedSteps_|=1u<<index;++stepCount_;
        progress3C_+=s.delta; // 1CC910..1CC930: single-precision +3C += delta.
        std::fprintf(stderr,"loading_progress_step screen=%s pc=0x%x delta=%.7g total=%.7g order=%u%s\n",
            screenCode_,static_cast<unsigned>(s.pc),s.delta,progress3C_,stepCount_,late?" late=1":"");
        if(presenter_)presenter_->progress(progress3C_);
    }
    void step(float delta,std::uint32_t pc)noexcept {
        if(finished_)return;
        const bool menu=menuLoad();
        const std::size_t count=menu?std::size(menuSteps_):std::size(raceSteps_);
        for(std::size_t i=0;i<count;++i){const auto& s=(menu?menuSteps_:raceSteps_)[i];
            if(s.pc!=pc)continue;
            if(s.delta!=delta||(appliedSteps_&(1u<<i))){
                std::fprintf(stderr,"loading_progress_step_rejected screen=%s pc=0x%x delta=%.7g repeat=%u\n",
                    screenCode_,static_cast<unsigned>(pc),delta,(appliedSteps_>>i)&1u);
                return;
            }
            applyStep(i,false);nextPoll_={};pump(nullptr);return;
        }
        std::fprintf(stderr,"loading_progress_step_rejected screen=%s pc=0x%x delta=%.7g unknown=1\n",
            screenCode_,static_cast<unsigned>(pc),delta);
    }
    // A load whose port path skipped an original call site still made that
    // call in the original before its loading screen ended: apply it here.
    void finishSteps()noexcept {
        const bool menu=menuLoad();
        const std::size_t count=menu?std::size(menuSteps_):std::size(raceSteps_);
        unsigned late=0;
        for(std::size_t i=0;i<count;++i)if(!(appliedSteps_&(1u<<i))){applyStep(i,true);++late;}
        std::fprintf(stderr,"loading_progress_total screen=%s total=%.7g steps=%u late=%u\n",
            screenCode_,progress3C_,stepCount_,late);
    }
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
        if(!presenter_){presenter_.emplace(video->host,application_.assetRoot,screenCode_,application_.riderProfile,sessionIndex_);
            if(stepCount_)presenter_->progress(progress3C_);}
        if(!presenter_->draw(stage_,quit_)){
            if(!failed_)std::fprintf(stderr,"player_loading_present_failure\n");
            failed_=true;
        }
    }
public:
    PlayerLoadingSession50DU(const original_main_entry50KG::Application& application,const char* stage,const char* screenCode,std::uint64_t sessionIndex)
        :application_(application),stage_(stage),screenCode_(screenCode),sessionIndex_(sessionIndex) {
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
                if(!self.finished_)self.finishSteps();
                self.nextPoll_={};self.pump(nullptr);self.finished_=true;
                const auto video=self.application_.playerVideo50DU();
                if(video&&video->host.created()&&!self.quit_&&!self.failed_){
                    rr::platform::GameHostDiagnostic50ER diagnostic;
                    if(!video->host.setWindowTitle("Star Wars: Racer Revenge",diagnostic))self.failed_=true;
                    self.application_.showBetaPersistenceStatus();
                }
            }};
        previousStep_=rr::platform::loading_progress50DU::stepObserver;
        rr::platform::loading_progress50DU::stepObserver={this,
            [](void* context,float delta,std::uint32_t pc)noexcept{static_cast<PlayerLoadingSession50DU*>(context)->step(delta,pc);}};
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
        rr::platform::loading_progress50DU::stepObserver=previousStep_;
        active_=previousActive_;installed_=false;
    }
    ~PlayerLoadingSession50DU()noexcept{end();}
};
