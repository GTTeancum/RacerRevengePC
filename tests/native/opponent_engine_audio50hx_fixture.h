#pragma once
// Cold original-PHY/sample owner + bounded HOST opponent engine/boost subscriber.
// Install under tests/native. This adds no original D40 object or opponent AI.
#include "pod_materials50ed_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "gameplay_audio_provider50he_fixture.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../src/game/gameplay_audio50he.h"
#include "../../src/game/host_race_limits50ht.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "../../platform/linux/native/logical_audio_owner.h"
#include <array>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace opponent_audio50HX {
inline constexpr std::size_t maxOpponents=rr::game::maxHostRaceOpponents50HT;
inline constexpr std::size_t firstVoice=96,voicesPerOpponent=2;
inline constexpr std::size_t voiceCount=maxOpponents*voicesPerOpponent;
inline constexpr std::size_t boostFirstVoice=firstVoice+voiceCount,boostVoiceCount=maxOpponents;
inline constexpr std::size_t ownedVoiceCount=voiceCount+boostVoiceCount;
static_assert(maxOpponents==7);
static_assert(firstVoice+ownedVoiceCount<=rr::platform::audioMaximumVoices50HD);

// Trivial per-racer emitter state belongs in the native snapshot bank. Retain
// the most recent genuine onset until the accepted world subscriber observes
// its serial; repeated observations never manufacture a second start.
struct BoostState {
    rr::game::PodBoostAudioState50HE source{};
    rr::game::AudioCommand50HE command{};
    std::uint64_t serial=0;
};
inline bool boostAtBoundary(BoostState& state,bool held,float heating,
    std::uint32_t previous64)noexcept {
    auto next=state.source;rr::game::AudioCommand50HE command{};std::size_t count=0;
    if(rr::game::updatePodBoostAudio50HE(held,heating,previous64,state.source,next,
        &command,1,count)!=rr::game::AudioEventStatus50HE::Complete||
        (count&&state.serial==std::numeric_limits<std::uint64_t>::max()))return false;
    state.source=next;
    if(count){state.command=command;++state.serial;}
    return true;
}

