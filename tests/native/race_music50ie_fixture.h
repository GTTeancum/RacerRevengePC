#pragma once
// Native cold preload/shared-mixer transport for category6 and collision music.
// Default preparation admits six originals; explicit50IW preparation adds six. It does not widen
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
inline constexpr std::size_t voice=117,clipCount=6,contactClipCount=6,totalClipCount=12;
inline constexpr std::size_t pcmByteCapacity=24u*1024u*1024u;
inline constexpr std::size_t sourceByteCapacity=16u*1024u*1024u;
struct Definition {
    std::uint32_t song;
    const char* name;
    const char* path;
    std::uint32_t bytes,payloadBytes,frames;
    const char* sha256;
};
inline constexpr std::array<Definition,totalClipCount> catalog{{
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
        582304,290144,507724,"f13c9d1449d8e4bdf9cffdf8000a1fea070ccb1df9c08d79cdd0ae4b6740ca46"},
    {38,"Music\\Race\\Combat\\combat01.vag","MUSIC/RACE/COMBAT/COMBAT01.VAG",
        515840,256912,449568,"57d1c9d6115949fad1b008039b63bb46435cb8827993332fdec6839b806d49d0"},
    {39,"Music\\Race\\Combat\\combat05.vag","MUSIC/RACE/COMBAT/COMBAT05.VAG",
        473120,235552,412188,"2f85f312bcefae061c081d19ef5f69ce877703e27ba32c75b5608a8f2e2796df"},
    {40,"Music\\Race\\Combat\\combat04.vag","MUSIC/RACE/COMBAT/COMBAT04.VAG",
        684416,341200,597072,"591679d0bd23daab7f5abf66c880ad4e256318d35d16cef14400627186a7e151"},
    {41,"Music\\Race\\Combat\\combat03.vag","MUSIC/RACE/COMBAT/COMBAT03.VAG",
        886432,442208,773836,"af5ab2479efc36b1d599331087456da019c5a11693395daf29a485ae765c8b12"},
    {42,"Music\\Race\\Combat\\combat02.vag","MUSIC/RACE/COMBAT/COMBAT02.VAG",
        479328,238656,417620,"50b48c1f1381331ecee0975f00eed8918f3bb1a2bbaed4e639363a05a90c6ea2"},
    {43,"Music\\Race\\Combat\\combat06.vag","MUSIC/RACE/COMBAT/COMBAT06.VAG",
        490112,244048,427056,"e0b49fa206c62f51620d28f9193664fc923875ea2f1967315c96cc15e3ce7b64"}
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
    Started,Suppressed,Unavailable,Unprepared,Paused,UnsupportedEvent,Input,Ownership,BackendRejected,
    Refreshed,PendingFade234440,PendingPlay,Updated
};
struct Receipt {
    Result result=Result::Unprepared;
    std::uint32_t song=0,sourcePlayPC=0,originalV0=0;
    bool stoppedPrevious=false;
    // A real mixer start or the recovered source admission return completed.
    // Unavailable/backend errors never acknowledge the category6 FIFO ticket.
    bool completed()const noexcept{return result==Result::Started||result==Result::Refreshed||result==Result::Suppressed;}
};
struct UpdateReceipt50IW {
    Result result=Result::Unprepared;
    std::uint32_t sourcePC=0,helperPC=0;
    float fadeSeconds=0.f;
    bool completed()const noexcept{return result==Result::Updated;}
};
struct Counters {
    std::uint64_t requests=0,starts=0,loopStarts=0,oneShotStarts=0,suppressed=0;
    std::uint64_t unavailable=0,rejected=0,stoppedPrevious=0,completions=0,resets=0;
    std::uint64_t contactRefreshes=0,fadeRequests=0;
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
        std::string& error,bool includeContactMusic=false) {
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
            const auto selectedCount=includeContactMusic?totalClipCount:clipCount;
            bank_.clips.resize(selectedCount);std::size_t pcmBytes=0;
            for(std::size_t i=0;i<selectedCount;++i){
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
            mixer_=&mixer;groups_=&groups;pcmBytes_=pcmBytes;
            contactMusicEnabled_=includeContactMusic;error.clear();return true;
        }catch(const std::exception& e){error=e.what();return false;}
        catch(...){error="50IE music cold preparation failed";return false;}
    }
    Receipt request(std::uint32_t song,std::uint32_t argumentA2,
        std::uint32_t liveMode11258)noexcept {
        Receipt r;r.song=song;++counters_.requests;
        if(!mixer_)return rejected(r,Result::Unprepared);
        if(paused_)return rejected(r,Result::Paused);
        if(fadePending_)return rejected(r,Result::PendingFade234440);
        if(playPending_){
            if(song!=pendingSong_||argumentA2)return rejected(r,Result::Ownership);
            return resumePlay(r);
        }
        const bool contact=rr::game::collisionMusicSong50IW(song);
        if(contact&&!contactMusicEnabled_)return rejected(r,Result::UnsupportedEvent);
        //1A46E0 reads existing mode/timer directly. Do not synthesize an
        //activity update before collision admission or the timer refresh.
        if(contact){
            if(!current()||(!owned_&&mixer_->active(voice)))return rejected(r,Result::Ownership);
        }else if(!pollCompletion())return rejected(r,Result::Ownership);
        const auto plan=contact?rr::game::planCollisionMusic50IW(state_,liveMode11258,song,argumentA2):
            rr::game::planCategory6Music50IE(state_,liveMode11258,song,argumentA2);
        if(plan.status==rr::game::RaceMusicPlanStatus50IE::Suppressed){
            r.result=Result::Suppressed;++counters_.suppressed;return r;
        }
        if(plan.status==rr::game::RaceMusicPlanStatus50IE::Refresh){
            state_.timer128=7.f;r.result=Result::Refreshed;r.originalV0=1;
            ++counters_.contactRefreshes;return r;
        }
        if(plan.status==rr::game::RaceMusicPlanStatus50IE::UnsupportedEvent)
            return rejected(r,Result::UnsupportedEvent);
        if(plan.status!=rr::game::RaceMusicPlanStatus50IE::Play)return rejected(r,Result::Input);
        const auto i=detail::index(song);
        if(i>=bank_.clips.size()||bank_.clips[i].state!=rr::assets::AudioAssetState50HC::ready){
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
        // Retain the exact pending start after committed timer/stop/mode
        //writes. A backend rejection must not turn a retry into a timer-only
        //Refresh and falsely acknowledge a song which never started.
        playPending_=true;pendingSong_=song;pendingLoop_=plan.loop;
        pendingPlayPC_=plan.playPC;pendingStoppedPrevious_=r.stoppedPrevious;
        pendingCallPC_=contact?0x1a47fcu:(plan.loop?0x1a482cu:0x1a48ccu);
        return resumePlay(r);
    }
    // Source1A4AF0 at the declared accepted-world seam, once per update.
    // The collision timer expires into a REAL unresolved234440(1.5) request.
    // Keep this owner and source queue held. No invented linear mixer ramp,
    //immediate stop, timer0 store or acknowledgement substitutes for it.
    UpdateReceipt50IW update(float sourceSeconds) noexcept {
        if(fadePending_)return {Result::PendingFade234440,0x1a4b3cu,0x234440u,1.5f};
        if(playPending_)return {Result::PendingPlay,pendingCallPC_,pendingPlayPC_,0.f};
        if(!mixer_)return {Result::Unprepared,0x1a4af0u,0,0.f};
        if(paused_)return {Result::Paused,0x1a4af0u,0,0.f};
        if(!current()||(!owned_&&mixer_->active(voice)))return {Result::Ownership,0x1a4af0u,0,0.f};
        const auto step=rr::game::beginRaceMusicUpdate50IW(sourceSeconds,state_);
        if(step.status==rr::game::RaceMusicUpdateStatus50IW::InvalidInput)
            return {Result::Input,0x1a4af0u,0,0.f};
        if(step.status==rr::game::RaceMusicUpdateStatus50IW::NeedFade234440){
            fadePending_=true;++counters_.fadeRequests;
            return {Result::PendingFade234440,step.sourcePC,0x234440u,step.fadeSeconds};
        }
        if(!pollCompletion())return {Result::Ownership,0x1a4b48u,0,0.f};
        return {Result::Updated,0x1a4b64u,0,0.f};
    }
    bool contactMusicEnabled()const noexcept{return contactMusicEnabled_;}
    bool fadePending()const noexcept{return fadePending_;}
    // Call after a real shared-mixer committed step, also before event dispatch.
    // Completion follows actual source-frame consumption, not a guessed timer.
    bool pollCompletion()noexcept {
        if(!current()||fadePending_||playPending_)return false;
        const bool active=mixer_->active(voice);
        if(!owned_&&active)return false; // Another writer violated117 ownership.
        if(owned_&&!active){owned_=false;currentSong_=0;++counters_.completions;}
        rr::game::observeCategory6MusicActivity50IE(active,state_);return true;
    }
    void pause(bool value)noexcept{paused_=value;}
    // Host race reset/close. No cumulative audio clock or other voice changes.
    void reset()noexcept {
        if(mixer_&&current()&&owned_)mixer_->stop(voice);
        owned_=false;currentSong_=0;state_={};paused_=false;fadePending_=false;playPending_=false;
        pendingSong_=0;pendingPlayPC_=0;pendingCallPC_=0;
        pendingLoop_=false;pendingStoppedPrevious_=false;++counters_.resets;
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
    Receipt resumePlay(Receipt r) noexcept {
        r.sourcePlayPC=pendingPlayPC_;r.stoppedPrevious=pendingStoppedPrevious_;
        if(!current()||(!owned_&&mixer_->active(voice)))return rejected(r,Result::Ownership);
        const auto i=detail::index(pendingSong_);
        if(i>=bank_.clips.size()||bank_.clips[i].state!=rr::assets::AudioAssetState50HC::ready)
            return rejected(r,Result::Unavailable);
        // Native transport: natural22050Hz PCM through the48000Hz mixer,
        //unity per-voice gain/pitch, independent sourceMusic118 group gain.
        //RACEMUS Pitch1881 is source SPU units, never a native pitch ratio.
        if(!mixer_->play(voice,clips_[i],1.f,1.f,pendingLoop_,0.f))
            return rejected(r,Result::BackendRejected);
        owned_=true;currentSong_=pendingSong_;r.result=Result::Started;r.originalV0=1;
        ++counters_.starts;if(pendingLoop_)++counters_.loopStarts;else ++counters_.oneShotStarts;
        if(pendingStoppedPrevious_)++counters_.stoppedPrevious;
        playPending_=false;pendingSong_=0;pendingPlayPC_=0;pendingCallPC_=0;
        pendingLoop_=false;pendingStoppedPrevious_=false;return r;
    }
    Receipt rejected(Receipt r,Result result)noexcept {
        r.result=result;++counters_.rejected;return r;
    }
    rr::assets::AudioBank50HC bank_{};
    std::array<rr::platform::AudioClip50HD,totalClipCount> clips_{};
    rr::platform::AudioMixer50HD* mixer_=nullptr;
    rr::platform::RaceAudioGroups50IC* groups_=nullptr;
    rr::game::RaceMusicState50IE state_{};
    Counters counters_{};
    std::size_t pcmBytes_=0;
    std::uint32_t currentSong_=0;
    bool attempted_=false,paused_=false,owned_=false;
    bool contactMusicEnabled_=false,fadePending_=false,playPending_=false;
    std::uint32_t pendingSong_=0,pendingPlayPC_=0,pendingCallPC_=0;
    bool pendingLoop_=false,pendingStoppedPrevious_=false;
};
} // namespace race_music50IE
