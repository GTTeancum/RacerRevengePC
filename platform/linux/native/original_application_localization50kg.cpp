#include "../../../src/assets/original_application_localization50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <bit>
#include <cstring>
#include <limits>
#include <exception>
#include <atomic>
namespace rr::assets {
namespace {
using Diagnostic=OriginalApplicationLocalizationDiagnostic50KG;
using Status=OriginalApplicationLocalizationStatus50KG;
bool fail(Diagnostic& d,Status status,std::uint32_t pc,bool published=false,std::size_t ordinal=0)noexcept {
 d={status,pc,ordinal,published};return false;
}
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
 return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
std::uint16_t u16(const std::uint8_t* p)noexcept{return std::uint16_t(p[0])|(std::uint16_t(p[1])<<8);}
std::uint32_t u32(const std::uint8_t* p)noexcept{return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);}
OriginalUiMemberView50KG indexMember(const OriginalUiArchive50KG& archive)noexcept {
 for(std::size_t i=0;i<archive.memberCount();++i){const auto view=archive.member(i);
  if(view.identity&&std::strcmp(view.identity->name,"loc\\english\\UI.loc")==0)return view;
 }
 return {};
}
OriginalUiMemberView50KG textMember(const OriginalUiArchive50KG& archive)noexcept {
 const auto view=archive.member(0);
 if(view.identity!=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::localization,0)||
    !view.identity||std::strcmp(view.identity->name,"loc\\english\\L_UI_ENG.bin")!=0)return {};
 return view;
}
std::string_view cString(std::string_view s)noexcept {const auto end=s.find('\0');return end==s.npos?s:s.substr(0,end);}
}
std::atomic<std::uint8_t>& OriginalApplicationLocalization50KG::countGuard83E4()noexcept {static std::atomic<std::uint8_t> value{0};return value;}
std::atomic<std::int32_t>& OriginalApplicationLocalization50KG::countAssertion427078()noexcept {static std::atomic<std::int32_t> value{0};return value;}
std::atomic<std::uint8_t>& OriginalApplicationLocalization50KG::maximumGuard83F4()noexcept {static std::atomic<std::uint8_t> value{0};return value;}
std::atomic<std::int32_t>& OriginalApplicationLocalization50KG::maximumAssertion427098()noexcept {static std::atomic<std::int32_t> value{0};return value;}
std::atomic<std::uint8_t>& OriginalApplicationLocalization50KG::selectedGuard80A0()noexcept {static std::atomic<std::uint8_t> value{0};return value;}
std::atomic<std::int32_t>& OriginalApplicationLocalization50KG::selectedAssertion3D6B20()noexcept {static std::atomic<std::int32_t> value{0};return value;}
OriginalApplicationLocalization50KG::~OriginalApplicationLocalization50KG()noexcept {
 // Actual source release is explicit17C8B8, not C++ member reverse order.
 if(issuer_&&phase_!=Phase::retired)std::terminate();
}
bool OriginalApplicationLocalization50KG::stage(const Application& root,std::shared_ptr<Authority> authority,
 std::shared_ptr<const OriginalUiArchive50KG> archive,std::shared_ptr<OriginalApplicationLocalization50KG>& out,
 Diagnostic& d)noexcept {
 // Immutable authority affinity precedes every mutable root/lifetime access.
 if(!authority||authority->nativeOwnerThread_!=std::this_thread::get_id())return fail(d,Status::authority,0x17DEDC);
 std::uint32_t pause=0;
 if(!authority->constructed_||authority->released_||!sameOwner(authority,root.entities())||
    !root.animationClock().readPause11C(pause))return fail(d,Status::authority,0x17DEDC);
 if(!archive||!archive->loaded()||archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::localization))
  return fail(d,Status::archive,0x14BD4C);
 const auto index=indexMember(*archive);
 if(!index.identity||!index.data||index.size<28)return fail(d,Status::archive,0x14BD4C);
 const auto domain=root.standardMaterialDomainOwner_;
 if(!domain||!domain->prepared())return fail(d,Status::authority,0x14BD4C);
 const auto provider=domain->archiveProvider3E1A70();
 if(!provider||!provider->sealed())return fail(d,Status::authority,0x14BD4C);
 try {
 const std::string lookupName="loc\\english\\UI.loc";
 OriginalStandardArchiveProvider50LA::MemberView mounted;
 if(provider->lookup132820(lookupName,mounted)!=OriginalStandardArchiveProvider50LA::Lookup::present||
    mounted.data!=index.data||mounted.size!=index.size)return fail(d,Status::archive,0x14BD4C);
 auto owner=root.sourceLocalization32COwner_;
 if(owner){
  if(!owner->current()||owner->phase_!=Phase::constructed||!sameOwner(owner->archive_,archive))
   return fail(d,Status::phase,0x17DED0,true);
 }else{
  if(authority->localization32CNativeOwner_)return fail(d,Status::authority,0x17DEF4);
  try{
   owner=std::shared_ptr<OriginalApplicationLocalization50KG>(new OriginalApplicationLocalization50KG);
   owner->authority_=authority;owner->lifetime_=root.animationClock();
   owner->archive_=std::move(archive);owner->issuedArchive_=owner->archive_;owner->indexIdentity_=index.identity;
   owner->provider_=provider;owner->issuedProvider_=provider;owner->lookupName_=lookupName;
   owner->issuedOwner_=owner;
   // Genuine constructor14BB60 precedes the actual root32C publication17DEF4.
   // No allocation/callback can fail between setting issuer and root ownership.
   owner->issuer_=&root;root.sourceLocalization32COwner_=owner;
   authority->localization32CNativeOwner_=owner.get(); //Native cleanup guard, no source count.
  }catch(...){return fail(d,Status::allocation,0x17DEDC);}
 }
 // Preserve the real constructed prefix for explicit cleanup if index staging
 // fails. Native preflight does not pretend original allocation failures rolled back.
 out=owner;return owner->loadIndex14BC30(d);
 }catch(...){return fail(d,Status::allocation,0x14BD4C);}
}
bool OriginalApplicationLocalization50KG::cleanupIdentityCurrent()const noexcept {
 if(ownerThread_!=std::this_thread::get_id()||
    (phase_!=Phase::constructed&&phase_!=Phase::indexed&&phase_!=Phase::selected)||!issuer_)return false;
 std::uint32_t pause=0;if(!lifetime_.readPause11C(pause))return false;
 const auto authority=authority_.lock();
 if(!(authority&&authority->nativeOwnerThread_==ownerThread_&&authority->constructed_&&!authority->released_&&
  authority->localization32CNativeOwner_==this&&sameOwner(authority,issuer_->entities())&&lifetime_.sameOwner(issuer_->animationClock())&&
  issuer_->sourceLocalization32COwner_.get()==this&&sameOwner(issuedOwner_.lock(),issuer_->sourceLocalization32COwner_)&&
  sameOwner(archive_,issuedArchive_.lock())&&archive_->loaded()&&
  archive_->identity()==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::localization)))return false;
 const auto domain=issuer_->standardMaterialDomainOwner_;
 if(!domain||!domain->prepared()||!sameOwner(provider_,issuedProvider_.lock())||
    !sameOwner(provider_,domain->archiveProvider3E1A70()))return false;
 return indexMember(*archive_).identity==indexIdentity_;
}
bool OriginalApplicationLocalization50KG::current()const noexcept {
 if(!cleanupIdentityCurrent())return false;
 const auto index=indexMember(*archive_);OriginalStandardArchiveProvider50LA::MemberView mounted;
 if(provider_->lookup132820(lookupName_,mounted)!=OriginalStandardArchiveProvider50LA::Lookup::present||
     mounted.data!=index.data||mounted.size!=index.size)return false;
 if(phase_!=Phase::selected)return true;
 const auto text=textMember(*archive_);
 return text.identity==textIdentity_&&textLookupName_=="loc\\english\\L_UI_ENG.bin"&&
     provider_->lookup132820(textLookupName_,mounted)==OriginalStandardArchiveProvider50LA::Lookup::present&&
     mounted.data==text.data&&mounted.size==text.size&&selectedRowsCurrent();
}
std::uint32_t OriginalApplicationLocalization50KG::hash2D3A80(std::string_view key)noexcept {
 std::uint32_t result=0;
 for(unsigned char byte:cString(key)){
  //LB sign-extension followed by32-bit ADDU, without host signed-overflow UB.
  result+=byte<128?std::uint32_t(byte):std::uint32_t(byte)-256u;
 }
 return result;
}
bool OriginalApplicationLocalization50KG::compare2D3AB0(const char* left,std::string_view right)noexcept {
 return left&&std::string_view(left)==cString(right);
}
bool OriginalApplicationLocalization50KG::bucketFor(std::uint32_t raw,std::uint32_t count,std::size_t& out)noexcept {
 if(!count)return false;
 const auto magnitude=(raw&0x80000000u)?0u-raw:raw;
 // Source SUBU leaves INT_MIN negative, then signed DIV would address outside
 // the bucket array. This bounded native envelope rejects that unsafe case.
 if(magnitude&0x80000000u)return false;
 out=magnitude%count;return true;
}
bool OriginalApplicationLocalization50KG::loadIndex14BC30(Diagnostic& d)noexcept {
 if(!current()||phase_!=Phase::constructed)return fail(d,Status::phase,0x14BC30,bool(issuer_));
 const auto index=indexMember(*archive_);
 if(index.identity!=indexIdentity_||!index.data||index.size<28)return fail(d,Status::archive,0x14BD4C,true);
 const auto count=std::bit_cast<std::int16_t>(u16(index.data+26));
 const std::size_t first=u32(index.data+4),table=u32(index.data+8);
 if(count!=467||first>=table||table>index.size||std::size_t(count)>(index.size-table)/8||table-first<=1)
  return fail(d,Status::shape,0x14BDD8,true);
 const auto nameBytes=table-first-1;
 if(nameBytes!=13616)return fail(d,Status::shape,0x14C24C,true);
 try{
  // Bounded native allocation/shape preflight: no source mutation on failure,
  // while already-published constructor ownership is retained. Original valid
  // source effects below retain their order, including both shared lazy records.
  auto temporary=std::make_unique<std::uint8_t[]>(std::size_t(count)*8);
  std::memcpy(temporary.get(),index.data+table,std::size_t(count)*8);
  auto rows=std::make_unique<Row[]>(count);
  auto names=std::make_unique<char[]>(nameBytes);
  auto pointers=std::make_unique<const char*[]>(nameBytes); //NOT count pointers.
  std::array<std::size_t,467> nameOffsets{};
  auto hash=std::make_unique<Hash>();
  auto buckets=std::make_unique<Node*[]>(count);
  hash->ownedNodes.reserve(count);
  for(std::int32_t i=0;i<count;++i)hash->ownedNodes.emplace_back(new Node);
  std::size_t cursor=0;
  for(std::int32_t i=0;i<count;++i){
   const auto maximum=std::bit_cast<std::int16_t>(u16(temporary.get()+std::size_t(i)*8+4));
   if(maximum>=481)return fail(d,Status::shape,0x14C0EC,true,i); //Source LH, negative is not unsigned480.
   const auto start=cursor;while(cursor<nameBytes&&index.data[first+cursor])++cursor;
   if(cursor==nameBytes)return fail(d,Status::shape,0x14C340,true,i);
   nameOffsets[i]=start;++cursor;
   std::size_t bucket=0;if(!bucketFor(hash2D3A80(std::string_view(reinterpret_cast<const char*>(index.data+first+start),cursor-start-1)),count,bucket))return fail(d,Status::shape,0x14CB78,true,i);
  }
  std::memcpy(base12_.data(),"UI",3);baseKnown_=true; //14BC50, exact bounded startup base.
  count10_=count; //14BDA0.
  if(!countGuard83E4().load()){
   countAssertion427078().store(1); //14BDC0.
   countGuard83E4().store(1); //14BDC4.
  }
  rows04_=std::move(rows); //14C080.
  for(std::int32_t i=0;i<count;++i){
   rows04_[i].maximum4=std::bit_cast<std::int16_t>(u16(temporary.get()+std::size_t(i)*8+4)); //14C0A0.
   if(!maximumGuard83F4().load()){
    maximumAssertion427098().store(1); //14C0B8.
    maximumGuard83F4().store(1); //14C0BC.
   }
  }
  // Native arrays were preallocated; source owner publication remains28,24,
  // then ordinal-ascending pointers, then actual hash construction/insertion.
  names28_=std::move(names);nameBytes_=nameBytes;
  names24_=std::move(pointers);
  std::memcpy(names28_.get(),index.data+first,nameBytes); //14C298 authentic member read.
  for(std::int32_t i=0;i<count;++i)names24_[i]=names28_.get()+nameOffsets[i]; //14C434.
  hash->count4=count;hash->buckets0=std::move(buckets);
  for(std::int32_t i=0;i<count;++i)hash->buckets0[i]=nullptr;
  hash->hash8=&hash2D3A80;hash->compareC=&compare2D3AB0; //14CA38/3C after every bucket was cleared.
  hash2C_=std::move(hash); //14C524.
  for(std::int32_t i=0;i<count;++i){
   const auto raw=hash2C_->hash8(names24_[i]);std::size_t bucket=0;
   if(!bucketFor(raw,count,bucket))std::terminate(); //Already preflighted exact immutable names.
   auto* node=hash2C_->ownedNodes[i].get();
   node->nextC=hash2C_->buckets0[bucket]; //14CBA0.
   hash2C_->buckets0[bucket]=node; //14CBAC BEFORE key/index/hash stores.
   node->key8=names24_[i];node->ordinal0=i;node->hash4=std::bit_cast<std::int32_t>(raw);
  }
  temporary.reset(); //14C564, before file close; no real file handle is fabricated.
  phase_=Phase::indexed;d={Status::complete,0x14C59C,0,true};return true;
 }catch(...){return fail(d,Status::allocation,0x14BF88,true);}
}
bool OriginalApplicationLocalization50KG::scanEnglishBin(const OriginalUiMemberView50KG& member,
 std::array<std::size_t,467>& offsets,std::array<std::size_t,467>& lengths,Diagnostic& d)noexcept {
 // Native malformed-input envelope. Original14C748 has no in-row bound and
 //14C724 logs a short file then returns count; neither is fabricated success.
 if(!member.data||member.size!=10637)return fail(d,Status::shape,0x14C654,true);
 std::array<std::size_t,467> nextOffsets{},nextLengths{};std::size_t cursor=0;
 for(std::size_t i=0;i<nextOffsets.size();++i){
  if(cursor>=member.size)return fail(d,Status::shape,0x14C710,true,i);
  nextOffsets[i]=cursor;
  while(cursor<member.size&&member.data[cursor])++cursor;
  if(cursor==member.size)return fail(d,Status::shape,0x14C748,true,i);
  nextLengths[i]=cursor-nextOffsets[i];++cursor;
 }
 // Exact authenticated English extent, not a general source trailing rule.
 if(cursor!=member.size)return fail(d,Status::shape,0x14C778,true,467);
 offsets=nextOffsets;lengths=nextLengths;return true;
}
bool OriginalApplicationLocalization50KG::selectedRowsCurrent()const noexcept {
 if(phase_!=Phase::selected||!archive_||!text00_||!rows04_||count10_!=467||
    !baseKnown_||base12_[0]!='U'||base12_[1]!='I'||base12_[2]!=0||
    textBytes_!=10637||publishedTextRows_!=467||selectionEpoch_!=1)return false;
 const auto text=textMember(*archive_);const auto index=indexMember(*archive_);
 if(!text.data||text.size!=textBytes_||text.identity!=textIdentity_||
    !index.data||index.size!=17688||index.identity!=indexIdentity_)return false;
 std::array<std::size_t,467> offsets{},lengths{};Diagnostic d;
 if(!scanEnglishBin(text,offsets,lengths,d)||offsets!=textOffsets_||lengths!=textLengths_||
    std::memcmp(text00_.get(),text.data,textBytes_)!=0)return false;
 for(std::size_t i=0;i<467;++i){
  if(!rows04_[i].textKnown||rows04_[i].text0!=text00_.get()+offsets[i]||
     rows04_[i].maximum4!=std::bit_cast<std::int16_t>(u16(index.data+13688+i*8+4)))return false;
 }
 return true;
}
bool OriginalApplicationLocalization50KG::firstEnglishPrefixCurrent()const noexcept {
 if(!current()||phase_!=Phase::indexed||text00_||textIdentity_||textBytes_||publishedTextRows_||
    selectionEpoch_||selectedBorrowCount_||!textLookupName_.empty()||count10_!=467||
    !rows04_||!names24_||!names28_||!hash2C_||!baseKnown_||
    base12_[0]!='U'||base12_[1]!='I'||base12_[2]!=0)return false;
 const auto index=indexMember(*archive_);
 if(!index.data||index.size!=17688||index.identity!=indexIdentity_)return false;
 for(std::size_t i=0;i<467;++i)if(rows04_[i].textKnown||rows04_[i].text0||
    rows04_[i].maximum4!=std::bit_cast<std::int16_t>(u16(index.data+13688+i*8+4)))return false;
 return true;
}
bool OriginalApplicationLocalization50KG::selectEnglish14C5F0(const Application& root,Diagnostic& d)noexcept {
 if(ownerThread_!=std::this_thread::get_id()||issuer_!=&root||!current())
  return fail(d,Status::authority,0x14C5F0,bool(issuer_));
 if(phase_!=Phase::indexed)return fail(d,Status::phase,0x14C5F0,true);
 const auto call=root.localizationRendererCall14C5A0_;
 if(!call||!call->permitsEnglishSelection14C5F0(root,*this))
  return fail(d,Status::rendererPending,0x14C5F0,true);
 PreparedEnglish prepared;
 if(!prepareFirstEnglish14C5F0(prepared,d))return false;
 //Actual retained invocation is revalidated after every allocating operation.
 //Cancellation/teardown from an allocator hook cannot authorize publication.
 if(!call->permitsEnglishSelection14C5F0(root,*this))
  return fail(d,Status::rendererPending,0x14C690,true);
 return publishFirstEnglish14C690(std::move(prepared),d);
}
bool OriginalApplicationLocalization50KG::prepareFirstEnglish14C5F0(PreparedEnglish& out,Diagnostic& d)noexcept {
 if(ownerThread_!=std::this_thread::get_id()||!current())return fail(d,Status::authority,0x14C5FC,bool(issuer_));
 if(!firstEnglishPrefixCurrent()||out.buffer||out.issuer)return fail(d,Status::phase,0x14C670,true);
 const auto text=textMember(*archive_);
 if(!text.identity||!text.data||text.size!=10637)return fail(d,Status::shape,0x14C5FC,true);
 std::uint32_t pc=0x14C9AC;
 try{
  PreparedEnglish prepared;
  //Fixed native configured-English mapping, not mutable36ADC0/profile2DC,
  //source131230 file handles or original mount chronology.
  prepared.lookup="loc\\english\\L_UI_ENG.bin";
  OriginalStandardArchiveProvider50LA::MemberView mounted;
  if(!firstEnglishPrefixCurrent()||provider_->lookup132820(prepared.lookup,mounted)!=OriginalStandardArchiveProvider50LA::Lookup::present||
     mounted.data!=text.data||mounted.size!=text.size)return fail(d,Status::archive,0x14C9AC,true);
  prepared.providerRevision=mounted.revision;
  if(!scanEnglishBin(text,prepared.offsets,prepared.lengths,d))return false;
  //Original has no safe malloc-null branch. This native unpublished staging
  //preserves a cleanable indexed prefix, not an invented original rollback.
  pc=0x14C688;prepared.buffer=std::unique_ptr<char[]>(new char[text.size]);
  if(!firstEnglishPrefixCurrent()||provider_->lookup132820(prepared.lookup,mounted)!=OriginalStandardArchiveProvider50LA::Lookup::present||
     mounted.revision!=prepared.providerRevision||mounted.data!=text.data||mounted.size!=text.size)
   return fail(d,Status::authority,0x14C690,true);
  prepared.issuer=this;prepared.issuedOwner=issuedOwner_;prepared.member=text;
  out=std::move(prepared);d={Status::complete,0x14C688,0,true};return true;
 }catch(...){return fail(d,Status::allocation,pc,true);}
}
bool OriginalApplicationLocalization50KG::publishFirstEnglish14C690(PreparedEnglish&& prepared,Diagnostic& d)noexcept {
 if(ownerThread_!=std::this_thread::get_id()||!firstEnglishPrefixCurrent())
  return fail(d,Status::phase,0x14C690,bool(issuer_));
 const auto text=textMember(*archive_);
 OriginalStandardArchiveProvider50LA::MemberView mounted;
 std::array<std::size_t,467> offsets{},lengths{};
 if(!prepared.buffer||prepared.issuer!=this||!sameOwner(prepared.issuedOwner.lock(),issuedOwner_.lock())||
    prepared.member.identity!=text.identity||prepared.member.data!=text.data||prepared.member.size!=10637||
    prepared.lookup!="loc\\english\\L_UI_ENG.bin"||
    provider_->lookup132820(prepared.lookup,mounted)!=OriginalStandardArchiveProvider50LA::Lookup::present||
    mounted.revision!=prepared.providerRevision||mounted.data!=text.data||mounted.size!=text.size||
    !scanEnglishBin(text,offsets,lengths,d)||offsets!=prepared.offsets||lengths!=prepared.lengths)
  return fail(d,Status::authority,0x14C690,true);
 //No allocation, callback or failure after actual source00 publication.
 text00_=std::move(prepared.buffer); //14C690.
 text00_[text.size-1]=0; //14C6B4 BEFORE the full read, not an extra terminator.
 std::memcpy(text00_.get(),text.data,text.size); //14C6D8, all authentic bytes.
 //The in-memory provider has no fabricated guest file handle/close effect.
 textIdentity_=text.identity;textBytes_=text.size;textLookupName_.swap(prepared.lookup);
 textOffsets_=offsets;textLengths_=lengths;
 for(std::size_t i=0;i<467;++i){
  rows04_[i].text0=text00_.get()+offsets[i]; //14C740, ascending source ordinals.
  rows04_[i].textKnown=true;++publishedTextRows_;
 }
 selectionEpoch_=1;phase_=Phase::selected; //Native metadata, no guest language field.
 d={Status::complete,0x14C82C,467,true};return true;
}
bool OriginalApplicationLocalization50KG::sourceText18AC60(std::int32_t ordinal,const char*& out,Diagnostic& d)const noexcept {
 if(!current())return fail(d,Status::authority,0x18AC60,bool(issuer_));
 if(phase_!=Phase::selected)return fail(d,Status::rendererPending,0x14C5F0,true);
 if(!selectedGuard80A0().load()){
  selectedAssertion3D6B20().store(1); //18AC88, process assertion BEFORE guard.
  selectedGuard80A0().store(1); //18AC8C, not reset per dictionary/root.
 }
 //Source lazy prefix precedes its signed range assertion. Even if its assert
 //record was disabled, this native safety envelope never dereferences OOB.
 if(ordinal<0||ordinal>=count10_)return fail(d,Status::shape,0x18ACBC,true);
 out=rows04_[ordinal].text0;d={Status::complete,0x18AD14,std::size_t(ordinal),true};return true;
}
OriginalApplicationLocalization50KG::SelectedText::SelectedText(
 std::shared_ptr<OriginalApplicationLocalization50KG> owner,std::int32_t ordinal,
 std::uint64_t epoch,const char* text,std::size_t bytes)noexcept
 :owner_(std::move(owner)),ordinal_(ordinal),epoch_(epoch),text_(text),bytes_(bytes){}
