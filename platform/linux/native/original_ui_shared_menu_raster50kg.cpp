#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_main_session50kg.h"
#include "original_ui_options_session50kg.h"
#include "vehicle_window_gl50ds.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
namespace rr::platform {
bool OriginalUiSharedDialogs50KG::readMenu470RasterInputs(OriginalUiMenuRasterInputs50KG& out)const noexcept {
    if(!menu470Current()||!menuDisplay_->sourceBufferCount0CInitialized_||
       menuDisplay_->sourceBufferCount0C_!=2u||menuDisplay_->sourceWidth4_!=640||menuDisplay_->sourceHeight8_!=448||
       !inputs_.video||inputs_.video->width!=640||inputs_.video->height!=448||
       inputs_.video->colorBits!=32||inputs_.video->packetZMaximum!=0xFFFFFF||inputs_.video->zBufferFormat!=0x31)return false;
    OriginalUiMenuRasterInputs50KG next;
    const bool okay=menuMainDraw_?
        menuMainSession_->readSharedMenuRasterInputs(*this,*menuMainDraw_,next):
        menuOptionsSession_->readSharedMenuRasterInputs(*this,*menuOptionsDraw_,next);
    const std::shared_ptr<const void> canonicalVideo=inputs_.video;
    if(!okay||next.videoLifetime.get()!=canonicalVideo.get()||
       next.videoLifetime.owner_before(canonicalVideo)||canonicalVideo.owner_before(next.videoLifetime)||
       next.viewport!=std::array<std::uint32_t,4>{{0,0,640,448}}||
       next.xyOffsetFixed4!=std::array<std::uint32_t,2>{{27648,29184}})return false;
    out=std::move(next);return true;
}
bool OriginalUiSharedDialogs50KG::applyMenu470RasterPrelude(Diagnostic& d)noexcept {
    d={};
    if(menuRasterAttempted_||menuRasterFailed_||!menu470RenderCurrent()||
       menu470Phase_!=Menu470Phase::callbacks||menu470Slot_!=0x48||menu470Callback_!=&dialog470_.entity||
       menu470CompletedSlots_!=1u||!readMenu470RasterInputs(menuRaster_))return fail(d,Status::owner,0x245B14);
    // Issued before any fallible GL work. A failed application is not reset by
    // another attempt or by releasing CPU packets; exact renderer retirement is required.
    menuRasterAttempted_=true;
    if(!menuDisplay_->renderer_.applyMenuRasterPrelude50KG(*this,d.renderer)){
        menuRasterFailed_=true;return fail(d,Status::resourcePending,0x245B14);}
    return true;
}
bool OriginalUiSharedDialogs50KG::menu470RasterCurrent()const noexcept {
    if(!menuRasterAttempted_||menuRasterFailed_)return false;
    OriginalUiMenuRasterInputs50KG current;
    return readMenu470RasterInputs(current)&&sameOriginalUiMenuRasterInputs50KG(current,menuRaster_)&&
        menuDisplay_->renderer_.menuRasterPreludeCurrent50KG(*this);
}
bool OriginalUiSharedDialogs50KG::sealMenu470RasterFrame(std::uint64_t frame,Diagnostic& d)noexcept {
    if(!menuRasterAttempted_)return true;
    if(!menu470RasterCurrent()||!frame||menuRasterFrame_||menuRasterNative_||
       !(menuMainDraw_?menuMainDraw_->sharedMenuFrameCurrent(*this,menu470Invocation_,frame):
         menuOptionsDraw_->sharedMenuFrameCurrent(*this,menu470Invocation_,frame)))return fail(d,Status::owner,0x24B7B0);
    menuRasterNative_=menuMainDraw_?menuMainDraw_->draw():menuOptionsDraw_->draw();
    menuRasterFrame_=frame;
    if(!menuDisplay_->renderer_.sealMenuRasterSourceFrame50KG(*this,d.renderer)){
        menuRasterFailed_=true;return fail(d,Status::resourcePending,0x24B7B0);}
    return true;
}
bool OriginalUiSharedDialogs50KG::releaseMenu470RasterAfterAcceptance()noexcept {
    if(!menuRasterAttempted_)return true;
    if(!menuDisplay_||!menuDisplay_->renderer_.acceptMenuRasterSource50KG(*this))return false;
    menuRaster_={};menuRasterAttempted_=false;menuRasterFailed_=false;menuRasterFrame_=0;menuRasterNative_=nullptr;
    return true;
}
bool OriginalUiSharedDialogs50KG::releaseMenu470RasterAfterRetirement(const VehicleWindowGl50DS& renderer,Diagnostic& d)noexcept {
    if(!menuRasterAttempted_)return true;
    if(!menuDisplay_||&menuDisplay_->renderer_!=&renderer||!renderer.menuRasterRetired50KG(*this))
        return fail(d,Status::retirementPending,0x245B14);
    menuRaster_={};menuRasterAttempted_=false;menuRasterFailed_=false;menuRasterFrame_=0;menuRasterNative_=nullptr;
    return true;
}
}
