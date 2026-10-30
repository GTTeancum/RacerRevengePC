#pragma once
namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::applySourceDirectionalColor1498A0(
    const ::ra_cold_aggregate50LA::Owner& world,std::size_t zone,Diagnostic& d)noexcept {
    d={};
    //No caller exists yet. Actual loop exit, S0 reset, sentinel comparison and
    //273BF8 invocation must be supplied by a future genuine source caller.
    if((zone!=1&&zone!=2&&zone!=8)||world.nativeScbThread_!=std::this_thread::get_id()||
       world.closed_||world.terminal_||world.nativeScbRevoked_||
       world.sourceLightPopulationPC_!=0x273bf8||!world.sourceLightPopulationS0_||
       *world.sourceLightPopulationS0_!=static_cast<std::int32_t>(zone)||
       !world.sourceLightPopulationCount_||*world.sourceLightPopulationCount_!=185||
       !world.preflightSourceLightPopulationNative()||!preflightSourceRaZonesNative(world)||
       !raZones_||raZones_->sourceNativeClosed||world.sourceTopologyLightManager_.get()!=this||
       world.sourceLoadApplication_!=issuer_||issuer_->sourceLightManager426F60Owner_.get()!=this||
       !raZones_->sourceSecondRegistered||raZones_->sourceSecondWithdrawn||
       count94_!=185||registryWritten_!=185||!registry9C_||!directional44_||count90_!=10||
       (raZones_->sourceArrayWithdrawnMask&16u)||raZones_->sourceDirectionalTailRGB[zone]||
       !world.sourceFirstWrapperReturned_||!world.sourceSecondWrapperReturned_||
       !world.sourceFirstDescriptorDisposed_||!world.sourceSecondDescriptorDisposed_||
       world.sourceTypedLights_.size()!=183)return fail(d,Status::phase,0x273bf8);
    const auto* list=world.nativeLightList3A8_.get();
    if(!list||!list->sourceMode||!list->sourceAuthorityEnrolled||!list->wrappers38||
       list->count30!=185||list->sourceWrappersConstructed!=185||list->loaded!=185||
       !list->zoneCount34||*list->zoneCount34!=10||list->colorsLoaded!=10||!list->colors3C)
        return fail(d,Status::identity,0x273bd4);
    for(std::size_t i=0;i<185;++i){const auto& wrapper=list->wrappers38[i];
        if(wrapper.sourceDescriptor04||!wrapper.live00||wrapper.live00!=registry9C_[i]||
           registryPublished_[i]!=wrapper.live00||wrapper.live00->parent1C!=&list->entity)
            return fail(d,Status::phase,0x273bbc);
        if(i>=2){const auto& f=world.sourceTypedLights_[i-2];
            if(!f||f->ordinal!=i||f->nativeClosed||!f->registrationReturned||f->registryWithdrawn||
               !f->light||!f->light->attached||!f->light->registered||f->descriptor||
               f->argumentsBorrowed||f->wrapper!=&wrapper||&f->light->entity!=wrapper.live00)
                return fail(d,Status::phase,0x273bbc);}}
    if((zone==2&&!raZones_->sourceDirectionalTailRGB[1])||
       (zone==8&&(!raZones_->sourceDirectionalTailRGB[1]||!raZones_->sourceDirectionalTailRGB[2])))
        return fail(d,Status::phase,0x273bf8);
    const auto& lanes=list->colors3C[zone].lanes;
    std::array<std::uint32_t,3> rgb{};
    for(unsigned k=0;k<3;++k){if(!lanes[k]||*lanes[k]!=20.f)return fail(d,Status::identity,0x273bdc);
        std::memcpy(&rgb[k],&*lanes[k],sizeof(rgb[k]));}
    //Actual1498A0 negative/out-of-range/NULL44 early-return cases are excluded
    //by this supplied caller's checked0..9/real44 envelope, never fabricated.
    constexpr std::uint32_t stores[]={0x1498d0,0x1498e0,0x1498f0};
    for(unsigned k=0;k<3;++k){d.sourcePC=stores[k];directional44_[zone][k]=rgb[k];}
    raZones_->sourceDirectionalTailRGB[zone]=rgb;
    //No W write, revision, vector30/default/A0 mutation, loop advance or World return.
    d={Status::complete,0x1498f4};return true;
}
}
