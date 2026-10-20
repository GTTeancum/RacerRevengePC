#pragma once
#include "rc_cold_sources50la.h"
#include "ga_trigger_authoring_fixture.h"
#include "../../src/game/original_gc_spray50kg.h"
#include "../../src/game/water_wake50jc.h"
#include <array>
#include <cfenv>
#include <cstring>
#include <limits>
#include <memory>
#include <optional>
#include <thread>

namespace rc_particle_authoring50RC {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t descriptorCount=7,retainedSectionCount=13,attachmentCount=10;
enum class Kind : std::uint8_t {Smoke0=0,Fire8=8,Fog12=12};
struct MemberPin {const char* name;std::size_t bytes;const char* sha256;};
inline constexpr std::array<MemberPin,6> members{{
 {"particle\\freakysmoke.emt",1294,"cd8b3d8e2975e7d1f5403ff60bdd27003013b42b2f76c3b0059d336be7a8ab21"},
 {"particle\\rcfire.emt",801,"0690f4d9116b73856703ff33241d0d4c53c54f8fc64a7043ff1955c8f17644f3"},
 {"particle\\cavesmoke.emt",2911,"352aea53f46e979ef0dbb6af7377e5ca065d9febddfa7ce086d5ad7d8c7c434f"},
 {"particle\\demoprt.prt",7671,"9ee08f9801aeae21750eb106a2892f1399a6ab13f41f754d1d66b698c1428958"},
 {"particle\\demoprt.psm",5022,"ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1"},
 {"particle\\demoprt.pst",132124,"730d836d0decb8c5460b1318ec90ef82d88f342fbd92fc81580c190b5c09f019"}
}};
struct Descriptor {
 std::uint32_t ordinal=0,startingZone=0,instances=0,sourceBlock=0;
 Kind kind=Kind::Smoke0;std::string attachToSoulTree,childObject,filename;
 std::array<float,4> offset{{0,0,0,1}};
 std::optional<std::int32_t> authoredStartOn;
 std::int32_t effectiveStartOn=0;
};
struct Attachment {
 std::size_t descriptor=0,instanceOrdinal=0,worldIndex=0;
 // Pointer into the SAME retained immutable World. Never a copied model or
 // an invented SceneGraph/render stamp. Runtime must bind the real graph.
 const a::WorldInstance48F* worldInstance=nullptr;
};
// These are source-derived numeric fields, with no POD/GC owner tag. The cold
// definitions do not issue controls, manager rows, Application or live leases.
struct Definition {
 Kind kind=Kind::Smoke0;g::OriginalGcCommonEmitterTiming50KG timing{};
 std::array<float,3> velocity60{},minimumVelocityNoise240{},maximumVelocityNoise250{},minRange{},maxRange{};
 std::int32_t particleGroup1B8=-1;
 float particleLife17C=1000,growth180=0,initialSize184=1,emitRate188=500,ageAtBirth1A0=0;
 float gravityMultiplier=3,groundLevel=std::numeric_limits<float>::max(),restitution=.55f;
 float framesPerSet=-1,inverseFramesPerSet=-1;std::uint32_t particleFlags198=0;
 bool randomMirrorX=false,randomMirrorY=false,applySpreadToVelocity=true;
 float minimumInitialSize208=0,maximumInitialSize20C=5,initialSizeToSpeedRatio210=.01f;
 float minimumLife214=500,maximumLife218=3500,minimumPercentLife21C=.5f;
 float minimumPercentLifeConvergance220=1,lifeToSpeedRatio224=5,initialPercentAge228=0;
 float minimumEmitRate22C=0,maximumEmitRate230=10,particlesToDistanceRatio234=.01f;
 float particlesToSpeedRatio238=0,minimumEmissionSpeed23C=0;
 float distanceSquared220=0,inverseDistanceSquared224=0,particlesInVolume228=0,oscillationPeriod22C=0,oscillationMagnitude234=0;
 bool parsed=false;
};
struct Atlas {
 std::array<g::OriginalGcParticleGroup50KG,9> groups{};
 std::array<g::OriginalGcParticleFrame50KG,105> frames{};
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
inline bool definition(const std::vector<std::uint8_t>& bytes,Kind kind,Definition& out,D& d){
 if(std::fegetround()!=FE_TONEAREST)return false;
 std::vector<Section> doc;if(!sections(bytes,doc,d))return false;
 const Section absent{};const auto& general=section(doc,"General Info",absent);
 const auto& properties=section(doc,"Properties",absent);const auto& animation=section(doc,"Animation",absent);
 Definition f;f.kind=kind;g::water_wake_detail50jc::Math m;
 auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){return a::originalUiNumber50KG(s,key,fallback,value,d,pc);};
 auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){return a::originalUiInteger50KG(s,key,fallback,value,d,pc);};
 auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){return a::originalUiTriple50KG(general,key,{},value,d,pc);};
 //1602B0 reads derived fog fields BEFORE15AEA0. All six fields are present
 // in the pinned file; explicit source defaults are retained for clarity.
 if(kind==Kind::Fog12){float distance=0;
  if(!v("MinRange",f.minRange,0x1602f0)||!v("MaxRange",f.maxRange,0x16030c)||
     !n(general,"DistanceToFade",200,distance,0x160328))return false;
  f.distanceSquared220=m.mul(distance,distance,0x160344);
  f.inverseDistanceSquared224=m.div(1,f.distanceSquared220,0x160354);
  if(!n(general,"ParticlesInVolume",100,f.particlesInVolume228,0x16036c)||
     !n(general,"OscillationSpeed",10,f.oscillationPeriod22C,0x160388)||
     !n(general,"OscillationMagnitude",1,f.oscillationMagnitude234,0x1603a4))return false;
 }
 if(kind==Kind::Fire8){
  //15E6D0 reads the two derived ranges and spread flag BEFORE15AEA0.
  if(!v("MinRange",f.minRange,0x15e710)||!v("MaxRange",f.maxRange,0x15e72c)||
     !a::originalUiBoolean50KG(general,"ApplySpreadToVelocity",true,f.applySpreadToVelocity,d,0x15e744)||!f.applySpreadToVelocity)return false;
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
 if(kind==Kind::Fog12)f.particleFlags198=(f.particleFlags198|0x80u)&~8u;

 if(kind==Kind::Smoke0){
  // Actual generic15C330 reader. All derived fields below are present in
  // this exact CaveSmoke member; keys/defaults come from its own reader.
  if(!n(general,"MinimumPercentLife",.5f,f.minimumPercentLife21C,0x15c378)||
     !n(general,"MinimumPercentLifeConvergance",1,f.minimumPercentLifeConvergance220,0x15c438)||
     !n(general,"MinimumInitialSize",0,f.minimumInitialSize208,0x15c4d8)||
     !n(general,"MaximumInitialSize",5,f.maximumInitialSize20C,0x15c57c)||
     !n(general,"InitialSizeToSpeedRatio",bits(0x3c23d70a),f.initialSizeToSpeedRatio210,0x15c624)||
     !n(general,"MinimumLife",500,f.minimumLife214,0x15c6c8)||
     !n(general,"MaximumLife",3500,f.maximumLife218,0x15c770)||
     !n(general,"LifeToSpeedRatio",5,f.lifeToSpeedRatio224,0x15c814)||
     !n(general,"MinimumEmitRate",0,f.minimumEmitRate22C,0x15c8b4)||
     !n(general,"MaximumEmitRate",10,f.maximumEmitRate230,0x15c958)||
     !n(general,"ParticlesToDistanceRatio",bits(0x3c23d70a),f.particlesToDistanceRatio234,0x15ca00)||
     !n(general,"ParticlesToSpeedRatio",0,f.particlesToSpeedRatio238,0x15caa0)||
     !n(general,"InitialPercentAge",0,f.initialPercentAge228,0x15cb40)||
     !n(general,"MinimumEmissionSpeed",0,f.minimumEmissionSpeed23C,0x15cbe0)||
     !v("MinimumVelocityNoise",f.minimumVelocityNoise240,0x15cc84)||
     !v("MaximumVelocityNoise",f.maximumVelocityNoise250,0x15cca0))return false;
 }
 if(m.badPC){d.sourcePC=m.badPC;return false;}
 // Bounded source-specific admission, not permissive arbitrary EMT support.
 if(f.timing.mode1D8||f.timing.delayTime1E0||f.timing.minTimeOn1E8||f.timing.maxTimeOn1EC||
    f.timing.minTimeOff1F0||f.timing.maxTimeOff1F4||!f.randomMirrorX||!f.randomMirrorY)return false;
 if(kind==Kind::Fog12){if(f.particleGroup1B8!=8||f.particleLife17C!=100||f.initialSize184!=3||f.growth180||f.emitRate188||
  f.ageAtBirth1A0||f.velocity60!=std::array<float,3>{}||f.distanceSquared220!=1||f.inverseDistanceSquared224!=1||
  f.particlesInVolume228!=500||f.oscillationPeriod22C!=6||f.oscillationMagnitude234!=2||f.framesPerSet||f.particleFlags198!=0x2081u)return false;}
 else if(kind==Kind::Fire8){if(f.particleGroup1B8!=4||f.particleLife17C!=1000||f.initialSize184!=40||f.growth180!=-6.5f||
  f.emitRate188!=250||f.framesPerSet!=8||f.particleFlags198!=0x1405u||f.velocity60!=std::array<float,3>{{0,80,0}})return false;}
 else if(f.particleGroup1B8!=0||f.growth180!=20||f.framesPerSet||f.particleFlags198!=0x300fu||
  f.minimumLife214!=800||f.maximumLife218!=1000||f.minimumPercentLife21C!=.7f||f.minimumPercentLifeConvergance220!=6||
  f.minimumInitialSize208!=20||f.maximumInitialSize20C!=40||f.minimumEmissionSpeed23C||f.velocity60!=std::array<float,3>{})return false;
 f.parsed=true;out=f;return true;
}
} // detail

