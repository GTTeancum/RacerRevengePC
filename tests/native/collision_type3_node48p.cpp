#include "../../src/game/collision_type3_node48p.h"
#include <cfenv>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <new>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
using namespace rr::game;
static std::uint64_t allocations=0;
void* operator new(std::size_t n) {++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n) {return ::operator new(n);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
static float fp(std::uint32_t b) {float f;std::memcpy(&f,&b,4);return f;}
int main(int argc,char** argv) {
    unsigned checks=0,index=0,sphereCount=0,rayCount=0,statusCounts[2][2]={};const char* kind="setup";
    try {
        std::fesetenv(FE_DFL_ENV);std::fesetround(FE_TONEAREST);
        const std::string directory=argc>1?argv[1]:"evidence/type3Node48P";
        for(unsigned type=0;type<2;++type) {
            kind=type==0?"sphere":"ray";
            std::ifstream in(directory+"/"+kind+"-fixtures.bin",std::ios::binary);
            if(!in)throw std::runtime_error("missing frozen node fixtures");
            std::uint32_t count=0,width=0;in.read(reinterpret_cast<char*>(&count),4);in.read(reinterpret_cast<char*>(&width),4);
            if(!in||count!=(type==0?467u:3964u)||width!=(type==0?11u:16u))throw std::runtime_error("invalid fixture header");
            for(index=0;index<count;++index) {
                std::uint32_t row[16]={};in.read(reinterpret_cast<char*>(row),width*4);
                if(!in)throw std::runtime_error("truncated fixtures");
                Vector3_48C local{},center{},half{};Type3Segment48P segment{};float radius=0.f;unsigned k=0;
                auto vector=[&](auto& v){for(float& x:v.v)x=fp(row[k++]);};
                if(type==0){vector(local);radius=fp(row[k++]);}
                else {vector(segment.start);vector(segment.direction);segment.length=fp(row[k++]);}
                vector(center);vector(half);
                if(k+1!=width)throw std::runtime_error("fixture layout");
                const auto beforeLocal=local,beforeCenter=center,beforeHalf=half;const auto beforeSegment=segment;
                const auto actual=type==0?type3SphereBox48P(local,radius,center,half):type3RayBox48P(segment,center,half);
                ++checks;
                if(static_cast<unsigned>(actual)!=row[k])throw std::runtime_error("status got="+std::to_string(static_cast<unsigned>(actual))+" expected="+std::to_string(row[k]));
                ++statusCounts[type][row[k]];++checks;
                if(std::memcmp(&local,&beforeLocal,sizeof(local))||std::memcmp(&center,&beforeCenter,sizeof(center))||
                   std::memcmp(&half,&beforeHalf,sizeof(half))||std::memcmp(&segment,&beforeSegment,sizeof(segment)))
                    throw std::runtime_error("predicate mutated input");
            }
            char trailing;if(in.read(&trailing,1))throw std::runtime_error("trailing fixture bytes");
            (type==0?sphereCount:rayCount)=count;
        }
        kind="guards";index=0;
        Vector3_48C local{{0.f,0.f,0.f}},center{{0.f,0.f,0.f}},half{{1.f,1.f,1.f}};float radius=1.f;
        Type3Segment48P segment{{{0.f,0.f,0.f,fp(0x7fc12345)}},{{0.f,0.f,0.f,fp(0xff812345)}},1.f};
        float* sphereLanes[]={&local.v[0],&local.v[1],&local.v[2],&radius,&center.v[0],&center.v[1],&center.v[2],&half.v[0],&half.v[1],&half.v[2]};
        float* rayLanes[]={&segment.start.v[0],&segment.start.v[1],&segment.start.v[2],&segment.direction.v[0],&segment.direction.v[1],&segment.direction.v[2],&segment.length,&center.v[0],&center.v[1],&center.v[2],&half.v[0],&half.v[1],&half.v[2]};
        const std::uint32_t invalidBits[]={0x7f800000,0xff800000,0x7fc12345,0xff812345};
        for(float* lane:sphereLanes)for(auto word:invalidBits) {
            const float saved=*lane;*lane=fp(word);++checks;
            if(type3SphereBox48P(local,radius,center,half)!=Type3NodeStatus48P::InvalidInput)throw std::runtime_error("nonfinite sphere source accepted");
            *lane=saved;
        }
        for(float* lane:rayLanes)for(auto word:invalidBits) {
            const float saved=*lane;*lane=fp(word);++checks;
            if(type3RayBox48P(segment,center,half)!=Type3NodeStatus48P::InvalidInput)throw std::runtime_error("nonfinite ray source accepted");
            *lane=saved;
        }
        for(float& extent:half.v) {
            extent=-1.f;checks+=2;
            if(type3SphereBox48P(local,radius,center,half)!=Type3NodeStatus48P::InvalidInput||
               type3RayBox48P(segment,center,half)!=Type3NodeStatus48P::InvalidInput)throw std::runtime_error("negative half extent accepted");
            extent=1.f;
        }
        ++checks;if(type3SphereBox48P(local,-1.f,center,half)!=Type3NodeStatus48P::InvalidInput)throw std::runtime_error("negative radius accepted");
        for(float length:{-1.f,-0.f,0.f}) {
            segment.length=length;checks+=2;
            if(type3RayBox48P(segment,center,half)!=Type3NodeStatus48P::Overlap)throw std::runtime_error("start-inside length semantics");
            segment.start.v[0]=2.f;
            if(type3RayBox48P(segment,center,half)!=Type3NodeStatus48P::Separated)throw std::runtime_error("stationary outside nonpositive length");
            segment.start.v[0]=0.f;
        }
        kind="allocation";segment.start.v[0]=2.f;segment.direction.v[0]=-1.f;segment.length=1.f;
        const auto beforeAllocations=allocations;
        for(unsigned i=0;i<100000;++i) {
            if(type3SphereBox48P(local,radius,center,half)!=Type3NodeStatus48P::Overlap||
               type3RayBox48P(segment,center,half)!=Type3NodeStatus48P::Overlap)throw std::runtime_error("serial node predicates");
        }
        ++checks;if(allocations!=beforeAllocations)throw std::runtime_error("runtime allocation");
        std::cout<<"PASS "<<checks<<" checks; "<<sphereCount<<" original sphere and "<<rayCount<<" original ray-box fixtures; overlaps="<<statusCounts[0][0]<<"/"<<statusCounts[1][0]<<", separated="<<statusCounts[0][1]<<"/"<<statusCounts[1][1]<<"; 100000 serial calls each, zero allocations\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"FAIL after "<<checks<<" checks "<<kind<<" fixture="<<index<<": "<<e.what()<<"\n";return 1;
    }
}
