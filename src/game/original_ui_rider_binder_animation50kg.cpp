#include "original_ui_rider_binder_animation50kg.h"
#include <utility>
namespace rr::game {
bool OriginalUiRiderBinderAnimation50KG::initialize(
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> assets) noexcept {
    if(assets_||!assets||!assets->loaded()||!assets->animation()||
       assets->animation()->frames.size()!=10)return false;
    OriginalUiTextureAnimationConfig50KG config;config.framesPerSecond=16.f;
    config.reverseLoopOnEnd=false;config.loopOnEndToFrame=0;
    // InitialFrame0, Looping=true are actual constructor defaults, not overrides.
    OriginalUiTextureAnimation50KG next;
    if(!initializeOriginalUiTextureAnimation50KG(next,config,10))return false;
    assets_=std::move(assets);state_=next;return true;
}
bool OriginalUiRiderBinderAnimation50KG::bind(std::uint32_t frame,
    OriginalUiRiderBinderFrame50KG& out)const noexcept {
    if(!assets_||!assets_->loaded()||!assets_->animation()||
       frame>=assets_->animation()->frames.size()||!assets_->images())return false;
    const auto* decoded=assets_->images()->decodedTexture(frame);const auto* pass=assets_->pass();
    if(!decoded||!pass)return false;
    const auto& range=assets_->animation()->frames[frame];
    out={pass,assets_->images(),decoded,frame,range.pstOffset,range.pstBytes};return true;
}
bool OriginalUiRiderBinderAnimation50KG::current(OriginalUiRiderBinderFrame50KG& out)const noexcept {
    if(!assets_)return false;
    std::uint32_t frame=0;if(!selectOriginalUiTextureAnimationFrame50KG(state_,frame))return false;
    return bind(frame,out);
}
bool OriginalUiRiderBinderAnimation50KG::advanceAfterGeometry(std::uint32_t milliseconds,
    OriginalUiRiderBinderFrame50KG& out) noexcept {
    if(!assets_)return false;
    auto next=state_;std::uint32_t frame=0;OriginalUiRiderBinderFrame50KG selected;
    if(!advanceOriginalUiTextureAnimation50KG(next,milliseconds,frame)||!bind(frame,selected))return false;
    state_=next;out=selected;return true;
}
}
