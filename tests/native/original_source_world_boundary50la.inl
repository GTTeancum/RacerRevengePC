#pragma once
// Complete Config/aggregate/base definitions required. No public entrypoint.
namespace ra_cold_aggregate50LA {
inline bool Owner::allocateLoadSourceWorldBoundary272970(std::uint32_t& pc)noexcept {
    pc=0; // Native admission refusal has no fabricated original allocator return.
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceBoundaryPhase_!=SourceBoundaryPhase::unreached||sourceBoundary348_||
       !sourceWorldBoundaryInvocationCurrent()||!sourceWorldConfig_||
       !worldBoundary348_.has_value()||*worldBoundary348_!=nullptr||
       worldAttempted_||topologyAttempted_||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_)return false;
    std::array<std::uint32_t,4> origin,forward;
    if(!copySourceWorldStartDefaults(origin,forward))return false;
    sourceBoundaryPhase_=SourceBoundaryPhase::entered;
    const auto at=[&](std::uint32_t p)noexcept{pc=sourceBoundaryPC_=p;};
    const auto bounded=[&](std::uint32_t p)noexcept{at(p);sourceBoundaryPhase_=SourceBoundaryPhase::bounded;return false;};
    at(0x272980);
    try {sourceBoundary348_.reset(new SourceWorldBoundary348);}catch(...){sourceBoundary348_.reset();}
    if(sourceBoundary348_){auto& b=*sourceBoundary348_;
        at(0x275b88);b.vtable20=0x3d2ba0;
        // SQ C2 vf0 stores architectural(0,0,0,1), not BSS or host defaults.
        at(0x275b8c);b.position00={{0u,0u,0u,0x3f800000u}};
        at(0x275b94);b.direction10={{0u,0u,0u,0x3f800000u}};
        at(0x275ba4);b.position00=origin;
        at(0x275bb4);b.direction10=forward;
    }
    at(0x272998);worldBoundary348_=sourceBoundary348_.get();sourceBoundaryPublished_=true;
    if(!sourceBoundary348_)return bounded(0x2729a0); // Actual NULL vtable dereference.
    bool returned=false;a::OriginalStandardTextureDiagnostic50LA d;
    at(0x275be0);
    if(!sourceWorldConfig_->selectSceneInfo157D00(returned,d))
        return bounded(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    if(!returned){
        at(0x2729b8);sourceBoundary348_.reset(); // Exact source free, no Entity/destructor call.
        at(0x2729c0);worldBoundary348_=nullptr;
        sourceBoundaryPhase_=SourceBoundaryPhase::returnedZero;
        at(0x270a58);return true; // Caller ignores original v0=0.
    }
    at(0x275c0c);
    if(!sourceWorldConfig_->readMissingVector158C00("DefaultStartPosition",origin,
        sourceBoundary348_->position00,returned,d))return bounded(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    // Source deliberately ignores getter v0; a missing key writes defaults.
    at(0x275c28);
    if(!sourceWorldConfig_->readMissingVector158C00("DefaultStartDirection",forward,
        sourceBoundary348_->direction10,returned,d))return bounded(d.sourcePC?d.sourcePC:sourceWorldConfig_->sourcePC_);
    sourceBoundaryPhase_=SourceBoundaryPhase::returnedOne;
    at(0x270a58);return true; // Only caller continuation; Environment not allocated.
}
inline bool Owner::preflightSourceWorldBoundaryNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceBoundaryPhase_==SourceBoundaryPhase::nativeClosed)return !sourceBoundary348_;
    if(sourceBoundaryPhase_==SourceBoundaryPhase::unreached)return !sourceBoundary348_&&!sourceBoundaryPublished_;
    // Exact ownership, not live Config or source caller currentness: partial
    // Config failures/terminal state must remain cleanable without replay.
    if(!sourceBoundaryPublished_||!worldBoundary348_.has_value()||
       *worldBoundary348_!=sourceBoundary348_.get()||topologyAttempted_||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    return true;
}
inline bool Owner::abortSourceWorldBoundaryNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldBoundaryNative())return false;
    if(sourceBoundaryPhase_==SourceBoundaryPhase::nativeClosed)return true;
    if(sourceBoundaryPhase_!=SourceBoundaryPhase::unreached){
        sourceBoundary348_.reset();
        // Native disposal is not26FDEC/26FDF4 source destruction: withdraw
        // knowledge rather than replace an historical source pointer withNULL.
        worldBoundary348_.reset();
    }
    sourceBoundaryPhase_=SourceBoundaryPhase::nativeClosed;return true;
}
} // namespace ra_cold_aggregate50LA
