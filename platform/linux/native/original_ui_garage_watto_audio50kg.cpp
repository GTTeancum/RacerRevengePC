// Native source-backed Sound objects and finite PCM instances, not IOP emulation.
#include "original_ui_garage_watto_audio50kg.h"
#include "original_sound_mode50kg.h"
#include "original_ui_garage_droid_audio50gn.h"
#include "original_ui_garage_watto_sound_group50kg.h"
#include <algorithm>
#include <bit>
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

Bytes authenticatedRead(const std::filesystem::path& root, const char* relative,
    std::size_t expectedSize, const char* expectedHash) {
    need(!std::filesystem::is_symlink(root) && std::filesystem::is_directory(root), "Watto audio root is not a real directory");
    auto path = root;
    for (const auto& part : std::filesystem::path(relative)) {
        need(part != "." && part != "..", "Unsafe Watto audio relative path");
        path /= part;
        need(!std::filesystem::is_symlink(path), "Watto audio symlink refused");
    }
    need(std::filesystem::is_regular_file(path), "Original Watto audio file missing");
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    need(file.is_open() && file.tellg() == static_cast<std::streamoff>(expectedSize), "Watto audio original extent differs");
    Bytes data(expectedSize); file.seekg(0);
    if (!data.empty()) file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    need(file.good() && file.peek() == std::char_traits<char>::eof(), "Watto audio original read failed");
    need(sha256(data) == expectedHash, "Watto audio original SHA256 differs");
    return data;
}

