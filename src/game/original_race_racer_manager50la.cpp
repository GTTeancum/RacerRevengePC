#include "original_race_racer_manager50la.h"
#include "original_application_entities50la.h"
#include "ai_runtime50iv.h"
#include "vehicle_frame49w.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/original_race_hud_scope50la.h"
#include "../../tests/native/original_camera_session50kx.h"
#include "../../tests/native/native_racer_bank50hn_fixture.h"
#include <array>
#include <exception>
#include <limits>
#include <optional>
#include <thread>
#include <utility>

namespace rr::game {
struct OriginalRaceRacerManager50LA::Impl final {
    const std::thread::id thread=std::this_thread::get_id();
    std::shared_ptr<OriginalApplicationEntities50LA> application;
    OriginalWreckComponent50LA component;
    native_racer50HN::Bank* bank=nullptr;
    VehicleFrameState49W* frame=nullptr;
    AiRuntimeState50IV* ai=nullptr;
    const std::uint32_t* humanCount=nullptr;
    const OriginalRaceConstructor50LA* race=nullptr;
    bool counted=false,constructed=false,closed=false,associated=false;
    // Exact constructor-known lanes1AD464..474. Native adaptation withdraws
    // null-array knowledge; it NEVER manufactures guest array addresses.
    std::uint32_t count30=0,humans34=0,opponents38=0;
    bool arrays3C44ConstructorNullKnown=true;
    //1AD440 does not initialize40. No record pointer/value exists here.
    native_racer50HN::Bank::ReadHandle<std::uint32_t> vehicle58,managerIndex;
    native_racer50HN::Bank::ReadHandle<bool> initialized,physicalPending;
    enum class RestartCut:std::uint8_t {unreached,aiLoop,firstRacer,placement,stopped};
    RestartCut cut=RestartCut::unreached;
    std::optional<std::uint32_t> resetA2;
    std::uint32_t indexS0=0,stopPC=0x1aea70;
    std::optional<std::int32_t> firstRankA1;
    // Reached1AEB20/28 SQ VF0 locals only, not persistent racer pose storage.
    std::array<std::uint32_t,4> position50{},heading60{};
    std::optional<std::uint32_t> nextNode98; //Actual2A2880 output, not live C88.

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
        native_racer50HN::Bank& actualBank,VehicleFrameState49W& actualFrame,
        AiRuntimeState50IV& actualAi,const std::uint32_t& actualHumans)
        :application(std::move(app)),bank(&actualBank),frame(&actualFrame),
         ai(&actualAi),humanCount(&actualHumans) {
        if(!application||application->nativeOwnerThread_!=thread||!application->application10()||
           application->attachmentLock423F58()||!generation||!bank->count()||
           bank->count()>8u||actualHumans>1u||actualHumans>bank->count()||
           application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return;
        // Bind every mutated scalar before constructing a counted owner. These
        // are registration proofs, not copies; no Bank visit/AI reset occurs.
        vehicle58=bank->bindRead(frame->fields.vehicle58);
        initialized=bank->bindRead(ai->initialized);
        physicalPending=bank->bindRead(ai->physicalReset2255B0Pending);
        managerIndex=bank->bindRead(ai->managerIndexFE0);
        const auto registered=[&](const auto& value){(void)bank->bindRead(value);};
        registered(ai->state.aiStateFE8);registered(ai->referenceFF8);
        registered(ai->packing.scaleFFC);registered(ai->packing.policy1000);
        registered(ai->target1008Present);registered(ai->timer100C);
        registered(ai->timer1010);registered(ai->timer1014);
        registered(ai->repair.cooldown1018);registered(ai->repair.duration101C);
        registered(ai->mode1050);registered(ai->navigation.choice1080);
        registered(ai->navigation.choice1084);registered(ai->field104C);
        registered(ai->boost.cooldown109C);registered(ai->state.stallTimer1090);
        registered(ai->state.wrongWayTimer1094);registered(ai->state.turnSide1098);
        registered(frame->bodies.main.controls.player.flagsB9C);
        registered(ai->state.orientation10A0);registered(ai->repair.repairFEC);
        auto& authority=application->entityAuthority_;
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&component));
        if(!authority.reserveAdditional(1)||!authority.canEnroll(component,key))return;
        const auto made=initializeOriginalWreckComponent50LA(component,
            OriginalWreckComponentKind50LA::entity141300,generation,key,true,
            application->objectCount423F60()); //1AD450:141300(A1=1).
        if(made.status!=OriginalWreckComponentStatus50LA::complete)return;
        counted=true;if(!authority.enroll(component))std::terminate();
        // Concrete native type represents3CEF00. No40 store occurs.
        arrays3C44ConstructorNullKnown=true;count30=humans34=opponents38=0;
        constructed=true;
    }
};

