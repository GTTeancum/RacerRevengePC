#pragma once
#include "water_height50jc.h"
#include <array>
#include <limits>

namespace rr::game {
// Explicit native composition policy. Retail publishes origins behind its
// water renderer's view/frustum and visibility gates. This native one-view
// policy publishes the same arithmetic once after each actual rendered frame.
// It is independent of racer58/human count and claims no retail scheduler parity.
enum class WaterPresentationPolicy50JC : std::uint8_t {
    Disabled,OneNativeViewEveryRenderedFrame
};
struct WaterPresentationOwner50JC {
    WaterPresentationPolicy50JC policy=WaterPresentationPolicy50JC::Disabled;
    std::uint64_t lastRenderedFrame=0,publishedFrames=0;
    bool hasRenderedFrame=false;
};
struct WaterPresentationSignature50JC {
    WaterSignature50JC* query=nullptr;
    const float* authoredPosition40=nullptr; // Actual three-element source position.
    const float* scrollB0=nullptr;
    const float* scrollB4=nullptr;
    bool sourceFieldsBound=false;
};
struct WaterPresentationInput50JC {
    // Receipt from the actual native renderer's completed frame, never the
    // simulation-step counter, wall clock, capture request or AI dispatch count.
    std::uint64_t renderedFrame=0;
    std::uint32_t viewCount=0;
    bool actualNativeRenderCompleted=false;
    const std::uint32_t* renderFoam3834F0=nullptr;
    const WaterPresentationSignature50JC* signatures=nullptr;
    std::size_t count=0;
    bool completeSignatureSetBound=false;
};
enum class WaterPresentationStatus50JC : std::uint8_t {
    Disabled,DuplicateFrame,Published,Rejected
};
struct WaterPresentationResult50JC {
    WaterPresentationStatus50JC status=WaterPresentationStatus50JC::Rejected;
    WaterHeightResult50JC diagnostic{};
};
// Atomic native presentation publication only. It does not report completion
// of original2BEBD0's rendering/effect continuation. The exact origin writes
// at2BEC50/60 are retained; render-foam/channel4 skip matches2BEBE4..2BEC04.
// Bind this only behind the explicit native water opt-in. Every binding and
// source array must remain valid and stable for the call; output queries may
// not alias one another. Rejecting leaves origins and frame receipt unchanged.
inline WaterPresentationResult50JC presentNativeWaterView50JC(
    WaterPresentationOwner50JC& owner,const WaterPresentationInput50JC& in) noexcept {
    using namespace water_height_detail50jc;
    using P=WaterPresentationStatus50JC;
    if(owner.policy==WaterPresentationPolicy50JC::Disabled)
        return {P::Disabled,fail(Status::Complete,0)};
    if(owner.policy!=WaterPresentationPolicy50JC::OneNativeViewEveryRenderedFrame||
       !in.actualNativeRenderCompleted||in.viewCount!=1u||
       !in.completeSignatureSetBound||in.count>32u||
       (in.count&&!in.signatures)||!in.renderFoam3834F0)
        return {P::Rejected,fail(Status::Owner,0x2b5578u)};
    if(owner.hasRenderedFrame){
        if(in.renderedFrame<owner.lastRenderedFrame)
            return {P::Rejected,fail(Status::Owner,0x2b4cc8u)};
        if(in.renderedFrame==owner.lastRenderedFrame)
            return {P::DuplicateFrame,fail(Status::Complete,0)};
    }
    if(owner.publishedFrames==std::numeric_limits<std::uint64_t>::max())
        return {P::Rejected,fail(Status::Unsupported,0)};
    if(std::fegetround()!=FE_TONEAREST)
        return {P::Rejected,fail(Status::Arithmetic,0x2bec4cu)};
    struct Origin {float x=0,z=0;bool write=false;};
    std::array<Origin,32> proposed{};Math m;
    for(std::size_t i=0;i<in.count;++i){const auto& s=in.signatures[i];
        if(!s.sourceFieldsBound||!s.query||!s.query->bound)
            return {P::Rejected,fail(Status::Owner,0x2bebd0u,i)};
        for(std::size_t prior=0;prior<i;++prior)
            if(in.signatures[prior].query==s.query)
                return {P::Rejected,fail(Status::Owner,0x2b5578u,i)};
        if(s.query->channelD8>4u)
            return {P::Rejected,fail(Status::Unsupported,0x2bebf8u,i)};
        if(!*in.renderFoam3834F0&&s.query->channelD8==4u)continue;
        if(!s.authoredPosition40||!s.scrollB0||!s.scrollB4)
            return {P::Rejected,fail(Status::Owner,0x2bec44u,i)};
        auto& p=proposed[i];
        p.x=m.add(s.authoredPosition40[0],*s.scrollB0,0x2bec4cu);
        p.z=m.add(s.authoredPosition40[2],*s.scrollB4,0x2bec5cu);
        if(m.badPC)return {P::Rejected,arithmetic(m)};
        p.write=true;
    }
    for(std::size_t i=0;i<in.count;++i)if(proposed[i].write){
        auto& q=*in.signatures[i].query;
        q.originXC0=proposed[i].x;q.originZC4=proposed[i].z;
        // Preserve the scheduling distinction in the consumed query metadata.
        q.originPublication=WaterOriginPublication50JC::NativeOneViewPresentation50JC;
    }
    owner.hasRenderedFrame=true;owner.lastRenderedFrame=in.renderedFrame;
    ++owner.publishedFrames;
    return {P::Published,fail(Status::Complete,0x2bec60u)};
}
} // namespace rr::game
