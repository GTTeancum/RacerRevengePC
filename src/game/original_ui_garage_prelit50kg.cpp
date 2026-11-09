#include "original_ui_garage_prelit50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game::garage_prelit50KG {
namespace {
#if defined(_MSC_VER)&&!defined(__clang__)&&!defined(RR_GARAGE_VOLATILE_KERNELS)
// MSVC /fp:strict (the Windows build) already rounds every float operation to
// IEEE single, in source order, without reassociation; fp_contract(off) also
// rules out fused multiply-add. A volatile spill adds nothing but memory
// traffic there. Other compilers keep the volatile barrier.
#pragma fp_contract(off)
float add(float a,float b) noexcept {return a+b;}
float sub(float a,float b) noexcept {return a-b;}
float mul(float a,float b) noexcept {return a*b;}
float div(float a,float b) noexcept {return a/b;}
#else
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
#endif
bool ready() noexcept {
    return sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
        std::numeric_limits<float>::digits==24&&std::fegetround()==FE_TONEAREST;
}
template<std::size_t N> bool finite(const std::array<float,N>& a) noexcept {
    for(float x:a)if(!std::isfinite(x))return false;
    return true;
}
Matrix compose(const Matrix& a,const Matrix& b) noexcept {
    Matrix c{};
    for(unsigned r=0;r<4;++r)for(unsigned k=0;k<4;++k){
        float v=mul(b[k],a[r*4]);v=add(v,mul(b[4+k],a[r*4+1]));
        v=add(v,mul(b[8+k],a[r*4+2]));c[r*4+k]=add(v,mul(b[12+k],a[r*4+3]));
    }return c;
}
std::array<float,4> transform(const std::array<std::int16_t,3>& p,const Matrix& m) noexcept {
    std::array<float,4> r{};
    for(unsigned c=0;c<4;++c){
        float x=mul(m[c],float(p[0]));x=add(x,mul(m[4+c],float(p[1])));
        x=add(x,mul(m[8+c],float(p[2])));r[c]=add(x,m[12+c]);
    }return r;
}
bool screenForm(const Matrix& m) noexcept {
    if(!finite(m)||m[15]!=1)return false;
    for(unsigned r=0;r<3;++r)for(unsigned c=0;c<4;++c)
        if(c!=r&&m[r*4+c]!=0)return false;
    return true;
}
using Operand=PreparedVertex;
bool finite(const Operand& v) noexcept {
    return finite(v.clip)&&finite(v.screen)&&finite(v.color)&&finite(v.uvq)&&std::isfinite(v.fog);
}
std::array<float,4> screenFromClip(const std::array<float,4>& c,const Matrix& s) noexcept {
    // Row43/44 diagonal-scale/offset operation,3397C0..E8 /367120..128.
    return {{add(mul(c[0],s[0]),mul(s[12],c[3])),
             add(mul(c[1],s[5]),mul(s[13],c[3])),
             add(mul(c[2],s[10]),mul(s[14],c[3])),c[3]}};
}
Status radial(const Preamble& p,const RawVertex& raw,std::uint8_t& out) noexcept {
    std::array<float,3> d{};
    for(unsigned a=0;a<3;++a)d[a]=sub(float(raw.position[a]),p.localCameraRaw[a]);
    float sum=add(mul(d[0],d[0]),mul(d[1],d[1]));sum=add(sum,mul(d[2],d[2]));
    if(!std::isfinite(sum)||sum<0)return Status::arithmetic;
    // Native sqrt is intentionally not a VU EFU/denormal/accumulator emulator.
    const float length=std::sqrt(sum);
    float biased=add(sub(524288.f,p.endTimesUnscaledSlope),mul(length,p.scaledSlope));
    if(!std::isfinite(biased))return Status::arithmetic;
    biased=std::clamp(biased,524288.f,524543.f);
    std::uint32_t bits=0;std::memcpy(&bits,&biased,4);
    // MTIR followed by mask0xFF0 at3665B8/366658 drops the sub-byte bits.
    out=static_cast<std::uint8_t>((bits>>4)&255u);return Status::complete;
}
Status operand(const Preamble& p,const RawVertex& raw,Operand& out) noexcept {
    Operand v;v.clip=transform(raw.position,p.ordinaryClip);
    v.screen=(p.modelFlags1B2&4)?screenFromClip(v.clip,p.sourceScreen140):
        transform(raw.position,p.combinedScreen);
    for(unsigned c=0;c<4;++c)v.color[c]=float(raw.rgba[c]);
    //339798 /339A60 /339DE0: BOTH signed UV lanes use row42.z.
    v.uvq={{mul(float(raw.uv[0]),p.uvScale),mul(float(raw.uv[1]),p.uvScale),1.f}};
    std::uint8_t f=255;
    if(p.modelFlags1B2&2){const auto s=radial(p,raw,f);if(s!=Status::complete)return s;}
    v.fog=float(f);
    if(!finite(v))return Status::arithmetic;
    out=v;return Status::complete;
}
float distance(const Operand& v,unsigned plane) noexcept {
    const auto axis=2-plane/2;
    return plane%2?sub(v.clip[3],v.clip[axis]):add(v.clip[3],v.clip[axis]);
}
std::uint8_t outside(const Operand& v) noexcept {
    std::uint8_t mask=0;
    for(unsigned a=0;a<3;++a){
        if(v.clip[a]>v.clip[3])mask|=std::uint8_t(1u<<(a*2));
        if(v.clip[a]<-v.clip[3])mask|=std::uint8_t(2u<<(a*2));
    }return mask;
}
Status intersection(const Operand& a,const Operand& b,float da,float db,bool aInside,Operand& out) noexcept {
    const float denominator=sub(da,db);
    if(!std::isfinite(denominator)||denominator==0)return Status::arithmetic;
    const float q=div(aInside?da:db,denominator);const auto& origin=aInside?a:b;
    Operand n;
    // Original distinct entering/leaving endpoint forms367330/367380.
    for(unsigned c=0;c<4;++c){
        n.clip[c]=add(origin.clip[c],mul(sub(b.clip[c],a.clip[c]),q));
        n.color[c]=add(origin.color[c],mul(sub(b.color[c],a.color[c]),q));
    }
    for(unsigned c=0;c<3;++c)n.uvq[c]=add(origin.uvq[c],mul(sub(b.uvq[c],a.uvq[c]),q));
    n.fog=add(origin.fog,mul(sub(b.fog,a.fog),q));
    if(!std::isfinite(q)||!finite(n))return Status::arithmetic;
    out=n;return Status::complete;
}
Status pack(const Operand& v,const Matrix& screen,bool clipped,PacketVertex& out) noexcept {
    if(!finite(v)||v.clip[3]<=0)return Status::unsupportedPacketRange;
    const auto xyzw=clipped?screenFromClip(v.clip,screen):v.screen;
    if(!finite(xyzw)||xyzw[3]<=0)return Status::unsupportedPacketRange;
    const float q=div(1.f,xyzw[3]);PacketVertex n;n.rasterW=xyzw[3];
    for(unsigned a=0;a<3;++a){
        const float value=mul(xyzw[a],q);
        if(!std::isfinite(value)||value<0||value>(a==2?16777215.f:4095.f))
            return Status::unsupportedPacketRange;
        // XYZF FTOI4. Native output separates Z's integer24 from XY's12.4.
        n.gsPosition[a]=a==2?std::trunc(value):div(std::trunc(mul(value,16.f)),16.f);
        n.stq[a]=mul(v.uvq[a],q);
    }
    for(unsigned c=0;c<4;++c){
        const float color=clipped?mul(v.color[c],q):v.color[c];
        // No invented clamp: valid convex source byte interpolation stays in
        // this range; exceptional native arithmetic is a surfaced boundary.
        if(!std::isfinite(color)||color<0||color>255)return Status::unsupportedPacketRange;
        n.rgba[c]=static_cast<std::uint8_t>(std::trunc(color));
    }
    if(!std::isfinite(v.fog)||v.fog<0||v.fog>255||!finite(n.stq))return Status::arithmetic;
    n.fogByte=static_cast<std::uint8_t>(std::uint32_t(std::trunc(mul(v.fog,16.f)))>>4);
    out=n;return Status::complete;
}
}
Status prepare(const Context& c,Preamble& out) noexcept {
    if(!ready())return Status::arithmetic;
    if(!finite(c.modelWorld100)||!finite(c.viewC0)||!finite(c.projection80)||
        !screenForm(c.sourceScreen140)||!finite(c.cameraPosition180)||
        c.modelWorld100[3]!=0||c.modelWorld100[7]!=0||c.modelWorld100[11]!=0||c.modelWorld100[15]!=1||
        !std::isfinite(c.positionScale1E0)||c.positionScale1E0<=0||
        !std::isfinite(c.uvScale1E8)||c.uvScale1E8<=0||
        !std::isfinite(c.fogEnd33Z)||!std::isfinite(c.fogSlope33W))return Status::input;
    Preamble p;p.modelFlags1B2=c.modelFlags1B2;p.sourceScreen140=c.sourceScreen140;p.uvScale=c.uvScale1E8;
    //33FE38..33FF30 and33FFF0..340008: ordinary projection, NOT guard290.
    const auto mv=compose(c.modelWorld100,c.viewC0);p.ordinaryClip=compose(mv,c.projection80);
    p.combinedScreen=compose(p.ordinaryClip,c.sourceScreen140);
    for(unsigned r=0;r<3;++r)for(unsigned k=0;k<4;++k){
        p.ordinaryClip[r*4+k]=mul(p.ordinaryClip[r*4+k],c.positionScale1E0);
        p.combinedScreen[r*4+k]=mul(p.combinedScreen[r*4+k],c.positionScale1E0);
    }
    const float iq=div(1.f,c.positionScale1E0);std::array<float,3> d{};
    for(unsigned a=0;a<3;++a)d[a]=sub(c.cameraPosition180[a],c.modelWorld100[12+a]);
    for(unsigned a=0;a<3;++a){
        const float x=mul(d[0],c.modelWorld100[a*4]),y=mul(d[1],c.modelWorld100[a*4+1]),z=mul(d[2],c.modelWorld100[a*4+2]);
        const float v=a==0?add(add(x,y),z):a==1?add(add(y,x),z):add(add(z,x),y);
        p.localCameraRaw[a]=mul(v,iq);
    }
    //3401F8 computes end*slope BEFORE340210 scales slope and340218 camera.
    p.endTimesUnscaledSlope=mul(c.fogEnd33Z,c.fogSlope33W);
    p.scaledSlope=mul(c.fogSlope33W,c.positionScale1E0);
    if(!finite(mv)||!finite(p.ordinaryClip)||!finite(p.combinedScreen)||!finite(p.localCameraRaw)||
        !std::isfinite(iq)||!std::isfinite(p.endTimesUnscaledSlope)||!std::isfinite(p.scaledSlope))return Status::arithmetic;
    out=p;return Status::complete;
}
Status prepareVertex(const Preamble& p,const RawVertex& raw,PreparedVertex& out) noexcept {
    if(!ready())return Status::arithmetic;
    if(!finite(p.ordinaryClip)||!finite(p.combinedScreen)||!screenForm(p.sourceScreen140)||
        !finite(p.localCameraRaw)||!std::isfinite(p.uvScale)||p.uvScale<=0||
        !std::isfinite(p.endTimesUnscaledSlope)||!std::isfinite(p.scaledSlope))return Status::input;
    return operand(p,raw,out);
}
Status projectPreparedTriangle(const Preamble& p,const std::array<PreparedVertex,3>& raw,Polygon& out) noexcept {
    if(!ready())return Status::arithmetic;
    if(!finite(p.ordinaryClip)||!finite(p.combinedScreen)||!screenForm(p.sourceScreen140)||
        !finite(p.localCameraRaw)||!std::isfinite(p.uvScale)||p.uvScale<=0||
        !std::isfinite(p.endTimesUnscaledSlope)||!std::isfinite(p.scaledSlope))return Status::input;
    std::array<Operand,9> polygon{},next{};Polygon result;std::uint8_t any=0,common=63;
    for(unsigned i=0;i<3;++i){
        if(!finite(raw[i]))return Status::arithmetic;
        polygon[i]=raw[i];
        const auto mask=outside(polygon[i]);any|=mask;common&=mask;
    }
    const bool clip=(p.modelFlags1B2&4)&&any;
    if(clip&&(common||!(p.modelFlags1B2&1))){out=result;return Status::complete;}
    unsigned count=3;
    if(clip){
        result.sourceTriangleFan=true;
        // Original packet STQ is restored before clipping (366678). Keep the
        // separately-rounded multiply/divide round trip, including Q lane.
        for(unsigned i=0;i<count;++i){
            if(polygon[i].clip[3]==0)return Status::unsupportedPacketRange;
            const float q=div(1.f,polygon[i].clip[3]);
            for(auto& uv:polygon[i].uvq)uv=mul(mul(uv,q),polygon[i].clip[3]);
            if(!finite(polygon[i]))return Status::arithmetic;
        }
        //366870..3670B8: -Z,+Z,-Y,+Y,-X,+X. Every plane rotates an
        // all-inside polygon. Preserve boundary duplicate intersections.
        for(unsigned plane=0;plane<6;++plane){
            unsigned n=0;
            for(unsigned i=0;i<count;++i){
                const auto& a=polygon[(i+count-1)%count];const auto& b=polygon[i];
                const float da=distance(a,plane),db=distance(b,plane);
                if(!std::isfinite(da)||!std::isfinite(db))return Status::arithmetic;
                const bool ai=da>=0,bi=db>=0;
                if(ai){if(n==next.size())return Status::capacity;next[n++]=a;}
                if(ai!=bi){
                    if(n==next.size())return Status::capacity;
                    const auto s=intersection(a,b,da,db,ai,next[n]);if(s!=Status::complete)return s;++n;
                }
            }
            count=n;polygon=next;
            if(!count){out=result;return Status::complete;}
            //3669C0 AFTER near clipping. Fog/UV are never W-weighted here.
            if(plane==0)for(unsigned i=0;i<count;++i)for(auto& color:polygon[i].color)
                color=mul(color,polygon[i].clip[3]);
        }
    }
    if(count<3){out=result;return Status::complete;}
    for(unsigned i=0;i<count;++i){
        const auto s=pack(polygon[i],p.sourceScreen140,clip,result.vertices[i]);
        if(s!=Status::complete)return s;
    }
    result.count=count;out=result;return Status::complete;
}
Status projectTriangle(const Preamble& p,const std::array<RawVertex,3>& raw,Polygon& out) noexcept {
    if(!ready())return Status::arithmetic;
    if(!finite(p.ordinaryClip)||!finite(p.combinedScreen)||!screenForm(p.sourceScreen140)||
        !finite(p.localCameraRaw)||!std::isfinite(p.uvScale)||p.uvScale<=0||
        !std::isfinite(p.endTimesUnscaledSlope)||!std::isfinite(p.scaledSlope))return Status::input;
    std::array<PreparedVertex,3> operands;
    for(unsigned i=0;i<3;++i){
        const auto s=operand(p,raw[i],operands[i]);if(s!=Status::complete)return s;
    }
    return projectPreparedTriangle(p,operands,out);
}
}
