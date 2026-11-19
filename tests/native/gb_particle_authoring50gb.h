#pragma once
#include "gb_cold_sources50la.h"
#include "course_archive_receipt50ci.h"
#include "../../src/assets/original_particle_emitters50pe.h"
#include "ga_trigger_authoring_fixture.h"
#include "../../src/game/original_gc_spray50kg.h"
#include "../../src/game/water_wake50jc.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <cstring>
#include <limits>
#include <memory>
#include <optional>
#include <thread>

namespace gb_particle_authoring50GB {
namespace a=rr::assets;namespace g=rr::game;
// descriptorCount: the common emitter reader's store (live count = the TOB's
// table; stock GB 12 is a receipt). definitionCount / attachmentCount /
// distinctPlacementCount: the native GB emitter chain's stores (PORT STAND-IN,
// flagged: gb_particle_attachments/runtime/render/type11 implement the stock
// four-definition, 67-instance, 55-placement layout; another layout is refused
// below by name, not ignored).
inline constexpr std::size_t descriptorCount=rr::assets::particle_emitters50PE::capacity,stockDescriptorCount=12;
inline constexpr std::size_t definitionCount=4,attachmentCount=67,distinctPlacementCount=55;
enum class Kind : std::uint8_t {Fire8=8,Sparks11=11};
struct MemberPin {const char* name;std::size_t bytes;const char* sha256;};
// Stock member identities: receipts only (course_member_identity lines).
inline constexpr std::array<MemberPin,7> members{{
 {"particle\\gbtemplefire.emt",795,"5e3e47cbea41a53a1ed40c5863b0edc7167f8f6f5364887517065a9240953d88"},
 {"particle\\genratorfire.emt",792,"9964592e987a2a550e87b81eb278b4222752ad398372e3e9623c49dd6ae681ec"},
 {"particle\\gbfireworks.emt",1573,"edb56ea7b2e6a0d95d426ef4fcbee505f6f61293c3d43b741f0b1a92f3630fa6"},
 {"particle\\gbredfireworks.emt",1576,"0c5e7a07d1ab9e24940f90b8592f372b3dac749183bb43be500f767c95c1c75c"},
 {"particle\\demoprt.prt",9385,"fc95eee7487a6ae0c00e50228cf3667f9978f74770fe913fe95310a35c59f15c"},
 {"particle\\demoprt.psm",5022,"ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1"},
 {"particle\\demoprt.pst",132124,"e00d1321525795ed19676514f733ea3b4027925f2b1367afd45b189c4bee2642"}
}};
struct Descriptor {
 std::uint32_t ordinal=0,startingZone=0,instances=0,sourceBlock=0;
 std::size_t definitionIndex=0,firstAttachment=0;
 Kind kind=Kind::Fire8;std::string attachToSoulTree,childObject,filename;
 std::array<float,4> offset{{0,0,0,1}};
 std::optional<std::int32_t> authoredStartOn;std::int32_t effectiveStartOn=0;
 const a::ParameterBlock48F* original=nullptr;
};
struct Attachment {
 std::size_t descriptor=0,instanceOrdinal=0,worldIndex=0;
 const a::WorldInstance48F* worldInstance=nullptr;
};
// Four actual source definitions, not one definition per subtype. These cold
// scalars grant no canonical POD identity, live graph or counted child owner.
struct Definition {
 Kind kind=Kind::Fire8;g::OriginalGcCommonEmitterTiming50KG timing{};
 std::array<float,3> velocity60{},minRange{},maxRange{};
 std::int32_t particleGroup1B8=-1;
 float particleLife17C=1000,growth180=0,initialSize184=1,emitRate188=500,ageAtBirth1A0=0;
 float gravityMultiplier=3,groundLevel=std::numeric_limits<float>::max(),restitution=.55f;
 float framesPerSet=-1,inverseFramesPerSet=-1;std::uint32_t particleFlags198=0;
 bool randomMirrorX=false,randomMirrorY=false,applySpreadToVelocity=true;
 float minStreamLength200=3,maxStreamLength204=5,particleSeparation208=1.5f,tailLife20C=.6f;
 float tailThickness210=1,headThickness214=2;bool superbrightHead218=true,movingEmitter21C=false;
 bool parsed=false;
};
struct Atlas {
 std::array<g::OriginalGcParticleGroup50KG,10> groups{};
 std::array<g::OriginalGcParticleFrame50KG,130> frames{};
 std::string textureMap;float glowScalar148=0;
};
enum class Status {Empty,Input,Capacity,Source,Authoring,Arithmetic,Ready};
struct Diagnostic {
 Status status=Status::Empty;std::string message;std::size_t ordinal=0,retainedPayloadBytes=0,peakReservedPayloadBytes=0;
 a::DecodeError48C decode{};a::OriginalUiPageDiagnostic50KG text{};
};
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
inline bool definition(const std::vector<std::uint8_t>& bytes,std::size_t index,Definition& out,D& d){
 if(std::fegetround()!=FE_TONEAREST)return false;
 std::vector<Section> doc;if(!sections(bytes,doc,d))return false;
 const Section absent{};const auto& general=section(doc,"General Info",absent);
 const auto& properties=section(doc,"Properties",absent);const auto& animation=section(doc,"Animation",absent);
 if(index>=definitionCount)return false;const auto kind=index<2?Kind::Fire8:Kind::Sparks11;
 Definition f;f.kind=kind;g::water_wake_detail50jc::Math m;
 auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){return a::originalUiNumber50KG(s,key,fallback,value,d,pc);};
 auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){return a::originalUiInteger50KG(s,key,fallback,value,d,pc);};
 auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){return a::originalUiTriple50KG(general,key,{},value,d,pc);};
 if(kind==Kind::Fire8){
  //15E6D0 reads the two derived ranges and spread flag BEFORE15AEA0.
  if(!v("MinRange",f.minRange,0x15e710)||!v("MaxRange",f.maxRange,0x15e72c)||
     !a::originalUiBoolean50KG(general,"ApplySpreadToVelocity",true,f.applySpreadToVelocity,d,0x15e744)||!f.applySpreadToVelocity)return false;
 }
 if(kind==Kind::Sparks11){
  if(!n(general,"MinStreamLength",3,f.minStreamLength200,0x15fc50)||
     !n(general,"MaxStreamLength",5,f.maxStreamLength204,0x15fc6c)||
     !n(general,"TailThickness",1,f.tailThickness210,0x15fc88)||
     !n(general,"HeadThickness",2,f.headThickness214,0x15fca4)||
     !n(general,"ParticleSeparationFactor",1.5f,f.particleSeparation208,0x15fcc0)||
     !n(general,"TailLifeModifier",bits(0x3f19999a),f.tailLife20C,0x15fce0)||
     !a::originalUiBoolean50KG(general,"SuperbrightHead",true,f.superbrightHead218,d,0x15fcf8)||
     !v("MinRange",f.minRange,0x15fd14)||!v("MaxRange",f.maxRange,0x15fd30)||
     !a::originalUiBoolean50KG(general,"MovingEmitter",false,f.movingEmitter21C,d,0x15fd48))return false;
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
 if(!f.randomMirrorX||!f.randomMirrorY||f.ageAtBirth1A0||f.timing.minTimeOn1E8||
    f.timing.minTimeOff1F0||f.timing.maxTimeOff1F4)return false;
 if(kind==Kind::Fire8){
  if(f.particleGroup1B8!=4||f.particleLife17C!=650||f.initialSize184!=(index==0?25.f:12.f)||
     f.growth180!=(index==0?-6.5f:-4.f)||f.emitRate188!=(index==0?100.f:50.f)||f.framesPerSet!=8||
     f.particleFlags198!=(index==0?0x1405u:0x3405u)||f.timing.mode1D8||f.timing.delayTime1E0||f.timing.maxTimeOn1EC||
     f.velocity60!=std::array<float,3>{{0,index==0?120.f:70.f,0}})return false;
 }else if(f.particleGroup1B8!=int(index==2?8:9)||f.particleLife17C!=2425||f.initialSize184!=5.555f||
   f.growth180!=-.5f||f.emitRate188!=(index==2?200.f:350.f)||f.framesPerSet!=1||f.particleFlags198!=0x3027u||
   f.timing.mode1D8!=1||f.timing.delayTime1E0!=(index==2?.5f:0.f)||f.timing.maxTimeOn1EC!=.3f||
   f.velocity60!=std::array<float,3>{{0,index==2?200.f:170.f,0}}||f.gravityMultiplier!=8.5f||
   f.minStreamLength200!=(index==2?7.f:5.f)||f.maxStreamLength204!=(index==2?10.f:8.f)||
   f.tailThickness210!=1||f.headThickness214!=2||f.particleSeparation208!=2||f.tailLife20C!=.65f||
   !f.superbrightHead218||f.movingEmitter21C)return false;
 f.parsed=true;out=f;return true;
}
} // detail

