#include "original_ui_tournament_camera50kg.h"
#include "original_ui_light_manager_lifetime50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Error=OriginalUiTournamentCameraError50KG;
using Diagnostic=OriginalUiTournamentCameraDiagnostic50KG;
using Matrix=std::array<float,16>;
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
bool fail(Diagnostic& d,Error error,std::uint32_t pc) noexcept {
    d.error=error;d.sourcePC=pc;return false;
}
bool finite(const Matrix& m) noexcept {for(float v:m)if(!std::isfinite(v))return false;return true;}
Matrix product(const Matrix& a,const Matrix& b) noexcept {
    Matrix result{};
    for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){
        float value=mul(b[c],a[r*4]);value=add(value,mul(b[4+c],a[r*4+1]));
        value=add(value,mul(b[8+c],a[r*4+2]));result[r*4+c]=add(value,mul(b[12+c],a[r*4+3]));
    }
    return result;
}
assets::TVCameraVector49E vector(const std::array<float,3>& v) noexcept {return {v[0],v[1],v[2],1};}
bool normalize(std::array<float,3>& v) noexcept {
    const float square=add(add(mul(v[0],v[0]),mul(v[1],v[1])),mul(v[2],v[2]));
    if(!std::isfinite(square)||square<=0)return false;
    volatile float inverse=1.0f/std::sqrt(square);
    for(float& value:v){value=mul(value,inverse);if(!std::isfinite(value))return false;}
    return true;
}
bool updateAuthored(OriginalUiRiderCamera50KG& camera,Diagnostic& d) noexcept {
    CameraSceneState49F prior;prior.aspect1F8=camera.aspect1F8;prior.rollRadians=camera.rollRadians;
    TVCameraUpdateOutput49F input;input.mode=0;input.position=vector(camera.position);
    input.look=vector(camera.look);input.up=vector(camera.up);input.fov=camera.fovDegrees;
    //1D7A90->114F90 is the authored pose setter. The normal update114AE0
    // invokes236DD0 and rebuilds even after the dirty countdown reaches zero.
    if(!composeCameraView49F(prior,input,camera.core))return fail(d,Error::arithmetic,0x236DD0);
    if(prepareOriginalUiParticleCameraFromView50KG(camera.core,640,448,camera.sourceScreen)!=
        OriginalUiMainParticleCameraStatus50KG::ok)return fail(d,Error::arithmetic,0x236DD0);
    camera.binderCameraColumn={{camera.core.view[2],camera.core.view[6],camera.core.view[10]}};
    return true;
}
bool readCamera(const assets::OriginalUiDat50KG& page,OriginalUiRiderCamera50KG& camera,
    Diagnostic& d) {
    const auto* fields=page.section("PageInfo");if(!fields)return fail(d,Error::page,0x1D7A90);
    camera.pageIdentity=page.identity();
    if(!assets::originalUiTriple50KG(*fields,"CameraPosition1",{{0,0,0}},camera.position,d.parse,0x1D7B74)||
        !assets::originalUiTriple50KG(*fields,"CameraLook1",{{0,-1,0}},camera.look,d.parse,0x1D7BB0)||
        !assets::originalUiTriple50KG(*fields,"CameraUp1",{{0,0,1}},camera.up,d.parse,0x1D7BE8)||
        !assets::originalUiNumber50KG(*fields,"CameraFOV1",35,camera.fovDegrees,d.parse,0x1D7C10)||
        !assets::originalUiNumber50KG(*fields,"CameraRoll1",0,camera.rollRadians,d.parse,0x1D7C34))
        return fail(d,Error::page,d.parse.sourcePC);
    const std::uint32_t aspectBits=0x3FAAAAABu;std::memcpy(&camera.aspect1DC,&aspectBits,4);
    const float halfWidth=mul(.5f,640),halfHeight=mul(.5f,448);
    const float left=sub(2048,halfWidth),right=add(2048,halfWidth);
    const float top=sub(2048,halfHeight),bottom=add(2048,halfHeight);
    camera.aspect1F8=div(div(sub(bottom,top),sub(right,left)),mul(camera.aspect1DC,div(448,640)));
    return updateAuthored(camera,d);
}
bool readFog(const assets::OriginalUiDat50KG& ui,OriginalUiRiderRenderContext50KG& result,
    Diagnostic& d) {
    const auto* fog=ui.section("Fog");if(!fog)return fail(d,Error::fog,0x1CE7F4);
    result.fogIdentity=ui.identity();
    // Do NOT repair authentic "Visibilty": original asks for "Visibility"
    // and therefore uses default1. Haziness is the real authored zero.
    if(!assets::originalUiNumber50KG(*fog,"Visibility",1,result.visibility,d.parse,0x1CE81C)||
        !assets::originalUiNumber50KG(*fog,"Haziness",.5f,result.haziness,d.parse,0x1CE8BC)||
        !assets::originalUiTriple50KG(*fog,"ColorRGB",{},result.sourceFogRgb,d.parse,0x1CE964))
        return fail(d,Error::fog,d.parse.sourcePC);
    if(result.visibility<0||result.visibility>1)result.visibility=1;
    result.fogEnd=add(900,mul(result.visibility,sub(1200,900)));
    result.fogStart=mul(sub(1,result.haziness),sub(result.fogEnd,200));
    result.packetFogStart33Z=result.fogEnd;
    const float range=sub(result.fogStart,result.fogEnd);
    result.packetFogScale33W=range==0?0:div(255,range);
    if(!std::isfinite(result.fogEnd)||result.fogEnd<=1||!std::isfinite(result.fogStart)||
        !std::isfinite(result.packetFogScale33W))return fail(d,Error::arithmetic,0x13E830);
    std::array<std::uint32_t,3> rgb{};
    for(unsigned i=0;i<3;++i){
        const auto f=result.sourceFogRgb[i];if(!std::isfinite(f)||f<0||f>255)return fail(d,Error::fog,0x1CE964);
        rgb[i]=static_cast<std::uint32_t>(f);
    }
    result.packedFogColorGs=rgb[0]|(rgb[1]<<8)|(rgb[2]<<16);
    return true;
}
bool readLights(const assets::OriginalUiDat50KG& ui,OriginalUiTournamentLighting50KG& result,
    Diagnostic& d) {
    const auto* lights=ui.section("Lights");const auto* first=ui.section("Light1");
    const auto* second=ui.section("Light2");std::int32_t count=0;
    if(!lights||!first||!second||!assets::originalUiInteger50KG(*lights,"NumberOfLights",2,count,d.parse,0x1CDB4C)||count!=2)
        return fail(d,Error::lights,0x1CDB4C);
    const auto* type1=first->find("Type");const auto* type2=second->find("Type");
    if(!type1||!type2||type1->text!="Ambient"||type2->text!="Directional")return fail(d,Error::lights,0x1CE014);
    std::array<float,3> ambient{},directional{},position{},target{};
    if(!first->find("ColorRGB")||!second->find("ColorRGB")||!second->find("Position")||
        !second->find("TargetPosition")||second->find("LookVector")||
        !assets::originalUiTriple50KG(*first,"ColorRGB",{},ambient,d.parse,0x1CE094)||
        !assets::originalUiTriple50KG(*second,"ColorRGB",{},directional,d.parse,0x1CE094)||
        !assets::originalUiTriple50KG(*second,"Position",{},position,d.parse,0x1CE0D4)||
        !assets::originalUiTriple50KG(*second,"TargetPosition",{},target,d.parse,0x1CE2C0))
        return fail(d,Error::lights,0x1CE094);
    result.uiIdentity=ui.identity();
    for(unsigned i=0;i<3;++i){
        if(!std::isfinite(ambient[i])||!std::isfinite(directional[i])||ambient[i]<0||ambient[i]>255||
            directional[i]<0||directional[i]>255)return fail(d,Error::lights,0x146FDC);
        result.ambient50[i]=mul(ambient[i],.5f);result.directional3C[i]=mul(directional[i],.5f);
        result.directionalLook78[i]=sub(target[i],position[i]);
    }
    result.ambient50[3]=64;result.ambientSlot60=result.ambient50;result.directional3C[3]=127.5f;
    if(!normalize(result.directionalLook78)||!normalize(result.directionalLook78))
        return fail(d,Error::arithmetic,0x147200);
    return true;
}
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
}
bool OriginalUiTournamentCameraOwner50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    OriginalUiTournamentCameraDiagnostic50KG& d) noexcept {
    d={};if(archive_)return fail(d,Error::alreadyBound,0x1D7A90);
    if(!archive||!archive->loaded()||archive->identity()!=
        assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::tournament))
        return fail(d,Error::archive,0x1D7A90);
    if(std::fegetround()!=FE_TONEAREST)return fail(d,Error::arithmetic,0x1D7A90);
    try {
        assets::OriginalUiDat50KG page,ui;
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UITourn/standing.dat",page,d.parse,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,Error::page,0x1D7A90);
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UITourn/ui.dat",ui,d.parse,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,Error::fog,0x1CE7B0);
        OriginalUiRiderCamera50KG camera;OriginalUiRiderRenderContext50KG fog;
        OriginalUiTournamentLighting50KG lights;
        if(!readCamera(page,camera,d)||!readFog(ui,fog,d)||!readLights(ui,lights,d))return false;
        // Cold arithmetic admission does NOT publish a drawable frame or act
        // as camera/observer/light-manager registration.
        fog.cameraAfterFog=camera;
        if(!applyFog(fog,d))return false;
        OriginalUiRiderClipCamera50KG clip;PortalZoneViewOutput49F zone;
        if(!prepareOriginalUiRiderClipCamera50KG(fog.cameraAfterFog,clip)||
            !composePortalZoneView280720(fog.cameraAfterFog.core,{{-1,1,-1,1}},zone))
            return fail(d,Error::arithmetic,0x237000);
        authored_=camera;fog_=fog;lighting_=lights;archive_=std::move(archive);return true;
    }catch(...){return fail(d,Error::allocation,0x1D7A90);}
}
bool OriginalUiTournamentCameraOwner50KG::prepareFrame(const OriginalApplicationAnimationClockView50KG& clock,
    std::shared_ptr<const OriginalUiLightManagerLifetime50KG> lightManager,
    OriginalUiTournamentCameraDiagnostic50KG& d) noexcept {
    d={};if(!archive_)return fail(d,Error::archive,0x114A00);
    if(issued_||serial_==std::numeric_limits<std::uint64_t>::max())return fail(d,Error::phase,0x114A00);
    if(!lightManager||!lightManager->matchesTournamentRegistry(lighting_))return fail(d,Error::lights,0x1499A0);
    const auto lightGeneration=lightManager->managerGeneration();
    if(!lightGeneration)return fail(d,Error::lights,0x1CDB10);
    OriginalApplicationAnimationClockSample50KG before,after;
    if(!clock.read(before))return fail(d,Error::clock,0x140C50);
    if(std::fegetround()!=FE_TONEAREST)return fail(d,Error::arithmetic,0x114AE0);
    OriginalUiTournamentCameraFrame50KG next;
    next.render_=fog_;next.render_.cameraAfterFog=authored_;next.lighting_=lighting_;
    if(!updateAuthored(next.render_.cameraAfterFog,d)||!applyFog(next.render_,d))return false;
    // Fog114F20->237000 copies all16 current projection words80 into290,
    // then applies final334/338. Never retain constructor-prefix290 or use80
    // as a substitute for the separately scaled clipping projection.
    if(!prepareOriginalUiRiderClipCamera50KG(next.cameraAfterFog(),next.clip_))
        return fail(d,Error::arithmetic,0x237000);
    PortalZoneViewOutput49F zone;
    if(!composePortalZoneView280720(next.cameraAfterFog().core,next.membership_.viewport4C,zone))
        return fail(d,Error::arithmetic,0x280720);
    next.membership_.projection=zone.projection;next.membership_.matrix60=zone.matrix60;
    auto& context=next.context_;const auto& camera=next.cameraAfterFog();
    context.viewC0=camera.core.view;context.projection80=camera.core.projection;
    context.clipProjection290=next.clip_.clipProjection290;context.screen140=camera.sourceScreen.sourceScreen140;
    context.cameraPosition180=camera.position;context.fogEnd33Z=next.render_.packetFogStart33Z;
    context.fogSlope33W=next.render_.packetFogScale33W;context.canvasOffset=next.render_.sourceCanvasOffset;
    // The actual retained shared manager has exactly this current Tournament
    // population, checked above and again before publication. Model selection
    // remains that manager's
    // separate14A0E8/149BB0 owner; these are renderer-global1499A0 rows only.
    if(projectOriginalUiGlobalLightRows50KM(true,lighting_.directionalLook78,lighting_.directional3C,
        lighting_.ambient50,context.lights)!=original_racer_lighting50KM::Status::complete)
        return fail(d,Error::arithmetic,0x1499A0);
    if(!clock.read(after)||before.current230!=after.current230||before.pause11C!=after.pause11C||
        before.pending120!=after.pending120)return fail(d,Error::clock,0x140C50);
    if(lightManager->managerGeneration()!=lightGeneration||!lightManager->matchesTournamentRegistry(lighting_))
        return fail(d,Error::lights,0x1499A0);
    next.current230_=before.current230;next.pause11C_=before.pause11C;next.pending120_=before.pending120;
    next.serial_=serial_+1;
    frame_=next;clock_=clock;lightManager_=std::move(lightManager);lightGeneration_=lightGeneration;
    serial_=next.serial_;issued_=true;return true;
}
const OriginalUiTournamentCameraFrame50KG* OriginalUiTournamentCameraOwner50KG::frame() const noexcept {
    return isCurrent(frame_)?&frame_:nullptr;
}
bool OriginalUiTournamentCameraOwner50KG::isCurrent(const OriginalUiTournamentCameraFrame50KG& frame) const noexcept {
    if(!archive_||!issued_||&frame!=&frame_||frame.serial_!=serial_)return false;
    if(!lightManager_||lightManager_->managerGeneration()!=lightGeneration_||
        !lightManager_->matchesTournamentRegistry(frame.lighting_))return false;
    OriginalApplicationAnimationClockSample50KG sample;
    return clock_.read(sample)&&sample.current230==frame.current230_&&sample.pause11C==frame.pause11C_&&
        sample.pending120==frame.pending120_;
}
bool OriginalUiTournamentCameraOwner50KG::retireFrame(const OriginalUiTournamentCameraFrame50KG& frame) noexcept {
    if(!archive_||!issued_||&frame!=&frame_||frame.serial_!=serial_)return false;
    issued_=false;lightManager_.reset();lightGeneration_=0;return true;
}
}
