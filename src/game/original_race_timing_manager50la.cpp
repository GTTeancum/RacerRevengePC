#include "original_race_timing_manager50la.h"
#include "original_application_entities50la.h"
#include "original_ai_manager_finish.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/original_race_hud_scope50la.h"
#include "../../tests/native/original_camera_session50kx.h"
#include "../../tests/native/terrain_interactive_continuation50du.h"
#include "../../tests/native/original_consumed_ra_stage50du.h"
#include "../../tests/native/ra_cold_aggregate50la.h"
#include "../../tests/native/native_ai_session50iv_fixture.h"
#include <exception>
#include <limits>
#include <optional>
#include <thread>
#include <utility>

namespace rr::game {
struct OriginalRaceTimingManager50LA::Impl final {
    const std::thread::id thread=std::this_thread::get_id();
    std::shared_ptr<OriginalApplicationEntities50LA> application;
    OriginalWreckComponent50LA component;
    native_ai50IV::Session* native=nullptr;
    const native_racer50HN::Bank* bank=nullptr;
    const OriginalRaceConstructor50LA* race48=nullptr;
    const TrackNavigation50EU* map=nullptr;
    const AiTrackMetadata50IU* metadata=nullptr;
    bool counted=false,constructed=false,closed=false;
    //Source34 has no existing native counterpart. It is not the native30
    //clock, Race94 countdown or Race98 fade timer; no duplicate clock runs.
    float scalar34=0.f;
    std::optional<std::uint32_t> count4C;
    bool sourceArraysNullKnown=true; //Ctor3C/40/44 only; withdrawn at adapter.
    std::uint8_t flags58KnownMask=0x5fu; //Ctor does NOT initialize bits5/7.
    enum class Cut:std::uint8_t {subtype,race,nativeRows,map,nativeReady};
    Cut cut=Cut::subtype;
    bool componentCurrent()const noexcept {
        return thread==std::this_thread::get_id()&&counted&&!closed&&application&&
            application->nativeOwnerThread_==thread&&application->application10()&&
            !application->attachmentLock423F58()&&
            component.kind==OriginalWreckComponentKind50LA::entity141300&&component.reference00==1&&
            component.sessionGeneration&&component.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&component))&&
            !component.parent1C&&!component.previous10&&!component.next14&&!component.firstChild18&&
            !component.mask20&&!component.descendants24&&component.category28==0xffu&&
            (component.flags29&15u)==3u&&application->entityAuthority_.canRetire(component);
    }
    Impl(std::shared_ptr<OriginalApplicationEntities50LA> app,std::uint64_t generation,
        native_ai50IV::Session& actual,const native_racer50HN::Bank& actualBank)
        :application(std::move(app)),native(&actual),bank(&actualBank) {
        if(!application||application->nativeOwnerThread_!=thread||!application->application10()||
           application->attachmentLock423F58()||!generation||actual.managerBound||actual.rankBound||
           actual.manager.time30!=0.f||actual.manager.flags58!=0u||actual.retired40.bound||
           actual.finished44.count54||actual.retired40.activeCount50||actual.trackLength38!=0.f||
           actual.all8FinishAwaitingDisplay||actual.all8ResultsComplete||!actualBank.count()||
           actualBank.count()>actual.finished44.managerRows.size()||
           application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return;
        auto& authority=application->entityAuthority_;
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&component));
        if(!authority.reserveAdditional(1)||!authority.canEnroll(component,key))return;
        const auto made=initializeOriginalWreckComponent50LA(component,
            OriginalWreckComponentKind50LA::entity141300,generation,key,true,
            application->objectCount423F60()); //1AF7FC: Entity141300(a1=1).
        if(made.status!=OriginalWreckComponentStatus50LA::complete)return;
        counted=true;if(!authority.enroll(component))std::terminate();
        //Concrete native type represents3CEF80. Exact reached constructor
        //stores, against the sole actual live lanes; bit7 is preserved, UNKNOWN.
        actual.manager.time30=0.f; //1AF810
        actual.manager.flags58=std::uint8_t(actual.manager.flags58&~1u);
        actual.manager.flags58=std::uint8_t(actual.manager.flags58&~2u);
        actual.manager.flags58=std::uint8_t(actual.manager.flags58&~0x18u);
        sourceArraysNullKnown=true; //1AF870/874/878:3C/44/40=null.
        scalar34=0.f; //1AF87C
        actual.manager.flags58=std::uint8_t(actual.manager.flags58&~4u);
        actual.retired40.activeCount50=0; //1AF894, same native50 owner.
        actual.manager.flags58=std::uint8_t(actual.manager.flags58|0x40u); //1AF8A0/4
        constructed=true;
    }
};
OriginalRaceTimingManager50LA::OriginalRaceTimingManager50LA(
    std::shared_ptr<OriginalApplicationEntities50LA> app,std::uint64_t generation,
    native_ai50IV::Session& native,const native_racer50HN::Bank& bank)
    :impl_(new Impl(std::move(app),generation,native,bank)){}
