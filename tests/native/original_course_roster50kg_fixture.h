#pragma once
// Cold Course handoff only. Existing-field selections preserve all eight
// original entries; the caller moves the complete selected entry to slot0.
// Supplemental originals replace only POD01, retaining seven genuine opponents.
// This is a bounded native field policy, not retail opponent construction.
#include "native_racer_rig50hw_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include <filesystem>
#include <string>

namespace original_course_roster50KG {
inline bool supportsPreparedRider(std::uint32_t racer60) noexcept { // find() reads the disc: not constexpr
    const auto* entry=rr::game::original_racer_selection50IJ::findByRacer60(racer60);
    return entry&&entry->humanJoinImplemented&&original_player_identity50IK::find(racer60);
}
// Field selection describes the admitted original identities only. Each Rig
// subsequently authenticates and decodes its own actual RES, PHY and model
// resources. No generated pod/import/roster JSON files are read or written.
inline native_racer50HW::J directDiscRoster(std::uint32_t racer60) {
    namespace selection=rr::game::original_racer_selection50IJ;
    namespace identities=original_player_identity50IK;
    using native_racer50HW::rig_detail::need;
    const auto* selected=selection::findByRacer60(racer60);
    need(selected&&selected->humanJoinImplemented&&identities::find(racer60),
        "Original disc selected racer has no native cold owner");
    native_racer50HW::J roster={{"schema","RR_ORIGINAL_DISC_ROSTER_V1"},{"player_index",0},
        {"racers",native_racer50HW::J::array()}};
    auto row=[](const selection::Entry& entry){const auto* identity=identities::find(entry.racer60);
        need(identity,"Original disc roster resource identity absent");
        return native_racer50HW::J{{"pod_id",std::string(entry.podId)},
            {"resource_sha256",identity->archiveSha256},{"phy_sha256",identity->phySha256}};};
    bool present=false;
    for(const auto& entry:selection::entries){roster["racers"].push_back(row(entry));present|=entry.racer60==racer60;}
    if(!present)roster["racers"][0]=row(*selection::findByRacer60(racer60));
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
// Preserve the original narrow entry point without requiring that POD19 be the
// only supplemental resource installed in an otherwise authentic preparation.
inline bool selectPod19(const std::filesystem::path& repository,
    const std::filesystem::path& rosterRoot,native_racer50HW::J& roster,
    std::string& error) noexcept {
    return selectPreparedRider(repository,rosterRoot,roster,19,error);
}
} // namespace original_course_roster50KG
