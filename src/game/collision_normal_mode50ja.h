#pragma once
#include <cstdint>

namespace rr::game {
// Effect of an actually reached121438/126FA0 call. A successful empty or
// filtered-out query has reached=false and cannot overwrite3DF5B0.
// Repeated dispatches within these bounded calls all use the same immutable
// query mode; their callbacks cannot read/write3DF5B0. The final write is the
// exact observable effect at the atomic query's Complete boundary.
struct CollisionNormalModeWrite50JA {
    std::uint32_t value=0;
    bool reached=false;
    bool bound=false; // authentic query-owned mode was supplied
};
// The same application collision namespace also owns121920's lazy sentinel
// and11DD00's separate vector-buffer counts. Native shape2/3/5 world calls
// reset both counts and do not use the shape0/1 vector-buffer finalizers.
// No source value is exposed until a real completed world call writes it.
struct CollisionWorldQueryGlobals50JA {
    std::uint8_t initializedGp8184=0;
    std::uint32_t sentinel3E0860=0,count3DFDC0=0,count3DFDC8=0;
};
inline void recordCollisionNormalMode50JA(CollisionNormalModeWrite50JA& write,
    std::uint32_t actualShapeMode) noexcept {
    if(write.bound){write.value=actualShapeMode;write.reached=true;}
}
// Application owner, outside every racer bank and cold-session lifetime. No
// source initial value is assumed. Reset cannot borrow the word until a real
// completed writer call has established it. Query failure remains its existing
// terminal atomic-call boundary; a stale result must never be published.
class CollisionNormalModeOwner50JA {
public:
    void acceptCompletedWorldReset() noexcept {
        if(!worldGlobals_.initializedGp8184){worldGlobals_.sentinel3E0860=0xffffffffu;worldGlobals_.initializedGp8184=1;}
        worldGlobals_.count3DFDC0=0;worldGlobals_.count3DFDC8=0;worldGlobalsWritten_=true;
    }
    const CollisionWorldQueryGlobals50JA* completedWorldGlobals()const noexcept {
        return worldGlobalsWritten_?&worldGlobals_:nullptr;
    }
    bool acceptCompleted(const CollisionNormalModeWrite50JA& write) noexcept {
        if(!write.bound)return false;
        if(write.reached){value_=write.value;written_=true;}
        return true;
    }
    const std::uint32_t* boundWord() const noexcept {
        return written_?&value_:nullptr;
    }
private:
    CollisionWorldQueryGlobals50JA worldGlobals_{};
    bool worldGlobalsWritten_=false;
    std::uint32_t value_=0; // unread until written_, not a source initializer
    bool written_=false;
};
} // namespace rr::game
