#include "vehicle_draw50cm.h"
#include "model_bounds49o.h"
#include "native_gc_selected_lights_candidate.h"
#include "original_racer_model_packets50la.h"
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
// Same admission, diagnostics and ordered arithmetic as rigid_vertices49s.cpp.
// Only the transaction boundary differs: these positions are private disposable
// draw scratch, so retain the first checked result instead of projecting twice.
// Keep the public49S projector transactional for its independent callers.
bool projectDrawPositions(RigidVertexView49S input, RigidPaletteView49S palette,
    RigidPositionView49S output, RigidVertexDiagnostic49S& d) noexcept {
    static_assert(sizeof(double) == 8 && std::numeric_limits<double>::is_iec559,
        "IEEE binary64 required");
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
        "IEEE binary32 required");
    const auto rejectPosition = [&](RigidVertexError49S error, std::uint32_t vertex = 0,
        std::uint32_t matrix = 0, std::uint32_t lane = 0) noexcept {
        d = {error, vertex, matrix, lane};
        return false;
    };
    d = {RigidVertexError49S::none, 0, 0, 0};
    if (!input.vertices || !palette.matrices || !output.positions)
        return rejectPosition(RigidVertexError49S::invalid_view);
    if (!input.count || input.count > maxRigidVertices49S || !palette.count ||
        palette.count > maxRigidPalette49S || output.capacity > maxRigidVertices49S)
        return rejectPosition(RigidVertexError49S::count_limit);
    if (output.capacity < input.count)
        return rejectPosition(RigidVertexError49S::output_capacity);
    if (std::fegetround() != FE_TONEAREST)
        return rejectPosition(RigidVertexError49S::rounding_mode);
    for (std::uint32_t i = 0; i < palette.count; ++i) {
        const auto& m = palette.matrices[i];
        for (std::uint32_t lane = 0; lane < 16; ++lane)
            if (!std::isfinite(m.v[lane]))
                return rejectPosition(RigidVertexError49S::matrix_nonfinite, 0, i, lane);
        for (std::uint32_t lane = 3; lane < 16; lane += 4)
            if (m.v[lane] != (lane == 15 ? 1.0f : 0.0f))
                return rejectPosition(RigidVertexError49S::matrix_nonaffine, 0, i, lane);
    }
    for (std::uint32_t i = 0; i < input.count; ++i) {
        const auto& v = input.vertices[i];
        if (v.paletteIndex >= palette.count)
            return rejectPosition(RigidVertexError49S::palette_index, i, v.paletteIndex);
        for (std::uint32_t lane = 0; lane < 3; ++lane)
            if (!std::isfinite(v.positionLocal[lane]))
                return rejectPosition(RigidVertexError49S::position_nonfinite, i, v.paletteIndex, lane);
        const auto& matrix = palette.matrices[v.paletteIndex];
        RigidPosition49S position{};
        for (std::uint32_t axis = 0; axis < 3; ++axis) {
            double sum = 0;
            for (std::uint32_t k = 0; k < 3; ++k) {
                const double product = v.positionLocal[k] * static_cast<double>(matrix.v[k * 4 + axis]);
                if (!std::isfinite(product))
                    return rejectPosition(RigidVertexError49S::arithmetic_nonfinite, i, v.paletteIndex, axis);
                sum = sum + product;
                if (!std::isfinite(sum))
                    return rejectPosition(RigidVertexError49S::arithmetic_nonfinite, i, v.paletteIndex, axis);
            }
            const double value = sum + static_cast<double>(matrix.v[12 + axis]);
            if (!std::isfinite(value))
                return rejectPosition(RigidVertexError49S::arithmetic_nonfinite, i, v.paletteIndex, axis);
            const double maximum = static_cast<double>(std::numeric_limits<float>::max());
            if (value > maximum || value < -maximum)
                return rejectPosition(RigidVertexError49S::output_range, i, v.paletteIndex, axis);
            position.v[axis] = static_cast<float>(value);
        }
        output.positions[i] = position;
    }
    return true;
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
    case VehicleDrawComponent50CM::bottomLeftEngine:
        root = bindings.visual.bottomLeft1C4;
        flags = state.fields.bottomLeft191;
        return root != SceneNodeIndex49P::none;
    case VehicleDrawComponent50CM::bottomRightEngine:
        root = bindings.visual.bottomRight1C0;
        flags = state.fields.bottomRight191;
        return root != SceneNodeIndex49P::none;
    }
    return false;
}
bool prepareSourceSpecularGeometry(const SceneGraph49P& graph, SceneNodeIndex49P root,
    const VehicleDrawSelection50CM& selection, const VehicleDrawMesh50CM& mesh,
    const BodyMatrix48G& rootWorld, VehicleDrawSubmission50CM& next,
    VehicleDrawDiagnostic50CM& d) noexcept {
    if ((selection.component != VehicleDrawComponent50CM::leftEngine &&
         selection.component != VehicleDrawComponent50CM::rightEngine &&
         selection.component != VehicleDrawComponent50CM::bottomLeftEngine &&
         selection.component != VehicleDrawComponent50CM::bottomRightEngine) ||
        !mesh.semanticScene || selection.sourceNodeCount <= 1 ||
        selection.sourceNodeCount > maxModelBoundsNodes49O ||
        mesh.semanticScene->nodes.size() != selection.sourceNodeCount)
        return reject(d, VehicleDrawError50CM::sourceSpecular);
    const auto& owner = graph.nodes[static_cast<std::uint32_t>(root)];
    ModelBoundsContext49O context{};
    std::memcpy(&context.rootWorld100, &rootWorld, sizeof(BodyMatrix48G));
    context.rootSceneFlags190 = owner.sceneFlags190;
    // The additional model owner has no semantic authored box. Do not invent
    // one, infer a sphere194, or substitute renderer geometry bounds.
    if (context.rootSceneFlags190 & 0x14u)
        return reject(d, VehicleDrawError50CM::sourceSpecular);
    std::array<ModelBoundsNode49O, maxModelBoundsNodes49O> nodes{};
    for (std::uint32_t n = 0; n < selection.sourceNodeCount; ++n) {
        const auto& authored = mesh.semanticScene->nodes[n];
        const auto sourceIndex = selection.sourceNodes[n];
        if (!indexValid(sourceIndex, graph.count) || sourceIndex == root ||
            authored.parent < -1 || authored.parent >= static_cast<std::int32_t>(n))
            return reject(d, VehicleDrawError50CM::sourceSpecular, n);
        for (std::uint32_t prior = 0; prior < n; ++prior)
            if (selection.sourceNodes[prior] == sourceIndex)
                return reject(d, VehicleDrawError50CM::sourceSpecular, n);
        const auto& current = graph.nodes[static_cast<std::uint32_t>(sourceIndex)];
        const auto expectedParent = authored.parent < 0 ? root :
            selection.sourceNodes[static_cast<std::uint32_t>(authored.parent)];
        if (current.parent180 != expectedParent)
            return reject(d, VehicleDrawError50CM::sourceSpecular, n);
        auto& row = nodes[n];
        // Current post-motion locals/flags and original stored boxes share the
        // same verified source-index map. Never restore authored bind locals.
        std::memcpy(&row.local, &current.localC0, sizeof(BodyMatrix48G));
        row.parent = authored.parent;
        row.sceneFlags190 = current.sceneFlags190;
        for (unsigned lane = 0; lane < 6; ++lane) {
            const double value = authored.stored_bounds[lane];
            const double limit = static_cast<double>(std::numeric_limits<float>::max());
            if (!std::isfinite(value) || value < -limit || value > limit)
                return reject(d, VehicleDrawError50CM::sourceSpecular, n);
            const float exact = static_cast<float>(value);
            if (static_cast<double>(exact) != value || (lane >= 3 && exact < 0.f))
                return reject(d, VehicleDrawError50CM::sourceSpecular, n);
            if (lane < 3) row.center140.v[lane] = exact;
            else row.halfExtent150.v[lane - 3] = exact;
        }
    }
    ModelBoundsWorkspace49O scratch{};
    ModelBoundsResult49O bounds{};
    if (!computeModelBounds49O(nodes.data(), selection.sourceNodeCount,
        context, scratch, bounds) || !bounds.seeded)
        return reject(d, VehicleDrawError50CM::sourceSpecular);
    for (unsigned axis = 0; axis < 3; ++axis) {
        //26F124..26F14C reads model local F0/F4/F8, not root world position.
        const float position = owner.localC0.v[12 + axis];
        if (!std::isfinite(position))
            return reject(d, VehicleDrawError50CM::sourceSpecular);
        next.sourcePosition50LA[axis] = position;
    }
    for (unsigned lane = 0; lane < 4; ++lane) {
        next.sourceAggregateBounds50LA.center160[lane] = bounds.center160[lane];
        next.sourceAggregateBounds50LA.halfExtent170[lane] = bounds.halfExtent170[lane];
    }
    next.sourceAggregateBounds50LA.radius198 = bounds.radius198;
    std::memcpy(&next.sourceRootWorld50LA, &rootWorld, sizeof(BodyMatrix48G));
    next.sourceSpecularGeometry50LA = true;
    return true;
}
bool prepareSourceColors(const VehicleDrawSelection50CM& selection,
    const VehicleDrawMesh50CM& mesh, const VehicleDrawSourceColors50LA& input,
    const BodyMatrix48G* nodeWorld, const OriginalRacerModelPacket50LA* packet,
    VehicleDrawWorkspace50CM& workspace, VehicleDrawDiagnostic50CM& d) noexcept {
    namespace light = original_racer_lighting50KM;
    if ((selection.component != VehicleDrawComponent50CM::leftEngine &&
         selection.component != VehicleDrawComponent50CM::rightEngine &&
         selection.component != VehicleDrawComponent50CM::bottomLeftEngine &&
         selection.component != VehicleDrawComponent50CM::bottomRightEngine) ||
        !input.materialModes || !input.materialCount || !input.selectedLights ||
        !input.selectedLights->ready() || !mesh.semanticScene)
        return reject(d, VehicleDrawError50CM::sourceColors);
    const auto& scene = *mesh.semanticScene;
    if (input.materialCount != scene.materials.size() || scene.world_space ||
        !scene.identity_matrix_palette || scene.lods.empty() ||
        scene.nodes.size() != workspace.palette.entryCount ||
        scene.lods[0].surfaces.size() != mesh.spanCount)
        return reject(d, VehicleDrawError50CM::sourceColors);
    const auto* selected = input.selectedLights->component(
        static_cast<unsigned>(selection.component));
    if (!selected || !light::selection_detail::arithmeticReady())
        return reject(d, VehicleDrawError50CM::sourceColors);
    bool hasSelected = false;
    for (std::uint32_t material = 0; material < input.materialCount; ++material) {
        if (input.materialModes[material] > 1)
            return reject(d, VehicleDrawError50CM::sourceColors, material);
        hasSelected = hasSelected || input.materialModes[material] == 1;
    }
    if (!hasSelected) return reject(d, VehicleDrawError50CM::sourceColors);
    const double authoredQ = scene.quantization[1];
    if (!std::isfinite(authoredQ) || authoredQ <= 0.0 ||
        authoredQ > static_cast<double>(std::numeric_limits<float>::max()))
        return reject(d, VehicleDrawError50CM::sourceColors);
    const float normalQ = static_cast<float>(authoredQ);
    if (static_cast<double>(normalQ) != authoredQ)
        return reject(d, VehicleDrawError50CM::sourceColors);
    const auto* compressedNormals = mesh.compressedNormals50LA &&
        mesh.compressedNormalScene50LA == &scene &&
        mesh.compressedNormalCount50LA == mesh.vertexCount &&
        mesh.compressedNormalQ50LA == authoredQ ? mesh.compressedNormals50LA : nullptr;
    const auto direction = [&](unsigned row, unsigned lane) noexcept {
        return packet ? packet->matrixSlots[0].v[4u * row + lane] : selected->directions[row][lane];
    };
    const auto diffuse = [&](unsigned row, unsigned lane) noexcept {
        return packet ? packet->matrixSlots[1].v[4u * row + lane] : selected->diffuse[row][lane];
    };
    for (unsigned row = 0; row < 4; ++row)
        for (unsigned lane = 0; lane < 4; ++lane)
            if (!std::isfinite(direction(row, lane)) || !std::isfinite(diffuse(row, lane)))
                return reject(d, VehicleDrawError50CM::sourceColors);

    //3400D0..340140: qNormal scales selected world-direction rows BEFORE
    // the actual per-node basis. Keep every binary32 multiply/add separate,
    // including the fourth zero term. No inverse transpose or normalization.
    std::array<light::Vec3, 3> scaled{};
    for (unsigned axis = 0; axis < 3; ++axis)
        for (unsigned lane = 0; lane < 3; ++lane) {
            scaled[axis][lane] = light::material_detail::mul(direction(axis, lane), normalQ);
            if (!std::isfinite(scaled[axis][lane]))
                return reject(d, VehicleDrawError50CM::sourceColors);
        }
    for (std::uint32_t bone = 0; bone < workspace.palette.entryCount; ++bone) {
        const auto& world = nodeWorld[bone];
        for (float x : world.v) if (!std::isfinite(x))
            return reject(d, VehicleDrawError50CM::sourceColors, bone);
        if (world.v[3] != 0.f || world.v[7] != 0.f || world.v[11] != 0.f || world.v[15] != 1.f)
            return reject(d, VehicleDrawError50CM::sourceColors, bone);
        auto& prepared = workspace.sourceColorPalette50LA[bone];
        for (unsigned row = 0; row < 4; ++row)
            for (unsigned lane = 0; lane < 4; ++lane)
                prepared.colorRows[row][lane] = diffuse(row, lane);
        for (unsigned axis = 0; axis < 3; ++axis)
            for (unsigned lane = 0; lane < 3; ++lane) {
                float value = light::material_detail::mul(scaled[0][lane], world.v[axis * 4]);
                if (!std::isfinite(value)) return reject(d, VehicleDrawError50CM::sourceColors, bone);
                for (unsigned c = 1; c < 3; ++c) {
                    const float product = light::material_detail::mul(scaled[c][lane], world.v[axis * 4 + c]);
                    if (!std::isfinite(product)) return reject(d, VehicleDrawError50CM::sourceColors, bone);
                    value = light::material_detail::add(value, product);
                    if (!std::isfinite(value)) return reject(d, VehicleDrawError50CM::sourceColors, bone);
                }
                prepared.normalAxes[axis][lane] = light::material_detail::add(value,
                    light::material_detail::mul(0.f, world.v[axis * 4 + 3]));
                if (!std::isfinite(prepared.normalAxes[axis][lane]))
                    return reject(d, VehicleDrawError50CM::sourceColors, bone);
            }
    }
    workspace.sourceColorCache50LA.clear();
    std::uint32_t vertex = 0;
    for (std::uint32_t s = 0; s < mesh.spanCount; ++s) {
        const auto& surface = scene.lods[0].surfaces[s];
        const auto& span = mesh.spans[s];
        if (surface.material_index >= input.materialCount ||
            span.material_index != surface.material_index || span.vertex_start != vertex ||
            span.vertex_count != surface.vertices.size() ||
            surface.vertices.size() > mesh.vertexCount - vertex)
            return reject(d, VehicleDrawError50CM::sourceColors, s);
        const bool selectedSlot = input.materialModes[surface.material_index] == 1;
        for (const auto& source : surface.vertices) {
            auto& color = workspace.sourceColors50LA[vertex];
            if (selectedSlot) {
                if (source.bone >= workspace.palette.entryCount)
                    return reject(d, VehicleDrawError50CM::sourceColors, vertex);
                light::CompressedNormal normal{};
                if (compressedNormals) {
                    if (!compressedNormals[vertex].valid)
                        return reject(d, VehicleDrawError50CM::sourceColors, vertex);
                    normal = compressedNormals[vertex].value;
                } else if (!vehicle_draw_detail50LA::decodeCompressedNormal(
                    source.normal_local, authoredQ, normal))
                    return reject(d, VehicleDrawError50CM::sourceColors, vertex);
                // Arithmetic mode and all palette coefficients were admitted above.
                static_assert(maxRigidPalette49S <= 256, "normal cache key must retain every bone");
                const auto key = vehicle_draw_detail50LA::normalKey(normal, static_cast<std::uint8_t>(source.bone));
                if (!workspace.sourceColorCache50LA.find(key, color)) {
                    if (light::material_detail::projectDiffuseCompressedRgbaFiniteInputs(workspace.sourceColorPalette50LA[source.bone],
                        normal, color) != light::Status::complete)
                        return reject(d, VehicleDrawError50CM::sourceColors, vertex);
                    workspace.sourceColorCache50LA.insert(key, color);
                }
            } else {
                // Unselected slots retain all four exact compatibility bytes.
                for (unsigned c = 0; c < 4; ++c) color.rgba[c] = mesh.vertexTemplate[vertex].color[c];
            }
            ++vertex;
        }
    }
    if (vertex != mesh.vertexCount)
        return reject(d, VehicleDrawError50CM::sourceColors, vertex);
    return true;
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
    VehicleDrawWorkspace50CM& workspace, VehicleDrawDiagnostic50CM& d,
    const VehicleDrawSourceColors50LA* sourceColors) noexcept {
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

    // The admitted scene count bounds every graph consumer below. Copy the
    // complete current scene, without copying unused capacity for each pod
    // component. Retain a private snapshot: palette getters mutate its caches.
    std::memcpy(workspace.nodes, state.nodes,
        state.sceneNodeCount * sizeof(SceneNode49P));
    const SceneGraph49P graph{workspace.nodes, state.sceneNodeCount};
    const ModelPaletteInput49S paletteInput{selection.phase, selection.suppliedState, flags, root,
        selection.sourceNodes, selection.sourceNodeCount, selection.paletteIds, selection.paletteCount};
    if (!projectModelPalette49S(graph, paletteInput, workspace.palette, workspace.paletteWorkspace, d.palette))
        return reject(d, VehicleDrawError50CM::palette);
    const OriginalRacerModelPacket50LA* packet = nullptr;
    const BodyMatrix48G* nodeWorld = workspace.palette.nodeWorld;
    const BodyMatrix48G* rootWorld = &workspace.palette.rootWorld;
    if (sourceColors && sourceColors->modelPackets50LA) {
        const auto* lights = sourceColors->selectedLights ? sourceColors->selectedLights->component(
            static_cast<unsigned>(selection.component)) : nullptr;
        if (!lights || !sourceColors->modelPackets50LA->preparePaletteMatrices241280(
            paletteInput, workspace.palette, *lights).okay())
            return reject(d, VehicleDrawError50CM::palette);
        packet = sourceColors->modelPackets50LA->packet(0);
        rootWorld = &packet->matrixSlots[packet->modelMatrixIndex()];
        nodeWorld = rootWorld + 1;
    }
    if (!projectDrawPositions({mesh.localVertices, mesh.vertexCount},
        {nodeWorld, workspace.palette.entryCount},
        {workspace.positions, maxRigidVertices49S}, d.vertices))
        return reject(d, VehicleDrawError50CM::vertices);
    if (sourceColors && !prepareSourceColors(selection, mesh, *sourceColors, nodeWorld, packet, workspace, d))
        return false;

    // Use the exact current per-node palette already used for positions. The
    // inverse transpose preserves authored hard edges under scale and shear.
    // Finish all fallible work before publishing either output stream.
    if (destination.sourceNormals) {
        std::array<std::array<double, 16>, maxRigidPalette49S> matrices{};
        for (std::uint32_t bone = 0; bone < workspace.palette.entryCount; ++bone)
            for (std::uint32_t lane = 0; lane < 16; ++lane)
                matrices[bone][lane] = nodeWorld[bone].v[lane];
        // Matrix-only cofactors are shared by every vertex bound to the bone.
        // Rebuild per draw from this current palette; never reuse across frames.
        std::array<assets::WorldNormalTransform50HK, maxRigidPalette49S> normalTransforms{};
        std::array<bool, maxRigidPalette49S> normalTransformReady{};
        std::array<bool, maxRigidPalette49S> normalTransformAttempted{};
        std::uint32_t vertex = 0;
        for (const auto& surface : mesh.semanticScene->lods[0].surfaces)
            for (const auto& source : surface.vertices) {
                if (vertex >= mesh.vertexCount || source.bone >= workspace.palette.entryCount)
                    return reject(d, VehicleDrawError50CM::normals, vertex);
                // An explicitly absent source normal stays absent even on a
                // collapsed effect node; the shader has a geometric fallback.
                if (source.normal_local[0] == 0.0 && source.normal_local[1] == 0.0 &&
                    source.normal_local[2] == 0.0) workspace.normals[vertex] = {};
                else {
                    // As before, reject nonfinite source normals before matrix arithmetic.
                    const bool finiteSource = std::isfinite(source.normal_local[0]) &&
                        std::isfinite(source.normal_local[1]) && std::isfinite(source.normal_local[2]);
                    if (finiteSource && !normalTransformAttempted[source.bone]) {
                        normalTransformReady[source.bone] = assets::prepareWorldNormalTransform50HK(
                            matrices[source.bone], normalTransforms[source.bone]);
                        normalTransformAttempted[source.bone] = true;
                    }
                    if (!finiteSource || !normalTransformReady[source.bone] ||
                        !assets::transformPreparedWorldNormal50HK(source.normal_local,
                            normalTransforms[source.bone], workspace.normals[vertex]))
                        // Preserve the existing geometric fallback for collapsed nodes.
                        workspace.normals[vertex] = {};
                }
                ++vertex;
            }
        if (vertex != mesh.vertexCount)
            return reject(d, VehicleDrawError50CM::normals, vertex);
    }

    VehicleDrawSubmission50CM next{destination.vertices, mesh.vertexCount, mesh.indices,
        mesh.indexCount, mesh.spans, mesh.spanCount, {}};
    if (sourceColors && sourceColors->prepareSpecular50LA &&
        !prepareSourceSpecularGeometry(graph, root, selection, mesh, *rootWorld, next, d))
        return false;
    if (sourceColors) {
        next.sourceColorMaterialModes50LA = sourceColors->materialModes;
        next.sourceColorMaterialCount50LA = sourceColors->materialCount;
        next.sourceColorOwner50LA = sourceColors->selectedLights->ownerIdentity();
        next.sourceColorPublication50LA = sourceColors->selectedLights->publication();
        next.sourceColorGraphicsSequence50LA = sourceColors->selectedLights->graphicsSequence();
        next.sourceColorRacerSlot50LA = sourceColors->selectedLights->racerSlot();
    }
    for (std::uint32_t axis = 0; axis < 3; ++axis)
        next.bounds.minimum[axis] = next.bounds.maximum[axis] = workspace.positions[0].v[axis];
    for (std::uint32_t n = 0; n < mesh.vertexCount; ++n) {
        // Position is always projected below; retain only template fields that survive.
        std::memcpy(destination.vertices[n].uv, mesh.vertexTemplate[n].uv,
            sizeof(destination.vertices[n].uv));
        if (sourceColors)
            for (unsigned c = 0; c < 4; ++c)
                destination.vertices[n].color[c] = workspace.sourceColors50LA[n].rgba[c];
        else
            std::memcpy(destination.vertices[n].color, mesh.vertexTemplate[n].color,
                sizeof(destination.vertices[n].color));
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