OriginalApplicationLocalization50KG::SelectedText::~SelectedText()noexcept {
 if(!registered_)return; //A failed shared control-block allocation was not issued.
 //Native cleanup only. Do not require mutable provider sealing, healthy row
 //contents, a live lookup or a renderer service to release this lifetime edge.
 if(!owner_||owner_->ownerThread_!=std::this_thread::get_id()||
    owner_->phase_!=Phase::selected||owner_->selectionEpoch_!=epoch_||!owner_->selectedBorrowCount_)
  std::terminate();
 --owner_->selectedBorrowCount_;
}
bool OriginalApplicationLocalization50KG::SelectedText::currentOwnedBy(
 const std::shared_ptr<const SelectedText>& supplied,const Application& root)const noexcept {
 if(!owner_||owner_->ownerThread_!=std::this_thread::get_id()||!registered_||
    supplied.get()!=this||!sameOwner(supplied,issued_.lock())||owner_->issuer_!=&root||
    !owner_->current()||owner_->phase_!=Phase::selected||epoch_!=owner_->selectionEpoch_||
    !owner_->selectedBorrowCount_||ordinal_<0||ordinal_>=owner_->count10_)return false;
 return sameOwner(owner_,owner_->issuedOwner_.lock())&&
     text_==owner_->rows04_[ordinal_].text0&&bytes_==owner_->textLengths_[ordinal_];
}
bool OriginalApplicationLocalization50KG::SelectedText::readCurrent(
 const std::shared_ptr<const SelectedText>& supplied,const Application& root,std::string_view& out)const noexcept {
 if(!currentOwnedBy(supplied,root))return false;
 out=std::string_view(text_,bytes_);return true;
}
bool OriginalApplicationLocalization50KG::borrowText18AC60(const Application& root,std::int32_t ordinal,
 std::shared_ptr<const SelectedText>& out,Diagnostic& d)noexcept {
 if(ownerThread_!=std::this_thread::get_id()||issuer_!=&root||!current())
  return fail(d,Status::authority,0x18AC60,bool(issuer_));
 if(out||phase_!=Phase::selected||selectedBorrowCount_==std::numeric_limits<std::uint32_t>::max())
  return fail(d,Status::phase,0x18AC60,true);
 const char* text=nullptr;if(!sourceText18AC60(ordinal,text,d))return false;
 try{
  auto token=std::shared_ptr<SelectedText>(new SelectedText(issuedOwner_.lock(),ordinal,
      selectionEpoch_,text,textLengths_[ordinal]));
  //Allocator hooks cannot replace the actual root/selection under publication.
  if(!current()||phase_!=Phase::selected||selectedBorrowCount_==std::numeric_limits<std::uint32_t>::max()||
     token->epoch_!=selectionEpoch_||token->text_!=rows04_[ordinal].text0)
   return fail(d,Status::authority,0x18AD14,true);
  token->issued_=token;++selectedBorrowCount_;token->registered_=true;
  out=std::move(token);d={Status::complete,0x18AD14,std::size_t(ordinal),true};return true;
 }catch(...){return fail(d,Status::allocation,0x18AD14,true);}
}
bool OriginalApplicationLocalization50KG::lookup20ED30(std::string_view key,std::int32_t& out)const noexcept {
 if(!current()||(phase_!=Phase::indexed&&phase_!=Phase::selected)||!hash2C_||!hash2C_->hash8||!hash2C_->compareC)return false;
 key=cString(key);const auto raw=hash2C_->hash8(key);std::size_t bucket=0;
 if(!bucketFor(raw,std::uint32_t(hash2C_->count4),bucket))return false;
 for(const Node* node=hash2C_->buckets0[bucket];node;node=node->nextC){
  if(std::bit_cast<std::uint32_t>(node->hash4)==raw&&hash2C_->compareC(node->key8,key)){out=node->ordinal0;return true;}
 }
 return false;
}
bool OriginalApplicationLocalization50KG::inspect(Observation& out)const noexcept {
 if(!current())return false;
 const bool indexed=phase_==Phase::indexed||phase_==Phase::selected;
 out={count10_,indexed?std::size_t(count10_)*8:0,nameBytes_,nameBytes_*4,
  indexed?std::size_t(count10_):0,hash2C_?hash2C_->ownedNodes.size():0,
  baseKnown_,indexed,!text00_,phase_==Phase::selected,indexIdentity_,
  phase_==Phase::selected?0u:indexed?0x14C5D0u:0x14BC30u,
  textIdentity_,textBytes_,publishedTextRows_,selectedBorrowCount_};return true;
}
bool OriginalApplicationLocalization50KG::selectedText18AC60(std::int32_t,const char*&,
 Diagnostic& d)const noexcept {
 //No18AC60 guard/assertion occurs: caller has not reached selected-text access.
 if(!current())return fail(d,Status::authority,0x14C5D0,bool(issuer_));
 if(phase_==Phase::selected)return fail(d,Status::selectedBorrowRequired,0x18AC60,true);
 if(phase_!=Phase::indexed)return fail(d,Status::phase,0x14BC30,true);
 return fail(d,Status::rendererPending,0x14C5D0,true);
}
bool OriginalApplicationLocalization50KG::retire(const Application& root,Diagnostic& d)noexcept {
 if(!cleanupIdentityCurrent()||issuer_!=&root)return fail(d,Status::authority,0x17C8B8,bool(issuer_));
 if(selectedBorrowCount_)return fail(d,Status::selectedBorrowLive,0x14BBA8,true);
 // Native retained service transaction must retire before its dictionary.
 // It is not a guest child/count or a completed renderer operation.
 if(root.localizationRendererCall14C5A0_)return fail(d,Status::phase,0x17C8B8,true);
 if(root.fonts328Owner_)return fail(d,Status::precedingFonts,0x17C8A4,true);
 auto keepAlive=root.sourceLocalization32COwner_;const auto authority=authority_.lock();
 //Exact14BB80 payload order. Name keys are borrowed and are NOT dereferenced
 //by the later hash-node destructor, even after the name allocation is freed.
 text00_.reset();rows04_.reset();names24_.reset();names28_.reset();
 if(hash2C_){
  for(std::int32_t b=0;b<hash2C_->count4;++b){
   auto* node=hash2C_->buckets0[b];
   while(node){auto* next=node->nextC;hash2C_->ownedNodes[node->ordinal0].reset();node=next;}
  }
  hash2C_->buckets0.reset();hash2C_.reset();
 }
 phase_=Phase::retired;archive_.reset();provider_.reset();root.sourceLocalization32COwner_.reset(); //17C8C0.
 authority->localization32CNativeOwner_=nullptr; //Native base-retirement guard only.
 d={Status::complete,0x17C8C0,0,false};return true;
}
}
namespace original_main_entry50KG {
bool Application::stageLocalizationIndex17DEF4(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> archive,
 std::shared_ptr<rr::assets::OriginalApplicationLocalization50KG>& out,
 rr::assets::OriginalApplicationLocalizationDiagnostic50KG& d)const noexcept {
 return rr::assets::OriginalApplicationLocalization50KG::stage(*this,entitiesOwner_,std::move(archive),out,d);
}
bool Application::retireLocalization17C8B8(rr::assets::OriginalApplicationLocalization50KG& owner,
 rr::assets::OriginalApplicationLocalizationDiagnostic50KG& d)const noexcept {return owner.retire(*this,d);}
bool Application::selectLocalizationEnglish14C5F0(rr::assets::OriginalApplicationLocalization50KG& owner,
 rr::assets::OriginalApplicationLocalizationDiagnostic50KG& d)const noexcept {
 return owner.selectEnglish14C5F0(*this,d);
}
bool Application::borrowLocalizationText18AC60(rr::assets::OriginalApplicationLocalization50KG& owner,
 std::int32_t ordinal,std::shared_ptr<const rr::assets::OriginalApplicationLocalization50KG::SelectedText>& out,
 rr::assets::OriginalApplicationLocalizationDiagnostic50KG& d)const noexcept {
 return owner.borrowText18AC60(*this,ordinal,out,d);
}
}

