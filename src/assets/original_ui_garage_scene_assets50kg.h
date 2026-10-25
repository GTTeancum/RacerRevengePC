#pragma once
#include "original_ui_pages50kg.h"
#include "original_standard_model_resource50la.h"
#include "model_bindings49o.h"
#include <array>
#include <memory>

namespace rr::assets {
inline constexpr std::size_t originalUiGarageStaticModelCount50KG=30;
inline constexpr std::size_t originalUiGarageActorCount50KG=10;
enum class OriginalUiGarageSceneAssetError50KG : std::uint8_t {
    none,alreadyLoaded,archive,config,member,hierarchy,binding,motion,geometry,allocation
};
struct OriginalUiGarageSceneAssetDiagnostic50KG {
    OriginalUiGarageSceneAssetError50KG error=OriginalUiGarageSceneAssetError50KG::none;
    std::uint32_t sourcePC=0,ordinal=0;
    OriginalUiPageDiagnostic50KG text{};
    OriginalStandardResourceDiagnostic50LA resource{};
    GeometryDiagnostic49O model{};
    MotionBindingDiagnostic49O binding{};
};
// Immutable original payload, not a live model, material table entry or cache
// reference. Different authored placements never become one live instance.
struct OriginalUiGarageModelAsset50KG {
    // Pins source/hierarchy even when a caller retains this shared payload
    // after releasing the enclosing scene asset collection.
    std::shared_ptr<const OriginalUiArchive50KG> archive;
    OriginalUiMemberView50KG source{};
    PsgHierarchyView49O hierarchy{};
    std::shared_ptr<const OriginalStandardModelResource50LA> payload;
    const Scene48D* scene() const noexcept {return payload?payload->scene():nullptr;}
};
struct OriginalUiGarageStaticModel50KG {
    std::uint32_t ordinal=0;
    std::string slt;
    std::array<float,3> position{},look{};
    std::shared_ptr<const OriginalUiGarageModelAsset50KG> model;
    // garage.scn authors Position and LookVector only. This descriptor does
    // not manufacture an UpVector or an attachment/observer registration.
};
struct OriginalUiGarageActorMotion50KG {
    std::string name;
    OriginalUiMemberView50KG source{};
    MotionView49O motion{};
    bool looping=false;
    std::int32_t authoredFrames=0;
    float authoredFps=0;
};
struct OriginalUiGarageActorAsset50KG {
    std::uint32_t ordinal=0;
    std::string mcfName,referenceName;
    OriginalUiDat50KG mcf;
    OriginalUiMemberView50KG vut{};
    MotionNodeBindings49O bindings{};
    std::shared_ptr<const OriginalUiGarageModelAsset50KG> model;
    std::array<OriginalUiGarageActorMotion50KG,4> motions{};
    std::uint32_t motionCount=0,startingZone=0;
    std::array<float,3> offset{};
    bool useLighting=false,useBlendedKeys=false,useCollision=false;
    // Bindings retain original PSG node indices, not indices shifted for an
    // invented parent. A real character graph owner remaps them as necessary.
};
struct OriginalUiGarageSelectedPodAsset50KG {
    std::shared_ptr<const OriginalUiGarageModelAsset50KG> model;
    std::array<float,3> position{},look{},up{};
    // Manager270+2B0 owns this model; Garage page4C4 borrows it. It is not a
    // User_Rotate widget or another static/animation row in garage.scn/toy.
};
// Cold, transactional preparation from the SAME retained authenticated Garage
// and selected-career E archives. Exactly30 static placements,10 keyframed actors
// and the actual selected pod. Watto belongs to its separate widget10 owner. Reusing
// an immutable PSG payload does not clone a model or change source row order.
// Success provides no route, Entity, observer, camera, material, renderer,
// animation tick, audio, particle, profile or purchase authority.
class OriginalUiGarageSceneAssets50KG final {
public:
    OriginalUiGarageSceneAssets50KG() noexcept;
    ~OriginalUiGarageSceneAssets50KG();
    OriginalUiGarageSceneAssets50KG(const OriginalUiGarageSceneAssets50KG&)=delete;
    OriginalUiGarageSceneAssets50KG& operator=(const OriginalUiGarageSceneAssets50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG> garage,
        std::shared_ptr<const OriginalUiArchive50KG> garagePod19,
        OriginalUiGarageSceneAssetDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    int selectedCareer() const noexcept;
    const OriginalUiGarageStaticModel50KG* staticModel(std::size_t) const noexcept;
    const OriginalUiGarageActorAsset50KG* actor(std::size_t) const noexcept;
    const OriginalUiGarageSelectedPodAsset50KG* selectedPod() const noexcept;
    const OriginalUiDat50KG* sceneDat() const noexcept;
    const OriginalUiDat50KG* toyDat() const noexcept;
    std::shared_ptr<const OriginalUiArchive50KG> archive() const noexcept;
    std::shared_ptr<const OriginalUiArchive50KG> selectedArchive() const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
