#pragma once
#include <cfenv>
#include <cmath>
#include <cstring>
#include <new>
namespace ra_cold_aggregate50LA {
inline bool Owner::releaseSourceFogInvocationNative(const void* object,
    const g::OriginalWorldFogInvocation50LA& invocation)noexcept {
    auto* owner=const_cast<Owner*>(static_cast<const Owner*>(object));
    if(!owner||invocation.world_.get()!=owner||invocation.borrowers_!=&owner->sourceFogTransferBorrowers_||
       !owner->sourceFogTransferBorrowers_||invocation.thread_!=std::this_thread::get_id())return false;
    if(owner->sourceFogTransferInvocation_&&owner->sourceFogTransferInvocation_!=&invocation)return false;
    //Successful outputs remain World-owned after this call occurrence ends.
    //Only a bounded invocation cancels its partial outputs on token release.
    if(owner->sourceFogTransfer_&&owner->sourceFogTransfer_->returned){
        owner->sourceFogTransferInvocation_=nullptr;return true;
    }
    std::uint32_t pc=0;return owner->abortSourceFogTransferNative(pc,&invocation);
}
inline bool Owner::abortSourceFogTransferNative(std::uint32_t& pc,
    const g::OriginalWorldFogInvocation50LA* releasing)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(!sourceFogTransfer_)return sourceFogTransferInvocation_==nullptr;
    if(sourceFogTransferBorrowers_&&(!releasing||sourceFogTransferInvocation_!=releasing))return false;
    auto& s=*sourceFogTransfer_;pc=s.pc;
    if(s.nativeCancelled)return true;
    if(!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    auto& authority=worldApplication_->entityAuthority_;
    std::uint32_t counted=0;
    const auto check=[&](const NativeFogEntity* e){
        if(!e||!e->counted)return true;
        ++counted;
        return e->entity.kind==g::OriginalWreckComponentKind50LA::entity141300&&
            e->entity.reference00==1&&e->entity.sessionGeneration==worldEntity_.sessionGeneration&&
            e->entity.objectKey==key(&e->entity)&&!e->entity.parent1C&&!e->entity.firstChild18&&
            !e->entity.previous10&&!e->entity.next14&&!e->entity.mask20&&!e->entity.descendants24&&
            authority.canRetire(e->entity);
    };
    if(!check(s.fog.get())||!check(s.off.get())||!check(s.on.get())||
       counted>worldApplication_->objectCount423F60())return false;
    if(s.onFog30&&(!s.on||!s.onInitialized||*s.onFog30!=s.fog.get()))return false;
    if(s.fog&&s.fog->buffer5C){const auto& b=*s.fog->buffer5C;
        if(b.field20||b.field24||b.field28||b.field2C!=-1)return false;
        for(const auto& word:b.unknown00To1C)if(word)return false;
    }
    const auto retire=[&](NativeFogEntity* e){
        if(e&&e->counted){if(!authority.retire(e->entity))std::terminate();
            --e->entity.reference00;--worldApplication_->objectCount423F60();e->counted=false;}
    };
    //Explicit native partial cancellation: borrower-On before Off before Fog.
    //Withdraw actual13F9DC raw borrow before its Fog owner. These are native
    //pointer withdrawals, not original WorldNULL stores or273F70 free.
    s.onFog30.reset();s.world3F0.reset();retire(s.on.get());s.on.reset();
    s.world3F4.reset();retire(s.off.get());s.off.reset();
    if(s.fog){s.fog->published5C.reset();s.fog->buffer5C.reset();}
    s.world3EC.reset();retire(s.fog.get());s.fog.reset();
    s.renderer.reset(); //No old renderer-color restoration; original write persists.
    s.descriptor=nullptr;s.nativeCancelled=true;sourceFogTransferInvocation_=nullptr;
    return true;
}
inline bool Owner::beginSourceFog273C30(const g::OriginalWorldFogInvocation50LA& invocation,
    std::uint32_t& pc)noexcept {
    pc=0x1aa178;
    if(nativeScbThread_!=std::this_thread::get_id()||invocation.thread_!=std::this_thread::get_id()||
       invocation.attempted_||sourceFogTransfer_||sourceFogTransferInvocation_||!sourceFogTransferBorrowers_||
       invocation.world_.get()!=this||invocation.borrowers_!=&sourceFogTransferBorrowers_||
       invocation.application_!=sourceLoadApplication_||invocation.generation_!=worldEntity_.sessionGeneration||
       !sourceLoadApplication_||sourceLoadApplication_->entities()!=worldApplication_||!worldApplication_||
       invocation.environment_!=sourceEnvironment3A4_.get()||!sourceEnvironmentPublished_||!nativeEnvironment3A4_||
       *nativeEnvironment3A4_!=sourceEnvironment3A4_.get()||
       (sourceEnvironment3A4_?sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedOne:
                               sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedZero)||
       closed_||terminal_||nativeScbRevoked_||!sourceFog3AC_||
       !preflightSourceWorldFogNative()||nativeFogLoadAttempted_||nativeFogLoadComplete_||
       nativeFog3EC_||nativeFogOff3F4_||nativeFogOn3F0_||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||worldApplication_->attachmentLock423F58())return false;
    //Only the private genuine ordered273C24 return constructs this token;
    //source descriptor availability alone supplies no stage.
    invocation.attempted_=true;
    struct PC {std::uint32_t& saved;std::uint32_t& current;~PC(){saved=current;}} save{invocation.pc_,pc};
    try{sourceFogTransfer_=std::make_unique<SourceFogTransfer273C30>();}catch(...){return false;}
    sourceFogTransferInvocation_=&invocation;auto& s=*sourceFogTransfer_;
    const auto at=[&](std::uint32_t value){pc=s.pc=value;};
    s.descriptor=*sourceFog3AC_;at(0x273c48);
    if(!s.descriptor){
        if(sourceFogPhase_!=SourceFogPhase::returnedZero)return false;
        at(0x273c60);s.returned=true;at(0x273f94);return true;
    }
    if(s.descriptor!=nativeFogDescriptor3AC_.get()||sourceFogPhase_!=SourceFogPhase::returnedOne||
       !s.descriptor->sourceMode)return false;
    const auto& descriptor=*s.descriptor;
    //Bound CVT.W.S to actual finite integral byte components. This is an
    //exact native domain, not a rounding policy for arbitrary authoring floats.
    const auto byte=[](std::uint32_t bits,std::uint32_t& out)noexcept {
        if((bits&0x7fffffffu)==0){out=0;return true;}
        if(bits>>31)return false;const auto exponent=(bits>>23)&255u;
        if(exponent<127u||exponent>134u)return false;
        const auto shift=150u-exponent;const auto significand=(bits&0x7fffffu)|0x800000u;
        if(significand&((1u<<shift)-1u))return false;
        out=significand>>shift;return out<=255u;
    };
    constexpr unsigned lanes[4]={3,0,1,2};
    constexpr std::uint32_t conversions[4]={0x273c78,0x273c80,0x273c8c,0x273c9c};
    std::uint32_t packed=0;
    for(unsigned i=0;i<4;++i){std::uint32_t component;at(conversions[i]);
        if(!byte(descriptor.sourceColor00[lanes[i]],component))return false;
        packed|=component<<(24u-8u*i);
    }
    s.packedColor=packed;
    auto& authority=worldApplication_->entityAuthority_;auto& total=worldApplication_->objectCount423F60();
    if(total>std::numeric_limits<std::uint32_t>::max()-3||!authority.reserveAdditional(3))return false;
    auto count=[&](NativeFogEntity& e,std::uint32_t ctor){
        if(!authority.canEnroll(e.entity,key(&e.entity)))return false;
        at(ctor);
        if(g::initializeOriginalWreckComponent50LA(e.entity,g::OriginalWreckComponentKind50LA::entity141300,
            worldEntity_.sessionGeneration,key(&e.entity),true,total).status!=g::OriginalWreckComponentStatus50LA::complete||
            !authority.enroll(e.entity))std::terminate();
        e.counted=true;e.constructorPC=ctor;return true;
    };
    at(0x273cac);s.fog.reset(new(std::nothrow) SourceFogObject13E6B0);
    if(s.fog){auto& f=*s.fog;if(!count(f,0x13e6b0))return false;
        at(0x13e6cc);f.vtable08=0x3cd120;
        at(0x13e6d0);f.words30To64[5]=0; //44
        at(0x13e6d8);f.words30To64[7]=0; //4C
        at(0x13e6dc);f.words30To64[6]=1; //48
        at(0x13e6e0);f.words30To64[12]=1; //60
        at(0x13e6e4);f.published5C=nullptr;
    }
    //Next malloc delay slots publish previous nullable constructor results.
    at(0x273cd0);s.world3EC=s.fog.get();if(s.fog)s.fog->published=true;
    at(0x273ccc);s.off.reset(new(std::nothrow) NativeFogEntity);
    if(s.off){if(!count(*s.off,0x13f850))return false;at(0x13f86c);s.offVtable08=0x3cd0a0;}
    at(0x273cf0);s.world3F4=s.off.get();if(s.off)s.off->published=true;
    at(0x273cec);s.on.reset(new(std::nothrow) NativeFogEntity);
    if(s.on){if(!count(*s.on,0x13f980))return false;at(0x13f99c);s.onVtable08=0x3cd020;}
    at(0x273d08);s.world3F0=s.on.get();if(s.on)s.on->published=true;
    //Actual1AA178 supplies4000 and raw44A6A99A, never a CameraBinding tuple.
    //Neither equals-1, so273D20/44 defaults are genuinely skipped.
    at(0x13e7c4);if(!s.fog)return false;
    auto& f=*s.fog;f.words30To64[8]=0x457a0000u;
    at(0x13e7c8);f.words30To64[9]=0x44a6a99au;
    at(0x13e7d4);f.words30To64[10]=0x42800000u; //malloc delay slot before allocation.
    at(0x13e7d0);f.bufferAttempted=true;f.buffer5C.reset(new(std::nothrow) NativeFogBuffer133F80);
    at(0x13e7e8);f.published5C=f.buffer5C.get(); //NULL does not block13E830.
    at(0x13e84c);f.words30To64[0]=packed|0xff000000u;
    at(0x13e850);f.words30To64[3]=descriptor.sourceVisibility10;
    at(0x13e854);f.words30To64[4]=descriptor.sourceHaziness14;
    //Same finite native policy as existing camera fog arithmetic, but with
    //original partial stores, not transactional adoption of precomputed values.
    auto read=[](std::uint32_t word){float value;std::memcpy(&value,&word,4);return value;};
    auto word=[](float value){std::uint32_t bits;std::memcpy(&bits,&value,4);return bits;};
    auto add=[](float a,float b){volatile float c=a+b;return c;};
    auto sub=[](float a,float b){volatile float c=a-b;return c;};
    auto mul=[](float a,float b){volatile float c=a*b;return c;};
    at(0x13e874);if(std::fegetround()!=FE_TONEAREST)return false;
    const float far=read(*f.words30To64[8]),near=read(*f.words30To64[9]);
    const float visibility=read(descriptor.sourceVisibility10),haze=read(descriptor.sourceHaziness14);
    if(!std::isfinite(visibility)||!std::isfinite(haze))return false;
    const float distance=sub(far,near);if(!std::isfinite(distance))return false;
    at(0x13e87c);const float scaled=mul(visibility,distance);if(!std::isfinite(scaled))return false;
    at(0x13e880);const float end=add(near,scaled);if(!std::isfinite(end))return false;
    at(0x13e884);f.words30To64[2]=word(end);
    at(0x13e890);const float span=sub(end,64.f);if(!std::isfinite(span))return false;
    at(0x13e894);const float remainder=sub(1.f,haze);if(!std::isfinite(remainder))return false;
    at(0x13e898);const float start=mul(remainder,span);if(!std::isfinite(start))return false;
    at(0x13e89c);f.words30To64[1]=word(start);
    //Reuse the existing scalar-only442F70 lease, NOT CameraBinding. It proves
    //same Root, native controlblock, displaygeneration, Video, ownerthread and
    //Application lifetime, then pins both real Display/Video dependencies.
    //No renderer initialization, queue service, camera or frame permission.
    at(0x13e8a4);
    using Lease=rr::platform::OriginalUiGarageFogColorLease50KG;
    s.renderer=Lease::retain(*sourceLoadApplication_,sourceLoadApplication_->raceDisplayOwner50LA_.lock());
    if(!s.renderer||!s.renderer->currentOwnedBy(s.renderer,sourceLoadApplication_))return false;
    at(0x13e8a8);
    s.renderer->display_->sourceFogColor28_=*f.words30To64[0];
    s.renderer->display_->sourceFogColor28Known_=true;s.rendererStored=true;
    at(0x13e8b0);f.words30To64[12]=1; //Branch delay store; a2=0 skips diagnostic.
    //13E780 returns actual Fog pointer, so caller skips its failed-return path.
    at(0x273e1c);if(!s.off)return false; //First Off vtable read.
    at(0x13f8a4);s.offInitialized=true; //13F890->literal return-this141C20.
    at(0x13f9dc);if(!s.on)return false;
    s.onFog30=s.fog.get();s.onInitialized=true; //Raw unretained13F9C0 borrow.
    //Only now does source273F70 free the SAME plain parsed0x1C descriptor.
    //No other retained descriptor consumer exists at this ordered occurrence.
    if(s.descriptor!=nativeFogDescriptor3AC_.get()||!sourceFog3AC_||
       *sourceFog3AC_!=s.descriptor||!preflightSourceWorldFogNative())return false;
    at(0x273f70);nativeFogDescriptor3AC_.reset();s.descriptor=nullptr;s.descriptorFreed=true;
    at(0x273f7c);sourceFog3AC_=nullptr;s.descriptorNullStored=true;
    s.returned=true;at(0x273f94);return true; //Actual callee return, not World readiness. 
}
}