class Owner final {
 std::shared_ptr<const gb_cold_sources50LA::Owner> source_;
 std::array<Descriptor,descriptorCount> descriptors_{};std::size_t live_=0;
 std::array<Attachment,attachmentCount> attachments_{};
 std::array<Definition,definitionCount> definitions_{};Atlas atlas_{};
 std::array<std::vector<std::uint8_t>,5> bytes_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false;
 static void add(std::size_t& n,std::size_t x)noexcept {n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;}
 bool fail(Diagnostic& d,Status status,const char* message)noexcept {d.status=status;try{d.message=message;}catch(...){}d.retainedPayloadBytes=retainedPayloadBytes();return false;}
 bool reserve(std::size_t limit,std::size_t extra,Diagnostic& d)const noexcept {
  auto n=retainedPayloadBytes();add(n,extra);d.peakReservedPayloadBytes=std::max(d.peakReservedPayloadBytes,n);return n<=limit;
 }
 void requireReady()const {if(!ready())throw std::runtime_error("GB particle source is not ready");}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 std::size_t retainedPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return 0;std::size_t n=sizeof(Owner);
  for(const auto& b:bytes_)add(n,b.capacity());
  for(const auto& s:descriptors_){add(n,s.attachToSoulTree.capacity()+1);add(n,s.childObject.capacity()+1);add(n,s.filename.capacity()+1);}
  add(n,atlas_.textureMap.capacity()+1);return n;
 }
 bool decode(std::shared_ptr<const gb_cold_sources50LA::Owner> source,std::size_t availableAdditionalCpuPayloadBytes,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id())return fail(d,Status::Input,"GB particle source thread mismatch");
  if(attempted_)return fail(d,Status::Input,"GB particle source is one-attempt");attempted_=true;
  try{using namespace detail;
   if(!source||!source->ready())return fail(d,Status::Input,"GB particle source requires actual cold owner");
   if(availableAdditionalCpuPayloadBytes>32u*1024u*1024u)
    return fail(d,Status::Capacity,"GB particle source cannot enlarge existing source allowance");
   source_=std::move(source);const auto& p=source_->triggerParameters();const auto& w=source_->world();
   need(!p.preservesDuplicateLabels&&p.original_bytes==source_->originalTob(),"GB full source document shape");
   // Stock shape / TOB identity: receipts only.
   course_archive_receipt50CI::countReceipt("particles_GB","tob_blocks",p.blocks.size(),70);
   course_archive_receipt50CI::countReceipt("particles_GB","world_instances",w.instances.size(),975);
   course_archive_receipt50CI::memberReceipt("particles_GB","tracks\\gb.tob",source_->originalTob(),0,0,20617,
    "96469b3953b18d940bd0c8772758c68a75a69c3375b49ef98b966901c2e598d1");
   if(!reserve(availableAdditionalCpuPayloadBytes,4*(source_->originalTob().size()+64u)+32768u,d))return fail(d,Status::Capacity,"GB source/descriptor scratch exceeds remaining budget");
   // The descriptor table is read by the ONE common reader (27DE00/27C600/
   // 27C7B0, src\assets\original_particle_emitters50pe.h).
   namespace pe=rr::assets::particle_emitters50PE;pe::Table table;
   need(pe::read(p,"GB",table),"GB emitter table rejected by the common reader (see particle_emitters line)");
   need(table.live<=descriptors_.size(),"GB emitter table exceeds the descriptor store");
   course_archive_receipt50CI::countReceipt("particles_GB","emitters",table.live,stockDescriptorCount);
   // Old per-index pins, now a stock-layout receipt only.
   constexpr const char* attach[]={"gam_temple.slt","stone_head.slt","stone_head.slt","pig_head.slt","pig_head.slt","gam_temple.slt","shade.slt","startgate.slt","startgate.slt","startgate.slt","startgate.slt","startgate.slt"};
   constexpr const char* names[]={"gbtemplefire.emt","gbtemplefire.emt","gbtemplefire.emt","gbtemplefire.emt","gbtemplefire.emt","gbtemplefire.emt","genratorfire.emt","gbfireworks.emt","gbfireworks.emt","gbredfireworks.emt","gbredfireworks.emt","gbredfireworks.emt"};
   constexpr std::uint32_t counts[]={1,4,4,3,3,1,46,1,1,1,1,1};
   constexpr std::array<float,4> offsets[]={{{-271.f,190.f,-150.f,1.f}},{{69.10f,36.2f,-76.4f,1.f}},{{-69.10f,36.2f,-76.4f,1.f}},{{139.9f,72.5f,-152.9f,1.f}},{{-139.9f,72.5f,-152.9f,1.f}},{{260.f,190.f,-150.f,1.f}},{{-4.3f,46.0f,0.f,1.f}},{{-154.8f,111.6f,36.5f,1.f}},{{154.8f,111.6f,36.5f,1.f}},{{234.5f,248.1f,126.3f,1.f}},{{-234.5f,248.1f,126.3f,1.f}},{{-3.8f,207.3f,181.8f,1.f}}};
   constexpr std::size_t placedIndices[]={712,437,438,494,495,437,438,494,495,575,608,744,575,608,744,712,56,57,58,59,184,185,186,187,188,189,190,191,192,193,194,195,196,197,282,283,432,433,434,435,436,491,492,493,576,577,578,579,685,686,745,746,898,899,961,962,963,964,965,966,967,968,0,0,0,0,0};
   bool stockLayout=table.live==stockDescriptorCount;
   // Definitions: one per distinct EmitterFilename, in first-use order (the
   // native chain holds them by index; PORT STAND-IN, flagged: it implements
   // Fire8, Fire8, Firework11, Firework11 in that order).
   std::array<std::string,definitionCount> definitionNames{};std::size_t definitionsUsed=0;
   std::size_t total=0;
   for(std::size_t row=0;row<table.live;++row){d.ordinal=row+1;const auto& e=table.rows[row];auto& s=descriptors_[row];
    s.original=e.block;s.ordinal=e.ordinal;s.sourceBlock=static_cast<std::uint32_t>(e.block-p.blocks.data());s.firstAttachment=total;
    // PORT STAND-IN (flagged): the GB native emitter chain implements loaded,
    // SoulTree-attached Fire8 / Firework11 emitters without ChildObject.
    need(e.loaded(),"GB emitter the original does not load (no filename/instances/type) is not supported natively");
    need(e.typeId==8||e.typeId==11,"PORT STAND-IN: GB native chain implements emitter TypeID 8 and 11 only");
    need(e.attached,"PORT STAND-IN: GB native chain implements SoulTree-attached emitters only");
    need(e.childObject=="NONE","PORT STAND-IN: GB native chain has no ChildObject route");
    need(e.startingZone>=0&&static_cast<std::size_t>(e.startingZone)<=w.zones.size(),"GB emitter zone outside the world");
    s.kind=static_cast<Kind>(e.typeId);s.startingZone=static_cast<std::uint32_t>(e.startingZone);s.instances=static_cast<std::uint32_t>(e.instances);
    s.attachToSoulTree=e.attachToSoulTree;s.filename=e.filename;s.childObject=e.childObject;
    for(unsigned k=0;k<3;++k)s.offset[k]=e.positionOrOffset[k];s.offset[3]=1;
    if(e.authoredStartOn)s.authoredStartOn=e.startOn?1:0;
    s.effectiveStartOn=e.startOn?1:0; //27C720..72C default1, then 158B30.
    std::size_t def=0;while(def<definitionsUsed&&!same(definitionNames[def],s.filename.c_str()))++def;
    if(def==definitionsUsed){need(definitionsUsed<definitionCount,"PORT STAND-IN: GB native chain holds four EMT definitions only");definitionNames[definitionsUsed++]=s.filename;}
    s.definitionIndex=def;
    need((def<2)==(s.kind==Kind::Fire8),"PORT STAND-IN: GB native chain holds Fire8 definitions 0..1 and Firework11 definitions 2..3");
    if(stockLayout&&!(s.startingZone==1&&s.instances==counts[row]&&s.attachToSoulTree==attach[row]&&
      s.filename==names[row]&&s.offset==offsets[row]&&s.effectiveStartOn==(row<7?1:0)&&
      s.authoredStartOn.has_value()==(row>=6)&&s.kind==(row<7?Kind::Fire8:Kind::Sparks11)))stockLayout=false;
    std::size_t matched=0;
    // Actual27CC4C..27CFBC global model-group traversal. StartingZone does
    // not filter this search. Stop each row at its declared match count.
    for(std::size_t index=0;index<w.instances.size()&&matched<s.instances;++index){const auto& model=w.instances[index];
     need(model.parameter_block<w.parameters.blocks.size(),"GB model source block");const auto& modelBlock=w.parameters.blocks[model.parameter_block];
     const auto* slt=optional(modelBlock,"slt");if(!slt||!same(*slt,s.attachToSoulTree.c_str()))continue;
     need(total<attachmentCount,"PORT STAND-IN: GB emitter instances exceed the native 67-attachment store");
     need(model.source_block_offset==modelBlock.offset,"GB original attachment provenance");
     if(stockLayout&&index!=placedIndices[total])stockLayout=false;
     attachments_[total++]={row,matched++,index,&model};
    }
    need(matched==s.instances,"GB missing original attachment instance");
   }
   live_=table.live;
   std::size_t unique=0;for(std::size_t i=0;i<total;++i){bool prior=false;
    for(std::size_t j=0;j<i;++j)prior|=attachments_[i].worldInstance==attachments_[j].worldInstance;
    if(!prior)++unique;}
   if(!stockLayout)std::cerr<<"course_count site=particles_GB stock_layout=0 emitters="<<live_<<" attachments="<<total<<" placements="<<unique<<'\n';
   std::cerr<<"gb_course_owner owner=emitter_authoring emitters="<<live_<<" definitions="<<definitionsUsed<<" attachments="<<total
       <<" placements="<<unique<<" stock_layout="<<(stockLayout?1:0)<<'\n';
   // PORT STAND-IN (flagged): the GB native attachment pins, runtime, render
   // and Type11 lanes implement the stock layout exactly (67 instances on 55
   // placements, four definitions). Another layout is refused here by name.
   need(total==attachmentCount&&unique==distinctPlacementCount&&definitionsUsed==definitionCount&&stockLayout,
    "PORT STAND-IN: GB native emitter chain implements the stock 12-descriptor / 67-instance / 55-placement layout only");
   // Members by name: the four EMT definitions are the files the emitter table
   // names, then the course atlas. The stock pins are receipts. Extraction caps
   // are the native stores (EMT 4 KiB, PRT 16 KiB, PSM 8 KiB, PST 136 KiB).
   constexpr std::size_t caps[7]={4096,4096,4096,4096,16384,8192,139264};
   std::array<std::string,7> memberNames{};
   for(std::size_t k=0;k<definitionCount;++k)memberNames[k]="particle\\"+definitionNames[k];
   // The course atlas: the SCB environment's ParticleTextureMapName stem (the
   // same members the cold reservation charges in gb_application_cold50gb.cpp).
   std::string atlasStem;
   for(const auto& b:w.parameters.blocks)if(b.name=="environment")for(const auto& f:b.fields)if(f.name=="particletexturemapname")atlasStem=f.value;
   need(!atlasStem.empty(),"GB SCB names no particle texture map");
   if(const auto dot=atlasStem.find_last_of('.');dot!=std::string::npos)atlasStem.resize(dot);
   {const char* extension[3]={".prt",".psm",".pst"};
    for(std::size_t k=0;k<3;++k)memberNames[definitionCount+k]="particle\\"+atlasStem+extension[k];}
   for(std::size_t index=0;index<members.size();++index){d.ordinal=index;const auto& pin=members[index];const auto& name=memberNames[index];
    // Extraction plus existing SHA helper's copied/padded input and bounded
    // parser scratch. Same source bytes are retained only for EMT/PRT; actual
    // canonical material ownership is established separately through the source.
    if(!reserve(availableAdditionalCpuPayloadBytes,4*caps[index]+65536+(index<5?3*256*sizeof(Section)+3*1024*sizeof(a::OriginalUiDatValue50KG)+16*caps[index]:0),d))return fail(d,Status::Capacity,"GB member phase exceeds remaining budget");
    std::vector<std::uint8_t> raw;need(source_->extractMember(name,raw,d.decode,caps[index])&&!raw.empty(),"GB particle member missing");
    const bool stockName=same(name,pin.name);
    course_archive_receipt50CI::memberReceipt("particles_GB",name,raw,0,0,stockName?pin.bytes:0,stockName?pin.sha256:nullptr);
    if(index<definitionCount)need(detail::definition(raw,index,definitions_[index],d.text),"GB actual EMT source definition rejected");
    if(index==4){std::vector<Section> doc;need(g::readOriginalGcParticleText50KG(raw,raw.size(),pod_materials50ed_fixture::detail::sha256(raw).c_str(),doc),"GB original atlas text");const Section absent{};
     const auto& general=section(doc,"General Info",absent);std::int32_t frames=0,groups=0;
     need(a::originalUiInteger50KG(general,"NumFrames",61,frames,d.text,0x161828)&&frames==130&&
      a::originalUiInteger50KG(general,"NumGroups",0,groups,d.text,0x161854)&&groups==10&&
      a::originalUiNumber50KG(general,"GlowScalar",255,atlas_.glowScalar148,d.text,0x1618a8)&&atlas_.glowScalar148==128,
      "GB exact atlas extent/glow scalar");
     const auto* texture=general.find("TextureMap");need(texture&&texture->text=="demoprt.tga","GB original atlas material name");atlas_.textureMap=texture->text;
     for(std::size_t k=0;k<atlas_.groups.size();++k){const auto& group=section(doc,("Group"+std::to_string(k)).c_str(),absent);auto& t=atlas_.groups[k];
      need(a::originalUiInteger50KG(group,"TypeID",-1,t.type,d.text,0x1618d0)&&a::originalUiInteger50KG(group,"FirstFrame",0,t.first,d.text,0x1618d0)&&
       a::originalUiInteger50KG(group,"LastFrame",0,t.last,d.text,0x1618d0)&&t.first>=0&&t.last>=t.first&&t.last<130,"GB original atlas group");}
     for(std::size_t k=0;k<atlas_.frames.size();++k){const auto& frame=section(doc,("Frame"+std::to_string(k)).c_str(),absent);float l=0,r=0,t=0,b=0;
      need(a::originalUiNumber50KG(frame,"UVLeft",0,l,d.text,0x161a0c)&&a::originalUiNumber50KG(frame,"UVRight",0,r,d.text,0x161a0c)&&
       a::originalUiNumber50KG(frame,"UVTop",0,t,d.text,0x161a0c)&&a::originalUiNumber50KG(frame,"UVBottom",0,b,d.text,0x161a0c),"GB original atlas UV");
      atlas_.frames[k].sourceUv={{{{l,t}},{{r,t}},{{r,b}},{{l,b}}}};}
    }
    if(index<bytes_.size())bytes_[index]=std::move(raw);
   }
   if(!reserve(availableAdditionalCpuPayloadBytes,0,d))return fail(d,Status::Capacity,"GB retained authoring exceeds remaining budget");
   ready_=true;d.status=Status::Ready;d.message.clear();d.retainedPayloadBytes=retainedPayloadBytes();return true;
  }catch(const std::exception& e){return fail(d,Status::Authoring,e.what());}catch(...){return fail(d,Status::Authoring,"GB emitter authoring failed");}
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&source_&&source_->ready();}
 const gb_cold_sources50LA::Owner* sourceOwner()const noexcept{return ready()?source_.get():nullptr;}
 const std::shared_ptr<const gb_cold_sources50LA::Owner>& sourcePin()const {requireReady();return source_;}
 const Descriptor& descriptor(std::size_t i)const {requireReady();if(i>=live_)throw std::out_of_range("GB emitter descriptor outside the live table");return descriptors_[i];}
 std::size_t descriptorLiveCount()const noexcept{return ready()?live_:0;}
 const Attachment& attachment(std::size_t i)const {requireReady();return attachments_.at(i);}
 const Definition& definition(std::size_t index)const {requireReady();return definitions_.at(index);}
 const Definition& definitionForDescriptor(std::size_t row)const {requireReady();return definitions_.at(descriptors_.at(row).definitionIndex);}
 const std::vector<std::uint8_t>& originalEmt(std::size_t index)const {requireReady();if(index>=definitionCount)throw std::out_of_range("GB EMT index");return bytes_[index];}
 const std::vector<std::uint8_t>& originalPrt()const {requireReady();return bytes_[4];}
 const Atlas& atlas()const {requireReady();return atlas_;}
};
} // namespace gb_particle_authoring50GB
