#include "original_race_music_manager50la.h"
#include "original_application_entities50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/original_race_hud_scope50la.h"
#include "../../tests/native/original_camera_session50kx.h"
#include "../../tests/native/race_music50ie_fixture.h"
#include "../../tests/native/source_announcer50ie_fixture.h"
#include "../../platform/linux/native/original_application_aural50kg.h"
#include <array>
#include <exception>
#include <limits>
#include <optional>
#include <thread>
#include <utility>

namespace rr::game {
struct OriginalRaceMusicManager50LA::Impl final {
    using Registration=::original_race_hud_scope50LA::Registration;
    const std::thread::id thread=std::this_thread::get_id();
    std::shared_ptr<OriginalApplicationEntities50LA> application;
    std::weak_ptr<Registration> registration;
    OriginalWreckComponent50LA component;
    race_music50IE::Owner* music=nullptr;
    source_announcer50IE::Owner* announcer=nullptr;
    RaceMusicState50IE* live128_12C_130=nullptr; //The ONE existing state, never copied.
    const AnnouncerState50IE* nativeAnnouncerState=nullptr;
    bool counted=false,constructed=false,closed=false;
    struct Fields {
        std::array<std::uint32_t,4> quad60{},quad70{},quad90{};
        const void* track3C=nullptr;
        std::optional<OriginalWreckComponent50LA*> child164,child168;
        std::array<std::optional<OriginalWreckComponent50LA*>,12> childrenC0;
        std::uint32_t word15C=0,word160=0,count38=1,word30=0;
        float scalar138=.5f,scalar13C=.25f,scalar140=.5f,scalar144=.5f,scalar148=.5f,scalar14C=.5f;
        std::array<std::array<std::uint32_t,2>,7> pairsF0{};
        std::uint32_t word134=0,word150=0;
        std::optional<const void*> events158;
    } fields;
    enum class LoadCut:std::uint8_t {count38,aural44,nativeChildren,nativeReady};
    LoadCut loadCut=LoadCut::count38;
    // Source44's owner is the actual retained Aural interface. This native
    // identity is not a fabricated guest+40 address or new mixer instance.
    std::shared_ptr<const platform::OriginalApplicationAural50KG> aural;
    const void* nativeInterface44=nullptr;
    static bool stop(OriginalRaceMusicManagerDiagnostic50LA& d,std::uint32_t pc,const char* why)noexcept {
        d.sourcePC=pc;d.operation=why;return false;
    }
    bool componentCurrent()const noexcept {
        return thread==std::this_thread::get_id()&&counted&&!closed&&application&&
            application->application10()&&!application->attachmentLock423F58()&&
            component.kind==OriginalWreckComponentKind50LA::entity141300&&component.reference00==1&&
            component.sessionGeneration&&component.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&component))&&
            !component.parent1C&&!component.previous10&&!component.next14&&!component.firstChild18&&
            !component.mask20&&!component.descendants24&&component.category28==0xffu&&
            (component.flags29&15u)==3u&&application->entityAuthority_.canRetire(component);
    }
    bool providersCurrent(const Registration& actual)const noexcept {
        if(thread!=std::this_thread::get_id()||actual.thread_!=thread)return false;
        const auto scope=registration.lock();const auto race=actual.race_.lock();
        return scope.get()==&actual&&actual.active_&&race&&race->endHud_.lock()==scope&&
            race->application_==application&&race->counted_&&!race->rolledBack_&&
            race->entity_.sessionGeneration==component.sessionGeneration&&
            actual.generation_==component.sessionGeneration&&actual.music_==music&&actual.announcer_==announcer&&
            music&&announcer&&live128_12C_130==&music->state_&&nativeAnnouncerState==&announcer->state_;
    }
    Impl(std::shared_ptr<OriginalApplicationEntities50LA> app,std::uint64_t generation,
        const std::shared_ptr<Registration>& scope):application(std::move(app)),registration(scope) {
        if(!scope||scope->thread_!=thread||!application||application->nativeOwnerThread_!=thread||
           !application->application10()||application->attachmentLock423F58()||!generation||
           scope->generation_!=generation||!scope->active_||!scope->music_||!scope->announcer_||
           application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return;
        music=scope->music_;announcer=scope->announcer_;
        // Existing PCM/semantic preparation is native work, not this source
        // constructor. Admit only a genuinely unused music state; no reset of
        // an active/previous manager to manufacture its constructor values.
        if(music->counters_.requests||music->counters_.resets||music->owned_||music->paused_||
           music->fadePending_||music->playPending_||music->playbackRevision_||
           music->state_.timer128!=0.f||music->state_.mode12C!=0||music->state_.latch130!=-1)return;
        auto& authority=application->entityAuthority_;
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&component));
        if(!authority.reserveAdditional(1)||!authority.canEnroll(component,key))return;
        const auto made=initializeOriginalWreckComponent50LA(component,
            OriginalWreckComponentKind50LA::entity141300,generation,key,true,
            application->objectCount423F60()); //1A39FC/3A00: A1=1.
        if(made.status!=OriginalWreckComponentStatus50LA::complete)return;
        counted=true;if(!authority.enroll(component))std::terminate();
        live128_12C_130=&music->state_;nativeAnnouncerState=&announcer->state_;
        // Exact1A3A14..ADC constructor store order.3CEE00 is this concrete
        // native type, never an executable guest pointer. No callbacks below.
        fields.quad60.fill(0);fields.quad70.fill(0);fields.quad90.fill(0);
        fields.track3C=nullptr;fields.child168=nullptr;
        for(auto& child:fields.childrenC0)child=nullptr;
        fields.child164=nullptr;fields.word15C=0;fields.word160=0;
        fields.count38=1;fields.word30=0;
        live128_12C_130->timer128=0.f;live128_12C_130->latch130=-1;
        fields.scalar138=.5f;fields.scalar13C=.25f;fields.scalar140=.5f;
        fields.scalar144=.5f;fields.scalar148=.5f;fields.scalar14C=.5f;
        for(auto& pair:fields.pairsF0){pair[0]=0;pair[1]=0;}
        live128_12C_130->mode12C=0;
        fields.word134=0;fields.word150=0;fields.events158=nullptr;
        constructed=true;
    }
};
OriginalRaceMusicManager50LA::OriginalRaceMusicManager50LA(
    std::shared_ptr<OriginalApplicationEntities50LA> app,std::uint64_t generation,
    const std::shared_ptr<::original_race_hud_scope50LA::Registration>& scope)
    :impl_(new Impl(std::move(app),generation,scope)){}
