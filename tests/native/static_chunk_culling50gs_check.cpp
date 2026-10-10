// Focused actual-asset render equivalence, not a driving/gameplay harness.
// Three frozen camera poses; no physics advancement, animation or oracle.
#include "runtime_startup50fl_fixture.h"
#include "authored_runtime_ground50cp_fixture.h"
#include "pod_materials50ed_fixture.h"
#include "track_materials50fa_fixture.h"
#include "source_texture50fc_fixture.h"
#include "pod_cables50fd_fixture.h"
#include "host_chase_camera50fn_fixture.h"
#include "pod_scene_rebase50fb.h"
#include "../../platform/linux/native/offscreen_gl_context50dc.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../platform/linux/native/material_texture_functions50ee.h"
#include "../../platform/linux/native/vehicle_view_camera50dg.h"
#include <algorithm>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace a = rr::assets;
namespace g = rr::game;
namespace p = rr::platform;
namespace f = runtime_startup50fl;
namespace pt = pod_materials50ed_fixture;
namespace tt = track_materials50fa_fixture;
using J = nlohmann::json;
namespace {
constexpr std::int32_t width = 640, height = 480;
constexpr std::size_t rgbaBytes = std::size_t(width) * height * 4;
void need(bool ok, const std::string& message) {
    if (!ok) throw std::runtime_error(message);
}
J rendererReport(const p::VehicleWindowDiagnostic50DS& d) {
    return {{"error", unsigned(d.error)}, {"operation", d.operation ? d.operation : "none"},
        {"gl_error", d.glError}, {"cleanup_gl_error", d.cleanupGlError},
        {"cleanup_incomplete", d.cleanupIncomplete}, {"component", d.component},
        {"material", d.material}, {"texture", d.texture}, {"track_span", d.trackSpan},
        {"framebuffer_status", d.framebufferStatus}, {"batch_accepted", d.batchAccepted},
        {"default_framebuffer_may_be_modified", d.defaultFramebufferMayBeModified},
        {"track_draws", d.trackDraws}, {"track_textured_draws", d.trackTexturedDraws},
        {"track_untextured_draws", d.trackUntexturedDraws}, {"track_indices", d.trackIndices},
        {"candidate_draws", d.trackCandidateDraws}, {"retained_draws", d.trackRetainedDraws},
        {"culled_draws", d.trackCulledDraws}, {"culled_indices", d.trackCulledIndices},
        {"uncertain_retained_draws", d.trackUncertainRetainedDraws},
        {"culling_applied", d.trackCullingApplied}, {"culling_fallback", d.trackCullingFallback},
        {"visibility_error", unsigned(d.trackVisibility.error)},
        {"cable_draws", d.cableDraws}, {"cable_indices", d.cableIndices},
        {"readback_width", d.readbackWidth}, {"readback_height", d.readbackHeight},
        {"readback_bytes", d.readbackBytes},
        {"readback_buffer_may_be_modified", d.readbackBufferMayBeModified}};
}
J textureReport(const p::MaterialTextureGlDiagnostic50EB& d) {
    return {{"error", unsigned(d.error)}, {"operation", d.operation ? d.operation : "none"},
        {"texture", d.texture}, {"gl_error", d.glError},
        {"cleanup_gl_error", d.cleanupGlError}, {"cleanup_incomplete", d.cleanupIncomplete}};
}
J contextReport(const p::OffscreenGlDiagnostic50DC& d) {
    return {{"error", unsigned(d.error)}, {"operation", d.operation ? d.operation : "none"},
        {"egl_error", d.eglError}, {"gl_error", d.glError},
        {"cleanup_error", unsigned(d.cleanupError)},
        {"cleanup_operation", d.cleanupOperation ? d.cleanupOperation : "none"},
        {"cleanup_egl_error", d.cleanupEglError}};
}
void saveResult(const std::filesystem::path& directory, const J& result) {
    std::ofstream file(directory / "result.json");
    need(bool(file), "cannot open result.json");
    file << result.dump(2) << '\n'; file.close();
    need(bool(file), "result.json write/close failed");
}
void saveImage(const std::filesystem::path& prefix, const std::vector<std::uint8_t>& bytes) {
    std::ofstream raw(prefix.string() + ".rgba", std::ios::binary);
    need(bool(raw), "cannot open raw RGBA output");
    raw.write(reinterpret_cast<const char*>(bytes.data()), bytes.size()); raw.close();
    need(bool(raw), "raw RGBA write/close failed");
    std::ofstream ppm(prefix.string() + ".ppm", std::ios::binary);
    need(bool(ppm), "cannot open PPM output");
    ppm << "P6\n" << width << ' ' << height << "\n255\n";
    for (int y = height - 1; y >= 0; --y) for (int x = 0; x < width; ++x)
        ppm.write(reinterpret_cast<const char*>(bytes.data() + (std::size_t(y) * width + x) * 4), 3);
    ppm.close(); need(bool(ppm), "PPM write/close failed");
}
g::BodyMatrix48G translated(const g::BodyMatrix48G& basis, const float* local) {
    auto result = basis;
    for (unsigned axis = 0; axis < 3; ++axis) {
        volatile float x = basis.v[axis] * local[0], y = basis.v[4 + axis] * local[1];
        volatile float z = basis.v[8 + axis] * local[2], xy = x + y, xyz = xy + z;
        result.v[12 + axis] = basis.v[12 + axis] + xyz;
    }
    return result;
}
}

