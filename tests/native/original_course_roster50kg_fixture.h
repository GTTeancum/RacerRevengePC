#pragma once
// Cold Course handoff only. Keep the seven existing original opponents and
// replace camera slot0 with the exact selected POD19. This is a native field
// policy, not the unimplemented retail opponent selection constructor.
#include "native_racer_rig50hw_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include <filesystem>
#include <string>

namespace original_course_roster50KG {
inline bool selectPod19(const std::filesystem::path& repository,
    const std::filesystem::path& rosterRoot,native_racer50HW::J& roster,
    std::string& error) noexcept {
    namespace detail=native_racer50HW::rig_detail;
    namespace identities=original_player_identity50IK;
    try {
        using detail::need;
        const auto* identity=identities::find(19);
        need(identity&&roster.at("schema")=="RR_ORIGINAL_ROSTER_V1"&&
            roster.at("player_index")==0&&roster.at("racers").is_array()&&
            roster.at("racers").size()==8,"Course POD19 requires the unchanged eight-racer source field");
        const auto& defaults=rr::game::original_racer_selection50IJ::entries;
        for(std::size_t i=0;i<defaults.size();++i)
            need(roster.at("racers").at(i).at("pod_id").get<std::string>()==defaults[i].podId,
                "Course POD19 default field identity/order differs");
        need(roster.contains("additional_racers")&&roster.at("additional_racers").is_array()&&
            roster.at("additional_racers").size()==1,
            "Course POD19 was not cold-prepared: use prepare_original_roster.py --additional-pods POD19");
        const auto& selected=roster.at("additional_racers").at(0);
        need(selected.at("pod_id")==identity->podId&&selected.at("path")=="pod19/pod.json"&&
            selected.at("resource_sha256")==identity->archiveSha256&&
            selected.at("phy_sha256")==identity->phySha256,
            "Course POD19 supplemental resource/handling identity differs");
        const auto directory=rosterRoot/"pod19";
        const auto pod=detail::json(directory/"pod.json");
        const auto imported=detail::json(directory/"import.json");
        need(pod.at("pod_id")==identity->podId&&
            pod.at("resource_sha256")==identity->archiveSha256&&
            pod.at("phy").at("source_sha256")==identity->phySha256&&
            detail::lower(pod.at("phy").at("source_member").get<std::string>())=="pods/pod19/pod19.phy"&&
            imported.at("source_container_bytes")==identity->archiveBytes&&
            imported.at("source_container_sha256")==identity->archiveSha256,
            "Course POD19 own imported assets/PHY are absent or differ");
        const auto raw=native_racer50HW::f::readBytes(
            identities::archivePath(repository/"work/native/project/disc",*identity));
        need(raw.size()==identity->archiveBytes&&
            pod_materials50ed_fixture::detail::sha256(raw)==identity->archiveSha256,
            "Course POD19 own original RES is absent or differs");
        auto next=roster;
        next.at("racers").at(0)=selected;
        roster.swap(next);error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="Course POD19 cold roster selection failed";}catch(...){}return false;}
}
} // namespace original_course_roster50KG
