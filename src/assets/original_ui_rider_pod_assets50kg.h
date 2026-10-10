#pragma once
#include "original_ui_pages50kg.h"
#include "model_geometry49o.h"
#include "import_scene48d.h"
#include <array>
#include <memory>

namespace rr::assets {
struct OriginalUiRiderPodConfig50KG {
    std::array<float,3> position{},look{},up{},velocity{};
    std::array<bool,3> rotate{};
    bool lit=false,startPaused=true,cleanFromMemory=false;
};
enum class OriginalUiRiderPodError50KG : std::uint8_t {
    none,alreadyLoaded,archive,widget,member,hierarchy,geometry,allocation
};
struct OriginalUiRiderPodDiagnostic50KG {
    OriginalUiRiderPodError50KG error=OriginalUiRiderPodError50KG::none;
    OriginalUiPageDiagnostic50KG text{};
    GeometryDiagnostic49O hierarchy{};
    DecodeError48C geometry{};
    std::uint32_t sourcePC=0;
};
// Authenticated cold owner for the real dynamic User_Rotate pod geometry.
// No fictitious character/engine split, static portrait or replacement mesh.
// loaded() proves cold assets only, not dynamic model/render/route readiness.
class OriginalUiRiderPodAssets50KG final {
public:
    OriginalUiRiderPodAssets50KG() noexcept;
    ~OriginalUiRiderPodAssets50KG();
    OriginalUiRiderPodAssets50KG(const OriginalUiRiderPodAssets50KG&)=delete;
    OriginalUiRiderPodAssets50KG& operator=(const OriginalUiRiderPodAssets50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG>,std::uint16_t widgetId,
        OriginalUiRiderPodDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    std::uint16_t widgetId() const noexcept;
    const OriginalUiRiderPodConfig50KG* config() const noexcept;
    const PsgHierarchyView49O* hierarchy() const noexcept;
    const Scene48D* scene() const noexcept;
    const OriginalUiMemberIdentity50KG* widgetSource() const noexcept;
    OriginalUiMemberView50KG modelSource() const noexcept;
    // Pin retains all original material members for the shared material owner.
    std::shared_ptr<const OriginalUiArchive50KG> archive() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
