#pragma once
// Common rewrite (R2, B-C09/B-C10): the ONE camera authoring load for every
// course. The TOB is the race Track's own TOB member, already read by the
// 1A9EE0 track load (common_track50CR::Load, R3); the CAM is the selected pod's
// own file (A5). The course's cut-camera row is derived from its own archive
// (course_cut_identity50ci.h), stock or registry alike, so the camera factory
// and the cut/intro authoring read only that course's data. No course is named.
#include "original_camera_session50kx.h"
#include "runtime_startup50fl_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "course_cut_identity50ci.h"
#include "common_track50cr.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace original_camera_session50KX {
// Cold native storage for the actual selected disc members. The caller borrows
// the PHY already authenticated by its retained original racer rig. No source
// decoder is run by recovery tooling: these calls belong to the native game.
struct AuthoredStorage {
    std::vector<std::uint8_t> cam,tob,secondCam;
    AuthoredInputs inputs{};
    void loadSecondRacer(const std::string& pod,const std::vector<std::uint8_t>& phy){
        using runtime_startup50fl::require;
        require(!cam.empty()&&secondCam.empty()&&!phy.empty()&&original_player_identity50IK::admittedPodId(pod),
            "second camera requires its selected racer rig");
        secondCam=runtime_startup50fl::readBytes(original_player_identity50IK::podCamera(
            std::filesystem::path("work/native/project/disc/PODS"),pod));
        require(!secondCam.empty(),"second selected camera CAM unavailable");
        inputs.secondCam=secondCam.data();inputs.secondCamBytes=secondCam.size();
        inputs.secondPhy=reinterpret_cast<const char*>(phy.data());inputs.secondPhyBytes=phy.size();
        inputs.secondPhyIdentity=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&phy));
    }
    // Throws (runtime_startup50fl::require) before publishing anything.
    void load(const common_track50CR::Track& track,const std::string& pod,
        const std::vector<std::uint8_t>& actualPhy) {
        using runtime_startup50fl::require;
        require(cam.empty()&&tob.empty(),"camera authoring requires fresh storage");
        require(track.loaded()&&track.identity.code.valid()&&track.archive.opened()&&track.archive.decoded(),
            "camera authoring requires the race's loaded track (1A9EE0)");
        require(!track.documents.tob.empty(),"camera authoring requires the track's TOB member");
        require(original_player_identity50IK::admittedPodId(pod)&&!actualPhy.empty(),
            "original camera requires actual selected pod authoring");
        auto selectedCam=runtime_startup50fl::readBytes(original_player_identity50IK::podCamera(
            std::filesystem::path("work/native/project/disc/PODS"),pod));
        require(!selectedCam.empty(),"original camera selected CAM unavailable");
        const auto code=track.identity.code.view();
        // The course's cut-camera row from its own archive (TOB [CSCameras],
        // tracks\<code>NN.vub, intro INIs), for every course. Absent data = no
        // row (logged); the camera step then refuses this course itself.
        {
            const std::string codeText(code);
            const std::string container="TRACKS/"+codeText+".RES";
            std::string cutError;
            if(!course_cut_identity50CI::derive(codeText.c_str(),*track.archive.decoded(),track.archive.sha256(),
                track.archive.bytes(),cutError,container.c_str()))
                std::cerr<<"course_cut_identity code="<<codeText<<" derived=0 ("<<cutError<<")\n";
        }
        // All throwing validation precedes publication. The actual selected
        // PHY remains borrowed from its existing retained original racer rig.
        auto selectedTob=track.documents.tob;
        cam=std::move(selectedCam);tob=std::move(selectedTob);
        inputs={cam.data(),cam.size(),reinterpret_cast<const char*>(actualPhy.data()),actualPhy.size(),
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&actualPhy)),tob.data(),tob.size(),
            track.identity.code};
    }
};
}
