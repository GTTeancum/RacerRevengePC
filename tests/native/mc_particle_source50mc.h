#pragma once
#include "mc_particle_authoring50mc.h"
#include "../../src/game/original_gc_spray50kg.h"
#include "../../src/game/water_wake50jc.h"
#include <algorithm>
#include <cfenv>
#include <cstring>
#include <limits>
namespace mc_particle_source50MC {
namespace a=rr::assets;namespace g=rr::game;
// Capacities, not stock counts: descriptorCount = the common emitter reader's
// store (live count = the TOB's table); definitionCount = distinct EMT files.
// atlasFrameCount stays the stock demoprt atlas store (PORT STAND-IN, see decode).
inline constexpr std::size_t descriptorCount=mc_particle_authoring50MC::count,definitionCount=7,attachmentCount=descriptorCount,atlasFrameCount=152;
using Kind=mc_particle_authoring50MC::Kind;
struct MemberPin {const char* name;std::size_t bytes;const char* sha256;};
// Stock member identities: receipts only (course_member_identity lines).
inline constexpr std::array<MemberPin,10> members{{
 {"particle\\mcFire.emt",793,"4c35ea94e3d7586c0989aebb0cd1e3aac309818f96ed6cfeb008bea4f8253386"},
 {"particle\\ma_water01.emt",1208,"10f3c94ca97b599bbc2349813985774555d60b42ad475ae537201f3f96c1ce7c"},
 {"particle\\ma_water02.emt",1209,"f607ae7f9b2989ca211e8cef4eb78c77e3240f9adc8c274bf72bd6cab0ec39ba"},
 {"particle\\ma_water03.emt",1210,"3abeda5ee2e92b9810aa7cd8a01f368563462242dfead690e8c630c74e917284"},
 {"particle\\ma_water04.emt",1209,"335ccbb940af360431f5e56b1da43ce0a6c3379a991bd9d6cbbfe5e433f6760b"},
 {"particle\\ma_water05.emt",1207,"0d66663f324900f5afd4e2bf4fc94f3818580513f67d68f723d46dfd9fc9d160"},
 {"particle\\ma_water06.emt",1206,"bc8dbabf1a108bfd61ed736e9fdd8ee36e25ecfdd8aefcaa5c64366d8c582722"},
 {"particle\\demoprt.prt",10770,"a92a82d85adea560886fc0e6911baede2f11e9448b9ac4365bf7037e9e809020"},
 {"particle\\demoprt.psm",5022,"ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1"},
 {"particle\\demoprt.pst",132124,"027296fc1e953a6a936482bf1f51a6ad91e653bf5c322225e73396a259ed2361"},
}};
struct Definition {
 Kind kind=Kind::Fire8;g::OriginalGcCommonEmitterTiming50KG timing{};
 std::array<float,3> velocity60{},minRange{},maxRange{};
 std::int32_t particleGroup1B8=-1;float particleLife17C=1000,growth180=0,initialSize184=1,emitRate188=500,ageAtBirth1A0=0;
 float gravityMultiplier=3,groundLevel=std::numeric_limits<float>::max(),restitution=.55f;
 float framesPerSet=-1,inverseFramesPerSet=-1;std::uint32_t particleFlags198=0;
 bool randomMirrorX=false,randomMirrorY=false,applySpreadToVelocity=true,movingEmitter200=true,parsed=false;
};
struct Attachment {std::size_t descriptor=0,worldIndex=0;const a::WorldInstance48F* worldInstance=nullptr;};
struct Atlas {std::array<g::OriginalGcParticleGroup50KG,10> groups{};
 std::array<g::OriginalGcParticleFrame50KG,atlasFrameCount> frames{};std::string textureMap;float glowScalar148=0;};
struct Diagnostic {std::string message;std::size_t member=0,retainedPayloadBytes=0,peakReservedPayloadBytes=0;
 a::DecodeError48C decode{};a::OriginalUiPageDiagnostic50KG text{};};
namespace detail {
using D=a::OriginalUiPageDiagnostic50KG;using E=a::OriginalUiPageError50KG;
using Section=a::OriginalUiDatSection50KG;
using ga_trigger_authoring::detail::need;
using ga_trigger_authoring::detail::block;
using ga_trigger_authoring::detail::optional;
using ga_trigger_authoring::detail::field;
using ga_trigger_authoring::detail::integer;
using ga_trigger_authoring::detail::number;
using ga_trigger_authoring::detail::vector;
inline char lower(char c) noexcept{return c>='A'&&c<='Z'?char(c+32):c;}
inline bool same(const std::string& a,const char* b) noexcept {
    const auto n=std::strlen(b);if(a.size()!=n)return false;
    for(std::size_t i=0;i<n;++i)if(lower(a[i])!=lower(b[i]))return false;return true;
}
inline bool space(char c) noexcept{return c==' '||c=='\t'||c=='\r';}
// Bounded source159440 cleaning and158648 first-present lookup. Unknown keys
// and duplicates remain in source order; quoted empty differs from missing.
// This is text authoring only, not a BPB/archive decoder or runtime interpreter.
inline bool sections(const std::vector<std::uint8_t>& bytes,std::vector<Section>& out,D& d) {
    auto fail=[&](E e,std::size_t at){d={e,at,0x159440u};return false;};
    if(bytes.empty()||bytes.size()>32768)return fail(E::capacity,0);
    auto size=bytes.size();if(bytes.back()==0)--size;
    for(std::size_t i=0;i<size;++i)if((bytes[i]<32&&bytes[i]!=9&&bytes[i]!=10&&bytes[i]!=13)||bytes[i]>126)
        return fail(E::unsupportedEncoding,i);
    for(std::size_t at=0;at<size;){
        const auto start=at;while(at<size&&bytes[at]!='\n')++at;auto end=at;
        if(at<size){++at;if(end>start&&bytes[end-1]=='\r')--end;}
        if(end-start>2048)return fail(E::capacity,start);
        auto first=start;while(first<end&&space(char(bytes[first])))++first;
        if(first==end||bytes[first]==';'||bytes[first]==':')continue;
        // Colon-prefixed EMT annotations cannot match any consumed source key.
        //158724/158800 skips a line without an '=' value; colon key/value
        // annotations also fail the requested-key comparison at15880C.
        const bool header=bytes[first]=='[';bool quoted=false;int brackets=0;
        std::string line;std::vector<std::size_t> offsets;
        for(auto i=start;i<end;++i){const char c=char(bytes[i]);
            if(header&&c==';'&&!quoted&&brackets==0&&!line.empty()&&line.back()==']')break;
            if(c=='"')quoted=!quoted;else if(c=='[')++brackets;else if(c==']')--brackets;
            if(brackets<0||brackets>16)return fail(E::textSyntax,i);
            if(space(c)&&!quoted&&brackets==0)continue;
            line.push_back(c);offsets.push_back(i);
        }
        if(line.empty()||line[0]==';')continue;
        if(brackets)return fail(E::textSyntax,start);
        if(line[0]=='['){
            if(line.size()<3||line.back()!=']')return fail(E::textSyntax,start);
            auto name=line.substr(1,line.size()-2);
            if(name.size()>127||name.find_first_of("[]=\"")!=std::string::npos)return fail(E::textSyntax,start);
            for(const auto& s:out)if(same(s.name,name.c_str()))return fail(E::duplicateSection,start);
            if(out.size()==64)return fail(E::capacity,start);
            out.push_back({std::move(name),offsets[1],{}});continue;
        }
        const auto equal=line.find('=');
        if(equal==std::string::npos)continue; //158800: no value token.
        if(out.empty())continue; //No current requested source block yet.
        if(!equal||equal>127)return fail(E::textSyntax,start);
        auto key=line.substr(0,equal);if(key.find_first_of("[];\"")!=std::string::npos)return fail(E::textSyntax,start);
        auto va=equal+1,vb=line.size();const bool present=va<vb;
        if(va<vb&&line[va]=='"')++va;if(va<vb&&line[vb-1]=='"')--vb;
        if(out.back().values.size()==256)return fail(E::capacity,start);
        const auto valueOffset=va<offsets.size()?offsets[va]:offsets[equal]+1;
        out.back().values.push_back({std::move(key),line.substr(va,vb-va),offsets[0],valueOffset,present});
    }
    return !out.empty()||fail(E::textSyntax,0);
}
inline const Section& section(const std::vector<Section>& doc,const char* name,const Section& absent) noexcept {
    for(const auto& s:doc)if(same(s.name,name))return s;return absent;
}

inline float bits(std::uint32_t word)noexcept {float f;std::memcpy(&f,&word,4);return f;}
inline bool definition(const std::vector<std::uint8_t>& bytes,Kind kind,Definition& out,D& d){
 if(std::fegetround()!=FE_TONEAREST)return false;
 std::vector<Section> doc;if(!sections(bytes,doc,d))return false;
 const Section absent{};const auto& general=section(doc,"General Info",absent);
 const auto& properties=section(doc,"Properties",absent);const auto& animation=section(doc,"Animation",absent);
 Definition f;f.kind=kind;g::water_wake_detail50jc::Math m;
 auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){return a::originalUiNumber50KG(s,key,fallback,value,d,pc);};
 auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){return a::originalUiInteger50KG(s,key,fallback,value,d,pc);};
 auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){return a::originalUiTriple50KG(general,key,{},value,d,pc);};
 if(kind==Kind::Fire8){
  //15E6D0 reads the two derived ranges and spread flag BEFORE15AEA0.
  if(!v("MinRange",f.minRange,0x15e710)||!v("MaxRange",f.maxRange,0x15e72c)||
     !a::originalUiBoolean50KG(general,"ApplySpreadToVelocity",true,f.applySpreadToVelocity,d,0x15e744)||!f.applySpreadToVelocity)return false;
 }
 if(kind==Kind::Sparks2){
  //15DD00 reads MovingEmitter default1 and both ranges before15AEA0.
  if(!a::originalUiBoolean50KG(general,"MovingEmitter",true,f.movingEmitter200,d,0x15dd3c)||
     !v("MinRange",f.minRange,0x15dd58)||!v("MaxRange",f.maxRange,0x15dd74))return false;
 }
 if(!i(general,"ParticleGroup",-1,f.particleGroup1B8,0x15aee0)||
    !n(general,"ParticleLife",1000,f.particleLife17C,0x15af70)||
    !n(general,"InitialSize",1,f.initialSize184,0x15af8c)||
    !n(general,"Growth",0,f.growth180,0x15afa4)||
    !n(general,"EmitRate",500,f.emitRate188,0x15afc0)||
    !n(general,"AgeAtBirth",0,f.ageAtBirth1A0,0x15afd8)||
    !v("Velocity",f.velocity60,0x15b004)||
    !n(general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,0x15b064)||
    !n(general,"GravityMultiplier",3,f.gravityMultiplier,0x15b080)||
    !n(general,"Restitution",bits(0x3f0ccccd),f.restitution,0x15b0a0))return false;
 if(g::readOriginalGcCommonEmitterTiming50KG(bytes,f.timing,d)!=g::OriginalGcSprayStatus50KG::complete)return false;
 f.ageAtBirth1A0=m.mul(f.ageAtBirth1A0,f.particleLife17C,0x15aff8);
 const char* keys[]={"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
 const std::uint32_t masks[]={2,4,8,0x1000,0x2000,1,0x20,0x800};
 const std::uint32_t pcs[]={0x15b1e8,0x15b200,0x15b218,0x15b230,0x15b248,0x15b260,0x15b278,0x15b290};
 for(unsigned k=0;k<8;++k){bool value=k==7;if(!a::originalUiBoolean50KG(properties,keys[k],k==7,value,d,pcs[k]))return false;
  if(k==7)value=!value;if(value)f.particleFlags198|=masks[k];}
 if(!a::originalUiBoolean50KG(properties,"RandomMirrorAboutX",false,f.randomMirrorX,d,0x15b2a8)||
    !a::originalUiBoolean50KG(properties,"RandomMirrorAboutY",false,f.randomMirrorY,d,0x15b2c0)||
    !n(animation,"FramesPerSet",-1,f.framesPerSet,0x15b490))return false;
 if(f.groundLevel!=std::numeric_limits<float>::max())f.particleFlags198|=0x200u;
 // Existing native policy for unused1/0: actual0 has animation flag400
 // clear; no active numerical consumer reads the stored reciprocal.
 f.inverseFramesPerSet=f.framesPerSet==0?0:m.div(1,f.framesPerSet,0x15b4b0);
 if(f.framesPerSet>1)f.particleFlags198|=0x400u;
 std::int32_t forced=0;if(!i(animation,"NumberOfForcedSets",0,forced,0x15b4c0)||forced!=0)return false;
 if(m.badPC){d.sourcePC=m.badPC;return false;}
 f.parsed=true;out=f;return true;
}
} // detail

