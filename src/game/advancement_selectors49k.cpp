#include "advancement_selectors49k.h"
#include "n64_airborne_gravity50ag.h"
#include <cmath>
#include <cfenv>
#include <cstring>
#include <initializer_list>

namespace rr::game { namespace {
float fp(std::uint32_t b) noexcept { float f;std::memcpy(&f,&b,4);return f; }
struct Math {
    bool valid=true;
    float keep(float f) noexcept { valid=valid&&std::isfinite(f);return f; }
    float add(float a,float b) noexcept { volatile float f=a+b;return keep(f); }
    float sub(float a,float b) noexcept { volatile float f=a-b;return keep(f); }
    float mul(float a,float b) noexcept { volatile float f=a*b;return keep(f); }
    float div(float a,float b) noexcept { volatile float f=a/b;return keep(f); }
};
bool finite3(const Vector3_48C& v) noexcept {for(float f:v.v)if(!std::isfinite(f))return false;return true;}
}
bool selectSchedule49K(float factor,float scale,Schedule49K& out) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    if(!std::isfinite(factor)||!std::isfinite(scale)||factor<0.0f||factor>fp(0x3d75c28fu)||scale<0.0f)return false;
    Math f;const float product=f.mul(factor,scale);
    if(!f.valid||product>=4.0f)return false;
    const std::uint32_t count=static_cast<std::uint32_t>(product)+1u;
    const float each=f.div(factor,static_cast<float>(count));
    if(!f.valid)return false;
    out={count,each};return true;
}
bool selectGravity49K(const GravitySelection49K& s,float& out) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    for(float x:{s.brake350_EC,s.cachedSpeed340_34,s.sceneUpY,s.vehicle378,s.vehicle37C,s.effective434730,s.effective434738})
        if(!std::isfinite(x))return false;
    float value=0.0f;Math f;
    // PORT EXTENSION (n64_airborne_gravity50ag.h): N64 airborne gravity when the
    // course authors it; otherwise the pod's own +37C (stock, unchanged).
    if(!s.incoming348_104)value=n64AirborneGravityOr50AG(s.vehicle37C);
    else if(!(s.brake350_EC>0.0f&&s.cachedSpeed340_34<fp(0x4212aa7fu))) {
        if(s.effective434738!=0.0f) {
            const float f2=f.sub(s.sceneUpY,s.effective434738);
            const float f0=f.sub(0.0f,s.effective434738);
            const float f1=f.mul(f2,f.sub(s.effective434730,s.vehicle378));
            value=f.add(s.vehicle378,f.div(f1,f0));
            if(s.effective434730<=s.vehicle378) {
                if(value<s.effective434730)value=s.effective434730;
                else if(!(value<=s.vehicle378))value=s.vehicle378;
            } else {
                if(!(value<=s.effective434730))value=s.effective434730;
                else if(value<s.vehicle378)value=s.vehicle378;
            }
        }
    }
    if(!f.valid)return false;
    out=value;return true;
}
EngineFlags49K selectEngineFlags49K(std::uint32_t state,std::uint8_t flags) noexcept {
    return {static_cast<std::uint32_t>(state==1u||state==2u),static_cast<std::uint32_t>((flags>>1u)&1u)};
}
bool prepareDisabledProbe49K(const DisabledProbeState49K& old,const float (&local)[4],
    const BodyMatrix48G& scene,const float (&clear)[4],DisabledProbeState49K& out) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    for(float x:local)if(!std::isfinite(x))return false;
    for(float x:scene.v)if(!std::isfinite(x))return false;
    for(float x:clear)if(!std::isfinite(x))return false;
    DisabledProbeState49K s=old;Math f;
    s.field108=0;s.field110=0;s.field114=0;s.field118=0;s.field1E8=0;s.field12C=0;
    for(unsigned i=0;i<4;++i)s.vector1D0[i]=clear[i];
    // VU MULA.x / MADDA.y / MADD.z use local X, Y, Z respectively;
    // every multiply and accumulation is rounded separately, then translation.
    for(unsigned i=0;i<3;++i) {
        const float a=f.mul(scene.v[i],local[0]);
        const float b=f.add(a,f.mul(scene.v[4+i],local[1]));
        const float c=f.add(b,f.mul(scene.v[8+i],local[2]));
        s.point150[i]=f.add(c,scene.v[12+i]);
    }
    s.point150[3]=1.0f;
    if(!f.valid)return false;
    out=s;return true;
}
bool respondDisabledProbe49K(const DisabledProbeState49K& old,float angle,float component,float live,
    const Vector3_48C& axis,const Vector3_48C& torque,DisabledProbeResponse49K& out) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    if(old.field110||old.field114||!std::isfinite(angle)||!std::isfinite(component)||!std::isfinite(live)||!finite3(axis)||!finite3(torque))return false;
    DisabledProbeResponse49K r{old,torque};Math f;
    r.state.field104=0;
    if(r.state.field11C)r.state.field11C=0;
    r.state.field10C=1;
    const float f4=f.mul(live,component),f3=f.mul(angle,f4);
    const float y=f.mul(f3,axis.v[1]);
    const float x=f.mul(f3,axis.v[0]);
    const float z=f.mul(f3,axis.v[2]);
    r.torque190.v[0]=f.add(torque.v[0],x);
    r.torque190.v[1]=f.add(torque.v[1],y);
    r.torque190.v[2]=f.add(torque.v[2],z);
    if(!f.valid)return false;
    out=r;return true;
}
void clearMainBodyPost20C49K(std::uint32_t& state) noexcept { state=0; }
std::uint32_t selectVehicle38049K(std::uint32_t value) noexcept { return value!=0u?1u:0u; }
LateFeedback49K selectLateFeedback49K(std::uint32_t probe,std::uint32_t c3C,std::uint32_t c44) noexcept {
    return {probe,probe,c3C,c44};
}
}
