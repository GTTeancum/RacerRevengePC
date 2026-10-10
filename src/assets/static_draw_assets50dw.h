#pragma once
#include "native_asset48b.h"
#include "import_scene48d.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rr::assets {
constexpr std::uint32_t maxStaticDrawAssets50DW = 4096;
constexpr std::uint32_t maxStaticDrawInstances50DW = 100000;
constexpr std::uint32_t maxStaticDrawSpans50DW = 100000;

// Actual retained RRSCENE1 bytes, including their matching RRMESH01 export.
// Borrowed during load only. Asset identity/resolution is the caller's job.
struct StaticDrawAssetInput50DW {
    const std::vector<std::uint8_t>* sceneBytes = nullptr;
};
struct StaticDrawInstance50DW {
    std::uint32_t asset = 0;
    // Explicit resolved placement, in WorldInstance48F::matrix row-vector
    // order. Replaces the saved PSG root; it is NOT appended to the bind mesh.
    // Zero initialization deliberately does not invent identity placement.
    std::array<double, 16> matrix{};
};
struct StaticDrawBudgets50DW {
    // Required caller-selected ceilings, not reservation requests. Zero rejects.
    std::uint32_t assetCapacity = 0, instanceCapacity = 0;
    // Aggregate input bytes, then aggregate semantic geometry across ALL LODs
    // of every supplied asset (counting an input asset once, not per instance).
    std::size_t sourceByteCapacity = 0;
    std::uint32_t sourceVertexCapacity = 0, sourceIndexCapacity = 0;
    std::uint32_t sourceSurfaceCapacity = 0;
    // Aggregate instantiated LOD0 draw capacities. Checked before any scene
    // parser or vertex/index/span allocation is invoked.
    std::uint32_t vertexCapacity = 0, indexCapacity = 0, spanCapacity = 0;
};
struct StaticDrawAssetsInput50DW {
    const StaticDrawAssetInput50DW* assets = nullptr;
    std::uint32_t assetCount = 0;
    const StaticDrawInstance50DW* instances = nullptr;
    std::uint32_t instanceCount = 0;
    StaticDrawBudgets50DW budgets{};
};
struct StaticDrawSpanSource50DW {
    // Original scene material index remains in the corresponding SceneSpan48D.
    // The pair (asset, material_index) identifies its material; no global
    // material-number remapping, deduplication or sorting is performed.
    std::uint32_t asset = 0, instance = 0, surface = 0;
};
struct StaticDrawView50DW {
    const Vertex48B* vertices = nullptr;
    std::uint32_t vertexCount = 0;
    const std::uint32_t* indices = nullptr;
    std::uint32_t indexCount = 0;
    const SceneSpan48D* spans = nullptr;
    const StaticDrawSpanSource50DW* spanSources = nullptr;
    std::uint32_t spanCount = 0;
    Bounds48B bounds{}; // Computed display bounds, never collision bounds.
};
enum class StaticDrawAssetError50DW : std::uint8_t {
    none, alreadyLoaded, input, budget, serializedScene, unsupportedScene,
    matrix, roundingMode, scene, compatibilityMesh, arithmetic, allocation,
    unexpectedFailure
};
struct StaticDrawAssetDiagnostic50DW {
    StaticDrawAssetError50DW error = StaticDrawAssetError50DW::none;
    // Zero-based rejected location; fields not relevant to an error are zero.
    std::uint32_t asset = 0, instance = 0, surface = 0, vertex = 0, lane = 0;
    std::size_t byteOffset = 0;
    std::string parserMessage;
};

// Cold load-once owner. Admission is the recovered static single-root subset:
// one root, no matrix palette, every semantic bone zero, one/four source LODs,
// and identity placement for world-quantized geometry. Multi-node/palette and
// nonzero-bone inputs reject. Caller supplies only resolved static visual
// instances: animation provenance is not encoded by RRSCENE1 and cannot be
// inferred here. Unsupported inputs are never converted to stand-ins.
//
// Each declared allocation count is scanned from bounded serialized bytes
// before parsing; aggregate source and instantiated output budgets are checked
// first. Existing parsers then validate full scene/embedded-mesh agreement.
// Failure leaves an unloaded owner retryable. Success commits all instances
// together. An already-loaded owner rejects without invalidating its views.
class StaticDrawAssets50DW final {
public:
    StaticDrawAssets50DW() noexcept;
    ~StaticDrawAssets50DW();
    StaticDrawAssets50DW(const StaticDrawAssets50DW&) = delete;
    StaticDrawAssets50DW& operator=(const StaticDrawAssets50DW&) = delete;
    StaticDrawAssets50DW(StaticDrawAssets50DW&&) = delete;
    StaticDrawAssets50DW& operator=(StaticDrawAssets50DW&&) = delete;

    // Serial, cold call. Inputs, backing arrays and diagnostic must be distinct
    // live storage with stable contents/extents. Requires FE_TONEAREST and
    // strict IEEE binary64/binary32; no fast math or contraction. Preserves
    // instance order, surface order, triangles (including degenerates), UVs
    // and compatibility color bytes. Only positions and global offsets change.
    bool load(const StaticDrawAssetsInput50DW&, StaticDrawAssetDiagnostic50DW&) noexcept;

    bool loaded() const noexcept;
    // Allocation-free, immutable borrowed access. Null when unloaded/out of
    // range. Keep this owner alive until all draws/uploads/readers finish.
    // Positions are already world-space; renderer must use baseVertex=0 and
    // must not apply a saved root or instance matrix again.
    const StaticDrawView50DW* draw() const noexcept;
    const Scene48D* scene(std::uint32_t asset) const noexcept;
    const Mesh48B* compatibilityMesh(std::uint32_t asset) const noexcept;
    const StaticDrawInstance50DW* instance(std::uint32_t instance) const noexcept;
    std::uint32_t assetCount() const noexcept;
    std::uint32_t instanceCount() const noexcept;

private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
