#include "original_audio_sample_cache50kg.h"
#include "original_audio_pending_waves50kg.h"
#include "original_ui_main_audio50kg.h"
#include <bit>
#include <exception>
#include <fstream>
#include <limits>
#include <stdexcept>
namespace rr::platform { namespace {
void need(bool value,const char* text){if(!value)throw std::runtime_error(text);}
void diagnose(std::string& error,const char* text)noexcept{try{error=text;}catch(...){error.clear();}}
bool fail(std::string& error,const char* text)noexcept{diagnose(error,text);return false;}
void authenticRoot(const std::filesystem::path& root){
 need(root.is_absolute(),"native backing requires the actual absolute AUDIO root");
 std::filesystem::path at;
 for(const auto& part:root){need(part!="."&&part!="..","native backing root traversal refused");at/=part;
  need(!std::filesystem::is_symlink(at),"native backing root symlink refused");}
 need(std::filesystem::is_directory(root),"native backing AUDIO root missing");
}
}
OriginalAudioSampleCache50KG::~OriginalAudioSampleCache50KG()noexcept {
 if(initialized_&&!closed_)std::terminate(); // No destructor eviction or guest clear.
}
std::shared_ptr<OriginalAudioSampleCache50KG> OriginalAudioSampleCache50KG::prepareNativeBacking(
 const std::shared_ptr<OriginalAudioPendingWaves50KG>& owner,
 std::shared_ptr<const assets::OriginalAudioDirectory50KX> directory,
 const std::filesystem::path& root,std::string& error)noexcept {
 try {
  need(owner&&owner->current()&&directory&&directory->prepared(),"native backing namespace/directory unavailable");
  authenticRoot(root);
  auto result=std::shared_ptr<OriginalAudioSampleCache50KG>(new OriginalAudioSampleCache50KG);
  result->namespace_=owner;result->directory_=std::move(directory);result->audioRoot_=root;
  result->self_=result;result->initialized_=true;error.clear();return result;
 }catch(const std::exception& e){diagnose(error,e.what());}catch(...){diagnose(error,"native backing owner allocation failed");}
 return {};
}
bool OriginalAudioSampleCache50KG::current()const noexcept {
 if(ownerThread_!=std::this_thread::get_id()||!initialized_||closed_||!generation_)return false;
 const auto owner=namespace_.lock();
 return owner&&owner->nativeBackingIdentityCurrent(*this,audioRoot_)&&directory_&&directory_->prepared();
}
bool OriginalAudioSampleCache50KG::owns(const NativePcmLease& lease)const noexcept {
 const auto cache=lease.cache_.lock();const auto self=self_.lock();
 if(!current()||cache.get()!=this||!self||cache.owner_before(self)||self.owner_before(cache)||lease.generation_!=generation_||!lease.backing_)return false;
 const auto matches=[&](const std::shared_ptr<const Backing>& b){return b&&lease.backing_.get()==b.get()&&!lease.backing_.owner_before(b)&&!b.owner_before(lease.backing_);};
 if(matches(wing_))return true;
 for(const auto& b:scene_)if(matches(b))return true;
 return false;
}
bool OriginalAudioSampleCache50KG::NativePcmLease::current()const noexcept {
 const auto cache=cache_.lock();return cache&&cache->owns(*this);
}
bool OriginalAudioSampleCache50KG::NativePcmLease::inspect(Observation& out)const noexcept {
 if(!current())return false;
 const auto& b=*backing_;
 out={b.directory.get(),b.row->sourceIndex,b.encodedBytes,b.payloadBytes,b.pcm.sampleRate,
      b.pcm.frames,generation_,b.milliseconds,b.durationBits,b.pitch};return true;
}
bool OriginalAudioSampleCache50KG::NativePcmLease::sameBacking(const NativePcmLease& other)const noexcept {
 return current()&&other.current()&&backing_.get()==other.backing_.get()&&
  !backing_.owner_before(other.backing_)&&!other.backing_.owner_before(backing_);
}
bool OriginalAudioSampleCache50KG::acquireWingBacking(
 const assets::OriginalAudioDirectoryRow50KX& row,NativePcmLease& out,std::string& error)noexcept {
 if(!current()||directory_->find("audio\\sfx\\ui\\watwing3.vag")!=&row)
  return fail(error,"native backing requires exact authenticated wing row and root identity");
 try {
  if(!wing_){
   auto path=audioRoot_;
   for(const auto* part:{"SFX","UI","WATWING3.VAG"}){path/=part;need(!std::filesystem::is_symlink(path),"native wing backing symlink refused");}
   need(std::filesystem::is_regular_file(path),"native wing backing file missing");
   std::ifstream file(path,std::ios::binary|std::ios::ate);
   need(file.is_open()&&file.tellg()==2384,"native wing backing original extent differs");
   std::vector<std::uint8_t> bytes(2384);file.seekg(0);file.read(reinterpret_cast<char*>(bytes.data()),2384);
   need(file.good()&&file.peek()==std::char_traits<char>::eof(),"native wing backing exact read failed");
   auto next=std::make_shared<Backing>();std::string detail;
   need(assets::decodeVagMono50HC(bytes.data(),bytes.size(),next->pcm,detail),"native wing backing decode failed");
   const auto& pcm=next->pcm;
   need(pcm.sourceSha256=="bb8aa4e50a3b01a4a8cbbedda4fd5ca7e192ec995c397a843e1a9c34f8112926"&&
    pcm.frames==4060&&pcm.samples.size()==4060&&pcm.sampleRate==22050&&pcm.channels==1&&
    !pcm.hasEmbeddedLoop&&!pcm.loopStart&&!pcm.loopEnd,"native wing backing authentic identity/shape differs");
   const auto be32=[&](std::size_t at){return (std::uint32_t(bytes[at])<<24)|(std::uint32_t(bytes[at+1])<<16)|(std::uint32_t(bytes[at+2])<<8)|bytes[at+3];};
   next->encodedBytes=2384;next->payloadBytes=be32(12);need(next->payloadBytes==2336&&be32(16)==22050,"native wing original encoded metadata differs");
   // Source metadata projection ONLY, not RPC15/16 or source Sound publication.
   next->milliseconds=(next->payloadBytes*1000u)/((pcm.sampleRate/28u)*16u);
   volatile float seconds=static_cast<float>(next->milliseconds)/1000.f;
   next->durationBits=std::bit_cast<std::uint32_t>(static_cast<float>(seconds));
   next->pitch=(pcm.sampleRate<<12)/48000u;
   need(next->milliseconds==185&&next->durationBits==0x3e3d70a4u&&next->pitch==1881,"native wing source metadata projection differs");
   next->directory=directory_;next->row=&row;
   wing_=std::move(next); // All reads, authentication, decoding and allocation complete.
  }
  NativePcmLease next;next.backing_=wing_;next.cache_=self_;next.generation_=generation_;
  out=std::move(next);error.clear();return true;
 }catch(const std::exception& e){diagnose(error,e.what());}catch(...){diagnose(error,"native immutable wing backing allocation failed");}
 return false;
}
bool OriginalAudioSampleCache50KG::NativeSampleBinding::retainsBacking()const noexcept {
 return static_cast<bool>(pcm_.backing_);
}
bool OriginalAudioSampleCache50KG::NativeSampleBinding::current()const noexcept {
 // Check the actual namespace thread/lifetime before touching the backend.
 if(!pcm_.current())return false;
 const auto main=main_.lock();
 if(!main||static_cast<bool>(audio_)==static_cast<bool>(droid_))return false;
 if(droid_){
  if(main->nativeDroidSampleTransport()!=droid_)return false;
  OriginalUiGarageDroidAudio50GN::Observation observed;
  return droid_->inspect(observed)&&observed.mixer==mixer_&&observed.mixerEpoch==epoch_&&
   mixer_&&mixer_->sampleRate()==rate_&&mixer_->count()==voices_;
 }
 if(main->nativeWattoSampleTransport()!=audio_)return false;
 OriginalUiGarageWattoAudio50KG::Observation observed;
 return audio_->inspect(observed)&&observed.mixer==mixer_&&observed.mixerEpoch==epoch_&&
  mixer_&&mixer_->sampleRate()==rate_&&mixer_->count()==voices_;
}
bool OriginalAudioSampleCache50KG::NativeSampleBinding::inspect(Observation& out)const noexcept {
 return current()&&pcm_.inspect(out);
}
bool OriginalAudioSampleCache50KG::NativeSampleBinding::sameBacking(const NativeSampleBinding& other)const noexcept {
 return current()&&other.current()&&pcm_.sameBacking(other.pcm_);
}
bool OriginalAudioSampleCache50KG::NativeSampleBinding::clipFor(
 const OriginalUiGarageWattoAudio50KG* audio,AudioClip50HD& out)const noexcept {
 if(audio!=audio_||droid_||!current())return false;
 const auto& pcm=pcm_.backing_->pcm;
 out={pcm.samples.data(),pcm.frames,pcm.sampleRate,static_cast<std::uint8_t>(pcm.channels),pcm.loopStart,pcm.loopEnd};return true;
}
bool OriginalAudioSampleCache50KG::NativeSampleBinding::clipForResolved(
 const OriginalUiGarageWattoAudio50KG* audio,const NativeSampleHandle& handle,AudioClip50HD& out)const noexcept {
 const auto cache=pcm_.cache_.lock();
 return cache&&cache->bindingMatchesHandle(*this,handle)&&clipFor(audio,out);
}
bool OriginalAudioSampleCache50KG::NativeSampleBinding::clipForRetainedVoice(
 const OriginalUiGarageWattoAudio50KG* audio,AudioClip50HD& out)const noexcept {
 // Called by actual Audio::current: authenticate its native identity without
 // recursively asking that same Audio owner to inspect all of its voices.
 if(audio!=audio_||droid_||!pcm_.current())return false;
 const auto main=main_.lock();
 if(!main||!main->nativeWattoSampleIdentity(audio_,mixer_,epoch_)||
    mixer_->sampleRate()!=rate_||mixer_->count()!=voices_)return false;
 const auto& pcm=pcm_.backing_->pcm;
 out={pcm.samples.data(),pcm.frames,pcm.sampleRate,static_cast<std::uint8_t>(pcm.channels),pcm.loopStart,pcm.loopEnd};return true;
}
bool OriginalAudioSampleCache50KG::bindWingNativeSample(const NativePcmLease& pcm,
 const std::shared_ptr<OriginalUiMainAudio50KG>& main,NativeSampleBinding& out,std::string& error)noexcept {
 if(!owns(pcm)||!main||!wing_||pcm.backing_.get()!=wing_.get())return fail(error,"native wing sample binding requires its exact backing and live Main audio owner");
 const auto* audio=main->nativeWattoSampleTransport();OriginalUiGarageWattoAudio50KG::Observation observed;
 if(!audio||!audio->inspect(observed)||!observed.mixer)
  return fail(error,"native sample binding requires authentic prepared Watto transport");
 NativeSampleBinding next;next.pcm_=pcm;next.main_=main;next.audio_=audio;next.mixer_=observed.mixer;
 next.epoch_=observed.mixerEpoch;next.rate_=observed.mixer->sampleRate();next.voices_=observed.mixer->count();
 if(!next.current())return fail(error,"native sample binding backend changed before publication");
 out=std::move(next);error.clear();return true;
}
bool OriginalAudioSampleCache50KG::owns(const NativeSampleHandle& handle)const noexcept {
 NativePcmLease lease;lease.backing_=handle.backing_.lock();lease.cache_=handle.cache_;lease.generation_=handle.generation_;
 return owns(lease);
}
bool OriginalAudioSampleCache50KG::NativeSampleHandle::current()const noexcept {
 const auto cache=cache_.lock();return cache&&cache->owns(*this);
}
bool OriginalAudioSampleCache50KG::makeNativeHandle(const NativeSampleBinding& binding,NativeSampleHandle& out)const noexcept {
 if(!owns(binding.pcm_)||!binding.current())return false;
 NativeSampleHandle next;next.backing_=binding.pcm_.backing_;next.cache_=self_;next.generation_=generation_;
 out=std::move(next);return true;
}
bool OriginalAudioSampleCache50KG::bindingMatchesHandle(const NativeSampleBinding& binding,const NativeSampleHandle& handle)const noexcept {
 const auto b=handle.backing_.lock();
 return owns(binding.pcm_)&&owns(handle)&&binding.current()&&b&&
  b.get()==binding.pcm_.backing_.get()&&!b.owner_before(binding.pcm_.backing_)&&!binding.pcm_.backing_.owner_before(b);
}
bool OriginalAudioSampleCache50KG::canRetireNativeBacking()const noexcept {
 // Every source Sound and active native voice retains its immutable PCM lease.
 // Never evict because a transport epoch changed while an old voice could borrow it.
 if(!current()||generation_==std::numeric_limits<std::uint64_t>::max()||(wing_&&wing_.use_count()!=1))return false;
 for(const auto& b:scene_)if(b&&b.use_count()!=1)return false;
 return true;
}
bool OriginalAudioSampleCache50KG::resetNativeBacking(std::string& error)noexcept {
 if(!canRetireNativeBacking())return fail(error,"native backing reset refuses stale owner or outstanding PCM leases");
 wing_.reset();for(auto& b:scene_)b.reset();++generation_;error.clear();return true;
}
bool OriginalAudioSampleCache50KG::closeNativeBacking(std::string& error)noexcept {
 if(!canRetireNativeBacking())return fail(error,"native backing final retirement refuses outstanding PCM leases");
 closeNativeBackingPreflighted();error.clear();return true;
}
void OriginalAudioSampleCache50KG::closeNativeBackingPreflighted()noexcept {
 wing_.reset();for(auto& b:scene_)b.reset();++generation_;closed_=true;directory_.reset();
}
bool OriginalAudioSampleCache50KG::canCloseAfterStoppedNativeVoices(
 const OriginalUiMainAudio50KG& main,const NativeSampleBinding* const* bindings,std::size_t count)const noexcept {
 if(!current()||generation_==std::numeric_limits<std::uint64_t>::max()||
    count>OriginalUiMainAudio50KG::finalGarageClipCount||(count&&!bindings))return false;
 const auto root=main.garagePersistentBacking_.lock();
 if(!root||main.finalNativePcmRetired_)return false;
 std::array<long,5> expected{{1,1,1,1,1}};
 const std::array<const std::shared_ptr<const Backing>*,5> canonical{{
  &wing_,&scene_[0],&scene_[1],&scene_[2],&scene_[3]}};
 for(std::size_t n=0;n<count;++n){
  const auto* binding=bindings[n];if(!binding||!owns(binding->pcm_))return false;
  for(std::size_t j=0;j<n;++j)if(bindings[j]==binding)return false;
  const auto owner=binding->main_.lock();
  if(!owner||owner.get()!=&main||owner.owner_before(root)||root.owner_before(owner)||
     binding->mixer_!=main.mixer_||binding->epoch_!=main.mixerEpoch_||
     binding->rate_!=main.mixerRate_||binding->voices_!=main.mixerCount_)return false;
  if(binding->audio_){
   if(binding->droid_||!main.nativeWattoSampleIdentity(binding->audio_,binding->mixer_,binding->epoch_))return false;
  }else if(!binding->droid_||!main.nativeDroidSampleIdentity(binding->droid_,binding->mixer_,binding->epoch_))return false;
  bool found=false;
  for(std::size_t j=0;j<canonical.size();++j){const auto& b=*canonical[j];
   if(b&&b.get()==binding->pcm_.backing_.get()&&
      !b.owner_before(binding->pcm_.backing_)&&!binding->pcm_.backing_.owner_before(b)){
    ++expected[j];found=true;break;
   }
  }
  if(!found)return false;
 }
 // No temporary strong Backing aliases are created by this census. This
 // exact count includes only the canonical owner and each authenticated
 // stopped voice's one binding. An extra source/external copy cannot hide.
 for(std::size_t j=0;j<canonical.size();++j){const auto& b=*canonical[j];
  if(b?b.use_count()!=expected[j]:expected[j]!=1)return false;
 }
 return true;
}
bool OriginalAudioSampleCache50KG::acquireSceneBacking(
 const assets::OriginalAudioDirectoryRow50KX& row,const assets::AudioClip50HC& clip,
 NativePcmLease& out,std::string& error)noexcept {
 struct Source {const char* name;std::uint32_t bytes;const char* sha;};
 static constexpr std::array<Source,4> originals{{
  {"audio\\sfx\\race\\pdwalk01.vag",101312,"e5cbeae6e6dd8d0778cf4d0459b16dc28e90b8a58195d1da1c60321430acf90e"},
  {"audio\\sfx\\race\\onfire01.vag",34112,"2615805c486ccb533ca0e030b1dfa0f5f3f81354d68f7593748d085a87ec7690"},
  {"audio\\sfx\\race\\exting01.vag",38304,"fd0872e6406db57dbda33afa04ec7a998ec68a9facdd5ee1f55bc71d9b225eff"},
  {"audio\\sfx\\race\\pdfall01.vag",147504,"db0cc49ce67d82531f878d07577eb777af32e981a969a41bdb8bcd18910f1334"}
 }};
 if(!current())return fail(error,"Scene sample backing requires the actual current root namespace");
 std::size_t index=originals.size();
 for(std::size_t i=0;i<originals.size();++i)if(directory_->find(originals[i].name)==&row){index=i;break;}
 if(index==originals.size())return fail(error,"Scene sample is not an authenticated authored Droid directory identity");
 const auto& expected=originals[index];const auto& pcm=clip.pcm;
 if(clip.state!=assets::AudioAssetState50HC::ready||clip.sourceBytes!=expected.bytes||clip.sourceSha256!=expected.sha||
    pcm.sourceSha256!=expected.sha||pcm.sampleRate!=22050||pcm.channels!=1||!pcm.frames||pcm.samples.size()!=pcm.frames||
    pcm.hasEmbeddedLoop||pcm.loopStart||pcm.loopEnd)
  return fail(error,"Scene sample source identity or decoded PCM ownership differs");
 try {
  if(!scene_[index]){
   auto next=std::make_shared<Backing>();next->pcm=pcm;next->directory=directory_;next->row=&row;
   // Authenticated original zeroed-header VAG files have exactly48 header bytes.
   // Their source size fields are101264/34064/38256/147456, each at22050Hz.
   next->encodedBytes=expected.bytes;next->payloadBytes=expected.bytes-48;
   next->milliseconds=(next->payloadBytes*1000u)/((pcm.sampleRate/28u)*16u);
   volatile float seconds=static_cast<float>(next->milliseconds)/1000.f;
   next->durationBits=std::bit_cast<std::uint32_t>(static_cast<float>(seconds));
   next->pitch=(pcm.sampleRate<<12)/48000u;
   scene_[index]=std::move(next);
  }
  NativePcmLease next;next.backing_=scene_[index];next.cache_=self_;next.generation_=generation_;
  if(!next.current())return fail(error,"Scene sample backing changed before lease publication");
  out=std::move(next);error.clear();return true;
 }catch(...){return fail(error,"Scene immutable PCM backing allocation failed");}
}
bool OriginalAudioSampleCache50KG::bindSceneNativeSample(const NativePcmLease& pcm,
 const std::shared_ptr<OriginalUiMainAudio50KG>& main,NativeSampleBinding& out,std::string& error)noexcept {
 if(!owns(pcm)||!main)return fail(error,"Scene native binding requires actual retained backing and Main audio");
 bool scene=false;for(const auto& b:scene_)if(b&&b.get()==pcm.backing_.get())scene=true;
 if(!scene)return fail(error,"Scene native binding cannot borrow a Watto sample");
 const auto* audio=main->nativeDroidSampleTransport();OriginalUiGarageDroidAudio50GN::Observation observed;
 if(!audio||!audio->inspect(observed)||!observed.mixer)return fail(error,"Scene native binding requires its actual Droid transport");
 NativeSampleBinding next;next.pcm_=pcm;next.main_=main;next.droid_=audio;next.mixer_=observed.mixer;
 next.epoch_=observed.mixerEpoch;next.rate_=observed.mixer->sampleRate();next.voices_=observed.mixer->count();
 if(!next.current())return fail(error,"Scene native binding changed before publication");
 out=std::move(next);error.clear();return true;
}
bool OriginalAudioSampleCache50KG::NativeSampleBinding::clipForRetainedVoice(
 const OriginalUiGarageDroidAudio50GN* audio,AudioClip50HD& out)const noexcept {
 if(audio!=droid_||audio_||!pcm_.current())return false;
 const auto main=main_.lock();
 if(!main||!main->nativeDroidSampleIdentity(droid_,mixer_,epoch_)||!mixer_||
    mixer_->sampleRate()!=rate_||mixer_->count()!=voices_)return false;
 const auto& pcm=pcm_.backing_->pcm;
 out={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,pcm.loopStart,pcm.loopEnd};return true;
}
bool OriginalAudioSampleCache50KG::NativeSampleBinding::clipForResolved(
 const OriginalUiGarageDroidAudio50GN* audio,const NativeSampleHandle& handle,AudioClip50HD& out)const noexcept {
 const auto cache=pcm_.cache_.lock();
 return cache&&cache->bindingMatchesHandle(*this,handle)&&clipForRetainedVoice(audio,out);
}
}
