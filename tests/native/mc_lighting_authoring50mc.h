#pragma once
// Exact MC cold authoring and registration-row projection only.
// No live light registration, render binding, flare resource or camera join.
#include "mc_world_semantics50mc.h"
#include "../../src/assets/original_light_pose_defaults_candidate.h"
#include <cfenv>
#include <memory>
#include <optional>
#include <thread>
namespace mc_lighting_authoring50MC {
namespace a=rr::assets;
namespace semantics=mc_world_semantics50MC;
namespace pose=a::original_light_pose_defaults_candidate;
using Vec3=std::array<float,3>;
using Vec4=std::array<float,4>;
enum class Type:std::uint32_t {directional=2,ambient=4};
struct Directional {
 pose::FreshPose pose;
 Vec3 attenuation{};
 float range=0.f,innerConeDegrees=30.f,outerConeDegrees=45.f;
 bool placeAtInfinity=true,castsShadows=true,emitsLight=true;
 bool hasLensFlare=false,hasLightFlare=false,hasFlasher=false;
 Vec3 lensFlareRgb{};float lensFlareBrightness=1.f;
 std::array<bool,5> lensFlareLayers{};
 std::string lensFlareTexture;
 // Presence bits distinguish read defaults from original authored values.
 bool attenuationAuthored=false,rangeAuthored=false,innerConeAuthored=false,outerConeAuthored=false;
 bool flareTextureAuthored=false;
};
struct Record {
 std::uint32_t ordinal=0,parameterBlock=0,initialZone=0;
 Type type=Type::ambient;bool zoneAuthored=false;
 Vec3 authoredRgb{},initializedRgb{};
 // Ambient reader returns at277514 before pose/attenuation/flag reads.
 // Null here is a dispatch distinction, not a zero light direction.
 std::optional<Directional> directional;
};
struct Snapshot {
 a::World48F world;
 std::array<Record,29> records{};
 std::array<Vec4,28> ambient60{},sun44{};
 Vec4 globalAmbient50{},authoredSunSpecular{};
 Vec3 sunLook{};
 std::array<std::size_t,28> overrideValueOffsets{};
 // These are prepared cold rows only; nothing is registered into gameplay.
};
struct Diagnostic {
 a::DecodeError48C decode{};pose::Diagnostic pose{};
 std::uint32_t sourcePC=0;std::string message;
};
namespace detail {
inline float half(float x)noexcept{volatile float y=x*.5f;return y;}
inline float word(std::uint32_t x)noexcept{float y;std::memcpy(&y,&x,4);return y;}
inline Vec3 rgb(const a::ParameterBlock48F& b,const char* key){
 const auto v=semantics::detail::vector(semantics::detail::field(b,key));Vec3 out{};
 for(unsigned c=0;c<3;++c){semantics::detail::malformed(v[c]>=0&&v[c]<=255,"mc.lights.rgb",b.offset,"Color outside authenticated domain");out[c]=static_cast<float>(v[c]);}return out;
}
inline bool has(const a::ParameterBlock48F& b,const char* key){return semantics::detail::optionalField(b,key)!=nullptr;}
inline bool flag(const a::ParameterBlock48F& b,const char* key,bool fallback){
 const auto* f=semantics::detail::optionalField(b,key);if(!f)return fallback;
 semantics::detail::malformed(f->value=="T"||f->value=="F","mc.lights.flag",f->value_offset,"Only exact MC flag spellings admitted");return f->value=="T";
}
inline float scalar(const a::ParameterBlock48F& b,const char* key,float fallback){
 const auto* f=semantics::detail::optionalField(b,key);return f?static_cast<float>(semantics::detail::decimal(f->value,f->value_offset)):fallback;
}
}
class Owner final {
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false;std::unique_ptr<Snapshot> snapshot_;
 void requireThread()const noexcept{if(thread_!=std::this_thread::get_id())std::terminate();}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){requireThread();}
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&snapshot_!=nullptr;}
 const Snapshot* source()const noexcept{return ready()?snapshot_.get():nullptr;}
 bool prepare(const a::World48F& world,Diagnostic& d){
  requireThread();d={};if(attempted_){d.message="MC lighting preparation is one-attempt";return false;}attempted_=true;
  if(std::fegetround()!=FE_TONEAREST){d.message="Round-to-nearest arithmetic required";return false;}
  if(!semantics::validate(world,d.decode)){d.message="MC source World authentication/semantics failed";return false;}
  try {
   using namespace semantics::detail;
   auto n=std::make_unique<Snapshot>();n->world=world;
   const auto& p=n->world.parameters;
   const auto& lights=p.blocks[findBlock(p,"lights")];const auto& environment=p.blocks[findBlock(p,"environment")];
   malformed(number(field(lights,"numberoflights"),29)==29,"mc.lights.count",lights.offset,"Exact MC light count required");
   malformed(!detail::has(environment,"ambientcolorflashminidletime"),"mc.lights.flash",environment.offset,"Unexpected ambient flash authority");
   for(std::uint32_t ordinal=1;ordinal<=29;++ordinal){
    auto& r=n->records[ordinal-1];r.ordinal=ordinal;r.parameterBlock=findBlock(p,"light"+std::to_string(ordinal));const auto& b=p.blocks[r.parameterBlock];
    r.authoredRgb=detail::rgb(b,"colorrgb");for(unsigned c=0;c<3;++c)r.initializedRgb[c]=detail::half(r.authoredRgb[c]);
    const auto* z=optionalField(b,"initialzone");r.zoneAuthored=z!=nullptr;r.initialZone=z?number(*z,28):1u;
    malformed(r.initialZone>=1&&r.initialZone<=28,"mc.lights.zone",b.offset,"Light zone out of range");
    if(ordinal<=28){
     malformed(field(b,"type").value=="Ambient"&&r.initialZone==ordinal&&r.zoneAuthored,"mc.lights.ambient",b.offset,"MC ambient order/zone differs");
     r.type=Type::ambient;
     const Vec4 row{{r.initializedRgb[0],r.initializedRgb[1],r.initializedRgb[2],64.f}};
     n->globalAmbient50=row;
     if(r.initialZone==1)for(auto& dest:n->ambient60)dest=row;
     else n->ambient60[r.initialZone-1]=row;
     continue;
    }
    malformed(field(b,"type").value=="Directional"&&r.initialZone==1&&!r.zoneAuthored,"mc.lights.directional",b.offset,"MC sun dispatch differs");
    r.type=Type::directional;r.directional.emplace();auto& sun=*r.directional;
    if(!pose::readFreshPose(b,28,sun.pose,d.pose)){d.sourcePC=d.pose.sourcePC;d.message="MC directional pose failed";return false;}
    malformed(sun.pose.positionAuthored&&!sun.pose.zoneAuthored&&sun.pose.lookSource==pose::LookSource::targetPosition&&sun.pose.lookWasNormalized,"mc.lights.pose",b.offset,"MC sun must use authored target minus position exactly once");
    sun.attenuationAuthored=detail::has(b,"attenuation");
    sun.attenuation={{detail::word(0x3cf5c28f),detail::word(0x3b03126f),detail::word(0x3a83126f)}};
    if(sun.attenuationAuthored){const auto v=vector(field(b,"attenuation"));for(unsigned c=0;c<3;++c)sun.attenuation[c]=static_cast<float>(v[c]);}
    sun.rangeAuthored=detail::has(b,"range");sun.range=detail::scalar(b,"range",0.f);
    sun.innerConeAuthored=detail::has(b,"innerconeangle");sun.innerConeDegrees=detail::scalar(b,"innerconeangle",30.f);
    sun.outerConeAuthored=detail::has(b,"outerconeangle");sun.outerConeDegrees=detail::scalar(b,"outerconeangle",45.f);
    sun.placeAtInfinity=detail::flag(b,"placelightatinfinity",true);sun.castsShadows=detail::flag(b,"castsshadows",true);sun.emitsLight=detail::flag(b,"emitslight",true);
    sun.hasLensFlare=detail::flag(b,"haslensflare",false);sun.hasLightFlare=detail::flag(b,"haslightflare",false);sun.hasFlasher=detail::flag(b,"hasflasher",false);
    malformed(sun.placeAtInfinity&&sun.castsShadows&&sun.emitsLight&&sun.hasLensFlare&&!sun.hasLightFlare&&!sun.hasFlasher,"mc.lights.flags",b.offset,"MC sun flags differ");
    sun.lensFlareRgb=detail::rgb(b,"lensflarecolorrgb");sun.lensFlareBrightness=detail::scalar(b,"lensflarebrightness",1.f);
    const auto& layers=field(b,"lensflarelayersvisible");malformed(layers.value=="T,T,T,T,T","mc.lights.layers",layers.value_offset,"Exact five authored layers required");sun.lensFlareLayers.fill(true);
    const auto* texture=optionalField(b,"lensflaretexturemapname");sun.flareTextureAuthored=texture!=nullptr;sun.lensFlareTexture=texture?texture->value:"none";
    n->sunLook=sun.pose.effectiveLook78;
    n->authoredSunSpecular={{r.initializedRgb[0],r.initializedRgb[1],r.initializedRgb[2],0.f}};
    for(auto& dest:n->sun44)dest=n->authoredSunSpecular;
   }
   // Source setters1498A0..1498F0 copy RGB directly, leaving W unchanged.
   for(std::uint32_t z=1;z<=28;++z){const auto key="directionallightcolorforzone"+std::to_string(z);const auto& f=field(lights,key);
    const auto v=detail::rgb(lights,key.c_str());for(unsigned c=0;c<3;++c)n->sun44[z-1][c]=v[c];n->overrideValueOffsets[z-1]=f.value_offset;
   }
   snapshot_=std::move(n);return true;
  }catch(const semantics::detail::Failure& f){d.decode=f.error;d.message="MC lighting source field rejected";}
   catch(const std::bad_alloc&){d.message="MC lighting allocation failed";}
  return false;
 }
};
}
