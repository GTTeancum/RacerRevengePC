#include "original_garage_sphere_bounds50gn.h"
#include <cfenv>
#include <cmath>

namespace rr::game {
namespace {
bool number(float v)noexcept {return std::isfinite(v)&&std::fpclassify(v)!=FP_SUBNORMAL;}
struct Math {
    OriginalGarageSphereDiagnostic50GN& diagnostic;
    bool valid=true;
    float checked(float v,std::uint32_t pc)noexcept {
        if(valid&&!number(v)){valid=false;diagnostic.sourcePC=pc;}
        return v;
    }
    float add(float a,float b,std::uint32_t pc)noexcept {
        checked(a,pc);checked(b,pc);if(!valid)return 0;
        volatile float v=a+b;return checked(v,pc);
    }
    float sub(float a,float b,std::uint32_t pc)noexcept {
        checked(a,pc);checked(b,pc);if(!valid)return 0;
        volatile float v=a-b;return checked(v,pc);
    }
    float mul(float a,float b,std::uint32_t pc)noexcept {
        checked(a,pc);checked(b,pc);if(!valid)return 0;
        volatile float v=a*b;return checked(v,pc);
    }
};
void transform(const BodyMatrix48G& matrix,const std::array<float,4>& p,
    std::array<float,4>& out,Math& m,std::uint32_t pc)noexcept {
    float a[3];
    for(unsigned c=0;c<3;++c)a[c]=m.mul(matrix.v[c],p[0],pc);
    for(unsigned c=0;c<3;++c)a[c]=m.add(a[c],m.mul(matrix.v[4+c],p[1],pc+4),pc+4);
    for(unsigned c=0;c<3;++c)a[c]=m.add(a[c],m.mul(matrix.v[8+c],p[2],pc+8),pc+8);
    for(unsigned c=0;c<3;++c)out[c]=m.add(a[c],matrix.v[12+c],pc+12);
    out[3]=1.f;
}
void inversePoint(const BodyMatrix48G& source,const std::array<float,4>& p,
    std::array<float,4>& out,Math& m)noexcept {
    BodyMatrix48G inverse=source;
    for(unsigned row=0;row<3;++row)for(unsigned c=0;c<4;++c)
        inverse.v[4*row+c]=m.add(source.v[4*row+c],0.f,0x28be3cu+row*4);
    inverse.v[4]=m.add(0.f,source.v[1],0x28be48u);
    inverse.v[8]=m.add(0.f,source.v[2],0x28be4cu);
    inverse.v[9]=m.add(0.f,source.v[6],0x28be50u);
    inverse.v[1]=m.add(0.f,source.v[4],0x28be54u);
    inverse.v[2]=m.add(0.f,source.v[8],0x28be58u);
    inverse.v[6]=m.add(0.f,source.v[9],0x28be5cu);
    for(unsigned c=0;c<4;++c)inverse.v[12+c]=m.add(source.v[12+c],0.f,0x28be68u);
    float a[3];
    for(unsigned c=0;c<3;++c)a[c]=m.mul(inverse.v[c],source.v[12],0x28be6cu);
    for(unsigned c=0;c<3;++c)a[c]=m.add(a[c],m.mul(inverse.v[4+c],source.v[13],0x28be70u),0x28be70u);
    for(unsigned c=0;c<3;++c)a[c]=m.add(a[c],m.mul(inverse.v[8+c],source.v[14],0x28be74u),0x28be74u);
    const float negativeOne=m.sub(0.f,1.f,0x28be7cu);
    for(unsigned c=0;c<3;++c)inverse.v[12+c]=m.mul(a[c],negativeOne,0x28be80u);
    transform(inverse,p,out,m,0x28beb0u);
}
bool index(const SceneGraph49P& graph,SceneNodeIndex49P i)noexcept {
    const auto n=static_cast<std::int32_t>(i);
    return n>=0&&static_cast<std::uint32_t>(n)<graph.count;
}
}
bool computeOriginalGarageSphereBounds26F4B050GN(const SceneGraph49P& graph,
    const OriginalGarageSphereRow50GN& root,const OriginalGarageSphereRow50GN* rows,
    std::uint32_t count,OriginalGarageSphereShared50GN& shared,
    OriginalGarageSphereResult50GN& output,OriginalGarageSphereDiagnostic50GN& d)noexcept {
    d={};d.sourceRow=-1;
    if(std::fegetround()!=FE_TONEAREST||!validateSceneGraph49P(graph)||!index(graph,root.node)||
       count>64u||(count&&!rows)||!(graph.nodes[static_cast<unsigned>(root.node)].sceneFlags190&0x10u)){
        d.sourcePC=0x26f4c0u;return false;
    }
    for(std::uint32_t i=0;i<count;++i){
        if(!index(graph,rows[i].node)||rows[i].node==root.node){d.sourcePC=0x26f560u;d.sourceRow=i;return false;}
        for(std::uint32_t j=0;j<i;++j)if(rows[i].node==rows[j].node){d.sourcePC=0x26f560u;d.sourceRow=i;return false;}
        auto parent=graph.nodes[static_cast<unsigned>(rows[i].node)].parent180;
        while(parent!=SceneNodeIndex49P::none&&parent!=root.node)
            parent=graph.nodes[static_cast<unsigned>(parent)].parent180;
        if(parent!=root.node){d.sourcePC=0x26f560u;d.sourceRow=i;return false;}
    }
    if(!shared.lazy8B5C){shared.assertion442F60=-1;shared.lazy8B5C=true;}
    if(!refreshSceneWorld49P(graph,root.node)){d.sourcePC=0x26f4f8u;return false;}
    auto& rootNode=graph.nodes[static_cast<unsigned>(root.node)];
    Math math{d};std::array<float,4> center{{0,0,0,1}};float radius=0;
    if(rootNode.sceneFlags190&4u){
        transform(rootNode.world100,root.center140,center,math,0x107150u);
        if(!math.valid)return false;
        if(!root.radius194||!number(*root.radius194)){d.sourcePC=0x26f538u;return false;}
        radius=*root.radius194;
    }
    for(std::uint32_t i=0;i<count;++i){
        d.sourceRow=static_cast<std::int32_t>(i);
        const auto& row=rows[i];auto& node=graph.nodes[static_cast<unsigned>(row.node)];
        if(!(node.sceneFlags190&4u))continue;
        const bool replace=radius<=0.f;
        if(replace){
            if(!row.radius194||!number(*row.radius194)){d.sourcePC=0x28da64u;return false;}
            radius=*row.radius194;
        }
        if(!refreshSceneWorld49P(graph,row.node)){d.sourcePC=replace?0x28da70u:0x28d9a0u;return false;}
        std::array<float,4> point{};transform(node.world100,row.center140,point,math,0x107150u);
        if(!math.valid)return false;
        if(replace){center=point;continue;}
        float extent=std::fabs(math.sub(point[0],center[0],0x28d9d0u));
        const float y=std::fabs(math.sub(point[1],center[1],0x28d9ecu));
        if(!(y<=extent))extent=y;
        const float z=std::fabs(math.sub(point[2],center[2],0x28da1cu));
        if(!(z<=extent))extent=z;
        if(!row.radius194||!number(*row.radius194)){d.sourcePC=0x28da40u;return false;}
        const float candidate=math.add(extent,*row.radius194,0x28da48u);
        if(!math.valid)return false;
        if(!(candidate<=radius))radius=candidate;
    }
    d.sourceRow=-1;
    //28BD50 internally requests this same root world again. It is ready unless
    // an unsupported external mutation occurred; there is no callback here.
    if(!refreshSceneWorld49P(graph,root.node)){d.sourcePC=0x28bd50u;return false;}
    OriginalGarageSphereResult50GN result{};
    inversePoint(rootNode.world100,center,result.center160,math);
    if(!math.valid)return false;
    result.radius198=radius;result.rootSceneFlags190=rootNode.sceneFlags190;
    if(!(radius<=0.f))result.rootSceneFlags190|=0x40u;
    output=result;d.sourcePC=0x26f5e0u;return true;
}
}
