#include "game/original_ui_garage_prelit50kg.h"
#include <cassert>
#include <cfenv>
#include <cmath>
#include <limits>
#include <iostream>
using namespace rr::game::garage_prelit50KG;
static Matrix identity(){Matrix m{};for(unsigned i=0;i<4;++i)m[i*5]=1;return m;}
static Context context(){
    Context c;c.modelWorld100=c.viewC0=c.projection80=identity();
    c.sourceScreen140[0]=320;c.sourceScreen140[5]=-224;
    c.sourceScreen140[10]=-8388590;c.sourceScreen140[12]=c.sourceScreen140[13]=2048;
    c.sourceScreen140[14]=8388591;c.sourceScreen140[15]=1;
    c.positionScale1E0=1;c.uvScale1E8=.5f;return c;
}
static RawVertex vertex(short x,short y,short z,short u,unsigned char r){
    RawVertex v;v.position={{x,y,z}};v.uv={{u,short(-u)}};v.rgba={{r,24,14,63}};return v;
}
int main(){
    assert(std::fesetround(FE_TONEAREST)==0);
    auto c=context();Preamble p;Polygon out;
    assert(prepare(c,p)==Status::complete);
    const std::array<RawVertex,3> raw{{vertex(0,0,0,2,29),vertex(1,0,0,4,127),vertex(0,1,0,-2,80)}};
    assert(projectTriangle(p,raw,out)==Status::complete&&out.count==3&&!out.sourceTriangleFan);
    assert(out.vertices[0].gsPosition==(std::array<float,3>{{2048,2048,8388591}}));
    assert(out.vertices[1].gsPosition[0]==2368&&out.vertices[2].gsPosition[1]==1824);
    for(unsigned i=0;i<3;++i){assert(out.vertices[i].rgba==raw[i].rgba);assert(out.vertices[i].fogByte==255);}
    assert(out.vertices[0].stq==(std::array<float,3>{{1,-1,1}}));
    assert(out.vertices[2].stq==(std::array<float,3>{{-1,1,1}}));
    // Existing hardware source alpha is63, never normalized/faked as128.
    assert(out.vertices[1].rgba[3]==63);

    //3401F8 before340210: world camera maps to raw local units; position
    // scale .5, source end1 and slope20, raw distance5 => F30, notF40.
    c=context();c.positionScale1E0=.5f;c.fogEnd33Z=1;c.fogSlope33W=20;c.modelFlags1B2=2;
    assert(prepare(c,p)==Status::complete&&p.endTimesUnscaledSlope==20&&p.scaledSlope==10);
    const auto fogVertex=vertex(3,4,0,0,100);
    assert(projectTriangle(p,{{fogVertex,fogVertex,fogVertex}},out)==Status::complete);
    assert(out.vertices[0].fogByte==30&&out.vertices[0].gsPosition[0]==2528&&out.vertices[0].gsPosition[1]==1600);
    c.cameraPosition180={{1,2,3}};c.modelWorld100[12]=1;c.modelWorld100[13]=1;c.modelWorld100[14]=1;
    assert(prepare(c,p)==Status::complete&&p.localCameraRaw==(std::array<float,3>{{0,2,4}}));

    // Left clip. The five all-inside planes still rotate the polygon; final
    // source fan anchor is AB intersection, not an arbitrarily sorted vertex.
    c=context();c.modelFlags1B2=5;assert(prepare(c,p)==Status::complete);
    const std::array<RawVertex,3> left{{vertex(-2,0,0,0,0),vertex(0,0,0,2,100),vertex(0,1,0,4,200)}};
    assert(projectTriangle(p,left,out)==Status::complete&&out.count==4&&out.sourceTriangleFan);
    constexpr float x[]={1728,2048,2048,1728},y[]={2048,2048,1824,1936},u[]={.5f,1,2,1};
    constexpr unsigned char red[]={50,100,200,100};
    for(unsigned i=0;i<4;++i){assert(out.vertices[i].gsPosition[0]==x[i]&&out.vertices[i].gsPosition[1]==y[i]);
        assert(out.vertices[i].rgba[0]==red[i]&&out.vertices[i].rgba[3]==63&&out.vertices[i].stq[0]==u[i]);}
    // No source bit01 permission: original marks a crossing triangle off.
    c.modelFlags1B2=4;assert(prepare(c,p)==Status::complete);
    assert(projectTriangle(p,left,out)==Status::complete&&out.count==0&&!out.sourceTriangleFan);
    // No bit04: program is unclipped even if ordinary-camera X is outside.
    c.modelFlags1B2=0;assert(prepare(c,p)==Status::complete);
    assert(projectTriangle(p,left,out)==Status::complete&&out.count==3&&!out.sourceTriangleFan);
    assert(out.vertices[0].gsPosition[0]==1408&&out.vertices[0].rgba[0]==0);
    c.modelFlags1B2=5;assert(prepare(c,p)==Status::complete);
    const auto outsideVertex=vertex(-2,0,0,0,5);
    assert(projectTriangle(p,{{outsideVertex,outsideVertex,outsideVertex}},out)==Status::complete&&out.count==0);

    // A negative-W input must reach source near clipping BEFORE color*W.
    // Ordinary projection Z=z-1,W=z; source near is Z+W, i.e.z=.5.
    c=context();c.modelFlags1B2=5;c.projection80={};c.projection80[0]=.5f;
    c.projection80[5]=1;c.projection80[10]=1;c.projection80[11]=1;c.projection80[14]=-1;
    assert(prepare(c,p)==Status::complete);
    const std::array<RawVertex,3> near{{vertex(0,0,-1,0,0),vertex(0,0,1,2,100),vertex(1,0,1,4,200)}};
    assert(projectTriangle(p,near,out)==Status::complete&&out.count==4&&out.sourceTriangleFan);
    constexpr unsigned char nearRed[]={100,200,150,75};
    constexpr float nearW[]={1,1,.5f,.5f},nearU[]={1,2,3,1.5f};
    for(unsigned i=0;i<4;++i){assert(out.vertices[i].rgba[0]==nearRed[i]&&out.vertices[i].rgba[3]==63);
        assert(out.vertices[i].rasterW==nearW[i]&&out.vertices[i].stq[0]==nearU[i]&&out.vertices[i].fogByte==255);}
    assert(out.vertices[2].gsPosition[2]==16777181&&out.vertices[3].gsPosition[2]==16777181);

    // Rejections leave output untouched and cannot mint owner/capability.
    const auto saved=out;auto invalid=p;invalid.uvScale=std::numeric_limits<float>::infinity();
    assert(projectTriangle(invalid,near,out)==Status::input&&out.count==saved.count&&out.vertices[2].rgba==saved.vertices[2].rgba);
    c=context();c.positionScale1E0=0;const auto before=p;
    assert(prepare(c,p)==Status::input&&p.ordinaryClip==before.ordinaryClip);
    assert(std::fesetround(FE_DOWNWARD)==0);
    assert(projectTriangle(p,near,out)==Status::arithmetic&&out.count==saved.count);
    assert(std::fesetround(FE_TONEAREST)==0);
    std::cout<<"Garage PreLitCompressed focused numeric vectors passed\n";
}
