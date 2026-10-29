#pragma once
#include <new>
namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::incrementSourceSecond149300(::ra_cold_aggregate50LA::Owner& world,unsigned stage,Diagnostic& d)noexcept {
    d={};
    if(stage>=3||!preflightSourceRaZonesNative(world)||!raZones_||!raZones_->sourceReturned||raZones_->sourceNativeClosed||
       !raZones_->sourceAmbientRegistered||raZones_->sourceAmbientWithdrawn||raZones_->sourceSecondRevisionBefore[stage]||
       !world.preflightSourceSecondLightNative()||!world.sourceSecondLight_||
       world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_)
        return fail(d,Status::phase,0x149300);
    auto& l=*world.sourceSecondLight_;
    if(stage==0){if(!l.colorsWritten||l.revisionWritten)return fail(d,Status::phase,0x149300);}
    else if(stage==1){if(!l.revisionWritten||!world.sourceSecondLuminance_||!l.positionWritten||l.positionRevision)
        return fail(d,Status::phase,0x149300);}
    else if(!l.revisionWritten||!world.sourceSecondPositionCompared_||(l.positionWritten&&!l.positionRevision)||
            !l.lookWritten||l.lookRevision)return fail(d,Status::phase,0x149300);
    raZones_->sourceSecondRevisionBefore[stage]=field84_;
    ++field84_; //Same149304 unsigned32 revision; no new registry or readiness.
    d={Status::complete,0x149310};return true;
}

bool OriginalUiLightManagerOwner50KG::incrementSourceAmbient149300(::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!preflightSourceRaZonesNative(world)||!raZones_||!raZones_->sourceReturned||raZones_->sourceNativeClosed||
       raZones_->sourceAmbientRevisionBefore||!world.preflightSourceFirstLightNative()||!world.sourceFirstLight_||
       !world.sourceFirstLight_->colorsWritten||world.sourceFirstLight_->revisionWritten||
       world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_)
        return fail(d,Status::phase,0x149300);
    raZones_->sourceAmbientRevisionBefore=field84_;
    ++field84_; //Actual149304 unsigned32 wrap, not a reset or invented revision.
    d={Status::complete,0x149310};return true;
}
bool OriginalUiLightManagerOwner50KG::appendSourceAmbient148F30(::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!preflightSourceRaZonesNative(world)||!raZones_||!raZones_->sourceReturned||raZones_->sourceNativeClosed||
       raZones_->sourceAmbientAttempted||!raZones_->sourceAmbientRevisionBefore||!world.sourceFirstLight_||
       !world.preflightSourceFirstLightNative()||!world.sourceFirstLight_->configured||world.sourceFirstLight_->registered||
       !world.sourceLightPopulationDescriptor_||world.sourceLightPopulationDescriptor_->zone20||
       world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_)
        return fail(d,Status::phase,0x148f30);
    //Only constructor-derived zero registry admitted. Warm retained source
    //backing is not adopted, cleared or falsely described as capacity10.
    if(count94_||capacity98_||registry9C_||registryWritten_||registryFailed_||raRegistryGrowthFailed_)
        return fail(d,Status::phase,0x148f30); //Native admission boundary, before any callee effect.
    if(!ambient60_||!ambient80_||count90_!=10)return fail(d,Status::phase,0x148f7c);
    auto& r=*raZones_;auto& light=*world.sourceFirstLight_;
    r.sourceAmbientAttempted=true;
    constexpr Words ambient{{0x42480000u,0x42480000u,0x42480000u,0x42800000u}};
    //148F7C..149040 actual zone0 ambient writes and ten-row propagation.
    default50_=default70_=ambient;for(std::size_t i=0;i<10;++i)ambient60_[i]=ambient80_[i]=ambient;
    r.sourceAmbientColors=true;
    //149184 count+1;149188 signed capacity<count+1 is true for proven0/0. Actual149198
    //capacity=count+10 precedes3080C0(NULL,40);1491AC publishes even NULL.
    capacity98_=10;r.sourceAmbientAllocation=true;
    registry9C_.reset(new(std::nothrow) const C*[10]);r.sourceAmbientPublished=true;
    if(!registry9C_){registryFailed_=true;return fail(d,Status::allocation,0x1491c0);}
    registry9C_[0]=&light.entity;registryPublished_[0]=&light.entity;registryWritten_=1;
    ++count94_;r.sourceAmbient=&light.entity;r.sourceAmbientRegistered=true;light.registered=true;
    d={Status::complete,0x1491e0};return true;
}
bool OriginalUiLightManagerOwner50KG::withdrawSourceAmbientNative(::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!preflightSourceRaZonesNative(world))return fail(d,Status::phase,0x148110);
    if(!raZones_||!raZones_->sourceMode||raZones_->sourceWorldIdentity!=&world)return true;
    auto& r=*raZones_;if(r.sourceAmbientWithdrawn)return true;
    if(r.sourceAmbientRegistered){
        if(!world.sourceFirstLight_||r.sourceAmbient!=&world.sourceFirstLight_->entity||count94_!=1)
            return fail(d,Status::identity,0x1491f0);
        --count94_;world.sourceFirstLight_->registered=false;r.sourceAmbient=nullptr;
    }
    //Retain actual capacity, NULL failure, known dead slot and revision history.
    //This withdraws native lifetime only; no fake guest destructor/rollback.
    r.sourceAmbientWithdrawn=true;return true;
}
}