int main(int argc, char** argv) {
    J result{{"case", "static_chunk_culling50gs_check"}, {"passed", false},
        {"renderer", "vehicle_window_gl50ds"}, {"camera_count", 3},
        {"width", width}, {"height", height}, {"rgba_bytes_per_image", rgbaBytes},
        {"physics_steps", 0}, {"gameplay_claim", false}, {"original_shader_parity", false},
        {"scene", "actual TA textured track and frozen source-derived Pod01 bind pose"},
        {"comparisons", J::array()}, {"failures", J::array()}, {"cleanup", J::array()}};
    std::filesystem::path directory;
    bool directoryCreated = false;
    int code = 1;
    try {
        if (argc != 2) {
            code = 2;
            throw std::runtime_error("usage: static_chunk_culling50gs_check NEW_OUTPUT_DIRECTORY (from repository root)");
        }
        directory = argv[1];
        need(!std::filesystem::exists(directory), "output directory already exists; old evidence is never overwritten");
        need(std::filesystem::create_directory(directory), "cannot create output directory; parent must exist");
        directoryCreated = true;
        need(std::fesetround(FE_TONEAREST) == 0, "round-to-nearest setup failed");
        saveResult(directory, result);

        // Actual native-only cold fixture and source terrain placement. Do not
        // advance vehicle physics or execute any original-word helper here.
        auto fixture = std::make_unique<f::Fixture>(0);
        auto& x = *fixture; auto& state = x.state;
        auto ground = authored_runtime_ground50cp_fixture::load({4, 16, 32, 64});
        need(ground.lift == x.parameters.visual.verticalLimit7EC, "Pod01 lift mismatch");
        std::vector<std::uint32_t> stack(ground.stackCount);
        const auto probe = authored_runtime_ground50cp_fixture::resolve(ground, 16, 0, stack.data(), stack.size());
        need(probe.phase == g::ResetProbePhase50AG::Hit, "authored grid ground probe missed");
        auto pose = ground.pose;
        for (unsigned i = 0; i < 4; ++i) pose.v[12 + i] = std::bit_cast<float>(probe.staged[i]);
        g::FreshPodGeometrySources49O sources{};
        sources.chariot = x.assets.hierarchy[2]; sources.rightEngine = x.assets.hierarchy[1];
        sources.forward = x.assets.motions[4]; sources.back = x.assets.motions[5];
        a::MotionBindingDiagnostic49O bindingDiagnostic{};
        need(a::readMotionNodeBindings49O(x.assets.hierarchy[2],
            reinterpret_cast<const char*>(x.assets.vut[2].data()), x.assets.vut[2].size(),
            sources.chariotBindings, bindingDiagnostic), "actual motion bindings");
        g::FreshPodGeometryWorkspace49O geometryWorkspace{};
        g::FreshPodGeometryResult49O geometry{}; g::FreshPodGeometryDiagnostic49O geometryDiagnostic{};
        need(g::buildFreshPodGeometry49O(sources, geometryWorkspace, geometry, geometryDiagnostic), "actual pod geometry");
        const auto phy = f::readBytes("work/impactAuthoring49Y/Pod01.phy");
        f::bind(phy, geometry, {g::AuthoredClass49N::A, false, true, false, false, {}},
            x.parameters, state, x.selectors, x.secondaryInputs);

        // A frozen authored graphics scene, not a claim that gameplay graphics
        // or animation ran. The same canonical source rebasing used at startup
        // clears nullcg XYZ; the roots use source-derived body-relative offsets.
        const g::SceneNodeIndex49P roots[] = {x.bindings.lifecycle.model1B4,
            x.bindings.visual.right1B8, x.bindings.visual.left1BC};
        const unsigned sourceComponents[] = {2, 1, 0};
        const float* offsets[] = {x.secondaryInputs.targetRelative2F0.v,
            x.parameters.visual.rightBase2C0.v, x.parameters.visual.leftBase2B0.v};
        g::SceneNodeIndex49P sourceNodes[3][18]{};
        g::PodSceneRebaseModel50FB models[3]{};
        for (unsigned component = 0; component < 3; ++component) {
            const auto root = static_cast<unsigned>(roots[component]);
            const auto& hierarchy = x.assets.hierarchy[sourceComponents[component]];
            need(hierarchy.nodeCount <= 18, "source node capacity");
            state.nodes[root].sceneFlags190 = 0x80;
            for (unsigned n = 0; n < hierarchy.nodeCount; ++n) {
                sourceNodes[component][n] = g::SceneNodeIndex49P(root + 1 + n);
                state.nodes[root + 1 + n].sceneFlags190 = 0xa4;
            }
            const auto modelPose = translated(pose, offsets[component]);
            need(g::setLocalMatrix49P({state.nodes, 53}, roots[component], modelPose), "source-derived model root");
            models[component] = {roots[component], hierarchy, sourceNodes[component], hierarchy.nodeCount};
        }
        auto rebaseWorkspace = std::make_unique<g::PodSceneRebaseWorkspace50FB>();
        g::ModelBoundsResult49O rebasedBounds[3]{}; g::PodSceneRebaseDiagnostic50FB rebaseDiagnostic{};
        need(g::rebaseColdPodScene50FB({state.nodes, 53}, models, *rebaseWorkspace,
            rebasedBounds, rebaseDiagnostic), "source cold pod rebase");

        a::VehicleDrawAssets50CR drawAssets; a::VehicleDrawAssetsInput50CR drawInput{};
        a::VehicleDrawAssetDiagnostic50CR assetDiagnostic{};
        std::vector<std::uint8_t> drawBytes[3];
        g::SceneNodeIndex49P drawNodes[3][18]{}; std::uint8_t palettes[3][18]{};
        const char* keys[] = {"cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5",
            "210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3",
            "f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60"};
        const unsigned counts[] = {18, 14, 14}, bases[] = {31, 1, 16}, mapped[] = {2, 0, 1};
        for (unsigned component = 0; component < 3; ++component) {
            drawBytes[component] = f::readBytes(std::string("evidence/visual49G/cacheobjects/") + keys[component] + ".rrscene");
            const auto& psg = x.assets.psg[mapped[component]];
            const unsigned padded = (counts[component] + 3) & ~3u;
            need(psg.size() >= 12 + padded, "actual palette suffix bounds");
            const auto suffix = psg.size() - (12 + padded);
            auto word = [&](std::size_t at) { return std::uint32_t(psg[at]) | (std::uint32_t(psg[at + 1]) << 8) |
                (std::uint32_t(psg[at + 2]) << 16) | (std::uint32_t(psg[at + 3]) << 24); };
            need(word(suffix) == 1 && word(suffix + 4) == padded && word(suffix + 8 + padded) == counts[component],
                "actual single palette record");
            for (unsigned n = 0; n < counts[component]; ++n) {
                palettes[component][n] = psg[suffix + 8 + n];
                drawNodes[component][n] = g::SceneNodeIndex49P(bases[component] + n);
            }
            drawInput.component[component] = {&drawBytes[component], drawNodes[component], counts[component],
                palettes[component], counts[component]};
        }
        need(drawAssets.load(drawInput, assetDiagnostic), "actual draw asset publication");
        g::VehicleDrawBatchInput50CO batchInput{}; g::VehicleDrawBatch50CO batch{};
        auto drawWorkspace = std::make_unique<g::VehicleDrawWorkspace50CM>();
        g::VehicleDrawBatchDiagnostic50CO drawDiagnostic{};
        need(drawAssets.batchInput(0, g::ModelPalettePhase49S::selectedRenderPreparation,
            g::ModelPaletteSceneState49S::completeCurrentScene, batchInput), "frozen graphics bank");
        need(g::buildVehicleDrawBatch50CO(state, x.bindings, batchInput, batch, *drawWorkspace, drawDiagnostic),
            "actual frozen pod draw preparation");
        result["pod"] = {{"vertices", batch.vertices}, {"indices", batch.indices}, {"draws", batch.draws},
            {"minimum", batch.bounds.minimum}, {"maximum", batch.bounds.maximum}};
        std::vector<std::uint8_t> preparedPodBytes;
        for (const auto& component : batch.component) {
            const auto* bytes = reinterpret_cast<const std::uint8_t*>(component.vertices);
            preparedPodBytes.insert(preparedPodBytes.end(), bytes, bytes + component.vertexCount * sizeof(a::Vertex48B));
        }
        result["pod"]["prepared_vertex_sha256"] = pt::detail::sha256(preparedPodBytes);
        result["authored_grid_pose"] = pose.v;

        auto podOwner = std::make_unique<pt::Owner>(); pt::Request podRequest{};
        podRequest.root = "evidence/visual49G"; podRequest.coldSourceByteCapacity = 8u * 1024u * 1024u;
        podRequest.textureBudgets = {5, 26, 1024u * 1024u, 1024u * 1024u, 256};
        for (unsigned component = 0; component < 3; ++component) {
            const auto* scene = drawAssets.scene(g::VehicleDrawComponent50CM(component));
            need(scene, "actual pod material scene");
            podRequest.choices[component].assign(scene->materials.size(),
                {1, 1, a::MaterialTextureRequirement50EA::required, pt::MipMode::largestRetained, 0});
        }
        pt::Diagnostic podDiagnostic{};
        need(podOwner->load(podRequest, drawAssets, podDiagnostic), "actual pod materials: " + podDiagnostic.message);
        a::Mesh48B trackMesh; std::string trackError;
        need(a::loadMesh48B("evidence/visual49G/export-placed-normal/ta-world.rrmesh", trackMesh, trackError),
            "actual TA mesh: " + trackError);
        auto trackOwner = std::make_unique<tt::Owner>(); tt::Request trackRequest{};
        trackRequest.root = "evidence/visual49G";
        trackRequest.exportReport = "evidence/visual49G/export-placed-normal/export-report.json";
        trackRequest.choice = {1, 1, a::MaterialTextureRequirement50EA::optionalUntextured, tt::MipMode::largestRetained, 0};
        trackRequest.coldSourceByteCapacity = 32u * 1024u * 1024u;
        trackRequest.sourceArchive = "work/native/project/disc/TRACKS/TA.RES";
        trackRequest.decodeSourceTexture = &source_texture50fc_fixture::decodeSourceTexture;
        trackRequest.allowStaticAnimationFrame = true;
        trackRequest.textureBudgets = {32, 1024, 8u * 1024u * 1024u, 8u * 1024u * 1024u, 1024};
        tt::Diagnostic trackDiagnostic{};
        need(trackOwner->load(trackRequest, trackMesh, trackDiagnostic), "actual TA materials: " + trackDiagnostic.message);
        auto cableOwner = std::make_unique<pod_cables50fd_fixture::Owner>();
        pod_cables50fd_fixture::Diagnostic cableDiagnostic{};
        need(cableOwner->load(drawAssets, x.bindings, state, cableDiagnostic), "actual cable texture/endpoints");

        // Close all borrowers explicitly before their immutable banks/context.
        p::OffscreenGlContext50DC context;
        p::MaterialTextureGl50EB podGpu, trackGpu, cableGpu;
        p::VehicleWindowGl50DS renderers[2];
        auto close = [&]() {
            bool clean = true;
            for (unsigned mode = 0; mode < 2; ++mode) if (renderers[mode].ownsResources()) {
                p::VehicleWindowDiagnostic50DS d{};
                const bool ok = renderers[mode].close(d); clean = clean && ok;
                result["cleanup"].push_back({{"renderer", mode}, {"ok", ok}, {"diagnostic", rendererReport(d)}});
            }
            if (renderers[0].ownsResources() || renderers[1].ownsResources()) return false;
            for (auto* bank : {&cableGpu, &trackGpu, &podGpu}) if (bank->ownsResources()) {
                p::MaterialTextureGlDiagnostic50EB d{};
                const bool ok = bank->close(d); clean = clean && ok;
                result["cleanup"].push_back({{"texture_bank", true}, {"ok", ok}, {"diagnostic", textureReport(d)}});
            }
            if (cableGpu.ownsResources() || trackGpu.ownsResources() || podGpu.ownsResources()) return false;
            if (context.ownsResources()) {
                p::OffscreenGlDiagnostic50DC d{};
                const bool ok = context.close(d); clean = clean && ok;
                result["cleanup"].push_back({{"context", true}, {"ok", ok}, {"diagnostic", contextReport(d)}});
            }
            return clean;
        };
        try {
            p::OffscreenGlRequest50DC contextRequest{};
            contextRequest.platform = p::OffscreenGlPlatform50DC::MesaSurfaceless;
            contextRequest.width = width; contextRequest.height = height;
            contextRequest.major = 3; contextRequest.minor = 2;
            contextRequest.profile = p::OffscreenGlProfile50DC::Core;
            contextRequest.redBits = contextRequest.greenBits = contextRequest.blueBits = contextRequest.alphaBits = 8;
            contextRequest.depthBits = 24; contextRequest.stencilBits = 0; contextRequest.samples = 0;
            p::OffscreenGlDiagnostic50DC contextDiagnostic{};
            if (!context.create(contextRequest, contextDiagnostic)) {
                result["failures"].push_back({{"context_create", contextReport(contextDiagnostic)}});
                throw std::runtime_error("surfaceless EGL context creation failed");
            }
            result["context"] = {{"gl_major", context.info().glMajor}, {"gl_minor", context.info().glMinor},
                {"platform", "MesaSurfaceless"}, {"width", context.info().width}, {"height", context.info().height}};
            p::VehicleWindowRequest50DS request{}; request.context = &context;
            request.procAddress = [](void* c, const char* name) noexcept -> p::VehicleWindowGlProc50DS {
                return static_cast<p::OffscreenGlContext50DC*>(c)->procAddress(name);
            };
            request.isCurrent = [](void* c) noexcept { return static_cast<p::OffscreenGlContext50DC*>(c)->current(); };
            request.maximumWidth = width; request.maximumHeight = height;
            request.output = p::VehicleWindowOutput50DS::headless;
            request.mode = p::VehicleWindowMode50DS::callerMaterialTextures;
            p::MaterialTextureGlRequest50EB textureRequest{};
            textureRequest.context = &context; textureRequest.isCurrent = request.isCurrent;
            const char* missing = nullptr;
            need(p::loadMaterialTextureFunctions50EE(&context, request.procAddress, textureRequest.gl, missing),
                "required texture GL entry point missing");
            auto upload = [&](const a::MaterialTextureAssets50EA* bank, p::MaterialTextureGl50EB& gpu, std::size_t limit) {
                need(bank, "actual CPU texture bank missing");
                std::vector<p::MaterialTextureSampler50EB> samplers(bank->textureCount(),
                    {p::MaterialTextureFilter50EB::linear, p::MaterialTextureFilter50EB::linear,
                     p::MaterialTextureWrap50EB::repeat, p::MaterialTextureWrap50EB::repeat});
                textureRequest.samplers = samplers.data(); textureRequest.samplerCount = samplers.size();
                textureRequest.maximumGpuBytes = limit;
                p::MaterialTextureGlDiagnostic50EB d{};
                if (!gpu.create(textureRequest, *bank, d)) {
                    result["failures"].push_back({{"texture_upload", textureReport(d)}});
                    throw std::runtime_error("actual texture upload failed");
                }
            };
            upload(podOwner->bank(), podGpu, 1024u * 1024u);
            upload(trackOwner->bank(), trackGpu, 8u * 1024u * 1024u);
            upload(cableOwner->bank(), cableGpu, 64u * 64u * 4u);
            need(podGpu.textureCount() == 5 && podGpu.gpuByteCount() == 622592, "exact Pod texture publication");
            result["textures"] = {{"pod_count", podGpu.textureCount()}, {"pod_bytes", podGpu.gpuByteCount()},
                {"track_count", trackGpu.textureCount()}, {"track_bytes", trackGpu.gpuByteCount()},
                {"cable_count", cableGpu.textureCount()}, {"cable_bytes", cableGpu.gpuByteCount()}};
            std::vector<p::VehicleWindowMaterialBinding50DS> materials[3];
            for (unsigned component = 0; component < 3; ++component) {
                const auto* selected = podOwner->component(component); need(selected, "pod material publication");
                for (const auto& entry : *selected)
                    materials[component].push_back({p::VehicleWindowMaterialMode50DS::requiredTexture, entry.textureIndex});
                request.materials.component[component] = {materials[component].data(), std::uint32_t(materials[component].size())};
            }
            request.materials.textures = &podGpu;
            const auto* spans = trackOwner->spans(); const auto* selections = trackOwner->selections();
            need(spans && selections, "TA span/material publication");
            std::vector<p::VehicleWindowTrackSpan50DS> bindings;
            for (const auto& span : *spans) {
                p::VehicleWindowTrackSpan50DS b{};
                b.indexStart = span.indexStart; b.indexCount = span.indexCount;
                b.vertexStart = span.vertexStart; b.vertexCount = span.vertexCount;
                b.material.mode = span.textureIndex == tt::unresolved ? p::VehicleWindowMaterialMode50DS::untextured :
                    p::VehicleWindowMaterialMode50DS::requiredTexture;
                b.material.textureIndex = span.textureIndex;
                need(span.materialSelection < selections->size(), "TA span material index");
                auto textureName = (*selections)[span.materialSelection].textureName;
                for (char& c : textureName) if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
                b.discardZeroAlpha = span.textureIndex != tt::unresolved && (textureName == "crowd" || textureName == "arrowred");
                bindings.push_back(b);
            }
            request.track.mesh = &trackMesh; request.track.maximumVertexBytes = 4u * 1024u * 1024u;
            request.track.maximumIndexBytes = 2u * 1024u * 1024u;
            request.track.mode = p::VehicleWindowMode50DS::callerMaterialTextures;
            request.track.vertexColorScale = 2.f; request.track.textures = &trackGpu;
            request.track.spans = bindings.data(); request.track.spanCount = bindings.size();
            request.cables.textures = &cableGpu; request.cables.textureIndex = 0;
            result["track"] = {{"vertices", trackMesh.vertices.size()}, {"indices", trackMesh.indices.size()},
                {"source_spans", bindings.size()}, {"vertex_color_scale", 2}};
            std::uint64_t expectedChunks = 0;
            for (const auto& span : bindings)
                expectedChunks += span.material.mode == p::VehicleWindowMaterialMode50DS::requiredTexture ?
                    (std::uint64_t(span.indexCount) + 767u) / 768u : 1u;
            result["chunk_triangles"] = 256; result["expected_chunks"] = expectedChunks;
            bool anyIndexReduction = false;
            for (unsigned mode = 0; mode < 2; ++mode) {
                request.track.cullingChunkTriangles = mode ? 256 : 0;
                request.track.enableFrustumCulling = mode != 0;
                p::VehicleWindowDiagnostic50DS d{};
                if (!renderers[mode].create(request, drawAssets, d)) {
                    result["failures"].push_back({{"renderer_create_mode", mode}, {"diagnostic", rendererReport(d)}});
                    throw std::runtime_error("shared 50DS renderer creation failed");
                }
            }

            const float heading = std::hypot(pose.v[8], pose.v[10]); need(heading > .0001f, "source grid heading");
            const float fx = pose.v[8] / heading, fz = pose.v[10] / heading, rx = -fz, rz = fx;
            const char* cameraNames[] = {"grid_chase", "grid_side", "grid_elevated"};
            const float offsetsByCamera[3][4] = {{-52.f, 0.f, 12.f, 38.f}, {-10.f, 90.f, 35.f, 0.f}, {-180.f, 0.f, 150.f, 90.f}};
            std::vector<std::uint8_t> images[2] = {std::vector<std::uint8_t>(rgbaBytes), std::vector<std::uint8_t>(rgbaBytes)};
            std::uint64_t totalUnculled = 0, totalCulled = 0;
            for (unsigned cameraIndex = 0; cameraIndex < 3; ++cameraIndex) {
                const auto& o = offsetsByCamera[cameraIndex];
                p::VehicleViewCamera50DG camera{};
                camera.eye[0] = pose.v[12] + fx * o[0] + rx * o[1]; camera.eye[1] = pose.v[13] + o[2];
                camera.eye[2] = pose.v[14] + fz * o[0] + rz * o[1];
                camera.target[0] = pose.v[12] + fx * o[3]; camera.target[1] = pose.v[13] + 3.f;
                camera.target[2] = pose.v[14] + fz * o[3]; camera.up[1] = 1.f;
                camera.verticalFovRadians = 1.0471975512f; camera.aspectWidthOverHeight = float(width) / height;
                camera.nearPlane = 1.f; camera.farPlane = 3000.f;
                host_chase_camera50fn_fixture::State framingState{};
                host_chase_camera50fn_fixture::Diagnostic framingDiagnostic{};
                p::VehicleViewCamera50DG framed{};
                need(host_chase_camera50fn_fixture::framePod(camera, batch, framingState, framed, framingDiagnostic),
                    "fixed-camera pod framing");
                camera = framed;
                p::VehicleViewMatrix50DG view{}; p::VehicleViewCameraError50DG cameraError{};
                need(p::composeVehicleViewCamera50DG(camera, view, cameraError), "fixed camera composition");
                g::PodCableMesh50FD cables{}; g::PodCableWorkspace50FD cableWorkspace{};
                g::PodCableDiagnostic50FD cableDrawDiagnostic{};
                need(g::buildPodCables50FD(state, x.bindings, cableOwner->bindings(),
                    {{camera.eye[0] - camera.target[0], camera.eye[1] - camera.target[1], camera.eye[2] - camera.target[2]}},
                    cables, cableWorkspace, cableDrawDiagnostic), "actual fixed-camera cables");
                const float clear[] = {.035f, .045f, .08f, 1.f};
                const p::VehicleWindowFrame50DS frame{width, height, view.worldViewProjection, clear, &cables};
                J row{{"camera", cameraNames[cameraIndex]}, {"eye", camera.eye}, {"target", camera.target},
                    {"world_view_projection", view.worldViewProjection}, {"pod_framed", framingDiagnostic.framed},
                    {"framing_adjusted", framingDiagnostic.adjusted}, {"framing_limited", framingDiagnostic.limited},
                    {"modes", J::array()}};
                p::VehicleWindowDiagnostic50DS receipts[2];
                bool validPair = true;
                for (unsigned mode = 0; mode < 2; ++mode) {
                    auto& d = receipts[mode];
                    const bool accepted = renderers[mode].submit(batch, frame, d);
                    J evidence{{"culling", mode != 0}, {"accepted", accepted}, {"submit", rendererReport(d)}};
                    if (!accepted || !d.batchAccepted || d.defaultFramebufferMayBeModified) validPair = false;
                    if (accepted && d.batchAccepted && !d.defaultFramebufferMayBeModified) {
                        p::VehicleWindowDiagnostic50DS readback{};
                        const bool read = renderers[mode].readbackRgba(images[mode].data(), images[mode].size(), readback);
                        evidence["readback"] = rendererReport(readback);
                        evidence["readback_ok"] = read;
                        if (!read || readback.readbackWidth != width || readback.readbackHeight != height || readback.readbackBytes != rgbaBytes)
                            validPair = false;
                        else {
                            const auto name = std::string(cameraNames[cameraIndex]) + (mode ? "-culled" : "-unculled");
                            saveImage(directory / name, images[mode]);
                            evidence["rgba_file"] = name + ".rgba"; evidence["ppm_file"] = name + ".ppm";
                            evidence["rgba_sha256"] = pt::detail::sha256(images[mode]);
                        }
                    }
                    row["modes"].push_back(evidence);
                    // Preserve even the first mode if the next operation fails.
                    result["in_progress"] = row; saveResult(directory, result);
                }
                row["valid_pair"] = validPair;
                if (validPair) {
                    std::size_t byteDifferences = 0, pixelDifferences = 0, first = rgbaBytes;
                    unsigned maximumDelta = 0; bool varied = false;
                    for (std::size_t at = 0; at < rgbaBytes; at += 4) {
                        bool pixel = false;
                        for (unsigned channel = 0; channel < 4; ++channel) {
                            const auto i = at + channel;
                            const unsigned delta = unsigned(std::abs(int(images[0][i]) - int(images[1][i])));
                            if (delta) { ++byteDifferences; pixel = true; first = std::min(first, i); }
                            maximumDelta = std::max(maximumDelta, delta);
                            if (channel < 3 && images[0][i] != images[0][channel]) varied = true;
                        }
                        pixelDifferences += pixel;
                    }
                    const auto& off = receipts[0]; const auto& on = receipts[1];
                    const bool counters = !off.trackCullingApplied && !off.trackCullingFallback &&
                        off.trackCandidateDraws == bindings.size() && off.trackRetainedDraws == bindings.size() &&
                        off.trackCulledDraws == 0 && off.trackCulledIndices == 0 &&
                        off.trackDraws == bindings.size() && off.trackIndices == trackMesh.indices.size() &&
                        on.trackCullingApplied && !on.trackCullingFallback && on.trackCandidateDraws == expectedChunks &&
                        std::uint64_t(on.trackRetainedDraws) + on.trackCulledDraws == expectedChunks &&
                        on.trackDraws == on.trackRetainedDraws && on.trackUncertainRetainedDraws <= on.trackRetainedDraws &&
                        std::uint64_t(on.trackIndices) + on.trackCulledIndices == trackMesh.indices.size() &&
                        on.trackIndices <= off.trackIndices && on.cableDraws == off.cableDraws && off.cableDraws == 1;
                    row["exact_rgba_equal"] = byteDifferences == 0; row["different_bytes"] = byteDifferences;
                    row["different_pixels"] = pixelDifferences; row["maximum_channel_delta"] = maximumDelta;
                    row["first_different_byte"] = first == rgbaBytes ? J(nullptr) : J(first);
                    row["nonconstant_rgb"] = varied; row["counter_invariants"] = counters;
                    row["track_draw_reduction"] = std::int64_t(off.trackDraws) - on.trackDraws;
                    row["track_index_reduction"] = std::int64_t(off.trackIndices) - on.trackIndices;
                    row["passed"] = byteDifferences == 0 && varied && counters;
                    anyIndexReduction |= on.trackIndices < off.trackIndices;
                    totalUnculled += off.trackDraws; totalCulled += on.trackDraws;
                } else row["passed"] = false;
                result["comparisons"].push_back(row); result.erase("in_progress");
                saveResult(directory, result);
            }
            result["total_unculled_track_draws"] = totalUnculled;
            result["total_culled_track_draws"] = totalCulled;
            result["aggregate_draw_reduction"] = std::int64_t(totalUnculled) - std::int64_t(totalCulled);
            std::size_t podOffset = 0; bool podUnchanged = true;
            for (const auto& component : batch.component) {
                const auto bytes = component.vertexCount * sizeof(a::Vertex48B);
                podUnchanged = podUnchanged && std::memcmp(preparedPodBytes.data() + podOffset, component.vertices, bytes) == 0;
                podOffset += bytes;
            }
            result["shared_prepared_pod_vertices_unchanged"] = podUnchanged;
            bool allPassed = podUnchanged && anyIndexReduction;
            for (const auto& row : result["comparisons"]) allPassed = allPassed && row["passed"].get<bool>();
            need(allPassed, "one or more exact comparisons/counter checks failed, or no draw reduction");
        } catch (...) {
            result["cleanup_complete"] = close(); saveResult(directory, result); throw;
        }
        result["cleanup_complete"] = close();
        need(result["cleanup_complete"].get<bool>(), "renderer/texture/context cleanup failed");
        result["passed"] = true; code = 0;
    } catch (const std::exception& error) { result["failure"] = error.what(); }
      catch (...) { result["failure"] = "unknown exception"; }
    if (directoryCreated) {
        try { saveResult(directory, result); }
        catch (const std::exception& error) { result["evidence_write_failure"] = error.what(); result["passed"] = false; code = 1; }
    }
    std::cout << result.dump() << '\n';
    return code;
}