// Borrow only during prepare(); bytes are the already-retained original PHY,
// not a generated config. Exact per-pod hashes below authenticate the mapping.
struct Source {std::string_view podId;const std::vector<std::uint8_t>* phy=nullptr;};
struct Mapping {
    std::string podId,engineName,binderName,phySha256;
    rr::game::PodEngineParameters50HE parameters{};
};
struct Position {float x=0.f,y=0.f,z=0.f;};
struct Input {
    bool active=false; // Actual committed, live opponent, including after finish.
    Position position{};
    float speedRatio70=0.f;
    // Damage modulation remains an explicit baseline; boost is source-owned.
    float enginePitchScale484=1.f;
    bool boostPitch=false;
    rr::game::AudioCommand50HE boostCommand{};
    std::uint64_t boostSerial=0;
};
// These are HOST audio presentation choices, not recovered PS2 distance words.
// Distances are source world units. The near/far transition is smoothstep.
// The sum of all opponent voice gains is capped without touching player gain.
struct DistancePolicy {
    float fullGainDistance=35.f,silentDistance=600.f;
    float opponentGain=.65f,totalGainCeiling=.45f;
};
enum class Status:std::uint8_t {Complete,Unprepared,Input,ApplyRejected};
enum class Result:std::uint8_t {Inactive,Started,Controlled,Stopped,Unavailable,Rejected};
struct Receipt {
    Status status=Status::Unprepared;
    std::array<Result,ownedVoiceCount> results{};
    std::array<float,maxOpponents> distanceGain{};
    float normalization=1.f;
    bool okay()const noexcept{return status==Status::Complete;}
};
struct Counters {
    std::uint64_t updates=0,starts=0,controls=0,stops=0,unavailable=0;
    std::uint64_t silentVoices=0,normalizedUpdates=0,rejected=0,resets=0;
    std::uint64_t boostOnsets=0,boostStarts=0,boostUnavailable=0;
};
namespace detail {
inline std::string_view trim(std::string_view s)noexcept {
    const auto first=s.find_first_not_of(" \t\r");
    if(first==s.npos)return {};
    return s.substr(first,s.find_last_not_of(" \t\r")-first+1);
}
inline float number(std::string_view s) {
    float value=0.f;
    const auto parsed=std::from_chars(s.data(),s.data()+s.size(),value);
    pod_materials50ed_fixture::detail::need(parsed.ec==std::errc{}&&
        parsed.ptr==s.data()+s.size()&&std::isfinite(value)&&value>=0.f,
        "50HX malformed original Pod Audio scalar");
    return value;
}
inline Mapping readMapping(const Source& source) {
    namespace d=pod_materials50ed_fixture::detail;
    const auto* identity=original_player_identity50IK::find(source.podId);
    d::need(identity&&source.phy&&!source.phy->empty()&&source.phy->size()<=8192,
        "50HX original opponent PHY mapping unavailable");
    Mapping out;out.podId=source.podId;
    out.phySha256=d::sha256(*source.phy);
    d::need(out.phySha256==identity->phySha256,"50HX original opponent PHY identity differs");
    const std::string_view text(reinterpret_cast<const char*>(source.phy->data()),source.phy->size());
    constexpr std::string_view keys[8]={"EngineAudioFile","BeamAudioFile","IdlePitchModifier",
        "PitchRangeModifier","IdleBinderModifier","BinderRangeModifier","EngineVolume","BinderVolume"};
    std::array<bool,8> seen{};bool section=false,foundSection=false;
    for(std::size_t at=0;at<text.size();) {
        auto end=text.find('\n',at);if(end==text.npos)end=text.size();
        const auto line=trim(text.substr(at,end-at));at=end+1;
        if(line.empty()||line.front()==';')continue;
        if(line.front()=='[') {
            section=line=="[Pod Audio]";
            if(section){d::need(!foundSection,"50HX duplicate Pod Audio section");foundSection=true;}
            continue;
        }
        if(!section)continue;
        const auto equals=line.find('=');
        d::need(equals!=line.npos,"50HX malformed Pod Audio field");
        const auto key=trim(line.substr(0,equals)),value=trim(line.substr(equals+1));
        std::size_t index=0;while(index<seen.size()&&keys[index]!=key)++index;
        d::need(index<seen.size()&&!seen[index]&&!value.empty(),"50HX unsupported/repeated Pod Audio field");
        seen[index]=true;
        if(index==0)out.engineName=value;
        else if(index==1)out.binderName=value;
        else {
            const float scalar=number(value);
            switch(index) {
                case 2:out.parameters.idlePitch=scalar;break;
                case 3:out.parameters.pitchRange=scalar;break;
                case 4:out.parameters.idleBinder=scalar;break;
                case 5:out.parameters.binderRange=scalar;break;
                case 6:out.parameters.engineVolume=scalar;break;
                case 7:out.parameters.binderVolume=scalar;break;
                default:break;
            }
        }
    }
    d::need(foundSection,"50HX original Pod Audio section missing");
    for(bool present:seen)d::need(present,"50HX original Pod Audio field missing");
    // 4096 is the existing native provider's normalized pitch representation.
    out.parameters.engineBasePitch=out.parameters.binderBasePitch=4096;
    return out;
}
inline bool finite(const Position& p)noexcept {
    return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);
}
inline bool valid(const DistancePolicy& p)noexcept {
    return std::isfinite(p.fullGainDistance)&&p.fullGainDistance>=0.f&&
        std::isfinite(p.silentDistance)&&p.silentDistance>p.fullGainDistance&&
        std::isfinite(p.opponentGain)&&p.opponentGain>=0.f&&p.opponentGain<=1.f&&
        std::isfinite(p.totalGainCeiling)&&p.totalGainCeiling>=0.f&&p.totalGainCeiling<=1.f;
}
inline float distanceGain(const Position& listener,const Position& source,
    const DistancePolicy& policy)noexcept {
    // Double differences keep finite float positions from overflowing the norm.
    const double x=double(source.x)-listener.x,y=double(source.y)-listener.y,z=double(source.z)-listener.z;
    const double distance=std::sqrt(x*x+y*y+z*z);
    if(distance<=policy.fullGainDistance)return 1.f;
    if(distance>=policy.silentDistance)return 0.f;
    const double t=(distance-policy.fullGainDistance)/
        (double(policy.silentDistance)-policy.fullGainDistance);
    return static_cast<float>((1.0-t)*(1.0-t)*(1.0+2.0*t));
}
inline bool validControls(float gain,float pitch)noexcept {
    return std::isfinite(gain)&&gain>=0.f&&gain<=8.f&&
        std::isfinite(pitch)&&pitch>=.125f&&pitch<=8.f;
}
}

