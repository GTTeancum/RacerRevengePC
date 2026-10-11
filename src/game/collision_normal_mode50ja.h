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
    bool acceptCompleted(const CollisionNormalModeWrite50JA& write) noexcept {
        if(!write.bound)return false;
        if(write.reached){value_=write.value;written_=true;}
        return true;
    }
    const std::uint32_t* boundWord() const noexcept {
        return written_?&value_:nullptr;
    }
private:
    std::uint32_t value_=0; // unread until written_, not a source initializer
    bool written_=false;
};
} // namespace rr::game
