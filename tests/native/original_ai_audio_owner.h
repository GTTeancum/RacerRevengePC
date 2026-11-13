#pragma once
#include "native_results_world_stop26d070.h"
#include "../../src/game/shared_random_owner50id.h"
#include "../../src/game/lap_host_clock50id.h"
#include <bit>
#include <cstring>
#include "opponent_engine_audio50hx_fixture.h"
#include "original_ai_material_resources.h"
#include "../../src/game/impact_effects50b.h"
#include "selected_player_audio50ij_fixture.h"
#include "../../src/game/original_ai_aural.h"
#include "../../src/game/original_pod_collision_response_owner.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include <array>
#include <cstdio>
#include <memory>
#include <iostream>
#include "original_material_input_diagnostics50ln.h"

namespace interactive_continuation50DU { struct Context; }
namespace original_race_hud_scope50LA { class Registration; class Scope; }
namespace original_main_entry50KG { struct Application; }
namespace native_racer50HN { class Bank; }

namespace native_original_ai_audio {
// Actual native-owned D40/D48/sound/resource instances for each admitted AI
// bank row:0..7 for ALL8,1..7 beside the existing human owner for Course.
// Course row0 D44 borrows that SAME human D40 and its canonical22C flags.
// No mutable physics, thermal, listener or RNG owner is copied here.
// Native phase policy: D48/D40 after the represented body children; Aural at
// the accepted world-audio boundary. Full retail child chronology is unclaimed.
class Owner final {
    friend class rr::platform::OriginalApplicationAural50KG;
    friend struct ::interactive_continuation50DU::Context;
    friend class ::original_race_hud_scope50LA::Registration;
    friend class ::original_race_hud_scope50LA::Scope;
    // Native producer receipt only: Context captures immediately after the
    // actual successful initializeSpeech call. It consumes no additional draw
    // and claims neither retail allocation chronology nor numeric validation.
    bool captureNativeSpeechReceipt(const original_main_entry50KG::Application& app,
        const native_racer50HN::Bank& bank,std::uint64_t generation,
        std::uint32_t& sharedRaw,std::size_t activeRacerCount)noexcept {
        if(nativeSpeechReceiptCaptured_||!prepared_||!speechPrepared_||
           !speechInitialized_||!generation||activeRacerCount!=racerCount_)return false;
        nativeSpeechReceiptCaptured_=true;
        nativeSpeechApplication_=&app;nativeSpeechBank_=&bank;
        nativeSpeechGeneration_=generation;nativeSpeechRaw_=&sharedRaw;
        nativeSpeechRacerCount_=activeRacerCount;return true;
    }
    bool nativeSpeechReceiptCurrent(const original_main_entry50KG::Application& app,
        const native_racer50HN::Bank& bank,std::uint64_t generation,
        const std::uint32_t& sharedRaw,std::size_t activeRacerCount)const noexcept {
        return nativeSpeechReceiptCaptured_&&prepared_&&speechPrepared_&&speechInitialized_&&
            nativeSpeechApplication_==&app&&nativeSpeechBank_==&bank&&
            nativeSpeechGeneration_==generation&&generation!=0&&
            nativeSpeechRaw_==&sharedRaw&&nativeSpeechRacerCount_==activeRacerCount&&
            activeRacerCount==racerCount_;
    }
    bool nativeSpeechOwner(const original_main_entry50KG::Application& app,
        const native_racer50HN::Bank& bank,std::uint64_t generation,
        const std::uint32_t& sharedRaw,std::size_t activeRacerCount,
        std::size_t slot,std::uint32_t actualRacer60,
        const rr::game::PodSpeechCounts50HG*& counts,
        rr::game::PodSpeechState50HG*& state)noexcept {
        counts=nullptr;state=nullptr;
        return nativeSpeechReceiptCurrent(app,bank,generation,sharedRaw,activeRacerCount)&&
            speechOwner(slot,actualRacer60,counts,state);
    }
    bool applyNativeSpeech(const original_main_entry50KG::Application& app,
        const native_racer50HN::Bank& bank,std::uint64_t generation,
        const std::uint32_t& sharedRaw,std::size_t activeRacerCount,
        std::size_t slot,std::uint32_t actualRacer60,
        const rr::game::AudioCommand50HE& command)noexcept {
        return nativeSpeechReceiptCurrent(app,bank,generation,sharedRaw,activeRacerCount)&&
            applySpeech(slot,actualRacer60,command);
    }
    void revokeNativeSpeechReceipt()noexcept {
        nativeSpeechApplication_=nullptr;nativeSpeechBank_=nullptr;
        nativeSpeechGeneration_=0;nativeSpeechRaw_=nullptr;nativeSpeechRacerCount_=0;
        // Never clear the one-shot captured bit or any source speech numbers.
    }
    const original_main_entry50KG::Application* nativeSpeechApplication_=nullptr;
    const native_racer50HN::Bank* nativeSpeechBank_=nullptr;
    const std::uint32_t* nativeSpeechRaw_=nullptr;
    std::uint64_t nativeSpeechGeneration_=0;
    std::size_t nativeSpeechRacerCount_=0;
    bool nativeSpeechReceiptCaptured_=false;
    // Additional SAME-owner original PCM: D40 wreck sounds, all roster D44
    // arrays (including actual human row0), and each AI D48 speech provider.
    // Aural owns aggregate disjoint claims/capacity before these private stops.
    bool canStopResultsAuxiliary26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&,std::uint64_t)const noexcept;
    std::size_t resultsAuxiliaryStopCount26D070()const noexcept;
    std::size_t resultsAuxiliaryStopVoice26D070(std::size_t)const noexcept;
    void stopResultsAuxiliaryPreflighted26D070()noexcept;
    // Course world-sound transports: the fixed-position World4D8 owners of
    // GB/RC/TB/MC. Original 26D070 stops every voice, these included, so
    // Aural claims and stops them with the other typed owners. The root
    // registers each cold; close() clears the list before they retire.
    static constexpr std::size_t maximumWorldSoundStops26D070=4;
    std::array<rr::platform::LogicalAudioOwner*,maximumWorldSoundStops26D070> worldSoundStops26D070_{};
    std::size_t worldSoundStopCount26D070_=0;
    // Course one-shot World4D8 owners (GB door PodSound, TC trigger Sounds).
    std::array<native_results_world_stop26D070::Owner*,maximumWorldSoundStops26D070> worldOwnerStops26D070_{};
    std::size_t worldOwnerStopCount26D070_=0;
    bool canStopResultsWorld26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&)const noexcept;
    std::size_t resultsWorldStopCount26D070()const noexcept;
    std::size_t resultsWorldStopVoice26D070(std::size_t)const noexcept;
    void stopResultsWorldPreflighted26D070()noexcept;
    bool canStopResultsEngines26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&,std::uint64_t)const noexcept;
    // Separate zero-human domain. Never relax the existing human Course gate.
    bool canStopAll8Engines26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&,std::uint64_t)const noexcept;
    bool canStopAll8Auxiliary26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&,std::uint64_t)const noexcept;
    bool canStopNativeEngineRows26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&,std::uint64_t)const noexcept;
    bool canStopNativeAuxiliaryRows26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&,std::uint64_t)const noexcept;
    std::size_t resultsEngineStopCount26D070()const noexcept {
        return (racerCount_-firstAiSlot_)*3;
    }
    std::size_t resultsEngineStopVoice26D070(std::size_t ordinal)const noexcept {
        if(ordinal>=resultsEngineStopCount26D070())return rr::platform::audioMaximumVoices50HD;
        return racers_[firstAiSlot_+ordinal/3].loops.resultsStopVoice26D070(ordinal%3);
    }
    void stopResultsEnginesPreflighted26D070()noexcept {
        for(std::size_t i=firstAiSlot_;i<racerCount_;++i)
            racers_[i].loops.stopResultsNativePreflighted26D070();
    }
    using E=rr::game::OriginalAiEngineEmitter;
    using Sound=rr::game::OriginalAiEmitterSound;
    using Channel=rr::platform::LogicalAudioOwner;
    struct Racer {
        E engine{};rr::game::OriginalAiEmitterSpatial binder{};
        rr::game::OriginalPodCollisionResponseOwners::Emitter* materialD44=nullptr;
        std::array<Sound,7> sounds{};
        Channel loops{},wrecks{},materials{};
        rr::game::PodSpeechCounts50HG speechCounts{};
        rr::game::PodSpeechState50HG speech{};
        gameplay_audio50HE::Provider speechProvider{};
        Owner* owner=nullptr;std::size_t slot=0;
    };
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner();
    bool prepare(const std::array<opponent_audio50HX::Source,8>& sources,
        const std::filesystem::path& audioRoot,const rr::assets::AudioBank50HC& events,
        rr::platform::AudioMixer50HD& mixer,std::uint64_t session,std::uint16_t actualPacked1125C,std::string& error) {
        namespace d=pod_materials50ed_fixture::detail;namespace a=rr::assets;
        if(attempted_||!session){error="AI audio requires fresh native ownership";return false;}
        attempted_=true;
        try {
            mode_=mixer.originalSoundMode50KG();
            d::need(mode_&&mode_->ready(),"AI audio requires the actual original sound-mode owner");
            d::need(materialD44_.prepare(session,actualPacked1125C),"actual roster PodRaceAudioManager D44 pool rejected");
            firstAiSlot_=materialD44_.humanCount();racerCount_=materialD44_.racerCount();
            std::vector<a::AudioRequest50HC> requests;
            for(std::size_t i=firstAiSlot_;i<racerCount_;++i){mappings_[i]=opponent_audio50HX::detail::readMapping(sources[i]);
                for(const auto* name:{&mappings_[i].engineName,&mappings_[i].binderName}){
                    bool exists=false;for(const auto& request:requests)if(request.name==*name)exists=true;
                    if(!exists)requests.push_back({*name,false,false,false});
                }
            }
            if(!a::loadAudioBank50HC(audioRoot,requests,bank_,error))return false;
            auto clip=[](const a::AudioClip50HC* asset,rr::platform::AudioClip50HD& out){
                if(!asset)return false;
                if(asset->state!=a::AudioAssetState50HC::ready)return true;
                const auto& p=asset->pcm;
                out={p.samples.data(),p.frames,p.sampleRate,p.channels,
                    p.hasEmbeddedLoop?p.loopStart:0,p.hasEmbeddedLoop?p.loopEnd:0};
                return p.channels==1&&rr::platform::AudioMixer50HD::validClip(out);
            };
            for(std::size_t i=firstAiSlot_;i<racerCount_;++i){auto& r=racers_[i];r.owner=this;r.slot=i;
                const auto* identity=original_player_identity50IK::find(sources[i].podId);
                d::need(identity,"AI audio racer identity unavailable");
                // Explicit stable native object IDs, never PS2 addresses.
                const auto base=std::uint64_t(i+1)*16;
                rr::game::OriginalAiEmitterIdentity id{session,base+1,base+2,base+3,
                    static_cast<std::uint32_t>(i),identity->racer60};
                d::need(rr::game::projectFreshOriginalAiEngineEmitter(r.engine,id)&&
                    rr::game::projectFreshOriginalAiSecondaryEmitter(r.binder,session,base+4)&&
                    rr::game::projectOriginalAiEmitterSpatialSetup2911E0(r.engine,r.binder),
                    "AI source emitter construction/setup rejected");
                const auto first=i?96+(i-1)*3:57;
                d::need(r.loops.bind(mixer,first,3)&&r.wrecks.bind(mixer,160+i*4,4),
                    "AI logical sound ranges unavailable");
                for(std::size_t part=0;part<7;++part){
                    const a::AudioClip50HC* asset=nullptr;
                    if(part<2)asset=bank_.find(part?mappings_[i].binderName:mappings_[i].engineName);
                    else asset=events.find(gameplay_audio50HE::cueFilename({part==2?
                        rr::game::AudioBank50HE::PodEngine:rr::game::AudioBank50HE::PodWreck,
                        part==2?3u:static_cast<std::uint32_t>(part-2)}));
                    rr::platform::AudioClip50HD pcm{};
                    d::need(clip(asset,pcm),"AI original sound PCM record invalid");
                    auto& owner=part<3?r.loops:r.wrecks;const auto ordinal=part<3?part:part-3;
                    d::need(owner.registerClip(ordinal,pcm,asset->state==a::AudioAssetState50HC::ready)&&
                        rr::game::bindOriginalAiNativeSound(r.sounds[part],session,base+5+part,owner,ordinal),
                        "AI original logical sound registration rejected");
                }
                auto& engine=*r.sounds[0].nativeSound;auto& binder=*r.sounds[1].nativeSound;
                engine.loop1B0=binder.loop1B0=1; //232DB0 flagsA9 /291810 flags84.
                engine.pitch40=819;binder.pitch40=4096; //16DE60, constructor44C/454.
                auto volume=[](float value){return static_cast<std::int32_t>(
                    rr::game::detail::scalarMin48O(rr::game::detail::scalarMax48O(0.f,value),100.f));};
                engine.volume38=volume(mappings_[i].parameters.engineVolume);
                binder.volume38=volume(mappings_[i].parameters.binderVolume);
                r.sounds[2].nativeSound->volume38=90; //178AE0 boost.
                r.binder.sound1F0=&r.sounds[1];r.engine.spatial.sound1F0=&r.sounds[0];
                //16DD60 secondary registration precedes primary registration.
                d::need(aural_.registerEmitter(r.binder),"AI binder Aural registration rejected");
                engine.multiplier54=binder.multiplier54=0.f;
                d::need(rr::game::stopOriginalAiSpatial291A20(r.engine,r.engine.spatial)&&
                    rr::game::stopOriginalAiSpatial291A20(r.engine,r.binder),"AI cold stop rejected");
                const auto& p=mappings_[i].parameters;
                d::need(rr::game::bindOriginalAiEngineEmitterAfterSetup(r.engine,r.sounds[0],r.binder,
                    p.idlePitch,p.pitchRange,p.idleBinder,p.binderRange,&r.sounds[2])&&
                    aural_.registerEmitter(r.engine.spatial),"AI D40 ownership join rejected");
                r.materialD44=materialD44_.bindAiRacer(r.engine,
                    static_cast<std::uint32_t>(i-firstAiSlot_));
                d::need(r.materialD44,"actual AI vehicle D44 pool-row binding rejected");
            }
            prepared_=true;error.clear();return true;
        }catch(const std::exception& e){error=e.what();return false;}
        catch(...){error="AI source audio preparation failed";return false;}
    }
    bool prepareHumanResetOwner(rr::game::OriginalHumanEngineResetOwner50LB& owner) noexcept {
        if(!prepared_||activated_||firstAiSlot_!=1||humanResetOwner_)return false;
        auto* row=materialD44_.bindHumanRacer(owner);
        if(!row)return false;
        humanResetOwner_=&owner;humanResetD44_=row;return true;
    }
    bool humanPhysicalResetOwners(std::uint32_t actualPod60,
        const rr::game::VehicleFrameState49W& frame,
        rr::game::OriginalHumanEngineResetOwner50LB& owner,
        std::uint8_t*& flags22C,std::uint32_t*& field24C) noexcept {
        flags22C=nullptr;field24C=nullptr;
        if(!prepared_||firstAiSlot_!=1||humanResetOwner_!=&owner||
           !owner.bound()||owner.identity().pod60!=actualPod60||
           frame.fields.vehicle58!=0u)return false;
        // The human wrapper reaches reset from its actual packed-byte cut;
        // phase admission belongs to that wrapper, not the AI PostPlayer gate.
        field24C=materialD44_.resetHumanField24C(humanResetD44_,owner);
        if(!field24C)return false;
        flags22C=&owner.flags22C();return true;
    }
    //2C1170 owns three distinct event banks per actual racer, all in the
    // application Character11C group. Only immutable PCM names can alias;
    // counts, six cursors, cooldown and logical voices belong to this D48.
    bool prepareSpeech(const std::filesystem::path& discRoot,
        const std::filesystem::path& audioRoot,rr::platform::AudioMixer50HD& mixer,
        rr::platform::RaceAudioGroups50IC& groups,std::string& error) {
        namespace d=pod_materials50ed_fixture::detail;namespace a=rr::assets;
        namespace g=rr::game;namespace p=rr::platform;
        if(!prepared_||speechAttempted_||activated_||!groups.uses(mixer)){
            error="AI D48 cold ownership lifecycle rejected";return false;
        }
        speechAttempted_=true;
        try {
            std::array<selected_player_audio50IJ::Catalog,8> catalogs{};
            std::vector<a::AudioRequest50HC> requests;
            for(std::size_t slot=firstAiSlot_;slot<racerCount_;++slot){
                selected_player_audio50IJ::Authoring own;
                if(!selected_player_audio50IJ::read(discRoot,racers_[slot].engine.identity.pod60,own,error))return false;
                racers_[slot].speechCounts=own.speech;
                for(std::size_t cue=0;cue<gameplay_audio50HE::cueCount;++cue){
                    const auto bank=gameplay_audio50HE::catalog[cue].bank;
                    if(bank!=g::AudioBank50HE::PodCurses&&bank!=g::AudioBank50HE::PodTaunts&&
                       bank!=g::AudioBank50HE::PodSpecial)continue;
                    const auto& request=own.catalog[cue];if(request.name.empty())continue;
                    d::need(!request.loop&&!request.stereo&&request.stream,
                        "AI D48 authored speech transport flags differ");
                    catalogs[slot][cue]=request;
                    bool exists=false;
                    for(const auto& prior:requests)
                        if(native_racer50HW::rig_detail::lower(prior.name)==
                           native_racer50HW::rig_detail::lower(request.name)){
                            d::need(prior.loop==request.loop&&prior.stereo==request.stereo&&
                                prior.stream==request.stream,"AI D48 aliased original flags differ");
                            exists=true;break;
                        }
                    if(!exists)requests.push_back(request);
                }
            }
            if(!a::loadAudioBank50HC(audioRoot,requests,speechBank_,error))return false;
            for(std::size_t slot=firstAiSlot_;slot<racerCount_;++slot){
                const auto first=p::originalSpeechFirstVoice50HD+slot*p::originalSpeechVoicesPerRacer50HD;
                std::array<std::size_t,gameplay_audio50HE::cueCount> voices{};
                std::size_t count=0;
                for(std::size_t cue=0;cue<voices.size();++cue)
                    if(!catalogs[slot][cue].name.empty())voices[cue]=first+count++;
                const auto& c=racers_[slot].speechCounts;
                const auto expected=std::size_t(c.lowDamage)+c.wallHit+c.passed+c.attacked+
                    c.passing+c.attacking+c.death+c.win+c.lose;
                d::need(count==expected&&count<=p::originalSpeechVoicesPerRacer50HD&&
                    groups.bindRange(first,count,g::RaceAudioGroup50IC::Character11C),
                    "AI D48 own authored speech rows/group range unavailable");
                if(!racers_[slot].speechProvider.prepare(speechBank_,mixer,first,error,
                    &catalogs[slot],&voices))return false;
            }
            speechPrepared_=true;error.clear();return true;
        }catch(const std::exception& e){error=e.what();return false;}
        catch(...){error="AI D48 original speech preparation failed";return false;}
    }
    // Native cold owner order is the actual active bank rows. In mixed Course,
    // initialize the SAME caller-owned human D48 first, then actual AI rows.
    // Only those real owners consume their shared308030 draws, never
    // copied selected-player state or an extra human initialization in ALL8.
    // This does not claim the unjoined retail allocator/constructor chronology.
    bool initializeSpeech(std::uint32_t& actualSharedRaw,
        const rr::game::PodSpeechCounts50HG* actualHumanCounts=nullptr,
        rr::game::PodSpeechState50HG* actualHumanState=nullptr)noexcept {
        if(!speechPrepared_||speechInitialized_)return false;
        if(firstAiSlot_==1u){
            if(!actualHumanCounts||!actualHumanState)return false;
            // The player's own 2C1170 (pod load, 2C1300) may already have drawn
            // this SAME D48's cursors at their source position; never redraw.
            const bool drawn=actualHumanState->initialized;
            if(!drawn&&rr::game::initializePodSpeech50HG(*actualHumanCounts,actualSharedRaw,*actualHumanState)!=
               rr::game::PodSpeechStatus50HG::Complete)return false;
            std::fprintf(stderr,"speech_cursor_init human=%s ai=%zu",drawn?"pod_load_2c1300":"all_eight",
                racerCount_>firstAiSlot_?racerCount_-firstAiSlot_:0u);std::fputc(10,stderr);
        }else if(actualHumanCounts||actualHumanState)return false;
        for(std::size_t slot=firstAiSlot_;slot<racerCount_;++slot){auto& r=racers_[slot];
            if(rr::game::initializePodSpeech50HG(r.speechCounts,actualSharedRaw,r.speech)!=
               rr::game::PodSpeechStatus50HG::Complete)return false;
        }
        speechInitialized_=true;return true;
    }
    bool speechOwner(std::size_t slot,std::uint32_t actualRacer60,
        const rr::game::PodSpeechCounts50HG*& counts,rr::game::PodSpeechState50HG*& state)noexcept {
        counts=nullptr;state=nullptr;
        if(!speechPrepared_||!speechInitialized_||!ownsSlot(slot))return false;
        auto& r=racers_[slot];
        if(r.engine.identity.managerOrdinal!=slot||r.engine.identity.pod60!=actualRacer60)return false;
        counts=&r.speechCounts;state=&r.speech;return true;
    }
    bool applySpeech(std::size_t slot,std::uint32_t actualRacer60,
        const rr::game::AudioCommand50HE& command)noexcept {
        const rr::game::PodSpeechCounts50HG* counts=nullptr;rr::game::PodSpeechState50HG* state=nullptr;
        if(!speechOwner(slot,actualRacer60,counts,state))return false;
        using Result=gameplay_audio50HE::ApplyResult;
        const auto result=racers_[slot].speechProvider.apply(command);
        // Missing PCM is the genuine negative audio return; it is never a
        // fabricated silent clip and does not undo source cursor/cooldown stores.
        return result==Result::Started||result==Result::Unavailable;
    }
    // Actual source-created D44 arrays borrow the authenticated track material
    // bank, which must outlive this owner. Each racer has separate logical
    // sounds. No sample ID/base-pitch lookup, playback, or RNG is invented.
    // A failed cold prefix is terminal for this join but does not invalidate
    // the independently completed D40/D44 reset fields or engine ownership.
    bool prepareMaterialArrays(rr::platform::AudioMixer50HD& mixer,
        rr::platform::RaceAudioGroups50IC& groups,
        const rr::assets::MaterialAudioCatalog50HI& catalog,
        const rr::assets::AudioBank50HC& bank,std::string& error) {
        if(!prepared_||materialAttempted_||activated_){error="AI D44 material cold lifecycle rejected";return false;}
        materialAttempted_=true;
        for(std::size_t slot=0;slot<racerCount_;++slot){
            auto* row=slot<firstAiSlot_?humanResetD44_:racers_[slot].materialD44;
            auto& channel=slot<firstAiSlot_?humanMaterials_:racers_[slot].materials;
            if(!row||!native_original_ai_material::prepareArrays(
                *row,channel,mixer,groups,catalog,bank,error))return false;
        }
        materialPrepared_=true;error.clear();return true;
    }
    bool materialArraysPrepared()const noexcept{return materialPrepared_;}
    // Explicit host audio-rendering choice, never a recovered IOP metadata
    // receipt. Sound basePitchKnown remains false; source RNG sites/order are
    // retained. The integer pitch scale/parity is an admitted native policy.
    bool selectNativeMaterialPitchPolicy()noexcept {
        if(!materialPrepared_||materialNativeBound_||activated_)return false;
        for(std::size_t slot=0;slot<racerCount_;++slot){
            const auto* row=slot<firstAiSlot_?humanResetD44_:racers_[slot].materialD44;
            if(!row||row->material().blocked||row->material().inDispatch)return false;
        }
        for(std::size_t slot=0;slot<racerCount_;++slot){
            auto* row=slot<firstAiSlot_?humanResetD44_:racers_[slot].materialD44;
            row->material().pitchPolicy=rr::game::OriginalPodMaterialPitchPolicy::nativeDecodedRateUnity4096;
        }
        materialNativeBound_=true;return true;
    }
    // Checked native event identity for the actual D40. The high namespace
    // keeps particle owner IDs disjoint; it is not a PS2 address or sound ID.
    bool materialEventTarget(std::size_t slot,std::uint32_t actualPod60,
        const rr::game::VehicleFrameState49W& frame,std::uint32_t& target)const noexcept {
        target=0;
        if(!activated_||!materialNativeBound_||
           frame.phase!=rr::game::VehicleFramePhase49W::PostImpactRepair)return false;
        const auto* id=materialIdentity(slot,actualPod60,frame);
        if(!id)return false;
        target=0x10000000u|static_cast<std::uint32_t>(id->emitterD40);return true;
    }
    rr::game::OriginalPodMaterialAudioReceipt materialChannel(std::size_t slot,
        std::uint32_t actualPod60,const rr::game::VehicleFrameState49W& frame,
        const rr::game::VehicleFrameBindings49W& bindings,
        const rr::game::ImpactEffectEvent50B& event,std::int32_t actualContactA9,
        float actualSpeedRatio70,rr::game::CoupledNoiseContext49M& shared,
        rr::game::RaceEventQueue50ID<128>& actualEvents,
        const rr::game::RaceActorResolver50ID& actualActors,bool actualEventOwner33CBound)noexcept {
        namespace g=rr::game;
        g::OriginalPodMaterialAudioReceipt rejected{};rejected.status=g::OriginalPodMaterialAudioStatus::owner;
        if(!activated_||!materialNativeBound_||!listenerReady_||!mode_||
           frame.phase!=g::VehicleFramePhase49W::PostImpactEffects50O)return rejected;
        const auto* id=materialIdentity(slot,actualPod60,frame);
        if(!id||event.target!=(0x10000000u|static_cast<std::uint32_t>(id->emitterD40))||
           event.kind!=g::ImpactEffectKind50B::ChannelA8||
           !((event.argument==2u&&event.sourcePC==0x224130u)||
             (event.argument==3u&&event.sourcePC==0x224148u)))return rejected;
        const auto node=static_cast<std::int32_t>(bindings.animation.bodyScene288);
        if(!frame.nodes||node<0||static_cast<std::uint32_t>(node)>=frame.sceneNodeCount||
           frame.sceneNodeCount>g::maxSceneNodes49P)return rejected;
        g::OriginalPodMaterialAudioInput input{};
        input.channelArgument=event.argument;input.vehicle58=frame.fields.vehicle58;
        input.contactMaterialA9=actualContactA9;input.speedRatio70=actualSpeedRatio70;
        //1790A8 ->22A180 copies actual288 LOCAL XYZ into the caller's vf0;
        //1790BC copies the actual340 body's full velocity70 including70.W.
        input.position={{0.f,0.f,0.f,1.f}};
        std::memcpy(input.position.v,frame.nodes[static_cast<std::size_t>(node)].localC0.v+12,12);
        std::memcpy(input.velocity.v,frame.bodies.main.components.root.body.velocity.v,12);
        std::memcpy(input.velocity.v+3,&frame.bodies.mainSphere.velocity70W,4);
        const auto result=slot<firstAiSlot_?
            g::dispatchOriginalPodMaterialAudio179070(humanResetD44_->material(),
                *humanResetOwner_,input,&listener_,1,mode_->fields(),shared,
                actualEvents,actualActors,actualEventOwner33CBound):
            g::dispatchOriginalPodMaterialAudio179070(racers_[slot].materialD44->material(),
                racers_[slot].engine,input,&listener_,1,mode_->fields(),shared);
        if(result.status!=g::OriginalPodMaterialAudioStatus::complete&&!materialFailureReported_){
            materialFailureReported_=true; // Report the first failure before caller teardown.
            try {
                std::cerr<<"original_material_input_failure slot="<<slot<<" pod60="<<actualPod60
                    <<" status="<<static_cast<unsigned>(result.status)<<" pc="<<std::hex<<result.sourcePC<<std::dec
                    <<" activated="<<activated_<<" material_bound="<<materialNativeBound_
                    <<" listener_ready="<<listenerReady_<<" mode_present="<<(mode_!=nullptr)
                    <<" phase="<<static_cast<unsigned>(frame.phase)<<" event_target="<<event.target
                    <<" event_pc="<<std::hex<<event.sourcePC<<std::dec
                    <<" first_ai_slot="<<firstAiSlot_<<" random_draws="<<result.randomDraws;
                original_material_input_diagnostics50LN::write(std::cerr,input,
                    slot<firstAiSlot_?0u:1u,&listener_,1,mode_->fields(),std::fegetround(),result);
                std::cerr<<'\n';std::cerr.flush();
            }catch(...){/* Diagnostics cannot replace the original failure receipt. */}
        }
        ++materialDispatches_;materialDraws_+=result.randomDraws;materialLast_=result;
        return result;
    }
    std::uint64_t materialDispatches()const noexcept{return materialDispatches_;}
    std::uint64_t materialRandomDraws()const noexcept{return materialDraws_;}
    // Native race activation owns these source lifecycle events. Not a claim
    // that the missing retail application event dispatcher was traversed.
    bool activate()noexcept {
        if(!prepared_||!speechInitialized_||activated_)return false;
        for(std::size_t slot=firstAiSlot_;slot<racerCount_;++slot){auto& r=racers_[slot];
         for(auto operation:{rr::game::OriginalAiEmitterLifecycle::startEngine16DB80,
            rr::game::OriginalAiEmitterLifecycle::startBinder16DC00})
            if(rr::game::applyOriginalAiEmitterLifecycle(r.engine,operation).status!=
                rr::game::OriginalAiEmitterStatus::complete)return false;
        }
        activated_=true;return true;
    }
    bool setListener(const rr::game::OriginalAiAudioListener& listener)noexcept {
        for(unsigned i=0;i<3;++i)if(!std::isfinite(listener.position[i])||
            !std::isfinite(listener.forward[i]))return false;
        listener_=listener;listenerReady_=true;return true;
    }
    bool thermal(std::size_t slot,const rr::game::VehicleFrameState49W& frame,
        const rr::game::VehicleFrameBindings49W& bindings,const rr::game::BoostInput50FK& input,
        const rr::game::BoostParameters50FK& parameters,rr::game::BoostState50FK& state,
        rr::game::BoostDiagnostic50FK& diagnostic)noexcept {
        if(!ownsSlot(slot)||!frame.fields.nativeAiOwnersBound50IV||
           frame.fields.vehicle58!=1u){
            std::fprintf(stderr,"ai_thermal_rejected slot=%zu reason=owner owns=%d bound=%d vehicle58=%u\n",slot,
                ownsSlot(slot),frame.fields.nativeAiOwnersBound50IV?1:0,static_cast<unsigned>(frame.fields.vehicle58));
            return false;
        }
        auto& r=racers_[slot];
        rr::game::OriginalAiEmitterRacerInput observed;
        if(!rr::game::readOriginalAiEmitterRacerInput(r.engine.identity,frame,bindings,state.speedRatio70,observed)){
            std::fprintf(stderr,"ai_thermal_rejected slot=%zu reason=racer_input\n",slot);return false;
        }
        const rr::game::OriginalAiEmitterBoostBackend backend{&r,attachBoost,startBoost};
        const auto receipt=rr::game::advanceOriginalAiThermalWithD40(r.engine,observed,input,parameters,state,
            backend,diagnostic);
        if(receipt.skippedBoostSoundPC&&skippedBoostSoundLogs_<16){
            ++skippedBoostSoundLogs_;
            std::fprintf(stderr,"ai_boost_sound_skipped slot=%zu source_pc=0x%x\n",slot,receipt.skippedBoostSoundPC);
        }
        if(receipt.status!=rr::game::OriginalAiEmitterStatus::complete)
            std::fprintf(stderr,"ai_thermal_rejected slot=%zu status=%u source_pc=0x%x\n",slot,
                static_cast<unsigned>(receipt.status),receipt.sourcePC);
        return receipt.status==rr::game::OriginalAiEmitterStatus::complete;
    }
    bool children(std::size_t slot,const rr::game::VehicleFrameState49W& frame,
        const rr::game::VehicleFrameBindings49W& bindings,const rr::game::BoostState50FK& thermal)noexcept {
        if(!ownsSlot(slot)||!frame.fields.nativeAiOwnersBound50IV||
           frame.fields.vehicle58!=1u)return false;auto& r=racers_[slot];
        rr::game::OriginalAiEmitterRacerInput observed;
        if(!rr::game::readOriginalAiEmitterRacerInput(r.engine.identity,frame,bindings,thermal.speedRatio70,observed))return false;
        // Actual2C20C0 D48 ordinary update. Its141300 child list starts empty;
        //2C1170 loads logical sounds, and does not append object children.
        if(!rr::game::advancePodSpeechCooldown50HG(observed.elapsed,r.speech))return false;
        return rr::game::advanceOriginalAiEngineEmitter16DA80(r.engine,observed,{}).status==
            rr::game::OriginalAiEmitterStatus::complete;
    }
    // Original AI D40 virtual9C = 16DF70, reached from 2261B0 when the racer
    // explodes. Picks one of D40+200's wreck cues (sounds[3..6]) from the shared
    // 308030 stream with the lazy 3D6B30 seed and 3D6B38 range guard, plays it
    // at the racer (104140 near 0, far 1200; start flags 6) unless already
    // active, stops the engine (v84=16DBC0) and binder (v88=16DC10) loops and
    // sets 22C bit2. Its 1A64B0 race event and 2C2810 D44 call are not joined.
    bool destroyed16DF70(std::size_t slot,const rr::game::VehicleFrameState49W& frame,
        const rr::game::VehicleFrameBindings49W& bindings,rr::game::SharedRandomOwner50ID& random,
        std::uint32_t& sharedRaw308030,rr::game::HostMonotonicClock50ID& clock,std::uint32_t& cue)noexcept {
        cue=0xffffffffu;
        if(!ownsSlot(slot)||frame.fields.vehicle58!=1u)return false;
        auto& r=racers_[slot];
        if(r.engine.flags22CLow4&4u)return true; //16DF94: already dead.
        // 16DFF8 22A180: body288 LOCAL XYZ, read directly (the destroy handler
        // runs mid-frame, outside the phase-gated emitter input reader).
        const auto node=static_cast<std::int32_t>(bindings.animation.bodyScene288);
        if(node<0||static_cast<std::uint32_t>(node)>=frame.sceneNodeCount){
            std::fprintf(stderr,"ai_wreck_audio_refused slot=%zu reason=body288\n",slot);return false;}
        rr::game::OriginalAiEmitterVector position{{0,0,0,1}};
        std::memcpy(position.v,frame.nodes[static_cast<std::uint32_t>(node)].localC0.v+12,12);
        constexpr std::int32_t count=4; //D40+200 wreck list: PodWreck cues1..4.
        if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
        if(!random.seed.seed3D6B30){
            std::uint32_t stamp=0;if(!rr::game::sampleHostTimestamp50ID(clock,stamp))return false;
            random.seed.seed3D6B30=stamp;sharedRaw308030=stamp; //101A50 then 308020.
        }
        sharedRaw308030=sharedRaw308030*0x41c64e6du+0x3039u; //308030
        const auto drawn=static_cast<std::int32_t>(sharedRaw308030&0x7fffffffu);
        volatile float unit=std::bit_cast<float>(0x30000000u)*static_cast<float>(drawn); //2^-31
        volatile float scaled=static_cast<float>(count)*unit;
        std::int32_t index=static_cast<std::int32_t>(scaled); //CVT.W.S toward zero.
        if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
        if(random.range.rangeCheck3D6B38&&(index<0||index>count-1))return false; //16E0E0 assertion.
        if(index>count-1)index=count-1;if(index<0)index=0;
        auto& sound=r.sounds[3+static_cast<std::size_t>(index)];cue=static_cast<std::uint32_t>(index);
        if(sound.nativeOwner&&sound.nativeSound&&!sound.nativeOwner->active(*sound.nativeSound)){
            bool returned=false;
            if(attachPositional(r,sound,position,0.f,1200.f,returned)&&returned){
                const auto started=rr::game::startAiLogicalSound233410(*sound.nativeOwner,*sound.nativeSound,6);
                if(started.status!=rr::game::AiLogicalSoundStatus::complete)
                    std::fprintf(stderr,"ai_wreck_sound_start_refused slot=%zu status=%u\n",slot,static_cast<unsigned>(started.status));
            }
        }
        r.engine.flags22CLow4=static_cast<std::uint8_t>((r.engine.flags22CLow4&~4u)|4u);
        const auto engine=rr::game::applyOriginalAiEmitterLifecycle(r.engine,rr::game::OriginalAiEmitterLifecycle::stopEngine16DBC0);
        const auto binder=rr::game::applyOriginalAiEmitterLifecycle(r.engine,rr::game::OriginalAiEmitterLifecycle::stopBinder16DC10);
        if(engine.status!=rr::game::OriginalAiEmitterStatus::complete||
           binder.status!=rr::game::OriginalAiEmitterStatus::complete)
            std::fprintf(stderr,"ai_wreck_audio_refused slot=%zu reason=lifecycle engine=%u/0x%x binder=%u/0x%x\n",slot,
                static_cast<unsigned>(engine.status),engine.sourcePC,static_cast<unsigned>(binder.status),binder.sourcePC);
        return engine.status==rr::game::OriginalAiEmitterStatus::complete&&
            binder.status==rr::game::OriginalAiEmitterStatus::complete;
    }
    bool mixBoundary(rr::game::CoupledNoiseContext49M& shared)noexcept {
        if(!prepared_||!activated_||!listenerReady_||!mode_)return false;
        last_=aural_.update(&listener_,1,mode_->fields(),shared);
        return last_.status==rr::game::OriginalAiAuralStatus::complete;
    }
    //216A50..216A84 borrows the SAME D40 and the real session D44 row.
    // No audio stop/restart or player controller replacement is implied.
    bool physicalResetOwners(std::size_t slot,std::uint32_t actualPod60,
        const rr::game::VehicleFrameState49W& frame,std::uint8_t*& flags22C,
        std::uint32_t*& field24C) noexcept {
        flags22C=nullptr;field24C=nullptr;
        if(!ownsSlot(slot)||frame.phase!=rr::game::VehicleFramePhase49W::PostPlayer||
           frame.fields.vehicle58!=1u||!frame.fields.nativeAiOwnersBound50IV)return false;
        auto& r=racers_[slot];
        if(r.engine.identity.managerOrdinal!=slot||r.engine.identity.pod60!=actualPod60)return false;
        field24C=materialD44_.resetField24C(r.materialD44,r.engine);
        if(!field24C)return false;flags22C=&r.engine.flags22CLow4;return true;
    }
    // Distinct source LostAI E8 borrow at the real PostWorld cut. Ordinary
    // AI recovery keeps physicalResetOwners' PostPlayer admission unchanged.
    // This is a short borrow of the SAME session D40/D44, never a new owner,
    // phase rewrite, audio restart or original-AI dispatch receipt.
    bool sourceTriggerResetOwners50MB(std::size_t slot,std::uint32_t actualPod60,
        const rr::game::VehicleFrameState49W& frame,std::uint8_t*& flags22C,
        std::uint32_t*& field24C) noexcept {
        flags22C=nullptr;field24C=nullptr;
        if(!ownsSlot(slot)||!actualPod60||frame.phase!=rr::game::VehicleFramePhase49W::PostWorld||
           frame.fields.vehicle58!=1u||frame.fields.vehicle3C||!frame.fields.nativeAiOwnersBound50IV||
           frame.bodies.main.phase!=rr::game::MainBodyPhase49K::AwaitingWorldPreparation||
           frame.bodies.phase!=rr::game::CoupledBodyPhase49M::AwaitingWorldPreparation)return false;
        auto& r=racers_[slot];
        if(r.engine.identity.managerOrdinal!=slot||r.engine.identity.pod60!=actualPod60)return false;
        field24C=materialD44_.resetField24C(r.materialD44,r.engine);
        if(!field24C)return false;flags22C=&r.engine.flags22CLow4;return true;
    }
    // Cold only, before the race; one entry per course world-sound owner.
    bool registerResultsWorldSound26D070(rr::platform::LogicalAudioOwner& transport)noexcept {
        if(worldSoundStopCount26D070_>=maximumWorldSoundStops26D070)return false;
        for(std::size_t n=0;n<worldSoundStopCount26D070_;++n)
            if(worldSoundStops26D070_[n]==&transport)return false;
        worldSoundStops26D070_[worldSoundStopCount26D070_++]=&transport;return true;
    }
    bool registerResultsWorldSound26D070(native_results_world_stop26D070::Owner& owner)noexcept {
        if(worldOwnerStopCount26D070_>=maximumWorldSoundStops26D070)return false;
        for(std::size_t n=0;n<worldOwnerStopCount26D070_;++n)
            if(worldOwnerStops26D070_[n]==&owner)return false;
        worldOwnerStops26D070_[worldOwnerStopCount26D070_++]=&owner;return true;
    }
    void close()noexcept {
        worldSoundStops26D070_.fill(nullptr);worldSoundStopCount26D070_=0; // Before those owners retire.
        worldOwnerStops26D070_.fill(nullptr);worldOwnerStopCount26D070_=0;
        revokeNativeSpeechReceipt(); // Before any provider/borrowed-owner teardown.
        // Close every public update admission before any borrowed sound is
        // retired. Preserve the reached speech preparation for actual voice stops.
        // A failed prepare may already have registered a subset of emitters.
        const bool stopSpeech=speechPrepared_;
        attempted_=true;prepared_=false;activated_=false;listenerReady_=false;mode_=nullptr;
        materialPrepared_=false;materialNativeBound_=false;
        speechPrepared_=false;speechInitialized_=false;
        aural_.retireNativeRegistrations();
        for(std::size_t slot=firstAiSlot_;slot<racerCount_;++slot){auto& r=racers_[slot];
        if(stopSpeech)r.speechProvider.stopAll();r.materials.close();r.materialD44=nullptr;r.wrecks.close();r.loops.close();}
        humanMaterials_.close();materialD44_.close();humanResetOwner_=nullptr;humanResetD44_=nullptr;
    }
    bool prepared()const noexcept{return prepared_;}
    bool ownsSlot(std::size_t actualManagerOrdinal)const noexcept {
        return prepared_&&materialD44_.ownsAiSlot(actualManagerOrdinal);
    }
