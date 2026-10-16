#pragma once
#include "original_ui_main_entry50kg.h"
#include "original_course_race_targets50kx.h"
#include "../../src/game/original_race_hud_blink50la.h"
#include <limits>
#include "../../platform/linux/native/race_audio_groups50ic.h"

namespace rr::game {
// Actual counted1A65C0 prefix, admitted only by consumed Course ownership.
// No Application28C publication or complete race-load callback is available.
class OriginalRaceConstructor50LA final {
    friend struct ::original_main_entry50KG::Application;
    std::shared_ptr<OriginalApplicationEntities50LA> application_;
    std::shared_ptr<platform::OriginalUiRiderProfileOwner50KG> profile_;
    OriginalWreckComponent50LA entity_;
    original_course_targets50KX::Selection launchSelection_{}; //Actual consumed provenance, not a guest field.
    OriginalRaceHudBlink50LA hudBlink_;
    // Native typed numeric lanes actually written by this constructor. Child
    // pointer slots acquire only explicitly reached native children below.
    // B8/BC/C0 are not represented as null because1A65C0 never writes them.
    float field94_=4.f;
    std::uint32_t field30_=0,field44_=0,field58_=0,field6C_=0,field70_=0;
    std::uint32_t field8C_=0,field90_=0,field98_=0,field9C_=0,fieldA0_=0,fieldA4_=0,fieldA8_=0;
    std::array<std::uint32_t,4> fields34_{},fields48_{};
    std::uint32_t field128_=3;
    std::array<std::array<std::uint32_t,7>,2> fields12C_{};
    std::array<std::uint32_t,2> fields164_{},fields17C_{},fields184_{},fields18C_{},fields194_{};
    std::array<std::array<std::uint32_t,2>,2> fields16C_{};
    std::int32_t field1A4_=-1;
    std::uint32_t field1A8_=0,field1AC_=0;
    // Constructor writes bits2..7 of1B0 and bits0..1 of1B1 only.
    // Unknown lanes are deliberately inaccessible, not zero-filled flags.
    const std::uint8_t flags1B0KnownMask_=0xfcu,flags1B0KnownValue_=0x08u;
    const std::uint8_t flags1B1KnownMask_=0x03u,flags1B1KnownValue_=0;
    bool counted_=false,constructorSuffix_=false,rolledBack_=false;
    platform::RaceAudioGroups50IC* raceAudio_=nullptr; //Actual caller retains through rollback.
    std::array<OriginalWreckComponent50LA*,6> raceGroups_{};
    std::size_t raceGroupCount_=0,raceGroupsAttached_=0;
    bool raceGroupsAttempted_=false,raceMaskRecomputed_=false;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> resourceDomain_;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> resourceContext_;
    bool resourcesAttempted_=false,resourcesPrepared_=false;
    std::shared_ptr<original_main_entry50KG::RaceDisplayLifetime50LA> loadDisplay_;
    std::shared_ptr<original_main_entry50KG::NativeCollectorFifo50LA> loadFifo_;
    bool loadPrefixAttempted_=false,loadPrefixReached_=false;
    std::shared_ptr<ra_cold_aggregate50LA::Owner> world74_;
    bool worldAttempted_=false;
    bool constructWorld1A6BC0(const original_main_entry50KG::Application&,
        std::shared_ptr<ra_cold_aggregate50LA::Owner>,std::uint32_t&)noexcept;
    bool populateResources1AA5A0(std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context,
        assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        d={};
        if(!detachedPrefix()||!constructorSuffix_||!loadPrefixReached_||resourcesAttempted_||raceGroupsAttempted_||
           !domain||!context){d.status=assets::OriginalStandardMaterialStatus50LA::phase;
            d.sourcePC=0x1aa5a0;return false;}
        resourcesAttempted_=true;resourceDomain_=std::move(domain);resourceContext_=std::move(context);
        // Exact1AA5AC/1AA5B8 pair. No14D440 collector rebuild belongs here.
        // The existing ScreenTexture leaf implements this identical ordered
        // whole-table pair over the SAME382B38/382B70/383400/resolver owners.
        if(!resourceDomain_->populateTimeStatsResources2DF144(resourceContext_,d))return false;
        resourcesPrepared_=true;d.sourcePC=0x1aa5c8;return true;
    }
    inline static constexpr RaceAudioGroup50IC groupOrder_[]={RaceAudioGroup50IC::Effects110,
        RaceAudioGroup50IC::Engines114,RaceAudioGroup50IC::Music118,RaceAudioGroup50IC::Character11C,
        RaceAudioGroup50IC::Announcer120,RaceAudioGroup50IC::Auxiliary124};
    OriginalRaceConstructor50LA(std::shared_ptr<OriginalApplicationEntities50LA> app,
        std::shared_ptr<platform::OriginalUiRiderProfileOwner50KG> profile)noexcept
        :application_(std::move(app)),profile_(std::move(profile)){}
    bool initializeCount(std::uint64_t session)noexcept {
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&entity_));
        if(application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max()||
           !application_->entityAuthority_.canEnroll(entity_,key))return false;
        const auto result=initializeOriginalWreckComponent50LA(entity_,
            OriginalWreckComponentKind50LA::entity141300,session,key,true,
            application_->objectCount423F60());
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        counted_=true;
        if(!application_->entityAuthority_.enroll(entity_))std::terminate();
        return true;
    }
    bool detachedPrefix()const noexcept {
        return counted_&&!rolledBack_&&application_&&entity_.reference00==1&&
            entity_.kind==OriginalWreckComponentKind50LA::entity141300&&
            entity_.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&entity_))&&
            entity_.sessionGeneration&&!entity_.previous10&&!entity_.next14&&!entity_.parent1C&&
            !entity_.firstChild18&&!entity_.mask20&&
            entity_.descendants24==((constructorSuffix_&&!raceMaskRecomputed_)?0xffffffffu:0u)&&
            entity_.category28==0xffu&&(entity_.flags29&15u)==3u;
    }