class Owner final {
 std::shared_ptr<const mc_cold_sources50LA::Owner> source_;mc_particle_authoring50MC::Owner authoring_;
 std::array<Definition,definitionCount> definitions_{};std::array<std::string,definitionCount> definitionNames_{};std::size_t definitionLive_=0;
 std::array<std::uint8_t,descriptorCount> definitionOf_{};
 std::array<Attachment,attachmentCount> attachments_{};std::size_t live_=0,attachedLive_=0;
 // EMT bytes per definition, then the three atlas members (PRT, PSM, PST).
 Atlas atlas_{};std::array<std::vector<std::uint8_t>,definitionCount+3> bytes_{};
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
 bool fail(Diagnostic& d,const char* text){d.message=text;d.retainedPayloadBytes=retainedPayloadBytes();return false;}
 static const MemberPin* stockPin(const std::string& name)noexcept{for(const auto& p:members)if(detail::same(name,p.name))return &p;return nullptr;}
 // Member by name; the stock pin (if this name has one) is a receipt.
 static bool member(const mc_cold_sources50LA::Owner& s,const std::string& name,std::size_t cap,std::vector<std::uint8_t>& out,Diagnostic& d){
  if(!s.extractMember(name,out,d.decode,cap)||out.empty())return false;
  const auto* pin=stockPin(name);
  course_archive_receipt50CI::memberReceipt("particles_MC",name,out,0,0,pin?pin->bytes:0,pin?pin->sha256:nullptr);return true;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 // Per-member extraction caps (the reservation is charged per slot, inside
 // the 64 MB console budget): EMT 4 KiB each, PRT 16 KiB, PSM 8 KiB, PST
 // 136 KiB (stock: <=1210, 10770, 5022, 132124 bytes). A larger member is
 // refused by capacity, never by identity.
 static constexpr std::size_t emtCap=4096;
 static constexpr std::array<std::size_t,3> atlasCap{{16384,8192,139264}};
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept{
  const std::size_t raw=definitionCount*emtCap+atlasCap[0]+atlasCap[1]+atlasCap[2];
  return sizeof(Owner)+mc_particle_authoring50MC::Owner::maximumAdditionalPayloadBytes()+8*raw+65536+
    3*256*sizeof(a::OriginalUiDatSection50KG)+3*1024*sizeof(a::OriginalUiDatValue50KG)+16*10770;
 }
 std::size_t retainedPayloadBytes()const noexcept{
  if(thread_!=std::this_thread::get_id())return 0;std::size_t n=sizeof(Owner)+atlas_.textureMap.capacity()+1;
  for(const auto& v:bytes_)n+=v.capacity();for(const auto& s:definitionNames_)n+=s.capacity()+1;return n;
 }
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::size_t available,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id())return fail(d,"MC particle source thread mismatch");
  if(attempted_)return fail(d,"MC particle source needs fresh lifetime");attempted_=true;
  d.peakReservedPayloadBytes=maximumAdditionalPayloadBytes();
  if(available>32u*1024u*1024u||available<d.peakReservedPayloadBytes)return fail(d,"MC particle source exceeds remaining allowance");
  try{using namespace detail;need(cold&&cold->ready(),"MC actual cold particle source");std::string why;
   need(authoring_.decode(cold,authoring_.maximumAdditionalPayloadBytes(),why),why.c_str());source_=std::move(cold);
   live_=authoring_.liveCount();
   const auto& world=source_->world();
   //27CC4C..27CFBC traverses actual model groups, stopping at the declared
   //match count (1 per MC descriptor): first placed model whose SLT names the
   //descriptor's AttachToSoultree.
   for(std::size_t row=0;row<live_;++row){const auto& descriptor=authoring_.source(row);if(!descriptor.attached)continue;
    std::size_t found=world.instances.size();
    for(std::size_t i=0;i<world.instances.size();++i){const auto& instance=world.instances[i];need(instance.parameter_block<world.parameters.blocks.size(),"MC attachment block bound");
     const auto& b=world.parameters.blocks[instance.parameter_block];const auto* slt=optional(b,"slt");
     if(slt&&same(*slt,descriptor.attachToSoulTree->c_str())){found=i;break;}}
    need(found<world.instances.size(),"MC attached emitter names no placed SoulTree model");
    const auto& instance=world.instances[found];
    need(instance.source_block_offset==world.parameters.blocks[instance.parameter_block].offset,"MC actual immutable attachment identity");
    // Old pins (rows 0..1 -> world 282 zone13-model8, rows 2..7 -> 441 zone27-model47) are receipts.
    if(live_==mc_particle_authoring50MC::stockCount&&found!=(row<2?282u:441u))
     std::cerr<<"course_count site=particles_MC attachment="<<row<<" world="<<found<<" stock="<<(row<2?282:441)<<"\n";
    attachments_[row]={row,found,&instance};++attachedLive_;
   }
   // One definition per distinct EmitterFilename, in first-use order; kind from
   // the descriptor's TypeID (a file used with two types is refused).
   for(std::size_t row=0;row<live_;++row){const auto& descriptor=authoring_.source(row);const auto name="particle\\"+*descriptor.filename;
    std::size_t k=0;while(k<definitionLive_&&!same(definitionNames_[k],name.c_str()))++k;
    if(k==definitionLive_){need(k<definitionCount,"MC distinct emitter definitions exceed the definition store");
     d.member=k;need(member(*source_,name,emtCap,bytes_[k],d),"MC emitter definition member missing");
     need(detail::definition(bytes_[k],descriptor.kind,definitions_[k],d.text),"MC original subtype/common EMT decoder");
     definitionNames_[k]=name;++definitionLive_;}
    need(definitions_[k].kind==descriptor.kind,"MC one EMT file authored with two emitter types");
    definitionOf_[row]=static_cast<std::uint8_t>(k);
   }
   if(live_==mc_particle_authoring50MC::stockCount)course_archive_receipt50CI::countReceipt("particles_MC","definitions",definitionLive_,7);
   const char* atlasNames[3]={"particle\\demoprt.prt","particle\\demoprt.psm","particle\\demoprt.pst"};
   for(std::size_t j=0;j<3;++j){d.member=definitionCount+j;need(member(*source_,atlasNames[j],atlasCap[j],bytes_[definitionCount+j],d),"MC particle atlas member missing");}
   const auto& prt=bytes_[definitionCount];
   // The atlas text reader verifies the bytes against the identity it is
   // given; it is given the actual bytes' own identity (the stock pin is the
   // receipt above), so a converted atlas is read, not refused by checksum.
   std::vector<Section> doc;need(g::readOriginalGcParticleText50KG(prt,prt.size(),pod_materials50ed_fixture::detail::sha256(prt).c_str(),doc),"MC actual atlas original text");
   const Section absent{};const auto& general=section(doc,"General Info",absent);std::int32_t frames=0,groups=0;
   // PORT STAND-IN (flagged): the atlas store is the stock demoprt extent
   // (152 frames, 10 groups); a different atlas is refused here.
   need(a::originalUiInteger50KG(general,"NumFrames",61,frames,d.text,0x161828)&&frames==152&&
    a::originalUiInteger50KG(general,"NumGroups",0,groups,d.text,0x161854)&&groups==10&&
    a::originalUiNumber50KG(general,"GlowScalar",255,atlas_.glowScalar148,d.text,0x1618a8)&&atlas_.glowScalar148==128,"MC exact atlas extents");
   const auto* texture=general.find("TextureMap");need(texture&&texture->text=="demoprt.tga","MC atlas original texture");atlas_.textureMap=texture->text;
   for(std::size_t i=0;i<atlas_.groups.size();++i){auto& group=atlas_.groups[i];const auto& b=section(doc,("Group"+std::to_string(i)).c_str(),absent);
    need(a::originalUiInteger50KG(b,"TypeID",-1,group.type,d.text,0x1618d0)&&a::originalUiInteger50KG(b,"FirstFrame",0,group.first,d.text,0x1618d0)&&
     a::originalUiInteger50KG(b,"LastFrame",0,group.last,d.text,0x1618d0)&&group.first>=0&&group.last>=group.first&&(group.last<152||(i==9&&group.type==8&&group.first==129&&group.last==152)),"MC actual atlas group");}
   for(std::size_t i=0;i<atlasFrameCount;++i){const auto& b=section(doc,("Frame"+std::to_string(i)).c_str(),absent);float l=0,r=0,t=0,bot=0;
    need(a::originalUiNumber50KG(b,"UVLeft",0,l,d.text,0x161a0c)&&a::originalUiNumber50KG(b,"UVRight",0,r,d.text,0x161a0c)&&
     a::originalUiNumber50KG(b,"UVTop",0,t,d.text,0x161a0c)&&a::originalUiNumber50KG(b,"UVBottom",0,bot,d.text,0x161a0c),"MC actual atlas UV");
    atlas_.frames[i].sourceUv={{{{l,t}},{{r,t}},{{r,bot}},{{l,bot}}}};}
   // Each definition's particle group must exist with the type its emitter
   // kind draws (Fire8 -> group type 8, Sparks2 -> type 6; checked again at staging).
   for(std::size_t k=0;k<definitionLive_;++k){const auto& f=definitions_[k];
    need(f.particleGroup1B8>=0&&static_cast<std::size_t>(f.particleGroup1B8)<atlas_.groups.size()&&
     atlas_.groups[f.particleGroup1B8].type==(f.kind==Kind::Fire8?8:6),"MC emitter definition particle group/type");
    // PORT STAND-IN (flagged): the native Sparks2 path implements the moving
    // emitter with timing mode 3 only.
    if(f.kind==Kind::Sparks2)need(f.movingEmitter200&&f.timing.mode1D8==3,"PORT STAND-IN: MC native Sparks2 implements MovingEmitter timing mode 3 only");}
   need(retainedPayloadBytes()<=available&&retainedPayloadBytes()<=maximumAdditionalPayloadBytes(),"MC retained particle payload budget");
   ready_=true;d.message.clear();d.retainedPayloadBytes=retainedPayloadBytes();return true;
  }catch(const std::exception& e){return fail(d,e.what());}
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&source_&&source_->ready()&&authoring_.ready();}
 const mc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?source_.get():nullptr;}
 // Live emitter table (the TOB's own count) and its attached subset.
 std::size_t emitterCount()const noexcept{return ready()?live_:0;}
 std::size_t attachedCount()const noexcept{return ready()?attachedLive_:0;}
 std::size_t definitionCountLive()const noexcept{return ready()?definitionLive_:0;}
 const mc_particle_authoring50MC::Source& descriptor(std::size_t i)const{if(!ready()||i>=live_)throw std::out_of_range("MC particle descriptor outside the live table");return authoring_.source(i);}
 const Definition& definition(std::size_t i)const{if(!ready()||i>=definitionLive_)throw std::runtime_error("MC particle resources unavailable");return definitions_[i];}
 std::size_t definitionIndexForDescriptor(std::size_t i)const{if(!ready()||i>=live_)throw std::out_of_range("MC particle descriptor outside the live table");return definitionOf_[i];}
 const Definition& definitionForDescriptor(std::size_t i)const{return definition(definitionIndexForDescriptor(i));}
 const Attachment& attachment(std::size_t i)const{if(!ready()||i>=live_||!authoring_.source(i).attached)throw std::runtime_error("MC particle attachments unavailable");return attachments_[i];}
 const Atlas& atlas()const{if(!ready())throw std::runtime_error("MC atlas unavailable");return atlas_;}
 const std::vector<std::uint8_t>& originalMember(std::size_t i)const{if(!ready())throw std::runtime_error("MC particle bytes unavailable");return bytes_.at(i);}
};
}
