#pragma once
#include "native_camera_view50dx.h"
#include "vehicle_view_camera50dg.h"
#include "../../../src/game/original_camera_start_effects.h"
#include <cmath>
#include <cstring>

namespace rr::platform {
// Rendering adaptation, not another camera controller. The actual camera
// session supplies its successfully published core49F result. World geometry
// uses those already-rounded matrix words through50DX; there is no host chase
// offset, bounding-box framing, look-at repair or FOV substitution.
struct OriginalCameraPresentation {
    VehicleViewMatrix50DG world{},sky{};
    float eye[3]{},backward[3]{};
    std::uint64_t cameraPublication=0;
    std::int32_t width=0,height=0;
};
enum class OriginalCameraPresentationStatus : std::uint8_t {
    complete,owner,position,worldProjection,skyProjection
};
struct OriginalCameraPresentationResult {
    OriginalCameraPresentationStatus status=OriginalCameraPresentationStatus::owner;
    NativeCameraViewError50DX conversion=NativeCameraViewError50DX::none;
    // Preserve the exact reached sky producer failure instead of reporting
    // only skyProjection with an untouched conversion error of none.
    game::OriginalCameraStartResult skyRebuild{};
};
// The whole actual drawable receives the recovered source viewport projection.
// Resize does not invent widescreen FOV/aspect: original640x448 scene/aspect is
// retained and stretched by this declared native output policy. The caller
// supplies current drawable pixels and preserves their equality through submit.
//
// Sky keeps the existing native far100000 policy, but follows the recovered
// positive-forward orientation and roll. It is a transient, source-centered
// projection only, not a second mutable camera owner. It cannot affect source
// scene state, visibility, original RNG, or the world geometry projection.
inline OriginalCameraPresentationResult projectOriginalCameraPresentation(
    const game::CameraViewOutput49F& currentCore,
    const game::OriginalCameraViewportFields& viewport,std::uint64_t publication,
    std::int32_t width,std::int32_t height,OriginalCameraPresentation& output) noexcept {
    OriginalCameraPresentationResult r;
    if(!publication||!viewport.prepared||!viewport.matricesReady||
       !viewport.sessionGeneration||!viewport.sceneKey||width<=0||height<=0)return r;
    OriginalCameraPresentation next;
    next.cameraPublication=publication;next.width=width;next.height=height;
    const float eye[]={currentCore.position.x,currentCore.position.y,currentCore.position.z};
    const float look[]={currentCore.sceneAfter.look.x,currentCore.sceneAfter.look.y,currentCore.sceneAfter.look.z};
    for(unsigned i=0;i<3;++i){
        if(!std::isfinite(eye[i])||!std::isfinite(look[i])){r.status=OriginalCameraPresentationStatus::position;return r;}
        next.eye[i]=eye[i];next.backward[i]=-look[i];
    }
    const NativeCameraViewContract50DX contract{
        NativeCameraViewProvenance50DX::currentCore49FForOutput,width,height,
        NativeCameraClipDepth50DX::negativeOneToOne};
    NativeCameraMatrix50DX converted;
    r.status=OriginalCameraPresentationStatus::worldProjection;
    if(!convertNativeCameraView50DX(currentCore,contract,converted,r.conversion))return r;
    std::memcpy(next.world.worldViewProjection,converted.worldViewProjection,sizeof converted.worldViewProjection);
    auto skyScene=currentCore.sceneAfter;skyScene.farPlane=100000.f;
    auto skyViewport=viewport;
    const rr::assets::TVCameraVector49E skyPosition{0,0,0,1};
    game::CameraViewOutput49F skyCore;
    r.status=OriginalCameraPresentationStatus::skyProjection;
    //115350 direct rebuild reads the owned Look/Up without a second normalize.
    //Only these local projection/scratch copies are written.
    r.skyRebuild=game::rebuildOriginalRaceScene236DD0(skyViewport,skyScene,skyPosition,skyCore);
    if(r.skyRebuild.status!=game::OriginalCameraStartStatus::complete||
       !convertNativeCameraView50DX(skyCore,contract,converted,r.conversion))return r;
    std::memcpy(next.sky.worldViewProjection,converted.worldViewProjection,sizeof converted.worldViewProjection);
    output=next;r.status=OriginalCameraPresentationStatus::complete;return r;
}
}
