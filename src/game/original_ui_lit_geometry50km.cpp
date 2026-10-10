#include "original_ui_lit_geometry50km.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using Status=OriginalUiLitGeometryStatus50KM;
using Matrix=OriginalUiLitMatrix50KM;
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
bool finite(float x) noexcept {return std::isfinite(x);}
bool ready() noexcept {return original_racer_lighting50KM::selection_detail::arithmeticReady();}
bool finiteMatrix(const Matrix& m) noexcept {for(float x:m)if(!finite(x))return false;return true;}
Matrix compose(const Matrix& a,const Matrix& b) noexcept {
    Matrix result{};
    for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){
        float v=mul(b[c],a[r*4]);
        v=add(v,mul(b[4+c],a[r*4+1]));
        v=add(v,mul(b[8+c],a[r*4+2]));
        result[r*4+c]=add(v,mul(b[12+c],a[r*4+3]));
    }
    return result;
}
std::array<float,4> transform(const std::array<float,3>& p,const Matrix& m) noexcept {
    std::array<float,4> result{};
    for(unsigned c=0;c<4;++c){
        float v=mul(m[c],p[0]);v=add(v,mul(m[4+c],p[1]));
        v=add(v,mul(m[8+c],p[2]));result[c]=add(v,m[12+c]);
    }
    return result;
}
bool raw(double expanded,double scale,double lo,double hi,float& output) noexcept {
    if(!std::isfinite(expanded)||!std::isfinite(scale)||scale<=0)return false;
    const double value=expanded/scale;
    if(!std::isfinite(value)||value<lo||value>hi||std::trunc(value)!=value||value*scale!=expanded)return false;
    output=static_cast<float>(value);return double(output)==value;
}
}
original_racer_lighting50KM::Status projectOriginalUiGlobalLightRows50KM(
    bool hasDirectional40,const std::array<float,3>& look78,
    const std::array<float,4>& color3C,const std::array<float,4>& ambient50,
    original_racer_lighting50KM::LightMatrices& output) noexcept {
    using S=original_racer_lighting50KM::Status;
    for(float x:ambient50)if(!finite(x))return S::input;
    original_racer_lighting50KM::LightMatrices next;
    std::uint32_t sourceAlphaWord=0x40;float sourceAlpha=0;
    std::memcpy(&sourceAlpha,&sourceAlphaWord,sizeof(sourceAlpha));
    for(unsigned r=0;r<3;++r)next.diffuse[r][3]=sourceAlpha;
    next.directions[3]={{1,1,1,1}};
    if(hasDirectional40)for(unsigned c=0;c<3;++c){
        if(!finite(look78[c])||!finite(color3C[c]))return S::input;
        next.directions[c][0]=-look78[c];next.diffuse[0][c]=color3C[c];
    }
    next.diffuse[3]=ambient50;output=next;return S::complete;
}
Status prepareOriginalUiLitPreamble50KM(const BodyMatrix48G& nodeWorld,
    const OriginalUiLitContext50KM& context,bool compressed,float positionQ,float normalQ,
    OriginalUiLitPreamble50KM& output) noexcept {
    if(!ready())return Status::arithmetic;
    if(!finite(positionQ)||!finite(normalQ)||positionQ<=0||normalQ<=0||
        (!compressed&&(positionQ!=1||normalQ!=1))||
        !finiteMatrix(context.viewC0)||!finiteMatrix(context.projection80)||
        !finiteMatrix(context.clipProjection290)||!finiteMatrix(context.screen140)||
        context.canvasOffset[0]!=1728||context.canvasOffset[1]!=1824||
        !finite(context.fogEnd33Z)||!finite(context.fogSlope33W))return Status::input;
    Matrix world;std::copy(nodeWorld.v,nodeWorld.v+16,world.begin());
    if(!finiteMatrix(world)||world[3]!=0||world[7]!=0||world[11]!=0||world[15]!=1)return Status::input;
    for(float x:context.cameraPosition180)if(!finite(x))return Status::input;
    for(const auto& row:context.lights.directions)for(float x:row)if(!finite(x))return Status::input;
    for(const auto& row:context.lights.diffuse)for(float x:row)if(!finite(x))return Status::input;
    OriginalUiLitPreamble50KM next;
    //33FE38..33FEB0,33FEB8..33FF30,33FF38..33FFB0,33FFE8..340060.
    const Matrix modelView=compose(world,context.viewC0);
    const Matrix projected=compose(modelView,context.projection80);
    next.screen=compose(projected,context.screen140);
    next.clip=compose(modelView,context.clipProjection290);
    if(!finiteMatrix(modelView)||!finiteMatrix(projected)||!finiteMatrix(next.screen)||
        !finiteMatrix(next.clip))return Status::arithmetic;
    if(compressed)for(unsigned r=0;r<3;++r)for(unsigned c=0;c<4;++c){
        next.screen[r*4+c]=mul(next.screen[r*4+c],positionQ);
        next.clip[r*4+c]=mul(next.clip[r*4+c],positionQ);
    }
    //3400D0..340140 scales light rows BEFORE the node basis composition.
    for(unsigned a=0;a<3;++a)for(unsigned light=0;light<3;++light){
        float v=mul(mul(context.lights.directions[0][light],normalQ),world[a*4]);
        v=add(v,mul(mul(context.lights.directions[1][light],normalQ),world[a*4+1]));
        v=add(v,mul(mul(context.lights.directions[2][light],normalQ),world[a*4+2]));
        next.normalAxes[a][light]=add(v,mul(0.f,world[a*4+3]));
        if(!finite(next.normalAxes[a][light]))return Status::arithmetic;
    }
    next.colorRows=context.lights.diffuse;
    //340188..3401F0: local camera uses transpose basis dot, not inverse matrix.
    std::array<float,3> delta{};
    for(unsigned c=0;c<3;++c)delta[c]=sub(context.cameraPosition180[c],world[12+c]);
    for(unsigned a=0;a<3;++a){
        const float x=mul(delta[0],world[a*4]);
        const float y=mul(delta[1],world[a*4+1]);
        const float z=mul(delta[2],world[a*4+2]);
        // Original Y lane is Y+X+Z; Z lane is Z+X+Y.
        next.localCamera[a]=a==0?add(add(x,y),z):a==1?add(add(y,x),z):add(add(z,x),y);
        if(!finite(next.localCamera[a]))return Status::arithmetic;
    }
    next.fogEndTimesSlope=mul(context.fogEnd33Z,context.fogSlope33W);
    next.fogSlope=context.fogSlope33W;next.compressed=compressed;
    if(!finite(next.fogEndTimesSlope)||!finiteMatrix(next.screen)||!finiteMatrix(next.clip))return Status::arithmetic;
    output=next;return Status::complete;
}
Status projectOriginalUiLitVertex50KM(const OriginalUiLitPreamble50KM& in,
    const std::array<float,3>& position,const std::array<float,3>& normal,
    const std::array<float,2>& uv,bool radialFog,OriginalUiLitVertex50KM& output) noexcept {
    if(!ready())return Status::arithmetic;
    if(radialFog&&in.compressed)return Status::unsupportedFog;
    for(float x:position)if(!finite(x))return Status::input;
    for(float x:normal)if(!finite(x))return Status::input;
    if(in.compressed){
        for(float x:position)if(x < -8388608.f || x > 8388607.f || std::trunc(x)!=x)return Status::input;
        for(float x:normal)if(x < -128.f || x > 127.f || std::trunc(x)!=x)return Status::input;
    }
    for(float x:uv)if(!finite(x))return Status::input;
    if(!finiteMatrix(in.screen)||!finiteMatrix(in.clip))return Status::input;
    for(const auto& row:in.normalAxes)for(float x:row)if(!finite(x))return Status::input;
    for(const auto& row:in.colorRows)for(float x:row)if(!finite(x))return Status::input;
    const auto clip=transform(position,in.clip);
    for(float x:clip)if(!finite(x))return Status::arithmetic;
    if(clip[3]<=0||std::abs(clip[0])>clip[3]||std::abs(clip[1])>clip[3]||std::abs(clip[2])>clip[3])
        return Status::unsupportedClipping;
    const auto screen=transform(position,in.screen);
    for(float x:screen)if(!finite(x))return Status::arithmetic;
    if(screen[3]<=0)return Status::unsupportedClipping;
    const float q=div(1.f,screen[3]);
    const float x=mul(screen[0],q),y=mul(screen[1],q),z=mul(screen[2],q);
    if(!finite(q)||!finite(x)||!finite(y)||!finite(z))return Status::arithmetic;
    // Whole-inside GS screen extent and positive D24 only. No host clipping,
    // wrapping, Z clamp or invented near-plane behavior is substituted.
    if(x<0||x>4095||y<0||y>4095||z<0||z>16777215)return Status::unsupportedClipping;
    OriginalUiLitVertex50KM next;
    next.vertex.position[0]=sub(div(std::trunc(mul(x,16.f)),16.f),1728.f);
    next.vertex.position[1]=sub(div(std::trunc(mul(y,16.f)),16.f),1824.f);
    next.vertex.position[2]=std::trunc(z);next.rasterW=screen[3];
    for(unsigned c=0;c<2;++c)next.vertex.uv[c]=uv[c];
    std::array<float,3> amount{};
    for(unsigned light=0;light<3;++light){
        float v=mul(in.normalAxes[0][light],normal[0]);
        v=add(v,mul(in.normalAxes[1][light],normal[1]));
        v=add(v,mul(in.normalAxes[2][light],normal[2]));
        if(!finite(v))return Status::arithmetic;amount[light]=std::max(0.f,v);
    }
    // Plain323E68..323ED8 and compressed3378F8..337978 operate on ALL four
    // lanes. Actual ambient W supplies packet alpha; there is no white default.
    for(unsigned c=0;c<4;++c){
        float packed=add(in.colorRows[3][c],8388608.f);
        for(unsigned light=0;light<3;++light){
            packed=add(packed,mul(in.colorRows[light][c],amount[light]));
            if(!finite(packed))return Status::arithmetic;
        }
        packed=std::min(packed,8388863.f);
        if(packed<8388608.f)return Status::arithmetic;
        std::uint32_t bits=0;std::memcpy(&bits,&packed,sizeof(bits));
        next.vertex.color[c]=static_cast<std::uint8_t>(bits&255u);
    }
    next.fogFactor=255.f/256.f;
    if(radialFog){
        if(!finite(in.fogEndTimesSlope)||!finite(in.fogSlope))return Status::input;
        std::array<float,3> delta{};
        for(unsigned c=0;c<3;++c){
            if(!finite(in.localCamera[c]))return Status::input;
            delta[c]=sub(position[c],in.localCamera[c]);
        }
        float sum=add(mul(delta[0],delta[0]),mul(delta[1],delta[1]));
        sum=add(sum,mul(delta[2],delta[2]));
        if(!finite(sum)||sum<0)return Status::arithmetic;
        const float distance=std::sqrt(sum);
        float biased=add(sub(524288.f,in.fogEndTimesSlope),mul(distance,in.fogSlope));
        if(!finite(distance)||!finite(biased))return Status::arithmetic;
        biased=std::clamp(biased,524288.f,524543.f);
        std::uint32_t bits=0;std::memcpy(&bits,&biased,sizeof(bits));
        next.fogFactor=float((bits>>4)&255u)/256.f;
    }
    output=next;return Status::complete;
}
Status projectOriginalUiRiderBinderPacket50KM(const OriginalUiRiderBinderQuad50KG& quad,
    const OriginalUiLitContext50KM& context,const std::array<float,4>& ambient50,
    OriginalUiLitBinderPacket50KM& output) noexcept {
    for(float x:ambient50)if(!finite(x))return Status::input;
    OriginalUiLitContext50KM binder=context;
    binder.lights={};
    for(unsigned i=0;i<4;++i)binder.lights.directions[i][i]=1.f;
    binder.lights.diffuse[3]=ambient50;
    BodyMatrix48G world{};for(unsigned i=0;i<4;++i)world.v[i*4+i]=1.f;
    OriginalUiLitPreamble50KM preamble;
    auto status=prepareOriginalUiLitPreamble50KM(world,binder,false,1.f,1.f,preamble);
    if(status!=Status::complete)return status;
    std::array<OriginalUiLitVertex50KM,4> source;
    constexpr std::array<float,3> normal{{0,1,0}};
    constexpr float expectedUv[4][2]={{0,0},{0,1},{1,0},{1,1}};
    for(unsigned i=0;i<4;++i){
        if(quad.position[i][3]!=1.f||quad.uv[i][0]!=expectedUv[i][0]||
            quad.uv[i][1]!=expectedUv[i][1])return Status::input;
        const std::array<float,3> position{{quad.position[i][0],quad.position[i][1],quad.position[i][2]}};
        status=projectOriginalUiLitVertex50KM(preamble,position,normal,quad.uv[i],true,source[i]);
        if(status!=Status::complete)return status;
    }
    OriginalUiLitBinderPacket50KM next;
    constexpr unsigned order[8]={0,1,2,2,2,1,3,3};
    for(unsigned i=0;i<8;++i){
        const auto& v=source[order[i]];next.vertices[i]=v.vertex;
        next.rasterW[i]=v.rasterW;next.fogFactors[i]=v.fogFactor;
    }
    output=next;return Status::complete;
}
Status projectOriginalUiLitSceneSurface50KM(const assets::Scene48D& scene,
    std::uint32_t lod,std::uint32_t surfaceIndex,const ModelPalette49S& palette,
    const OriginalUiLitContext50KM& context,bool radialFog,
    OriginalUiLitSurfaceOutput50KM& output,OriginalUiLitGeometryWorkspace50KM& work) noexcept {
    if(!ready())return Status::arithmetic;
    if(radialFog)return Status::unsupportedFog;
    if(scene.world_space||!scene.identity_matrix_palette||scene.nodes.empty()||
        scene.nodes.size()>maxModelPaletteEntries49S||palette.entryCount!=scene.nodes.size()||
        lod>=scene.lods.size()||surfaceIndex>=scene.lods[lod].surfaces.size()||
        scene.quantization[2]!=scene.quantization[3]||scene.quantization[4]!=0||
        scene.quantization[5]!=0||scene.quantization[6]!=0)return Status::unsupportedScene;
    const auto& surface=scene.lods[lod].surfaces[surfaceIndex];
    const auto triangles=surface.indices.size()/3;
    if(surface.has_original_colors||surface.material_index>=scene.materials.size()||
        surface.vertices.empty()||surface.vertices.size()>originalUiLitMaximumSurfaceVertices50KM||
        !triangles||surface.indices.size()%3||triangles>originalUiLitMaximumTriangles50KM)return Status::unsupportedScene;
    if(!output.vertices||!output.rasterW||!output.fogFactors||output.slotCapacity<triangles)return Status::input;
    const float positionQ=static_cast<float>(scene.quantization[0]);
    const float normalQ=static_cast<float>(scene.quantization[1]);
    const float uvQ=static_cast<float>(scene.quantization[2]);
    if(double(positionQ)!=scene.quantization[0]||double(normalQ)!=scene.quantization[1]||
        double(uvQ)!=scene.quantization[2]||!finite(uvQ)||uvQ<=0)return Status::unsupportedScene;
    for(unsigned b=0;b<palette.entryCount;++b){
        const auto result=prepareOriginalUiLitPreamble50KM(palette.nodeWorld[b],context,true,
            positionQ,normalQ,work.palette[b]);
        if(result!=Status::complete)return result;
    }
    for(std::size_t v=0;v<surface.vertices.size();++v){
        const auto& source=surface.vertices[v];
        if(source.bone>=palette.entryCount)return Status::unsupportedScene;
        std::array<float,3> position{},normal{};std::array<float,2> uv{};
        for(unsigned c=0;c<3;++c)
            if(!raw(source.position_local[c],scene.quantization[0],-8388608,8388607,position[c])||
                !raw(source.normal_local[c],scene.quantization[1],-128,127,normal[c]))return Status::unsupportedScene;
        for(unsigned c=0;c<2;++c){
            float packed=0;
            if(!raw(source.uv[c],scene.quantization[2],-32768,32767,packed))return Status::unsupportedScene;
            uv[c]=mul(packed,uvQ);
        }
        const auto result=projectOriginalUiLitVertex50KM(work.palette[source.bone],position,normal,uv,
            false,work.vertices[v]);
        if(result!=Status::complete)return result;
    }
    for(auto index:surface.indices)if(index>=surface.vertices.size())return Status::unsupportedScene;
    for(std::size_t t=0;t<triangles;++t)for(unsigned corner=0;corner<4;++corner){
        const auto source=surface.indices[t*3+(corner==3?2:corner)];
        const auto target=t*4+corner;const auto& v=work.vertices[source];
        output.vertices[target]=v.vertex;output.rasterW[target]=v.rasterW;output.fogFactors[target]=v.fogFactor;
    }
    output.slotCount=static_cast<std::uint32_t>(triangles);output.materialIndex=surface.material_index;
    return Status::complete;
}
}
