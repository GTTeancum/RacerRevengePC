#pragma once
#include "pod_materials50ed_fixture.h"
#include "watchtower_source_fixture.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/game/terrain_query48j.h"
#include <array>
#include <charconv>
#include <cmath>
#include <filesystem>
#include <limits>
#include <string>

// Cold decoding of the ten actual tracks\\ga.tob trigger definitions. This is
// authoring and callback binding only. It does not select a hit, invent a radius
// test, substitute a pose/route reset, or enable an unsupported query dispatcher.
namespace ga_trigger_authoring {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t count=10,lostCount=8,gate=9,defensegate=0;
enum class Action {LostAI,Animation,PodSound};
struct ActionSource {
 Action kind=Action::LostAI;
 std::uint32_t targetNodeIndex=0,animationIndex=0;
 bool reset=false;
 g::Vector3_48C position{};
 float minDistance=0.f,maxDistance=0.f;
 std::string filename;
};
struct Source {
 std::string name;
 std::uint32_t zone=0,actionCount=0;
 g::CollisionSphere48J sphere{};
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=false;
 std::int32_t randomTriggerPercent=100;
 std::array<ActionSource,2> actions{};
};
namespace detail {
inline void need(bool valid,const char* message){if(!valid)throw std::runtime_error(message);}
inline const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
 const a::ParameterBlock48F* found=nullptr;
 for(const auto& b:p.blocks)if(b.name==name){need(!found,"GA trigger duplicate source block");found=&b;}
 need(found,"GA trigger source block missing");return *found;
}
inline const std::string* optional(const a::ParameterBlock48F& block,const char* name){
 const std::string* found=nullptr;
 for(const auto& field:block.fields)if(field.name==name){need(!found,"GA trigger duplicate source field");found=&field.value;}
 return found;
}
inline const std::string& field(const a::ParameterBlock48F& block,const char* name){
 const auto* value=optional(block,name);need(value,"GA trigger source field missing");return *value;
}
inline float number(const std::string& text){
 double value=0.;const auto read=std::from_chars(text.data(),text.data()+text.size(),value);
 need(read.ec==std::errc{}&&read.ptr==text.data()+text.size()&&std::isfinite(value),"GA trigger source number rejected");
 const float result=static_cast<float>(value);need(std::isfinite(result),"GA trigger source float range");return result;
}
inline std::uint32_t integer(const std::string& text){
 std::uint32_t value=0;const auto read=std::from_chars(text.data(),text.data()+text.size(),value);
 need(read.ec==std::errc{}&&read.ptr==text.data()+text.size(),"GA trigger source integer rejected");return value;
}
inline bool boolean(const std::string& text){
 if(text=="T"||text=="TRUE")return true;
 need(text=="F"||text=="FALSE","GA trigger source boolean rejected");return false;
}
inline g::Vector3_48C vector(const std::string& text){
 g::Vector3_48C out{};std::size_t begin=0;
 for(unsigned i=0;i<3;++i){const auto end=text.find(',',begin);
  need((end==std::string::npos)==(i==2),"GA trigger source vector extent");
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
  if(attempted_){error="GA trigger owner requires a fresh stable lifetime";return false;}attempted_=true;
  try{using namespace detail;namespace f=pod_materials50ed_fixture::detail;
   f::Inputs input;input.capacity=watchtower_source_fixture::archiveBytes;const auto bytes=input.read(archivePath,watchtower_source_fixture::archiveBytes);
   need(bytes.size()==watchtower_source_fixture::archiveBytes&&f::sha256(bytes)==watchtower_source_fixture::archiveSha,"GA trigger original archive identity");
   a::ResArchive48C archive;a::DecodeError48C why{};std::vector<std::uint8_t> tob;
   a::DecodeLimits48C limits{};limits.resource_file_bytes=watchtower_source_fixture::archiveBytes;
   limits.resource_decoded_bytes=watchtower_source_fixture::decodedBytes;limits.resource_entries=watchtower_source_fixture::archiveEntries;
   need(a::decodeResource48C(bytes,archive,why,nullptr,limits)&&a::extractResMember48C(archive,"tracks\\ga.tob",tob,why),"GA trigger original TOB missing");
   need(f::sha256(tob)==watchtower_source_fixture::objects.sha,"GA trigger original TOB identity");
   a::ParameterDocument48F p;need(a::decodeParameters48F(tob,p,why),"GA trigger original TOB rejected");
   need(integer(field(block(p,"triggers"),"numberoftriggers"))==count,"GA trigger original count changed");
   constexpr std::uint32_t targets[lostCount]={80,85,93,47,63,102,35,40};
   constexpr std::uint32_t zones[count]={11,5,5,5,5,7,7,5,5,7};
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){auto& s=next[i];const auto name="trigger"+std::to_string(i+1);const auto& b=block(p,name);
    if(const auto* name=optional(b,"name"))s.name=*name;
    s.zone=integer(field(b,"zone"));s.actionCount=integer(field(b,"numberofactions"));
    s.sphere.center=vector(field(b,"position"));s.sphere.radius=number(field(b,"radius"));
    need(s.zone==zones[i]&&s.zone<=zoneCount&&s.sphere.radius>0.f&&s.actionCount==(i==gate?2u:1u),"GA original trigger bounds/count changed");
    // 2A0290/2DBDA0 defaults StartEnabled=true, RandomTriggerPercent=100.
    // The archive pins the exact omitted/present fields; defaults are not an
    // inference that every trigger should always run.
    if(const auto* v=optional(b,"startenabled"))s.startEnabled=boolean(*v);
    if(const auto* v=optional(b,"randomtriggerpercent")){
     const float percent=number(*v);need(percent==100.f,"GA trigger non100 RNG route not admitted");s.randomTriggerPercent=100;
    }
    s.oneTimeOnly=boolean(field(b,"onetimeonly"));
    if(const auto* v=optional(b,"ignoretrigger")){need(*v=="PLAYER","GA trigger IgnoreTrigger scope changed");s.ignorePlayer=true;}
    need(s.startEnabled==(i!=defensegate)&&s.oneTimeOnly==(i==gate)&&s.ignorePlayer==(i>defensegate&&i<gate),"GA trigger original activation/ignore policy changed");
    need(!optional(b,"attachtosoultree")&&!optional(b,"size")&&!optional(b,"extents"),"GA trigger non-sphere attachment not admitted");
    const auto& first=block(p,name+"action1");auto& action=s.actions[0];
    if(i>defensegate&&i<gate){
     need(field(first,"actiontype")=="LostAI","GA original LostAI action order changed");
     action.kind=Action::LostAI;action.targetNodeIndex=integer(field(first,"targetnodeindex"));
     need(action.targetNodeIndex==targets[i-1]&&action.targetNodeIndex<aiNodeCount,"GA original LostAI target rejected");
     // 1B5A60/1B5A98 initializes Reset=0.1B27C0 writes ONLY racer.C80 when omitted.
     need(!optional(first,"reset"),"GA LostAI reset route not admitted");action.reset=false;
    }else{
     need(s.name.empty()&&optional(b,"startenabled")&&optional(b,"randomtriggerpercent"),"GA animation trigger authored fields differ");
     need(field(first,"actiontype")=="Animation"&&integer(field(first,"animationnumber"))==(i==defensegate?1u:2u)&&boolean(field(first,"onetimeonly")),"GA Gate original animation action changed");
     need(!optional(first,"timetillreset")&&!optional(first,"animationnameref")&&!optional(first,"specificobject"),"GA Gate animation optional route changed");
     action.kind=Action::Animation;action.animationIndex=i==defensegate?0u:1u;
     // Source parser ignores action OneTimeOnly; the trigger flag supplies it.
     // Trigger1 remains disabled, repeating policy retained. No startup open.
     if(i==defensegate)continue;
     const auto& second=block(p,name+"action2");auto& sound=s.actions[1];
     need(field(second,"actiontype")=="PodSound","GA Gate original second action changed");sound.kind=Action::PodSound;
     sound.filename=field(second,"filename");sound.position=vector(field(second,"position"));
     sound.minDistance=number(field(second,"mindistance"));sound.maxDistance=number(field(second,"maxdistance"));
     need(sound.filename=="sfx\\race\\bigdoor.vag"&&sound.minDistance==0.f&&sound.maxDistance==2500.f&&sound.position.v[0]==11425.f&&sound.position.v[1]==2058.f&&sound.position.v[2]==13227.f&&!optional(second,"specificobject"),"GA Gate original sound action changed");
    }
   }
   sources_=std::move(next);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_;}
 const Source& source(std::size_t i)const{return sources_.at(i);}
};
} // namespace ga_trigger_authoring
