#include "../../tests/native/original_race_display_lifetime50la.h"
#include "original_player_overlay_renderer_lease50la.h"
#include "original_race_music_manager50la.h"
#include "../assets/original_config_process_globals50la.h"
#include "original_pause_storage50la.h"
#include "original_application_entities50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/original_race_hud_scope50la.h"
#include "../../tests/native/original_camera_session50kx.h"
#include "../../tests/native/race_music50ie_fixture.h"
#include "../../tests/native/source_announcer50ie_fixture.h"
#include "../../platform/linux/native/original_application_aural50kg.h"
#include "race_event_queue50id.h"
#include "lap_event6_subscriber50id.h"
#include "../../platform/linux/native/original_ui_main_session50kg.h"
#include "dynamic_impact_events50iw.h"
#include "../../tests/native/live_audio50hf_fixture.h"
#include "../../tests/native/native_ai_session50iv_fixture.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cfenv>
#include <cmath>
#include <exception>
#include <limits>
#include <optional>
#include <thread>
#include <utility>

#include "original_race_music_source_storage50la.inl"

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
    RaceEventQueue50ID<128>* nativeEvents=nullptr; //Same lexical Session owner, not copied.
    std::optional<std::uint32_t> endQueueInitialCount;
    std::optional<RaceEventEntry50ID> endQueuePending; //Native retained descriptor, not manufactured payload.
    LapEventCursor50ID endLapCursor{};
    bool endLapStarted=false;
    std::uint32_t endLapSelectedIndex=0; //Reached1A585C, retained on terminal range cut.
    std::array<std::optional<std::uint32_t>,7> endDamageWords{}; //40,58,draw,138,repeat,60,fraction.
    LapEventRequest50ID endDamageRequest{};
    bool endDamageStarted=false,endDamageNeedsAnnouncer=false;
    bool endDamageNeedsSpeech=false;
    std::array<std::optional<std::uint32_t>,16> endDamageSpeechWords{};
    std::array<std::uint32_t,128> endDamageSpeechScratch{};
    std::array<bool,128> endDamageSpeechScratchKnown{};
    DynamicContactEventCursor50IW endContactCursor{};
    std::array<std::optional<std::uint32_t>,18> endContactWords{};
    LapEventRequest50ID endContactRequest{};
    bool endContactStarted=false,endContactNeedsAnnouncer=false;
    bool endContactNeedsSpeech=false,endContactSpeechTaunt=false;
    std::uint32_t endContactSpeechActor=0;
    std::array<std::optional<std::uint32_t>,16> endContactSpeechWords{};
    std::array<std::optional<std::uint32_t>,3> endContactSpeechOffsets{};
    std::array<std::uint32_t,128> endContactSpeechScratch{};
    std::array<bool,128> endContactSpeechScratchKnown{};
    std::optional<race_music50IE::UpdateReceipt50IW> restartFadeReceipt;
    std::optional<race_music50IE::UpdateReceipt50IW> pauseReturnFadeReceipt;
    bool endQueueStopped=false;
    std::uint32_t endQueueStopPC=0x1a4cd4;

    const bool sourceOnly;
    RaceMusicState50IE source128_12C_130{}; // Used only by the distinct source occurrence.
    std::unique_ptr<original_race_music_source50LA::Child> sourceChild;
    platform::RaceAudioGroups50IC* sourceGroups=nullptr; // Private native lifetime borrow.
    bool sourceLoadAttempted=false;
    std::uint32_t sourcePC=0x1a3c00;
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
        std::optional<const void*> events158; //Guest address stays unknown; nativeEvents is the explicit adapter.
    } fields;
    enum class LoadCut:std::uint8_t {count38,aural44,nativeChildren,nativeReady};
    LoadCut loadCut=LoadCut::count38;
    // Source44's owner is the actual retained Aural interface. This native
    // identity is not a fabricated guest+40 address or new mixer instance.
    std::shared_ptr<platform::OriginalApplicationAural50KG> aural;
    bool listenerAttempted=false,listenerRetired=false,listenerStepStopped=false;
    std::optional<std::uint64_t> listenerMixCursor;
    std::uint64_t listenerCameraPublication=0;
    // Retain reached read/scratch prefixes on a terminal native consumed-step.
    std::array<std::optional<std::uint32_t>,14> listenerWords{};
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
    static bool freshMusicState(const Registration& scope)noexcept {
        const auto* music=scope.music_;
        return music&&!music->counters_.requests&&!music->counters_.resets&&!music->owned_&&!music->paused_&&
            !music->fadePending_&&!music->playPending_&&!music->playbackRevision_&&
            music->state_.timer128==0.f&&music->state_.mode12C==0&&music->state_.latch130==-1;
    }
    Impl(std::shared_ptr<OriginalApplicationEntities50LA> app,std::uint64_t generation,
        const std::shared_ptr<Registration>& scope,bool originalSource=false)
        :application(std::move(app)),registration(scope),sourceOnly(originalSource) {
        if(!application||application->nativeOwnerThread_!=thread||!application->application10()||
           application->attachmentLock423F58()||!generation||
           application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return;
        if(sourceOnly){if(scope)return;}
        else {
            if(!scope||scope->thread_!=thread||scope->generation_!=generation||!scope->active_||
               !scope->music_||!scope->announcer_)return;
            music=scope->music_;announcer=scope->announcer_;
        }
        // Existing PCM/semantic preparation is native work, not this source
        // constructor. Admit only a genuinely unused music state; no reset of
        // an active/previous manager to manufacture its constructor values.
        if(!sourceOnly&&!freshMusicState(*scope))return;
        auto& authority=application->entityAuthority_;
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&component));
        if(!authority.reserveAdditional(1)||!authority.canEnroll(component,key))return;
        const auto made=initializeOriginalWreckComponent50LA(component,
            OriginalWreckComponentKind50LA::entity141300,generation,key,true,
            application->objectCount423F60()); //1A39FC/3A00: A1=1.
        if(made.status!=OriginalWreckComponentStatus50LA::complete)return;
        counted=true;if(!authority.enroll(component))std::terminate();
        live128_12C_130=sourceOnly?&source128_12C_130:&music->state_;
        nativeAnnouncerState=sourceOnly?nullptr:&announcer->state_;
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
#include "original_race_music_source50la.inl"
#include "original_race_music_config_open50la.inl"
#include "original_race_music_metadata50la.inl"
#include "original_race_music_config_retire50la.inl"
#include "original_race_music_format50la.inl"
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
bool OriginalRaceMusicManager50LA::freshAll8NativePrefix(const OriginalRaceConstructor50LA& race,
    const ::original_race_hud_scope50LA::Registration& scope)noexcept {
    // Native admission BEFORE allocation/count/stores. These observations do
    // not stand in for original child loads or the later FIFO/listener calls.
    if(scope.thread_!=std::this_thread::get_id()||!scope.active_||
       scope.race_.lock().get()!=&race||race.endHud_.lock().get()!=&scope||race.music10C_||
       !scope.music_||!scope.announcer_||!scope.gameplay_||!Impl::freshMusicState(scope))return false;
    const auto& music=*scope.music_;const auto& announcer=*scope.announcer_;
    auto* mixer=scope.gameplay_->mixerForMaterialOwner();
    if(!mixer||scope.gameplay_->groupsForProviders()!=race.raceAudio_||
       music.mixer_!=mixer||announcer.mixer_!=mixer||
       music.groups_!=race.raceAudio_||announcer.groups_!=race.raceAudio_||
       announcer.episode_!=scope.gameplay_->sharedAnnouncer()||
       !music.transportIdentity()||music.bank_.ready!=music.expectedReadyClips()||
       !announcer.current()||announcer.episode_->active()||announcer.episode_->episode()||
       announcer.playbackRevision_||announcer.cursor_.phase!=AnnouncerPhase50IE::Idle||
       announcer.update_.phase!=AnnouncerUpdatePhase50IE::Idle)return false;
    // The actual ALL8 shared queue stays untouched, including its ticket and
    // logical capacity. Decline a used owner; never reset it to fake freshness.
    if(!scope.events_||!scope.eventBindings_||!scope.eventBindings_->eventOwner33CBound)return false;
    const auto& q=*scope.events_;
    return !q.count150&&!q.lastTicket&&!q.dispatchIndex&&!q.flushing&&
        !q.endFlushPending50LA()&&q.capacity154==32u;
}
bool OriginalRaceMusicManager50LA::stageNativeLoadPrefix(OriginalRaceConstructor50LA& race,
    const std::shared_ptr<platform::OriginalApplicationAural50KG>& actualAural,
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
    d.sourcePC=0x1a3c34;return true; //38/44 only; child/listener/nativeReady not reached.
}
bool OriginalRaceMusicManager50LA::acceptNativeChildren(OriginalRaceConstructor50LA& race,
    const std::shared_ptr<platform::OriginalApplicationAural50KG>& actualAural,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    if(!stageNativeLoadPrefix(race,actualAural,d))return false;
    auto& p=*impl_;const auto scope=p.registration.lock();
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
        // Explicit native load join for the exact1A4620 listener subgraph.
        // Other platform child IO remains the documented native adapter.
        if(p.fields.count38!=1u||!scope->gameplay_)
            return Impl::stop(d,0x1a4620,"Only the actual single-view native listener owner is represented");
        platform::OriginalApplicationAural50KG::Diagnostic listener;
        const bool registered=p.aural->beginSingleRaceListener1A4620(*this,listener);
        // Native custody, NOT readiness: only this actual issuer owns cleanup.
        // A pre-admission rejection must not acquire/remove another manager's listener.
        p.listenerAttempted=p.aural->nativeListenerIssuer_==this;
        if(!registered)
            return Impl::stop(d,listener.sourcePC,"Actual listener registration retained its source prefix");
        if(!p.listenerMixCursor)p.listenerMixCursor=scope->gameplay_->mixedFrames();
        if(scope->events_){
            const auto& q=*scope->events_;
            if(!scope->eventBindings_||!scope->eventBindings_->eventOwner33CBound||
               q.count150||q.lastTicket||q.dispatchIndex||q.flushing||q.endFlushPending50LA()||
               q.capacity154!=32u)
                return Impl::stop(d,0x1a3c84,"Actual native Session FIFO is not fresh; no reset/adoption of used events");
            //Native allocation adapter: real fixed128 reservation, original
            //logical initial32. Does NOT claim source305700 returned or grant
            //ownership to arbitrary events. Existing producers already share
            //this exact Session FIFO;21080C..818 identifies their33C as10C.
            p.nativeEvents=scope->events_;
        }
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
    if(!p.componentCurrent()||p.loadCut!=Impl::LoadCut::nativeReady||p.listenerStepStopped||!p.providersCurrent(scope)||
       !p.aural||!p.aural->current()||!p.aural->listenerCurrent(*this)||
       !p.music->current()||!p.announcer->current()||
       p.nativeEvents!=scope.events_||
       (p.nativeEvents&&(!scope.eventBindings_||!scope.eventBindings_->eventOwner33CBound)))return false;
    const auto view=p.aural->inspect();const auto race=scope.race_.lock();
    return race&&race->music10C_.get()==this&&view.interfaceKnown&&view.nativeInitialized&&
        view.interfaceA0==p.nativeInterface44&&view.mixer&&
        p.application->auralNativeOwner_==p.aural.get()&&p.application->aural00Live_&&
        p.application->aural00_==view.entity&&p.music->mixer_==p.announcer->mixer_&&
        p.music->groups_==race->raceAudio_&&p.announcer->groups_==race->raceAudio_;
}
bool OriginalRaceMusicManager50LA::serviceNativeMenuAudio(
    const OriginalRaceConstructor50LA& race,const Impl::Registration& scope,std::uint32_t micros,
    platform::AudioOutputDiagnostic50HD& audio,OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    d={};
    if(!nativeBackendCurrent(scope)||race.music10C_.get()!=this||!scope.gameplay_)
        return Impl::stop(d,0x1a4440,"Actual ALL8 listener/menu mixer owner unavailable");
    auto& p=*impl_;const auto before=scope.gameplay_->mixedFrames();
    if(!p.listenerMixCursor||before!=*p.listenerMixCursor){
        p.listenerStepStopped=true;
        return Impl::stop(d,0x1a4440,"Unaccounted mix precedes actual native menu service");
    }
    // The private Application caller proves actual open Pause and App11C.
    // Existing service checks World4D8's actual pause and bounded host interval,
    // polls real duck completion and consumes the SAME output exactly once.
    const bool serviced=scope.gameplay_->serviceNativeMenuAudio(micros,audio);
    const auto after=scope.gameplay_->mixedFrames();
    p.listenerMixCursor=after; //Actual consumption, including partial output failure.
    if(after<before||after-before>live_audio50HF::stepFrames||!serviced){
        p.listenerStepStopped=true;
        return Impl::stop(d,0x1a4440,"Native menu service stopped; actual mixed-frame custody retained");
    }
    // Scheduling custody ONLY. No listener pose/velocity/publication writes,
    // music timer, world delta, source update return or synthesized768 frames.
    // Fractional native output clock can consume zero frames for a short call.
    d.sourcePC=0x1a4440;return true;
}
bool OriginalRaceMusicManager50LA::restartNativeFade1AC960(
    const OriginalRaceConstructor50LA& race,const Impl::Registration& scope,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    d={};
    //Authenticate only the canonical component and lexical state borrow here.
    //Transport readiness, pending work and fade arithmetic belong AFTER the
    //unconditional source130/12C stores, not in nativeBackendCurrent preflight.
    if(!impl_||!impl_->componentCurrent()||!impl_->providersCurrent(scope)||
       impl_->loadCut!=Impl::LoadCut::nativeReady||race.music10C_.get()!=this)
        return Impl::stop(d,0x1ac964,"Actual same-Race10C/native state owner unavailable");
    auto& p=*impl_;
    if(p.restartFadeReceipt)
        return Impl::stop(d,p.restartFadeReceipt->helperPC?p.restartFadeReceipt->helperPC:
            p.restartFadeReceipt->sourcePC,"Restart music invocation already attempted; no replay");
    //1AC958/95C constructs actual F12=1.0 before JAL1A4AB0. Source helper
    //writes130 then12C, leaving128 unchanged, before loading child164.
    p.music->writeRestartFadePrefix1A4AB0();
    p.restartFadeReceipt=p.music->requestRestartFadeTail234440();
    const auto& result=*p.restartFadeReceipt;
    if(!result.completed())
        return Impl::stop(d,result.helperPC?result.helperPC:result.sourcePC,
            "Actual native234440 transport continuation stopped; source music prefix and reached fade effects retained");
    d.sourcePC=0x1ac968;return true; //Native transport adaptation, not guest IOP/logging completion.
}
bool OriginalRaceMusicManager50LA::pauseReturnNativeFade1A4AB0(
    const OriginalRaceConstructor50LA& race,const Impl::Registration& scope,PauseReturnCall call,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    d={};const auto caller=static_cast<std::uint32_t>(call);
    if(call!=PauseReturnCall::main1BD9F0&&call!=PauseReturnCall::course1BDB88)
        return Impl::stop(d,0x1a4ab0,"Unsupported private Pause music caller");
    // Actual retained Pause row9/10 caller only, after its real Aural26D070. Authenticate
    // component, lexical providers and Race10C before borrowing source state;
    // transport admission must follow the unconditional1A4AB0 stores.
    if(!impl_||!impl_->componentCurrent()||!impl_->constructed||
       !impl_->providersCurrent(scope)||impl_->loadCut!=Impl::LoadCut::nativeReady||
       scope.race_.lock().get()!=&race||race.music10C_.get()!=this)
        return Impl::stop(d,caller+4u,"Actual Pause same-Race10C/native state owner unavailable");
    auto& p=*impl_;
    if(p.pauseReturnFadeReceipt)
        return Impl::stop(d,p.pauseReturnFadeReceipt->helperPC?p.pauseReturnFadeReceipt->helperPC:
            p.pauseReturnFadeReceipt->sourcePC,"Pause return music invocation already attempted; no replay");
    //1BD9E4/E8 or1BDB7C/80 supplies F12=.5 at the actual call.1A4AB4 writes130=-1,
    //1A4AB8 writes12C=0, then1A4ABC/C0 tail-calls234440 on child164.
    // Neither128 nor restart's independent one-time custody is touched.
    p.music->writePauseReturnFadePrefix1A4AB0();
    p.pauseReturnFadeReceipt=race_music50IE::UpdateReceipt50IW{
        race_music50IE::Result::Ownership,0x1a4ac0u,0x234440u,.5f,p.music->nativeFade_.phase,false};
    // Even backend failure is terminal for this invocation, with both source
    // stores retained. Do not preflight it before130/12C or repair/retry it.
    if(!nativeBackendCurrent(scope)||!p.aural->backendCurrent())
        return Impl::stop(d,0x234440,"Actual Pause native backend unavailable; source130/12C retained");
    p.pauseReturnFadeReceipt=p.music->requestPauseReturnStoppedFadeTail234440();
    const auto& result=*p.pauseReturnFadeReceipt;
    if(!result.completed())
        return Impl::stop(d,result.helperPC?result.helperPC:result.sourcePC,
            "Actual Pause stopped music continuation rejected; source prefix and native ownership retained");
    d.sourcePC=caller+8u;return true; //Native stopped-stream adapter, not original IOP/logging completion.
}
bool OriginalRaceMusicManager50LA::advanceNativeListenerConsumedStep(
    const OriginalRaceConstructor50LA& race,const Impl::Registration& scope,
    const ::original_camera_session50KX::Session& camera,std::uint64_t beforeMix,
    bool heldSourceScene,OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    d={};if(!nativeBackendCurrent(scope)||race.music10C_.get()!=this||!scope.gameplay_)
        return Impl::stop(d,0x1a4440,"Actual same-Race listener/mix owner unavailable");
    auto& p=*impl_;
    if(p.listenerStepStopped||!p.listenerMixCursor)
        return Impl::stop(d,0x1a4440,"Prior consumed listener step remains terminal");
    const auto afterMix=scope.gameplay_->mixedFrames();
    if(beforeMix!=*p.listenerMixCursor||afterMix<beforeMix||afterMix-beforeMix!=live_audio50HF::stepFrames)
        return Impl::stop(d,0x1a4440,"Listener requires the next actual shared-mixer consumption");
    // One native consumed-step token, claimed BEFORE any source-dependent read.
    // No repair/re-read/replay after any failure, including absent camera pose.
    p.listenerMixCursor=afterMix;p.listenerStepStopped=true;p.listenerWords={};
    std::array<std::uint32_t,3> old{},velocity{};
    std::array<const float*,9> actualXYZ{};
    std::uint64_t publication=0;
    if(!scope.readNativeListenerScene(race,camera,heldSourceScene,p.listenerCameraPublication,
            publication,actualXYZ))
        return Impl::stop(d,0x1a4474,"Actual same-Race camera34 publication/held route unavailable");
    // Native scheduling adaptation: actual consumed768/48000, NOT a claim
    // that original music update's F20 or omitted prefix/suffix ran.
    if(std::fegetround()!=FE_TONEAREST)
        return Impl::stop(d,0x1a44b0,"Native consumed listener delta rounding unsupported");
    volatile float delta=static_cast<float>(afterMix-beforeMix)/static_cast<float>(live_audio50HF::sampleRate);
    p.listenerWords[0]=std::bit_cast<std::uint32_t>(static_cast<float>(delta));
    platform::OriginalApplicationAural50KG::Diagnostic listener;
    if(!p.aural->readSingleRaceListener290E10(*this,old,listener))
        return Impl::stop(d,listener.sourcePC,"Actual preceding listener position unavailable");
    //1A44D8..4508 actual old/new XYZ;106790 creates scratch with W=1.
    for(std::size_t k=0;k<3;++k)p.listenerWords[1+k]=old[k];
    for(std::size_t k=0;k<3;++k)p.listenerWords[4+k]=std::bit_cast<std::uint32_t>(*actualXYZ[k]);
    for(std::size_t k=0;k<3;++k){
        volatile float difference=std::bit_cast<float>(*p.listenerWords[4+k])-std::bit_cast<float>(old[k]);
        p.listenerWords[7+k]=std::bit_cast<std::uint32_t>(static_cast<float>(difference));
    }
    volatile float inverse=1.f/delta;
    p.listenerWords[10]=std::bit_cast<std::uint32_t>(static_cast<float>(inverse));
    // Source multiplication order Y, X, Z; scratch stores X then Y then Z.
    volatile float y=std::bit_cast<float>(*p.listenerWords[8])*inverse;
    volatile float x=std::bit_cast<float>(*p.listenerWords[7])*inverse;
    velocity[0]=std::bit_cast<std::uint32_t>(static_cast<float>(x));
    volatile float z=std::bit_cast<float>(*p.listenerWords[9])*inverse;
    velocity[1]=std::bit_cast<std::uint32_t>(static_cast<float>(y));
    velocity[2]=std::bit_cast<std::uint32_t>(static_cast<float>(z));
    for(std::size_t k=0;k<3;++k)p.listenerWords[11+k]=velocity[k];
    // Native finite projection rejects before external XYZ writes. Scratch
    // above and consumed token remain retained; it is never retried/staged back.
    if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z))
        return Impl::stop(d,0x1a4558,"Listener velocity numeric domain unsupported");
    if(!p.aural->writeSingleRaceListener1030B0(*this,actualXYZ,velocity,listener))
        return Impl::stop(d,listener.sourcePC,"Listener source setter prefix retained");
    p.listenerCameraPublication=publication;p.listenerStepStopped=false;
    d.sourcePC=0x1a45a8;return true;
}
bool OriginalRaceMusicManager50LA::invokeNativeContactSpeech1(
    const OriginalRaceConstructor50LA& race,const Impl::Registration& scope,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    auto& p=*impl_;auto& words=p.endContactSpeechWords;auto& offsets=p.endContactSpeechOffsets;
    const bool taunt=p.endContactSpeechTaunt;const auto actor=p.endContactSpeechActor;
    std::uint32_t pc=taunt?0x1a5aa4u:0x1a5ab8u;const auto& c=p.endContactCursor;
    if(!p.endQueuePending||!scope.camera_||!nativeBackendCurrent(scope)||!scope.gameplay_||
       !p.listenerCameraPublication||!p.listenerMixCursor||
       scope.gameplay_->mixedFrames()!=*p.listenerMixCursor||
       scope.camera_->publicationCount()<p.listenerCameraPublication||
       (taunt?(!c.partnerSpeech||c.selfSpeech||actor!=c.event.actor0C||c.phase!=DynamicContactEventPhase50IW::PartnerSpeech):
              (!c.selfSpeech||c.partnerSpeech||actor!=c.event.actor10||c.phase!=DynamicContactEventPhase50IW::SelfSpeech)))
        return Impl::stop(d,pc,"Selected contact speech owner/request/prior listener history unavailable");
    const PodSpeechCounts50HG* counts=nullptr;PodSpeechState50HG* speech=nullptr;
    std::array<std::uint32_t,3> local{};std::array<std::uint32_t,4> main70{};
    if(!scope.readNativePodSpeechOwner(race,actor,counts,speech,local,main70,words,pc))
        return Impl::stop(d,pc,"Actual selected actor initialized native D48/body/scene owner unavailable");
    bool audible=false;platform::OriginalApplicationAural50KG::Diagnostic aural;
    if(!p.aural->observeSingleRaceSpeech104140(*this,local,main70,p.endContactSpeechScratch,
        p.endContactSpeechScratchKnown,audible,aural))
        return Impl::stop(d,aural.sourcePC,"Contact audibility prefix retained; no retry");
    words[14]=audible?1u:0u;
    if(!audible){d.sourcePC=taunt?0x2c1cc8u:0x2c1be0u;return true;}
    words[7]=std::bit_cast<std::uint32_t>(speech->cooldown224);
    const float cooldown=std::bit_cast<float>(*words[7]);
    if(!std::isfinite(cooldown))return Impl::stop(d,taunt?0x2c1c34u:0x2c1ac4u,
        "Reached contact cooldown numeric domain unsupported");
    if(!(cooldown<=0.f)){d.sourcePC=taunt?0x2c1cc8u:0x2c1be0u;return true;}
    const std::size_t cursor=taunt?5u:3u;
    words[8]=speech->cursors[cursor]; //222 at2C1C84, or220 at2C1B8C.
    words[9]=taunt?counts->attacking:counts->attacked; //219 /217, only reached category.
    std::uint32_t selected=*words[8]+1u;words[10]=selected;
    if(!*words[9])return Impl::stop(d,taunt?0x2c1c90u:0x2c1b98u,
        "Reached contact category zero divisor unsupported");
    speech->cursors[cursor]=static_cast<std::uint8_t>(selected%*words[9]); //1CA0 /1BA8 immediateSB.
    words[11]=speech->cursors[cursor];
    //Bank offset reads occur AFTER cursor store; add to unmodded old+1,
    //never the stored remainder. No unrelated category is prevalidated.
    if(taunt){offsets[0]=counts->passing;selected+=*offsets[0];} //1CA4/1CA8.
    else{
        offsets[0]=counts->lowDamage;offsets[1]=counts->wallHit;offsets[2]=counts->passed;
        selected+=*offsets[0];selected+=*offsets[1];selected+=*offsets[2]; //1BB8/BC/C0.
    }
    words[10]=selected; //Reached source A1 request, retained even if provider rejects.
    if(!scope.playNativePodSpeech(race,actor,taunt,selected,pc))
        return Impl::stop(d,pc,"Actual selected actor play did not return; cursor/provider prefix retained");
    words[12]=1u; //Native completed-helper custody, NOT guessed guest V0.
    speech->cooldown224=std::bit_cast<float>(std::uint32_t{0x40a00000}); //1CC4 /1BDC AFTER actual return.
    words[13]=0x40a00000u;d.sourcePC=taunt?0x2c1cc8u:0x2c1be0u;return true;
}
bool OriginalRaceMusicManager50LA::invokeNativeLowDamageSpeech5(
    const OriginalRaceConstructor50LA& race,const Impl::Registration& scope,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    auto& p=*impl_;auto& words=p.endDamageSpeechWords;std::uint32_t pc=0x1a532c;
    if(!p.endQueuePending||!scope.camera_||!nativeBackendCurrent(scope)||!scope.gameplay_||
       !p.listenerCameraPublication||!p.listenerMixCursor||
       scope.gameplay_->mixedFrames()!=*p.listenerMixCursor||
       scope.camera_->publicationCount()<p.listenerCameraPublication)
        return Impl::stop(d,pc,"Selected speech requires actual initialized prior listener history; no future mix pose");
    const PodSpeechCounts50HG* counts=nullptr;PodSpeechState50HG* speech=nullptr;
    std::array<std::uint32_t,3> local{};std::array<std::uint32_t,4> main70{};
    if(!scope.readNativeDamageSpeech5Owner(race,p.endQueuePending->event,counts,speech,local,main70,words,pc))
        return Impl::stop(d,pc,"Selected native D48 initializer/actor/body/local scene owner unavailable");
    bool audible=false;platform::OriginalApplicationAural50KG::Diagnostic aural;
    if(!p.aural->observeSingleRaceSpeech104140(*this,local,main70,p.endDamageSpeechScratch,
            p.endDamageSpeechScratchKnown,audible,aural))
        return Impl::stop(d,aural.sourcePC,"Actual audibility scratch/numeric prefix retained; no retry");
    words[14]=audible?1u:0u;
    if(!audible){d.sourcePC=0x2c1be0;return true;} //No cooldown/count/cursor reads.
    pc=0x2c1ab8;words[7]=std::bit_cast<std::uint32_t>(speech->cooldown224);
    const float cooldown=std::bit_cast<float>(*words[7]);
    if(!std::isfinite(cooldown))return Impl::stop(d,0x2c1ac4,"Reached speech cooldown numeric domain unsupported");
    if(!(cooldown<=0.f)){d.sourcePC=0x2c1be0;return true;}
    //Actual type3 branch: no other category/count is prevalidated or read.
    words[8]=speech->cursors[0]; //2C1B08 LBU21D.
    words[9]=counts->lowDamage; //2C1B0C LBU214.
    const std::uint32_t selected=*words[8]+1u;words[10]=selected;
    if(!*words[9])return Impl::stop(d,0x2c1b14,"Reached zero original category divisor unsupported");
    speech->cursors[0]=static_cast<std::uint8_t>(selected%*words[9]); //2C1B28 directSB BEFORE play.
    words[11]=speech->cursors[0];
    if(!scope.playNativeDamageSpeech5(race,selected,pc))
        return Impl::stop(d,pc,"Actual play provider did not return; cursor/effects retained, no cooldown or replay");
    words[12]=1u; //Native actual helper-return custody, not a fabricated guest V0.
    speech->cooldown224=std::bit_cast<float>(std::uint32_t{0x40a00000}); //2C1BDC AFTER helper return.
    words[13]=0x40a00000u;d.sourcePC=0x2c1be0;return true;
}
bool OriginalRaceMusicManager50LA::readQueuedCount150(const OriginalRaceConstructor50LA& race,
    const ::original_race_hud_scope50LA::Registration& scope,std::uint32_t& count)const noexcept {
    if(race.music10C_.get()!=this||!nativeBackendCurrent(scope)||
       impl_->application!=race.application_||impl_->component.sessionGeneration!=race.entity_.sessionGeneration)
        return false;
    //Native FIFO adoption reads the actual shared producer count. Without
    //that adoption, keep the constructor-owned150 lane; unknown guest158 is
    //never cast/dereferenced or replaced by an invented payload pointer.
    auto& p=*impl_;
    if(!p.nativeEvents){count=p.fields.word150;return true;}
    if(!scope.camera_||!scope.endStateCurrent(race,*scope.camera_))return false;
    auto& q=*p.nativeEvents;
    if(q.count150>=q.capacity154||q.capacity154>q.entries.size())return false;
    if(q.endFlushOwner50LA_!=this){
        if(q.endFlushOwner50LA_||q.flushing||q.dispatchIndex)return false;
        q.endFlushOwner50LA_=this;q.flushing=true; //Native exclusive consumer custody.
        q.dispatchIndex=0; //Source S0=0 before initial count read.
        p.endQueueInitialCount=q.count150;
    }
    if(!p.endQueueInitialCount||p.endQueueStopped)return false;
    count=*p.endQueueInitialCount;return true; //Never repair a saved first read.

}
bool OriginalRaceMusicManager50LA::dispatchNativeQueuedEvent3_1A4D30(
    const OriginalRaceConstructor50LA& race,const ::original_race_hud_scope50LA::Registration& scope,
    std::uint32_t savedInitialCount,OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    d={};if(race.music10C_.get()!=this||!nativeBackendCurrent(scope)||!impl_->nativeEvents||
       impl_->application!=race.application_||impl_->component.sessionGeneration!=race.entity_.sessionGeneration)
        return Impl::stop(d,0x1a4cd4,"Actual same-manager native FIFO/payload owner unavailable");
    if(!scope.camera_||!scope.endStateCurrent(race,*scope.camera_))
        return Impl::stop(d,0x1a4cd4,"Exact same App/Bank/manager/current Camera join unavailable");
    auto& p=*impl_;auto& q=*p.nativeEvents;
    if(q.endFlushOwner50LA_!=this||!q.flushing||!p.endQueueInitialCount||
       *p.endQueueInitialCount!=savedInitialCount)
        return Impl::stop(d,0x1a4cd4,"Saved End count/cursor does not own this native FIFO");
    if(p.endQueueStopped)return Impl::stop(d,p.endQueueStopPC,
        "Saved unsupported event and consumed prefix retained; no payload reread, replay or clear");
    //Event3 is JR/NOP. Supported1/5/6 paths use only this same Registration's
    //real source-ordered actors and held native helpers; other cuts remain stopped.
    std::uint32_t count=savedInitialCount;
    for(;;){
        if(q.dispatchIndex>=count){d.sourcePC=0x1a4d04;return true;}
        p.endQueueStopPC=0x1a4cd4;
        if(q.dispatchIndex>=q.entries.size()||q.dispatchIndex>=q.count150){
            p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,"Actual indexed native event158 unavailable");
        }
        p.endQueuePending=q.entries[q.dispatchIndex]; //Retain actual descriptor/ticket, not externally supplied event.
        p.endQueueStopPC=0x1a4d38;
        if(!p.endQueuePending->ticket){
            p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,"Actual event native ticket unavailable");
        }
        const auto type=p.endQueuePending->event.type4;
        if(type==1u){
            if(!p.endContactStarted){
                p.endContactStarted=true;p.endContactCursor.lastTicket=p.endQueuePending->ticket;
                if(!scope.prepareNativeContactEvent1(race,p.endQueuePending->event,
                    p.endContactCursor,p.endContactWords,p.endContactRequest,
                    p.endContactNeedsAnnouncer,p.endContactNeedsSpeech,p.endContactSpeechTaunt,
                    p.endContactSpeechActor,p.endQueueStopPC)){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Actual contact prefix retained; selected D48/actor/chance boundary remains unsupported");
                }
            }
            if(p.endContactNeedsSpeech){
                if(!invokeNativeContactSpeech1(race,scope,d)){
                    p.endQueueStopped=true;p.endQueueStopPC=d.sourcePC;return false;
                }
                p.endContactNeedsSpeech=false; //AFTER actual helper return, never a synthetic ack.
                //Source selected flags are mutually exclusive; after partner
                //return1A5AB0 tests false S4, or self returns at1A5AC4.
                if(!scope.finishNativeContactEvent1(race,p.endContactCursor,p.endContactWords,
                    p.endContactRequest,p.endContactNeedsAnnouncer,p.endQueueStopPC)){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Post-speech source contact tail retained; no helper/draw replay");
                }
            }
            if(p.endContactNeedsAnnouncer){
                const auto state=scope.application_->state;
                if(!state||!state->clock101A50||!p.announcer||
                   p.announcer->raw_!=&state->rawRandom308030||
                   p.announcer->lazy_!=&state->sharedRandomSelectors.seed||
                   p.announcer->providers_.sharedRange!=&state->sharedRandomSelectors.range||
                   p.announcer->clock_!=state->clock101A50.get()){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Actual contact Announcer shared raw/lazy/range/clock owner unavailable");
                }
                std::uint32_t ignoredV0=0;
                if(!scope.invokeNativeLapEvent6(race,p.endContactRequest,ignoredV0)){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Actual Announcer1/2 did not return; contact request/draws retained without retry");
                }
                p.endContactCursor.phase=DynamicContactEventPhase50IW::Complete;
            }
        }else if(type==5u){
            //3AA284 ->1A4D84 ->1A52B0. Preserve exact pending payload and
            //every reached raw/read prefix; do not invoke atomic50HG dispatch.
            if(!p.endDamageStarted){
                p.endDamageStarted=true;
                if(!scope.prepareNativeDamageEvent5(race,p.endQueuePending->event,
                    p.endDamageWords,p.endDamageRequest,p.endDamageNeedsAnnouncer,p.endDamageNeedsSpeech,p.endQueueStopPC)){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Damage source prefix retained; selected D48 speech/owner/scalar boundary remains unsupported");
                }
            }
            if(p.endDamageNeedsSpeech){
                if(!invokeNativeLowDamageSpeech5(race,scope,d)){
                    p.endQueueStopped=true;p.endQueueStopPC=d.sourcePC;return false;
                }
                //1A5338 is reached even after selected speech; only AFTER the
                //real2C1A90 return. S2 remains true, so no Announcer fallback.
                p.endQueueStopPC=0x1a5338;p.endDamageWords[4]=p.endQueuePending->event.word8;
            }
            if(p.endDamageNeedsAnnouncer){
                const auto state=scope.application_->state;
                if(!state||!state->clock101A50||!p.announcer||
                   p.announcer->raw_!=&state->rawRandom308030||
                   p.announcer->lazy_!=&state->sharedRandomSelectors.seed||
                   p.announcer->providers_.sharedRange!=&state->sharedRandomSelectors.range||
                   p.announcer->clock_!=state->clock101A50.get()){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Actual damage Announcer shared raw/lazy/range/clock owner unavailable");
                }
                std::uint32_t ignoredV0=0;
                //Same already-authenticated native helper adapter used by6.
                //False is terminal; no repaired request or prior draw replay.
                if(!scope.invokeNativeLapEvent6(race,p.endDamageRequest,ignoredV0)){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Actual Announcer12 did not return; damage request/payload/effects retained without retry");
                }
            }
        }else if(type==6u){
            //3AA288 ->1A4D64 ->1A55F0. Never use Loop's actor snapshot or
            //ordinary front/ack API to bypass this manager's exclusive guard.
            if(!p.endLapStarted){
                p.endLapStarted=true;
                if(!scope.beginNativeLapEvent6(race,p.endQueuePending->event.actor0C,
                    p.endLapCursor,p.endQueueStopPC)){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Actual source-ordered lap actor prefix unavailable; saved event/reads retained");
                }
            }
            for(;;){
                const auto step=scope.advanceNativeLapEvent6(race,p.endLapCursor,p.endLapSelectedIndex);
                p.endQueueStopPC=step.sourcePC;
                if(step.status==LapEventStatus50ID::Complete)break;
                if(step.status!=LapEventStatus50ID::NeedHelper){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Lap source scalar/static boundary; earlier helper/RNG effects retained");
                }
                if(step.request.kind==LapEventCall50ID::Announcer2C2C20){
                    const auto state=scope.application_->state;
                    if(!state||!state->clock101A50||!p.announcer||
                       p.announcer->raw_!=&state->rawRandom308030||
                       p.announcer->lazy_!=&state->sharedRandomSelectors.seed||
                       p.announcer->providers_.sharedRange!=&state->sharedRandomSelectors.range||
                       p.announcer->clock_!=state->clock101A50.get()){
                        p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                            "Actual Announcer shared raw/lazy/clock is not the same Application owner");
                    }
                }
                std::uint32_t actualV0=0;
                if(!scope.invokeNativeLapEvent6(race,step.request,actualV0)){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Actual lap provider did not return; held request/payload and effects retained without retry");
                }
                if(!acceptLapEventHelperReturn50ID(p.endLapCursor,step.request.sequence,
                    step.request.kind,actualV0)){
                    p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                        "Returned actual lap helper acknowledgement rejected; no helper replay");
                }
            }
        }else if(type!=3u){
            p.endQueueStopped=true;return Impl::stop(d,p.endQueueStopPC,
                "Actual event retained; only source subscribers3/6 and bounded1/5 branches are joined here");
        }
        //1/5:source suppression/Announcer;3:JR/NOP;6:owned1A55F0 continuation.
        //No added word0/shape checks; each path consumes only reached source fields.
        ++q.dispatchIndex; //1A4CE8 only after actual subscriber return (CE4 advances byte offset20).
        p.endQueuePending.reset();p.endLapCursor={};p.endLapStarted=false;p.endLapSelectedIndex=0;
        p.endDamageWords={};p.endDamageRequest={};p.endDamageStarted=false;p.endDamageNeedsAnnouncer=false;
        p.endDamageNeedsSpeech=false;p.endDamageSpeechWords={};
        p.endDamageSpeechScratch={};p.endDamageSpeechScratchKnown={};
        p.endContactCursor={};p.endContactWords={};p.endContactRequest={};
        p.endContactStarted=false;p.endContactNeedsAnnouncer=false;
        p.endContactNeedsSpeech=false;p.endContactSpeechTaunt=false;p.endContactSpeechActor=0;
        p.endContactSpeechWords={};p.endContactSpeechOffsets={};
        p.endContactSpeechScratch={};p.endContactSpeechScratchKnown={};
        count=q.count150; //1A4CF0 reload live count AFTER each returned subscriber.
        if(count>=q.capacity154||q.capacity154>q.entries.size()){
            p.endQueueStopPC=0x1a4cf0;p.endQueueStopped=true;
            return Impl::stop(d,p.endQueueStopPC,"Native queue reservation invalid after returned prefix");
        }
    }
}
bool OriginalRaceMusicManager50LA::clearQueuedCount1A5C40(const OriginalRaceConstructor50LA& race,
    const ::original_race_hud_scope50LA::Registration& scope)noexcept {
    if(race.music10C_.get()!=this||!nativeBackendCurrent(scope)||
       impl_->application!=race.application_||impl_->component.sessionGeneration!=race.entity_.sessionGeneration)
        return false;
    auto& p=*impl_;
    if(p.nativeEvents){
        if(!scope.camera_||!scope.endStateCurrent(race,*scope.camera_))return false;
        auto& q=*p.nativeEvents;
        if(q.endFlushOwner50LA_){
            if(q.endFlushOwner50LA_!=this||!q.flushing||p.endQueueStopped||
               !p.endQueueInitialCount||q.dispatchIndex!=q.count150)return false;
        }else if(q.flushing||q.dispatchIndex)return false; //Never clear another consumer's live prefix.
        q.count150=0; //Actual shared150 store1A5C44; entries158 remain untouched.
        q.dispatchIndex=0;q.flushing=false;q.endFlushOwner50LA_=nullptr;
        p.endQueueInitialCount.reset();p.endQueuePending.reset(); //Native custody release AFTER source clear.
    }else p.fields.word150=0; //Existing genuinely unjoined native queue path.
    return true;
}
void OriginalRaceMusicManager50LA::revokeNativeProviders(
    const ::original_race_hud_scope50LA::Registration& scope)noexcept {
    if(!impl_||impl_->thread!=std::this_thread::get_id()||impl_->registration.lock().get()!=&scope)return;
    impl_->music=nullptr;impl_->announcer=nullptr;impl_->live128_12C_130=nullptr;
    impl_->nativeAnnouncerState=nullptr;
    //Do not clear a suspended queue. Session lifetime closes its backing only
    //after this registration revokes; private guard remains a nondereferenced
    //identity so other consumers cannot mistake abandonment for completion.
    impl_->nativeEvents=nullptr;impl_->registration.reset();
}
bool OriginalRaceMusicManager50LA::retireNativeDetached(OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    d={};if(!impl_)return true;auto& p=*impl_;
    if(p.thread!=std::this_thread::get_id())return Impl::stop(d,0x1a3af0,"Music manager retirement thread mismatch");
    if(p.closed)return true;
    if(p.sourceOnly&&!retireSourceMusicChildNative(d))return false;
    if(p.listenerAttempted&&!p.listenerRetired){
        platform::OriginalApplicationAural50KG::Diagnostic listener;
        if(!p.aural||!p.aural->removeSingleRaceListener103040(*this,listener))
            return Impl::stop(d,listener.sourcePC,"Actual listener removal must precede manager retirement");
        p.listenerRetired=true; // Native receipt AFTER1A45C0's ignored-return removal.
    }
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
Application::All8MusicStatus Application::stageConsumedRaAll8MusicNative10C(
    original_camera_session50KX::Session& session,std::uint32_t& pc)const noexcept {
    using S=All8MusicStatus;using Manager=rr::game::OriginalRaceMusicManager50LA;
    pc=0x1a6c90;
    // Genuine completed native services on the exact ALL8 Session. This
    // private adapter does not assert a retail1A9240 return, promote Camera34,
    // or call the separate human Camera/Music/world completion route.
    if(!consumedRaAll8ListenerCustodyNativeCurrent(session))return S::unavailable;
    const auto race=sourceRace28COwner_;const auto scope=race->endHud_.lock();
    if(race->music10C_)return S::failed; //One synchronous native entry, no replay/adoption.
    if(!Manager::freshAll8NativePrefix(*race,*scope))return S::unavailable;
    // Unsupported/missing/used native preparation above has no source effect.
    // Once entered, real allocation/count/load failure is terminal to this
    // publication attempt. Retain the actual partial owner for existing close.
    try {
        std::unique_ptr<Manager> next(new Manager(entitiesOwner_,race->entity_.sessionGeneration,scope));
        if(!next->constructed()){pc=0x1a39f0;return S::failed;}
        race->music10C_=std::move(next); //Actual1A6CAC publication BEFORE load38/44.
    }catch(...){return S::failed;}
    rr::game::OriginalRaceMusicManagerDiagnostic50LA d;
    // Reuse the actual shared native child owners and exact existing listener
    // allocator, not a second PCM/mixer or invented original child pointers.
    // This preserves38/44, reached activation/reservation/allocation stores and
    // real listener cleanup custody on any later failure. No fallback/replay.
    if(!race->music10C_->acceptNativeChildren(*race,sourceAural00Owner_,d)){
        pc=d.sourcePC;return S::failed;
    }
    pc=d.sourcePC;return S::nativeChildrenReady;
}
bool Application::consumedRaAll8ListenerCustodyNativeCurrent(
    const original_camera_session50KX::Session& session)const noexcept {
    if(!consumedRaAll8CameraCustodyNative34Current(session))return false;
    const auto race=sourceRace28COwner_;const auto scope=race->endHud_.lock();
    // Camera-only custody remains independent of Scope for active-Pause close.
    // This separate live listener borrow requires the exact lexical providers
    // and manager. Never publish camera_/HUD or authorize human End/Results.
    return scope&&scope->thread_==std::this_thread::get_id()&&scope->active_&&
        scope->application_==this&&scope->race_.lock()==race&&
        scope->generation_==race->entity_.sessionGeneration&&scope->bank_==&session.actual_.bank&&
        scope->manager_ && &scope->manager_->manager==&session.actual_.all8Manager&&
        !scope->camera_&&!scope->published_;
}
bool Application::advanceConsumedRaAll8MusicListenerNative(
    const original_camera_session50KX::Session& session,std::uint64_t beforeMix,
    bool heldSourceScene,std::uint32_t& pc)const noexcept {
    pc=0x1a4440;
    if(!consumedRaAll8CameraCustodyNative34Current(session))return false;
    const auto race=sourceRace28COwner_;
    // Only the never-entered native preparation route has no counted owner.
    // This includes an unavailable lexical Scope. Once10C exists, neither
    // revoked providers, partial load nor failed consumption can fall back.
    if(!race->music10C_)return true;
    if(!consumedRaAll8ListenerCustodyNativeCurrent(session))return false;
    const auto scope=race->endHud_.lock();
    rr::game::OriginalRaceMusicManagerDiagnostic50LA d;
    const bool okay=race->music10C_->advanceNativeListenerConsumedStep(*race,*scope,session,
        beforeMix,heldSourceScene,d);
    pc=d.sourcePC;return okay;
}
bool Application::serviceConsumedRaAll8MenuAudioNative(
    const original_camera_session50KX::Session& session,live_audio50HF::Owner& actualGameplay,
    std::uint32_t micros,rr::platform::AudioOutputDiagnostic50HD& audio,std::uint32_t& pc)const noexcept {
    pc=0x1a4440;
    if(!consumedRaAll8CameraCustodyNative34Current(session))return false;
    const auto race=sourceRace28COwner_;
    const auto pause=race->pause100_;std::uint32_t paused=0;
    if(!pause||!pause->active200_||!pause->entity_||
       !race->nativeRootAttached_||pause->entity_->application!=entitiesOwner_||
       !pause->entity_->initialized||pause->entity_->releasing||
       !pause->entity_->currentForest()||pause->entity_->component.parent1C!=entitiesOwner_->application10()||
       pause->entity_->component.mask20!=0x1402u||(pause->entity_->component.flags29&3u)!=3u||
       !session.actual_.applicationAnimation.readPause11C(paused)||!paused)return false;
    // Invoke the existing service here, rather than accepting a caller-supplied
    // gap receipt or resetting the listener cursor at Resume. No second mix.
    if(!race->music10C_)return actualGameplay.serviceNativeMenuAudio(micros,audio);
    if(!consumedRaAll8ListenerCustodyNativeCurrent(session))return false;
    const auto scope=race->endHud_.lock();
    if(scope->gameplay_!=&actualGameplay)return false;
    rr::game::OriginalRaceMusicManagerDiagnostic50LA d;
    const bool okay=race->music10C_->serviceNativeMenuAudio(*race,*scope,micros,audio,d);
    pc=d.sourcePC;return okay;
}
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
bool Application::advanceConsumedRaMusicListenerNative(
    const original_camera_session50KX::Session& session,std::uint64_t beforeMix,
    bool heldSourceScene,std::uint32_t& pc)const noexcept {
    pc=0x1a4440;
    if(!consumedRaMusicNative10CCurrent(session))return false;
    const auto race=sourceRace28COwner_;const auto scope=race->endHud_.lock();
    if(!scope||scope->application_!=this||scope->camera_!=&session)return false;
    rr::game::OriginalRaceMusicManagerDiagnostic50LA d;
    const bool okay=race->music10C_->advanceNativeListenerConsumedStep(*race,*scope,session,
        beforeMix,heldSourceScene,d);
    pc=d.sourcePC;return okay;
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
    if(race.sourcePlayerMusicBorrows_)return false;
    if(!race.music10C_)return true;
    rr::game::OriginalRaceMusicManagerDiagnostic50LA d;
    if(!race.music10C_->retireNativeDetached(d)){pc=d.sourcePC;return false;}
    race.music10C_.reset();return true; //Native slot withdrawal, not source null store.
}
}
namespace original_camera_session50KX {
bool Session::advanceNativeMusicListenerConsumedStep(std::uint64_t beforeMix,
    bool heldSourceScene,std::uint32_t& pc)const noexcept {
    pc=0x1a4440;
    if(!onOwnerThread())return false;
    // Other courses have no joined counted RA10C route. Do not fabricate one.
    if(!nativeTaIssuance_||nativeTaIssuance_->course!=rr::game::OriginalCameraStartCourse::ra)return true;
    if(!nativeTaIssuance_->issuer)return false;
    return nativeTaIssuance_->issuer->advanceConsumedRaMusicListenerNative(*this,beforeMix,heldSourceScene,pc);
}
bool Session::completeCountedRaceMusicNative10C(std::uint32_t& pc)noexcept {
    pc=0x1a6c90;
    if(f8Intro_.ownerThread_!=std::this_thread::get_id())return false;
    if(!nativeTaIssuance_||nativeTaIssuance_->course!=rr::game::OriginalCameraStartCourse::ra)return true;
    std::uint32_t pause=0;
    if(!nativeTaIssuance_->issuer||!actual_.applicationAnimation.readPause11C(pause))return false;
    return nativeTaIssuance_->issuer->completeConsumedRaMusicNative10C(*this,pc);
}
}

