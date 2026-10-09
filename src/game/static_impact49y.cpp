#include "static_impact49y.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
struct Math {
    bool valid=true;
    float read(float x) noexcept {
        std::uint32_t b;std::memcpy(&b,&x,4);
        // The admitted ordinary domain excludes denormal consumed operands and
        // intermediates. PCSX2 SQRT_S and VU vuDouble flush those differently
        // from host IEEE; a finite-only check would not establish equivalence.
        valid=valid&&std::isfinite(x)&&((b&0x7f800000u)!=0||(b&0x7fffffffu)==0);
        return x;
    }
    float add(float x,float y) noexcept {read(x);read(y);volatile float z=x+y;return read(z);}
    float sub(float x,float y) noexcept {read(x);read(y);volatile float z=x-y;return read(z);}
    float mul(float x,float y) noexcept {read(x);read(y);volatile float z=x*y;return read(z);}
    float sqrt(float x) noexcept {read(x);volatile float z=static_cast<float>(std::sqrt(static_cast<double>(x)));return read(z);}
    float reciprocal(float x) noexcept {read(x);volatile float z=1.0f/x;return read(z);}
    float maximum(float x,float y) noexcept {read(x);read(y);return detail::scalarMax48O(x,y);}
};
float maxFinite() noexcept {const std::uint32_t bits=0x7f7fffffu;float x;std::memcpy(&x,&bits,4);return x;}
void inverseDirection(const BodyMatrix48G& m,const float* in,float* out,Math& f) noexcept {
    BodyMatrix48G t=m;
    //28BC60..28BD30: preserve original transpose ADDs and unused translation
    // arithmetic; no general inverse or orthonormality repair is substituted.
    for(unsigned i=0;i<12;++i)t.v[i]=f.add(m.v[i],0.0f);
    t.v[4]=f.add(0.0f,m.v[1]);t.v[8]=f.add(0.0f,m.v[2]);
    t.v[9]=f.add(0.0f,m.v[6]);t.v[1]=f.add(0.0f,m.v[4]);
    t.v[2]=f.add(0.0f,m.v[8]);t.v[6]=f.add(0.0f,m.v[9]);
    for(unsigned i=12;i<16;++i)t.v[i]=f.add(m.v[i],0.0f);
    for(unsigned i=0;i<3;++i){float a=f.mul(t.v[i],m.v[12]);a=f.add(a,f.mul(t.v[4+i],m.v[13]));a=f.add(a,f.mul(t.v[8+i],m.v[14]));t.v[12+i]=f.mul(a,-1.0f);}
    for(unsigned i=0;i<3;++i){float a=f.mul(t.v[i],in[0]);a=f.add(a,f.mul(t.v[4+i],in[1]));a=f.add(a,f.mul(t.v[8+i],in[2]));out[i]=f.add(a,0.0f);}
}
}
StaticImpactStatus49Y recordStaticImpact49Y(const StaticImpactInput49Y& in,
    const StaticImpactState49Y& state,StaticImpactState49Y& out,float& nextW) noexcept {
    using S=StaticImpactStatus49Y;
    if((in.flagsCD8&1u)||in.vehicle40==3||in.vehicle40==4){out=state;nextW=in.priorVf9W;return S::skipped;}
    if(in.vehicle58>1u||in.bodyContactPartnerCount20C!=0||!(in.sceneFlags190&1u))return S::unsupported;
    const bool ai=in.vehicle58==1u;
    if(ai&&(!in.aiOwnersBound||(!in.vehicleCFC&&!in.tuning.aiEnvironmentMultiplierBound)))return S::unsupported;
    if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    Math f;
    //223078..22309C scalar world magnitude; no vector/hypot replacement.
    const float yy=f.mul(in.accumulatedImpact1C0[1],in.accumulatedImpact1C0[1]);
    const float xx=f.mul(in.accumulatedImpact1C0[0],in.accumulatedImpact1C0[0]);
    const float magnitude=f.sqrt(f.add(f.add(xx,yy),f.mul(in.accumulatedImpact1C0[2],in.accumulatedImpact1C0[2])));
    float local[3];inverseDirection(in.readyMainScene190,in.accumulatedImpact1C0,local,f);
    const float w=f.add(in.priorVf9W,1.0f);
    //2230D4..F8: dotXYZ, original VRSQRT zero result, VMULq.xyz.
    const float x2=f.mul(local[0],local[0]),y2=f.mul(local[1],local[1]),z2=f.mul(local[2],local[2]);
    const float dot=f.add(f.add(x2,y2),f.mul(1.0f,z2));
    const float q=dot==0.0f?maxFinite():f.reciprocal(f.sqrt(dot));
    float normal[3];for(unsigned i=0;i<3;++i)normal[i]=f.mul(local[i],q);
    //222798..22281C first reached threshold; unsorted authored values allowed.
    float chosen=0.0f;unsigned thresholdIndex=5;
    for(unsigned i=0;i<5;++i){if(!(magnitude<f.read(in.tuning.thresholds00[i]))){chosen=f.read(in.tuning.severities14[i]);thresholdIndex=i;break;}}
    //2227AC/2227C8/2227E4/222800 branch straight to22286C. The weaker
    // XS match alone falls through222820; even no match bypasses this gate.
    if(ai&&thresholdIndex==4u&&in.partnerState){
        const float first=f.read(in.damage7D0),second=f.read(in.damage7D4);
        const float pair=f.maximum(second,first);
        const float third=f.read(in.damage7DC);
        //223508 retains the source MAX.S third,third before the final MAX.
        if(f.maximum(pair,f.maximum(third,third))>0.9f)chosen=0.0f;
    }
    //22286C..22289C. This multiply also executes for zero chosen severity.
    if(ai&&!in.vehicleCFC)chosen=f.mul(chosen,in.tuning.aiEnvironmentMultiplier60);
    const float severity=f.mul(chosen,f.sub(1.0f,in.reductionCF0));
    StaticImpactState49Y s=state;
    auto raise=[&](unsigned index,float value){if(f.read(s.directionalD08[index])<value)s.directionalD08[index]=value;};
    //22313C..2231A0 unscaled lateral maxima always run, even severity<=0.1.
    if(local[0]<-0.1f)raise(7,-local[0]);
    else if(local[0]>0.1f)raise(6,local[0]);
    if(severity>0.1f){
        const float pz=f.mul(severity,normal[2]),py=f.mul(severity,normal[1]),px=f.mul(severity,normal[0]);
        if(local[0]<-0.1f)raise(0,-px);else if(local[0]>0.1f)raise(1,px);
        if(local[1]<-0.1f)raise(3,-py);else if(local[1]>0.1f)raise(2,py);
        if(local[2]<-0.1f&&normal[2]<-0.75f){
            //22334C compares the old maximum before the AI-specific gate.
            const float rear=-pz;
            if(f.read(s.directionalD08[4])<rear&&
                (!ai||!in.vehicleCFC||normal[2]<-0.85f))s.directionalD08[4]=rear;
        }
        else if(local[2]>0.1f)raise(5,pz);
    }
    //2233F0..2234C4 independent absolute-coordinate selection; ties keep bits.
    for(unsigned i=0;i<3;++i)if(std::fabs(f.read(in.accumulatedImpact1C0[i]))>std::fabs(f.read(s.impactBA0[i])))s.impactBA0[i]=in.accumulatedImpact1C0[i];
    if(!f.valid)return S::arithmetic;
    out=s;nextW=w;return S::applied;
}
}
