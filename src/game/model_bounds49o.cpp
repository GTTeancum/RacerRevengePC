#include "model_bounds49o.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
std::uint32_t bits(float x) noexcept {std::uint32_t u;std::memcpy(&u,&x,4);return u;}
float fromBits(std::uint32_t u) noexcept {float x;std::memcpy(&x,&u,4);return x;}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return keep(x);}
    float root(float a) noexcept {
        if((bits(a)&0x7f800000u)==0)return fromBits(bits(a)&0x80000000u);
        volatile float x=static_cast<float>(std::sqrt(static_cast<double>(a)));
        return keep(x);
    }
    BodyMatrix48G product(const BodyMatrix48G& a,const BodyMatrix48G& b) noexcept {
        BodyMatrix48G o{};
        for(unsigned r=0;r<4;++r)for(unsigned j=0;j<4;++j)
            o.v[r*4+j]=add(add(add(mul(b.v[j],a.v[r*4]),mul(b.v[4+j],a.v[r*4+1])),mul(b.v[8+j],a.v[r*4+2])),mul(b.v[12+j],a.v[r*4+3]));
        return o;
    }
    Vector3_48C direction(const Vector3_48C& p,const BodyMatrix48G& m) noexcept {
        Vector3_48C o{};
        for(unsigned j=0;j<3;++j)o.v[j]=add(add(mul(m.v[j],p.v[0]),mul(m.v[4+j],p.v[1])),mul(m.v[8+j],p.v[2]));
        return o;
    }
    Vector3_48C point(const Vector3_48C& p,const BodyMatrix48G& m) noexcept {
        auto o=direction(p,m);for(unsigned j=0;j<3;++j)o.v[j]=add(o.v[j],m.v[12+j]);return o;
    }
    Vector3_48C toRoot(Vector3_48C p,const BodyMatrix48G& sourceRoot,bool point) noexcept {
        //28DC68..28DC78 (and each following inverse helper) clears vf30,
        //then VADDs +0 to every root row before transpose/subtraction.
        //This canonicalizes -0 under the admitted nearest rounding mode.
        BodyMatrix48G root=sourceRoot;
        for(float& lane:root.v)lane=add(lane,0.0f);
        if(point)for(unsigned j=0;j<3;++j)p.v[j]=sub(p.v[j],root.v[12+j]);
        Vector3_48C o{};
        for(unsigned j=0;j<3;++j)o.v[j]=add(add(mul(root.v[4*j],p.v[0]),mul(root.v[4*j+1],p.v[1])),mul(root.v[4*j+2],p.v[2]));
        return o;
    }
};
bool affine(const BodyMatrix48G& m) noexcept {
    for(float x:m.v)if(!std::isfinite(x))return false;
    return m.v[3]==0&&m.v[7]==0&&m.v[11]==0&&m.v[15]==1;
}
bool validBox(const Vector3_48C& c,const Vector3_48C& e) noexcept {
    for(unsigned j=0;j<3;++j)if(!std::isfinite(c.v[j])||!std::isfinite(e.v[j])||e.v[j]<0)return false;
    return true;
}
float minimum(float a,float b) noexcept {
    const auto ai=static_cast<std::int32_t>(bits(a)),bi=static_cast<std::int32_t>(bits(b));
    return ai<0&&bi<0?(ai>bi?a:b):(ai<bi?a:b);
}
float maximum(float a,float b) noexcept {
    const auto ai=static_cast<std::int32_t>(bits(a)),bi=static_cast<std::int32_t>(bits(b));
    return ai<0&&bi<0?(ai<bi?a:b):(ai>bi?a:b);
}
void merge(Math& m,ModelBoundsResult49O& o,const Vector3_48C& c,const Vector3_48C& e) noexcept {
    for(unsigned j=0;j<3;++j){const float lo=m.sub(c.v[j],e.v[j]),hi=m.add(c.v[j],e.v[j]);o.minimum[j]=o.seeded?minimum(o.minimum[j],lo):lo;o.maximum[j]=o.seeded?maximum(o.maximum[j],hi):hi;}
    o.seeded=true;
}
}
namespace {
bool computeBounds(const ModelBoundsNode49O* nodes,std::size_t count,
    const ModelBoundsContext49O& context,ModelBoundsWorkspace49O& workspace,
    ModelBoundsResult49O& output,bool directRoot) noexcept {
    if(std::fegetround()!=FE_TONEAREST||count>maxModelBoundsNodes49O||(count&&!nodes)||(context.rootSceneFlags190&0x10u)||!affine(context.rootWorld100))return false;
    if((context.rootSceneFlags190&4u)&&!validBox(context.rootCenter140,context.rootHalfExtent150))return false;
    for(std::size_t i=0;i<count;++i){const auto& n=nodes[i];if(n.parent<-1||n.parent>=static_cast<std::int32_t>(i)||!affine(n.local))return false;if((n.sceneFlags190&4u)&&!validBox(n.center140,n.halfExtent150))return false;}
    if(directRoot){
        if(!count||!(context.rootSceneFlags190&4u)||nodes[0].parent!=-1||
           nodes[0].sceneFlags190!=context.rootSceneFlags190)return false;
        for(unsigned j=0;j<3;++j)
            if(bits(nodes[0].center140.v[j])!=bits(context.rootCenter140.v[j])||
               bits(nodes[0].halfExtent150.v[j])!=bits(context.rootHalfExtent150.v[j]))return false;
        for(std::size_t i=1;i<count;++i)if(nodes[i].parent<0)return false;
    }
    Math m;ModelBoundsResult49O o{};o.minimum[3]=o.maximum[3]=o.center160[3]=o.halfExtent170[3]=1;
    if(context.rootSceneFlags190&4u)merge(m,o,context.rootCenter140,context.rootHalfExtent150);
    for(std::size_t i=0;i<count;++i){const auto& n=nodes[i];
        if(directRoot&&i==0)workspace.world[i]=context.rootWorld100;
        else workspace.world[i]=m.product(n.local,n.parent<0?context.rootWorld100:workspace.world[n.parent]);
        if(!(n.sceneFlags190&4u))continue;
        const auto center=m.toRoot(m.point(n.center140,workspace.world[i]),context.rootWorld100,true);
        Vector3_48C axes[3]{},extent{};
        for(unsigned axis=0;axis<3;++axis){Vector3_48C v{};v.v[axis]=n.halfExtent150.v[axis];axes[axis]=m.toRoot(m.direction(v,workspace.world[i]),context.rootWorld100,false);}
        for(unsigned j=0;j<3;++j)extent.v[j]=m.add(std::fabs(axes[2].v[j]),m.add(std::fabs(axes[0].v[j]),std::fabs(axes[1].v[j])));
        merge(m,o,center,extent);
    }
    for(unsigned j=0;j<3;++j){o.center160[j]=m.mul(.5f,m.add(o.minimum[j],o.maximum[j]));o.halfExtent170[j]=m.mul(.5f,m.sub(o.maximum[j],o.minimum[j]));}
    const float x=m.mul(o.halfExtent170[0],o.halfExtent170[0]),y=m.mul(o.halfExtent170[1],o.halfExtent170[1]);
    const float xy=m.add(x,y),xyz=m.add(xy,m.mul(o.halfExtent170[2],o.halfExtent170[2]));o.radius198=m.root(xyz);
    o.rootSceneFlags190=static_cast<std::uint8_t>(context.rootSceneFlags190|1u|(o.radius198>0?0x40u:0u));
    if(!m.valid)return false;
    output=o;return true;
}
}
bool computeModelBounds49O(const ModelBoundsNode49O* nodes,std::size_t count,
    const ModelBoundsContext49O& context,ModelBoundsWorkspace49O& workspace,
    ModelBoundsResult49O& output) noexcept {
    return computeBounds(nodes,count,context,workspace,output,false);
}
bool computeDirectRootModelBounds50KG(const ModelBoundsNode49O* sourceRows,
    std::size_t count,const ModelBoundsContext49O& context,
    ModelBoundsWorkspace49O& workspace,ModelBoundsResult49O& output) noexcept {
    return computeBounds(sourceRows,count,context,workspace,output,true);
}
}
