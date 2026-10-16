#pragma once
#include "track_session_assets50ig_fixture.h"
#include "original_course_roster50kg_fixture.h"
#include "../../src/game/original_ui_course_race_domain50kg.h"
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
#include <string>
#include <utility>
#include <vector>

namespace original_course_targets50KX {
using Selection=rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection;
using Track=rr::platform::HostTrack50IM;
struct Target {
    Track track;
    const track_session_assets50IG::SessionAssets* assets;
    std::uint32_t course,planet;
};
inline const std::array<Target,5> targets{{
    {Track::TA,&track_session_assets50IG::retainedTA,0,0},
    {Track::SA,&track_session_assets50IG::retainedSA,11,4},
    {Track::MA,&track_session_assets50IG::retainedMA,4,5},
    {Track::GA,&track_session_assets50IG::retainedGA,8,1},
    // Authenticated UICourse Control11/Group1 slot11: Gamorr C, course10.
    // Capability only. Actual locked profile/UI state must independently admit
    // this destination. The genuine ALL8 world request or explicit diagnostic
    // acknowledgement, plus mode2, is checked below; no profile is unlocked.
    {Track::GC,&track_session_assets50IG::retainedGC,10,1}}};
inline const Target* target(const Selection& s) noexcept {
    if(!rr::game::nativeCourseRacerOwned50KG(s.rider)||s.subtype!=1||s.laps!=3||s.entrants!=8||
       (s.packed1125C&31u)!=s.laps||((s.packed1125C>>5)&3u)!=1||((s.packed1125C>>7)&15u)!=7||
       (s.packed1125C>>11)!=s.rider)return nullptr;
    for(const auto& t:targets)if(t.course==s.course&&t.planet==s.planet)return &t;
    return nullptr;
}
inline bool same(const Selection& a,const Selection& b) noexcept {
    return a.course==b.course&&a.planet==b.planet&&a.rider==b.rider&&a.subtype==b.subtype&&
        a.laps==b.laps&&a.entrants==b.entrants&&a.packed1125C==b.packed1125C;
}
// Cold request admission only. Retains each actual authenticated target archive
// before Course input starts. Native mesh/texture/AI/camera construction is NOT
// reported complete here; the unchanged target cold loader must perform it.
// A missing target disables only that destination, with no fallback identity.
class Catalog final {
    std::array<std::vector<std::uint8_t>,targets.size()> originals_;
    inline static constexpr auto riders=rr::game::nativeCourseRacers50KG;
    std::array<native_racer50HW::J,riders.size()> authenticatedRosters_;
    bool attempted_=false;
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
        if(!std::filesystem::exists(rosterPath)){
            std::cerr<<"Course race destinations unavailable: original roster absent: "<<rosterPath.string()<<'\n';
            return;
        }
        // Only absence is optional. Read/parse/metadata failures from a supplied
        // roster propagate; never turn malformed input into a readiness fallback.
        std::ifstream input(rosterPath);
        native_racer50HW::rig_detail::need(bool(input),"Cannot open Course roster metadata: "+rosterPath.string());
        // Unlike stream extraction, strict parse rejects a valid JSON prefix
        // followed by trailing malformed data or another document.
        const auto originals=native_racer50HW::J::parse(input);
        validateRosterMetadata(originals);
        for(std::size_t i=0;i<riders.size();++i){
            auto roster=originals;std::string error;
            if(original_course_roster50KG::selectPreparedRider(".",rosterRoot,roster,riders[i],error))
                authenticatedRosters_[i]=std::move(roster);
            else std::cerr<<"Course cold rider unavailable: "<<riders[i]<<" "<<error<<'\n';
        }
        for(std::size_t i=0;i<targets.size();++i){const auto& t=targets[i];
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
    const Target* admits(const Selection& s)const noexcept {
        const auto* selected=target(s);if(!attempted_||!selected)return nullptr;
        bool rosterPresent=false;
        for(std::size_t i=0;i<riders.size();++i)
            if(s.rider==riders[i]&&!authenticatedRosters_[i].is_null())rosterPresent=true;
        if(!rosterPresent)return nullptr;
        for(std::size_t i=0;i<targets.size();++i)
            if(selected==&targets[i]&&originals_[i].size()==selected->assets->archiveBytes)return selected;
        return nullptr;
    }
};
// Native application message, not a completed destination or source pointer.
// Publish only after takeCourseLaunch: its original queue9 and renderer drain
// have really completed. The outer owner consumes after old-world destruction,
// rechecking the SAME actual profile before selecting the next cold loader.
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
};
}