namespace original_main_entry50KG {
bool Application::prepareFrontendLanguageServices(Frontend& frontend,
 rr::platform::VehicleWindowGl50DS& renderer,std::string& error)const {
 using Dictionary=rr::assets::OriginalApplicationLocalization50KG;
 using Fonts=rr::assets::OriginalApplicationFonts50LA;
 using Id=rr::assets::OriginalUiArchiveId50KG;
 //The existing immutable Root construction-thread identity is shared by
 //these native service owners; it does not depend on audio startup/readiness.
 if(garageAudioOwnerThread_!=std::this_thread::get_id()){
  error="language startup called off the actual Root owner thread";return false;
 }
 const auto display=raceDisplayOwner50LA_.lock();
 const auto sameOwner=[](const auto& a,const auto& b)noexcept {
  return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
 };
 const auto authentic=[](const auto& archive,Id id)noexcept {
  return archive&&archive->loaded()&&archive->identity()==rr::assets::originalUiArchiveIdentity50KG(id);
 };
 if(frontend.materialApplicationOwner!=this||!entitiesOwner_||
    !display||display->application_!=this||!display->frontendOwnerCurrent50KG()||
    !display->created_||display->retiring_||
    &display->renderer_!=&renderer||!renderer.initialized()||
    !materialContextOwner_||!materialContextOwner_->live()||
    !authentic(frontend.languageArchive,Id::localization)||!authentic(frontend.fontArchive,Id::fonts)) {
  error="same-root initialized frontend/context and authenticated language/font archives required";return false;
 }
 if(frontendLanguagePhase_==FrontendLanguagePhase::selected){
  const auto font=fonts328Owner_?fonts328Owner_->find282210(Fonts::Name::frontendVenus16):nullptr;
  if(!sourceLocalization32COwner_||!sourceLocalization32COwner_->current()||
     sourceLocalization32COwner_->phase_!=Dictionary::Phase::selected||
     localizationRendererCall14C5A0_||!standardMaterialDomainOwner_||!font||
     !sameOwner(fonts328Owner_->domain_,standardMaterialDomainOwner_)||
     !sameOwner(fonts328Owner_->context_,materialContextOwner_)||
     !Fonts::currentResource281F90(*standardMaterialDomainOwner_,*font)){
   error="retained native language/font ownership is no longer current";return false;
  }
  return true; //Actual retained selection, not a second14C5A0 invocation.
 }
 if(frontendLanguagePhase_!=FrontendLanguagePhase::untouched||sourceLocalization32COwner_||localizationRendererCall14C5A0_){
  error="language startup already attempted or owned by another source prefix";return false;
 }
 //The existing race-hosted UI can continue its ordinary immutable-resource
 //path without issuing this new dedicated-frontend source service. Leave32C
 //and the phase untouched: shared dialogs/Garage cannot mistake this no-op
 //for selected language or a native service-completion receipt.
 if(!display->frontendCreated_)return true;
 frontendLanguagePhase_=FrontendLanguagePhase::preparing;
 frontendLanguageArchive_=frontend.languageArchive;frontendFontsArchive_=frontend.fontArchive;
 const auto failed=[&](const char* stage,std::uint32_t pc){
  frontendLanguagePhase_=FrontendLanguagePhase::failed;
  error=std::string(stage)+" at source PC "+std::to_string(pc);return false;
 };
 try {
  const auto domain=standardMaterialDomain();const auto provider=domain->archiveProvider3E1A70();
  rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mount;
  if(!provider||!provider->mountAuthenticatedUi(frontendLanguageArchive_,mount)||
     !provider->mountAuthenticatedUi(frontendFontsArchive_,mount)||!provider->sealNativeMounts(mount))
   return failed("authentic native language/font namespace",0x14BD4C);
  std::shared_ptr<Dictionary> dictionary;
  rr::assets::OriginalApplicationLocalizationDiagnostic50KG d;
  if(!stageLocalizationIndex17DEF4(frontendLanguageArchive_,dictionary,d))return failed("language index",d.sourcePC);
  std::shared_ptr<rr::platform::OriginalLocalizationRendererLease50KG> call;
  rr::platform::OriginalLocalizationRendererDiagnostic50KG service;
  if(!stageLocalizationRenderer14C5A0(call,service))return failed("native language service owner",service.sourcePC);
  const bool completed=advanceLocalizationRenderer14C5A0(*call,service);
  const auto servicePC=service.sourcePC;
  const bool selected=completed&&selectLocalizationEnglish14C5F0(*dictionary,d);
  //Release the actual non-yielding native invocation while its Display still
  //exists. This neither undoes completion nor fabricates it on failure. The
  //attempt ledger and any genuine dictionary prefix remain process-owned.
  rr::platform::OriginalLocalizationRendererDiagnostic50KG release;
  if(!cancelLocalizationRenderer14C5A0(*call,release))return failed("native language service retirement",release.sourcePC);
  if(!completed)return failed("ordered native language completion",servicePC);
  if(!selected)return failed("authenticated English selection",d.sourcePC);
  // Mounting supplies lookup authority, not the canonical texture corpus.
  // Admit only the three genuine fonts reached by language/shared widgets.
  // This publishes immutable source bytes, NOT font/texture resource rows;
  // each original font call still performs its own whole-table281F34 scan.
  const auto fontResolver=domain->resolver3E1A70();
  const bool resolverPreparedBefore=fontResolver&&fontResolver->prepared();
  std::vector<rr::assets::OriginalStandardTextureSourceInput50LA> fontSources;
  std::size_t fontSourceBytes=0;
  for(const char* name:{"font\\venus16.pst","font\\denmrk16.pst","font\\denmrk14.pst"}){
   const auto pst=frontendFontsArchive_->find(name);
   rr::assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
   if(!pst.identity||!pst.data||!pst.size||
      provider->lookup132820(name,mounted)!=rr::assets::OriginalStandardArchiveProvider50LA::Lookup::present||
      !mounted.member||!mounted.data||mounted.size!=pst.size||
      std::memcmp(mounted.data,pst.data,pst.size))
    return failed("authenticated canonical frontend font PST source",0x281880);
   rr::assets::OriginalStandardTextureSourceInput50LA source;
   source.member=*mounted.member;source.bytes.assign(pst.data,pst.data+pst.size);
   fontSourceBytes+=source.bytes.size();fontSources.push_back(std::move(source));
  }
  rr::assets::OriginalStandardMaterialDiagnostic50LA fontCorpus;
  const bool fontSourceAdmitted=domain->appendOriginalSources({},fontSources,fontCorpus);
  std::cerr<<"original_frontend_font_corpus resolver_prepared_before="<<resolverPreparedBefore
      <<" resolver_prepared_after="<<(fontResolver&&fontResolver->prepared())
      <<" sources="<<fontSources.size()<<" bytes="<<fontSourceBytes<<" admitted="<<fontSourceAdmitted
      <<" status="<<static_cast<unsigned>(fontCorpus.status)
      <<" source_pc="<<fontCorpus.sourcePC<<'\n';
  if(!fontSourceAdmitted)return failed("canonical frontend font PST admission",
      fontCorpus.sourcePC?fontCorpus.sourcePC:0x281880);
  rr::assets::OriginalApplicationFontDiagnostic50LA font;
  if(!loadGarageTextFont29CB74(frontendFontsArchive_,Fonts::Name::frontendVenus16,font))
   return failed("canonical frontend Venus16 font",font.sourcePC);
  frontendLanguagePhase_=FrontendLanguagePhase::selected;return true;
 }catch(...){return failed("native language startup allocation/owner failure",0x17DEF4);}
}
bool Application::retireFrontendLanguageServices17C8A4(std::string& error)const noexcept {
 if(garageAudioOwnerThread_!=std::this_thread::get_id()){
  error="language shutdown called off the actual Root owner thread";return false;
 }
 if(frontendLanguagePhase_==FrontendLanguagePhase::untouched||frontendLanguagePhase_==FrontendLanguagePhase::retired)return true;
 //Only terminal Root shutdown, after actual frontend and gameplay display
 //retirement. A menu-to-race handoff must keep the selected bytes and fonts.
 if(!entitiesOwner_||
    !raceDisplayOwner50LA_.expired()||localizationRendererCall14C5A0_||
    (state&&state->sharedDialogs)||(sharedUiScopeOwner_&&!sharedUiScopeOwner_->closed_)||
    (sourceLocalization32COwner_&&(!sourceLocalization32COwner_->cleanupIdentityCurrent()||
                                 sourceLocalization32COwner_->selectedBorrowCount_))) {
  error="language shutdown requires actual display/dialog/text retirement";return false;
 }
 //Font close preflights EVERY live Entry lease before releasing any texture.
 //The dictionary's actual17C8B8 guard then observes a genuinely absent328.
 if(fonts328Owner_){
  if(fonts328Owner_.use_count()!=1||!fonts328Owner_->close282190()){
   error="canonical font registry or Entry still borrowed";return false;
  }
  fonts328Owner_.reset();
 }
 if(sourceLocalization32COwner_){
  rr::assets::OriginalApplicationLocalizationDiagnostic50KG d;
  if(!retireLocalization17C8B8(*sourceLocalization32COwner_,d)){
   error="actual localization destructor refused retained ownership";return false;
  }
 }
 frontendLanguageArchive_.reset();frontendFontsArchive_.reset();
 frontendLanguagePhase_=FrontendLanguagePhase::retired;return true;
}
}