OriginalRaceMusicManager50LA::~OriginalRaceMusicManager50LA()noexcept {
    OriginalRaceMusicManagerDiagnostic50LA d;if(impl_&&!retireNativeDetached(d))std::terminate();
}
bool OriginalRaceMusicManager50LA::constructed()const noexcept {
    return impl_&&impl_->componentCurrent()&&impl_->constructed;
}
bool OriginalRaceMusicManager50LA::noNativePreparation(
    const ::original_race_hud_scope50LA::Registration& scope)noexcept {
    if(scope.thread_!=std::this_thread::get_id()||!scope.active_||!scope.music_||!scope.announcer_)return false;
    // Preserve the old genuinely no-audio native route BEFORE any new count
    // or source write. A failed/partial preparation is never a fallback case.
    return !scope.music_->attempted_&&!scope.announcer_->attempted_&&
        !scope.music_->mixer_&&!scope.announcer_->prepared_&&!scope.announcer_->mixer_;
}
bool OriginalRaceMusicManager50LA::acceptNativeChildren(OriginalRaceConstructor50LA& race,
    const std::shared_ptr<const platform::OriginalApplicationAural50KG>& actualAural,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    d={};if(!impl_)return Impl::stop(d,0x1a3c00,"Counted music manager absent");auto& p=*impl_;
    if(p.thread!=std::this_thread::get_id())
        return Impl::stop(d,0x1a3c00,"Music manager load requires its construction thread");
    const auto scope=p.registration.lock();
    if(!p.componentCurrent()||!p.constructed||!scope||!p.providersCurrent(*scope)||
       race.music10C_.get()!=this||race.application_!=p.application||
       (race.flags1B0KnownMask_&3u)!=3u)
        return Impl::stop(d,0x1a3c00,"Actual published10C/provider lifetime unavailable");
    if(p.loadCut==Impl::LoadCut::count38){
        p.fields.count38=race.flags1B0KnownValue_&3u; //1A3C14, AFTER1A6CAC publication.
        p.loadCut=Impl::LoadCut::aural44;
    }
    if(p.loadCut==Impl::LoadCut::aural44){
        if(!actualAural||!actualAural->current())
            return Impl::stop(d,0x1a3c24,"Actual Application00/Aural interface unavailable;38 retained");
        const auto view=actualAural->inspect();
        if(!view.constructed||!view.interfaceKnown||!view.nativeInitialized||!view.interfaceA0||
           !view.mixer||p.application->auralNativeOwner_!=actualAural.get()||
           !p.application->aural00Live_||p.application->aural00_!=view.entity)
            return Impl::stop(d,0x1a3c30,"Actual Application00/Aural interface identity unavailable;38 retained");
        // The retained Aural mixer is the frontend interface owner. Native
        // race PCM uses the EXISTING separate RaceAudioGroups mixer; they are
        // not relabeled as the same transport. The same Application and its
        // canonical Race118/120 group owner join them, not pointer equality.
        p.aural=actualAural;p.nativeInterface44=view.interfaceA0;p.loadCut=Impl::LoadCut::nativeChildren;
    }
    if(p.loadCut==Impl::LoadCut::nativeChildren){
        // EXPLICIT NATIVE BACKEND REPLACEMENT for the platform-specific
        // Music164 / soundC0 / Announcer168 IO interval. The retained exact
        // PCM, voice117/Music118 and shared Dax episode are the real owners.
        // Existing prepare() ran earlier as native preparation; it is NOT
        // retroactively treated as2C0980/2C0A60/2C20F0/2C29A0 or1A3C00.
        if(!p.music->transportIdentity()||!p.announcer->current()||
           p.music->groups_!=race.raceAudio_||p.announcer->groups_!=race.raceAudio_||
           p.music->mixer_!=p.announcer->mixer_||
           p.music->bank_.ready!=p.music->expectedReadyClips()||
           p.music->owned_||p.music->playPending_||p.music->fadePending_||
           p.announcer->episode_->active()||p.announcer->episode_->episode()||
           p.announcer->playbackRevision_||p.announcer->cursor_.phase!=AnnouncerPhase50IE::Idle||
           p.announcer->update_.phase!=AnnouncerUpdatePhase50IE::Idle)
            return Impl::stop(d,0x1a3c34,"Actual prepared native music/Dax backend unavailable;10C and38/44 retained");
        // Unrepresented original child constructor/load/attach effects are
        // UNKNOWN, not fabricated null children, counts or successful calls.
        // Native transports keep their OWN already-prepared PCM and state.
        p.fields.child164.reset();p.fields.child168.reset();
        for(auto& child:p.fields.childrenC0)child.reset();p.fields.events158.reset();
        p.loadCut=Impl::LoadCut::nativeReady;
    }
    if(!nativeBackendCurrent(*scope))
        return Impl::stop(d,0x1a3c34,"Accepted native music backend changed; prior effects retained");
    d.sourcePC=0x1a6cc8;return true; //Native successor ONLY, not retail1A3C00 return.
}
bool OriginalRaceMusicManager50LA::nativeBackendCurrent(
    const ::original_race_hud_scope50LA::Registration& scope)const noexcept {
    if(!impl_)return false;const auto& p=*impl_;
    if(!p.componentCurrent()||p.loadCut!=Impl::LoadCut::nativeReady||!p.providersCurrent(scope)||
       !p.aural||!p.aural->current()||!p.music->current()||!p.announcer->current())return false;
    const auto view=p.aural->inspect();const auto race=scope.race_.lock();
    return race&&race->music10C_.get()==this&&view.interfaceKnown&&view.nativeInitialized&&
        view.interfaceA0==p.nativeInterface44&&view.mixer&&
        p.application->auralNativeOwner_==p.aural.get()&&p.application->aural00Live_&&
        p.application->aural00_==view.entity&&p.music->mixer_==p.announcer->mixer_&&
        p.music->groups_==race->raceAudio_&&p.announcer->groups_==race->raceAudio_;
}
bool OriginalRaceMusicManager50LA::readQueuedCount150(const OriginalRaceConstructor50LA& race,
    const ::original_race_hud_scope50LA::Registration& scope,std::uint32_t& count)const noexcept {
    if(race.music10C_.get()!=this||!nativeBackendCurrent(scope)||
       impl_->application!=race.application_||impl_->component.sessionGeneration!=race.entity_.sessionGeneration)
        return false;
    //Actual mutable counted manager lane, initialized1A3AD4 and retained by
    //the explicit native backend adapter. Never an inferred constant/empty
    //queue receipt. The158 payload remains unknown and is NOT dereferenced.
    count=impl_->fields.word150;return true;
}
bool OriginalRaceMusicManager50LA::clearQueuedCount1A5C40(const OriginalRaceConstructor50LA& race,
    const ::original_race_hud_scope50LA::Registration& scope)noexcept {
    if(race.music10C_.get()!=this||!nativeBackendCurrent(scope)||
       impl_->application!=race.application_||impl_->component.sessionGeneration!=race.entity_.sessionGeneration)
        return false;
    impl_->fields.word150=0; //1A5C44, JR delay slot; payload158 untouched.
    return true;
}
void OriginalRaceMusicManager50LA::revokeNativeProviders(
    const ::original_race_hud_scope50LA::Registration& scope)noexcept {
    if(!impl_||impl_->thread!=std::this_thread::get_id()||impl_->registration.lock().get()!=&scope)return;
    impl_->music=nullptr;impl_->announcer=nullptr;impl_->live128_12C_130=nullptr;
    impl_->nativeAnnouncerState=nullptr;impl_->registration.reset();
}
bool OriginalRaceMusicManager50LA::retireNativeDetached(OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    d={};if(!impl_)return true;auto& p=*impl_;
    if(p.thread!=std::this_thread::get_id())return Impl::stop(d,0x1a3af0,"Music manager retirement thread mismatch");
    if(p.closed)return true;
    if(p.counted){
        if(!p.componentCurrent()||!p.application->objectCount423F60()||
           !p.application->entityAuthority_.retire(p.component))
            return Impl::stop(d,0x1a3af0,"Actual detached music manager retirement unavailable");
        --p.component.reference00;--p.application->objectCount423F60();
    }
    // Native backing/count withdrawal ONLY. No1A3AF0 child destructor,
    // global stop/volume100 or backend reset is invented. Actual provider
    // closure still belongs to its existing unique Setup owner and schedule.
    p.music=nullptr;p.announcer=nullptr;p.live128_12C_130=nullptr;p.nativeAnnouncerState=nullptr;
    p.registration.reset();p.aural.reset();p.nativeInterface44=nullptr;p.closed=true;return true;
}
}

