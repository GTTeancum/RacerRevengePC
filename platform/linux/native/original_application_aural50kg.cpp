#include "../../../src/game/original_loading_manager33050la.h"
#include <cstdio>
#include "original_application_aural50kg.h"
#include "original_ui_garage_audio_backing50kg.h"
#include "original_ui_light_manager_owner50kg.h"
#include "native_source_reverb50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <bit>
#include <cfenv>
#include "../../../src/game/body_scene48g.h"
#include "../../../src/assets/original_config_process_globals50la.h"
#include <cmath>
#include <limits>
namespace rr::platform {
using K=game::OriginalWreckComponentKind50LA;
namespace {
// UI_MUSIC songs started by 234880 (stream+40 = mode1, single play): UIWin
// Song10 trphy1.vag and Song8 trphy2_3.vag (208810). Every other Main song is
// started by 234900 (mode4, loop). Mirrored in original_ui_main_audio50kg.cpp.
constexpr bool songPlaysOnce234880(std::uint32_t song)noexcept {return song==10u||song==8u;}
template<class T> bool same(const std::shared_ptr<T>& a,const std::shared_ptr<T>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
}
OriginalApplicationAural50KG::NativeInterface::~NativeInterface()noexcept {
    if(raceIntroLease||(streamGeneration&&!streamRetirementAuthorized))std::terminate();
    // Only actual interface destruction discharges remaining stopped stream
    // slots. No ordinary stop, clear103DD0, Group retirement or reentry does.
    streamSlots={};
}
bool OriginalApplicationAural50KG::List::initialize104410()noexcept {
    if(!constructed||count||capacity||storage||allocatedBit0)return false;
    //104510..530 publish growth/capacity/bit even when allocation returned null.
    try{storage.reset(new C*[8]);}catch(...){storage.reset();}
    growth=16;capacity=8;allocatedBit0=true;return bool(storage);
}
bool OriginalApplicationAural50KG::List::valid()const noexcept {
    return constructed&&count<=capacity&&(!count||storage)&&
        (allocatedBit0?(growth==16&&capacity>=8):(!count&&!growth&&!capacity&&!storage));
}
OriginalApplicationAural50KG::C* OriginalApplicationAural50KG::List::at104750(std::uint32_t i)const noexcept {
    return count&&storage&&i<count?storage[i]:nullptr;
}
bool OriginalApplicationAural50KG::List::remove1046D0(const C* node)noexcept {
    for(std::uint32_t i=0;i<count;++i)if(storage[i]==node){
        storage[i]=storage[count-1];--count;return true; // No stable erase/null tail store.
    }
    return false;
}
OriginalApplicationAural50KG::OriginalApplicationAural50KG(const Application& root,std::shared_ptr<E> e)noexcept
    :issuer_(&root),entities_(std::move(e)),lifetime_(root.animationClock()),generation_(key(entities_.get())){}
OriginalApplicationAural50KG::~OriginalApplicationAural50KG()noexcept {
    if(constructed_&&!retired_)std::terminate();
}
bool OriginalApplicationAural50KG::sourceIdentity()const noexcept {
    if(thread_!=std::this_thread::get_id()||!entities_||entities_->nativeOwnerThread_!=thread_)return false;
    std::uint32_t pause=0;
    if(!issuer_||!lifetime_.readPause11C(pause))return false; // Before raw issuer dereference.
    return constructed_&&!retired_&&same(issuer_->entities(),entities_)&&
        lifetime_.sameOwner(issuer_->animationClock())&&entities_->constructed_&&!entities_->released_&&
        entities_->auralNativeOwner_==this&&entities_->aural00Live_&&entities_->aural00_==&entity_&&
        entities_->auralPrefixIssued_&&entity_.kind==K::auralScape102BE0&&
        entity_.sessionGeneration==generation_&&entity_.objectKey==key(&entity_)&&entity_.reference00==1&&
        contains(entities_->entityAuthority_.view(),&entity_);
}
bool OriginalApplicationAural50KG::currentIncludingIncompleteListener()const noexcept {
    if(!sourceIdentity()||entities_->attachmentLock423F58_||!listenerCustodyCurrent(true))return false;
    for(const auto& list:lists_)if(!list.valid())return false;
    return game::original_entity_chain_detail50LA::forest(entities_->entityAuthority_.view());
}
bool OriginalApplicationAural50KG::listenerTerminalCleanupCurrent()const noexcept {
    // A failed reservation remains poisoned, but after its real manager has
    // withdrawn no listener can be serviced or retried. Permit ONLY actual
    // output/Pending/final-Aural retirement through this private predicate.
    return currentIncludingIncompleteListener()&&!nativeListenerIssuer_&&
        (nativeListenerPhase_==NativeListenerPhase::failed||
         (nativeListenerPhase_==NativeListenerPhase::sourceReserved&&
          sourceListenerReservation_.nativeWithdrawn&&!sourceListenerReservation_.issuer));
}
bool OriginalApplicationAural50KG::current()const noexcept {
    return currentIncludingIncompleteListener()&&listenerCustodyCurrent(false);
}
bool OriginalApplicationAural50KG::listenerCustodyCurrent(bool allowIncomplete)const noexcept {
    if(nativeListenerPhase_==NativeListenerPhase::retiring)return false;
    const auto& source=sourceListenerReservation_;
    if(nativeListenerPhase_!=NativeListenerPhase::sourceReserved&&
       (source.issuer||source.before34||source.after34||source.index||source.nativeWithdrawn||
        source.wrapperIdentity||source.wrapperPublished||source.nativeWrapperRetired||
        source.nativeSlotBorrowWithdrawn||source.wrapperAural00Bound||source.backendPrefix||
        source.backendIdentity||source.backendPrefixRetained||source.nativeBackendRetired||
        source.backendConstructorReturned||source.backendFactoryReturned||source.wrapperBackend04Published||
        source.initializerReturn||source.before48||source.after48||source.registrationReturn||source.listenerReturn))return false;
    for(std::size_t n=0;n<listeners4C_.size();++n){
        if(listeners4C_[n]!=nativeListenerOwners_[n].get())return false;
        if(n&&listeners4C_[n]&&(nativeListenerPhase_==NativeListenerPhase::sourceReserved||n>=2))return false;
        if(nativeListenerPhase_!=NativeListenerPhase::sourceReserved&&
           (n<word48_)!=bool(listeners4C_[n]))return false;
        if(n<word48_&&nativeListenerPhase_!=NativeListenerPhase::sourceReserved&&
           !listeners4C_[n]->current(*this))return false;
    }
    switch(nativeListenerPhase_){
    case NativeListenerPhase::empty:
        return !nativeListenerIssuer_&&!word34_&&!word48_&&!listeners4C_[0];
    case NativeListenerPhase::ready:
        return nativeListenerIssuer_&&word34_>=1u&&word34_<=2u&&word34_==word48_;
    case NativeListenerPhase::constructing:
    case NativeListenerPhase::failed:
        // 102EF0 reserves the index before its wrapper/backend allocation.
        return allowIncomplete&&word34_<=2u&&word48_<=word34_&&word34_-word48_<=1u;
    case NativeListenerPhase::sourceReserved:
        //This phase remains separate from native ready even after genuine
        //source initialization/registration/listener return. Native disposal
        //retains the actual reserved34 and reached48; it never repairs counts.
        if(!allowIncomplete||nativeListenerIssuer_||source.before34!=0u||
           source.after34!=1u||source.index!=0u||word34_!=*source.after34||
           (source.issuer?source.nativeWithdrawn:!source.nativeWithdrawn))return false;
        if(source.initializerReturn==1u){
            //No callback/fallible cut separates this supported positive suffix.
            if(!source.wrapperBackend04Published||source.before48!=0u||source.after48!=1u||
               source.registrationReturn!=1u||source.listenerReturn!=1u||word48_!=*source.after48)return false;
        }else{
            if((source.initializerReturn&&source.initializerReturn!=0u)||source.before48||source.after48||
               source.registrationReturn||source.listenerReturn||word48_)return false;
            //A genuine zero-bit factory result has no backend; unknown host
            //allocation outcomes must not masquerade as this source return0.
            if(source.initializerReturn==0u&&source.backendPrefixRetained)return false;
        }
        if(source.backendPrefixRetained){
            if(!source.backendIdentity||!source.wrapperPublished||!source.wrapperAural00Bound)return false;
            //No fallible boundary/callback separates constructor return,
            //factory return and04 publication in the joined suffix.
            if(source.backendConstructorReturned!=source.backendFactoryReturned||
               source.backendFactoryReturned!=source.wrapperBackend04Published)return false;
            if(source.nativeBackendRetired){
                if(!source.nativeWithdrawn||source.backendPrefix||!source.nativeWrapperRetired)return false;
                //Never dereference the retired historical backend identity.
            }else if(source.nativeWithdrawn||source.backendPrefix.get()!=source.backendIdentity||
                     (source.backendConstructorReturned?!source.backendPrefix->sourceConstructorCopiesCurrent():
                                                        !source.backendPrefix->sourcePrefixBefore4349D0()))return false;
        }else if(source.backendPrefix||source.backendIdentity||source.nativeBackendRetired||
                 source.backendConstructorReturned||source.backendFactoryReturned||source.wrapperBackend04Published)return false;
        if(!source.wrapperPublished)
            return !source.wrapperIdentity&&!source.nativeWrapperRetired&&!source.wrapperAural00Bound&&
                !source.nativeSlotBorrowWithdrawn&&!listeners4C_[0];
        if(!source.wrapperIdentity)return false;
        if(source.nativeWrapperRetired)
            return source.nativeSlotBorrowWithdrawn&&source.nativeWithdrawn&&
                !source.issuer&&!listeners4C_[0]; //Historical pointer is NEVER dereferenced.
        return !source.nativeSlotBorrowWithdrawn&&!source.nativeWithdrawn&&source.issuer&&
            listeners4C_[0]==source.wrapperIdentity&&source.wrapperIdentity->sourceCancellationBacking()&&
            source.wrapperIdentity->sourceOwner00()==(source.wrapperAural00Bound?this:nullptr)&&
            source.wrapperIdentity->backend04_==(source.wrapperBackend04Published?source.backendIdentity:nullptr);
    default:return false;
    }
}
bool OriginalApplicationAural50KG::listenerCurrent(const ListenerManager& manager)const noexcept {
    return current()&&nativeListenerPhase_==NativeListenerPhase::ready&&nativeListenerIssuer_==&manager;
}
bool OriginalApplicationAural50KG::beginRaceListeners1A4620(
    const ListenerManager& manager,std::uint32_t count,Diagnostic& d)noexcept {
    d={};
    // The manager supplies the count loaded from race+1B0 at 1A3C14.
    //These are native ownership gates, not invented guest branch results.
    if((count!=1&&count!=2)||!current()||nativeListenerPhase_!=NativeListenerPhase::empty||
       !issuer_->state||!nativeInitialized_||!backendCurrent()||!canActivateUiInitializer1B7340(d))
        return fail(d,Status::owner,0x1a4620);
    //141560 activation is BEFORE any listener reservation. Its real childless
    //source owner was checked above; do not replace it with clear103DD0.
    if(!activateUiInitializer1B7340(d)){
        //Any already-issued activation prefix is terminal, even before34.
        nativeListenerIssuer_=&manager;nativeListenerPhase_=NativeListenerPhase::failed;return false;
    }
    nativeListenerIssuer_=&manager;nativeListenerPhase_=NativeListenerPhase::constructing;
    flags9CBit0_=true; //1A4678, preserve other bits in the existing projection.
    for(std::uint32_t index=0;index<count;++index){
        ++word34_; //102EF0: reserve the listener index BEFORE allocation/assertion stores.
        //This native adapter uses a projected per-App guard, not the authentic
        //shared process owner. Irreversibly poison BEFORE even reading that copy.
        assets::OriginalConfigProcessGlobals50LA::invalidateListenerAssertionHistoryNative();
        auto& app=*issuer_->state;
        if(!app.listenerGuard80C4){
            app.listenerAssertion3DF4C0=1u;app.listenerGuard80C4=1u; //102F48/4C.
        }
        //Each admitted index makes the represented133660(index<4) return0, with no
        //assertion callback on either existing diagnostic-enable branch.
        NativeListener290A90* wrapper=nullptr;
        try{wrapper=new NativeListener290A90;}catch(...){wrapper=nullptr;}
        nativeListenerOwners_[index].reset(wrapper);
        listeners4C_[index]=wrapper; //102FC0 includes the genuine allocation-null result.
        if(!wrapper){
            listeners4C_[index]=nullptr; //103004 after genuine null290AA0 no-op.
            nativeListenerPhase_=NativeListenerPhase::failed;return fail(d,Status::allocation,0x102fc8);
        }
        if(!app.listenerGuard8C58){
            app.listenerAssertion4344B0=1u;app.listenerGuard8C58=1u; //290B48/4C.
        }
        //Actual same Aural is nonnull;133660 at290B6C has no callback here.
        wrapper->bindAural290BAC(*this);
        NativeListenerBackend2357A0* backend=nullptr;
        //232D50 is actual interface28-bit0 gated. No device/RPC factory substitute.
        if((**interfaceA0_).initialized28Bit0){
            try{backend=new NativeListenerBackend2357A0;}catch(...){backend=nullptr;}
        }
        wrapper->installBackend290BBC(backend); //Publish even null, before return test.
        if(!backend){
            nativeListenerOwners_[index].reset(); //102FF4 ->290AA0 BEFORE null slot store.
            listeners4C_[index]=nullptr;nativeListenerPhase_=NativeListenerPhase::failed;
            return fail(d,Status::allocation,0x290bc4);
        }
        //Four exact PS2 backend leaves return1 without stores/callbacks/hardware.
        (void)wrapper->initialize290CF0(0x40400000u);
        (void)wrapper->initialize290D30(0x3f800000u);
        (void)wrapper->initialize290CB0(0x3e9c0ebfu);
        (void)wrapper->update290C70();
        ++word48_; //10301C only after actual successful290B10.
    }
    nativeListenerPhase_=NativeListenerPhase::ready;
    d={Status::complete,0x1a46dc};return true;
}
bool OriginalApplicationAural50KG::removeRaceListeners103040(
    const ListenerManager& manager,Diagnostic& d)noexcept {
    d={};
    if(!currentIncludingIncompleteListener()||nativeListenerPhase_==NativeListenerPhase::sourceReserved)
        return fail(d,Status::owner,0x103040); //Source cancellation is not this native adapter.
    if(!nativeListenerIssuer_){d={Status::complete,0x1a4618};return true;} //Never began/already withdrawn.
    if(nativeListenerIssuer_!=&manager)return fail(d,Status::owner,0x103040);
    const auto registered=word48_;
    for(std::uint32_t index=0;index<registered;++index){
        nativeListenerOwners_[index].reset();
        listeners4C_[index]=nullptr;
        --word48_;--word34_;
    }
    if(!word34_)nativeListenerPhase_=NativeListenerPhase::empty;
    //Original enclosing1A45C0 ignores103040's null-slot false. It never repairs
    //reserved34 on a failed constructor. Withdraw native issuer, keep poison.
    nativeListenerIssuer_=nullptr;
    d={Status::complete,0x1a4618};return true;
}
bool OriginalApplicationAural50KG::readSingleRaceListener290E10(
    const ListenerManager& manager,ListenerXYZ& out,Diagnostic& d)const noexcept {
    d={};if(!listenerCurrent(manager))return fail(d,Status::owner,0x290e10);
    if(!listeners4C_[0]->readPosition290E10(out))return fail(d,Status::backend,0x290e14);
    d={Status::complete,0x290e40};return true;
}
bool OriginalApplicationAural50KG::writeSingleRaceListener1030B0(
    const ListenerManager& manager,const std::array<const float*,9>& actualCameraXYZ,
    const ListenerXYZ& velocity,Diagnostic& d)noexcept {
    d={};if(!listenerCurrent(manager)||!issuer_->state)return fail(d,Status::owner,0x1030b0);
    //Private manager/Registration proves these exact live source member lanes.
    //Validate native addresses only; no source float is read before lazy stores.
    for(const auto* lane:actualCameraXYZ)if(!lane)return fail(d,Status::owner,0x1030b0);
    auto& app=*issuer_->state;
    if(!app.listenerGuard80C8){app.listenerAssertion3DF4C8=1u;app.listenerGuard80C8=1u;}
    //Index0 assertion is true and does not call assertion/logging callbacks.
    //Direct per-lane read/store chronology290E50 then290DC0 then290ED0;
    //do not copy all camera values before reached position stores.
    auto& backend=*listeners4C_[0]->backend04_;
    backend.position10_[0]=std::bit_cast<std::uint32_t>(*actualCameraXYZ[0]);
    backend.position10_[1]=std::bit_cast<std::uint32_t>(*actualCameraXYZ[1]);
    backend.position10_[2]=std::bit_cast<std::uint32_t>(*actualCameraXYZ[2]);
    backend.look20_[0]=std::bit_cast<std::uint32_t>(*actualCameraXYZ[3]);
    backend.look20_[1]=std::bit_cast<std::uint32_t>(*actualCameraXYZ[4]);
    backend.look20_[2]=std::bit_cast<std::uint32_t>(*actualCameraXYZ[5]);
    backend.up30_[0]=std::bit_cast<std::uint32_t>(*actualCameraXYZ[6]);
    backend.up30_[1]=std::bit_cast<std::uint32_t>(*actualCameraXYZ[7]);
    backend.up30_[2]=std::bit_cast<std::uint32_t>(*actualCameraXYZ[8]);
    backend.velocity40_[0]=velocity[0];backend.velocity40_[1]=velocity[1];backend.velocity40_[2]=velocity[2];
    d={Status::complete,0x1031d4};return true;
}
bool OriginalApplicationAural50KG::observeSingleRaceSpeech104140(
    const ListenerManager& manager,const std::array<std::uint32_t,3>& actualLocalXYZ,
    const std::array<std::uint32_t,4>& actualMain70,std::array<std::uint32_t,128>& scratch,
    std::array<bool,128>& known,bool& audible,Diagnostic& d)const noexcept {
    d={};
    if(!listenerCurrent(manager))return fail(d,Status::owner,0x104140);
    if(std::fegetround()!=FE_TONEAREST)return fail(d,Status::phase,0x104140);
    //Native knownness, not guest zero stores. First80 words are104140's frame;
    //next48 are102950's. Unrepresented saved registers are NEVER made known.
    known.fill(false);
    const auto store=[&](std::size_t index,std::uint32_t bits)noexcept {scratch[index]=bits;known[index]=true;};
    const auto fp=[](std::uint32_t bits)noexcept {return std::bit_cast<float>(bits);};
    const auto word=[](float value)noexcept {return std::bit_cast<std::uint32_t>(value);};
    const auto vector=[&](std::size_t at)noexcept {
        store(at,0);store(at+1,0);store(at+2,0);store(at+3,0x3f800000u);
    };
    const auto copy=[&](std::size_t to,std::size_t from)noexcept {
        for(std::size_t lane=0;lane<4;++lane)store(to+lane,scratch[from+lane]);
    };
    const auto normalOrZero=[](float v)noexcept {
        const auto exp=std::bit_cast<std::uint32_t>(v)&0x7f800000u;
        return exp!=0x7f800000u&&(exp!=0u||v==0.f);
    };
    //Bounded existing native binary32 convention: separate rounded operations,
    //no FMA/hypot, NaN/overflow/denormal emulation, or unchecked integer casts.
    const auto binary=[&](std::uint32_t pc,char op,float a,float b,float& out)noexcept {
        if(!normalOrZero(a)||!normalOrZero(b)||(op=='/'&&b==0.f))return fail(d,Status::phase,pc);
        double exact=0;volatile float rounded=0;
        switch(op){
        case '+':exact=double(a)+double(b);rounded=a+b;break;
        case '-':exact=double(a)-double(b);rounded=a-b;break;
        case '*':exact=double(a)*double(b);rounded=a*b;break;
        default:exact=double(a)/double(b);rounded=a/b;break;
        }
        if(!normalOrZero(rounded)||(rounded==0.f&&exact!=0.))return fail(d,Status::phase,pc);
        out=rounded;return true;
    };
    const auto root=[&](std::uint32_t pc,float value,float& out)noexcept {
        if(!normalOrZero(value)||value<0.f)return fail(d,Status::phase,pc);
        volatile float rounded=static_cast<float>(std::sqrt(double(value)));
        if(!normalOrZero(rounded))return fail(d,Status::phase,pc);
        out=rounded;return true;
    };
    const auto signedWord=[&](std::uint32_t pc,float value,std::int32_t& out)noexcept {
        if(!normalOrZero(value)||double(value)<-2147483648.||double(value)>=2147483648.)
            return fail(d,Status::phase,pc);
        out=static_cast<std::int32_t>(value);return true; //Retained EE signed truncation contract.
    };
    for(std::size_t at=0x80/4;at<=0x100/4;at+=4)vector(at);
    //Only exact actual48==1 admitted.104140's unrelated zero/multiview paths
    //are not synthesized when an authentic listener cannot be established.
    const auto& backend=*listeners4C_[0]->backend04_;
    for(unsigned lane=0;lane<3;++lane)store(0xf0/4+lane,backend.position10_[lane]); //290E10 XYZ.
    float dx=0,dz=0,acc=0,zz=0,sum=0,distance=0;
    if(!binary(0x104210,'-',fp(scratch[0xf0/4]),fp(actualLocalXYZ[0]),dx))return false;
    dx=std::fabs(dx);store(0x134/4,word(dx));
    if(!binary(0x10422c,'-',fp(scratch[0xf8/4]),fp(actualLocalXYZ[2]),dz))return false;
    if(!binary(0x104230,'*',dx,dx,acc))return false; //MULA.S ACC before ABS.Z store.
    dz=std::fabs(dz);store(0x138/4,word(dz));
    if(!binary(0x104244,'*',dz,dz,zz)||!binary(0x104244,'+',acc,zz,sum)||
       !root(0x104248,sum,distance))return false;
    store(0x13c/4,word(distance));store(0x120/4,word(distance));
    copy(0x80/4,0xf0/4); //104278/28C; self-comparison distance[0]<itself is false.
    for(unsigned lane=0;lane<3;++lane)store(0x90/4+lane,backend.look20_[lane]); //290D70 first XYZ.
    for(unsigned lane=0;lane<3;++lane)store(0xa0/4+lane,backend.up30_[lane]); //Reached despite later dead use.
    for(unsigned lane=0;lane<3;++lane)store(0xe0/4+lane,backend.velocity40_[lane]); //290E90.
    copy(0xc0/4,0x80/4);
    for(unsigned lane=0;lane<3;++lane)store(0xd0/4+lane,actualLocalXYZ[lane]);
    store(0xdc/4,0x3f800000u); //Actual2C205C/22A180 position W survives XYZ copy.
    float displacement[3]{};
    if(!binary(0x104358,'-',fp(scratch[0xd0/4]),fp(scratch[0xc0/4]),displacement[0])||
       !binary(0x104360,'-',fp(scratch[0xd4/4]),fp(scratch[0xc4/4]),displacement[1]))return false;
    store(0xb0/4,word(displacement[0]));
    if(!binary(0x104368,'-',fp(scratch[0xd8/4]),fp(scratch[0xc8/4]),displacement[2]))return false;
    store(0xb4/4,word(displacement[1]));store(0xb8/4,word(displacement[2]));
    //102950 entry compare precedes stack copies; no callback/global effects.
    const bool inRange=distance<200.f;
    copy(80+0x60/4,0xe0/4);
    for(unsigned lane=0;lane<4;++lane)store(80+0x70/4+lane,actualMain70[lane]);
    store(0x12c/4,0xc51c4000u); //1029A0 sentinel.
    store(0x118/4,0);store(0x11c/4,0); //1029A4 signed64 pan.
    store(0x130/4,0); //1029AC branch delay Doppler.
    if(!inRange){store(0x12c/4,0);audible=false;d={Status::complete,0x1043d0};return true;}
    const bool atNear=distance<=0.f;
    float beyond=0;if(!binary(0x1029c8,'-',distance,0.f,beyond))return false; //Delay executes both branches.
    if(atNear)store(0x12c/4,0x42c80000u);
    else {
        float span=0,fraction=0,scaled=0,attenuation=0;
        if(!binary(0x1029e0,'-',200.f,0.f,span)||!binary(0x1029e4,'/',beyond,span,fraction)||
           !binary(0x1029f0,'*',100.f,fraction,scaled)||!binary(0x1029f4,'-',100.f,scaled,attenuation))return false;
        store(0x12c/4,word(attenuation));
    }
    //Existing exact13C1F0 finite quadrant/polynomial convention, NOT atan2.
    //It has only private scalar scratch, no globals, callbacks, errno, or RNG.
    float firstAngle=0,secondAngle=0,degrees=0;std::int32_t first=0,second=0;
    if(!normalOrZero(fp(scratch[0x90/4]))||!normalOrZero(fp(scratch[0x98/4]))||
       !game::originalAngle48G(fp(scratch[0x98/4]),fp(scratch[0x90/4]),firstAngle))
        return fail(d,Status::phase,0x102a00);
    if(!binary(0x102a18,'*',fp(0x42652ee1u),firstAngle,degrees)||!signedWord(0x102a20,degrees,first))return false;
    if(!game::originalAngle48G(displacement[2],displacement[0],secondAngle))return fail(d,Status::phase,0x102a28);
    if(!binary(0x102a40,'*',fp(0x42652ee1u),secondAngle,degrees)||!signedWord(0x102a44,degrees,second))return false;
    const std::int64_t wide=std::int64_t(second)-first;
    if(wide<-2147483648LL||wide>2147483647LL)return fail(d,Status::phase,0x102a50);
    std::int32_t pan=static_cast<std::int32_t>(wide);
    if(pan>180)pan-=359;else if(pan< -180)pan+=359;
    const auto pan64=static_cast<std::uint64_t>(static_cast<std::int64_t>(pan));
    store(0x118/4,static_cast<std::uint32_t>(pan64));store(0x11c/4,static_cast<std::uint32_t>(pan64>>32));
    float direction[3]{};
    if(!binary(0x102aa8,'-',fp(scratch[0x90/4]),displacement[0],direction[0])||
       !binary(0x102ab0,'-',fp(scratch[0x94/4]),displacement[1],direction[1]))return false;
    store(80+0xa0/4,word(direction[0]));
    if(!binary(0x102ab8,'-',fp(scratch[0x98/4]),displacement[2],direction[2]))return false;
    store(80+0xa4/4,word(direction[1]));store(80+0xa8/4,word(direction[2]));store(80+0xac/4,0x3f800000u);
    copy(80+0x80/4,80+0xa0/4);
    float squared[3]{},xy=0,zTerm=0,length2=0,length=0,q=0,normalized[3]{};
    for(unsigned lane=0;lane<3;++lane)
        if(!binary(0x102ad4,'*',direction[lane],direction[lane],squared[lane]))return false;
    if(!binary(0x102adc,'+',squared[0],squared[1],xy)||
       !binary(0x102ae0,'*',1.f,squared[2],zTerm)||!binary(0x102ae0,'+',xy,zTerm,length2))return false;
    if(length2==0.f)q=fp(0x7f7fffffu); //Existing recovered VRSQRT zero-length finite-Q convention.
    else if(!root(0x102ae4,length2,length)||!binary(0x102ae4,'/',1.f,length,q))return false;
    //VWAITQ; VF6.w=VF4.w+VF0.x (known1+0); all used VF/ACC lanes were written.
    for(unsigned lane=0;lane<3;++lane)
        if(!binary(0x102af0,'*',direction[lane],q,normalized[lane]))return false;
    for(unsigned lane=0;lane<3;++lane)store(80+0x80/4+lane,word(normalized[lane]));
    store(80+0x8c/4,0x3f800000u);
    float relative[3]{};
    if(!binary(0x102b14,'-',fp(scratch[80+0x70/4]),fp(scratch[80+0x60/4]),relative[0])||
       !binary(0x102b18,'-',fp(scratch[80+0x74/4]),fp(scratch[80+0x64/4]),relative[1]))return false;
    store(80+0xb0/4,word(relative[0]));
    if(!binary(0x102b20,'-',fp(scratch[80+0x78/4]),fp(scratch[80+0x68/4]),relative[2]))return false;
    store(80+0xb4/4,word(relative[1]));store(80+0xb8/4,word(relative[2]));store(80+0xbc/4,0x3f800000u);
    copy(80+0x90/4,80+0xb0/4);store(80+0x94/4,0);relative[1]=0.f; //102B38 AFTER Y subtraction.
    float product[3]{},dot=0;
    for(unsigned lane=0;lane<3;++lane)
        if(!binary(0x102b44,'*',normalized[lane],relative[lane],product[lane]))return false;
    if(!binary(0x102b4c,'+',product[0],product[1],xy)||
       !binary(0x102b50,'*',1.f,product[2],zTerm)||!binary(0x102b50,'+',xy,zTerm,dot))return false;
    std::int32_t doppler=0;if(!signedWord(0x102b60,dot,doppler))return false;
    store(0x130/4,std::bit_cast<std::uint32_t>(doppler)); //102B70.
    //Actual A1=null skips233AE0 only now, after the complete scalar/VU leaf.
    audible=true;d={Status::complete,0x1043d0};return true;
}
std::shared_ptr<const OriginalApplicationAural50KG>
OriginalApplicationAural50KG::retainColdStreamManager232DB0(
    const std::shared_ptr<E>& app,
    const std::shared_ptr<const assets::OriginalStreamMetadata50KZ>& metadata,
    const std::shared_ptr<const assets::OriginalStandardArchiveProvider50LA>& provider)const noexcept {
    if(!current()||!nativeInitialized_||!backendCurrent()||!same(entities_,app)||
       !issuer_->sourceAural00Owner_||issuer_->sourceAural00Owner_.get()!=this||
       !issuer_->standardMaterialDomainOwner_||!issuer_->standardMaterialDomainOwner_->prepared())return {};
    const auto& main=(**interfaceA0_).main;
    const std::shared_ptr<const assets::OriginalStandardArchiveProvider50LA> canonicalProvider=
        issuer_->standardMaterialDomainOwner_->archiveProvider3E1A70();
    if(!main||!same(main->nativeSourceMetadata_,metadata)||!same(canonicalProvider,provider))return {};
    // current checked issuer lifetime before raw issuer access. Return the
    // actual Application's control block, never shared_ptr(this).
    return issuer_->sourceAural00Owner_;
}
bool OriginalApplicationAural50KG::exactAuralOnlyStartupPrefix()const noexcept {
    if(!current()||attached_||entity_.parent1C||entity_.previous10||entity_.next14||entity_.firstChild18||
       entity_.mask20||entity_.descendants24||(entity_.flags29&15u)||entities_->root10_.firstChild18||
       entities_->objectCount423F60_!=3)return false;
    const auto& a=entities_->entityAuthority_;
    return a.count_==3&&a.enrollmentSerial_==3&&a.nonRootEnrollmentObserved_&&a.auralStartupPrefix_==&entity_&&
        a.entries_[0].node==&entities_->outer26C_&&a.entries_[1].node==&entities_->root10_&&a.entries_[2].node==&entity_;
}
bool OriginalApplicationAural50KG::firstStartupInputsCurrent(const Application& root)noexcept {
    const auto& e=root.entities();std::uint32_t pause=0;
    if(!e||e->nativeOwnerThread_!=std::this_thread::get_id()||!root.animationClock().readPause11C(pause))return false;
    const auto& a=e->entityAuthority_;
    return e->constructed_&&!e->released_&&!e->attachmentLock423F58_&&!e->auralPrefixIssued_&&
        !e->aural00_&&!e->auralNativeOwner_&&e->objectCount423F60_==2&&a.count_==2&&
        a.enrollmentSerial_==2&&!a.nonRootEnrollmentObserved_&&!e->root10_.firstChild18&&
        game::original_entity_chain_detail50LA::forest(a.view());
}
bool OriginalApplicationAural50KG::construct13FE24(const Application& root,
    std::shared_ptr<OriginalApplicationAural50KG>& out,Diagnostic& d)noexcept {
    d={};if(out)return fail(d,Status::phase,0x13FE24);
    const auto e=root.entities();std::uint32_t pause=0;
    if(!e||!root.animationClock().readPause11C(pause))return fail(d,Status::owner,0x13FE24);
    if(e->nativeOwnerThread_!=std::this_thread::get_id())return fail(d,Status::affinity,0x13FE24);
    const auto& a=e->entityAuthority_;
    if(!e->constructed_||e->released_||e->attachmentLock423F58_||e->auralPrefixIssued_||e->aural00_||e->auralNativeOwner_||
       e->objectCount423F60_!=2||a.count_!=2||a.enrollmentSerial_!=2||a.nonRootEnrollmentObserved_||
       e->root10_.firstChild18||!game::original_entity_chain_detail50LA::forest(a.view()))return fail(d,Status::phase,0x13FE24);
    std::shared_ptr<OriginalApplicationAural50KG> next;
    try{next=std::shared_ptr<OriginalApplicationAural50KG>(new OriginalApplicationAural50KG(root,e));}
    catch(...){return fail(d,Status::allocation,0x13FE10);}
    if(!e->entityAuthority_.reserveAdditional(1)||!e->entityAuthority_.canEnroll(next->entity_,key(&next->entity_)))return fail(d,Status::allocation,0x141300);
    const auto init=game::initializeOriginalWreckComponent50LA(next->entity_,K::auralScape102BE0,
        next->generation_,key(&next->entity_),false,e->objectCount423F60_);
    if(init.status!=game::OriginalWreckComponentStatus50LA::complete)std::terminate();
    for(auto& list:next->lists_)list.construct1043E0();
    next->constructed_=true;
    if(!e->entityAuthority_.enrollAuralStartupPrefix(next->entity_,e->outer26C_,e->root10_))std::terminate();
    e->aural00_=&next->entity_;e->auralNativeOwner_=next.get();e->aural00Live_=true;e->auralPrefixIssued_=true;
    out=std::move(next);d={Status::complete,0x13FE2C};return true;
}
bool OriginalApplicationAural50KG::nativeOutputSourceQuiescent()const noexcept {
    // Also used before device-close stops owned ordinary voices. Validate
    // their exact stream custody here, but leave final stopped-slot discharge
    // exclusively to retireNativeOrEmpty102CD0 after real output retirement.
    if((!current()&&!listenerTerminalCleanupCurrent())||!backendCurrent()||!nativeStreamSlotsCurrent())return false;
    const auto& interface=**interfaceA0_;std::array<std::uint64_t,8> references{};
    if(!interface.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,references))return false;
    for(std::size_t n=0;n<references.size();++n)
        if(references[n]&&(!interface.streamSlots[n].occupied||references[n]!=interface.streamSlots[n].generation))return false;
    for(const auto& list:lists_)if(list.count)return false;
    for(const auto* listener:listeners4C_)if(listener)return false;
    for(const auto* group:entities_->soundGroups434A10_)if(group)return false;
    return true;
}
std::shared_ptr<OriginalAudioPendingWaves50KG> OriginalApplicationAural50KG::nativeOutputPendingNamespace()const noexcept {
    return (current()||listenerTerminalCleanupCurrent())?entities_->pendingWaves98A4_:nullptr;
}
bool OriginalApplicationAural50KG::nativeOutputSourceRetired(const Application& root,const C* historical)noexcept {
    const auto entities=root.entities();
    if(!historical||!entities||root.sourceAural00Owner_||!entities->auralPrefixIssued_||
       entities->auralNativeOwner_||entities->aural00Live_||entities->aural00_!=historical||entities->pendingWaves98A4_)
        return false;
    for(const auto* group:entities->soundGroups434A10_)if(group)return false;
    // Historical pointer comparison only. Actual102CD0 already released the
    // native interface and counted Entity; no freed source object is read.
    return true;
}
bool OriginalApplicationAural50KG::backendCurrent(bool permitFailedTransport)const noexcept {
    if(!sourceIdentity()||!interfaceA0_||!*interfaceA0_)return false;
    const auto& i=**interfaceA0_;const auto& b=issuer_->garageAudioBackingOwner_;
    if(!b||!b->inputsCurrent(*issuer_)||(!permitFailedTransport&&b->failed_)||!i.main||i.main.get()!=&b->transport_||
       i.main.owner_before(b)||b.owner_before(i.main)||i.mixer!=&b->mixer_||
       i.mixerEpoch!=b->mixer_.groupEpoch50IC()||i.nativeRate!=b->mixer_.sampleRate()||
       i.nativeCount!=b->mixer_.count()||i.main->mixer_!=i.mixer||!i.main->prepared_||
       i.main->mixerEpoch_!=i.mixerEpoch||i.main->preparedSoundMode_!=b->soundMode_.get()||
       (!permitFailedTransport&&i.main->finalNativePcmRetired_)||
       (i.main->finalNativePcmRetired_&&!i.main->finalNativePcmRetirementCurrent()))return false;
    return i.wetProcessor&&i.wetProcessorBound&&i.wetProcessor->configured()&&
        i.main->nativeWetOwner_==this&&i.main->nativeWetProcessor_==i.wetProcessor.get()&&
        i.main->nativeWetEpoch_==i.mixerEpoch&&i.main->nativeWetInterfaceCurrent()&&
        i.main->nativeWetPlaySong_==&OriginalApplicationAural50KG::playNativeMainSong234170&&
        i.main->nativeWetPlayEffect_==&OriginalApplicationAural50KG::playNativeMainEffect233410&&
        i.main->nativeWetPlayRider_==&OriginalApplicationAural50KG::playNativeMainRider233410&&
        i.main->nativeWetPlayWatto_==&OriginalApplicationAural50KG::playNativeWattoStream233410&&
        i.main->nativeWetCanPublish_==&OriginalApplicationAural50KG::canPublishNativeGarageVoices&&
        i.mixer->nativeSourceWetProcessorCurrent(*this,*i.wetProcessor)&&
        i.mastersApplied&&i.mixer->auralMasterOwner_==this&&i.masterPercent[0]==100&&i.masterPercent[1]==100&&
        i.mixer->auralMasterGain_[0]==1.f&&i.mixer->auralMasterGain_[1]==1.f&&
        entities_->soundMasterSlotsInitialized_&&entities_->soundGroupReserved434A10_[0]&&entities_->soundGroupReserved434A10_[1];
}
bool OriginalApplicationAural50KG::initializeNative102DE0(Diagnostic& d)noexcept {
    d={};if(!exactAuralOnlyStartupPrefix()||initializationAttempted_||interfaceA0_)
        return fail(d,Status::phase,0x102DE0);
    // Concrete native endpoint replaces only267910's platform bootstrap. The
    // source register/list/master effects below still execute on real owners.
    const auto b=issuer_->garageAudioBackingOwner_;
    const auto rejected=[](bool failed,const char* prerequisite) noexcept {
        if(failed)std::fprintf(stderr,"aural_native_startup_rejected prerequisite=%s\n",prerequisite);
        return failed;
    };
    if(rejected(!b,"!b")||
       rejected(!b->inputsCurrent(*issuer_),"!b->inputsCurrent(*issuer_)")||
       rejected(b->failed_,"b->failed_")||
       rejected(b->stage_!=OriginalUiGarageAudioBacking50KG::Stage::transportPrepared,"b->stage_!=OriginalUiGarageAudioBacking50KG::Stage::transportPrepared")||
       rejected(b->mixer_.sampleRate()!=NativeSourceReverb50KG::nativeRate,"b->mixer_.sampleRate()!=NativeSourceReverb50KG::nativeRate")||
       rejected(b->mixer_.mixedFrames(),"b->mixer_.mixedFrames()")||
       rejected(b->transport_.ownsMusic_,"b->transport_.ownsMusic_")||
       rejected(b->transport_.paused_,"b->transport_.paused_")||
       rejected(b->transport_.nativeGroupControlCount_,"b->transport_.nativeGroupControlCount_")||
       rejected(b->mixer_.auralMasterOwner_,"b->mixer_.auralMasterOwner_")||
       rejected(b->mixer_.masterGain()!=1.f,"b->mixer_.masterGain()!=1.f")||
       rejected(b->transport_.nativeWetOwner_,"b->transport_.nativeWetOwner_")||
       rejected(b->transport_.nativeWetProcessor_,"b->transport_.nativeWetProcessor_")||
       rejected(b->transport_.nativeWetEpoch_,"b->transport_.nativeWetEpoch_")||
       rejected(b->transport_.nativeWetSetRawMasters_,"b->transport_.nativeWetSetRawMasters_")||
       rejected(b->transport_.nativeWetPlaySong_,"b->transport_.nativeWetPlaySong_")||
       rejected(b->transport_.nativeWetPlayEffect_,"b->transport_.nativeWetPlayEffect_")||
       rejected(b->transport_.nativeWetPlayRider_,"b->transport_.nativeWetPlayRider_")||
       rejected(b->transport_.nativeWetPlayWatto_,"b->transport_.nativeWetPlayWatto_")||
       rejected(b->transport_.nativeWetCanPublish_,"b->transport_.nativeWetCanPublish_")||
       rejected(b->transport_.nativeWetInterfaceRetired_,"b->transport_.nativeWetInterfaceRetired_")||
       rejected(!b->soundMode_,"!b->soundMode_")||
       rejected(!b->soundMode_->ready(),"!b->soundMode_->ready()")||
       rejected(b->soundMode_->fields().outputMode9900!=1,"b->soundMode_->fields().outputMode9900!=1")||
       rejected(b->soundMode_->fields().reverseAngle9918,"b->soundMode_->fields().reverseAngle9918")||
       rejected(b->soundMode_->revision()==std::numeric_limits<std::uint64_t>::max(),"b->soundMode_->revision()==std::numeric_limits<std::uint64_t>::max()")||
       rejected(!b->mixer_.canRecordControlChanges(2),"!b->mixer_.canRecordControlChanges(2)")||
       rejected(!b->transport_.prepared_,"!b->transport_.prepared_")||
       rejected(!b->transport_.nativeSourceMetadata_,"!b->transport_.nativeSourceMetadata_")||
       rejected(b->transport_.mixer_!=&b->mixer_,"b->transport_.mixer_!=&b->mixer_")||
       rejected(b->transport_.mixerEpoch_!=b->mixer_.groupEpoch50IC(),"b->transport_.mixerEpoch_!=b->mixer_.groupEpoch50IC()")||
       rejected(b->transport_.preparedSoundMode_!=b->soundMode_.get(),"b->transport_.preparedSoundMode_!=b->soundMode_.get()"))
        return fail(d,Status::backend,0x232AA0);
    if(entities_->soundMasterSlotsInitialized_)return fail(d,Status::phase,0x232B18);
    for(std::size_t n=0;n<25;++n)if(entities_->soundGroups434A10_[n]||entities_->soundGroupReserved434A10_[n])
        return fail(d,Status::phase,0x232B18);
    for(std::size_t n=0;n<b->mixer_.count();++n)if(b->mixer_.active(n)||b->mixer_.playbackRevision(n))
        return fail(d,Status::backend,0x267910);
    initializationAttempted_=true;interfaceA0_.emplace(); //102E30 is now known, initially allocation failure/null.
    try{*interfaceA0_=std::make_unique<NativeInterface>(*this);}
    catch(...){return fail(d,Status::allocation,0x102E18);}
    auto& i=**interfaceA0_;
    i.main=std::shared_ptr<OriginalUiMainAudio50KG>(b,&b->transport_);i.mixer=&b->mixer_;
    i.mixerEpoch=b->mixer_.groupEpoch50IC();i.nativeRate=b->mixer_.sampleRate();i.nativeCount=static_cast<std::uint32_t>(b->mixer_.count());
    i.channelCount37C=8; // Incoming13FE40 value, NOT the native mixer voice count.
    try{
        i.handles9C0.reset(new std::uint32_t[8]);i.reuseAge9C8.reset(new std::uint32_t[8]);i.rows9CC.reset(new std::uint64_t[8]);
    }catch(...){return fail(d,Status::allocation,0x26792C);}
    for(unsigned n=0;n<8;++n){i.handles9C0[n]=0;i.reuseAge9C8[n]=0x43340000u;}
    // Actual267910 platform initialization precedes232BD0/232C20 masters.
    // The supplied PS2SOUND startup installs mode107/delay48/feedback24,
    // raw7fff depths and real cleared work RAM on both cores. Retain that
    // numerical producer here, not per SoundGroup or per Garage visit.
    try{i.wetProcessor=std::make_unique<NativeSourceReverb50KG>();}
    catch(...){return fail(d,Status::allocation,0x267910);}
    NativeSourceReverb50KG::Startup wetStartup{};
    wetStartup.outputRate=i.nativeRate;wetStartup.firstFrame=i.mixer->mixedFrames();
    if(!i.wetProcessor->initialize(wetStartup))return fail(d,Status::backend,0x267910);
    // Publish only the identity needed for the private same-mixer/epoch bind.
    // Dry master writes have not happened. A rejected cold bind withdraws this
    // provisional pointer and leaves the real partial interface for cleanup.
    i.mixer->auralMasterOwner_=this;
    if(!i.mixer->bindNativeSourceWetProcessor(*this,*i.wetProcessor,
        &NativeSourceReverb50KG::preflight,&NativeSourceReverb50KG::process)){
        i.mixer->auralMasterOwner_=nullptr;return fail(d,Status::backend,0x267910);
    }
    i.wetProcessorBound=true;
    // Aural strongly retains both Main and the DSP; publish only an exact
    // non-owning control borrow into that same Main after the successful bind.
    // The typed member is issued here, so ordinary Main targets do not acquire
    // an unresolved direct dependency on the numerical implementation TU.
    i.main->nativeWetOwner_=this;i.main->nativeWetProcessor_=i.wetProcessor.get();
    i.main->nativeWetEpoch_=i.mixerEpoch;i.main->nativeWetSetRawMasters_=&NativeSourceReverb50KG::setRawMasters;
    i.main->nativeWetPlaySong_=&OriginalApplicationAural50KG::playNativeMainSong234170;
    i.main->nativeWetPlayEffect_=&OriginalApplicationAural50KG::playNativeMainEffect233410;
    i.main->nativeWetPlayRider_=&OriginalApplicationAural50KG::playNativeMainRider233410;
    i.main->nativeWetPlayWatto_=&OriginalApplicationAural50KG::playNativeWattoStream233410;
    i.main->nativeWetCanPublish_=&OriginalApplicationAural50KG::canPublishNativeGarageVoices;
    // Native synchronous dry master operations. No queue item or wet-output
    // receipt is fabricated. Their two source stores and PCM consumers agree.
    for(std::size_t n=0;n<i.nativeCount;++n)i.mixer->auralMasterBus_[n]=n==OriginalUiMainAudio50KG::musicVoice?1u:0u;
    for(unsigned bus=0;bus<2;++bus){i.masterPercent[bus]=100;i.mixer->auralMasterGain_[bus]=1.f;i.mixer->noteControlChange();}
    i.mastersApplied=true;
    //232B60(1) writes the actual retained mode consumed by native routing. It
    // does not pretend to apply a profile transaction or replace the owner.
    b->soundMode_->fields_.outputMode9900=1;b->soundMode_->fields_.reverseAngle9918=0;
    ++b->soundMode_->revision_;i.initialized28Bit0=true;
    entities_->soundGroupReserved434A10_[0]=entities_->soundGroupReserved434A10_[1]=true;
    entities_->soundMasterSlotsInitialized_=true;
    word38_=0;word3C_=48; //232D40 is literally return48, not host channel availability.
    for(unsigned n=0;n<3;++n)if(!lists_[n].initialize104410()){
        //102EC4 calls actual virtual10 after a list allocation fails. Retire
        // this actual empty owner and its known A0, retaining Main00 identity.
        Diagnostic release;
        if(!retireNativeOrEmpty102CD0(release))std::terminate();
        return fail(d,Status::allocation,0x102E7C+0x18u*n);
    }
    nativeInitialized_=true;flags9CBit0_=false; //13FE5C follows successful setup.
    d={Status::complete,0x13FE5C};return true;
}
bool OriginalApplicationAural50KG::attachNativePrefix140108(Diagnostic& d)noexcept {
    d={};if(!current()||!nativeInitialized_||!backendCurrent()||attached_||entity_.parent1C||entity_.previous10||entity_.next14)
        return fail(d,Status::phase,0x140108);
    // Only this exact bounded native Light predecessor is admitted. Missing
    // retail virtual94/texture-manager Entity are NOT inferred complete.
    const auto* light=entities_->lightManager426F60_;
    const auto& a=entities_->entityAuthority_;
    if(!light||entities_->root10_.firstChild18!=light||light->next14||light->previous10||light->firstChild18||
       light->parent1C!=&entities_->root10_||light->kind!=K::lightManager147ED0||
       a.count_!=4||a.enrollmentSerial_!=4||entities_->objectCount423F60_!=4)
        return fail(d,Status::topology,0x140108);
    const auto r=entities_->entityAuthority_.attachChildren(entities_->root10_,entity_,0xffffffffu,0);
    if(r.status!=game::OriginalWreckComponentStatus50LA::complete)return fail(d,Status::topology,r.sourcePC);
    attached_=true;d={Status::complete,0x140108};return true;
}
bool OriginalApplicationAural50KG::releaseRegistered103D00(C& emitter,Diagnostic& d)noexcept {
    // Native gameplay's private AI list has not yet transferred these real
    // Entity owners here. Refuse BEFORE1046D0/detach/release rather than erase
    // a registration without its actual virtual10 destructor. A future typed
    // producer must add its concrete preflight/release branch here first.
    (void)emitter;return fail(d,Status::registeredEmitter,0x103DA8);
}
bool OriginalApplicationAural50KG::restartOwnedList103210(const Application& root,
    const std::shared_ptr<E>& expected,std::optional<std::int32_t>& indexS0,
    std::optional<std::uint32_t>& count5C,const C*& reachedEmitter,Diagnostic& d)noexcept {
    d={};
    if(!expected||expected->nativeOwnerThread_!=std::this_thread::get_id())
        return fail(d,Status::affinity,0x1ac944);
    //Read the actual App00 owner, never infer null from missing native audio.
    const auto held=root.sourceAural00Owner_;
    if(root.entities()!=expected||!held||held->issuer_!=&root||held->entities_!=expected||
       !held->sourceIdentity()||expected->attachmentLock423F58_||
       !held->lists_[0].constructed)
        return fail(d,Status::owner,0x1ac944);
    indexS0=0; //103228 branch delay slot, BEFORE the first actual5C count read.
    count5C=held->lists_[0].count; //103250. No cache/storage/child scan first.
    if(*indexS0>=std::bit_cast<std::int32_t>(*count5C)){
        d={Status::complete,0x10326c};return true;
    }
    //Only a reached positive list requires indexed backing. Preserve the
    //actual iterator/count/entry even though no typed child callback exists.
    if(!held->lists_[0].valid())return fail(d,Status::topology,0x104750);
    reachedEmitter=held->lists_[0].at104750(static_cast<std::uint32_t>(*indexS0));
    return fail(d,reachedEmitter?Status::registeredEmitter:Status::topology,0x103238);
}
bool OriginalApplicationAural50KG::clear103DD0(Diagnostic& d)noexcept {
    d={};if(!current()||clearCalls_==std::numeric_limits<std::uint64_t>::max())return fail(d,Status::owner,0x103DD0);
    // Every iteration uses the REAL retained list, not a temporary empty list
    // or native_original_ai_audio::Owner's shutdown. No A0/backend mutation.
    while(lists_[0].count){
        auto* emitter=lists_[0].at104750(0);
        if(!emitter)return fail(d,Status::topology,0x103DF0);
        if(!releaseRegistered103D00(*emitter,d))return false;
    }
    ++clearCalls_;d={Status::complete,0x103E14};return true;
}
bool OriginalApplicationAural50KG::canActivateUiInitializer1B7340(Diagnostic& d)const noexcept {
    d={};if(!current()||!nativeInitialized_||!backendCurrent()||!attached_||
       entity_.parent1C!=&entities_->root10_)return fail(d,Status::owner,0x1B7340);
    // Actual Aural table3CC900+20 is BASE141560, including1414C0 notification.
    // This reached native owner is childless; never replace a nonempty real
    // notification tree with an activity bit or an assumed no-op.
    if(entity_.firstChild18)return fail(d,Status::topology,0x1414CC);
    d={Status::complete,0x1B7340};return true;
}
bool OriginalApplicationAural50KG::activateUiInitializer1B7340(Diagnostic& d)noexcept {
    if(!canActivateUiInitializer1B7340(d))return false;
    game::OriginalWreckComponent50LA* const node=&entity_;
    const game::OriginalWreckComponentDomain50LA reached{entity_.sessionGeneration,&node,1};
    const auto activated=game::setOriginalWreckBaseActivity50LA(reached,entity_,true);
    if(activated.status!=game::OriginalWreckComponentStatus50LA::complete)
        return fail(d,Status::topology,activated.sourcePC);
    d={Status::complete,0x141598};return true;
}
bool OriginalApplicationAural50KG::canInitializeUiReverb232C70(Diagnostic& d)const noexcept {
    d={};
    // Source1B7364 is a separate reached interface command, after activation
    // but before any UI sound. Preflight is also legal before that activation.
    if(!current()||!nativeInitialized_||!backendCurrent()||!attached_||
       entity_.parent1C!=&entities_->root10_||entity_.firstChild18||entity_.descendants24)
        return fail(d,Status::owner,0x1B7364);
    const auto& i=**interfaceA0_;const auto& main=*i.main;
    if(i.mixer->mixedFrames()||i.wetProcessor->nextFrame()||i.streamGeneration||
       main.nativeGroupControlCount_||main.nativeGroupControlSequence_||main.nativeWetMasterCount_||
       main.nativeWetMasterSequence_||main.nativeWetMasterFrame_||main.nativeWetMasterPercent_||main.nativeWetMasterRaw_||
       !main.canRecordControlChange()||!i.mixer->canRecordControlChanges())
        return fail(d,Status::phase,0x26A820);
    for(std::size_t n=0;n<i.nativeCount;++n)
        if(i.mixer->active(n)||i.mixer->playbackRevision(n))return fail(d,Status::phase,0x26A820);
    d={Status::complete,0x1B7364};return true;
}
bool OriginalApplicationAural50KG::initializeUiReverb232C70(Diagnostic& d)noexcept {
    if(!canInitializeUiReverb232C70(d))return false;
    if((entity_.flags29&0x0du)!=1u)return fail(d,Status::phase,0x1B7340);
    auto& i=**interfaceA0_;auto& main=*i.main;
    // Exact232C70(AuralA0,0,0,-1,-1): absolute zero, optional parameters
    // untouched. Native synchronous command2 shares Main's real ordered
    // control sequence with later command25/Page command2 publications.
    const std::array<NativeSourceReverb50KG::RawMaster,2> zero{{{0,0},{0,0}}};
    if(!i.wetProcessor->setRawMasters(zero,i.mixer->mixedFrames()))
        return fail(d,Status::backend,0x26A820);
    ++main.nativeWetMasterCount_;main.nativeWetMasterSequence_=++main.nativeGroupControlSequence_;
    main.nativeWetMasterFrame_=i.mixer->mixedFrames();main.nativeWetMasterPercent_=0;main.nativeWetMasterRaw_=0;
    i.mixer->noteControlChange();main.noteControlChange();
    // No fallible work after the numerical write. Startup history, allpass/
    // FIR phase, clock and dry gains remain intact; this is not DSP completion.
    d={Status::complete,0x232D0C};return true;
}
bool OriginalApplicationAural50KG::canInitializeUiReentryReverb232C70(Diagnostic& d)const noexcept {
    d={};
    // Application alone issues this branch from a new1B7300, after proving
    // the prior dispatcher/dialog retirement. It may preflight before the
    // new counted dispatcher exists; that constructor owns the no-replay
    // boundary, not a persistent readiness bit on Aural or its PCM backing.
    if(!current()||!nativeInitialized_||!backendCurrent()||!attached_||
       entity_.parent1C!=&entities_->root10_||entity_.firstChild18||entity_.descendants24||
       issuer_->sourceAural00Owner_.get()!=this||issuer_->frontendAudioShutdownComplete_||
       issuer_->garageCustodyOwner_||!nativeOutputSourceQuiescent())
        return fail(d,Status::owner,0x1B7364);
    const auto& b=issuer_->garageAudioBackingOwner_;
    const auto& i=**interfaceA0_;const auto& main=*i.main;
    if(b->stage_!=OriginalUiGarageAudioBacking50KG::Stage::transportPrepared||
       !b->frontendOutputAttempted_||b->frontendOutputClosed_||b->frontendOutputCloseFailed_||
       b->frontendOutputAccountingFailed_||b->frontendOutputSerial_<=1||
       b->frontendOutputAuralEntity_!=&entity_||!b->frontendOutputOwner_)
        return fail(d,Status::owner,0x1B7364);
    const auto& output=*b->frontendOutputOwner_;
    const auto transport=output.transport();const auto info=output.info();
    const auto frame=i.mixer->mixedFrames();
    // prepareFrontendAudioOutput already consumed the genuine device-close
    // receipt before rebinding this same transport. Rebind changes control
    // revisions, so do not demand the now-consumed old revision snapshot.
    // Its stopped playback/PCM cursors and cumulative frame remain exact.
    if(!output.enabled()||!transport||transport.get()!=i.main.get()||
       transport.owner_before(b)||b.owner_before(transport)||
       output.retainedOutputSerial()!=b->frontendOutputSerial_||
       output.sourceFrameOrigin()!=frame||output.sourceMixedFrames()!=frame||
       b->frontendClosedSourceFrames_!=frame||output.acceptedNanoseconds()||
       output.remainderNanoseconds()||output.lastPcmDrain().prepared||
       info.sampleRate!=i.nativeRate||info.simulationMicroseconds||info.mixedFrames||info.capturedFrames||
       info.deviceQueuedFrames||info.deviceConsumedFrames||info.deviceOverflowFrames)
        return fail(d,Status::phase,0x26A820);
    // Only normal initial/retired Garage source ownership can cross a real
    // frontend output rebind. Terminal failed-entry families remain closed.
    if(!main.current()||!main.reconstructibleGarageRowsCurrent()||
       (main.garageSourceLifetime_!=OriginalUiMainAudio50KG::GarageSourceLifetime::initialPrefix&&
        main.garageSourceLifetime_!=OriginalUiMainAudio50KG::GarageSourceLifetime::retired)||
       main.nativeGroupControlCount_||i.mixer->nativeWattoRouting_.command||i.mixer->nativeSceneRouting_.command||
       main.ownsMusic_||main.ownsRiderConfirm_||main.musicSong_||main.fading_||
       !entities_->pendingWaves98A4_||!entities_->pendingWaves98A4_->current()||
       i.wetProcessor->nextFrame()!=frame||!main.nativeWetMasterCount_||
       !main.nativeWetMasterSequence_||main.nativeWetMasterSequence_>main.nativeGroupControlSequence_||
       main.nativeWetMasterFrame_>frame||
       main.nativeWetMasterCount_==std::numeric_limits<std::uint64_t>::max()||
       main.nativeGroupControlSequence_==std::numeric_limits<std::uint64_t>::max()||
       !main.canRecordControlChange(1)||!i.mixer->canRecordControlChanges(1))
        return fail(d,Status::phase,0x26A820);
    for(const auto& command:main.nativeGroupControls_)
        if(command)return fail(d,Status::phase,0x26A820);
    for(bool owned:main.ownsEffect_)if(owned)return fail(d,Status::phase,0x26A820);
    for(std::size_t n=0;n<i.nativeCount;++n){
        AudioVoiceObservation50HD voice;
        if(!i.mixer->inspectVoice(n,voice)||voice.voice.active)
            return fail(d,Status::phase,0x26A820);
        if((n<OriginalUiMainAudio50KG::courseRequiredVoices||n>=OriginalUiMainAudio50KG::garageSceneVoiceEnd)&&
           (voice.playbackRevision!=b->frontendClosedPlaybackRevisions_[n]||
            voice.voice.sourceFrame!=b->frontendClosedSourceFramesByVoice_[n]))
            return fail(d,Status::phase,0x26A820);
    }
    d={Status::complete,0x1B7364};return true;
}
bool OriginalApplicationAural50KG::initializeUiReentryReverb232C70(Diagnostic& d)noexcept {
    if(!canInitializeUiReentryReverb232C70(d))return false;
    if((entity_.flags29&0x0du)!=1u)return fail(d,Status::phase,0x1B7340);
    auto& i=**interfaceA0_;auto& main=*i.main;
    // The real repeated232C70(AuralA0,0,0,-1,-1), after1B7340. Apply at
    // the unchanged cumulative sample boundary, never rebuild/configure or
    // clear the processor, advance voices, rewind clocks or erase history.
    const std::array<NativeSourceReverb50KG::RawMaster,2> zero{{{0,0},{0,0}}};
    if(!i.wetProcessor->setRawMasters(zero,i.mixer->mixedFrames()))
        return fail(d,Status::backend,0x26A820);
    ++main.nativeWetMasterCount_;main.nativeWetMasterSequence_=++main.nativeGroupControlSequence_;
    main.nativeWetMasterFrame_=i.mixer->mixedFrames();main.nativeWetMasterPercent_=0;main.nativeWetMasterRaw_=0;
    i.mixer->noteControlChange();main.noteControlChange();
    // Numerical write is the last fallible operation. Existing work/FIR,
    // stream ages/generations, clips, cursors and output accounting survive.
    d={Status::complete,0x232D0C};return true;
}
bool OriginalApplicationAural50KG::finishNativeControls267BE0(Diagnostic& d)noexcept {
    d={};
    if(!current()||issuer_->sourceApplicationMode22C_!=0)return fail(d,Status::owner,0x267BE0);
    return submitNativeControls267BE0(d);
}
bool OriginalApplicationAural50KG::canFinishGaragePageInput267BE0(
    const OriginalUiGaragePageOwner50KG& page,Diagnostic& d)const noexcept {
    d={};
    if(!current()||!backendCurrent()||!nativeStreamSlotsCurrent()||
       !attached_||entity_.parent1C!=&entities_->root10_)return fail(d,Status::owner,0x267BE0);
    const auto& i=**interfaceA0_;const auto& main=*i.main;
    if(!main.nativeGaragePageInputAudioCurrent(page)||main.nativeWetOwner_!=this||
       i.controlBoundaryCount==std::numeric_limits<std::uint64_t>::max()||
       !main.precedingGroupControlsCurrent()||main.pendingNativeGroupControls()||
       !main.canRecordControlChange(0u)||!i.mixer->canRecordControlChanges(0u))
        return fail(d,Status::backend,0x267BE0);
    std::array<std::uint64_t,8> references{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,references))
        return fail(d,Status::backend,0x267BE0);
    for(std::size_t n=0;n<references.size();++n)
        if(references[n]&&(!i.streamSlots[n].occupied||references[n]!=i.streamSlots[n].generation))
            return fail(d,Status::backend,0x267BE0);
    return true;
}
bool OriginalApplicationAural50KG::finishGaragePageInput267BE0(
    const OriginalUiGaragePageOwner50KG& page,std::uint64_t& completedBoundary,Diagnostic& d)noexcept {
    completedBoundary=0;
    if(!canFinishGaragePageInput267BE0(page,d)||!submitNativeControls267BE0(d))return false;
    // Caller records this only after the real Page effect returned. The same
    // boundary count may never be used to replay its preceding input effects.
    completedBoundary=(**interfaceA0_).controlBoundaryCount;return true;
}
bool OriginalApplicationAural50KG::submitNativeControls267BE0(Diagnostic& d)noexcept {
    d={};
    if(!current()||!backendCurrent()||!nativeStreamSlotsCurrent()||
       !attached_||entity_.parent1C!=&entities_->root10_)
        return fail(d,Status::owner,0x267BE0);
    auto& i=**interfaceA0_;auto& main=*i.main;
    if(i.controlBoundaryCount==std::numeric_limits<std::uint64_t>::max()||
       !main.current()||!main.precedingGroupControlsCurrent())
        return fail(d,Status::backend,0x267BE0);
    const auto frame=i.mixer->mixedFrames();
    std::array<std::uint64_t,8> references{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,references))
        return fail(d,Status::backend,0x267BE0);
    for(std::size_t n=0;n<references.size();++n)
        if(references[n]&&(!i.streamSlots[n].occupied||references[n]!=i.streamSlots[n].generation))
            return fail(d,Status::backend,0x267BE0);
    // Retail267BE0 submits its active EE command buffer via266A10 and clears
    // the newly selected packet. The native implementation has no guest RPC
    // buffers: actual play/stop/master calls already publish synchronously.
    // Retained command25 objects are strong owner custody, not pending work.
    // Only their real unapplied routing writes need this ordered consumer.
    std::string detail;
    if(!main.serviceColdGarageRouting(detail))return fail(d,Status::backend,0x266A10);
    if(main.pendingNativeGroupControls()||!main.precedingGroupControlsCurrent()||
       !nativeStreamSlotsCurrent()||i.mixer->mixedFrames()!=frame||i.wetProcessor->nextFrame()!=frame)
        return fail(d,Status::backend,0x267C3C);
    ++i.controlBoundaryCount;i.controlBoundaryFrame=frame;
    i.controlBoundaryMainRevision=main.controlRevision_;
    i.controlBoundaryMixerRevision=i.mixer->controlRevision();
    i.controlBoundaryGroupSequence=main.nativeGroupControlSequence_;
    // No output/device drain, SourceSound233E00,267CB0 or FIR processing is
    // part of267BE0. The enclosing caller retains this exact native receipt.
    d={Status::complete,0x267CA4};return true;
}
bool OriginalApplicationAural50KG::nativeStreamSlotsCurrent(bool permitRetired)const noexcept {
    if(!backendCurrent(permitRetired))return false;
    const auto& i=**interfaceA0_;
    if(i.channelCount37C!=8||!i.handles9C0||!i.reuseAge9C8||!i.rows9CC||i.streamSelector9C4!=-1)return false;
    for(std::size_t n=0;n<i.streamSlots.size();++n){const auto& slot=i.streamSlots[n];
        if(i.handles9C0[n]!=std::uint32_t(slot.occupied)||
           i.reuseAge9C8[n]!=std::bit_cast<std::uint32_t>(slot.age)||
           !std::isfinite(slot.age)||slot.age<0.f)return false;
        if(!slot.generation){if(slot.occupied||slot.metadata||slot.age!=180.f)return false;continue;}
        if(slot.generation>i.streamGeneration||!slot.metadata)return false;
        // Strong Main custody outlives replaced/explicitly stopped native PCM.
        // Do not compare against the most recent Jukebox handle or revision.
        const auto* row=slot.kind==NativeInterface::StreamKind::song?
            i.main->nativeSongSourceCurrent(slot.sourceOrdinal,slot.clip):
            slot.kind==NativeInterface::StreamKind::riderConfirm?
            i.main->nativeRiderSourceCurrent(slot.sourceOrdinal,slot.clip):
            slot.kind==NativeInterface::StreamKind::watto?
            i.main->nativeWattoStreamSourceCurrent(slot.sourceOrdinal,slot.clip):
            slot.kind==NativeInterface::StreamKind::raceIntro&&slot.raceAssets?
            slot.raceAssets->source(slot.sourceOrdinal,slot.clip):nullptr;
        if(row!=slot.metadata)return false;
    }
    return true;
}
bool OriginalApplicationAural50KG::nativeStreamFinalRetirementCurrent()const noexcept {
    if(!nativeStreamSlotsCurrent(true))return false;
    const auto& i=**interfaceA0_;
    if(i.streamRetirementAuthorized||(i.streamGeneration&&!i.main->ordinaryOutputRetired_))return false;
    // Root's real device-close/ordinary retirement precedes Pending teardown.
    // EOF or a quiet decoder alone never permits final stream-slot discharge.
    for(std::size_t n=0;n<i.nativeCount;++n)
        if(i.mixer->active(n)||i.mixer->nativeSourceVoiceRoutes_[n].count)return false;
    std::array<std::uint64_t,8> references{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,references))return false;
    for(std::size_t n=0;n<references.size();++n)
        if(references[n]&&(!i.streamSlots[n].occupied||references[n]!=i.streamSlots[n].generation))return false;
    // Exact no-effects release preconditions before listener destructor prefix.
    // No callback/yield occurs between this proof and release after destruction.
    if(!i.wetProcessor||!i.mixer->canReleaseNativeSourceWetProcessor(*this,*i.wetProcessor))return false;
    // Mixer release independently checks every reserved pool cell and claim.
    // Unreserved stream-bit cells/occupied slots may persist until this actual
    // interface dies; neither its final guard nor this observation ages them.
    return true;
}
bool OriginalApplicationAural50KG::prepareNativeStreamStart(OriginalUiMainAudio50KG& main,
    NativeInterface::StreamKind kind,std::uint32_t ordinal,const AudioClip50HD& clip,bool loop,
    std::array<NativeInterface::StreamSlot,8>& next,AudioMixer50HD::NativeSourceStreamStart50KG& start)const noexcept {
    if(!nativeStreamSlotsCurrent()||(**interfaceA0_).raceIntroLease||(**interfaceA0_).main.get()!=&main)return false;
    const bool music=kind==NativeInterface::StreamKind::song;
    // Songs loop (234900 mode4) except UIWin Song10/Song8, which 234880 plays
    // once (mode1). All non-song streams are single plays.
    const bool expectedLoop=music&&!songPlaysOnce234880(ordinal);
    if(loop!=expectedLoop||clip.channels!=(music?2u:1u))return false;
    const auto* row=music?main.nativeSongSourceCurrent(ordinal,clip):
        kind==NativeInterface::StreamKind::riderConfirm?main.nativeRiderSourceCurrent(ordinal,clip):
        kind==NativeInterface::StreamKind::watto?main.nativeWattoStreamSourceCurrent(ordinal,clip):nullptr;
    if(!row)return false;
    const auto& i=**interfaceA0_;
    if(i.streamGeneration>std::numeric_limits<std::uint64_t>::max()-clip.channels)return false;
    next=i.streamSlots;start={};start.count=static_cast<std::uint8_t>(clip.channels);
    start.duration=row->admittedDurationSeconds;start.loop=loop;
    for(std::size_t channel=0;channel<clip.channels;++channel){
        std::size_t selected=next.size();float best=0.f;
        for(std::size_t n=0;n<next.size();++n)if(!next[n].occupied){
            // Exact266B70: zero best selects every free candidate; positive
            // equal ages retain the earlier candidate. Never steal a slot.
            if(best==0.f||next[n].age>best){selected=n;best=next[n].age;}
        }
        if(selected==next.size())return false;
        auto& slot=next[selected];slot.kind=kind;slot.sourceOrdinal=ordinal;
        slot.generation=i.streamGeneration+channel+1;slot.metadata=row;slot.clip=clip;
        slot.occupied=true;slot.age=0.f;
        start.slots[channel]=static_cast<std::uint8_t>(selected);start.generations[channel]=slot.generation;
    }
    return true;
}
void OriginalApplicationAural50KG::commitNativeStreamSlots(
    const std::array<NativeInterface::StreamSlot,8>& next)const noexcept {
    auto& i=**interfaceA0_;i.streamSlots=next;
    for(std::size_t n=0;n<next.size();++n){
        i.handles9C0[n]=std::uint32_t(next[n].occupied);
        i.reuseAge9C8[n]=std::bit_cast<std::uint32_t>(next[n].age);
        if(next[n].generation>i.streamGeneration)i.streamGeneration=next[n].generation;
    }
    // Native typed rows above retain pitch/duration authoring. Do not fill
    // unknown raw9CC group fields with fabricated source group IDs.
}
bool OriginalApplicationAural50KG::playNativeMainSong234170(OriginalUiMainAudio50KG& main,
    std::uint32_t song,const AudioClip50HD& clip,float gain,float pitch)const noexcept {
    std::array<NativeInterface::StreamSlot,8> next;AudioMixer50HD::NativeSourceStreamStart50KG start;
    // 234170 passes loop=(stream+40==4) to 269650: 234900 writes mode4 (loop),
    // 234880 writes mode1 (single play). Only UIWin Song10/Song8 use 234880.
    const bool loop=!songPlaysOnce234880(song);
    if(!prepareNativeStreamStart(main,NativeInterface::StreamKind::song,song,clip,loop,next,start))return false;
    auto& i=**interfaceA0_;
    // Existing actual Main native bus policy: Song gain40% on dry master1.
    // This does not invent original UI Group+50/registry3 or a command25.
    const float expectedPitch=song==6?OriginalUiGarageWattoAudio50KG::nativeRendererPitch:1.f;
    if(gain!=.4f||pitch!=expectedPitch||i.mixer->auralMasterBus_[0]!=1u||
       i.mixer->hasVoiceGroupGain50IC(0))return false;
    // Native atomic admission: failed preflight leaves the outgoing native
    // voice/slots intact; this is NOT a fabricated original success receipt.
    if(!i.mixer->playNativeSourceOrdinaryStream(*this,OriginalUiMainAudio50KG::musicVoice,clip,gain,pitch,start))return false;
    commitNativeStreamSlots(next);return true;
}
bool OriginalApplicationAural50KG::playNativeMainEffect233410(OriginalUiMainAudio50KG& main,
    std::size_t index,const AudioClip50HD& clip,float gain,float pitch)const noexcept {
    if(!nativeStreamSlotsCurrent()||(**interfaceA0_).raceIntroLease||(**interfaceA0_).main.get()!=&main)return false;
    AudioMixer50HD::NativeSourceSampleStart50KG start;
    if(!main.nativeEffectSourceCurrent(index,clip,start))return false;
    auto& i=**interfaceA0_;const auto voice=OriginalUiMainAudio50KG::effectVoiceForClip(index);
    if(gain!=1.f||pitch!=1.f||i.mixer->auralMasterBus_[voice]!=0u||i.mixer->hasVoiceGroupGain50IC(voice))return false;
    return i.mixer->playNativeSourceOrdinarySample(*this,voice,clip,gain,pitch,start);
}
bool OriginalApplicationAural50KG::playNativeMainRider233410(OriginalUiMainAudio50KG& main,
    std::size_t index,const AudioClip50HD& clip,float gain,float pitch)const noexcept {
    if(index>std::numeric_limits<std::uint32_t>::max())return false;
    std::array<NativeInterface::StreamSlot,8> next;AudioMixer50HD::NativeSourceStreamStart50KG start;
    if(!prepareNativeStreamStart(main,NativeInterface::StreamKind::riderConfirm,
        static_cast<std::uint32_t>(index),clip,false,next,start))return false;
    auto& i=**interfaceA0_;
    constexpr auto voice=OriginalUiMainAudio50KG::riderConfirmVoice;
    if(gain!=1.f||pitch!=1.f||i.mixer->auralMasterBus_[voice]!=0u||i.mixer->hasVoiceGroupGain50IC(voice))return false;
    if(!i.mixer->playNativeSourceOrdinaryStream(*this,voice,clip,gain,pitch,start))return false;
    commitNativeStreamSlots(next);return true;
}
bool OriginalApplicationAural50KG::playNativeWattoStream233410(OriginalUiMainAudio50KG& main,
    const void* command,std::size_t index,std::size_t voice,const AudioClip50HD& clip,
    std::int32_t volume,float pitch,std::int32_t angle)const noexcept {
    if(index>std::numeric_limits<std::uint32_t>::max()||volume!=100||angle||
       pitch!=OriginalUiGarageWattoAudio50KG::nativeRendererPitch||
       !main.nativeWattoStreamStartCurrent(index,voice,clip,command))return false;
    std::array<NativeInterface::StreamSlot,8> next;AudioMixer50HD::NativeSourceStreamStart50KG start;
    if(!prepareNativeStreamStart(main,NativeInterface::StreamKind::watto,
        static_cast<std::uint32_t>(index),clip,false,next,start))return false;
    auto& i=**interfaceA0_;
    if(!i.mixer->playNativeSourceStreamVolume(command,*this,voice,clip,volume,pitch,angle,start))return false;
    commitNativeStreamSlots(next);return true;
}
bool OriginalApplicationAural50KG::previewNativeStreamSuffix(float factor,
    std::array<NativeInterface::StreamSlot,8>& next)const noexcept {
    if(!nativeStreamSlotsCurrent()||!std::isfinite(factor)||factor<0.f)return false;
    const auto& i=**interfaceA0_;std::array<std::uint64_t,8> before{},after{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,factor,false,before)||
       !i.mixer->nativeSourceStreamReferences267CB0(*this,factor,true,after))return false;
    next=i.streamSlots;
    for(std::size_t n=0;n<next.size();++n){auto& slot=next[n];
        if((before[n]&&(!slot.occupied||before[n]!=slot.generation))||
           (after[n]&&(!slot.occupied||after[n]!=slot.generation)))return false;
        // Channel cleanup267F7C..267FA8 precedes the suffix. A newly released
        // slot takes free-age268144 this visit; a later orphan2682BC does not.
        if(before[n]&&!after[n])slot.occupied=false;
        if(!slot.occupied&&slot.age<180.f){
            const float advanced=slot.age+factor;if(!std::isfinite(advanced))return false;slot.age=advanced;
        }
        if(slot.occupied&&!after[n])slot.occupied=false; //9C4 is proven -1.
    }
    return true;
}
bool OriginalApplicationAural50KG::boundedSourceBodyCurrent()const noexcept {
    if(word30_||word34_||word38_||word3C_!=48||word40_||word44_||word48_||
       word98_!=-1||entities_->diagnostic14_||entity_.firstChild18||entity_.descendants24)return false;
    for(const auto& list:lists_)if(!list.valid()||list.count)return false;
    for(const auto* listener:listeners4C_)if(listener)return false;
    return true;
}
bool OriginalApplicationAural50KG::canPublishNativeGarageVoices(const OriginalUiMainAudio50KG& main,
    std::size_t maximumMonoPlays)const noexcept {
    if(!current()||!backendCurrent()||!nativeStreamSlotsCurrent()||(**interfaceA0_).raceIntroLease||(**interfaceA0_).main.get()!=&main||
       !attached_||entity_.parent1C!=&entities_->root10_||(entity_.flags29&0x0du)!=1u||
       !(entity_.mask20&2u)||!boundedSourceBodyCurrent()||
       !issuer_->nativeSourceAudioDispatcherCurrent(*this))return false;
    const auto& i=**interfaceA0_;std::array<NativeInterface::StreamSlot,8> next;
    if(maximumMonoPlays>i.streamSlots.size()||
       !i.mixer->canServiceNativeSourceVoices267CB0(*this,0.f)||!previewNativeStreamSuffix(0.f,next)||
       !i.mixer->canReserveNativeSourceMonoVoices(maximumMonoPlays))return false;
    std::size_t free=0;for(const auto& slot:i.streamSlots)if(!slot.occupied)++free;
    // Conservative canExecute bound before shared RNG/source mutations: every
    // requested play could be a mono response. A later actual wing is a sample.
    // No speculative stop or preview aging can manufacture extra free slots.
    return free>=maximumMonoPlays;
}
bool OriginalApplicationAural50KG::canUpdate103720(float factor,Diagnostic& d)const noexcept {
    d={};if(!current()||!backendCurrent()||!attached_||entity_.parent1C!=&entities_->root10_||
       (entity_.flags29&0x0du)!=1u||!(entity_.mask20&2u))return fail(d,Status::owner,0x103720);
    if((**interfaceA0_).raceIntroLease||!std::isfinite(factor)||factor<0.f||issuer_->sourceApplicationMode22C_!=2||
       !issuer_->sourceApplicationElapsed23C_||std::bit_cast<std::uint32_t>(factor)!=
       std::bit_cast<std::uint32_t>(*issuer_->sourceApplicationElapsed23C_))
        return fail(d,Status::phase,0x103758);
    // Preflight the whole supported body before its first source effect.
    // No invented listener/emitter traversal, capacity/eviction or debug draw.
    if(word30_||word38_||word3C_!=48||word40_||word44_||
       !listenerCustodyCurrent(false)||word98_!=-1||entities_->diagnostic14_||
       entity_.firstChild18||entity_.descendants24)
        return fail(d,Status::phase,0x103280);
    for(const auto& list:lists_)if(list.count)return fail(d,Status::registeredEmitter,0x103280);
    for(std::uint32_t n=0;n<word48_;++n)
        if(!listeners4C_[n]||!listeners4C_[n]->current(*this))return fail(d,Status::phase,0x103790);
    const auto& i=**interfaceA0_;
    std::array<NativeInterface::StreamSlot,8> next;
    if(!i.mixer->canServiceNativeSourceVoices267CB0(*this,factor)||!previewNativeStreamSuffix(factor,next))
        return fail(d,Status::backend,0x267CB0);
    d={Status::complete,0x103758};return true;
}
bool OriginalApplicationAural50KG::update103720(float factor,Diagnostic& d)noexcept {
    if(!canUpdate103720(factor,d))return false;
    auto& i=**interfaceA0_;
    std::array<NativeInterface::StreamSlot,8> next;
    if(!previewNativeStreamSuffix(factor,next)||!i.mixer->serviceNativeSourceVoices267CB0(*this,factor))
        return fail(d,Status::backend,0x267CB0);
    commitNativeStreamSlots(next); // All suffix failure checks preceded channel mutation.
    //103760 ->103280: empty5C skips both passes. Actual1047C0 writes the
    // two empty list counts without discarding allocated backing/capacity.
    lists_[1].count=0;lists_[2].count=0; //1032E8/1032F0
    // Exact0+48-0 capacity admits no eviction. Source103780 reaches each
    // typed listener's290C70 ->235890 literal return1 after the list stores.
    for(std::uint32_t n=0;n<word48_;++n)
        if(!listeners4C_[n]->update290C70())return fail(d,Status::phase,0x103780);
    // Empty84, null14 and childless inherited141ED0 take their real paths. This is not a
    // general active-source service or playback-readiness receipt.
    d={Status::complete,0x103B04};return true;
}
bool OriginalApplicationAural50KG::retireNativeOrEmpty102CD0(Diagnostic& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id())return fail(d,Status::affinity,0x102CD0);
    if(retired_)return true;
    //Actual manager removal must withdraw its native issuer before App teardown.
    //Never destroy a listener still borrowed by a live same-source manager.
    if(nativeListenerIssuer_||sourceListenerReservation_.issuer||!currentIncludingIncompleteListener()||entity_.firstChild18||entity_.next14||!entities_->entityAuthority_.canRetire(entity_))return fail(d,Status::phase,0x102CD0);
    if(attached_?(entity_.parent1C!=&entities_->root10_):(entity_.parent1C||entity_.previous10))
        return fail(d,Status::topology,0x142590);
    // Final102CD0 owns every actually published typed slot, including partial
    // initialization; unlike103040 it does NOT decrement34/48 or null-store4C.
    // Preflight backend retirement below before this irreversible prefix.
    if(!listenerCustodyCurrent(true))return fail(d,Status::phase,0x102D14);
    for(const auto& list:lists_)if(list.count)return fail(d,Status::registeredEmitter,0x102D60);
    if(interfaceA0_&&*interfaceA0_){auto& i=**interfaceA0_;
        if(i.mastersApplied){
            if(!backendCurrent(true)||!nativeStreamFinalRetirementCurrent()||
               i.main->nativeGroupControlCount_||entities_->pendingWaves98A4_)
                return fail(d,Status::backend,0x232A38);
            for(const auto* group:entities_->soundGroups434A10_)if(group)return fail(d,Status::phase,0x232A38);
            for(std::size_t n=0;n<i.nativeCount;++n)if(i.mixer->active(n))return fail(d,Status::backend,0x232A38);
            // Release the borrowed numerical producer while the exact Aural
            // identity, interface and strong processor owner are still alive.
            // Mixer release independently requires all voice/route claims gone.
            // A rejection changes neither master ownership nor stored history.
            if(!i.wetProcessor||!i.wetProcessorBound)return fail(d,Status::backend,0x232A38);
            nativeListenerPhase_=NativeListenerPhase::retiring;
            for(auto& listener:nativeListenerOwners_)listener.reset(); //102D08..2C beforeA0.
            // Source4C values remain historical identities, never dereferenced.
            if(!i.mixer->releaseNativeSourceWetProcessor(*this,*i.wetProcessor))
                return fail(d,Status::backend,0x232A38);
            i.wetProcessorBound=false;i.streamRetirementAuthorized=true;
            // Withdraw the Main control borrow after Mixer has stopped using
            // the DSP, before destroying its owning interface or Main alias.
            i.main->nativeWetPlaySong_=nullptr;i.main->nativeWetPlayEffect_=nullptr;
            i.main->nativeWetPlayRider_=nullptr;i.main->nativeWetPlayWatto_=nullptr;i.main->nativeWetCanPublish_=nullptr;
            i.main->nativeWetSetRawMasters_=nullptr;i.main->nativeWetProcessor_=nullptr;
            i.main->nativeWetOwner_=nullptr;i.main->nativeWetEpoch_=0;i.main->nativeWetInterfaceRetired_=true;
            i.mixer->auralMasterOwner_=nullptr;i.mastersApplied=false;
        }
        // Earlier allocation/startup/bind failures may retain an unbound
        // processor, but a live borrower can never be silently destroyed.
        if(i.wetProcessorBound||(i.main&&(i.main->nativeWetOwner_||i.main->nativeWetProcessor_||
           i.main->nativeWetEpoch_||i.main->nativeWetSetRawMasters_||i.main->nativeWetPlaySong_||i.main->nativeWetPlayEffect_||i.main->nativeWetPlayRider_||i.main->nativeWetPlayWatto_||i.main->nativeWetCanPublish_)))return fail(d,Status::backend,0x232A38);
        // Release actual native interface arrays/transport borrow, never reset
        // Main while another source owner still pins it. This is the actual
        // interface retirement where retained wet history is finally destroyed.
        // Root retires the separately owned backing later.
        if(nativeListenerPhase_!=NativeListenerPhase::retiring){
            nativeListenerPhase_=NativeListenerPhase::retiring;
            for(auto& listener:nativeListenerOwners_)listener.reset();
        }
        // Explicit native cancellation of an unissued/partial source library,
        // not26DB20, Config1573B0 or a source loaded-flag reset.
        if(!i.sourceStreamLibrary.closeNative())return fail(d,Status::phase,0x2668e0);
        interfaceA0_->reset();
    }
    if(nativeListenerPhase_!=NativeListenerPhase::retiring){
        nativeListenerPhase_=NativeListenerPhase::retiring;
        for(auto& listener:nativeListenerOwners_)listener.reset();
    }
    for(std::size_t n=3;n;--n)lists_[n-1].storage.reset(); //102D60/74/88 order.
    if(attached_){
        if(entity_.parent1C!=&entities_->root10_)return fail(d,Status::topology,0x142590);
        if(entity_.previous10)entity_.previous10->next14=entity_.next14;
        else entities_->root10_.firstChild18=entity_.next14;
    }else if(entity_.parent1C||entity_.previous10)return fail(d,Status::topology,0x142590);
    if(!entities_->entityAuthority_.retire(entity_))std::terminate();
    --entity_.reference00;--entities_->objectCount423F60_;
    entities_->auralNativeOwner_=nullptr;entities_->aural00Live_=false;retired_=true;
    //Source Application00 remains its stale identity, never forged null.
    d={Status::complete,nativeInitialized_?0x102D94u:0u};return true;
}
OriginalApplicationAural50KG::Observation OriginalApplicationAural50KG::inspect()const noexcept {
    Observation o;if(thread_!=std::this_thread::get_id())return o;
    o.entity=constructed_?&entity_:nullptr;o.constructed=constructed_;o.attached=attached_;o.retired=retired_;
    o.interfaceKnown=interfaceA0_.has_value();o.nativeInitialized=nativeInitialized_;o.sourceCapacity3C=word3C_;o.clearCalls=clearCalls_;
    for(std::size_t n=0;n<3;++n){const auto& l=lists_[n];o.lists[n]={l.count,l.growth,l.capacity,l.storage.get(),l.constructed,l.allocatedBit0};}
    if(interfaceA0_&&*interfaceA0_){const auto& i=**interfaceA0_;o.interfaceA0=&i;o.nativeMain=i.main.get();o.mixer=i.mixer;
        o.nativeVoiceCount=i.nativeCount;o.masterPercent=i.masterPercent;o.masterControlsApplied=i.mastersApplied;
        o.wetProcessor=i.wetProcessor.get();
        if(i.wetProcessor){o.wetProcessorConfigured=i.wetProcessor->configured();o.wetProcessorNextFrame=i.wetProcessor->nextFrame();
            o.wetProcessorBound=i.wetProcessorBound&&i.mixer&&i.mixer->nativeSourceWetProcessorCurrent(*this,*i.wetProcessor);}}
    // Numerical ownership is not actual source-routed wet playback support.
    // wetOutputSupported remains false until the real route/producer is joined.
    return o;
}
}