class Owner final {
 std::shared_ptr<const rc_cold_sources50LA::Owner> source_;
 std::array<Descriptor,descriptorCount> descriptors_{};
 std::array<Attachment,attachmentCount> attachments_{};
 std::array<Definition,3> definitions_{};Atlas atlas_{};
 std::array<std::vector<std::uint8_t>,4> bytes_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false;
 static void add(std::size_t& n,std::size_t x)noexcept {n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;}
 bool fail(Diagnostic& d,Status status,const char* message)noexcept {d.status=status;try{d.message=message;}catch(...){}d.retainedPayloadBytes=retainedPayloadBytes();return false;}
 bool reserve(std::size_t limit,std::size_t extra,Diagnostic& d)const noexcept {
  auto n=retainedPayloadBytes();add(n,extra);d.peakReservedPayloadBytes=std::max(d.peakReservedPayloadBytes,n);return n<=limit;
 }
 void requireReady()const {if(!ready())throw std::runtime_error("RC particle source is not ready");}
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
 bool decode(std::shared_ptr<const rc_cold_sources50LA::Owner> source,std::size_t availableAdditionalCpuPayloadBytes,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id())return fail(d,Status::Input,"RC particle source thread mismatch");
  if(attempted_)return fail(d,Status::Input,"RC particle source is one-attempt");attempted_=true;
  try{using namespace detail;
   if(!source||!source->ready())return fail(d,Status::Input,"RC particle source requires actual cold owner");
   source_=std::move(source);const auto& p=source_->triggerParameters();const auto& w=source_->world();
   need(p.blocks.size()==907&&!p.preservesDuplicateLabels&&p.original_bytes==source_->originalTob()&&
    w.zones.size()==15&&w.portals.size()==15&&w.instances.size()==1277,"RC full source document shape");
   need(integer(field(block(p,"particleemitters"),"numberofemitters"))==descriptorCount,"RC declared emitter count");
   // Count bound27DEB0/27DF70 selects1..7. Preserve8..13 in the shared TOB;
   // none of those retained but inactive records becomes a runtime descriptor.
   std::size_t seen=0;for(const auto& b:p.blocks)if(b.name.rfind("particleemitter",0)==0&&b.name!="particleemitters")++seen;
   need(seen==retainedSectionCount,"RC retained emitter sections");
   constexpr const char* attach[]={"rockj06.slt","Mushroom03_149.slt","rockf18.slt","rocka61.slt","MushroomNewLittle03.slt","FireP0t01.slt","Cylinder07.slt"};
   constexpr const char* names[]={"FreakySmoke.emt","FreakySmoke.emt","FreakySmoke.emt","FreakySmoke.emt","FreakySmoke.emt","rcfire.emt","CaveSmoke.emt"};
   constexpr std::uint32_t zones[]={12,13,3,7,6,1,4};constexpr float offsets[]={50,50,50,50,50,25,-25};
   constexpr const char* placed[]={"zone12-model49","zone13-model159","zone3-model163","zone7-model32","zone6-model94","zone1-model39","zone1-model40","zone1-model41","zone1-model42","zone4-model22"};
   if(!reserve(availableAdditionalCpuPayloadBytes,32768,d))return fail(d,Status::Capacity,"RC descriptor staging exceeds remaining budget");
   std::size_t total=0;
   for(std::size_t row=0;row<descriptorCount;++row){d.ordinal=row+1;auto& s=descriptors_[row];const auto& b=block(p,"particleemitter"+std::to_string(row+1));
    s.ordinal=static_cast<std::uint32_t>(row+1);s.sourceBlock=static_cast<std::uint32_t>(&b-p.blocks.data());
    s.startingZone=integer(field(b,"startingzone"));s.instances=integer(field(b,"numberofinstances"));
    const auto type=integer(field(b,"typeid"));need(type==(row<5?12u:row==5?8u:0u),"RC actual emitter subtype");s.kind=static_cast<Kind>(type);
    s.attachToSoulTree=field(b,"attachtosoultree");s.filename=field(b,"emitterfilename");
    const auto offset=vector(field(b,"offsetfromsoultree"));for(unsigned k=0;k<3;++k)s.offset[k]=offset.v[k];
    need(!optional(b,"starton")&&!optional(b,"childobject")&&!optional(b,"position"),"RC exact omitted emitter fields changed");
    s.childObject="NONE";s.effectiveStartOn=1; //27C720..72C a2=1 then158B30.
    need(b.fields.size()==6&&s.startingZone==zones[row]&&s.instances==(row==5?4u:1u)&&s.attachToSoulTree==attach[row]&&
      s.filename==names[row]&&s.offset==std::array<float,4>{{0,offsets[row],0,1}},"RC actual descriptor identity/order");
    std::size_t matched=0;
    for(std::size_t index=0;index<w.instances.size();++index){const auto& model=w.instances[index];if(model.zone!=s.startingZone)continue;
     need(model.parameter_block<w.parameters.blocks.size(),"RC model source block");const auto& modelBlock=w.parameters.blocks[model.parameter_block];
     const auto* slt=optional(modelBlock,"slt");if(!slt||!same(*slt,s.attachToSoulTree.c_str()))continue;
     need(total<attachmentCount&&matched<s.instances&&model.section==placed[total],"RC original attachment traversal/order");
     need(model.source_block_offset==modelBlock.offset&&model.look==std::array<double,3>{{0,0,1}}&&model.up==std::array<double,3>{{0,1,0}}&&!model.use_lighting,
       "RC actual placed model provenance");
     attachments_[total++]={row,matched++,index,&model};
    }
    need(matched==s.instances,"RC missing or ambiguous original attachment instance");
   }
   need(total==attachmentCount,"RC full declared emitter instance extent");
   for(std::size_t index=0;index<members.size();++index){d.ordinal=index;const auto& pin=members[index];
    // Extraction plus existing SHA helper's copied/padded input and bounded
    // parser scratch. Same source bytes are retained only for EMT/PRT; actual
    // canonical material ownership is established separately through the source.
    if(!reserve(availableAdditionalCpuPayloadBytes,4*pin.bytes+65536+(index<4?3*128*sizeof(Section)+3*512*sizeof(a::OriginalUiDatValue50KG)+16*pin.bytes:0),d))return fail(d,Status::Capacity,"RC member phase exceeds remaining budget");
    std::vector<std::uint8_t> raw;need(source_->extractMember(pin.name,raw,d.decode,pin.bytes)&&raw.size()==pin.bytes&&
      pod_materials50ed_fixture::detail::sha256(raw)==pin.sha256,"RC exact particle member identity");
    if(index<3){const auto kind=index==0?Kind::Fog12:index==1?Kind::Fire8:Kind::Smoke0;
     need(detail::definition(raw,kind,definitions_[index],d.text),"RC actual EMT source definition rejected");}
    if(index==3){std::vector<Section> doc;need(g::readOriginalGcParticleText50KG(raw,pin.bytes,pin.sha256,doc),"RC original atlas text");const Section absent{};
     const auto& general=section(doc,"General Info",absent);std::int32_t frames=0,groups=0;
     need(a::originalUiInteger50KG(general,"NumFrames",61,frames,d.text,0x161828)&&frames==105&&
      a::originalUiInteger50KG(general,"NumGroups",0,groups,d.text,0x161854)&&groups==9&&
      a::originalUiNumber50KG(general,"GlowScalar",255,atlas_.glowScalar148,d.text,0x1618a8)&&atlas_.glowScalar148==128,
      "RC exact atlas extent/glow scalar");
     const auto* texture=general.find("TextureMap");need(texture&&texture->text=="demoprt.tga","RC original atlas material name");atlas_.textureMap=texture->text;
     for(std::size_t k=0;k<atlas_.groups.size();++k){const auto& group=section(doc,("Group"+std::to_string(k)).c_str(),absent);auto& t=atlas_.groups[k];
      need(a::originalUiInteger50KG(group,"TypeID",-1,t.type,d.text,0x1618d0)&&a::originalUiInteger50KG(group,"FirstFrame",0,t.first,d.text,0x1618d0)&&
       a::originalUiInteger50KG(group,"LastFrame",0,t.last,d.text,0x1618d0)&&t.first>=0&&t.last>=t.first&&t.last<105,"RC original atlas group");}
     for(std::size_t k=0;k<atlas_.frames.size();++k){const auto& frame=section(doc,("Frame"+std::to_string(k)).c_str(),absent);float l=0,r=0,t=0,b=0;
      need(a::originalUiNumber50KG(frame,"UVLeft",0,l,d.text,0x161a0c)&&a::originalUiNumber50KG(frame,"UVRight",0,r,d.text,0x161a0c)&&
       a::originalUiNumber50KG(frame,"UVTop",0,t,d.text,0x161a0c)&&a::originalUiNumber50KG(frame,"UVBottom",0,b,d.text,0x161a0c),"RC original atlas UV");
      atlas_.frames[k].sourceUv={{{{l,t}},{{r,t}},{{r,b}},{{l,b}}}};}
    }
    if(index<bytes_.size())bytes_[index]=std::move(raw);
   }
   if(!reserve(availableAdditionalCpuPayloadBytes,0,d))return fail(d,Status::Capacity,"RC retained authoring exceeds remaining budget");
   ready_=true;d.status=Status::Ready;d.message.clear();d.retainedPayloadBytes=retainedPayloadBytes();return true;
  }catch(const std::exception& e){return fail(d,Status::Authoring,e.what());}catch(...){return fail(d,Status::Authoring,"RC emitter authoring failed");}
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&source_&&source_->ready();}
 const rc_cold_sources50LA::Owner* sourceOwner()const noexcept{return ready()?source_.get():nullptr;}
 const std::shared_ptr<const rc_cold_sources50LA::Owner>& sourcePin()const {requireReady();return source_;}
 const Descriptor& descriptor(std::size_t i)const {requireReady();return descriptors_.at(i);}
 const Attachment& attachment(std::size_t i)const {requireReady();return attachments_.at(i);}
 const Definition& definition(Kind kind)const {requireReady();return definitions_.at(kind==Kind::Fog12?0:kind==Kind::Fire8?1:kind==Kind::Smoke0?2:3);}
 const std::vector<std::uint8_t>& originalEmt(Kind kind)const {requireReady();return bytes_.at(kind==Kind::Fog12?0:kind==Kind::Fire8?1:kind==Kind::Smoke0?2:4);}
 const std::vector<std::uint8_t>& originalPrt()const {requireReady();return bytes_[3];}
 const Atlas& atlas()const {requireReady();return atlas_;}
};
} // namespace rc_particle_authoring50RC
