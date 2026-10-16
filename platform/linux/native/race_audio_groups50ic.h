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

namespace live_audio50HF { class Owner; }
namespace rr::game { class OriginalRaceConstructor50LA; }
namespace rr::platform {
class RaceAudioGroups50IC final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::live_audio50HF::Owner;
    friend class rr::game::OriginalRaceConstructor50LA;
public:
    RaceAudioGroups50IC() noexcept { bindings_.fill(game::RaceAudioGroup50IC::Unrepresented); }
    ~RaceAudioGroups50IC()noexcept { for(const auto& p:coldGroups_)if(p.constructed)std::terminate(); }
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
               !coldApplication_||!p.constructed||!p.loaded||p.count48||
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
    // Concrete2356A0 for this owner's actual counted group. Only the real
    // race/world owner friends can call it; no synthetic component is accepted.
    bool pauseColdGroup2356A0(game::RaceAudioGroup50IC,bool)noexcept;
    bool canPauseGroupSuffix2356EC(game::RaceAudioGroup50IC group,bool requested)const noexcept {
        if(!current()||!groups_.initialized||!represented(group))return false;
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
        return true;
    }
    bool canReleaseEmptyCold142590(game::RaceAudioGroup50IC)const noexcept;
    struct ColdGroup {
        game::OriginalWreckComponent50LA entity;
        std::unique_ptr<game::OriginalWreckComponent50LA*[]> logicalMembers4C;
        std::uint32_t count48=0,growth50=0,capacity54=0;
        std::int32_t slot40=2;
        bool constructed=false,loaded=false,enabled30=false,allocated58=false;
    };
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