namespace original_main_entry50KG {
bool Application::nativeSourceAudioDispatcherCurrent(const rr::platform::OriginalApplicationAural50KG& aural)const noexcept {
    const auto& scope=sharedUiScopeOwner_;
    const auto rejected=[](bool failed,const char* prerequisite) noexcept {
        if(failed)std::fprintf(stderr,"aural_dispatcher_rejected prerequisite=%s\n",prerequisite);
        return failed;
    };
    if(rejected(!scope,"!scope")||
       rejected(sourceAural00Owner_.get()!=&aural,"sourceAural00Owner_.get()!=&aural")||
       rejected(aural.issuer_!=this,"aural.issuer_!=this")||
       rejected((aural.entities_!=entitiesOwner_||aural.entities_.owner_before(entitiesOwner_)||entitiesOwner_.owner_before(aural.entities_)),"(aural.entities_!=entitiesOwner_||aural.entities_.owner_before(entitiesOwner_)||entitiesOwner_.owner_before(aural.entities_))")||
       rejected(scope->issuer_!=this,"scope->issuer_!=this")||
       rejected((scope->application_!=entitiesOwner_||scope->application_.owner_before(entitiesOwner_)||entitiesOwner_.owner_before(scope->application_)),"(scope->application_!=entitiesOwner_||scope->application_.owner_before(entitiesOwner_)||entitiesOwner_.owner_before(scope->application_))")||
       rejected(!scope->live().okay(),"!scope->live().okay()")||
       rejected(!scope->dispatcherLive_,"!scope->dispatcherLive_"))return false;
    const auto& dispatcher=scope->dispatcher_;
    // The genuine loading manager330 is constructed before dispatcher startup.
    // Its initialized frontend tree is inactive (1415A0), but remains linked
    // between Aural and the dispatcher. Authenticate that exact owner rather
    // than accepting an arbitrary intermediate entity or requiring adjacency.
    const auto& loading=sourceLoading330Owner_;
    const auto* predecessor=&aural.entity_;
    if(loading){
        if(loading->issuer_!=this||!loading->prefixRetained()||!loading->managerInitialized_||
           !loading->managerAttached_||loading->manager_.previous10!=&aural.entity_||
           aural.entity_.next14!=&loading->manager_||loading->manager_.next14!=&dispatcher||
           !scope->linked(loading->manager_,&entitiesOwner_->root10_))return false;
        predecessor=&loading->manager_;
    }
    return !rejected(dispatcher.parent1C!=&entitiesOwner_->root10_,"dispatcher.parent")&&
        !rejected(dispatcher.previous10!=predecessor,"dispatcher.previous")&&
        !rejected(predecessor->next14!=&dispatcher,"dispatcher.predecessor.next")&&
        !rejected(!scope->linked(dispatcher,&entitiesOwner_->root10_),"dispatcher.linked");
}
bool Application::stageAuralCold13FE24(std::shared_ptr<rr::platform::OriginalApplicationAural50KG>& out,
    rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(out||sourceAural00Owner_){d={O::Status::phase,0x13FE24};return false;}
    if(!O::construct13FE24(*this,out,d))return false;
    sourceAural00Owner_=out;return true;
}
bool Application::initializeAuralNative102DE0(rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(!sourceAural00Owner_||sourceAural00Owner_->issuer_!=this){d={O::Status::owner,0x102DE0};return false;}
    return sourceAural00Owner_->initializeNative102DE0(d);
}
bool Application::attachAuralNativePrefix140108(rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(!sourceAural00Owner_||sourceAural00Owner_->issuer_!=this){d={O::Status::owner,0x140108};return false;}
    return sourceAural00Owner_->attachNativePrefix140108(d);
}
bool Application::clearAuralRegistrations103DD0(rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(!sourceAural00Owner_||sourceAural00Owner_->issuer_!=this){d={O::Status::owner,0x27015C};return false;}
    return sourceAural00Owner_->clear103DD0(d);
}
bool Application::releaseAuralCold102CD0(rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(!sourceAural00Owner_||sourceAural00Owner_->issuer_!=this){d={O::Status::owner,0x102CD0};return false;}
    if(!sourceAural00Owner_->retireNativeOrEmpty102CD0(d))return false;
    sourceAural00Owner_.reset();return true;
}
}

namespace original_main_entry50KG {
bool Application::stageAuralLightNativePrefix13FE24(std::uint32_t nativeOutputRate,
    rr::platform::OriginalApplicationAural50KG::Diagnostic& d,std::string& error)const noexcept {
    using A=rr::platform::OriginalApplicationAural50KG;
    d={};error.clear();
    // This is a coherent private native join, not original main(argc) startup
    // or ordinary Garage admission. Every failed published prefix stays Root
    // owned for explicit cleanup; no destructor silently invents a rollback.
    std::shared_ptr<A> aural;
    if(!stageAuralCold13FE24(aural,d))return false;
    if(!prepareGarageAudioBackingCold(nativeOutputRate,error)){
        d={A::Status::backend,0x102E4C};return false;
    }
    if(!initializeAuralNative102DE0(d))return false;
    std::shared_ptr<rr::platform::OriginalUiLightManagerOwner50KG> light;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic ld;
    if(!stageLightManagerAfterAuralNative140074(light,ld)){
        d={A::Status::phase,ld.sourcePC};return false;
    }
    return attachAuralNativePrefix140108(d);
}
}

#include "native_race_intro_transport50la.inl"

#include "original_stream_library_source50la.inl"