namespace original_main_entry50KG {
std::shared_ptr<const rr::game::OriginalPlayerRaceMusicBorrow50LA>
Application::borrowSourcePlayerMusic210810(const rr::game::OriginalRaceConstructor50LA* expected)const noexcept {
    const auto race=sourceRace28COwner_;
    if(!enabled||!expected||!race||race.get()!=expected||raceConstructorColdOwner50LA_.lock()!=race||
       garageAudioOwnerThread_!=std::this_thread::get_id()||race->resultsFeedbackThread_!=std::this_thread::get_id()||
       race->application_!=entitiesOwner_||race->profile_!=riderProfile||!race->counted_||race->rolledBack_||
       race->entity_.reference00!=1||!race->entity_.sessionGeneration||!race->music10C_||
       !race->music10C_->constructed()||race->sourcePlayerMusicBorrows_==std::numeric_limits<std::size_t>::max())return {};
    //Only an existing canonical1A6CAC publication is read. NEVER invoke its
    //native preparation path or fabricate source-knownNULL from absent custody.
    try{return std::shared_ptr<const rr::game::OriginalPlayerRaceMusicBorrow50LA>(
        new rr::game::OriginalPlayerRaceMusicBorrow50LA(race,race->music10C_.get(),race->sourcePlayerMusicBorrows_));}
    catch(...){return {};}
}
}

