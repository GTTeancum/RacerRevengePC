#pragma once
#include "original_source_world_light_read50la.inl"
#include <cstdio>
#include <cstring>
#include <new>
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldParticleDescriptorsNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceParticleDescriptorPhase_==SourceParticleDescriptorPhase::unreached)return !sourceParticleDescriptorAttempted_;
    if(sourceParticleDescriptorPhase_==SourceParticleDescriptorPhase::nativeClosed)return !nativeParticleList3D8_&&!sourceParticleList3D8_;
    if(!sourceParticleDescriptorAttempted_||!worldApplication_||worldEntity_.reference00!=1||
       runtimeWorldBorrows_||runtimeCameraBorrows_||constructorRacerWorldBorrows_||
       (sourceParticleDescriptorConfigArg_&&sourceParticleDescriptorConfigArg_!=sourceWorldObjectConfig_.get()))return false;
    if(sourceParticleDescriptorPublished_){if(!sourceParticleList3D8_||*sourceParticleList3D8_!=nativeParticleList3D8_.get())return false;}
    else if(!sourceParticleList3D8_||*sourceParticleList3D8_)return false; //Still actual resetNULL.
    if(!nativeParticleList3D8_)return true;
    const auto& l=*nativeParticleList3D8_;
    if(!l.sourceMode||l.actualOwner||l.complete||l.published||l.loaded||l.servicesCompleted||l.serviceEntered||
       l.serviceTerminal||l.serviceComplete||(l.sourceConstructorMask&~7u)||
       (l.sourceConstructorReturned&&l.sourceConstructorMask!=7u)||
       !preflightSourceParticleListCountNative(l.entity,l.counted,l.sourceEnrolled))return false;
    if(l.sourceArrayAttempted){
        if(l.count30!=4||!l.sourceArrayPublished||!l.sourceArray34||*l.sourceArray34!=l.pointers34.get())return false;
    }else if(l.pointers34||l.sourceArrayPublished||l.sourceArray34||((l.sourceConstructorMask&2u)&&l.count30))return false;
    for(std::size_t i=0;i<4;++i){const auto& r=l.rows[i];
        if(l.sourceSlotPublished[i]){if(!l.sourceRowAttempted[i]||!l.pointers34||l.pointers34[i]!=r.get())return false;}
        if(!r){if(l.sourceRowReturned[i])return false;continue;}
        if(!l.sourceRowAttempted[i]||!r->sourceMode||r->sourceConstructorMask!=7u||r->emitterPointers00||
           r->emitterPublished||r->emitterEnrolled||r->configured||r->started||r->enabled||r->attached||
           r->descriptorRetired||r->attachment74||r->unknown08||r->unknown0C||r->unknown10)return false;
        if(r->sourceDescriptorPublished){
            if(!r->sourceDescriptorAttempted||!r->sourceDescriptor04||*r->sourceDescriptor04!=r->descriptor04.get())return false;
        }else if(r->descriptor04||r->sourceDescriptor04||r->sourceDescriptorAttempted)return false;
        if(!r->descriptor04){if(l.sourceRowReturned[i])return false;continue;}
        const auto& d=*r->descriptor04;
        if(!d.sourceMode||d.actualAttachment||d.loaded||d.child10C||d.offset320||d.offsetW32C||
           (d.sourceKnownMask&~0x7ffu)||(d.sourceOffsetKnown&~15u))return false;
        const auto textCurrent=[](const std::array<char,260>& text,const std::optional<std::size_t>& n){return !n||(*n<text.size()&&text[*n]==0);};
        if(!textCurrent(d.sourceAttach08,d.sourceAttachBytes)||!textCurrent(d.sourceChild10C,d.sourceChildBytes)||
           !textCurrent(d.sourceEmitter210,d.sourceEmitterBytes)||
           (bool(d.instances330)!=bool(d.sourceKnownMask&512u))||
           (bool(d.startOn334)!=bool(d.sourceKnownMask&1024u))||
           (l.sourceRowReturned[i]&&!d.sourceLoaded))return false;
    }
    return true; //No unknown raw slot or unproduced descriptor byte was read.
}
bool Owner::abortSourceWorldParticleDescriptorsNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldParticleCountNative())return false;
    if(sourceParticleDescriptorPhase_==SourceParticleDescriptorPhase::unreached||
       sourceParticleDescriptorPhase_==SourceParticleDescriptorPhase::nativeClosed)return true;
    if(nativeParticleList3D8_){auto& l=*nativeParticleList3D8_;
        //Potential authority refusal precedes every plain backing withdrawal.
        if(!releaseSourceParticleListCountNative(l.entity,l.counted,l.sourceEnrolled))return false;
        sourceParticleList3D8_.reset();
        for(std::size_t i=4;i-->0;)if(l.rows[i]){
            l.rows[i]->sourceDescriptor04.reset();l.rows[i]->descriptor04.reset();l.rows[i].reset();}
        l.sourceArray34.reset();l.pointers34.reset();nativeParticleList3D8_.reset();
    }else sourceParticleList3D8_.reset();
    sourceParticleDescriptorConfigArg_=nullptr;sourceParticleDescriptorPhase_=SourceParticleDescriptorPhase::nativeClosed;
    return true; //Native disposal only, not source NULL/destructor/loader history.
}
bool Owner::loadSourceWorldParticleDescriptors2722F0(std::uint32_t& pc)noexcept {
    pc=0;std::array<std::uint32_t,4> origin,forward;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceParticleDescriptorPhase_!=SourceParticleDescriptorPhase::unreached||
       !sourceWorldParticleDescriptorsInvocationCurrent()||nativeParticleList3D8_||
       !sourceParticleList3D8_||*sourceParticleList3D8_||!sourceWorldObjectConfig_||
       !copySourceWorldStartDefaults(origin,forward)||!reserveSourceParticleListNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceParticleDescriptorPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceParticleDescriptorPhase_=SourceParticleDescriptorPhase::bounded;return false;};
    sourceParticleDescriptorAttempted_=true;sourceParticleDescriptorPhase_=SourceParticleDescriptorPhase::entered;
    at(0x272308);nativeParticleList3D8_.reset(new(std::nothrow) NativeParticleList3D8);
    if(nativeParticleList3D8_){auto& l=*nativeParticleList3D8_;l.sourceMode=true;
        if(!constructSourceParticleList27DC90(l.entity,l.counted,l.sourceEnrolled,pc))return stop(pc);
        at(0x27dcac);l.sourceConstructorMask|=1; //vtable08=3D24F0.
        at(0x27dcb0);l.count30=0;l.sourceConstructorMask|=2;
        at(0x27dcb4);l.pointers34.reset();l.sourceConstructorMask|=4;l.sourceConstructorReturned=true;
    }
    at(0x272324);sourceParticleList3D8_=nativeParticleList3D8_.get();sourceParticleDescriptorPublished_=true;
    at(0x27232c);if(!nativeParticleList3D8_)return stop(pc);
    auto& l=*nativeParticleList3D8_;auto& config=*sourceWorldObjectConfig_;
    at(0x272338);sourceParticleDescriptorConfigArg_=&config;
    a::OriginalStandardTextureDiagnostic50LA diagnostic;bool returned=false,found=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept {
        if(!config.readSourceParticleDescriptorField158240(key,value,present,copy,diagnostic)){
            pc=diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    at(0x27de34);sourceParticleDescriptorSuccessS1_=0; //Selector call delay slot before effects.
    at(0x27de30);if(!config.selectSourceParticleDescriptorSection157D00(0,returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x27de40); //Unjoined missing-root diagnostic/false return, not success.
    std::int32_t count;at(0x27de64);if(!read.integer("numberofemitters",0,&count,found))return stop(pc);
    sourceParticleDescriptorCountStack16C_=count;
    at(0x27de70);if(count!=4)return stop(pc); //Exact authenticated cohort, no policy clamp.
    at(0x27deb0);l.count30=count;
    at(0x27deb8);l.sourceArrayAttempted=true;l.pointers34.reset(new(std::nothrow) NativeParticleRow27DE00*[4]);
    at(0x27dec0);l.sourceArray34=l.pointers34.get();l.sourceArrayPublished=true;
    at(0x27dec4);sourceParticleDescriptorRowS0_=0;
    at(0x27decc);sourceParticleDescriptorOffsetS2_=0;
    for(std::size_t i=0;i<4;++i){
        if(!sourceParticleDescriptorRowS0_||*sourceParticleDescriptorRowS0_!=static_cast<std::int32_t>(i)||
           !sourceParticleDescriptorOffsetS2_||*sourceParticleDescriptorOffsetS2_!=i*4)return stop(0x27ded0);
        at(0x27dedc);constexpr char prefix[]="ParticleEmitter";
        std::memcpy(sourceParticleDescriptorName60_.data(),prefix,sizeof(prefix)-1);
        sourceParticleDescriptorName60_[sizeof(prefix)-1]=static_cast<char>('1'+i);
        sourceParticleDescriptorName60_[sizeof(prefix)]=0;sourceParticleDescriptorNameKnown_=true;
        at(0x27dee8);if(!config.selectSourceParticleDescriptorSection157D00(static_cast<std::uint32_t>(i+1),returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
        if(!returned)return stop(0x27def8); //Do not invent a NULL slot for missing section.
        at(0x27df14);l.sourceRowAttempted[i]=true;l.rows[i].reset(new(std::nothrow) NativeParticleRow27DE00);
        if(l.rows[i]){auto& r=*l.rows[i];r.sourceMode=true;
            at(0x27df2c);r.sourceConstructorMask|=1;
            at(0x27df30);r.emitterPointers00.reset();r.sourceConstructorMask|=2;
            at(0x27df34);r.descriptor04.reset();r.sourceConstructorMask|=4;}
        at(0x27df40);if(!l.pointers34)return stop(pc); //Allocated row stays owned if destination is NULL.
        l.pointers34[i]=l.rows[i].get();l.sourceSlotPublished[i]=true;
        at(0x27df50);if(!l.rows[i])return stop(pc);
        auto& r=*l.rows[i];at(0x27c768);r.sourceDescriptorAttempted=true;
        r.descriptor04.reset(new(std::nothrow) NativeParticleDescriptor27C430(NativeParticleDescriptor27C430::SourceTag{}));
        if(r.descriptor04){auto& d=*r.descriptor04;
            at(0x27c44c);d.sourceKnownMask|=1; //vtable338=3D2588.
            at(0x27c454);d.sourceOffset320={{0,0,0,0x3f800000u}};d.sourceOffsetKnown=15;
            //Both 8BB0/8BB4 guard blocks are genuinely skipped:260!=4.
            at(0x27c500);std::memcpy(d.sourceAttach08.data(),"none",4);
            at(0x27c50c);d.sourceAttach08[4]=0;d.sourceAttachBytes=4;d.sourceKnownMask|=2;
            at(0x27c5b8);std::memcpy(d.sourceEmitter210.data(),"none",4);
            at(0x27c5c4);d.sourceEmitter210[4]=0;d.sourceEmitterBytes=4;d.sourceKnownMask|=4;
            at(0x27c5c8);d.startingZone00=0;d.sourceKnownMask|=8;
            at(0x27c5cc);d.type04=0;d.sourceKnownMask|=16;
            at(0x27c5dc);d.sourceOffset320=origin;d.sourceOffsetKnown=15;d.sourceKnownMask|=32;
        }
        at(0x27c780);r.sourceDescriptor04=r.descriptor04.get();r.sourceDescriptorPublished=true;
        at(0x27c788);if(!r.descriptor04)return stop(pc);
        auto& d=*r.descriptor04;
        at(0x27c628);if(!read.integer("startingzone",0,&d.startingZone00,found))return stop(pc);d.sourceKnownMask|=64;
        at(0x27c640);if(!read.integer("typeid",0,&d.type04,found))return stop(pc);d.sourceKnownMask|=128;
        at(0x27c660);if(!read.text("attachtosoultree","NONE",d.sourceAttach08.data(),d.sourceAttach08.size(),found))return stop(pc);
        d.sourceAttachBytes=std::strlen(d.sourceAttach08.data());
        at(0x27c678);if(*d.sourceAttachBytes==4&&std::memcmp(d.sourceAttach08.data(),"NONE",4)==0)return stop(pc); //Different actual position branch unjoined.
        at(0x27c6c0);if(!read.text("childobject","NONE",d.sourceChild10C.data(),d.sourceChild10C.size(),found))return stop(pc);
        d.sourceChildBytes=std::strlen(d.sourceChild10C.data());d.sourceKnownMask|=256;
        at(0x27c6dc);if(!read.vector("offsetfromsoultree",origin,&d.sourceOffset320,found))return stop(pc);
        std::int32_t n;at(0x27c6f4);if(!read.integer("numberofinstances",1,&n,found))return stop(pc);d.instances330=n;d.sourceKnownMask|=512;
        at(0x27c714);if(!read.text("emitterfilename","NONE",d.sourceEmitter210.data(),d.sourceEmitter210.size(),found))return stop(pc);
        d.sourceEmitterBytes=std::strlen(d.sourceEmitter210.data());
        at(0x27c72c);if(!read.integer("starton",1,&n,found))return stop(pc);d.startOn334=n;d.sourceKnownMask|=1024;
        at(0x27c740);d.sourceLoaded=true;l.sourceRowReturned[i]=true;
        at(0x27df60);++*sourceParticleDescriptorSuccessS1_;
        at(0x27df68);*sourceParticleDescriptorOffsetS2_+=4;
        at(0x27df6c);++*sourceParticleDescriptorRowS0_;
    }
    at(0x27df8c);if(sourceParticleDescriptorLogAttempted_)return stop(pc);
    sourceParticleDescriptorLogAttempted_=true;
    //Explicit native diagnostic adapter, not original stdio emulation. Exact
    //source format and actual completed arguments4/4; source ignores result.
    constexpr char message[]="Read 4 particle emitters out of a total of 4.\n";
    sourceParticleDescriptorLogWritten_=std::fwrite(message,1,sizeof(message)-1,stdout);
    sourceParticleDescriptorLogError_=std::ferror(stdout)!=0;
    at(0x27df94); //Actual loader v0=1;27233C skips source deletion.
    at(0x271d90);sourceParticleDescriptorPhase_=SourceParticleDescriptorPhase::returned;return true;
}
}