struct Original {const char* path;std::size_t bytes;const char* sha;std::uint32_t frames,song,offset;const char* duration;};
constexpr std::array<Original,40> originals{{
#include "original_ui_garage_watto_audio50kg_catalog.inc"
}};
constexpr Original wing{"SFX/UI/WATWING3.VAG",2384,"bb8aa4e50a3b01a4a8cbbedda4fd5ca7e192ec995c397a843e1a9c34f8112926",4060,0,0,""};
bool sameClip(const AudioClip50HD& a,const AudioClip50HD& b) noexcept {
 return a.samples==b.samples&&a.frames==b.frames&&a.sampleRate==b.sampleRate&&a.channels==b.channels&&a.loopStart==b.loopStart&&a.loopEnd==b.loopEnd;
}
bool pcmMatches(const assets::AudioPcm50HC& p,const AudioClip50HD& c,const Original& d) noexcept {
 return p.sourceSha256==d.sha&&p.frames==d.frames&&p.sampleRate==22050&&p.channels==1&&!p.hasEmbeddedLoop&&!p.loopStart&&!p.loopEnd&&p.samples.size()==d.frames&&
  c.samples==p.samples.data()&&c.frames==p.frames&&c.sampleRate==p.sampleRate&&c.channels==1&&!c.loopStart&&!c.loopEnd&&AudioMixer50HD::validClip(c);
}
}
bool OriginalUiGarageWattoAudio50KG::prepare(const std::filesystem::path& root,
 std::shared_ptr<const assets::OriginalStreamMetadata50KZ> metadata,AudioMixer50HD& mixer,std::string& error,const OriginalUiGarageDroidAudio50GN* sceneAudio) noexcept {
 if(attempted_){error="Watto audio requires a fresh stable owner";return false;}attempted_=true;
 try {
  need(metadata&&metadata->prepared(),"Watto requires authenticated original stream metadata");
  const auto frontendVoices=sceneAudio?OriginalUiGarageDroidAudio50GN::requiredFrontendVoices:requiredFrontendVoices;
  if(sceneAudio){OriginalUiGarageDroidAudio50GN::Observation observed;
   need(sceneAudio->inspect(observed)&&observed.mixer==&mixer&&sceneAudio->transportValid(),"Watto requires the actual retained Droid transport range, not later Scene construction");}
  need(mixer.count()==frontendVoices&&mixer.sampleRate()&&!mixer.mixedFrames()&&!mixer.requiresVoiceGroupGains50IC()&&mixer.masterGain()==1.f,
   "Watto requires the explicit cold49 or authenticated aggregate53 frontend transport");
  need(mixer.originalSoundMode50KG()&&mixer.originalSoundMode50KG()->ready(),"Watto requires the real application sound-mode owner");
  for(std::size_t i=0;i<instanceCount;++i)need(!mixer.active(firstVoice+i)&&!mixer.hasVoiceGroupGain50IC(firstVoice+i),"Watto subgroup already occupied or bound");
  authenticatedRead(root,"UIWATTO.INI",3075,"47a83b53b2bf076eb12e6a82a866137bf03c3e215905366611dcfa9ca8c220c5");
  std::array<Sound,soundCount> next;
  for(std::size_t i=0;i<soundCount;++i){
   const auto& def=i<responseCount?originals[i]:wing;
   const auto bytes=authenticatedRead(root,def.path,def.bytes,def.sha);std::string detail;
   need(assets::decodeVagMono50HC(bytes.data(),bytes.size(),next[i].pcm,detail),"Original Watto VAG decode failed");
   auto& sound=next[i];const auto& p=sound.pcm;
   sound.clip={p.samples.data(),p.frames,p.sampleRate,p.channels,0,0};
   need(pcmMatches(p,sound.clip,def),"Original Watto PCM identity/shape differs");
   if(i<responseCount){
    std::string key=def.path;std::replace(key.begin(),key.end(),'/','\\');
    sound.metadata=metadata->findForStreamRequest(true,key);
    const auto* row=sound.metadata;
    need(row&&row->songNumber==def.song&&row->blockByteOffset==def.offset&&
     row->authoredDuration.value==def.duration&&row->originalPitch==originalBasePitch&&row->originalSampleRate==22050&&
     row->originalSize+48==def.bytes,"Watto Sound+20 stream metadata identity differs");
   }
  }
  need(mixer.canRecordControlChanges(instanceCount),"Watto subgroup revision capacity unavailable");
  sounds_=std::move(next);metadata_=std::move(metadata);mixer_=&mixer;mode_=mixer.originalSoundMode50KG();
  epoch_=mixer.groupEpoch50IC();rate_=mixer.sampleRate();sceneAudio_=sceneAudio;frontendVoices_=frontendVoices;
  for(std::size_t i=0;i<instanceCount;++i){
   auto& s=sounds_[i];s.clip={s.pcm.samples.data(),s.pcm.frames,s.pcm.sampleRate,s.pcm.channels,0,0};
   if(!mixer.setVoiceGroupGain50IC(firstVoice+i,1.f)){poisoned_=true;error="Watto group binding partially failed";return false;}
   playbackRevisions_[i]=mixer.playbackRevision(firstVoice+i);
  }
  prepared_=true;revision_=1;error.clear();return true;
 }catch(const std::exception& e){error=e.what();}catch(...){error="Watto audio cold preparation failed";}
 return false;
}
bool OriginalUiGarageWattoAudio50KG::canRevise(std::uint64_t count) const noexcept {
 return !poisoned_&&revision_&&count<=std::numeric_limits<std::uint64_t>::max()-revision_;
}
bool OriginalUiGarageWattoAudio50KG::nativeSampleLeasesEmpty()const noexcept {
 for(const auto& sound:sounds_)if(sound.nativeSample.retainsBacking())return false;
 for(const auto& binding:nativeVoices_)if(binding.retainsBacking())return false;
 return true;
}
bool OriginalUiGarageWattoAudio50KG::current() const noexcept {
 if(!prepared_||poisoned_||!mixer_||!metadata_||!metadata_->prepared()||!canRevise(0)||
  mixer_->groupEpoch50IC()!=epoch_||mixer_->count()!=frontendVoices_||mixer_->sampleRate()!=rate_||
  mixer_->requiresVoiceGroupGains50IC()||mixer_->masterGain()!=1.f||mixer_->originalSoundMode50KG()!=mode_||!mode_||!mode_->ready()||!mixer_->canRecordControlChanges())return false;
 if(sceneAudio_&&!sceneAudio_->transportValid())return false;
 game::OriginalSoundRouting50KG route;
 if(!game::projectOriginalSoundRouting26CBA050KG(mode_->fields(),0,100,route)||route.phaseSelector||route.positional||route.leftPercent!=100||route.rightPercent!=100)return false;
 for(std::size_t i=0;i<soundCount;++i){const auto& s=sounds_[i];const auto& def=i<responseCount?originals[i]:wing;
  if(!pcmMatches(s.pcm,s.clip,def))return false;
  if(i<responseCount&&(!s.metadata||metadata_->row(def.song-1)!=s.metadata))return false;
 }
 for(std::size_t i=0;i<instanceCount;++i){
  AudioVoiceObservation50HD v;
  if(!mixer_->inspectVoice(firstVoice+i,v)||!v.groupKnown||v.groupGain!=1.f||v.groupPaused||v.playbackRevision!=playbackRevisions_[i])return false;
  const auto id=soundIds_[i];
  if(!id){if(v.voice.active||v.playbackRevision||nativeVoices_[i].retainsBacking())return false;continue;}
  if(id>soundCount)return false;
  const auto& s=sounds_[id-1];const auto gain=id==soundCount?17.f/100.f:1.f;
  auto expected=s.clip;
  if(nativeVoices_[i].retainsBacking()&&!nativeVoices_[i].clipForRetainedVoice(this,expected))return false;
  if(!sameClip(v.voice.clip,expected)||v.voice.gain!=1.f||v.voice.pitch!=nativeRendererPitch||v.voice.pan!=0.f||v.voice.loop||
   v.originalLeft!=gain||v.originalRight!=gain||!std::isfinite(v.voice.sourceFrame)||v.voice.sourceFrame<0.0||
   (v.voice.active&&v.voice.sourceFrame>=static_cast<double>(expected.frames)))return false;
 }
 return true;
}
bool OriginalUiGarageWattoAudio50KG::ready() const noexcept {
 if(!current())return false;
 if(!sourceGroupAttempted_)return true;
 return sourceGroup_&&!sourceGroupReleased_&&sourceGroup_->main_&&
     sourceGroup_->main_->nativeWattoPlaybackCurrent(*this);
}
bool OriginalUiGarageWattoAudio50KG::quiet() const noexcept {
 if(!current())return false;
 for(std::size_t i=0;i<instanceCount;++i)if(mixer_->active(firstVoice+i))return false;
 return true;
}
bool OriginalUiGarageWattoAudio50KG::inspect(Observation& out) const noexcept {
 out={};if(!current())return false;
 out.owner=this;out.mixer=mixer_;out.metadata=metadata_.get();out.revision=revision_;out.mixerRevision=mixer_->controlRevision();out.mixerEpoch=epoch_;out.mixedFrames=mixer_->mixedFrames();out.soundIds=soundIds_;
 for(std::size_t i=0;i<instanceCount;++i){if(!mixer_->inspectVoice(firstVoice+i,out.voices[i]))return false;if(out.voices[i].voice.active)++out.activeInstances;else ++out.freeInstances;}
 return true;
}
bool OriginalUiGarageWattoAudio50KG::canExecute(std::size_t plays,bool stopResponses) const noexcept {
 if(paused_||plays>instanceCount||!ready()||
  plays>static_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::max()-nextToken_)||!canRevise(plays+std::uint64_t(stopResponses))||
  !mixer_->canRecordControlChanges(plays+(stopResponses?responseCount:0)))return false;
 if(sourceGroupAttempted_&&(!sourceGroup_||!sourceGroup_->main_||
    !sourceGroup_->main_->nativeSourcePlaybackCurrent(plays)))return false;
 std::size_t free=0;for(std::size_t i=0;i<instanceCount;++i)
  if(!mixer_->active(firstVoice+i)&&(!sourceGroupAttempted_||
     !mixer_->nativeSourceVoiceReserved(firstVoice+i,playbackRevisions_[i])))++free;
 return free>=plays;
}
const assets::OriginalStreamMetadataRow50KZ* OriginalUiGarageWattoAudio50KG::eventMetadata(std::uint32_t event) const noexcept {
 return event&&event<=responseCount&&current()?sounds_[event-1].metadata:nullptr;
}
bool OriginalUiGarageWattoAudio50KG::queryDuration(std::uint32_t event,float& duration) const noexcept {
 const auto* metadata=eventMetadata(event);if(!metadata)return false;duration=metadata->admittedDurationSeconds;return true;
}
bool OriginalUiGarageWattoAudio50KG::sourceSampleStartCurrent(std::size_t sound,const AudioClip50HD& clip,
 AudioMixer50HD::NativeSourceSampleStart50KG& out)const noexcept {
 if(sound!=responseCount||!current()||paused_||!sourceGroup_||!canRevise(1)||
    nextToken_==std::numeric_limits<std::int32_t>::max())return false;
 const auto& s=sounds_[sound];
 if(!sourceSoundCurrent(s)||!s.source||s.source->flags10!=1||s.source->stream1A8||
    s.source->stereo1AC||s.source->loop1B0||!s.source->resourceA0||!s.source->nativeSample28||
    !s.source->nativeSample28->current()||s.source->volume38!=17||s.source->pan3C)return false;
 AudioClip50HD owned;OriginalAudioSampleCache50KG::Observation metadata;
 if(!s.nativeSample.clipForResolved(this,*s.source->nativeSample28,owned)||!sameClip(owned,clip)||
    !s.nativeSample.inspect(metadata)||metadata.sourceOrdinal!=*s.source->resourceA0||
    std::bit_cast<std::uint32_t>(s.source->duration20)!=metadata.observedSourceDurationBits||
    s.source->basePitch50!=static_cast<std::int32_t>(metadata.observedSourcePitch)||
    s.source->pitch40!=s.source->basePitch50||s.source->pitch4C!=s.source->basePitch50||
    s.source->basePitch50!=static_cast<std::int32_t>(originalBasePitch))return false;
 // Capture already-published2332F0/233E90 source fields, never PCM duration.
 // resourceA0 is directory identity only; no integer sample28 is fabricated.
 out={s.source->duration20,s.source->loop1B0,s.source->stream1A8,s.source->stereo1AC};return true;
}
bool OriginalUiGarageWattoAudio50KG::publishNativeVoice(std::size_t sound,std::size_t& instance)noexcept {
 // Native transport half only. Public playSound still requires the unchanged
 // completed-source/reverb ready gate before calling this private operation.
 if(sound>=soundCount||paused_||!current()||!canRevise(1)||!mixer_->canRecordControlChanges(1))return false;
 auto& s=sounds_[sound];auto clip=s.clip;
 std::size_t slot=mixer_->count();
 if(s.source){
  // An inactive decoder can still own its actual source reservation.
  for(std::size_t n=0;n<instanceCount;++n)if(!mixer_->active(firstVoice+n)&&
      !mixer_->nativeSourceVoiceReserved(firstVoice+n,playbackRevisions_[n])){slot=firstVoice+n;break;}
 }else slot=mixer_->availableVoice(firstVoice,instanceCount);
 if(slot==mixer_->count())return false;
 OriginalAudioSampleCache50KG::NativeSampleBinding binding;
 if(s.source&&!s.source->stream1A8){
  if(!s.source->nativeSample28||!s.nativeSample.clipForResolved(this,*s.source->nativeSample28,clip))return false;
  binding=s.nativeSample; // Hold the canonical bytes before publishing their raw clip.
 }else if(s.nativeSample.retainsBacking())return false;
 bool started=false;
 if(s.source){
  if(!sourceGroup_||!sourceGroup_->main_)return false;
  started=s.source->stream1A8?
      sourceGroup_->main_->publishNativeWattoStream(*this,sound,slot,clip):
      sourceGroup_->main_->publishNativeWattoSample(*this,sound,slot,clip);
 }else started=mixer_->playSourceVolume(slot,clip,sound==responseCount?17:100,nativeRendererPitch,false,0);
 if(!started){poisoned_=true;return false;}
 const auto i=slot-firstVoice;
 nativeVoices_[i]=std::move(binding); // Previous slot borrow is overwritten before its lease retires.
 soundIds_[i]=static_cast<std::uint32_t>(sound+1);playbackRevisions_[i]=mixer_->playbackRevision(slot);
 ++revision_;instance=i;return true;
}
bool OriginalUiGarageWattoAudio50KG::playSound(std::size_t sound) noexcept {
 if(sound>=soundCount||!canExecute(1,false))return false;
 std::size_t i=instanceCount;if(!publishNativeVoice(sound,i))return false;
 auto& s=sounds_[sound];s.latest=i;s.latestRevision=playbackRevisions_[i];s.latestToken=++nextToken_;
 if(s.source){auto& f=*s.source;
  if(!f.stream1A8){
   // The native source start consumed already-published exact metadata.
   // Do not overwrite duration20/pitch with a post-start container estimate.
   f.volume44=f.volume38;f.pan48=f.pan3C;
  }
  f.channel2C=s.latestToken;f.elapsedBits24=0;f.word7C=1;
 }
 return true;
}
bool OriginalUiGarageWattoAudio50KG::playResponse(std::uint32_t event,std::uint32_t restart,std::uint32_t immediate) noexcept {
 return event&&event<=responseCount&&!restart&&!immediate&&playSound(event-1);
}
bool OriginalUiGarageWattoAudio50KG::playWing(std::uint32_t flags) noexcept {return flags==2&&playSound(responseCount);}
bool OriginalUiGarageWattoAudio50KG::stopAllResponses() noexcept {
 if(!canExecute(0,true))return false;
 for(std::size_t i=0;i<responseCount;++i){auto& s=sounds_[i];
  // Reused finite slots have new revisions: the obsolete handle is already
  // naturally complete, not permission to stop a different Sound's instance.
  if(s.source){if(!stopSource233670(s)){poisoned_=true;return false;}}
  else {
   if(s.latest<instanceCount&&playbackRevisions_[s.latest]==s.latestRevision&&soundIds_[s.latest]==i+1)
    mixer_->stop(firstVoice+s.latest);
   s.latest=instanceCount;s.latestRevision=0;s.latestToken=-1;
  }
 }
 ++revision_;return true;
}
bool OriginalUiGarageWattoAudio50KG::drainInstances() noexcept {
 if(!mixer_||mixer_->groupEpoch50IC()!=epoch_||mixer_->count()!=frontendVoices_)return false;
 const bool valid=current()&&canRevise(1)&&mixer_->canRecordControlChanges(instanceCount);
 for(std::size_t i=0;i<instanceCount;++i)mixer_->stop(firstVoice+i);
 for(auto& s:sounds_){
  s.latest=instanceCount;s.latestRevision=0;s.latestToken=-1;
  if(s.source&&s.source->reference00==0)s.nativeSample={};
 }
 if(valid){++revision_;return true;}poisoned_=true;return false;
}
bool OriginalUiGarageWattoAudio50KG::canRearmRetiredSourceGroupCold()const noexcept {
 if(!current()||paused_||!sourceGroupAttempted_||!sourceGroupReleased_||sourceGroup_||nextToken_||!canRevise(1))return false;
 const OriginalUiGarageWattoSoundGroup50KG* identity=nullptr;
 for(const auto& s:sounds_){
  // Compare dead source identities only. The enclosing manager proof owns
  // their retirement; never dereference the former Group pointer.
  if(!s.source||!s.source->group34||s.source->reference00||s.source->channel2C!=-1||
     s.source->elapsedBits24||s.source->word7C||s.latest!=instanceCount||s.latestRevision||
     s.latestToken!=-1||s.nativeSample.retainsBacking())return false;
  if(identity&&identity!=s.source->group34)return false;
  identity=s.source->group34;
 }
 for(std::size_t i=0;i<instanceCount;++i){AudioVoiceObservation50HD v;
  if(soundIds_[i]||playbackRevisions_[i]||nativeVoices_[i].retainsBacking()||
     !mixer_->inspectVoice(firstVoice+i,v)||v.voice.active||v.playbackRevision||
     v.voice.clip.samples||v.voice.clip.frames||v.voice.sourceFrame!=0.0)return false;
 }
 return true;
}
bool OriginalUiGarageWattoAudio50KG::rearmRetiredSourceGroupCold()noexcept {
 if(!canRearmRetiredSourceGroupCold())return false;
 for(auto& s:sounds_)s.source.reset();
 sourceGroupAttempted_=false;sourceGroupReleased_=false;++revision_;return true;
}
bool OriginalUiGarageWattoAudio50KG::sourceGroupCold()const noexcept {
 if(!current()||paused_||sourceGroupAttempted_||nextToken_)return false;
 for(const auto& s:sounds_)if(s.source||s.latest!=instanceCount||s.latestRevision||s.latestToken!=-1)return false;
 for(std::size_t i=0;i<instanceCount;++i)if(soundIds_[i]||playbackRevisions_[i]||mixer_->active(firstVoice+i))return false;
 return true;
}
bool OriginalUiGarageWattoAudio50KG::stoppedInstancesCurrent()const noexcept {
 if(!current())return false;
 for(std::size_t i=0;i<instanceCount;++i){AudioVoiceObservation50HD v;
  if(!mixer_->inspectVoice(firstVoice+i,v)||v.voice.active||
     mixer_->nativeSourceVoiceReserved(firstVoice+i,v.playbackRevision))return false;
  if(v.playbackRevision&&soundIds_[i]==soundCount&&!nativeVoices_[i].retainsBacking())return false;
 }
 return true;
}
bool OriginalUiGarageWattoAudio50KG::canRearmRetiredSourceGroupRetainingHistory()const noexcept {
 if(!stoppedInstancesCurrent()||paused_||!sourceGroupAttempted_||!sourceGroupReleased_||sourceGroup_||!canRevise(1))return false;
 const OriginalUiGarageWattoSoundGroup50KG* identity=nullptr;
 for(const auto& s:sounds_){
  if(!s.source||!s.source->group34||s.source->reference00||s.source->channel2C!=-1||
     s.latest!=instanceCount||s.latestRevision||s.latestToken!=-1||s.nativeSample.retainsBacking())return false;
  if(identity&&identity!=s.source->group34)return false;
  identity=s.source->group34;
 }
 return true;
}
bool OriginalUiGarageWattoAudio50KG::rearmRetiredSourceGroupRetainingHistory()noexcept {
 if(!canRearmRetiredSourceGroupRetainingHistory())return false;
 for(auto& s:sounds_)s.source.reset();
 // Only dead source references retire. Actual stopped clip leases, native
 // revision/token monotonicity and history remain owned by this same backend.
 sourceGroupAttempted_=false;sourceGroupReleased_=false;retainedSourceRearm_=true;++revision_;return true;
}
bool OriginalUiGarageWattoAudio50KG::sourceGroupConstructionCurrent()const noexcept {
 if(sourceGroupCold())return true;
 if(!retainedSourceRearm_||!stoppedInstancesCurrent()||paused_||sourceGroupAttempted_)return false;
 for(const auto& s:sounds_)if(s.source||s.latest!=instanceCount||s.latestRevision||s.latestToken!=-1)return false;
 return true;
}
bool OriginalUiGarageWattoAudio50KG::sourceSoundCurrent(const Sound& sound)const noexcept {
 return current()&&sourceSoundIdentityAfterCurrent(sound);
}
bool OriginalUiGarageWattoAudio50KG::sourceSoundIdentityAfterCurrent(const Sound& sound)const noexcept {
 // Keep the genuine source-group, source reference, and exact retained array
 // membership checks. Only the enclosing Group's readonly row pass omits the
 // already completed whole-Audio scan; no source effect occurs between them.
 if(!sourceGroup_||!sourceGroupAttempted_||sourceGroupReleased_||!sound.source||
  sound.source->group34!=sourceGroup_||sound.source->reference00!=1)return false;
 for(const auto& s:sounds_)if(&s==&sound)return true;
 return false;
}
bool OriginalUiGarageWattoAudio50KG::sourceSoundActive(const Sound& s)const noexcept {
 if(!sourceSoundCurrent(s)||s.source->channel2C<0||s.source->channel2C!=s.latestToken||
  s.latest>=instanceCount||!s.latestRevision||playbackRevisions_[s.latest]!=s.latestRevision||
  !mixer_->nativeSourceVoiceReserved(firstVoice+s.latest,s.latestRevision))return false;
 const auto id=soundIds_[s.latest];return id&&id<=soundCount&&&sounds_[id-1]==&s;
}
bool OriginalUiGarageWattoAudio50KG::stopSource233670(Sound& s)noexcept {
 if(!sourceSoundCurrent(s))return false;
 if(sourceGroup_->main_->nativeWetProcessor_&&!sourceGroup_->main_->nativeWetInterfaceCurrent())return false;
 auto& f=*s.source;if(f.channel2C==-1)return true;
 // Source reservation expiration, NOT decoder EOF, selects this branch.
 // Actual267CB0 owns that expiry; no PCM-clock/elapsed-time surrogate exists.
 if(!sourceSoundActive(s)){f.channel2C=-1;return true;}
 mixer_->stop(firstVoice+s.latest);f.elapsedBits24=0;f.channel2C=-1;f.word7C=0;
 return true;
}
bool OriginalUiGarageWattoAudio50KG::serviceSource233E00(Sound& s,float dt)noexcept {
 if(!sourceSoundCurrent(s))return false;
 auto& f=*s.source;
 // Source233E00 tests signed channel only, not current PCM activity.
 if(f.channel2C>=0){volatile float elapsed=std::bit_cast<float>(f.elapsedBits24)+dt;
  f.elapsedBits24=std::bit_cast<std::uint32_t>(static_cast<float>(elapsed));}
 return true;
}
bool OriginalUiGarageWattoAudio50KG::releaseSource104970(Sound& s)noexcept {
 if(!sourceSoundCurrent(s)||s.source->channel2C!=-1)return false;
 //233070 flags2 branch sees channel-1 after ordered233670.1027F0/104910
 //does not own any Entity/global GameObject count contribution.
 --s.source->reference00;return true;
}
void OriginalUiGarageWattoAudio50KG::reset() noexcept {
 // A retained real group must run its typed235150 teardown first. Native
 // reset must never silently erase live source references or shared slot.
 if(sourceGroup_&&!sourceGroupReleased_)std::terminate();
 // Every issued Sound binding requires real source Group release plus the
 // matching-epoch all-instance drain. Epoch replacement can leave an old
 // voice array untouched, so invalid current() is NOT proof of retirement.
 for(const auto& sound:sounds_)if(sound.nativeSample.retainsBacking())std::terminate();
 bool retainedVoice=false;for(const auto& binding:nativeVoices_)retainedVoice=retainedVoice||binding.retainsBacking();
 if(retainedVoice&&(!mixer_||mixer_->groupEpoch50IC()!=epoch_||mixer_->count()!=frontendVoices_))std::terminate();
 // Teardown must still drain backing PCM if observation was poisoned by a
 // foreign command. The subgroup reservation is exclusive for its epoch.
 if(mixer_&&mixer_->groupEpoch50IC()==epoch_&&mixer_->count()==frontendVoices_){
  for(std::size_t i=0;i<instanceCount;++i)mixer_->stop(firstVoice+i);
 }
 // All actual reserved slots are now inactive in the proven same epoch.
 // The transport is being retired; no later current()/replay borrows this clip.
 for(auto& binding:nativeVoices_)binding={};
 prepared_=false;poisoned_=true;mixer_=nullptr;sceneAudio_=nullptr;frontendVoices_=0;mode_=nullptr;metadata_.reset();
}
} // namespace rr::platform
