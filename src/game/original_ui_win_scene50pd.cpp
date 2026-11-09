#include "original_ui_win_scene50pd.h"
#include "original_ui_win_page50pd.h"
#include "original_racer_light_selection50km.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <new>
#include <utility>

namespace rr::game {
namespace {
using D=OriginalUiWinSceneDiagnostic50PD;using I=SceneNodeIndex49P;using LS=OriginalUiLitGeometryStatus50KM;
float add(float a,float b)noexcept{volatile float r=a+b;return r;}
float sub(float a,float b)noexcept{volatile float r=a-b;return r;}
float mul(float a,float b)noexcept{volatile float r=a*b;return r;}
float div(float a,float b)noexcept{volatile float r=a/b;return r;}
bool fail(D& d,std::uint32_t pc)noexcept{d.sourcePC=pc;return false;}
bool finite(const std::array<float,16>& m)noexcept{for(float f:m)if(!std::isfinite(f))return false;return true;}
bool authentic(const assets::OriginalUiArchive50KG& a)noexcept{
    return a.loaded()&&a.identity()==assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::win);}
assets::TVCameraVector49E vector(const std::array<float,3>& v)noexcept{return {v[0],v[1],v[2],1};}
std::array<float,16> product(const std::array<float,16>& a,const std::array<float,16>& b)noexcept{
    std::array<float,16> o{};
    for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){float v=mul(b[c],a[r*4]);v=add(v,mul(b[4+c],a[r*4+1]));
        v=add(v,mul(b[8+c],a[r*4+2]));o[r*4+c]=add(v,mul(b[12+c],a[r*4+3]));}
    return o;
}
bool normalize(std::array<float,3>& v)noexcept{
    const float sq=add(add(mul(v[0],v[0]),mul(v[1],v[1])),mul(v[2],v[2]));
    if(!std::isfinite(sq)||sq<=0)return false;volatile float inv=1.0f/std::sqrt(sq);
    for(float& f:v){f=mul(f,inv);if(!std::isfinite(f))return false;}return true;
}
bool raw(double expanded,double scale,double lo,double hi,float& out)noexcept{
    if(!std::isfinite(expanded)||!std::isfinite(scale)||scale<=0)return false;
    const double value=expanded/scale;
    if(!std::isfinite(value)||value<lo||value>hi||std::trunc(value)!=value||value*scale!=expanded)return false;
    out=static_cast<float>(value);return double(out)==value;
}
}
bool prepareOriginalUiWinCamera50PD(const assets::OriginalUiArchive50KG& archive,OriginalUiWinCamera50PD& out,D& d) noexcept{
    d={};if(!authentic(archive)||std::fegetround()!=FE_TONEAREST)return fail(d,0x1D7A90);
    try{
        assets::OriginalUiDat50KG page;
        if(!assets::readOriginalUiDat50KG(archive,"UI/UIWin/win.dat",page,d.parse,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,0x1D7A90);
        const auto* f=page.section("PageInfo");if(!f)return fail(d,0x1D7A90);
        OriginalUiWinCamera50PD r;r.pageIdentity=page.identity();
        if(!assets::originalUiTriple50KG(*f,"CameraPosition1",{{0,0,0}},r.position,d.parse,0x1D7B74)||
           !assets::originalUiTriple50KG(*f,"CameraLook1",{{0,-1,0}},r.look,d.parse,0x1D7BB0)||
           !assets::originalUiTriple50KG(*f,"CameraUp1",{{0,0,1}},r.up,d.parse,0x1D7BE8)||
           !assets::originalUiNumber50KG(*f,"CameraFOV1",35,r.fovDegrees,d.parse,0x1D7C10)||
           !assets::originalUiNumber50KG(*f,"CameraRoll1",0,r.rollRadians,d.parse,0x1D7C34))return fail(d,0x1D7B74);
        const std::uint32_t aspectBits=0x3faaaaabu;std::memcpy(&r.aspect1DC,&aspectBits,4);
        const float hw=mul(.5f,640),hh=mul(.5f,448);
        const float left=sub(2048,hw),right=add(2048,hw),top=sub(2048,hh),bottom=add(2048,hh);
        r.aspect1F8=div(div(sub(bottom,top),sub(right,left)),mul(r.aspect1DC,div(448,640)));
        CameraSceneState49F prior;prior.aspect1F8=r.aspect1F8;prior.rollRadians=r.rollRadians;
        TVCameraUpdateOutput49F input;input.mode=0;input.position=vector(r.position);
        input.look=vector(r.look);input.up=vector(r.up);input.fov=r.fovDegrees;
        if(!composeCameraView49F(prior,input,r.core))return fail(d,0x1D7C50);
        if(prepareOriginalUiParticleCameraFromView50KG(r.core,640,448,r.sourceScreen)!=OriginalUiMainParticleCameraStatus50KG::ok)return fail(d,0x236DD0);
        out=r;return true;
    }catch(...){return fail(d,0x1D7A90);}
}
bool prepareOriginalUiWinRenderContext50PD(const assets::OriginalUiArchive50KG& archive,const OriginalUiWinCamera50PD& camera,
    OriginalUiWinRenderContext50PD& out,D& d) noexcept{
    d={};if(!authentic(archive)||camera.pageIdentity!=archive.find("UI/UIWin/win.dat").identity||
        std::fegetround()!=FE_TONEAREST||!finite(camera.core.view)||!finite(camera.core.projection))return fail(d,0x1CE7B0);
    try{
        assets::OriginalUiDat50KG ui;
        if(!assets::readOriginalUiDat50KG(archive,"UI/UIWin/ui.dat",ui,d.parse,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,0x1CE7F4);
        const auto* fog=ui.section("Fog");if(!fog)return fail(d,0x1CE7F4);
        OriginalUiWinRenderContext50PD r;r.fogIdentity=ui.identity();
        if(!assets::originalUiNumber50KG(*fog,"Visibility",1,r.visibility,d.parse,0x1CE81C)||
           !assets::originalUiNumber50KG(*fog,"Haziness",.5f,r.haziness,d.parse,0x1CE8BC)||
           !assets::originalUiTriple50KG(*fog,"ColorRGB",{},r.sourceFogRgb,d.parse,0x1CE964))return fail(d,0x1CE81C);
        if(r.visibility<0||r.visibility>1)r.visibility=1; //1CE840 (1CE8E0 rechecks Visibility)
        r.fogEnd=add(900,mul(r.visibility,sub(1200,900)));
        r.fogStart=mul(sub(1,r.haziness),sub(r.fogEnd,200));
        r.packetFogStart33Z=r.fogEnd;const float range=sub(r.fogStart,r.fogEnd);
        r.packetFogScale33W=range==0?0:div(255,range);
        if(!std::isfinite(r.fogEnd)||r.fogEnd<=1||!std::isfinite(r.fogStart)||!std::isfinite(r.packetFogScale33W))return fail(d,0x13E830);
        for(float f:r.sourceFogRgb)if(!std::isfinite(f)||f<0||f>255)return fail(d,0x1CE964);
        r.cameraAfterFog=camera;auto& core=r.cameraAfterFog.core;
        core.sceneAfter.nearPlane=1;core.sceneAfter.farPlane=r.fogEnd>100000?100000:r.fogEnd;
        const float z=div(1,sub(1,div(core.sceneAfter.nearPlane,core.sceneAfter.farPlane)));
        core.projection[10]=z;core.projection[14]=mul(-z,core.sceneAfter.nearPlane);
        core.combined=product(core.view,core.projection);
        if(!finite(core.combined)||prepareOriginalUiParticleCameraFromView50KG(core,640,448,r.cameraAfterFog.sourceScreen)!=
            OriginalUiMainParticleCameraStatus50KG::ok)return fail(d,0x13EA30);
        // 237240 guard factors and 236EC0/237090 clipping projection.
        const auto& screen=r.cameraAfterFog.sourceScreen.sourceScreen140;
        const float x=std::fabs(screen[0]),y=std::fabs(screen[5]);
        const float ex=add(screen[12],x),ey=add(screen[13],y);
        const float rx=std::min(ex,sub(4095,ex)),ry=std::min(ey,sub(4095,ey));
        if(!(x>0&&y>0&&rx>0&&ry>0))return fail(d,0x237240);
        const float gx=div(x,rx),gy=div(y,ry);r.clipProjection290=core.projection;
        for(unsigned row=0;row<4;++row){r.clipProjection290[4*row]=mul(r.clipProjection290[4*row],gx);
            r.clipProjection290[4*row+1]=mul(r.clipProjection290[4*row+1],gy);}
        if(!finite(r.clipProjection290))return fail(d,0x236EC0);
        out=r;return true;
    }catch(...){return fail(d,0x1CE7B0);}
}
bool prepareOriginalUiWinLighting50PD(const assets::OriginalUiArchive50KG& archive,OriginalUiWinLighting50PD& out,D& d) noexcept{
    d={};if(!authentic(archive)||std::fegetround()!=FE_TONEAREST)return fail(d,0x1CDB10);
    try{
        assets::OriginalUiDat50KG ui;
        if(!assets::readOriginalUiDat50KG(archive,"UI/UIWin/ui.dat",ui,d.parse,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,0x1CDB10);
        const auto* lights=ui.section("Lights");const auto* first=ui.section("Light1");const auto* second=ui.section("Light2");std::int32_t count=0;
        if(!lights||!first||!second||!assets::originalUiInteger50KG(*lights,"NumberOfLights",2,count,d.parse,0x1CDB4C)||count!=2)return fail(d,0x1CDB4C);
        const auto* t1=first->find("Type");const auto* t2=second->find("Type");
        if(!t1||!t2||t1->text!="Ambient"||t2->text!="Directional")return fail(d,0x1CE014);
        std::array<float,3> ambient{},directional{},position{},target{};
        if(!first->find("ColorRGB")||!second->find("ColorRGB")||!second->find("Position")||!second->find("TargetPosition")||second->find("LookVector")||
           !assets::originalUiTriple50KG(*first,"ColorRGB",{},ambient,d.parse,0x1CE094)||
           !assets::originalUiTriple50KG(*second,"ColorRGB",{},directional,d.parse,0x1CE094)||
           !assets::originalUiTriple50KG(*second,"Position",{},position,d.parse,0x1CE0D4)||
           !assets::originalUiTriple50KG(*second,"TargetPosition",{},target,d.parse,0x1CE2C0))return fail(d,0x1CE094);
        OriginalUiWinLighting50PD r;r.uiIdentity=ui.identity();
        for(unsigned i=0;i<3;++i){
            if(ambient[i]<0||ambient[i]>255||directional[i]<0||directional[i]>255)return fail(d,0x146FDC);
            r.ambient50[i]=mul(ambient[i],.5f);r.directional3C[i]=mul(directional[i],.5f);r.directionalLook78[i]=sub(target[i],position[i]);}
        r.ambient50[3]=64;r.directional3C[3]=127.5f; //148FF4..148FFC
        if(!normalize(r.directionalLook78)||!normalize(r.directionalLook78))return fail(d,0x147200);
        out=r;return true;
    }catch(...){return fail(d,0x1CDB10);}
}
bool prepareOriginalUiWinLitContext50PD(const OriginalUiWinRenderContext50PD& render,const OriginalUiWinLighting50PD& lights,
    OriginalUiLitContext50KM& out) noexcept{
    if(!render.fogIdentity||render.fogIdentity!=lights.uiIdentity)return false;
    OriginalUiLitContext50KM lit;
    if(projectOriginalUiGlobalLightRows50KM(true,lights.directionalLook78,lights.directional3C,lights.ambient50,lit.lights)!=
        original_racer_lighting50KM::Status::complete)return false;
    lit.viewC0=render.cameraAfterFog.core.view;lit.projection80=render.cameraAfterFog.core.projection;
    lit.clipProjection290=render.clipProjection290;lit.screen140=render.cameraAfterFog.sourceScreen.sourceScreen140;
    lit.cameraPosition180=render.cameraAfterFog.position;lit.fogEnd33Z=render.packetFogStart33Z;
    lit.fogSlope33W=render.packetFogScale33W;lit.canvasOffset=render.sourceCanvasOffset;
    out=lit;return true;
}

struct OriginalUiWinModel50PD::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    std::uint16_t widget=0;OriginalUiWinRotateConfig50PD config;
    assets::OriginalUiMemberView50KG member{};assets::PsgHierarchyView49O hierarchy{};assets::Scene48D scene;
    std::array<SceneNode49P,2> nodes{};std::array<I,1> sources{{static_cast<I>(1)}};std::array<std::uint8_t,1> palette{{0}};
    bool attached=false;
    SceneGraph49P view()noexcept{return {nodes.data(),2};}
    bool build()noexcept{
        assets::PsgNode49O source{};if(!assets::readPsgNode49O(hierarchy,0,source)||source.parent!=-1)return false;
        nodes={};auto& root=nodes[0];root.sceneFlags190=0x80; //2893A0 local identity, world zero, dirty80
        if(!originalUiUserRotatePose50KG(config.position,config.look,config.up,root.localC0))return false;
        auto& node=nodes[1];node.sceneFlags190=0xa4;std::memcpy(node.localC0.v,source.local,sizeof source.local);
        node.parent180=static_cast<I>(0);root.firstChild184=static_cast<I>(1);
        return validateSceneGraph49P(view());
    }
};
OriginalUiWinModel50PD::OriginalUiWinModel50PD() noexcept=default;
OriginalUiWinModel50PD::~OriginalUiWinModel50PD()=default;
bool OriginalUiWinModel50PD::load(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,std::uint16_t widget,
    const OriginalUiWinRotateConfig50PD& config,D& d) noexcept{
    d={};d.widget=widget;if(storage_)return fail(d,0x1D3590);
    if(!archive||!authentic(*archive)||widget<9||widget>14)return fail(d,0x1D3590);
    try{
        auto s=std::make_unique<Storage>();s->archive=std::move(archive);s->widget=widget;s->config=config;
        s->member=s->archive->find("UI/Geometry/"+config.stem+".psg");
        if(!s->member.identity||!s->member.data)return fail(d,0x1D35C4);
        if(!assets::readPsgHierarchy49O(s->member.data,s->member.size,s->hierarchy,d.hierarchy)||s->hierarchy.nodeCount!=1)return fail(d,0x1D35C4);
        const std::vector<std::uint8_t> bytes(s->member.data,s->member.data+s->member.size);
        if(!assets::decodeScene48D(bytes,s->scene,d.geometry)||s->scene.nodes.size()!=1||s->scene.world_space||
           !s->scene.identity_matrix_palette||s->scene.lods.size()!=1||s->scene.lods[0].surfaces.empty())return fail(d,0x1D35C4);
        for(const auto& surface:s->scene.lods[0].surfaces)if(surface.has_original_colors||surface.material_index>=s->scene.materials.size())return fail(d,0x1D35C4);
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,0x1D3590);}
}
bool OriginalUiWinModel50PD::loaded()const noexcept{return bool(storage_);}
std::uint16_t OriginalUiWinModel50PD::widgetId()const noexcept{return storage_?storage_->widget:0;}
const assets::Scene48D* OriginalUiWinModel50PD::scene()const noexcept{return storage_?&storage_->scene:nullptr;}
const assets::OriginalUiMemberIdentity50KG* OriginalUiWinModel50PD::geometrySource()const noexcept{return storage_?storage_->member.identity:nullptr;}
bool OriginalUiWinModel50PD::attach(D& d) noexcept{
    d={};if(!storage_)return fail(d,0x1D3590);d.widget=storage_->widget;
    if(!storage_->build())return fail(d,0x2A8C90);storage_->attached=true;return true;
}
void OriginalUiWinModel50PD::release() noexcept{if(storage_){storage_->attached=false;storage_->nodes={};}}
bool OriginalUiWinModel50PD::attached()const noexcept{return storage_&&storage_->attached;}
bool OriginalUiWinModel50PD::advance(float dt,D& d) noexcept{
    d={};if(!storage_||!storage_->attached)return fail(d,0x1D38B0);auto& s=*storage_;d.widget=s.widget;
    OriginalUiUserRotateParameters50KG p;p.enabled=s.config.rotate;p.revolutionsPerSecond=s.config.velocity;p.position=s.config.position;
    return advanceOriginalUiUserRotate50KG(p,true,s.config.startPaused,dt,s.view(),static_cast<I>(0))||fail(d,0x1D38B0);
}
bool OriginalUiWinModel50PD::projectPalette(ModelPalette49S& out,D& d) noexcept{
    d={};if(!storage_||!storage_->attached)return fail(d,0x2412E4);auto& s=*storage_;d.widget=s.widget;
    ModelPaletteInput49S input{};input.phase=ModelPalettePhase49S::selectedRenderPreparation;
    input.suppliedState=ModelPaletteSceneState49S::completeCurrentScene;input.modelFlags191=1;input.modelRoot=static_cast<I>(0);
    input.sourceNodes=s.sources.data();input.sourceNodeCount=1;input.paletteIds=s.palette.data();input.paletteCount=1;
    auto scratch=std::make_unique<ModelPaletteWorkspace49S>();
    return projectModelPalette49S(s.view(),input,out,*scratch,d.palette)||fail(d,0x2412E4);
}
bool OriginalUiWinModel50PD::radialFogClear(const OriginalUiWinRenderContext50PD& render,D& d)const noexcept{
    d={};if(!storage_||!storage_->attached)return fail(d,0x241320);const auto& s=*storage_;d.widget=s.widget;
    const auto& root=s.nodes[0].localC0;const auto& cam=render.cameraAfterFog.position;
    const double dx=double(root.v[12])-cam[0],dy=double(root.v[13])-cam[1],dz=double(root.v[14])-cam[2];
    const double radius=s.scene.nodes[0].stored_bounds[6];
    const double reach=std::sqrt(dx*dx+dy*dy+dz*dz)+std::fabs(radius);
    // Conservative native admission: only a sphere that cannot reach fog start.
    return (std::isfinite(reach)&&reach<double(render.fogStart))||fail(d,0x241320);
}
bool OriginalUiWinModel50PD::project(std::uint32_t surfaceIndex,bool specular,const ModelPalette49S& palette,
    const OriginalUiLitContext50KM& context,OriginalUiLitSurfaceOutput50KM& output,OriginalUiLitGeometryWorkspace50KM& work,D& d)const noexcept{
    d={};const auto status=[&](LS s){d.lit=s;d.sourcePC=0x240C00;return false;};
    if(!storage_||!storage_->attached)return status(LS::input);d.widget=storage_->widget;
    if(!original_racer_lighting50KM::selection_detail::arithmeticReady())return status(LS::arithmetic);
    const auto& scene=storage_->scene;
    if(scene.world_space||!scene.identity_matrix_palette||scene.nodes.size()!=1||palette.entryCount!=1||
       scene.lods.size()!=1||surfaceIndex>=scene.lods[0].surfaces.size()||scene.quantization[2]!=scene.quantization[3]||
       scene.quantization[4]!=0||scene.quantization[5]!=0||scene.quantization[6]!=0)return status(LS::unsupportedScene);
    const auto& surface=scene.lods[0].surfaces[surfaceIndex];const auto triangles=surface.indices.size()/3;
    if(surface.has_original_colors||surface.material_index>=scene.materials.size()||surface.vertices.empty()||
       surface.vertices.size()>originalUiLitMaximumSurfaceVertices50KM||!triangles||surface.indices.size()%3||
       triangles>originalUiLitMaximumTriangles50KM)return status(LS::unsupportedScene);
    if(!output.vertices||!output.rasterW||!output.fogFactors||output.slotCapacity<triangles)return status(LS::input);
    const float positionQ=static_cast<float>(scene.quantization[0]),normalQ=static_cast<float>(scene.quantization[1]),uvQ=static_cast<float>(scene.quantization[2]);
    if(double(positionQ)!=scene.quantization[0]||double(normalQ)!=scene.quantization[1]||double(uvQ)!=scene.quantization[2]||!std::isfinite(uvQ)||uvQ<=0)
        return status(LS::unsupportedScene);
    auto r=prepareOriginalUiLitPreamble50KM(palette.nodeWorld[0],context,true,positionQ,normalQ,work.palette[0]);
    if(r!=LS::complete)return status(r);
    OriginalUiSpecularNormalAxes50KM axes{};
    // Single palette 33B918..33B928: the StripPreamble's own saved modelView,
    // which for this one-entry palette is nodeWorld[0]*viewC0.
    if(specular&&(r=prepareOriginalUiSpecularNormalAxes50KM(palette.nodeWorld[0],context.viewC0,normalQ,axes))!=LS::complete)return status(r);
    for(std::size_t v=0;v<surface.vertices.size();++v){
        const auto& source=surface.vertices[v];if(source.bone!=0)return status(LS::unsupportedScene);
        std::array<float,3> position{},normal{};std::array<float,2> uv{};
        for(unsigned c=0;c<3;++c)if(!raw(source.position_local[c],scene.quantization[0],-8388608,8388607,position[c])||
            !raw(source.normal_local[c],scene.quantization[1],-128,127,normal[c]))return status(LS::unsupportedScene);
        if(!specular)for(unsigned c=0;c<2;++c){float packed=0;if(!raw(source.uv[c],scene.quantization[2],-32768,32767,packed))return status(LS::unsupportedScene);uv[c]=mul(packed,uvQ);}
        r=specular?projectOriginalUiSpecularMapCompressedVertex50KM(work.palette[0],axes,position,normal,work.vertices[v]):
            projectOriginalUiLitVertex50KM(work.palette[0],position,normal,uv,false,work.vertices[v]);
        if(r!=LS::complete)return status(r);
    }
    for(auto index:surface.indices)if(index>=surface.vertices.size())return status(LS::unsupportedScene);
    for(std::size_t t=0;t<triangles;++t)for(unsigned corner=0;corner<4;++corner){
        const auto source=surface.indices[t*3+(corner==3?2:corner)];const auto target=t*4+corner;const auto& v=work.vertices[source];
        output.vertices[target]=v.vertex;output.rasterW[target]=v.rasterW;output.fogFactors[target]=v.fogFactor;}
    output.slotCount=static_cast<std::uint32_t>(triangles);output.materialIndex=surface.material_index;d={};return true;
}
}
