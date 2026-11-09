#include "original_ui_unlock_camera50pd.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using Error=OriginalUiUnlockCameraError50PD;using Diagnostic=OriginalUiUnlockCameraDiagnostic50PD;using Matrix=std::array<float,16>;
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
bool fail(Diagnostic& d,Error e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
bool finite(const Matrix& m) noexcept {for(float v:m)if(!std::isfinite(v))return false;return true;}
Matrix product(const Matrix& a,const Matrix& b) noexcept {
    Matrix r{};
    for(unsigned i=0;i<4;++i)for(unsigned c=0;c<4;++c){
        float v=mul(b[c],a[i*4]);v=add(v,mul(b[4+c],a[i*4+1]));v=add(v,mul(b[8+c],a[i*4+2]));r[i*4+c]=add(v,mul(b[12+c],a[i*4+3]));}
    return r;
}
assets::TVCameraVector49E vector(const std::array<float,3>& v) noexcept {return {v[0],v[1],v[2],1};}
bool normalize(std::array<float,3>& v) noexcept {
    const float square=add(add(mul(v[0],v[0]),mul(v[1],v[1])),mul(v[2],v[2]));
    if(!std::isfinite(square)||square<=0)return false;
    volatile float inverse=1.0f/std::sqrt(square);
    for(float& value:v){value=mul(value,inverse);if(!std::isfinite(value))return false;}
    return true;
}
// 1D7A90 -> 114F90 authored pose, rebuilt by the normal update 114AE0/236DD0.
bool updateAuthored(OriginalUiRiderCamera50KG& camera,Diagnostic& d) noexcept {
    CameraSceneState49F prior;prior.aspect1F8=camera.aspect1F8;prior.rollRadians=camera.rollRadians;
    TVCameraUpdateOutput49F input;input.mode=0;input.position=vector(camera.position);
    input.look=vector(camera.look);input.up=vector(camera.up);input.fov=camera.fovDegrees;
    if(!composeCameraView49F(prior,input,camera.core))return fail(d,Error::arithmetic,0x236DD0);
    if(prepareOriginalUiParticleCameraFromView50KG(camera.core,640,448,camera.sourceScreen)!=
        OriginalUiMainParticleCameraStatus50KG::ok)return fail(d,Error::arithmetic,0x236DD0);
    camera.binderCameraColumn={{camera.core.view[2],camera.core.view[6],camera.core.view[10]}};
    return true;
}
bool readCamera(const assets::OriginalUiDat50KG& page,OriginalUiRiderCamera50KG& camera,Diagnostic& d) {
    const auto* f=page.section("PageInfo");if(!f)return fail(d,Error::page,0x1D7A90);
    camera.pageIdentity=page.identity();
    // Every key is authored in unlock.dat; the readers keep the 1D7Bxx defaults.
    if(!assets::originalUiTriple50KG(*f,"CameraPosition1",{{0,0,0}},camera.position,d.parse,0x1D7B74)||
       !assets::originalUiTriple50KG(*f,"CameraLook1",{{0,-1,0}},camera.look,d.parse,0x1D7BB0)||
       !assets::originalUiTriple50KG(*f,"CameraUp1",{{0,0,1}},camera.up,d.parse,0x1D7BE8)||
       !assets::originalUiNumber50KG(*f,"CameraFOV1",35,camera.fovDegrees,d.parse,0x1D7C10)||
       !assets::originalUiNumber50KG(*f,"CameraRoll1",0,camera.rollRadians,d.parse,0x1D7C34))
        return fail(d,Error::page,d.parse.sourcePC);
    const std::uint32_t aspectBits=0x3FAAAAABu;std::memcpy(&camera.aspect1DC,&aspectBits,4);
    const float halfWidth=mul(.5f,640),halfHeight=mul(.5f,448);
    const float left=sub(2048,halfWidth),right=add(2048,halfWidth),top=sub(2048,halfHeight),bottom=add(2048,halfHeight);
    camera.aspect1F8=div(div(sub(bottom,top),sub(right,left)),mul(camera.aspect1DC,div(448,640)));
    return updateAuthored(camera,d);
}
// 1CE7B0 fog: the authentic "Visibilty" typo leaves Visibility at default 1.
bool readFog(const assets::OriginalUiDat50KG& ui,OriginalUiRiderRenderContext50KG& r,Diagnostic& d) {
    const auto* fog=ui.section("Fog");if(!fog)return fail(d,Error::fog,0x1CE7F4);
    r.fogIdentity=ui.identity();
    if(!assets::originalUiNumber50KG(*fog,"Visibility",1,r.visibility,d.parse,0x1CE81C)||
       !assets::originalUiNumber50KG(*fog,"Haziness",.5f,r.haziness,d.parse,0x1CE8BC)||
       !assets::originalUiTriple50KG(*fog,"ColorRGB",{},r.sourceFogRgb,d.parse,0x1CE964))return fail(d,Error::fog,d.parse.sourcePC);
    if(r.visibility<0||r.visibility>1)r.visibility=1;
    r.fogEnd=add(900,mul(r.visibility,sub(1200,900)));
    r.fogStart=mul(sub(1,r.haziness),sub(r.fogEnd,200));
    r.packetFogStart33Z=r.fogEnd;
    const float range=sub(r.fogStart,r.fogEnd);r.packetFogScale33W=range==0?0:div(255,range);
    if(!std::isfinite(r.fogEnd)||r.fogEnd<=1||!std::isfinite(r.fogStart)||!std::isfinite(r.packetFogScale33W))
        return fail(d,Error::arithmetic,0x13E830);
    std::array<std::uint32_t,3> rgb{};
    for(unsigned i=0;i<3;++i){const auto f=r.sourceFogRgb[i];if(!std::isfinite(f)||f<0||f>255)return fail(d,Error::fog,0x1CE964);
        rgb[i]=static_cast<std::uint32_t>(f);}
    r.packedFogColorGs=rgb[0]|(rgb[1]<<8)|(rgb[2]<<16);return true;
}
// 1CDB10: Light1 Ambient, Light2 Directional (Position/TargetPosition).
bool readLights(const assets::OriginalUiDat50KG& ui,OriginalUiRiderLighting50KG& r,Diagnostic& d) {
    const auto* lights=ui.section("Lights");const auto* first=ui.section("Light1");const auto* second=ui.section("Light2");
    std::int32_t count=0;
    if(!lights||!first||!second||!assets::originalUiInteger50KG(*lights,"NumberOfLights",2,count,d.parse,0x1CDB4C)||count!=2)
        return fail(d,Error::lights,0x1CDB4C);
    const auto* type1=first->find("Type");const auto* type2=second->find("Type");
    if(!type1||!type2||type1->text!="Ambient"||type2->text!="Directional")return fail(d,Error::lights,0x1CE014);
    std::array<float,3> ambient{},directional{},position{},target{};
    if(!first->find("ColorRGB")||!second->find("ColorRGB")||!second->find("Position")||!second->find("TargetPosition")||
       second->find("LookVector")||
       !assets::originalUiTriple50KG(*first,"ColorRGB",{},ambient,d.parse,0x1CE094)||
       !assets::originalUiTriple50KG(*second,"ColorRGB",{},directional,d.parse,0x1CE094)||
       !assets::originalUiTriple50KG(*second,"Position",{},position,d.parse,0x1CE0D4)||
       !assets::originalUiTriple50KG(*second,"TargetPosition",{},target,d.parse,0x1CE2C0))return fail(d,Error::lights,0x1CE094);
    r.uiIdentity=ui.identity();
    for(unsigned i=0;i<3;++i){
        if(!std::isfinite(ambient[i])||!std::isfinite(directional[i])||ambient[i]<0||ambient[i]>255||directional[i]<0||directional[i]>255)
            return fail(d,Error::lights,0x146FDC);
        r.ambient50[i]=mul(ambient[i],.5f);r.directional3C[i]=mul(directional[i],.5f);r.directionalLook78[i]=sub(target[i],position[i]);
    }
    r.ambient50[3]=64;r.ambientSlot60=r.ambient50;r.directional3C[3]=127.5f;
    if(!normalize(r.directionalLook78)||!normalize(r.directionalLook78))return fail(d,Error::arithmetic,0x147200);
    return true;
}
// 13EA30: near 1, far fogEnd; projection Z terms, combined100, screen250.
bool applyFog(OriginalUiRiderRenderContext50KG& render,Diagnostic& d) noexcept {
    auto& camera=render.cameraAfterFog;auto& core=camera.core;
    core.sceneAfter.nearPlane=1;core.sceneAfter.farPlane=render.fogEnd>100000?100000:render.fogEnd;
    const float z=div(1,sub(1,div(core.sceneAfter.nearPlane,core.sceneAfter.farPlane)));
    core.projection[10]=z;core.projection[14]=mul(-z,core.sceneAfter.nearPlane);
    core.combined=product(core.view,core.projection);
    if(!finite(core.combined)||prepareOriginalUiParticleCameraFromView50KG(core,640,448,camera.sourceScreen)!=
        OriginalUiMainParticleCameraStatus50KG::ok)return fail(d,Error::arithmetic,0x13EA30);
    return true;
}
void context(const OriginalUiRiderRenderContext50KG& render,const OriginalUiRiderClipCamera50KG& clip,
    const original_racer_lighting50KM::LightMatrices& lights,OriginalUiLitContext50KM& out) noexcept {
    const auto& camera=render.cameraAfterFog;OriginalUiLitContext50KM c;
    c.viewC0=camera.core.view;c.projection80=camera.core.projection;c.clipProjection290=clip.clipProjection290;
    c.screen140=camera.sourceScreen.sourceScreen140;c.cameraPosition180=camera.position;c.lights=lights;
    c.fogEnd33Z=render.packetFogStart33Z;c.fogSlope33W=render.packetFogScale33W;c.canvasOffset=render.sourceCanvasOffset;out=c;
}
}
bool OriginalUiUnlockCamera50PD::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    d={};if(archive_)return fail(d,Error::alreadyBound,0x1D7A90);
    if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::unlock))
        return fail(d,Error::archive,0x1D7A90);
    if(std::fegetround()!=FE_TONEAREST)return fail(d,Error::arithmetic,0x1D7A90);
    try {
        assets::OriginalUiDat50KG page,ui;
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UIUnlock/unlock.dat",page,d.parse,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))
            return fail(d,Error::page,0x1D7A90);
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UIUnlock/ui.dat",ui,d.parse,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))
            return fail(d,Error::fog,0x1CE7B0);
        OriginalUiUnlockCameraFrame50PD next;OriginalUiRiderCamera50KG camera;
        if(!readCamera(page,camera,d)||!readFog(ui,next.render,d)||!readLights(ui,next.lighting,d))return false;
        next.render.cameraAfterFog=camera;
        if(!applyFog(next.render,d))return false;
        if(!prepareOriginalUiRiderClipCamera50KG(next.render.cameraAfterFog,next.clip))return fail(d,Error::arithmetic,0x237000);
        // Pod: renderer-global 1499A0 rows from the two registered lights.
        original_racer_lighting50KM::LightMatrices global{};
        if(projectOriginalUiGlobalLightRows50KM(true,next.lighting.directionalLook78,next.lighting.directional3C,
            next.lighting.ambient50,global)!=original_racer_lighting50KM::Status::complete)return fail(d,Error::arithmetic,0x1499A0);
        context(next.render,next.clip,global,next.podContext);
        // Character: 14A0E8 -> 149BB0 small registry (ambient type4, then the
        // directional type2 with 146C40's 3C mirrored into 4C; zone ambient=60).
        original_racer_lighting50KM::SmallDirectionalRegistry registry{};
        registry.count=2;registry.registered[0].type=4;
        auto& directional=registry.registered[1];directional.type=2;directional.direction78=next.lighting.directionalLook78;
        directional.diffuse3C={next.lighting.directional3C[0],next.lighting.directional3C[1],next.lighting.directional3C[2]};
        directional.specular4C=directional.diffuse3C;registry.selectedZoneAmbient=next.lighting.ambientSlot60;
        if(original_racer_lighting50KM::projectSmallDirectionalRegistry(registry,next.characterLights)!=
            original_racer_lighting50KM::Status::complete)return fail(d,Error::arithmetic,0x149BB0);
        context(next.render,next.clip,next.characterLights,next.characterContext);
        frame_=next;archive_=std::move(archive);return true;
    }catch(...){return fail(d,Error::allocation,0x1D7A90);}
}
}
