#include <bit>
#include "original_audio_pending_waves50kg.h"
#include "original_ui_garage_audio_backing50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <exception>
#include <limits>

namespace rr::platform {
namespace {
template<class T> bool sameShared(const std::shared_ptr<T>& a,const std::shared_ptr<T>& b)noexcept {
 return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
bool fail(std::string& error,const char* text)noexcept {try{error=text;}catch(...){error.clear();}return false;}
}
OriginalAudioPendingWaves50KG::~OriginalAudioPendingWaves50KG()noexcept {
 // No made-up232EB0 clear on destruction. Actual2668E0 does not walk row+4;
 // this stricter native envelope prevents dangling borrows, not source retain.
 if(revision_&&!closed_)std::terminate();
}
OriginalAudioPendingWaves50KG::RootOperation::RootOperation(
 const Application& issuer,const std::shared_ptr<Authority>& authority,
 Kind kind)noexcept {
 if(!authority||authority->nativeOwnerThread_!=ownerThread_||!authority->constructed_||
    authority->released_||!sameShared(issuer.entities(),authority))return;
 auto lifetime=issuer.animationClock();std::uint32_t pause=0;if(!lifetime.readPause11C(pause))return;
 issuer_=&issuer;authority_=authority;lifetime_=lifetime;kind_=kind;
}
std::shared_ptr<OriginalAudioPendingWaves50KG> OriginalAudioPendingWaves50KG::prepare232EA0(
 const RootOperation& operation,std::shared_ptr<const Directory> directory,std::string& error)noexcept {
 // Reject affinity before reading actual Application/clock mutable storage.
 const auto authority=operation.authority();
 if(operation.ownerThread()!=std::this_thread::get_id()||!authority||
    authority->nativeOwnerThread_!=std::this_thread::get_id()||!operation.issuer()||
    operation.kind()!=RootOperation::Kind::construct232EA0){
  fail(error,"232EA0 actual Application authority/thread unavailable");return {};
 }
 auto lifetime=operation.lifetime();std::uint32_t pause=0;
 if(!lifetime.readPause11C(pause)){
  fail(error,"232EA0 actual Application lifetime has ended");return {};
 }
 const auto& issuer=*operation.issuer();
 if(!authority->constructed_||authority->released_||!sameShared(issuer.entities(),authority)||
    !lifetime.sameOwner(issuer.animationClock())||
    authority->pendingWaves98A4_||authority->pendingWavesCreated_){
  fail(error,"232EA0 requires the same live, never-issued Application namespace");return {};
 }
 if(!directory||!directory->prepared()||
    directory->size()!=assets::originalAudioDirectoryRows50KX){
  fail(error,"232EA0 requires real Application lifetime and authenticated267-row AudioStr owner");return {};
 }
 for(std::size_t i=0;i<directory->size();++i){const auto* row=directory->row(i);
  if(!row||row->sourceIndex!=i||directory->find(row->sourceName)!=row){
   fail(error,"232EA0 authenticated source directory identity/ordering changed");return {};
  }
 }
 try {
  auto next=std::shared_ptr<OriginalAudioPendingWaves50KG>(new OriginalAudioPendingWaves50KG);
  next->authority_=authority;next->issuer_=&issuer;next->issuerLifetime_=lifetime;
  next->directory_=std::move(directory);
  // No callback/failable step after publication. This host issuance ledger
  // is not an original guest field or a claim that all reconstruction is impossible.
  next->revision_=1;authority->pendingWavesCreated_=true;authority->pendingWaves98A4_=next;
  error.clear();return next;
 }catch(...){fail(error,"232EA0 native pending namespace allocation failed before publication");return {};}
}
std::shared_ptr<OriginalAudioPendingWaves50KG> OriginalAudioPendingWaves50KG::retainForRoot(
 const RootOperation& operation,std::string& error)noexcept {
 // Reject affinity before reading the root publication or Application state.
 if(operation.ownerThread()!=std::this_thread::get_id()||!operation.issuer()){
  fail(error,"pending namespace retain requires exact live root/thread");return {};
 }
 const auto authority=operation.authority();
 if(!authority||authority->nativeOwnerThread_!=std::this_thread::get_id()){
  fail(error,"pending namespace retain has no actual root authority");return {};
 }
 auto owner=authority->pendingWaves98A4_;
 if(!owner||!owner->sameRoot(operation)){
  fail(error,"pending namespace is not published by this live Application");return {};
 }
 error.clear();return owner;
}
bool OriginalAudioPendingWaves50KG::applicationMayRetire(const Application& application)noexcept {
 // Read-only destruction guard, not a mutation capability. A correctly
 // retired authority may already have released its base Entities; that must
 // not turn completed cleanup into a destructor failure. Check affinity
 // before reading mutable publication storage, just as the Video root guard.
 const auto& authority=application.entities();
 return authority&&authority->nativeOwnerThread_==std::this_thread::get_id()&&
  !authority->pendingWaves98A4_;
}
bool OriginalAudioPendingWaves50KG::current()const noexcept {
 if(ownerThread_!=std::this_thread::get_id()||closed_||!revision_||!issuer_||!directory_)return false;
 // A clock view pins lifetime storage, NOT the raw Application. Test it before
 // dereferencing issuer_, including possible same-address Application reuse.
 std::uint32_t pause=0;if(!issuerLifetime_.readPause11C(pause))return false;
 const auto authority=authority_.lock();
 return authority&&authority->nativeOwnerThread_==ownerThread_&&authority->constructed_&&!authority->released_&&
  authority->pendingWavesCreated_&&authority->pendingWaves98A4_.get()==this&&
  sameShared(issuer_->entities(),authority)&&issuerLifetime_.sameOwner(issuer_->animationClock())&&
  directory_->prepared()&&directory_->size()==rows_.size();
}
bool OriginalAudioPendingWaves50KG::nativeBackingIdentityCurrent(
 const OriginalAudioSampleCache50KG& cache,const std::filesystem::path& root)const noexcept {
 return current()&&nativeSamples_.get()==&cache&&issuer_->audioRoot==root&&sameShared(directory_,cache.directory_);
}
bool OriginalAudioPendingWaves50KG::prepareNativeSampleBacking(const RootOperation& operation,std::string& error)noexcept {
 if(!sameRoot(operation)||nativeUnresolvedSceneAbortRevision_||operation.kind()!=RootOperation::Kind::nativeBackingPrepare||nativeSamples_)
  return fail(error,"native backing preparation requires exact sealed root and an unissued child");
 auto owner=authority_.lock()->pendingWaves98A4_;
 auto next=OriginalAudioSampleCache50KG::prepareNativeBacking(owner,directory_,issuer_->audioRoot,error);
 if(!next)return false;
 nativeSamples_=std::move(next);error.clear();return true;
}
bool OriginalAudioPendingWaves50KG::acquireNativeWingBacking(Group& group,
 OriginalAudioSampleCache50KG::NativePcmLease& lease,std::string& error)noexcept {
 if(!current()||nativeUnresolvedSceneAbortRevision_||!nativeSamples_||!group.current()||!group.grant_||group.grant_->issuer()!=issuer_||
    !sameShared(group.grant_->application(),authority_.lock())||group.pendingWaves_.get()!=this||!group.audio_)
  return fail(error,"native backing request requires genuine registered same-root wing owner");
 const auto& sound=group.audio_->sounds_[OriginalUiGarageWattoAudio50KG::responseCount];
 if(!group.count48_||group.members4C_[0]!=&sound||!group.audio_->sourceSoundCurrent(sound)||
    !sound.source->resourceA0||sound.source->flags10!=1||sound.source->stream1A8||sound.source->stereo1AC||sound.source->loop1B0)
  return fail(error,"native backing request has no genuine registered mono/nonstream wing");
 const auto* row=directory_->row(*sound.source->resourceA0);
 if(!row||directory_->find("audio\\sfx\\ui\\watwing3.vag")!=row)
  return fail(error,"native backing request A0 is not the authentic wing directory identity");
 return nativeSamples_->acquireWingBacking(*row,lease,error);
}
bool OriginalAudioPendingWaves50KG::bindNativeWingSample(Group& group,
 OriginalAudioSampleCache50KG::NativeSampleBinding& out,std::string& error)noexcept {
 OriginalAudioSampleCache50KG::NativePcmLease pcm;
 if(!acquireNativeWingBacking(group,pcm,error))return false;
 // Same registered source Sound/root and actual retained Main backend. This
 // does not write sample28, resolve the pending row, or authorize reverb/play.
 return nativeSamples_->bindWingNativeSample(pcm,group.main_,out,error);
}
bool OriginalAudioPendingWaves50KG::flushNativeWing26D9F0(const RootOperation& operation,std::string& error)noexcept {
 if(!sameRoot(operation)||nativeUnresolvedSceneAbortRevision_||operation.kind()!=RootOperation::Kind::nativeWingResolve26D9F0||!nativeSamples_)
  return fail(error,"26D9F0 native resolution requires exact sealed root operation and real cache");
 const auto* authentic=directory_->find("audio\\sfx\\ui\\watwing3.vag");
 if(!authentic)return fail(error,"26D9F0 authentic wing directory identity missing");
 const auto ordinal=authentic->sourceIndex;
 // This bounded positive loader supports only the genuine registered wing.
 // A different pending asset is not skipped, fabricated or partially flushed.
 for(std::size_t i=0;i<rows_.size();++i)if(i!=ordinal&&
  (rows_[i].count8||rows_[i].hasRepresentative()))
  return fail(error,"26D9F0 another pending asset has no native sample producer");
 auto& row=rows_[ordinal];
 if(!row.count8){
  if(row.hasRepresentative())return fail(error,"26D9F0 inconsistent pending representative");
  error.clear();return true;
 }
 if(row.count8>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())||
    revision_==std::numeric_limits<std::uint64_t>::max()||!row.representative4||!row.representativeGroup||row.sceneRepresentative||row.sceneRepresentativeOwner)
  return fail(error,"26D9F0 pending representative/revision unavailable");
 bool registeredOwner=false;
 for(const auto& owner:groupBorrows_)if(owner.watto==row.representativeGroup&&!owner.scene)registeredOwner=true;
 if(!registeredOwner)return fail(error,"26D9F0 latest representative has no retained source Group borrow");
 auto& group=*row.representativeGroup;
 OriginalAudioSampleCache50KG::NativeSampleBinding binding;
 if(!bindNativeWingSample(group,binding,error))return false;
 auto& sound=group.audio_->sounds_[OriginalUiGarageWattoAudio50KG::responseCount];
 OriginalAudioSampleCache50KG::Observation observed;
 OriginalAudioSampleCache50KG::NativeSampleHandle handle;
 if(row.representative4!=&sound||sound.source->nativeSample28||!binding.inspect(observed)||
    observed.sourceOrdinal!=ordinal||!nativeSamples_->makeNativeHandle(binding,handle))
  return fail(error,"26D9F0 latest genuine Sound/native result preflight rejected");
 // Native lifetime is secured BEFORE source-visible publication. The source
 //2332F0 success order is28,20,50,40,4C, then26D9F0 row+4, then count8 reset.
 sound.nativeSample=std::move(binding);
 sound.source->nativeSample28=handle;
 sound.source->duration20=std::bit_cast<float>(observed.observedSourceDurationBits);
 sound.source->basePitch50=static_cast<std::int32_t>(observed.observedSourcePitch);
 sound.source->pitch40=sound.source->basePitch50;sound.source->pitch4C=sound.source->basePitch50;
 row.resolvedNativeSample=handle;row.representative4=nullptr;row.representativeGroup=nullptr;
 row.count8=0;++revision_;error.clear();return true;
}
bool OriginalAudioPendingWaves50KG::resolveNativeWing233E90(Group& group,std::string& error)noexcept {
 if(!current()||nativeUnresolvedSceneAbortRevision_||!nativeSamples_||!group.current()||!group.grant_||group.grant_->issuer()!=issuer_||
    !sameShared(group.grant_->application(),authority_.lock())||group.pendingWaves_.get()!=this||!group.audio_)
  return fail(error,"233E90 requires the actual registered same-root Sound owner");
 auto& sound=group.audio_->sounds_[OriginalUiGarageWattoAudio50KG::responseCount];
 if(!group.count48_||group.members4C_[0]!=&sound||!group.audio_->sourceSoundCurrent(sound)||
    !sound.source->resourceA0||sound.source->flags10!=1||sound.source->stream1A8||sound.source->stereo1AC||sound.source->loop1B0)
  return fail(error,"233E90 actual nonstream wing identity unavailable");
 // Source already-positive sample28 is a no-op, not a repeated metadata write.
 if(sound.source->nativeSample28){
  if(!nativeSamples_->bindingMatchesHandle(sound.nativeSample,*sound.source->nativeSample28))
   return fail(error,"233E90 existing native result is stale; no replacement fabricated");
  error.clear();return true;
 }
 const auto ordinal=*sound.source->resourceA0;
 if(ordinal>=rows_.size()||directory_->row(ordinal)!=directory_->find("audio\\sfx\\ui\\watwing3.vag"))
  return fail(error,"233E90 source A0 is not authentic wing identity");
 const auto& row=rows_[ordinal];
 if(row.count8||row.hasRepresentative()||!row.resolvedNativeSample||!row.resolvedNativeSample->current())
  return fail(error,"233E90 directory has no resolved native sample; no pending pointer reinterpreted");
 OriginalAudioSampleCache50KG::NativeSampleBinding binding;
 OriginalAudioSampleCache50KG::Observation observed;
 if(!bindNativeWingSample(group,binding,error)||!nativeSamples_->bindingMatchesHandle(binding,*row.resolvedNativeSample)||!binding.inspect(observed))
  return fail(error,"233E90 resolved sample could not bind actual target backend");
 sound.nativeSample=std::move(binding);sound.source->nativeSample28=*row.resolvedNativeSample;
 sound.source->duration20=std::bit_cast<float>(observed.observedSourceDurationBits);
 sound.source->basePitch50=static_cast<std::int32_t>(observed.observedSourcePitch);
 sound.source->pitch40=sound.source->basePitch50;sound.source->pitch4C=sound.source->basePitch50;
 error.clear();return true;
}
bool OriginalAudioPendingWaves50KG::resetNativeSampleBacking(const RootOperation& operation,std::string& error)noexcept {
 if(!sameRoot(operation)||nativeUnresolvedSceneAbortRevision_||operation.kind()!=RootOperation::Kind::nativeBackingReset17D858||!nativeSamples_)
  return fail(error,"native backing reset requires exact sealed root17D858 boundary");
 // ONLY native backing retirement. Full268300 channel/cache/RPC35 remains
 // unjoined; pending clear26D960 is a different operation and never calls this.
 return nativeSamples_->resetNativeBacking(error);
}
bool OriginalAudioPendingWaves50KG::sameRoot(const RootOperation& operation)const noexcept {
 return operation.ownerThread()==std::this_thread::get_id()&&current()&&
  operation.issuer()==issuer_&&sameShared(authority_.lock(),operation.authority())&&
  issuerLifetime_.sameOwner(operation.lifetime());
}
std::size_t OriginalAudioPendingWaves50KG::rowCount()const noexcept {return current()?rows_.size():0;}
const assets::OriginalAudioDirectoryRow50KX* OriginalAudioPendingWaves50KG::lookup26DAD0(
 std::string_view name)const noexcept {
 // Existing full-name ASCII case-insensitive308F00 comparison; no normalization,
 // resource insertion, basename fallback or caller-chosen source ordinal.
 return current()?directory_->find(name):nullptr;
}
bool OriginalAudioPendingWaves50KG::inspect(std::uint32_t index,RowObservation& out)const noexcept {
 if(!current()||index>=rows_.size())return false;
 const auto& row=rows_[index];
 out={directory_->row(index),row.representative4,row.representativeGroup,row.count8,revision_,row.resolvedNativeSample.has_value(),row.resolvedNativeSample&&row.resolvedNativeSample->current(),
  row.sceneRepresentativeOwner,row.sceneRepresentative?row.sceneRepresentative->ordinal:4,
  row.nativeUnresolvedSceneReceipt.has_value()};return true;
}
bool OriginalAudioPendingWaves50KG::registerWattoWing2332A0(Group& group,std::string& error)noexcept {
 if(!current()||nativeUnresolvedSceneAbortRevision_||revision_==std::numeric_limits<std::uint64_t>::max()||!group.current()||!group.grant_||
    group.grant_->issuer()!=issuer_||!sameShared(group.grant_->application(),authority_.lock())||
    !group.audio_||group.slot40_<0||static_cast<std::size_t>(group.slot40_)>=groupBorrows_.size()||
    (group.pendingWaves_&&group.pendingWaves_.get()!=this))
  return fail(error,"2332A0 pending registration requires exact live Application/group/namespace");
 auto& sound=group.audio_->sounds_[OriginalUiGarageWattoAudio50KG::responseCount];
 if(!group.count48_||group.members4C_[0]!=&sound||!group.audio_->sourceSoundCurrent(sound)||
    sound.source->flags10!=1||sound.source->stream1A8||sound.source->stereo1AC||sound.source->loop1B0)
  return fail(error,"2332A0 requires the genuine first nonstream WatWing3 source Sound");
 // Application17C7BC..C4 supplies audio\\; actual wing factory supplies
 // SFX\\UI\\watwing3.vag. The authenticated PCM owner confirms the same asset.
 const auto* resource=directory_->find("audio\\sfx\\ui\\watwing3.vag");
 if(!resource||resource->sourceIndex>=rows_.size()||
    (sound.source->resourceA0&&*sound.source->resourceA0!=resource->sourceIndex))
  return fail(error,"26DAD0 actual wing AudioStr lookup or retained A0 identity failed");
 auto& row=rows_[resource->sourceIndex];
 //298AE4/AE8/AEC uses LW/ADDIU/SW: source32-bit wrap is nontrapping.
 //The ORIGINAL flush selects signed count>0. This bounded NATIVE admission
 //envelope rejects before signed-positive overflow; it does NOT implement or
 //claim source wrap. Borrowed-pointer lifetime never depends on count>0.
 if(row.count8>=static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))
  return fail(error,"26DAF0 historical registration count cannot remain signed-positive");
 auto& groupBorrow=groupBorrows_[static_cast<std::size_t>(group.slot40_)];
 if(groupBorrow.scene||(groupBorrow.watto&&groupBorrow.watto!=&group))return fail(error,"2332A0 claimed group slot has a different native lifetime borrow");
 const auto authority=authority_.lock();
 if(!authority)return fail(error,"2332A0 actual Application lifetime unavailable");
 // Preflight complete. Preserve2332A0→298A30→298F80 order. No source reference
 // increment, old representative release, sample publication or PCM command.
 sound.source->resourceA0=resource->sourceIndex;
 //298A40..5C is unconditional on a VALID count operation too. Preserve the
 //actual process initializer after2332A0 storesA0 and before298A30 count+1.
 if(!authority->pendingCountGuard8CB0_){
  authority->pendingCountAssertion4435F0_=1;
  authority->pendingCountGuard8CB0_=1;
 }
 ++row.count8;
 row.resolvedNativeSample.reset();row.representative4=&sound;row.representativeGroup=&group;
 row.sceneRepresentative=nullptr;row.sceneRepresentativeOwner=nullptr;groupBorrow.watto=&group;++revision_;error.clear();return true;
}
bool OriginalAudioPendingWaves50KG::registerSceneSound2332A0(
 Scene& scene,std::size_t ordinal,std::string_view originalName,std::string& error)noexcept {
 if(!current()||nativeUnresolvedSceneAbortRevision_||revision_==std::numeric_limits<std::uint64_t>::max()||!scene.currentPrefix()||
    !scene.grant_||scene.grant_->issuer()!=issuer_||!sameShared(scene.grant_->application(),authority_.lock())||
    scene.pendingWaves_.get()!=this||!scene.backend_||ordinal>=scene.group4D8_.count48_||
    scene.group4D8_.slot40_<0||static_cast<std::size_t>(scene.group4D8_.slot40_)>=groupBorrows_.size())
  return fail(error,"2332A0 requires the actual same-root typed Scene SoundGroup");
 auto& sound=*scene.group4D8_.sounds_[ordinal];
 if(!scene.sourceRowIdentityCurrent(*scene.backend_,ordinal,false)||sound.sourceRegistered||sound.resourceA0||
    originalName.empty()||originalName.size()>259||sound.nativeSample28||sound.nativeSound)
  return fail(error,"233110 requires a fresh genuine nonstream Scene Sound");
 try {
  const std::string lookup="audio\\"+std::string(originalName);
  const auto* resource=directory_->find(lookup);
  if(!resource||resource->sourceIndex>=rows_.size())
   return fail(error,"26DAD0 actual Scene callback resource is absent from authenticated AudioStr");
  auto& row=rows_[resource->sourceIndex];auto& borrow=groupBorrows_[static_cast<std::size_t>(scene.group4D8_.slot40_)];
  if(row.count8>=static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())||
     borrow.watto||(borrow.scene&&borrow.scene!=&scene))
   return fail(error,"2332A0 Scene count/actual shared group borrow preflight refused");
  const auto authority=authority_.lock();if(!authority)return fail(error,"2332A0 actual Scene root lifetime ended");
  //2332A0 ->26DAF0 ->26DB00. No source reference increment, sample28,
  //PCM publication, clear or flush is implied by this successful loader.
  sound.resourceA0=resource->sourceIndex;
  if(!authority->pendingCountGuard8CB0_){authority->pendingCountAssertion4435F0_=1;authority->pendingCountGuard8CB0_=1;}
  ++row.count8;row.resolvedNativeSample.reset();
  row.representative4=nullptr;row.representativeGroup=nullptr;
  row.sceneRepresentative=&sound;row.sceneRepresentativeOwner=&scene;borrow.scene=&scene;
  ++revision_;error.clear();return true;
 }catch(...){return fail(error,"Scene pending lookup allocation failed before publication; source prefix retained");}
}
bool OriginalAudioPendingWaves50KG::acquireNativeSceneBacking(Scene& scene,std::size_t ordinal,
 OriginalAudioSampleCache50KG::NativePcmLease& out,std::string& error)noexcept {
 if(!current()||nativeUnresolvedSceneAbortRevision_||!nativeSamples_||!scene.currentPrefix()||scene.pendingWaves_.get()!=this||
    !scene.grant_||scene.grant_->issuer()!=issuer_||!sameShared(scene.grant_->application(),authority_.lock())||
    !scene.backend_||!scene.sourceRowIdentityCurrent(*scene.backend_,ordinal,true))
  return fail(error,"native Scene backing requires actual registered same-root Sound owner");
 const auto& sound=*scene.group4D8_.sounds_[ordinal];
 const auto* resource=directory_->row(*sound.resourceA0);
 if(!resource||!sound.authenticatedSource)return fail(error,"native Scene backing has no authenticated A0/clip identity");
 return nativeSamples_->acquireSceneBacking(*resource,*sound.authenticatedSource,out,error);
}
bool OriginalAudioPendingWaves50KG::bindNativeSceneSample(Scene& scene,std::size_t ordinal,
 OriginalAudioSampleCache50KG::NativeSampleBinding& out,std::string& error)noexcept {
 OriginalAudioSampleCache50KG::NativePcmLease pcm;
 return acquireNativeSceneBacking(scene,ordinal,pcm,error)&&nativeSamples_->bindSceneNativeSample(pcm,scene.main_,out,error);
}
bool OriginalAudioPendingWaves50KG::flushNativeScene26D9F0(
 const RootOperation& operation,Scene& scene,std::string& error)noexcept {
 if(!sameRoot(operation)||nativeUnresolvedSceneAbortRevision_||operation.kind()!=RootOperation::Kind::nativeSceneResolve1D1B9C||!nativeSamples_||
    !scene.currentPrefix()||scene.pendingWaves_.get()!=this||!scene.backend_||
    !scene.backend_->sourceConstructionComplete()||scene.group4D8_.count48_!=OriginalUiGarageDroidAudio50GN::soundCount)
  return fail(error,"later1D1B9C native Scene resolution requires exact root and complete source registrations");
 const auto slot=scene.group4D8_.slot40_;
 if(slot<0||static_cast<std::size_t>(slot)>=groupBorrows_.size()||
    groupBorrows_[slot].scene!=&scene||groupBorrows_[slot].watto)
  return fail(error,"26D9F0 genuine Scene group borrow unavailable");
 for(std::size_t ordinal=0;ordinal<scene.group4D8_.sounds_.size();++ordinal){
  if(!scene.sourceRowIdentityCurrent(*scene.backend_,ordinal,true))
   return fail(error,"26D9F0 exact Scene source row is unavailable");
  const auto& sound=*scene.group4D8_.sounds_[ordinal];const auto& row=rows_[*sound.resourceA0];
  if(sound.nativeSample28){
   if(row.count8||row.hasRepresentative()||!row.resolvedNativeSample||
      !nativeSamples_->bindingMatchesHandle(sound.nativeSample,*sound.nativeSample28)||
      !nativeSamples_->bindingMatchesHandle(sound.nativeSample,*row.resolvedNativeSample))
    return fail(error,"26D9F0 earlier committed Scene result is stale, not replayable");
  }else if(!row.count8||row.sceneRepresentative!=&sound||row.sceneRepresentativeOwner!=&scene)
   return fail(error,"26D9F0 unresolved Scene Sound lost its genuine pending publication");
 }
 // Preflight the entire shared directory. Never skip an unsupported pending
 // asset or reinterpret a Watto pointer as Scene. Earlier1D4688 wing flush
 // and later1D1B9C are distinct source boundaries, even with the same engine.
 std::size_t unresolved=0;
 for(std::size_t i=0;i<rows_.size();++i){const auto& row=rows_[i];
  if(!row.count8){if(row.hasRepresentative())return fail(error,"26D9F0 zero count has a dangling representative");continue;}
  if(row.count8>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())||
     row.representative4||row.representativeGroup||!row.sceneRepresentative||row.sceneRepresentativeOwner!=&scene)
   return fail(error,"26D9F0 pending asset has no supported genuine Scene sample producer");
  const auto ordinal=row.sceneRepresentative->ordinal;
  if(ordinal>=scene.group4D8_.sounds_.size()||scene.group4D8_.sounds_[ordinal].get()!=row.sceneRepresentative||
     !scene.sourceRowIdentityCurrent(*scene.backend_,ordinal,true)||
     !row.sceneRepresentative->resourceA0||*row.sceneRepresentative->resourceA0!=i||
     row.sceneRepresentative->nativeSample28||row.sceneRepresentative->nativeSound)
   return fail(error,"26D9F0 latest Scene representative identity is inconsistent");
  ++unresolved;
 }
 if(unresolved>std::numeric_limits<std::uint64_t>::max()-revision_)
  return fail(error,"26D9F0 native revision capacity unavailable before publication");
 // Genuine directory order, not actor/slot order. Native producer failure
 // retains each earlier committed row and this exact unresolved suffix.
 for(std::size_t i=0;i<rows_.size();++i){auto& row=rows_[i];if(!row.count8)continue;
  auto& sound=*row.sceneRepresentative;const auto ordinal=sound.ordinal;
  OriginalAudioSampleCache50KG::NativeSampleBinding binding;
  OriginalAudioSampleCache50KG::NativeSampleHandle handle;
  OriginalAudioSampleCache50KG::Observation observed;
  if(!bindNativeSceneSample(scene,ordinal,binding,error)||!binding.inspect(observed)||
     observed.sourceOrdinal!=i||!nativeSamples_->makeNativeHandle(binding,handle))
   return fail(error,"26D9F0 native Scene sample acquisition refused; exact prefix retained");
  sound.nativeSample=binding; //Strong backing BEFORE any native borrowed clip.
  if(!scene.backend_->bindResolvedSourceRow(ordinal,binding,handle,error))return false;
  // No failable calls after native row admission. Source2332F0 order remains
  // sample28, duration20, basePitch50, pitch40, pitch4C, then row+4/count8.
  auto& native=scene.backend_->rows_[ordinal];sound.nativeSound=native.sound;
  sound.nativeSample28=handle;
  sound.duration20=std::bit_cast<float>(observed.observedSourceDurationBits);
  native.sourceDuration20=sound.duration20;
  sound.basePitch50=static_cast<std::int32_t>(observed.observedSourcePitch);
  native.basePitch50=sound.basePitch50;
  native.sound->pitch40=sound.basePitch50;native.sound->pitch4C=sound.basePitch50;
  sound.sampleContinuationPending=false;++scene.backend_->revision_;
  row.resolvedNativeSample=handle;
  row.sceneRepresentative=nullptr;row.sceneRepresentativeOwner=nullptr;
  row.count8=0;++revision_;
 }
 error.clear();return true;
}
bool OriginalAudioPendingWaves50KG::unresolvedSceneNativeRowsCurrent(const Scene& scene)const noexcept {
 // This pure row half is usable on both sides of the real Group registry
 // release. Only canAbort checks its live registry; only commit accepts ref0.
 if(!current()||nativeUnresolvedSceneAbortRevision_||
    revision_==std::numeric_limits<std::uint64_t>::max()||
    !scene.constructed_||!scene.sceneLive_||!scene.groupLive_||!scene.main_||
    !scene.grant_||!scene.grant_->current()||scene.grant_->issuer()!=issuer_||
    !sameShared(scene.grant_->application(),authority_.lock())||
    scene.pendingWaves_.get()!=this||!scene.backend_||
    scene.backend_->sourceSceneGroup_!=&scene)return false;
 const auto& group=scene.group4D8_;const auto& entity=group.entity;
 const auto address=[](const auto* p)noexcept{return reinterpret_cast<std::uintptr_t>(p);};
 if(group.slot40_<0||static_cast<std::size_t>(group.slot40_)>=groupBorrows_.size()||
    groupBorrows_[group.slot40_].scene!=&scene||groupBorrows_[group.slot40_].watto||
    group.count48_!=4||group.sounds_.size()!=4||!group.members4C_||
    group.capacity54_!=8||group.growth50_!=16||group.manager5C!=scene.main_.get()||
    !group.enabled_||group.volume38_!=100||group.pan3C_||group.state44_||
    entity.kind!=Scene::K::soundGroup2350D0||!scene.grant_->generation_||!scene.grant_->epoch_||
    entity.sessionGeneration!=scene.grant_->generation_||entity.objectKey!=address(&entity)||
    entity.parent1C||entity.previous10||entity.next14||entity.firstChild18||entity.mask20||entity.descendants24||
    scene.podScene_.sessionGeneration!=scene.grant_->generation_||
    scene.podScene_.objectKey!=address(&scene.podScene_))return false;
 for(std::size_t i=0;i<groupBorrows_.size();++i)
  if(i!=static_cast<std::size_t>(group.slot40_)&&groupBorrows_[i].scene==&scene)return false;
 std::array<std::uint32_t,4> resources{};
 for(std::size_t ordinal=0;ordinal<group.sounds_.size();++ordinal){
  const auto* sound=group.sounds_[ordinal].get();
  if(!sound||group.members4C_[ordinal]!=sound||sound->group34!=&group||
     sound->manager30!=scene.main_.get()||sound->ordinal!=ordinal||
     sound->reference00!=1||sound->flags10!=1||sound->channel2C!=-1||
     !sound->sourceRegistered||!sound->sampleContinuationPending||!sound->authenticatedSource||
     sound->stream1A8||sound->stereo1AC||sound->loop1B0||
     !sound->resourceA0||*sound->resourceA0>=rows_.size()||sound->nativeSample28||sound->nativeSound)return false;
  const auto resource=*sound->resourceA0;const auto* directoryRow=directory_->row(resource);
  if(!directoryRow||directoryRow->sourceIndex!=resource)return false;
  for(std::size_t prior=0;prior<ordinal;++prior)
   if(resources[prior]==resource||group.sounds_[prior].get()==sound)return false;
  resources[ordinal]=resource;
  const auto& row=rows_[resource];
  if(row.count8!=1||row.representative4||row.representativeGroup||row.resolvedNativeSample||
     row.nativeUnresolvedSceneReceipt||row.sceneRepresentative!=sound||row.sceneRepresentativeOwner!=&scene)return false;
 }
 for(std::size_t i=group.count48_;i<group.capacity54_;++i)if(group.members4C_[i])return false;
 // No extra directory row may retain this Scene or any of its four Sounds.
 // Do not dereference a representative to discover its owner or ordinal.
 for(std::size_t i=0;i<rows_.size();++i){const auto& row=rows_[i];
  if(row.nativeUnresolvedSceneReceipt)return false;
  bool expected=false;
  for(std::size_t ordinal=0;ordinal<resources.size();++ordinal){
   if(resources[ordinal]==i)expected=true;
   if(row.sceneRepresentative==group.sounds_[ordinal].get()&&resources[ordinal]!=i)return false;
  }
  if(row.sceneRepresentativeOwner==&scene&&!expected)return false;
 }
 return true;
}
bool OriginalAudioPendingWaves50KG::canAbortUnresolvedSceneNative(const Scene& scene)const noexcept {
 if(!unresolvedSceneNativeRowsCurrent(scene)||scene.nativeUnresolvedEntryRetiring_||
    scene.soundGroupRetirement_!=Scene::SoundGroupRetirement::live||
    scene.group4D8_.entity.reference00!=1||
    !scene.grant_->sceneSoundGroupCurrent(scene.group4D8_.entity,scene.group4D8_.slot40_)||
    (!scene.grant_->podSceneOwnerCurrent(&scene,scene.podScene_)&&
     !scene.grant_->scenePodRetirementCurrent(scene.podScene_))||
    !scene.backend_->preflightUnresolvedSceneEntryRetirement(scene))return false;
 // This also authenticates authored resource names and A0s while the real
 // registry still owns the Group. No clear/flush or source retain is implied.
 for(std::size_t ordinal=0;ordinal<scene.group4D8_.sounds_.size();++ordinal)
  if(!scene.sourceRowIdentityCurrent(*scene.backend_,ordinal,true))return false;
 return true;
}
void OriginalAudioPendingWaves50KG::abortUnresolvedSceneNativePreflighted(Scene& scene)noexcept {
 // The caller preflighted immediately before the actual Group begin. Its
 // private flag is issued only for the genuine Session failure after ref0,
 // never by merely finding an unresolved row or an empty registry slot.
 if(!unresolvedSceneNativeRowsCurrent(scene)||!scene.nativeUnresolvedEntryRetiring_||
    scene.retirementStage_!=Scene::RetirementStage::configClosed||
    scene.soundGroupRetirement_!=Scene::SoundGroupRetirement::registryReleased||
    !scene.soundGroupRetirementEntered235150())std::terminate();
 const auto address=[](const auto* p)noexcept{return reinterpret_cast<std::uintptr_t>(p);};
 const auto abortRevision=revision_+1;
 for(std::size_t ordinal=0;ordinal<scene.group4D8_.sounds_.size();++ordinal){
  const auto& sound=*scene.group4D8_.sounds_[ordinal];auto& row=rows_[*sound.resourceA0];
  NativeUnresolvedSceneReceipt receipt;
  receipt.applicationIdentity=address(issuer_);receipt.namespaceIdentity=address(this);
  receipt.directoryIdentity=address(directory_.get());receipt.sceneIdentity=address(&scene);
  receipt.groupIdentity=address(&scene.group4D8_.entity);receipt.soundIdentity=address(&sound);
  receipt.generation=scene.grant_->generation_;receipt.sceneKey=scene.podScene_.objectKey;
  receipt.groupKey=scene.group4D8_.entity.objectKey;receipt.grantEpoch=scene.grant_->epoch_;
  receipt.abortRevision=abortRevision;receipt.resourceA0=*sound.resourceA0;
  receipt.soundOrdinal=ordinal;receipt.groupSlot=scene.group4D8_.slot40_;
  row.nativeUnresolvedSceneReceipt=receipt;
  // Replace only the native live-borrow representation, BEFORE Sound release.
  // Actual233070(flags1) never unregisters. count8 stays exactly one and this
  // is NOT source26D960/26D9F0, sample resolution, or a reference decrement.
  row.sceneRepresentative=nullptr;row.sceneRepresentativeOwner=nullptr;
 }
 nativeUnresolvedSceneAbortRevision_=abortRevision;revision_=abortRevision;
}
bool OriginalAudioPendingWaves50KG::nativeUnresolvedSceneReceiptCurrent(
 const Row& row,std::size_t resource)const noexcept {
 if(!nativeUnresolvedSceneAbortRevision_||revision_!=nativeUnresolvedSceneAbortRevision_||
    !row.nativeUnresolvedSceneReceipt||
    row.count8!=1||row.hasRepresentative()||row.resolvedNativeSample)return false;
 const auto& receipt=*row.nativeUnresolvedSceneReceipt;
 const auto address=[](const auto* p)noexcept{return reinterpret_cast<std::uintptr_t>(p);};
 const auto* directoryRow=directory_->row(resource);
 return receipt.applicationIdentity==address(issuer_)&&receipt.namespaceIdentity==address(this)&&
  receipt.directoryIdentity==address(directory_.get())&&receipt.sceneIdentity&&
  receipt.groupIdentity&&receipt.soundIdentity&&receipt.generation&&receipt.sceneKey&&
  receipt.groupKey==receipt.groupIdentity&&receipt.grantEpoch&&
  receipt.abortRevision==nativeUnresolvedSceneAbortRevision_&&
  receipt.resourceA0==resource&&directoryRow&&directoryRow->sourceIndex==resource&&
  receipt.soundOrdinal<4&&receipt.groupSlot>=0&&
  static_cast<std::size_t>(receipt.groupSlot)<groupBorrows_.size();
}
bool OriginalAudioPendingWaves50KG::nativeOutputRegistrationRowsQuiescent()const noexcept {
 if(!current())return false;
 // Match final2668E0's historical-row census without consuming it. Device
 // close neither clears registrations nor performs final namespace release.
 const NativeUnresolvedSceneReceipt* firstHistorical=nullptr;
 std::array<std::uintptr_t,4> historicalSounds{};std::uint32_t historicalOrdinals=0;
 std::size_t historicalCount=0;
 for(std::size_t i=0;i<rows_.size();++i){const auto& row=rows_[i];
  if(row.hasRepresentative())return false;
  if(row.nativeUnresolvedSceneReceipt){
   if(!nativeUnresolvedSceneReceiptCurrent(row,i)||!row.nativeUnresolvedSceneReceipt->groupRetired)return false;
   const auto& receipt=*row.nativeUnresolvedSceneReceipt;
   if(historicalOrdinals&(1u<<receipt.soundOrdinal))return false;
   if(firstHistorical&&(receipt.sceneIdentity!=firstHistorical->sceneIdentity||
      receipt.groupIdentity!=firstHistorical->groupIdentity||receipt.generation!=firstHistorical->generation||
      receipt.sceneKey!=firstHistorical->sceneKey||receipt.groupKey!=firstHistorical->groupKey||
      receipt.grantEpoch!=firstHistorical->grantEpoch||receipt.groupSlot!=firstHistorical->groupSlot))return false;
   for(const auto sound:historicalSounds)if(sound==receipt.soundIdentity)return false;
   if(!firstHistorical)firstHistorical=&receipt;
   historicalSounds[receipt.soundOrdinal]=receipt.soundIdentity;
   historicalOrdinals|=1u<<receipt.soundOrdinal;++historicalCount;
  }else if(row.count8)return false;
 }
 return nativeUnresolvedSceneAbortRevision_?(historicalCount==4&&historicalOrdinals==15):historicalCount==0;
}
bool OriginalAudioPendingWaves50KG::canRetire(const Scene& scene)const noexcept {
 const auto slot=scene.group4D8_.slot40_;
 if(!current()||slot<0||static_cast<std::size_t>(slot)>=groupBorrows_.size()||
    groupBorrows_[slot].scene!=&scene||groupBorrows_[slot].watto)return false;
 std::size_t historical=0;std::uint32_t ordinalMask=0;
 const auto address=[](const auto* p)noexcept{return reinterpret_cast<std::uintptr_t>(p);};
 for(std::size_t i=0;i<rows_.size();++i){const auto& row=rows_[i];
  if(row.sceneRepresentativeOwner==&scene)return false;
  if(!row.nativeUnresolvedSceneReceipt||row.nativeUnresolvedSceneReceipt->sceneIdentity!=address(&scene))continue;
  const auto& receipt=*row.nativeUnresolvedSceneReceipt;
  if(!scene.nativeUnresolvedEntryRetiring_||!scene.grant_||!scene.grant_->current()||
     !nativeUnresolvedSceneReceiptCurrent(row,i)||receipt.groupRetired||
     receipt.groupIdentity!=address(&scene.group4D8_.entity)||receipt.groupSlot!=slot||
     receipt.generation!=scene.grant_->generation_||receipt.grantEpoch!=scene.grant_->epoch_||
     receipt.sceneKey!=scene.podScene_.objectKey||receipt.groupKey!=scene.group4D8_.entity.objectKey||
     (ordinalMask&(1u<<receipt.soundOrdinal)))return false;
  ordinalMask|=1u<<receipt.soundOrdinal;++historical;
 }
 return scene.nativeUnresolvedEntryRetiring_?(historical==4&&ordinalMask==15):historical==0;
}
bool OriginalAudioPendingWaves50KG::retireSceneBorrow(Scene& scene)noexcept {
 if(!canRetire(scene))return false;
 if(scene.nativeUnresolvedEntryRetiring_){
  const auto& group=scene.group4D8_;
  // The real finish helper has already removed the canonical Group/count.
  // groupLive_ is cleared ONLY after that helper succeeds. The Scene phase
  // becomes groupReleased later, after this exact pending borrow is retired.
  if(scene.retirementStage_!=Scene::RetirementStage::configClosed||
     scene.soundGroupRetirement_!=Scene::SoundGroupRetirement::registryReleased||
     !scene.sceneLive_||scene.groupLive_||scene.pendingWaves_.get()!=this||
     !scene.grant_||scene.grant_->issuer()!=issuer_||
     !sameShared(scene.grant_->application(),authority_.lock())||
     !scene.grant_->scenePodRetirementCurrent(scene.podScene_)||group.entity.reference00||
     group.entity.kind!=Scene::K::soundGroup2350D0||
     group.entity.sessionGeneration!=scene.grant_->generation_||
     scene.podScene_.sessionGeneration!=scene.grant_->generation_||
     group.count48_||group.members4C_||group.capacity54_||
     group.entity.firstChild18||group.entity.parent1C||group.entity.previous10||group.entity.next14||
     group.entity.mask20||group.entity.descendants24||
     scene.grant_->sceneSoundGroupCurrent(group.entity,group.slot40_))return false;
  for(const auto& sound:group.sounds_)if(sound)return false;
  const auto sceneIdentity=reinterpret_cast<std::uintptr_t>(&scene);
  // No dead Sound is inspected. These exact receipts are the only writer's
  // immutable historical identities, now sealed by genuine completed Group.
  for(auto& row:rows_)if(row.nativeUnresolvedSceneReceipt&&
     row.nativeUnresolvedSceneReceipt->sceneIdentity==sceneIdentity)
   row.nativeUnresolvedSceneReceipt->groupRetired=true;
 }
 groupBorrows_[scene.group4D8_.slot40_].scene=nullptr;return true;
}
bool OriginalAudioPendingWaves50KG::canRetire(const Group& group)const noexcept {
 if(!current()||group.slot40_<0||static_cast<std::size_t>(group.slot40_)>=groupBorrows_.size()||
    (groupBorrows_[static_cast<std::size_t>(group.slot40_)].watto!=&group||groupBorrows_[static_cast<std::size_t>(group.slot40_)].scene))return false;
 for(const auto& row:rows_)if(row.representativeGroup==&group)return false;
 return true;
}
bool OriginalAudioPendingWaves50KG::retireGroupBorrow(Group& group)noexcept {
 if(!canRetire(group))return false;
 groupBorrows_[static_cast<std::size_t>(group.slot40_)].watto=nullptr;return true;
}
bool OriginalAudioPendingWaves50KG::clear26D960(const RootOperation& operation,std::string& error)noexcept {
 if(!sameRoot(operation)||nativeUnresolvedSceneAbortRevision_||revision_==std::numeric_limits<std::uint64_t>::max()||
    (operation.kind()!=RootOperation::Kind::clearGameplay17D86C&&operation.kind()!=RootOperation::Kind::clearFrontend17DC98))
  return fail(error,"26D960 requires an exact authorized live root clear boundary");
 // Source directory order:299060 count reset BEFORE298F80 row+4 zero. Names,
 // actual Sounds, A0 fields, groups and native PCM remain untouched.
 for(auto& row:rows_){row.count8=0;row.representative4=nullptr;row.representativeGroup=nullptr;row.sceneRepresentative=nullptr;row.sceneRepresentativeOwner=nullptr;row.resolvedNativeSample.reset();}
 ++revision_;error.clear();return true;
}
bool OriginalAudioPendingWaves50KG::retire2668E0(const RootOperation& operation,
 OriginalUiMainAudio50KG* finalTransport,std::string& error)noexcept {
 if(!sameRoot(operation)||operation.kind()!=RootOperation::Kind::retireInterface2668E0)
  return fail(error,"2668E0 final namespace retirement requires exact live root identity");
 const NativeUnresolvedSceneReceipt* firstHistorical=nullptr;
 std::array<std::uintptr_t,4> historicalSounds{};std::uint32_t historicalOrdinals=0;
 std::size_t historicalCount=0;
 for(std::size_t i=0;i<rows_.size();++i){const auto& row=rows_[i];
  if(row.hasRepresentative())
   return fail(error,"2668E0 native safety envelope refuses live pending Sound borrows");
  if(row.nativeUnresolvedSceneReceipt){
   if(!nativeUnresolvedSceneReceiptCurrent(row,i)||!row.nativeUnresolvedSceneReceipt->groupRetired)
    return fail(error,"2668E0 historical Scene row lacks exact completed native Group retirement");
   const auto& receipt=*row.nativeUnresolvedSceneReceipt;
   if(historicalOrdinals&(1u<<receipt.soundOrdinal))
    return fail(error,"2668E0 historical Scene receipt ordinal is repeated");
   if(firstHistorical&&(receipt.sceneIdentity!=firstHistorical->sceneIdentity||
      receipt.groupIdentity!=firstHistorical->groupIdentity||receipt.generation!=firstHistorical->generation||
      receipt.sceneKey!=firstHistorical->sceneKey||receipt.groupKey!=firstHistorical->groupKey||
      receipt.grantEpoch!=firstHistorical->grantEpoch||receipt.groupSlot!=firstHistorical->groupSlot))
    return fail(error,"2668E0 historical Scene receipts do not describe one exact aborted Group");
   for(const auto sound:historicalSounds)if(sound==receipt.soundIdentity)
    return fail(error,"2668E0 historical Scene Sound identity is repeated");
   if(!firstHistorical)firstHistorical=&receipt;
   historicalSounds[receipt.soundOrdinal]=receipt.soundIdentity;
   historicalOrdinals|=1u<<receipt.soundOrdinal;++historicalCount;
  }else if(row.count8)
   return fail(error,"2668E0 native safety envelope refuses unrelated unresolved registrations");
 }
 if(nativeUnresolvedSceneAbortRevision_?(historicalCount!=4||historicalOrdinals!=15):historicalCount!=0)
  return fail(error,"2668E0 native abort requires exactly four sealed historical Scene rows");
 for(const auto& group:groupBorrows_)if(!group.empty())
  return fail(error,"2668E0 native safety envelope requires registered source groups retired first");
 OriginalUiMainAudio50KG::FinalNativePcmRetirement finalReceipt;
 if(finalTransport){
  if(!nativeSamples_||!finalTransport->canRetireFinalNativePcm(*nativeSamples_,finalReceipt))
   return fail(error,"2668E0 requires exact stopped Main clips and no external native PCM lease");
 }else if(nativeSamples_&&!nativeSamples_->canRetireNativeBacking())
  return fail(error,"2668E0 final retirement refuses outstanding native PCM leases");
 const auto authority=authority_.lock();
 if(!authority||authority->pendingWaves98A4_.get()!=this)
  return fail(error,"2668E0 lost its exact published namespace before final release");
 auto retain=authority->pendingWaves98A4_;
 // All rejecting work, including root/device/route/lease-count authentication,
 // is over. No fallible close call occurs after the first raw-clip withdrawal.
 if(finalTransport)finalTransport->retireFinalNativePcmPreflighted(finalReceipt);
 if(nativeSamples_)nativeSamples_->closeNativeBackingPreflighted();
 // Actual source directory destructor frees rows without dereferencing row+4.
 // The stronger native guard is not an invented original destructor traversal.
 closed_=true;nativeSamples_.reset();directory_.reset();authority->pendingWaves98A4_.reset();
 error.clear();return true;
}
bool OriginalAudioPendingWaves50KG::flush26D9F0(std::string& error)const noexcept {
 if(!current())return fail(error,"26D9F0 actual pending namespace is not current");
 if(nativeUnresolvedSceneAbortRevision_)return fail(error,"26D9F0 terminal native-aborted namespace cannot replay historical registrations");
 return fail(error,"26D9F0 real sample-cache lease/2332F0 producer unavailable; no row resolved");
}
}

