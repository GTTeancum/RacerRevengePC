#include "spring_contact48j.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
using namespace rr::game;
int main(int argc,char** argv) {
    if(argc!=2)return 2;
    std::ifstream in(std::filesystem::path(argv[1])/"evidence/spring48J/friction-fixtures.bin",std::ios::binary);
    std::uint32_t count=0,width=0;in.read(reinterpret_cast<char*>(&count),4);in.read(reinterpret_cast<char*>(&width),4);
    if(!in||count!=1024||width!=80)return 2;
    unsigned checks=0,failures=0;
    auto check=[&](bool ok){++checks;if(!ok)++failures;};
    for(unsigned row=0;row<count;++row) {
        std::array<float,80> f{};in.read(reinterpret_cast<char*>(f.data()),sizeof(f));if(!in)return 2;
        auto vector=[&](unsigned i){return Vector3_48C{{f[i],f[i+1],f[i+2]}};};
        SpringPointForceParameters48J p{vector(14),vector(11),f[10],f[17]!=0,f[18]!=0};
        SpringFrictionBody48J a{},b{};
        std::memcpy(a.scene.v,f.data()+20,64);std::memcpy(b.scene.v,f.data()+36,64);
        a.loads={vector(52),vector(55)};b.loads={vector(58),vector(61)};a.mass=f[19];
        SpringFrictionResult48J out{};
        check(applySpringFriction48J(vector(0),vector(3),vector(6),f[9],p,a,f[64]!=0?&b:nullptr,out));
        std::array<float,15> got{};
        std::memcpy(got.data(),out.worldForce.v,12);std::memcpy(got.data()+3,out.primary.force.v,12);
        std::memcpy(got.data()+6,out.primary.torque.v,12);std::memcpy(got.data()+9,out.other.force.v,12);
        std::memcpy(got.data()+12,out.other.torque.v,12);
        for(unsigned i=0;i<15;++i) {
            std::uint32_t actual,expected;std::memcpy(&actual,&got[i],4);std::memcpy(&expected,&f[65+i],4);
            check(actual==expected);
            if(actual!=expected&&failures<12)std::cerr<<"case "<<row<<" word "<<i<<" actual "<<std::hex<<actual<<" expected "<<expected<<std::dec<<'\n';
        }
    }
    SpringFrictionBody48J body{};body.mass=1;for(unsigned i=0;i<4;++i)body.scene.v[i*5]=1;
    SpringPointForceParameters48J p{};p.torqueScale=1;
    SpringFrictionResult48J out{};out.worldForce.v[0]=123;const auto before=out;
    Vector3_48C bad{{std::numeric_limits<float>::quiet_NaN(),0,0}},zero{},normal{{0,1,0}};
    check(!applySpringFriction48J(bad,zero,zero,1,p,body,nullptr,out));check(std::memcmp(&out,&before,sizeof(out))==0);
    body.mass=std::numeric_limits<float>::max();Vector3_48C speed{{100,0,0}};
    check(!applySpringFriction48J(normal,zero,speed,1,p,body,nullptr,out));check(std::memcmp(&out,&before,sizeof(out))==0);
    BodyLoads48C loads{},result{};result.force.v[0]=123;const auto saved=result;
    BodyMatrix48G invalid{};invalid.v[0]=bad.v[0];p.torqueScale=bad.v[0];
    check(addSpringPointForce48J(invalid,loads,p,speed,zero,result));check(result.force.v[0]==100);
    result=saved;check(!addSpringPointForce48J(invalid,loads,p,speed,normal,result));check(std::memcmp(&result,&saved,sizeof(result))==0);
    std::cout<<"spring48J "<<checks<<" checks, "<<failures<<" failures\n";return failures?1:0;
}
