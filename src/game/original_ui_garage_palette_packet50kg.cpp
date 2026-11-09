#include "original_ui_garage_palette_packet50kg.h"
#include "original_ui_garage_palette_vertex50kg.h"
#include "original_racer_model_packets50la.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace rr::game::garage_palette50KG {
namespace {
#if defined(_MSC_VER)&&!defined(__clang__)&&!defined(RR_GARAGE_VOLATILE_KERNELS)
// See original_ui_garage_prelit50kg.cpp: /fp:strict + fp_contract(off).
#pragma fp_contract(off)
float add(float a,float b)noexcept{return a+b;}
float mul(float a,float b)noexcept{return a*b;}
#else
float add(float a,float b)noexcept{volatile float r=a+b;return r;}
float mul(float a,float b)noexcept{volatile float r=a*b;return r;}
#endif
Status map(OriginalUiLitGeometryStatus50KM s)noexcept{
    return s==OriginalUiLitGeometryStatus50KM::complete?Status::complete:
        s==OriginalUiLitGeometryStatus50KM::arithmetic?Status::arithmetic:Status::input;
}
bool affine(const BodyMatrix48G& m)noexcept{
    for(float value:m.v)if(!std::isfinite(value))return false;
    return m.v[3]==0&&m.v[7]==0&&m.v[11]==0&&m.v[15]==1;
}
}
Status prepare(const OriginalRacerModelPacket50LA& packet,const Context& c,
    Family family,Preamble& out)noexcept{
    if(!original_racer_lighting50KM::selection_detail::arithmeticReady())return Status::arithmetic;
    const auto count=packet.sourcePaletteRows.size();
    if(!count||count>maxModelPaletteEntries49S||packet.extraMatrixCount!=2||
       packet.matrixSlots.size()!=3+count||!affine(packet.matrixSlots[2])||
       !(c.guardX334>0&&c.guardX334<=1&&c.guardY338>0&&c.guardY338<=1)||
       (family!=Family::diffuseCompressed&&family!=Family::specularMapCompressed))return Status::input;
    for(float scale:packet.scale40)if(!std::isfinite(scale)||scale<=0)return Status::input;
    for(auto id:packet.sourcePaletteRows)if(id>=maxModelPaletteEntries49S)return Status::input;
    Preamble p;p.entryCount=static_cast<std::uint32_t>(count);p.family=family;
    p.guardX334=c.guardX334;p.guardY338=c.guardY338;
    auto lighting=c.camera;
    for(unsigned row=0;row<4;++row){
        std::copy_n(packet.matrixSlots[0].v+4*row,4,lighting.lights.directions[row].begin());
        std::copy_n(packet.matrixSlots[1].v+4*row,4,lighting.lights.diffuse[row].begin());
    }
    if(family==Family::specularMapCompressed){
        const auto s=prepareOriginalUiSpecularNormalAxes50KM(packet.matrixSlots[2],
            c.camera.viewC0,packet.scale40[1],p.specularNormalUV);
        if(s!=OriginalUiLitGeometryStatus50KM::complete)return map(s);
    }
    for(std::size_t i=0;i<count;++i){
        const auto& world=packet.matrixSlots[3+i];if(!affine(world))return Status::input;
        garage_prelit50KG::Context g;
        std::copy_n(world.v,16,g.modelWorld100.begin());g.viewC0=c.camera.viewC0;
        g.projection80=c.camera.projection80;g.sourceScreen140=c.camera.screen140;
        g.cameraPosition180=c.camera.cameraPosition180;
        g.positionScale1E0=packet.scale40[0];g.uvScale1E8=packet.scale40[2];
        g.fogEnd33Z=c.camera.fogEnd33Z;g.fogSlope33W=c.camera.fogSlope33W;
        g.modelFlags1B2=c.modelFlags1B2;
        // Both palette programs retain the actual04 dispatch:337D68,
        //338D70 and33BFF0. Specular alone forces F255, not a clip branch.
        if(family==Family::specularMapCompressed)
            g.modelFlags1B2=static_cast<std::uint8_t>(g.modelFlags1B2&~2u);
        auto s=garage_prelit50KG::prepare(g,p.nodes[i].geometry);
        if(s!=Status::complete)return s;
        const auto l=prepareOriginalUiLitPreamble50KM(world,lighting,true,
            packet.scale40[0],packet.scale40[1],p.nodes[i].lighting);
        if(l!=OriginalUiLitGeometryStatus50KM::complete)return map(l);
    }
    out=p;return Status::complete;
}
Status checkTriangle(const Preamble& p,const std::array<RawVertex,3>& raw)noexcept{
    if(!original_racer_lighting50KM::selection_detail::arithmeticReady())return Status::arithmetic;
    if(!p.entryCount||p.entryCount>maxModelPaletteEntries49S||raw[2].paletteAdcWord<=0||
       !(p.guardX334>0&&p.guardX334<=1&&p.guardY338>0&&p.guardY338<=1)||
       (p.family!=Family::diffuseCompressed&&p.family!=Family::specularMapCompressed))return Status::input;
    return Status::complete;
}
Status prepareVertex(const Preamble& p,const RawVertex& raw,
    garage_prelit50KG::PreparedVertex& out,std::uint8_t& guard)noexcept{
    out={};guard=0;
    const int signedWord=raw.paletteAdcWord;
    const int word=signedWord<0?-signedWord:signedWord;
    if(!word||word%16||static_cast<unsigned>(word/16)>p.entryCount)return Status::input;
    const auto& node=p.nodes[static_cast<unsigned>(word/16-1)];
    if(node.geometry.modelFlags1B2!=p.nodes[0].geometry.modelFlags1B2||
       node.geometry.sourceScreen140!=p.nodes[0].geometry.sourceScreen140)return Status::input;
    garage_prelit50KG::RawVertex r;r.position=raw.position;r.uv=raw.uv;
    auto s=garage_prelit50KG::prepareVertex(node.geometry,r,out);
    if(s!=Status::complete)return s;
    if(p.family==Family::specularMapCompressed){
        //33C1A8..33C1E0: root model/view axes, independent of this vertex's
        // actual position/light palette. No per-node UV substitute.
        for(unsigned lane=0;lane<2;++lane){
            float n=mul(p.specularNormalUV[0][lane],float(raw.normal[0]));
            n=add(n,mul(p.specularNormalUV[1][lane],float(raw.normal[1])));
            n=add(n,mul(p.specularNormalUV[2][lane],float(raw.normal[2])));
            out.uvq[lane]=add(mul(n,.5f),.5f);
        }
        out.uvq[2]=1;out.fog=255;
    }
    std::array<float,3> amount{};
    for(unsigned light=0;light<3;++light){
        float n=mul(node.lighting.normalAxes[0][light],float(raw.normal[0]));
        n=add(n,mul(node.lighting.normalAxes[1][light],float(raw.normal[1])));
        n=add(n,mul(node.lighting.normalAxes[2][light],float(raw.normal[2])));
        if(!std::isfinite(n))return Status::arithmetic;
        amount[light]=std::max(0.f,n);
    }
    for(unsigned lane=0;lane<4;++lane){
        float packed=add(node.lighting.colorRows[3][lane],8388608.f);
        for(unsigned light=0;light<3;++light){
            packed=add(packed,mul(node.lighting.colorRows[light][lane],amount[light]));
            if(!std::isfinite(packed))return Status::arithmetic;
        }
        packed=std::min(packed,8388863.f);if(packed<8388608.f)return Status::arithmetic;
        std::uint32_t bits=0;std::memcpy(&bits,&packed,sizeof bits);
        out.color[lane]=float(bits&255u);
    }
    //337EB0/337FC8,338F40/339110,33C188/33C330 multiply only the
    // rejection X/Y lanes. The cached986/989 clipper coordinates retain
    // the ordinary projection result. Never feed guard290 into366430.
    auto guarded=out.clip;
    guarded[0]=mul(guarded[0],p.guardX334);guarded[1]=mul(guarded[1],p.guardY338);
    for(unsigned axis=0;axis<3;++axis){
        if(!std::isfinite(guarded[axis]))return Status::arithmetic;
        if(guarded[axis]>guarded[3])guard|=std::uint8_t(1u<<(2*axis));
        if(guarded[axis]<-guarded[3])guard|=std::uint8_t(2u<<(2*axis));
    }
    return Status::complete;
}
Status projectPrepared(const Preamble& p,const std::array<garage_prelit50KG::PreparedVertex,3>& vertices,
    std::uint8_t guardAny,garage_prelit50KG::Polygon& out)noexcept{
    auto clipping=p.nodes[0].geometry;
    // Guard-inside primitives retain the untouched packet, even outside the
    // ordinary viewport. Actual GS scissor remains a renderer responsibility.
    if(!guardAny)clipping.modelFlags1B2=static_cast<std::uint8_t>(clipping.modelFlags1B2&~4u);
    return garage_prelit50KG::projectPreparedTriangle(clipping,vertices,out);
}
Status projectTriangle(const Preamble& p,const std::array<RawVertex,3>& raw,
    garage_prelit50KG::Polygon& out)noexcept{
    auto s=checkTriangle(p,raw);if(s!=Status::complete)return s;
    std::array<garage_prelit50KG::PreparedVertex,3> vertices{};
    std::uint8_t guardAny=0;
    for(unsigned i=0;i<3;++i){
        std::uint8_t guard=0;
        s=prepareVertex(p,raw[i],vertices[i],guard);if(s!=Status::complete)return s;
        guardAny|=guard;
    }
    return projectPrepared(p,vertices,guardAny,out);
}
}