namespace original_main_entry50KG {
bool Application::readSourcePlayerRaceMode199980(const rr::game::OriginalRaceConstructor50LA* expected,
    std::int32_t& mode)const noexcept {
    using Step=rr::game::OriginalRaceConstructor50LA::CameraInitStep;
    const auto& race=sourceRace28COwner_;
    if(!enabled||!race||race.get()!=expected||raceConstructorColdOwner50LA_.lock()!=race||
       garageAudioOwnerThread_!=std::this_thread::get_id()||race->resultsFeedbackThread_!=std::this_thread::get_id()||
       race->application_!=entitiesOwner_||race->profile_!=riderProfile||!race->counted_||race->rolledBack_||
       race->cameraInitStep_==Step::unreached||race->cameraInitStep_==Step::nativeRetiring||
       race->cameraInitStep_==Step::retired)return false;
    //Current canonical producer:1A9284 writes0 before advancing to pause.
    //The C++ field initializer alone is NOT provenance. No camera method is
    //invoked here, and no native Session copy supplies this word.
    mode=std::bit_cast<std::int32_t>(race->field8C_);return true;
}
}

namespace rr::game {
OriginalPlayerOverlayRendererLease50LA::OriginalPlayerOverlayRendererLease50LA(
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> display,
    const ::original_main_entry50KG::Application* issuer)noexcept
    :display_(std::move(display)),issuer_(issuer),generation_(display_->generation_),
     applicationLifetime_(issuer->animationClock()){display_->pin();}
OriginalPlayerOverlayRendererLease50LA::~OriginalPlayerOverlayRendererLease50LA()noexcept {
    if(display_)display_->unpin();
}
bool OriginalPlayerOverlayRendererLease50LA::current(bool acquiring)const noexcept {
    std::uint32_t pause=0;
    if(!display_||!applicationLifetime_.readPause11C(pause)||!issuer_||
       !display_->ownerThread()||display_->application_!=issuer_||display_->generation_!=generation_||
       !generation_||display_->video_!=issuer_->videoOwner_||
       issuer_->raceDisplayOwner50LA_.lock()!=display_)return false;
    if(acquiring&&display_->retiring_)return false;
    // Held-pin native cancellation remains legal during retirement and at
    // pin capacity. Never resurrect a destroyed/replaced host/context.
    return display_->localizationServiceIdentityCurrent50KG()&&display_->created_&&
        display_->video_->originalMain&&display_->video_->host.created()&&
        display_->video_->host.current()&&display_->renderer_.initialized();
}
bool OriginalPlayerOverlayRendererLease50LA::bindMaterial(OriginalAIDamageMaterialRow50LA& row)const noexcept {
    if(!current(true)||!issuer_->standardMaterialDomainOwner_)return false;
    rr::assets::OriginalStandardMaterialDiagnostic50LA d;
    return row.bindApplication(issuer_->standardMaterialDomainOwner_->materials382B38(),d);
}
bool OriginalPlayerOverlayRendererLease50LA::constructArrays(OriginalPodAIDamageOverlay50LA& overlay,
    std::uint32_t& pc)noexcept {
    pc=0x199990;
    if(!current(true)||!overlay.sourceMode_||!overlay.sourceMode40Known_||
       overlay.sourceArraysAttempted_||overlay.array30||overlay.array34)return false;
    OriginalAIDamageMaterialRow50LA material;
    if(!bindMaterial(material))return false;
    // No directory copy/override and no row population is fabricated here.
    const auto& directory=issuer_->materialDirectory36B970_;
    if(!material.acceptsDirectory(directory.c_str(),directory.size()+1u))return false;
    overlay.sourceArraysAttempted_=true;
    for(unsigned i=0;i<2;++i){
        overlay.sourceArrays_[i].reset(new(std::nothrow) OriginalAIDamagePrimitiveArray50LA);
        auto*& slot=i==0?overlay.array30:overlay.array34;
        slot=overlay.sourceArrays_[i].get(); //1999A8 publishes even NULL.
        pc=0x2e1ae0;
        if(!slot)return false; //Before original null receiver dereference.
        if(!slot->construct199950(material,directory.c_str(),directory.size()+1u)){
            pc=slot->materialDiagnostic().sourcePC;
            if(!pc)pc=0x2e1ae0;return false;
        }
    }
    //133F20 on array30 only, after both original setup returns.
    overlay.array30->primaryColor199A7C();overlay.prepared_=true;
    pc=0x199ab0;return true;
}
bool OriginalPlayerOverlayRendererLease50LA::cancelArrays(OriginalPodAIDamageOverlay50LA& overlay)noexcept {
    if(!current(false))return false;
    // Both obligations are retained:244D60(1), then unconditional245090.
    // Callbacks and ANY nonempty source queue remain unimplemented owners;
    // completing this native context cannot discharge them.
    if(display_->dispatch_.pendingCallbackDC||display_->dispatch_.count54||display_->dispatch_.count60)return false;
    rr::platform::VehicleWindowDiagnostic50DS d;
    const auto result=overlay.releaseArrays1998A0(display_->dispatch_,display_->renderer_,d);
    if(!result.okay())return false;
    for(auto& array:overlay.sourceArrays_)array.reset();
    overlay.sourceArraysAttempted_=false;return true;
}
}
namespace original_main_entry50KG {
std::shared_ptr<rr::game::OriginalPlayerOverlayRendererLease50LA>
Application::borrowSourcePlayerOverlayRenderer199990(const rr::game::OriginalRaceConstructor50LA* expected)const noexcept {
    std::int32_t mode=0;
    if(!readSourcePlayerRaceMode199980(expected,mode)||!standardMaterialDomainOwner_||!videoOwner_)return {};
    const auto display=raceDisplayOwner50LA_.lock();
    if(!display||display->application_!=this||display->video_!=videoOwner_||!display->generation_||
       !display->canRetain())return {};
    std::uint32_t pause=0;if(!animationClock().readPause11C(pause))return {};
    try{return std::shared_ptr<rr::game::OriginalPlayerOverlayRendererLease50LA>(
        new rr::game::OriginalPlayerOverlayRendererLease50LA(display,this));}
    catch(...){return {};}
}
}
