#pragma once
namespace original_main_entry50KG { struct Application; }
#include "track_session_assets50ig_fixture.h"
#include "original_course_roster50kg_fixture.h"
#include "../../src/game/original_course_load_selection50kg.h"
#include "runtime_startup50fl_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "../../platform/linux/native/host_track_selection50im.h"
#include <array>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace original_course_targets50KX {
using Selection=rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection;
using Track=rr::platform::HostTrack50IM;
// Admission capability only, never proof of cold world/resource readiness.
// Keep this set synchronized with the direct original human runtime owners.
// Actual source profile visibility, rider/roster identity, mode2 and target
// archive authentication remain independent requirements below.
inline constexpr bool directHumanTrack(Track track) noexcept {
    return track==Track::RA||track==Track::GC||track==Track::TA||track==Track::GA||track==Track::MA||track==Track::SA||track==Track::MB||track==Track::SC||track==Track::TC||track==Track::RC||track==Track::TB||track==Track::GB||track==Track::MC;
}
inline constexpr bool directDiscTarget(Track track) noexcept {
    return directHumanTrack(track)||track==Track::RA;
}
struct Target {
    Track track;
    const track_session_assets50IG::SessionAssets* assets;
    std::uint32_t course,planet;
};
inline const std::array<Target,13> targets{{
    {Track::TA,&track_session_assets50IG::retainedTA,0,0},
    {Track::SA,&track_session_assets50IG::retainedSA,11,4},
    {Track::MA,&track_session_assets50IG::retainedMA,4,5},
    {Track::GA,&track_session_assets50IG::retainedGA,8,1},
    {Track::RA,&track_session_assets50IG::retainedRA,7,2},
    {Track::MB,&track_session_assets50IG::retainedMB,5,5},
    {Track::SC,&track_session_assets50IG::retainedSC,12,4},
    // Authenticated UICourse Control11/Group1 slot11: Gamorr C, course10.
    // Capability only. Actual locked profile/UI state must independently admit
    // this destination. The genuine ALL8 world request or explicit diagnostic
    // acknowledgement, plus mode2, is checked below; no profile is unlocked.
    {Track::GC,&track_session_assets50IG::retainedGC,10,1},
    {Track::TC,&track_session_assets50IG::retainedTC,2,0},
    {Track::RC,&track_session_assets50IG::retainedRC,3,2},
    {Track::TB,&track_session_assets50IG::retainedTB,1,0},
    {Track::GB,&track_session_assets50IG::retainedGB,9,1},
    {Track::MC,&track_session_assets50IG::retainedMC,6,5}}};