namespace original_main_entry50KG {
namespace {
bool pendingRootFailure(std::string& error,const char* text)noexcept {
 try{error=text;}catch(...){error.clear();}return false;
}
}
bool Application::preparePendingWaves17CD84(
 std::shared_ptr<rr::platform::OriginalAudioPendingWaves50KG>& out,std::string& error)const noexcept {
 using Waves=rr::platform::OriginalAudioPendingWaves50KG;
 out.reset();
 const Waves::RootOperation operation(*this,entitiesOwner_,Waves::RootOperation::Kind::construct232EA0);
 if(operation.issuer()!=this)return pendingRootFailure(error,"17CD84 requires actual live Application/thread");
 // Idempotent native retain, not a repeated original232EA0 invocation. Every
 // manager receives this exact publication, never a fresh per-manager table.
 if(auto owner=Waves::retainForRoot(operation,error)){out=std::move(owner);return true;}
 try {
  const auto archive=appStart(assetRoot);
  if(!archive)return pendingRootFailure(error,"17CD84 actual Application APPSTART unavailable");
  auto directory=std::make_shared<rr::assets::OriginalAudioDirectory50KX>();
  rr::assets::AudioDirectoryDiagnostic50KX diagnostic;
  if(!directory->prepare(*archive,diagnostic))
   return pendingRootFailure(error,"17CD84 authentic APPSTART AudioStr preparation rejected");
  out=Waves::prepare232EA0(operation,std::move(directory),error);
  return bool(out);
 }catch(...){return pendingRootFailure(error,"17CD84 APPSTART preparation failed before namespace publication");}
}
bool Application::retainPendingWaves98A4(
 std::shared_ptr<rr::platform::OriginalAudioPendingWaves50KG>& out,std::string& error)const noexcept {
 using Waves=rr::platform::OriginalAudioPendingWaves50KG;
 out.reset();
 const Waves::RootOperation operation(*this,entitiesOwner_,Waves::RootOperation::Kind::construct232EA0);
 if(operation.issuer()!=this)return pendingRootFailure(error,"98A4 retain requires actual live Application/thread");
 out=Waves::retainForRoot(operation,error);return bool(out);
}
bool Application::preparePendingWaveNativeBacking(std::string& error)const noexcept {
 using Waves=rr::platform::OriginalAudioPendingWaves50KG;
 const Waves::RootOperation operation(*this,entitiesOwner_,Waves::RootOperation::Kind::nativeBackingPrepare);
 if(operation.issuer()!=this)return pendingRootFailure(error,"native backing requires actual live Application/thread");
 auto owner=Waves::retainForRoot(operation,error);
 // Existing producer reads this Application's actual audioRoot. Failure keeps
 // the issued namespace intact and explicitly retireable; no fake root clear.
 return owner&&owner->prepareNativeSampleBacking(operation,error);
}
bool Application::retirePendingWaves2668E0(
 const std::shared_ptr<rr::platform::OriginalAudioPendingWaves50KG>& expected,std::string& error)const noexcept {
 using Waves=rr::platform::OriginalAudioPendingWaves50KG;
 const Waves::RootOperation operation(*this,entitiesOwner_,Waves::RootOperation::Kind::retireInterface2668E0);
 if(operation.issuer()!=this)return pendingRootFailure(error,"2668E0 requires actual live Application/thread");
 auto owner=Waves::retainForRoot(operation,error);
 if(!owner||!expected||owner.get()!=expected.get()||owner.owner_before(expected)||expected.owner_before(owner))
  return pendingRootFailure(error,"2668E0 requires the exact retained root namespace");
 rr::platform::OriginalUiMainAudio50KG* finalTransport=nullptr;
 const auto& backing=garageAudioBackingOwner_;
 if(owner->nativeSamples_&&backing&&backing->frontendOutputAttempted_){
  // Only the actual final namespace path can consume played native leases.
  // Device close alone deliberately retains clips for a possible rebind.
  if(garageAudioOwnerThread_!=std::this_thread::get_id()||!backing->inputsCurrent(*this)||
     backing->failed_||!backing->frontendOutputClosed_||backing->frontendOutputOwner_||
     backing->frontendOutputCloseFailed_||backing->frontendOutputAccountingFailed_||
     backing.use_count()!=2||garageCustodyOwner_||!sourceAural00Owner_||
     !sourceAural00Owner_->nativeOutputSourceQuiescent()||
     backing->frontendOutputAuralEntity_!=&sourceAural00Owner_->entity_||
     !frontendAudioClosedReceiptCurrent())
   return pendingRootFailure(error,"2668E0 played native PCM requires exact final Root/Aural/device-close custody");
  const auto auralNamespace=sourceAural00Owner_->nativeOutputPendingNamespace();
  if(!auralNamespace||auralNamespace.get()!=owner.get()||
     auralNamespace.owner_before(owner)||owner.owner_before(auralNamespace))
   return pendingRootFailure(error,"2668E0 namespace is not the exact live Aural publication");
  finalTransport=&backing->transport_;
 }
 // Cold and legacy/no-cache retirement keep their original strict lease rule.
 return owner->retire2668E0(operation,finalTransport,error);
}
}
