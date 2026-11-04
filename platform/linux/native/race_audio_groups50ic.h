#pragma once
// Main-thread bridge from the recovered original group owner to the native
// mixer. Host voice-slot assignments are supplied explicitly by each provider.
#include "audio_mixer50hd.h"
#include "../../../src/game/announcer_callbacks50ic.h"
#include "../../../src/game/original_ui_game_options_controller50kg.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
#include <array>
#include <bit>
#include <memory>
#include <thread>
#include "../../../src/game/original_application_entities50la.h"
#include <exception>
#include <optional>
#include "original_application_aural50kg.h"
#include "../../../src/assets/original_ta_intro_authoring_candidate.h"
#include "native_race_intro_transport50la.h"
#include "native_player_speech_constructor50la.h"
#include "../../../src/game/pod_speech50hg.h"

namespace live_audio50HF { class Owner; }
namespace interactive_loop50DU { struct Context; }
namespace rr::game { class OriginalRaceConstructor50LA; class OriginalGcIntroExitOwner; }
namespace rr::platform {
class RaceAudioGroups50IC final {
    friend struct ::interactive_loop50DU::Context;
    friend class OriginalApplicationAural50KG;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::live_audio50HF::Owner;
    friend class rr::game::OriginalRaceConstructor50LA;
    friend class rr::game::OriginalGcIntroExitOwner;
    friend class ::original_camera_session50KX::Session;
public:
    RaceAudioGroups50IC() noexcept { bindings_.fill(game::RaceAudioGroup50IC::Unrepresented); }
    ~RaceAudioGroups50IC()noexcept { if(coldTaIntroEmitter_||coldNativePlayerSpeech_)std::terminate();for(const auto& p:coldGroups_)if(p.constructed)std::terminate(); }
    RaceAudioGroups50IC(const RaceAudioGroups50IC&)=delete;
    RaceAudioGroups50IC& operator=(const RaceAudioGroups50IC&)=delete;
    RaceAudioGroups50IC(RaceAudioGroups50IC&&)=delete;
    RaceAudioGroups50IC& operator=(RaceAudioGroups50IC&&)=delete;

