#pragma once
#include <new>
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldAmbientNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceAmbientPhase_==SourceAmbientPhase::unreached)
        return !nativeEmptyAmbient3B4_||!nativeEmptyAmbient3B4_->sourceMode;
    if(sourceAmbientPhase_==SourceAmbientPhase::nativeClosed)
        return !nativeEmptyAmbient3B4_&&!sourceAmbient3B4_&&!sourceAmbientConfigArg_;
    if(!worldApplication_||worldEntity_.reference00!=1||runtimeWorldBorrows_||runtimeCameraBorrows_||
       constructorRacerWorldBorrows_||nativeEmptyAmbient271470Complete_||
       (sourceAmbientConfigArg_&&sourceAmbientConfigArg_!=sourceWorldAudioConfig_.get()))return false;
    if(sourceAmbientPublished_){
        if(!sourceAmbient3B4_||*sourceAmbient3B4_!=nativeEmptyAmbient3B4_.get())return false;
    }else if(!sourceAmbient3B4_||*sourceAmbient3B4_)return false;
    if(sourceAmbientDestructorReturned_&&nativeEmptyAmbient3B4_)return false;
    if(sourceAmbientPhase_==SourceAmbientPhase::returnedZero&&
       (!sourceAmbientPublished_||!sourceAmbientLoaderReturned_||!sourceAmbientGetterReturned_||
        !sourceAmbientDestructorReturned_||nativeEmptyAmbient3B4_))return false;
    if(!nativeEmptyAmbient3B4_)return true;
    const auto& l=*nativeEmptyAmbient3B4_;
    return l.sourceMode&&!(l.sourceConstructorMask&~7u)&&
        (bool(l.sourceVtableActual08)==bool(l.sourceConstructorMask&1u))&&
        (!l.sourceVtableActual08||*l.sourceVtableActual08==0x3d2980)&&
        (!(l.sourceConstructorMask&2u)||l.count30==0)&&
        (!(l.sourceConstructorMask&4u)||l.rows34==nullptr)&&
        (!sourceAmbientPublished_||l.sourceConstructorMask==7u)&&
        (!sourceAmbientGetterReturned_||l.sourceConstructorMask==7u)&&
        preflightSourceAmbientListCountNative(l.entity,l.sourceCounted,l.sourceEnrolled);
}
bool Owner::abortSourceWorldAmbientNative(std::uint32_t& pc)noexcept {
    if(sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached&&sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::nativeClosed)return false;
    if(sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::unreached&&sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::nativeClosed)return false;
    //Parent combines exact Particle+Ambient count contributions before any
    //mutation. This local preflight deliberately never calls that helper.
    if(!preflightSourceWorldParticleCountNative())return false;
    if(sourceAmbientPhase_==SourceAmbientPhase::unreached||sourceAmbientPhase_==SourceAmbientPhase::nativeClosed)return true;
    if(nativeEmptyAmbient3B4_){auto& l=*nativeEmptyAmbient3B4_;
        if(!releaseSourceAmbientListCountNative(l.entity,l.sourceCounted,l.sourceEnrolled))return false;
        sourceAmbient3B4_.reset();nativeEmptyAmbient3B4_.reset();
    }else sourceAmbient3B4_.reset();
    sourceAmbientConfigArg_=nullptr;sourceAmbientPhase_=SourceAmbientPhase::nativeClosed;
    return true; //Native knowledge withdrawal, never a guest destructor/NULL receipt.
}
bool Owner::loadSourceWorldAmbient271470(std::uint32_t& pc)noexcept {
    pc=sourceAmbientPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceAmbientPhase_!=SourceAmbientPhase::unreached||!sourceWorldAmbientInvocationCurrent()||
       nativeEmptyAmbient3B4_||nativeEmptyAmbient271470Complete_||!sourceAmbient3B4_||*sourceAmbient3B4_||
       !sourceWorldAudioConfig_||!reserveSourceAmbientListNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceAmbientPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceAmbientPhase_=SourceAmbientPhase::bounded;return false;};
    sourceAmbientPhase_=SourceAmbientPhase::entered;
    at(0x271480);nativeEmptyAmbient3B4_.reset(new(std::nothrow) NativeEmptyAmbient3B4);
    if(nativeEmptyAmbient3B4_){auto& l=*nativeEmptyAmbient3B4_;l.sourceMode=true;
        if(!constructSourceAmbientList2792E0(l.entity,l.sourceCounted,l.sourceEnrolled,pc))return stop(pc);
        at(0x2792fc);l.sourceVtableActual08=0x3d2980;l.sourceConstructorMask|=1; //Actual vtable3D2980.
        at(0x279300);l.count30=0;l.sourceConstructorMask|=2;
        at(0x279304);l.rows34=nullptr;l.sourceConstructorMask|=4;
    }
    at(0x27149c);sourceAmbient3B4_=nativeEmptyAmbient3B4_.get();sourceAmbientPublished_=true;
    at(0x2714a4);if(!nativeEmptyAmbient3B4_)return stop(pc);
    auto& l=*nativeEmptyAmbient3B4_;auto& config=*sourceWorldAudioConfig_;
    at(0x2714b0);sourceAmbientConfigArg_=&config;
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    at(0x2793c8);if(!config.selectSourceAmbientSounds157D00(returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    //Source ignores selector v0. The exact missing-selection branch is not
    //admitted by this cohort's subsequent getter; preserve prior effects.
    at(0x2793e0);if(!config.readSourceAmbientCount158B30(l.count30,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    sourceAmbientGetterReturned_=true;
    at(0x2793e8);if(l.count30!=0)return stop(pc); //No positive array/descriptor path granted.
    at(0x2793f8);sourceAmbientLoaderReturned_=true; //Actual loader v0=0.
    at(0x2714c8);
    if(!preflightSourceWorldParticleCountNative()||!l.sourceCounted||!l.sourceEnrolled||
       l.sourceConstructorMask!=7u||l.count30||l.rows34)return stop(pc);
    if(!destroySourceEmptyAmbient279320(l.entity,l.sourceCounted,l.sourceEnrolled,l.sourceVtableActual08,pc))return stop(pc);
    //All actual104970/destructor gates were checked before the reference store;
    //the exact empty object has no fallible payload/resource disposal here.
    nativeEmptyAmbient3B4_.reset();sourceAmbientDestructorReturned_=true;
    at(0x2714d0);sourceAmbient3B4_=nullptr;
    sourceAmbientPhase_=SourceAmbientPhase::returnedZero;at(0x2714d8);
    return true; //Segment completed; vEC/vE8 and next42-row list have NOT returned.
}
}
