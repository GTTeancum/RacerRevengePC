#pragma once
#include "mc_cold_sources50la.h"
#include "native_water_runtime50jc_fixture.h"
#include "../../src/assets/track_height_descriptor50jb.h"
#include "../../src/game/undertrack_reset50ew.h"
#include "audio_retained_payload50mc.h"

namespace mc_water50MC {
// MC course source adapter over the common loader. The WaterFile comes from
// the course SCB Environment, resolved beside the scene member (1B38DC), like
// track_height_cold50JB; nothing here names an MC file.
inline bool member(const mc_cold_sources50LA::Owner& source,const std::string& name,
 std::vector<std::uint8_t>& bytes,water_resources50JC::Member& identity,std::string& error){
 const rr::assets::ResourceEntry48C* hit=nullptr;
 for(const auto& entry:source.archive().entries())if(water_resources50JC::sameName(entry.name,name)){
  if(hit){error="water resource has duplicate catalog identities";return false;}hit=&entry;
 }
 if(!hit){error="water resource requires an unrepresented original catalog continuation: "+name;return false;}
 rr::assets::DecodeError48C diagnostic;
 if(!source.archive().extractMember(hit->name,bytes,diagnostic)){error="water resource extraction rejected: "+diagnostic.code;return false;}
 identity={hit->name,pod_materials50ed_fixture::detail::sha256(bytes),hit->logical_offset,bytes.size()};return true;
}
inline bool prepareResources(const mc_cold_sources50LA::Owner& source,
 std::shared_ptr<water_resources50JC::Owner>& output,std::string& error){
 if(!source.ready()){error="water requires actual retained course source";return false;}
 rr::assets::TrackHeightDescriptor50JB descriptor{};
 const auto read=rr::assets::readTrackHeightDescriptor50JB(source.world().parameters,descriptor);
 if(read!=rr::assets::TrackHeightReadStatus50JB::Complete){
  error="original Environment WaterFile unavailable (status "+std::to_string(static_cast<unsigned>(read))+
      ", parameter bytes "+std::to_string(source.world().parameters.original_bytes.size())+")";return false;}
 const std::string water=descriptor.waterFile110.value.data();
 //2745B0: exact, case-sensitive "none" bypasses water construction.
 if(water=="none"){error="course SCB authors no water";return false;}
 const std::string scene=mc_cold_sources50LA::sources[0].name;
 const auto slash=scene.find_last_of("/\\");
 if(slash==std::string::npos){error="track source directory unavailable";return false;}
 const std::string name=scene.substr(0,slash+1)+water;
 std::vector<std::uint8_t> wtt;water_resources50JC::Member identity;
 if(!member(source,name,wtt,identity,error))return false;
 return water_resources50JC::prepareAuthored(
  [&](const std::string& n,std::vector<std::uint8_t>& bytes,water_resources50JC::Member& id,std::string& e){
   return member(source,n,bytes,id,e);},
  [&](const std::string& n){
   for(const auto& entry:source.archive().entries())if(water_resources50JC::sameName(entry.name,n))return true;return false;},
  identity.name,identity.logicalOffset,wtt,output,error);
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
