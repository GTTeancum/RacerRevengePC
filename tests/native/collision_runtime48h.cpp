#include "../../src/assets/collision_runtime48h.h"
#include "../../src/assets/import_collision48g.h"
#include "../../tools/assets/collision_runtime_words48h.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
using namespace rr::assets;
static std::uint32_t bits(float x){std::uint32_t u;std::memcpy(&u,&x,4);return u;}
static float fp(std::uint32_t u){float x;std::memcpy(&x,&u,4);return x;}
static std::array<std::int16_t,3> original(const std::array<float,3>&normal){
    std::array<std::uint32_t,32>g{},f{};std::array<std::uint8_t,256>memory{};
    std::memcpy(memory.data()+0x70,normal.data(),12);f[2]=bits(32767);
    for(auto w:rr_normal_pack48h){
        const auto op=w>>26,rs=w>>21&31,rt=w>>16&31,rd=w>>11&31,fd=w>>6&31,fn=w&63;
        const auto a=g[rs]+static_cast<std::int16_t>(w&65535);
        if(w==0)continue;
        if(op==15)g[rt]=(w&65535)<<16;
        else if(op==0x31){if(a+4>memory.size())throw std::runtime_error("load bounds");std::memcpy(&f[rt],memory.data()+a,4);}
        else if(op==0x29){if(a+2>memory.size())throw std::runtime_error("store bounds");const auto half=static_cast<std::uint16_t>(g[rt]);std::memcpy(memory.data()+a,&half,2);}
        else if(op==0x11){
            if(rs==4)f[rd]=g[rt];else if(rs==0)g[rt]=f[rd];
            else if(rs==16&&fn==0){volatile float x=fp(f[rd])+fp(f[rt]);f[fd]=bits(x);}
            else if(rs==16&&fn==2){volatile float x=fp(f[rd])*fp(f[rt]);f[fd]=bits(x);}
            else if(rs==16&&fn==0x24){const auto u=f[rd];f[fd]=(u&0x7f800000u)<=0x4e800000u?static_cast<std::uint32_t>(static_cast<std::int32_t>(fp(u))):((u&0x80000000u)?0x80000000u:0x7fffffffu);}
            else throw std::runtime_error("FPU opcode");
        }else throw std::runtime_error("opcode");g[0]=0;
    }
    std::array<std::int16_t,3>out;std::memcpy(out.data(),memory.data()+0xd0,6);return out;
}
int main(int argc,char**argv){std::uint64_t checks=0;unsigned faces=0,files=0;
    auto require=[&](bool ok,const char*why){++checks;if(!ok)throw std::runtime_error(why);};
    auto compare=[&](const std::array<float,3>&n){std::array<std::int16_t,3>a{};require(packCollisionFaceNormal48H(n,a),"pack rejected");require(a==original(n),"original packing mismatch");};
    try{
        require(argc==2,"root argument");const auto root=std::filesystem::path(argv[1]);std::vector<std::array<float,3>>normals;
        for(const auto&e:std::filesystem::directory_iterator(root/"evidence/importCollision48G"))if(e.path().extension()==".col"){
            std::ifstream f(e.path(),std::ios::binary);std::vector<std::uint8_t>bytes((std::istreambuf_iterator<char>(f)),{});Collision48G c;DecodeError48C error;
            require(decodeCollision48G(bytes,c,error),"original fixture rejected");++files;faces+=static_cast<unsigned>(c.faces.size());for(const auto&face:c.faces)normals.push_back(face.normal);
        }
        require(files==38&&faces==14586,"TA fixture count");
        for(int mode:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}){
            require(std::fesetround(mode)==0,"rounding");for(const auto&n:normals)compare(n);
            for(float x:{-65537.f,-2.f,-1.f,-.5f,-0.f,0.f,.5f,1.f,2.f,65537.f})compare({x,x,x});
            for(int n=-32767;n<=32767;n+=31){const float x=(static_cast<float>(n)-.5f)/32767;compare({x,std::nextafter(x,-INFINITY),std::nextafter(x,INFINITY)});}
        }
        std::fesetround(FE_TONEAREST);std::array<std::int16_t,3>a{1,2,3},unchanged=a;
        require(!packCollisionFaceNormal48H({0,INFINITY,0},a)&&a==unchanged,"nonfinite transaction");
        require(!packCollisionFaceNormal48H({std::numeric_limits<float>::max(),0,0},a)&&a==unchanged,"overflow transaction");
        require(packCollisionFaceNormal48H({-1,0,1},a)&&a[0]==-32766&&a[1]==0&&a[2]==32767,"asymmetric original rounding");
        std::cout<<checks<<" collision normal packing checks passed; "<<faces<<" original faces; four rounding modes\n";return 0;
    }catch(const std::exception&e){std::fesetround(FE_TONEAREST);std::cerr<<"FAIL after "<<checks<<": "<<e.what()<<"\n";return 1;}
}
