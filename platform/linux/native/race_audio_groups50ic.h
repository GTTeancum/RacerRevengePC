#pragma once
// Main-thread bridge from the recovered original group owner to the native
// mixer. Host voice-slot assignments are supplied explicitly by each provider.
#include "audio_mixer50hd.h"
#include "../../../src/game/announcer_callbacks50ic.h"
#include <array>
#include <memory>
#include <thread>
#include "../../../src/game/original_application_entities50la.h"
#include <exception>
#include <optional>
#include "original_application_aural50kg.h"
#include "../../../src/assets/original_ta_intro_authoring_candidate.h"
#include "native_race_intro_transport50la.h"

namespace live_audio50HF { class Owner; }
namespace rr::game { class OriginalRaceConstructor50LA; class OriginalGcIntroExitOwner; }
namespace rr::platform {
class RaceAudioGroups50IC final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::live_audio50HF::Owner;
    friend class rr::game::OriginalRaceConstructor50LA;
    friend class rr::game::OriginalGcIntroExitOwner;
    friend class ::original_camera_session50KX::Session;
public:
    RaceAudioGroups50IC() noexcept { bindings_.fill(game::RaceAudioGroup50IC::Unrepresented); }
    ~RaceAudioGroups50IC()noexcept { if(coldTaIntroEmitter_)std::terminate();for(const auto& p:coldGroups_)if(p.constructed)std::terminate(); }
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
        if(coldTaIntroEmitter_&&!taIntroReady())return false;
        for(std::size_t n=0;n<coldGroups_.size();++n){const auto& p=coldGroups_[n];
            if((p.count48||p.taIntroAuthoring||p.taIntroManager)&&
               (n!=static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)||!taIntroReady()))return false;
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
               !coldApplication_||!p.constructed||!p.loaded||(p.count48&&(!taIntroReady()||group!=game::RaceAudioGroup50IC::Announcer120))||
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
        if(!receipt.complete||!synchronize(groups,receipt))return {};
        groups_=groups;duck_=duck;return receipt;
    }
    game::AnnouncerDuckingReceipt50IC end()noexcept {
        if(!current())return {};
        auto groups=groups_;auto duck=duck_;
        auto receipt=game::endAnnouncerDucking50IC(settings_,groups,duck);
        if(!receipt.complete||!synchronize(groups,receipt))return {};
        groups_=groups;duck_=duck;return receipt;
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
    struct ColdSound;
    struct ColdGroup;
    struct ColdTaIntroEmitter;
    bool beginTaIntro177750(std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate>,
        game::OriginalWreckComponent50LA&,std::shared_ptr<const assets::AudioBank50HC>,
        std::size_t firstVoice,std::size_t voiceCount)noexcept;
    bool finishTaIntro177750(game::OriginalWreckComponent50LA&)noexcept;
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
        bool taIntroStarted=false,taIntroConstructed=false; // Historical begun vs all-three-complete.
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
    bool synchronize(const game::RaceAudioGroups50IC& groups,
                     const game::AnnouncerDuckingReceipt50IC& receipt)noexcept {
        if(!current()||!game::announcer_callbacks50IC_detail::valid(groups)||
           !receipt.complete||receipt.count>receipt.writes.size())return false;
        for(std::size_t i=0;i<count_;++i)
            if(represented(bindings_[i])&&!mixer_->hasVoiceGroupGain50IC(i))return false;
        for(std::size_t n=0;n<receipt.count;++n)
            if(!represented(receipt.writes[n].group))return false;
        // Preserve original callback group-write order. The Dax group has no
        // begin/end writes and is never touched by this callback propagation.
        for(std::size_t n=0;n<receipt.count;++n)
            for(std::size_t i=0;i<count_;++i)
                if(bindings_[i]==receipt.writes[n].group&&
                   !mixer_->setVoiceGroupGain50IC(i,normalized(groups,bindings_[i])))return false;
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
