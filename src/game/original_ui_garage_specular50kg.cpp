#include "original_ui_garage_specular50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace rr::game::garage_specular50KG {
namespace {
float add(float a,float b)noexcept{volatile float r=a+b;return r;}
float mul(float a,float b)noexcept{volatile float r=a*b;return r;}
Status map(OriginalUiLitGeometryStatus50KM s)noexcept{
    return s==OriginalUiLitGeometryStatus50KM::complete?Status::complete:
        s==OriginalUiLitGeometryStatus50KM::arithmetic?Status::arithmetic:Status::input;
}
}
Status prepare(const Context& c,Preamble& out)noexcept{
    // Both helpers consume the same authentic matrix fields. Requiring exact
    // equality prevents a caller from lighting one pose and projecting another.
    if(c.lighting.viewC0!=c.geometry.viewC0||c.lighting.projection80!=c.geometry.projection80||
       c.lighting.screen140!=c.geometry.sourceScreen140||
       c.lighting.cameraPosition180!=c.geometry.cameraPosition180||
       c.lighting.fogEnd33Z!=c.geometry.fogEnd33Z||c.lighting.fogSlope33W!=c.geometry.fogSlope33W)
        return Status::input;
    Preamble p;auto g=c.geometry;
    // Specular's original packet always emits F255, independently of material
    // FGE and model radial bit. Source clipping bits01/04 remain actual.
    g.modelFlags1B2=static_cast<std::uint8_t>(g.modelFlags1B2&~2u);
    auto s=garage_prelit50KG::prepare(g,p.geometry);if(s!=Status::complete)return s;
    BodyMatrix48G world;std::copy(g.modelWorld100.begin(),g.modelWorld100.end(),world.v);
    auto l=prepareOriginalUiLitPreamble50KM(world,c.lighting,true,g.positionScale1E0,c.normalScale1E4,p.lighting);
    if(l!=OriginalUiLitGeometryStatus50KM::complete)return map(l);
    l=prepareOriginalUiSpecularNormalAxes50KM(world,g.viewC0,c.normalScale1E4,p.normalUV);
    if(l!=OriginalUiLitGeometryStatus50KM::complete)return map(l);
    out=p;return Status::complete;
}
Status projectTriangle(const Preamble& p,const std::array<RawVertex,3>& raw,
    garage_prelit50KG::Polygon& out)noexcept{
    if(!original_racer_lighting50KM::selection_detail::arithmeticReady())return Status::arithmetic;
    std::array<garage_prelit50KG::PreparedVertex,3> v{};
    for(unsigned i=0;i<3;++i){
        garage_prelit50KG::RawVertex r;r.position=raw[i].position;
        auto status=garage_prelit50KG::prepareVertex(p.geometry,r,v[i]);if(status!=Status::complete)return status;
        //33BA58..33BA68 and33BAB0/33BAD0: completed modelView axes were
        // scaled by qNormal before the dot; .5 + .5*N, no normalisation.
        for(unsigned lane=0;lane<2;++lane){
            float n=mul(p.normalUV[0][lane],float(raw[i].normal[0]));
            n=add(n,mul(p.normalUV[1][lane],float(raw[i].normal[1])));
            n=add(n,mul(p.normalUV[2][lane],float(raw[i].normal[2])));
            v[i].uvq[lane]=add(mul(n,.5f),.5f);
        }
        v[i].uvq[2]=1;v[i].fog=255;
        std::array<float,3> amount{};
        for(unsigned light=0;light<3;++light){
            float n=mul(p.lighting.normalAxes[0][light],float(raw[i].normal[0]));
            n=add(n,mul(p.lighting.normalAxes[1][light],float(raw[i].normal[1])));
            n=add(n,mul(p.lighting.normalAxes[2][light],float(raw[i].normal[2])));
            if(!std::isfinite(n))return Status::arithmetic;
            amount[light]=std::max(0.f,n);
        }
        //33BAA8..33BB38: same binary32-bias packed light equation as the
        // existing generic Specular leaf, but BEFORE source clipper entry.
        for(unsigned c=0;c<4;++c){
            float packed=add(p.lighting.colorRows[3][c],8388608.f);
            for(unsigned light=0;light<3;++light){
                packed=add(packed,mul(p.lighting.colorRows[light][c],amount[light]));
                if(!std::isfinite(packed))return Status::arithmetic;
            }
            packed=std::min(packed,8388863.f);if(packed<8388608.f)return Status::arithmetic;
            std::uint32_t bits=0;std::memcpy(&bits,&packed,4);v[i].color[c]=float(bits&255u);
        }
    }
    return garage_prelit50KG::projectPreparedTriangle(p.geometry,v,out);
}
}