// Own at a fixed address after the live-audio owner, and destroy before its
// mixer. The shared global boost PCM belongs to the live audio bank, which
// must also outlive this owner. PCM vectors never move/resize after preparation.
// All methods are on the simulation thread; neither this owner nor its source
// files reach SDL's callback. No hot allocation, decoding or shared RNG use.
class Owner final {
public:
    Owner()=default;~Owner()noexcept{close();}
    Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;

    bool prepare(const std::array<Source,maxOpponents>& sources,
        const std::filesystem::path& audioRoot,const rr::assets::AudioBank50HC& sharedEvents,
        rr::platform::AudioMixer50HD& mixer,
        std::string& error,const DistancePolicy& policy={}) {
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HX preparation requires a fresh stable owner";return false;}
        attempted_=true;
        try {
            d::need(detail::valid(policy),"50HX invalid host distance policy");
            d::need(mixer.count()>=firstVoice+ownedVoiceCount,"50HX exclusive voices96..116 unavailable");
            for(std::size_t v=0;v<ownedVoiceCount;++v)
                d::need(!mixer.active(firstVoice+v)&&!mixer.hasVoiceGroupGain50IC(firstVoice+v),
                    "50HX reserved opponent voice already occupied or bound");
            std::vector<a::AudioRequest50HC> requests;requests.reserve(voiceCount);
            for(std::size_t i=0;i<maxOpponents;++i) {
                mappings_[i]=detail::readMapping(sources[i]);
                for(std::size_t j=0;j<i;++j)d::need(mappings_[i].podId!=mappings_[j].podId,
                    "50HX duplicate original opponent mapping");
                for(const auto* name:{&mappings_[i].engineName,&mappings_[i].binderName}) {
                    bool exists=false;for(const auto& request:requests)if(request.name==*name)exists=true;
                    // The exact PHY declares filenames, not IsLoop/IsStream.
                    // Continuous engine/binder looping is explicit runtime policy.
                    if(!exists)requests.push_back({*name,false,false,false});
                }
            }
            if(!a::loadAudioBank50HC(audioRoot,requests,bank_,error))return false;
            for(std::size_t i=0;i<maxOpponents;++i) {
                for(std::size_t part=0;part<voicesPerOpponent;++part) {
                    const auto* asset=bank_.find(part?mappings_[i].binderName:mappings_[i].engineName);
                    d::need(asset,"50HX authored sample record missing from prepared bank");
                    auto& sound=sounds_[i*voicesPerOpponent+part];sound.availability=asset->state;
                    if(asset->state==a::AudioAssetState50HC::ready) {
                        const auto& pcm=asset->pcm;
                        sound.clip={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,
                            pcm.hasEmbeddedLoop?pcm.loopStart:0,pcm.hasEmbeddedLoop?pcm.loopEnd:0};
                        d::need(pcm.channels==1&&pcm.frames==pcm.samples.size()&&
                            rr::platform::AudioMixer50HD::validClip(sound.clip),"50HX ready original PCM invalid");
                    }
                }
            }
            // PodEngine3 is a verified global executable literal, not a POD01
            // character row. The selected-player loader authenticates it for
            // every selection. Borrow that exact immutable PCM, never choose
            // an opponent sample by display name or substitute boostmax.
            const auto* boost=sharedEvents.find(gameplay_audio50HE::cueFilename(
                {rr::game::AudioBank50HE::PodEngine,3}));
            d::need(boost&&!boost->request.loop&&!boost->request.stereo&&!boost->request.stream,
                "50HX authenticated global boost record unavailable");
            Sound boostSound;boostSound.availability=boost->state;
            if(boost->state==a::AudioAssetState50HC::ready){
                const auto& pcm=boost->pcm;
                boostSound.clip={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,
                    pcm.hasEmbeddedLoop?pcm.loopStart:0,pcm.hasEmbeddedLoop?pcm.loopEnd:0};
                d::need(pcm.channels==1&&pcm.frames==pcm.samples.size()&&
                    rr::platform::AudioMixer50HD::validClip(boostSound.clip),"50HX ready global boost PCM invalid");
            }
            for(std::size_t i=0;i<maxOpponents;++i)sounds_[voiceCount+i]=boostSound;
            d::need(channels_.bind(mixer,firstVoice,ownedVoiceCount),
                "AI native logical channel range unavailable");
            for(std::size_t i=0;i<ownedVoiceCount;++i)
                d::need(channels_.registerClip(i,sounds_[i].clip,
                    sounds_[i].availability==a::AudioAssetState50HC::ready),
                    "AI native logical sound registration rejected");
            policy_=policy;mixer_=&mixer;error.clear();return true;
        }catch(const std::exception& e){error=e.what();return false;}
        catch(...){error="50HX original opponent audio preparation failed";return false;}
    }

