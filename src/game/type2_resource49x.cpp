#include "type2_resource49x.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Status=Type2ResourceStatus49X;
struct Extent { std::uintptr_t first=0,last=0; };
bool extent(const void* p,std::size_t n,std::size_t size,std::size_t alignment,Extent& e) noexcept {
    const auto start=reinterpret_cast<std::uintptr_t>(p);
    if((n&&!p)||(n&&start%alignment)||n>std::numeric_limits<std::size_t>::max()/size)return false;
    const auto bytes=n*size;
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-start)return false;
    e={start,start+bytes};return true;
}
bool overlap(const Extent& a,const Extent& b) noexcept {
    return a.first<a.last&&b.first<b.last&&a.first<b.last&&b.first<a.last;
}
template<class T> bool range(const T* p,std::size_t n,Extent& e) noexcept {return extent(p,n,sizeof(T),alignof(T),e);}
bool separate(const Extent* writable,std::size_t nw,const Extent* inputs,std::size_t ni) noexcept {
    for(std::size_t i=0;i<nw;++i){
        for(std::size_t j=0;j<i;++j)if(overlap(writable[i],writable[j]))return false;
        for(std::size_t j=0;j<ni;++j)if(overlap(writable[i],inputs[j]))return false;
    }return true;
}
float fp(std::uint32_t x) noexcept {float f;std::memcpy(&f,&x,4);return f;}
std::uint32_t bits(float x) noexcept {std::uint32_t b;std::memcpy(&b,&x,4);return b;}
float add(float a,float b) noexcept {volatile float f=a+b;return f;}
float sub(float a,float b) noexcept {volatile float f=a-b;return f;}
float mul(float a,float b) noexcept {volatile float f=a*b;return f;}
float div(float a,float b) noexcept {volatile float f=a/b;return f;}
std::uint32_t u32(const std::uint8_t* p) noexcept {return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);}
std::uint16_t u16(const std::uint8_t* p) noexcept {return std::uint16_t(p[0]|(std::uint16_t(p[1])<<8));}
float readf(const std::uint8_t* p) noexcept {return fp(u32(p));}
bool affine(const float* m) noexcept {
    for(unsigned i=0;i<16;++i)if(!std::isfinite(m[i]))return false;
    return m[3]==0.f&&m[7]==0.f&&m[11]==0.f&&m[15]==1.f;
}
bool finite3(const float* v) noexcept {return std::isfinite(v[0])&&std::isfinite(v[1])&&std::isfinite(v[2]);}
bool half3(const float* h) noexcept {return finite3(h)&&h[0]>=0.f&&h[1]>=0.f&&h[2]>=0.f;}
bool radius(const float* h,float& out) noexcept {
    // ADDA.S xx,yy; MADD.S zz, followed by scalar positive RSQRT.S.
    const float square=add(add(mul(h[0],h[0]),mul(h[1],h[1])),mul(h[2],h[2]));
    if(!std::isfinite(square))return false;
    if(square==0.f){out=0.f;return true;}
    // EE scalar RSQRT input exponent-zero handling, independently checked
    // against retained ee_sqrt14.h in the test. Exceptional NaN/Inf excluded.
    volatile float inverse=(bits(square)&0x7f800000u)==0?fp(0x7f7fffffu):
        static_cast<float>(1.0/std::sqrt(static_cast<double>(square)));
    out=div(1.f,inverse);return std::isfinite(out);
}
bool quantizeReconstruct(float x,float& out) noexcept {
    // global444500 is initialized as separately rounded 1/32767; the
    // reconstruction literal38000100 has the same bits. Do not cancel products.
    const float scaled=mul(fp(0x38000100u),x);
    const float biased=add(.5f,mul(32767.f,scaled));
    if(!std::isfinite(biased))return false;
    // EE CVT.W.S truncates/saturates; SH then LH retain/sign-extend low16.
    const auto integer=biased>=2147483648.f?std::numeric_limits<std::int32_t>::max():
        (biased<=-2147483648.f?std::numeric_limits<std::int32_t>::min():static_cast<std::int32_t>(biased));
    const auto low=static_cast<std::uint32_t>(integer)&0xffffu;
    const auto signedLow=low>=0x8000u?static_cast<std::int32_t>(low)-65536:static_cast<std::int32_t>(low);
    out=mul(32767.f,mul(fp(0x38000100u),static_cast<float>(signedLow)));
    return std::isfinite(out);
}
bool node(const std::uint8_t* p,Type2Node49X& out) noexcept {
    out.volume=readf(p);
    if(!std::isfinite(out.volume)||out.volume<0.f)return false;
    std::array<float,3> adjusted{};
    for(unsigned i=0;i<3;++i){
        const float center=readf(p+4+4*i),half=readf(p+16+4*i);
        if(!std::isfinite(center)||!std::isfinite(half)||half<0.f)return false;
        if(!quantizeReconstruct(center,out.center[i]))return false;
        const float error=std::fabs(sub(out.center[i],center));
        float initial;
        if(!quantizeReconstruct(add(half,error),initial))return false;
        // Compare ORIGINAL authored half, not center-error-inflated input.
        adjusted[i]=initial<half?add(initial,1.f):initial;
        if(!quantizeReconstruct(adjusted[i],out.half[i]))return false;
    }
    // Wrapped negative extents are not an admitted native box. Ordinary
    // out-of-range integer wrapping remains modeled for center coordinates.
    if(!half3(out.half.data())||!half3(adjusted.data())||!radius(adjusted.data(),out.radius))return false;
    out.children[0]=u16(p+28);out.children[1]=u16(p+30);return true;
}
// Each record stores parent index+1. Unique-parent and reachability checks
// reject DAGs, cycles, orphan components and duplicate leaves. Cold-only O(ND)
// depth validation avoids recursion or another work span (D <= nodeCount).
template<class Child> bool topology(std::size_t nn,std::size_t nl,Child child,
    std::uint32_t* parents,std::size_t& depth) noexcept {
    for(std::size_t i=0;i<nn+nl;++i)parents[i]=0;
    for(std::size_t i=0;i<nn;++i)for(unsigned side=0;side<2;++side){
        const auto tag=child(i,side);const bool leaf=(tag&0x80000000u)!=0;
        const auto index=tag&0x7fffffffu;
        if(index>=(leaf?nl:nn))return false;
        const auto record=leaf?nn+index:index;
        if(record==0||parents[record])return false;
        parents[record]=static_cast<std::uint32_t>(i+1);
    }
    depth=0;
    for(std::size_t i=1;i<nn+nl;++i){
        if(!parents[i])return false;
        std::size_t d=0,r=i;
        while(r){if(!parents[r]||++d>nn)return false;r=parents[r]-1;}
        if(d>depth)depth=d;
    }return true;
}
bool counts(std::size_t nn,std::size_t nl) noexcept {return nn>0&&nn<=32767&&nl==nn+1&&nl<=32768;}
}
static_assert(sizeof(Type2Node49X)==36,"Original node size");
static_assert(offsetof(Type2Node49X,center)==12&&offsetof(Type2Node49X,half)==24,"Original node fields");
static_assert(sizeof(Type2Ray49X)==32,"Original ray size");

