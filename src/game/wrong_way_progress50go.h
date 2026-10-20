#pragma once
#include "lap_progress50gf.h"

namespace rr::game {
// Source 1A2E30, called at 1A1620 after navigation and before undertrack.
// Own these fields in the same player as 50GF/50ET. In particular, navigation
// consumes the PREVIOUS update's flags10CC bit1; never clear it before that.
struct WrongWayProgressState50GO {
    std::uint32_t vehicleFF8=0;
    std::uint8_t flags10CC=0;
};
struct WrongWayProgressInput50GO {
    std::uint32_t application34C=0,vehicle40=0;
    std::uint32_t vehicleC8C=0,vehicleC88=0,viewportMode8C=0;
    std::uint8_t config58=0,vehicleCD8=0;
    // 28A530's first output: vehicle340->288 LOCAL matrix E0/E4/E8.
    // SceneNode49P.localC0.v[8..10], not world basis or frame displacement.
    float localForwardE0[3]{};
    // The actual native app28C->17C[vehicle5C] owner, or its actual absence.
    // No guest pointer word, fabricated owner, or implicit HUD implementation.
    void* warningOwner=nullptr;
};
enum class WrongWayHudOperation50GO : std::uint8_t {
    None,SetColor,SetPosition,SetText,Show,Hide
};
struct WrongWayHudRequest50GO {
    WrongWayHudOperation50GO operation=WrongWayHudOperation50GO::None;
    void* owner=nullptr;
    std::uint32_t callPC=0,calleePC=0,returnPC=0;
    std::uint32_t argumentA1=0,argumentA2=0,argumentA3=0;
    // Positional source f12..f19 and the two stack float arguments to Show.
    float argumentF[8]{},stack0=0.f,stack8=0.f;
    // SetText alone supplies the exact original string, including terminator.
    // Its native pointer is NOT argumentA1 or a guest address.
    const char* text=nullptr;
};
struct WrongWayProgressContinuation50GO {
    WrongWayHudRequest50GO pending{};
    // Native continuation storage for source f20/f21 captured before callbacks.
    std::uint32_t positionX=0,positionY=0;
};
struct WrongWayProgressReceipt50GO {
    bool markerCursorFilled=false,directionEvaluated=false,wrongWay=false;
    bool warningBranchReached=false;
    float directionDot=0.f,sourceAngle=0.f;
};
enum class WrongWayProgressError50GO : std::uint8_t {
    None,RoundingMode,Track,Arithmetic,CallbackPending,NoPendingCallback,
    InvalidContinuation
};
struct WrongWayProgressDiagnostic50GO {
    WrongWayProgressError50GO error=WrongWayProgressError50GO::None;
    std::uint32_t sourcePC=0;
};
// Begins the concrete source operation. It clears ONLY 10CC bit1; app34C's
// nonzero branch fills FF8 with actual map ProgressNode count and returns.
// Otherwise config58 bit1=1/bit0=0 admits the direction/warning path. CD8 bit0
// or vehicle40 suppresses direction computation, but still takes the hide path.
// Uses the exact scalar normalization, VU dot, and 13C770 polynomial, without
// substituting libm acos or the different originalAngle48G atan approximation.
//
// Success commits the represented state and may stop at the FIRST HUD call.
// pending.operation!=None means the operation is NOT complete. The caller must
// execute every requested native effect and resume in order. Actual null HUD
// ownership requires no calls. Arithmetic/input failure preserves all state,
// continuation and receipt; source-committed callback effects are never undone.
// Caller-backed/disjoint serial records, strict binary32, FE_TONEAREST, finite
// consumed values/intermediates and normal positive normalization squared norms
// (exact zero is supported). No allocation, I/O, guest execution or callbacks.
bool beginWrongWayProgress50GO(const UndertrackTrack50ET& map,
    const LapProgressTrack50GF& markers,const WrongWayProgressInput50GO&,
    WrongWayProgressState50GO&,WrongWayProgressContinuation50GO&,
    WrongWayProgressReceipt50GO&,WrongWayProgressDiagnostic50GO&) noexcept;
bool beginWrongWayProgress50GO(const TrackNavigationView50EU& map,
    const LapProgressTrackView50MB& markers,const WrongWayProgressInput50GO&,
    WrongWayProgressState50GO&,WrongWayProgressContinuation50GO&,
    WrongWayProgressReceipt50GO&,WrongWayProgressDiagnostic50GO&) noexcept;
// TC-CONSUMER-BEGIN 0
bool beginWrongWayProgress50GO(const TrackNavigationView50TC& map,
    const LapProgressTrackView50TC& markers,const WrongWayProgressInput50GO&,
    WrongWayProgressState50GO&,WrongWayProgressContinuation50GO&,
    WrongWayProgressReceipt50GO&,WrongWayProgressDiagnostic50GO&) noexcept;
// TC-CONSUMER-END 0

// TB-CONSUMER-BEGIN 0
bool beginWrongWayProgress50GO(const TrackNavigationView50TB& map,
    const LapProgressTrackView50TB& markers,const WrongWayProgressInput50GO&,
    WrongWayProgressState50GO&,WrongWayProgressContinuation50GO&,
    WrongWayProgressReceipt50GO&,WrongWayProgressDiagnostic50GO&) noexcept;
// TB-CONSUMER-END 0

// Invoke only after pending's actual native effect has completed. This advances
// SetColor -> SetPosition -> SetText -> Show -> complete, or Hide -> complete.
// A missing native HUD effect is not completion. Pending owner and arguments
// must remain unchanged between begin/resume; never restart a suspended frame.
bool resumeWrongWayProgress50GO(WrongWayProgressContinuation50GO&,
    WrongWayProgressDiagnostic50GO&) noexcept;
}
