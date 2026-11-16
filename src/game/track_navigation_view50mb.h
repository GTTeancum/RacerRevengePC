#pragma once
#include "ai_cold_owners50iu.h"
#include <memory>
#include <utility>
#include <thread>

namespace mb_navigation50MB { class Owner; }
namespace mb_collision_world50MB { class Owner; }
namespace rr::game {
enum class NavigationRole50MB : std::uint8_t { Map,Ai };
// Issued only after the genuine MB Ground+TrackStorage aggregate is ready.
// The aggregate owns every backing span, cannot replace topology, and never
// owns navigation. This retain therefore has no ownership cycle.
class MbCollisionWorldLease {
    std::shared_ptr<const mb_collision_world50MB::Owner> owner_;
    const LiveZoneWorld50EN* world_=nullptr;
    LiveZoneWorld50EN descriptor_{};
    std::uint64_t generation_=0;
    std::thread::id issuingThread_{};
    friend class ::mb_collision_world50MB::Owner;
    MbCollisionWorldLease(std::shared_ptr<const mb_collision_world50MB::Owner> owner,
        const LiveZoneWorld50EN& world,std::uint64_t generation) noexcept
        :owner_(std::move(owner)),world_(&world),descriptor_(world),generation_(generation),
         issuingThread_(std::this_thread::get_id()){}
public:
    MbCollisionWorldLease()=default;
    bool valid() const noexcept {
        return issuingThread_==std::this_thread::get_id()&&owner_&&world_&&generation_&&
            world_->portalHeads==descriptor_.portalHeads&&
            world_->zoneCount==descriptor_.zoneCount&&world_->portalLinks==descriptor_.portalLinks&&
            world_->portalLinkCount==descriptor_.portalLinkCount&&world_->portals==descriptor_.portals&&
            world_->portalCount==descriptor_.portalCount;
    }
    const LiveZoneWorld50EN* world() const noexcept {return valid()?world_:nullptr;}
    const mb_collision_world50MB::Owner* ownerIdentity() const noexcept {return owner_.get();}
    std::uint64_t generation() const noexcept {return valid()?generation_:0;}
};
// ONE navigation store for every course read through the 50MB spline family
// (MB, GB, MC, TB, TC). Capacity, not a stock count: the largest stock spline
// (TC's AI spline, 293 nodes) fits. The live count is the course's own
// NumNodes (1..capacity); a spline above the capacity is refused by the reader
// (2A1260 node-index limit), never by identity. Portal capacity bounds the
// SCB's authored portals copied for the topology check (zones are <=256 by
// the live-world rule). This is the ONE place both capacities are defined;
// they never replace the legacy 256-node bound (maxUndertrackNodes50ET).
inline constexpr std::size_t courseNavigationCapacity50MB=293;
inline constexpr std::size_t courseNavigationPortalCapacity50MB=256;
namespace navigation_storage_detail50MB {
// Internal cold records do not issue views; only the authenticated Owner is a
// view issuer.
template<std::size_t N> struct Graph {
    UndertrackNode50ET nodes[N]{};std::uint32_t count=0;
};
template<std::size_t N> struct Reader {
    Graph<N> track{};std::uint8_t seen[N]{},fields[N]{},nextFields[N]{};
};
template<std::size_t N> struct Navigation {
    Graph<N> track{};TrackNavigationNode50EU nodes[N]{};
    std::uint8_t nodeIndex78[N]{};bool gatesPrepared=false,indicesPrepared=false;
};
template<std::size_t N> struct Scratch {
    Reader<N> reader{};Navigation<N> candidate{};std::uint32_t fieldMask[N]{};
};
template<std::size_t N> struct Metadata {
    AiTrackNodeMetadata50IU nodes[N]{};float trackLength3C=0.f;
    std::uint32_t count=0;bool bound=false;
};
struct Storage {
    Navigation<courseNavigationCapacity50MB> map;Navigation<courseNavigationCapacity50MB> ai;
    Metadata<courseNavigationCapacity50MB> mapMetadata;Metadata<courseNavigationCapacity50MB> aiMetadata;
    LiveZonePortalInput50EN authoredPortals[courseNavigationPortalCapacity50MB]{};
    std::uint32_t authoredZoneCount=0,authoredPortalCount=0; // live counts from the SCB
    bool gatesReady=false;
private:
    Storage()=default;friend class ::mb_navigation50MB::Owner;
};
struct Indices {
    std::uint8_t map[courseNavigationCapacity50MB]{},ai[courseNavigationCapacity50MB]{};
    MbCollisionWorldLease world;
private:
    Indices()=default;friend class ::mb_navigation50MB::Owner;
};
}
class TrackNavigationView50EU;
class UndertrackTrackView50ET {
    std::shared_ptr<const navigation_storage_detail50MB::Storage> storage_;
    std::shared_ptr<const navigation_storage_detail50MB::Indices> indices_;
    NavigationRole50MB role_=NavigationRole50MB::Map;
    friend class TrackNavigationView50EU;
    UndertrackTrackView50ET(std::shared_ptr<const navigation_storage_detail50MB::Storage> s,
        std::shared_ptr<const navigation_storage_detail50MB::Indices> i,NavigationRole50MB role) noexcept
        :storage_(std::move(s)),indices_(std::move(i)),role_(role){}
public:
    UndertrackTrackView50ET()=default;
    bool valid() const noexcept {return storage_&&storage_->gatesReady;}
    bool runtimeValid() const noexcept {return valid()&&indices_&&indices_->world.valid();}
    NavigationRole50MB role() const noexcept {return role_;}
    std::size_t capacity() const noexcept {return valid()?courseNavigationCapacity50MB:0u;}
    std::uint32_t count() const noexcept {return valid()?(role_==NavigationRole50MB::Map?storage_->map.track.count:storage_->ai.track.count):0u;}
    const UndertrackNode50ET* nodes() const noexcept {return valid()?(role_==NavigationRole50MB::Map?storage_->map.track.nodes:storage_->ai.track.nodes):nullptr;}
};
class TrackNavigationView50EU {
    std::shared_ptr<const navigation_storage_detail50MB::Storage> storage_;
    std::shared_ptr<const navigation_storage_detail50MB::Indices> indices_;
    NavigationRole50MB role_=NavigationRole50MB::Map;
    friend class ::mb_navigation50MB::Owner;
    friend class AiTrackMetadataView50IU;
    TrackNavigationView50EU(std::shared_ptr<const navigation_storage_detail50MB::Storage> s,
        std::shared_ptr<const navigation_storage_detail50MB::Indices> i,NavigationRole50MB role) noexcept
        :storage_(std::move(s)),indices_(std::move(i)),role_(role){}
public:
    TrackNavigationView50EU()=default;
    UndertrackTrackView50ET track() const noexcept {return {storage_,indices_,role_};}
    std::uint32_t count() const noexcept {return track().count();}
    std::size_t capacity() const noexcept {return track().capacity();}
    const TrackNavigationNode50EU* nodes() const noexcept {return gatesPrepared()?(role_==NavigationRole50MB::Map?storage_->map.nodes:storage_->ai.nodes):nullptr;}
    const std::uint8_t* nodeIndex78() const noexcept {return indicesPrepared()?(role_==NavigationRole50MB::Map?indices_->map:indices_->ai):nullptr;}
    bool gatesPrepared() const noexcept {return storage_&&storage_->gatesReady;}
    bool indicesPrepared() const noexcept {return gatesPrepared()&&indices_&&indices_->world.valid();}
    bool runtimeValid() const noexcept {return indicesPrepared()&&count()&&count()<=capacity();}
    NavigationRole50MB role() const noexcept {return role_;}
    std::uint64_t generation() const noexcept {return indicesPrepared()?indices_->world.generation():0;}
    bool sameOwner(const TrackNavigationView50EU& other) const noexcept {
        return runtimeValid()&&other.runtimeValid()&&storage_==other.storage_&&indices_==other.indices_;
    }
    bool sameWorld(const LiveZoneWorld50EN& world) const noexcept {
        return runtimeValid()&&indices_->world.world()==&world;
    }
};
class AiTrackMetadataView50IU {
    TrackNavigationView50EU navigation_;
    friend class ::mb_navigation50MB::Owner;
    explicit AiTrackMetadataView50IU(TrackNavigationView50EU n) noexcept: navigation_(std::move(n)){}
public:
    AiTrackMetadataView50IU()=default;
    bool bound() const noexcept {return navigation_.gatesPrepared()&&
        (navigation_.role_==NavigationRole50MB::Map?navigation_.storage_->mapMetadata.bound:navigation_.storage_->aiMetadata.bound);}
    bool runtimeValid() const noexcept {return bound()&&navigation_.runtimeValid();}
    NavigationRole50MB role() const noexcept {return navigation_.role();}
    std::uint32_t count() const noexcept {return bound()?navigation_.count():0;}
    std::size_t capacity() const noexcept {return bound()?navigation_.capacity():0;}
    const AiTrackNodeMetadata50IU* nodes() const noexcept {return bound()?
        (navigation_.role_==NavigationRole50MB::Map?navigation_.storage_->mapMetadata.nodes:navigation_.storage_->aiMetadata.nodes):nullptr;}
    float trackLength3C() const noexcept {return bound()?
        (navigation_.role_==NavigationRole50MB::Map?navigation_.storage_->mapMetadata.trackLength3C:navigation_.storage_->aiMetadata.trackLength3C):0.f;}
    bool matches(const TrackNavigationView50EU& n) const noexcept {
        return bound()&&navigation_.sameOwner(n)&&navigation_.role()==n.role()&&count()==n.count();
    }
};
struct AiColdTrackBindingsView50MB {
    TrackNavigationView50EU mapFF0,aiFF4;
    AiTrackMetadataView50IU mapMetadata,aiMetadata;
};
inline bool bindAiColdTracks50IU(const TrackNavigationView50EU& map,const TrackNavigationView50EU& ai,
    const AiTrackMetadataView50IU& mapMetadata,const AiTrackMetadataView50IU& aiMetadata,
    const TrackNavigationProgress50EU& progress,AiColdTrackBindingsView50MB& out) noexcept {
    if(!map.sameOwner(ai)||map.role()!=NavigationRole50MB::Map||ai.role()!=NavigationRole50MB::Ai||
       !mapMetadata.matches(map)||!aiMetadata.matches(ai)||progress.reset.vehicleC80!=1||
       progress.vehicleC84!=0||progress.reset.vehicleC88!=1||progress.reset.vehicleC8C!=0)return false;
    out={map,ai,mapMetadata,aiMetadata};return true;
}
// Runtime-only overloads. A gate-only view is accepted only by the cold grid.
bool selectUndertrackReposition50ET(const UndertrackTrackView50ET&,
    const UndertrackProgress50ET&,const UndertrackInput50ET&,UndertrackSelection50ET&,
    UndertrackDiagnostic50ET&) noexcept;
bool undertrackBindings50EU(const TrackNavigationView50EU&,
    const std::uint32_t*,const std::uint32_t*,std::size_t,UndertrackTableBindings50ET&) noexcept;
bool bindUndertrackPlacement50ET(const UndertrackSelection50ET&,const TrackNavigationView50EU&,
    const std::uint32_t*,const std::uint32_t*,std::size_t,UndertrackPlacement50ET&,
    UndertrackDiagnostic50ET&) noexcept;
bool advancePlayerTrackNavigation50EU(const TrackNavigationView50EU&,const TrackNavigationView50EU&,
    const TrackNavigationInput50EU&,TrackNavigationProgress50EU&,TrackNavigationReceipt50EU&,
    TrackNavigationDiagnostic50EU&) noexcept;
bool advancePlayerTrackNavigationEffects50GH(const TrackNavigationView50EU&,const TrackNavigationView50EU&,
    const TrackNavigationInput50EU&,const TrackNavigationMapEffects50GH&,TrackNavigationProgress50EU&,
    TrackNavigationReceipt50EU&,TrackNavigationDiagnostic50EU&) noexcept;
}
