#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/game/terrain_query48j.h"
#include <array>
#include <charconv>
#include <cmath>
#include <filesystem>
#include <limits>
#include <string>

// Cold decoding of the nine actual tracks\\gc.tob trigger definitions. This is
// authoring and callback binding only. It does not select a hit, invent a radius
// test, substitute a pose/route reset, or enable an unsupported query dispatcher.
namespace gc_trigger_authoring50IN {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t count=9,lostCount=8,gate=8;
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
 for(const auto& b:p.blocks)if(b.name==name){need(!found,"GC trigger duplicate source block");found=&b;}
 need(found,"GC trigger source block missing");return *found;
}
inline const std::string* optional(const a::ParameterBlock48F& block,const char* name){
 const std::string* found=nullptr;
 for(const auto& field:block.fields)if(field.name==name){need(!found,"GC trigger duplicate source field");found=&field.value;}
 return found;
}
inline const std::string& field(const a::ParameterBlock48F& block,const char* name){
 const auto* value=optional(block,name);need(value,"GC trigger source field missing");return *value;
}
inline float number(const std::string& text){
 double value=0.;const auto read=std::from_chars(text.data(),text.data()+text.size(),value);
 need(read.ec==std::errc{}&&read.ptr==text.data()+text.size()&&std::isfinite(value),"GC trigger source number rejected");
 const float result=static_cast<float>(value);need(std::isfinite(result),"GC trigger source float range");return result;
}
inline std::uint32_t integer(const std::string& text){
 std::uint32_t value=0;const auto read=std::from_chars(text.data(),text.data()+text.size(),value);
 need(read.ec==std::errc{}&&read.ptr==text.data()+text.size(),"GC trigger source integer rejected");return value;
}
inline bool boolean(const std::string& text){
 if(text=="T"||text=="TRUE")return true;
 need(text=="F"||text=="FALSE","GC trigger source boolean rejected");return false;
}
inline g::Vector3_48C vector(const std::string& text){
 g::Vector3_48C out{};std::size_t begin=0;
 for(unsigned i=0;i<3;++i){const auto end=text.find(',',begin);
  need((end==std::string::npos)==(i==2),"GC trigger source vector extent");
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
  if(attempted_){error="GC trigger owner requires a fresh stable lifetime";return false;}attempted_=true;
  try{using namespace detail;namespace f=pod_materials50ed_fixture::detail;
   f::Inputs input;input.capacity=4205067;const auto bytes=input.read(archivePath,4205067);
   need(bytes.size()==4205067&&f::sha256(bytes)=="69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9","GC trigger original archive identity");
   a::ResArchive48C archive;a::DecodeError48C why{};std::vector<std::uint8_t> tob;
   need(a::decodeResource48C(bytes,archive,why)&&a::extractResMember48C(archive,"tracks\\gc.tob",tob,why),"GC trigger original TOB missing");
   need(f::sha256(tob)=="e677f80d12da2c2225c8ba030e2a29e3a358c52bcfd4f9c981b79c3c53b72454","GC trigger original TOB identity");
   a::ParameterDocument48F p;need(a::decodeParameters48F(tob,p,why),"GC trigger original TOB rejected");
   need(integer(field(block(p,"triggers"),"numberoftriggers"))==count,"GC trigger original count changed");
   constexpr std::uint32_t targets[lostCount]={22,172,181,134,186,137,193,49};
   constexpr std::uint32_t zones[count]={3,3,8,8,8,8,3,3,11};
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){auto& s=next[i];const auto name="trigger"+std::to_string(i+1);const auto& b=block(p,name);
    s.name=field(b,"name");s.zone=integer(field(b,"zone"));s.actionCount=integer(field(b,"numberofactions"));
    s.sphere.center=vector(field(b,"position"));s.sphere.radius=number(field(b,"radius"));
    need(s.zone==zones[i]&&s.zone<=zoneCount&&s.sphere.radius>0.f&&s.actionCount==(i==gate?2u:1u),"GC original trigger bounds/count changed");
    // 2A0290/2DBDA0 defaults StartEnabled=true, RandomTriggerPercent=100.
    // The archive pins the exact omitted/present fields; defaults are not an
    // inference that every trigger should always run.
    if(const auto* v=optional(b,"startenabled"))s.startEnabled=boolean(*v);
    if(const auto* v=optional(b,"randomtriggerpercent")){
     const float percent=number(*v);need(percent==100.f,"GC trigger non100 RNG route not admitted");s.randomTriggerPercent=100;
    }
    s.oneTimeOnly=boolean(field(b,"onetimeonly"));
    if(const auto* v=optional(b,"ignoretrigger")){need(*v=="PLAYER","GC trigger IgnoreTrigger scope changed");s.ignorePlayer=true;}
    need(s.startEnabled&&s.oneTimeOnly==(i==gate)&&s.ignorePlayer==(i!=gate),"GC trigger original activation/ignore policy changed");
    need(!optional(b,"attachtosoultree")&&!optional(b,"size")&&!optional(b,"extents"),"GC trigger non-sphere attachment not admitted");
    const auto& first=block(p,name+"action1");auto& action=s.actions[0];
    if(i<lostCount){
     need(field(first,"actiontype")=="LostAI","GC original LostAI action order changed");
     action.kind=Action::LostAI;action.targetNodeIndex=integer(field(first,"targetnodeindex"));
     need(action.targetNodeIndex==targets[i]&&action.targetNodeIndex<aiNodeCount,"GC original LostAI target rejected");
     // 1B5A60/1B5A98 initializes Reset=0.1B27C0 writes ONLY racer.C80 when omitted.
     need(!optional(first,"reset"),"GC LostAI reset route not admitted");action.reset=false;
    }else{
     need(s.name=="Gate"&&!optional(b,"startenabled")&&!optional(b,"randomtriggerpercent"),"GC Gate original defaults changed");
     need(field(first,"actiontype")=="Animation"&&integer(field(first,"animationnumber"))==1&&boolean(field(first,"onetimeonly")),"GC Gate original animation action changed");
     need(!optional(first,"timetillreset")&&!optional(first,"animationnameref")&&!optional(first,"specificobject"),"GC Gate animation optional route changed");
     action.kind=Action::Animation;action.animationIndex=0; // One-based2DA190 source resolution.
     const auto& second=block(p,name+"action2");auto& sound=s.actions[1];
     need(field(second,"actiontype")=="PodSound","GC Gate original second action changed");sound.kind=Action::PodSound;
     sound.filename=field(second,"filename");sound.position=vector(field(second,"position"));
     sound.minDistance=number(field(second,"mindistance"));sound.maxDistance=number(field(second,"maxdistance"));
     need(sound.filename=="sfx\\race\\bigdoor.vag"&&sound.minDistance==0.f&&sound.maxDistance==2500.f&&!optional(second,"specificobject"),"GC Gate original sound action changed");
    }
   }
   sources_=std::move(next);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_;}
 const Source& source(std::size_t i)const{return sources_.at(i);}
};
} // namespace gc_trigger_authoring50IN
