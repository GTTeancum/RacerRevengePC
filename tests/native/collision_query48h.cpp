#include "../../src/game/collision_query48h.h"
#include "../../tools/gameplay/query_fixtures48h.h"
#include <cfenv>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
using namespace rr::game;
static float fp(std::uint32_t b) {float f;std::memcpy(&f,&b,4);return f;}
static std::uint32_t bits(float f) {std::uint32_t b;std::memcpy(&b,&f,4);return b;}
int main() {
    unsigned checks=0,index=0,hits=0,misses=0,boundaries=0;
    try {
        std::fesetround(FE_TONEAREST);
        for(const auto& f:queryFixtures48H) {
            Triangle48H t{};DirectedSegment48H s{};
            unsigned k=0;
            for(auto& v:t.vertices)for(float& x:v.v)x=fp(f.input[k++]);
            for(auto& v:t.vertexNormals)for(float& x:v.v)x=fp(f.input[k++]);
            for(float& x:s.start.v)x=fp(f.input[k++]);
            for(float& x:s.direction.v)x=fp(f.input[k++]);
            s.length=fp(f.input[k++]);t.hasVertexNormals=f.normals;
            TriangleHit48H out{{{111.f,222.f,333.f}},{{444.f,555.f,666.f}},777.f};
            const TriangleHit48H before=out;
            const auto status=intersectTriangle48H(s,t,out);++checks;
            if(static_cast<unsigned>(status)!=f.status)throw std::runtime_error("status got="+std::to_string(static_cast<unsigned>(status))+" expected="+std::to_string(f.status));
            if(status==TriangleQueryStatus48H::Hit) {
                ++hits;
                for(unsigned i=0;i<7;++i) {
                    const float value=i<3?out.point.v[i]:i<6?out.normal.v[i-3]:out.fraction;
                    ++checks;if(bits(value)!=f.output[i])throw std::runtime_error("lane="+std::to_string(i)+" got="+std::to_string(bits(value))+" expected="+std::to_string(f.output[i]));
                }
            } else {
                if(status==TriangleQueryStatus48H::Miss)++misses;else ++boundaries;
                ++checks;if(std::memcmp(&out,&before,sizeof(out)))throw std::runtime_error("non-hit changed output");
            }
            ++index;
        }
        Triangle48H t{};DirectedSegment48H s{};TriangleHit48H out{};
        s.start.v[0]=std::numeric_limits<float>::quiet_NaN();++checks;
        if(intersectTriangle48H(s,t,out)!=TriangleQueryStatus48H::InvalidInput)throw std::runtime_error("NaN accepted");
        std::cout<<"PASS "<<checks<<" checks; "<<index<<" original-word leaf fixtures, hits="<<hits<<", misses="<<misses<<", explicit interpolation boundaries="<<boundaries<<"\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"FAIL after "<<checks<<" checks fixture="<<index<<": "<<e.what()<<"\n";return 1;
    }
}
