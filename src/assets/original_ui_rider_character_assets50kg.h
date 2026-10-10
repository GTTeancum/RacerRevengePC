#pragma once
#include "original_ui_pages50kg.h"
#include "model_geometry49o.h"
#include "model_bindings49o.h"
#include "import_scene48d.h"
#include <array>
#include <memory>

namespace rr::assets {
enum class OriginalUiRiderCharacterAssetError50KG : std::uint8_t {
    none,alreadyLoaded,archive,widget,config,member,hierarchy,binding,motion,geometry,allocation
};
struct OriginalUiRiderCharacterAssetDiagnostic50KG {
    OriginalUiRiderCharacterAssetError50KG error=OriginalUiRiderCharacterAssetError50KG::none;
    OriginalUiPageDiagnostic50KG text{};
    GeometryDiagnostic49O model{};
    MotionBindingDiagnostic49O binding{};
    DecodeError48C geometry{};
    std::uint32_t sourcePC=0,motionOrdinal=0;
};
struct OriginalUiRiderCharacterMotion50KG {
    std::string name;
    OriginalUiMemberView50KG source{};
    MotionView49O motion{};
    bool looping=false;
    // MCF frameCount/FPS retained as authoring metadata, never a replacement
    // for runtime motion.duration read from the actual MOT header at1525C4.
    std::int32_t authoredFrames=0;
    float authoredFps=0;
};
// Cold immutable owner for ONE actual Soultree_Character widget. Reuse the
// existing PSG, VUT, MOT and original scene-geometry readers; no race rig/name
// fallback, guest address, host file IO, GPU call, synthesized mesh or pose.
// Loaded does NOT mean character playback/preview/route readiness. Model
// creation, root pose, current motion time, crossfade154A40/154FB0 and scale
// restoration still require their actual native owners before publication.
// Requires the source-backed shared ShortAxis MOT extension for real Rider
// motions. Without it load fails explicitly with error=motion, never ByteAxis
// reinterpretation or static-portrait fallback.
class OriginalUiRiderCharacterAssets50KG final {
public:
    OriginalUiRiderCharacterAssets50KG() noexcept;
    ~OriginalUiRiderCharacterAssets50KG();
    OriginalUiRiderCharacterAssets50KG(const OriginalUiRiderCharacterAssets50KG&)=delete;
    OriginalUiRiderCharacterAssets50KG& operator=(const OriginalUiRiderCharacterAssets50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG>,std::uint16_t widgetId,
        OriginalUiRiderCharacterAssetDiagnostic50KG&) noexcept;
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
    const OriginalUiRiderCharacterMotion50KG* motion(std::size_t ordinal) const noexcept;
    const OriginalUiRiderCharacterMotion50KG* findMotion(const std::string& name) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
} // namespace rr::assets
