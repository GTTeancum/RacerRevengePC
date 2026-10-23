#include "original_ui_garage_diffuse50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace rr::game::garage_diffuse50KG {
namespace {
float add(float a,float b)noexcept{volatile float r=a+b;return r;}
float mul(float a,float b)noexcept{volatile float r=a*b;return r;}
Status map(OriginalUiLitGeometryStatus50KM s)noexcept{
    return s==OriginalUiLitGeometryStatus50KM::complete?Status::complete:
        s==OriginalUiLitGeometryStatus50KM::arithmetic?Status::arithmetic:Status::input;
}
}
Status prepare(const Context& c,Preamble& out)noexcept{
    if(c.lighting.viewC0!=c.geometry.viewC0||c.lighting.projection80!=c.geometry.projection80||
       c.lighting.screen140!=c.geometry.sourceScreen140||
       c.lighting.cameraPosition180!=c.geometry.cameraPosition180||
       c.lighting.fogEnd33Z!=c.geometry.fogEnd33Z||c.lighting.fogSlope33W!=c.geometry.fogSlope33W)
        return Status::input;
    Preamble p;auto g=c.geometry;
    //3377C0..337858: the single-matrix no-fog bit08 branch bypasses the
    // clip-aware program, even when04 is set. Radial-fog entry338318 instead
    // tests04 directly. Do not mutate the model's actual retained flag word.
    if(!(g.modelFlags1B2&2u)&&(g.modelFlags1B2&8u))
        g.modelFlags1B2=static_cast<std::uint8_t>(g.modelFlags1B2&~4u);
    auto s=garage_prelit50KG::prepare(g,p.geometry);if(s!=Status::complete)return s;
    BodyMatrix48G world;std::copy(g.modelWorld100.begin(),g.modelWorld100.end(),world.v);
    const auto l=prepareOriginalUiLitPreamble50KM(world,c.lighting,true,
        g.positionScale1E0,c.normalScale1E4,p.lighting);
    if(l!=OriginalUiLitGeometryStatus50KM::complete)return map(l);
    out=p;return Status::complete;
}
Status projectTriangle(const Preamble& p,const std::array<RawVertex,3>& raw,
    garage_prelit50KG::Polygon& out)noexcept{
    if(!original_racer_lighting50KM::selection_detail::arithmeticReady())return Status::arithmetic;
    std::array<garage_prelit50KG::PreparedVertex,3> vertices{};
    for(unsigned i=0;i<3;++i){
        garage_prelit50KG::RawVertex r;r.position=raw[i].position;r.uv=raw[i].uv;
        auto status=garage_prelit50KG::prepareVertex(p.geometry,r,vertices[i]);
        if(status!=Status::complete)return status;
        //337868..3378B8, or3384A8..3384F0: raw signed normal lanes,
        // qNormal already applied to the source light axes before the model
        // basis composition. No normalization or inverse-transpose substitute.
        std::array<float,3> amount{};
        for(unsigned light=0;light<3;++light){
            float n=mul(p.lighting.normalAxes[0][light],float(raw[i].normal[0]));
            n=add(n,mul(p.lighting.normalAxes[1][light],float(raw[i].normal[1])));
            n=add(n,mul(p.lighting.normalAxes[2][light],float(raw[i].normal[2])));
            if(!std::isfinite(n))return Status::arithmetic;
            amount[light]=std::max(0.f,n);
        }
        //337900..337978 and338548..338600: binary32 bias packing is
        // performed BEFORE the common clipper. Ambient alpha is a real fourth
        // lane, not255 or an imported authored-color replacement. The ordinary
        // and no-fog bit08 branches both use this same light-generated RGBA.
        for(unsigned lane=0;lane<4;++lane){
            float packed=add(p.lighting.colorRows[3][lane],8388608.f);
            for(unsigned light=0;light<3;++light){
                packed=add(packed,mul(p.lighting.colorRows[light][lane],amount[light]));
                if(!std::isfinite(packed))return Status::arithmetic;
            }
            packed=std::min(packed,8388863.f);
            if(packed<8388608.f)return Status::arithmetic;
            std::uint32_t bits=0;std::memcpy(&bits,&packed,sizeof bits);
            vertices[i].color[lane]=float(bits&255u);
        }
    }
    return garage_prelit50KG::projectPreparedTriangle(p.geometry,vertices,out);
}
}
