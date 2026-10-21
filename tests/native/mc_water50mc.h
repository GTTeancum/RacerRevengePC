#pragma once
#include "mc_cold_sources50la.h"
#include "native_water_runtime50jc_fixture.h"
#include "../../src/game/undertrack_reset50ew.h"
#include "audio_retained_payload50mc.h"

namespace mc_water50MC {
struct Pin {const char* name;std::size_t offset,bytes;const char* sha;};
inline constexpr Pin pins[]{
 {"tracks\\mc.wtt",466944u,3097u,"43ce93a91873f4b32edcfea72f5f6296d178d9e3d48d2730b566f6f5177d15fb"},
 {"tracks\\Sigs\\test0000.pst",491520u,22860u,"28e2a50aa10e1376cf26ac567c4442dba3870452b97bc70d8bf815d8ade327fb"},
 {"textures\\WaterEnvironment.pst",540672u,33820u,"d4db790a0b5d70754e3d41dc64849832ecf1152d34b3516642129a5ef4fad440"},
 {"tracks\\baseDisplacement.tga",589824u,1068u,"07e69a2bfc92e646788d2ddb0e6112be2b6f52ccb8ef7b84fd64fc889fc84be9"},
 {"textures\\WaterDefault.psm",11722752u,2623u,"b46e8c236ecd7a082a6cc07b3d5f3460ffa23c9a887883d8fd27113cc637b5a1"},
};
inline bool member(const mc_cold_sources50LA::Owner& source,const std::string& name,
 std::vector<std::uint8_t>& bytes,water_resources50JC::Member& identity,std::string& error){
 const Pin* pin=nullptr;for(const auto& p:pins)if(water_resources50JC::sameName(name,p.name))pin=&p;
 if(!pin){error="MC water member lacks actual source identity";return false;}
 const rr::assets::ResourceEntry48C* hit=nullptr;
 for(const auto& entry:source.archive().entries())if(water_resources50JC::sameName(entry.name,name)){
  if(hit){error="MC duplicate water resource";return false;}hit=&entry;
 }
 rr::assets::DecodeError48C diagnostic;
 if(!hit||hit->logical_offset!=pin->offset||!source.archive().extractMember(name,bytes,diagnostic,pin->bytes)||
    bytes.size()!=pin->bytes||pod_materials50ed_fixture::detail::sha256(bytes)!=pin->sha){
  error="MC original water resource identity differs: "+name;return false;
 }
 identity={hit->name,pin->sha,hit->logical_offset,bytes.size()};return true;
}
inline bool prepareResources(const mc_cold_sources50LA::Owner& source,
 std::shared_ptr<water_resources50JC::Owner>& output,std::string& error){
 if(!source.ready()){error="MC water requires actual retained source";return false;}
 auto staged=std::make_shared<water_resources50JC::Owner>();
 std::vector<std::uint8_t> waterBytes;
 if(!member(source,"tracks\\mc.wtt",waterBytes,staged->waterMember,error))return false;
 rr::assets::AiAuthoringDiagnostic50IU authoring;
 if(!rr::assets::readWaterAuthoring50JC(reinterpret_cast<const char*>(waterBytes.data()),waterBytes.size(),staged->authored,authoring)){
  error="MC original water authoring rejected";return false;
 }
 const auto& general=staged->authored.general;
 if(general.poolCount!=0||general.signatureCount!=3||!general.enableBaseDisplacement){
  error="MC actual pool/signature/noise construction differs";return false;
 }
 staged->signatures.resize(3);
 for(std::size_t i=0;i<3;++i){const auto& authored=staged->authored.signatures[i];auto& signature=staged->signatures[i];
  if(authored.frameCount!=1||authored.channel!=static_cast<std::int32_t>(i)||std::string(authored.filename.data())!="test"){
   error="MC actual three one-frame signature channels differ";return false;
  }
  signature.members.resize(1);signature.textures.resize(1);signature.frames.resize(1);
  std::vector<std::uint8_t> bytes;
  if(!member(source,"tracks\\Sigs\\test0000.pst",bytes,signature.members[0],error))return false;
  rr::assets::DecodeError48C diagnostic;
  if(!rr::assets::readWaterSignatureTexture50JC(bytes,signature.textures[0],diagnostic)){
   error="MC actual signature texture rejected: "+diagnostic.code;return false;
  }
  const auto& texture=signature.textures[0];
  signature.frames[0]={{texture.indices.data(),texture.indices.size(),true},texture.rowStride};
  const auto cold=rr::assets::prepareWaterSignatureCold50JC(staged->authored,i,signature.frames.data(),1,
      texture.dimension,texture.pixelStride,signature.cold);
  if(cold.status!=rr::game::WaterHeightStatus50JC::Complete){error="MC signature cold projection rejected";return false;}
 }
 // Verify original archive candidate order, not an inferred global resource miss.
 for(const auto& e:source.archive().entries())if(water_resources50JC::sameName(e.name,"tracks\\baseDisplacement.pst")||
     water_resources50JC::sameName(e.name,"tracks\\baseDisplacement.ipu")){
  error="MC earlier noise candidate needs its own reader";return false;
 }
 std::vector<std::uint8_t> noise;rr::assets::DecodeError48C diagnostic;
 if(!member(source,"tracks\\baseDisplacement.tga",noise,staged->noiseMember,error))return false;
 // This MC-pinned file is independently byte-identical to GC's TGA. The
 // shared reader validates its same16x16 BGRA layout, not a GC archive alias.
 if(!rr::assets::readWaterNoiseTexture50JC(noise,staged->noise,diagnostic)){
  error="MC original noise texture rejected: "+diagnostic.code;return false;
 }
 staged->coldResourcesBound=true;output=std::move(staged);error.clear();return true;
}
class Owner final {
 std::shared_ptr<const mc_cold_sources50LA::Owner> source_;
 std::shared_ptr<water_resources50JC::Owner> resources_;
 native_water_runtime50JC::Owner runtime_;
 bool attempted_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 bool prepare(std::shared_ptr<const mc_cold_sources50LA::Owner> source,
     native_water_runtime50JC::Application& application,std::string& error){
  if(attempted_){error="MC water requires fresh stable owner";return false;}attempted_=true;
  try{
   if(!source||!source->ready()){error="MC water source unavailable";return false;}
   if(!prepareResources(*source,resources_,error))return false;
   runtime_.initialize(application,resources_,native_water_runtime50JC::Policy::OneNativeView);
   source_=std::move(source);error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return source_&&source_->ready()&&resources_&&resources_->coldResourcesBound&&runtime_.initialized();}
 const mc_cold_sources50LA::Owner* sourceIdentity()const noexcept{return ready()?source_.get():nullptr;}
 const std::shared_ptr<water_resources50JC::Owner>& resources()const noexcept{return resources_;}
 native_water_runtime50JC::Owner& runtime()noexcept{return runtime_;}
 const native_water_runtime50JC::Owner& runtime()const noexcept{return runtime_;}
 std::size_t retainedPayloadBytes()const noexcept{
  namespace payload=audio_retained_payload50MC;
  std::size_t bytes=sizeof(*this)+runtime_.additionalPayloadBytes();
  if(!resources_)return bytes;
  const auto memberBytes=[](const water_resources50JC::Member& m){
   return payload::externalStringBytes(m.name)+payload::externalStringBytes(m.sha256);
  };
  bytes+=sizeof(water_resources50JC::Owner)+resources_->signatures.capacity()*sizeof(water_resources50JC::Signature)+
      memberBytes(resources_->waterMember)+memberBytes(resources_->noiseMember);
  for(const auto& signature:resources_->signatures){
   bytes+=signature.members.capacity()*sizeof(water_resources50JC::Member)+
       signature.textures.capacity()*sizeof(rr::assets::WaterSignatureTexture50JC)+
       signature.frames.capacity()*sizeof(rr::game::WaterTextureFrame50JC);
   for(const auto& member:signature.members)bytes+=memberBytes(member);
   for(const auto& texture:signature.textures)bytes+=texture.indices.capacity();
  }
  return bytes;
 }
 bool bindResetDomain(const rr::game::UndertrackPhysicsDomain50EW& source,const float& actual348Lift1F0,
     rr::game::UndertrackPhysicsDomain50EW& output)const noexcept{
  return ready()&&runtime_.bindRuntimeHeightDomain(source,actual348Lift1F0,output);
 }
};
}