OriginalRaceTimingManager50LA::~OriginalRaceTimingManager50LA()noexcept {
    std::uint32_t pc=0;if(impl_&&!retireNativeDetached(pc))std::terminate();
}
bool OriginalRaceTimingManager50LA::constructed()const noexcept {
    return impl_&&impl_->componentCurrent()&&impl_->constructed;
}
bool OriginalRaceTimingManager50LA::continueNativeAssociation(OriginalRaceConstructor50LA& race,
    native_ai50IV::Session& actualNative,const native_racer50HN::Bank& actualBank,
    const TrackNavigation50EU& actualMap,const AiTrackMetadata50IU& actualMetadata,
    std::uint32_t& pc)noexcept {
    pc=0x1a7754;if(!impl_)return false;auto& p=*impl_;
    if(!p.componentCurrent()||!p.constructed||race.timingB4_.get()!=this||
       race.application_!=p.application||race.entity_.sessionGeneration!=p.component.sessionGeneration||
       p.native!=&actualNative||p.bank!=&actualBank)return false;
    auto& actual=*p.native;
    if(p.cut==Impl::Cut::subtype){
        //Publication1A7730 has ALREADY happened.1AF960's141C20 is return-self;
        //only actual Profile11258==1 writes bit5. No caller mode substitutes.
        platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selection;
        if(!race.profile_||!race.profile_->courseRaceSnapshot(selection))return false;
        actual.manager.flags58=std::uint8_t((actual.manager.flags58&~0x20u)|
            (selection.subtype==1u?0x20u:0u)); //1AF9A4/8
        p.flags58KnownMask|=0x20u;p.cut=Impl::Cut::race;
    }
    if(p.cut==Impl::Cut::race){
        pc=0x1afaf4;p.race48=&race;p.cut=Impl::Cut::nativeRows;
    }
    if(p.cut==Impl::Cut::nativeRows){
        pc=0x1afaf8;
        if(actual.managerBound||actual.rankBound||!p.bank->count()||
           p.bank->count()>actual.finished44.managerRows.size())return false;
        //Explicit NATIVE backing replacement for B0 count/getter, allocated
        //C90-pointer arrays and each racer virtual74. The existing registered
        //Bank owns all genuine cold-created racers and native manager rows;
        //no guest array, fresh216690 return or second racer reset is invented.
        p.count4C=static_cast<std::uint32_t>(p.bank->count());
        p.sourceArraysNullKnown=false;
        //Existing list storage is used in-place in original manager order,
        //not rank/display order. Constructor-null guest pointers are now unknown.
        for(std::size_t i=0;i<*p.count4C;++i){
            actual.retired40.managerRows[i]=0;actual.finished44.managerRows[i]=0;
        }
        //Native reset scalar/list portion after the accepted cold-owner
        //adapter, exact1AFD04..1AFD5C order. Never run again on a live manager.
        actual.manager.flags58=std::uint8_t(actual.manager.flags58&~0x18u);
        actual.manager.flags58=std::uint8_t(actual.manager.flags58&~2u);
        actual.manager.flags58=std::uint8_t(actual.manager.flags58&~1u);
        actual.manager.flags58=std::uint8_t(actual.manager.flags58&~4u);
        actual.manager.time30=0.f;p.scalar34=0.f;
        actual.finished44.count54=0;actual.retired40.activeCount50=*p.count4C;
        actual.retired40.bound=true; //Native custody, not a source field.
        p.map=&actualMap;p.metadata=&actualMetadata;p.cut=Impl::Cut::map;
    }
    if(p.map!=&actualMap||p.metadata!=&actualMetadata||p.race48!=&race)return false;
    if(p.cut==Impl::Cut::map){
        pc=0x1afb98;
        if(!actualMetadata.bound||actualMap.track.count<2u)return false;
        //Actual retained RA MAP B8 scalar source; pure arithmetic shared with
        //legacy native initialization. Failure retains constructor/list effects.
        if(calculateOriginalAiTrackLength38(actualMetadata.nodes[1].distanceToFinish60,
            actualMap.track.nodes[0].position,actualMap.track.nodes[1].position,
            actual.trackLength38)!=OriginalAiFinishStatus::Complete)return false;
        p.cut=Impl::Cut::nativeReady;
    }
    pc=0x1a7768;return true; //Native cold-adapter successor ONLY, not1AFAE0 return.
}
bool OriginalRaceTimingManager50LA::readNativeFlags(const OriginalRaceConstructor50LA& race,
    const native_ai50IV::Session& actual,const native_racer50HN::Bank& bank,std::uint8_t& value)const noexcept {
    if(!impl_)return false;const auto& p=*impl_;
    //Caller first proves actual World/Session/Bank lifetime and lexical Scope;
    //neither a nonnull B4 nor this cursor alone authorizes dereferencing backing.
    if(!p.componentCurrent()||p.cut!=Impl::Cut::nativeReady||p.race48!=&race||
       race.timingB4_.get()!=this||race.application_!=p.application||
       race.entity_.sessionGeneration!=p.component.sessionGeneration||p.native!=&actual||p.bank!=&bank||
       !p.count4C||*p.count4C!=bank.count()||!actual.managerBound||!actual.retired40.bound||
       actual.retired40.activeCount50>*p.count4C||actual.finished44.count54>*p.count4C||
       !(p.flags58KnownMask&1u))return false;
    value=actual.manager.flags58;return true; //ONE per-occurrence1C2134 byte read.
}
bool OriginalRaceTimingManager50LA::readQueuedEventEnable58(const OriginalRaceConstructor50LA& race,
    const native_ai50IV::Session& actual,const native_racer50HN::Bank& bank,bool& enabled)const noexcept {
    if(!impl_||(impl_->flags58KnownMask&2u)!=2u)return false;
    std::uint8_t flags=0;
    if(!readNativeFlags(race,actual,bank,flags))return false;
    //1A4CA4/CA8: DSLL32 sa30 then DSRL32 sa31 selects bit1, NOT bit0.
    enabled=(flags&2u)!=0;return true;
}
bool OriginalRaceTimingManager50LA::readEndCallerMode(const OriginalRaceConstructor50LA& race,
    const native_ai50IV::Session& actual,const native_racer50HN::Bank& bank,
    std::uint8_t& value)const noexcept {
    // Source1A7F68/6C extracts bits3..4, NOT native finish bit0 or a host mode.
    if(!impl_||(impl_->flags58KnownMask&0x18u)!=0x18u)return false;
    std::uint8_t flags=0;
    if(!readNativeFlags(race,actual,bank,flags))return false;
    value=std::uint8_t((flags>>3u)&3u);return true;
}
bool OriginalRaceTimingManager50LA::readAssociationHeadCD8(const OriginalRaceConstructor50LA& race,
    const native_ai50IV::Session& actual,const native_racer50HN::Bank& bank,
    const std::uint8_t& actualArenaCD8,bool& finished)const noexcept {
    std::uint8_t unused=0;
    if(!readNativeFlags(race,actual,bank,unused))return false;
    // The accepted nativeRows adapter is the existing association producer:
    //1AFB44..78 stores getter-index i's C90 at3C[i]. Source reset/update/rank
    //do not sort3C. No retirement40 or finish44 list substitutes here.
    // This private reference comes ONLY from the same Camera's actual arena.
    // A Bank handle retains its registered span/offset, not a copied flag.
    try {
        const auto lane=bank.bindRead(actualArenaCD8);
        const auto flags=bank.read(0,lane); //1A8078/7C:3C[0]+48 == racerCD8.
        finished=(flags&1u)!=0;return true;
    }catch(...){return false;}
}
bool OriginalRaceTimingManager50LA::readAssociatedCount30(const OriginalRaceConstructor50LA& race,
    const native_ai50IV::Session& actual,const native_racer50HN::Bank& bank,
    std::int32_t& count)const noexcept {
    std::uint8_t unused=0;
    if(!readNativeFlags(race,actual,bank,unused))return false;
    // Same existing native B0 getter/count replacement used at association,
    // not B4 retirement activeCount50, finish count54, or host standings.
    count=static_cast<std::int32_t>(bank.count());return true;
}
bool OriginalRaceTimingManager50LA::retireNativeDetached(std::uint32_t& pc)noexcept {
    pc=0x1af8c0;if(!impl_)return true;auto& p=*impl_;
    if(p.thread!=std::this_thread::get_id())return false;
    if(p.closed)return true;
    if(p.counted){
        if(!p.componentCurrent()||!p.application->objectCount423F60()||
           !p.application->entityAuthority_.retire(p.component))return false;
        --p.component.reference00;--p.application->objectCount423F60();
    }
    //Native detached-count withdrawal only. Borrowed Bank/Setup may already
    //have retired: do NOT dereference/reset them or fabricate1AF8C0 array frees.
    p.native=nullptr;p.bank=nullptr;p.race48=nullptr;p.map=nullptr;p.metadata=nullptr;
    p.closed=true;return true;
}

