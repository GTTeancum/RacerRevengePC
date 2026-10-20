#pragma once
#include "camera_view49f.h"
#include "original_shared_rng50iu.h"
#include "scene_graph49p.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
using OriginalAttachedVector=assets::TVCameraVector49E;
struct OriginalAttachedOffset {
    std::uint64_t sourceMemberIdentity=0;std::int32_t pod60=0;
    OriginalAttachedVector racerCE0{0,0,0,1};
    bool authored=false;
    std::size_t keyOffset=0,valueOffset=0,valueBytes=0;
};
//215C9C..215CC4, actual PHY[CameraInfo]/FirstPersonCamOffset. Input is the
//unchanged decoded PHY already authenticated by the actual pod asset owner.
//158C00 genuinely copies source434630 XYZ=(0,0,0) when absent; constructor
//20F5D4 keeps W1. No generic runtime camera offset is invented.
bool readOriginalAttachedOffset215CC0(std::int32_t actualPod60,std::uint64_t authenticatedMemberIdentity,
    const char* phy,std::size_t bytes,OriginalAttachedOffset&) noexcept;
struct OriginalAttachedBinding {
    std::uint64_t sessionGeneration=0,racerKey=0,mainBodyKey=0,model1B4Key=0;
    SceneNodeIndex49P body288=SceneNodeIndex49P::none,model1B4=SceneNodeIndex49P::none;
    std::int32_t pod60=0;
};
struct OriginalAttachedFilter {float value=0,tau=.15f,blend=1;};
struct OriginalAttachedCamera {
    OriginalAttachedBinding binding{};
    bool prepared=false,active=false,snap3C=false;
    std::uint64_t revision=0;
    float fov04=80;
    OriginalAttachedVector offset10{0,0,0,1};
    float shakeAmplitude28=0,shakeScale2C=0;
    std::array<OriginalAttachedFilter,3> direction30{};
};
//ONE application owner for the four reached source static guards8338..8344
//and values423EB0/3EB8/3EC0/3EC8. Do not reset/copy it per frame or per camera.
struct OriginalAttachedGlobals {
    bool lazy8338=false,lazy833C=false,lazy8340=false,lazy8344=false;
    float yawInputRange3EB0=0,verticalInputRange3EB8=0,yawAngle3EC0=0,verticalAngle3EC8=0;
};
struct OriginalAttachedFrame {
    std::uint64_t sessionGeneration=0,racerKey=0,mainBodyKey=0,model1B4Key=0;
    float mainBodyA4=0,mainBodyVelocityY74=0,elapsed=0;
};
enum class OriginalAttachedStatus : std::uint8_t {ok,owner,authoring,graph,arithmetic,sceneFlags};
struct OriginalAttachedResult {OriginalAttachedStatus status=OriginalAttachedStatus::ok;std::uint32_t sourcePC=0;};
//Fresh13DCA0,1750D4->13DEF0(model1B4),1750E0->13DEE0(main340),
//175208..175220->13DDB0(actualCE0,FOV90). All bindings remain common-racer.
OriginalAttachedResult prepareOriginalAttachedCamera(OriginalAttachedCamera&,
    const OriginalAttachedBinding&,const OriginalAttachedOffset&) noexcept;
//Actual virtual+8=13E480 only sets3C=1. Previous pose and a2 are ignored.
OriginalAttachedResult activateOriginalAttachedCamera(OriginalAttachedCamera&) noexcept;
//13E530 copied from real controller's existing source shake timer.
OriginalAttachedResult setOriginalAttachedShake(OriginalAttachedCamera&,float scale,float amplitude) noexcept;
//13E490 defines only its first three vectors. Source stack bytes30..5F are
//unwritten; the typed snapshot preserves that unknownness rather than zeroing.
struct OriginalAttachedSnapshot {
    std::array<OriginalAttachedVector,3> vectors{{{0,0,0,1},{0,0,0,1},{0,0,0,1}}};
    std::uint32_t definedWordMask=0x00000fffu;
};
OriginalAttachedSnapshot snapshotOriginalAttached13E490() noexcept;
//Complete13DF00, including real model1B4 lazy world refresh, three direction
//filters,142B80 rotation,28B9B0 offsetXYZ,13E540 shared RNG and mode0 Look/Up.
//target is untouched; output is a live initialized controller output record.
//Consumes three actual308030 draws even when both shake fields are zero.
OriginalAttachedResult advanceOriginalAttachedCamera(OriginalAttachedCamera&,
    const OriginalAttachedFrame&,const SceneGraph49P&,OriginalAttachedGlobals&,
    CoupledNoiseContext49M&,TVCameraUpdateOutput49F&) noexcept;
OriginalAttachedResult advanceOriginalAttachedCameraView(OriginalAttachedCamera&,
    const OriginalAttachedFrame&,const SceneGraph49P&,OriginalAttachedGlobals&,
    CoupledNoiseContext49M&,TVCameraUpdateOutput49F&,CameraSceneState49F&,
    CameraViewOutput49F&,std::uint8_t rootFlags228,std::uint8_t sceneFlags29) noexcept;
//Bounded, fixed-storage, serial native instruction-step arithmetic: normal/zero
//live operands/results, separately rounded binary32, nearest-even, no contraction/fast math.
//The raw roll A4 input to13E074..13E114 and the dead13E118..13E1BC
//vertical-map input (actual velocityY) admit finite subnormals; its first subtraction result and all later results retain
//normal/zero admission. Actual range2 absorbs every subnormal roll in the
//first subtraction; unchanged ordered yaw arithmetic yields +0. The dead
//vertical block does not publish yaw/pose/state. No raw input is normalized.
//No flushing, replacement input, exception-mode change or skipped arithmetic.
//Existing49U/49F normalization semantics are retained; no universal EE/VU
//exception or scratch-register-file equivalence is claimed. 28B9B0's scratch
//W is dead: source13E338..13E358 consumes onlyXYZ and explicitly constructs W1.
//Disjoint stable owners/graph/input/output/backing are required. Rejection
//preserves numeric state, globals, real shared RNG, graph caches and output.
}
