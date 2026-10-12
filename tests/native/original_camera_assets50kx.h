#pragma once
#include "original_camera_session50kx.h"
#include "runtime_startup50fl_fixture.h"
#include "track_session_assets50ig_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_decode48c.h"

namespace original_camera_session50KX {
// Cold native storage for the actual selected disc members. The caller borrows
// the PHY already authenticated by its retained original racer rig. No source
// decoder is run by recovery tooling: these calls belong to the native game.
struct AuthoredStorage {
    std::vector<std::uint8_t> cam,tob;
    AuthoredInputs inputs{};
    void load(const track_session_assets50IG::SessionAssets& track,
        const std::string& pod,const std::vector<std::uint8_t>& actualPhy) {
        using runtime_startup50fl::require;
        const bool ga=track_session_assets50IG::codeEquals(track.world->code,"GA");
        require(ga||track_session_assets50IG::codeEquals(track.world->code,"GC"),
            "original camera authoring supports actual GC/GA only");
        require(pod.size()==5&&pod[0]=='P'&&pod[1]=='O'&&pod[2]=='D'&&
            pod[3]>='0'&&pod[3]<='9'&&pod[4]>='0'&&pod[4]<='9'&&!actualPhy.empty(),
            "original camera requires actual selected pod authoring");
        cam=runtime_startup50fl::readBytes(std::filesystem::path("work/native/project/disc/PODS")/pod/(pod+".CAM"));
        const auto bytes=runtime_startup50fl::readBytes(track.archive);
        require(bytes.size()==track.archiveBytes&&pod_materials50ed_fixture::detail::sha256(bytes)==track.archiveSha256,
            "original camera track archive identity rejected");
        rr::assets::ResArchive48C archive;rr::assets::DecodeError48C diagnostic;
        rr::assets::DecodeLimits48C limits;
        limits.resource_file_bytes=track.archiveBytes;
        // Bounds, not an alternate identity: the original archive SHA above
        // and exact member catalog in Session::prepare remain authoritative.
        limits.resource_decoded_bytes=64u*1024u*1024u;
        limits.resource_entries=4096;
        require(rr::assets::decodeResource48C(bytes,archive,diagnostic,nullptr,limits)&&
            rr::assets::extractResMember48C(archive,ga?"tracks\\ga.tob":"tracks\\gc.tob",tob,diagnostic),
            "original camera TOB member unavailable");
        inputs={cam.data(),cam.size(),reinterpret_cast<const char*>(actualPhy.data()),actualPhy.size(),
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&actualPhy)),tob.data(),tob.size(),
            ga?rr::game::OriginalCameraStartCourse::ga:rr::game::OriginalCameraStartCourse::gc};
    }
};
}
