#pragma once
#include "vehicle_draw50cm.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Original racer specular projection requires strict finite IEEE arithmetic"
#endif

namespace rr::game {
struct OriginalRacerSpecularRequest50LA {
    // Actual current CameraViewOutput49F view/position.
    // Native converted view-projection is not a replacement for this view.
    std::array<float, 16> view{};
    std::array<float, 3> cameraPosition{};
};
struct OriginalRacerSpecularResult50LA {
    float distance1BC = 0;
    bool secondEnabled = false;
};
struct OriginalRacerSpecularWorkspace50LA {
    // 64 KiB UV stream plus a bounded 256-entry per-call normal cache.
    // Retain caller-owned storage
    // rather than introducing a large hidden presentation-stack allocation.
    std::array<std::array<float, 2>, maxRigidVertices49S> map2Uv;
    vehicle_draw_detail50LA::NormalResultCache<std::array<float, 2>> normalCache;
};

namespace original_racer_specular_detail50LA {
inline bool affine(const float* matrix) noexcept {
    for (unsigned lane = 0; lane < 16; ++lane)
        if (!std::isfinite(matrix[lane])) return false;
    return matrix[3] == 0.f && matrix[7] == 0.f && matrix[11] == 0.f && matrix[15] == 1.f;
}
}

// Bounded source arithmetic only. The renderer must join material, selected
// lighting and retained-geometry identities before consuming this result.
// This function never creates an owner, publication, material or camera.
// Recompute for EVERY presentation, including retained stopped-racer geometry.
// No camera pointer is stored in the submission or workspace.
//
// Requires the immutable prepared semantic mesh, the matching successful
// sourceSpecularGeometry50LA submission and the actual current camera. Only
// the original multi-node aggregate-center160 route is admitted; no sphere194,
// render AABB or estimated root-origin distance route is substituted.
// UV follows rootWorld * view, then qNormal-scaled XYZ rows and raw signed-byte
// normal dots, using projectSpecularMapDualUv's separate binary32 operations.
// There is no inverse transpose, normalization, reflection, UV clamp or Y flip.
// The actual model owner supplies its just-prepared26F070 distance and241280
// map-2 flag from this same camera. The UV leaf neither recomputes that distance
// nor substitutes a launch-subtype constant for the shared382FA0 owner.
// Callers still own source group/pass order and fragment state.
//
// Serial live nonoverlapping mesh/backing arrays, submission, request, UV
// destination, workspace and result are caller preconditions. Strict finite
// binary32 FE_TONEAREST, no contraction/fast math, no allocation or callbacks.
// Failure preserves destination/result; workspace may change. All fallible
// work finishes before publishing any UV. No graph, AI, pause or RNG mutation.
inline bool prepareOriginalRacerSpecular50LA(const VehicleDrawMesh50CM& mesh,
    const VehicleDrawSubmission50CM& submission, const OriginalRacerSpecularRequest50LA& request,
    const OriginalRacerModelDistanceFlags50LA& modelView,
    std::array<float, 2>* map2Uv, std::uint32_t capacity,
    OriginalRacerSpecularWorkspace50LA& workspace, OriginalRacerSpecularResult50LA& output) noexcept {
    namespace light = original_racer_lighting50KM;
    namespace detail = original_racer_specular_detail50LA;
    if (!light::selection_detail::arithmeticReady() || !map2Uv || !mesh.semanticScene ||
        !mesh.vertexCount || mesh.vertexCount > maxRigidVertices49S || capacity < mesh.vertexCount ||
        !mesh.spans || !mesh.spanCount || mesh.spanCount > maxVehicleDrawSpans50CM ||
        mesh.sourceNodeCount <= 1 || mesh.sourceNodeCount > maxRigidPalette49S ||
        !submission.sourceSpecularGeometry50LA || !submission.vertices ||
        submission.vertexCount != mesh.vertexCount || submission.indices != mesh.indices ||
        submission.indexCount != mesh.indexCount || submission.spans != mesh.spans ||
        submission.spanCount != mesh.spanCount ||
        !detail::affine(submission.sourceRootWorld50LA.v) || !detail::affine(request.view.data()))
        return false;
    const auto& scene = *mesh.semanticScene;
    if (scene.world_space || !scene.identity_matrix_palette || scene.lods.empty() ||
        scene.nodes.size() != mesh.sourceNodeCount || scene.lods[0].surfaces.size() != mesh.spanCount)
        return false;
    const double authoredQ = scene.quantization[1];
    if (!std::isfinite(authoredQ) || authoredQ <= 0.0 ||
        authoredQ > static_cast<double>(std::numeric_limits<float>::max())) return false;
    const float normalQ = static_cast<float>(authoredQ);
    if (static_cast<double>(normalQ) != authoredQ) return false;
    const auto* compressedNormals = mesh.compressedNormals50LA &&
        mesh.compressedNormalScene50LA == &scene &&
        mesh.compressedNormalCount50LA == mesh.vertexCount &&
        mesh.compressedNormalQ50LA == authoredQ ? mesh.compressedNormals50LA : nullptr;

    if (!std::isfinite(modelView.distance1BC) || modelView.distance1BC < 0.f ||
        (modelView.flags20 & ~0x0202u)) return false;
    OriginalRacerSpecularResult50LA next{};
    next.distance1BC = modelView.distance1BC;
    next.secondEnabled = modelView.secondEnabled();

    light::SpecularMapDualInputs coefficients{};
    const auto& root = submission.sourceRootWorld50LA;
    for (unsigned axis = 0; axis < 3; ++axis)
        for (unsigned lane = 0; lane < 2; ++lane) {
            // Original row-vector model/rootWorld * view composition. Keep
            // the fourth zero term and every binary32 multiply/add separate.
            float value = light::material_detail::mul(request.view[lane], root.v[axis * 4]);
            if (!std::isfinite(value)) return false;
            for (unsigned term = 1; term < 4; ++term) {
                const float product = light::material_detail::mul(request.view[term * 4 + lane],
                    root.v[axis * 4 + term]);
                if (!std::isfinite(product)) return false;
                value = light::material_detail::add(value, product);
                if (!std::isfinite(value)) return false;
            }
            //32C450..32C460 scales the composed rows by authored qNormal.
            coefficients.cameraNormalAxes[axis][lane] = light::material_detail::mul(value, normalQ);
            if (!std::isfinite(coefficients.cameraNormalAxes[axis][lane])) return false;
        }

    // Coefficients are uniform across this whole model and current camera.
    // Keep all per-vertex source/bone validation, memoizing only exact normals.
    workspace.normalCache.clear();
    std::uint32_t vertex = 0;
    for (std::uint32_t s = 0; s < mesh.spanCount; ++s) {
        const auto& surface = scene.lods[0].surfaces[s];
        const auto& span = mesh.spans[s];
        if (surface.material_index >= scene.materials.size() ||
            span.material_index != surface.material_index || span.vertex_start != vertex ||
            span.vertex_count != surface.vertices.size() ||
            surface.vertices.size() > mesh.vertexCount - vertex) return false;
        for (const auto& source : surface.vertices) {
            if (source.bone >= mesh.sourceNodeCount) return false;
            light::CompressedNormal normal{};
            if (compressedNormals) {
                if (!compressedNormals[vertex].valid) return false;
                normal = compressedNormals[vertex].value;
            } else if (!vehicle_draw_detail50LA::decodeCompressedNormal(
                source.normal_local, authoredQ, normal)) return false;
            // Arithmetic mode and all six coefficients were admitted above.
            const auto key = vehicle_draw_detail50LA::normalKey(normal);
            if (!workspace.normalCache.find(key, workspace.map2Uv[vertex])) {
                if (light::material_detail::projectSpecularMapDualUvFiniteInputs(coefficients, normal,
                    workspace.map2Uv[vertex]) != light::Status::complete) return false;
                workspace.normalCache.insert(key, workspace.map2Uv[vertex]);
            }
            ++vertex;
        }
    }
    if (vertex != mesh.vertexCount) return false;
    for (std::uint32_t n = 0; n < vertex; ++n) map2Uv[n] = workspace.map2Uv[n];
    output = next;
    return true;
}
// GPU vertex path: identical admission and per-model coefficients as
// prepareOriginalRacerSpecular50LA, without the per-vertex UV loop. The
// vertex shader evaluates 0.5 * (coefficients . normal) + 0.5 per vertex.
inline bool prepareOriginalRacerSpecularCoefficients50LA(const VehicleDrawMesh50CM& mesh,
    const VehicleDrawSubmission50CM& submission, const OriginalRacerSpecularRequest50LA& request,
    const OriginalRacerModelDistanceFlags50LA& modelView,
    original_racer_lighting50KM::SpecularMapDualInputs& coefficients,
    OriginalRacerSpecularResult50LA& output) noexcept {
    namespace light = original_racer_lighting50KM;
    namespace detail = original_racer_specular_detail50LA;
    if (!light::selection_detail::arithmeticReady() || !mesh.semanticScene ||
        !mesh.vertexCount || mesh.vertexCount > maxRigidVertices49S ||
        !mesh.spans || !mesh.spanCount || mesh.spanCount > maxVehicleDrawSpans50CM ||
        mesh.sourceNodeCount <= 1 || mesh.sourceNodeCount > maxRigidPalette49S ||
        !submission.sourceSpecularGeometry50LA || !submission.skin ||
        submission.vertexCount != mesh.vertexCount || submission.indices != mesh.indices ||
        submission.indexCount != mesh.indexCount || submission.spans != mesh.spans ||
        submission.spanCount != mesh.spanCount ||
        !detail::affine(submission.sourceRootWorld50LA.v) || !detail::affine(request.view.data()))
        return false;
    const auto& scene = *mesh.semanticScene;
    if (scene.world_space || !scene.identity_matrix_palette || scene.lods.empty() ||
        scene.nodes.size() != mesh.sourceNodeCount || scene.lods[0].surfaces.size() != mesh.spanCount)
        return false;
    const double authoredQ = scene.quantization[1];
    if (!std::isfinite(authoredQ) || authoredQ <= 0.0 ||
        authoredQ > static_cast<double>(std::numeric_limits<float>::max())) return false;
    const float normalQ = static_cast<float>(authoredQ);
    if (static_cast<double>(normalQ) != authoredQ) return false;
    if (!std::isfinite(modelView.distance1BC) || modelView.distance1BC < 0.f ||
        (modelView.flags20 & ~0x0202u)) return false;
    OriginalRacerSpecularResult50LA next{};
    next.distance1BC = modelView.distance1BC;
    next.secondEnabled = modelView.secondEnabled();
    light::SpecularMapDualInputs result{};
    const auto& root = submission.sourceRootWorld50LA;
    for (unsigned axis = 0; axis < 3; ++axis)
        for (unsigned lane = 0; lane < 2; ++lane) {
            float value = light::material_detail::mul(request.view[lane], root.v[axis * 4]);
            if (!std::isfinite(value)) return false;
            for (unsigned term = 1; term < 4; ++term) {
                const float product = light::material_detail::mul(request.view[term * 4 + lane],
                    root.v[axis * 4 + term]);
                if (!std::isfinite(product)) return false;
                value = light::material_detail::add(value, product);
                if (!std::isfinite(value)) return false;
            }
            result.cameraNormalAxes[axis][lane] = light::material_detail::mul(value, normalQ);
            if (!std::isfinite(result.cameraNormalAxes[axis][lane])) return false;
        }
    coefficients = result;
    output = next;
    return true;
}
} // namespace rr::game
