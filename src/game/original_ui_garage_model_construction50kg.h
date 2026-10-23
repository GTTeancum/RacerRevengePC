#pragma once
#include "../assets/original_ui_garage_scene_assets50kg.h"
#include "scene_graph49p.h"
#include <memory>

namespace rr::game {
enum class OriginalUiGarageModelConstructionError50KG : std::uint8_t {
    none,alreadyPrepared,asset,hierarchy,graph,allocation
};
struct OriginalUiGarageModelConstructionDiagnostic50KG {
    OriginalUiGarageModelConstructionError50KG error=OriginalUiGarageModelConstructionError50KG::none;
    std::uint32_t sourcePC=0;
};
// Numerical/source-layout preparation ONLY for the normal selected19E branch.
// Manager1D1CD0 passes loader a2=0: PSG row0 IS the actual model root. Do not
// create a second identity parent and retain the PSG translation in its child.
// The manager replaces that root's translation and axes at1D1E60..1D1E84.
//
// This immutable direct-root graph does not construct Entity8, acquire source
// references, register observers, create packets, join a scene or publish a
// render stamp. It cannot be used as a readiness receipt for those operations.
// The real manager owns model270+2B0; Garage page4C4 only borrows it (1FB660).
class OriginalUiGarageModelConstruction50KG final {
public:
    OriginalUiGarageModelConstruction50KG() noexcept;
    ~OriginalUiGarageModelConstruction50KG();
    OriginalUiGarageModelConstruction50KG(const OriginalUiGarageModelConstruction50KG&)=delete;
    OriginalUiGarageModelConstruction50KG& operator=(const OriginalUiGarageModelConstruction50KG&)=delete;
    bool prepareSelected19E(std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,
        OriginalUiGarageModelConstructionDiagnostic50KG&) noexcept;
    bool prepared() const noexcept;
    const SceneNode49P* root() const noexcept;
    const assets::PsgNode49O* originalRoot() const noexcept;
    const assets::OriginalUiGarageSelectedPodAsset50KG* source() const noexcept;
    std::uint32_t graphNodeCount() const noexcept;
    SceneNodeIndex49P sourceRowIndex(std::uint32_t) const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
