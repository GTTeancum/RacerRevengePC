// Authenticated original-source reconstruction, 2026-10-12; no runtime validation.
#pragma once
#include "original_ui_pages50kg.h"
#include "model_geometry49o.h"
#include "model_bindings49o.h"
#include "import_scene48d.h"
#include <array>
#include <memory>

namespace rr::assets {
enum class OriginalUiControlsModelAssetError50KG : std::uint8_t {
    none,alreadyLoaded,archive,widget,config,member,hierarchy,binding,motion,geometry,allocation
};
struct OriginalUiControlsModelAssetDiagnostic50KG {
    OriginalUiControlsModelAssetError50KG error=OriginalUiControlsModelAssetError50KG::none;
    OriginalUiPageDiagnostic50KG text{};
    GeometryDiagnostic49O model{};
    MotionBindingDiagnostic49O binding{};
    DecodeError48C geometry{};
    std::uint32_t sourcePC=0,motionOrdinal=0;
};
struct OriginalUiControlsModelMotion50KG {
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
// manager1CD304 ->2A85F0/2A8770. Two original MOTs, one PSG and VUT;
// no Rider six-motion lifecycle, synthesized mesh, GPU admission or guest code.
class OriginalUiControlsModelAssets50KG final {
public:
    OriginalUiControlsModelAssets50KG() noexcept;
    ~OriginalUiControlsModelAssets50KG();
    OriginalUiControlsModelAssets50KG(const OriginalUiControlsModelAssets50KG&)=delete;
    OriginalUiControlsModelAssets50KG& operator=(const OriginalUiControlsModelAssets50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG>,std::uint16_t widgetId,
        OriginalUiControlsModelAssetDiagnostic50KG&) noexcept;
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
    const OriginalUiControlsModelMotion50KG* motion(std::size_t ordinal) const noexcept;
    const OriginalUiControlsModelMotion50KG* findMotion(const std::string& name) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
} // namespace rr::assets