    // New race owner only: mixer has been initialized and has no active or
    // previously group-bound voices. Settings must come from the shared config
    // owner. Fresh defaults are an explicit separate source operation.
    bool initialize(AudioMixer50HD& mixer,const game::RaceAudioSettings50IC& settings) noexcept {
        if(coldStarted_&&(coldRetiring_||coldNext_!=game::raceAudioGroupCount50IC||coldMixer_!=&mixer||
           coldThread_!=std::this_thread::get_id()||coldEpoch_!=mixer.groupEpoch50IC()))return false;
        if(mixer_||!mixer.count()||mixer.requiresVoiceGroupGains50IC())return false;
        for(std::size_t i=0;i<mixer.count();++i)
            if(mixer.active(i)||mixer.hasVoiceGroupGain50IC(i))return false;
        game::RaceAudioGroups50IC next;
        const auto initialized=game::initializeRaceAudioGroups50IC(settings,next);
        if(!initialized.complete||!mixer.requireVoiceGroupGains50IC())return false;
        mixer_=&mixer;epoch_=mixer.groupEpoch50IC();count_=mixer.count();
        settings_=settings;groups_=next;duck_={};return true;
    }
    bool current()const noexcept {
        if(coldStarted_&&coldThread_!=std::this_thread::get_id())return false;
        // Populated source Sounds are admitted only through the concrete TA
        // source transport, member/service and lifetime checks below.
        if(coldTaIntroEmitter_&&!taIntroBackingCurrent())return false;
        if(coldNativePlayerSpeech_&&!nativePlayerSpeechBackingCurrent())return false;
        for(std::size_t n=0;n<coldGroups_.size();++n){const auto& p=coldGroups_[n];
            if((p.count48||p.taIntroAuthoring||p.taIntroManager)&&
               (n!=static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)||!taIntroBackingCurrent()))return false;
        }
        return (!coldStarted_||coldThread_==std::this_thread::get_id())&&
            !coldRetiring_&&!nativeBindingsRetired_&&mixer_&&mixer_->groupEpoch50IC()==epoch_&&mixer_->count()==count_&&
            mixer_->requiresVoiceGroupGains50IC();
    }
    bool uses(const AudioMixer50HD& mixer)const noexcept {return current()&&mixer_==&mixer;}
    static bool represented(game::RaceAudioGroup50IC group)noexcept {
        return static_cast<std::size_t>(group)<game::raceAudioGroupCount50IC;
    }
    bool canBind(std::size_t slot,game::RaceAudioGroup50IC group)const noexcept {
        if(!current()||slot>=count_||!represented(group))return false;
        // Native provider membership is the actual bindings_ ledger below,
        // not a fabricated source logicalMembers4C Entity registration.
        if(coldStarted_){
            const auto& p=coldGroups_[static_cast<std::size_t>(group)];
            if(coldThread_!=std::this_thread::get_id()||coldMixer_!=mixer_||coldEpoch_!=epoch_||
               !coldApplication_||!p.constructed||!p.loaded||(p.count48&&(!taIntroBackingCurrent()||group!=game::RaceAudioGroup50IC::Announcer120))||
               p.slot40<0||static_cast<std::size_t>(p.slot40)>=coldApplication_->soundGroups434A10_.size()||
               coldApplication_->soundGroups434A10_[p.slot40]!=&p.entity)return false;
        }
        if(bindings_[slot]==group)return mixer_->hasVoiceGroupGain50IC(slot);
        return bindings_[slot]==game::RaceAudioGroup50IC::Unrepresented&&
            !mixer_->active(slot)&&!mixer_->hasVoiceGroupGain50IC(slot);
    }
    // Bind only after the caller has proved this provider's constructor group.
    // A slot cannot change owner group mid-race. Same-owner rebinding is inert.
    bool bind(std::size_t slot,game::RaceAudioGroup50IC group)noexcept {
        if(!canBind(slot,group))return false;
        if(bindings_[slot]==group)return true;
        const auto changes=1u+unsigned(mixer_->groupPaused50IC_[slot]!=
            (groups_.state44[static_cast<std::size_t>(group)]!=0));
        if(!mixer_->canRecordControlChanges(changes))return false;
        if(!mixer_->setVoiceGroupGain50IC(slot,normalized(groups_,group)))return false;
        if(!mixer_->setVoiceGroupPaused50IC(slot,groups_.state44[static_cast<std::size_t>(group)]!=0))return false;
        bindings_[slot]=group;return true;
    }
    bool bindRange(std::size_t first,std::size_t count,game::RaceAudioGroup50IC group)noexcept {
        if(!current()||!count||first>=count_||count>count_-first||!represented(group))return false;
        std::uint64_t changes=0;
        for(std::size_t i=0;i<count;++i){
            if(!canBind(first+i,group))return false;
            if(bindings_[first+i]!=group)changes+=1u+unsigned(mixer_->groupPaused50IC_[first+i]!=
                (groups_.state44[static_cast<std::size_t>(group)]!=0));
        }
        if(!mixer_->canRecordControlChanges(changes))return false;
        // All setters are now infallible on this single-threaded owner.
        for(std::size_t i=0;i<count;++i)if(!bind(first+i,group))return false;
        return true;
    }
    game::RaceAudioGroup50IC binding(std::size_t slot)const noexcept {
        return current()&&slot<count_?bindings_[slot]:game::RaceAudioGroup50IC::Unrepresented;
    }
    // A shared config edit changes the eventual source restore target. It does
    // not silently rewrite currently ducked levels or simulate other setters.
    bool setConfiguredLevels(const game::RaceAudioSettings50IC& settings)noexcept {
        if(!current()||!game::announcer_callbacks50IC_detail::settings(settings))return false;
        settings_=settings;return true;
    }
    // Invoke at the actual recovered callback boundaries, never a host timer.
    // The returned source receipt is diagnostic; its writes were already
    // applied here and must not be multiplied into ordinary per-voice Controls.
    game::AnnouncerDuckingReceipt50IC begin()noexcept {
        if(!current())return {};
        auto groups=groups_;auto duck=duck_;
        auto receipt=game::beginAnnouncerDucking50IC(groups,duck);
        if(!receipt.complete||!synchronizeDucking(receipt,duck,true))return {};
        return receipt;
    }
    game::AnnouncerDuckingReceipt50IC end()noexcept {
        if(!current())return {};
        auto groups=groups_;auto duck=duck_;
        auto receipt=game::endAnnouncerDucking50IC(settings_,groups,duck);
        if(!receipt.complete||!synchronizeDucking(receipt,duck,false))return {};
        return receipt;
    }
    // Audio suffix2356EC..700 reached by actual1ACF50 World4D8 setter.
    // The original caller must first perform Entity142040 propagation; this
    // method is not a receipt for that prefix or a full Pause open/close.
    bool pauseWorld2356EC(bool requested)noexcept {
        return pauseGroupSuffix2356EC(game::RaceAudioGroup50IC::World4D8,requested);
    }
    // Borrow the SAME initialized group word for source logical-sound
    // inhibition. The original world pause suffix updates this SAME owner.
    const std::uint32_t* sourceState44(game::RaceAudioGroup50IC group)const noexcept {
        return current()&&groups_.initialized&&represented(group)?
            &groups_.state44[static_cast<std::size_t>(group)]:nullptr;
    }
    const game::RaceAudioGroups50IC& levels()const noexcept{return groups_;}
    const game::RaceAudioSettings50IC& configuredLevels()const noexcept{return settings_;}
    const game::AnnouncerDuckingState50IC& ducking()const noexcept{return duck_;}
    enum class TaIntroDispatchResult : std::uint8_t {unavailable,sourceFalse,sourceTrue};
    bool taIntroReady()const noexcept;
    TaIntroDispatchResult dispatchTaIntro292360(unsigned eventId,unsigned restart=0,unsigned immediate=0)noexcept;
    bool stopTaIntro2924A0()noexcept;
    bool serviceTaIntro235720(const original_ui_source_timing50LH::PublicationView&)noexcept;
    bool retireTaIntro142590()noexcept;
    // Inactive source constructor/load leaves. Caller must interleave actual
    // world141300 before the seventh group and attach the six groups as race children itself.
    bool constructNextCold2350D0(AudioMixer50HD&,
        std::shared_ptr<game::OriginalApplicationEntities50LA>,std::uint64_t session,
        game::RaceAudioGroup50IC)noexcept;
    bool releaseEmptyCold142590(game::RaceAudioGroup50IC)noexcept;
    // Explicit native shutdown after real provider voice stops. Does not stop
    // live playback or execute source2353D0 logical-member callbacks.
    bool retireNativeVoiceBindings()noexcept;
private:
    // Shared private native Pause transport. The human loop authenticates
    // its Application/profile/Camera/liveAudio; the canonical Pause caller
    // separately authenticates actual Race/group/World ownership. Neither uses
    // the host next-session settings copy. This leaf alone is not an original
    // Pause callback or a source26D200/SPU command receipt.
    bool observeNativePauseAudio(const game::OriginalUiGameSettingsProfile50KG& profile,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        const AudioMixer50HD& mixer,std::array<std::uint32_t,4>& out)const noexcept {
        namespace d=game::announcer_callbacks50IC_detail;
        if(std::fegetround()!=FE_TONEAREST||!application||!uses(mixer)||
           !game::validOriginalUiGameSettingsProfile50KG(profile)||!d::settings(settings_)||
           !d::valid(groups_)||!mixer_->canRecordControlChanges())return false;
        const std::array<std::uint32_t,4> configured{{std::uint32_t(settings_.effects11020),
            std::uint32_t(settings_.music11024),std::uint32_t(settings_.character11028),
            std::uint32_t(settings_.announcer1102C)}};
        if(profile.volume11020!=configured)return false;
        if(coldStarted_){
            if(coldApplication_!=application||coldMixer_!=mixer_||coldEpoch_!=epoch_||
               coldNext_!=game::raceAudioGroupCount50IC||application->attachmentLock423F58())return false;
            for(const auto& group:coldGroups_){
                if(!group.constructed||!group.loaded||group.entity.sessionGeneration!=coldSession_||
                   group.slot40<0||std::size_t(group.slot40)>=application->soundGroups434A10_.size()||
                   application->soundGroups434A10_[group.slot40]!=&group.entity)return false;
            }
        }
        // Compare to CURRENT group words, not the configured restore target:
        // ducking, or a prior Pause adjustment while ducked, can differ from it.
        for(std::size_t i=0;i<count_;++i){
            if(!represented(bindings_[i])){
                if(bindings_[i]!=game::RaceAudioGroup50IC::Unrepresented)return false;
                continue;
            }
            if(!mixer_->hasVoiceGroupGain50IC(i)||
               mixer_->groupGains50IC_[i]!=normalized(groups_,bindings_[i])||
               mixer_->groupPaused50IC_[i]!=(groups_.state44[static_cast<std::size_t>(bindings_[i])]!=0))return false;
        }
        out=configured;return true;
    }
    bool adjustNativePauseAudio(game::OriginalUiGameSettingsProfile50KG& profile,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        AudioMixer50HD& mixer,const game::OriginalApplicationAnimationClockView50KG& clock,
        const std::array<std::uint32_t,4>& observed,std::uint32_t row,bool increase)noexcept {
        namespace d=game::announcer_callbacks50IC_detail;
        std::array<std::uint32_t,4> current{};std::uint32_t paused=0;
        if(row>=4||!observeNativePauseAudio(profile,application,mixer,current)||current!=observed||
           !clock.readPause11C(paused)||!paused)return false;
        const auto old=current[row];
        if((increase&&old==100u)||(!increase&&old==0u))return true; //Source limit: no store/setter.
        const auto next=increase?(old>90u?100u:old+10u):(old<10u?0u:old-10u);
        auto configured=settings_;
        auto* field=row==0?&configured.effects11020:row==1?&configured.music11024:
            row==2?&configured.character11028:&configured.announcer1102C;
        *field=static_cast<std::int32_t>(next);
        using G=game::RaceAudioGroup50IC;
        const std::array<G,3> order=row==0?std::array<G,3>{{G::Effects110,G::Engines114,G::World4D8}}:
            std::array<G,3>{{row==1?G::Music118:row==2?G::Character11C:G::Announcer120,
                G::Unrepresented,G::Unrepresented}};
        const std::size_t writes=row==0?3u:1u;
        std::array<std::int32_t,3> levels{};std::array<float,3> gains{};
        std::uint64_t changes=0;
        for(std::size_t n=0;n<writes;++n){
            // Same bounded binary32 product/truncation as the existing race
            // setup and restore. Native admission requires FE_TONEAREST.
            levels[n]=d::restored(order[n],configured);
            gains[n]=static_cast<float>(levels[n])/100.f;
            for(std::size_t i=0;i<count_;++i)if(bindings_[i]==order[n]){
                if(!mixer_->nativeWetVoiceAdmitted(i)&&!mixer_->nativeRaceIntroClaimed(i))return false;
                if(mixer_->groupGains50IC_[i]!=gains[n])++changes;
            }
        }
        // Preflight the entire native gain transaction, including revision
        // capacity, before the source profile stores. No counter wrap/retry.
        if(!mixer_->canRecordControlChanges(changes))return false;
        auto& value=profile.volume11020[row];
        value+=increase?10u:std::uint32_t(-10); //17FE30/90/F0 or17FF50 first store.
        if(increase){if(value>100u)value=100u;}
        else if(std::bit_cast<std::int32_t>(value)<0)value=0u;
        settings_=configured; //SAME current target for later actual duck end.
        for(std::size_t n=0;n<writes;++n){
            groups_.volume38[static_cast<std::size_t>(order[n])]=levels[n]; //235500 BEFORE transport.
            for(std::size_t i=0;i<count_;++i)if(bindings_[i]==order[n]){
                // All setter rejection/capacity paths were preflighted on
                // this serial owner. Never roll back or replay reached stores.
                if(!mixer_->setVoiceGroupGain50IC(i,gains[n]))std::terminate();
            }
        }
        // Preserve duck flags/deltas, state44, per-voice controls and cursors.
        // No source feedback3/4/6, mixing, pause toggling or disk save is implied.
        return true;
    }
    // Explicit native audio-only restore action. Source1865CC..F8 provides
    // the four defaults; source race setup supplies category scale/order.
    // This is NOT whole Game Options1F6840, a profile reset, or Pause input.
    bool restoreNativePauseVolumes(game::OriginalUiGameSettingsProfile50KG& profile,
        const std::shared_ptr<game::OriginalApplicationEntities50LA>& application,
        AudioMixer50HD& mixer,const game::OriginalApplicationAnimationClockView50KG& clock,
        const std::array<std::uint32_t,4>& observed)noexcept {
        namespace d=game::announcer_callbacks50IC_detail;
        std::array<std::uint32_t,4> current{};std::uint32_t paused=0;
        if(!observeNativePauseAudio(profile,application,mixer,current)||current!=observed||
           !clock.readPause11C(paused)||!paused)return false;
        constexpr auto defaults=game::freshOriginalAudioSettings50IC();
        using G=game::RaceAudioGroup50IC;
        constexpr std::array<G,6> order{{G::Effects110,G::Engines114,G::World4D8,
            G::Music118,G::Character11C,G::Announcer120}};
        std::array<std::int32_t,6> levels{};std::array<float,6> gains{};
        std::uint64_t changes=0;
        for(std::size_t n=0;n<order.size();++n){
            levels[n]=d::restored(order[n],defaults);
            gains[n]=static_cast<float>(levels[n])/100.f;
            for(std::size_t i=0;i<count_;++i)if(bindings_[i]==order[n]){
                if(!mixer_->nativeWetVoiceAdmitted(i)&&!mixer_->nativeRaceIntroClaimed(i))return false;
                if(mixer_->groupGains50IC_[i]!=gains[n])++changes;
            }
        }
        // All six native categories form one admission, not four partially
        // successful slider edits. Source/profile stores begin only afterward.
        if(!mixer_->canRecordControlChanges(changes))return false;
        profile.volume11020[0]=std::uint32_t(defaults.effects11020); //1865CC
        profile.volume11020[1]=std::uint32_t(defaults.music11024); //1865DC
        profile.volume11020[2]=std::uint32_t(defaults.character11028); //1865EC
        profile.volume11020[3]=std::uint32_t(defaults.announcer1102C); //1865F8
        settings_=defaults; // Same actual target later consumed by duck end.
        for(std::size_t n=0;n<order.size();++n){
            groups_.volume38[static_cast<std::size_t>(order[n])]=levels[n]; //235500 before transport.
            for(std::size_t i=0;i<count_;++i)if(bindings_[i]==order[n])
                if(!mixer_->setVoiceGroupGain50IC(i,gains[n]))std::terminate();
        }
        // Deliberately reapply even if profile values were already defaults:
        // current ducked groups may differ. Like the live volume setters,
        // retain duck flags/deltas rather than inventing an end/begin callback.
        // Auxiliary, sound mode, flags11050, HUD, pause and playback survive.
        return true;
    }
    struct ColdSound;
    struct ColdGroup;
    struct ColdTaIntroEmitter;
    bool beginTaIntro177750(std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate>,
        game::OriginalWreckComponent50LA&,std::shared_ptr<const assets::AudioBank50HC>,
        std::size_t firstVoice,std::size_t voiceCount)noexcept;
    bool finishTaIntro177750(game::OriginalWreckComponent50LA&)noexcept;
    //Native prewarming custody: backing identity is separate from permission
    //to dispatch/service the source-published intro. Never resets after failure.
    bool taIntroBackingCurrent()const noexcept;
    bool reserveNativeTaIntro177750(game::OriginalWreckComponent50LA&)noexcept;
    bool publishNativeTaIntro177750(game::OriginalWreckComponent50LA&)noexcept;
    bool stopTaIntroBacking2924A0()noexcept;
    bool taIntroNativeReserved_=false,taIntroNativePublished_=false,taIntroNativeWithdrawn_=false;
    game::OriginalWreckComponent50LA* taIntroExpectedController_=nullptr;
    // Camera Session is the sole caller. These are reached source construction
    // and never-started generic-child closure, not playback capabilities.
    enum class TaIntroConstructionResult : std::uint8_t {
        rejected, allocationPending, eventsComplete
    };
    TaIntroConstructionResult constructTaIntroEvents177750(
        std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate>)noexcept;
    bool attachTaIntro141960(game::OriginalWreckComponent50LA&)noexcept;
    bool retireNeverStartedTaIntro142590()noexcept;
    game::OriginalWreckComponent50LA* taIntroEntity80()noexcept {
        return coldTaIntroEmitter_?&coldTaIntroEmitter_->entity:nullptr;
    }
    // Private unactivated proposal leaves for the existing actual race-owner
    // friends only. Intentionally uncalled. Genuine canonical
    // Announcer120 membership; no voice/play/service or controller attachment.
    std::unique_ptr<NativeRaceIntroTransport50LA> taIntroTransport_;
    std::array<NativeRaceIntroTransport50LA::Playback,3> taIntroLatest_{};
    bool taIntroLive_=false;
    bool taIntroConstructionAdmission_=false;
    std::size_t taIntroFirstVoice_=0,taIntroVoiceCount_=0;
    bool taIntroExistingBindingsCurrent()const noexcept;
    bool constructColdTaIntroSounds232ED0(
        std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate>)noexcept;
    // Never-started stop/release, then235C40. SAME group remains alive for its
    // existing empty typed retirement. No source or native clock is involved.
    bool clearColdTaIntroSounds2353D0()noexcept;
    bool prepareColdTaIntroConstruction(
        const std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate>&,
        std::shared_ptr<const OriginalApplicationAural50KG>&)const noexcept;
    ColdSound* appendPreparedColdTaIntroSound232ED0(std::unique_ptr<ColdSound>)noexcept;
    enum class ColdTaEmitterAppendResult : std::uint8_t {
        ownerRejected,allocationPending,eventAppended,eventsComplete
    };
    bool constructDetachedColdTaIntroEmitter177750(
        std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate>)noexcept;
    bool coldTaIntroEmitterIdentity()const noexcept;
    ColdTaEmitterAppendResult appendNextColdTaIntroEvent292650()noexcept;
    // Explicit native failed-prefix abort, NOT a retail292220 operation.
    bool abortPendingColdTaIntroEventNative()noexcept;
    bool retireDetachedColdTaIntroEmitter142590()noexcept;

