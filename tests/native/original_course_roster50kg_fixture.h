#pragma once
// Cold Course handoff only. The free-play field is a list of racer ids
// (courseFieldEntries); every row then loads its own pod's files through the
// one roster (common_roster50cr.h), stock or add-on alike.
// PORT STAND-IN (flagged, B-P04 field rule): the original's free-play opponents
// come from 1AE620's non-tournament branch (RNG, not ported). Until it is, the
// field is the eight retained host entries with the selected racer in slot 0:
// a racer already among them swaps with slot 0, any other racer (a supplemental
// stock pod or an add-on row, the same rule) takes the place of entry 0.
#include "native_racer_rig50hw_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include <array>
#include <filesystem>
#include <utility>
#include <string>

namespace original_course_roster50KG {
inline bool supportsPreparedRider(std::uint32_t racer60) noexcept { // find() reads the disc: not constexpr
    const auto* entry=rr::game::original_racer_selection50IJ::findByRacer60(racer60);
    return entry&&entry->humanJoinImplemented&&original_player_identity50IK::find(racer60);
}
// The free-play field entries (stand-in rule above): racer60 per slot, the
// selected racer first. Refuses a racer with no pod row (A5 table or registry).
inline bool courseFieldEntries(std::uint32_t racer60,
    std::array<std::uint32_t,rr::game::original_racer_selection50IJ::entries.size()>& out) noexcept {
    namespace selection=rr::game::original_racer_selection50IJ;
    const auto* selected=selection::findAnyByRacer60(racer60);
    if(!selected||!selected->humanJoinImplemented)return false;
    std::array<std::uint32_t,selection::entries.size()> next{};
    std::size_t at=next.size();
    for(std::size_t i=0;i<next.size();++i){next[i]=selection::entries[i].racer60;if(next[i]==racer60)at=i;}
    if(at<next.size())std::swap(next[0],next[at]);else next[0]=racer60;
    out=next;return true;
}
// Field selection describes the admitted identities only, in the order of
// courseFieldEntries. Each Rig subsequently authenticates and decodes its own
// actual RES, PHY and model resources. No generated JSON is read or written.
inline native_racer50HW::J directDiscRoster(std::uint32_t racer60) {
    namespace identities=original_player_identity50IK;
    using native_racer50HW::rig_detail::need;
    std::array<std::uint32_t,rr::game::original_racer_selection50IJ::entries.size()> ids{};
    need(courseFieldEntries(racer60,ids)&&identities::find(racer60),
        "Original disc selected racer has no native cold owner");
    native_racer50HW::J roster={{"schema","RR_ORIGINAL_DISC_ROSTER_V1"},{"player_index",0},
        {"racers",native_racer50HW::J::array()}};
    for(const auto id:ids){
        const auto* entry=rr::game::original_racer_selection50IJ::findAnyByRacer60(id);
        const auto* identity=identities::find(id);
        need(entry&&identity,"Original disc roster resource identity absent");
        roster["racers"].push_back(native_racer50HW::J{{"pod_id",std::string(entry->podId)},
            {"resource_sha256",identity->archiveSha256},{"phy_sha256",identity->phySha256}});
    }
    return roster;
}
// Authenticate the selected original's own cold inputs. Mutable rig, collision,
// GPU publication and race construction remain the actual target loaders' work
// after old-world destruction. Never retag an already loaded Rider19 owner.
inline bool selectPreparedRider(const std::filesystem::path& repository,
    const std::filesystem::path& rosterRoot,native_racer50HW::J& roster,
    std::uint32_t racer60,std::string& error) noexcept {
    namespace detail=native_racer50HW::rig_detail;
    namespace identities=original_player_identity50IK;
    namespace selection=rr::game::original_racer_selection50IJ;
    try {
        using detail::need;
        const auto* identity=identities::find(racer60);
        const auto* entry=selection::findByRacer60(racer60);
        need(supportsPreparedRider(racer60)&&identity&&entry&&entry->humanJoinImplemented,
            "Course racer has no admitted original cold race owner");
        need(roster.at("schema")=="RR_ORIGINAL_ROSTER_V1"&&roster.at("player_index")==0&&
            roster.at("racers").is_array()&&roster.at("racers").size()==selection::entries.size(),
            "Course selection requires the unchanged eight-racer original field");
        const native_racer50HW::J* selected=nullptr;
        for(std::size_t i=0;i<selection::entries.size();++i){
            const auto& original=selection::entries[i];
            const auto& row=roster.at("racers").at(i);
            need(row.at("pod_id").get<std::string>()==original.podId,
                "Course original field identity/order differs");
            if(original.racer60==racer60)selected=&row;
        }
        const bool supplemental=selected==nullptr;
        if(supplemental){
            need(roster.contains("additional_racers")&&roster.at("additional_racers").is_array()&&
                !roster.at("additional_racers").empty()&&roster.at("additional_racers").size()<=5,
                "Course supplemental original was not cold-prepared: use --additional-pods");
            // Preparation may retain several genuine supplemental originals.
            // Select by exact identity, never first row or an index-as-racer ID.
            for(const auto& row:roster.at("additional_racers")){
                if(row.at("pod_id").get<std::string>()!=identity->podId)continue;
                need(!selected,"Course supplemental selected original is duplicated");
                selected=&row;
            }
            need(selected,"Course selected supplemental original is absent from preparation");
        }
        const std::string podId(identity->podId),relative(entry->retainedPodJson);
        need(selected->at("pod_id")==podId&&selected->at("path")==relative&&
            selected->at("resource_sha256")==identity->archiveSha256&&
            selected->at("phy_sha256")==identity->phySha256,
            "Course selected original field resource/handling identity differs");
        // Canonical path comes from the retained source identity table, not JSON.
        const auto podPath=rosterRoot/std::filesystem::path(relative);
        const auto pod=detail::json(podPath);
        const auto imported=detail::json(podPath.parent_path()/"import.json");
        const auto lowerPod=detail::lower(podId);
        need(pod.at("pod_id")==podId&&pod.at("resource_sha256")==identity->archiveSha256&&
            pod.at("phy").at("source_sha256")==identity->phySha256&&
            detail::lower(pod.at("phy").at("source_member").get<std::string>())==
                "pods/"+lowerPod+"/"+lowerPod+".phy"&&
            imported.at("source_container_bytes")==identity->archiveBytes&&
            imported.at("source_container_sha256")==identity->archiveSha256,
            "Course selected original imported assets/PHY are absent or differ");
        const auto raw=native_racer50HW::f::readBytes(
            identities::archivePath(repository/runtime_asset_paths50LB::resolve("work/native/project/disc"),*identity));
        need(raw.size()==identity->archiveBytes&&
            pod_materials50ed_fixture::detail::sha256(raw)==identity->archiveSha256,
            "Course selected original RES is absent or differs");
        if(supplemental){
            // Commit the complete original row only after every selected source
            // check succeeds. A failure cannot partially mutate the old field.
            auto next=roster;next.at("racers").at(0)=*selected;roster.swap(next);
        }
        // Existing-field selections remain untouched for the caller's complete
        // selectedSlot swap. Supplemental selection already occupies slot0.
        error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="Course selected original cold roster preparation failed";}catch(...){}return false;}
}
} // namespace original_course_roster50KG
