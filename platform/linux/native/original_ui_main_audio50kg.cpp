#include "original_ui_main_audio50kg.h"
#include "original_sound_mode50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace rr::platform { namespace {
using Bytes = std::vector<std::uint8_t>;
void need(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
// Same retained-byte SHA256 routine already used by audio_assets50hc.cpp.
inline std::uint32_t rotr(std::uint32_t x, unsigned n) { return (x >> n) | (x << (32 - n)); }
inline std::string sha256(const Bytes& input) {
    static const std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    std::array<std::uint32_t,8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    Bytes data(input);
    const auto bits = static_cast<std::uint64_t>(data.size()) * 8;
    data.push_back(0x80);
    while (data.size() % 64 != 56) data.push_back(0);
    for (int i = 7; i >= 0; --i) data.push_back(static_cast<std::uint8_t>(bits >> (i * 8)));
    for (std::size_t at = 0; at < data.size(); at += 64) {
        std::uint32_t w[64]{};
        for (unsigned i = 0; i < 16; ++i)
            for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | data[at+i*4+j];
        for (unsigned i = 16; i < 64; ++i) {
            const auto s0 = rotr(w[i-15],7) ^ rotr(w[i-15],18) ^ (w[i-15] >> 3);
            const auto s1 = rotr(w[i-2],17) ^ rotr(w[i-2],19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        auto v = h;
        for (unsigned i = 0; i < 64; ++i) {
            const auto s1 = rotr(v[4],6) ^ rotr(v[4],11) ^ rotr(v[4],25);
            const auto ch = (v[4] & v[5]) ^ (~v[4] & v[6]);
            const auto t1 = v[7] + s1 + ch + k[i] + w[i];
            const auto s0 = rotr(v[0],2) ^ rotr(v[0],13) ^ rotr(v[0],22);
            const auto maj = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
            for (unsigned j = 7; j > 0; --j) v[j] = v[j-1];
            v[4] += t1; v[0] = t1 + s0 + maj;
        }
        for (unsigned i = 0; i < 8; ++i) h[i] += v[i];
    }
    std::ostringstream stream;
    stream << std::hex << std::setfill('0');
    for (const auto word : h) stream << std::setw(8) << word;
    return stream.str();
}

struct OriginalFile { const char* path; std::size_t bytes; const char* sha; std::uint32_t frames; };
constexpr std::array<OriginalFile,7> originals{{
    {"MUSIC/UI/SCREENS/UI_MAIN.VAG",441024,"50617b8f8029f3fa80744a9de62d7c1987ea0e2d4b35487b8ce7ffd34401934a",384104},
    {"SFX/UI/SELECT.VAG",19424,"6688b65b124428d51cc1e3b531426c417be1bc066a8d560ffe0e7384b48d389a",33880},
    {"SFX/UI/LEFT.VAG",736,"b351f9789f73e8a1d41c954d0238d9b7fb93e257a55fea13b798ec60abb9c8eb",1176},
    {"SFX/UI/RIGHT.VAG",608,"6f515380394424ca812087219c7dad6928c535a68dfce1fd7cbccbb0683fe206",952},
    {"SFX/UI/BADCALL.VAG",6240,"f7d9fa2932f0354a472412c8f3ceea6cf7215f702a5d2ef6abb19590170ac385",10808},
    {"SFX/UI/BACK.VAG",11568,"3306a9ef40ab752a26a645f28f96ab61d2a7bb91b7a83c989496bd22e7a24b18",20132},
    {"SFX/UI/ADVANCE.VAG",2864,"e49eedb1fdfdc1c1ecf13d2d34f0357a5be77835f5293b295f4811050cd077ec",4900}
}};
constexpr OriginalFile riderOriginal{
    "MUSIC/UI/SCREENS/UI_PEOPL.VAG",709504,
    "2ac7db7a81ebcad40afa37998ac3e1dba16d5cd774d0da464db03faa411ed9e4",619024};
bool sameClip(const AudioClip50HD& a,const AudioClip50HD& b) noexcept {
    return a.samples==b.samples&&a.frames==b.frames&&a.sampleRate==b.sampleRate&&
        a.channels==b.channels&&a.loopStart==b.loopStart&&a.loopEnd==b.loopEnd;
}
bool preparedClipMatches(const AudioClip50HD& clip,const assets::AudioPcm50HC& pcm,
    const OriginalFile& original,std::uint8_t channels) noexcept {
    // Retained authentication/PCM ownership facts only; no load or decode.
    return AudioMixer50HD::validClip(clip)&&pcm.sourceSha256==original.sha&&
        pcm.frames==original.frames&&pcm.sampleRate==22050&&pcm.channels==channels&&
        !pcm.hasEmbeddedLoop&&!pcm.loopStart&&!pcm.loopEnd&&
        pcm.samples.size()==static_cast<std::size_t>(original.frames)*channels&&
        clip.samples==pcm.samples.data()&&clip.frames==pcm.frames&&clip.sampleRate==pcm.sampleRate&&
        clip.channels==pcm.channels&&!clip.loopStart&&!clip.loopEnd;
}
bool fixedVoiceMatches(const AudioVoiceObservation50HD& observed,const AudioClip50HD& clip,
    std::uint64_t playbackRevision,float gain,bool loop) noexcept {
    const auto& voice=observed.voice;
    return playbackRevision&&observed.playbackRevision==playbackRevision&&sameClip(voice.clip,clip)&&
        voice.gain==gain&&voice.pitch==1.f&&voice.pan==0.f&&voice.loop==loop&&
        observed.originalLeft==1.f&&observed.originalRight==1.f&&
        std::isfinite(voice.sourceFrame)&&voice.sourceFrame>=0.0&&
        voice.sourceFrame<static_cast<double>(clip.frames);
}
Bytes authenticatedRead(const std::filesystem::path& root, const char* relative,
    std::size_t expectedSize, const char* expectedHash) {
    need(!std::filesystem::is_symlink(root) && std::filesystem::is_directory(root), "Main audio root is not a real directory");
    auto path = root;
    for (const auto& part : std::filesystem::path(relative)) {
        need(part != "." && part != "..", "Unsafe Main audio relative path");
        path /= part;
        need(!std::filesystem::is_symlink(path), "Main audio symlink refused");
    }
    need(std::filesystem::is_regular_file(path), "Original Main audio file missing");
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    need(file.is_open() && file.tellg() == static_cast<std::streamoff>(expectedSize), "Main audio original extent differs");
    Bytes data(expectedSize); file.seekg(0);
    if (!data.empty()) file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    need(file.good() && file.peek() == std::char_traits<char>::eof(), "Main audio original read failed");
    need(sha256(data) == expectedHash, "Main audio original SHA256 differs");
    return data;
}
std::uint32_t be32(const std::uint8_t* p) noexcept {
    return (std::uint32_t(p[0]) << 24) | (std::uint32_t(p[1]) << 16) | (std::uint32_t(p[2]) << 8) | p[3];
}
assets::AudioPcm50HC decodeMenuStereo(const Bytes& data,const OriginalFile& original,std::size_t payload) {
    constexpr std::size_t prefix = 0x7e0;
    const auto frames=original.frames;
    need(data.size() == prefix + 2 * payload && be32(data.data()+12) == payload && be32(data.data()+16) == 22050,
        "Main stereo source framing differs");
    for (std::size_t i=0; i<prefix; ++i)
        if (i<12 || i>=20) need(data[i]==0, "Main stereo source prefix differs");
    const auto blocks = payload / 16;
    for (std::size_t i=0; i<blocks; ++i) {
        const auto at=prefix+i*32;
        const auto flag=static_cast<std::uint8_t>(i+2==blocks ? 1 : 0);
        need(data[at+1]==flag && data[at+17]==flag, "Main stereo channel flags differ");
    }
    for (std::size_t c=0; c<2; ++c) {
        const auto at=data.size()-32+c*16;
        need(data[at]==7 && data[at+1]==0, "Main stereo pad differs");
        for (std::size_t n=2; n<16; ++n) need(data[at+n]==0x77, "Main stereo pad data differs");
    }
    // Deinterleave original compressed channel bytes into the existing bounded
    // zero-header mono decoder. No fixture import or replacement PCM is used.
    std::array<assets::AudioPcm50HC,2> channels;
    Bytes mono(48+payload,0); std::copy(data.begin(),data.begin()+48,mono.begin());
    for (std::size_t c=0; c<2; ++c) {
        for (std::size_t i=0; i<blocks; ++i)
            std::memcpy(mono.data()+48+i*16,data.data()+prefix+i*32+c*16,16);
        std::string error;
        need(assets::decodeVagMono50HC(mono.data(),mono.size(),channels[c],error), "Original Main stereo channel decode rejected");
        need(channels[c].channels==1 && channels[c].sampleRate==22050 && channels[c].frames==frames && !channels[c].hasEmbeddedLoop,
            "Original Main decoded channel metadata differs");
    }
    assets::AudioPcm50HC result;
    result.channels=2; result.sampleRate=22050; result.frames=frames;
    result.sourceSha256=original.sha; result.samples.resize(frames*2);
    for (std::size_t n=0; n<frames; ++n) {
        result.samples[n*2]=channels[0].samples[n]; result.samples[n*2+1]=channels[1].samples[n];
    }
    return result;
}
}

bool OriginalUiMainAudio50KG::prepare(const std::filesystem::path& audioRoot,
    AudioMixer50HD& mixer, std::string& error) noexcept {
    if (attempted_) { error="Main audio needs a fresh stable owner"; return false; }
    attempted_=true;
    try {
        need(mixer.count()>=requiredVoices && mixer.sampleRate()!=0 && !mixer.requiresVoiceGroupGains50IC(),
            "Main audio requires a dedicated fresh frontend mixer");
        for (std::size_t i=0; i<mixer.count(); ++i)
            need(!mixer.active(i) && !mixer.hasVoiceGroupGain50IC(i), "Frontend mixer is already owned");
        authenticatedRead(audioRoot,"UIEVENTS.INI",790,"ed897d482c72b158f209c20baa6c4e8270a10cdbb9031315cd143a8666557115");
        authenticatedRead(audioRoot,"UI_MUSIC.JUK",1517,"4b814f8e4fec2fbcfa25807d5e147ecc164dbb0b0cb9ce5d064e2f3e3c89de98");
        authenticatedRead(audioRoot,"UI_MUSIC.JUB",3623,"f97a5cbceec8056dfc9b277501087aaa54d86a0ccdc0c71cfde73522a048c661");
        std::array<assets::AudioPcm50HC,7> next;
        for (std::size_t i=0; i<originals.size(); ++i) {
            const auto& def=originals[i]; const auto bytes=authenticatedRead(audioRoot,def.path,def.bytes,def.sha);
            if (i==0) next[i]=decodeMenuStereo(bytes,def,219504);
            else {
                std::string decodeError;
                need(assets::decodeVagMono50HC(bytes.data(),bytes.size(),next[i],decodeError), "Original Main effect decode rejected");
            }
            need(next[i].frames==def.frames && next[i].sampleRate==22050 && next[i].channels==(i==0?2:1) &&
                !next[i].hasEmbeddedLoop, "Original Main PCM metadata differs");
        }
        pcm_=std::move(next);
        for (std::size_t i=0; i<clips_.size(); ++i) {
            const auto& p=pcm_[i]; clips_[i]={p.samples.data(),p.frames,p.sampleRate,p.channels,0,0};
            need(AudioMixer50HD::validClip(clips_[i]), "Original Main PCM rejected by native mixer");
        }
        mixer_=&mixer; mixerEpoch_=mixer.groupEpoch50IC(); mixerCount_=mixer.count();
        mixerRate_=mixer.sampleRate();preparedSoundMode_=mixer.originalSoundMode50KG();
        noteControlChange();
        prepared_=true; error.clear(); return true;
    } catch (const std::exception& e) { error=e.what(); }
    catch (...) { error="Main audio cold preparation failed"; }
    return false;
}
bool OriginalUiMainAudio50KG::prepareRiderMenu(const std::filesystem::path& audioRoot,
    std::string& error) noexcept {
    if(riderAttempted_){error="Rider music needs a fresh cold preparation";return false;}
    riderAttempted_=true;
    try {
        need(current()&&!paused_&&!ownsMusic_&&!mixer_->mixedFrames(),
            "Rider music requires the prepared cold frontend transport");
        for(std::size_t i=0;i<mixer_->count();++i)
            need(!mixer_->active(i),"Rider music preparation cannot alter live playback");
        // The optional AUDIO root must carry the exact same original menu
        // authoring. No parsed index, filename guess or alternative sample.
        authenticatedRead(audioRoot,"UI_MUSIC.JUK",1517,"4b814f8e4fec2fbcfa25807d5e147ecc164dbb0b0cb9ce5d064e2f3e3c89de98");
        authenticatedRead(audioRoot,"UI_MUSIC.JUB",3623,"f97a5cbceec8056dfc9b277501087aaa54d86a0ccdc0c71cfde73522a048c661");
        const auto bytes=authenticatedRead(audioRoot,riderOriginal.path,riderOriginal.bytes,riderOriginal.sha);
        auto pcm=decodeMenuStereo(bytes,riderOriginal,353744);
        need(pcm.frames==riderOriginal.frames&&pcm.channels==2&&pcm.sampleRate==22050&&!pcm.hasEmbeddedLoop,
            "Original Rider music PCM metadata differs");
        const AudioClip50HD clip{pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,0,0};
        need(AudioMixer50HD::validClip(clip),"Original Rider music rejected by native mixer");
        riderPcm_=std::move(pcm);
        riderClip_={riderPcm_.samples.data(),riderPcm_.frames,riderPcm_.sampleRate,riderPcm_.channels,0,0};
        riderPrepared_=true;noteControlChange();error.clear();return true;
    }catch(const std::exception& e){error=e.what();}
    catch(...){error="Rider music cold preparation failed";}
    return false;
}
bool OriginalUiMainAudio50KG::current() const noexcept {
    return prepared_ && mixer_ && mixer_->groupEpoch50IC()==mixerEpoch_ && mixer_->count()==mixerCount_ &&
        !mixer_->requiresVoiceGroupGains50IC();
}
bool OriginalUiMainAudio50KG::canRecordControlChange() const noexcept {
    return !controlRevisionExhausted_&&controlRevision_!=std::numeric_limits<std::uint64_t>::max();
}
void OriginalUiMainAudio50KG::noteControlChange() noexcept {
    if(canRecordControlChange())++controlRevision_;
    else controlRevisionExhausted_=true;
}
bool OriginalUiMainAudio50KG::ready() const noexcept { return current(); }
bool OriginalUiMainAudio50KG::riderMenuReady() const noexcept {
    return current()&&riderPrepared_&&AudioMixer50HD::validClip(riderClip_);
}
const OriginalSoundMode50KG* OriginalUiMainAudio50KG::soundModeOwner50KG() const noexcept {
    return current()?mixer_->originalSoundMode50KG():nullptr;
}
bool OriginalUiMainAudio50KG::canApplySoundMode18874050KG(std::uint32_t mode) const noexcept {
    const auto* owner=soundModeOwner50KG();
    return !paused_&&owner&&owner->canApply188740(*mixer_,mode);
}
bool OriginalUiMainAudio50KG::applySoundMode18874050KG(std::uint32_t mode) noexcept {
    if(!canApplySoundMode18874050KG(mode)||!mixer_->originalSoundMode50KG()->apply188740(*mixer_,mode))return false;
    noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::startMainLoop9() noexcept {
    return startMusicLoop(clips_[0],9);
}
bool OriginalUiMainAudio50KG::startRiderLoop7() noexcept {
    return riderMenuReady()&&startMusicLoop(riderClip_,7);
}
bool OriginalUiMainAudio50KG::startMusicLoop(const AudioClip50HD& clip,std::uint32_t song) noexcept {
    if (!current() || paused_) return false;
    if (!ownsMusic_ && mixer_->active(musicVoice)) return false;
    //234900(restart1) replaces the outgoing music, including any pending
    //234440 retirement, before beginning the selected original clip at0.
    if (!mixer_->play(musicVoice,clip,0.4f,1.f,true,0.f)) return false;
    musicPlaybackRevision_=mixer_->playbackRevision(musicVoice);noteControlChange();
    ownsMusic_=true; fading_=false; musicSong_=song;
    musicGain_=0.4f; fadeElapsed_=fadeDuration_=fadeRetireAt_=0.f;
    return true;
}
bool OriginalUiMainAudio50KG::fadeRiderMusic() noexcept {
    if(!riderMenuReady()||(ownsMusic_&&musicSong_!=7))return false;
    return fadeMain(1.f);
}
bool OriginalUiMainAudio50KG::canPlayRiderBack5() const noexcept {
    return riderMenuReady()&&canPlayEffectClip(5);
}
bool OriginalUiMainAudio50KG::playRiderBack5() noexcept {
    return canPlayRiderBack5()&&playEffectClip(5);
}
bool OriginalUiMainAudio50KG::inspectSteadyRiderTransport(SteadyTransportObservation& out) const noexcept {
    out={};out.transport=this;out.mixer=mixer_;out.controlRevision=controlRevision_;
    out.paused=paused_;out.ownsMusic=ownsMusic_;out.fading=fading_;
    out.musicSong=musicSong_;out.musicGain=musicGain_;
    auto reject=[&](SteadyRiderError error,const char* detail,std::size_t voice=requiredVoices) noexcept {
        out.error=error;out.detail=detail;out.rejectedVoice=voice;return false;
    };
    if(!prepared_||!riderPrepared_)
        return reject(SteadyRiderError::preparationUnavailable,"Original Rider PCM preparation unavailable");
    if(!mixer_)return reject(SteadyRiderError::mixerUnavailable,"Rider mixer unavailable");
    out.mixerEpoch=mixer_->groupEpoch50IC();out.voiceCount=mixer_->count();out.outputRate=mixer_->sampleRate();
    out.mixerControlRevision=mixer_->controlRevision();out.sourceMixedFrames=mixer_->mixedFrames();
    out.masterGain=mixer_->masterGain();out.soundMode=mixer_->originalSoundMode50KG();
    if(out.mixerEpoch!=mixerEpoch_)
        return reject(SteadyRiderError::mixerEpochChanged,"Rider mixer initialization changed");
    if(out.voiceCount!=requiredVoices||out.voiceCount!=mixerCount_||out.outputRate!=mixerRate_||!out.outputRate)
        return reject(SteadyRiderError::mixerShapeChanged,"Rider requires its exact dedicated seven-voice mixer/rate");
    if(controlRevisionExhausted_||!controlRevision_||!mixer_->canRecordControlChanges())
        return reject(SteadyRiderError::revisionUnavailable,"Rider control identity exhausted or unavailable");
    if(paused_)return reject(SteadyRiderError::paused,"Rider transport paused");
    if(!out.soundMode||!out.soundMode->ready())
        return reject(SteadyRiderError::soundModeUnavailable,"Rider requires the installed application sound-mode owner");
    if(out.soundMode!=preparedSoundMode_)
        return reject(SteadyRiderError::soundModeOwnerChanged,"Rider prepared sound-mode owner changed");
    out.soundModeRevision=out.soundMode->revision();
    out.profileSoundMode=static_cast<std::uint32_t>(out.soundMode->fields().profile1101C);
    game::OriginalSoundRouting50KG routing;
    if(!game::projectOriginalSoundRouting26CBA050KG(out.soundMode->fields(),0,100,routing)||
        routing.phaseSelector!=0||routing.positional||routing.leftPercent!=100||routing.rightPercent!=100)
        return reject(SteadyRiderError::unsupportedRouting,"Rider requires supported source centered routing");
    if(out.masterGain!=1.f||mixer_->requiresVoiceGroupGains50IC())
        return reject(SteadyRiderError::unsupportedGainLayer,"Rider master/group gain layer differs");
    for(std::size_t voice=0;voice<requiredVoices;++voice){
        auto& observed=voice==musicVoice?out.music:out.effects[voice-firstEffectVoice];
        if(!mixer_->inspectVoice(voice,observed))
            return reject(SteadyRiderError::revisionUnavailable,"Rider live voice inspection unavailable",voice);
        if(observed.groupKnown||observed.groupGain!=1.f)
            return reject(SteadyRiderError::unsupportedGainLayer,"Rider foreign per-voice group gain layer",voice);
    }
    if(!ownsMusic_||musicSong_!=7||!out.music.voice.active)
        return reject(SteadyRiderError::wrongMusic,"Rider requires owned active Song7",musicVoice);
    if(fading_||musicGain_!=0.4f||fadeElapsed_!=0.f||fadeDuration_!=0.f||fadeRetireAt_!=0.f)
        return reject(SteadyRiderError::nonsteadyEnvelope,"Rider Song7 envelope is not the settled start state",musicVoice);
    if(!preparedClipMatches(riderClip_,riderPcm_,riderOriginal,2))
        return reject(SteadyRiderError::invalidMusicClip,"Rider authenticated Song7 PCM identity/metadata differs",musicVoice);
    if(!fixedVoiceMatches(out.music,riderClip_,musicPlaybackRevision_,0.4f,true))
        return reject(SteadyRiderError::musicVoiceChanged,"Rider Song7 live playback identity/controls/routing differs",musicVoice);
    for(std::size_t i=0;i<effectVoiceCount;++i){
        const auto& observed=out.effects[i];
        // The actual mixer ends one-shots. Bookkeeping may await a real page
        // update; inspecting or servicing PCM must not clear/restart them.
        if(!observed.voice.active)continue;
        const auto voice=firstEffectVoice+i,clip=i+1;
        if(!ownsEffect_[i]||observed.playbackRevision!=effectPlaybackRevisions_[i])
            return reject(SteadyRiderError::foreignEffect,"Rider active effect is not this transport's playback",voice);
        if(!preparedClipMatches(clips_[clip],pcm_[clip],originals[clip],1))
            return reject(SteadyRiderError::invalidEffectClip,"Rider authenticated effect PCM identity/metadata differs",voice);
        if(!fixedVoiceMatches(observed,clips_[clip],effectPlaybackRevisions_[i],1.f,false))
            return reject(SteadyRiderError::effectVoiceChanged,"Rider live one-shot controls/routing differs",voice);
    }
    out.error=SteadyRiderError::none;out.detail="Owned steady Rider Song7 and bounded effects observed";return true;
}
bool OriginalUiMainAudio50KG::canPlayRiderAdvance2() const noexcept {
    SteadyTransportObservation observed;
    return inspectSteadyRiderTransport(observed)&&canRecordControlChange()&&
        mixer_->canRecordControlChanges(2)&&
        preparedClipMatches(clips_[6],pcm_[6],originals[6],1)&&canPlayEffectClip(6);
}
bool OriginalUiMainAudio50KG::playRiderAdvance2(RiderAdvanceCommandReceipt& receipt) noexcept {
    receipt={};
    if(!canPlayRiderAdvance2())return false;
    RiderAdvanceCommandObservation observed;
    observed.transport=this;observed.mixer=mixer_;observed.soundMode=mixer_->originalSoundMode50KG();
    observed.controlRevisionBefore=controlRevision_;observed.mixerControlRevisionBefore=mixer_->controlRevision();
    // Exactly the existing authenticated ADVANCE.VAG stop-before-play path.
    // The preflight reserves both mixer control identities before any stop.
    if(!playEffectClip(6))return false;
    observed.controlRevisionAfter=controlRevision_;observed.mixerControlRevisionAfter=mixer_->controlRevision();
    observed.mixerEpoch=mixerEpoch_;observed.soundModeRevision=observed.soundMode->revision();
    observed.sourceMixedFrames=mixer_->mixedFrames();observed.eventId=2;observed.voiceSlot=6;
    // Guaranteed by the capacity preflight and exclusive main-thread owner.
    // A violation cannot be rolled back or retried as an unplayed command.
    if(!mixer_->inspectVoice(6,observed.voice))return false;
    receipt.observation_=observed;receipt.issued_=true;return true;
}
bool OriginalUiMainAudio50KG::fadeMain(float seconds) noexcept {
    if (!current() || paused_ || (seconds!=0.f && seconds!=1.f)) return false;
    // Source234480: inactive stream(+5C==-1) returns without effects; Main
    // B4 ignores that source result. Native true means command handled, not
    // a fabricated source return1. A foreign active voice is still rejected.
    if (!ownsMusic_ && !mixer_->active(musicVoice)) return true;
    if (!ownsMusic_ || !mixer_->active(musicVoice)) return false;
    if (seconds==0.f) {
        mixer_->stop(musicVoice); ownsMusic_=false; fading_=false; musicSong_=0; musicGain_=0.f;
        noteControlChange();return true;
    }
    fadeStartGain_=musicGain_; fadeElapsed_=0.f; fadeDuration_=seconds;
    volatile float retirement=seconds+0.35f; fadeRetireAt_=retirement;
    fading_=true;noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::playMenuEffect(std::uint32_t originalId) noexcept {
    if (!current() || paused_) return false;
    const std::size_t clip = originalId==1?1 : originalId==3?2 : originalId==4?3 : originalId==7?4 : clips_.size();
    return playEffectClip(clip);
}
bool OriginalUiMainAudio50KG::playOptionsEffect(std::uint32_t originalId) noexcept {
    const std::size_t clip=originalId==1?1:originalId==3?2:originalId==4?3:originalId==5?5:clips_.size();
    return playEffectClip(clip);
}
bool OriginalUiMainAudio50KG::canPlayGameOptionsEffect(std::uint32_t originalId) const noexcept {
    const std::size_t clip=originalId==1?1:originalId==2?6:originalId==3?2:originalId==4?3:
        originalId==5?5:originalId==7?4:clips_.size();return canPlayEffectClip(clip);
}
bool OriginalUiMainAudio50KG::playGameOptionsEffect(std::uint32_t originalId) noexcept {
    const std::size_t clip=originalId==1?1:originalId==2?6:originalId==3?2:originalId==4?3:
        originalId==5?5:originalId==7?4:clips_.size();return playEffectClip(clip);
}
bool OriginalUiMainAudio50KG::canPlayEffectClip(std::size_t clip) const noexcept {
    if (!current() || paused_ || !clip || clip>=clips_.size()) return false;
    const auto voice=firstEffectVoice+clip-1;
    if (voice>=mixer_->count() || !AudioMixer50HD::validClip(clips_[clip]) ||
        (!ownsEffect_[clip-1] && mixer_->active(voice))) return false;
    if(const auto* mode=mixer_->originalSoundMode50KG()){
        game::OriginalSoundRouting50KG routing;
        if(!mode->ready()||!game::projectOriginalSoundRouting26CBA050KG(mode->fields(),0,100,routing)||
            routing.phaseSelector!=0)return false;
    }
    return true;
}
bool OriginalUiMainAudio50KG::playEffectClip(std::size_t clip) noexcept {
    if(!canPlayEffectClip(clip))return false;
    const auto voice=firstEffectVoice+clip-1;
    const auto revision=mixer_->controlRevision();
    // Source233458..233488 performs stop-before-play for flags1, not overlap
    // or suppression of a repeated event. AudioMixer::play restarts at frame0.
    if (ownsEffect_[clip-1]) mixer_->stop(voice);
    if (!mixer_->play(voice,clips_[clip],1.f,1.f,false,0.f)) {
        if(revision!=mixer_->controlRevision()||!mixer_->canRecordControlChanges())noteControlChange();
        return false;
    }
    ownsEffect_[clip-1]=true;effectPlaybackRevisions_[clip-1]=mixer_->playbackRevision(voice);
    noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::update(float seconds) noexcept {
    if (!current() || !std::isfinite(seconds) || seconds<0.f || seconds>1.f) return false;
    if (paused_) return true;
    if (!ownsMusic_ && mixer_->active(musicVoice)) return false;
    for (std::size_t i=0; i<effectVoiceCount; ++i) {
        if (!ownsEffect_[i] && mixer_->active(firstEffectVoice+i)) return false;
        if (ownsEffect_[i] && !mixer_->active(firstEffectVoice+i)) ownsEffect_[i]=false;
    }
    if (ownsMusic_ && !mixer_->active(musicVoice)) { ownsMusic_=false; fading_=false; musicSong_=0; }
    if (!fading_) return true;
    const auto previousElapsed=fadeElapsed_,previousGain=musicGain_;
    const auto revision=mixer_->controlRevision();
    volatile float elapsed=fadeElapsed_+seconds; fadeElapsed_=elapsed;
    musicGain_=fadeStartGain_*std::max(0.f,1.f-fadeElapsed_/fadeDuration_);
    if (!mixer_->controls(musicVoice,musicGain_,1.f,0.f)) {
        if(previousElapsed!=fadeElapsed_||previousGain!=musicGain_)noteControlChange();
        return false;
    }
    if (fadeElapsed_>=fadeRetireAt_) {
        mixer_->stop(musicVoice); ownsMusic_=false; fading_=false; musicSong_=0; musicGain_=0.f;
    }
    if(previousElapsed!=fadeElapsed_||previousGain!=musicGain_||revision!=mixer_->controlRevision()||
        !mixer_->canRecordControlChanges())noteControlChange();
    return true;
}
bool OriginalUiMainAudio50KG::mixStereo16(std::int16_t* output, std::size_t frames) noexcept {
    if (!current() || frames>mixer_->sampleRate() || (!output && frames)) return false;
    if (paused_) {
        if (frames) std::fill(output,output+frames*2,std::int16_t(0));
        return true;
    }
    return mixer_->mixStereo16(output,frames);
}
bool OriginalUiMainAudio50KG::mainLoopActive() const noexcept {
    return activeMusicSong()==9;
}
bool OriginalUiMainAudio50KG::riderLoopActive() const noexcept {
    return activeMusicSong()==7;
}
std::uint32_t OriginalUiMainAudio50KG::activeMusicSong() const noexcept {
    return current()&&ownsMusic_&&mixer_->active(musicVoice)?musicSong_:0;
}
void OriginalUiMainAudio50KG::reset() noexcept {
    const bool changed=prepared_||riderPrepared_||mixer_||ownsMusic_||fading_||paused_||musicSong_;
    if (current()) {
        if (ownsMusic_) mixer_->stop(musicVoice);
        for (std::size_t i=0; i<effectVoiceCount; ++i)
            if (ownsEffect_[i]) mixer_->stop(firstEffectVoice+i);
    }
    prepared_=false; riderPrepared_=false; mixer_=nullptr; ownsMusic_=false; fading_=false; paused_=false; musicSong_=0;
    ownsEffect_.fill(false); musicGain_=0.f;
    if(changed)noteControlChange();
}
}
