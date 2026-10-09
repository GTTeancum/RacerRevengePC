#pragma once
#include "../game/vehicle_draw_batch50co.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rr::assets {
constexpr std::uint32_t vehicleDrawComponentCount50CR = 3;
constexpr std::uint32_t vehicleDrawStagingBankCount50CR = 2;

// Cold input, in chariot/left-engine/right-engine order. RRSCENE1 already
// contains its matching neutral_lod0 RRMESH01 bytes. The source table must be
// the actual model+1B8 PSG source-index table, and paletteIds must be the actual
// selected PSG palette. Neither table is inferred from node names or ranges.
// These inputs are borrowed only for load(); success owns copies of all data.
struct VehicleDrawAssetInput50CR {
    const std::vector<std::uint8_t>* sceneBytes = nullptr;
    const game::SceneNodeIndex49P* sourceNodes = nullptr;
    std::uint32_t sourceNodeCount = 0;
    const std::uint8_t* paletteIds = nullptr;
    std::uint32_t paletteCount = 0;
};
struct VehicleDrawAssetsInput50CR {
    VehicleDrawAssetInput50CR component[vehicleDrawComponentCount50CR]{};
};
enum class VehicleDrawAssetError50CR : std::uint8_t {
    none, alreadyLoaded, input, scene, compatibilityMesh, binding,
    drawMesh, allocation, unexpectedFailure
};
struct VehicleDrawAssetDiagnostic50CR {
    VehicleDrawAssetError50CR error = VehicleDrawAssetError50CR::none;
    // Fixed component order above; item is the rejected source/palette slot.
    std::uint32_t component = 0, item = 0;
    std::string parserMessage;
    game::VehicleDrawDiagnostic50CM draw{};
};

// A caller-owned, load-once lifetime anchor for the three immutable assets,
// copied source/palette tables and two separately writable vertex banks.
// Noncopyable/nonmovable: prepared descriptors never need pointer rebasing.
// There is no reload/reset operation. Destroy only after every borrowed view,
// batch and pending renderer upload has stopped using this owner.
class VehicleDrawAssets50CR final {
public:
    VehicleDrawAssets50CR() noexcept;
    ~VehicleDrawAssets50CR();
    VehicleDrawAssets50CR(const VehicleDrawAssets50CR&) = delete;
    VehicleDrawAssets50CR& operator=(const VehicleDrawAssets50CR&) = delete;
    VehicleDrawAssets50CR(VehicleDrawAssets50CR&&) = delete;
    VehicleDrawAssets50CR& operator=(VehicleDrawAssets50CR&&) = delete;

    // Cold only: reuses parseSceneAsset48D, parseMesh48B and
    // prepareVehicleDrawMesh50CM. Allocates only before publication, including
    // exact-sized local-vertex storage and both vertex banks for each model.
    // All three components commit together; failure leaves this owner empty
    // and retryable. An already-loaded owner rejects without invalidating it.
    // Inputs/descriptors/diagnostic are distinct live storage. Array extents
    // and authentic provenance of the supplied binding tables are caller
    // preconditions. FE_TONEAREST and the bridge's strict FP rules apply.
    bool load(const VehicleDrawAssetsInput50CR&,
        VehicleDrawAssetDiagnostic50CR&) noexcept;

    bool loaded() const noexcept;
    // Const access preserves original material names/indices, semantic data,
    // compatibility UV/color bytes and prepared descriptors. Null if unloaded
    // or passed an invalid component enum. No material policy is added.
    const Scene48D* scene(game::VehicleDrawComponent50CM) const noexcept;
    const Mesh48B* compatibilityMesh(game::VehicleDrawComponent50CM) const noexcept;
    const game::VehicleDrawMesh50CM* preparedMesh(game::VehicleDrawComponent50CM) const noexcept;

    // Allocation-free descriptor assembly, directly consumable by
    // buildVehicleDrawBatch50CO. Supply the currently inactive CPU bank, 0 or 1.
    // Each phase/state tag is an explicit current-frame caller attestation;
    // loading assets does not establish that graphics ran. Unsupported tags,
    // unloaded owners and invalid banks reject without changing output.
    //
    // Asset/binding views are immutable after load. Only staging vertex bytes
    // are writable. A batch failure can change its staging bank; retain the
    // prior published batch in the other bank and switch only after success.
    // Do not write/reuse a bank while any reader still consumes it. Workspace,
    // frame, bindings, output and diagnostics remain separate caller storage.
    bool batchInput(std::uint32_t bank, game::ModelPalettePhase49S phase,
        game::ModelPaletteSceneState49S suppliedState,
        game::VehicleDrawBatchInput50CO& output) noexcept;

private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
