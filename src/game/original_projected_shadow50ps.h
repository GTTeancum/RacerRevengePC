#pragma once
// Original projected pod shadows, one path for every course and every pod.
// D:\Programming\GitHub\RacerRevengePC\work\scratch\pod-shadows\PLAN.md
// (sections 2, 3 and "Step 1 results").
//
//  274CD0  World CreateShadows(count): count = racers in a single-view race,
//          0 otherwise (Race+1B0 & 3 == 1 ? 17DB50 : 0).
//  1B32E0  per frame: light (231900), 1000-unit cull, caster list.
//          While a race is running (settings +58 bit 2 set, bit 0 clear) only
//          the human racers (manager +34) are processed, and a human whose
//          view camera is attached (mode 5) or on follow record 2 casts its
//          engines only. Otherwise chariot, engines, optional extra models
//          at LOD 2 and the four cable-shadow meshes at LOD 0. Cables never
//          cast. LOD = min(434360/434368 = 2, model LOD count - 1) (2316D0).
//  231DF0  caster box, light camera fit, orthographic caster transform and
//          the texture projection used by the receivers.
//  2DEBC0 / 2DE900  receiver gather: static models of the camera's zone and,
//          when 383650 is set (always), of every zone reached through portals
//          the shadow camera sees (230880 / 263260 / 2DE7C0); each model must
//          allow shadow receiving, pass camera distance - radius < 500, then
//          |centre - C|^2 - radius^2 < 400 or the shadow-camera box test
//          (2B0370 -> 2B0400); at most 200. Only static models receive.
//  240C00  main-camera box test that selects the receiver's light-facing rule.
//
// Everything here is host memory with fixed capacities; nothing is written
// into game-owned structures. Every array index is checked before the write
// and an input that does not fit is refused, not truncated.
#include <array>
#include <cstdint>

