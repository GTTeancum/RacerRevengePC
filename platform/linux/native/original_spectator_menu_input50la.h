#pragma once
#include "game_host50er.h"
#include <exception>
#include "../../../src/game/event_dispatch48z.h"

namespace original_pause_observation50LA { class Owner; }
namespace rr::platform {
// Physical host UI edges only. Code8 is the native ALL8 spectator-menu
// command; it does not claim original127870 or human-racer input admission.
class OriginalSpectatorMenuInput50LA final {
    bool keyboardArmed_=false,padArmed_=false;
    std::int32_t padInstance_=-1;
    friend struct ::interactive_loop50DU::Context;
    friend struct ::original_main_entry50KG::Application;
    friend class GameHost50ER;
    friend class ::original_pause_observation50LA::Owner;
    struct Batch {
        std::array<game::InputEvent48Z,6> events{};
        std::array<game::DeviceButtonRecord48Y,6> records{};
        std::size_t count=0;
    };
    // One-shot custody for the live native producer, not guest input admission.
    // Storage stays at the original addresses through synchronous callbacks,
    // actual retained row9 continuation and terminal partial failure.
    enum class Phase : std::uint8_t { empty,ready,dispatching,suspended,retired,failed };
    const std::thread::id thread_=std::this_thread::get_id();
    Batch batch_{};
    Phase phase_=Phase::empty;
    GameHost50ER* host_=nullptr; //Actual outer host outlives this publication-local owner.
    std::uint64_t pollRevision_=0;
    const original_pause_observation50LA::Owner* claimedOwner_=nullptr;
    std::size_t next_=0;
    bool eventStarted_=false;
    bool retainedForPauseReturn_=false; //Only actual Application continuation may transfer this poll.
    bool currentPoll()const noexcept {
        return thread_==std::this_thread::get_id()&&host_&&host_->ownerThread()&&
            host_->created()&&host_->spectatorBatchOwner50LA_==this&&
            (host_->sharedDialogPollCurrent50KG_||retainedForPauseReturn_)&&pollRevision_&&
            host_->sharedDialogPollRevision50KG_==pollRevision_;
    }
    bool beginDispatch(const original_pause_observation50LA::Owner& owner)noexcept {
        if(thread_!=std::this_thread::get_id()||phase_!=Phase::ready||
           claimedOwner_||next_||eventStarted_||!currentPoll())return false;
        // No callback may run before this one-shot claim. A second dispatch
        // cannot restart an event, including one whose effects partly failed.
        claimedOwner_=&owner;phase_=Phase::dispatching;return true;
    }
    bool retire(const original_pause_observation50LA::Owner& owner)noexcept {
        if(thread_!=std::this_thread::get_id()||phase_!=Phase::dispatching||
           claimedOwner_!=&owner||eventStarted_||next_!=batch_.count||!currentPoll())return false;
        if(retainedForPauseReturn_){
            host_->sharedDialogPollCurrent50KG_=false;host_->loadingInputBarrier50DU_=true;
        }
        host_->spectatorBatchOwner50LA_=nullptr;host_=nullptr;retainedForPauseReturn_=false;
        phase_=Phase::retired;return true;
    }
    bool observe(GameHost50ER& host,GameHostInput50ER& actual,bool active)noexcept {
        if(thread_!=std::this_thread::get_id()||!host.ownerThread()||!host.created()||
           (phase_!=Phase::empty&&phase_!=Phase::retired)||host_||
           host.spectatorBatchOwner50LA_||host.retainedResultsPollOwner50LA_||
           !host.sharedDialogPollCurrent50KG_||!host.sharedDialogPollRevision50KG_||
           host.spectatorPollDestination50LA_!=&actual)return false;
        // Consume the real successful poll's exact output once. Neither a
        // copied snapshot nor a producer-local serial can issue this claim.
        host_=&host;pollRevision_=host.sharedDialogPollRevision50KG_;
        host.spectatorBatchOwner50LA_=this;host.spectatorPollDestination50LA_=nullptr;
        phase_=Phase::failed;claimedOwner_=nullptr;next_=0;eventStarted_=false;batch_={};
        const auto& input=actual.keys;
        auto& out=batch_;
        constexpr unsigned keys[]={41,82,81,40,88,80,79}; //Escape, Up, Down, Return, keypad Enter, Left, Right.
        constexpr unsigned buttons[]={6,11,12,0,13,14}; //SDL mapped Start, Up, Down, A, Left, Right.
        for(auto key:keys)if(input.down[key]>1||input.pressed[key]>1||input.released[key]>1)return false;
        const auto& pad=input.controllers[0];
        const bool connected=input.gameControllersEnabled&&pad.connected;
        if(connected){
            if(pad.instanceId<0)return false;
            for(auto button:buttons)if(pad.buttons[button]>1||pad.pressed[button]>1||pad.released[button]>1)return false;
        }
        if(!connected){padInstance_=-1;padArmed_=false;}
        else if(padInstance_!=pad.instanceId){padInstance_=pad.instanceId;padArmed_=false;}
        if(!active||!input.focused||input.minimized||input.activityLost||input.eventBudgetExhausted||input.quitRequested){
            keyboardArmed_=padArmed_=false;phase_=Phase::ready;return true;
        }
        std::array<bool,6> edges{};
        bool keyboardNeutral=true;
        for(auto key:keys)keyboardNeutral=keyboardNeutral&&!input.down[key]&&!input.pressed[key];
        if(keyboardArmed_){
            edges={{input.pressed[41]!=0,input.pressed[82]!=0,input.pressed[81]!=0,
                input.pressed[40]!=0||input.pressed[88]!=0,input.pressed[80]!=0,input.pressed[79]!=0}};
        }else keyboardArmed_=keyboardNeutral;
        if(connected){
            bool neutral=true;
            for(auto button:buttons)neutral=neutral&&!pad.buttons[button]&&!pad.pressed[button];
            if(padArmed_){
                for(std::size_t i=0;i<edges.size();++i)edges[i]=edges[i]||pad.pressed[buttons[i]];
            }else padArmed_=neutral;
        }
        constexpr std::uint32_t codes[]={8,12,14,2,15,13};
        // Keyboard and first physical controller share native UI device0.
        // Simultaneous matching edges dispatch once; no repeat or held drive packet.
        // Existing opening/up/down/Confirm order remains; source1BD7C0 tests
        // Left15 before Right13. These are native physical aliases, not a pad packet.
        for(std::size_t i=0;i<edges.size();++i)if(edges[i]){
            out.events[out.count]={codes[i],2,0,1,0};
            out.records[out.count]={1,1.f,0,0,0,0};++out.count;
        }
        phase_=Phase::ready;return true;
    }
public:
    OriginalSpectatorMenuInput50LA()=default;
    OriginalSpectatorMenuInput50LA(const OriginalSpectatorMenuInput50LA&)=delete;
    OriginalSpectatorMenuInput50LA& operator=(const OriginalSpectatorMenuInput50LA&)=delete;
    OriginalSpectatorMenuInput50LA(OriginalSpectatorMenuInput50LA&&)=delete;
    OriginalSpectatorMenuInput50LA& operator=(OriginalSpectatorMenuInput50LA&&)=delete;
    ~OriginalSpectatorMenuInput50LA()noexcept {
        if(thread_!=std::this_thread::get_id())std::terminate();
        if(retainedForPauseReturn_)std::terminate(); //Real backend must cancel before publication unwinds.
        if(!host_)return;
        if(!host_->ownerThread()||host_->spectatorBatchOwner50LA_!=this)std::terminate();
        // Whole-call abandonment on publication unwind only. No child tail,
        // rollback, source completion, or suspended fade cancellation is
        // implied. A retained blocking continuation must already have cancelled
        // its real backend and released its pins before reaching this point.
        host_->spectatorBatchOwner50LA_=nullptr;
        host_->spectatorPollDestination50LA_=nullptr;
        host_->sharedDialogPollCurrent50KG_=false;
        host_->loadingInputBarrier50DU_=true;
    }
};
}
