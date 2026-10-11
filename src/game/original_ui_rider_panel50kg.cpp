#include "original_ui_rider_panel50kg.h"
#include <cfenv>
#include <cmath>
namespace rr::game {
namespace {
using Phase=OriginalUiTransitionPhase50KG;
using Life=OriginalUiTransitionLifecycle50KG;
using A=OriginalUiApertureState50KG;
bool ordinary(float v) noexcept{return std::isfinite(v)&&(v==0||std::isnormal(v));}
float step(float time,float dt) noexcept {volatile float speed=140.f/time;volatile float delta=speed*dt;return delta;}
}
bool OriginalUiRiderPanel50KG::bind(OriginalUiApertureOwner50KG& aperture,
    const OriginalUiRiderSelectionData50KG& selection,std::uint8_t player) noexcept {
    const auto* config=aperture.config();const auto* page=selection.page();
    if(aperture_||!config||!page||player>1||config->pageIdentity!=page->identity||
       !ordinary(config->time4B0)||config->time4B0<=0||std::fegetround()!=FE_TONEAREST)return false;
    if(aperture.pollCompletion({5,static_cast<std::uint8_t>(player+1)},Phase::none)==OriginalUiTransitionCompletion50KG::unavailable)return false;
    aperture_=&aperture;page_=page;player_=player;return true;
}
void OriginalUiRiderPanel50KG::entry74() noexcept {if(aperture_){blocked_=false;bypass_=false;}}
void OriginalUiRiderPanel50KG::acceptedBack() noexcept {if(aperture_&&player_==0)bypass_=true;}
void OriginalUiRiderPanel50KG::confirmWhileClosing() noexcept {if(aperture_&&blocked_)bypass_=true;}
OriginalUiApertureStatus50KG OriginalUiRiderPanel50KG::begin7C(Phase phase) noexcept {
    if(!aperture_)return OriginalUiApertureStatus50KG::notBound;
    if(phase==Phase::entry)return aperture_->beginTransition7C(phase);
    //1ED1C8..1ED1F4 does nothing for exit.1ECFE0 initiates it after panel gate.
    return phase==Phase::exit?OriginalUiApertureStatus50KG::noChange:OriginalUiApertureStatus50KG::invalidPhase;
}
bool OriginalUiRiderPanel50KG::update38(float dt,Life life,Phase phase,
    const OriginalUiNavigation50KG& nav,OriginalUiRiderDynamicOwner50KG& dynamic) noexcept {
    if(!aperture_||!ordinary(dt)||dt<0||std::fegetround()!=FE_TONEAREST||nav.page!=page_)return false;
    if(life!=Life::waitScreenExit&&life!=Life::waitPageTransition)return true;
    const auto state=aperture_->snapshot().state;
    if(phase==Phase::entry){
        blocked_=false;if(state!=A::open)return true;
        const float amount=step(aperture_->config()->time4B0,dt);
        volatile float next=offset_-amount;if(!ordinary(amount)||!ordinary(next))return false;
        offset_=next<0?0:next;return true;
    }
    if(phase!=Phase::exit)return false;
    blocked_=true;
    if(offset_<140){
        const float amount=step(aperture_->config()->time4B0,dt);
        volatile float next=offset_+amount;if(!ordinary(amount)||!ordinary(next))return false;
        // Source does not clamp/check readiness until a subsequent update.
        offset_=next;return true;
    }
    offset_=140;
    if(page_->groups.size()<=1)return false;
    const auto slot=nav.groups[1].selectedSlot;const auto& slots=page_->groups[1].controls;
    if(!slot||slot>slots.size())return false;const auto index=slots[slot-1];
    if(index<0||static_cast<std::size_t>(index)>=page_->controls.size())return false;
    const auto& control=page_->controls[static_cast<std::size_t>(index)];if(!control.present)return false;
    bool ready=true;
    if(control.widgets.size()==4){const auto character=control.widgets[3];
        if(dynamic.state(character).kind!=OriginalUiRiderWidgetKind50KG::character)return false;
        ready=dynamic.sourceCharacterReadiness(character);
    }
    if(!ready&&!bypass_)return true;
    if(state==A::closed||state==A::closing)return true;
    const auto result=aperture_->beginTransition7C(Phase::exit);
    return result==OriginalUiApertureStatus50KG::ok||result==OriginalUiApertureStatus50KG::noChange;
}
OriginalUiTransitionCompletion50KG OriginalUiRiderPanel50KG::pollCompletion(Phase phase)const noexcept {
    if(!aperture_)return OriginalUiTransitionCompletion50KG::unavailable;
    const auto aperture=aperture_->pollCompletion({5,static_cast<std::uint8_t>(player_+1)},phase);
    if(aperture!=OriginalUiTransitionCompletion50KG::complete)return aperture;
    return ((phase==Phase::entry&&offset_==0)||(phase==Phase::exit&&offset_==140))?
        OriginalUiTransitionCompletion50KG::complete:OriginalUiTransitionCompletion50KG::pending;
}
}
