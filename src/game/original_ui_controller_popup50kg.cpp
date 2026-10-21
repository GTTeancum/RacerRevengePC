#include "original_ui_controller_popup50kg.h"
#include "scalar_minmax48o.h"
#include <cmath>

namespace rr::game {
namespace {
// Keep each source binary32 arithmetic instruction separate, including under
// host expression contraction. Only ordinary finite nonnegative dt is admitted.
float rounded(float v) noexcept { volatile float out=v;return out; }
void place(OriginalUiControllerPopupState50KG& s,float half,float edgeHeight) noexcept {
    const float top=rounded(216.f-half),bottom=rounded(216.f+half);
    s.strips[0]={0,rounded(top-16.f),640,edgeHeight};
    s.strips[1]={0,bottom,640,edgeHeight};
    s.strips[2]={0,top,640,rounded(bottom-top)};
}
OriginalUiControllerPopupStatus50KG begin(const OriginalUiControllerPopupState50KG& s,OriginalUiControllerPopupPlan50KG& p,std::uint32_t pc) noexcept {
    p={};p.state=s;p.sourcePC=pc;
    return OriginalUiControllerPopup50KG::valid(s)?OriginalUiControllerPopupStatus50KG::ok:OriginalUiControllerPopupStatus50KG::invalidState;
}
}
bool OriginalUiControllerPopup50KG::valid(const OriginalUiControllerPopupState50KG& s) noexcept {
    if(!std::isfinite(s.delta3C0)||s.delta3C0<0||!std::isfinite(s.timer3C4)||s.timer3C4>.25f||s.result3D8>3)return false;
    for(const auto& q:s.strips)if(!std::isfinite(q.x)||!std::isfinite(q.y)||!std::isfinite(q.width)||!std::isfinite(q.height)||q.width!=640.f||q.height<0)return false;
    return true;
}
OriginalUiControllerPopupState50KG OriginalUiControllerPopup50KG::initialState() noexcept {
    OriginalUiControllerPopupState50KG s{};
    // Loader 205B4C..206094 initializes these heights; x/y are unused until
    // active render recomputes them. Constructor 2054D0 clears lifecycle fields.
    s.strips[0].height=16;s.strips[1].height=16;s.strips[2].height=14;
    return s;
}
OriginalUiControllerPopupStatus50KG OriginalUiControllerPopup50KG::attach(const OriginalUiControllerPopupState50KG& s,OriginalUiControllerPopupPlan50KG& p) noexcept {
    const auto status=begin(s,p,0x207330);if(status!=OriginalUiControllerPopupStatus50KG::ok)return status;
    p.state.timer3C4=.25f;p.state.delta3C0=0;p.accepted=true;return status;
}
OriginalUiControllerPopupStatus50KG OriginalUiControllerPopup50KG::optionsEntryResetResult(const OriginalUiControllerPopupState50KG& s,OriginalUiControllerPopupPlan50KG& p) noexcept {
    const auto status=begin(s,p,0x1F9614);if(status!=OriginalUiControllerPopupStatus50KG::ok)return status;
    p.state.result3D8=0;p.accepted=true;return status;
}
OriginalUiControllerPopupStatus50KG OriginalUiControllerPopup50KG::startController2(const OriginalUiControllerPopupState50KG& s,OriginalUiControllerPopupPlan50KG& p) noexcept {
    const auto status=begin(s,p,0x2060A0);if(status!=OriginalUiControllerPopupStatus50KG::ok)return status;
    p.state.hasConfirm3D0=true; // Nonnull a2. Null a3 does NOT clear stale 3D4.
    p.state.timer3C4=.125f;p.state.closing3C8=false;p.state.active3CC=true;p.state.result3D8=1;p.accepted=true;
    return status;
}
OriginalUiControllerPopupStatus50KG OriginalUiControllerPopup50KG::input(const OriginalUiControllerPopupState50KG& s,std::uint8_t controller,std::uint8_t sourceInput,bool eventKind2,OriginalUiControllerPopupPlan50KG& p) noexcept {
    const auto status=begin(s,p,0x2062D0);if(status!=OriginalUiControllerPopupStatus50KG::ok)return status;
    if(!s.active3CC||s.result3D8!=1||controller!=0||!eventKind2)return status;
    if(sourceInput!=4&&(sourceInput!=6||!s.hasCancel3D4))return status;
    p.state.timer3C4=.125f;p.state.closing3C8=true;p.state.result3D8=sourceInput==4?2:3;
    p.accepted=true;p.soundRequested=true;p.flushRequested=true;p.soundForce=true;
    p.soundId=sourceInput==4?1:5;p.soundPC=sourceInput==4?0x20635C:0x2063D8;p.flushPC=sourceInput==4?0x206374:0x2063F0;
    return status;
}
OriginalUiControllerPopupStatus50KG OriginalUiControllerPopup50KG::update(const OriginalUiControllerPopupState50KG& s,float delta,OriginalUiControllerPopupPlan50KG& p) noexcept {
    const auto status=begin(s,p,0x206420);if(status!=OriginalUiControllerPopupStatus50KG::ok)return status;
    if(!std::isfinite(delta)||delta<0)return OriginalUiControllerPopupStatus50KG::invalidDelta;
    p.state.delta3C0=delta;p.accepted=true;return status;
}
OriginalUiControllerPopupStatus50KG OriginalUiControllerPopup50KG::render(const OriginalUiControllerPopupState50KG& s,OriginalUiControllerPopupRender50KG& p) noexcept {
    p={};p.state=s;p.strips=s.strips;
    if(!valid(s))return OriginalUiControllerPopupStatus50KG::invalidState;
    if(!s.active3CC)return OriginalUiControllerPopupStatus50KG::ok;
    p.drawDim=true;p.drawStrips=true;
    if(s.timer3C4>0){
        const float ratio=rounded(s.timer3C4/.25f);
        const float fraction=s.closing3C8?ratio:rounded(1.f-ratio);
        const float half=detail::scalarMin48O(60.f,rounded(60.f*fraction));
        place(p.state,half,16.f);
        p.state.timer3C4=rounded(s.timer3C4-s.delta3C0);
        if(p.state.timer3C4<=0){
            place(p.state,s.closing3C8?0.f:60.f,s.closing3C8?0.f:16.f);
            if(s.closing3C8)p.state.active3CC=false;
        }
    }
    p.strips=p.state.strips;
    p.drawMessage=p.state.active3CC&&p.state.timer3C4<=0;
    p.drawConfirm=p.drawMessage&&p.state.hasConfirm3D0;p.drawCancel=p.drawMessage&&p.state.hasCancel3D4;
    return valid(p.state)?OriginalUiControllerPopupStatus50KG::ok:OriginalUiControllerPopupStatus50KG::invalidState;
}
}
