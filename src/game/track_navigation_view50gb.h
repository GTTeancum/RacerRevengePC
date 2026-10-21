#pragma once
#include "track_navigation_view50mb.h"
#include <array>
#include <exception>
#include <memory>
#include <thread>
#include <utility>

namespace gb_cold_sources50LA { class Owner; }
namespace gb_navigation50GB { class Owner; }
namespace gb_collision_world50GB { class Owner; }
namespace rr::game {
enum class NavigationRole50GB : std::uint8_t { Map,Ai };
// A distinct GB aggregate is the sole issuer. It owns all actual topology and
// Ground/Storage spans, retains this same cold source, and never owns navigation.
struct GbCollisionWorldBorrowState final {
    std::size_t leases=0;
    const std::thread::id thread=std::this_thread::get_id();
};
class GbCollisionWorldLease final {
    std::shared_ptr<const gb_collision_world50GB::Owner> owner_;
    std::shared_ptr<const gb_cold_sources50LA::Owner> source_;
    const LiveZoneWorld50EN* world_=nullptr;
    LiveZoneWorld50EN descriptor_{};
    std::uint64_t generation_=0;
    std::thread::id issuingThread_{};
    GbCollisionWorldBorrowState* borrowState_=nullptr;
    friend class ::gb_collision_world50GB::Owner;
    GbCollisionWorldLease(std::shared_ptr<const gb_collision_world50GB::Owner> owner,
        std::shared_ptr<const gb_cold_sources50LA::Owner> source,
        const LiveZoneWorld50EN& world,std::uint64_t generation,GbCollisionWorldBorrowState& state) noexcept
        :owner_(std::move(owner)),source_(std::move(source)),world_(&world),
         descriptor_(world),generation_(generation),issuingThread_(std::this_thread::get_id()),borrowState_(&state){++borrowState_->leases;}
public:
    GbCollisionWorldLease()=default;
    GbCollisionWorldLease(const GbCollisionWorldLease& other)
        :owner_(other.owner_),source_(other.source_),world_(other.world_),descriptor_(other.descriptor_),
         generation_(other.generation_),issuingThread_(other.issuingThread_),borrowState_(other.borrowState_){
        if(borrowState_){if(borrowState_->thread!=std::this_thread::get_id())std::terminate();++borrowState_->leases;}}
    GbCollisionWorldLease(GbCollisionWorldLease&& other)noexcept
        :owner_(std::move(other.owner_)),source_(std::move(other.source_)),world_(other.world_),descriptor_(other.descriptor_),
         generation_(other.generation_),issuingThread_(other.issuingThread_),borrowState_(std::exchange(other.borrowState_,nullptr)){}
    GbCollisionWorldLease& operator=(const GbCollisionWorldLease&)=delete;
    GbCollisionWorldLease& operator=(GbCollisionWorldLease&&)=delete;
    ~GbCollisionWorldLease(){if(owner_&&issuingThread_!=std::this_thread::get_id())std::terminate();
        if(borrowState_){if(borrowState_->thread!=std::this_thread::get_id()||!borrowState_->leases)std::terminate();--borrowState_->leases;}}
    bool valid() const noexcept {
        return issuingThread_==std::this_thread::get_id()&&owner_&&source_&&world_&&generation_&&borrowState_&&borrowState_->leases&&
            world_->portalHeads==descriptor_.portalHeads&&world_->zoneCount==descriptor_.zoneCount&&
            world_->portalLinks==descriptor_.portalLinks&&world_->portalLinkCount==descriptor_.portalLinkCount&&
            world_->portals==descriptor_.portals&&world_->portalCount==descriptor_.portalCount;
    }
    const LiveZoneWorld50EN* world() const noexcept {return valid()?world_:nullptr;}
    const gb_cold_sources50LA::Owner* sourceIdentity() const noexcept {return valid()?source_.get():nullptr;}
    const gb_collision_world50GB::Owner* ownerIdentity() const noexcept {return valid()?owner_.get():nullptr;}
    std::uint64_t generation() const noexcept {return valid()?generation_:0;}
};
namespace navigation_storage_detail50GB {
// Reuse storage shapes, never MB's private Storage/Indices or MB admission.
using navigation_storage_detail50MB::Graph;
using navigation_storage_detail50MB::Reader;
using navigation_storage_detail50MB::Navigation;
using navigation_storage_detail50MB::Scratch;
using navigation_storage_detail50MB::Metadata;
struct Storage final {
    Navigation<213> map;Navigation<257> ai;
    Metadata<213> mapMetadata;Metadata<257> aiMetadata;
    std::array<std::uint32_t,213> mapFieldMask{};
    LiveZonePortalInput50EN authoredPortals[17]{};
    const gb_cold_sources50LA::Owner* const sourceIdentity;
    const std::thread::id ownerThread=std::this_thread::get_id();
    bool gatesReady=false;
    ~Storage(){if(ownerThread!=std::this_thread::get_id())std::terminate();}
private:
    explicit Storage(const gb_cold_sources50LA::Owner* source):sourceIdentity(source){}
    friend class ::gb_cold_sources50LA::Owner;
};
struct Indices final {
    std::uint8_t map[213]{},ai[257]{};
    const GbCollisionWorldLease world;
private:
    explicit Indices(const GbCollisionWorldLease& lease):world(lease){}
    friend class ::gb_navigation50GB::Owner;
};
}
class TrackNavigationView50GB;
class UndertrackTrackView50GB final {
    std::shared_ptr<const gb_cold_sources50LA::Owner> source_;
    std::shared_ptr<const navigation_storage_detail50GB::Storage> storage_;
    std::shared_ptr<const navigation_storage_detail50GB::Indices> indices_;
    NavigationRole50GB role_=NavigationRole50GB::Map;
    friend class TrackNavigationView50GB;
    UndertrackTrackView50GB(std::shared_ptr<const gb_cold_sources50LA::Owner> source,
        std::shared_ptr<const navigation_storage_detail50GB::Storage> storage,
        std::shared_ptr<const navigation_storage_detail50GB::Indices> indices,NavigationRole50GB role) noexcept
        :source_(std::move(source)),storage_(std::move(storage)),indices_(std::move(indices)),role_(role){}
public:
    UndertrackTrackView50GB()=default;
    bool valid() const noexcept {return source_&&storage_&&
        storage_->ownerThread==std::this_thread::get_id()&&storage_->sourceIdentity==source_.get()&&storage_->gatesReady;}
    bool runtimeValid() const noexcept {return valid()&&indices_&&indices_->world.valid()&&
        indices_->world.sourceIdentity()==source_.get()&&count()==capacity();}
    NavigationRole50GB role() const noexcept {return role_;}
    std::size_t capacity() const noexcept {return valid()?(role_==NavigationRole50GB::Map?213u:257u):0u;}
    std::uint32_t count() const noexcept {return valid()?(role_==NavigationRole50GB::Map?storage_->map.track.count:storage_->ai.track.count):0u;}
    const UndertrackNode50ET* nodes() const noexcept {return valid()?(role_==NavigationRole50GB::Map?storage_->map.track.nodes:storage_->ai.track.nodes):nullptr;}
};
class TrackNavigationView50GB final {
    std::shared_ptr<const gb_cold_sources50LA::Owner> source_;
    std::shared_ptr<const navigation_storage_detail50GB::Storage> storage_;
    std::shared_ptr<const navigation_storage_detail50GB::Indices> indices_;
    NavigationRole50GB role_=NavigationRole50GB::Map;
    friend class ::gb_cold_sources50LA::Owner;
    friend class ::gb_navigation50GB::Owner;
    friend class AiTrackMetadataView50GB;
    TrackNavigationView50GB(std::shared_ptr<const gb_cold_sources50LA::Owner> source,
        std::shared_ptr<const navigation_storage_detail50GB::Storage> storage,
        std::shared_ptr<const navigation_storage_detail50GB::Indices> indices,NavigationRole50GB role) noexcept
        :source_(std::move(source)),storage_(std::move(storage)),indices_(std::move(indices)),role_(role){}
public:
    TrackNavigationView50GB()=default;
    UndertrackTrackView50GB track() const noexcept {return {source_,storage_,indices_,role_};}
    bool gatesPrepared() const noexcept {return track().valid();}
    bool indicesPrepared() const noexcept {return track().runtimeValid();}
    bool runtimeValid() const noexcept {return indicesPrepared();}
    std::uint32_t count() const noexcept {return track().count();}
    std::size_t capacity() const noexcept {return track().capacity();}
    const TrackNavigationNode50EU* nodes() const noexcept {return gatesPrepared()?
        (role_==NavigationRole50GB::Map?storage_->map.nodes:storage_->ai.nodes):nullptr;}
    const std::uint8_t* nodeIndex78() const noexcept {return indicesPrepared()?
        (role_==NavigationRole50GB::Map?indices_->map:indices_->ai):nullptr;}
    NavigationRole50GB role() const noexcept {return role_;}
    const gb_cold_sources50LA::Owner* sourceIdentity() const noexcept {return gatesPrepared()?source_.get():nullptr;}
    std::uint64_t generation() const noexcept {return runtimeValid()?indices_->world.generation():0;}
    bool sameOwner(const TrackNavigationView50GB& other) const noexcept {
        return runtimeValid()&&other.runtimeValid()&&source_==other.source_&&storage_==other.storage_&&indices_==other.indices_;
    }
    bool sameWorld(const LiveZoneWorld50EN& world) const noexcept {
        return runtimeValid()&&indices_->world.world()==&world;
    }
};
class AiTrackMetadataView50GB final {
    TrackNavigationView50GB navigation_;
    friend class ::gb_cold_sources50LA::Owner;
    friend class ::gb_navigation50GB::Owner;
    explicit AiTrackMetadataView50GB(TrackNavigationView50GB n) noexcept:navigation_(std::move(n)){}
public:
    AiTrackMetadataView50GB()=default;
    bool bound() const noexcept {return navigation_.gatesPrepared()&&
        (navigation_.role_==NavigationRole50GB::Map?navigation_.storage_->mapMetadata.bound:navigation_.storage_->aiMetadata.bound);}
    bool runtimeValid() const noexcept {return bound()&&navigation_.runtimeValid();}
    NavigationRole50GB role() const noexcept {return navigation_.role();}
    std::uint32_t count() const noexcept {return bound()?navigation_.count():0;}
    std::size_t capacity() const noexcept {return bound()?navigation_.capacity():0;}
    const AiTrackNodeMetadata50IU* nodes() const noexcept {return bound()?
        (navigation_.role_==NavigationRole50GB::Map?navigation_.storage_->mapMetadata.nodes:navigation_.storage_->aiMetadata.nodes):nullptr;}
    float trackLength3C() const noexcept {return bound()?
        (navigation_.role_==NavigationRole50GB::Map?navigation_.storage_->mapMetadata.trackLength3C:navigation_.storage_->aiMetadata.trackLength3C):0.f;}
    const gb_cold_sources50LA::Owner* sourceIdentity() const noexcept {return bound()?navigation_.sourceIdentity():nullptr;}
    bool matches(const TrackNavigationView50GB& n) const noexcept {
        return bound()&&navigation_.sameOwner(n)&&navigation_.role()==n.role()&&count()==n.count();
    }
};
struct AiColdTrackBindingsView50GB {
    TrackNavigationView50GB mapFF0,aiFF4;
    AiTrackMetadataView50GB mapMetadata,aiMetadata;
};
inline bool bindAiColdTracks50IU(const TrackNavigationView50GB& map,const TrackNavigationView50GB& ai,
    const AiTrackMetadataView50GB& mapMetadata,const AiTrackMetadataView50GB& aiMetadata,
    const TrackNavigationProgress50EU& progress,AiColdTrackBindingsView50GB& out) noexcept {
    if(!map.sameOwner(ai)||map.role()!=NavigationRole50GB::Map||ai.role()!=NavigationRole50GB::Ai||
       !mapMetadata.matches(map)||!aiMetadata.matches(ai)||progress.reset.vehicleC80!=1||
       progress.vehicleC84!=0||progress.reset.vehicleC88!=1||progress.reset.vehicleC8C!=0)return false;
    out={map,ai,mapMetadata,aiMetadata};return true;
}
bool selectUndertrackReposition50ET(const UndertrackTrackView50GB&,
    const UndertrackProgress50ET&,const UndertrackInput50ET&,UndertrackSelection50ET&,UndertrackDiagnostic50ET&) noexcept;
bool undertrackBindings50EU(const TrackNavigationView50GB&,
    const std::uint32_t*,const std::uint32_t*,std::size_t,UndertrackTableBindings50ET&) noexcept;
bool bindUndertrackPlacement50ET(const UndertrackSelection50ET&,const TrackNavigationView50GB&,
    const std::uint32_t*,const std::uint32_t*,std::size_t,UndertrackPlacement50ET&,UndertrackDiagnostic50ET&) noexcept;
bool advancePlayerTrackNavigation50EU(const TrackNavigationView50GB&,const TrackNavigationView50GB&,
    const TrackNavigationInput50EU&,TrackNavigationProgress50EU&,TrackNavigationReceipt50EU&,TrackNavigationDiagnostic50EU&) noexcept;
bool advancePlayerTrackNavigationEffects50GH(const TrackNavigationView50GB&,const TrackNavigationView50GB&,
    const TrackNavigationInput50EU&,const TrackNavigationMapEffects50GH&,TrackNavigationProgress50EU&,
    TrackNavigationReceipt50EU&,TrackNavigationDiagnostic50EU&) noexcept;
}