bool OriginalRaceConstructor50LA::initializeNativeTimingB4(
    ::interactive_continuation50DU::Context& context,::native_racer50HN::Bank& bank,
    std::uint32_t& pc)noexcept {
    pc=0x1a7710;
    if(resultsFeedbackThread_!=std::this_thread::get_id())return false;
    //This is the actual EXISTING native cold manager producer, not permission
    //to skip the incomplete original1A6BF0/world/controller/attachment tails.
    if(!context.isRaTrack||!context.originalAiEnabled||context.consumedRaStage.race().get()!=this||
       context.originalMainApplication.race28C()!=this||application_!=context.originalMainApplication.entities()||
       !counted_||rolledBack_||!world74_||world74_!=context.raWorld||
       world74_->terminal_||world74_->closed_||!world74_->runtimeQueries_||!world74_->source_)return false;
    const auto& runtime=*world74_->runtimeQueries_;
    if(runtime.terminal_||runtime.closed_||!runtime.cameraBankCurrent()||runtime.bankIdentity_!=&bank)return false;
    const auto* source=world74_->source_->sourcesDecoded();
    if(!source||context.navigationMap!=&source->map||!source->mapMetadata.bound||
       source->map.track.count<2u||!profile_||
       profile_!=context.originalMainApplication.riderProfile)return false;
    try {
        if(!timingB4_){
            std::unique_ptr<OriginalRaceTimingManager50LA> next(new OriginalRaceTimingManager50LA(
                application_,entity_.sessionGeneration,context.originalAiSession,bank));
            if(!next->constructed()){pc=0x1af7f0;return false;}
            timingB4_=std::move(next); //Actual1A7730 publication BEFORE subtype/load.
        }
        return timingB4_->continueNativeAssociation(*this,context.originalAiSession,bank,
            source->map,source->mapMetadata,pc);
    }catch(...){return false;} //Retain published B4/count and any reached effects.
}
bool OriginalRaceConstructor50LA::readResultsManagerFlags58(
    const ::original_camera_session50KX::Session& session,std::uint8_t& value,
    OriginalResults104Diagnostic50LA& d)const noexcept {
    d={};d.status=OriginalResults104Diagnostic50LA::Status::owner;d.sourcePC=0x1c2124;
    d.operation="actual Race B4 manager flags58";
    if(resultsFeedbackThread_!=std::this_thread::get_id())return false;
    if(!timingB4_||!ownsNativeCameraAdapter34(session))return false;
    const auto scope=endHud_.lock();
    if(!scope||!scope->endStateCurrent(*this,session)||!scope->bank_||!scope->manager_||
       !timingB4_->readNativeFlags(*this,*scope->manager_,*scope->bank_,value))return false;
    d={};return true;
}
bool OriginalRaceConstructor50LA::retireNativeTimingB4(std::uint32_t& pc)noexcept {
    pc=0x1af8c0;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!endHud_.expired())return false;
    if(!timingB4_)return true;
    if(!timingB4_->retireNativeDetached(pc))return false;
    timingB4_.reset();return true;
}
}

namespace original_race_hud_scope50LA {
bool Registration::readEndAssociationHead1A8074(
    const rr::game::OriginalRaceConstructor50LA& race,bool& finished)const noexcept {
    if(std::this_thread::get_id()!=thread_||!camera_||!bank_||!manager_||
       !endStateCurrent(race,*camera_)||!race.timingB4_)return false;
    return race.timingB4_->readAssociationHeadCD8(race,*manager_,*bank_,
        camera_->actual_.arena.fields.impactFlagsCD8,finished);
}
bool Registration::readEndAssociatedCount1A80AC(
    const rr::game::OriginalRaceConstructor50LA& race,std::int32_t& count)const noexcept {
    if(std::this_thread::get_id()!=thread_||!camera_||!bank_||!manager_||
       !endStateCurrent(race,*camera_)||!race.timingB4_)return false;
    return race.timingB4_->readAssociatedCount30(race,*manager_,*bank_,count);
}
}
