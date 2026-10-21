#pragma once
#include <cstdint>
#include <bit>
#include <memory>
#include <utility>

namespace original_main_entry50KG { struct Application; }
namespace rr::game {
// Application382B80 lanes, not a host frame counter or legacy UI active time.
struct OriginalApplicationAnimationClockSample50KG {
    std::uint32_t current230=0,pause11C=0,pending120=0;
};
// Actual140C50 uncapped Application250 publication, valid only between the
// original update begin/commit boundaries. No caller-supplied delta is accepted.
struct OriginalApplicationUpdateClockSample50KG {
    std::uint32_t current230=0,previous234=0;
    float elapsed250=0;
    std::uint64_t publication=0;
};
class OriginalApplicationAnimationClockView50KG;
class OriginalApplicationAnimationClock50KG final {
    friend struct original_main_entry50KG::Application;
    friend class OriginalApplicationAnimationClockView50KG;
    OriginalApplicationAnimationClock50KG() noexcept=default;
    //13FC44/4C/50 initialize the words. Zero alone is not a sampled timestamp.
    OriginalApplicationAnimationClockSample50KG value_{};
    bool published_=false,applicationAlive_=true;
    std::uint32_t previous234_=0; //13FC40.
    float elapsed250_=0; //13FCBC; distinct from capped world/update factor23C.
    std::uint64_t updatePublication_=0;
    bool updateInProgress_=false;
    static float delta140BEC(std::uint32_t current,std::uint32_t previous)noexcept {
        const std::uint32_t delta=current-previous; //Original unsigned wrapping difference.
        float converted=0;
        if(!(delta&0x80000000u))converted=static_cast<float>(delta);
        else {
            //140C04..1C: logical half OR low bit, signed-positive CVT.S.W,
            //then add the converted value to itself. Preserve this source path.
            volatile float half=static_cast<float>((delta>>1)|(delta&1u));
            volatile float doubled=half+half;converted=doubled;
        }
        volatile float result=converted*std::bit_cast<float>(std::uint32_t{0x3a83126f});
        return result;
    }
public:
    ~OriginalApplicationAnimationClock50KG()=default;
    OriginalApplicationAnimationClock50KG(const OriginalApplicationAnimationClock50KG&)=delete;
    OriginalApplicationAnimationClock50KG& operator=(const OriginalApplicationAnimationClock50KG&)=delete;
};
// Main-thread-only live capability. Pins publication backing, not Application
// or Video; it has no clock sampler, callback, writer or GPU-readiness claim.
// Re-read at EACH original boundary:29AC90 entry, texture leaf reads, and
//29AD48 after callbacks (even callback=false). Do not cache the entry stamp
// as the final stamp across a callback that can publish application time.
class OriginalApplicationAnimationClockView50KG final {
    friend struct original_main_entry50KG::Application;
    std::shared_ptr<const OriginalApplicationAnimationClock50KG> owner_;
    explicit OriginalApplicationAnimationClockView50KG(
        std::shared_ptr<const OriginalApplicationAnimationClock50KG> owner) noexcept
        :owner_(std::move(owner)) {}
public:
    OriginalApplicationAnimationClockView50KG() noexcept=default;
    // Identity of the same still-live Application publication owner. Equal
    // sampled values from two different applications do not confer authority.
    bool sameOwner(const OriginalApplicationAnimationClockView50KG& other) const noexcept {
        return owner_&&other.owner_&&owner_.get()==other.owner_.get()&&
            !owner_.owner_before(other.owner_)&&!other.owner_.owner_before(owner_)&&
            owner_->applicationAlive_;
    }
    bool readUpdate250(OriginalApplicationUpdateClockSample50KG& out)const noexcept {
        if(!owner_||!owner_->applicationAlive_||!owner_->updateInProgress_||!owner_->updatePublication_)return false;
        out={owner_->value_.current230,owner_->previous234_,owner_->elapsed250_,owner_->updatePublication_};
        return true;
    }
    //13FC4C initialized this lane independently of timestamp publication.
    // Read the actual shared pause owner; never invent a sampled timestamp.
    bool readPause11C(std::uint32_t& out)const noexcept {
        if(!owner_||!owner_->applicationAlive_)return false;
        out=owner_->value_.pause11C;return true;
    }
    bool read(OriginalApplicationAnimationClockSample50KG& out) const noexcept {
        if(!owner_||!owner_->applicationAlive_||!owner_->published_)return false;
        out=owner_->value_;return true;
    }
};
}
