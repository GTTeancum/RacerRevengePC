#pragma once
#include "ga_trigger_authoring_fixture.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Cold, authenticated MB source projection only. No action callback, collision
// membership, reset completion or runtime admission is provided by this owner.
namespace mb_trigger_authoring {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t count=3,sphereCount=3,boxCount=0;
inline constexpr std::size_t archiveBytes=4421779,decodedBytes=16392192,tobBytes=19909;
inline constexpr std::uint32_t archiveEntries=487;
inline constexpr const char* archiveSha="6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca";
inline constexpr const char* tobSha="f25f10fff204f124e5525ff2c4a7c1e72f9ce17476e1404d1932f6ab6e8c776d";
struct Source {
 std::string name,targetNodeText;
 std::uint32_t zone=0,actionCount=1,shapeType=5,targetNodeIndex=0;
 g::CollisionSphere48J sphere{};
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=true,reset=false;
 std::int32_t randomTriggerPercent=100;
};
namespace detail {using namespace ga_trigger_authoring::detail;}
class Owner final {
 std::array<Source,count> sources_{};
 std::vector<std::uint8_t> originalTob_;
 bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool load(const std::filesystem::path& archivePath,std::uint32_t aiNodeCount,
           std::uint32_t zoneCount,std::string& error){
  if(attempted_){error="MB trigger owner requires a fresh stable lifetime";return false;}attempted_=true;
  try{using namespace detail;namespace f=pod_materials50ed_fixture::detail;
   f::Inputs input;input.capacity=archiveBytes;const auto bytes=input.read(archivePath,archiveBytes);
   need(bytes.size()==archiveBytes&&f::sha256(bytes)==archiveSha,"MB trigger original archive identity");
   a::ResArchive48C archive;a::DecodeError48C why{};std::vector<std::uint8_t> tob;
   a::DecodeLimits48C limits{};limits.resource_file_bytes=archiveBytes;
   limits.resource_decoded_bytes=decodedBytes;limits.resource_entries=archiveEntries;
   need(a::decodeResource48C(bytes,archive,why,nullptr,limits)&&archive.entries.size()==archiveEntries&&archive.decoded.size()==decodedBytes&&
       a::extractResMember48C(archive,"tracks\\mb.tob",tob,why,tobBytes),"MB trigger original TOB missing");
   need(tob.size()==tobBytes&&f::sha256(tob)==tobSha,"MB trigger original TOB identity");
   a::ParameterDocument48F p;need(a::decodeParameters48F(tob,p,why),"MB trigger original TOB rejected");
   need(p.blocks.size()==135&&integer(field(block(p,"triggers"),"numberoftriggers"))==count,"MB original trigger count changed");
   constexpr const char* names[count]={"DoorBranchRight","DoorBranchLeft","UpShaft"};
   constexpr const char* targetText[count]={"255","42","148d"};
   constexpr std::uint32_t targets[count]={255,42,148},zones[count]={7,7,11};
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){auto& s=next[i];const auto name="trigger"+std::to_string(i+1);const auto& b=block(p,name);
    need(b.fields.size()==9,"MB trigger exact field set changed");
    s.name=field(b,"name");s.zone=integer(field(b,"zone"));s.actionCount=integer(field(b,"numberofactions"));
    s.sphere.center=vector(field(b,"position"));s.sphere.radius=number(field(b,"radius"));
    need(s.name==names[i]&&s.zone==zones[i]&&s.zone<=zoneCount&&s.actionCount==1&&s.sphere.radius>0.f,"MB original trigger tuple changed");
    s.startEnabled=boolean(field(b,"startenabled"));s.oneTimeOnly=boolean(field(b,"onetimeonly"));
    need(s.startEnabled&&!s.oneTimeOnly&&field(b,"ignoretrigger")=="PLAYER"&&number(field(b,"randomtriggerpercent"))==100.f,
      "MB original activation/ignore/RNG policy changed");
    const auto& action=block(p,name+"action1");
    need(action.fields.size()==(i==2?3u:2u)&&field(action,"actiontype")=="LostAI","MB original LostAI action changed");
    s.targetNodeText=field(action,"targetnodeindex");need(s.targetNodeText==targetText[i],"MB exact target text changed");
    // Original1B5A88 ->158B30 ->158920 ->30AE70(base0) ignores the end
    // pointer. Authenticated148d therefore becomes decimal148. Preserve the
    // bytes above; never widen the generic strict integer helper for this case.
    s.targetNodeIndex=i==2?148u:integer(s.targetNodeText);
    const auto* reset=optional(action,"reset");
    if(i==2){need(reset&&*reset=="1","MB UpShaft Reset1 changed");s.reset=true;}
    else need(!reset,"MB omitted Reset default changed");
    need(s.targetNodeIndex==targets[i]&&s.targetNodeIndex<aiNodeCount,"MB original LostAI target out of actual AI range");
   }
   sources_=std::move(next);originalTob_=std::move(tob);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_;}
 const Source& source(std::size_t i)const {
  if(!ready_)throw std::runtime_error("MB trigger source owner not ready");return sources_.at(i);
 }
 const std::vector<std::uint8_t>& originalTob()const {
  if(!ready_)throw std::runtime_error("MB original TOB owner not ready");return originalTob_;
 }
};
} // namespace mb_trigger_authoring
