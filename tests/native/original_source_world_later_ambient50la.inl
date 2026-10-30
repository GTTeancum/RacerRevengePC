#pragma once
//Conditional typed Ambient path. No loop caller or ordinal admission is issued.
namespace ra_cold_aggregate50LA {
bool Owner::continueSourceLaterAmbient146F60(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !sourceTopologyLightManager_||!preflightSourceTypedLightsNative()||f.ordinal<2||
       f.ordinal-2>=sourceTypedLights_.size()||sourceTypedLights_[f.ordinal-2].get()!=&f||
       !sourceLightPopulationS0_||*sourceLightPopulationS0_!=static_cast<std::int32_t>(f.ordinal)||
       !f.light||!f.descriptor||f.descriptor!=f.wrapper->sourceDescriptor04.get()||
       !f.enrolled||!f.returned||!f.published||!f.argumentsBorrowed||!f.ordinalStack08||
       f.wrapper->live00!=&f.light->entity||f.pc!=0x277e84||f.laterAmbientConfigEntered||
       f.spotlightConfigEntered||f.light->colorsWritten)return false;
    auto& d=*f.descriptor;auto& l=*f.light;
    //Exact supplied later Ambient shape. Constructor never initialized cones;
    //do not inspect those unused descriptor lanes or invent vector allocations.
    if(!d.loaded||d.type00!=4||d.position14||d.look18||d.attenuation1C||d.range24||
       d.knownFlags28!=0x7f||(d.flags28&0x7f)!=4||
       d.color04!=std::array<std::uint32_t,4>{{0x41c80000u,0x41c80000u,0x41c80000u,0x437f0000u}}||
       (d.zone20!=0&&d.zone20!=1&&d.zone20!=7))return false;
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};
    const auto stop=[&](std::uint32_t p){at(p);return false;};
    f.laterAmbientConfigEntered=true;
    at(0x146fc0);l.type34=4;at(0x146fd8);l.ordinalC8=*f.ordinalStack08;
    //Actual25/2 and255/2 are exact; no host numeric probe or general parser.
    at(0x146c84);l.ambient5C={{12.5f,12.5f,12.5f,127.5f}};
    at(0x146cc0);l.diffuse3C={};l.specular4C={};l.colorsWritten=true;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic dgn;
    at(0x146e30);if(!sourceTopologyLightManager_->incrementSourceTyped149300(*this,f.ordinal,0,dgn))return stop(dgn.sourcePC);
    l.revisionWritten=true;
    at(0x146e3c);std::uint32_t luminance=0;if(!sourceTypedLuminance146E60(f,luminance))return stop(0x146e60);
    at(0x146e44);f.luminance=luminance;l.luminanceCC=luminance;
    at(0x147028); //Range0 and NULL position/look skip setters and revisions.
    at(0x147424);l.attenuation8C[0]=1.f;
    at(0x147428);l.attenuation8C[1]=0.f;
    at(0x14742c);l.attenuation8C[2]=0.f;
    at(0x14743c); //Type4 retains constructor mode3; no mode store.
    at(0x147494);l.configured=true;f.configured=true;
    at(0x277fcc);if(!sourceTopologyLightManager_->appendSourceLaterAmbient148F30(*this,f.ordinal,dgn))return stop(dgn.sourcePC);
    at(0x277fd4);return true; //Wrapper cleanup/attachment/next ordinal still unissued.
}
}
namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::appendSourceLaterAmbient148F30(
    ::ra_cold_aggregate50LA::Owner& world,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};if(ordinal<2||ordinal>=185||ordinal-2>=world.sourceTypedLights_.size()||
       !world.preflightSourceTypedLightsNative()||!preflightSourceRaZonesNative(world)||
       !raZones_||!raZones_->sourceReturned||raZones_->sourceNativeClosed||
       !raZones_->sourceSecondRegistered||raZones_->sourceSecondWithdrawn||
       world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_||
       issuer_->sourceLightManager426F60Owner_.get()!=this)return fail(d,Status::phase,0x148f30);
    auto& f=*world.sourceTypedLights_[ordinal-2];
    if(!f.laterAmbientConfigEntered||!f.light||!f.configured||f.light->type34!=4||!f.descriptor||f.descriptor->type00!=4||
       !f.zone||*f.zone!=f.descriptor->zone20||*f.zone<0||static_cast<std::uint32_t>(*f.zone)>=count90_||
       f.pc!=0x277fcc||f.registrationEntered||f.registryIndex||f.observerEntered||
       !ambient60_||!ambient80_||count90_!=10||!registry9C_||registryFailed_||raRegistryGrowthFailed_||
       !capacity98_||capacity98_>190||capacity98_%10||registryWritten_!=count94_)
        return fail(d,Status::phase,0x148f30);
    auto& l=*f.light;const auto zone=static_cast<std::size_t>(*f.zone);
    const auto at=[&](std::uint32_t pc){f.pc=pc;d.sourcePC=pc;};
    const auto stop=[&](std::uint32_t pc){at(pc);return fail(d,Status::phase,pc);};
    at(0x148f30);f.registrationEntered=true;f.registryIndex=count94_;f.registryManager=this;
    constexpr std::uint32_t stores[3][4]={{0x148f7c,0x148f88,0x148f8c,0x148f90},
        {0x148fa0,0x148fac,0x148fb0,0x148fb4},{0x148fc4,0x148fd0,0x148fd4,0x148fd8}};
    //All destinations preflighted above; no callback/allocation between stores.
    for(unsigned k=0;k<3;++k){std::uint32_t word=0;std::memcpy(&word,&l.ambient5C[k],sizeof(word));
        at(stores[k][0]);ambient80_[zone][k]=word;
        at(stores[k][1]);ambient60_[zone][k]=word;
        at(stores[k][2]);default70_[k]=word;
        at(stores[k][3]);default50_[k]=word;}
    at(0x148fe4);ambient80_[zone][3]=0x42800000u;
    at(0x148ff0);ambient60_[zone][3]=0x42800000u;
    at(0x148ff4);default70_[3]=0x42800000u;
    at(0x148ffc);default50_[3]=0x42800000u; //Branch delay executes for all zones.
    if(!zone){for(std::size_t i=0;i<count90_;++i){
        at(0x14901c);ambient60_[i]=default50_;
        at(0x149030);ambient80_[i]=default50_;}}
    f.laterAmbientStoresReturned=true;
    at(0x149048); //Type4 skips observer1474A0.
    at(0x1490c0); //Type4 skips vector30, then field40/directional44 and A0.
    at(0x149188);
    if(capacity98_<count94_+1u){at(0x149194);f.registryGrowthEntered=true;
        if(!growRaRegistry149194(d))return stop(d.sourcePC);
        at(0x1491ac);f.registryGrowthReturned=true;}
    if(!registry9C_||count94_!=*f.registryIndex||capacity98_<=count94_)return stop(0x1491b4);
    at(0x1491c0);registry9C_[*f.registryIndex]=&l.entity;registryPublished_[*f.registryIndex]=&l.entity;
    registryWritten_=*f.registryIndex+1;f.registryOwner=&l.entity;f.registrySlotPublished=true;
    at(0x1491cc);++count94_;f.registryCountStored=true;l.registered=true;
    at(0x1491e0);f.registrationReturned=true;d={Status::complete,0x1491e0};return true;
}
}
