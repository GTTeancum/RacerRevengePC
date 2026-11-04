#pragma once
#include <new>
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceDefaultObserverNative()const noexcept {
    if(sourceDefaultObserverPhase_==SourceDefaultObserverPhase::unreached)
        return !sourceDefaultObserver_&&!sourceDefaultObserverOuterReached_&&!sourceDefaultObserverLease_&&!sourceDefaultObserverPublished_;
    if(sourceDefaultObserverPhase_==SourceDefaultObserverPhase::nativeClosed)
        return !sourceDefaultObserver_&&!sourceDefaultObserverLease_;
    if(!worldApplication_||!sourceTopologyMode_||sourceTopologyWithdrawn_||particlePredecessorAttempted_||
       topologyAttempted_||topologyConstructed_||nativeDefaultObserver443148_||nativeEnvironmentAliases273174_)return false;
    //272F28 is a historical read. Later genuine1B4E20 or274708 may write35C=1
    //without rewriting that completed call's input receipt.
    if(sourceParticleEnvironment35CWritten_){
        if(!sourceParticlePredecessorEntered_||!sourceParticleVisibilityWasNull_||!*sourceParticleVisibilityWasNull_||
           !sourceParticleSelectorNone_||*sourceParticleSelectorNone_||!sourceFogCallReturnV0_||
           *sourceFogCallReturnV0_!=1||useEnvironment35C_!=1||!sourceDefaultEnvironment35C_||*sourceDefaultEnvironment35C_!=0)return false;
    }else if(sourceWaterFactory274570_.environment35CWritten){
        const auto& h=sourceWaterFactory274570_;
        if(!h.entered||h.existsV0!=1u||h.visibilityWasNull!=true||!h.visibilityFallbackEntered||
           !sourcePostSkyTail_.water[0].dispatched||sourcePostSkyTail_.water[0].target!=0x274570u||
           sourcePostSkyTail_.water[0].equalsNone!=false||useEnvironment35C_!=1||
           !sourceDefaultEnvironment35C_||*sourceDefaultEnvironment35C_!=0)return false;
    }else if(sourceDefaultEnvironment35C_&&*sourceDefaultEnvironment35C_!=useEnvironment35C_)return false;
    if(sourceDefaultObserverPublished_&&worldApplication_->defaultZoneObserver443148_!=sourceDefaultObserver_.get())return false;
    if(sourceDefaultObserverLease_){
        if(!sourceDefaultObserver_)return false;
        const auto& shared=worldApplication_->garageObserverShared_;
        if(sourceDefaultObserverCtorReturned_){if(!shared.ownsReturnedSourceObserver(sourceDefaultObserver_.get()))return false;}
        else if(!shared.sourceObserverCurrent280890(sourceDefaultObserver_.get()))return false;
    }
    if(!sourceDefaultObserver_)return !sourceDefaultObserverLease_&&!sourceDefaultObserverCtorReturned_&&
        !sourceDefaultObserverChildStored_&&!sourceDefaultObserverMethodStored_;
    const auto& o=*sourceDefaultObserver_;
    if(o.unknown18To1C[0]||o.unknown18To1C[1]||o.scalarKnownMask>63)return false;
    if(o.vectorKnown&&o.vector20!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})return false;
    if((o.scalarKnownMask&1u)&&o.next00)return false;
    if((o.scalarKnownMask&2u)&&!sourceDefaultObserverChildStored_&&o.child04)return false;
    if((o.scalarKnownMask&4u)&&!sourceDefaultObserverMethodStored_&&o.method08)return false;
    if((o.scalarKnownMask&8u)&&o.field0C)return false;
    if((o.scalarKnownMask&16u)&&o.field10)return false;
    if((o.scalarKnownMask&32u)&&o.field14)return false;
    if(sourceDefaultObserverCtorReturned_&&(!o.vectorKnown||o.scalarKnownMask!=63||!sourceDefaultObserverLease_))return false;
    if(sourceDefaultObserverChildStored_&&(!sourceTopologyReturnV0_||*sourceTopologyReturnV0_!=1))return false;
    if(sourceDefaultObserverChildStored_&&(!sourceCollision4C4_||!*sourceCollision4C4_||sourceCollisionPublished_<1||!sourceSearchNodes_[1][0].owner||
       o.child04!=&sourceSearchNodes_[1][0].owner->entity))return false;
    if(sourceDefaultObserverMethodStored_&&(!sourceDefaultObserverMethodRead_||o.method08!=*sourceDefaultObserverMethodRead_))return false;
    return true;
}
bool Owner::abortSourceDefaultObserverNative(std::uint32_t& pc)noexcept {
    for(const auto& f:sourceModelObservers_)if(f&&!f->withdrawn)return false;
    if(!preflightSourceDefaultObserverNative())return false;
    if(sourceDefaultObserverPhase_==SourceDefaultObserverPhase::unreached||sourceDefaultObserverPhase_==SourceDefaultObserverPhase::nativeClosed)return true;
    //Native withdrawal before any referenced source nodes retire, not guest destructor replay.
    if(sourceDefaultObserverLease_){
        auto& shared=worldApplication_->garageObserverShared_;
        if(sourceDefaultObserverCtorReturned_){if(!shared.retireReturnedSourceObserverNative(sourceDefaultObserver_.get()))return false;}
        else if(!shared.abortSourceObserver280890(sourceDefaultObserver_.get()))return false;
    }
    sourceDefaultObserverLease_=false;
    if(sourceDefaultObserverPublished_)worldApplication_->defaultZoneObserver443148_=nullptr;
    sourceDefaultObserver_.reset();sourceDefaultObserverPhase_=SourceDefaultObserverPhase::nativeClosed;
    return true;
}
bool Owner::continueSourceDefaultObserver272F28(std::uint32_t& pc)noexcept {
    pc=sourceDefaultObserverPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceDefaultObserverPhase_!=SourceDefaultObserverPhase::unreached||
       sourceTopologyPhase_!=SourceTopologyPhase::methodsReturned||sourceTopologyPC_!=0x272f28||sourceTrackPC_!=0x272f28||
       sourceMethodArraysPhase_!=SourceMethodArraysPhase::returned||sourceMethodArraysPC_!=0x272f28||
       sourceSceneInfoPhase_!=SourceSceneInfoPhase::returnedPrefix||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||sourceLoadApplication_->entities()!=worldApplication_||
       particlePredecessorAttempted_||topologyAttempted_||topologyConstructed_||nativeDefaultObserver443148_||
       nativeEnvironmentAliases273174_||!worldApplication_||worldApplication_->defaultZoneObserver443148_||
       !preflightSourceWorldTopologyCountNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceDefaultObserverPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceDefaultObserverPhase_=SourceDefaultObserverPhase::bounded;return false;};
    sourceDefaultObserverPhase_=SourceDefaultObserverPhase::entered;
    at(0x272f28);sourceDefaultEnvironment35C_=useEnvironment35C_; //Same actual270A2C producer, not authoring metadata.
    at(0x272f2c);
    if(*sourceDefaultEnvironment35C_!=0)return stop(0x272f3c); //Actual v100=272FC0 remains unjoined; no legacy adoption.
    at(0x272f48);sourceDefaultObserver_.reset(new(std::nothrow) SourceDefaultObserver280890);
    sourceDefaultObserverOuterReached_=true;
    if(sourceDefaultObserver_){
        auto& o=*sourceDefaultObserver_;auto& shared=worldApplication_->garageObserverShared_;
        //Native owner-ledger admission precedes the constructor's first represented effect.
        if(!shared.beginSourceObserver280890(&o))return stop(0);
        sourceDefaultObserverLease_=true;
        at(0x2808a4);o.vector20={{0,0,0,0x3f800000u}};o.vectorKnown=true; //SQ C2 VF0.
        if(!shared.sourceObserverAllocator2808AC(&o,pc))return stop(pc);
        at(0x2808e0);o.next00=nullptr;o.scalarKnownMask|=1u;
        at(0x2808e4);o.child04=nullptr;o.scalarKnownMask|=2u;
        at(0x2808e8);o.method08=nullptr;o.scalarKnownMask|=4u;
        at(0x2808ec);o.field0C=0;o.scalarKnownMask|=8u;
        at(0x2808f0);o.field10=0;o.scalarKnownMask|=16u;
        at(0x2808f4);o.field14=0;o.scalarKnownMask|=32u;
        if(!shared.sourceObserverIncrement28090C(&o,pc))return stop(pc);
        //The exact280918 return completes the constructor frame, not the observer
        //lifetime. Keep this SAME owner/pool/count alive for later type1 lights.
        at(0x280918);if(!shared.finishSourceObserver280918(&o))return stop(pc);
        sourceDefaultObserverCtorReturned_=true;
    }
    at(0x272f64);worldApplication_->defaultZoneObserver443148_=sourceDefaultObserver_.get();
    sourceDefaultObserverPublished_=true; //OuterNULL skips ctor but still publishes NULL.
    at(0x272f68);if(!sourceCollision4C4_)return stop(pc);auto* collision=*sourceCollision4C4_;
    at(0x272f70);auto* observer=worldApplication_->defaultZoneObserver443148_;
    at(0x272f74);sourceTopologyReturnV0_=1; //Actual scalar survives remaining call-free stores/epilogue.
    at(0x272f78);if(!collision||sourceCollisionPublished_<1||!sourceSearchNodes_[1][0].owner||
        collision[0]!=sourceSearchNodes_[1][0].owner.get())return stop(pc);
    const auto* child=&collision[0]->entity;
    at(0x272f80);if(!observer||observer!=sourceDefaultObserver_.get())return stop(pc);
    sourceDefaultObserver_->child04=child;sourceDefaultObserverChildStored_=true; //Raw borrowed pointer, no retain.
    at(0x272f84);const auto* method=worldApplication_->visibility443A98_;sourceDefaultObserverMethodRead_=method; //Actual same App global, including NULL.
    at(0x272f8c);observer=worldApplication_->defaultZoneObserver443148_;
    at(0x272f90);if(!observer||observer!=sourceDefaultObserver_.get())return stop(pc);
    sourceDefaultObserver_->method08=method;sourceDefaultObserverMethodStored_=true;
    sourceDefaultObserverPhase_=SourceDefaultObserverPhase::returned;at(0x272f94);
    return preflightSourceDefaultObserverNative(); //No topology return or readiness claim.
}
}
