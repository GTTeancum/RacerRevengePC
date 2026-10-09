#include "spring_impulse48k.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
using namespace rr::game;
namespace {
Vector3_48C vec(const float* f){return {{f[0],f[1],f[2]}};}
SpringImpactBody48K body(const float* f){
    SpringImpactBody48K b{};std::memcpy(b.state.scene.v,f,64);b.localCenter=vec(f+16);
    b.parameters.mass=f[19];b.parameters.inverseMass=f[20];
    for(unsigned i=0;i<3;++i)b.parameters.inverseInertiaColumns[i]=vec(f+21+3*i);
    b.state.body.velocity=vec(f+30);b.state.body.angularVelocity=vec(f+33);
    b.state.body.linearAcceleration=vec(f+36);b.state.body.angularAcceleration=vec(f+39);
    b.state.loads={vec(f+42),vec(f+45)};b.state.positionCorrection1a0=vec(f+48);b.state.rotationCorrection1b0=vec(f+51);
    b.state.accumulator1c0=vec(f+54);b.state.accumulator1d0=vec(f+57);return b;
}
void outputs(const RootBodyState48H& b,float* out){
    const Vector3_48C* fields[]={&b.body.velocity,&b.body.angularVelocity,&b.loads.force,&b.loads.torque,
        &b.positionCorrection1a0,&b.rotationCorrection1b0,&b.accumulator1c0,&b.accumulator1d0};
    for(unsigned i=0;i<8;++i)std::memcpy(out+i*3,fields[i]->v,12);
}
}
int main(int argc,char** argv){
    if(argc!=2)return 2;
    std::ifstream in(std::filesystem::path(argv[1])/"evidence/spring48K/impact-fixtures.bin",std::ios::binary);
    std::uint32_t count=0,width=0;in.read(reinterpret_cast<char*>(&count),4);in.read(reinterpret_cast<char*>(&width),4);
    if(!in||count!=1024||width!=197)return 2;
    unsigned checks=0,failures=0;auto check=[&](bool ok){++checks;if(!ok)++failures;};
    for(unsigned row=0;row<count;++row){
        std::array<float,197> f{};in.read(reinterpret_cast<char*>(f.data()),sizeof(f));if(!in)return 2;
        const auto* p=f.data()+8;
        SpringImpactParameters48K parameters{vec(p),vec(p+3),vec(p+6),p[9],p[10],p[11],p[12],p[13],p[14],p[15],p[16],p[17]!=0,p[18]!=0,p[19]!=0,p[20]!=0};
        auto a=body(f.data()+29),b=body(f.data()+89);SpringImpactResult48K result{};
        check(applySpringImpact48K(vec(f.data()),vec(f.data()+3),f[6],parameters,a,f[7]!=0?&b:nullptr,result));
        std::array<float,48> got{};outputs(result.primary,got.data());outputs(result.other,got.data()+24);
        for(unsigned i=0;i<48;++i){
            std::uint32_t actual,expected;std::memcpy(&actual,&got[i],4);std::memcpy(&expected,&f[149+i],4);check(actual==expected);
            if(actual!=expected&&failures<15)std::cerr<<"case "<<row<<" word "<<i<<" actual "<<std::hex<<actual<<" expected "<<expected<<std::dec<<'\n';
        }
        // The original does not alter scene, derivative caches or position here.
        check(std::memcmp(result.primary.scene.v,a.state.scene.v,64)==0);
        check(std::memcmp(result.primary.body.linearAcceleration.v,a.state.body.linearAcceleration.v,12)==0);
        check(std::memcmp(result.primary.body.angularAcceleration.v,a.state.body.angularAcceleration.v,12)==0);
    }
    SpringImpactBody48K b{};b.parameters.inverseMass=1;for(unsigned i=0;i<4;++i)b.state.scene.v[i*5]=1;
    SpringImpactParameters48K p{};p.disableLeverArms5C=true;p.contactVelocity120={{0,-1,0}};
    SpringImpactResult48K result{};result.primary.body.position.v[0]=123;const auto saved=result;
    Vector3_48C normal{{0,1,0}},zero{};
    check(!applySpringImpact48K(normal,zero,0,p,b,&b,result));check(std::memcmp(&saved,&result,sizeof(saved))==0);
    b.parameters.inverseMass=0;
    check(!applySpringImpact48K(normal,zero,0,p,b,nullptr,result));check(std::memcmp(&saved,&result,sizeof(saved))==0);
    b.parameters.inverseMass=1;p.contactVelocity120.v[1]=std::numeric_limits<float>::quiet_NaN();
    check(!applySpringImpact48K(normal,zero,0,p,b,nullptr,result));check(std::memcmp(&saved,&result,sizeof(saved))==0);
    std::cout<<"spring48K "<<checks<<" checks, "<<failures<<" failures\n";return failures?1:0;
}