Status decodeType2Resource49X(const std::uint8_t* bytes,std::size_t byteCount,
    const Type2OpaqueEndpointWords49X* opaque,std::size_t opaqueCount,
    Type2Node49X* nodes,std::size_t nodeCapacity,Type2Ray49X* rays,
    std::size_t rayCapacity,const Type2ResourceWorkspace49X& ws,Type2ResourceView49X& output) noexcept {
    Extent w[4],in[3];
    if(!range(nodes,nodeCapacity,w[0])||!range(rays,rayCapacity,w[1])||!range(ws.topology,ws.topologyCapacity,w[2])||
       !range(&output,1,w[3])||!range(bytes,byteCount,in[0])||!range(opaque,opaqueCount,in[1])||!range(&ws,1,in[2])||
       !separate(w,4,in,3))return Status::InvalidInput;
    if(std::fegetround()!=FE_TONEAREST||byteCount<92||u32(bytes)!=1234u||u32(bytes+4)!=0x006c6f63u||
       u32(bytes+8)!=1||u32(bytes+12)!=2)return Status::InvalidInput;
    const std::size_t nr=u32(bytes+80),nn=u32(bytes+84),nl=u32(bytes+88);
    if(!counts(nn,nl)||nr!=nl||byteCount!=92+32*nn+24*nl)return Status::InvalidInput;
    if(nodeCapacity<nn||rayCapacity<nr||opaqueCount<nr||ws.topologyCapacity<nn+nl)return Status::InsufficientCapacity;
    Type2ResourceView49X result{};
    for(unsigned i=0;i<16;++i)result.localMatrix.v[i]=readf(bytes+16+4*i);
    if(!affine(result.localMatrix.v))return Status::InvalidInput;
    for(std::size_t i=0;i<nn;++i){Type2Node49X n;if(!node(bytes+92+32*i,n))return Status::InvalidInput;}
    const auto* leaves=bytes+92+32*nn;
    for(std::size_t i=0;i<nr*6;++i)if(!std::isfinite(readf(leaves+4*i)))return Status::InvalidInput;
    const auto children=[&](std::size_t i,unsigned side) noexcept {
        const auto tag=u16(bytes+92+32*i+28+2*side);
        return static_cast<std::uint32_t>(tag&0x7fffu)|((tag&type2Leaf49X)?0x80000000u:0u);
    };
    if(!topology(nn,nl,children,ws.topology,result.maxDepth))return Status::InvalidInput;
    // No fallible operation remains. Recompute each bounded node using the
    // exact arithmetic already admitted, then commit all outputs and metadata.
    for(std::size_t i=0;i<nn;++i){Type2Node49X n;node(bytes+92+32*i,n);nodes[i]=n;}
    for(std::size_t i=0;i<nr;++i){
        for(unsigned j=0;j<3;++j){rays[i].first.v[j]=readf(leaves+24*i+4*j);rays[i].second.v[j]=readf(leaves+24*i+12+4*j);}
        std::memcpy(&rays[i].first.v[3],&opaque[i].first,4);std::memcpy(&rays[i].second.v[3],&opaque[i].second,4);
    }
    result.nodes=nodes;result.nodeCount=nn;result.rays=rays;result.rayCount=nr;result.root=0;output=result;
    return Status::Complete;
}
Status validateType2Resource49X(const Type2ResourceView49X& view,const Type2ResourceWorkspace49X& ws,std::size_t& maxDepthOut) noexcept {
    Extent w[2],in[4];
    if(!range(ws.topology,ws.topologyCapacity,w[0])||!range(&maxDepthOut,1,w[1])||!range(&view,1,in[0])||
       !range(&ws,1,in[1])||!range(view.nodes,view.nodeCount,in[2])||!range(view.rays,view.rayCount,in[3])||
       !separate(w,2,in,4))return Status::InvalidInput;
    if(std::fegetround()!=FE_TONEAREST||view.root!=0||!counts(view.nodeCount,view.rayCount)||!affine(view.localMatrix.v))return Status::InvalidInput;
    if(ws.topologyCapacity<view.nodeCount+view.rayCount)return Status::InsufficientCapacity;
    for(std::size_t i=0;i<view.nodeCount;++i){const auto& n=view.nodes[i];
        if(!std::isfinite(n.volume)||n.volume<0.f||!std::isfinite(n.radius)||n.radius<0.f||!finite3(n.center.data())||!half3(n.half.data()))return Status::InvalidInput;
    }
    for(std::size_t i=0;i<view.rayCount;++i)if(!finite3(view.rays[i].first.v)||!finite3(view.rays[i].second.v))return Status::InvalidInput;
    const auto children=[&](std::size_t i,unsigned side) noexcept {const auto t=view.nodes[i].children[side];return std::uint32_t(t&0x7fffu)|((t&type2Leaf49X)?0x80000000u:0u);};
    std::size_t depth=0;
    if(!topology(view.nodeCount,view.rayCount,children,ws.topology,depth)||depth!=view.maxDepth)return Status::InvalidInput;
    maxDepthOut=depth;return Status::Complete;
}
Status prepareStaticTargetRadii49X(const assets::CollisionMesh48I& mesh,float* radii,
    std::size_t radiusCapacity,const Type2ResourceWorkspace49X& ws,std::size_t& maxDepth) noexcept {
    Extent w[3],in[6];
    if(!range(radii,radiusCapacity,w[0])||!range(ws.topology,ws.topologyCapacity,w[1])||!range(&maxDepth,1,w[2])||
       !range(&mesh,1,in[0])||!range(&ws,1,in[1])||!range(mesh.nodes.data(),mesh.nodes.size(),in[2])||
       !range(mesh.faces.data(),mesh.faces.size(),in[3])||!range(mesh.vertices.data(),mesh.vertices.size(),in[4])||
       !range(mesh.vertex_normals.data(),mesh.vertex_normals.size(),in[5])||!separate(w,3,in,6))return Status::InvalidInput;
    const auto nn=mesh.nodes.size(),nf=mesh.faces.size(),nv=mesh.vertices.size();
    if(std::fegetround()!=FE_TONEAREST||!counts(nn,nf)||!nv||nv>65535||mesh.reverse_mode!=0||
       !affine(mesh.local_matrix.data())||(!mesh.vertex_normals.empty()&&mesh.vertex_normals.size()!=nv))return Status::InvalidInput;
    if(radiusCapacity<nn||ws.topologyCapacity<nn+nf)return Status::InsufficientCapacity;
    for(const auto& v:mesh.vertices)if(!finite3(v.data()))return Status::InvalidInput;
    for(const auto& n:mesh.vertex_normals)if(!finite3(n.data()))return Status::InvalidInput;
    for(const auto& f:mesh.faces){if(!std::isfinite(f.d))return Status::InvalidInput;for(auto v:f.vertices)if(v>=nv)return Status::InvalidInput;}
    for(const auto& n:mesh.nodes){float r;
        if(!std::isfinite(n.volume)||n.volume<0.f||!finite3(n.center.data())||!half3(n.half.data())||!radius(n.half.data(),r))return Status::InvalidInput;
    }
    const auto children=[&](std::size_t i,unsigned side) noexcept {return mesh.nodes[i].children[side];};
    std::size_t depth=0;if(!topology(nn,nf,children,ws.topology,depth))return Status::InvalidInput;
    for(std::size_t i=0;i<nn;++i)radius(mesh.nodes[i].half.data(),radii[i]);
    maxDepth=depth;
    return Status::Complete;
}
}
