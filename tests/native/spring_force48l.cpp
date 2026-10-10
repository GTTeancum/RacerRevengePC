#include "spring_force48l.h"
#include <array>
#include <cfenv>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
using namespace rr::game;
namespace {
using V=Vector3_48C;
V vec(const float* p){return {{p[0],p[1],p[2]}};}
SpringImpactBody48K body(const float* p){
    SpringImpactBody48K b{};std::memcpy(b.state.scene.v,p,64);
    b.parameters.mass=p[16];b.parameters.inverseMass=p[17];
    b.state.loads={vec(p+18),vec(p+21)};return b;
}
void output(const SpringFrictionResult48J& r,float* p){
    const V* fields[]={&r.worldForce,&r.primary.force,&r.primary.torque,&r.other.force,&r.other.torque};
    for(unsigned i=0;i<5;++i)std::memcpy(p+3*i,fields[i]->v,12);
}
}
int main(int argc,char** argv){
    if(argc!=2)return 2;
    unsigned checks=0,failures=0;
    auto check=[&](bool ok){++checks;if(!ok){++failures;if(failures<15)std::cerr<<"failed check "<<checks<<'\n';}};
    auto same=[&](float a,float b){std::uint32_t x,y;std::memcpy(&x,&a,4);std::memcpy(&y,&b,4);return x==y;};
    std::ifstream in(std::filesystem::path(argv[1])/"evidence/springResponse48L/spring-force-fixtures.bin",std::ios::binary);
    std::uint32_t count=0,width=0;in.read(reinterpret_cast<char*>(&count),4);in.read(reinterpret_cast<char*>(&width),4);
    if(!in||count!=2048||width!=91)return 2;
    for(unsigned row=0;row<count;++row){
        std::array<float,91> f{};in.read(reinterpret_cast<char*>(f.data()),sizeof(f));if(!in)return 2;
        SpringForceParameters48L p{vec(f.data()+8),{vec(f.data()+14),vec(f.data()+11),f[17],f[26]!=0,f[27]!=0},
            f[18],f[19],f[20],f[21],f[22],f[23],f[24]!=0,f[25]!=0};
        auto a=body(f.data()+28),b=body(f.data()+52);SpringFrictionResult48J result{};
        a.localCenter=p.pointForce.primaryLocalCenter;const auto beforeA=a,beforeB=b;
        check(applySpringForce48L(vec(f.data()),vec(f.data()+3),f[6],p,a,f[7]!=0?&b:nullptr,result));
        std::array<float,15> got{};output(result,got.data());
        for(unsigned i=0;i<15;++i){
            const bool equal=same(got[i],f[76+i]);check(equal);
            if(!equal&&failures<15)std::cerr<<"force case "<<row<<" word "<<i<<" got "<<got[i]<<" expected "<<f[76+i]<<'\n';
        }
        check(std::memcmp(&a,&beforeA,sizeof(a))==0);check(std::memcmp(&b,&beforeB,sizeof(b))==0);
    }
    if(in.peek()!=std::ifstream::traits_type::eof())return 2;
    std::ifstream secondary(std::filesystem::path(argv[1])/"evidence/springResponse48L/secondary-fixtures.bin",std::ios::binary);
    secondary.read(reinterpret_cast<char*>(&count),4);secondary.read(reinterpret_cast<char*>(&width),4);
    if(!secondary||count!=512||width!=12)return 2;
    for(unsigned row=0;row<count;++row){
        std::array<float,12> f{};secondary.read(reinterpret_cast<char*>(f.data()),sizeof(f));if(!secondary)return 2;
        V result{};check(removeSpringInwardVelocity48L(vec(f.data()),vec(f.data()+3),vec(f.data()+6),result));
        for(unsigned i=0;i<3;++i)check(same(result.v[i],f[9+i]));
    }
    if(secondary.peek()!=std::ifstream::traits_type::eof())return 2;
    const float nan=std::numeric_limits<float>::quiet_NaN(),huge=std::numeric_limits<float>::max();
    const V normal{{0,1,0}},point{{2,3,4}},zero{},bad{{nan,0,0}};
    SpringForceParameters48L base{};base.contactVelocity120={{0,-2,0}};
    base.pointForce.torqueScale=1;base.maxPenetration90=2;base.maxSpeed94=10;
    base.stiffness9C=8;base.dampingA0=4;base.impactStiffnessA4=16;base.impactDampingA8=2;
    SpringImpactBody48K primary{};for(unsigned i=0;i<4;++i)primary.state.scene.v[5*i]=1;
    primary.parameters.mass=2;primary.parameters.inverseMass=.5f;
    auto other=primary;other.parameters.mass=4;other.parameters.inverseMass=.25f;
    SpringFrictionResult48J result{};result.worldForce={{123,456,789}};const auto saved=result;
    auto rejected=[&](const V& n,const V& q,float d,const SpringForceParameters48L& p,
                      const SpringImpactBody48K& a,const SpringImpactBody48K* b){
        result=saved;check(!applySpringForce48L(n,q,d,p,a,b,result));check(std::memcmp(&result,&saved,sizeof(saved))==0);
    };
    rejected(normal,point,1,base,primary,&primary);
    rejected(bad,point,1,base,primary,nullptr);rejected(normal,bad,1,base,primary,nullptr);
    rejected(normal,point,nan,base,primary,nullptr);
    for(unsigned field=0;field<5;++field){
        auto p=base;float* values[]={&p.contactVelocity120.v[0],&p.maxPenetration90,&p.maxSpeed94,&p.stiffness9C,&p.dampingA0};
        *values[field]=nan;rejected(normal,point,1,p,primary,nullptr);
    }
    auto p=base;p.impactStiffnessA4=nan;p.impactDampingA8=nan;
    check(applySpringForce48L(normal,point,1,p,primary,nullptr,result));
    p.impact54=true;rejected(normal,point,1,p,primary,nullptr);
    p.otherSpring58=true;check(applySpringForce48L(normal,point,1,p,primary,&other,result));
    p=base;p.impact54=true;p.stiffness9C=nan;p.dampingA0=nan;
    check(applySpringForce48L(normal,point,1,p,primary,nullptr,result));
    check(applySpringForce48L(normal,point,1,p,primary,&other,result));
    p.otherSpring58=true;rejected(normal,point,1,p,primary,&other);
    auto a=primary,b=other;a.parameters.mass=nan;a.parameters.inverseMass=nan;
    check(applySpringForce48L(normal,point,1,base,a,nullptr,result));
    rejected(normal,point,1,base,a,&b);a=primary;b.parameters.mass=nan;rejected(normal,point,1,base,a,&b);
    b=other;a.parameters.inverseMass=nan;check(applySpringForce48L(normal,point,1,base,a,&b,result));
    b.parameters.inverseMass=nan;rejected(normal,point,1,base,a,&b);
    a=primary;b=other;a.parameters.mass=8;b.parameters.inverseMass=nan;
    check(applySpringForce48L(normal,point,1,base,a,&b,result));
    a.parameters.inverseMass=nan;rejected(normal,point,1,base,a,&b);
    a=primary;b=other;b.parameters.mass=a.parameters.mass;a.parameters.inverseMass=nan;
    check(applySpringForce48L(normal,point,1,base,a,&b,result));
    b.parameters.inverseMass=nan;rejected(normal,point,1,base,a,&b);
    // All unrelated body state, inertia and centers are genuinely unconsumed.
    a=primary;a.state.body.velocity=bad;a.state.body.angularVelocity=bad;a.state.body.linearAcceleration=bad;
    a.parameters.inverseInertiaColumns[0]=bad;a.localCenter=bad;
    check(applySpringForce48L(normal,point,1,base,a,nullptr,result));
    // Point-force sentinel does not inspect scenes, centers, or torque scale.
    a.state.scene.v[0]=nan;b=a;p=base;p.pointForce.torqueScale=nan;
    p.pointForce.primaryLocalCenter=bad;p.pointForce.customLocalCenter=bad;
    check(applySpringForce48L(normal,zero,1,p,a,&b,result));
    rejected(normal,point,1,p,a,&b);
    p=base;p.pointForce.customLocalCenter=bad;
    check(applySpringForce48L(normal,point,1,p,primary,nullptr,result));
    p.pointForce.useCustomCenter=true;rejected(normal,point,1,p,primary,nullptr);
    p=base;p.pointForce.useCustomCenter=true;p.pointForce.primaryLocalCenter=bad;
    check(applySpringForce48L(normal,point,1,p,primary,nullptr,result));
    // Failure in the opposite body's accumulation never commits the primary.
    b=other;b.state.loads.force=bad;rejected(normal,point,1,base,primary,&b);
    b=other;b.state.loads.torque=bad;rejected(normal,point,1,base,primary,&b);
    b=other;b.state.scene.v[0]=nan;rejected(normal,point,1,base,primary,&b);
    a=primary;a.state.loads.force=bad;rejected(normal,point,1,base,a,nullptr);
    a=primary;a.state.loads.torque=bad;rejected(normal,point,1,base,a,nullptr);
    // Reject overflow even if a later speed/force clamp would hide it.
    p=base;p.contactVelocity120={{0,huge,0}};rejected(V{{0,2,0}},zero,1,p,primary,nullptr);
    p=base;p.stiffness9C=huge;rejected(normal,zero,2,p,primary,nullptr);
    p=base;p.dampingA0=huge;rejected(normal,zero,1,p,primary,nullptr);
    p=base;p.contactVelocity120={{0,0,0}};p.stiffness9C=huge;p.maxPenetration90=1;
    rejected(V{{0,2,0}},zero,1,p,primary,nullptr);
    a=primary;b=other;a.parameters.mass=huge;b.parameters.mass=huge;b.parameters.inverseMass=2;
    p=base;p.stiffness9C=0;p.dampingA0=0;rejected(normal,zero,1,p,a,&b);
    V velocity{{1,-8,3}},out{{123,456,789}};const auto old=out;
    check(removeSpringInwardVelocity48L(normal,V{{0,-2,0}},velocity,out));check(out.v[1]==-6);
    auto secondaryRejected=[&](const V& n,const V& c,const V& v){
        out=old;check(!removeSpringInwardVelocity48L(n,c,v,out));check(std::memcmp(&out,&old,sizeof(out))==0);
    };
    secondaryRejected(bad,zero,zero);secondaryRejected(normal,bad,zero);secondaryRejected(normal,zero,bad);
    secondaryRejected(V{{0,2,0}},V{{0,huge,0}},zero);
    secondaryRejected(V{{0,2,0}},V{{0,-huge/2,0}},zero);
    secondaryRejected(normal,V{{0,-huge,0}},V{{0,huge,0}});
    velocity={{1,-8,3}};check(removeSpringInwardVelocity48L(normal,V{{0,-2,0}},velocity,velocity));check(velocity.v[1]==-6);
    // The helper never changes the caller's rounding mode.
    const int originalRounding=std::fegetround();
    if(std::fesetround(FE_DOWNWARD)==0){
        check(removeSpringInwardVelocity48L(normal,zero,zero,out));check(std::fegetround()==FE_DOWNWARD);
        check(applySpringForce48L(normal,zero,1,base,primary,nullptr,result));check(std::fegetround()==FE_DOWNWARD);
        check(std::fesetround(originalRounding)==0);
    }else check(false);
    std::cout<<"spring48L force/secondary "<<checks<<" checks, "<<failures<<" failures\n";return failures?1:0;
}
