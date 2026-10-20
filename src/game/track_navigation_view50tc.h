#pragma once
#include "track_navigation_view50mb.h"
#include <array>
#include <exception>
#include <memory>
#include <thread>
#include <utility>

namespace tc_cold_sources50LA { class Owner; }
namespace tc_navigation50TC { class Owner; }
namespace tc_collision_world50TC { class Owner; }
namespace rr::game {
enum class NavigationRole50TC : std::uint8_t { Map,Ai };
// A distinct TC aggregate is the sole issuer. It owns all actual topology and
// Ground/Storage spans, retains this same cold source, and never owns navigation.
class TcCollisionWorldLease final {
    std::shared_ptr<const tc_collision_world50TC::Owner> owner_;
    std::shared_ptr<const tc_cold_sources50LA::Owner> source_;
    const LiveZoneWorld50EN* world_=nullptr;
    LiveZoneWorld50EN descriptor_{};
    std::uint64_t generation_=0;
    std::thread::id issuingThread_{};
    friend class ::tc_collision_world50TC::Owner;
    TcCollisionWorldLease(std::shared_ptr<const tc_collision_world50TC::Owner> owner,
        std::shared_ptr<const tc_cold_sources50LA::Owner> source,
        const LiveZoneWorld50EN& world,std::uint64_t generation) noexcept
        :owner_(std::move(owner)),source_(std::move(source)),world_(&world),
         descriptor_(world),generation_(generation),issuingThread_(std::this_thread::get_id()){}
public:
    TcCollisionWorldLease()=default;
    TcCollisionWorldLease(const TcCollisionWorldLease&)=default;
    TcCollisionWorldLease(TcCollisionWorldLease&&)=default;
    TcCollisionWorldLease& operator=(const TcCollisionWorldLease&)=delete;
    TcCollisionWorldLease& operator=(TcCollisionWorldLease&&)=delete;
    ~TcCollisionWorldLease(){if(owner_&&issuingThread_!=std::this_thread::get_id())std::terminate();}
    bool valid() const noexcept {
        return issuingThread_==std::this_thread::get_id()&&owner_&&source_&&world_&&generation_&&
            world_->portalHeads==descriptor_.portalHeads&&world_->zoneCount==descriptor_.zoneCount&&
            world_->portalLinks==descriptor_.portalLinks&&world_->portalLinkCount==descriptor_.portalLinkCount&&
            world_->portals==descriptor_.portals&&world_->portalCount==descriptor_.portalCount;
    }
    const LiveZoneWorld50EN* world() const noexcept {return valid()?world_:nullptr;}
    const tc_cold_sources50LA::Owner* sourceIdentity() const noexcept {return valid()?source_.get():nullptr;}
    const tc_collision_world50TC::Owner* ownerIdentity() const noexcept {return valid()?owner_.get():nullptr;}
    std::uint64_t generation() const noexcept {return valid()?generation_:0;}
};
namespace navigation_storage_detail50TC {
// Reuse storage shapes, never MB's private Storage/Indices or MB admission.
using navigation_storage_detail50MB::Graph;
using navigation_storage_detail50MB::Reader;
using navigation_storage_detail50MB::Navigation;
using navigation_storage_detail50MB::Scratch;
using navigation_storage_detail50MB::Metadata;
struct Storage final {
    Navigation<268> map;Navigation<293> ai;
    Metadata<268> mapMetadata;Metadata<293> aiMetadata;
    std::array<std::uint32_t,268> mapFieldMask{};
    LiveZonePortalInput50EN authoredPortals[26]{};
    const tc_cold_sources50LA::Owner* const sourceIdentity;
    const std::thread::id ownerThread=std::this_thread::get_id();
    bool gatesReady=false;
    ~Storage(){if(ownerThread!=std::this_thread::get_id())std::terminate();}
private:
    explicit Storage(const tc_cold_sources50LA::Owner* source):sourceIdentity(source){}
    friend class ::tc_cold_sources50LA::Owner;
};
struct Indices final {
    std::uint8_t map[268]{},ai[293]{};
    const TcCollisionWorldLease world;
private:
    explicit Indices(const TcCollisionWorldLease& lease):world(lease){}
    friend class ::tc_navigation50TC::Owner;
};
}
class TrackNavigationView50TC;
class UndertrackTrackView50TC final {
    std::shared_ptr<const tc_cold_sources50LA::Owner> source_;
    std::shared_ptr<const navigation_storage_detail50TC::Storage> storage_;
    std::shared_ptr<const navigation_storage_detail50TC::Indices> indices_;
    NavigationRole50TC role_=NavigationRole50TC::Map;
    friend class TrackNavigationView50TC;
    UndertrackTrackView50TC(std::shared_ptr<const tc_cold_sources50LA::Owner> source,
        std::shared_ptr<const navigation_storage_detail50TC::Storage> storage,
        std::shared_ptr<const navigation_storage_detail50TC::Indices> indices,NavigationRole50TC role) noexcept
        :source_(std::move(source)),storage_(std::move(storage)),indices_(std::move(indices)),role_(role){}
public:
    UndertrackTrackView50TC()=default;
    bool valid() const noexcept {return source_&&storage_&&
        storage_->ownerThread==std::this_thread::get_id()&&storage_->sourceIdentity==source_.get()&&storage_->gatesReady;}
    bool runtimeValid() const noexcept {return valid()&&indices_&&indices_->world.valid()&&
        indices_->world.sourceIdentity()==source_.get()&&count()==capacity();}
    NavigationRole50TC role() const noexcept {return role_;}
    std::size_t capacity() const noexcept {return valid()?(role_==NavigationRole50TC::Map?268u:293u):0u;}
    std::uint32_t count() const noexcept {return valid()?(role_==NavigationRole50TC::Map?storage_->map.track.count:storage_->ai.track.count):0u;}
    const UndertrackNode50ET* nodes() const noexcept {return valid()?(role_==NavigationRole50TC::Map?storage_->map.track.nodes:storage_->ai.track.nodes):nullptr;}
};
class TrackNavigationView50TC final {
    std::shared_ptr<const tc_cold_sources50LA::Owner> source_;
    std::shared_ptr<const navigation_storage_detail50TC::Storage> storage_;
    std::shared_ptr<const navigation_storage_detail50TC::Indices> indices_;
    NavigationRole50TC role_=NavigationRole50TC::Map;
    friend class ::tc_cold_sources50LA::Owner;
    friend class ::tc_navigation50TC::Owner;
    friend class AiTrackMetadataView50TC;
    TrackNavigationView50TC(std::shared_ptr<const tc_cold_sources50LA::Owner> source,
        std::shared_ptr<const navigation_storage_detail50TC::Storage> storage,
        std::shared_ptr<const navigation_storage_detail50TC::Indices> indices,NavigationRole50TC role) noexcept
        :source_(std::move(source)),storage_(std::move(storage)),indices_(std::move(indices)),role_(role){}
public:
    TrackNavigationView50TC()=default;
    UndertrackTrackView50TC track() const noexcept {return {source_,storage_,indices_,role_};}
    bool gatesPrepared() const noexcept {return track().valid();}
    bool indicesPrepared() const noexcept {return track().runtimeValid();}
    bool runtimeValid() const noexcept {return indicesPrepared();}
    std::uint32_t count() const noexcept {return track().count();}
    std::size_t capacity() const noexcept {return track().capacity();}
    const TrackNavigationNode50EU* nodes() const noexcept {return gatesPrepared()?
        (role_==NavigationRole50TC::Map?storage_->map.nodes:storage_->ai.nodes):nullptr;}
    const std::uint8_t* nodeIndex78() const noexcept {return indicesPrepared()?
        (role_==NavigationRole50TC::Map?indices_->map:indices_->ai):nullptr;}
    NavigationRole50TC role() const noexcept {return role_;}
    const tc_cold_sources50LA::Owner* sourceIdentity() const noexcept {return gatesPrepared()?source_.get():nullptr;}
    std::uint64_t generation() const noexcept {return runtimeValid()?indices_->world.generation():0;}
    bool sameOwner(const TrackNavigationView50TC& other) const noexcept {
        return runtimeValid()&&other.runtimeValid()&&source_==other.source_&&storage_==other.storage_&&indices_==other.indices_;
    }
    bool sameWorld(const LiveZoneWorld50EN& world) const noexcept {
        return runtimeValid()&&indices_->world.world()==&world;
    }
};
class AiTrackMetadataView50TC final {
    TrackNavigationView50TC navigation_;
    friend class ::tc_cold_sources50LA::Owner;
    friend class ::tc_navigation50TC::Owner;
    explicit AiTrackMetadataView50TC(TrackNavigationView50TC n) noexcept:navigation_(std::move(n)){}
public:
    AiTrackMetadataView50TC()=default;
    bool bound() const noexcept {return navigation_.gatesPrepared()&&
        (navigation_.role_==NavigationRole50TC::Map?navigation_.storage_->mapMetadata.bound:navigation_.storage_->aiMetadata.bound);}
    bool runtimeValid() const noexcept {return bound()&&navigation_.runtimeValid();}
    NavigationRole50TC role() const noexcept {return navigation_.role();}
    std::uint32_t count() const noexcept {return bound()?navigation_.count():0;}
    std::size_t capacity() const noexcept {return bound()?navigation_.capacity():0;}
    const AiTrackNodeMetadata50IU* nodes() const noexcept {return bound()?
        (navigation_.role_==NavigationRole50TC::Map?navigation_.storage_->mapMetadata.nodes:navigation_.storage_->aiMetadata.nodes):nullptr;}
    float trackLength3C() const noexcept {return bound()?
        (navigation_.role_==NavigationRole50TC::Map?navigation_.storage_->mapMetadata.trackLength3C:navigation_.storage_->aiMetadata.trackLength3C):0.f;}
    const tc_cold_sources50LA::Owner* sourceIdentity() const noexcept {return bound()?navigation_.sourceIdentity():nullptr;}
    bool matches(const TrackNavigationView50TC& n) const noexcept {
        return bound()&&navigation_.sameOwner(n)&&navigation_.role()==n.role()&&count()==n.count();
    }
};
struct AiColdTrackBindingsView50TC {
    TrackNavigationView50TC mapFF0,aiFF4;
    AiTrackMetadataView50TC mapMetadata,aiMetadata;
};
inline bool bindAiColdTracks50IU(const TrackNavigationView50TC& map,const TrackNavigationView50TC& ai,
    const AiTrackMetadataView50TC& mapMetadata,const AiTrackMetadataView50TC& aiMetadata,
    const TrackNavigationProgress50EU& progress,AiColdTrackBindingsView50TC& out) noexcept {
    if(!map.sameOwner(ai)||map.role()!=NavigationRole50TC::Map||ai.role()!=NavigationRole50TC::Ai||
       !mapMetadata.matches(map)||!aiMetadata.matches(ai)||progress.reset.vehicleC80!=1||
       progress.vehicleC84!=0||progress.reset.vehicleC88!=1||progress.reset.vehicleC8C!=0)return false;
    out={map,ai,mapMetadata,aiMetadata};return true;
}
bool selectUndertrackReposition50ET(const UndertrackTrackView50TC&,
    const UndertrackProgress50ET&,const UndertrackInput50ET&,UndertrackSelection50ET&,UndertrackDiagnostic50ET&) noexcept;
bool undertrackBindings50EU(const TrackNavigationView50TC&,
    const std::uint32_t*,const std::uint32_t*,std::size_t,UndertrackTableBindings50ET&) noexcept;
bool bindUndertrackPlacement50ET(const UndertrackSelection50ET&,const TrackNavigationView50TC&,
    const std::uint32_t*,const std::uint32_t*,std::size_t,UndertrackPlacement50ET&,UndertrackDiagnostic50ET&) noexcept;
bool advancePlayerTrackNavigation50EU(const TrackNavigationView50TC&,const TrackNavigationView50TC&,
    const TrackNavigationInput50EU&,TrackNavigationProgress50EU&,TrackNavigationReceipt50EU&,TrackNavigationDiagnostic50EU&) noexcept;
bool advancePlayerTrackNavigationEffects50GH(const TrackNavigationView50TC&,const TrackNavigationView50TC&,
    const TrackNavigationInput50EU&,const TrackNavigationMapEffects50GH&,TrackNavigationProgress50EU&,
    TrackNavigationReceipt50EU&,TrackNavigationDiagnostic50EU&) noexcept;
}
