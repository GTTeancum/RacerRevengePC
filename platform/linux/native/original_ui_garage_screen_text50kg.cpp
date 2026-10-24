#include "original_ui_garage_screen_text50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cfenv>
#include "../../../src/game/original_ui_text50kg.h"
#include "../../../src/game/original_ui_text_position50kg.h"
#include <cstring>
#include <exception>
#include <cstdio>
#include <new>
#include <limits>
namespace rr::platform {
namespace {
using Status=OriginalUiGarageScreenTextStatus50KG;
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
 return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
void firstUse(std::atomic<std::uint8_t>& guard,std::atomic<std::int32_t>& assertion)noexcept {
 if(!guard.load(std::memory_order_acquire)){
  assertion.store(1,std::memory_order_relaxed); //Source word store precedes byte.
  guard.store(1,std::memory_order_release);
 }
}
bool fontDirectory(const std::string& s)noexcept {
 const auto n=s.size();if(n!=4&&n!=5)return false;
 constexpr char literal[]="font";
 for(std::size_t i=0;i<4;++i){const char c=s[i]>='A'&&s[i]<='Z'?char(s[i]+32):s[i];if(c!=literal[i])return false;}
 return n==4||s[4]=='\\'; //Never fold slash for source299BE0.
}
struct TextInputs {
 const char* material;const char* cel;const char* pst;std::string_view basename;
 assets::OriginalUiArchiveId50KG archive=assets::OriginalUiArchiveId50KG::garage;
};
bool textInputs(assets::OriginalApplicationFonts50LA::Name font,std::string_view material,
 std::int32_t capacity,TextInputs& out)noexcept {
 using N=assets::OriginalApplicationFonts50LA::Name;
 if(font==N::frontendVenus16&&material=="venus16popup"&&capacity==40){
  out={"font\\venus16popup.psm","font\\venus16.cel","font\\venus16.pst","venus16popup",assets::OriginalUiArchiveId50KG::controllerPopup};return true;
 }
 // Only the authored Garage text rows. These are concrete input
 // identities, not a changed source capacity limit or a renderer budget.
 if(font==N::frontendDenmrk16&&material=="denmrk16Top"&&
    (capacity==6||capacity==10||capacity==21||capacity==36)){
  out={"font\\denmrk16Top.psm","font\\denmrk16.cel","font\\denmrk16.pst","denmrk16Top"};return true;
 }
 if(font==N::frontendVenus16&&material=="venus16Top"&&capacity==14){
  out={"font\\venus16Top.psm","font\\venus16.cel","font\\venus16.pst","venus16Top"};return true;
 }
 if(font==N::frontendDenmrk16&&material=="denmrk16"&&(capacity==40||capacity==6)){
  out={"font\\denmrk16.psm","font\\denmrk16.cel","font\\denmrk16.pst","denmrk16"};return true;
 }
 if(font==N::frontendDenmrk14&&material=="denmrk14"&&
    (capacity==12||capacity==9||capacity==8||capacity==7||capacity==6)){
  out={"font\\denmrk14.psm","font\\denmrk14.cel","font\\denmrk14.pst","denmrk14"};return true;
 }
 return false;
}
bool retainedTextMaterial(const assets::OriginalUiArchive50KG& archive,
 const assets::OriginalUiMemberIdentity50KG* identity,const char* expected)noexcept {
 if(!identity||!expected)return false;
 for(std::size_t i=0;i<archive.memberCount();++i){const auto member=archive.member(i);
  if(member.identity!=identity)continue;
  const char* actual=identity->name;if(!actual)return false;
  const auto fold=[](char c)noexcept{return c=='\\'?'/' : c>='A'&&c<='Z'?char(c+32):c;};
  for(;*actual&&*expected;++actual,++expected)if(fold(*actual)!=fold(*expected))return false;
  return !*actual&&!*expected;
 }
 return false;
}
}
#define SHARED_BYTE(method) std::atomic<std::uint8_t>& OriginalUiGarageScreenText50KG::method()noexcept {static std::atomic<std::uint8_t> value{0};return value;}
#define SHARED_WORD(method) std::atomic<std::int32_t>& OriginalUiGarageScreenText50KG::method()noexcept {static std::atomic<std::int32_t> value{0};return value;}
SHARED_BYTE(guard8F14) SHARED_WORD(assertion445400)
SHARED_BYTE(guard8F18) SHARED_WORD(assertion445408)
SHARED_BYTE(guard8F20) SHARED_WORD(assertion445418)
SHARED_BYTE(guard8F24) SHARED_WORD(assertion445420)
SHARED_BYTE(guard8CD8) SHARED_WORD(assertion443640)
#undef SHARED_BYTE
#undef SHARED_WORD
void OriginalUiGarageScreenText50KG::PacketDelete::operator()(Qword* p)const noexcept {
 ::operator delete[](p,std::align_val_t{64});
}
OriginalUiGarageScreenText50KG::~OriginalUiGarageScreenText50KG()noexcept {
 //The enclosing real29C9C0 owns this retirement boundary. No hidden source
 //release from a C++ reverse-member destructor or automatic framebuffer drain.
 if(phase_!=Phase::constructed&&phase_!=Phase::retired)std::terminate();
}
bool OriginalUiGarageScreenText50KG::stop(Diagnostic& d,Status status,std::uint32_t pc)noexcept {
 d.status=status;d.sourcePC=pc;d.sourceEffectsCommitted=phase_!=Phase::constructed;
 d.publishedPrimitives=publishedPrimitives_;
 if(phase_==Phase::loading)phase_=Phase::failed;
 return false;
}
bool OriginalUiGarageScreenText50KG::authorityThread(const std::shared_ptr<Authority>& authority)noexcept {
 return authority&&authority->nativeOwnerThread_==std::this_thread::get_id();
}
bool OriginalUiGarageScreenText50KG::coldInputsCurrent()const noexcept {
 if(ownerThread_!=std::this_thread::get_id()||!authorityThread(authority_))return false;
 // Revalidate the exact immutable tuple admitted by loadColdSource2DFD90.
 // The two real470 Venus16Popup texts retain Popup, not Garage. This is not
 // permission for ordinary Garage text to adopt an arbitrary second archive.
 TextInputs inputs{};
 if(!textInputs(fontName_,materialName_,capacity5B_,inputs))return false;
 std::uint32_t pause=0;
 if(!lifetime_.readPause11C(pause)||!issuer_||!sameOwner(authority_,issuer_->entities())||
    !authority_->constructed_||authority_->released_)return false;
 return domain_&&domain_->prepared()&&context_&&context_->live()&&fonts_&&!fonts_->terminal()&&font4C_&&
  sameOwner(fonts_->domain_,domain_)&&sameOwner(fonts_->context_,context_)&&
  sameOwner(font4C_->domain_,domain_)&&sameOwner(font4C_->context_,context_)&&
  sameOwner(fonts_->find282210(fontName_),font4C_)&&
  Fonts::domainApplicationMatches(domain_,context_->materialApplication())&&
  domain_->materials382B38()->screenApplicationMatches(*context_->materialApplication())&&
  font4C_->archive_&&font4C_->archive_->loaded()&&font4C_->archive_->identity()==
    assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::fonts)&&
  font4C_->metrics().decoded()&&font4C_->texture281F90()&&font4C_->texture408_>=0&&
  Fonts::currentResource281F90(*domain_,*font4C_)&&
  garage_&&garage_->loaded()&&garage_->identity()==
    assets::originalUiArchiveIdentity50KG(inputs.archive)&&
  retainedTextMaterial(*garage_,materialIdentity_,inputs.material);
}
bool OriginalUiGarageScreenText50KG::loadCold2DFD90(
 const Application& root,std::shared_ptr<Domain> domain,std::shared_ptr<const Context> context,
 std::shared_ptr<Fonts> fonts,std::shared_ptr<const Fonts::Entry> font,std::shared_ptr<const Archive> garage,
 std::string& actualDirectory,const std::string& actualFontDirectory,std::int32_t capacity,Diagnostic& d)noexcept {
 return loadCold2DFD90(root,std::move(domain),std::move(context),std::move(fonts),std::move(font),
  std::move(garage),actualDirectory,actualFontDirectory,Fonts::Name::frontendDenmrk16,"denmrk16Top",capacity,d);
}
bool OriginalUiGarageScreenText50KG::loadCold2DFD90(
 const Application& root,std::shared_ptr<Domain> domain,std::shared_ptr<const Context> context,
 std::shared_ptr<Fonts> fonts,std::shared_ptr<const Fonts::Entry> font,std::shared_ptr<const Archive> garage,
 std::string& actualDirectory,const std::string& actualFontDirectory,Fonts::Name fontName,
 std::string_view materialName,std::int32_t capacity,Diagnostic& d)noexcept {
 return loadColdSource2DFD90(root,std::move(domain),std::move(context),std::move(fonts),std::move(font),
  std::move(garage),actualDirectory,actualFontDirectory,fontName,materialName,capacity,false,d);
}
bool OriginalUiGarageScreenText50KG::loadCost1FA360(
 const Application& root,std::shared_ptr<Domain> domain,std::shared_ptr<const Context> context,
 std::shared_ptr<Fonts> fonts,std::shared_ptr<const Fonts::Entry> font,std::shared_ptr<const Archive> garage,
 std::string& actualDirectory,const std::string& actualFontDirectory,Diagnostic& d)noexcept {
 return loadColdSource2DFD90(root,std::move(domain),std::move(context),std::move(fonts),std::move(font),
  std::move(garage),actualDirectory,actualFontDirectory,Fonts::Name::frontendDenmrk16,"denmrk16",6,true,d);
}
bool OriginalUiGarageScreenText50KG::loadColdSource2DFD90(
 const Application& root,std::shared_ptr<Domain> domain,std::shared_ptr<const Context> context,
 std::shared_ptr<Fonts> fonts,std::shared_ptr<const Fonts::Entry> font,std::shared_ptr<const Archive> garage,
 std::string& actualDirectory,const std::string& actualFontDirectory,Fonts::Name fontName,
 std::string_view materialName,std::int32_t capacity,bool initialNull,Diagnostic& d,std::string_view initialText)noexcept {
 d={};
 if(ownerThread_!=std::this_thread::get_id()||!authorityThread(root.entities())){
  d.status=Status::authority;d.sourcePC=0x2DFD90;return false;
 }
 std::uint32_t pause=0;
 if(!root.animationClock().readPause11C(pause)||!root.entities()->constructed_||root.entities()->released_)
  return stop(d,Status::authority,0x2DFD90);
 if(phase_!=Phase::constructed)return stop(d,Status::phase,0x2DFD90);
 if((initialNull&&!initialText.empty())||initialText.find('\0')!=std::string_view::npos||
    initialText.size()>static_cast<std::size_t>(capacity)||initialText.size()>255)
  return stop(d,Status::authority,0x2DFD90);
 TextInputs inputs{};
 if(!textInputs(fontName,materialName,capacity,inputs)||
    !fontDirectory(actualFontDirectory)||!domain||!context||!fonts||!font||!garage)
  return stop(d,Status::authority,0x2DFD90);
 try {
  //Validate without changing even constructor fields on a rejected issuer.
  if(!context->live()||!context->applicationAnimationClock().sameOwner(root.animationClock())||
     !sameOwner(context->materialApplication(),root.materialApplication)||
     !domain->prepared()||fonts->terminal()||
     !sameOwner(fonts->domain_,domain)||!sameOwner(fonts->context_,context)||
     !sameOwner(font->domain_,domain)||!sameOwner(font->context_,context)||
     !sameOwner(fonts->find282210(fontName),font)||
     !Fonts::domainApplicationMatches(domain,context->materialApplication())||
     !domain->materials382B38()->screenApplicationMatches(*context->materialApplication())||
     !font->archive_||!font->archive_->loaded()||font->archive_->identity()!=
       assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::fonts)||
     !font->metrics().decoded()||!font->texture281F90()||font->texture408_<0||!Fonts::currentResource281F90(*domain,*font)||
     !garage->loaded()||garage->identity()!=assets::originalUiArchiveIdentity50KG(inputs.archive))
   return stop(d,Status::authority,0x2DFD90);
  const auto material=garage->find(inputs.material);
  const auto cel=font->archive_->find(inputs.cel),pst=font->archive_->find(inputs.pst);
  const auto provider=domain->archiveProvider3E1A70();
  assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
  using Lookup=assets::OriginalStandardArchiveProvider50LA::Lookup;
  const auto materialSourceCurrent=[&]()noexcept {
   if(!material.identity||!material.data||material.size!=material.identity->bytes||
      provider->lookup132820(material.identity->name,mounted)!=Lookup::present||
      !mounted.member||!mounted.data||mounted.size!=material.size)return false;
   if(mounted.data==material.data)return true;
   //132820 retains the original authenticated mount on an equal-byte repeat.
   //A fresh Popup constructor owns a new archive allocation, not new contents.
   //Admit only this exact Popup PSM with its real canonical mount provenance;
   //ordinary Garage materials and the retained font atlas still require identity.
   if(materialName!="venus16popup"||*mounted.member!=material.identity->name)return false;
   assets::OriginalStandardArchiveProvider50LA::MountView mount;
   const auto* expected=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::controllerPopup);
   return expected&&provider->mount(mounted.mount,mount)&&mount.archiveName&&mount.archiveSha256&&
    *mount.archiveName==expected->discPath&&*mount.archiveSha256==expected->sha256&&
    mount.archiveBytes==expected->bytes&&mount.memberCount==expected->memberCount&&
    std::memcmp(mounted.data,material.data,material.size)==0;
  };
  if(!provider||!provider->sealed()||!materialSourceCurrent()||
     !cel.identity||!cel.data||!pst.identity||!pst.data||
     cel.identity!=font->celIdentity()||pst.identity!=font->atlasIdentity())
   return stop(d,Status::source,0x132820);
  const auto fontSourceCurrent=[&](const assets::OriginalUiMemberView50KG& source)noexcept {
   if(source.size!=source.identity->bytes||
      provider->lookup132820(source.identity->name,mounted)!=Lookup::present||
      !mounted.member||!mounted.data||mounted.size!=source.size)return false;
   if(mounted.data==source.data)return true;
   //282430 authenticates the supplied font archive against canonical bytes,
   //then retains that Entry's archive. After frontend reentry this may be a
   //fresh equal-byte allocation while132820 still owns the original mount.
   //Keep the exact Entry/resource identities above and require genuine UIFONTS
   //provenance plus the complete member bytes, not merely atlas dimensions.
   if(*mounted.member!=source.identity->name)return false;
   assets::OriginalStandardArchiveProvider50LA::MountView mount;
   const auto* expected=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::fonts);
   const auto* canonical=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::fonts,mounted.ordinal);
   return expected&&canonical&&canonical->name&&canonical->sha256&&source.identity->sha256&&
    *mounted.member==canonical->name&&canonical->bytes==source.size&&
    std::strcmp(canonical->sha256,source.identity->sha256)==0&&
    provider->mount(mounted.mount,mount)&&mount.archiveName&&mount.archiveSha256&&
    *mount.archiveName==expected->discPath&&*mount.archiveSha256==expected->sha256&&
    mount.archiveBytes==expected->bytes&&mount.memberCount==expected->memberCount&&
    std::memcmp(mounted.data,source.data,source.size)==0;
  };
  if(!fontSourceCurrent(cel)){
   std::fprintf(stderr,"original_screen_text_source_failed field=font_cel source_pc=1255456\n");
   return stop(d,Status::source,0x132820);
  }
  if(!fontSourceCurrent(pst)){
   std::fprintf(stderr,"original_screen_text_source_failed field=font_pst source_pc=1255456\n");
   return stop(d,Status::source,0x132820);
  }
  if(domain->materials382B38()->populationTerminal()||domain->materials382B38()->demandTerminal()||
     !domain->textures383400()||domain->textures383400()->populationTerminal())return stop(d,Status::phase,0x14D590);
  issuer_=&root;authority_=root.entities();lifetime_=root.animationClock();
  domain_=std::move(domain);context_=std::move(context);fonts_=std::move(fonts);
  garage_=std::move(garage);materialIdentity_=material.identity;
  fontName_=fontName;materialName_=inputs.basename;initialNullInput_=initialNull;phase_=Phase::loading;
  flags5A_|=8; //2DFDD8, BEFORE2DFC60(empty original constructor storage).
  firstUse(guard8F14(),assertion445400()); //2DFDF0 then2DFDF4.
  if(assertion445400().load()&&capacity>=101)return stop(d,Status::assertion,0x2DFE14);
  firstUse(guard8F18(),assertion445408()); //2DFE58 then2DFE5C.
  if(assertion445408().load()&&!font)return stop(d,Status::assertion,0x2DFE7C);
  capacity5B_=static_cast<std::uint8_t>(capacity);
  packetCount5D_=static_cast<std::uint8_t>((capacity5B_+35)/36);active5E_=packetCount5D_;
  font4C_=std::move(font); //2DFF48, after both source assertions.
  cells60_=&font4C_->metrics();cells60Published_=true;flags5A_|=3;
  d.sourcePC=0x2DF918;string8_=std::unique_ptr<char[]>(new char[std::size_t(capacity5B_)+1]);
  d.sourcePC=0x2DF934;glyphsC_=std::make_unique<Glyph[]>(capacity5B_);
  //Original save/copy/restore. Restoration after native failure is a native
  //exception envelope, not a claimed original exception/rollback path.
  d.sourcePC=0x2DF968;
  struct DirectoryScope {
   std::string& actual;std::string old;
   DirectoryScope(std::string& a,const std::string& font):actual(a),old(a){actual=font;}
   ~DirectoryScope(){actual.swap(old);}
  } directory(actualDirectory,actualFontDirectory);
  count0_=packetCount5D_; //2DF994 before pointer-array allocation.
  d.sourcePC=0x2DF99C;primitives4_=std::make_unique<std::unique_ptr<Primitive>[]>(count0_);
  source4Observation_=primitives4_.get();
  d.sourcePC=0x2DF9B4;packets64_=std::make_unique<Packet[]>(packetCount5D_);
  std::size_t first=0;
  for(std::size_t p=0;p<packetCount5D_;++p){
   auto& packet=packets64_[p];packet.glyphs=std::min<std::size_t>(36,capacity5B_-first);
   packet.qwords=packet.glyphs*6+6;
   d.sourcePC=0x2DFA2C;
   auto* storage=static_cast<Qword*>(::operator new[](packet.qwords*sizeof(Qword),std::align_val_t{64}));
   packet.buffer00.reset(storage);++allocatedPackets_; //2DFA40 publication BEFORE zero/helper.
   for(std::size_t q=0;q<packet.qwords;++q)std::construct_at(storage+q);
   if(!buildPacket2E03F0(packet,first,d))return false;
   first+=packet.glyphs;
   d.sourcePC=0x2DFA78;pendingPrimitive_=std::make_unique<Primitive>(); //2461E0 remains owned unpublished.
   if(!loadPrimitive2465B0(*pendingPrimitive_,actualDirectory,d))return false;
   pendingPrimitive_->flags0F&=std::uint8_t(~1u); //2DFAD0, packet is borrowed.
   pendingPrimitive_->packet10=packet.buffer00.get(); //2DFAE4.
   primitives4_[p]=std::move(pendingPrimitive_);++publishedPrimitives_; //ONLY2DFAF4.
  }
  //DirectoryScope now ends at the try-block boundary: source2DFB10 restore
  //occurs before2DFFA4 and the following initial-text/color stores.
 }catch(...){return stop(d,Status::allocation,d.sourcePC?d.sourcePC:0x2DFD90);}
 //29CBD0 passes NONNULL empty text, while Garage cost1FA360 passes NULL.
 //Preserve the genuinely different stores, not merely the displayed string.
 length5C_=0;
 if(initialNullInput_){
  string8_[0]=0;initializedStringBytes_=1; //2DFFDC/2DFFE4 ONLY.
 }else{
  length5C_=static_cast<std::uint8_t>(initialText.size()); //strlen then byte store2DFFB4, NOT setter truncation.
  std::memset(string8_.get(),0,capacity5B_); //309980 fills the remainder through capacity.
  if(!initialText.empty())std::memcpy(string8_.get(),initialText.data(),initialText.size());
  string8_[capacity5B_]=0;initializedStringBytes_=std::size_t(capacity5B_)+1; //2DFFD8.
 }
 vector10_[0]=vector10_[1]=vector20_[0]=vector20_[1]=0;
 for(std::size_t g=0;g<capacity5B_;++g)for(auto& v:glyphsC_[g].vertices){
  //133F20 writes RGB halves,1D8850 writes signed16alpha>>2 separately.
  v.rgba1C[0]->words={{127,127,127,63}};
 }
 color54_=0xffffffffu;alpha58_=255;
 scale40_=1;angle44_=0;vector30_[0]=vector30_[1]=0;depth48_=1;justification50_=0;
 flags5A_=std::uint8_t((flags5A_|3u)&~4u);phase_=Phase::coldLoaded;
 d={};d.sourceEffectsCommitted=true;d.publishedPrimitives=publishedPrimitives_;return true;
}
bool OriginalUiGarageScreenText50KG::buildPacket2E03F0(Packet& packet,std::size_t first,Diagnostic& d)noexcept {
 auto* q=packet.buffer00.get();if(!q||packet.qwords!=packet.glyphs*6+6||first+packet.glyphs>capacity5B_)
  return stop(d,Status::phase,0x2E03F0);
 q[1].words={{0x11000000u,0x03000000u,0x02000200u,0x6c008000u|static_cast<std::uint32_t>((packet.glyphs*6+2)<<16)}};
 packet.vif04=q+1;
 //GIFtag: NLOOP2*n,EOP1,PRE1,PRIM0x5e,FLG0,NREG3; registers2,1,5.
 q[2].words={{static_cast<std::uint32_t>(0x8000u+packet.glyphs*2),0x302f4000u,0x00000512u,0}};
 packet.gif08=q+2;
 q[3].words={{0,0,0x40c00000u,0}};packet.giftag0C=q+3;
 std::size_t cursor=4;
 for(std::size_t i=first;i<first+packet.glyphs;++i){
  auto& glyph=glyphsC_[i];
  for(std::size_t v=0;v<2;++v){
   auto& vertex=glyph.vertices[v];
   if(!v)firstUse(guard8F20(),assertion445418());else firstUse(guard8F24(),assertion445420());
   const auto asserted=!v?assertion445418().load():assertion445420().load();
   if((asserted&&vertex.aliases10==2)||vertex.aliases10>=2)return stop(d,Status::assertion,!v?0x2E05E8:0x2E0710);
   const auto slot=vertex.aliases10;
   q[cursor].words={};vertex.rgba1C[slot]=q+cursor++;
   q[cursor].words={};vertex.stq14[slot]=q+cursor++;
   q[cursor].words={{0,0,0x3f800000u,0}};vertex.xyz24[slot]=q+cursor++;
   ++vertex.aliases10;
  }
 }
 q[cursor].words={{0x140000b0u,0x10000000u,0,0}};
 q[0].words[0]=0x30000000u|static_cast<std::uint32_t>(packet.glyphs*6+4);
 //q[0].words[1] is NOT a source pointer encoding. Native equivalent is typed.
 packet.dmaData4=q+1;packet.dmaPointerPublished=true;
 return true;
}
bool OriginalUiGarageScreenText50KG::loadPrimitive2465B0(Primitive& primitive,const std::string& directory,Diagnostic& d)noexcept {
 try {
  std::string requested=directory;if(!requested.empty()&&requested.back()!='\\')requested+='\\';
  requested+=materialName_;
  std::int32_t row=-1;
  const auto table=domain_->materials382B38();const bool found=table->find298C80(requested,row,d.material);
  primitive.material00=row;primitive.materialStored=true; //299CB4 before assertion.
  firstUse(guard8CD8(),assertion443640()); //299CCC then299CD0.
  if(assertion443640().load()&&primitive.material00==-1)return stop(d,Status::assertion,0x299CF8);
  if(!found)return stop(d,Status::material,d.material.sourcePC);
  if(!table->retainScreen14D590(row,d.material))return stop(d,Status::material,d.material.sourcePC);
  primitive.materialRetained=true;
  primitive.type0C=15; //2465CC, AFTER299BE0 returns.
  return true;
 }catch(...){return stop(d,Status::allocation,0x299BE0);}
}
void OriginalUiGarageScreenText50KG::releasePrimitive246260(std::unique_ptr<Primitive>& p)noexcept {
 if(!p)return;
 if(p->optional04||p->optional14||(p->flags0F&1u))std::terminate();
 if(p->materialRetained){
  if(!domain_||!domain_->materials382B38()->release14D500(p->material00))std::terminate();
  p->materialRetained=false;
 }
 p.reset(); //Borrowed packet10 is NEVER freed here.
}
bool OriginalUiGarageScreenText50KG::draw2E0170(OriginalUiGarageDrawContribution50KG& draw,
 const OriginalUiGarageTextOverlay50KG& owner,Diagnostic& d)noexcept {
 d={};
 if(!draw.textInvocation29D3F0Current(owner)||!coldInputsCurrent())return stop(d,Status::authority,0x2E01C0);
 if(!authority_->screenTextEnabled383688_)return true;
 OriginalUiGarageDrawContribution50KG::TextRenderInputs input;std::uint32_t pc=0;
 if(!draw.readTextInputs29D3F0(owner,input,pc))return stop(d,Status::authority,pc);
 if(!regenerate2E0F50(input.viewportWidth1E8,input.viewportHeight1EC,input.videoDepthMaximum60,d))return false;
 for(std::size_t p=0;p<active5E_;++p)if(!draw.collectScreenTextPrimitive246760(*this,p,d))return false;
 return true;
}
bool OriginalUiGarageScreenText50KG::draw2E0170(OriginalUiGarageDrawContribution50KG& draw,
 Diagnostic& d)noexcept {
 d={};
 if(!draw.screenTextInvocation2E0170Current(*this)||!coldInputsCurrent())return stop(d,Status::authority,0x2E01C0);
 if(!authority_->screenTextEnabled383688_)return true;
 OriginalUiGarageDrawContribution50KG::TextRenderInputs input;std::uint32_t pc=0;
 if(!draw.readPageTextInputs2E0170(*this,input,pc))return stop(d,Status::authority,pc);
 if(!regenerate2E0F50(input.viewportWidth1E8,input.viewportHeight1EC,input.videoDepthMaximum60,d))return false;
 for(std::size_t p=0;p<active5E_;++p)if(!draw.collectScreenTextPrimitive246760(*this,p,d))return false;
 return true;
}
bool OriginalUiGarageScreenText50KG::drawWarning4702E0170(OriginalUiGarageDrawContribution50KG& draw,
 OriginalUiSharedDialogs50KG& shared,Diagnostic& d)noexcept {
 d={};
 if(!shared.warning470TextCurrent(draw,*this)||!coldInputsCurrent())return stop(d,Status::authority,0x2E01C0);
 if(!authority_->screenTextEnabled383688_)return true;
 OriginalUiGarageDrawContribution50KG::TextRenderInputs input;std::uint32_t pc=0;
 if(!draw.readWarning470TextInputs2E0170(shared,*this,input,pc))return stop(d,Status::authority,pc);
 if(!regenerate2E0F50(input.viewportWidth1E8,input.viewportHeight1EC,input.videoDepthMaximum60,d))return false;
 bool effects=d.sourceEffectsCommitted;
 for(std::size_t p=0;p<active5E_;++p){
  if(!draw.collectWarning470Text246760(shared,*this,p,d)){d.sourceEffectsCommitted|=effects;return false;}
  effects|=d.sourceEffectsCommitted;
 }
 d.sourceEffectsCommitted=effects;return true;
}
bool OriginalUiGarageScreenText50KG::applyOverlay29D690(std::uint32_t color,
 std::int16_t alpha,float scale,float rotation,std::int32_t x,std::int32_t y,
 std::int32_t justification,Diagnostic& d)noexcept {
 d={};
 if(nativeGarageFifoPins_||phase_!=Phase::coldLoaded||!coldInputsCurrent()||!glyphsC_||
    !std::isfinite(scale)||!std::isfinite(rotation)||scale==0)
  return stop(d,Status::phase,0x29D690);
 // Every source vertex alias is validated before the first color write.
 for(std::size_t i=0;i<capacity5B_;++i)for(const auto& v:glyphsC_[i].vertices){
  if(!v.aliases10||v.aliases10>2)return stop(d,Status::phase,0x133F20);
  for(std::size_t a=0;a<v.aliases10;++a)if(!v.rgba1C[a])return stop(d,Status::phase,0x133F20);
 }
 const bool rgb=color54_!=color,a=alpha58_!=alpha;
 const auto rgba=game::originalUiTextPacketColor50KG(color,alpha);
 if(rgb||a)for(std::size_t i=0;i<capacity5B_;++i)for(auto& v:glyphsC_[i].vertices)
  for(std::size_t n=0;n<v.aliases10;++n){
   if(rgb)for(std::size_t c=0;c<3;++c)v.rgba1C[n]->words[c]=rgba[c];
   if(a)v.rgba1C[n]->words[3]=rgba[3];
  }
 color54_=color;alpha58_=alpha;
 if(scale40_!=scale){scale40_=scale;flags5A_|=3u;}
 if(angle44_!=rotation){angle44_=rotation;flags5A_|=2u;}
 if(vector10_[0]!=float(x)||vector10_[1]!=float(y)){
  vector10_[0]=float(x);vector10_[1]=float(y);flags5A_|=3u;
 }
 const auto j=static_cast<std::uint32_t>(justification);
 if(justification50_!=j){justification50_=j;flags5A_|=3u;}
 d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiGarageScreenText50KG::regenerate2E0F50(std::uint32_t viewportWidth,
 std::uint32_t viewportHeight,float videoMaximum,Diagnostic& d)noexcept {
 d={};
 if(nativeGarageFifoPins_||phase_!=Phase::coldLoaded||!coldInputsCurrent()||
    !string8_||!glyphsC_||!packets64_||!primitives4_||length5C_>capacity5B_||
    string8_[length5C_]!=0||angle44_!=0||!std::isfinite(scale40_)||scale40_==0||
    !std::isfinite(depth48_)||!viewportWidth||!viewportHeight||!std::isfinite(videoMaximum)||videoMaximum<0)
  return stop(d,Status::phase,0x2E0F50);
 // Clean geometry retains the previous source vertex images. The source
 // still refreshes per-primitive viewport constants on every draw.
 if(!(flags5A_&7u)){
  if(active5E_>packetCount5D_)return stop(d,Status::phase,0x2E103C);
  for(std::size_t p=0;p<active5E_;++p)if(!packets64_[p].gif08||!packets64_[p].giftag0C)
   return stop(d,Status::phase,0x2E103C);
  for(std::size_t p=0;p<active5E_;++p){
   auto& packet=packets64_[p];
   packet.gif08->words[1]=(packet.gif08->words[1]&~0x03ff8000u)|(0x5eu<<15);
   packet.giftag0C->words={{std::bit_cast<std::uint32_t>(float(viewportWidth)),
    std::bit_cast<std::uint32_t>(float(viewportHeight)),0x40c00000u,0}};
  }
  d.sourceEffectsCommitted=true;return true;
 }
 const auto* texture=font4C_->texture281F90();
 if(!texture||!texture->sourceWidth14()||!texture->sourceHeight18()||
    *texture->sourceWidth14()<=0||*texture->sourceHeight18()<=0)
  return stop(d,Status::source,0x281F90);
 game::OriginalUiTextInput50KG input;input.font=&font4C_->metrics();
 input.text=reinterpret_cast<const std::uint8_t*>(string8_.get());input.length=length5C_;
 game::OriginalUiTextLayout50KG run;game::OriginalUiTextDiagnostic50KG gd;
 auto layout=[&](float x,float y)noexcept {
  input.localX=x;input.localY=y;
  return game::layoutOriginalUiTextDimensions50LA(input,
   static_cast<std::uint32_t>(*texture->sourceWidth14()),
   static_cast<std::uint32_t>(*texture->sourceHeight18()),run,gd);
 };
 if(!layout(0,0))return stop(d,Status::source,gd.sourcePC);
 float x=vector20_[0],y=vector20_[1];
 if(flags5A_&1u){
  x=vector10_[0];y=vector10_[1];
  if(length5C_&&justification50_){
   std::int32_t width=0,height=0,coordinate=0;
   if(!game::roundOriginalUiTextMetric50KG(run.widthBeforeScaleAndRounding,scale40_,width)||
      !game::roundOriginalUiTextMetric50KG(run.firstGlyphHeightBeforeScaleAndRounding,scale40_,height))
    return stop(d,Status::source,0x2E13A0);
   if(justification50_!=2){
    volatile float shift=(justification50_==1||justification50_==3)?float(width)*.5f:float(width);
    if(!game::roundOriginalUiTextMetric50KG(x-shift,1,coordinate))return stop(d,Status::source,0x2E149C);
    x=float(coordinate);
   }
   if(justification50_==2||justification50_==3){
    volatile float shift=float(height)*.5f;
    if(!game::roundOriginalUiTextMetric50KG(y-shift,1,coordinate))return stop(d,Status::source,0x2E14DC);
    y=float(coordinate);
   }
  }
 }
 const bool layoutDirty=(flags5A_&5u)!=0;
 if(layoutDirty&&!layout(x/scale40_,y/scale40_))return stop(d,Status::source,gd.sourcePC);
 const auto active=std::size_t((length5C_+35u)/36u);
 if(active>packetCount5D_)return stop(d,Status::phase,0x2E0880);
 for(std::size_t p=0;p<packetCount5D_;++p){
  const auto& packet=packets64_[p];
  if(!packet.buffer00||!packet.gif08||!packet.giftag0C||!primitives4_[p]||
     primitives4_[p]->packet10!=packet.buffer00.get())return stop(d,Status::phase,0x2E103C);
 }
 const float z=depth48_<=0?-depth48_:videoMaximum-depth48_;
 if(!std::isfinite(z))return stop(d,Status::source,0x2E1110);
 for(std::size_t n=0;n<length5C_;++n)for(std::size_t v=0;v<2;++v){
  const auto& vertex=glyphsC_[n].vertices[v];const auto& point=run.quads[n].vertices[v?2:0];
  if(!vertex.aliases10||vertex.aliases10>2||
     !std::isfinite((layoutDirty?point.x:vertex.authored00[0])*scale40_)||
     !std::isfinite((layoutDirty?point.y:vertex.authored00[1])*scale40_))
   return stop(d,Status::source,0x2E1284);
  for(std::size_t a=0;a<vertex.aliases10;++a)if(!vertex.stq14[a]||!vertex.xyz24[a])
   return stop(d,Status::phase,0x2E1284);
 }
 active5E_=static_cast<std::uint8_t>(active);vector20_[0]=x;vector20_[1]=y;
 for(std::size_t p=0;p<active;++p){
  auto& packet=packets64_[p];
  const auto vertices=2*std::min<std::size_t>(36,length5C_-p*36);
  packet.gif08->words[0]=(packet.gif08->words[0]&~0x7fffu)|static_cast<std::uint32_t>(vertices);
  packet.gif08->words[1]=(packet.gif08->words[1]&~0x03ff8000u)|(0x5eu<<15);
  packet.giftag0C->words={{std::bit_cast<std::uint32_t>(float(viewportWidth)),
   std::bit_cast<std::uint32_t>(float(viewportHeight)),0x40c00000u,0}};
 }
 for(std::size_t n=0;n<length5C_;++n)for(std::size_t v=0;v<2;++v){
  auto& vertex=glyphsC_[n].vertices[v];const auto& point=run.quads[n].vertices[v?2:0];
  if(layoutDirty){vertex.authored00[0]=point.x;vertex.authored00[1]=point.y;}
  vertex.authored00[2]=z;
  for(std::size_t a=0;a<vertex.aliases10;++a){
   // The retained legacy names are misleading: source +14 is XYZ,
   // while +24 is STQ. Preserve actual aliases, not a new quad stream.
   if(layoutDirty){
    vertex.xyz24[a]->words[0]=std::bit_cast<std::uint32_t>(point.u);
    vertex.xyz24[a]->words[1]=std::bit_cast<std::uint32_t>(point.v);
   }
   auto transformed=vertex.authored00;transformed[0]*=scale40_;transformed[1]*=scale40_;
   for(std::size_t c=0;c<4;++c)vertex.stq14[a]->words[c]=std::bit_cast<std::uint32_t>(transformed[c]);
  }
 }
 flags5A_=std::uint8_t(flags5A_&~7u);d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiGarageScreenText50KG::writeText29D990(std::string_view text,Diagnostic& d)noexcept {
 d={};
 const auto rejectSetter=[&](const char* stage)noexcept {
  const bool storageBounded=phase_==Phase::coldLoaded&&string8_&&length5C_<=capacity5B_;
  const bool terminated=storageBounded&&string8_[length5C_]==0;
  const bool sameLength=text.size()==length5C_;
  const bool equal=terminated&&sameLength&&(text.empty()||!std::memcmp(string8_.get(),text.data(),text.size()));
  std::fprintf(stderr,"original_garage_text_setter_rejected stage=%s pc=2742592 pins=%u phase=%u cold=%u storage=%u terminated=%u length=%u capacity=%u input_bytes=%zu input_nul=%u same_length=%u exact_equal=%u flags=%u\n",
   stage,static_cast<unsigned>(nativeGarageFifoPins_),static_cast<unsigned>(phase_),unsigned(coldInputsCurrent()),
   unsigned(storageBounded),unsigned(terminated),unsigned(length5C_),unsigned(capacity5B_),text.size(),
   unsigned(text.find('\0')!=std::string_view::npos),unsigned(sameLength),unsigned(equal),unsigned(flags5A_));
  return stop(d,Status::phase,0x29D940);
 };
 if(phase_!=Phase::coldLoaded||!coldInputsCurrent()||!string8_||
    length5C_>capacity5B_||string8_[length5C_]||
    text.size()>std::size_t(std::numeric_limits<std::int32_t>::max())||
    text.find('\0')!=std::string_view::npos)return rejectSetter("screen_guard");
 //29D970..988 compare the FULL source strlen before capacity truncation.
 if(text.size()==length5C_&&(text.empty()||!std::memcmp(string8_.get(),text.data(),text.size())))return true;
 // The source equality branch above performs no store. Only an actual text
 // change needs exclusive access to the retained packet/string owner; do not
 // admit a truncated-prefix match or clear its live FIFO borrow.
 if(nativeGarageFifoPins_)return rejectSetter("screen_pinned_mutation");
 const auto copied=std::min<std::size_t>(text.size(),capacity5B_);
 length5C_=static_cast<std::uint8_t>(copied); //29D99C before copy.
 if(copied)std::memcpy(string8_.get(),text.data(),copied); //29D9A8.
 string8_[length5C_]=0; //29D9C8.
 flags5A_=std::uint8_t((flags5A_&~1u)|1u); //29D9DC.
 flags5A_=std::uint8_t((flags5A_&~2u)|2u); //29D9EC.
 d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiGarageScreenText50KG::preflightRetirement2E1340(Diagnostic& d)const noexcept {
 d={};
 if(ownerThread_!=std::this_thread::get_id()){d.status=Status::authority;d.sourcePC=0x2DFC60;return false;}
 if(nativeGarageFifoPins_){d.status=Status::phase;d.sourcePC=0x2E1340;return false;}
 if(phase_==Phase::retired)return true;
 if(phase_==Phase::loading){d.status=Status::phase;d.sourcePC=0x2DFC60;return false;}
 //Preflight ALL real ref releases before the first source payload free. A live
 //collector/FIFO borrow must leave string/glyph/material/packet untouched.
 if(publishedPrimitives_&&(!primitives4_||publishedPrimitives_>count0_)){
  d.status=Status::phase;d.sourcePC=0x14D500;return false;
 }
 auto valid=[&](const std::unique_ptr<Primitive>& p)noexcept {
  if(!p)return true;
  if(p->optional04||p->optional14||(p->flags0F&1u))return false;
  if(!p->materialRetained)return true;
  // Several packets may retain the SAME canonical row. Validate their whole
  // release total before any source payload free, not just refs>0 per packet.
  std::size_t releases=pendingPrimitive_&&pendingPrimitive_->materialRetained&&
      pendingPrimitive_->material00==p->material00?1u:0u;
  for(std::size_t i=0;i<publishedPrimitives_;++i){const auto& owned=primitives4_[i];
   if(owned&&owned->materialRetained&&owned->material00==p->material00)++releases;
  }
  assets::OriginalStandardMaterialRowView50LA row;
  return domain_&&domain_->materials382B38()->canRetireNativeCollector29C300()&&
   domain_->materials382B38()->row(p->material00,row)&&row.references08&&*row.references08>0&&
   static_cast<std::uint64_t>(*row.references08)>=releases;
 };
 if(!valid(pendingPrimitive_)){d.status=Status::phase;d.sourcePC=0x14D500;return false;}
 for(std::size_t i=0;i<publishedPrimitives_;++i)if(!primitives4_||!valid(primitives4_[i])){
  d.status=Status::phase;d.sourcePC=0x14D500;return false;
 }
 return true;
}
bool OriginalUiGarageScreenText50KG::retire2E1340(Diagnostic& d)noexcept {
 if(!preflightRetirement2E1340(d))return false;
 if(phase_==Phase::retired)return true;
 string8_.reset(); //2DFC84/8C.
 glyphsC_.reset(); //2DFCA0/A8.
 for(std::size_t i=0;i<publishedPrimitives_;++i)releasePrimitive246260(primitives4_[i]);
 //Unpublished latest primitive is native failure ownership, not an invented
 //source4 slot. It is destroyed before packet storage it may borrow.
 releasePrimitive246260(pendingPrimitive_);
 if(primitives4_){primitives4_.reset();primitiveArrayFreed_=true;count0_=0;} //2DFD08 only when4 exists.
 //Source4 stays stale after array free; source4Observation_ records it without
 //ever dereferencing freed storage. The C++owner's reset is NOT a source clear.
 if(packets64_){for(std::size_t i=0;i<allocatedPackets_;++i)packets64_[i].buffer00.reset();packets64_.reset();}
 font4C_.reset();fonts_.reset();context_.reset();garage_.reset();domain_.reset();
 phase_=Phase::retired;d.sourceEffectsCommitted=true;return true;
}
}
namespace original_main_entry50KG {
bool Application::prepareGarageScreenText29CBD8(std::shared_ptr<rr::platform::OriginalUiGarageScreenText50KG>&,
 rr::platform::OriginalUiGarageScreenTextDiagnostic50KG& d)const noexcept {
 using S=rr::platform::OriginalUiGarageScreenTextStatus50KG;d={};
 if(!rr::platform::OriginalUiGarageScreenText50KG::authorityThread(entitiesOwner_)){d.status=S::authority;d.sourcePC=0x2A9458;return false;}
 const auto dictionary=sourceLocalization32COwner_;
 if(!dictionary||dictionary->issuer_!=this||!dictionary->current()){d.status=S::authority;d.sourcePC=0x2A9458;return false;}
 std::int32_t ordinal=-1;
 if(!dictionary->lookup20ED30("STR_ACCEPT",ordinal)||ordinal!=9){d.status=S::source;d.sourcePC=0x2A9458;return false;}
 const char* selected=nullptr;
 if(!dictionary->selectedText18AC60(ordinal,selected,d.localization)){
  d.status=S::selectedTextPending;d.sourcePC=d.localization.sourcePC;return false;
 }
 //A raw future pointer is still NOT a retained selected-language lifetime.
 //No literal/decodedEnglish substitution, no font load or ScreenText allocation,
 //and no output mutation until that genuine typed producer is implemented.
 d.status=S::selectedTextPending;d.sourcePC=0x18AC60;return false;
}
}

namespace rr::platform {
bool OriginalUiGarageScreenText50KG::borrowQueuedFontAtlas(FontAtlasBorrow& out,Diagnostic& d)const noexcept {
 d={};
 if(!nativeGarageFifoPins_||!coldInputsCurrent()||!font4C_||!font4C_->archive_||
    !font4C_->archive_->loaded()||!font4C_->pst_.identity||!font4C_->pst_.data||
    font4C_->pst_.size!=font4C_->pst_.identity->bytes||font4C_->texture408_<0||
    !Fonts::currentResource281F90(*domain_,*font4C_)){
  d.status=Status::authority;d.sourcePC=0x281F90;return false;
 }
 const auto member=font4C_->archive_->find(font4C_->pst_.identity->name);
 assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
 const auto provider=domain_->archiveProvider3E1A70();
 if(member.identity!=font4C_->pst_.identity||member.data!=font4C_->pst_.data||member.size!=font4C_->pst_.size||
    !provider||!provider->sealed()||provider->lookup132820(member.identity->name,mounted)!=
       assets::OriginalStandardArchiveProvider50LA::Lookup::present||
    !mounted.member||!mounted.data||mounted.size!=member.size){d.status=Status::source;d.sourcePC=0x132820;return false;}
 if(mounted.data!=member.data){
  //The queued Entry retains its authenticated archive after282430 reentry,
  //while132820 keeps its original equal-byte backing. Match the same narrow
  //canonical UIFONTS proof used by loadColdSource2DFD90, not pointer freshness.
  assets::OriginalStandardArchiveProvider50LA::MountView mount;
  const auto* expected=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::fonts);
  const auto* canonical=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::fonts,mounted.ordinal);
  if(!expected||!canonical||!canonical->name||!canonical->sha256||!member.identity->sha256||
     *mounted.member!=member.identity->name||*mounted.member!=canonical->name||canonical->bytes!=member.size||
     std::strcmp(canonical->sha256,member.identity->sha256)!=0||
     !provider->mount(mounted.mount,mount)||!mount.archiveName||!mount.archiveSha256||
     *mount.archiveName!=expected->discPath||*mount.archiveSha256!=expected->sha256||
     mount.archiveBytes!=expected->bytes||mount.memberCount!=expected->memberCount||
     std::memcmp(mounted.data,member.data,member.size)!=0){d.status=Status::source;d.sourcePC=0x132820;return false;}
 }
 out={font4C_,font4C_->archive_,font4C_->pst_,font4C_->texture408_};return true;
}
bool OriginalUiGarageScreenText50KG::lowerQueuedPacket(std::size_t primitiveIndex,
 const std::array<std::uint32_t,2>& offset,LoweredPacket& out,Diagnostic& d)const {
 d={};auto reject=[&](std::uint32_t pc){d.status=Status::source;d.sourcePC=pc;return false;};
 if(!nativeGarageFifoPins_||!coldInputsCurrent()||phase_!=Phase::coldLoaded||
    (flags5A_&7u)||std::fegetround()!=FE_TONEAREST||offset[0]>65535u||offset[1]>65535u||
    primitiveIndex>=active5E_||primitiveIndex>=packetCount5D_||!packets64_||!primitives4_||!glyphsC_)
  return reject(0x327ED8);
 const auto& packet=packets64_[primitiveIndex];const auto& primitive=primitives4_[primitiveIndex];
 const auto* q=packet.buffer00.get();const auto first=primitiveIndex*36u;
 if(!q||!primitive||!primitive->materialRetained||primitive->material00<0||primitive->type0C!=15||
    primitive->optional04||primitive->optional14||primitive->flags0F||primitive->packet10!=q||
    packet.qwords!=packet.glyphs*6+6||packet.glyphs>36||first>=length5C_||
    first+packet.glyphs>capacity5B_||packet.vif04!=q+1||packet.gif08!=q+2||packet.giftag0C!=q+3||
    !packet.dmaPointerPublished||packet.dmaData4!=q+1)return reject(0x2E03F0);
 const auto count=std::min<std::size_t>(36,length5C_-first);
 if(q[0].words[0]!=(0x30000000u|static_cast<std::uint32_t>(packet.glyphs*6+4))||
    q[1].words!=std::array<std::uint32_t,4>{{0x11000000u,0x03000000u,0x02000200u,
         0x6c008000u|static_cast<std::uint32_t>((packet.glyphs*6+2)<<16)}}||
    q[2].words!=std::array<std::uint32_t,4>{{0x8000u+static_cast<std::uint32_t>(count*2),0x302f4000u,0x512u,0}}||
    q[3].words[2]!=0x40c00000u||q[3].words[3]||
    q[4+packet.glyphs*6].words!=std::array<std::uint32_t,4>{{0x140000b0u,0x10000000u,0,0}})
  return reject(0x327ED8);
 const float width=std::bit_cast<float>(q[3].words[0]),height=std::bit_cast<float>(q[3].words[1]);
 if(!std::isfinite(width)||!std::isfinite(height)||width<=0||height<=0)return reject(0x327F40);
 LoweredPacket next;next.vertices.reserve(count*4);next.stq.reserve(count*4);
 next.rasterW.reserve(count*4);next.fog.reserve(count*4);
 for(std::size_t n=0;n<count;++n){
  std::array<assets::Vertex48B,2> endpoints{};std::array<std::array<float,3>,2> texture{};
  for(std::size_t v=0;v<2;++v){
   const auto index=4+n*6+v*3;const auto& glyph=glyphsC_[first+n].vertices[v];
   bool alias=false;if(!glyph.aliases10||glyph.aliases10>2)return reject(0x2E1284);
   for(std::size_t a=0;a<glyph.aliases10;++a)if(glyph.rgba1C[a]==q+index&&
      glyph.stq14[a]==q+index+1&&glyph.xyz24[a]==q+index+2)alias=true;
   if(!alias)return reject(0x2E1284);
   const auto& rgba=q[index].words;const auto& xyz=q[index+1].words;const auto& uv=q[index+2].words;
   for(std::size_t c=0;c<4;++c){if(rgba[c]>(c==3?128u:255u))return reject(0x328420);
      endpoints[v].color[c]=static_cast<std::uint8_t>(rgba[c]);}
   for(std::size_t c=0;c<2;++c){
    const float coordinate=std::bit_cast<float>(xyz[c]);
    volatile float sum=coordinate+2048.f;volatile float half=(c?height:width)*.5f;
    volatile float gs=sum-half;volatile float scaled=gs*16.f;
    const float integer=std::trunc(scaled);
    if(!std::isfinite(integer)||integer<0||integer>65535)return reject(0x3283F8);
    endpoints[v].position[c]=(integer-static_cast<float>(offset[c]))/16.f;
   }
   const float z=std::trunc(std::bit_cast<float>(xyz[2]));
   if(!std::isfinite(z)||z<0||z>16777215.f||xyz[3]!=0x3f800000u||uv[2]!=0x3f800000u||uv[3])return reject(0x328408);
   endpoints[v].position[2]=z;
   for(std::size_t c=0;c<3;++c){texture[v][c]=std::bit_cast<float>(uv[c]);if(!std::isfinite(texture[v][c]))return reject(0x328430);}
  }
  if(endpoints[0].position[2]!=endpoints[1].position[2]||!std::equal(endpoints[0].color,endpoints[0].color+4,endpoints[1].color))
   return reject(0x328420);
  // This actual GIFtag is PRIM6 (sprite), not the embedded Quad's PRIM4
  // strip. Expand its two retained opposite corners into an affine rectangle;
  // no third/fourth source corner or source triangle diagonal is discarded.
  constexpr std::array<std::array<std::size_t,2>,4> corners{{{{0,0}},{{1,0}},{{1,1}},{{0,1}}}};
  for(const auto& corner:corners){auto vertex=endpoints[1];
   vertex.position[0]=endpoints[corner[0]].position[0];vertex.position[1]=endpoints[corner[1]].position[1];
   std::array<float,3> uv{{texture[corner[0]][0],texture[corner[1]][1],1.f}};
   vertex.uv[0]=uv[0];vertex.uv[1]=uv[1];next.vertices.push_back(vertex);next.stq.push_back(uv);
   next.rasterW.push_back(1.f);next.fog.push_back(0.f);
  }
 }
 next.quadCount=static_cast<std::uint32_t>(count);out=std::move(next);d.sourcePC=0x328448;return true;
}
}
