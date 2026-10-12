#pragma once
// Bind the retained authored provider catalog without inferring owner identity
// from a slot number or VAG filename. This does not decode assets or play audio.
#include "gameplay_audio_provider50he_fixture.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"

namespace race_audio_groups50IC {
struct CatalogBindingReceipt {
    bool complete=false;
    std::size_t represented=0,unsupported=0;
    std::array<bool,gameplay_audio50HE::cueCount> supported{};
};
inline CatalogBindingReceipt bindGameplayCatalog(
    rr::platform::RaceAudioGroups50IC& groups,std::size_t firstVoice,bool extraSpeech=false)noexcept {
    using namespace rr::game;
    CatalogBindingReceipt receipt;
    const auto count=extraSpeech?gameplay_audio50HE::cueCount:gameplay_audio50HE::legacyCueCount;
    if(firstVoice>rr::platform::audioMaximumVoices50HD||
       gameplay_audio50HE::legacyCueCount>rr::platform::audioMaximumVoices50HD-firstVoice)return receipt;
    // Whole represented catalog is preflighted before any mapping is changed.
    for(std::size_t i=0;i<count;++i){
        const auto& cue=gameplay_audio50HE::catalog[i];
        const auto group=gameplayAudioGroup50IC({cue.bank,cue.key});
        if(group==RaceAudioGroup50IC::Unrepresented){++receipt.unsupported;continue;}
        const auto voice=gameplay_audio50HE::cueVoice(firstVoice,i);
        if(voice>=rr::platform::audioMaximumVoices50HD||
            (i>=gameplay_audio50HE::legacyCueCount&&voice>=firstVoice&&
             voice-firstVoice<gameplay_audio50HE::legacyCueCount)||!groups.canBind(voice,group))return {};
        receipt.supported[i]=true;++receipt.represented;
    }
    for(std::size_t i=0;i<gameplay_audio50HE::cueCount;++i)if(receipt.supported[i]){
        const auto& cue=gameplay_audio50HE::catalog[i];
        if(!groups.bind(gameplay_audio50HE::cueVoice(firstVoice,i),
            gameplayAudioGroup50IC({cue.bank,cue.key})))return {};
    }
    receipt.complete=true;return receipt;
}
// Explicit provider roles proved by source constructors. Supply the ACTUAL
// range owned by that prepared provider; these helpers reserve no voice slots.
// Never call with an entire broader reservation that includes another role.
enum class ProviderRole:std::uint8_t {
    LapPodEvent,MaterialCollision,CountdownRaceSnd,EngineBinderBoost,InRaceDax
};
inline bool bindProviderRange(rr::platform::RaceAudioGroups50IC& groups,
    ProviderRole role,std::size_t first,std::size_t count)noexcept {
    using Group=rr::game::RaceAudioGroup50IC;
    Group group=Group::Unrepresented;
    switch(role){
        case ProviderRole::LapPodEvent:
        case ProviderRole::MaterialCollision:
        case ProviderRole::CountdownRaceSnd:group=Group::Effects110;break;
        case ProviderRole::EngineBinderBoost:group=Group::Engines114;break;
        case ProviderRole::InRaceDax:group=Group::Announcer120;break;
    }
    return groups.bindRange(first,count,group);
}
} // namespace race_audio_groups50IC
