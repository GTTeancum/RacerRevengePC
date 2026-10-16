#pragma once
#include "ga_trigger_authoring_fixture.h"
#include "../../src/game/native_trigger_box_geometry50ma.h"
#include <array>
#include <cmath>
#include <filesystem>
#include <string>

// Exact MA.RES/ma.tob admission. Runtime decoding is authored here but was
// not executed during this source-only recovery. No synthetic actor placement.
namespace ma_trigger_authoring {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t count=13,sphereCount=3,boxCount=10;
inline constexpr std::size_t archiveBytes=3691592,decodedBytes=15532032,tobBytes=32691;
inline constexpr std::uint32_t archiveEntries=485;
inline constexpr const char* archiveSha="2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228";
inline constexpr const char* tobSha="4a3bcecce43a53b3ec3ac28df9c9672a61e825a2987c11bbd0ab055b29f7da55";
struct Source {
 std::string name;
 std::uint32_t zone=0,actionCount=1,shapeType=5,targetNodeIndex=0;
 g::CollisionSphere48J sphere{}; // shape5 primitive; shape6 coarse bound only
 g::NativeTriggerBox50MA box{};
 float yaw=0,pitch=0,roll=0;
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=true,reset=false;
 std::int32_t randomTriggerPercent=100;
};
namespace detail {
using namespace ga_trigger_authoring::detail;
inline g::BodyMatrix48G boxWorld(const g::Vector3_48C& position,float yaw,float pitch,float roll){
 // 2DBF98..C4 uses binary32 degree factor0x3c8efa34;157100 forms look/up
 // by rollZ then pitchX then yawY, with its original handedness. Native std
 // trig/normalization below deliberately does not claim13BFE0 or VU parity.
 constexpr float radians=0x1.1df468p-6f;
 const double y=double(yaw*radians),p=double(pitch*radians),r=double(roll*radians);
 const double sy=std::sin(y),cy=std::cos(y),sp=std::sin(p),cp=std::cos(p),sr=std::sin(r),cr=std::cos(r);
 using namespace g::native_gate_geometry50kg_detail;
 V look{cp*sy,sp,cp*cy},up{sr*cy-cr*sp*sy,cr*cp,-cr*sp*cy-sr*sy};
 look=scale(look,1.0/std::sqrt(dot(look,look)));up=scale(up,1.0/std::sqrt(dot(up,up)));
 const V right=cross(up,look);const V axes[3]={right,up,look};g::BodyMatrix48G out{};
 for(unsigned c=0;c<3;++c){out.v[c*4]=static_cast<float>(axes[c].x);out.v[c*4+1]=static_cast<float>(axes[c].y);out.v[c*4+2]=static_cast<float>(axes[c].z);out.v[12+c]=position.v[c];}
 out.v[15]=1.f;return out;
}
inline float boxRadius(const g::NativeTriggerBox50MA& box){
 // Conservative native coarse sphere from all8 actual transformed corners.
 // It cannot substitute for oriented-box narrowphase.
 double maximum=0;
 for(unsigned bits=0;bits<8;++bits){double p[3]{};
  for(unsigned a=0;a<3;++a)for(unsigned c=0;c<3;++c)p[a]+=double(box.world.v[c*4+a])*box.half.v[c]*((bits&(1u<<c))?1.0:-1.0);
  maximum=std::max(maximum,p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
 }
 return std::nextafter(static_cast<float>(std::sqrt(maximum)),std::numeric_limits<float>::infinity());
}
}
class Owner final {
 std::array<Source,count> sources_{};std::vector<std::uint8_t> originalTob_;bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool load(const std::filesystem::path& archivePath,std::uint32_t aiNodeCount,
           std::uint32_t zoneCount,std::string& error){
  if(attempted_){error="MA trigger owner requires a fresh stable lifetime";return false;}attempted_=true;
  try{using namespace detail;namespace f=pod_materials50ed_fixture::detail;
   f::Inputs input;input.capacity=archiveBytes;const auto bytes=input.read(archivePath,archiveBytes);
   need(bytes.size()==archiveBytes&&f::sha256(bytes)==archiveSha,"MA trigger original archive identity");
   a::ResArchive48C archive;a::DecodeError48C why{};std::vector<std::uint8_t> tob;
   a::DecodeLimits48C limits{};limits.resource_file_bytes=archiveBytes;
   limits.resource_decoded_bytes=decodedBytes;limits.resource_entries=archiveEntries;
   need(a::decodeResource48C(bytes,archive,why,nullptr,limits)&&archive.entries.size()==archiveEntries&&archive.decoded.size()==decodedBytes&&
     a::extractResMember48C(archive,"tracks\\ma.tob",tob,why,tobBytes),"MA trigger original TOB missing");
   need(tob.size()==tobBytes&&f::sha256(tob)==tobSha,"MA trigger original TOB identity");
   a::ParameterDocument48F p;need(a::decodeParameters48F(tob,p,why),"MA trigger original TOB rejected");
   need(p.blocks.size()==188&&integer(field(block(p,"triggers"),"numberoftriggers"))==count,"MA original trigger count changed");
   constexpr std::uint32_t targets[count]={113,113,182,158,177,178,179,162,191,184,187,187,196};
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){auto& s=next[i];const auto name="trigger"+std::to_string(i+1);const auto& b=block(p,name);
    s.name=field(b,"name");s.zone=integer(field(b,"zone"));s.actionCount=integer(field(b,"numberofactions"));
    s.sphere.center=vector(field(b,"position"));
    need(s.zone==(i<2?5u:2u)&&s.zone<=zoneCount&&s.actionCount==1,"MA trigger original zone/action count changed");
    s.startEnabled=boolean(field(b,"startenabled"));s.oneTimeOnly=boolean(field(b,"onetimeonly"));
    need(s.startEnabled&&!s.oneTimeOnly&&field(b,"ignoretrigger")=="PLAYER"&&number(field(b,"randomtriggerpercent"))==100.f,
      "MA trigger original activation/ignore/RNG policy changed");
    need(!optional(b,"parent")&&!optional(b,"attachtosoultree"),"MA trigger attachment not admitted");
    if(i<sphereCount){
     need(!optional(b,"box")&&!optional(b,"yaw")&&!optional(b,"pitch")&&!optional(b,"roll"),"MA sphere source changed");
     s.sphere.radius=number(field(b,"radius"));need(s.sphere.radius>0.f,"MA sphere radius rejected");
    }else{
     need(!optional(b,"radius"),"MA box radius route changed");s.shapeType=6;
     s.box.half=vector(field(b,"box"));s.yaw=number(field(b,"yaw"));s.pitch=number(field(b,"pitch"));s.roll=number(field(b,"roll"));
     s.box.world=boxWorld(s.sphere.center,s.yaw,s.pitch,s.roll);
     need(g::validNativeTriggerBox50MA(s.box),"MA box geometry rejected");s.sphere.radius=boxRadius(s.box);
     need(std::isfinite(s.sphere.radius)&&s.sphere.radius>0.f,"MA box coarse radius rejected");
    }
    const auto& action=block(p,name+"action1");
    need(field(action,"actiontype")=="LostAI"&&!optional(action,"reset"),"MA original LostAI route changed");
    s.targetNodeIndex=integer(field(action,"targetnodeindex"));
    need(s.targetNodeIndex==targets[i]&&s.targetNodeIndex<aiNodeCount,"MA original LostAI target rejected");
   }
   sources_=std::move(next);originalTob_=std::move(tob);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 const std::vector<std::uint8_t>& originalTob()const {
  if(!ready_)throw std::runtime_error("MA original TOB owner not ready");return originalTob_;
 }
 bool ready()const noexcept{return ready_;}
 const Source& source(std::size_t i)const{return sources_.at(i);}
};
} // namespace ma_trigger_authoring
