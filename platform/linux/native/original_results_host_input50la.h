#pragma once
#include "window_host50dq.h"
#include "../../../src/game/event_dispatch48z.h"
#include <array>
#include <cstdint>
#include <limits>
namespace original_main_entry50KG { struct Application; }
namespace rr::game { class OriginalResults10450LA; class OriginalRaceConstructor50LA; }
namespace rr::platform {
class GameHost50ER;
// EXPLICIT NATIVE device policy, not original pad6C4/238CB0/SIO2 storage.
// GameHost alone observes its actual successful Window poll. No injected
// packet, caller-ready bit, generic callback or public event setter exists.
class OriginalResultsHostInput50LA final {
    friend class GameHost50ER;
    friend struct ::original_main_entry50KG::Application;
    friend class rr::game::OriginalResults10450LA;
    friend class rr::game::OriginalRaceConstructor50LA;
    // One immutable native UI configuration, distinct from remappable49D
    // driving rows and from original managerCBC. SDL supplies already-mapped
    // logical Dpad/A; no raw joystick index or source CBC custody is implied.
    // Changing this transport requires a new policy version and a fresh poll.
    static constexpr std::uint32_t fixedUiPolicyVersion_=3;
    static constexpr std::array<unsigned,6> keys_{{82,81,80,79,40,88}};
    static constexpr std::array<unsigned,5> buttons_{{11,12,13,14,0}};
    static constexpr std::array<std::uint32_t,5> codes_{{12,14,15,13,2}};
    // Exact17CB04..98 category2 defaults, used ONLY by the labeled native
    // mapped127870 policy (Race predecessors). This is NOT configurable
    // originalCBC storage and MUST NOT remap literal1277D0 comparisons.
    static constexpr std::array<std::int32_t,16> sourceDefaultActions_{{
        15,13,12,14,2,3,0,1,9,8,6,7,4,5,11,10}};
    std::uint32_t pollPolicyVersion_=0;
    std::uint64_t claimedRevision_=0;
    //Native handback quarantine, not source held/repeat state. A key still
    //down when Results loses ownership cannot become a new driver/menu edge.
    std::array<bool,6> handbackKeys_{};
    std::array<std::array<bool,5>,2> handbackButtons_{};
    std::array<std::int32_t,2> handbackInstances_{{-1,-1}};
    // B/right-pad-right and native H use this same Window producer, but are never
    // emitted until the actual active Results issuer offers this exact poll.
    // Ordinary menus keep their existing stream and five-button policy.
    std::array<bool,2> hideArmed_{},hideEdges_{},hideHandback_{};
    std::array<std::int32_t,2> hideInstances_{{-1,-1}};
    static constexpr unsigned hideKeyboardKey_=11; //SDL H, native alias only.
    bool hideKeyboardArmed_=false,hideKeyboardEdge_=false,hideKeyboardHandback_=false;
    std::uint64_t hideObservationRevision_=0;
    bool hideObservationValid_=false;
    enum class HideAppend : std::uint8_t {absent,appended,capacity,invalid};
    HideAppend hideAppend_=HideAppend::absent;
    const game::OriginalResults10450LA* hideIssuer_=nullptr;
    bool fixedUiCurrent()const noexcept {
        return current_&&revision_&&pollPolicyVersion_==fixedUiPolicyVersion_;
    }
    bool matchesNativeCategory2Code1277D0(std::uint32_t code,const game::InputEvent48Z& event,
        bool& matches)const noexcept {
        if(!fixedUiCurrent()||claimedRevision_!=revision_||
           event.deviceId>1u||event.category!=2u||event.transition!=1u)return false;
        //1277E0..1277FC compares the supplied literal code and category.
        // Category2 skips the category0 context-mask query at127808..127840.
        // The native producer already emitted the actual logical button code;
        // indexing CBC defaults here would map Confirm2 to Up12 and Restart3
        // to Down14 a second time. No source CBC or hardware receipt is made.
        matches=event.code==code;return true;
    }
    bool matchesNativeDefaultAction(std::uint32_t action,const game::InputEvent48Z& event,
        bool& matches)const noexcept {
        if(!fixedUiCurrent()||claimedRevision_!=revision_||action>=sourceDefaultActions_.size()||
           event.deviceId>1u||event.category!=2u||event.transition!=1u)return false;
        const auto code=sourceDefaultActions_[action];
        matches=code!=-1&&event.code==static_cast<std::uint32_t>(code);return true;
    }
    // Only the GameHost's exact successfully-polled destination may reach
    // this retirement. Clear only this UI transport's controls, never quit,
    // capture, physical identity, unrelated buttons or source-held records.
    void retireClaimedControls(WindowInput50DQ& output)noexcept {
        if(fixedUiCurrent()){
            const bool claimed=claimedRevision_==revision_;
            for(std::size_t i=0;i<keys_.size();++i){
                const auto key=keys_[i];
                if(claimed||handbackKeys_[i]){
                    handbackKeys_[i]=output.down[key]||output.pressed[key];
                    output.down[key]=output.pressed[key]=output.released[key]=0;
                }
            }
            // H belongs only to the authenticated Results extension, never
            // to an ordinary menu's six-key claim. Retain its own handback
            // quarantine until the actual key is released.
            if((claimed&&hideIssuer_)||hideKeyboardHandback_){
                hideKeyboardHandback_=output.down[hideKeyboardKey_]||output.pressed[hideKeyboardKey_];
                output.down[hideKeyboardKey_]=output.pressed[hideKeyboardKey_]=output.released[hideKeyboardKey_]=0;
            }
            for(std::size_t port=0;port<2;++port){
                auto& pad=output.controllers[port];
                if(!pad.connected||handbackInstances_[port]!=pad.instanceId)
                    handbackButtons_[port]={};
                handbackInstances_[port]=pad.connected?pad.instanceId:-1;
                for(std::size_t i=0;i<buttons_.size();++i){
                    const auto button=buttons_[i];
                    if(claimed||handbackButtons_[port][i]){
                        handbackButtons_[port][i]=pad.buttons[button]||pad.pressed[button];
                        pad.buttons[button]=pad.pressed[button]=pad.released[button]=0;
                    }
                }
                // Retire B only for its authenticated extended issuer, or
                // while releasing that SAME controller's previous hold.
                if(!pad.connected||hideInstances_[port]!=pad.instanceId)hideHandback_[port]=false;
                if((claimed&&hideIssuer_)||hideHandback_[port]){
                    hideHandback_[port]=pad.buttons[1]||pad.pressed[1];
                    pad.buttons[1]=pad.pressed[1]=pad.released[1]=0;
                }
            }
        }
        edgesOpen_=false;
    }
    struct Event {game::InputEvent48Z event{};game::DeviceButtonRecord48Y record{};};
    std::array<Event,10> events_{};
    std::size_t count_=0;
    std::array<bool,2> available_{};
    std::array<bool,2> padArmed_{};
    std::array<std::int32_t,2> padInstance_{{-1,-1}};
    bool keyboardArmed_=false,current_=false,edgesOpen_=false;
    std::uint64_t revision_=0;
    void invalidate()noexcept {
        current_=edgesOpen_=false;count_=0;pollPolicyVersion_=0;claimedRevision_=0;
        hideEdges_={};hideKeyboardEdge_=false;hideObservationRevision_=0;hideObservationValid_=false;
        hideIssuer_=nullptr;hideAppend_=HideAppend::absent;
    }
    void interrupt()noexcept {
        invalidate();keyboardArmed_=false;padArmed_={};padInstance_={{-1,-1}};
        handbackKeys_={};handbackButtons_={};handbackInstances_={{-1,-1}};
        hideArmed_={};hideHandback_={};hideInstances_={{-1,-1}};
        hideKeyboardArmed_=hideKeyboardHandback_=false;
    }
    bool appendResultsHideSamePoll(const game::OriginalResults10450LA& issuer)noexcept {
        if(!fixedUiCurrent()||!edgesOpen_||claimedRevision_||hideIssuer_||
           hideAppend_!=HideAppend::absent||hideObservationRevision_!=revision_)return false;
        hideIssuer_=&issuer;
        if(!hideObservationValid_){hideAppend_=HideAppend::invalid;count_=0;return true;}
        const bool port0Hide=hideKeyboardEdge_||hideEdges_[0];
        const std::size_t extra=std::size_t(port0Hide)+std::size_t(hideEdges_[1]);
        if(count_>events_.size()||extra>events_.size()-count_){
            // Keep the original ten-event budget. Reject the WHOLE batch;
            // Results must claim/retire controls before reporting refusal.
            hideAppend_=HideAppend::capacity;count_=0;return true;
        }
        // Preserve every existing event's relative order. Native extension
        // appends logical0 (H/B coalesced) then B1; this is not a PS2
        // event-order claim or a remapping of original1277D0 code1.
        for(std::size_t port=0;port<2;++port)if(port==0?port0Hide:hideEdges_[port]){
            events_[count_].event={1u,2u,static_cast<std::uint32_t>(port),1u,0u};
            events_[count_].record={1u,1.f,0u,0u,0u,0u};++count_;
        }
        hideAppend_=HideAppend::appended;return true;
    }
    void observeWindow(const WindowInput50DQ& input,bool actualKeyboardConnected,
        bool loadingBarrier)noexcept {
        invalidate();
        if(revision_==std::numeric_limits<std::uint64_t>::max())return;
        ++revision_;
        // The native after-load barrier (keys/sticks still held from loading
        // or a blocking fade) only withholds edges. It is not a disconnected
        // device: 1B9FE0's 239C10 read is the pad connection state, so the
        // Results "insert controller" text must not follow the barrier.
        const bool present=input.focused&&!input.minimized&&!input.activityLost&&
            !input.eventBudgetExhausted&&!input.quitRequested;
        const bool active=present&&!loadingBarrier;
        // Logical native UI device0 owns keyboard plus mapped physical port0.
        // Port1 exists ONLY for the actual second physical controller. These
        // are host availability observations, never PS2 connection receipts.
        available_[0]=present&&(actualKeyboardConnected||
            (input.gameControllersEnabled&&input.controllers[0].connected));
        available_[1]=present&&input.gameControllersEnabled&&input.controllers[1].connected;
        const auto& keys=keys_;const auto& buttons=buttons_;const auto& codes=codes_;
        for(auto key:keys)if(input.down[key]>1||input.pressed[key]>1||input.released[key]>1){interrupt();return;}
        std::array<std::array<bool,5>,2> edges{};
        bool neutral=true;
        for(auto key:keys)neutral=neutral&&!input.down[key]&&!input.pressed[key];
        if(!active||!actualKeyboardConnected)keyboardArmed_=false;
        else if(!keyboardArmed_)keyboardArmed_=neutral;
        else {
            for(std::size_t i=0;i<4;++i)edges[0][i]=input.pressed[keys[i]]!=0;
            edges[0][4]=input.pressed[40]!=0||input.pressed[88]!=0;
        }
        for(std::size_t port=0;port<2;++port){
            const auto& pad=input.controllers[port];
            if(!input.gameControllersEnabled||!pad.connected){padInstance_[port]=-1;padArmed_[port]=false;continue;}
            if(pad.instanceId<0){interrupt();return;}
            const bool changed=padInstance_[port]!=pad.instanceId;
            if(changed){padInstance_[port]=pad.instanceId;padArmed_[port]=false;}
            bool padNeutral=true;
            for(auto button:buttons){
                if(pad.buttons[button]>1||pad.pressed[button]>1||pad.released[button]>1){interrupt();return;}
                padNeutral=padNeutral&&!pad.buttons[button]&&!pad.pressed[button];
            }
            if(!active)padArmed_[port]=false;
            else if(changed||!padArmed_[port])padArmed_[port]=padNeutral;
            else for(std::size_t i=0;i<5;++i)edges[port][i]=edges[port][i]||pad.pressed[buttons[i]]!=0;
        }
        // Deterministic native per-poll order; not PS2 manager hardware order.
        // Coalesce matching keyboard/port0 edges once. No held-key repeat.
        for(std::size_t port=0;port<2;++port)for(std::size_t i=0;i<5;++i)if(edges[port][i]){
            events_[count_].event={codes[i],2u,static_cast<std::uint32_t>(port),1u,0u};
            events_[count_].record={1u,1.f,0u,0u,0u,0u};++count_;
        }
        hideObservationValid_=true;
        // Separate release-to-arm state: adding a Results-only H alias must
        // not make holding H suppress ordinary arrows/Confirm, or retire H
        // when the actual Results issuer has not claimed this exact poll.
        if(!active||!actualKeyboardConnected)hideKeyboardArmed_=false;
        else if(input.down[hideKeyboardKey_]>1||input.pressed[hideKeyboardKey_]>1||
                input.released[hideKeyboardKey_]>1){
            hideObservationValid_=false;hideKeyboardArmed_=false;
        }else if(!hideKeyboardArmed_)
            hideKeyboardArmed_=!input.down[hideKeyboardKey_]&&!input.pressed[hideKeyboardKey_];
        else hideKeyboardEdge_=input.pressed[hideKeyboardKey_]!=0;
        for(std::size_t port=0;port<2;++port){
            const auto& pad=input.controllers[port];
            if(!input.gameControllersEnabled||!pad.connected){
                hideInstances_[port]=-1;hideArmed_[port]=false;hideHandback_[port]=false;continue;
            }
            const bool changed=hideInstances_[port]!=pad.instanceId;
            if(changed){hideInstances_[port]=pad.instanceId;hideArmed_[port]=false;hideHandback_[port]=false;}
            if(pad.instanceId<0||pad.buttons[1]>1||pad.pressed[1]>1||pad.released[1]>1){
                hideObservationValid_=false;hideArmed_[port]=false;continue;
            }
            if(!active)hideArmed_[port]=false;
            else if(changed||!hideArmed_[port])hideArmed_[port]=!pad.buttons[1]&&!pad.pressed[1];
            else hideEdges_[port]=pad.pressed[1]!=0;
        }
        hideObservationRevision_=revision_;
        pollPolicyVersion_=fixedUiPolicyVersion_;current_=true;edgesOpen_=true;
    }
};
}