    // Once at the accepted world boundary, after all contacts/race observations
    // and before the existing ONE committedStep mix. Never in the callback,
    // render loop, paused path, abandoned frame, or individual opponent step.
    // Boost release changes only the source pitch flag. A playing onset keeps
    // its natural one-shot tail, distance controls and shared opponent ceiling.
    Receipt update(const Position& listener,const std::array<Input,maxOpponents>& inputs)noexcept {
        Receipt receipt;if(!mixer_)return receipt;
        struct Planned {bool active=false;float gain=0.f,pitch=1.f;bool restart=false,loop=true;};
        std::array<Planned,ownedVoiceCount> plan{};double totalGain=0.0;
        auto reject=[&]()noexcept{receipt.status=Status::Input;++counters_.rejected;return receipt;};
        if(!detail::finite(listener))return reject();
        for(std::size_t i=0;i<maxOpponents;++i) {
            const auto& input=inputs[i];if(!input.active)continue;
            if(!detail::finite(input.position))return reject();
            rr::game::PodEngineControls50HE controls{};
            const rr::game::PodEngineInput50HE engineInput{
                input.speedRatio70,input.enginePitchScale484,input.boostPitch};
            if(!rr::game::podEngineControls50HE(mappings_[i].parameters,engineInput,controls))return reject();
            const float attenuate=detail::distanceGain(listener,input.position,policy_);
            receipt.distanceGain[i]=attenuate;
            const float gain[2]={controls.engineGain,controls.binderGain};
            const float pitch[2]={controls.enginePitch,controls.binderPitch};
            for(std::size_t part=0;part<voicesPerOpponent;++part) {
                const auto index=i*voicesPerOpponent+part;
                const float outputGain=gain[part]*attenuate*policy_.opponentGain;
                if(!detail::validControls(outputGain,pitch[part]))return reject();
                plan[index]={true,outputGain,pitch[part]};
                if(sounds_[index].availability==rr::assets::AudioAssetState50HC::ready)totalGain+=outputGain;
            }
            if(input.boostSerial<boostSerials_[i])return reject();
            const bool onset=input.boostSerial>boostSerials_[i];
            if(onset&&input.boostSerial-boostSerials_[i]!=1u)return reject();
            const auto index=voiceCount+i;
            if(onset||channels_.active(*channels_.sound(index))){
                const auto& command=onset?input.boostCommand:boostCommands_[i];
                if(command.operation!=rr::game::AudioOperation50HE::Start||
                    command.cue.bank!=rr::game::AudioBank50HE::PodEngine||command.cue.key!=3||
                    command.loop||!detail::validControls(command.gain,command.pitch))return reject();
                const float outputGain=command.gain*attenuate*policy_.opponentGain;
                if(!detail::validControls(outputGain,command.pitch))return reject();
                plan[index]={true,outputGain,command.pitch,onset,false};
                if(sounds_[index].availability==rr::assets::AudioAssetState50HC::ready)totalGain+=outputGain;
            }
        }
        if(totalGain>policy_.totalGainCeiling) {
            receipt.normalization=static_cast<float>(double(policy_.totalGainCeiling)/totalGain);
            // Keep the ceiling conservative after rounding the shared scale.
            if(double(receipt.normalization)*totalGain>policy_.totalGainCeiling)
                receipt.normalization=std::nextafter(receipt.normalization,0.f);
        }
        // Every input/parameter/clip was admitted before any mixer mutation.
        // Mixer lifetime and exclusive range are a caller contract. No voice
        // stealing, master-gain changes, or player/provider operations occur.
        receipt.status=Status::Complete;++counters_.updates;
        if(receipt.normalization<1.f)++counters_.normalizedUpdates;
        for(std::size_t i=0;i<maxOpponents;++i)if(inputs[i].active&&inputs[i].boostSerial>boostSerials_[i]){
            boostSerials_[i]=inputs[i].boostSerial;boostCommands_[i]=inputs[i].boostCommand;
            ++counters_.boostOnsets;
        }
        for(std::size_t index=0;index<ownedVoiceCount;++index) {
            const auto& control=plan[index];auto& sound=*channels_.sound(index);
            auto& result=receipt.results[index];
            if(!control.active) {
                const bool active=channels_.active(sound);
                channels_.stop233670(sound);
                if(active){result=Result::Stopped;++counters_.stops;}
                continue;
            }
            if(sounds_[index].availability!=rr::assets::AudioAssetState50HC::ready) {
                // Missing/invalid/unsupported stay distinct in the availability accessors.
                result=Result::Unavailable;++counters_.unavailable;
                if(index>=voiceCount)++counters_.boostUnavailable;
                continue;
            }
            const float gain=control.gain*receipt.normalization;
            const bool wasActive=channels_.active(sound);
            const bool continuing=wasActive&&!control.restart;
            const auto applied=continuing?channels_.controls(sound,gain,control.pitch):
                channels_.start(sound,gain,control.pitch,control.loop);
            const bool accepted=applied==rr::platform::LogicalAudioOwner::Result::complete;
            if(!accepted){result=Result::Rejected;receipt.status=Status::ApplyRejected;++counters_.rejected;}
            else if(continuing){result=Result::Controlled;++counters_.controls;}
            else {result=Result::Started;++counters_.starts;if(index>=voiceCount)++counters_.boostStarts;}
            if(accepted&&gain==0.f)++counters_.silentVoices;
        }
        return receipt;
    }

