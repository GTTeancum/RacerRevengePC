#pragma once
#include "original_ui_pages50kg.h"
#include "original_stream_metadata50kz.h"
#include "model_geometry49o.h"
#include "model_bindings49o.h"
#include "import_scene48d.h"
#include <array>
#include <memory>
#include <string_view>
namespace rr::assets {
enum class OriginalUiGarageWattoAssetError50KG : std::uint8_t {
    none,alreadyLoaded,archive,widget,config,member,hierarchy,binding,motion,geometry,audio,allocation
};
struct OriginalUiGarageWattoAssetDiagnostic50KG {
    OriginalUiGarageWattoAssetError50KG error=OriginalUiGarageWattoAssetError50KG::none;
    std::uint32_t sourcePC=0,ordinal=0;
    OriginalUiPageDiagnostic50KG text{};
    GeometryDiagnostic49O model{};
    MotionBindingDiagnostic49O binding{};
    DecodeError48C geometry{};
};
struct OriginalUiGarageWattoMotion50KG {
    std::string name;
    OriginalUiMemberView50KG source{};
    MotionView49O motion{};
    bool looping=false;
    std::int32_t authoredFrames=0;
    float authoredFps=0;
};
struct OriginalUiGarageWattoEvent50KG {
    std::uint32_t id=0;
    std::string filename;
    bool stream=false,stereo=false,loop=false;
    // Pinned by this asset owner's immutable stream metadata owner. This is
    // source Sound+20 authoring, not PCM time, an active voice or readiness.
    const OriginalStreamMetadataRow50KZ* metadata=nullptr;
};
// Immutable authenticated widget10 closure only. No file IO, native audio,
// model attachment, material registration, route or profile authority.
class OriginalUiGarageWattoAssets50KG final {
public:
    OriginalUiGarageWattoAssets50KG() noexcept;
    ~OriginalUiGarageWattoAssets50KG();
    OriginalUiGarageWattoAssets50KG(const OriginalUiGarageWattoAssets50KG&)=delete;
    OriginalUiGarageWattoAssets50KG& operator=(const OriginalUiGarageWattoAssets50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<const OriginalStreamMetadata50KZ>,OriginalUiGarageWattoAssetDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    const OriginalUiGarageWattoMotion50KG* motion(std::size_t) const noexcept;
    const OriginalUiGarageWattoMotion50KG* findMotion(std::string_view) const noexcept;
    const OriginalUiGarageWattoEvent50KG* event(std::uint32_t) const noexcept;
    const PsgHierarchyView49O* hierarchy() const noexcept;
    const MotionNodeBindings49O* bindings() const noexcept;
    const Scene48D* scene() const noexcept;
    const std::array<std::uint32_t,64>* wingMask() const noexcept;
    const std::array<float,3>* position() const noexcept;
    const std::array<float,3>* look() const noexcept;
    const std::array<float,3>* up() const noexcept;
    float randomMotionProbability() const noexcept;
    float switchSideProbability() const noexcept;
    const OriginalUiMemberIdentity50KG* geometrySource() const noexcept;
    const OriginalUiMemberIdentity50KG* audioSource() const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
