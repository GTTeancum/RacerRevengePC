#pragma once
#include "rc_cold_sources50la.h"
#include <array>
#include <charconv>
#include <cmath>
#include <memory>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace rc_trigger_authoring50LA {
namespace a=rr::assets;
inline constexpr std::size_t count=11;
// Authoring values only. No geometry kernel, transform, collider, C80 lease,
// action callback, native Entity ID or source-manager PLAYER resolution.
struct Vector3 {std::array<float,3> values{};};
enum class Shape {sphere,box};
struct Source {
 std::string name;
 std::uint32_t sourceZoneOrdinal=0,targetAiNodeIndex=0;
 Shape shape=Shape::sphere;
 Vector3 position;
 std::optional<float> radius,yaw,pitch,roll;
 std::optional<Vector3> box; // Original local Box fields, not transformed corners.
 std::optional<std::string> ignoreTrigger;
 bool startEnabled=false,oneTimeOnly=false;
 float randomTriggerPercent=0;
 std::optional<bool> authoredReset;
 bool resetDefault1B5A60=false; // All eleven omit Reset; actual original default0.
 // Borrow exact blocks; absent fields and unused authoring remain observable.
 const a::ParameterBlock48F* originalTrigger=nullptr;
 const a::ParameterBlock48F* originalAction=nullptr;
};
class Owner final {
 std::shared_ptr<const rc_cold_sources50LA::Owner> source_;
 std::array<Source,count> sources_{};
 bool attempted_=false,decoded_=false;
 static void need(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
 static const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
  const a::ParameterBlock48F* found=nullptr;
  for(const auto& b:p.blocks)if(b.name==name){need(!found,"duplicate RC block");found=&b;}
  need(found,"missing RC block");return *found;
 }
 static const std::string* optional(const a::ParameterBlock48F& b,const char* name){
  const std::string* found=nullptr;
  for(const auto& f:b.fields)if(f.name==name){need(!found,"duplicate RC field");found=&f.value;}return found;
 }
 static const std::string& field(const a::ParameterBlock48F& b,const char* name){
  const auto* value=optional(b,name);need(value,"missing RC field");return *value;
 }
 static std::uint32_t integer(const std::string& text){
  std::uint32_t value=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),value);
  need(r.ec==std::errc{}&&r.ptr==text.data()+text.size(),"RC integer text");return value;
 }
 static float number(const std::string& text){
  float value=0;need(a::parseTrackNumber50AD(text.data(),text.size(),value)&&std::isfinite(value),"RC source decimal");return value;
 }
 static Vector3 vector(const std::string& text){
  Vector3 result;std::size_t begin=0;
  for(std::size_t i=0;i<3;++i){const auto end=text.find(',',begin);
   need((end==std::string::npos)==(i==2),"RC vector extent");
   result.values[i]=number(text.substr(begin,end==std::string::npos?end:end-begin));begin=end+1;
  }return result;
 }
public:
 struct Diagnostic {const char* failure=nullptr;std::size_t trigger=0;};
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 bool decode(std::shared_ptr<const rc_cold_sources50LA::Owner> source,Diagnostic& d)noexcept {
  d={};if(attempted_){d.failure="RC trigger owner already attempted";return false;}attempted_=true;
  try{
   need(source&&source->ready()&&source->world().zones.size()==15&&source->ai().valid()&&source->ai().count()==209,
        "actual authenticated RC source and AI spline owner required");
   source_=std::move(source);const auto& p=source_->triggerParameters();
   need(field(block(p,"triggers"),"numberoftriggers")=="11","RC trigger count");
   constexpr std::uint32_t targets[count]={150,12,42,165,179,62,204,69,191,78,193};
   constexpr std::uint32_t zones[count]={10,10,13,13,9,9,14,14,14,4,4};
   constexpr const char* names[count]={"Branch1Right","Branch1Left","Branch2Right","Branch2Left","Branch3Right","Branch3Left","Branch4Right","Branch4Middle","Branch4Left","Branch5Right","Branch5Left"};
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){d.trigger=i+1;
    const auto label="trigger"+std::to_string(i+1);const auto& b=block(p,label);const auto& action=block(p,label+"action1");auto& out=next[i];
    need(field(b,"name")==names[i]&&integer(field(b,"zone"))==zones[i]&&field(b,"numberofactions")=="1","RC trigger identity");
    need(field(b,"startenabled")=="TRUE"&&field(b,"onetimeonly")=="FALSE"&&field(b,"randomtriggerpercent")=="100.000000","RC activation authoring");
    const auto* ignore=optional(b,"ignoretrigger");
    need(i>=2?(ignore&&*ignore=="PLAYER"):!ignore,"RC authored/absent PLAYER distinction");
    if(ignore)out.ignoreTrigger=*ignore;
    need(!optional(b,"parent")&&!optional(b,"attachtosoultree"),"RC unexpected attachment");
    out.name=names[i];out.sourceZoneOrdinal=zones[i];out.position=vector(field(b,"position"));
    const bool sphere=true;
    if(sphere){
     need(!optional(b,"box")&&!optional(b,"yaw")&&!optional(b,"pitch")&&!optional(b,"roll"),"RC sphere source shape");
     out.radius=number(field(b,"radius"));need(*out.radius>0,"RC sphere radius");out.shape=Shape::sphere;
    }else{
     need(!optional(b,"radius"),"RC box source shape");out.shape=Shape::box;out.box=vector(field(b,"box"));
     for(const auto extent:out.box->values)need(extent>0,"RC authored box extent");
     out.yaw=number(field(b,"yaw"));out.pitch=number(field(b,"pitch"));out.roll=number(field(b,"roll"));
    }
    need(field(action,"actiontype")=="LostAI"&&integer(field(action,"targetnodeindex"))==targets[i]&&
         targets[i]<source_->ai().count()&&!optional(action,"reset"),"RC original LostAI target or omitted Reset");
    out.targetAiNodeIndex=targets[i];out.startEnabled=true;out.oneTimeOnly=false;out.randomTriggerPercent=100;
    // Keep authoredReset empty. No default is substituted into source fields.
    out.resetDefault1B5A60=false;out.originalTrigger=&b;out.originalAction=&action;
   }
   sources_=std::move(next);decoded_=true;return true;
  }catch(...){d.failure="RC trigger authoring rejected; private prefix retained";return false;}
 }
 std::size_t descriptorPayloadBytes()const noexcept {
  std::size_t n=sizeof(Owner);
  const auto add=[&](std::size_t v){if(v>std::numeric_limits<std::size_t>::max()-n)n=std::numeric_limits<std::size_t>::max();else n+=v;};
  for(const auto& s:sources_){add(s.name.capacity());add(1);if(s.ignoreTrigger){add(s.ignoreTrigger->capacity());add(1);}}
  return n;
 }
 const std::array<Source,count>* sources()const noexcept{return decoded_?&sources_:nullptr;}
};
} // namespace rc_trigger_authoring50LA
