#pragma once
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldPhysicsNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourcePhysicsPhase_==SourcePhysicsPhase::unreached)
        return !worldPhysics3D4_||!worldPhysics3D4_->sourceMode_;
    if(!worldPhysics3D4_)return !sourcePhysics3D4_||*sourcePhysics3D4_==nullptr;
    return worldPhysics3D4_->sourceMode_&&sourcePhysics3D4_&&
        *sourcePhysics3D4_==worldPhysics3D4_.get()&&worldPhysics3D4_->sourceWorld_==&worldEntity_&&
        !livePhysicsIndexAttempted_&&!livePhysicsIndexComplete_&&
        !runtimeCameraBorrows_&&!runtimeWorldBorrows_&&!constructorRacerWorldBorrows_&&
        worldPhysics3D4_->canRelease27C030();
}
inline bool Owner::abortSourceWorldPhysicsNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldPhysicsNative())return false;
    if(sourcePhysicsPhase_==SourcePhysicsPhase::unreached)return true;
    sourcePhysics3D4_.reset(); //Withdraw outer pointer knowledge BEFORE backing.
    //Exact same-thread preflight covers every release refusal; no intervening
    //callback or source mutation. Existing release owns actual acquired arrays.
    if(worldPhysics3D4_&&!worldPhysics3D4_->release27C030())std::terminate();
    worldPhysics3D4_.reset();sourcePhysicsPhase_=SourcePhysicsPhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldPhysics270CC4(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourcePhysicsPhase_!=SourcePhysicsPhase::unreached||!sourceWorldPhysicsInvocationCurrent()||
       worldPhysics3D4_||sourcePhysics3D4_||livePhysicsIndexAttempted_||livePhysicsIndexComplete_||
       !worldApplication_||!worldEntity_.sessionGeneration||!worldEntity_.reference00||
       !worldZones34C_||*worldZones34C_!=10||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    sourcePhysicsPhase_=SourcePhysicsPhase::entered;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourcePhysicsPC_=at;sourcePhysicsPhase_=SourcePhysicsPhase::bounded;return false;};
    pc=0x270cc4;
    using Physics=g::OriginalWorldPhysicsIndex50LA;
    try{worldPhysics3D4_.reset(new Physics(Physics::SourceTag{},worldEntity_));}catch(...){worldPhysics3D4_.reset();}
    pc=0x270cdc;sourcePhysics3D4_=worldPhysics3D4_.get(); //Genuine publication, includingNULL.
    pc=0x270ce0;const auto zones=static_cast<std::int32_t>(*worldZones34C_);
    pc=0x270ce4;
    if(!worldPhysics3D4_)return stop(0x27c124); //Positive10 before firstNULL04 load.
    if(!worldPhysics3D4_->prepareSource27C110(worldEntity_,zones,sourcePhysicsPrepareReturned_,pc))return stop(pc);
    //Original caller ignores27C110 v0. No legacy livePhysicsIndexComplete_
    //or Standard-only immutable model proof is created by this source prefix.
    sourcePhysicsPhase_=SourcePhysicsPhase::returned;pc=sourcePhysicsPC_=0x270cec;return true;
}
} //namespace ra_cold_aggregate50LA
