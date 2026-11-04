#pragma once
#include "original_ui_main_entry50kg.h"
#include "ra_cold_navigation_lease50la.h"
#include "source_grid50ig_fixture.h"
#include <thread>
#include <array>
#include <exception>

namespace rr::game {
enum class OriginalTunnelTrackRole50LA : unsigned { mapB8,aiBC };
// Actual2A1160 Entity lifetime over the existing authenticated navigation
// projection. No second map/AI arrays and no claim of full2A1260 loader dispatch.
class OriginalTunnelTrack50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalRaceConstructor50LA;
    friend class OriginalRaceRacerManager50LA;
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<OriginalApplicationEntities50LA> application_;
    std::shared_ptr<const ra_cold_sources50LA::ColdNavigationLease> navigation_;
    OriginalWreckComponent50LA entity_;
    const OriginalTunnelTrackRole50LA role_;
    const TrackNavigation50EU* projection_=nullptr;
    // Exact constructor scalar writes, without inventing uninitialized38.
    std::uint32_t field34_=0,field3C_=0,field40_=0,field48_=0,field74_=0;
    std::array<std::uint32_t,10> fields4CThrough70_{};
    bool constructed_=false,closed_=false;
    OriginalTunnelTrack50LA(std::shared_ptr<OriginalApplicationEntities50LA> app,
        std::shared_ptr<const ra_cold_sources50LA::ColdNavigationLease> navigation,
        OriginalTunnelTrackRole50LA role,std::uint64_t session) noexcept
        :application_(std::move(app)),navigation_(std::move(navigation)),role_(role) {
        if(!application_||!application_->application10()||!navigation_||!session||
           application_->attachmentLock423F58())return;
        const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity_));
        if(!application_->entityAuthority_.reserveAdditional(1)||
           !application_->entityAuthority_.canEnroll(entity_,key))return;
        const auto r=initializeOriginalWreckComponent50LA(entity_,
            OriginalWreckComponentKind50LA::entity141300,session,key,true,
            application_->objectCount423F60()); //1A6F48/1A7078 A1=1.
        constructed_=r.status==OriginalWreckComponentStatus50LA::complete;
        if(constructed_&&!application_->entityAuthority_.enroll(entity_))std::terminate();
    }
    bool detached()const noexcept {
        return thread_==std::this_thread::get_id()&&constructed_&&!closed_&&application_&&application_->application10()&&
            entity_.kind==OriginalWreckComponentKind50LA::entity141300&&entity_.sessionGeneration&&
            entity_.objectKey==static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity_))&&
            entity_.reference00==1&&!entity_.previous10&&!entity_.next14&&!entity_.parent1C&&
            !entity_.firstChild18&&!entity_.mask20&&!entity_.descendants24&&
            entity_.category28==0xffu&&(entity_.flags29&15u)==3u;
    }
    bool bindRetainedProjection2A1260()noexcept {
        if(!detached()||projection_||!navigation_||
           !navigation_->noShortcutAllocation2A13D8(role_==OriginalTunnelTrackRole50LA::mapB8))return false;
        const auto& candidate=role_==OriginalTunnelTrackRole50LA::mapB8?
            navigation_->map():navigation_->ai();
        if(!candidate.track.count||candidate.track.count>maxUndertrackNodes50ET||
           !candidate.gatesPrepared)return false;
        projection_=&candidate;return true;
    }
    // Only a real owning Race/manager may use this retained map. This is
    // the bounded node0/ranks0..7 numeric2A2880 projection, not a claim that
    // the original loader2A1260 or its unrepresented fields completed.
    bool prepareNativeRestartGrid2A2880(
        const std::shared_ptr<OriginalApplicationEntities50LA>& application,
        std::uint64_t generation,const TrackNavigation50EU* actualMap,
        std::int32_t rank,GridCenterResult50AC& output,std::uint32_t& pc)const noexcept {
        pc=0x1aeb38;
        if(!detached()||application_!=application||entity_.sessionGeneration!=generation||
           role_!=OriginalTunnelTrackRole50LA::mapB8||!navigation_||!projection_||
           projection_!=actualMap||projection_!=&navigation_->map()||field74_!=0||
           application_->attachmentLock423F58()||!application_->entityAuthority_.canRetire(entity_))return false;
        pc=0x2a2880;
        if(rank<0||rank>7)return false;
        // Existing arithmetic retains its explicit rounding/nondegeneracy
        // bounds. No zero-vector fallback, node order or lateral rank is invented.
        return source_grid50IG::prepare(*projection_,static_cast<std::uint32_t>(rank),output);
    }
public:
    OriginalTunnelTrack50LA(const OriginalTunnelTrack50LA&)=delete;
    OriginalTunnelTrack50LA& operator=(const OriginalTunnelTrack50LA&)=delete;
    ~OriginalTunnelTrack50LA()noexcept {
        if(constructed_&&!closed_&&!release142590())std::terminate();
    }
    const TrackNavigation50EU* navigation()const noexcept{return closed_?nullptr:projection_;}
    // Bounded actual142590 route: no siblings/parent/children exist in this
    // detached owner. Lock refusal precedes104970 and preserves all ownership.
    bool release142590()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(closed_)return true;
        if(!detached()||application_->attachmentLock423F58())return false;
        if(!application_->entityAuthority_.retire(entity_))return false;
        --entity_.reference00; //104970 last reference, then table+8=2A11E0.
        //2A11E0 retires node/next-array30 then shortcut-array44 before1413E0.
        // Loaded adoption proved44 absent from actual authored NumShortcuts.
        // Native node/next arrays live together in the SAME immutable source owner;
        // release this borrow only. Other real clients retain their own leases.
        projection_=nullptr;navigation_.reset();
        --application_->objectCount423F60();
        closed_=true;return true;
    }
};
}

namespace original_main_entry50KG {
inline std::shared_ptr<rr::game::OriginalTunnelTrack50LA> Application::constructTunnelTrackCold2A1160(
    std::uint64_t session,rr::game::OriginalTunnelTrackRole50LA role,
    std::shared_ptr<const ra_cold_sources50LA::ColdNavigationLease> navigation)const noexcept {
    using T=rr::game::OriginalTunnelTrack50LA;
    const auto index=static_cast<unsigned>(role);
    if(index>=tunnelTrackColdOwners50LA_.size()||!session||!navigation||!entitiesOwner_||
       !entitiesOwner_->application10()||!tunnelTrackColdOwners50LA_[index].expired())return {};
    try {
        auto owner=std::shared_ptr<T>(new T(entitiesOwner_,std::move(navigation),role,session));
        if(!owner->detached())return {};
        tunnelTrackColdOwners50LA_[index]=owner;return owner;
    }catch(...){return {};}
}
inline bool Application::bindTunnelTrackColdProjection2A1260(rr::game::OriginalTunnelTrack50LA& owner)const noexcept {
    const auto index=static_cast<unsigned>(owner.role_);
    if(index>=tunnelTrackColdOwners50LA_.size()||
       tunnelTrackColdOwners50LA_[index].lock().get()!=&owner||owner.application_!=entitiesOwner_)return false;
    return owner.bindRetainedProjection2A1260();
}
}