namespace original_main_entry50KG {
bool Application::completeConsumedRaMusicNative10C(original_camera_session50KX::Session& session,
    std::uint32_t& pc)const noexcept {
    pc=0x1a6c90;
    // Revalidate the actual completed camera services, not just its enum.
    if(!completeConsumedRaCameraNative34(session))return false;
    const auto race=sourceRace28COwner_;const auto scope=race->endHud_.lock();
    if(!scope||scope->thread_!=std::this_thread::get_id()||!scope->active_||scope->application_!=this||
       scope->race_.lock()!=race||scope->generation_!=race->entity_.sessionGeneration||
       scope->bank_!=&session.actual_.bank||!scope->music_||!scope->announcer_||
       (scope->camera_&&scope->camera_!=&session))return false;
    using Manager=rr::game::OriginalRaceMusicManager50LA;
    if(!race->music10C_){
        // Existing native no-audio route stays unchanged and has NO10C or
        // successor admission. Never use this escape after any source effect.
        if(Manager::noNativePreparation(*scope))return true;
        try {
            std::unique_ptr<Manager> next(new Manager(entitiesOwner_,race->entity_.sessionGeneration,scope));
            if(!next->constructed()){pc=0x1a39f0;return false;}
            race->music10C_=std::move(next); //1A6CAC publication BEFORE load.
        }catch(...){pc=0x1a6c90;return false;}
    }
    rr::game::OriginalRaceMusicManagerDiagnostic50LA d;
    const bool accepted=race->music10C_->acceptNativeChildren(*race,sourceAural00Owner_,d);
    pc=d.sourcePC;return accepted;
}
bool Application::consumedRaMusicNative10CCurrent(const original_camera_session50KX::Session& session)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       !session.onOwnerThread())return false;
    const auto race=sourceRace28COwner_;
    if(!race||raceConstructorColdOwner50LA_.lock()!=race||race->application_!=entitiesOwner_||race->profile_!=riderProfile||
       !race->ownsNativeCameraAdapter34(session)||race->cameraNativeAdapter34_!=&session||
       race->cameraInitStep_!=rr::game::OriginalRaceConstructor50LA::CameraInitStep::nativeReady||
       !race->music10C_)return false;
    const auto scope=race->endHud_.lock();
    return scope&&scope->application_==this&&scope->bank_==&session.actual_.bank&&
        race->music10C_->nativeBackendCurrent(*scope);
}
bool Application::consumedRaMusicNativeOnlyNoPreparation(const original_camera_session50KX::Session& session)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       !session.onOwnerThread())return false;
    const auto race=sourceRace28COwner_;
    if(!race||raceConstructorColdOwner50LA_.lock()!=race||race->application_!=entitiesOwner_||race->profile_!=riderProfile||
       race->music10C_||!race->ownsNativeCameraAdapter34(session)||race->cameraNativeAdapter34_!=&session||
       race->cameraInitStep_!=rr::game::OriginalRaceConstructor50LA::CameraInitStep::nativeReady)return false;
    const auto scope=race->endHud_.lock();
    return scope&&scope->thread_==std::this_thread::get_id()&&scope->application_==this&&
        scope->race_.lock()==race&&scope->generation_==race->entity_.sessionGeneration&&
        scope->bank_==&session.actual_.bank&&(!scope->camera_||scope->camera_==&session)&&
        rr::game::OriginalRaceMusicManager50LA::noNativePreparation(*scope);
}
bool Application::retireConsumedRaMusicNative10C(rr::game::OriginalRaceConstructor50LA& race,
    std::uint32_t& pc)const noexcept {
    pc=0x1a3af0;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       race.resultsFeedbackThread_!=std::this_thread::get_id()||
       raceConstructorColdOwner50LA_.lock().get()!=&race||race.application_!=entitiesOwner_||
       !race.endHud_.expired())return false;
    if(!race.music10C_)return true;
    rr::game::OriginalRaceMusicManagerDiagnostic50LA d;
    if(!race.music10C_->retireNativeDetached(d)){pc=d.sourcePC;return false;}
    race.music10C_.reset();return true; //Native slot withdrawal, not source null store.
}
}
namespace original_camera_session50KX {
bool Session::completeCountedRaceMusicNative10C(std::uint32_t& pc)noexcept {
    pc=0x1a6c90;
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return false;
    if(!nativeTaIssuance_||nativeTaIssuance_->course!=rr::game::OriginalCameraStartCourse::ra)return true;
    std::uint32_t pause=0;
    if(!nativeTaIssuance_->issuer||!actual_.applicationAnimation.readPause11C(pause))return false;
    return nativeTaIssuance_->issuer->completeConsumedRaMusicNative10C(*this,pc);
}
}
