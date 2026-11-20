#pragma once
#include "original_ui_main_entry50kg.h"
#include "common_track50cr.h"
#include "source_grid50ig_fixture.h"
#include "../../src/assets/ai_authoring50iu.h"
#include <thread>
#include <array>
#include <exception>

namespace rr::game {
enum class OriginalTunnelTrackRole50LA : unsigned { mapB8,aiBC };
// Actual 2A1160 TunnelTrack Entity lifetime over the ONE navigation store of
// the race's common Track (common rewrite R4): race+B8 is the map role, +BC the
// AI role, for every course. The node arrays are the store's (sized from the
// spline's own NumNodes by readNavigation2A1260); this owner keeps the Track
// alive and borrows its view. No second map/AI arrays and no claim of the full
// 2A1260 loader dispatch beyond what the store holds.
class OriginalTunnelTrack50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalRaceConstructor50LA;
    friend class OriginalRaceRacerManager50LA;
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<OriginalApplicationEntities50LA> application_;
    std::shared_ptr<const ::common_track50CR::Track> track_;
    OriginalWreckComponent50LA entity_;
    const OriginalTunnelTrackRole50LA role_;
    ::common_track50CR::NavigationView projection_{};
    bool projected_=false;
    // Exact constructor scalar writes, without inventing uninitialized38.
    std::uint32_t field34_=0,field3C_=0,field40_=0,field48_=0,field74_=0;
    std::array<std::uint32_t,10> fields4CThrough70_{};
    bool constructed_=false,closed_=false;
    ::common_track50CR::NavigationRole navigationRole()const noexcept {
        return role_==OriginalTunnelTrackRole50LA::mapB8?::common_track50CR::NavigationRole::map:
            ::common_track50CR::NavigationRole::ai;
    }
    OriginalTunnelTrack50LA(std::shared_ptr<OriginalApplicationEntities50LA> app,
        std::shared_ptr<const ::common_track50CR::Track> track,
        OriginalTunnelTrackRole50LA role,std::uint64_t session) noexcept
        :application_(std::move(app)),track_(std::move(track)),role_(role) {
        if(!application_||!application_->application10()||!track_||!track_->roles.navigation||!session||
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
    // Exact constructor40=0 + optional 2A13D8 NumShortcuts lookup over this
    // role's own spline bytes. Array44 holds shortcut pairs, not per-node zone
    // indices; it is not represented, so a spline that authors shortcuts is
    // refused here (data rule, every course), never guessed.
    bool noShortcutAllocation2A13D8()const noexcept {
        namespace detail=rr::assets::ai_authoring_detail50IU;
        const auto& bytes=role_==OriginalTunnelTrackRole50LA::mapB8?
            track_->documents.mapSpline:track_->documents.aiSpline;
        rr::assets::AiAuthoringDiagnostic50IU diagnostic;
        bool trackSeen=false;
        auto visit=[&](detail::Slice section,detail::Slice key,detail::Slice value)noexcept {
            if(!detail::equal(section,"Track"))return true;
            trackSeen=true;
            if(!key.n||!detail::equal(key,"NumShortcuts"))return true;
            std::int32_t count=0;
            return detail::integer(value,count)&&count==0;
        };
        return detail::scan<navigationNodeLimit50ET+1>(reinterpret_cast<const char*>(bytes.data()),bytes.size(),
            maxUndertrackText50ET,0x2a13d8,visit,diagnostic)&&trackSeen;
    }
    bool bindRetainedProjection2A1260()noexcept {
        if(!detached()||projected_||!track_||!noShortcutAllocation2A13D8())return false;
        auto candidate=::common_track50CR::navigationView(*track_,navigationRole());
        if(candidate.count()<2||!candidate.gatesPrepared()||candidate.role()!=navigationRole())return false;
        projection_=std::move(candidate);projected_=true;return true;
    }
    // Only a real owning Race/manager may use this retained map. This is
    // the bounded node0/ranks0..7 numeric 2A2880 projection, not a claim that
    // the original loader 2A1260 or its unrepresented fields completed.
    bool prepareNativeRestartGrid2A2880(
        const std::shared_ptr<OriginalApplicationEntities50LA>& application,
        std::uint64_t generation,const ::common_track50CR::NavigationView& actualMap,
        std::int32_t rank,GridCenterResult50AC& output,std::uint32_t& pc)const noexcept {
        pc=0x1aeb38;
        if(!detached()||application_!=application||entity_.sessionGeneration!=generation||
           role_!=OriginalTunnelTrackRole50LA::mapB8||!track_||!projected_||
           actualMap.storage()!=projection_.storage()||actualMap.role()!=::common_track50CR::NavigationRole::map||
           field74_!=0||application_->attachmentLock423F58()||!application_->entityAuthority_.canRetire(entity_))return false;
        pc=0x2a2880;
        if(rank<0||rank>7)return false;
        // Existing arithmetic retains its explicit rounding/nondegeneracy
        // bounds. No zero-vector fallback, node order or lateral rank is invented.
        return source_grid50IG::prepare(projection_,static_cast<std::uint32_t>(rank),output);
    }
public:
    OriginalTunnelTrack50LA(const OriginalTunnelTrack50LA&)=delete;
    OriginalTunnelTrack50LA& operator=(const OriginalTunnelTrack50LA&)=delete;
    ~OriginalTunnelTrack50LA()noexcept {
        if(constructed_&&!closed_&&!release142590())std::terminate();
    }
    // The retained view (map B8 or AI BC), nullptr before 2A1260 binding or after release.
    const ::common_track50CR::NavigationView* navigation()const noexcept{return closed_||!projected_?nullptr:&projection_;}
    // The race's one Track this TunnelTrack keeps alive (race tuning 1A7184 reads its .tun).
    const std::shared_ptr<const ::common_track50CR::Track>& track()const noexcept{return track_;}
    // Bounded actual 142590 route: no siblings/parent/children exist in this
    // detached owner. Lock refusal precedes 104970 and preserves all ownership.
    bool release142590()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(closed_)return true;
        if(!detached()||application_->attachmentLock423F58())return false;
        if(!application_->entityAuthority_.retire(entity_))return false;
        --entity_.reference00; //104970 last reference, then table+8=2A11E0.
        //2A11E0 retires node/next-array30 then shortcut-array44 before1413E0.
        // Loaded adoption proved44 absent from actual authored NumShortcuts.
        // Native node/next arrays live together in the ONE navigation store of
        // the Track; release this borrow only. Other real clients keep their views.
        projection_={};projected_=false;track_.reset();
        --application_->objectCount423F60();
        closed_=true;return true;
    }
};
}

namespace original_main_entry50KG {
inline std::shared_ptr<rr::game::OriginalTunnelTrack50LA> Application::constructTunnelTrackCold2A1160(
    std::uint64_t session,rr::game::OriginalTunnelTrackRole50LA role,
    std::shared_ptr<const common_track50CR::Track> track)const noexcept {
    using T=rr::game::OriginalTunnelTrack50LA;
    const auto index=static_cast<unsigned>(role);
    if(index>=tunnelTrackColdOwners50LA_.size()||!session||!track||!track->roles.navigation||!entitiesOwner_||
       !entitiesOwner_->application10()||!tunnelTrackColdOwners50LA_[index].expired())return {};
    try {
        auto owner=std::shared_ptr<T>(new T(entitiesOwner_,std::move(track),role,session));
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