inline const Target* target(const Selection& s) noexcept {
    if(!rr::game::originalCourseSelectionEncoding50KG(s)||
       !rr::game::nativeSingleHumanCourseMode50KG(s))return nullptr;
    for(const auto& t:targets)if(t.course==s.course&&t.planet==s.planet&&
        (s.laps==3||directHumanTrack(t.track)))return &t;
    return nullptr;
}
inline bool same(const Selection& a,const Selection& b) noexcept {
    return a.course==b.course&&a.planet==b.planet&&a.rider==b.rider&&a.subtype==b.subtype&&
        a.laps==b.laps&&a.entrants==b.entrants&&a.packed1125C==b.packed1125C&&
        a.mode34==b.mode34;
}
// Cold request admission only. Retains each actual authenticated target archive
// before Course input starts. Native mesh/texture/AI/camera construction is NOT
// reported complete here; the unchanged target cold loader must perform it.
// A missing target disables only that destination, with no fallback identity.
class Catalog final {
    std::array<std::vector<std::uint8_t>,targets.size()> originals_;
    inline static constexpr auto riders=[](){
        std::array<std::uint32_t,original_player_identity50IK::originals.size()> ids{};
        for(std::size_t i=0;i<ids.size();++i)ids[i]=original_player_identity50IK::originals[i].racer60;
        return ids;
    }();
    std::array<native_racer50HW::J,riders.size()> authenticatedRosters_;
    // Disc-only admission retains each verified encoded original once. The
    // real post-handoff Rig/model loaders still create and validate their own
    // actual resource owners. This is source availability, never race readiness.
    std::array<std::vector<std::uint8_t>,original_player_identity50IK::originals.size()> originalPods_;
    bool attempted_=false,directDisc_=false;
    void prepareDirectDiscRosters() {
        namespace identities=original_player_identity50IK;
        namespace detail=native_racer50HW::rig_detail;
        const auto disc=runtime_asset_paths50LB::resolve("work/native/project/disc");
        for(std::size_t i=0;i<identities::originals.size();++i){const auto& identity=identities::originals[i];
            try {
                auto raw=runtime_startup50fl::readBytes(identities::archivePath(disc,identity));
                detail::need(raw.size()==identity.archiveBytes&&
                    pod_materials50ed_fixture::detail::sha256(raw)==identity.archiveSha256,
                    "Original disc roster archive identity differs");
                rr::assets::ResArchive48C archive;rr::assets::DecodeError48C diagnostic;
                detail::need(rr::assets::decodeResource48C(raw,archive,diagnostic),
                    "Original disc roster archive decode rejected: "+diagnostic.code);
                const auto lowerPod=detail::lower(identity.podId);
                const auto wanted="pods/"+lowerPod+"/"+lowerPod+".phy";
                const rr::assets::ResourceEntry48C* member=nullptr;
                for(const auto& entry:archive.entries)if(detail::lower(entry.name)==wanted){
                    detail::need(!member,"Original disc roster PHY is ambiguous");member=&entry;}
                detail::need(member,"Original disc roster PHY is absent");
                std::vector<std::uint8_t> phy;
                detail::need(rr::assets::extractResMember48C(archive,member->name,phy,diagnostic)&&
                    pod_materials50ed_fixture::detail::sha256(phy)==identity.phySha256,
                    "Original disc roster PHY identity differs");
                originalPods_[i]=std::move(raw);
            }catch(const std::exception& error){
                std::cerr<<"Course direct original pod unavailable: "<<identity.podId<<" "<<error.what()<<'\n';
            }
        }
        for(std::size_t i=0;i<riders.size();++i){
            auto roster=original_course_roster50KG::directDiscRoster(riders[i]);bool complete=true;
            for(const auto& row:roster.at("racers")){
                bool verified=false;
                for(std::size_t pod=0;pod<identities::originals.size();++pod){const auto& identity=identities::originals[pod];
                    if(row.at("pod_id")==identity.podId&&row.at("resource_sha256")==identity.archiveSha256&&
                       row.at("phy_sha256")==identity.phySha256&&originalPods_[pod].size()==identity.archiveBytes)
                        verified=true;
                }
                complete=complete&&verified;
            }
            if(complete)authenticatedRosters_[i]=std::move(roster);
            else std::cerr<<"Course direct original field unavailable for rider: "<<riders[i]<<'\n';
        }
    }
    // A supplied catalog must describe the exact original field. Validate its
    // admission-bearing structure/identities before the per-rider asset checks:
    // malformed metadata is fatal, while an absent optional pod stays unavailable.
    // This metadata check creates no authenticated resource or race readiness.
    static void validateRosterMetadata(const native_racer50HW::J& roster) {
        namespace selection=rr::game::original_racer_selection50IJ;
        namespace identities=original_player_identity50IK;
        using native_racer50HW::rig_detail::need;
        need(roster.is_object()&&roster.at("schema")=="RR_ORIGINAL_ROSTER_V1"&&
            roster.at("player_index").is_number_integer()&&roster.at("player_index")==0&&
            roster.at("racers").is_array()&&roster.at("racers").size()==selection::entries.size(),
            "Course roster metadata requires the unchanged eight-racer original field");
        const auto validateRow=[](const native_racer50HW::J& row,const selection::Entry& entry){
            const auto* identity=identities::find(entry.racer60);
            need(identity&&row.is_object()&&row.at("pod_id")==std::string(entry.podId)&&
                row.at("path")==std::string(entry.retainedPodJson)&&
                row.at("resource_sha256")==identity->archiveSha256&&
                row.at("phy_sha256")==identity->phySha256,
                "Course roster metadata original resource/handling identity differs");
        };
        for(std::size_t i=0;i<selection::entries.size();++i)
            validateRow(roster.at("racers").at(i),selection::entries[i]);
        if(!roster.contains("additional_racers"))return;
        const auto& additional=roster.at("additional_racers");
        need(additional.is_array()&&additional.size()<=5,
            "Course roster supplemental metadata extent differs");
        std::array<std::uint32_t,5> seen{};std::size_t count=0;
        for(const auto& row:additional){
            need(row.is_object(),"Course roster supplemental metadata row differs");
            const auto podId=row.at("pod_id").get<std::string>();
            const auto* entry=selection::findByPodId(podId);
            need(entry&&original_course_roster50KG::supportsPreparedRider(entry->racer60),
                "Course roster supplemental original identity is unsupported");
            for(const auto& original:selection::entries)
                need(entry->racer60!=original.racer60,"Course roster repeats an original field racer as supplemental");
            for(std::size_t i=0;i<count;++i)
                need(seen[i]!=entry->racer60,"Course roster supplemental original is duplicated");
            validateRow(row,*entry);seen[count++]=entry->racer60;
        }
    }
public:
    void prepare(const rr::platform::HostTrackAdmission50IM& admission) {
        runtime_startup50fl::require(!attempted_,"Course target catalog is cold one-shot");attempted_=true;
        const auto rosterRoot=runtime_asset_paths50LB::resolve("evidence/originalRoster50HW/raw-imports");
        const auto rosterPath=rosterRoot/"roster.json";
        if(!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty()||
           !std::filesystem::exists(rosterPath)){
            // Explicit original-disc mode is independent of any leftover
            // generated catalog. Other tracks keep their prepared-data gate.
            directDisc_=true;prepareDirectDiscRosters();
        }else{
            // A supplied legacy roster must still validate. A malformed catalog
            // cannot silently fall through to another readiness mode.
            std::ifstream input(rosterPath);
            native_racer50HW::rig_detail::need(bool(input),"Cannot open Course roster metadata: "+rosterPath.string());
            // Strict parse rejects a JSON prefix followed by trailing content.
            const auto originals=native_racer50HW::J::parse(input);
            validateRosterMetadata(originals);
            for(std::size_t i=0;i<riders.size();++i){
                auto roster=originals;std::string error;
                if(original_course_roster50KG::selectPreparedRider(".",rosterRoot,roster,riders[i],error))
                    authenticatedRosters_[i]=std::move(roster);
                else std::cerr<<"Course cold rider unavailable: "<<riders[i]<<" "<<error<<'\n';
            }
        }
        for(std::size_t i=0;i<targets.size();++i){const auto& t=targets[i];
            if(directDisc_&&!directDiscTarget(t.track))continue;
            // GA has no admitted legacy path. Every direct human target also
            // requires explicit original-disc ownership; an absent generated
            // roster alone must not opt TA/MA/SA into their direct loaders.
            if((t.track==Track::GA||(directDisc_&&directHumanTrack(t.track)))&&
               runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty())continue;
            if(!admission.gcMode2||!rr::platform::hostTrackAdmitted50IM(t.track,admission))continue;
            try {
                auto bytes=runtime_startup50fl::readBytes(t.assets->archive);
                if(bytes.size()!=t.assets->archiveBytes||
                   pod_materials50ed_fixture::detail::sha256(bytes)!=t.assets->archiveSha256){
                    std::cerr<<"Course cold target unavailable: original "<<t.assets->world->code<<" archive identity differs\n";
                    continue;
                }
                originals_[i]=std::move(bytes);
            }catch(const std::exception& e){
                std::cerr<<"Course cold target unavailable: "<<t.assets->world->code<<" "<<e.what()<<'\n';
            }
        }
    }
    rr::game::OriginalCourseLoadAvailability50KG availability(const Selection& s)const noexcept {
        using A=rr::game::OriginalCourseLoadAvailability50KG;
        if(!rr::game::originalCourseSelectionEncoding50KG(s))return A::invalidSelection;
        if(!rr::game::nativeSingleHumanCourseMode50KG(s))return A::unsupportedNativeMode;
        if(!attempted_)return A::hostNotReady;
        const Target* selected=nullptr;
        for(const auto& t:targets)if(t.course==s.course&&t.planet==s.planet){selected=&t;break;}
        if(!selected)return A::courseResourcesUnavailable;
        if((s.laps!=3&&!directHumanTrack(selected->track))||
           (directDisc_&&!directDiscTarget(selected->track)))return A::unsupportedNativeMode;
        bool rosterPresent=false;
        for(std::size_t i=0;i<riders.size();++i)
            if(s.rider==riders[i]&&!authenticatedRosters_[i].is_null())rosterPresent=true;
        if(!rosterPresent)return A::riderResourcesUnavailable;
        for(std::size_t i=0;i<targets.size();++i)
            if(selected==&targets[i]&&originals_[i].size()==selected->assets->archiveBytes)return A::ready;
        return A::courseResourcesUnavailable;
    }
    const Target* admits(const Selection& s)const noexcept {
        return availability(s)==rr::game::OriginalCourseLoadAvailability50KG::ready?target(s):nullptr;
    }
};
// Native application message, not a completed destination or source pointer.
// Publish only after takeCourseLaunch: its original queue9 and renderer drain
// have really completed. The outer owner consumes after old-world destruction,
// rechecking the SAME actual profile before selecting the next cold loader.
// Native retained result of a real Course request consumption. This carries
// no Entity, parent, scheduler or constructor completion. Only ColdLoadRequest
// can mint it, after validating the actual outgoing request and profile.
class ConsumedLaunch final {
    friend class ColdLoadRequest;
    friend struct ::original_main_entry50KG::Application;
    bool raceConstructorClaimed_=false;
    std::shared_ptr<const rr::platform::OriginalUiRiderProfileOwner50KG> profile_;
    Selection selection_{};
    const Target* target_=nullptr;
    std::uint64_t nextGeneration_=0;
    ConsumedLaunch()=default;
public:
    ConsumedLaunch(const ConsumedLaunch&)=delete;
    ConsumedLaunch& operator=(const ConsumedLaunch&)=delete;
    bool matches(const rr::platform::OriginalUiRiderProfileOwner50KG& actual,
        std::uint64_t generation,Track loadedTrack,std::uint32_t loadedRacer)const noexcept {
        Selection current;
        return profile_.get()==&actual&&target_&&generation==nextGeneration_&&
            target_->track==loadedTrack&&selection_.rider==loadedRacer&&
            actual.courseRaceSnapshot(current)&&same(selection_,current)&&target(current)==target_;
    }
    // Restart Race re-runs the SAME consumed race constructor. Mint an
    // unclaimed launch for the next session only while the actual profile
    // still holds this exact selection; never for a changed selection.
    bool renewForRestart(std::uint64_t destroyedGeneration,std::unique_ptr<ConsumedLaunch>& result)const noexcept {
        Selection current;
        if(result||!profile_||!target_||destroyedGeneration!=nextGeneration_||
           destroyedGeneration==std::numeric_limits<std::uint64_t>::max()||
           !profile_->courseRaceSnapshot(current)||!same(selection_,current)||target(current)!=target_)return false;
        std::unique_ptr<ConsumedLaunch> renewed;
        try {renewed.reset(new ConsumedLaunch);}catch(...){return false;}
        renewed->profile_=profile_;renewed->selection_=selection_;renewed->target_=target_;
        renewed->nextGeneration_=destroyedGeneration+1;
        result=std::move(renewed);return true;
    }
    bool targetsTrack(Track track)const noexcept{return target_&&target_->track==track;}
};
class ColdLoadRequest final {
    Selection selection_{};
    const Target* target_=nullptr;
    std::uint64_t outgoingGeneration_=0;
public:
    bool pending()const noexcept{return target_!=nullptr;}
    bool publish(const Catalog& catalog,const Selection& selection,std::uint64_t generation)noexcept {
        if(pending()||!generation)return false;
        const auto* selected=catalog.admits(selection);if(!selected)return false;
        selection_=selection;target_=selected;outgoingGeneration_=generation;return true;
    }
    bool consume(const rr::platform::OriginalUiRiderProfileOwner50KG& actual,
        std::uint64_t destroyedGeneration,Track& nextTrack,std::uint32_t& nextRacer)noexcept {
        Selection current;
        if(!pending()||destroyedGeneration!=outgoingGeneration_||!actual.courseRaceSnapshot(current)||
           !same(selection_,current)||target(current)!=target_)return false;
        nextTrack=target_->track;nextRacer=current.rider;
        target_=nullptr;outgoingGeneration_=0;return true;
    }
    // Compatibility: the existing consume above retains its exact behavior.
    // Allocate before consuming so native allocation failure cannot lose an
    // authenticated request. Publication below is infallible after consume.
    bool consumeForNextSession(
        std::shared_ptr<const rr::platform::OriginalUiRiderProfileOwner50KG> actual,
        std::uint64_t destroyedGeneration,Track& nextTrack,std::uint32_t& nextRacer,
        std::unique_ptr<ConsumedLaunch>& result)noexcept {
        if(!actual||result||destroyedGeneration==std::numeric_limits<std::uint64_t>::max())return false;
        std::unique_ptr<ConsumedLaunch> retained;
        try {retained.reset(new ConsumedLaunch);}catch(...){return false;}
        const auto selected=selection_;const auto* destination=target_;
        if(!consume(*actual,destroyedGeneration,nextTrack,nextRacer))return false;
        retained->profile_=std::move(actual);retained->selection_=selected;
        retained->target_=destination;retained->nextGeneration_=destroyedGeneration+1;
        result=std::move(retained);return true;
    }
};
}
