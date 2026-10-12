// Reconstructed source-only candidate, 2026-10-12; not byte-verified against lost files.
#pragma once
#include "original_ui_pages50kg.h"
#include "model_geometry49o.h"
#include "model_bindings49o.h"
#include "import_scene48d.h"
#include <array>
#include <memory>

namespace rr::assets {
enum class OriginalUiCourseHologramAssetError50KG : std::uint8_t {
    none,alreadyLoaded,archive,widget,config,member,hierarchy,binding,motion,geometry,allocation
};
struct OriginalUiCourseHologramAssetDiagnostic50KG {
    OriginalUiCourseHologramAssetError50KG error=OriginalUiCourseHologramAssetError50KG::none;
    OriginalUiPageDiagnostic50KG text{};
    GeometryDiagnostic49O model{};
    MotionBindingDiagnostic49O binding{};
    DecodeError48C geometry{};
    std::uint32_t sourcePC=0,motionOrdinal=0;
};
struct OriginalUiCourseHologramMotion50KG {
    std::string name;
    OriginalUiMemberView50KG source{};
    MotionView49O motion{};
    bool looping=false;
    // MCF frameCount/FPS retained as authoring metadata, never a replacement
    // for runtime motion.duration read from the actual MOT header at1525C4.
    std::int32_t authoredFrames=0;
    float authoredFps=0;
};
// Cold authenticated common Soultree_Character owner, created eagerly by
// manager1CD304 ->2A85F0/2A8770. Exactly one original MOT, PSG and VUT;
// no Rider six-motion lifecycle, synthesized mesh, GPU admission or guest code.
class OriginalUiCourseHologramAssets50KG final {
public:
    OriginalUiCourseHologramAssets50KG() noexcept;
    ~OriginalUiCourseHologramAssets50KG();
    OriginalUiCourseHologramAssets50KG(const OriginalUiCourseHologramAssets50KG&)=delete;
    OriginalUiCourseHologramAssets50KG& operator=(const OriginalUiCourseHologramAssets50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG>,std::uint16_t widgetId,
        OriginalUiCourseHologramAssetDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    std::uint16_t widgetId() const noexcept;
    const std::string* baseName() const noexcept;
    const OriginalUiMemberIdentity50KG* widgetSource() const noexcept;
    const OriginalUiMemberIdentity50KG* mcfSource() const noexcept;
    const PsgHierarchyView49O* hierarchy() const noexcept;
    const MotionNodeBindings49O* bindings() const noexcept;
    const Scene48D* scene() const noexcept;
    const std::array<float,3>* position() const noexcept;
    const std::array<float,3>* look() const noexcept;
    const std::array<float,3>* up() const noexcept;
    const OriginalUiCourseHologramMotion50KG* motion(std::size_t ordinal) const noexcept;
    const OriginalUiCourseHologramMotion50KG* findMotion(const std::string& name) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
} // namespace rr::assets