public:
    OriginalRaceConstructor50LA(const OriginalRaceConstructor50LA&)=delete;
    OriginalRaceConstructor50LA& operator=(const OriginalRaceConstructor50LA&)=delete;
    ~OriginalRaceConstructor50LA()noexcept {if(counted_&&!rolledBack_)std::terminate();}
    // Actual six2350D0/load/141960 calls. Earlier1A6AA0 load-prefix effects
    // and later world/racer loading are not certified by this bounded leaf.
    bool constructRaceGroups1A6B5C(platform::RaceAudioGroups50IC& audio,
        platform::AudioMixer50HD& mixer,std::uint32_t& sourcePC)noexcept {
        sourcePC=0x1a6b5c;
        if(!detachedPrefix()||!constructorSuffix_||!loadPrefixReached_||!resourcesPrepared_||raceGroupsAttempted_||
           audio.coldStarted_||audio.mixer_||audio.groups_.initialized)return false;
        raceGroupsAttempted_=true;raceAudio_=&audio;
        for(std::size_t i=0;i<6;++i){
            sourcePC=0x1a6b80;
            if(!audio.constructNextCold2350D0(mixer,application_,entity_.sessionGeneration,groupOrder_[i]))return false;
            auto& child=audio.coldGroups_[static_cast<std::size_t>(groupOrder_[i])].entity;
            // The existing helper fuses constructor+load. Its load cannot
            // observe the race slot; retain that actual pointer before attach.
            raceGroups_[raceGroupCount_++]=&child; //Original slot store1A6B88.
            sourcePC=0x1a6ba8;
            const auto result=application_->entityAuthority_.attachChildren(entity_,child,0xffffffffu,
                application_->attachmentLock423F58());
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
            ++raceGroupsAttached_;
        }
        sourcePC=0x1a6bc0;return true;
    }
    bool rollbackNativeRaceWorldFailure(std::uint32_t& sourcePC)noexcept;
    // Reverse native construction-failure cleanup. If world construction has
    // acquired its seventh group, its own explicit rollback must run first.
    bool rollbackNativeRaceGroupsFailure(std::uint32_t& sourcePC)noexcept {
        sourcePC=0x1a6b5c;
        if(world74_)return false;
        if(!raceAudio_)return raceGroupCount_==0;
        auto& audio=*raceAudio_;
        if(!counted_||rolledBack_||application_->attachmentLock423F58()||
           (raceGroupCount_&&(audio.coldApplication_!=application_||
             audio.coldThread_!=std::this_thread::get_id()||!audio.coldMixer_||
             audio.coldEpoch_!=audio.coldMixer_->groupEpoch50IC()))||
           audio.coldGroups_[static_cast<std::size_t>(RaceAudioGroup50IC::World4D8)].constructed||
           entity_.firstChild18!=(raceGroupsAttached_?raceGroups_[0]:nullptr))return false;
        // Validate the exact admitted child chain, including a possible final
        // constructed-but-not-attached group, before the first detach.
        for(std::size_t i=0;i<raceGroupCount_;++i){
            const auto* child=raceGroups_[i];
            if(!child||child!=&audio.coldGroups_[static_cast<std::size_t>(groupOrder_[i])].entity||
               child->reference00!=1||child->firstChild18)return false;
            if(i<raceGroupsAttached_){
                if(child->parent1C!=&entity_||child->previous10!=(i?raceGroups_[i-1]:nullptr)||
                   child->next14!=(i+1<raceGroupsAttached_?raceGroups_[i+1]:nullptr))return false;
            }else if(child->parent1C||child->previous10||child->next14)return false;
        }
        while(raceGroupCount_){
            const auto i=raceGroupCount_-1;auto& child=*raceGroups_[i];
            if(i<raceGroupsAttached_){
                sourcePC=0x141a70;
                if(application_->entityAuthority_.detach(child,application_->attachmentLock423F58()).status!=
                    OriginalWreckComponentStatus50LA::complete)return false;
                --raceGroupsAttached_;raceMaskRecomputed_=true;
            }
            sourcePC=0x235150;
            if(!audio.releaseEmptyCold142590(groupOrder_[i]))return false;
            raceGroups_[i]=nullptr;--raceGroupCount_;
        }
        raceAudio_=nullptr;return true;
    }
    // Explicit native allocation-failure/cancel policy, NOT1A6840/142590.
    // Group rollback must finish first; only this constructor's detached count
    // and retained native backing retire here. Profile side effects stay reached.
    bool rollbackNativeConstructionFailure(std::uint32_t& sourcePC)noexcept {
        sourcePC=0x1a65cc;
        if(rolledBack_)return true;
        if(!counted_){rolledBack_=true;return true;}
        if(world74_||loadDisplay_||loadFifo_||raceAudio_||raceGroupCount_||!detachedPrefix()||application_->attachmentLock423F58()||
           !application_->objectCount423F60()||!application_->entityAuthority_.retire(entity_))return false;
        --entity_.reference00;--application_->objectCount423F60();
        rolledBack_=true;profile_.reset();resourceContext_.reset();resourceDomain_.reset();return true;
    }
};
}
namespace original_main_entry50KG {
inline bool Application::populateConsumedRaceResources1AA5A0(rr::game::OriginalRaceConstructor50LA& owner,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    d={};
    if(raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile||!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
       !materialContextOwner_||!materialContextOwner_->live()||
       materialContextOwner_->videoIdentity()!=videoOwner_.get()||
       materialContextOwner_->materialApplication()!=materialApplication||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()){
        d.status=rr::assets::OriginalStandardMaterialStatus50LA::video;d.sourcePC=0x1aa5a0;return false;
    }
    return owner.populateResources1AA5A0(standardMaterialDomainOwner_,materialContextOwner_,d);
}
inline std::shared_ptr<rr::game::OriginalRaceConstructor50LA> Application::constructConsumedRaceCold1A65C0(
    original_course_targets50KX::ConsumedLaunch& launch,std::uint64_t session,std::uint32_t& sourcePC)const noexcept {
    using T=rr::game::OriginalRaceConstructor50LA;
    sourcePC=0x1a65c0;
    if(!enabled||!profile||!riderProfile||!entitiesOwner_||!entitiesOwner_->application10()||
       entitiesOwner_->attachmentLock423F58()||!raceConstructorColdOwner50LA_.expired()||
       launch.raceConstructorClaimed_||!launch.target_||
       !launch.matches(*riderProfile,session,launch.target_->track,launch.selection_.rider))return {};
    std::shared_ptr<T> owner;
    try {owner=std::shared_ptr<T>(new T(entitiesOwner_,riderProfile));}catch(...){return {};}
    owner->launchSelection_=launch.selection_;
    if(!owner->initializeCount(session))return {};
    launch.raceConstructorClaimed_=true;
    sourcePC=0x1a66cc;
    if(!riderProfile->beginPodRace1A66CC(*profile)){
        std::uint32_t rollbackPC=0;
        if(!owner->rollbackNativeConstructionFailure(rollbackPC))std::terminate();
        return {};
    }
    owner->entity_.descendants24=0xffffffffu; //1A6808, not generic detached0.
    owner->constructorSuffix_=true;
    raceConstructorColdOwner50LA_=owner;sourcePC=0x1a6830;return owner;
}
}
