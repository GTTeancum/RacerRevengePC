#pragma once
#include "original_camera_session50kx.h"
#include "runtime_startup50fl_fixture.h"
#include "track_session_assets50ig_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "course_archive_receipt50ci.h"
#include "course_cut_identity50ci.h"
#include "../../platform/linux/native/host_track_selection50im.h"
#include <iostream>
#include <string>
#include "../../src/assets/import_decode48c.h"
#include "rc_cold_sources50la.h"
#include "tb_cold_sources50la.h"
#include "gb_cold_sources50la.h"
#include <memory>
#include <utility>

namespace original_camera_session50KX {
// Cold native storage for the actual selected disc members. The caller borrows
// the PHY already authenticated by its retained original racer rig. No source
// decoder is run by recovery tooling: these calls belong to the native game.
struct AuthoredStorage {
private:
    // Declared first so source-backed input storage outlives other members.
    // Owner access and final release enforce the issuing thread.
    std::shared_ptr<const rc_cold_sources50LA::Owner> rcSource_;
    std::shared_ptr<const tb_cold_sources50LA::Owner> tbSource_;
    std::shared_ptr<const gb_cold_sources50LA::Owner> gbSource_;
public:

    std::vector<std::uint8_t> cam,tob;
    AuthoredInputs inputs{};
    // RC Application path: borrow the already authenticated complete TOB.
    // The legacy load below remains the data-only cold checker path for RC.
    void loadRc(const track_session_assets50IG::SessionAssets& track,
        const std::string& pod,const std::vector<std::uint8_t>& actualPhy,
        std::shared_ptr<const rc_cold_sources50LA::Owner> source) {
        using runtime_startup50fl::require;
        require(cam.empty()&&tob.empty()&&!rcSource_&&!tbSource_&&!gbSource_,
            "RC camera authoring requires fresh storage");
        require(track.world&&track_session_assets50IG::codeEquals(track.world->code,"RC")&&
            track.archiveBytes==4655141u&&track.archiveSha256&&std::string(track.archiveSha256)==
                "ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b"&&
            source&&source->ready(),
            "RC camera requires the authenticated retained RC source");
        require(original_player_identity50IK::admittedPodId(pod)&&!actualPhy.empty(),
            "original camera requires actual selected pod authoring");
        const auto& sourceTob=source->originalTob();
        require(sourceTob.size()==124930u&&pod_materials50ed_fixture::detail::sha256(sourceTob)==
            "71b1f9609c0a6ce84d71ff69355d9cfcbc8771eb7ddb8876473b4fddd6e23e6d",
            "RC camera retained TOB identity rejected");
        auto selectedCam=runtime_startup50fl::readBytes(
            original_player_identity50IK::podCamera(std::filesystem::path("work/native/project/disc/PODS"),pod));
        require(!selectedCam.empty(),"original camera selected CAM unavailable");
        // All throwing validation precedes publication. The actual selected
        // PHY remains borrowed from its existing retained original racer rig.
        cam=std::move(selectedCam);
        rcSource_=std::move(source);
        inputs={cam.data(),cam.size(),reinterpret_cast<const char*>(actualPhy.data()),actualPhy.size(),
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&actualPhy)),
            sourceTob.data(),sourceTob.size(),rr::game::OriginalCameraStartCourse::rc};
    }
    // TB Application path: borrow the already authenticated complete TOB.
    // Complete TB authoring remains borrowed from its one retained compact source.
    void loadTb(const track_session_assets50IG::SessionAssets& track,
        const std::string& pod,const std::vector<std::uint8_t>& actualPhy,
        std::shared_ptr<const tb_cold_sources50LA::Owner> source) {
        using runtime_startup50fl::require;
        require(cam.empty()&&tob.empty()&&!tbSource_&&!rcSource_&&!gbSource_,
            "TB camera authoring requires fresh storage");
        require(track.world&&track_session_assets50IG::codeEquals(track.world->code,"TB")&&
            track.archiveBytes==3749557u&&track.archiveSha256&&std::string(track.archiveSha256)==
                "1188206d7b1f64910c119192829fc357b9d4a9f021d64e8a2b7561f2c113a09b"&&
            source&&source->ready(),
            "TB camera requires the authenticated retained TB source");
        require(original_player_identity50IK::admittedPodId(pod)&&!actualPhy.empty(),
            "original camera requires actual selected pod authoring");
        const auto& sourceTob=source->originalTob();
        require(sourceTob.size()==106139u&&pod_materials50ed_fixture::detail::sha256(sourceTob)==
            "12c908874736ea91dcd6d3f8af97413a0cfe3bcd87cfce0798e8ff86fe5fb0bc",
            "TB camera retained TOB identity rejected");
        auto selectedCam=runtime_startup50fl::readBytes(
            original_player_identity50IK::podCamera(std::filesystem::path("work/native/project/disc/PODS"),pod));
        require(!selectedCam.empty(),"original camera selected CAM unavailable");
        // All throwing validation precedes publication. The actual selected
        // PHY remains borrowed from its existing retained original racer rig.
        cam=std::move(selectedCam);
        tbSource_=std::move(source);
        inputs={cam.data(),cam.size(),reinterpret_cast<const char*>(actualPhy.data()),actualPhy.size(),
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&actualPhy)),
            sourceTob.data(),sourceTob.size(),rr::game::OriginalCameraStartCourse::tb};
    }
    // GB Application path: borrow the already authenticated complete TOB.
    // Complete GB authoring remains borrowed from its one retained compact source.
    void loadGb(const track_session_assets50IG::SessionAssets& track,
        const std::string& pod,const std::vector<std::uint8_t>& actualPhy,
        std::shared_ptr<const gb_cold_sources50LA::Owner> source) {
        using runtime_startup50fl::require;
        require(cam.empty()&&tob.empty()&&!gbSource_&&!rcSource_&&!tbSource_,
            "GB camera authoring requires fresh storage");
        // The GB slot's session row and its retained cold source (which
        // admitted the archive actually loaded; stock identity is a receipt
        // there). No archive pin here.
        require(track.world&&track_session_assets50IG::codeEquals(track.world->code,"GB")&&
            source&&source->ready(),
            "GB camera requires the authenticated retained GB source");
        require(original_player_identity50IK::admittedPodId(pod)&&!actualPhy.empty(),
            "original camera requires actual selected pod authoring");
        const auto& sourceTob=source->originalTob();
        // Stock TOB identity is a receipt (course_archive_receipt50ci.h).
        require(!sourceTob.empty(),"GB camera retained TOB missing");
        if(!(sourceTob.size()==20617u&&pod_materials50ed_fixture::detail::sha256(sourceTob)==
            "96469b3953b18d940bd0c8772758c68a75a69c3375b49ef98b966901c2e598d1"))
            std::cerr<<"course_member_identity site=gb_camera member=tracks\\gb.tob bytes="<<sourceTob.size()<<" stock=0\n";
        auto selectedCam=runtime_startup50fl::readBytes(
            original_player_identity50IK::podCamera(std::filesystem::path("work/native/project/disc/PODS"),pod));
        require(!selectedCam.empty(),"original camera selected CAM unavailable");
        // All throwing validation precedes publication. The actual selected
        // PHY remains borrowed from its existing retained original racer rig.
        cam=std::move(selectedCam);
        gbSource_=std::move(source);
        inputs={cam.data(),cam.size(),reinterpret_cast<const char*>(actualPhy.data()),actualPhy.size(),
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&actualPhy)),
            sourceTob.data(),sourceTob.size(),rr::game::OriginalCameraStartCourse::gb};
    }
    void load(const track_session_assets50IG::SessionAssets& track,
        const std::string& pod,const std::vector<std::uint8_t>& actualPhy) {
        using runtime_startup50fl::require;
        const bool ga=track_session_assets50IG::codeEquals(track.world->code,"GA");
        const bool ta=track_session_assets50IG::codeEquals(track.world->code,"TA");
        const bool ma=track_session_assets50IG::codeEquals(track.world->code,"MA");
        const bool sa=track_session_assets50IG::codeEquals(track.world->code,"SA");
        const bool ra=track_session_assets50IG::codeEquals(track.world->code,"RA");
        const bool tc=track_session_assets50IG::codeEquals(track.world->code,"TC");
        const bool sc=track_session_assets50IG::codeEquals(track.world->code,"SC");
        const bool mb=track_session_assets50IG::codeEquals(track.world->code,"MB");
        const bool rc=track_session_assets50IG::codeEquals(track.world->code,"RC");
        // The common route (OriginalCameraStartCourse::sa): stock SA, or the
        // latched add-on course whose code is not stock (146r, PORT EXTENSION).
        // Its row's own code names every member; no course name is keyed.
        const char* routeCode=rr::platform::courseRouteCode50IM();
        const bool registry=track.world&&track.world->code&&
            rr::assets::originalTrackCutRegistryCode50LA(track.world->code)&&
            routeCode&&track_session_assets50IG::codeEquals(track.world->code,routeCode);
        const bool commonRoute=sa||registry;
        require(tc||sc||ga||ta||ma||commonRoute||ra||rc||mb||track_session_assets50IG::codeEquals(track.world->code,"GC"),
            "original camera authoring requires an owned original track");
        require(original_player_identity50IK::admittedPodId(pod)&&!actualPhy.empty(),
            "original camera requires actual selected pod authoring");
        cam=runtime_startup50fl::readBytes(original_player_identity50IK::podCamera(std::filesystem::path("work/native/project/disc/PODS"),pod));
        const auto bytes=runtime_startup50fl::readBytes(track.archive);
        // Common course rule: identity is a receipt; shared decode capacity.
        course_archive_receipt50CI::receipt("camera_assets",track.world->code,bytes,track.archiveSha256,track.archiveBytes);
        rr::assets::ResArchive48C archive;rr::assets::DecodeError48C diagnostic;
        const auto limits=course_archive_receipt50CI::limits();
        // tracks\<code>.tob: one rule for every course (stock names unchanged).
        std::string tobMember="tracks\\";
        for(const char* c=track.world->code;*c;++c)tobMember+=(*c>='A'&&*c<='Z')?char(*c+32):*c;
        tobMember+=".tob";
        require(rr::assets::decodeResource48C(bytes,archive,diagnostic,nullptr,limits)&&
            rr::assets::extractResMember48C(archive,tobMember,tob,diagnostic),
            "original camera TOB member unavailable");
        if(registry){
            // The registry course's cut-camera row, from its own archive (TOB
            // [CSCameras], tracks\<code>NN.vub, intro INIs) into the one
            // registry slot. Absent data = no row (logged); the camera step
            // then refuses this course with its own message.
            std::string cutError;
            const bool derived=course_cut_identity50CI::derive(track.world->code,archive,bytes,cutError,track.world->container);
            if(!derived)std::cerr<<"course_cut_identity code="<<track.world->code<<" derived=0 ("<<cutError<<")\n";
        }
        inputs={cam.data(),cam.size(),reinterpret_cast<const char*>(actualPhy.data()),actualPhy.size(),
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&actualPhy)),tob.data(),tob.size(),
            tc?rr::game::OriginalCameraStartCourse::tc:sc?rr::game::OriginalCameraStartCourse::sc:mb?rr::game::OriginalCameraStartCourse::mb:ga?rr::game::OriginalCameraStartCourse::ga:ta?rr::game::OriginalCameraStartCourse::ta:
                ma?rr::game::OriginalCameraStartCourse::ma:commonRoute?rr::game::OriginalCameraStartCourse::sa:
                ra?rr::game::OriginalCameraStartCourse::ra:rc?rr::game::OriginalCameraStartCourse::rc:
                rr::game::OriginalCameraStartCourse::gc};
        if(commonRoute){
            // The common route carries its row's code (stock "SA" or the add-on code).
            const char* code=track.world->code;
            for(std::size_t i=0;i<2&&code[i];++i)inputs.courseCode[i]=(code[i]>='a'&&code[i]<='z')?char(code[i]-32):code[i];
            inputs.courseCode[2]='\0';
        }
    }
};
}