namespace rr::game {
struct VehicleDrawSubmission50CM;

inline constexpr std::uint32_t maxProjectedShadows50PS = 8;     // 8 racers (archive has shadowCaster1..9)
inline constexpr std::uint32_t projectedShadowTargetSize50PS = 256; // shadowCasterN.psm RenderTarget
inline constexpr std::uint32_t maxShadowReceivers50PS = 200;    // 2DEB74 slti 0xC8
inline constexpr float projectedShadowCullDistanceSquared50PS = 1.0e6f; // 1B32E0: 1000 units
inline constexpr float projectedShadowLightDistance50PS = 800.f;        // 382EA8
inline constexpr float shadowReceiverCameraLimit50PS = 500.f;   // ObjectShadow +364 (2DE000)
inline constexpr float shadowReceiverNearLimit50PS = 400.f;     // ObjectShadow +368 (2DE000)
// PSXCamera defaults (1144D8..114514), not changed for the shadow camera.
inline constexpr float shadowCameraNear50PS = 1.f, shadowCameraFar50PS = 99840.f;
// 1B32E0 view rule: camera mode 5 (attached), or mode 0 on follow record 2.
inline constexpr std::uint32_t shadowAttachedCameraMode50PS = 5, shadowFollowCameraMode50PS = 0,
    shadowCloseFollowRecord50PS = 2;
inline constexpr std::uint32_t maxShadowZones50PS = 256;        // WorldLimits48F::zones
inline constexpr std::uint32_t maxShadowPortals50PS = 4096;     // WorldLimits48F::portals
inline constexpr std::uint32_t projectedShadowBodyLod50PS = 2;  // 434360 / 434368
inline constexpr std::uint32_t projectedShadowCableLod50PS = 0; // 1B32E0 cable-shadow meshes
// Receiver darkening: vertex alpha 127, TEXA 32 -> source alpha 31/128,
// blend Cd + (0 - Cd) * 31 >> 7 = Cd * 97/128 (the 76 percent result).
inline constexpr float projectedShadowSourceAlpha50PS = 31.f / 128.f;
// 240C00 guard band of the original 640x448 single view (GS window 2048 +-
// half size, guard edge 4080): (4080 - 2048 - half) / half, per axis.
inline constexpr float projectedShadowGuardX50PS = (4080.f - 2048.f - 320.f) / 320.f;
inline constexpr float projectedShadowGuardY50PS = (4080.f - 2048.f - 224.f) / 224.f;

// Caster slots in 1B32E0 order. Values are the racer offsets they read.
enum class ShadowCasterSlot50PS : std::uint8_t {
    chariot1B4, right1B8, left1BC, extra1C0, extra1C4,
    cableL01_1D0, cableR01_1D4, cableL02_1D8, cableR02_1DC
};
inline constexpr std::uint32_t shadowCasterSlotCount50PS = 9;
inline constexpr std::uint32_t firstCableShadowSlot50PS = 5;

// Cable-shadow casters are small CPU-placed meshes (four per pod, 1360-byte
// PSGs). World-space triangle list per racer, all four meshes together.
inline constexpr std::uint32_t maxCableShadowTriangles50PS = 256;

struct ShadowBox50PS {
    float minimum[3]{}, maximum[3]{};
    bool valid = false;
};

// One racer as 1B32E0 sees it. Slot boxes are world boxes built exactly as
// 231DF0 B.1 (local +160/+170 through the model world matrix, abs-sum
// extents); see shadowBoxFromSubmission50PS. An invalid box = null slot.
struct ShadowRacerInput50PS {
    bool present = false;
    // 1B32E0 1B35B0..1B3604, read only while viewMode58: race +48[i] is view
    // i's camera; (camera +44 == 5) || (camera +44 == 0 && camera +64 -> +100
    // == 2), i.e. attached camera or follow record 2. Engines only.
    bool engineOnlyView = false;
    // r+58 != 0 (racer class58: 0 for humans); consulted only with viewMode58.
    bool viewRule58 = false;
    // Model LOD count (+1B4 -> +10) per slot; 0 for a null slot.
    std::uint32_t lodCount[shadowCasterSlotCount50PS]{};
    ShadowBox50PS box[shadowCasterSlotCount50PS]{};
    // 1B32E0 distance reference: world position of r+1B4 plus model +130.
    float reference[3]{};
    bool referenceValid = false;
};

struct ShadowFrameInput50PS {
    std::uint64_t frame = 0;
    std::uint32_t raceViews = 1;          // Race+1B0 & 3
    std::uint32_t racerCount = 0;         // 17DB50
    // settings (race +B4) +58: bit 2 && !bit 0 (race running, not ended);
    // selects manager +34 (humans) as the racer count.
    bool viewMode58 = false;
    std::uint32_t viewRacerCount = 0;     // manager +34
    const ShadowRacerInput50PS* racers = nullptr; // racerCount entries, slot order
    float cameraEye[3]{};                 // active camera +180
    std::uint32_t cameraZone = 0;         // active camera zone, 1-based like model +28; 0 = unknown
};

struct ShadowCaster50PS {
    std::uint8_t slot = 0;  // ShadowCasterSlot50PS
    std::uint8_t lod = 0;   // min(requested, lodCount - 1) (2316D0)
};

struct ProjectedShadow50PS {
    bool culled = true;          // ps+14C
    std::uint32_t racer = 0;     // racer slot (0 = player)
    std::uint32_t casterCount = 0;
    ShadowCaster50PS casters[shadowCasterSlotCount50PS]{};
    // 231900 light basis: d (towards the ground), side t, up.
    float light[3]{}, side[3]{}, up[3]{};
    // 231DF0 fit.
    float boxMinimum[3]{}, boxMaximum[3]{};   // ps+D0 / ps+E0
    float centre[3]{}, halfSize[3]{};         // ps+C0 / ps+B0
    float radius = 0.f, inverseRadius = 0.f;  // ps+12C / ps+130
    float eye1[3]{}, fovDegrees = 0.f;        // perspective camera: receiver frustum only
    // cam+100 of that camera (236DD0 from eye1): GL column-major, column
    // vectors, PS2 clip (|x|,|y|,|z| <= |w|): clip = M * (P, 1).
    float receiverClip[16]{};
    // GL column-major, column vectors: clip = casterClip * (P, 1).
    // x = dot(P - C, side) / r, y = dot(P - C, up) / r, z = 0, w = 1.
    float casterClip[16]{};
    // GL column-major, column vectors: (s, t) = (texture * (P, 1)).xy.
    // s = 0.5 + 0.5 x, t = 0.5 + 0.5 y (GL rows bottom-up = GS -0.5 y).
    float texture[16]{};
    // Cable-shadow casters in world space (filled by the cable-shadow owner).
    std::uint32_t cableTriangles = 0;
    float cableVertices[maxCableShadowTriangles50PS * 9]{};
    bool hasCasters() const noexcept { return !culled && casterCount != 0; }
};

struct OriginalProjectedShadowFrame50PS {
    std::uint64_t frame = 0;
    // Copied from the input for the receiver gather (renderer side).
    float cameraEye[3]{};
    std::uint32_t cameraZone = 0;
    std::uint32_t shadowCount = 0;        // world+4AC
    std::uint32_t racersConsidered = 0;   // fp in 1B32E0
    std::uint32_t culled = 0, viewSkipped = 0, engineOnly = 0, active = 0;
    // Racers with casters whose combined box has zero radius (cannot be fit;
    // the original would divide by zero). Their shadow draws nothing.
    std::uint32_t degenerate = 0;
    // Caster slots 1B32E0 asked for that the port could not supply a box for
    // (null in the port but non-null in the original rule).
    std::uint32_t missingSlots = 0;
    ProjectedShadow50PS shadows[maxProjectedShadows50PS]{};
};

enum class ProjectedShadowError50PS : std::uint8_t {
    none, input, capacity, arithmetic
};
struct ProjectedShadowDiagnostic50PS {
    ProjectedShadowError50PS error = ProjectedShadowError50PS::none;
    std::uint32_t racer = 0, slot = 0;
    const char* operation = nullptr;
};

// 274CD0 + 1B32E0 + 231900 + 231DF0 for one frame. Output is rebuilt from
// scratch; on failure it is left with shadowCount = 0 (no shadows drawn).
bool buildProjectedShadows50PS(const ShadowFrameInput50PS& input,
    OriginalProjectedShadowFrame50PS& output, ProjectedShadowDiagnostic50PS& diagnostic) noexcept;

// 231DF0 B.1 for one drawn part. Uses the part's source model world matrix
// and +160/+170 local bounds when the submission carries them; otherwise the
// part's world display bounds (a port stand-in, logged by the caller).
bool shadowBoxFromSubmission50PS(const VehicleDrawSubmission50CM& part, ShadowBox50PS& box,
    bool& sourceBounds) noexcept;
// Same B.1 rule for any row-vector model world matrix (BodyMatrix48G layout).
bool shadowBoxFromLocal50PS(const float (&world)[16], const float (&centre160)[3],
    const float (&half170)[3], ShadowBox50PS& box) noexcept;

// ---- Receivers (2DEBC0 / 2DE900 / 240C00) --------------------------------

// One static world instance. Bounds are world space.
struct ShadowReceiverStatic50PS {
    std::uint32_t instance = 0;   // world instance index (draw-range key)
    std::uint32_t zone = 0;       // model +28 (1-based zone number)
    bool receives = false;        // +1B3 bit 4 (TOB AllowShadowReceiving)
    float centre[3]{};            // model world position + model +130
    float halfSize[3]{};          // box half size (240C00 corners)
    float radius = 0.f;           // +198 (or +194 when +1A0 == 1)
};
struct ShadowReceiverView50PS {
    std::uint32_t cameraZone = 0;           // active camera current zone; 0 = unknown
    float cameraEye[3]{};
    // Main camera, GL clip (z in -w..w). Same storage as
    // VehicleWindowFrame50DS::worldViewProjection: column-major, used as
    // vec4(P, 1) * M, so clip[j] = sum_i P[i] * M[j * 4 + i].
    const float* worldViewProjection = nullptr;
};
// Portal between two zones (1-based, as WorldPortal48F), four world corners.
struct ShadowPortal50PS {
    std::uint32_t zone[2]{};
    float corner[4][3]{};
};
struct ShadowReceiverWorld50PS {
    const ShadowReceiverStatic50PS* statics = nullptr; // world (zone) order
    std::uint32_t staticCount = 0;
    const ShadowPortal50PS* portals = nullptr;
    std::uint32_t portalCount = 0;
    std::uint32_t zoneCount = 0;                       // highest zone number
};
struct ShadowReceiverList50PS {
    std::uint32_t count = 0;
    std::uint32_t instance[maxShadowReceivers50PS]{};
    // 240C00 result per receiver: true = clipping path (light-facing drop).
    std::uint8_t facingRule[maxShadowReceivers50PS]{};
    // Counters for the verification log.
    std::uint32_t zonesVisited = 0, portalsSeen = 0, portalsVisible = 0, duplicates = 0,
        flagRejected = 0, cameraRejected = 0, nearAccepted = 0, frustumAccepted = 0,
        frustumRejected = 0, capped = 0;
};
bool gatherShadowReceivers50PS(const ProjectedShadow50PS& shadow,
    const ShadowReceiverWorld50PS& world, const ShadowReceiverView50PS& view,
    ShadowReceiverList50PS& output) noexcept;
// 2B0370 -> 2B0400: box (centre +- half) through M; false when all eight
// corners are outside one PS2 clip plane.
bool shadowBoxVisible50PS(const float (&centre)[3], const float (&half)[3], const float (&m)[16]) noexcept;
// 240C00 for one receiver box: true when the box is outside the main view,
// crosses the near plane, or crosses the guard band (far-only does not count).
bool shadowReceiverFacingRule50PS(const ShadowReceiverStatic50PS& receiver,
    const float* worldViewProjection) noexcept;
} // namespace rr::game