    // Scope is ONLY96..116. Restart eligible continuous loops after a reset,
    // but retain consumed boost serials: only a new genuine onset can replay it.
    void reset()noexcept {
        if(!mixer_)return;
        for(std::size_t v=0;v<ownedVoiceCount;++v)channels_.stop233670(*channels_.sound(v));
        ++counters_.resets;
    }
    void close()noexcept{reset();channels_.close();mixer_=nullptr;}
    bool prepared()const noexcept{return mixer_!=nullptr;}
    const rr::assets::AudioBank50HC& bank()const noexcept{return bank_;}
    const std::array<Mapping,maxOpponents>& mappings()const noexcept{return mappings_;}
    const Counters& counters()const noexcept{return counters_;}
    const DistancePolicy& policy()const noexcept{return policy_;}
    rr::assets::AudioAssetState50HC boostAvailability()const noexcept{return sounds_[voiceCount].availability;}
    rr::assets::AudioAssetState50HC availability(std::size_t opponent,std::size_t part)const noexcept {
        if(opponent>=maxOpponents||part>=voicesPerOpponent)return rr::assets::AudioAssetState50HC::invalid;
        return sounds_[opponent*voicesPerOpponent+part].availability;
    }
private:
    struct Sound {
        rr::platform::AudioClip50HD clip{};
        rr::assets::AudioAssetState50HC availability=rr::assets::AudioAssetState50HC::missing;
    };
    rr::assets::AudioBank50HC bank_{};
    std::array<Mapping,maxOpponents> mappings_{};
    std::array<Sound,ownedVoiceCount> sounds_{};
    std::array<rr::game::AudioCommand50HE,maxOpponents> boostCommands_{};
    std::array<std::uint64_t,maxOpponents> boostSerials_{};
    rr::platform::AudioMixer50HD* mixer_=nullptr;
    rr::platform::LogicalAudioOwner channels_{};
    DistancePolicy policy_{};Counters counters_{};bool attempted_=false;
};
}
