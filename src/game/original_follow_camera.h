#pragma once
#include "camera_view49f.h"
#include "original_shared_rng50iu.h"
#include "scene_graph49p.h"
#include "original_follow_collision.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <limits>

namespace rr::game {
using OriginalFollowVector = assets::TVCameraVector49E;
// Typed projection of2868C0's original80-byte record, not a raw layout.
// Angles are converted from authored degrees by binary32 0x3C8EFA34.
struct OriginalFollowPreset {
    std::int32_t id00=0;
    float angleY04=0,angleX08=0,distance0C=0,fov10=0;
    float reverseAngle14=0,reverseDistance18=0,reverseLookAhead1C=0,switchSpeed20=0;
    std::int32_t cycle24=0,filter28=0;
    std::optional<std::int32_t> dontRotate2C; // absent in all23 originals; never read by17A050.
    float lookAhead30=0;
    std::int32_t lookBack34=0;
    float cameraSwing38=0,lookSwing3C=0,minDistance40=0,maxDistance44=0,minFov48=0,maxFov4C=0;
};
// This is the decoded source authoring boundary, NOT effective defaults. Each
// section must contain the19 consumed/authored keys; absent DontRotate stays unknown. Views must be from the actual CAM
// named by17517C..1751B0 for racer+60. The23 transferred original CAM files are retained beside this candidate.
struct OriginalFollowAuthoredValue {const char* key=nullptr;const char* value=nullptr;std::size_t valueBytes=0;};
struct OriginalFollowAuthoredSection {const char* name=nullptr;const OriginalFollowAuthoredValue* values=nullptr;std::size_t count=0;};
struct OriginalFollowCamSource {
    std::uint64_t memberIdentity=0;
    std::int32_t pod60=0,layout118=0;
    const OriginalFollowAuthoredSection* sections=nullptr;std::size_t sectionCount=0;
};
struct OriginalFollowPresetBank {
    std::uint64_t memberIdentity=0;
    std::int32_t pod60=0,layout118=0;
    const OriginalFollowPreset* records=nullptr;std::size_t count=0;
};
// Actual five-lane scalar-filter storage. Existing numeric consumers read
//0/4/8; original constructors also own bounds0C/10. Three-value standalone
//initializers retain the same unbounded defaults and arithmetic semantics.
struct OriginalFollowFilter {
    float value=0,tau=0,blend=1;
    float upper0C=std::numeric_limits<float>::max();
    float lower10=-std::numeric_limits<float>::max();
};
struct OriginalFollowCamera {
    std::uint64_t sessionGeneration=0,racerKey=0,mainBodyKey=0,containerKey=0,revision=0;
    OriginalFollowPresetBank bank{};
    bool prepared=false,active=false;
    std::size_t current100=0;
    float distance04=0,angleY08=0,angleX0C=0,fov10=80,cameraSwing14=1,lookSwing18=1;
    std::array<OriginalFollowFilter,3> target28{},position34{},direction40{};
    OriginalFollowFilter fov4C{},roll50{};
    float distanceOffset58=0,yawOffset5C=0,pitchOffset60=0,fovOffset64=0;
    std::array<float,4> savedOffsets68{};
    float reverseAngle78=0,reverseDistance7C=0,reverseLookAhead80=0;
    float reverseThreshold84=15,bodySpeedScale88=0,bodyScale8C=3;
    bool snap10C=false;
    std::uint32_t terrain110=0;
    // Source120 is null in the admitted fresh race. Nonnull calls original
    // TerrainStub2E83D0 assert; never substitute zero height or host terrain.
    bool terrainProvider120Present=false;
    float heightOffset124=0,clearance128=3;
    OriginalFollowFilter extra12C{},extra154{};
    OriginalFollowCollisionState collision{};
    std::array<OriginalFollowFilter,3> velocity130{};
    float dynamicFov15C=0,dynamicDistance160=0,racerModeTimer164=0;
    std::int32_t debugCountdown168=0,debugIndex16C=0;
    float shakeAmplitude170=0,shakeScale174=0,queryRadius178=3.5f;
    OriginalFollowVector scratch180{0,0,0,1};
    OriginalFollowVector historyC0{0,0,0,1},historyD0{0,0,0,1};
};
// Exact world reads of racer194; body velocity and A4 come from racer340.
// This is common racer/body data. It does not reinterpret an AI as Player.
struct OriginalFollowSubject {
    std::uint64_t sessionGeneration=0,racerKey=0,mainBodyKey=0,containerKey=0;
    std::int32_t racerMode64=0;
    OriginalFollowVector worldPosition{0,0,0,1},worldForward{0,0,0,1},worldUp{0,0,0,1},bodyVelocity70{};
    float bodyA4=0;
};
struct OriginalFollowIdentity {
    std::uint64_t sessionGeneration=0,racerKey=0,mainBodyKey=0,containerKey=0;
    std::int32_t pod60=0;
};
// Real input to179DD0, separate from racer class. The optional debug cycle and
// reload branches remain explicit: unsupported requests are not ignored.
struct OriginalFollowInput {
    std::int32_t distanceFlag00=0,fovFlag04=0,reloadFlag08=0;
    float axis20=0,axis24=0;
    bool debugQuery4_0_12=false;
};
struct OriginalFollowPoseWork {
    OriginalFollowCamera after{};
    OriginalFollowCollisionInput collision{};
    float fov118=0,roll11C=0;
    std::uint64_t baseRevision=0;
    bool pending=false;
};
enum class OriginalFollowStatus : std::uint8_t {
    ok,owner,authoring,capacity,arithmetic,missingCollision,terrainStub,
    unsupportedDebug,unsupportedReload,sceneFlags
};
struct OriginalFollowResult {
    OriginalFollowStatus status=OriginalFollowStatus::ok;
    std::uint32_t sourcePC=0;
    std::size_t record=0;
};
// Authenticate exact unmodified original CAM bytes against the23-member
// transferred catalog, parse the admitted original ASCII syntax and select
// layout0/1/2 sections. Layoutq is genuinely absent from these originals.
OriginalFollowResult loadOriginalFollowCam(std::int32_t pod60,std::int32_t layout118,
    const std::uint8_t* bytes,std::size_t byteCount,
    OriginalFollowPreset* storage,std::size_t capacity,OriginalFollowPresetBank&) noexcept;
//2867A0/2868C0 decoded-section projection. Requires NumberOfPresets section
// with NumberOfPresets key, then Preset1... or h/v/q suffix for layout1/2/other.
// Caller-owned storage; success writes count records and the bank descriptor.
OriginalFollowResult readOriginalFollowPresets(const OriginalFollowCamSource&,
    OriginalFollowPreset* storage,std::size_t capacity,OriginalFollowPresetBank&) noexcept;
//179320/179580/283CF0 +1750A0's original0.55/0.1/0.1/3 arguments, select0.
// Fresh-only numeric ownership; an already prepared owner is rejected.
// Activation requires the actual controller.
OriginalFollowResult prepareOriginalFollowCamera(OriginalFollowCamera&,
    const OriginalFollowIdentity&,const OriginalFollowPresetBank&) noexcept;
//179F40->2851A0 with a2=0, the actual177AB0 Follow activation.179DD0 is INPUT.
OriginalFollowResult activateOriginalFollowCamera(OriginalFollowCamera&) noexcept;
//17BD00->2842F0->285580; preserve/restore semantics for a2/a3 are represented.
OriginalFollowResult selectOriginalFollowPreset(OriginalFollowCamera&,std::int32_t id,
    bool saveOffsets=false,bool restoreOffsets=false) noexcept;
OriginalFollowResult applyOriginalFollowInput(OriginalFollowCamera&,OriginalFollowInput&) noexcept;
//17BB60 actual controller broadcast. No synthetic trigger/timer is created.
OriginalFollowResult setOriginalFollowShake(OriginalFollowCamera&,float f12Scale,float f13Amplitude) noexcept;
// Snapshot28B1A0/28A700(null-relative), preserving graph lazy-cache ownership.
// graph is mutable through its descriptor; source copy-only W of samples is1.
OriginalFollowResult readOriginalFollowSubject(const OriginalFollowIdentity&,const SceneGraph49P&,
    SceneNodeIndex49P racerContainer194,std::int32_t racerMode64,
    OriginalFollowVector bodyVelocity70,float bodyA4,OriginalFollowSubject&) noexcept;
// First half17A050 through17A640. Both genuine query children24/190 must have
// been constructed/registered externally. Work does not mutate the camera.
OriginalFollowResult beginOriginalFollowPose17A050(const OriginalFollowCamera&,
    const OriginalFollowSubject&,float elapsed,std::uint8_t raceFlags1B0,
    OriginalFollowPoseWork&) noexcept;
// Whole17A050: real17A870 world-query producer,17BB70,history C0/D0.
// The caller supplies the actual graph/history owners; this function overrides
// collision's distance/profile/velocity from the selected camera/subject.
// Always consumes three shared308030 draws after all preceding world queries.
OriginalFollowResult advanceOriginalFollowCamera(OriginalFollowCamera&,
    const OriginalFollowSubject&,float elapsed,std::uint8_t raceFlags1B0,
    const OriginalFollowCollisionSources&,OriginalCameraQueryPairOwner&,
    CoupledNoiseContext49M&,TVCameraUpdateOutput49F&,
    OriginalFollowCollisionResult&) noexcept;
OriginalFollowResult advanceOriginalFollowCameraView(OriginalFollowCamera&,
    const OriginalFollowSubject&,float elapsed,std::uint8_t raceFlags1B0,
    const OriginalFollowCollisionSources&,OriginalCameraQueryPairOwner&,
    CoupledNoiseContext49M&,TVCameraUpdateOutput49F&,CameraSceneState49F&,
    CameraViewOutput49F&,OriginalFollowCollisionResult&,
    std::uint8_t rootFlags228,std::uint8_t sceneFlags29) noexcept;
// Strict nearest-even separate binary32 operations; only normal/zero reached
// inputs/results. Uses the existing source142B80/13BFE0 rotation49U. This is
// bounded native arithmetic, not universal PS2 Q/MADD/denormal equivalence.
// Live disjoint owner/backing objects, serial access and real provenance are
// caller obligations. Rejection leaves represented outputs/state unchanged.
// World-query effects preceding pose publication require a caller-owned transaction.
}
