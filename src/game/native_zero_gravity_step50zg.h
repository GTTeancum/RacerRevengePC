#pragma once
// ============================================================================
// PC ENHANCEMENT, NOT ORIGINAL BEHAVIOUR: N64 zero gravity, the per-step
// context the frame code reads. Light header (std only) so the force/contact
// code (vehicle_frame49w.cpp), the aero code and the follow camera can include
// it without pulling in the reader. The latch, the racing-line table and the
// rates live in native_zero_gravity50zg.h.
//
// ONE context, host memory only. The physics step of one racer sets it for
// exactly the duration of that racer's step (ScopedZeroGravityStep50ZG) and
// clears it on every exit, including exceptions. Outside that scope, and for
// a racer in Normal mode, it is inactive and every reader takes the original
// path unchanged. Nothing here points into game or guest memory.
// ============================================================================
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
enum class ZeroGravityMode50ZG : std::uint8_t {
    Normal=0,  // original behaviour
    Flight=1,  // N64 ZON  (flags0 0x02000000)
    Recover=2, // N64 ZOFF (flags0 0x04000000)
};
inline constexpr std::size_t zeroGravitySlots50ZG=8;

struct ZeroGravityStep50ZG {
    bool active=false;                 // inside one racer's physics step
    ZeroGravityMode50ZG mode=ZeroGravityMode50ZG::Normal;
    float dt=0.f;                      // this step's factor (s)
    // Spline follow (N64 0x8006AF48), see native_zero_gravity50zg.h.
    float lookAhead[3]{};              // world point on the racing line
    float forwardBlend=0.f;            // 1-0.8^(f0 dt)
    float pitchRateDegrees=0.f;        // zeroGPitchRate (+0x220) x 1.5, deg/s
    float flightRollRateDegrees=0.f;   // ZON roll rate (+0x21C), deg/s
    float recoverRollGain=0.f;         // ZOFF: roll rate = gain x bank error (1/s)
    bool haveSurfaceNormal=false;      // ZOFF: surface whose bank the pod rights to
    float surfaceNormal[3]{0.f,1.f,0.f};
    // Results the frame reports back (host side, for logs only).
    bool basisWritten=false;
    float bankErrorDegrees=0.f;
};
inline ZeroGravityStep50ZG zeroGravityStep50ZG;

// True only while one racer's step is in zero-g Flight / Recover.
inline bool zeroGravityStepIs50ZG(ZeroGravityMode50ZG mode) noexcept {
    return zeroGravityStep50ZG.active&&zeroGravityStep50ZG.mode==mode;
}
inline bool zeroGravityStepAny50ZG() noexcept {
    return zeroGravityStep50ZG.active&&zeroGravityStep50ZG.mode!=ZeroGravityMode50ZG::Normal;
}

// Each racer's mode as of its last completed step (written by the latch,
// per slot, bounds-checked there). Read outside the physics step by the
// follow camera (N64 chase cam tests ZON|ZOFF, 0x80041FF0, mask 0x06000000)
// and the AI steering (N64 autopilot measures heading about world_gravity,
// which is -pod up in ZON).
inline bool zeroGravityFlightForSlot50ZG(std::size_t slot) noexcept;
inline std::array<ZeroGravityMode50ZG,zeroGravitySlots50ZG> zeroGravityPublishedMode50ZG{};
inline bool zeroGravityFlightForSlot50ZG(std::size_t slot) noexcept {
    return slot<zeroGravitySlots50ZG&&zeroGravityPublishedMode50ZG[slot]==ZeroGravityMode50ZG::Flight;
}

// AI steering frame (patch C, ai_runtime50iv.h). The original 172E60 measures
// heading in world XZ with a world-up sign flip; the N64 autopilot in ZON
// measures it about the pod's own up. podFrame() re-expresses the same
// direction in the pod's basis (local copies only), up.y 1, yaw rate about
// the pod's up. Untouched (original values) unless called.
//
// The frame is the pod's basis turned 90 deg about its own up, so the pod's
// forward is (1,0,0), not (0,0,1). Reason: 172E60 reads both angles with the
// original angle reader (player_finish50ia.h directionAngle), whose x == 0
// branch returns 0 for +z instead of +pi/2. In the plain pod basis the pod's
// own forward is exactly (0,0,1), so every flight step read the pod as 90 deg
// off its target: steering pinned at -1 (full left), one engine at 1 %, and the
// roll rate pinned at its limit (silo-veng-r3/r16/r22, zerog136h-TA). A turn
// about up is a pure shift of that angle measure (atan2(z,x) -> -pi/2), so the
// heading error 172E60 forms is the same as in the plain pod basis; only the
// exact-zero branch is no longer hit. direction = (ahead, up, -right) . d.
struct AiSteerFrame50ZG {
    float direction[3]{},forward[3]{},upY=0.f,angularA4=0.f;
    AiSteerFrame50ZG(const float d[3],const float f[3],float y,float a) noexcept {
        for(unsigned i=0;i<3;++i){direction[i]=d[i];forward[i]=f[i];}upY=y;angularA4=a;}
    // local16 = parentless main190 matrix (rows right, up, forward at 0, 4, 8);
    // angular = world angular velocity.
    void podFrame(const float* local16,const float* angular) noexcept {
        const float* right=local16;const float* up=local16+4;const float* ahead=local16+8;
        const float d[3]{direction[0],direction[1],direction[2]};
        const float alongRight=d[0]*right[0]+d[1]*right[1]+d[2]*right[2];
        const float alongUp=d[0]*up[0]+d[1]*up[1]+d[2]*up[2];
        const float alongAhead=d[0]*ahead[0]+d[1]*ahead[1]+d[2]*ahead[2];
        direction[0]=alongAhead;direction[1]=alongUp;direction[2]=-alongRight;
        forward[0]=1.f;forward[1]=0.f;forward[2]=0.f;upY=1.f;
        angularA4=angular[0]*up[0]+angular[1]*up[1]+angular[2]*up[2];
    }
};

class ScopedZeroGravityStep50ZG {
public:
    explicit ScopedZeroGravityStep50ZG(const ZeroGravityStep50ZG& step) noexcept {
        zeroGravityStep50ZG=step;zeroGravityStep50ZG.active=step.mode!=ZeroGravityMode50ZG::Normal;
    }
    ~ScopedZeroGravityStep50ZG(){zeroGravityStep50ZG=ZeroGravityStep50ZG{};}
    ScopedZeroGravityStep50ZG(const ScopedZeroGravityStep50ZG&)=delete;
    ScopedZeroGravityStep50ZG& operator=(const ScopedZeroGravityStep50ZG&)=delete;
};
} // namespace rr::game
