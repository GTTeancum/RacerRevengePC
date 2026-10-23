#include "../../src/game/original_ui_garage_specular50kg.h"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace rr::game;
using namespace rr::game::garage_specular50KG;
static garage_prelit50KG::Matrix identity(){garage_prelit50KG::Matrix m{};for(unsigned i=0;i<4;++i)m[i*5]=1;return m;}
static Context context(){
    Context c;auto& g=c.geometry;g.modelWorld100=g.viewC0=g.projection80=identity();
    g.sourceScreen140=identity();g.sourceScreen140[0]=320;g.sourceScreen140[5]=-224;
    g.sourceScreen140[10]=100;g.sourceScreen140[12]=2048;g.sourceScreen140[13]=2048;g.sourceScreen140[14]=100;
    g.positionScale1E0=.5f;g.uvScale1E8=.25f;g.modelFlags1B2=7;
    g.fogEnd33Z=0;g.fogSlope33W=0;c.normalScale1E4=1.f/127.f;
    auto& l=c.lighting;l.viewC0=g.viewC0;l.projection80=g.projection80;l.clipProjection290=g.projection80;
    l.screen140=g.sourceScreen140;l.fogEnd33Z=g.fogEnd33Z;l.fogSlope33W=g.fogSlope33W;
    l.lights.directions[1][0]=1;l.lights.diffuse[0]={{30,60,90,0}};l.lights.diffuse[3]={{10,20,30,64}};
    return c;
}
int main(){
    auto c=context();Preamble p;assert(prepare(c,p)==Status::complete);
    assert((p.geometry.modelFlags1B2&2)==0); // source Specular ignores radial bit
    std::array<RawVertex,3> raw{};raw[0].position={{-1,-1,0}};raw[1].position={{1,-1,0}};raw[2].position={{0,1,0}};
    for(auto& v:raw)v.normal={{0,127,0}};
    garage_prelit50KG::Polygon out;assert(projectTriangle(p,raw,out)==Status::complete);
    assert(out.count==3&&!out.sourceTriangleFan);
    for(const auto& v:out.vertices) {if(!v.rasterW)continue;assert((v.rgba==std::array<std::uint8_t,4>({40,80,120,64})));assert(v.fogByte==255);assert(v.stq[0]==.5f&&v.stq[1]==1&&v.stq[2]==1);}
    // Exact reuse agreement with original generic whole-inside Specular math.
    for(unsigned i=0;i<3;++i){OriginalUiLitVertex50KM baseline;
        std::array<float,3> xyz{},n{};for(unsigned a=0;a<3;++a){xyz[a]=float(raw[i].position[a]);n[a]=float(raw[i].normal[a]);}
        assert(projectOriginalUiSpecularMapCompressedVertex50KM(p.lighting,p.normalUV,xyz,n,baseline)==OriginalUiLitGeometryStatus50KM::complete);
        for(unsigned a=0;a<4;++a)assert(baseline.vertex.color[a]==out.vertices[i].rgba[a]);
        assert(baseline.vertex.position[0]+1728==out.vertices[i].gsPosition[0]);
        assert(baseline.vertex.position[1]+1824==out.vertices[i].gsPosition[1]);
    }
    raw[0].position={{-3,0,0}};raw[1].normal={{0,0,0}};
    assert(projectTriangle(p,raw,out)==Status::complete&&out.count==4&&out.sourceTriangleFan);
    assert(out.vertices[0].rgba[0]==32&&out.vertices[1].rgba[0]==10&&out.vertices[2].rgba[0]==40&&out.vertices[3].rgba[0]==40);
    for(unsigned i=0;i<out.count;++i){assert(out.vertices[i].rgba[3]==64);assert(out.vertices[i].fogByte==255);}
    // The saved modelView basis, not world normals, determines original UV.
    c=context();c.geometry.viewC0=identity();c.geometry.viewC0[0]=0;c.geometry.viewC0[1]=1;c.geometry.viewC0[4]=-1;c.geometry.viewC0[5]=0;c.lighting.viewC0=c.geometry.viewC0;
    assert(prepare(c,p)==Status::complete);for(auto& v:raw){v.position={{0,0,0}};v.normal={{127,0,0}};}
    assert(projectTriangle(p,raw,out)==Status::complete);assert(out.vertices[0].stq[0]==.5f&&out.vertices[0].stq[1]==1);
    auto bad=c;bad.lighting.viewC0=identity();const auto preserved=p;
    assert(prepare(bad,p)==Status::input&&p.normalUV==preserved.normalUV);
    std::puts("Garage single-node Specular numeric PASS: genuine global light RGBA, saved modelView UV, fixed F255, original polygon clip.");
}
