#include "original_ui_main_camera50kg.h"
#include <cfenv>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
float bits(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,sizeof(f));return f;}
float add(float x,float y) noexcept {volatile float f=x+y;return f;}
float sub(float x,float y) noexcept {volatile float f=x-y;return f;}
float mul(float x,float y) noexcept {volatile float f=x*y;return f;}
float div(float x,float y) noexcept {volatile float f=x/y;return f;}
assets::TVCameraVector49E vector(const std::array<float,3>& p) noexcept {return {p[0],p[1],p[2],1};}
}
OriginalUiMainCameraStatus50KG prepareOriginalUiMainCamera50KG(
    const assets::OriginalUiMainCameraConfig50KG& config,OriginalUiMainCameraPreparation50KG& output) noexcept {
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559,"Main camera requires binary32");
    if(!assets::isOriginalUiMainCameraConfig50KG(config))return OriginalUiMainCameraStatus50KG::invalidDescriptor;
    if(std::fegetround()!=FE_TONEAREST)return OriginalUiMainCameraStatus50KG::unsupportedArithmetic;
    OriginalUiMainCameraPreparation50KG next;
    next.configurationIdentity=config.identity;next.originalWidth=640;next.originalHeight=448;
    next.aspect1DC=bits(0x3FAAAAAB);next.cameraInfoApplied=config.hasCameraInfo;
    const float width=640.0f,height=448.0f;
    const float halfWidth=mul(0.5f,width),fullRatio=div(height,width),halfHeight=mul(0.5f,height);
    const float left=sub(2048.0f,halfWidth),right=add(2048.0f,halfWidth);
    const float top=sub(2048.0f,halfHeight),bottom=add(2048.0f,halfHeight);
    const float viewportHeight=sub(bottom,top),viewportWidth=sub(right,left);
    const float viewportRatio=div(viewportHeight,viewportWidth);
    const float denominator=mul(next.aspect1DC,fullRatio);
    next.aspect1F8=div(viewportRatio,denominator);
    CameraSceneState49F prior;
    prior.aspect1F8=next.aspect1F8;
    prior.rollRadians=config.rollRadians;
    TVCameraUpdateOutput49F supplied;
    supplied.mode=0;supplied.position=vector(config.position);
    supplied.look=vector(config.look);supplied.up=vector(config.up);
    // Absent CameraInfo does not invoke114F90 and preserves ctor FOV77.
    // Authored/default zero FOV likewise preserves the original prior FOV.
    supplied.fov=config.hasCameraInfo?config.fovDegrees:0.0f;
    if(!composeCameraView49F(prior,supplied,next.core))
        return OriginalUiMainCameraStatus50KG::unsupportedArithmetic;
    output=next;return OriginalUiMainCameraStatus50KG::ok;
}
}
