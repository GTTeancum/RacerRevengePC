#include "../../src/game/original_water_topology_numeric50la.h"
#include <cassert>
#include <cstdio>
#include <random>

namespace w=rr::game::water_topology_numeric50LA;
int main(){
    assert(std::fesetround(FE_TONEAREST)==0);
    std::mt19937 random(0x299420u);
    auto sample=[&](){return float(int(random()%8193u)-4096)/1024.f;};
    unsigned checked=0;double largest=0;
    for(unsigned trial=0;trial<12000u;++trial){
        std::array<w::Vertex,3> vertices{};
        for(auto& v:vertices){for(auto& x:v.position)x=sample();for(auto& x:v.uv)x=sample();}
        const w::Vec3 normal{sample(),sample(),sample()};
        std::array<w::Vec3,3> input{normal,normal,normal},out{};
        //Independent geometric reference: solve dP/du with a2x2 UV inverse,
        //project onto the normal's perpendicular plane, then scale its unit
        //direction by |normal|. No emulated VU instructions or helper reuse.
        const double du1=double(vertices[1].uv[0])-vertices[0].uv[0];
        const double dv1=double(vertices[1].uv[1])-vertices[0].uv[1];
        const double du2=double(vertices[2].uv[0])-vertices[0].uv[0];
        const double dv2=double(vertices[2].uv[1])-vertices[0].uv[1];
        const double det=du1*dv2-du2*dv1;
        if(std::abs(det)<0.5)continue;
        std::array<double,3> derivative{},projected{};
        double normalSquared=0,dot=0;
        for(unsigned i=0;i<3;++i){
            derivative[i]=((double(vertices[1].position[i])-vertices[0].position[i])*dv2-
                           (double(vertices[2].position[i])-vertices[0].position[i])*dv1)/det;
            normalSquared+=double(normal[i])*normal[i];dot+=derivative[i]*normal[i];
        }
        if(normalSquared<0.25)continue;
        double projectedSquared=0;
        for(unsigned i=0;i<3;++i){projected[i]=derivative[i]-normal[i]*dot/normalSquared;projectedSquared+=projected[i]*projected[i];}
        if(projectedSquared<0.25)continue;
        const auto result=w::prepare(vertices,input,out);assert(result.prepared);
        const double scale=std::sqrt(normalSquared/projectedSquared);
        for(unsigned i=0;i<3;++i){
            const double error=std::abs(double(out[0][i])-projected[i]*scale);
            largest=std::max(largest,error);assert(error<0.0001);
        }
        assert((out[1]==w::Vec3{})&&(out[2]==w::Vec3{}));
        ++checked;
    }
    assert(checked>8000u);
    std::printf("Water independent geometry: %u cases, largest component error %.9g\n",checked,largest);
}
