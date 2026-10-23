#include "original_ui_garage_renderer_lease50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include "../../../src/game/original_frontend_camera_cold50la.h"
#include <utility>
namespace rr::platform {
OriginalUiGarageRendererLease50KG::OriginalUiGarageRendererLease50KG(
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> display,
    const ::original_main_entry50KG::Application* issuer)noexcept
    :display_(std::move(display)),issuer_(issuer),generation_(display_->generation_),
     applicationLifetime_(issuer->animationClock()) {
    display_->pin();
}
std::shared_ptr<OriginalUiGarageRendererLease50KG> OriginalUiGarageRendererLease50KG::retain(
    const ::original_main_entry50KG::Application* issuer,
    const std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA>& display)noexcept {
    if(!issuer||!display||display->application_!=issuer||!display->generation_||!display->canRetain())return {};
    std::uint32_t pause=0;
    if(!issuer->animationClock().readPause11C(pause))return {};
    try{return std::shared_ptr<OriginalUiGarageRendererLease50KG>(new OriginalUiGarageRendererLease50KG(display,issuer));}
    catch(...){return {};}
}
bool OriginalUiGarageRendererLease50KG::currentFor(const ::original_main_entry50KG::Application* issuer)const noexcept {
    std::uint32_t pause=0;
    // Retained backing is invalidated by Application destruction. Check it
    // before using raw identity; do not retain or resurrect the Application.
    return display_&&display_->ownerThread()&&applicationLifetime_.readPause11C(pause)&&issuer&&issuer==issuer_&&display_&&display_->application_==issuer&&
        generation_&&display_->generation_==generation_&&display_->canRetain();
}
bool OriginalUiGarageRendererLease50KG::poll245030(bool& complete,VehicleWindowDiagnostic50DS& d)noexcept {
    complete=false;return currentFor(issuer_)&&display_->renderer_.pollOriginalRenderer245030(complete,d);
}
bool OriginalUiGarageRendererLease50KG::drain1404BC(VehicleWindowDiagnostic50DS& d,game::OriginalCameraStartResult& r)noexcept {
    if(!currentFor(issuer_)){r={game::OriginalCameraStartStatus::owner,0x1404BC};return false;}
    std::int32_t value=0;r=game::serviceOriginalRenderer244D60(display_->dispatch_,true,display_->renderer_,d,value,
        game::OriginalCameraDrainPolicy::initializedContext);
    return r.status==game::OriginalCameraStartStatus::complete;
}
bool OriginalUiGarageRendererLease50KG::applyFogColor244A40(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t serial,std::uint32_t argb,VehicleWindowDiagnostic50DS& d)noexcept {
    if(!currentFor(issuer_)||display_->dispatch_.pendingCallbackDC||display_->dispatch_.count54||display_->dispatch_.count60)return false;
    if(!display_->renderer_.applyGarageFogColor244A40(owner,serial,argb,d))return false;
    display_->sourceFogGsColor3D_=argb;display_->sourceFogGsColor3DKnown_=true;
    return true;
}
bool OriginalUiGarageRendererLease50KG::clearViewport2458C0(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t serial,bool color,std::uint32_t argb,VehicleWindowDiagnostic50DS& d)noexcept {
    return currentFor(issuer_)&&display_->renderer_.clearGarageSourceViewport50KG(owner,serial,color,argb,d);
}
bool OriginalUiGarageRendererLease50KG::ownsRenderPrelude(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t serial)const noexcept {
    return display_&&display_->renderer_.ownsGarageSourcePrelude50KG(owner,serial);
}
bool OriginalUiGarageRendererLease50KG::releaseRenderPrelude(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t serial,VehicleWindowDiagnostic50DS& d)noexcept {
    return currentFor(issuer_)&&display_->renderer_.releaseGarageSourcePrelude50KG(owner,serial,d);
}
bool OriginalUiGarageRendererLease50KG::complete245090(VehicleWindowDiagnostic50DS& d,game::OriginalCameraStartResult& r)noexcept {
    using S=game::OriginalCameraStartStatus;
    if(!currentFor(issuer_)){r={S::owner,0x245090};return false;}
    //245090 is the blocking242EE0(1,3) service, not material collection.
    //The native empty-queue equivalent completes the actual current context;
    //an unrepresented source queue/callback cannot be discharged by glFinish.
    const auto& fields=display_->dispatch_;
    if(fields.pendingCallbackDC){r={S::rendererCallback,0x242F78};return false;}
    if(fields.count54!=0||fields.count60!=0){r={S::rendererQueue,0x245090};return false;}
    if(!display_->renderer_.finishInitializedContextWork(d)||!d.contextWorkCompleted){
        r={S::rendererDrain,0x245090};return false;}
    r={S::complete,0x245098};return true;
}
bool OriginalUiGarageRendererLease50KG::serviceEntity141F3C(
    VehicleWindowDiagnostic50DS& diagnostic,game::OriginalCameraStartResult& result)noexcept {
    if(!currentFor(issuer_)){
        result={game::OriginalCameraStartStatus::owner,0x141F3Cu};return false;
    }
    std::int32_t actualV0=0;
    result=game::serviceOriginalRenderer244D60(display_->dispatch_,false,display_->renderer_,diagnostic,actualV0);
    return result.status==game::OriginalCameraStartStatus::complete;
}
bool OriginalUiGarageRendererLease50KG::textViewport1159D0(
    std::array<std::int32_t,4>& rectangle,std::array<std::int32_t,2>& video,
    std::uint32_t& pc)const noexcept {
    pc=0x29CDF0;
    if(!currentFor(issuer_)||!display_->sourceCurrentCamera0Known_||
       !display_->sourceSelectedCamera0_||!display_->video_)return false;
    const auto& camera=*display_->sourceSelectedCamera0_;
    using Camera=game::OriginalFrontendCameraCold50LA;
    if(!camera.nativeRegistrationIdentityCurrent()||camera.entities_.get()!=issuer_->entities().get()||
       camera.phase_!=Camera::Phase::pageAttached1FB620)return false;
    const auto width=camera.fields_.viewport1E0[2],height=camera.fields_.viewport1E0[3];
    //1159D0's unsigned->float->int conversion is exact in the represented
    //native viewport range. Larger/nonpositive or retired targets are not
    //silently clamped and cannot authorize the later divide.
    pc=0x1159E4;
    if(!width||!height||width>0x01000000u||height>0x01000000u)return false;
    rectangle={0,0,static_cast<std::int32_t>(static_cast<float>(width)),
                    static_cast<std::int32_t>(static_cast<float>(height))};
    pc=0x29CE18;
    if(display_->video_->width<=0||display_->video_->height<=0)return false;
    video={display_->video_->width,display_->video_->height};
    pc=0x115A5C;return true;
}
OriginalUiGarageRendererLease50KG::~OriginalUiGarageRendererLease50KG()noexcept {
    if(display_)display_->unpin();
}
}
