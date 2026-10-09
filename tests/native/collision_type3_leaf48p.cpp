#include "../../src/game/collision_type3_leaf48p.h"
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
static std::uint32_t bits(float f) {std::uint32_t b;std::memcpy(&b,&f,4);return b;}
int main(int argc,char** argv) {
    unsigned checks=0,index=0,statusCounts[5]={};
    try {
        std::fesetround(FE_TONEAREST);
        const char* path=argc>1?argv[1]:"evidence/type3Leaf48P/leaf-fixtures.bin";
        std::ifstream in(path,std::ios::binary);if(!in)throw std::runtime_error("missing frozen leaf fixtures");
        std::uint32_t count=0,width=0;in.read(reinterpret_cast<char*>(&count),4);in.read(reinterpret_cast<char*>(&width),4);
        if(!in||count!=904||width!=78)throw std::runtime_error("invalid fixture header");
        for(;index<count;++index) {
            std::uint32_t f[78];in.read(reinterpret_cast<char*>(f),sizeof(f));if(!in)throw std::runtime_error("truncated fixtures");
            Type3Triangle48P t{};Type3Segment48P s{};Type3PairLeafState48P p{};Type3SelectedLeafState48P selected{};
            unsigned k=0;
            auto vector=[&](auto& v){for(float& x:v.v)x=fp(f[k++]);};
            for(auto& v:t.vertices)vector(v);
            for(auto& v:t.vertexNormals)vector(v);
            t.hasVertexNormals=f[k++]!=0;t.normalMode=f[k++];vector(s.start);vector(s.direction);s.length=fp(f[k++]);
            p.fraction=fp(f[k++]);vector(p.normal);vector(p.point);p.hit=f[k++];
            selected.fraction=fp(f[k++]);vector(selected.normal);vector(selected.point);vector(selected.correction);selected.hitCount=f[k++];
            if(k!=53)throw std::runtime_error("fixture layout");
            const auto status=applyType3Triangle48P(s,t,p,selected);++checks;++statusCounts[static_cast<unsigned>(status)];
            if(static_cast<unsigned>(status)!=f[k++])throw std::runtime_error("status got="+std::to_string(static_cast<unsigned>(status))+" expected="+std::to_string(f[53]));
            auto scalar=[&](float x){++checks;const unsigned lane=k++;if(bits(x)!=f[lane])throw std::runtime_error("lane="+std::to_string(lane)+" got="+std::to_string(bits(x))+" expected="+std::to_string(f[lane]));};
            auto outVector=[&](const auto& v){for(float x:v.v)scalar(x);};
            scalar(p.fraction);outVector(p.normal);outVector(p.point);++checks;if(p.hit!=f[k++])throw std::runtime_error("pair hit");
            scalar(selected.fraction);outVector(selected.normal);outVector(selected.point);outVector(selected.correction);++checks;
            if(selected.hitCount!=f[k++])throw std::runtime_error("geometric hit count");
            if(k!=78)throw std::runtime_error("output layout");
        }
        char trailing; if(in.read(&trailing,1))throw std::runtime_error("trailing fixture bytes");
        Type3Segment48P s{{{.25f,1.f,.125f,1.f}},{{0.f,-1.f,0.f,1.f}},2.f};
        Type3Triangle48P t{{{{-2.f,0.f,-2.f}},{{0.f,0.f,2.f}},{{2.f,0.f,-2.f}}},{},true,2};
        Type3PairLeafState48P p{};Type3SelectedLeafState48P selected{};
        const auto beforeP=p;const auto beforeSelected=selected;
        ++checks;if(applyType3Triangle48P(s,t,p,selected)!=Type3LeafStatus48P::UnsupportedNormalMode)throw std::runtime_error("unrecovered mode accepted");
        ++checks;if(std::memcmp(&p,&beforeP,sizeof(p))||std::memcmp(&selected,&beforeSelected,sizeof(selected)))throw std::runtime_error("error mutated state");
        s.start.v[0]=std::numeric_limits<float>::quiet_NaN();++checks;
        if(applyType3Triangle48P(s,t,p,selected)!=Type3LeafStatus48P::InvalidInput)throw std::runtime_error("NaN accepted");
        s.start.v[0]=.25f;t.hasVertexNormals=false;t.normalMode=0;
        const auto beforeAllocations=allocations;
        for(unsigned i=0;i<100000;++i) {
            p=Type3PairLeafState48P{};
            if(applyType3Triangle48P(s,t,p,selected)!=Type3LeafStatus48P::Hit)throw std::runtime_error("serial leaf hit");
        }
        ++checks;if(allocations!=beforeAllocations)throw std::runtime_error("runtime allocation");
        std::cout<<"PASS "<<checks<<" checks; "<<index<<" original type3 one-pair leaf fixtures, hits="<<statusCounts[1]<<", misses="<<statusCounts[0]<<", interpolation boundaries="<<statusCounts[3]<<"; 100000 serial calls, zero allocations\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"FAIL after "<<checks<<" checks fixture="<<index<<": "<<e.what()<<"\n";return 1;
    }
}
