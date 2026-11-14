#pragma once
#include "../../src/game/vehicle_static_query50h.h"
#include "../../src/game/spring_records48n.h"
#include "../../src/game/vehicle_static_response50g.h"
#include "../../src/game/live_zone_membership50en.h"
#include <array>
#include <memory>
#include <stdexcept>
namespace interactive_setup50DU {struct Context;}
namespace track_runtime_storage50IG {struct Storage;}
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace native_spring_detector_policy50LA {
// Explicit native type2 detector policy, NOT a source358 Entity. The existing
// driver formerly supplied literal admission booleans. Both consumers now read
// one initialized, owned policy against the same registered query/spring arena.
class Owner final {
    friend struct ::interactive_setup50DU::Context;
    friend struct ::track_runtime_storage50IG::Storage;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using Query=rr::game::Type2StaticQuery49X;
    using Spring=rr::game::StaticSpringState50G;
    using Settings=rr::game::SpringGeneratedParameters48K;
    using Frame=rr::game::VehicleFrameState49W;
    using Bindings=rr::game::VehicleFrameBindings49W;
    using Membership=rr::game::LiveZoneMembership50EN;
    enum class PairPolicy:std::uint8_t {nativeType2NoPairs,unsupported};
    enum class ProviderPolicy:std::uint8_t {nativeAbsent,unsupported};
    struct Lanes {
        bool nativeEnabled=true;
        PairPolicy pairs=PairPolicy::nativeType2NoPairs;
        ProviderPolicy provider=ProviderPolicy::nativeAbsent;
    };
    struct Entry {Query* query;Spring* spring;Settings* settings;Membership* membership;std::uint32_t ref;};
    std::array<Lanes,2> lanes_{}; //Genuinely initialized native policy; banked, never guest60/68.
    std::array<Entry,2> entries_;
    struct SharedMembership {Membership* owner=nullptr;std::uint32_t ref=0;};
    std::array<std::array<SharedMembership,2>,8> sharedMemberships_{};
    const ::track_runtime_storage50IG::Storage* sharedWorld_=nullptr;
    std::size_t sharedCount_=0;bool sharedRetired_=false;
    Frame* frame_;Bindings* bindings_;
    bool live_=true,registered_=false;
    //Shared world (Stage B): each racer queries with its OWN 690/364 row, so
    //the membership is the caller's current row from the track storage
    //accessor (attached at that row's own ref), not one fixed member.
    bool perRowMemberships_=false;
    Owner(Frame& frame,Bindings& bindings,Query& main,Spring& spring,Settings& settings,
        Membership& mainMembership,std::uint32_t mainRef,Query& secondary,Spring& secondarySpring,
        Settings& secondarySettings,Membership& secondaryMembership,std::uint32_t secondaryRef,bool perRowMemberships=false)
        :entries_{{{&main,&spring,&settings,&mainMembership,mainRef},
                   {&secondary,&secondarySpring,&secondarySettings,&secondaryMembership,secondaryRef}}},
         frame_(&frame),bindings_(&bindings),perRowMemberships_(perRowMemberships) {}
    template<class Bank>void registerMutableOwners(Bank& bank){
        if(!live_||registered_)throw std::runtime_error("native detector policy registration repeated");
        bank.owner(lanes_);registered_=true;
    }
    bool revokeShared(const ::track_runtime_storage50IG::Storage& world)noexcept {
        if(sharedWorld_&&sharedWorld_!=&world)return false;
        sharedMemberships_={};sharedCount_=0;sharedWorld_=nullptr;sharedRetired_=true;return true;
    }
    void revoke()noexcept {sharedMemberships_={};sharedCount_=0;sharedWorld_=nullptr;sharedRetired_=true;live_=false;frame_=nullptr;bindings_=nullptr;for(auto& e:entries_)e={};}
    struct Lease {
        std::shared_ptr<Owner> owner;
        ~Lease(){if(owner)owner->revoke();}
    };
public:
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // Read-only current native policy. Passing another frame/query/spring or
    // another membership never manufactures an admission. This is not a phase
    // transition, original detector publication or nullable guest-field getter.
    bool admission(bool secondary,const Frame& frame,const Bindings& bindings,
        const Query& query,const Spring& spring,const Settings& settings,
        const Membership& membership,std::uint32_t ref,
        rr::game::MainStaticDetectorAdmission50H& out)const noexcept {
        const auto index=secondary?1u:0u;const auto& e=entries_[index];const auto& policy=lanes_[index];
        if(sharedWorld_||sharedRetired_||!live_||!registered_||frame_!=&frame||bindings_!=&bindings||e.query!=&query||
           e.spring!=&spring||e.settings!=&settings||!ref||
           (perRowMemberships_?(!membership.attached||membership.object!=ref):(e.membership!=&membership||e.ref!=ref))||
           !query.resource||!query.filter||
           policy.pairs!=PairPolicy::nativeType2NoPairs||policy.provider!=ProviderPolicy::nativeAbsent)return false;
        out={true,true,policy.nativeEnabled,false,false,settings.pointContacts48,
            query.resetRecordsBefore,true,2,ref,
            secondary?bindings.lifecycle.physical19C:bindings.lifecycle.main190};
        return true;
    }
};
}