    // Source-only v5 continuation for the SAME real never-started Sounds.
    // Native refusal is distinct from the original setter's false/true result.
    // No result is consumed as a factory/playback/clock/route capability.
    enum class ColdTaSoundSetResult : std::uint8_t {
        ownerRejected,numberRejected,sourceFalse,sourceTrue
    };
    bool coldTaIntroSettersCurrent(unsigned eventId)const noexcept;
    static bool coldTaIntroVolumeWord233750(float,std::int32_t&)noexcept;
    ColdTaSoundSetResult setColdTaIntroBaseVolume233830(unsigned eventId,float)noexcept;
    ColdTaSoundSetResult setColdTaIntroEffectiveVolume233750(unsigned eventId,float)noexcept;
    ColdTaSoundSetResult setColdTaIntroBasePitch233A50(unsigned eventId,std::int32_t)noexcept;
    ColdTaSoundSetResult setColdTaIntroEffectivePitch2339A0(unsigned eventId,std::int32_t)noexcept;

    // Concrete2356A0 for this owner's actual counted group. Only the real
    // race/world owner friends can call it; no synthetic component is accepted.
    bool canPauseColdGroup2356A0(game::RaceAudioGroup50IC,bool)const noexcept;
    bool pauseColdGroup2356A0(game::RaceAudioGroup50IC,bool)noexcept;
    bool canPauseGroupSuffix2356EC(game::RaceAudioGroup50IC group,bool requested)const noexcept {
        if(!current()||!groups_.initialized||!represented(group))return false;
        if(group==game::RaceAudioGroup50IC::Announcer120&&taIntroLive_&&
           (!taIntroTransport_||!taIntroTransport_->canSetPaused(requested)))return false;
        const auto state=groups_.state44[static_cast<std::size_t>(group)];
        if((state!=0)==requested)return true;
        std::uint64_t changes=0;
        for(std::size_t i=0;i<count_;++i)if(bindings_[i]==group){
            if(!mixer_->hasVoiceGroupGain50IC(i)||mixer_->groupPaused50IC_[i]!=(state!=0))return false;
            ++changes;
        }
        return mixer_->canRecordControlChanges(changes);
    }
    bool pauseGroupSuffix2356EC(game::RaceAudioGroup50IC group,bool requested)noexcept {
        if(!canPauseGroupSuffix2356EC(group,requested))return false;
        auto& state=groups_.state44[static_cast<std::size_t>(group)];
        if((state!=0)==requested)return true; //No lower command on Boolean equality.
        state=requested?1u:0u; //2356EC precedes26D380 on the actual allocated group.
        for(std::size_t i=0;i<count_;++i)if(bindings_[i]==group)
            if(!mixer_->setVoiceGroupPaused50IC(i,requested))std::terminate();
        if(group==game::RaceAudioGroup50IC::Announcer120&&taIntroLive_&&
           !taIntroTransport_->setPaused(requested))std::terminate();
        return true;
    }
    bool canReleaseEmptyCold142590(game::RaceAudioGroup50IC)const noexcept;
    struct ColdGroup;
    // Typed source Sound projection, NOT Entity or native voice. Unknown guest
    // words are absent rather than fabricated zero bytes. The concrete TA live
    // path publishes revision-safe transport serials, never guest channel IDs.
    struct ColdSound {
        std::uint64_t reference00=0;
        std::uint32_t flags10=0,word14=0,word18=0;
        float duration20=0.f;
        std::uint32_t elapsedBits24=0;
        std::int32_t sample28=-1,channel2C=-1;
        const void* manager30=nullptr; // Actual retained Aural interfaceA0.
        ColdGroup* group34=nullptr;
        std::int32_t volume38=0,pan3C=0,pitch40=0;
        std::int32_t volume44=0,pan48=0,pitch4C=0,basePitch50=0;
        std::uint32_t factor54Bits=0,factor58Bits=0,factor5CBits=0;
        std::array<std::uint32_t,4> vector60Bits{},vector90Bits{};
        std::uint32_t word70=0,word74=0,word78=0,word7C=0,word80=0;
        std::int32_t resourceA0=-1;
        std::array<char,260> filenameA4{};
        bool stream1A8=false,stereo1AC=false,loop1B0=false;
        std::uint32_t word1B4=0;
        std::int32_t directoryOrdinal1B8=-1; // TXT ordinal, never a channel.
        const assets::OriginalStreamMetadataRow50KZ* metadata=nullptr;
    };
    bool coldTaIntroGroupIdentity()const noexcept;
    bool coldTaIntroSoundIdentity(std::size_t)const noexcept;
    struct ColdGroup {
        game::OriginalWreckComponent50LA entity;
        std::unique_ptr<ColdSound*[]> logicalMembers4C;
        std::uint32_t count48=0,growth50=0,capacity54=0;
        std::int32_t slot40=2;
        bool constructed=false,loaded=false,enabled30=false,allocated58=false;
        // Native lifetime only; Sound reference00 remains1. Actual canonical
        // slot occupancy already bars Aural102CD0/interface retirement.
        std::array<std::unique_ptr<ColdSound>,3> taIntroSounds;
        std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate> taIntroAuthoring;
        std::shared_ptr<const OriginalApplicationAural50KG> taIntroManager;
        bool taIntroStarted=false,taIntroConstructed=false; // Historical begun vs all-authored-events-complete.
    };
    // Native holding storage, NOT source Sound-group ownership. The counted
    // Entity starts detached, then attaches to the actual controller generic
    // child chain; it is independently retired before group-owned Sounds.
    struct ColdTaIntroEmitter {
        struct Event {std::uint32_t id00=0;std::optional<ColdSound*> sound04;};
        game::OriginalWreckComponent50LA entity;
        ColdGroup* group30=nullptr;
        std::array<std::uint32_t,4> previous40{};
        std::uint32_t serial50=0;char name54=0;
        std::uint32_t flags158=0;
        // Represents exact160..1CF words. Unwritten caller descriptor words
        // become unknown at configuration, rather than fabricated zero values.
        std::array<std::optional<std::uint32_t>,28> configuration160;
        std::uint32_t distance1D0Bits=0;std::uint8_t flags1D4Low5=0;
        const game::OriginalWreckComponent50LA* manager1D8=nullptr;
        std::uint32_t trigger1DCBits=0,chance1E0Bits=0;
        std::uint16_t type1E4=0;
        std::uint32_t word1E8=0,word1EC=0;ColdSound* sound1F0=nullptr;
        std::uint32_t count200=0;
        std::unique_ptr<Event*[]> members204;
        std::uint32_t growth208=0,capacity20C=0;
        bool allocated210=false;
        std::array<std::unique_ptr<Event>,3> ownedEvents;
        std::unique_ptr<Event> pendingEvent; // Native retained292650 in-flight allocation.
        bool configured=false,nativeAppendAborted=false;
        game::OriginalWreckComponent50LA* controllerParent=nullptr;
        bool everAttached=false; // Native custody history; not a source flag.
    };
    // Source2C1000 owner only. This is NOT a Character Sound member or a
    // second native speech scheduler. Numeric source fields borrow the exact
    // Context storage used by existing speech consumers.
    struct ColdNativePlayerSpeech {
        game::OriginalWreckComponent50LA entity;
        std::uint32_t dispatch08=0;
        ColdGroup* group30=nullptr;
        std::array<std::uint32_t,4> previous40{};
        std::uint32_t serial50=0;char name54=0;
        std::uint32_t flags158=0;
        std::array<std::uint32_t,28> configuration160{};
        std::uint32_t distance1D0Bits=0;std::uint8_t flags1D4Low5=0;
        const game::OriginalWreckComponent50LA* manager1D8=nullptr;
        std::uint32_t trigger1DCBits=0,chance1E0Bits=0;
        std::uint16_t type1E4=0;
        std::uint32_t word1E8=0,word1EC=0;ColdSound* sound1F0=nullptr;
        std::array<const void*,3> jukebox200{};
        game::OriginalWreckComponent50LA* racer20C=nullptr;
        // Constructor does NOT initialize210. Nullopt is unknown, not null.
        std::optional<const void*> timing210;
        game::PodSpeechCounts50HG* counts214=nullptr;
        game::PodSpeechState50HG* state21D=nullptr;
        // Native custody is deliberately separate from source racer20C.
        const void* nativeIssuer=nullptr;
        const OriginalApplicationAural50KG* nativeAural=nullptr;
        const game::OriginalWreckComponent50LA* nativeRace=nullptr;
        game::OriginalWreckComponent50LA* nativeExpectedRacer=nullptr;
        std::uint64_t nativeExpectedRacerKey=0; //Session identity, not the Entity address.
    };
    bool nativePlayerSpeechContextCurrent(
        const std::shared_ptr<game::OriginalApplicationEntities50LA>&,std::uint64_t,
        const game::OriginalWreckComponent50LA&,const game::OriginalWreckComponent50LA&,
        const game::OriginalWreckComponent50LA&)const noexcept;
    NativePlayerSpeechConstructorResult50LA constructNativePlayerSpeech2C1000(
        const void*,const std::shared_ptr<game::OriginalApplicationEntities50LA>&,std::uint64_t,
        const game::OriginalWreckComponent50LA&,const game::OriginalWreckComponent50LA&,
        game::OriginalWreckComponent50LA&,game::PodSpeechCounts50HG&,game::PodSpeechState50HG&)noexcept;
    bool nativePlayerSpeechBackingCurrent()const noexcept;
    bool canRetireNativePlayerSpeech2C10B0(const void*,
        const game::OriginalWreckComponent50LA&,const game::OriginalWreckComponent50LA*)const noexcept;
    bool retireNativePlayerSpeech2C10B0(const void*,
        const game::OriginalWreckComponent50LA&,const game::OriginalWreckComponent50LA*)noexcept;
    std::unique_ptr<ColdNativePlayerSpeech> coldNativePlayerSpeech_;
    bool nativePlayerSpeechAttempted_=false; // Never cleared by retirement.
    std::unique_ptr<ColdTaIntroEmitter> coldTaIntroEmitter_;
    std::array<ColdGroup,game::raceAudioGroupCount50IC> coldGroups_;
    std::shared_ptr<game::OriginalApplicationEntities50LA> coldApplication_;
    AudioMixer50HD* coldMixer_=nullptr;
    std::uint64_t coldEpoch_=0,coldSession_=0;
    std::thread::id coldThread_{};
    std::size_t coldNext_=0;
    bool coldStarted_=false,coldRetiring_=false,nativeBindingsRetired_=false;
    static float normalized(const game::RaceAudioGroups50IC& groups,
                            game::RaceAudioGroup50IC group)noexcept {
        return static_cast<float>(groups.volume38[static_cast<std::size_t>(group)])/100.f;
    }
    // Scalar helpers above prepare the exact callback writes on copies. A
    // native rejection must leave ALL live groups, duck words and gains intact;
    // there is no source receipt for a partly applied native transaction.
    bool synchronizeDucking(const game::AnnouncerDuckingReceipt50IC& receipt,
        const game::AnnouncerDuckingState50IC& nextDuck,bool beginning)noexcept {
        namespace d=game::announcer_callbacks50IC_detail;
        if(!current()||std::fegetround()!=FE_TONEAREST||!d::valid(groups_)||
           !receipt.complete||receipt.count>duck_.flags.size())return false;
        // Compare actual bound voices with CURRENT source group words, not
        // settings_ or an assumed duck fraction. A legitimate live Pause edit
        // can replace a group level while its original duck flag stays set.
        for(std::size_t i=0;i<count_;++i){
            if(!represented(bindings_[i])){
                if(bindings_[i]!=game::RaceAudioGroup50IC::Unrepresented)return false;
                continue;
            }
            if(!mixer_->hasVoiceGroupGain50IC(i)||
               mixer_->groupGains50IC_[i]!=normalized(groups_,bindings_[i])||
               mixer_->groupPaused50IC_[i]!=(groups_.state44[static_cast<std::size_t>(bindings_[i])]!=0))return false;
        }
        std::array<float,5> gains{};
        std::array<std::size_t,5> flagIndex{};
        std::size_t n=0;std::uint64_t changes=0;
        // Only the actual callback's active subsequence is admitted. This also
        // proves each group is written once, so revision costs cannot double
        // count an intermediate gain or accidentally touch Announcer/Auxiliary.
        for(std::size_t k=0;k<duck_.flags.size();++k){
            if(beginning?duck_.flags[k]!=0:duck_.flags[k]==0)continue;
            if(n>=receipt.count)return false;
            const auto& write=receipt.writes[n];
            if(write.group!=d::groups[k]||!d::volume(write.volume)||
               write.callSite!=(beginning?d::beginSites[k]:d::endSites[k])||
               nextDuck.flags[k]!=(beginning?1u:0u)||
               nextDuck.deltas[k]!=(beginning?
                   groups_.volume38[static_cast<std::size_t>(write.group)]-write.volume:
                   duck_.deltas[k]))return false;
            flagIndex[n]=k;gains[n]=static_cast<float>(write.volume)/100.f;
            for(std::size_t i=0;i<count_;++i)if(bindings_[i]==write.group){
                // Match the setter's real lease predicate, including the
                // existing typed race-intro exception. Never steal wet voices.
                if(!mixer_->nativeWetVoiceAdmitted(i)&&!mixer_->nativeRaceIntroClaimed(i))return false;
                if(mixer_->groupGains50IC_[i]!=gains[n])++changes;
            }
            ++n;
        }
        // This checks all admitted setters before ANY source or mixer store.
        // An already exhausted identity rejects even a zero-write callback;
        // an unexhausted terminal revision still permits genuine no-op gains.
        if(n!=receipt.count||!mixer_->canRecordControlChanges(changes))return false;
        for(n=0;n<receipt.count;++n){
            const auto& write=receipt.writes[n];const auto k=flagIndex[n];
            groups_.volume38[static_cast<std::size_t>(write.group)]=write.volume; //235500 first.
            // Preserve source callback group order and the existing native
            // ascending voice order. All setter failure paths and revision
            // costs are preflighted on this serial owner, with no callbacks.
            for(std::size_t i=0;i<count_;++i)if(bindings_[i]==write.group)
                if(!mixer_->setVoiceGroupGain50IC(i,gains[n]))std::terminate();
            //1A3E20/24 and counterparts follow the begin setter's return.
            //1A3F64 and counterparts clear only the flag after end's setter.
            if(beginning)duck_.deltas[k]=nextDuck.deltas[k];
            duck_.flags[k]=nextDuck.flags[k];
        }
        // No blanket copy after transport: retain exact reached store order,
        // untouched deltas, state44, ordinary voice controls and playback.
        return true;
    }
    AudioMixer50HD* mixer_=nullptr;
    std::uint64_t epoch_=0;
    std::size_t count_=0;
    game::RaceAudioSettings50IC settings_{};
    game::RaceAudioGroups50IC groups_{};
    game::AnnouncerDuckingState50IC duck_{};
    std::array<game::RaceAudioGroup50IC,audioMaximumVoices50HD> bindings_{};
};
} // namespace rr::platform

#include "race_audio_groups_cold50la.inl"

#include "race_audio_groups_ta_intro_cold_candidate.inl"

#include "race_audio_groups_ta_intro_setters_candidate.inl"

#include "race_audio_groups_ta_intro_emitter_candidate.inl"

#include "race_audio_groups_ta_intro_controller_candidate.inl"

#include "race_audio_groups_ta_intro_live_candidate.inl"

#include "race_audio_groups_native_player_speech50la.inl"
