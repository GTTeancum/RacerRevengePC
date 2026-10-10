#pragma once
#include "original_ui_pages50kg.h"
#include <array>

namespace rr::assets {
struct OriginalUiMainCameraConfig50KG {
    const OriginalUiMemberIdentity50KG* identity=nullptr;
    bool hasCameraInfo=false;
    // Constructor1143F0 defaults when CameraInfo is absent. When the section
    // exists,1CF4F0 instead supplies Position(0,0,0), Look(0,-1,0), Up(0,0,1),
    // FOV35, Roll0 as per-key fallbacks. W for each vector is original1.
    std::array<float,3> position{{0,50,35}},look{{0,0,1}},up{{0,1,0}};
    float fovDegrees=77,rollRadians=0;
    std::array<bool,5> authoredFields{}; // Position, Look, Up, FOV, Roll.
};
bool isOriginalUiMainCameraConfig50KG(const OriginalUiMainCameraConfig50KG&) noexcept;
// Reads only authenticated Main ui.dat CameraInfo using the manager's actual
// virtual+B0=1CF4F0. Atomic; does not read camera fields from a page DAT.
bool readOriginalUiMainCameraConfig50KG(const OriginalUiDat50KG& ui,
    OriginalUiMainCameraConfig50KG&,OriginalUiPageDiagnostic50KG&) noexcept;

struct OriginalUiMainUnusedCameraMetadata50KG {
    const OriginalUiMemberIdentity50KG* pageIdentity=nullptr;
    // Original source entries, deliberately NOT parsed as effective camera
    // values. Main's factory+C4=1E5CF0 aliases manager+2B0 and never reads them.
    // Pointers remain valid only while the immutable page DAT remains alive.
    std::array<const OriginalUiDatValue50KG*,5> fields{};
};
// Exact five known keys; callers may admit these ONLY for proven Main page1.
// This is not a generic PageInfo camera whitelist. Other page classes invoke
// the genuine1D7A90 reader and cannot use this source-unused interpretation.
bool isOriginalUiMainUnusedCameraField50KG(const std::string& key) noexcept;
bool retainOriginalUiMainUnusedCameraMetadata50KG(const OriginalUiDat50KG& main,
    OriginalUiMainUnusedCameraMetadata50KG&,OriginalUiPageDiagnostic50KG&) noexcept;
}
