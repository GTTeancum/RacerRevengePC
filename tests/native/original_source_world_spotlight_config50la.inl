#pragma once
//Private conditional continuation, deliberately uncalled by the current loop.
//The real caller must already have enrolled/published this exact Light and
//borrowed its live descriptor arguments. This leaf creates no stage authority.
namespace ra_cold_aggregate50LA {
bool Owner::configureSourceSpotlight146F60(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    namespace n=g::directional_native_proposal;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !worldApplication_||!sourceTopologyLightManager_||!preflightSourceTypedLightsNative()||
       f.ordinal<2||f.ordinal-2>=sourceTypedLights_.size()||sourceTypedLights_[f.ordinal-2].get()!=&f||
       !sourceLightPopulationS0_||*sourceLightPopulationS0_!=static_cast<std::int32_t>(f.ordinal)||
       !f.light||!f.returned||!f.enrolled||!f.published||!f.argumentsBorrowed||!f.ordinalStack08||
       !f.descriptor||f.descriptor!=f.wrapper->sourceDescriptor04.get()||
       f.wrapper->live00!=&f.light->entity||f.spotlightConfigEntered||f.light->colorsWritten||
       f.pc!=0x277e84)return false;
    auto& d=*f.descriptor;auto& l=*f.light;
    //Current supplied Spotlight cohort only. No parser/name/hash substitutes.
    if(!d.loaded||d.type00!=3||!d.position14||!d.look18||!d.attenuation1C||
       !d.inner2C||!d.outer30||d.range24||d.knownFlags28!=0x7f||(d.flags28&0x7f)!=7||
       *d.attenuation1C!=n::Words{{0x3f800000u,0x358637bdu,0x360637bdu,0x3f800000u}}||
       std::fegetround()!=FE_TONEAREST)return false;
    for(auto v:d.color04)if(!n::detail::normalOrZeroWord(v))return false;
    for(auto v:*d.position14)if(!n::detail::normalOrZeroWord(v))return false;
    for(auto v:*d.look18)if(!n::detail::normalOrZeroWord(v))return false;
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};
    const auto stop=[&](std::uint32_t p){at(p);return false;};
    f.spotlightConfigEntered=true;
    at(0x146fc0);l.type34=3;at(0x146fd8);l.ordinalC8=*f.ordinalStack08;
    std::array<float,4> half{};
    //Four source DIV.S/stack stores, signed RGB retained. Strict native
    //binary32 adapter, not EE/FCSR parity. Refusal retains prior owner stores.
    constexpr std::uint32_t divPC[4]={0x146fe0,0x146fec,0x146ff8,0x147004};
    for(unsigned k=0;k<4;++k){at(divPC[k]);volatile float value=n::detail::fromWord(d.color04[k])/2.f;
        if(!n::detail::normalOrZero(value))return stop(pc);half[k]=value;}
    at(0x146d04);f.spotlightHalfColor=half;l.diffuse3C=l.specular4C=half;l.ambient5C={};l.colorsWritten=true;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic md;
    at(0x146e30);if(!sourceTopologyLightManager_->incrementSourceTyped149300(*this,f.ordinal,0,md))return stop(md.sourcePC);
    l.revisionWritten=true;
    at(0x146e3c);std::uint32_t luminance=0;if(!sourceTypedLuminance146E60(f,luminance))return stop(0x146e60);
    at(0x146e44);f.luminance=luminance;l.luminanceCC=luminance;
    at(0x147028); //Actual zero Range skips setter.
    const auto sameXYZ=[](const n::Words& a,const n::Words& b){for(unsigned k=0;k<3;++k)
        if(n::detail::fromWord(a[k])!=n::detail::fromWord(b[k]))return false;return true;};
    at(0x1470bc);f.positionCompared=true;
    if(!sameXYZ(*d.position14,l.positionA0)){
        at(0x147130);l.positionA0=*d.position14;f.positionWords=*d.position14;
        at(0x147600); //Fresh D0 NULL; no observer callback.
        at(0x147138);l.position6C[0]=n::detail::fromWord(l.positionA0[0]);
        at(0x147140);l.position6C[1]=n::detail::fromWord(l.positionA0[1]);
        at(0x147148);l.position6C[2]=n::detail::fromWord(l.positionA0[2]);l.positionWritten=true;
        at(0x147184);if(!sourceTopologyLightManager_->incrementSourceTyped149300(*this,f.ordinal,1,md))return stop(md.sourcePC);
        l.positionRevision=true;
    }
    at(0x147194);f.lookCompared=true;
    if(!sameXYZ(*d.look18,l.lookB0)){
        at(0x147200);n::Words normalized{};
        if(n::normalize106EA0(*d.look18,normalized)!=n::Status::complete)return stop(pc);
        f.spotlightLookWords=normalized;l.lookB0=normalized;
        at(0x147210);l.look78[0]=n::detail::fromWord(normalized[0]);
        at(0x147218);l.look78[1]=n::detail::fromWord(normalized[1]);
        at(0x147220);l.look78[2]=n::detail::fromWord(normalized[2]);l.lookWritten=true;
        at(0x147230);if(!sourceTopologyLightManager_->incrementSourceTyped149300(*this,f.ordinal,2,md))return stop(md.sourcePC);
        l.lookRevision=true;
    }
    at(0x147248);l.attenuation8C[0]=n::detail::fromWord((*d.attenuation1C)[0]);
    at(0x147250);l.attenuation8C[1]=n::detail::fromWord((*d.attenuation1C)[1]);
    at(0x147264);l.attenuation8C[2]=n::detail::fromWord((*d.attenuation1C)[2]);f.attenuationWords=*d.attenuation1C;
    at(0x147268);std::uint32_t range=0;if(!sourceTypedAttenuation147268(f,range))return stop(pc);
    at(0x14732c);f.attenuationRange=range;
    if(n::detail::fromWord(range)!=n::detail::fromWord(l.range84)){
        at(0x147340);l.range84=range;l.rangeWritten=true;
        at(0x147354);if(!sourceTopologyLightManager_->incrementSourceTyped149300(*this,f.ordinal,3,md))return stop(md.sourcePC);
        l.rangeRevision=true;
    }
    at(0x14745c); //Type3 branches past all mode writes: constructor mode3 stays.
    at(0x147494);l.configured=true;f.configured=true;
    at(0x277fb4); //Original Spotlight-only146F30, before148F30 registration.
    const auto cone=[&](std::uint32_t degrees,std::uint32_t& result){
        if(!n::detail::normalOrZeroWord(degrees))return false;
        const float value=n::detail::fromWord(degrees);if(value<0.f||value>180.f)return false;
        float product;if(!n::detail::product(n::detail::fromWord(0x3c8efa34),value,product))return false;
        result=n::detail::word(product);return true;};
    std::uint32_t radians=0;at(0x146f40);if(!cone(*d.inner2C,radians))return stop(pc);
    at(0x146f44);f.spotlightCone98=radians;l.cone98=radians;
    at(0x146f48);if(!cone(*d.outer30,radians))return stop(pc);
    at(0x146f50);f.spotlightCone9C=radians;l.cone9C=radians;
    //Do not reorder/reject the actual inner60/outer0 case. No cosine here.
    at(0x277fcc);return true; //Configuration only. Registration remains uncalled.
}
}
