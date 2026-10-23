#include "../../../src/game/original_video_preflip50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <exception>
#include <limits>
#include <new>
#include <utility>
namespace rr::game {
namespace {
using D=OriginalVideoPreFlipDiagnostic50KG;
using S=OriginalVideoPreFlipStatus50KG;
bool fail(D& d,S status,std::uint32_t pc)noexcept {d.status=status;d.sourcePC=pc;return false;}
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
 return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
}
OriginalVideoPreFlipList50KG::Records& OriginalVideoPreFlipList50KG::records()noexcept {
 static Records value;return value;
}
bool OriginalVideoPreFlipList50KG::firstUse(LazyRecord& record)noexcept {
 if(record.guard.load())return false;
 record.assertion.store(1);record.guard.store(1);return true;
}
OriginalVideoPreFlipList50KG::OriginalVideoPreFlipList50KG(const Video& video,const Application& root)noexcept
 :issuer_(&root),video_(&video),authority_(root.entities()),lifetime_(root.animationClock()) {
 //2AE1E0's exact zero/low-bit constructor state precedes2AE210(1,1).
 //Process records retain BSS lifetime; native atomics are not guest instructions.
 auto& process=records();
 firstUse(process.size8D14); //443A68 then8D14, before the positive-size assertion.
 firstUse(process.empty8D18); //443A70 then8D18, before capacity==0 assertion.
 //Unlike throwing native factories, original2AE210 publishes null on failure,
 //then growth/capacity/initialized. Its caller2AE080 ignores the false result.
 slots4_.reset(new(std::nothrow) Slot[1]); //2AE308/310, no source zero-slot claim.
 growth8_=1;capacityC_=1;initialized10Bit0_=true; //2AE314/318/330.
}
void OriginalVideoPreFlipList50KG::bindPublishedVideo(const std::shared_ptr<Video>& video)noexcept {
 //Only the actual Application factory calls this immediately after publication.
 //All operations are nonallocating. An internal misuse cannot leave a silently
 //usable half-bound list; failed backing allocation remains explicitly visible.
 if(bound_||thread_!=std::this_thread::get_id()||!video||video.get()!=video_||
    !issuer_||!sameOwner(video,issuer_->videoOwner_)||&video->sourcePreFlip18_!=this)std::terminate();
 issuedVideo_=video;bound_=true;
}
bool OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(const Application& root)noexcept {
 //The actual root authority shared owner is const after construction. Its
 //immutable affinity is checked before any mutable factory or clock access.
 const auto& authority=root.entitiesOwner_;
 if(!authority||authority->nativeOwnerThread_!=std::this_thread::get_id())return false;
 if(!authority->constructed_||authority->released_)return false;
 std::uint32_t pause=0;return root.animationClock().readPause11C(pause);
}
bool OriginalVideoPreFlipList50KG::applicationMayRetire(const Application& root)noexcept {
 const auto& authority=root.entities();
 return authority&&authority->nativeOwnerThread_==std::this_thread::get_id()&&
     !authority->videoPreFlipNativeBorrow_;
}
bool OriginalVideoPreFlipList50KG::current()const noexcept {
 if(thread_!=std::this_thread::get_id()||!bound_||!issuer_||!video_)return false;
 const auto authority=authority_.lock();std::uint32_t pause=0;
 if(!authority||authority->nativeOwnerThread_!=thread_||!authority->constructed_||authority->released_||
    !lifetime_.readPause11C(pause))return false;
 if(liveMemberships_ ? authority->videoPreFlipNativeBorrow_!=this :
                      authority->videoPreFlipNativeBorrow_!=nullptr)return false;
 //The retained clock and immutable affinity precede mutable raw-root reads.
 const auto video=issuedVideo_.lock();
 return sameOwner(authority,issuer_->entities())&&lifetime_.sameOwner(issuer_->animationClock())&&
    video&&video.get()==video_&&sameOwner(video,issuer_->videoOwner_)&&&video->sourcePreFlip18_==this;
}
bool OriginalVideoPreFlipList50KG::retainMembership(D& d)noexcept {
 if(!current())return fail(d,S::authority,0x2ADFD0);
 if(liveMemberships_==std::numeric_limits<std::uint32_t>::max())
  return fail(d,S::nativeLifetime,0x2ADFD0);
 auto authority=authority_.lock();
 //All admission checks precede these nonfailing native stores and the source
 //base vtable. Even failed append returns a source-constructed base prefix.
 if(!liveMemberships_)authority->videoPreFlipNativeBorrow_=this;
 ++liveMemberships_;return true;
}
void OriginalVideoPreFlipList50KG::releaseMembership()noexcept {
 if(!current()||!liveMemberships_)std::terminate();
 auto authority=authority_.lock();
 --liveMemberships_;
 if(!liveMemberships_)authority->videoPreFlipNativeBorrow_=nullptr;
}
bool OriginalVideoPreFlipList50KG::grow2AE550(std::int32_t capacity,D& d)noexcept {
 if(capacity<=capacityC_)return fail(d,S::capacity,0x2AE570);
 //Original allocation byte count is 32-bit SLL(capacity,2). Reject wrapped
 //positive source sizes rather than treating larger host Slot[] as equivalent.
 if(static_cast<std::uint32_t>(capacity)>std::numeric_limits<std::uint32_t>::max()/4u)
  return fail(d,S::capacity,0x2AE584);
 std::unique_ptr<Slot[]> next(new(std::nothrow) Slot[static_cast<std::size_t>(capacity)]);
 if(!next)return fail(d,S::allocation,0x2AE594);
 //Original memcpy copies OLD CAPACITY*4, not count. Native Slot carries explicit
 //unknownness, so copying unused slots never reads an indeterminate C++ pointer.
 //The source backing has32-bit pointer entries; native objects use host pointers.
 for(std::int32_t i=0;i<capacityC_;++i)next[i]=slots4_[i];
 auto old=std::move(slots4_);slots4_=std::move(next); //New pointer2AE5B8 before free.
 old.reset();capacityC_=capacity; //2AE5B4 free, then capacity2AE5C0.
 d.sourceEffectsCommitted=true;return true;
}
bool OriginalVideoPreFlipList50KG::append2AE370(OriginalVideoPreFlipObject50KG& object,D& d)noexcept {
 if(!current())return fail(d,S::authority,0x2AE370);
 //Native memory-safety envelope, before claiming an original invalid dereference.
 if(!slots4_||!initialized10Bit0_||count0_<0||capacityC_<=0||count0_>capacityC_||growth8_<=0)
  return fail(d,S::backing,0x2AE458);
 auto& process=records();
 d.sourceEffectsCommitted=firstUse(process.initialized8D1C)||d.sourceEffectsCommitted;
 d.sourceEffectsCommitted=firstUse(process.nonnull8D20)||d.sourceEffectsCommitted;
 if(count0_>=capacityC_){
  //Original ADDU wraps; this bounded native allocation envelope rejects an
  //unrepresentable positive size instead of host signed-overflow UB or a write.
  if(count0_>std::numeric_limits<std::int32_t>::max()-growth8_)
   return fail(d,S::capacity,0x2AE478);
  if(!grow2AE550(count0_+growth8_,d))return false;
 }
 slots4_[count0_]={&object,true}; //Pointer publication2AE4A0 precedes count2AE4AC.
 ++count0_;d={S::complete,0x2AE4AC,true};return true;
}
bool OriginalVideoPreFlipList50KG::remove2AE4D0(OriginalVideoPreFlipObject50KG& object)noexcept {
 //Called only after the concrete membership's exact current/lifetime checks.
 if(count0_<=0)return false;
 for(std::int32_t i=0;i<count0_;++i){
  if(!slots4_[i].known)std::terminate(); //Cannot fabricate a produced pointer.
  if(slots4_[i].value!=&object)continue;
  slots4_[i]=slots4_[count0_-1]; //2AE518, swap last into FIRST matched slot.
  --count0_; //2AE52C. Source does NOT clear the stale trailing slot.
  return true;
 }
 return false;
}
bool OriginalVideoPreFlipList50KG::inspect(Observation& out)const noexcept {
 if(!current())return false;
 out={count0_,growth8_,capacityC_,initialized10Bit0_,bool(slots4_),false};return true;
}
OriginalVideoPreFlipList50KG::~OriginalVideoPreFlipList50KG()noexcept {
 //Source2AE164 frees only array storage, never borrowed callback objects.
 //The native envelope forbids losing a live membership, rather than silently
 //auto-removing it or pretending the original list destructor released it.
 if(thread_!=std::this_thread::get_id()||count0_!=0||liveMemberships_)std::terminate();
 slots4_.reset();
}
const OriginalVideoPreFlipObject50KG::SourceVtable&
OriginalVideoPreFlipObject50KG::baseVtable3D44D0()noexcept {
 //Authenticated base table has destructor+8 and NULL callback+0C. This is not
 //a successful no-op implementation. No callback is invoked by this leaf.
 static const SourceVtable value{&OriginalVideoPreFlipObject50KG::retire2AE010,nullptr};return value;
}
bool OriginalVideoPreFlipObject50KG::construct2ADFD0(OriginalVideoPreFlipList50KG& list,D& d)noexcept {
 d={};
 if(thread_!=std::this_thread::get_id()||constructed_||retired_||!list.current())return fail(d,S::authority,0x2ADFD0);
 if(!list.slots4_||!list.initialized10Bit0_)return fail(d,S::backing,0x2ADFF4);
 const auto retainedVideo=list.issuedVideo_.lock();
 if(!retainedVideo||!list.retainMembership(d))return false;
 video_=retainedVideo;list_=&list; //Native retention, not a source refcount.
 vtable0_=&baseVtable3D44D0();constructed_=true; //2ADFE8 before actual list append.
 d.sourceEffectsCommitted=true;
 //Original ignores append failure and returns this. The native false result
 //preserves that constructed base prefix, including its retained Video, for
 //explicit2AE010 cleanup. No ScreenRain derived state or callback is invented.
 inserted_=list.append2AE370(*this,d);return inserted_;
}
bool OriginalVideoPreFlipObject50KG::retire2AE010(D& d)noexcept {
 d={};
 if(thread_!=std::this_thread::get_id()||!constructed_||retired_||!list_||!video_||
    !list_->current()||!sameOwner(video_,list_->issuedVideo_.lock()))return fail(d,S::authority,0x2AE010);
 auto keepAlive=video_; //List must outlive unregister and membership pointer cleanup.
 vtable0_=&baseVtable3D44D0(); //2AE034 before actual remove2AE040.
 (void)list_->remove2AE4D0(*this); //Source ignores not-found, including failed append.
 list_->releaseMembership(); //After actual remove, before final Video release.
 inserted_=false;retired_=true;list_=nullptr;video_.reset();
 d={S::complete,0x2AE048,true};return true;
}
OriginalVideoPreFlipObject50KG::~OriginalVideoPreFlipObject50KG()noexcept {
 if(constructed_&&!retired_)std::terminate();
}
}
