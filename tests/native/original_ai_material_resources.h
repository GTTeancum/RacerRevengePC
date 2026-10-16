#pragma once
#include "../../src/game/original_pod_collision_response_owner.h"
#include "../../src/assets/material_audio50hi.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include <string>

namespace native_original_ai_material {
using Pool=rr::game::OriginalPodCollisionResponseOwners;
using Channel=rr::platform::LogicalAudioOwner;
// The caller already authenticated the selected track AUB and PCM bank through
// material_audio50HI::Provider. Borrow those same immutable owners; never load
// another file, invent a name or merge duplicate authored logical sound rows.
inline bool prepareArrays(Pool::Emitter& emitter,Channel& channel,
    rr::platform::AudioMixer50HD& mixer,rr::platform::RaceAudioGroups50IC& groups,
    const rr::assets::MaterialAudioCatalog50HI& catalog,
    const rr::assets::AudioBank50HC& bank,std::string& error) {
    namespace g=rr::game;namespace a=rr::assets;namespace p=rr::platform;
    const auto reject=[&](const char* why){error=why;return false;};
    auto& state=emitter.material();
    const auto collision=static_cast<std::size_t>(catalog.parameters.rowCount);
    const auto drive=static_cast<std::size_t>(catalog.parameters.driveRowCount);
    const auto count=collision+drive;
    if(!state.setupApplied||!emitter.vehicle248()||state.vehicle248!=emitter.vehicle248()||
       state.collisionCount||state.driveCount||state.blocked||state.inDispatch||
       !collision||collision>state.collision.size()||!drive||drive>state.drive.size()||
       count>p::originalMaterialRowsPerRacer50HD||emitter.poolOrdinal()>=Pool::count||
       !groups.uses(mixer))return reject("actual fresh D44 material-array ownership rejected");
    const auto first=p::originalMaterialFirstVoice50HD+
        emitter.poolOrdinal()*p::originalMaterialRowsPerRacer50HD;
    if(first>mixer.count()||count>mixer.count()-first)
        return reject("actual D44 exclusive logical-sound range is unavailable");
    const auto* group44=groups.sourceState44(g::RaceAudioGroup50IC::Effects110);
    if(!group44)return reject("actual shared Effects110 state44 is unbound");
    for(std::size_t i=0;i<count;++i){
        const bool isDrive=i>=collision;const auto key=isDrive?i-collision:i;
        const auto& name=isDrive?catalog.drive[key]:catalog.collision[key];
        const auto section=isDrive?catalog.driveSectionKeys[key]:catalog.collisionSectionKeys[key];
        const auto* clip=bank.find(name);
        // Constructor-null append compaction needs separate source handling.
        // Admit only the real all-ready native factory path, never keep a
        // fabricated unavailable placeholder in an allegedly constructed row.
        if(name.empty()||!section||!clip||clip->state!=a::AudioAssetState50HC::ready||
           clip->pcm.channels!=1||clip->pcm.sourceSha256.empty()||
           !groups.canBind(first+i,g::RaceAudioGroup50IC::Effects110))
            return reject("actual authored D44 sound row/PCM/group unavailable");
        const auto& pcm=clip->pcm;
        const p::AudioClip50HD view{pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,
            pcm.hasEmbeddedLoop?pcm.loopStart:0,pcm.hasEmbeddedLoop?pcm.loopEnd:0};
        if(!p::AudioMixer50HD::validClip(view))return reject("actual D44 PCM row is invalid");
    }
    if(!channel.bind(mixer,first,count)||
       !groups.bindRange(first,count,g::RaceAudioGroup50IC::Effects110))
        return reject("actual D44 logical-sound transport/group binding rejected");
    //279C80 collision1..N precedes279F60 drive1..M. Native allocation and PCM
    // registration are real completed operations; no source sample ID is
    // fabricated. Source233E90/26C240 base-pitch metadata remains unknown.
    for(std::size_t i=0;i<count;++i){
        const bool isDrive=i>=collision;const auto key=isDrive?i-collision:i;
        const auto* clip=bank.find(isDrive?catalog.drive[key]:catalog.collision[key]);
        const auto& pcm=clip->pcm;
        const p::AudioClip50HD view{pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,
            pcm.hasEmbeddedLoop?pcm.loopStart:0,pcm.hasEmbeddedLoop?pcm.loopEnd:0};
        if(!channel.registerClip(i,view,true))return reject("actual D44 logical sound registration rejected");
        auto* sound=channel.sound(i);
        if(!sound)return reject("actual D44 registered logical sound is absent");
        //292AF0 flags1;292CB0 flags81. Source232DB0 maps bit80 to loop1B0.
        //All other logical numeric fields retain the actual232ED0 constructor.
        sound->loop1B0=isDrive?1u:0u;
        auto& row=isDrive?state.drive[key].sound:state.collision[key];
        row.nativeOwner=&channel;row.nativeSound=sound;
        row.authoredKey4=static_cast<std::uint32_t>(key+1u);
        row.basePitch50=4096;row.basePitchKnown=false;
        row.soundInhibited80=false;row.groupState44=group44;
        if(isDrive){auto& target=state.drive[key];const auto& input=catalog.parameters.drive[key];
            target.sampleSpeed8=input.sampleSpeed;target.minVolumeC=input.minVolume;
            target.minSpeed10=input.minSpeed;target.maxSpeed14=input.maxSpeed;
            target.resonant18=input.resonantSpeed;target.slope1C=input.frequencySlope;
            target.cachedSlope20=-1;++state.driveCount;
        }else ++state.collisionCount;
    }
    error.clear();return true;
}
} // namespace native_original_ai_material
