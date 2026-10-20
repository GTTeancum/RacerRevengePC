#pragma once
#include "live_zone_membership50en.h"
#include <exception>
#include <memory>
#include <thread>
#include <utility>
namespace rc_cold_sources50LA { class Owner; }
namespace rc_collision_world50RC { class Owner; }
namespace rr::game {
// Distinct RC authority. Only the actual RC aggregate may issue this lease.
// It retains stable Application topology and the exact same cold source.
class RcCollisionWorldLease final {
    std::shared_ptr<const rc_collision_world50RC::Owner> owner_;
    std::shared_ptr<const rc_cold_sources50LA::Owner> source_;
    const LiveZoneWorld50EN* world_=nullptr;
    LiveZoneWorld50EN descriptor_{};
    std::uint64_t generation_=0;
    std::thread::id issuingThread_{};
    friend class ::rc_collision_world50RC::Owner;
    RcCollisionWorldLease(std::shared_ptr<const rc_collision_world50RC::Owner> owner,
        std::shared_ptr<const rc_cold_sources50LA::Owner> source,
        const LiveZoneWorld50EN& world,std::uint64_t generation) noexcept
        :owner_(std::move(owner)),source_(std::move(source)),world_(&world),
         descriptor_(world),generation_(generation),issuingThread_(std::this_thread::get_id()){}
public:
    RcCollisionWorldLease()=default;
    RcCollisionWorldLease(const RcCollisionWorldLease&)=default;
    RcCollisionWorldLease(RcCollisionWorldLease&&)=default;
    RcCollisionWorldLease& operator=(const RcCollisionWorldLease&)=delete;
    RcCollisionWorldLease& operator=(RcCollisionWorldLease&&)=delete;
    ~RcCollisionWorldLease(){if(owner_&&issuingThread_!=std::this_thread::get_id())std::terminate();}
    bool valid() const noexcept {
        return issuingThread_==std::this_thread::get_id()&&owner_&&source_&&world_&&generation_&&
            world_->portalHeads==descriptor_.portalHeads&&world_->zoneCount==descriptor_.zoneCount&&
            world_->portalLinks==descriptor_.portalLinks&&world_->portalLinkCount==descriptor_.portalLinkCount&&
            world_->portals==descriptor_.portals&&world_->portalCount==descriptor_.portalCount;
    }
    const LiveZoneWorld50EN* world() const noexcept {return valid()?world_:nullptr;}
    const rc_cold_sources50LA::Owner* sourceIdentity() const noexcept {return valid()?source_.get():nullptr;}
    const rc_collision_world50RC::Owner* ownerIdentity() const noexcept {return valid()?owner_.get():nullptr;}
    std::uint64_t generation() const noexcept {return valid()?generation_:0;}
};
}
