#include "vehicle_draw50cm.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Vehicle draw projection requires strict finite IEEE arithmetic"
#endif

namespace rr::game {
namespace {
bool reject(VehicleDrawDiagnostic50CM& d, VehicleDrawError50CM error,
    std::uint32_t item = 0) noexcept {
    d.error = error;
    d.item = item;
    return false;
}
bool indexValid(SceneNodeIndex49P index,std::uint32_t count) noexcept {
    const auto n = static_cast<std::int32_t>(index);
    return n >= 0 && static_cast<std::uint32_t>(n) < count;
}
bool belongsTo(const SceneNode49P* nodes, SceneNodeIndex49P node,
    SceneNodeIndex49P root,std::uint32_t count) noexcept {
    for (std::uint32_t depth = 0; depth < count; ++depth) {
        if (!indexValid(node,count)) return false;
        if (node == root) return true;
        node = nodes[static_cast<std::uint32_t>(node)].parent180;
    }
    return false;
}
bool model(const VehicleFrameState49W& state, const VehicleFrameBindings49W& bindings,
    VehicleDrawComponent50CM component, SceneNodeIndex49P& root, std::uint8_t& flags) noexcept {
    switch (component) {
    case VehicleDrawComponent50CM::chariot:
        root = bindings.lifecycle.model1B4;
        flags = state.fields.chariot191;
        return root == bindings.animation.chariot1B4;
    case VehicleDrawComponent50CM::leftEngine:
        root = bindings.visual.left1BC;
        flags = state.fields.left191;
        return root == bindings.lifecycle.left1BC && root == bindings.animation.left1BC;
    case VehicleDrawComponent50CM::rightEngine:
        root = bindings.visual.right1B8;
        flags = state.fields.right191;
        return root == bindings.lifecycle.right1B8 && root == bindings.animation.right1B8;
    }
    return false;
}
}

bool prepareVehicleDrawMesh50CM(const assets::Scene48D& scene, const assets::Mesh48B& mesh,
    RigidVertex49S* local, std::uint32_t capacity,
    VehicleDrawMesh50CM& output, VehicleDrawDiagnostic50CM& d) noexcept {
    d = {};
    if (!local || mesh.vertices.empty() || mesh.vertices.size() > maxRigidVertices49S ||
        capacity < mesh.vertices.size()) return reject(d, VehicleDrawError50CM::storage);
    if (std::fegetround() != FE_TONEAREST || scene.world_space || !scene.identity_matrix_palette ||
        scene.nodes.empty() || scene.nodes.size() > maxRigidPalette49S || scene.lods.empty())
        return reject(d, VehicleDrawError50CM::scene);
    const auto& surfaces = scene.lods[0].surfaces;
    if (surfaces.empty() || surfaces.size() > maxVehicleDrawSpans50CM ||
        scene.spans_lod0.size() != surfaces.size() || mesh.indices.empty() ||
        mesh.indices.size() > assets::max_mesh_indices)
        return reject(d, VehicleDrawError50CM::geometry);

    std::uint32_t vertex = 0, index = 0;
    for (std::uint32_t s = 0; s < surfaces.size(); ++s) {
        const auto& surface = surfaces[s];
        const auto& span = scene.spans_lod0[s];
        if (surface.vertices.size() > mesh.vertices.size() - vertex ||
            surface.indices.size() > mesh.indices.size() - index || surface.indices.size() % 3 ||
            surface.material_index >= scene.materials.size() ||
            span.material_index != surface.material_index ||
            span.has_original_colors != surface.has_original_colors ||
            span.vertex_start != vertex || span.vertex_count != surface.vertices.size() ||
            span.index_start != index || span.index_count != surface.indices.size())
            return reject(d, VehicleDrawError50CM::geometry, s);
        for (std::uint32_t n = 0; n < surface.vertices.size(); ++n) {
            const auto& source = surface.vertices[n];
            const auto& target = mesh.vertices[vertex + n];
            if (source.bone >= scene.nodes.size()) return reject(d, VehicleDrawError50CM::geometry, s);
            for (const double value : source.position_local)
                if (!std::isfinite(value)) return reject(d, VehicleDrawError50CM::geometry, s);
            for (const double value : source.normal_local)
                if (!std::isfinite(value)) return reject(d, VehicleDrawError50CM::normals, s);
            for (std::uint32_t lane = 0; lane < 2; ++lane) {
                const double uv = source.uv[lane];
                const double limit = static_cast<double>(std::numeric_limits<float>::max());
                if (!std::isfinite(uv) || uv < -limit || uv > limit)
                    return reject(d, VehicleDrawError50CM::vertexTemplate, s);
                const float expected = static_cast<float>(uv);
                if (std::memcmp(&expected, &target.uv[lane], sizeof(float)))
                    return reject(d, VehicleDrawError50CM::vertexTemplate, s);
            }
            for (std::uint32_t lane = 0; lane < 4; ++lane)
                if (target.color[lane] != (surface.has_original_colors ? source.color[lane] : 255u))
                    return reject(d, VehicleDrawError50CM::vertexTemplate, s);
        }
        for (std::uint32_t n = 0; n < surface.indices.size(); ++n)
            if (surface.indices[n] >= surface.vertices.size() ||
                mesh.indices[index + n] != vertex + surface.indices[n])
                return reject(d, VehicleDrawError50CM::geometry, s);
        vertex += static_cast<std::uint32_t>(surface.vertices.size());
        index += static_cast<std::uint32_t>(surface.indices.size());
    }
    if (vertex != mesh.vertices.size() || index != mesh.indices.size())
        return reject(d, VehicleDrawError50CM::geometry);

    vertex = 0;
    for (const auto& surface : surfaces) for (const auto& source : surface.vertices) {
        auto& target = local[vertex++];
        for (std::uint32_t lane = 0; lane < 3; ++lane) target.positionLocal[lane] = source.position_local[lane];
        target.paletteIndex = source.bone;
    }
    output = {local, mesh.vertices.data(), vertex, static_cast<std::uint32_t>(scene.nodes.size()),
        mesh.indices.data(), index, scene.spans_lod0.data(), static_cast<std::uint32_t>(surfaces.size())};
    output.semanticScene = &scene;
    return true;
}

bool buildVehicleDraw50CM(const VehicleFrameState49W& state, const VehicleFrameBindings49W& bindings,
    const VehicleDrawSelection50CM& selection, const VehicleDrawMesh50CM& mesh,
    VehicleDrawBuffer50CM destination, VehicleDrawSubmission50CM& output,
    VehicleDrawWorkspace50CM& workspace, VehicleDrawDiagnostic50CM& d) noexcept {
    d = {};
    if (!destination.vertices || !mesh.localVertices || !mesh.vertexTemplate || !mesh.indices ||
        !mesh.spans || !mesh.vertexCount || mesh.vertexCount > maxRigidVertices49S ||
        destination.capacity < mesh.vertexCount || !mesh.indexCount ||
        mesh.indexCount > assets::max_mesh_indices || !mesh.spanCount ||
        mesh.spanCount > maxVehicleDrawSpans50CM)
        return reject(d, VehicleDrawError50CM::storage);
    if (bool(destination.sourceNormals) != bool(destination.normalCapacity) ||
        (destination.sourceNormals && (destination.normalCapacity < mesh.vertexCount ||
            !mesh.semanticScene || mesh.semanticScene->lods.empty())))
        return reject(d, VehicleDrawError50CM::storage);
    if ((state.phase != VehicleFramePhase49W::Ready && state.phase != VehicleFramePhase49W::PostGraphics) ||
        selection.phase != ModelPalettePhase49S::selectedRenderPreparation ||
        selection.suppliedState != ModelPaletteSceneState49S::completeCurrentScene)
        return reject(d, VehicleDrawError50CM::phase);
    SceneNodeIndex49P root = SceneNodeIndex49P::none;
    std::uint8_t flags = 0;
    if (!validVehicleSceneCount49W(state,bindings)||!model(state, bindings, selection.component, root, flags) || !indexValid(root,state.sceneNodeCount) ||
        !selection.sourceNodes || !selection.paletteIds || !mesh.sourceNodeCount ||
        mesh.sourceNodeCount > maxRigidPalette49S || selection.sourceNodeCount != mesh.sourceNodeCount ||
        selection.paletteCount != mesh.sourceNodeCount)
        return reject(d, VehicleDrawError50CM::binding);
    for (std::uint32_t i = 0; i < selection.paletteCount; ++i)
        if (selection.paletteIds[i] != i || !belongsTo(state.nodes, selection.sourceNodes[i], root,state.sceneNodeCount))
            return reject(d, VehicleDrawError50CM::binding, i);

    std::memcpy(workspace.nodes, state.nodes, sizeof(workspace.nodes));
    const SceneGraph49P graph{workspace.nodes, state.sceneNodeCount};
    const ModelPaletteInput49S paletteInput{selection.phase, selection.suppliedState, flags, root,
        selection.sourceNodes, selection.sourceNodeCount, selection.paletteIds, selection.paletteCount};
    if (!projectModelPalette49S(graph, paletteInput, workspace.palette, workspace.paletteWorkspace, d.palette))
        return reject(d, VehicleDrawError50CM::palette);
    if (!projectRigidVertices49S({mesh.localVertices, mesh.vertexCount},
        {workspace.palette.nodeWorld, workspace.palette.entryCount},
        {workspace.positions, maxRigidVertices49S}, d.vertices))
        return reject(d, VehicleDrawError50CM::vertices);

    // Use the exact current per-node palette already used for positions. The
    // inverse transpose preserves authored hard edges under scale and shear.
    // Finish all fallible work before publishing either output stream.
    if (destination.sourceNormals) {
        std::array<std::array<double, 16>, maxRigidPalette49S> matrices{};
        for (std::uint32_t bone = 0; bone < workspace.palette.entryCount; ++bone)
            for (std::uint32_t lane = 0; lane < 16; ++lane)
                matrices[bone][lane] = workspace.palette.nodeWorld[bone].v[lane];
        std::uint32_t vertex = 0;
        for (const auto& surface : mesh.semanticScene->lods[0].surfaces)
            for (const auto& source : surface.vertices) {
                if (vertex >= mesh.vertexCount || source.bone >= workspace.palette.entryCount)
                    return reject(d, VehicleDrawError50CM::normals, vertex);
                // An explicitly absent source normal stays absent even on a
                // collapsed effect node; the shader has a geometric fallback.
                if (source.normal_local[0] == 0.0 && source.normal_local[1] == 0.0 &&
                    source.normal_local[2] == 0.0) workspace.normals[vertex] = {};
                else if (!assets::transformWorldNormal50HK(source.normal_local,
                    matrices[source.bone], workspace.normals[vertex]))
                    // A collapsed/degenerate node has no inverse transpose.
                    // Keep valid geometry drawable with the shader's geometric
                    // fallback rather than rejecting an entire race frame.
                    workspace.normals[vertex] = {};
                ++vertex;
            }
        if (vertex != mesh.vertexCount)
            return reject(d, VehicleDrawError50CM::normals, vertex);
    }

    VehicleDrawSubmission50CM next{destination.vertices, mesh.vertexCount, mesh.indices,
        mesh.indexCount, mesh.spans, mesh.spanCount, {}};
    for (std::uint32_t axis = 0; axis < 3; ++axis)
        next.bounds.minimum[axis] = next.bounds.maximum[axis] = workspace.positions[0].v[axis];
    for (std::uint32_t n = 0; n < mesh.vertexCount; ++n) {
        std::memcpy(destination.vertices + n, mesh.vertexTemplate + n, sizeof(assets::Vertex48B));
        for (std::uint32_t axis = 0; axis < 3; ++axis) {
            const float value = workspace.positions[n].v[axis];
            destination.vertices[n].position[axis] = value;
            if (value < next.bounds.minimum[axis]) next.bounds.minimum[axis] = value;
            if (value > next.bounds.maximum[axis]) next.bounds.maximum[axis] = value;
        }
    }
    if (destination.sourceNormals) {
        std::memcpy(destination.sourceNormals, workspace.normals,
            std::size_t(mesh.vertexCount) * sizeof(assets::WorldNormal50HK));
        next.sourceNormals = destination.sourceNormals;
        next.sourceNormalCount = mesh.vertexCount;
    }
    output = next;
    return true;
}
}
