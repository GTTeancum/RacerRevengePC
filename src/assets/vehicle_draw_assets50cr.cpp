#include "vehicle_draw_assets50cr.h"
#include "scene_asset48d.h"
#include <array>
#include <new>
#include <stdexcept>

namespace rr::assets {
namespace {
constexpr game::VehicleDrawComponent50CM components[vehicleDrawComponentCount50CR] = {
    game::VehicleDrawComponent50CM::chariot,
    game::VehicleDrawComponent50CM::leftEngine,
    game::VehicleDrawComponent50CM::rightEngine
};

std::uint32_t componentIndex(game::VehicleDrawComponent50CM component) noexcept {
    switch (component) {
    case game::VehicleDrawComponent50CM::chariot: return 0;
    case game::VehicleDrawComponent50CM::leftEngine: return 1;
    case game::VehicleDrawComponent50CM::rightEngine: return 2;
    }
    return vehicleDrawComponentCount50CR;
}

bool reject(VehicleDrawAssetDiagnostic50CR& diagnostic,
    VehicleDrawAssetError50CR error, std::uint32_t item = 0) noexcept {
    diagnostic.error = error;
    diagnostic.item = item;
    return false;
}
}

struct VehicleDrawAssets50CR::Storage {
    struct Component {
        Scene48D scene;
        Mesh48B mesh;
        std::vector<game::RigidVertex49S> localVertices;
        std::array<game::SceneNodeIndex49P, game::maxRigidPalette49S> sourceNodes{};
        std::array<std::uint8_t, game::maxRigidPalette49S> paletteIds{};
        game::VehicleDrawSelection50CM selection{};
        game::VehicleDrawMesh50CM prepared{};
        std::vector<Vertex48B> staging[vehicleDrawStagingBankCount50CR];
        std::vector<WorldNormal50HK> normalStaging[vehicleDrawStagingBankCount50CR];
    } component[vehicleDrawComponentCount50CR];
};

VehicleDrawAssets50CR::VehicleDrawAssets50CR() noexcept = default;
VehicleDrawAssets50CR::~VehicleDrawAssets50CR() = default;

bool VehicleDrawAssets50CR::load(const VehicleDrawAssetsInput50CR& input,
    VehicleDrawAssetDiagnostic50CR& diagnostic) noexcept {
    diagnostic.error = VehicleDrawAssetError50CR::none;
    diagnostic.component = diagnostic.item = 0;
    diagnostic.parserMessage.clear();
    diagnostic.draw = {};
    if (storage_) return reject(diagnostic, VehicleDrawAssetError50CR::alreadyLoaded);

    // Check every descriptor before parsing/allocating any component. The
    // supplied vector/array storage must remain stable throughout this call.
    for (std::uint32_t i = 0; i < vehicleDrawComponentCount50CR; ++i) {
        diagnostic.component = i;
        const auto& source = input.component[i];
        if (!source.sceneBytes || !source.sourceNodes || !source.paletteIds ||
            !source.sourceNodeCount || source.sourceNodeCount > game::maxRigidPalette49S ||
            !source.paletteCount || source.paletteCount > game::maxRigidPalette49S)
            return reject(diagnostic, VehicleDrawAssetError50CR::input);
    }

    try {
        // Commit only the pointer. Storage and all of its vectors stay at
        // their prepared addresses; no Component/Scene/Mesh is moved later.
        std::unique_ptr<Storage> next(new Storage);
        for (std::uint32_t i = 0; i < vehicleDrawComponentCount50CR; ++i) {
            diagnostic.component = i;
            const auto& source = input.component[i];
            auto& target = next->component[i];
            if (!parseSceneAsset48D(*source.sceneBytes, target.scene, diagnostic.parserMessage))
                return reject(diagnostic, VehicleDrawAssetError50CR::scene);
            if (!parseMesh48B(target.scene.neutral_lod0.data(), target.scene.neutral_lod0.size(),
                target.mesh, diagnostic.parserMessage))
                return reject(diagnostic, VehicleDrawAssetError50CR::compatibilityMesh);

            if (source.sourceNodeCount != target.scene.nodes.size() ||
                source.paletteCount != target.scene.nodes.size())
                return reject(diagnostic, VehicleDrawAssetError50CR::binding);
            for (std::uint32_t n = 0; n < source.sourceNodeCount; ++n) {
                const auto index = static_cast<std::int32_t>(source.sourceNodes[n]);
                // Preserve the supplied values; never synthesize an identity
                // palette or assume the diagnostic Pod01 graph's node ranges.
                if (index < 0 || static_cast<std::uint32_t>(index) >= game::vehicleFrameNodeCount49W ||
                    source.paletteIds[n] != n)
                    return reject(diagnostic, VehicleDrawAssetError50CR::binding, n);
                target.sourceNodes[n] = source.sourceNodes[n];
                target.paletteIds[n] = source.paletteIds[n];
            }

            // Bound new bridge storage before allocating it. The existing
            // parsers retain their own cold semantic/compatibility limits.
            if (target.mesh.vertices.empty() || target.mesh.vertices.size() > game::maxRigidVertices49S) {
                diagnostic.draw.error = game::VehicleDrawError50CM::storage;
                return reject(diagnostic, VehicleDrawAssetError50CR::drawMesh);
            }
            const auto vertexCount = static_cast<std::uint32_t>(target.mesh.vertices.size());
            target.localVertices.resize(vertexCount);
            if (!game::prepareVehicleDrawMesh50CM(target.scene, target.mesh,
                target.localVertices.data(), vertexCount, target.prepared, diagnostic.draw))
                return reject(diagnostic, VehicleDrawAssetError50CR::drawMesh);

            target.selection.component = components[i];
            target.selection.sourceNodes = target.sourceNodes.data();
            target.selection.sourceNodeCount = source.sourceNodeCount;
            target.selection.paletteIds = target.paletteIds.data();
            target.selection.paletteCount = source.paletteCount;
            for (auto& bank : target.staging) bank.resize(vertexCount);
            for (auto& bank : target.normalStaging) bank.resize(vertexCount);
        }
        storage_.swap(next);
        diagnostic.component = 0;
        return true;
    } catch (const std::bad_alloc&) {
        return reject(diagnostic, VehicleDrawAssetError50CR::allocation);
    } catch (const std::length_error&) {
        return reject(diagnostic, VehicleDrawAssetError50CR::allocation);
    } catch (...) {
        // No callbacks run here. Keep the owner empty even if a future parser
        // throws an exception outside its present bool/error contract.
        return reject(diagnostic, VehicleDrawAssetError50CR::unexpectedFailure);
    }
}

bool VehicleDrawAssets50CR::loaded() const noexcept { return bool(storage_); }

const Scene48D* VehicleDrawAssets50CR::scene(game::VehicleDrawComponent50CM component) const noexcept {
    const auto index = componentIndex(component);
    return storage_ && index < vehicleDrawComponentCount50CR ? &storage_->component[index].scene : nullptr;
}

const Mesh48B* VehicleDrawAssets50CR::compatibilityMesh(game::VehicleDrawComponent50CM component) const noexcept {
    const auto index = componentIndex(component);
    return storage_ && index < vehicleDrawComponentCount50CR ? &storage_->component[index].mesh : nullptr;
}

const game::VehicleDrawMesh50CM* VehicleDrawAssets50CR::preparedMesh(
    game::VehicleDrawComponent50CM component) const noexcept {
    const auto index = componentIndex(component);
    return storage_ && index < vehicleDrawComponentCount50CR ? &storage_->component[index].prepared : nullptr;
}

bool VehicleDrawAssets50CR::batchInput(std::uint32_t bank, game::ModelPalettePhase49S phase,
    game::ModelPaletteSceneState49S suppliedState, game::VehicleDrawBatchInput50CO& output) noexcept {
    if (!storage_ || bank >= vehicleDrawStagingBankCount50CR ||
        phase != game::ModelPalettePhase49S::selectedRenderPreparation ||
        suppliedState != game::ModelPaletteSceneState49S::completeCurrentScene)
        return false;
    game::VehicleDrawBatchInput50CO next{};
    for (std::uint32_t i = 0; i < vehicleDrawComponentCount50CR; ++i) {
        auto& source = storage_->component[i];
        next.selection[i] = source.selection;
        next.selection[i].phase = phase;
        next.selection[i].suppliedState = suppliedState;
        next.mesh[i] = source.prepared;
        next.staging[i] = {source.staging[bank].data(), source.prepared.vertexCount,
            source.normalStaging[bank].data(), source.prepared.vertexCount};
    }
    output = next;
    return true;
}
}
