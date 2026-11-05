#include "../../src/game/original_water_row_storage50la.h"
#include <cassert>
#include <cstdio>
using S=rr::game::OriginalWaterRowStorage50LA;
using V=rr::game::water_topology_numeric50LA::Vec3;
static void put(std::span<std::uint8_t> destination,std::size_t at,float value){
    const auto word=std::bit_cast<std::uint32_t>(value);
    for(unsigned i=0;i<4;++i)destination[at+i]=std::uint8_t(word>>(8u*i));
}
int main(){
    assert(std::fesetround(FE_TONEAREST)==0);
    std::array<std::uint8_t,96> vertices;vertices.fill(0xff); //Unused fields include NaN bits.
    const std::array<V,3> positions{{{0,0,0},{1,0,0},{0,0,1}}};
    const std::array<std::array<float,2>,3> uv{{{0,0},{1,0},{0,1}}};
    for(unsigned i=0;i<3;++i){
        for(unsigned lane=0;lane<3;++lane)put(vertices,i*32u+lane*4u,positions[i][lane]);
        put(vertices,i*32u+24u,uv[i][0]);put(vertices,i*32u+28u,uv[i][1]);
    }
    std::array<std::uint8_t,12> normal{};put(normal,4,1.f);
    S full;assert(!full.allocateVertices()&&!full.allocateOutput());
    assert(full.allocateNormals(3)&&full.allocateVertices());
    std::array<std::uint8_t,97> excessive{};
    assert(!full.copyVertexPrefix(excessive)&&full.copyVertexPrefix(vertices));
    const std::array<std::uint8_t,13> tooWide{};
    assert(!full.copyNormalPrefix(0,tooWide));
    assert(!full.copyNormalPrefix(1,normal)); //No skipped occurrence.
    for(unsigned i=0;i<3;++i){
        assert(full.copyNormalPrefix(i,normal));
        assert(!full.normalByte(i,12)&&!full.normalByte(i,15));
        if(i<2u)assert(!full.allocateOutput());
    }
    assert(!full.copyVertexPrefix(vertices)&&!full.copyNormalPrefix(0,normal));
    assert(full.allocateOutput()&&full.calculateXYZ().prepared);
    assert(full.outputXYZ(0)==V({1,0,0})&&full.outputXYZ(1)==V{}&&full.outputXYZ(2)==V{});
    assert(!full.outputXYZ(3)&&!full.calculateXYZ().prepared);
    //A partial bulk read must not consume its unknown last UV byte.
    S shortVertices;assert(shortVertices.allocateNormals(3)&&shortVertices.allocateVertices());
    assert(shortVertices.copyVertexPrefix(std::span(vertices).first(95))&&!shortVertices.vertexByte(95));
    for(unsigned i=0;i<3;++i)assert(shortVertices.copyNormalPrefix(i,normal));
    assert(shortVertices.allocateOutput()&&!shortVertices.calculateXYZ().prepared&&!shortVertices.outputXYZ(0));
    //Every normal short-read length, including EOF, must stay unknown enough
    //to block XYZ computation; unread padding is never mistaken for data.
    for(unsigned length=0;length<12u;++length){
        S shortNormal;assert(shortNormal.allocateNormals(3)&&shortNormal.allocateVertices()&&shortNormal.copyVertexPrefix(vertices));
        assert(shortNormal.copyNormalPrefix(0,std::span(normal).first(length))&&!shortNormal.normalByte(0,length));
        assert(shortNormal.copyNormalPrefix(1,normal)&&shortNormal.copyNormalPrefix(2,normal));
        assert(shortNormal.allocateOutput()&&!shortNormal.calculateXYZ().prepared&&!shortNormal.outputXYZ(0));
    }
    //Two-row tails do not read vertex fields, even after a zero-byte transfer.
    S tails;assert(tails.allocateNormals(2)&&tails.allocateVertices()&&tails.copyVertexPrefix({}));
    assert(tails.copyNormalPrefix(0,normal)&&tails.copyNormalPrefix(1,normal)&&tails.allocateOutput());
    assert(tails.calculateXYZ().prepared&&tails.outputXYZ(0)==V{}&&tails.outputXYZ(1)==V{});
    S one;assert(one.allocateNormals(1)&&one.allocateVertices());
    assert(one.copyVertexPrefix(std::span(vertices).first(32))&&one.copyNormalPrefix(0,normal)&&one.allocateOutput());
    assert(!one.calculateXYZ().prepared&&!one.outputXYZ(0)); //Two tails cannot fit.
    S empty;
    if(empty.allocateNormals(0)&&empty.allocateVertices()){
        assert(empty.copyVertexPrefix({}));
        if(empty.allocateOutput())assert(!empty.calculateXYZ().prepared);
    } //Native malloc(0) may return null; neither outcome invents data.
    assert(!empty.outputXYZ(0));
    //Exercise every row at maximum capacity, across a long alternating strip.
    constexpr unsigned capacity=16384u;
    std::vector<std::uint8_t> strip(std::size_t(capacity)*32u,255u);
    for(unsigned i=0;i<capacity;++i){
        const float x=float(i/2u),z=float(i%2u);const auto at=std::size_t(i)*32u;
        put(strip,at,x);put(strip,at+4u,0);put(strip,at+8u,z);
        put(strip,at+24u,x);put(strip,at+28u,z);
    }
    S maximum;assert(maximum.allocateNormals(capacity)&&maximum.allocateVertices()&&maximum.copyVertexPrefix(strip));
    for(unsigned i=0;i<capacity;++i)assert(maximum.copyNormalPrefix(i,normal));
    assert(maximum.allocateOutput()&&maximum.calculateXYZ().prepared);
    for(unsigned i=0;i<capacity;++i){
        assert(maximum.outputXYZ(i)==(i+2u<capacity?V{1,0,0}:V{}));
        assert(!maximum.normalByte(i,12));
    }
    assert(!maximum.outputXYZ(capacity)&&!maximum.vertexByte(strip.size()));
    S refused;assert(!refused.allocateNormals(16385)&&!refused.normalsIdentity());
    std::puts("Water row storage: binary layout to XYZ, short/EOF refusal, padding knownness and tails passed");
}
