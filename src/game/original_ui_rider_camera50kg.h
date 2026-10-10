#pragma once
#include "../assets/original_ui_pages50kg.h"
#include "original_ui_main_particle_camera50kg.h"
namespace rr::game {
struct OriginalUiRiderCamera50KG {
    const assets::OriginalUiMemberIdentity50KG* pageIdentity=nullptr;
    std::array<float,3> position{},look{{0,-1,0}},up{{0,0,1}};
    float fovDegrees=35,rollRadians=0,aspect1DC=0,aspect1F8=0;
    // Exact source matrices C0,80,100 and140,250. Despite the legacy shared
    // output type's Main name, this is the independently authenticated Rider
    // camera, never a Main identity or manager-camera substitute.
    CameraViewOutput49F core{};
    OriginalUiMainParticleCamera50KG sourceScreen{};
    std::array<float,3> binderCameraColumn{};
};
struct OriginalUiRiderCameraDiagnostic50KG {
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint32_t sourcePC=0;
};
// Rider vtable3D1110+C4=1D7A90, invoked during shared page create. Creates
// page+4BC from rider.dat PageInfo Camera*1; both player pages use this same
// authenticated file. Entry1E9E30 attaches this camera. Not the manager default.
// Only normal pose/matrix path; native caller must retain source debug gates.
bool prepareOriginalUiRiderCamera50KG(const assets::OriginalUiArchive50KG&,
    OriginalUiRiderCamera50KG&,OriginalUiRiderCameraDiagnostic50KG&) noexcept;
}
