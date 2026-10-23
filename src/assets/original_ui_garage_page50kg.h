#pragma once
#include "original_ui_pages50kg.h"
#include "original_ui_movement_control50kg.h"
#include <memory>

namespace rr::assets {
enum class OriginalUiGaragePageStatus50KG : std::uint8_t {
    ok,alreadyLoaded,archive,page,parse,resource,allocation
};
struct OriginalUiGaragePageDiagnostic50KG {
    OriginalUiGaragePageStatus50KG status=OriginalUiGaragePageStatus50KG::ok;
    std::uint32_t sourcePC=0;
    OriginalUiPageDiagnostic50KG parse{};
};
// Original PageInfo camera values, not a live camera/frame or registration.
// Garage vtable3D0A10+C4 points to shared camera reader1D7A90. Values remain
// authored, not normalized, projected or altered to fit a host window.
struct OriginalUiGarageCameraConfig50KG {
    const OriginalUiMemberIdentity50KG* identity=nullptr;
    std::array<float,3> position{},look{{0,-1,0}},up{{0,0,1}};
    float fovDegrees=35,rollRadians=0;
};
// Authenticated, immutable cold metadata for screen13/page1. This pins the
// original archive and retains unknown DAT fields without changing their
// spelling or values. Text, scene, Watto, model/material/texture, audio, camera
// and input owners must separately prove readiness. Success here cannot admit
// a Garage route, construct a session, authorize a purchase or write a profile.
class OriginalUiGaragePageData50KG final {
public:
    OriginalUiGaragePageData50KG()=default;
    OriginalUiGaragePageData50KG(const OriginalUiGaragePageData50KG&)=delete;
    OriginalUiGaragePageData50KG& operator=(const OriginalUiGaragePageData50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG>,
        OriginalUiGaragePageDiagnostic50KG&) noexcept;
    bool loaded() const noexcept {return bool(storage_);}
    const OriginalUiPage50KG* page() const noexcept;
    const OriginalUiDat50KG* uiDat() const noexcept;
    const OriginalUiDat50KG* pageDat() const noexcept;
    const OriginalUiDat50KG* widgetsDat() const noexcept;
    const OriginalUiGarageCameraConfig50KG* camera() const noexcept;
    // Authored widget6 is not in a page control's widget membership. Retaining
    // its descriptor is not permission to attach or route its input callbacks.
    const OriginalUiMovementControl50KG* movement() const noexcept;
    OriginalUiMemberView50KG wattoMcf() const noexcept;
    std::shared_ptr<const OriginalUiArchive50KG> archive() const noexcept;
private:
    struct Storage {
        std::shared_ptr<const OriginalUiArchive50KG> archive;
        OriginalUiDat50KG ui,pageDat,widgets;
        OriginalUiPage50KG page;
        OriginalUiGarageCameraConfig50KG camera;
        OriginalUiMovementControl50KG movement;
        OriginalUiMemberView50KG wattoMcf;
    };
    std::unique_ptr<Storage> storage_;
};
}
