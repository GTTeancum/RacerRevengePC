#include "original_ui_rider_binder_animation50kg.h"
#include <utility>
namespace rr::game {
struct OriginalUiRiderBinderAnimation50KG::Storage {
    enum class Phase : std::uint8_t {unowned,live,invalidated};
    OriginalUiRiderMaterialLifetime50KG* ledger=nullptr;
    OriginalUiRiderMaterialFootprint50KG footprint{};
    OriginalUiRiderMaterialOwnerToken50KG token{};
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> assets;
    OriginalUiTextureAnimation50KG state;
    Phase phase=Phase::unowned;
};
OriginalUiRiderBinderAnimation50KG::OriginalUiRiderBinderAnimation50KG() noexcept=default;
OriginalUiRiderBinderAnimation50KG::~OriginalUiRiderBinderAnimation50KG(){
    // Backend explicitly destroys local candidates and detached helpers. An
    // invariant violation must preserve both the wrapper and its exact token.
    if(storage_&&storage_->phase!=Storage::Phase::unowned)(void)storage_.release();
}
bool OriginalUiRiderBinderAnimation50KG::initialize(
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> assets,
    OriginalUiRiderMaterialLifetime50KG& ledger,const OriginalUiRiderMaterialFootprint50KG& footprint,
    OriginalUiRiderMaterialLifetimeDiagnostic50KG& d) noexcept {
    d={};
    if(storage_||!assets||!assets->loaded()||!assets->animation()||!assets->pass()||
       assets->animation()->frames.size()!=10||
       footprint.kind()!=OriginalUiRiderMaterialOwnerKind50KG::binderHelper||
       footprint.source()!=assets->pass()->identity){d.error=OriginalUiRiderMaterialLifetimeError50KG::footprint;return false;}
    if(!ledger.canAcquire(footprint,ledger.sequence(),d))return false;
    OriginalUiTextureAnimationConfig50KG config;config.framesPerSecond=16.f;
    config.reverseLoopOnEnd=false;config.loopOnEndToFrame=0;
    // InitialFrame0, Looping=true are actual constructor defaults, not overrides.
    try {
        auto next=std::make_unique<Storage>();
        if(!initializeOriginalUiTextureAnimation50KG(next->state,config,10)){
            d.error=OriginalUiRiderMaterialLifetimeError50KG::input;return false;
        }
        next->assets=std::move(assets);
        OriginalUiRiderMaterialMutation50KG mutation;
        // This helper's complete mutable wrapper now exists. Its token is
        // local to this candidate before backend membership can be published.
        if(!ledger.acquire(footprint,ledger.sequence(),next->token,mutation,d)){
            next->assets.reset();next->state={};return false;
        }
        next->ledger=&ledger;next->footprint=footprint;next->phase=Storage::Phase::live;
        storage_=std::move(next);return true;
    }catch(...){d.error=OriginalUiRiderMaterialLifetimeError50KG::allocation;return false;}
}
bool OriginalUiRiderBinderAnimation50KG::destroy(OriginalUiRiderMaterialLifetimeDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(s.phase==Storage::Phase::unowned){storage_.reset();return true;}
    if(!s.ledger||!s.ledger->owns(s.token)){d.error=OriginalUiRiderMaterialLifetimeError50KG::invariant;return false;}
    if(!s.ledger->canRelease(s.token,s.ledger->sequence(),d))return false;
    if(s.phase==Storage::Phase::live){
        // Retained immutable assets are not owners. Invalidate this actual
        // helper's material wrapper and unique animation state FIRST.
        s.assets.reset();s.state={};s.phase=Storage::Phase::invalidated;
    }
    OriginalUiRiderMaterialMutation50KG mutation;
    if(!s.ledger->release(s.token,s.ledger->sequence(),mutation,d))return false;
    s.token={};s.footprint={};s.ledger=nullptr;s.phase=Storage::Phase::unowned;
    storage_.reset();return true;
}
bool OriginalUiRiderBinderAnimation50KG::initialized()const noexcept {
    return storage_&&storage_->phase==Storage::Phase::live&&bool(storage_->assets)&&
        storage_->ledger&&storage_->ledger->owns(storage_->token);
}
bool OriginalUiRiderBinderAnimation50KG::bind(std::uint32_t frame,
    OriginalUiRiderBinderFrame50KG& out)const noexcept {
    if(!initialized())return false;const auto& assets=storage_->assets;
    if(!assets->loaded()||!assets->animation()||
       frame>=assets->animation()->frames.size()||!assets->images())return false;
    const auto* decoded=assets->images()->decodedTexture(frame);const auto* pass=assets->pass();
    if(!decoded||!pass)return false;
    const auto& range=assets->animation()->frames[frame];
    out={pass,assets->images(),decoded,frame,range.pstOffset,range.pstBytes};return true;
}
bool OriginalUiRiderBinderAnimation50KG::current(OriginalUiRiderBinderFrame50KG& out)const noexcept {
    if(!initialized())return false;
    std::uint32_t frame=0;if(!selectOriginalUiTextureAnimationFrame50KG(storage_->state,frame))return false;
    return bind(frame,out);
}
bool OriginalUiRiderBinderAnimation50KG::advanceAfterGeometry(std::uint32_t milliseconds,
    OriginalUiRiderBinderFrame50KG& out) noexcept {
    if(!initialized())return false;
    auto next=storage_->state;std::uint32_t frame=0;OriginalUiRiderBinderFrame50KG selected;
    if(!advanceOriginalUiTextureAnimation50KG(next,milliseconds,frame)||!bind(frame,selected))return false;
    storage_->state=next;out=selected;return true;
}
}
