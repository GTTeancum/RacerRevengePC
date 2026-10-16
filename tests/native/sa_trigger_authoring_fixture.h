#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/game/terrain_query48j.h"
#include <array>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Cold authenticated authoring of the four actual tracks\\sa.tob triggers.
// LostAI only: no animation, sound, pose reset, or geometric-entry substitute.
// This source candidate has not been compiled or executed.
namespace sa_trigger_authoring {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t count=4,lostCount=count;
inline constexpr std::size_t archiveBytes=3786696,decodedBytes=29638656,tobBytes=13314;
inline constexpr std::uint32_t archiveEntries=1065,maxTargetNodeIndex=119;
inline constexpr const char* archiveSha="34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a";
inline constexpr const char* tobSha="b2b53df201ddfc5bd0283cd50911c6e94937f3633bba33b864da13557f415542";
struct ActionSource {
 std::uint32_t targetNodeIndex=0;
 bool reset=false;
};
struct Source {
 std::string name;
 std::uint32_t zone=0,actionCount=0;
 g::CollisionSphere48J sphere{};
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=false;
 std::int32_t randomTriggerPercent=100;
 std::array<ActionSource,1> actions{};
};
namespace detail {
inline void need(bool valid,const char* message){if(!valid)throw std::runtime_error(message);}
inline const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
 const a::ParameterBlock48F* found=nullptr;
 for(const auto& b:p.blocks)if(b.name==name){need(!found,"SA trigger duplicate source block");found=&b;}
 need(found,"SA trigger source block missing");return *found;
}
inline const std::string* optional(const a::ParameterBlock48F& block,const char* name){
 const std::string* found=nullptr;
 for(const auto& f:block.fields)if(f.name==name){need(!found,"SA trigger duplicate source field");found=&f.value;}
 return found;
}
inline const std::string& field(const a::ParameterBlock48F& block,const char* name){
 const auto* value=optional(block,name);need(value,"SA trigger source field missing");return *value;
}
inline float number(const std::string& text){
 double value=0.;const auto read=std::from_chars(text.data(),text.data()+text.size(),value);
 need(read.ec==std::errc{}&&read.ptr==text.data()+text.size()&&std::isfinite(value),"SA trigger source number rejected");
 const float result=static_cast<float>(value);need(std::isfinite(result),"SA trigger source float range");return result;
}
inline std::uint32_t integer(const std::string& text){
 std::uint32_t value=0;const auto read=std::from_chars(text.data(),text.data()+text.size(),value);
 need(read.ec==std::errc{}&&read.ptr==text.data()+text.size(),"SA trigger source integer rejected");return value;
}
inline bool boolean(const std::string& text){
 if(text=="T"||text=="TRUE")return true;
 need(text=="F"||text=="FALSE","SA trigger source boolean rejected");return false;
}
inline g::Vector3_48C vector(const std::string& text){
 g::Vector3_48C out{};std::size_t begin=0;
 for(unsigned i=0;i<3;++i){const auto end=text.find(',',begin);
  need((end==std::string::npos)==(i==2),"SA trigger source vector extent");
  out.v[i]=number(text.substr(begin,end==std::string::npos?end:end-begin));begin=end+1;
 }return out;
}
}
class Owner final {
 std::array<Source,count> sources_{};
 bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 bool load(const std::filesystem::path& archivePath,std::uint32_t aiNodeCount,
           std::uint32_t zoneCount,std::string& error){
  if(attempted_){error="SA trigger owner requires a fresh stable lifetime";return false;}attempted_=true;
  try{using namespace detail;namespace f=pod_materials50ed_fixture::detail;
   need(aiNodeCount>maxTargetNodeIndex,"SA trigger original AI node extent");
   f::Inputs input;input.capacity=archiveBytes;const auto bytes=input.read(archivePath,archiveBytes);
   need(bytes.size()==archiveBytes&&f::sha256(bytes)==archiveSha,"SA trigger original archive identity");
   a::ResArchive48C archive;a::DecodeError48C why{};std::vector<std::uint8_t> tob;
   a::DecodeLimits48C limits{};limits.resource_file_bytes=archiveBytes;
   limits.resource_decoded_bytes=decodedBytes;limits.resource_entries=archiveEntries;
   need(a::decodeResource48C(bytes,archive,why,nullptr,limits),"SA trigger original archive rejected");
   need(archive.entries.size()==archiveEntries&&archive.decoded.size()==decodedBytes,"SA trigger original archive extent");
   need(a::extractResMember48C(archive,"tracks\\sa.tob",tob,why,tobBytes),"SA trigger original TOB missing");
   need(tob.size()==tobBytes&&f::sha256(tob)==tobSha,"SA trigger original TOB identity");
   a::ParameterDocument48F p;need(a::decodeParameters48F(tob,p,why),"SA trigger original TOB rejected");
   need(integer(field(block(p,"triggers"),"numberoftriggers"))==count,"SA trigger original count changed");
   constexpr std::uint32_t targets[count]={111,15,103,119},zones[count]={4,4,21,21};
   constexpr const char* names[count]={"RightBranch","LeftBranch","RightBranch2","LeftBranch2"};
   constexpr const char* positions[count]={"6082.106445,1163.937012,7030.507813","5971.093750,1143.794800,6889.531738",
    "11501.325195,1153.865845,5221.774414","11657.583008,1158.901245,5174.889648"};
   constexpr const char* radii[count]={"78.640366","101.706543","76.411758","76.475983"};
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){auto& s=next[i];const auto name="trigger"+std::to_string(i+1);const auto& b=block(p,name);
    s.name=field(b,"name");s.zone=integer(field(b,"zone"));s.actionCount=integer(field(b,"numberofactions"));
    const auto& position=field(b,"position");const auto& radius=field(b,"radius");
    need(s.name==names[i]&&s.zone==zones[i]&&s.zone<=zoneCount&&s.actionCount==1&&
     position==positions[i]&&radius==radii[i],"SA original trigger identity/geometry changed");
    s.sphere.center=vector(position);s.sphere.radius=number(radius);
    need(s.sphere.radius>0.f,"SA original trigger radius rejected");
    s.startEnabled=boolean(field(b,"startenabled"));s.oneTimeOnly=boolean(field(b,"onetimeonly"));
    need(number(field(b,"randomtriggerpercent"))==100.f,"SA trigger non100 RNG route not admitted");
    s.randomTriggerPercent=100;
    need(field(b,"ignoretrigger")=="PLAYER","SA trigger IgnoreTrigger scope changed");s.ignorePlayer=true;
    need(s.startEnabled&&!s.oneTimeOnly&&s.ignorePlayer,"SA trigger original activation policy changed");
    need(!optional(b,"attachtosoultree")&&!optional(b,"size")&&!optional(b,"extents"),"SA non-sphere attachment not admitted");
    const auto& actionBlock=block(p,name+"action1");auto& action=s.actions[0];
    need(field(actionBlock,"actiontype")=="LostAI","SA original LostAI action changed");
    action.targetNodeIndex=integer(field(actionBlock,"targetnodeindex"));
    need(action.targetNodeIndex==targets[i]&&action.targetNodeIndex<aiNodeCount,"SA original LostAI target rejected");
    // 1B5A60/1B5A98 initializes Reset=0; 1B27C0 writes racer.C80 only.
    need(!optional(actionBlock,"reset"),"SA LostAI reset route not admitted");action.reset=false;
   }
   sources_=std::move(next);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_;}
 const Source& source(std::size_t i)const{return sources_.at(i);}
};
} // namespace sa_trigger_authoring
