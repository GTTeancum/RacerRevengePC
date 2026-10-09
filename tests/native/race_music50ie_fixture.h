#pragma once
// Native cold preload and actual shared-mixer transport for category6 music.
// This admits SIX authenticated original stereo files only. It does not widen
//50HC's generic mono admission, run the original IOP streamer, or invent music.
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../src/game/race_music_event50ie.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include <array>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

namespace race_music50IE {
inline constexpr std::size_t voice=117,clipCount=6;
inline constexpr std::size_t pcmByteCapacity=8u*1024u*1024u;
inline constexpr std::size_t sourceByteCapacity=8u*1024u*1024u;
struct Definition {
    std::uint32_t song;
    const char* name;
    const char* path;
    std::uint32_t bytes,payloadBytes,frames;
    const char* sha256;
};
inline constexpr std::array<Definition,clipCount> catalog{{
    {26,"Music\\Race\\Finish\\bad_fin.vag","MUSIC/RACE/FINISH/BAD_FIN.VAG",
        97184,47584,83244,"092f622aa2e170d109f36a09f9f23f823789437568d1da01aba374c0f7df7849"},
    {27,"Music\\Race\\Finish\\best_fin.vag","MUSIC/RACE/FINISH/BEST_FIN.VAG",
        235104,116544,203924,"b3691dedd139f37064e6287177b9f3d12ae748ece320b880c7b54c224a7d44bf"},
    {28,"Music\\Race\\Finish\\good_fin.vag","MUSIC/RACE/FINISH/GOOD_FIN.VAG",
        143104,70544,123424,"14b87f6f3dd89211d624367db0b6b1779fe71583234889932fa919b8c32ce804"},
    {29,"Music\\Race\\FinalLap\\fin_lap2.vag","MUSIC/RACE/FINALLAP/FIN_LAP2.VAG",
        713408,355696,622440,"4c0dc426d1cf0ffded7d324e6bb7955159f49efd8af995ec6f7c160f8abdf115"},
    {33,"Music\\Race\\FinalLap\\fin_lap3.vag","MUSIC/RACE/FINALLAP/FIN_LAP3.VAG",
        599072,298528,522396,"4dd28e6cb520f5a2e7c02ddd30ebedbc5236e176273d5fb30c36f17ab8df36d4"},
    {34,"Music\\Race\\FinalLap\\fin_lap1.vag","MUSIC/RACE/FINALLAP/FIN_LAP1.VAG",
        582304,290144,507724,"f13c9d1449d8e4bdf9cffdf8000a1fea070ccb1df9c08d79cdd0ae4b6740ca46"}
}};
namespace detail {
namespace d=pod_materials50ed_fixture::detail;
inline std::uint32_t be32(const std::uint8_t* p)noexcept {
    return (std::uint32_t(p[0])<<24)|(std::uint32_t(p[1])<<16)|
        (std::uint32_t(p[2])<<8)|p[3];
}
inline std::size_t index(std::uint32_t song)noexcept {
    for(std::size_t i=0;i<catalog.size();++i)if(catalog[i].song==song)return i;
    return catalog.size();
}
// Caller selects an explicit root. Every relative component must be a real
// directory/file, never a symlink. All names below are authenticated constants.
inline std::filesystem::path selectedPath(const std::filesystem::path& root,
    const char* relative) {
    d::need(!std::filesystem::is_symlink(root),"50IE root is a symlink");
    std::filesystem::path path=root;
    for(const auto& component:std::filesystem::path(relative)){
        d::need(component!=".."&&component!=".","50IE unsafe selected path");
        path/=component;
        d::need(!std::filesystem::is_symlink(path),"50IE selected component is a symlink");
    }
    return path;
}
inline void authenticate(d::Inputs& input,const std::filesystem::path& root,
    const char* relative,std::size_t bytes,const char* sha) {
    const auto data=input.read(selectedPath(root,relative),bytes);
    d::need(data.size()==bytes&&d::sha256(data)==sha,
        std::string("50IE original identity differs: ")+relative);
}
// The retained files have a zeroed0x7E0 prefix except BE length/rate at0x0C/10.
// Starting0x7E0, each32-byte pair is one16-byte ADPCM frame per channel.
// Their final data pair has flag1 in BOTH channels; exactly one original
//070077... padding pair follows. Header length counts that padding frame.
// Original byte identities are checked BEFORE interpreting this narrow layout.
inline rr::assets::AudioPcm50HC decodeSelected(const d::Bytes& bytes,
    const Definition& definition) {
    d::need(bytes.size()==definition.bytes&&d::sha256(bytes)==definition.sha256,
        "50IE selected original music hash/extent differs");
    d::need(bytes.size()==0x7e0u+2u*definition.payloadBytes&&
        definition.payloadBytes%16u==0&&definition.payloadBytes>=32,
        "50IE selected stereo extent differs");
    d::need(be32(bytes.data()+12)==definition.payloadBytes&&
        be32(bytes.data()+16)==22050u,"50IE selected length/rate differs");
    for(std::size_t i=0;i<0x7e0u;++i)
        if(i<12u||i>=20u)d::need(bytes[i]==0,"50IE selected prefix differs");
    const auto blocks=definition.payloadBytes/16u;
    for(std::size_t i=0;i<blocks;++i){
        const auto at=0x7e0u+i*32u;
        const auto flag=static_cast<std::uint8_t>(i+2u==blocks?1u:0u);
        d::need(bytes[at+1]==flag&&bytes[at+17]==flag,
            "50IE channel flags/end pair differ");
    }
    for(std::size_t c=0;c<2;++c){
        const auto at=bytes.size()-32u+c*16u;
        d::need(bytes[at]==7&&bytes[at+1]==0,"50IE trailing pad header differs");
        for(std::size_t n=2;n<16;++n)d::need(bytes[at+n]==0x77,"50IE trailing pad differs");
    }
    // Reframe each ORIGINAL channel byte stream into the already-supported
    // native50HC zero-header mono layout. No sample or replacement is invented.
    std::array<rr::assets::AudioPcm50HC,2> channels;
    d::Bytes mono(48u+definition.payloadBytes,0);
    std::copy(bytes.begin(),bytes.begin()+48,mono.begin());
    for(std::size_t c=0;c<2;++c){
        for(std::size_t i=0;i<blocks;++i)
            std::memcpy(mono.data()+48u+i*16u,bytes.data()+0x7e0u+i*32u+c*16u,16);
        std::string error;
        d::need(rr::assets::decodeVagMono50HC(mono.data(),mono.size(),channels[c],error),
            std::string("50IE original channel decode rejected: ")+error);
        d::need(channels[c].channels==1&&channels[c].sampleRate==22050u&&
            channels[c].frames==definition.frames&&!channels[c].hasEmbeddedLoop,
            "50IE decoded channel framing/rate/end differs");
    }
    rr::assets::AudioPcm50HC out;
    out.sampleRate=22050;out.channels=2;out.frames=definition.frames;
    out.sourceSha256=definition.sha256;out.samples.resize(std::size_t(out.frames)*2u);
    for(std::size_t n=0;n<definition.frames;++n){
        out.samples[n*2u]=channels[0].samples[n];out.samples[n*2u+1u]=channels[1].samples[n];
    }
    return out;
}
} // namespace detail

enum class Result : std::uint8_t {
    Started,Suppressed,Unavailable,Unprepared,Paused,UnsupportedEvent,Input,Ownership,BackendRejected
};
struct Receipt {
    Result result=Result::Unprepared;
    std::uint32_t song=0,sourcePlayPC=0,originalV0=0;
    bool stoppedPrevious=false;
    // A real mixer start or the recovered source admission return completed.
    // Unavailable/backend errors never acknowledge the category6 FIFO ticket.
    bool completed()const noexcept{return result==Result::Started||result==Result::Suppressed;}
};
struct Counters {
    std::uint64_t requests=0,starts=0,loopStarts=0,oneShotStarts=0,suppressed=0;
    std::uint64_t unavailable=0,rejected=0,stoppedPrevious=0,completions=0,resets=0;
};
// Own at a stable address. The shared mixer/groups must outlive this owner.
//117 is exclusive; no second music writer is admitted. No hot file access,
//decode, allocation, wall timer, extra RNG, guessed voice, or Dax operation.
class Owner final {
public:
    Owner()=default;~Owner()noexcept{reset();}
    Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
    bool prepare(const std::filesystem::path& discRoot,const std::filesystem::path& audioRoot,
        rr::platform::AudioMixer50HD& mixer,rr::platform::RaceAudioGroups50IC& groups,
        std::string& error) {
        namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50IE preparation requires a fresh stable owner";return false;}
        attempted_=true;
        try {
            d::need(mixer.count()>voice&&groups.uses(mixer)&&!mixer.active(voice)&&
                groups.binding(voice)==rr::game::RaceAudioGroup50IC::Unrepresented&&
                !mixer.hasVoiceGroupGain50IC(voice),"50IE exclusive music slot117 unavailable");
            d::Inputs input;input.capacity=sourceByteCapacity;
            detail::authenticate(input,discRoot,"SLUS_202.68",2972720,
                "c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30");
            detail::authenticate(input,audioRoot,"RACEMUS.JUK",5952,
                "676a9f212a588f0c489f92e46b0c928abee578b10f93f54c5a88068a985d5ec5");
            detail::authenticate(input,audioRoot,"RACEMUS.JUB",14043,
                "bff85d5a51a94157669be85bb90bb4d6be8bf4d802d95d08b1d2b235e9483bf1");
            bank_.clips.resize(clipCount);std::size_t pcmBytes=0;
            for(std::size_t i=0;i<clipCount;++i){
                const auto& definition=catalog[i];auto& clip=bank_.clips[i];
                clip.request={definition.name,false,true,true};
                const auto path=detail::selectedPath(audioRoot,definition.path);
                clip.sourcePath=path.string();
                if(!std::filesystem::exists(path)){
                    clip.state=rr::assets::AudioAssetState50HC::missing;
                    clip.detail="50IE exact original music sample missing";++bank_.missing;continue;
                }
                try {
                    const auto bytes=input.read(path,definition.bytes);
                    clip.sourceBytes=bytes.size();clip.sourceSha256=d::sha256(bytes);
                    clip.pcm=detail::decodeSelected(bytes,definition);
                    d::add(pcmBytes,clip.pcm.samples.size()*sizeof(std::int16_t),
                        pcmByteCapacity,"50IE retained PCM capacity exceeded");
                    const auto& p=clip.pcm;
                    // All loops are SOURCE transport loops, not embedded flags.
                    // Native full-clip bounds are [0,frames), in source frames.
                    clips_[i]={p.samples.data(),p.frames,p.sampleRate,p.channels,0,0};
                    d::need(rr::platform::AudioMixer50HD::validClip(clips_[i]),"50IE invalid ready PCM");
                    clip.state=rr::assets::AudioAssetState50HC::ready;
                    clip.detail="Authenticated original stereo; native cold preload";++bank_.ready;
                }catch(const std::exception& e){
                    clip.state=rr::assets::AudioAssetState50HC::invalid;clip.detail=e.what();
                    clip.pcm={};clips_[i]={};++bank_.invalid;
                }
            }
            //1A3C50..60 supplies manager group's+118 to2C0980's music ctor.
            d::need(groups.bind(voice,rr::game::RaceAudioGroup50IC::Music118),
                "50IE source Music118 binding rejected");
            mixer_=&mixer;groups_=&groups;pcmBytes_=pcmBytes;error.clear();return true;
        }catch(const std::exception& e){error=e.what();return false;}
        catch(...){error="50IE music cold preparation failed";return false;}
    }
    Receipt request(std::uint32_t song,std::uint32_t argumentA2,
        std::uint32_t liveMode11258)noexcept {
        Receipt r;r.song=song;++counters_.requests;
        if(!mixer_)return rejected(r,Result::Unprepared);
        if(paused_)return rejected(r,Result::Paused);
        if(!pollCompletion())return rejected(r,Result::Ownership);
        const auto plan=rr::game::planCategory6Music50IE(state_,liveMode11258,song,argumentA2);
        if(plan.status==rr::game::RaceMusicPlanStatus50IE::Suppressed){
            r.result=Result::Suppressed;++counters_.suppressed;return r;
        }
        if(plan.status==rr::game::RaceMusicPlanStatus50IE::UnsupportedEvent)
            return rejected(r,Result::UnsupportedEvent);
        if(plan.status!=rr::game::RaceMusicPlanStatus50IE::Play)return rejected(r,Result::Input);
        const auto i=detail::index(song);
        if(i==clipCount||bank_.clips[i].state!=rr::assets::AudioAssetState50HC::ready){
            r.result=Result::Unavailable;++counters_.unavailable;return r;
        }
        // Preflight first. Blocked/missing PCM leaves the original request and
        // current music untouched. The stop is real, ordered before replacement.
        if(!rr::platform::AudioMixer50HD::validClip(clips_[i]))
            return rejected(r,Result::BackendRejected);
        r.stoppedPrevious=owned_&&mixer_->active(voice);
        rr::game::beginCategory6MusicPlay50IE(plan,state_);
        mixer_->stop(voice);owned_=false;
        rr::game::finishCategory6MusicStop50IE(plan,state_);
        // Native transport: natural22050Hz PCM through the48000Hz mixer,
        // unity per-voice gain/pitch and one independent source Music118 gain.
        // RACEMUS Pitch1881 is original SPU units, NEVER a native ratio.
        if(!mixer_->play(voice,clips_[i],1.0f,1.0f,plan.loop,0.0f)){
            rr::game::observeCategory6MusicActivity50IE(false,state_);
            return rejected(r,Result::BackendRejected);
        }
        owned_=true;currentSong_=song;r.result=Result::Started;
        r.sourcePlayPC=plan.playPC;r.originalV0=1;++counters_.starts;
        if(plan.loop)++counters_.loopStarts;else ++counters_.oneShotStarts;
        if(r.stoppedPrevious)++counters_.stoppedPrevious;
        return r;
    }
    // Call after a real shared-mixer committed step, also before event dispatch.
    // Completion follows actual source-frame consumption, not a guessed timer.
    bool pollCompletion()noexcept {
        if(!current())return false;
        const bool active=mixer_->active(voice);
        if(!owned_&&active)return false; // Another writer violated117 ownership.
        if(owned_&&!active){owned_=false;currentSong_=0;++counters_.completions;}
        rr::game::observeCategory6MusicActivity50IE(active,state_);return true;
    }
    void pause(bool value)noexcept{paused_=value;}
    // Host race reset/close. No cumulative audio clock or other voice changes.
    void reset()noexcept {
        if(mixer_&&current()&&owned_)mixer_->stop(voice);
        owned_=false;currentSong_=0;state_={};paused_=false;++counters_.resets;
    }
    bool prepared()const noexcept{return mixer_!=nullptr;}
    bool current()const noexcept {
        return mixer_&&groups_&&groups_->uses(*mixer_)&&mixer_->count()>voice&&
            groups_->binding(voice)==rr::game::RaceAudioGroup50IC::Music118;
    }
    bool active()const noexcept{return current()&&owned_&&mixer_->active(voice);}
    std::uint32_t currentSong()const noexcept{return currentSong_;}
    const rr::game::RaceMusicState50IE& state()const noexcept{return state_;}
    const rr::assets::AudioBank50HC& bank()const noexcept{return bank_;}
    const Counters& counters()const noexcept{return counters_;}
    std::size_t pcmBytes()const noexcept{return pcmBytes_;}
private:
    Receipt rejected(Receipt r,Result result)noexcept {
        r.result=result;++counters_.rejected;return r;
    }
    rr::assets::AudioBank50HC bank_{};
    std::array<rr::platform::AudioClip50HD,clipCount> clips_{};
    rr::platform::AudioMixer50HD* mixer_=nullptr;
    rr::platform::RaceAudioGroups50IC* groups_=nullptr;
    rr::game::RaceMusicState50IE state_{};
    Counters counters_{};
    std::size_t pcmBytes_=0;
    std::uint32_t currentSong_=0;
    bool attempted_=false,paused_=false,owned_=false;
};
} // namespace race_music50IE
