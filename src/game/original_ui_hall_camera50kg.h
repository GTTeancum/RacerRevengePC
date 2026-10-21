#pragma once
#include "../assets/original_ui_pages50kg.h"
#include "original_ui_main_particle_camera50kg.h"
namespace rr::game {
struct OriginalUiHallCamera50KG {
    const assets::OriginalUiMemberIdentity50KG* pageIdentity=nullptr;
    std::array<float,3> position{},look{{0,-1,0}},up{{0,0,1}};
    float fovDegrees=35,rollRadians=0,aspect1DC=0,aspect1F8=0;
    // Exact source matrices C0,80,100 and140,250. Despite the legacy shared
    // output type's Main name, this is the independently authenticated Hall
    // camera, never a Main identity or manager-camera substitute.
    CameraViewOutput49F core{};
    OriginalUiMainParticleCamera50KG sourceScreen{};
    std::array<float,3> binderCameraColumn{};
};
struct OriginalUiHallCameraDiagnostic50KG {
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint32_t sourcePC=0;
};
// Hall vtable3D1380+C4=1D7A90, invoked during shared page create. Creates
// page+4BC from hall.dat PageInfo Camera*1; this page uses its own
// authenticated file. Hall entry attaches this camera. Not the manager default.
// Only normal pose/matrix path; native caller must retain source debug gates.
bool prepareOriginalUiHallCamera50KG(const assets::OriginalUiArchive50KG&,
    OriginalUiHallCamera50KG&,OriginalUiHallCameraDiagnostic50KG&) noexcept;
}