private:
    const rr::game::OriginalAiEmitterIdentity* materialIdentity(std::size_t slot,
        std::uint32_t actualPod60,const rr::game::VehicleFrameState49W& frame)const noexcept {
        if(!prepared_||slot>=racerCount_)return nullptr;
        const rr::game::OriginalAiEmitterIdentity* id=nullptr;
        const rr::game::OriginalPodCollisionResponseOwners::Emitter* row=nullptr;
        if(slot<firstAiSlot_){
            if(slot!=0u||firstAiSlot_!=1u||!humanResetOwner_||!humanResetOwner_->bound()||
               frame.fields.vehicle58!=0u||frame.fields.nativeAiOwnersBound50IV)return nullptr;
            id=&humanResetOwner_->identity();row=humanResetD44_;
        }else{
            if(!ownsSlot(slot)||frame.fields.vehicle58!=1u||
               !frame.fields.nativeAiOwnersBound50IV)return nullptr;
            id=&racers_[slot].engine.identity;row=racers_[slot].materialD44;
        }
        if(!row||row->poolOrdinal()!=slot||row->vehicle248()!=id||id->managerOrdinal!=slot||
           id->pod60!=actualPod60||!id->emitterD40||id->emitterD40>0x0fffffffu)return nullptr;
        return id;
    }
    static bool startBoost(void* context,Sound& sound,std::uint32_t flags)noexcept {
        auto& r=*static_cast<Racer*>(context);
        if(&sound!=&r.sounds[2]||!sound.nativeOwner||!sound.nativeSound){
            std::fprintf(stderr,"ai_boost_start_refused reason=binding slot_sound=%d owner=%d native=%d\n",
                &sound==&r.sounds[2],sound.nativeOwner!=nullptr,sound.nativeSound!=nullptr);return false;
        }
        const auto result=rr::game::startAiLogicalSound233410(*sound.nativeOwner,*sound.nativeSound,flags);
        if(result.status!=rr::game::AiLogicalSoundStatus::complete)
            std::fprintf(stderr,"ai_boost_start_refused reason=start233410 status=%u source_pc=0x%x returned=%d\n",
                static_cast<unsigned>(result.status),result.sourcePC,result.sourceReturned?1:0);
        return result.status==rr::game::AiLogicalSoundStatus::complete;
    }
    // 104140 positional attach shared by the boost and wreck one-shots.
    static bool attachPositional(Racer& r,Sound& sound,const rr::game::OriginalAiEmitterVector& position,
        float nearDistance,float farDistance,bool& returned)noexcept {
        auto& owner=*r.owner;returned=false;
        if(!owner.listenerReady_||!owner.mode_||!sound.nativeSound||!sound.nativeOwner)return false;
        using namespace rr::game::original_ai_aural_detail;
        const auto& l=owner.listener_;const auto distanceXZ=distance(position.v[0],position.v[2],l.position[0],l.position[2]);
        if(!std::isfinite(distanceXZ))return false;
        if(!(distanceXZ<farDistance))return true;
        float forward=0.f,target=0.f;
        if(!rr::game::originalAngle48G(l.forward[2],l.forward[0],forward)||
           !rr::game::originalAngle48G(sub(position.v[2],l.position[2]),sub(position.v[0],l.position[0]),target))return false;
        const auto degrees=literal(0x42652ee1u);
        auto angle=static_cast<std::int32_t>(mul(target,degrees))-static_cast<std::int32_t>(mul(forward,degrees));
        if(angle>180)angle-=359;else if(angle< -180)angle+=359;
        for(unsigned i=0;i<3;++i)sound.nativeSound->position60[i]=position.v[i];
        const float attenuation=distanceXZ<=nearDistance?100.f:
            sub(100.f,mul(100.f,div(sub(distanceXZ,nearDistance),sub(farDistance,nearDistance))));
        const auto result=rr::game::spatialAiLogicalSound233AE0(*sound.nativeOwner,*sound.nativeSound,
            owner.mode_->fields(),attenuation,angle,l.position,false);
        if(result.status!=rr::game::AiLogicalSoundStatus::complete)return false;
        returned=true;return true;
    }
    static bool attachBoost(void* context,Sound& sound,const rr::game::OriginalAiEmitterVector& position,
        const rr::game::OriginalAiEmitterVector&,float nearDistance,float farDistance,bool& returned)noexcept {
        auto& r=*static_cast<Racer*>(context);auto& owner=*r.owner;
        if(&sound!=&r.sounds[2]||!owner.listenerReady_||!owner.mode_||!sound.nativeSound){
            std::fprintf(stderr,"ai_boost_attach_refused reason=binding slot_sound=%d listener=%d mode=%d native=%d\n",
                &sound==&r.sounds[2],owner.listenerReady_?1:0,owner.mode_!=nullptr,sound.nativeSound!=nullptr);return false;
        }
        using namespace rr::game::original_ai_aural_detail;
        const auto& l=owner.listener_;const auto distanceXZ=distance(position.v[0],position.v[2],l.position[0],l.position[2]);
        if(!std::isfinite(distanceXZ)||nearDistance!=0.f||farDistance!=450.f){
            std::fprintf(stderr,"ai_boost_attach_refused reason=distance distance=%g near=%g far=%g\n",
                distanceXZ,nearDistance,farDistance);return false;
        }
        returned=false;if(!(distanceXZ<farDistance))return true;
        float forward=0.f,target=0.f;
        if(!rr::game::originalAngle48G(l.forward[2],l.forward[0],forward)||
           !rr::game::originalAngle48G(sub(position.v[2],l.position[2]),sub(position.v[0],l.position[0]),target)){
            std::fprintf(stderr,"ai_boost_attach_refused reason=angle listener_fwd=%g,%g delta=%g,%g\n",
                l.forward[0],l.forward[2],sub(position.v[0],l.position[0]),sub(position.v[2],l.position[2]));return false;
        }
        const auto degrees=literal(0x42652ee1u);
        auto angle=static_cast<std::int32_t>(mul(target,degrees))-static_cast<std::int32_t>(mul(forward,degrees));
        if(angle>180)angle-=359;else if(angle< -180)angle+=359;
        for(unsigned i=0;i<3;++i)sound.nativeSound->position60[i]=position.v[i];
        const float attenuation=distanceXZ<=nearDistance?100.f:
            sub(100.f,mul(100.f,div(sub(distanceXZ,nearDistance),sub(farDistance,nearDistance))));
        const auto result=rr::game::spatialAiLogicalSound233AE0(*sound.nativeOwner,*sound.nativeSound,
            owner.mode_->fields(),attenuation,angle,l.position,false);
        if(result.status!=rr::game::AiLogicalSoundStatus::complete){
            std::fprintf(stderr,"ai_boost_attach_refused reason=spatial233AE0 status=%u source_pc=0x%x attenuation=%g angle=%d\n",
                static_cast<unsigned>(result.status),result.sourcePC,attenuation,angle);return false;
        }
        returned=true;return true;
    }
    unsigned skippedBoostSoundLogs_=0;
    rr::game::OriginalHumanEngineResetOwner50LB* humanResetOwner_=nullptr;
    rr::game::OriginalPodCollisionResponseOwners::Emitter* humanResetD44_=nullptr;
    Channel humanMaterials_{};
    rr::game::OriginalPodCollisionResponseOwners materialD44_{};
    std::size_t firstAiSlot_=0,racerCount_=0;
    std::array<Racer,8> racers_{};
    std::array<opponent_audio50HX::Mapping,8> mappings_{};
    rr::assets::AudioBank50HC bank_{},speechBank_{};
    rr::game::OriginalAiAural aural_{};
    rr::game::OriginalAiAuralReceipt last_{};
    rr::game::OriginalAiAudioListener listener_{};
    rr::platform::OriginalSoundMode50KG* mode_=nullptr;
    bool attempted_=false,prepared_=false,activated_=false,listenerReady_=false;
    bool materialAttempted_=false,materialPrepared_=false;
    bool materialNativeBound_=false;
    bool materialFailureReported_=false; // Diagnostic-only, no dispatch admission role.
    std::uint64_t materialDispatches_=0,materialDraws_=0;
    rr::game::OriginalPodMaterialAudioReceipt materialLast_{};
    bool speechAttempted_=false,speechPrepared_=false,speechInitialized_=false;
};
}