OriginalRaceRacerManager50LA::OriginalRaceRacerManager50LA(
    std::shared_ptr<OriginalApplicationEntities50LA> app,std::uint64_t generation,
    native_racer50HN::Bank& bank,VehicleFrameState49W& frame,
    AiRuntimeState50IV& ai,const std::uint32_t& humans)
    :impl_(new Impl(std::move(app),generation,bank,frame,ai,humans)){}
OriginalRaceRacerManager50LA::~OriginalRaceRacerManager50LA()noexcept {
    std::uint32_t pc=0;if(impl_&&!retireNativeDetached(pc))std::terminate();
}
bool OriginalRaceRacerManager50LA::constructed()const noexcept {
    return impl_&&impl_->componentCurrent()&&impl_->constructed;
}
bool OriginalRaceRacerManager50LA::associateNativeRoster(OriginalRaceConstructor50LA& race,
    native_racer50HN::Bank& bank,VehicleFrameState49W& frame,AiRuntimeState50IV& ai,
    const std::uint32_t& humans,std::uint32_t& pc)noexcept {
    pc=0x1aa564;if(!impl_)return false;auto& p=*impl_;
    if(!p.componentCurrent()||!p.constructed||p.associated||race.racerB0_.get()!=this||
       race.application_!=p.application||race.entity_.sessionGeneration!=p.component.sessionGeneration||
       p.bank!=&bank||p.frame!=&frame||p.ai!=&ai||p.humanCount!=&humans||
       !bank.count()||bank.count()>8u||humans>1u||humans>bank.count())return false;
    try {
        for(std::size_t i=0;i<bank.count();++i){
            if(bank.read(i,p.vehicle58)!=(i<humans?0u:1u))return false;
            if(i>=humans&&(!bank.read(i,p.initialized)||bank.read(i,p.managerIndex)!=i||
               bank.read(i,p.physicalPending)))return false;
        }
    }catch(...){return false;}
    // Native association of the SAME genuine roster, deliberately not1AD520.
    // In particular ALL8's actual0+8 does not rewrite a retail1+7 Profile.
    p.count30=static_cast<std::uint32_t>(bank.count());p.humans34=humans;
    p.opponents38=p.count30-p.humans34;p.arrays3C44ConstructorNullKnown=false;
    p.race=&race;p.associated=true;pc=0x1aa580;return true;
}
bool OriginalRaceRacerManager50LA::continueRestart1AEA70(OriginalRaceConstructor50LA& race,
    native_racer50HN::Bank& bank,VehicleFrameState49W& frame,AiRuntimeState50IV& ai,
    const std::uint32_t& humans,const TrackNavigation50EU* actualMap,
    std::uint32_t actualA2,std::uint32_t& pc)noexcept {
    pc=0x1aea70;if(!impl_)return false;auto& p=*impl_;
    // The lexical caller already proved live World/Camera/Bank custody. Do
    // not touch any borrowed backing when that guard was unavailable.
    if(!p.componentCurrent()||!p.associated||p.race!=&race||race.racerB0_.get()!=this||
       race.application_!=p.application||race.entity_.sessionGeneration!=p.component.sessionGeneration||
       p.bank!=&bank||p.frame!=&frame||p.ai!=&ai||p.humanCount!=&humans||
       p.count30!=bank.count()||p.humans34!=humans||p.opponents38!=p.count30-p.humans34||
       p.arrays3C44ConstructorNullKnown)return false;
    if(p.cut!=Impl::RestartCut::unreached){pc=p.stopPC;return false;}
    p.resetA2=actualA2;p.indexS0=p.humans34;p.cut=Impl::RestartCut::aiLoop; //1AEA88/8C.
    try {
        while(p.indexS0<p.count30){ //1AEAB8/BC/C0, signed nonnegative native extent.
            const auto slot=p.indexS0;p.stopPC=0x1aeaa8;
            // Source44[slot] is the same registered native racer, never a
            // copied snapshot or reconstructed AI. Preserve prior slot writes
            // if a later native lease/identity check rejects.
            if(bank.read(slot,p.vehicle58)!=1u||!bank.read(slot,p.initialized)||
               bank.read(slot,p.managerIndex)!=slot||bank.read(slot,p.physicalPending)){
                p.cut=Impl::RestartCut::stopped;pc=p.stopPC;return false;
            }
            bool returned=false;
            bank.visit(slot,[&](){
                if(frame.fields.vehicle58!=1u||!ai.initialized||ai.managerIndexFE0!=slot||
                   ai.physicalReset2255B0Pending)return;
                //1AEAA8 calls16E830 with A1=1 in its delay slot.16E830
                //overwrites A1 with -1 immediately; no extra reset mode exists.
                resetAiRuntimeScalars50IV(ai,frame.bodies.main.controls.player.flagsB9C);
                returned=true;
            });
            if(!returned){p.cut=Impl::RestartCut::stopped;pc=p.stopPC;return false;}
            ++p.indexS0; //1AEAB0, only after this exact16E830 returned.
        }
        p.indexS0=0;p.cut=Impl::RestartCut::firstRacer; //1AEACC.
        p.stopPC=0x1aeae4;
        const auto firstClass=bank.read(0,p.vehicle58); //1AEAD0..1AEAE4.
        //1AEAF0/F8 versus1AEAFC/1AEB00. Bank order is unchanged; this is
        // only the source grid rank argument, never a roster permutation.
        p.firstRankA1=firstClass==1u?-static_cast<std::int32_t>(p.humans34):
            static_cast<std::int32_t>(p.opponents38);
        // The same published Race is known nonnull from the lexical caller.
        // Original1AEB20/28 zero XYZ and set W=1 via SQ VF0.
        p.position50={{0u,0u,0u,0x3f800000u}};p.heading60=p.position50;
        p.cut=Impl::RestartCut::placement;p.stopPC=0x1aeb38;
        if(!race.mapB8_){pc=p.stopPC;return false;}
        GridCenterResult50AC grid{};
        if(!race.mapB8_->prepareNativeRestartGrid2A2880(p.application,p.component.sessionGeneration,
            actualMap,*p.firstRankA1,grid,p.stopPC)){
            p.cut=Impl::RestartCut::stopped;pc=p.stopPC;return false;
        }
        for(std::size_t k=0;k<4;++k){p.position50[k]=grid.position[k];p.heading60[k]=grid.heading[k];}
        p.nextNode98=grid.nextNode;
        // Actual2A2880 projection returned. Its outputs remain caller locals;
        // no canonical BC0/BD0, zone refs, scene or body was written. Stop
        // BEFORE7F4/C84 stores,2169C0,virtual78,844,3C,1AED30 or source return.
        p.stopPC=0x1aeb44;pc=p.stopPC;return false;
    }catch(...){p.cut=Impl::RestartCut::stopped;pc=p.stopPC;return false;}
}
bool OriginalRaceRacerManager50LA::retireNativeDetached(std::uint32_t& pc)noexcept {
    pc=0x1ad490;if(!impl_)return true;auto& p=*impl_;
    if(p.thread!=std::this_thread::get_id())return false;
    if(p.closed)return true;
    if(p.counted){
        if(!p.componentCurrent()||!p.application->objectCount423F60()||
           !p.application->entityAuthority_.retire(p.component))return false;
        --p.component.reference00;--p.application->objectCount423F60();
    }
    // Native detached cancellation. The lexical Bank/Camera may already have
    // expired: compare/reset pointers only, never dereference or reset backing.
    // No guest44/3C free or source1AD490 child teardown is invented.
    p.bank=nullptr;p.frame=nullptr;p.ai=nullptr;p.humanCount=nullptr;p.race=nullptr;
    p.closed=true;return true;
}
bool OriginalRaceConstructor50LA::retireNativeMapB8(std::uint32_t& pc)noexcept {
    pc=0x2a11e0;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!endHud_.expired()||racerB0_)return false;
    if(!mapB8_)return true;
    if(!mapB8_->release142590())return false;
    mapB8_.reset();return true;
}
bool OriginalRaceConstructor50LA::retireNativeRacerB0(std::uint32_t& pc)noexcept {
    pc=0x1ad490;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!endHud_.expired())return false;
    if(!racerB0_)return true;
    if(!racerB0_->retireNativeDetached(pc))return false;
    racerB0_.reset();return true;
}
}

namespace original_race_hud_scope50LA {
bool Registration::restartRacerScalars1AEA70(rr::game::OriginalRaceConstructor50LA& race,
    std::uint32_t actualA2,std::uint32_t& pc)const noexcept {
    pc=0x1ac998;
    if(std::this_thread::get_id()!=thread_||!camera_||!bank_||!manager_||
       !endStateCurrent(race,*camera_)||!race.racerB0_||
       &camera_->actual_.bank!=bank_)return false;
    return race.racerB0_->continueRestart1AEA70(race,camera_->actual_.bank,
        camera_->actual_.arena,camera_->actual_.ai,camera_->actual_.humanCount34,
        camera_->actual_.actualRadarMapB8,actualA2,pc);
}
}
