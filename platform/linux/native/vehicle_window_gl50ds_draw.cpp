#include "vehicle_window_gl50ds_storage.h"
#include "../../../tests/native/ra_static_fire_visual50la.h"
#include "native_fxaa_pass.h"
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace rr::platform {
void VehicleWindowGl50DS::Storage::acceptCompletedTrackVisibility() noexcept {
        // Race banks are distinct from host-menu, UI and blocking-fade targets.
        // Only a fully successful gameplay submit sets the pending target.
        if(!trackVisibilityPendingTarget)return;
        const bool actualGameplay=trackVisibilityPendingTarget==displayedTarget &&
            displayedTarget==targets[published];
        trackVisibilityPendingTarget=0;
        if(!actualGameplay)return;
        if(trackVisibilityCompletedGeneration==std::numeric_limits<std::uint64_t>::max()) {
            trackVisibilityCompletedValid=false;
            if(raStaticFireOwner)raStaticFireOwner->nativeCompletionLive_=false;
            return;
        }
        for(U i=0;i<trackSpanCount;++i)
            trackSpans[i].submittedCompletedGameplay=trackSpans[i].submittedGameplayCandidate;
        ++trackVisibilityCompletedGeneration;trackVisibilityCompletedValid=true;
        if(raStaticFireOwner){
            // Exact cold admission proved source-order spans0..3 and retained
            // their actual worldInstance identities. Read only issued draw bits.
            if(trackSpanCount!=4){trackVisibilityCompletedValid=false;raStaticFireOwner->nativeCompletionLive_=false;return;}
            std::array<bool,4> submitted{};
            for(U i=0;i<4;++i)submitted[i]=trackSpans[i].submittedCompletedGameplay;
            if(!raStaticFireOwner->recordNativeCompleted(raStaticFireRenderer,
                raStaticFirePublicationSerial,trackVisibilityCompletedGeneration,submitted)){
                trackVisibilityCompletedValid=false;raStaticFireOwner->nativeCompletionLive_=false;
            }
        }
    }

bool VehicleWindowGl50DS::Storage::validateTrackAnimationFrames(const VehicleWindowFrame50DS& frame,
        VehicleWindowDiagnostic50DS& d) const noexcept {
        if (frame.trackAnimationFrameCount != trackAnimationCount ||
            bool(frame.trackAnimationFrames) != bool(trackAnimationCount))
            return fail(d, VehicleWindowError50DS::frame, "one explicit current frame per original track animation required");
        for (U i = 0; i < trackAnimationCount; ++i)
            if (frame.trackAnimationFrames[i] >= trackAnimations[i].frameCount)
                return fail(d, VehicleWindowError50DS::frame, "current track animation frame outside original stored extent");
        return true;
    }

void VehicleWindowGl50DS::Storage::prepareTrackVisibility() noexcept {
        // Start each frame at the complete list. On helper failure its output
        // prefix is discarded by leaving this mode/count unchanged.
        frameTrackDrawCount = trackDrawCount;
        frameTrackCullingApplied = false;
        diagnostic->trackCandidateDraws = diagnostic->trackRetainedDraws = trackDrawCount;
        if (!request.track.enableFrustumCulling || !trackDrawCount) return;
        game::DrawVisibilityInput50EI input{};
        input.draws = trackDrawBounds.get(); input.drawCount = trackDrawCount;
        input.worldViewProjection = matrix; // Same copied bytes as both GL_FALSE uploads.
        input.clipDepth = game::DrawClipDepth50EI::negativeOneToOne;
        input.uncertainPolicy = game::DrawUncertainPolicy50EI::retain;
        const game::DrawVisibilityBuffer50EI buffer{trackVisibleDrawIds.get(), trackDrawCount};
        game::DrawVisibilityResult50EI result{};
        if (!game::collectVisibleStaticDraws50EI(input, buffer, result, diagnostic->trackVisibility)) {
            diagnostic->trackCullingFallback = true;
            return;
        }
        frameTrackDrawCount = result.retained;
        frameTrackCullingApplied = true;
        diagnostic->trackCullingApplied = true;
        diagnostic->trackRetainedDraws = result.retained;
        diagnostic->trackCulledDraws = result.culled;
        diagnostic->trackUncertainRetainedDraws = result.uncertainRetained;
        U retainedIndices = 0;
        for (U i = 0; i < result.retained; ++i) {
            const U draw = trackVisibleDrawIds[i];
            retainedIndices += trackChunkCount ? trackChunks[draw].indexCount :
                (trackSpanCount ? trackSpans[draw].indexCount : static_cast<U>(trackIndexCount));
        }
        diagnostic->trackCulledIndices = trackCandidateIndexCount - retainedIndices;
    }

bool VehicleWindowGl50DS::Storage::validateMaterialUvs(const VehicleWindowMaterialUv50DS* uvs, U count,
        VehicleWindowDiagnostic50DS& d, U archetype) const noexcept {
        const auto selected = racerView(archetype);
        if ((count && !uvs) || count > selected.materialCount || (!count && uvs))
            return fail(d, VehicleWindowError50DS::frame, "material UV array requires valid bounded entries");
        U previousKey = 0;
        for (U i = 0; i < count; ++i) {
            const auto& uv = uvs[i];
            const U c = componentIndex(uv.component);
            if (c >= selected.resources->componentCount || uv.materialIndex >= selected.scenes[c]->materials.size())
                return fail(d, VehicleWindowError50DS::frame, "material UV identity outside original scene");
            const U key = selected.materialOffsets[c] + uv.materialIndex;
            if (i && key <= previousKey)
                return fail(d, VehicleWindowError50DS::frame, "material UV array must be unique and source ordered");
            previousKey = key;
            if (selected.materials[key].policy.sourceSpecularPair50LA &&
                (uv.uScale != 1.f || uv.vScale != 1.f || uv.uOffset != 0.f || uv.vOffset != 0.f))
                return fail(d, VehicleWindowError50DS::material, "source pair requires its actual unmodified base UV");
            const float values[] = {uv.uScale, uv.vScale, uv.uOffset, uv.vOffset};
            for (float value : values)
                if (!std::isfinite(value))
                    return fail(d, VehicleWindowError50DS::frame, "nonfinite material UV transform");
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::retainedMesh(const game::VehicleDrawBatch50CO& batch, VehicleWindowDiagnostic50DS& d,
        U archetype, RacerNormalReceipt* normalReceipt) const noexcept {
        const auto selected = racerView(archetype);
        if (batch.componentCount != selected.resources->componentCount)
            return fail(d, VehicleWindowError50DS::assets, "batch component count differs from actual assets");
        for (U c = 0; c < batch.componentCount; ++c) {
            const auto& mesh = *selected.meshes[c];
            const auto& part = batch.component[c];
            if (part.indices != mesh.indices || part.indexCount != mesh.indexCount ||
                part.spans != mesh.spans || part.spanCount != mesh.spanCount || part.vertexCount != mesh.vertexCount)
                return fail(d, VehicleWindowError50DS::assets, "batch is not the retained prepared mesh");
            // Full semantic material identity, including unused entries, is
            // retained with the generated RGB. A subset mask must not relabel
            // imported RGB as source color or route generated RGB to legacy.
            const auto count = static_cast<U>(selected.scenes[c]->materials.size());
            const auto& summary = selected.materialSummary[c];
            const bool sourceDiffuse = summary.sourceDiffuse, sourcePair = summary.sourcePair;
            if (part.sourceSpecularGeometry50LA != sourcePair)
                return fail(d, VehicleWindowError50DS::frame, "source pair geometry differs from actual cold body policy");
            if (!sourceDiffuse) {
                if (part.sourceColorMaterialModes50LA || part.sourceColorMaterialCount50LA ||
                    part.sourceColorOwner50LA || part.sourceColorPublication50LA ||
                    part.sourceColorGraphicsSequence50LA || part.sourceColorRacerSlot50LA)
                    return fail(d, VehicleWindowError50DS::frame,
                        "legacy racer component must not carry source-color metadata");
                continue;
            }
            d.component = c;
            if (!sourceBodyComponent50LA(c, batch.componentCount) ||
                !part.sourceColorMaterialModes50LA || part.sourceColorMaterialCount50LA != count ||
                !part.sourceColorOwner50LA || !part.sourceColorPublication50LA ||
                !part.sourceColorGraphicsSequence50LA)
                return fail(d, VehicleWindowError50DS::frame,
                    "source diffuse requires the actual engine mask and light identity in a three/five-model pod");
            for (U m = 0; m < count; ++m) {
                d.material = m;
                const auto& binding = selected.materials[selected.materialOffsets[c] + m].policy;
                if (part.sourceColorMaterialModes50LA[m] !=
                    (binding.sourceDiffuseCompressed50LA ? 1u : 0u))
                    return fail(d, VehicleWindowError50DS::material,
                        "source-color material mask differs from the retained actual material policy");
            }
            // Bounded scalar research receipt: first observed component for
            // each racer on this renderer thread. Not a runtime admission
            // rule: later lighting, materials and sessions can differ.
            static thread_local bool alphaObserved[8][maxComponents]{};
            // Process launch configuration; read once, not per racer component per frame.
            static const bool alphaTraceRequested = [] {
                const char* value = std::getenv("RR_TRACE_PHASES");
                return value && value[0] == '1';
            }();
            const bool traceAlpha = alphaTraceRequested &&
                part.sourceColorRacerSlot50LA < 8 &&
                !alphaObserved[part.sourceColorRacerSlot50LA][c];
            for (U i = 0; i < part.spanCount; ++i) {
                const auto& span = part.spans[i];
                if (!span.index_count) continue;
                d.material = span.material_index;
                if (span.material_index >= count ||
                    part.sourceColorMaterialModes50LA[span.material_index] !=
                    (selected.materials[selected.materialOffsets[c] + span.material_index].
                        policy.sourceDiffuseCompressed50LA ? 1u : 0u))
                    return fail(d, VehicleWindowError50DS::material,
                        "source-color selected span differs from its retained material policy");
                const auto& material = selected.materials[selected.materialOffsets[c] + span.material_index];
                if (traceAlpha && material.policy.sourceSpecularPair50LA && part.vertices &&
                    span.vertex_start <= part.vertexCount && span.vertex_count <= part.vertexCount - span.vertex_start) {
                    unsigned minimum = 255, maximum = 0;
                    for (U v = span.vertex_start; v < span.vertex_start + span.vertex_count; ++v) {
                        const unsigned alpha = part.vertices[v].color[3];
                        if (alpha < minimum) minimum = alpha;
                        if (alpha > maximum) maximum = alpha;
                    }
                    std::fprintf(stderr, "racer_pair_alpha slot=%u component=%u material=%u span=%u vertices=%u min=%u max=%u base=%llu map2=%llu sequence=%llu\n",
                        unsigned(part.sourceColorRacerSlot50LA), c, span.material_index, i, span.vertex_count, minimum, maximum,
                        static_cast<unsigned long long>(material.textureIdentity50LA),
                        static_cast<unsigned long long>(material.specularIdentity50LA),
                        static_cast<unsigned long long>(part.sourceColorGraphicsSequence50LA));
                }
            }
            if (traceAlpha) alphaObserved[part.sourceColorRacerSlot50LA][c] = true;
        }
        d.component = maxComponents;
        d.material = assets::unresolvedMaterialTexture50EA;
        if (normalReceipt && normalReceipt->matches(batch)) return true;
        if (normalReceipt) normalReceipt->completedBatch = nullptr;
        if (!validateRacerNormals(batch, d)) return false;
        if (normalReceipt) normalReceipt->accept(batch);
        return true;
    }

bool VehicleWindowGl50DS::Storage::validateRacerNormals(const game::VehicleDrawBatch50CO& batch,
        VehicleWindowDiagnostic50DS& d) const noexcept {
        if (!game::validVehicleDrawComponentCount50CM(batch.componentCount))
            return fail(d, VehicleWindowError50DS::frame, "invalid active racer component count");
        for (U c = 0; c < batch.componentCount; ++c) {
            const auto& part = batch.component[c];
            if (bool(part.sourceNormals) != bool(part.sourceNormalCount) ||
                (part.sourceNormals && part.sourceNormalCount != part.vertexCount))
                return fail(d, VehicleWindowError50DS::frame,
                    "racer normal stream must match its exact live vertex count");
            for (U n = 0; n < part.sourceNormalCount; ++n) {
                const auto& normal = part.sourceNormals[n].xyz;
                const double length = std::hypot(normal[0], normal[1], normal[2]);
                if (!std::isfinite(length) || (length != 0.0 && std::abs(length - 1.0) > 0.001))
                    return fail(d, VehicleWindowError50DS::frame,
                        "racer normals must be finite unit vectors or explicit zero");
            }
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::validateOpponents(const game::VehicleDrawBatch50CO& player, const VehicleWindowFrame50DS& frame,
        VehicleWindowDiagnostic50DS& d,
        RacerNormalReceipt (&normalReceipts)[1 + maxVehicleWindowOpponents50DS]) const noexcept {
        if (frame.opponentCount > maxVehicleWindowOpponents50DS ||
            bool(frame.opponentCount) != bool(frame.opponents))
            return fail(d, VehicleWindowError50DS::frame, "opponents require an explicit bounded array of at most seven");
        if (!frame.opponentCount) return true;
        const auto capacity = adapter->backend().capacity;
        game::VehicleDrawPlan50CS plan{};
        if (!game::preflightVehicleDrawBatch50CS(player, capacity, plan, d.submission))
            return fail(d, VehicleWindowError50DS::submission, "player batch preflight rejected before opponent rendering");
        if (!retainedMesh(player, d, 0, &normalReceipts[0])) return false;
        for (U i = 0; i < frame.opponentCount; ++i) {
            d.opponentIndex = i;
            const auto& opponent = frame.opponents[i];
            if (!opponent.batch)
                return fail(d, VehicleWindowError50DS::frame, "opponent requires an actual successful batch");
            if (opponent.racerArchetype > racerArchetypeCount)
                return fail(d, VehicleWindowError50DS::frame, "opponent selects an unprepared racer archetype");
            const auto selected = racerView(opponent.racerArchetype);
            game::VehicleDrawCapacity50CS racerCapacity{};
            racerCapacity.commands = game::maxVehicleDrawCommands50CS;
            for (U c = 0; c < selected.resources->componentCount; ++c) {
                racerCapacity.vertices[c] = selected.meshes[c]->vertexCount;
                racerCapacity.indices[c] = selected.meshes[c]->indexCount;
            }
            if (!game::preflightVehicleDrawBatch50CS(*opponent.batch, racerCapacity, plan, d.submission))
                return fail(d, VehicleWindowError50DS::submission, "opponent batch preflight rejected");
            if (!retainedMesh(*opponent.batch, d, opponent.racerArchetype, &normalReceipts[i + 1]) ||
                !validateMaterialUvs(opponent.materialUvs, opponent.materialUvCount, d,
                    opponent.racerArchetype)) return false;
            if (opponent.cables && (!selected.cableTextureName || !validateCableFrame(*opponent.cables, d)))
                return fail(d, VehicleWindowError50DS::frame, "opponent cables require valid geometry and the cold cable bank");
            // Each independent live 50CO stream must survive until deferred
            // effects. Reject overlapping player/opponent/component storage.
            for (U c = 0; c < selected.resources->componentCount; ++c) {
                const auto& part = opponent.batch->component[c];
                const auto start = reinterpret_cast<std::uintptr_t>(part.vertices);
                const auto bytes = std::uintptr_t(part.vertexCount) * sizeof(assets::Vertex48B);
                if (start > std::numeric_limits<std::uintptr_t>::max() - bytes)
                    return fail(d, VehicleWindowError50DS::frame, "opponent vertex range overflow");
                for (U prior = 0; prior <= i + 1; ++prior) {
                    const auto& other = prior ? *frame.opponents[prior - 1].batch : player;
                    for (U k = 0; k < (prior == i + 1 ? c : other.componentCount); ++k) {
                        const auto begin = reinterpret_cast<std::uintptr_t>(other.component[k].vertices);
                        const auto size = std::uintptr_t(other.component[k].vertexCount) * sizeof(assets::Vertex48B);
                        if (begin > std::numeric_limits<std::uintptr_t>::max() - size ||
                            (start < begin + size && begin < start + bytes))
                            return fail(d, VehicleWindowError50DS::frame, "racers require disjoint live world-space vertex arrays");
                    }
                }
                if (part.skin) {
                    if (!selected.skin50LA || !selected.skin50LA[c].eligible ||
                        part.skin->boneCount != selected.skin50LA[c].boneCount)
                        return fail(d, VehicleWindowError50DS::frame, "GPU-skinned opponent component is not admitted");
                    continue;
                }
                for (U v = 0; v < part.vertexCount; ++v) {
                    for (float value : part.vertices[v].position)
                        if (!std::isfinite(value))
                            return fail(d, VehicleWindowError50DS::frame, "opponent contains nonfinite world-space position");
                    for (float value : part.vertices[v].uv)
                        if (!std::isfinite(value))
                            return fail(d, VehicleWindowError50DS::frame, "opponent contains nonfinite source UV");
                }
            }
        }
        d.opponentIndex = maxVehicleWindowOpponents50DS;
        return true;
    }

bool VehicleWindowGl50DS::Storage::copyNativeGcLights(NativeRacerLightingPolicy50DS policy,
        const game::NativeGcSelectedLightsCandidate* input, U expectedRacerSlot,
        const game::VehicleDrawBatch50CO& batch, U archetype,
        std::uintptr_t& fleetOwner, game::NativeGcSelectedLightsCandidate& output,
        VehicleWindowDiagnostic50DS& d, RacerNormalReceipt& normalReceipt) const noexcept {
        output = {};
        if (archetype > racerArchetypeCount)
            return fail(d, VehicleWindowError50DS::frame,
                "native GC packet requires the actual prepared racer archetype");
        if (!retainedMesh(batch, d, archetype, &normalReceipt)) return false;
        if (policy == NativeRacerLightingPolicy50DS::unchangedHost) {
            if (input)
                return fail(d, VehicleWindowError50DS::frame,
                    "unchanged host lighting requires no selected-light packet");
            for (U c = 0; c < batch.componentCount; ++c)
                if (batch.component[c].sourceColorMaterialModes50LA)
                    return fail(d, VehicleWindowError50DS::frame,
                        "unchanged host lighting cannot consume source-generated racer colors");
            return true;
        }
        if (policy != NativeRacerLightingPolicy50DS::nativeGcSelectedLights || !input ||
            request.mode != VehicleWindowMode50DS::callerMaterialTextures)
            return fail(d, VehicleWindowError50DS::frame,
                "native GC lighting requires explicit textured policy and a selected-light packet");
        // Copy once, then validate/use that copy only. A retained stopped racer
        // may legitimately have an older graphics sequence than another racer;
        // runtime must retain it with the exact geometry batch it describes.
        const game::NativeGcSelectedLightsCandidate packet = *input;
        if (!packet.ready() || packet.componentCount() != batch.componentCount || !packet.graphicsSequence() || packet.racerSlot() != expectedRacerSlot ||
            !packet.publication() || !packet.ownerIdentity() ||
            (fleetOwner && fleetOwner != packet.ownerIdentity()))
            return fail(d, VehicleWindowError50DS::frame,
                "native GC packet requires ready matching racer and shared owner identity");
        const auto selected = racerView(archetype);
        if (!selected.materials)
            return fail(d, VehicleWindowError50DS::material,
                "native GC lighting requires the actual retained material bank");
        for (U c = 0; c < batch.componentCount; ++c) {
            d.component = c;
            // Inspect actual nonempty draw spans only, not unused material
            // table entries. A requested lit opaque untextured surface cannot
            // silently fall back to the fixed-gray host light. Existing unlit,
            // source-prelit and transparent/additive paths remain excluded.
            const auto& geometry = batch.component[c];
            if (geometry.sourceColorMaterialModes50LA &&
                (geometry.sourceColorOwner50LA != packet.ownerIdentity() ||
                 geometry.sourceColorPublication50LA != packet.publication() ||
                 geometry.sourceColorGraphicsSequence50LA != packet.graphicsSequence() ||
                 geometry.sourceColorRacerSlot50LA != packet.racerSlot()))
                return fail(d, VehicleWindowError50DS::frame,
                    "source-generated colors require their exact selected-light publication");
            for (U i = 0; i < geometry.spanCount; ++i) {
                const auto& span = geometry.spans[i];
                if (!span.index_count) continue;
                if (span.material_index >= selected.scenes[c]->materials.size())
                    return fail(d, VehicleWindowError50DS::material,
                        "native GC draw span material is outside its actual racer scene");
                const auto& material = selected.materials[selected.materialOffsets[c] + span.material_index];
                const auto& binding = material.policy;
                if (binding.unlit || binding.sourcePrelit ||
                    binding.blend != VehicleWindowBlend50DS::opaque) continue;
                if (binding.mode != VehicleWindowMaterialMode50DS::requiredTexture ||
                    !material.name || material.textureIndex == assets::unresolvedMaterialTexture50EA) {
                    d.material = span.material_index;
                    d.texture = material.textureIndex;
                    return fail(d, VehicleWindowError50DS::material,
                        "native GC lit opaque draw requires its real retained texture; no host-light fallback");
                }
            }
            const auto* light = packet.component(c);
            if (!light)
                return fail(d, VehicleWindowError50DS::frame,
                    "native GC packet is missing the actual component selection");
            for (U row = 0; row < 4; ++row) {
                for (U lane = 0; lane < 4; ++lane) {
                    const float direction = light->directions[row][lane];
                    const float diffuse = light->diffuse[row][lane];
                    const float specular = light->specular[row][lane];
                    const auto rejectMatrixLane = [&](const char* operation) noexcept {
                        if (d.error == VehicleWindowError50DS::none) {
                            // Failure-only record; %a preserves every finite
                            // binary32 value without changing diagnostic ABI.
                            std::fprintf(stderr,
                                "native selected-light matrix rejection racer=%u component=%u row=%u lane=%u direction=%a diffuse=%a specular=%a\n",
                                static_cast<unsigned>(expectedRacerSlot), static_cast<unsigned>(c),
                                static_cast<unsigned>(row), static_cast<unsigned>(lane),
                                static_cast<double>(direction), static_cast<double>(diffuse),
                                static_cast<double>(specular));
                        }
                        return fail(d, VehicleWindowError50DS::frame, operation);
                    };
                    if (!std::isfinite(direction) || !std::isfinite(diffuse) ||
                        !std::isfinite(specular))
                        return rejectMatrixLane("native selected-light matrix contains a nonfinite lane");
                    if (std::abs(direction) > 1.f)
                        return rejectMatrixLane("native selected-light direction exceeds the admitted unit bound");
                    // These are selected floating-point light coefficients,
                    // not packed color bytes. Light::Init halves authored RGB,
                    // but 1498A0 copies zone sun overrides without halving and
                    // spatial selection divides local RGB by attenuation.
                    // 149A50 folds negative DIFFUSE into sun/ambient only;
                    // signed specular survives unchanged (TA negative spots).
                    // Preserve those values: the native shader consumes only
                    // diffuse RGB / 128, not this retained specular matrix.
                    if (diffuse < 0.f)
                        return rejectMatrixLane("native selected-light diffuse lane was not negative-light folded");
                    if ((row == 3 || lane == 3) && direction != 0.f)
                        return rejectMatrixLane("native GC selected direction fourth row and column must be zero");
                    if ((lane == 3 && diffuse != (row == 3 ? 64.f : 0.f)) ||
                        ((row == 3 || lane == 3) && specular != 0.f))
                        return rejectMatrixLane("native GC selected color rows do not match admitted source layout");
                }
            }
        }
        fleetOwner = packet.ownerIdentity();
        output = packet;
        d.component = maxComponents;
        return true;
    }

bool VehicleWindowGl50DS::Storage::prepareSpecularFrame50LA(const game::VehicleDrawBatch50CO& player,
        const VehicleWindowFrame50DS& frame, VehicleWindowDiagnostic50DS& d,
        RacerNormalReceipt (&normalReceipts)[1 + maxVehicleWindowOpponents50DS]) noexcept {
        if (!hasSpecularPairs50LA && !hasSingleDiffuse50LA) {
            for (U i = 0; i < frame.opponentCount; ++i)
                for (auto* model : frame.opponents[i].sourceBodyModelPackets50LA)
                    if (model) return fail(d, VehicleWindowError50DS::frame,
                        "source opponent model view owner supplied without paired body capability");
            for (auto* model : frame.sourceBodyModelPackets50LA)
                if (model) return fail(d, VehicleWindowError50DS::frame,
                    "source player model view owner supplied without paired body capability");
            if(request.track.rcCrystal)return frame.sourceBodyCamera50LA&&frame.sourceBodyCameraOwner50LA&&
                frame.sourceBodyCameraPublication50LA&&!frame.sourceBodyFog50LA&&!frame.sourceBodyGlobals50LA ||
                fail(d,VehicleWindowError50DS::frame,"RC crystal requires its actual source camera publication");
            return (!frame.sourceBodyCamera50LA && !frame.sourceBodyCameraOwner50LA &&
                !frame.sourceBodyCameraPublication50LA && !frame.sourceBodyFog50LA &&
                !frame.sourceBodyGlobals50LA) ||
                fail(d, VehicleWindowError50DS::frame, "source body camera supplied without paired body capability");
        }
        if (!frame.sourceBodyCamera50LA || !frame.sourceBodyCameraOwner50LA ||
            !frame.sourceBodyCameraPublication50LA || !frame.sourceBodyFog50LA ||
            !frame.sourceBodyGlobals50LA || (hasSpecularPairs50LA && (!specularWorkspace50LA || !frameSpecularUvs50LA)))
            return fail(d, VehicleWindowError50DS::frame, "source body pair requires actual current camera publication");
        // A current source view is independent of the retained/stopped lighting
        // publication. Its exact combined matrix must produce this same raster.
        NativeCameraViewContract50DX contract{};
        contract.provenance = NativeCameraViewProvenance50DX::currentCore49FForOutput;
        contract.width = frame.width; contract.height = frame.height;
        contract.clipDepth = NativeCameraClipDepth50DX::negativeOneToOne;
        NativeCameraMatrix50DX converted{};
        NativeCameraViewError50DX error{};
        const game::CameraViewOutput49F camera = *frame.sourceBodyCamera50LA;
        if (!convertNativeCameraView50DX(camera, contract, converted, error) ||
            std::memcmp(converted.worldViewProjection, frame.worldViewProjection, sizeof(converted.worldViewProjection)))
            return fail(d, VehicleWindowError50DS::frame, "source body camera differs from actual frame world-to-clip matrix");
        if(hasSingleDiffuse50LA){
            if(!frame.sourceDiffuseCamera50LA||!diffuseWorkspace50LA||!frameDiffuseFog50LA||
               !frame.sourceDiffuseCamera50LA->read(frameDiffuseCamera50LA)||
               frameDiffuseCamera50LA.owner!=frame.sourceBodyCameraOwner50LA||
               frameDiffuseCamera50LA.publication!=frame.sourceBodyCameraPublication50LA||
               std::memcmp(frameDiffuseCamera50LA.camera.combined.data(),camera.combined.data(),sizeof(float)*16)||
               frameDiffuseCamera50LA.fog.color30!=frame.sourceBodyFog50LA->color30||
               frameDiffuseCamera50LA.fog.start34!=frame.sourceBodyFog50LA->start34||
               frameDiffuseCamera50LA.fog.end38!=frame.sourceBodyFog50LA->end38)
                return fail(d,VehicleWindowError50DS::frame,"single diffuse current private camera/guard/fog publication rejected");
            frameDiffuseEndpoint50LA=frame.sourceDiffuseCamera50LA;
        }
        game::OriginalRacerSpecularRequest50LA actual{};
        actual.view = camera.view;
        actual.cameraPosition = {camera.position.x, camera.position.y, camera.position.z};
        for (U slot = 0; slot <= frame.opponentCount; ++slot) {
            const U archetype = slot ? frame.opponents[slot - 1].racerArchetype : 0;
            const auto selected = racerView(archetype);
            const auto& batch = slot ? *frame.opponents[slot - 1].batch : player;
            if (!retainedMesh(batch, d, archetype, &normalReceipts[slot])) return false;
            const auto& models = slot ? frame.opponents[slot - 1].sourceBodyModelPackets50LA :
                frame.sourceBodyModelPackets50LA;
            // These four fixed slots preserve each component's own model,
            // semantic normals/root/bounds and stopped-geometry preparation.
            for (U bodySlot = 0; bodySlot < sourceBodyCount50LA; ++bodySlot) {
                frameSpecularSecond50LA[slot][bodySlot] = false;
                auto* model = models[bodySlot];
                frameDiffuseSelected50LA[slot][bodySlot]=true;
                if (!selected.specular[bodySlot].enabled && !selected.specular[bodySlot].singleDiffuse) {
                    if (model) return fail(d, VehicleWindowError50DS::frame,
                        "source model view owner supplied without paired body");
                    continue;
                }
                const U c = bodySlot + 1;
                if (!sourceBodyComponent50LA(c, batch.componentCount))
                    return fail(d, VehicleWindowError50DS::frame,
                        "source body slot has no actual engine in this three/five-model batch");
                game::OriginalRacerModelDistanceFlags50LA modelView{};
                const auto& body = batch.component[c];
                if (!model || !body.sourceSpecularGeometry50LA ||
                    !model->prepareDistanceFlags241280(body.sourceAggregateBounds50LA,
                        body.sourcePosition50LA, actual.cameraPosition, frame.sourceBodyFog50LA->start34,
                        *frame.sourceBodyGlobals50LA, modelView).okay())
                    return fail(d, VehicleWindowError50DS::frame,
                        "actual model radius distance and view flags rejected");
                const U uvSlot = slot * sourceBodyCount50LA + bodySlot;
                if(selected.specular[bodySlot].singleDiffuse){
                    game::OriginalRacerDiffuseClip50LA clip;
                    if(body.sourceGeometryModel50LA!=model||
                       !model->prepareDiffuseSelection240C00(*selected.meshes[c]->semanticScene,
                        body.sourceGeometryRevision50LA,body.sourceRootWorld50LA,body.sourceAggregateBounds50LA,
                        frameDiffuseCamera50LA.camera.combined,frameDiffuseCamera50LA.guardCombined,clip))
                        return fail(d,VehicleWindowError50DS::frame,"single diffuse actual null-observer selection rejected");
                    frameDiffuseSelected50LA[slot][bodySlot]=clip.selected;
                    auto* fog=frameDiffuseFog50LA.get()+uvSlot*game::maxRigidVertices49S;
                    if(clip.selected){
                        if(!game::original_racer_diffuse50LA::prepareFrame(selected.specular[bodySlot].diffuseGeometry,
                            *selected.meshes[c],body,*model,modelView,clip,frameDiffuseCamera50LA,*diffuseWorkspace50LA,
                            fog,game::maxRigidVertices49S))
                            return fail(d,VehicleWindowError50DS::frame,"single diffuse original palette fog operands rejected");
                    }else std::fill_n(fog,body.vertexCount,255.f);
                }
                if(!selected.specular[bodySlot].enabled)continue;
                game::OriginalRacerSpecularResult50LA result{};
                if (body.skin) {
                    if (!game::prepareOriginalRacerSpecularCoefficients50LA(*selected.meshes[c], body, actual, modelView,
                        frameSpecularCoefficients50LA[slot][bodySlot], result))
                        return fail(d, VehicleWindowError50DS::frame, "GPU-skinned body specular coefficients rejected");
                    frameSpecularSecond50LA[slot][bodySlot] = result.secondEnabled;
                    continue;
                }
                if (!game::prepareOriginalRacerSpecular50LA(*selected.meshes[c], body, actual, modelView,
                    frameSpecularUvs50LA.get() + uvSlot * game::maxRigidVertices49S, game::maxRigidVertices49S,
                    *specularWorkspace50LA, result))
                    return fail(d, VehicleWindowError50DS::frame, "actual current camera and retained body specular projection rejected");
                frameSpecularSecond50LA[slot][bodySlot] = result.secondEnabled;
            }
        }
        frameSpecularCamera50LA = actual;
        frameSpecularCameraOwner50LA = frame.sourceBodyCameraOwner50LA;
        frameSpecularCameraPublication50LA = frame.sourceBodyCameraPublication50LA;
        return true;
    }

bool VehicleWindowGl50DS::Storage::uploadSpecularUvs50LA(std::uint8_t bank, U slot, U archetype) noexcept {
        const auto selected = racerView(archetype);
        for (U bodySlot = 0; bodySlot < sourceBodyCount50LA; ++bodySlot) {
            const auto& body = selected.specular[bodySlot];
            if (!body.enabled && !body.singleDiffuse) continue;
            const U c = bodySlot + 1, uvSlot = slot * sourceBodyCount50LA + bodySlot;
            if(body.singleDiffuse){
                game::OriginalRacerDiffuseCameraSnapshot50LA current;
                if(!frameDiffuseEndpoint50LA||!frameDiffuseEndpoint50LA->read(current)||
                   current.owner!=frameDiffuseCamera50LA.owner||current.revision!=frameDiffuseCamera50LA.revision||
                   current.generation!=frameDiffuseCamera50LA.generation||current.publication!=frameDiffuseCamera50LA.publication||
                   current.frame!=frameDiffuseCamera50LA.frame)
                    return fail(*diagnostic,VehicleWindowError50DS::frame,"single diffuse camera publication changed before upload");
                drawGl.bindBuffer(arrayBuffer,body.fogBuffers[bank]);
                drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(selected.meshes[c]->vertexCount)*sizeof(float),
                    frameDiffuseFog50LA.get()+uvSlot*game::maxRigidVertices49S);
                if(!check("upload actual current single diffuse fog bytes"))return false;
            }
            if(!body.enabled)continue;
            if (slot && slot <= frameOpponentCount && frameOpponentBatches[slot - 1].component[c].skin) continue;
            if (!sourceBodyComponent50LA(c, selected.resources->componentCount))
                return fail(*diagnostic, VehicleWindowError50DS::assets,
                    "source map-2 upload has no actual engine in this three/five-model pod");
            drawGl.bindBuffer(arrayBuffer, body.buffers[bank]);
            // CPU preparation still admits every current-camera UV. The same
            // original distance flag prevents all map-2 draws for this body;
            // other passes' results do not depend on this attribute. Every
            // map-2-enabled body uploads its complete stream, even after a skip,
            // bank switch, shared-archetype draw or failed prior submission.
            if (frameSpecularSecond50LA[slot][bodySlot])
                drawGl.bufferSubData(arrayBuffer, 0,
                    std::intptr_t(selected.meshes[c]->vertexCount) * sizeof(std::array<float, 2>),
                    frameSpecularUvs50LA.get() + uvSlot * game::maxRigidVertices49S);
            if (!check("upload source map-2 UVs for this current camera and actual racer body")) return false;
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::uploadRacerNormals(std::uint8_t bank, const game::VehicleDrawBatch50CO& batch,
        U archetype, bool effectsOnly) noexcept {
        const auto selected = racerView(archetype);
        static constexpr assets::WorldNormal50HK absent[game::maxRigidVertices49S]{};
        for (U c = 0; c < batch.componentCount; ++c) {
            const auto& part = batch.component[c];
            if (part.skin) continue; // the vertex shader derives normals from bones
            drawGl.bindBuffer(arrayBuffer, selected.normalBuffers[bank][c]);
            // A complete cold bank with no normal-dependent lighting keeps its
            // verified zero-filled attribute. CPU normals still feed source
            // colors/specular and validation; absent tables stay conservative.
            // Deferred draws select nonopaque materials, making the normal
            // varying output-independent. Preserve the conservative null-table
            // route, every bind/check, and all ordinary opaque uploads.
            if (!selected.materials || (!effectsOnly && selected.materialSummary[c].normalDependent))
                drawGl.bufferSubData(arrayBuffer, 0,
                    std::intptr_t(part.vertexCount) * sizeof(assets::WorldNormal50HK),
                    part.sourceNormals ? part.sourceNormals : absent);
            if (!check("upload current per-node racer source normals")) return false;
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::validateDynamicWorldFrame(const VehicleWindowDynamicWorldFrame50DS& frame,
        VehicleWindowDiagnostic50DS& d) const noexcept {
        if (frame.vertexCount != dynamicWorldVertexCount ||
            bool(frame.vertices) != bool(dynamicWorldVertexCount) ||
            frame.sourceNormalCount != dynamicWorldNormalCount ||
            bool(frame.sourceNormals) != bool(dynamicWorldNormalCount))
            return fail(d, VehicleWindowError50DS::frame,
                "dynamic-world streams must exactly match cold vertex/normal counts");
        for (U v = 0; v < frame.vertexCount; ++v) {
            const auto& vertex = frame.vertices[v];
            for (float value : vertex.position)
                if (!std::isfinite(value))
                    return fail(d, VehicleWindowError50DS::frame, "nonfinite dynamic-world position");
            const auto& source = dynamicWorldSourceVertices[v];
            if (std::memcmp(vertex.uv, source.uv, sizeof(source.uv)) ||
                std::memcmp(vertex.color, source.color, sizeof(source.color)))
                return fail(d, VehicleWindowError50DS::frame,
                    "dynamic-world UV/color must retain exact cold source bytes");
        }
        for (U n = 0; n < frame.sourceNormalCount; ++n) {
            const auto& normal = frame.sourceNormals[n].xyz;
            const double length = std::hypot(normal[0], normal[1], normal[2]);
            if (!std::isfinite(length) || (length != 0.0 && std::abs(length - 1.0) > 0.0001))
                return fail(d, VehicleWindowError50DS::frame,
                    "dynamic-world normals must be finite unit vectors or explicit zero");
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::uploadDynamicWorld(U bank) noexcept {
        if (!dynamicWorldVertexCount) return true;
        drawGl.bindBuffer(arrayBuffer, dynamicWorldVertexBuffers[bank]);
        drawGl.bufferSubData(arrayBuffer, 0,
            std::intptr_t(dynamicWorldVertexCount) * sizeof(assets::Vertex48B), frameDynamicWorld.vertices);
        if (!check("upload current source-animated dynamic-world vertices")) return false;
        diagnostic->dynamicWorldVerticesUploaded = dynamicWorldVertexCount;
        if (dynamicWorldNormalCount) {
            drawGl.bindBuffer(arrayBuffer, dynamicWorldNormalBuffers[bank]);
            drawGl.bufferSubData(arrayBuffer, 0,
                std::intptr_t(dynamicWorldNormalCount) * sizeof(assets::WorldNormal50HK), frameDynamicWorld.sourceNormals);
            if (!check("upload current source-animated dynamic-world normals")) return false;
            diagnostic->dynamicWorldNormalsUploaded = dynamicWorldNormalCount;
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::drawCables(const game::PodCableMesh50FD* cables, U textureName) noexcept {
        if (!cables) return true;
        // The source-generated mesh was preflighted before any frame GL work.
        // Same-context ordering protects this small fixed VBO across frames;
        // old committed images are renderbuffer pixels, not references to it.
        drawGl.bindVertexArray(cableVertexArray);
        drawGl.bindBuffer(arrayBuffer, cableVertexBuffer);
        const U vertexCount = cables->cableCount * game::podCableVerticesPerCable50FD;
        const U indexCount = cables->cableCount * game::podCableIndicesPerCable50FD;
        drawGl.bufferSubData(arrayBuffer, 0, vertexCount * sizeof(cables->vertices[0]), cables->vertices);
        drawGl.bindBuffer(elementBuffer, cableIndexBuffer);
        gl.useProgram(shader->program());
        gl.uniformMatrix4fv(shader->uniforms().worldViewProjection, 1, 0, matrix);
        gl.activeTexture(textureUnit0);
        gl.bindTexture(texture2D, textureName);
        if (gl.bindSampler) gl.bindSampler(0, 0);
        gl.uniform1i(shader->uniforms().useTexture, 1);
        gl.uniform1i(shader->uniforms().image, 0);
        gl.uniform1i(shader->uniforms().useLighting, 0); // Energy cables stay emissive.
        resetNativeGcLighting();
        if (!check("upload actual current source cable vertices and bind real texture")) return false;
        drawGl.drawElementsBaseVertex(0x0004, indexCount, 0x1405, nullptr, 0);
        if (!check("draw source cable strips after pod")) return false;
        ++diagnostic->cableDraws;
        diagnostic->cableVertices += vertexCount;
        diagnostic->cableIndices += indexCount;
        return true;
    }

bool VehicleWindowGl50DS::Storage::drawGameplayBinders() noexcept {
        if(!frameGameplayBinderCount)return true;
        // Native semantic Diffuse material: packetRGBA and original ATM pixels.
        // Existing native world shader handles fog/clipping. Source-A byte
        // testing and independent replacement are explicit; exact source RGB
        // raster quantization and group ordering remain open.
        const auto* bank=request.gameplayBinderTextures;
        if(!bank||!trackProgram||!cableVertexArray)
            return fail(*diagnostic,VehicleWindowError50DS::material,"Binder shader, pixel bank or quad buffers missing");
        const U texture=bank->texture(frameGameplayBinderImage);if(!texture)return false;
        resetPodState();
        gl.useProgram(trackProgram);gl.uniform1i(rcCrystalPassUniform,0);gl.uniform1i(rcCrystalPayloadUniform,0);
        gl.uniformMatrix4fv(trackMatrixUniform,1,0,matrix);
        gl.uniform1f(trackColorScaleUniform,255.f/128.f);
        gl.uniform1i(trackUseTextureUniform,1);gl.uniform1i(trackImageUniform,0);
        gl.uniform1i(trackDiscardZeroAlphaUniform,2); // DefaultMT: ATE1,ATST6,AREF0.
        gl.uniform1f(trackAlphaReferenceUniform,0.f); // Test quantized source A, not texel A.
        gl.uniform1i(trackEffectModeUniform,3);
        gl.uniform1f(trackAlphaScaleUniform,(255.f/128.f)*(255.f/128.f));
        gl.uniform1i(trackUnlitUniform,1); // Diffuse color70 is already source-packed on CPU.
        gl.uniform1i(trackUseSourceNormalsUniform,0);gl.uniform1i(trackProjectionPassUniform,0);
        gl.uniform4f(trackUvTranslationUniform,0.f,0.f,0.f,0.f);
        gl.uniform1i(nativeShadowEnabledUniform,0);gl.uniform1i(nativeShadowOpponentCountUniform,0);
        const auto& fog=request.track.worldFog;
        // Authored fog is on by default; retain the explicit scene-fog
        // diagnostic opt-out without dropping the actual Binder geometry.
        gl.uniform1i(worldFogEnabledUniform,fog.enabled?1:0);
        gl.uniform4f(worldFogCameraUniform,worldFogCamera[0],worldFogCamera[1],worldFogCamera[2],0.f);
        gl.uniform4f(worldFogColorUniform,fog.color[0],fog.color[1],fog.color[2],1.f);
        gl.uniform4f(worldFogRangeUniform,fog.start,fog.end,0.f,0.f);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,texture);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.enable(0x0BE2);gl.blendEquation(0x8006);gl.blendFunc(0x0302,0x0303);
        gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(0);gl.disable(0x0B44);
        drawGl.bindVertexArray(cableVertexArray);drawGl.bindBuffer(arrayBuffer,cableVertexBuffer);
        drawGl.bindBuffer(elementBuffer,cableIndexBuffer);
        bool accepted=true;U alphaDraws=0;
        for(U i=0;accepted&&i<frameGameplayBinderCount;++i){
            drawGl.bufferSubData(arrayBuffer,0,sizeof(frameGameplayBinders[i].vertices),frameGameplayBinders[i].vertices.data());
            // The first canonical cable segment has the SAME source quad
            // triangles0,1,2 /2,1,3; its cold36-vertex storage is ample for4.
            gl.enable(0x0BE2);gl.colorMask(1,1,1,0);gl.uniform1i(trackEffectModeUniform,3);
            drawGl.drawElementsBaseVertex(0x0004,6,0x1405,nullptr,0);
            accepted=check("draw original gameplay Binder RGB quad");
            if(accepted){
                ++diagnostic->gameplayBinderDraws;
                // The genuine material uses source alpha and DATE0. Its RGB
                // has no destination-A dependency, so a same-geometry A-only
                // store after this quad preserves source ordering. Depth is
                // unchanged: both passes use LEQUAL with writes disabled.
                gl.disable(0x0BE2);gl.colorMask(0,0,0,1);gl.uniform1i(trackEffectModeUniform,4);
                drawGl.drawElementsBaseVertex(0x0004,6,0x1405,nullptr,0);
                accepted=check("store original gameplay Binder source alpha");
                if(accepted)++alphaDraws;
            }
        }
        if(accepted&&!gameplayBinderFirstDrawRecorded){
            // Query the RGB-pass state, not the final alpha-only store state.
            gl.enable(0x0BE2);gl.colorMask(1,1,1,0);gl.uniform1i(trackEffectModeUniform,3);
            // One startup observation per renderer, not a per-frame trace or
            // a replacement shading policy. Capture the real player packet
            // and current GL state before resetting the shared world program.
            I program=0,activeUnit=0,boundTexture=0,sourceBlend=0,destinationBlend=0;
            I vertexArray=0,vertexBuffer=0,indexBuffer=0;
            I depthFunction=0,depthWrite=0,colorWrite[4]={0,0,0,0},sampler=-1;
            I filters[2]={-1,-1},wraps[2]={-1,-1},extent[2]={-1,-1};
            gl.getIntegerv(0x8B8D,&program); // CURRENT_PROGRAM
            gl.getIntegerv(0x84E0,&activeUnit); // ACTIVE_TEXTURE
            gl.getIntegerv(0x8069,&boundTexture); // TEXTURE_BINDING_2D
            gl.getIntegerv(0x85B5,&vertexArray); // VERTEX_ARRAY_BINDING
            gl.getIntegerv(0x8894,&vertexBuffer);gl.getIntegerv(0x8895,&indexBuffer);
            gl.getIntegerv(0x80C9,&sourceBlend);gl.getIntegerv(0x80C8,&destinationBlend);
            gl.getIntegerv(0x0B74,&depthFunction);gl.getIntegerv(0x0B72,&depthWrite);
            gl.getIntegerv(0x0C23,colorWrite); // COLOR_WRITEMASK
            // Sampler objects are not required by GL3.2. Never query their
            // enum on a context where the existing capability was not loaded.
            if(gl.bindSampler)gl.getIntegerv(0x8919,&sampler);
            if(gl.getTexParameteriv){
                gl.getTexParameteriv(texture2D,0x2801,&filters[0]);
                gl.getTexParameteriv(texture2D,0x2800,&filters[1]);
                gl.getTexParameteriv(texture2D,0x2802,&wraps[0]);
                gl.getTexParameteriv(texture2D,0x2803,&wraps[1]);
            }
            if(gl.getTexLevelParameteriv){
                gl.getTexLevelParameteriv(texture2D,0,0x1000,&extent[0]);
                gl.getTexLevelParameteriv(texture2D,0,0x1001,&extent[1]);
            }
            accepted=check("observe first original gameplay Binder material state");
            if(accepted){
                const auto& rgba=frameGameplayBinders[0].vertices[0].color;
                std::fprintf(stderr,"gameplay_binder_first_draw frame=%u quads=%u alpha_draws=%u cpu_player_rgba=%u,%u,%u,%u "
                    "program=%d expected_program=%u active_unit=0x%x texture=%d expected_texture=%u "
                    "vao=%d expected_vao=%u vbo=%d expected_vbo=%u ebo=%d expected_ebo=%u "
                    "sampler=%d min_mag=%d,%d wrap_st=%d,%d texture_extent=%d,%d "
                    "blend_src_dst=0x%x,0x%x depth_func=0x%x depth_write=%d color_write=%d,%d,%d,%d "
                    "fog=%d fog_range=%.9g,%.9g fog_rgb=%.9g,%.9g,%.9g "
                    "requested_rgb_scale=255/128 requested_blend_alpha=source_byte/128 requested_store_alpha=source_byte/255 requested_unlit=1 rgb_pass_state=1 observation_only=1\n",
                    frameGameplayBinderImage,frameGameplayBinderCount,alphaDraws,unsigned(rgba[0]),unsigned(rgba[1]),
                    unsigned(rgba[2]),unsigned(rgba[3]),program,trackProgram,unsigned(activeUnit),boundTexture,
                    texture,vertexArray,cableVertexArray,vertexBuffer,cableVertexBuffer,indexBuffer,cableIndexBuffer,
                    sampler,filters[0],filters[1],wraps[0],wraps[1],extent[0],extent[1],
                    unsigned(sourceBlend),unsigned(destinationBlend),unsigned(depthFunction),depthWrite,
                    colorWrite[0],colorWrite[1],colorWrite[2],colorWrite[3],fog.enabled?1:0,
                    double(fog.start),double(fog.end),double(fog.color[0]),double(fog.color[1]),double(fog.color[2]));
                gameplayBinderFirstDrawRecorded=true;
            }
        }
        // Restore shared masks and selectors even if an upload, RGB draw,
        // alpha store or first-draw observation failed. Never publish half of
        // the two-pass source operation; the caller retains transaction abort.
        gl.colorMask(1,1,1,1);gl.uniform1i(trackEffectModeUniform,0);
        gl.uniform1i(trackDiscardZeroAlphaUniform,0);gl.uniform1f(trackAlphaReferenceUniform,0.f);
        resetPodState();const bool restored=check("restore native world state after Binder");
        return accepted&&restored;
    }

bool VehicleWindowGl50DS::Storage::buildRacerPodShadows(const game::VehicleDrawBatch50CO& batch, U archetype,
        NativePodShadow50HM (&output)[3]) noexcept {
        const auto selected = racerView(archetype);
        for (U c = 0; c < 3; ++c) {
            const auto& part = batch.component[c];
            std::uint64_t opaque = 0;
            for (U span = 0; span < part.spanCount; ++span) {
                bool casts = true;
                if (request.mode == VehicleWindowMode50DS::callerMaterialTextures) {
                    const auto& policy = selected.materials[selected.materialOffsets[c] +
                        part.spans[span].material_index].policy;
                    casts = policy.blend == VehicleWindowBlend50DS::opaque && policy.depthWrite;
                }
                if (casts) opaque |= std::uint64_t{1} << span;
            }
            const bool built = part.skin && selected.skin50LA && selected.skin50LA[c].eligible ?
                racerSkinShadow50LA(part, selected.skin50LA[c], output[c]) :
                buildNativePodShadow50HM(part, opaque, output[c]);
            if (!built) {
                diagnostic->component = c;
                return fail(*diagnostic, VehicleWindowError50DS::assets, "native pod shadow source geometry rejected");
            }
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::ready(void* context, const VehicleGlResources50CV& resources, std::uint8_t bank,
        const game::VehicleDrawBatch50CO& batch, const game::VehicleDrawPlan50CS&) noexcept {
        auto& s = *static_cast<Storage*>(context);
        // Check exact current context/drawable before even querying GL errors.
        // Cold-verified private object storage remains unchanged by contract.
        if (!s.current(*s.diagnostic) || !s.texturesCurrent(*s.diagnostic)) return false;
        if (!s.initialized || s.active || !s.frameSet || bank != (s.published ^ 1u) ||
            !s.shader->program() || !s.targets[bank]) return false;
        if (s.coverageDemand && (!s.coverageLive.scene[bank] ||
            s.coverageLive.width != s.width || s.coverageLive.height != s.height))
            return fail(*s.diagnostic, VehicleWindowError50DS::frame, "exact native coverage extent is not prepared");
        if (!s.retainedMesh(batch, *s.diagnostic)) return false;
        if (s.trackIndexCount && (!s.trackVertexBuffer || !s.trackIndexBuffer || !s.trackVertexArray))
            return fail(*s.diagnostic, VehicleWindowError50DS::assets, "static track resources are not ready");
        for (U i = 0; i < s.trackAnimationCount; ++i) {
            const auto& animation = s.trackAnimations[i];
            const auto selected = animation.selectedFrame;
            if (selected >= animation.frameCount)
                return fail(*s.diagnostic, VehicleWindowError50DS::frame, "track animation frame is not prepared");
            const auto* image = s.request.trackAnimationTextures->asset(animation.firstTextureIndex+selected);
            if (!image || image->name != animation.names[selected])
                return fail(*s.diagnostic, VehicleWindowError50DS::material,
                    "selected original animation frame no longer matches retained publication");
        }
        if (s.dynamicWorldIndexCount && (!s.dynamicWorldIndexBuffer ||
            !s.dynamicWorldVertexBuffers[bank] || !s.dynamicWorldArrays[bank] ||
            (s.dynamicWorldNormalCount && !s.dynamicWorldNormalBuffers[bank])))
            return fail(*s.diagnostic, VehicleWindowError50DS::assets, "dynamic-world resources are not ready");
        for (U i = 0; i < s.dynamicWorldSpanCount; ++i) {
            const auto& material = s.dynamicWorldSpans[i].material;
            if (!material.name) continue;
            const auto* texture = s.request.dynamicWorld.textures->asset(material.textureIndex);
            if (!texture || texture->name != material.name) {
                s.diagnostic->dynamicWorldSpan = i;
                s.diagnostic->texture = material.textureIndex;
                return fail(*s.diagnostic, VehicleWindowError50DS::material,
                    "required dynamic-world texture is no longer the retained publication");
            }
        }
        if (s.usesTrackShader() && !s.trackProgram)
            return fail(*s.diagnostic, VehicleWindowError50DS::shader, "source-color track program is not ready");
        if (s.frameHostHud && (!s.request.enableHostHud || !s.hostHudGeometry ||
            !s.hostHudProgram || !s.hostHudVertexBuffer || !s.hostHudIndexBuffer || !s.hostHudVertexArray))
            return fail(*s.diagnostic, VehicleWindowError50DS::frame, "host HUD resources are not ready");
        if (s.request.cables.textures) {
            const auto* texture = s.request.cables.textures->asset(s.request.cables.textureIndex);
            if (!s.frameCables || !s.cableVertexBuffer || !s.cableIndexBuffer || !s.cableVertexArray ||
                !texture || !texture->name || texture->name != s.cableTextureName)
                return fail(*s.diagnostic, VehicleWindowError50DS::material, "source cable resources are not ready");
        }
        for (U i = 0; i < s.trackSpanCount; ++i) {
            const auto& material = s.trackSpans[i].material;
            if (!material.name) continue; // Explicit source-color-only surface.
            const auto* texture = s.request.track.textures->asset(material.textureIndex);
            if (!texture || texture->name != material.name) {
                s.diagnostic->trackSpan = i;
                s.diagnostic->texture = material.textureIndex;
                return fail(*s.diagnostic, VehicleWindowError50DS::material,
                    "required track span texture is no longer the retained publication");
            }
        }
        if (s.skyIndexCount && (!s.skyVertexBuffer || !s.skyIndexBuffer || !s.skyVertexArray))
            return fail(*s.diagnostic, VehicleWindowError50DS::assets, "authored sky resources are not ready");
        for (U i = 0; i < s.skySpanCount; ++i) {
            const auto& material = s.skySpans[i].material;
            if (!material.name) continue;
            const auto* texture = s.request.sky.textures->asset(material.textureIndex);
            if (!texture || texture->name != material.name)
                return fail(*s.diagnostic, VehicleWindowError50DS::material,
                    "required sky texture is no longer the retained publication");
        }
        for (U c = 0; c < resources.componentCount; ++c) {
            const auto& mesh = *s.meshes[c];
            const auto& b = batch.component[c];
            if (b.indices != mesh.indices || b.indexCount != mesh.indexCount ||
                b.spans != mesh.spans || b.spanCount != mesh.spanCount || b.vertexCount != mesh.vertexCount ||
                resources.indices[c].buffer != s.resources.indices[c].buffer)
                return fail(*s.diagnostic, VehicleWindowError50DS::assets,
                    "batch is not the retained prepared mesh");
        }
        for (auto& racer : s.podShadows)
            for (auto& shadow : racer) shadow = {};
        if (s.request.enableNativePodShadows) {
            if (!s.framePlayerHidden && !s.buildRacerPodShadows(batch, 0, s.podShadows[0])) return false;
            for (U i = 0; i < s.frameOpponentCount; ++i) {
                s.diagnostic->opponentIndex = i;
                if (s.frameOpponentHidden[i]) { for (auto& v : s.podShadowOpponentBounds[i]) v = 0.f; continue; } // destroyed: no caster
                if (!s.buildRacerPodShadows(s.frameOpponentBatches[i], s.frameOpponentArchetypes[i],
                    s.podShadows[i + 1])) return false;
                if (!buildNativePodShadowReceiverBounds50HM(s.podShadows[i + 1], s.podShadowOpponentBounds[i]))
                    return fail(*s.diagnostic, VehicleWindowError50DS::assets,
                        "native opponent shadow receiver bounds rejected");
            }
            s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        }
        return s.check("before vehicle preflight");
    }

bool VehicleWindowGl50DS::Storage::materialReady(void* context, game::VehicleDrawComponent50CM component, U material) noexcept {
        auto& s = *static_cast<Storage*>(context);
        const U c = componentIndex(component);
        if (c >= s.resources.componentCount || material >= s.scenes[c]->materials.size())
            return fail(*s.diagnostic, VehicleWindowError50DS::assets,
                "material is outside loaded component scene");
        if (s.request.mode == VehicleWindowMode50DS::callerMaterialTextures) {
            const auto& binding = s.materials[s.materialOffsets[c] + material];
            if (binding.name) {
                const auto* texture = s.request.materials.textures->asset(binding.textureIndex);
                if (!texture || texture->name != binding.name) {
                    s.diagnostic->component = c;
                    s.diagnostic->material = material;
                    s.diagnostic->texture = binding.textureIndex;
                    return fail(*s.diagnostic, VehicleWindowError50DS::material,
                        "required material texture is no longer the retained publication");
                }
            }
        }
        return true;
    }

I VehicleWindowGl50DS::Storage::effectMode(const VehicleWindowMaterialBinding50DS& policy) noexcept {
        return policy.blend == VehicleWindowBlend50DS::opaque ? 0 : (policy.fixedBlendAlpha ? 2 : 1);
    }

void VehicleWindowGl50DS::Storage::configureBlend(const VehicleWindowMaterialBinding50DS& policy, bool layeredDepth) noexcept {
        gl.enable(0x0B71);
        gl.depthMask(policy.depthWrite ? 1 : 0);
        gl.depthFunc(layeredDepth || policy.depthEqual ? 0x0203 : 0x0201);
        if (policy.blend == VehicleWindowBlend50DS::opaque) gl.disable(0x0BE2);
        else {
            gl.enable(0x0BE2);
            gl.blendEquation(0x8006); // FUNC_ADD; never inherit caller equations.
            gl.blendFunc(policy.fixedBlendAlpha ? 1 : 0x0302,
                policy.blend == VehicleWindowBlend50DS::additive ? 1 : 0x0303);
        }
    }

void VehicleWindowGl50DS::Storage::unbindSpecularFeedback50LA() noexcept {
        if (!hasSpecularPairs50LA) return;
        gl.activeTexture(textureUnit0 + 1); gl.bindTexture(texture2D, 0);
        if (gl.bindSampler) gl.bindSampler(1, 0);
        gl.activeTexture(textureUnit0);
    }

void VehicleWindowGl50DS::Storage::resetNativeGcLighting() noexcept {
        coverageAa0();
        // This capability owns no gameplay shader or coefficients to clear.
        // The UI path immediately binds its own complete shader/state below.
        if(frontendOnly)return;
        // Both lighting selectors and coefficients are explicitly cleared at
        // renderer transitions, including cable/sky/track and an aborted racer
        // frame followed by UI. Source body MODULATE must never leak to them.
        gl.useProgram(shader->program());
        const auto& u = shader->uniforms();
        gl.uniform1i(u.useNativeGcSelectedLights, 0);
        gl.uniform1i(u.useSourceDiffuseCompressed50LA, 0);
        gl.uniform1i(u.sourcePairPass50LA, 0); gl.uniform1i(u.sourcePairFog50LA, 0);
        gl.uniform1i(u.sourcePairAlphaTest50LA, 0);
        gl.uniform1i(u.sourceSingleDiffuseRgba50LA,0);
        gl.uniform1i(u.sourceSingleFogEnabled50LA,0);
        gl.uniform1i(u.sourceSingleAlphaTest50LA,0);
        gl.uniform1i(u.sourcePairDestination50LA, hasSpecularPairs50LA ? 1 : 0);
        unbindSpecularFeedback50LA();
        gl.uniform4f(u.nativeGcAmbient, 0.f, 0.f, 0.f, 0.f);
        for (U i = 0; i < 3; ++i) {
            gl.uniform4f(u.nativeGcDiffuse[i], 0.f, 0.f, 0.f, 0.f);
            gl.uniform4f(u.nativeGcDirection[i], 0.f, 0.f, 0.f, 0.f);
        }
    }

void VehicleWindowGl50DS::Storage::resetPodState() noexcept {
        // Explicitly clear the shared world shader before any pod/sky/UI path.
        if (trackProgram) {
            gl.useProgram(trackProgram);
            gl.uniform1i(worldFogEnabledUniform, 0);
            gl.uniform1i(rcCrystalPassUniform,0);gl.uniform1i(rcCrystalPayloadUniform,0);
        }
        gl.disable(0x0BE2); gl.enable(0x0B71); gl.depthMask(1); gl.depthFunc(0x0201);
        gl.useProgram(shader->program());
        const auto& u = shader->uniforms();
        gl.uniformMatrix4fv(u.worldViewProjection, 1, 0, matrix);
        gl.uniform1i(u.useTexture, 0); gl.uniform1i(u.image, 0);
        gl.uniform1i(u.effectMode, 0); gl.uniform1f(u.alphaScale, 1.f);
        gl.uniform1i(u.suppressDraw, 0); gl.uniform1i(u.useUvTransform, 0);
        gl.uniform1i(u.useLighting, 0);
        gl.uniform1i(u.skinMode50LA, 0); gl.uniform1i(u.skinDiffuseSelected50LA, 0);
        resetNativeGcLighting();
    }

bool VehicleWindowGl50DS::Storage::drawSky() noexcept {
        if (!skyIndexCount || !frameSkyEnabled) return true;
        // Genuine source sky, separate caller camera transform, color only.
        // Nothing here scales geometry or manufactures missing faces.
        resetPodState();
        gl.disable(0x0B71); gl.depthMask(0);
        gl.uniformMatrix4fv(shader->uniforms().worldViewProjection, 1, 0, skyMatrix);
        gl.uniform1i(shader->uniforms().effectMode, 3); // Unlit source RGB, opaque.
        drawGl.bindVertexArray(skyVertexArray);
        drawGl.bindBuffer(elementBuffer, skyIndexBuffer);
        for (U i = 0; i < skySpanCount; ++i) {
            const auto& span = skySpans[i];
            gl.bindTexture(texture2D, span.material.name);
            gl.uniform1i(shader->uniforms().useTexture, span.material.name ? 1 : 0);
            if (!check("bind actual authored sky tile")) return false;
            drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.indexCount), 0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(span.indexStart) * sizeof(U)), 0);
            if (!check("draw actual authored sky tile")) return false;
            ++diagnostic->skyDraws; diagnostic->skyIndices += span.indexCount;
        }
        resetPodState();
        return check("restore world transform and depth after sky");
    }

bool VehicleWindowGl50DS::Storage::drawTrack(bool transparent) noexcept {
        auto& s = *this;
        resetNativeGcLighting();
        if (s.frameTrackDrawCount && (!transparent || s.trackSpanCount)) {
            // The entire draw is still unpublished and abortable. The following
            // vehicle draws retain this exact target, matrix, and written depth.
            if (s.request.track.mode == VehicleWindowMode50DS::sourceVertexColors ||
                s.request.track.mode == VehicleWindowMode50DS::callerMaterialTextures) {
                s.gl.useProgram(s.trackProgram);
                const auto& fog = s.request.track.worldFog;
                s.gl.uniform1i(s.worldFogEnabledUniform, 0);
                s.gl.uniform4f(s.worldFogCameraUniform, s.worldFogCamera[0], s.worldFogCamera[1], s.worldFogCamera[2], 0.f);
                s.gl.uniform4f(s.worldFogColorUniform, fog.color[0], fog.color[1], fog.color[2], 1.f);
                s.gl.uniform4f(s.worldFogRangeUniform, fog.start, fog.end, 0.f, 0.f);
                s.gl.uniformMatrix4fv(s.trackMatrixUniform, 1, 0, s.matrix);
                s.gl.uniform1f(s.trackColorScaleUniform, s.request.track.vertexColorScale);
                s.gl.uniform1i(s.trackUseTextureUniform, 0);
                s.gl.uniform1i(s.trackImageUniform, 0);
                s.gl.uniform1i(s.trackDiscardZeroAlphaUniform, 0);
                s.gl.uniform1i(s.trackEffectModeUniform, 0);
                s.gl.uniform1f(s.trackAlphaScaleUniform, 1.f);
                s.gl.uniform1i(s.trackUnlitUniform, 0);
                s.gl.uniform1i(s.trackUseSourceNormalsUniform, s.trackNormalBuffer ? 1 : 0);
                s.gl.uniform1i(s.trackProjectionPassUniform, 0);
                s.gl.uniform1i(s.rcCrystalPassUniform,0);s.gl.uniform1i(s.rcCrystalPayloadUniform,0);
                s.gl.uniform4f(s.rcCrystalAmbientUniform,s.rcCrystalAmbient[0],s.rcCrystalAmbient[1],s.rcCrystalAmbient[2],s.rcCrystalAmbient[3]);
                s.gl.uniform4f(s.trackUvTranslationUniform,0.f,0.f,0.f,0.f);
                s.gl.uniform1i(s.nativeShadowEnabledUniform,
                    s.request.enableNativePodShadows && !transparent ? 1 : 0);
                s.gl.uniform1i(s.nativeShadowOpponentCountUniform,
                    s.request.enableNativePodShadows && !transparent ? static_cast<I>(s.frameOpponentCount) : 0);
                // One upload for the opaque track pass. Inactive slots are
                // excluded by the explicit count, including after a roster
                // shrinks; alpha/unlit/depth policy still disables all shadows.
                if (s.request.enableNativePodShadows && !transparent) {
                    for (U racer = 0; racer <= s.frameOpponentCount; ++racer) {
                        for (U c = 0; c < 3; ++c) {
                            const U caster = 3 * racer + c;
                            const auto& center = s.podShadows[racer][c].centerAndReach;
                            const auto& shape = s.podShadows[racer][c].shapeAndOpacity;
                            s.gl.uniform4f(s.nativeShadowCenterUniforms[caster], center[0], center[1], center[2], center[3]);
                            s.gl.uniform4f(s.nativeShadowShapeUniforms[caster], shape[0], shape[1], shape[2], shape[3]);
                        }
                    }
                    for (U i = 0; i < s.frameOpponentCount; ++i) {
                        const auto& bounds = s.podShadowOpponentBounds[i];
                        s.gl.uniform4f(s.nativeShadowOpponentBoundsUniforms[i], bounds[0], bounds[1], bounds[2], bounds[3]);
                    }
                }
            }
            s.drawGl.bindVertexArray(s.trackVertexArray);
            s.drawGl.bindBuffer(elementBuffer, s.trackIndexBuffer);
            if (!s.check("bind static track geometry")) return false;
            if (s.trackSpanCount) {
                // Stable source order within each explicit pass. No distance
                // sorting, index rebasing or reconstructed global-sort claim.
                U boundSpan = assets::unresolvedMaterialTexture50EA;
                // Only this uninterrupted track pass owns these material bindings.
                // Do not infer state from pass setup, another program, or a prior frame.
                I trackIntegerValues[8]{};
                U trackTextureName = 0;
                std::uint8_t trackAlphaReference = 0;
                unsigned char trackAlphaScaleBytes[sizeof(float)]{};
                bool trackMaterialKnown = false;
                // Raster state belongs only to this uninterrupted track pass.
                // Opaque tuples canonicalize blend factors that are not applied.
                struct TrackRasterState {
                    bool depthWrite = false, blendEnabled = false;
                    U depthFunction = 0, sourceFactor = 0, destinationFactor = 0;
                    bool same(const TrackRasterState& other) const noexcept {
                        return depthWrite == other.depthWrite && blendEnabled == other.blendEnabled &&
                            depthFunction == other.depthFunction && sourceFactor == other.sourceFactor &&
                            destinationFactor == other.destinationFactor;
                    }
                };
                TrackRasterState trackRasterState{};
                bool trackRasterKnown = false;
                for (U n = 0; n < s.frameTrackDrawCount; ++n) {
                    const U draw = s.frameTrackCullingApplied ? s.trackVisibleDrawIds[n] : n;
                    const U i = s.trackChunkCount ? s.trackChunks[draw].sourceSpan : draw;
                    const auto& span = s.trackSpans[i];
                    // Layered materials stay adjacent to their first source pass.
                    // Their alpha layer is not postponed into a separate world pass.
                    const bool deferredAlpha = span.sourcePassCount == 1 &&
                        span.material.policy.blend != VehicleWindowBlend50DS::opaque;
                    if (deferredAlpha != transparent) continue;
                    s.diagnostic->trackSpan = i;
                    s.diagnostic->texture = span.material.textureIndex;
                    // Retained chunks of one span remain contiguous. Preserve
                    // its exact material policy without rebinding every chunk.
                    if (boundSpan != i) {
                        // The public UV callback has no GL-state purity contract.
                        // Emit every value on its span and invalidate for the next span,
                        // even though the current MA controller callback is CPU-only.
                        const bool reuseMaterial = trackMaterialKnown && !span.sourceUvAnimation;
                        const auto integerUniform = [&](U slot, I location, I value) {
                            if (!reuseMaterial || trackIntegerValues[slot] != value)
                                s.gl.uniform1i(location, value);
                            trackIntegerValues[slot] = value;
                        };
                        const U textureName = span.animation == assets::unresolvedMaterialTexture50EA ?
                            span.material.name : s.trackAnimations[span.animation].names[
                                s.trackAnimations[span.animation].selectedFrame];
                        if (!reuseMaterial || trackTextureName != textureName)
                            s.gl.bindTexture(texture2D, textureName);
                        integerUniform(0, s.trackUseTextureUniform, span.material.name ? 1 : 0);
                        integerUniform(1, s.trackDiscardZeroAlphaUniform, span.sourceAlphaTest ? 2 : (span.discardZeroAlpha ? 1 : 0));
                        if (!reuseMaterial || trackAlphaReference != span.sourceAlphaReference)
                            s.gl.uniform1f(s.trackAlphaReferenceUniform,static_cast<float>(span.sourceAlphaReference));
                        integerUniform(2, s.worldFogEnabledUniform,
                            s.request.track.worldFog.enabled && span.worldFogEligible ? 1 : 0);
                        // Establish the final authored depth rule once, before drawing.
                        // Coplanar later passes use LEQUAL; single passes retain policy.
                        const auto& rasterPolicy = span.material.policy;
                        const bool blendEnabled = !span.rcCrystalPass && rasterPolicy.blend != VehicleWindowBlend50DS::opaque;
                        const TrackRasterState nextRasterState{
                            rasterPolicy.depthWrite, blendEnabled,
                            (span.sourcePass > 1 || rasterPolicy.depthEqual) ? U{0x0203} : U{0x0201},
                            blendEnabled ? (rasterPolicy.fixedBlendAlpha ? U{1} : U{0x0302}) : U{0},
                            blendEnabled ? (rasterPolicy.blend == VehicleWindowBlend50DS::additive ?
                                U{1} : U{0x0303}) : U{0}};
                        // The UV callback below has no GL-state purity contract:
                        // force this span and leave the next span unknown.
                        if (span.sourceUvAnimation) trackRasterKnown = false;
                        if (!trackRasterKnown || !trackRasterState.same(nextRasterState)){
                            if(span.rcCrystalPass){s.gl.disable(0x0BE2);s.gl.depthMask(0);s.gl.depthFunc(0x0203);}
                            else s.configureBlend(span.material.policy, span.sourcePass > 1);
                        }
                        integerUniform(7,s.rcCrystalPassUniform,span.rcCrystalPass);
                        integerUniform(3, s.trackProjectionPassUniform, static_cast<I>(span.projectionPass));
                        float uOffset=0.f,vOffset=0.f;
                        if(span.sourceUvAnimation && (!s.frameTrackUv.service ||
                            !s.frameTrackUv.service(s.frameTrackUv.owner,i,uOffset,vOffset) ||
                            !std::isfinite(uOffset) || !std::isfinite(vOffset)))
                            return fail(*s.diagnostic,VehicleWindowError50DS::frame,
                                "source track UV controller rejected selected material service");
                        // Reuse only the exact zero tuple of consecutive unanimated spans.
                        if (!reuseMaterial)
                            s.gl.uniform4f(s.trackUvTranslationUniform,uOffset,vOffset,0.f,0.f);
                        integerUniform(4, s.trackEffectModeUniform, effectMode(span.material.policy));
                        // Byte identity preserves signed zero without changing the value.
                        if (!reuseMaterial || std::memcmp(trackAlphaScaleBytes,
                            &span.material.policy.alphaScale, sizeof(float)) != 0)
                            s.gl.uniform1f(s.trackAlphaScaleUniform, span.material.policy.alphaScale);
                        integerUniform(5, s.trackUnlitUniform,
                            span.material.policy.unlit || span.material.policy.sourcePrelit ? 1 : 0);
                        integerUniform(6, s.nativeShadowEnabledUniform, s.request.enableNativePodShadows &&
                            !transparent && span.allowPodShadow && span.sourcePass == 1 && span.material.policy.depthWrite && !span.material.policy.unlit ? 1 : 0);
                        if (!s.check("bind explicit source-ordered track material")) return false;
                        trackRasterState = nextRasterState;
                        trackRasterKnown = !span.sourceUvAnimation;
                        trackTextureName = textureName;
                        trackAlphaReference = span.sourceAlphaReference;
                        std::memcpy(trackAlphaScaleBytes, &span.material.policy.alphaScale, sizeof(float));
                        trackMaterialKnown = !span.sourceUvAnimation;
                        boundSpan = i;
                    }
                    // Coverage binding mutates neither depth nor blend. Keep it
                    // independent of the root-owned pass-local tuple cache.
                    s.bindTrackCoverage(span.nativeCoverage);
                    const U indexStart = s.trackChunkCount ? s.trackChunks[draw].indexStart : span.indexStart;
                    const U indexCount = s.trackChunkCount ? s.trackChunks[draw].indexCount : span.indexCount;
                    const auto offset = std::uintptr_t(indexStart) * sizeof(U);
                    if(span.rcCrystalPass){
                        if(!s.drawRcCrystalRange(s.activeFrameBank,indexStart,indexCount,span.rcCrystalPass,trackTextureName))return false;
                        // Bridge restores the exact source program/material/raster tuple.
                    }else s.drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(indexCount),
                        0x1405, reinterpret_cast<const void*>(offset), 0);
                    if (!s.check("draw actual track surface range")) return false;
                    if(indexCount)s.trackSpans[i].submittedGameplayCandidate=true;
                    ++s.diagnostic->trackDraws;
                    if (span.material.policy.blend != VehicleWindowBlend50DS::opaque) ++s.diagnostic->transparentTrackDraws;
                    if (span.material.name) ++s.diagnostic->trackTexturedDraws;
                    else ++s.diagnostic->trackUntexturedDraws;
                    s.diagnostic->trackIndices += indexCount;
                }
                s.diagnostic->trackSpan = s.diagnostic->texture = assets::unresolvedMaterialTexture50EA;
            } else {
                s.drawGl.drawElementsBaseVertex(0x0004, s.trackIndexCount, 0x1405, nullptr, 0);
                if (!s.check("draw world-space track before vehicle")) return false;
                s.diagnostic->trackDraws = s.diagnostic->trackUntexturedDraws = 1;
                s.diagnostic->trackIndices = static_cast<U>(s.trackIndexCount);
            }
        }
        resetPodState();
        return s.check("restore opaque vehicle state after track pass");
    }

bool VehicleWindowGl50DS::Storage::drawDynamicWorld(U bank, bool transparent) noexcept {
        if (!dynamicWorldSpanCount) return true;
        resetNativeGcLighting();
        gl.useProgram(trackProgram);
        gl.uniform1i(rcCrystalPassUniform,0);gl.uniform1i(rcCrystalPayloadUniform,0);
        gl.uniform1i(worldFogEnabledUniform, 0);
        gl.uniformMatrix4fv(trackMatrixUniform, 1, 0, matrix);
        gl.uniform1f(trackColorScaleUniform, request.dynamicWorld.vertexColorScale);
        gl.uniform1i(trackImageUniform, 0);
        gl.uniform1i(trackUseSourceNormalsUniform, dynamicWorldNormalCount ? 1 : 0);
        gl.uniform1i(trackProjectionPassUniform, 0);
        gl.uniform4f(trackUvTranslationUniform,0.f,0.f,0.f,0.f);
        // Static receiver/culling bounds are deliberately not reused for moving owners.
        gl.uniform1i(nativeShadowEnabledUniform, 0);
        gl.uniform1i(nativeShadowOpponentCountUniform, 0);
        drawGl.bindVertexArray(dynamicWorldArrays[bank]);
        drawGl.bindBuffer(elementBuffer, dynamicWorldIndexBuffer);
        // Local to this pass: the first submitted span always establishes state,
        // including texture zero. No call in the loop switches texture units,
        // binds/deletes textures, or changes the texture-use uniform elsewhere.
        U boundTexture = 0;
        bool textureBound = false;
        // Cache exactly configureBlend's inputs for this owned loop only.
        // No intervening call changes blend/depth state or mutates policies.
        const VehicleWindowMaterialBinding50DS* boundPolicy = nullptr;
        bool boundLayeredDepth = false;
        for (U i = 0; i < dynamicWorldSpanCount; ++i) {
            const auto& span = dynamicWorldSpans[i];
            const auto& material = span.material;
            const bool deferred = span.sourcePassCount == 1 &&
                material.policy.blend != VehicleWindowBlend50DS::opaque;
            if (deferred != transparent) continue;
            diagnostic->dynamicWorldSpan = i;
            diagnostic->texture = material.textureIndex;
            const auto& policy = material.policy;
            const bool layeredDepth = span.sourcePass > 1;
            if (!boundPolicy || boundPolicy->blend != policy.blend ||
                boundPolicy->depthWrite != policy.depthWrite ||
                boundPolicy->depthEqual != policy.depthEqual ||
                boundPolicy->fixedBlendAlpha != policy.fixedBlendAlpha ||
                boundLayeredDepth != layeredDepth) {
                configureBlend(policy, layeredDepth);
                boundPolicy = &policy;
                boundLayeredDepth = layeredDepth;
            }
            if (!textureBound || boundTexture != material.name) {
                gl.bindTexture(texture2D, material.name);
                gl.uniform1i(trackUseTextureUniform, material.name ? 1 : 0);
                boundTexture = material.name;
                textureBound = true;
            }
            gl.uniform1i(trackDiscardZeroAlphaUniform, span.sourceAlphaTest ? 2 : (span.discardZeroAlpha ? 1 : 0));
            gl.uniform1f(trackAlphaReferenceUniform,static_cast<float>(span.sourceAlphaReference));
            gl.uniform1i(trackEffectModeUniform, effectMode(material.policy));
            gl.uniform1f(trackAlphaScaleUniform, material.policy.alphaScale);
            gl.uniform1i(trackUnlitUniform,
                material.policy.unlit || material.policy.sourcePrelit ? 1 : 0);
            if (!check("bind authenticated dynamic-world material")) return false;
            drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.indexCount), 0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(span.indexStart) * sizeof(U)), 0);
            if (!check("draw current original dynamic-world source surface")) return false;
            ++diagnostic->dynamicWorldDraws;
            diagnostic->dynamicWorldIndices += span.indexCount;
        }
        diagnostic->dynamicWorldSpan = diagnostic->texture = assets::unresolvedMaterialTexture50EA;
        resetPodState();
        return check("restore pod state after dynamic-world pass");
    }

bool VehicleWindowGl50DS::Storage::applyPodMaterial(const Material& material, bool suppress, const float* overrideUv,
        const game::original_racer_lighting50KM::LightMatrices* selectedLights) noexcept {
        const bool sourceDiffuse = material.policy.sourceDiffuseCompressed50LA;
        if (sourceDiffuse && (!selectedLights || suppress || !material.name))
            return fail(*diagnostic, VehicleWindowError50DS::material,
                "source diffuse requires the authenticated unsuppressed textured body draw");
        configureBlend(material.policy);
        const auto& u = shader->uniforms();
        unbindSpecularFeedback50LA();
        gl.uniform1i(u.sourcePairPass50LA, 0); gl.uniform1i(u.sourcePairFog50LA, 0);
        gl.uniform1i(u.sourcePairAlphaTest50LA, 0);
        gl.uniform1i(u.sourceSingleDiffuseRgba50LA,0);
        gl.uniform1i(u.sourceSingleFogEnabled50LA,0);
        gl.uniform1i(u.sourceSingleAlphaTest50LA,0);
        gl.bindTexture(texture2D, material.name);
        gl.uniform1i(u.useTexture, material.name ? 1 : 0);
        gl.uniform1i(u.effectMode, effectMode(material.policy));
        gl.uniform1f(u.alphaScale, material.policy.alphaScale);
        gl.uniform1i(u.suppressDraw, suppress ? 1 : 0);
        gl.uniform1i(u.useSourceDiffuseCompressed50LA, sourceDiffuse ? 1 : 0);
        if(material.policy.sourceSingleDiffuseRgba50LA){
            gl.uniform1i(u.sourceSingleDiffuseRgba50LA,1);
            gl.uniform1i(u.sourceSingleFogEnabled50LA,material.policy.sourceBaseFog50LA?1:0);
            gl.uniform1i(u.sourceSingleAlphaTest50LA,material.policy.sourceBaseAlphaTest50LA?1:0);
            const auto color=frameDiffuseCamera50LA.fog.color30;
            // Source color30 is AARRGGBB; 13EB90 reverses it for GS FOGCOL.
            gl.uniform4f(u.sourceSingleFogColor50LA,float((color>>16)&255u),float((color>>8)&255u),float(color&255u),0.f);
        }
        // Retain source-alpha/additive effect appearance and explicit emissive
        // materials. Generated source body RGB has already been lit on CPU;
        // only the remaining opaque pod surfaces use host illumination.
        const bool hostLit = podMaterialUsesNormals(material.policy);
        gl.uniform1i(u.useLighting, hostLit ? 1 : 0);
        const bool nativeGc = selectedLights && hostLit && !suppress && material.name &&
            material.policy.mode == VehicleWindowMaterialMode50DS::requiredTexture;
        gl.uniform1i(u.useNativeGcSelectedLights, nativeGc ? 1 : 0);
        if (nativeGc) {
            // Source byte MODULATE neutral 128 is the explicit native RGB
            // coefficient convention. No exposure scalar or specular term.
            constexpr float coefficient = 1.f / 128.f;
            const auto& ambient = selectedLights->diffuse[3];
            gl.uniform4f(u.nativeGcAmbient, ambient[0] * coefficient,
                ambient[1] * coefficient, ambient[2] * coefficient, 0.f);
            for (U i = 0; i < 3; ++i) {
                const auto& diffuse = selectedLights->diffuse[i];
                gl.uniform4f(u.nativeGcDiffuse[i], diffuse[0] * coefficient,
                    diffuse[1] * coefficient, diffuse[2] * coefficient, 0.f);
                gl.uniform4f(u.nativeGcDirection[i], selectedLights->directions[0][i],
                    selectedLights->directions[1][i], selectedLights->directions[2][i], 0.f);
            }
        }
        gl.uniform1i(u.useUvTransform, 1);
        const auto* uv = overrideUv ? overrideUv : material.uv;
        gl.uniform4f(u.uvScaleOffset, uv[0], uv[1], uv[2], uv[3]);
        return check("bind explicit vehicle material blend and UV policy");
    }

bool VehicleWindowGl50DS::Storage::drawSpecularSpan50LA(std::uint8_t bank, U slot, U archetype, U component,
        U ordinal, const assets::SceneSpan48D& span) noexcept {
        const auto selected = racerView(archetype);
        const Material* material = selected.materials ?
            &selected.materials[selected.materialOffsets[component] + span.material_index] : nullptr;
        if(component>=1&&component<=sourceBodyCount50LA&&selected.specular[component-1].singleDiffuse&&
           !frameDiffuseSelected50LA[slot][component-1])return true;
        if (!material || !material->policy.sourceSpecularPair50LA) {
            VehicleGlTimingScope50CV timing(request.enableGameplayDisplayWallTiming&&slot==0&&archetype==0?
                observePlayerTiming:nullptr,this,VehicleGlTimingPhase50CV::unpairedSpan,component);
            drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.index_count), 0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(span.index_start) * sizeof(U)), 0);
            return check("draw unchanged original unpaired source span");
        }
        if (!sourceBodyComponent50LA(component, selected.resources->componentCount))
            return fail(*diagnostic, VehicleWindowError50DS::assets,
                "body pair requires an actual engine in a three/five-model pod");
        const U bodySlot = component - 1;
        const auto& groups = selected.specular[bodySlot];
        if (!groups.enabled || ordinal >= game::maxVehicleDrawSpans50CM ||
            !groups.spanCount[ordinal] || !sourcePairColors50LA[bank] || !gl.textureBarrier)
            return fail(*diagnostic, VehicleWindowError50DS::assets, "body pair lost its exact original group resources");
        const auto& u = shader->uniforms();
        bool accepted = true;
        bool firstCoverageSetup = true;
        const U first = groups.spanFirst[ordinal], end = first + groups.spanCount[ordinal];
        const bool drawSecond = frameSpecularSecond50LA[slot][bodySlot];
        VehicleGlTimingScope50CV timing(request.enableGameplayDisplayWallTiming&&slot==0&&archetype==0?
            observePlayerTiming:nullptr,this,drawSecond?VehicleGlTimingPhase50CV::pairedSpan:
            VehicleGlTimingPhase50CV::baseOnlySpan,component);
        const bool sameFog = material->policy.sourceBaseFog50LA == material->policy.sourceSpecularFog50LA;
        // No draw in this owned span changes these states. Establish them
        // once; the first existing base-bind check still audits every call.
        gl.disable(0x0BE2); gl.enable(0x0B71); gl.depthFunc(0x0203);
        gl.colorMask(1, 1, 1, 1);
        // The shader consults this test only in pass 2. Its material policy
        // therefore stays invariant even while complete base groups draw.
        // Both callers bind this material first; applyPodMaterial has already
        // established zero and checked it. No intervening command writes this
        // uniform. Retain the unconditional reset below, including failures.
        if (material->policy.sourceSpecularAlphaTest50LA)
            gl.uniform1i(u.sourcePairAlphaTest50LA, 1);
        // Establish sampler 0 on unit 1 once. Group setup preserves it;
        // only single-sample later bases need to unbind feedback.
        // Coverage bridge tails retain their checked feedback unbind.
        unbindSpecularFeedback50LA();
        if (!drawSecond) {
            // The source distance gate removes every map-2 command in this
            // span. Cold group admission proved that these groups partition
            // this exact contiguous EBO range in order. With one unchanged
            // base state and no feedback reads, issue those same triangles as
            // one native draw. Keep source-group accounting, not host-call
            // counts, in the diagnostics. This never combines paired passes.
            gl.bindTexture(texture2D, material->name);
            gl.uniform1i(u.sourcePairPass50LA, 1);
            gl.uniform1i(u.sourcePairFog50LA, material->policy.sourceBaseFog50LA ? 1 : 0);
            gl.depthMask(1);
            accepted = check("bind original distance-skipped source base span");
            if (accepted) {
                drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.index_count), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(span.index_start) * sizeof(U)), 0);
                accepted = check("draw ordered distance-skipped source base span");
                if (accepted) {
                    diagnostic->sourceSpecularBaseGroups50LA += groups.spanCount[ordinal];
                    diagnostic->sourceSpecularSkippedGroups50LA += groups.spanCount[ordinal];
                }
            }
        }
        // PC/Xbox fixed-function route: each source group keeps its original
        // base-then-map-2 order, but map 2 is one GPU-blended draw (src RGB
        // * DST_ALPHA + dst RGB, alpha replaced) instead of destination-
        // feedback runs. The GPU orders overlapping fragments itself, so no
        // texture barrier, feedback binding or pair proof is needed.
        // The stored byte/128 alpha scale represents destination alpha only
        // up to 127, which source MODULATE guarantees when every span vertex
        // alpha is at most 64 (texture alpha 255 -> 127). Any other span keeps
        // the byte-exact route rather than saturate.
        const auto spanAlphaAdmitted = [&]() noexcept {
            if (slot > frameOpponentCount) return false;
            const auto& owner = slot ? frameOpponentBatches[slot - 1] : framePlayerBatch;
            if (component >= owner.componentCount) return false;
            const auto& part = owner.component[component];
            if (part.skin) {
                // Selected source colour: alpha = round(ambient lane 3) when no
                // light carries alpha; otherwise the cold template alpha decides.
                const bool sourceSelected = part.skin->sourceColors && part.sourceColorMaterialModes50LA &&
                    span.material_index < part.sourceColorMaterialCount50LA &&
                    part.sourceColorMaterialModes50LA[span.material_index] == 1;
                if (sourceSelected) {
                    const auto& rows = part.skin->colorRows;
                    return rows[0][3] == 0.f && rows[1][3] == 0.f && rows[2][3] == 0.f &&
                        rows[3][3] >= 0.f && rows[3][3] <= 64.f;
                }
                return selected.skin50LA && ordinal < game::maxVehicleDrawSpans50CM &&
                    selected.skin50LA[component].templateAlphaAdmitted[ordinal] != 0;
            }
            if (!part.vertices || span.vertex_start > part.vertexCount ||
                span.vertex_count > part.vertexCount - span.vertex_start) return false;
            for (U v = span.vertex_start; v < span.vertex_start + span.vertex_count; ++v)
                if (part.vertices[v].color[3] > 64) return false;
            return true;
        };
        const bool fixedBlend = drawSecond && !coverageDemand && fixedSpecularBlend50LA &&
            gl.blendFuncSeparate && spanAlphaAdmitted();
        if (drawSecond && !coverageDemand && fixedSpecularBlend50LA && !fixedBlend) ++coveragePairExactFallbackSpans;
        if (fixedBlend) {
            gl.uniform1i(u.sourcePairFixedBlend50LA, 1);
            gl.blendEquation(0x8006); // FUNC_ADD
            for (U g = first; accepted && g < end; ++g) {
                const auto& group = groups.groups[g];
                gl.bindTexture(texture2D, material->name);
                gl.uniform1i(u.sourcePairPass50LA, 1);
                if (g == first || !sameFog)
                    gl.uniform1i(u.sourcePairFog50LA, material->policy.sourceBaseFog50LA ? 1 : 0);
                gl.depthMask(1);
                drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(group.indexCount), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(group.indexStart) * sizeof(U)), 0);
                gl.bindTexture(texture2D, material->specularName50LA);
                gl.uniform1i(u.sourcePairPass50LA, 2);
                if (!sameFog)
                    gl.uniform1i(u.sourcePairFog50LA, material->policy.sourceSpecularFog50LA ? 1 : 0);
                gl.depthMask(0); // Same linked position shader/VAO and LEQUAL coverage.
                gl.enable(0x0BE2);
                gl.blendFuncSeparate(0x0304, 1, 1, 0); // RGB: DST_ALPHA, ONE; A: ONE, ZERO
                drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(group.indexCount), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(group.indexStart) * sizeof(U)), 0);
                gl.disable(0x0BE2);
                ++diagnostic->sourceSpecularBaseGroups50LA;
                diagnostic->sourceSpecularSecondTriangles50LA += group.indexCount / 3;
            }
            accepted = check("draw fixed-function original source-pair groups");
            gl.uniform1i(u.sourcePairFixedBlend50LA, 0);
        }
        // Single-sample: one operand cache for the whole paired span. Vertex
        // positions are fixed for this draw, so batch planning and run
        // building reuse projections instead of recomputing per batch.
        if (!fixedBlend && drawSecond && !coverageDemand && first < end) {
            const auto& last = groups.groups[end - 1];
            beginRacerRasterOperandGroup(slot, component, groups.groups[first].indexStart,
                last.indexStart + last.indexCount - groups.groups[first].indexStart);
        }
        for (U g = first; !fixedBlend && drawSecond && accepted && g < end; ++g) {
            const U groupIndex = g;
            // Single-sample consecutive groups whose triangles provably share
            // no sample with another group's form one batch: one base draw of
            // their contiguous elements, then one map-2 run sequence. Each
            // pixel still sees only its own group's base-then-map-2 order.
            // AA coverage and unproven groups remain exactly one source group.
            U batch = 1;
            if (!coverageDemand)
                batch = planSeparableGroupBatch(slot, component, groups.groups.get(), g, end);
            SpecularGroup50LA group = groups.groups[g];
            for (U k = 1; k < batch; ++k) group.indexCount += groups.groups[g + k].indexCount;
            coveragePairBatchedGroups += batch - 1;
            g += batch - 1;
            // Original single32CC28/multi32D670 issue the ENTIRE MSCNT group
            // base before single32CD18/multi32D750 issue that group's map 2.
            // Several source strips can share the same group. Keep them joined.
            // Map 2 changes these states, so restore them for every paired
            // group. On the source distance-skip path only base draws follow;
            // their texture/uniform/depth state and unbound feedback persist.
            if (drawSecond || groupIndex == first) {
                if (groupIndex != first && !coverageDemand) {
                    gl.activeTexture(textureUnit0 + 1); gl.bindTexture(texture2D, 0);
                    gl.activeTexture(textureUnit0);
                }
                gl.bindTexture(texture2D, material->name);
                gl.uniform1i(u.sourcePairPass50LA, 1);
                // Equal pass policies need one write per span. No command
                // inside the group loop otherwise changes this uniform.
                if (groupIndex == first || !sameFog)
                    gl.uniform1i(u.sourcePairFog50LA, material->policy.sourceBaseFog50LA ? 1 : 0);
                gl.depthMask(1);
            }
            accepted = check("bind original source-pair base group");
            if (accepted) {
                drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(group.indexCount), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(group.indexStart) * sizeof(U)), 0);
                accepted = check("draw complete original source-pair base group");
                if (accepted) diagnostic->sourceSpecularBaseGroups50LA += batch;
            }
            // Original distance130/1000 skip retains the complete base draw.
            if (!accepted) continue;
            if (!drawSecond) {
                diagnostic->sourceSpecularSkippedGroups50LA += batch;
                continue;
            }
            gl.bindTexture(texture2D, material->specularName50LA);
            gl.uniform1i(u.sourcePairPass50LA, 2);
            if (!sameFog)
                gl.uniform1i(u.sourcePairFog50LA, material->policy.sourceSpecularFog50LA ? 1 : 0);
            gl.depthMask(0); // Same linked position shader/VAO and LEQUAL coverage.
            // Coverage entry and checked bridge tails retain unit 1 unbound
            // and unit 0 current, including groups with no visible triangles.
            if (!coverageDemand) {
                gl.activeTexture(textureUnit0 + 1);
                // The span's initial unbind selected sampler 0 on unit 1;
                // no intervening command changes sampler-object bindings.
                gl.bindTexture(texture2D, sourcePairColors50LA[bank]); gl.activeTexture(textureUnit0);
            }
            accepted = check("bind original source-pair second group");
            // The unchanged single-sample route reads one actual triangle's
            // own pixel once. A barrier before EACH draw observes overlapping
            // prior second triangles, including their independent source-A store.
            // Surviving map-2 samples were covered by the identical base group;
            // the optional GREATER0/KEEP test can only narrow that coverage.
            // Source sampling/clipping/depth remain the admitted native policy.
            if (accepted) {
                // Both coverage routes share conservative disjoint runs. A
                // single-sample texture barrier is sufficient for a run whose
                // primitives cannot read or write one another's pixels.
                if (coverageDemand) beginRacerRasterOperandGroup(slot,component,group.indexStart,group.indexCount);
                accepted = drawCoveragePairGroup(bank, slot, component,
                    group.indexStart, group.indexCount, material->specularName50LA, firstCoverageSetup);
                if (coverageDemand) endRacerRasterOperandGroup();
                // The AA1 route checks each scratch/copy/composition operation.
                // Cold source topology bounds one MSCNT group to 3..16
                // vertices, hence at most 14 ordered triangles. Poll errors
                // once for that bounded command batch, not once per triangle.
                // On the unchanged route no intervening call consumes GL errors; reject before the
                // next group and restore/unbind below, without publication.
                const bool commandsOk = check("draw ordered source map-2 group with current destination alpha");
                accepted = accepted && commandsOk;
                if (accepted) diagnostic->sourceSpecularSecondTriangles50LA += group.indexCount / 3;
            }
        }
        endRacerRasterOperandGroup();
        // Unbind the target even after failure, before any base/unrelated draw
        // or abort. Publication still belongs solely to the existing bank owner.
        unbindSpecularFeedback50LA();
        gl.bindTexture(texture2D, material->name);
        gl.uniform1i(u.sourcePairPass50LA, 0); gl.uniform1i(u.sourcePairFog50LA, 0);
        gl.uniform1i(u.sourcePairAlphaTest50LA, 0);
        gl.uniform1i(u.sourceSingleDiffuseRgba50LA,0);
        gl.uniform1i(u.sourceSingleFogEnabled50LA,0);
        gl.uniform1i(u.sourceSingleAlphaTest50LA,0); gl.depthMask(1);
        const bool restored = check("restore state after original body source pair");
        return accepted && restored;
    }

bool VehicleWindowGl50DS::Storage::drawSpan50LA(void* context, game::VehicleDrawComponent50CM component,
        U ordinal, const assets::SceneSpan48D& span, I baseVertex) noexcept {
        auto& s = *static_cast<Storage*>(context);
        const U c = componentIndex(component);
        if (!s.active || c >= s.resources.componentCount || baseVertex)
            return fail(*s.diagnostic, VehicleWindowError50DS::assets, "invalid owned body-pair span draw");
        return s.drawSpecularSpan50LA(static_cast<std::uint8_t>(s.published ^ 1u), 0, 0, c, ordinal, span);
    }

bool VehicleWindowGl50DS::Storage::uploadRacer(std::uint8_t bank, const game::VehicleDrawBatch50CO& batch,
        U archetype, bool effectsOnly) noexcept {
        const auto selected = racerView(archetype);
        if (!uploadRacerNormals(bank, batch, archetype, effectsOnly)) return false;
        // Every batch was structurally preflighted and matched to these exact
        // cold buffers before begin. Only the inactive bank is ever reused.
        for (U c = 0; c < selected.resources->componentCount; ++c) {
            if (batch.component[c].skin) continue; // GPU vertex path: static skin VBO
            drawGl.bindBuffer(arrayBuffer, selected.resources->bank[bank].vertexBuffer[c]);
            // No deferred span consumes an all-opaque component. Upload the
            // complete current stream for every possible deferred consumer;
            // never assume submitted UV/color bytes equal the cold template.
            if (!effectsOnly || !selected.materials || selected.materialSummary[c].hasDeferredEffects)
                drawGl.bufferSubData(arrayBuffer, 0,
                    std::intptr_t(batch.component[c].vertexCount) * sizeof(assets::Vertex48B), batch.component[c].vertices);
            if (!check("upload independent world-space racer into inactive buffers")) return false;
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::drawOpaqueOpponent(std::uint8_t bank, U opponent) noexcept {
        resetPodState();
        const U archetype = frameOpponentArchetypes[opponent];
        const auto selected = racerView(archetype);
        if (!uploadRacer(bank, frameOpponentBatches[opponent], archetype) ||
            !uploadSpecularUvs50LA(bank, opponent + 1, archetype)) return false;
        for (U c = 0; c < selected.resources->componentCount; ++c) {
            drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[c]);
            drawGl.bindBuffer(elementBuffer, selected.resources->indices[c].buffer);
            const auto& part = frameOpponentBatches[opponent].component[c];
            if (!bindRacerSkin50LA(part, opponent + 1, c)) return false;
            for (U i = 0; i < selected.meshes[c]->spanCount; ++i) {
                const auto& span = selected.meshes[c]->spans[i];
                if (selected.materials) {
                    const U material = selected.materialOffsets[c] + span.material_index;
                    if (selected.materials[material].policy.blend != VehicleWindowBlend50DS::opaque) continue;
                    if (!applyPodMaterial(selected.materials[material], false,
                        opponentMaterialUvs[opponent * opponentMaterialStride + material].value,
                        frameOpponentLights[opponent]->component(c))) return false;
                }
                selectRacerSkinSpan50LA(part, span.material_index);
                if (!drawSpecularSpan50LA(bank, opponent + 1, archetype, c, i, span)) return false;
                ++diagnostic->opponentOpaqueDraws;
                diagnostic->opponentOpaqueIndices += span.index_count;
            }
        }
        ++diagnostic->opponentRacersDrawn;
        resetPodState();
        return check("restore state after opaque opponent");
    }

bool VehicleWindowGl50DS::Storage::drawTransparentPod(std::uint8_t bank, U opponent) noexcept {
        const auto selected = racerView(opponent < maxVehicleWindowOpponents50DS ?
            frameOpponentArchetypes[opponent] : 0);
        if (!selected.materials) return true;
        resetPodState();
        const U slot=opponent<maxVehicleWindowOpponents50DS?opponent+1u:0u;
        for (U c = 0; c < selected.resources->componentCount; ++c) {
            // Source selected191 suppresses the complete model, including its
            // deferred flame/effect materials, not just the opaque body maps.
            if(c>0&&c<=sourceBodyCount50LA&&selected.specular[c-1].singleDiffuse&&
               !frameDiffuseSelected50LA[slot][c-1])continue;
            bool bound = false;
            for (U i = 0; i < selected.meshes[c]->spanCount; ++i) {
                const auto& span = selected.meshes[c]->spans[i];
                const auto& material = selected.materials[selected.materialOffsets[c] + span.material_index];
                if (material.policy.blend == VehicleWindowBlend50DS::opaque) continue;
                const game::VehicleDrawSubmission50CM* part = opponent < maxVehicleWindowOpponents50DS ?
                    &frameOpponentBatches[opponent].component[c] : nullptr;
                if (!bound) {
                    drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[c]);
                    drawGl.bindBuffer(elementBuffer, selected.resources->indices[c].buffer);
                    if (part && !bindRacerSkin50LA(*part, slot, c)) return false;
                    bound = true;
                }
                const float* uv = opponent < maxVehicleWindowOpponents50DS ?
                    opponentMaterialUvs[opponent * opponentMaterialStride +
                        selected.materialOffsets[c] + span.material_index].value : nullptr;
                if (!applyPodMaterial(material, false, uv)) return false;
                if (part) selectRacerSkinSpan50LA(*part, span.material_index);
                drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.index_count), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(span.index_start) * sizeof(U)), 0);
                if (!check("draw deferred source pod effect span")) return false;
                ++diagnostic->transparentPodDraws;
                diagnostic->transparentPodIndices += span.index_count;
            }
        }
        resetPodState();
        return check("restore opaque state after deferred pod effects");
    }

bool VehicleWindowGl50DS::Storage::bindRacerSkin50LA(const game::VehicleDrawSubmission50CM& part,
        U slot, U component) noexcept {
        const auto& u = shader->uniforms();
        if (!part.skin) { gl.uniform1i(u.skinMode50LA, 0); return true; }
        const auto& skin = *part.skin;
        if (!skin.boneCount || skin.boneCount > game::maxRigidPalette49S || !gl.uniform4fv ||
            !gl.uniform3fv || !gl.uniform2fv)
            return fail(*diagnostic, VehicleWindowError50DS::frame, "GPU skin inputs unavailable");
        float rows[vehicleGlSkinBoneRows50LA * 4];
        for (U bone = 0; bone < skin.boneCount; ++bone) {
            const auto& m = skin.bones[bone].v;
            for (U axis = 0; axis < 3; ++axis) {
                float* row = rows + (bone * 3 + axis) * 4;
                row[0] = m[axis]; row[1] = m[4 + axis]; row[2] = m[8 + axis]; row[3] = m[12 + axis];
            }
        }
        gl.uniform4fv(u.skinBones50LA, static_cast<I>(skin.boneCount * 3), rows);
        gl.uniform3fv(u.skinLightDirections50LA, 3, skin.lightDirections[0].data());
        gl.uniform4fv(u.skinColorRows50LA, 4, skin.colorRows[0].data());
        float specular[6]{};
        if (component >= 1 && component <= sourceBodyCount50LA && slot <= maxVehicleWindowOpponents50DS) {
            const auto& axes = frameSpecularCoefficients50LA[slot][component - 1].cameraNormalAxes;
            for (U axis = 0; axis < 3; ++axis) { specular[axis * 2] = axes[axis][0]; specular[axis * 2 + 1] = axes[axis][1]; }
        }
        gl.uniform2fv(u.skinSpecular50LA, 3, specular);
        gl.uniform1i(u.skinDiffuseSelected50LA, 0);
        gl.uniform1i(u.skinMode50LA, 1);
        return check("bind GPU skin inputs");
    }

void VehicleWindowGl50DS::Storage::selectRacerSkinSpan50LA(const game::VehicleDrawSubmission50CM& part,
        U material) noexcept {
        if (!part.skin) return;
        const bool selected = part.skin->sourceColors && part.sourceColorMaterialModes50LA &&
            material < part.sourceColorMaterialCount50LA && part.sourceColorMaterialModes50LA[material] == 1;
        gl.uniform1i(shader->uniforms().skinDiffuseSelected50LA, selected ? 1 : 0);
    }

void VehicleWindowGl50DS::Storage::releaseRacerSkin50LA() noexcept {
        const auto& u = shader->uniforms();
        gl.uniform1i(u.skinMode50LA, 0); gl.uniform1i(u.skinDiffuseSelected50LA, 0);
    }

bool VehicleWindowGl50DS::Storage::racerSkinShadow50LA(const game::VehicleDrawSubmission50CM& part,
        const RacerSkin50LA& cold, NativePodShadow50HM& output) const noexcept {
        // Same footprint construction as buildNativePodShadow50HM, from exact
        // per-bone moments (covariance) and transformed per-bone casting AABB
        // corners (a superset of the extreme vertices for the min/max terms).
        const auto& skin = *part.skin;
        NativePodShadow50HM result{};
        double low[3] = {INFINITY, INFINITY, INFINITY}, high[3] = {-INFINITY, -INFINITY, -INFINITY};
        double samples = 0;
        double sum[3] = {}, square[3][3] = {};
        const auto corner = [&](const game::BodyMatrix48G& m, const std::array<float, 6>& b, unsigned k, double (&w)[3]) {
            const double local[3] = {b[(k & 1) ? 3 : 0], b[(k & 2) ? 4 : 1], b[(k & 4) ? 5 : 2]};
            for (unsigned axis = 0; axis < 3; ++axis)
                w[axis] = local[0] * m.v[axis] + local[1] * m.v[4 + axis] + local[2] * m.v[8 + axis] + m.v[12 + axis];
        };
        for (U bone = 0; bone < skin.boneCount && bone < cold.boneCount; ++bone) {
            const double n = cold.count[bone];
            if (!n) continue;
            const auto& m = skin.bones[bone];
            for (unsigned k = 0; k < 8; ++k) {
                double w[3]; corner(m, cold.bounds[bone], k, w);
                for (unsigned axis = 0; axis < 3; ++axis) { low[axis] = std::min(low[axis], w[axis]); high[axis] = std::max(high[axis], w[axis]); }
            }
            // p = A x + t with A[a][k] = m[k*4 + a].
            double A[3][3], t[3];
            for (unsigned a = 0; a < 3; ++a) { t[a] = m.v[12 + a]; for (unsigned k = 0; k < 3; ++k) A[a][k] = m.v[k * 4 + a]; }
            const auto& s1 = cold.sum[bone];
            const auto& q = cold.square[bone];
            const double S[3][3] = {{q[0], q[1], q[2]}, {q[1], q[3], q[4]}, {q[2], q[4], q[5]}};
            double As[3] = {};
            for (unsigned a = 0; a < 3; ++a) for (unsigned k = 0; k < 3; ++k) As[a] += A[a][k] * s1[k];
            for (unsigned a = 0; a < 3; ++a) {
                sum[a] += As[a] + n * t[a];
                for (unsigned b = 0; b < 3; ++b) {
                    double ASA = 0;
                    for (unsigned k = 0; k < 3; ++k) for (unsigned l = 0; l < 3; ++l) ASA += A[a][k] * S[k][l] * A[b][l];
                    square[a][b] += ASA + t[a] * As[b] + t[b] * As[a] + n * t[a] * t[b];
                }
            }
            samples += n;
        }
        if (!samples) { output = result; return true; }
        for (unsigned axis = 0; axis < 3; ++axis) if (!std::isfinite(low[axis]) || !std::isfinite(high[axis])) return false;
        const double originX = (low[0] + high[0]) * .5, originY = high[1], originZ = (low[2] + high[2]) * .5;
        // x = a.p + cx, z = b.p + cz.
        const double a[3] = {1., -nativeShadowSlopeX50HM, 0.}, b[3] = {0., -nativeShadowSlopeZ50HM, 1.};
        const double cx = nativeShadowSlopeX50HM * originY - originX, cz = nativeShadowSlopeZ50HM * originY - originZ;
        const auto dot = [](const double (&u)[3], const double (&v)[3]) { return u[0] * v[0] + u[1] * v[1] + u[2] * v[2]; };
        const auto quad = [&](const double (&u)[3], const double (&v)[3]) {
            double r = 0; for (unsigned i = 0; i < 3; ++i) for (unsigned j = 0; j < 3; ++j) r += u[i] * square[i][j] * v[j]; return r;
        };
        const double sumX = dot(a, sum) + samples * cx, sumZ = dot(b, sum) + samples * cz;
        const double sumXX = quad(a, a) + 2. * cx * dot(a, sum) + samples * cx * cx;
        const double sumZZ = quad(b, b) + 2. * cz * dot(b, sum) + samples * cz * cz;
        const double sumXZ = quad(a, b) + cx * dot(b, sum) + cz * dot(a, sum) + samples * cx * cz;
        const double covarianceXX = sumXX - sumX * sumX / samples;
        const double covarianceXZ = sumXZ - sumX * sumZ / samples;
        const double covarianceZZ = sumZZ - sumZ * sumZ / samples;
        const double angle = .5 * std::atan2(2. * covarianceXZ, covarianceXX - covarianceZZ);
        const double uX = std::cos(angle), uZ = std::sin(angle);
        double minU = INFINITY, maxU = -INFINITY, minV = INFINITY, maxV = -INFINITY;
        for (U bone = 0; bone < skin.boneCount && bone < cold.boneCount; ++bone) {
            if (!cold.count[bone]) continue;
            for (unsigned k = 0; k < 8; ++k) {
                double w[3]; corner(skin.bones[bone], cold.bounds[bone], k, w);
                const double x = w[0] - originX - nativeShadowSlopeX50HM * (w[1] - originY);
                const double z = w[2] - originZ - nativeShadowSlopeZ50HM * (w[1] - originY);
                const double u = x * uX + z * uZ, v = -x * uZ + z * uX;
                minU = std::min(minU, u); maxU = std::max(maxU, u); minV = std::min(minV, v); maxV = std::max(maxV, v);
            }
        }
        const double radiusU = (maxU - minU) * .55, radiusV = (maxV - minV) * .55;
        if (radiusU < 1e-4 || radiusV < 1e-4) { output = result; return true; }
        const double midU = (minU + maxU) * .5, midV = (minV + maxV) * .5;
        const double inverseU = 1. / (radiusU * radiusU), inverseV = 1. / (radiusV * radiusV);
        const double values[8] = {
            originX + midU * uX - midV * uZ, originY, originZ + midU * uZ + midV * uX,
            std::clamp(8. * std::sqrt(radiusU * radiusV), 32., 256.),
            uX * uX * inverseU + uZ * uZ * inverseV, uX * uZ * (inverseU - inverseV),
            uZ * uZ * inverseU + uX * uX * inverseV, .48
        };
        for (unsigned i = 0; i < 8; ++i) {
            if (!std::isfinite(values[i]) || std::abs(values[i]) > std::numeric_limits<float>::max()) return false;
            (i < 4 ? result.centerAndReach[i] : result.shapeAndOpacity[i - 4]) = static_cast<float>(values[i]);
        }
        const double qxx = result.shapeAndOpacity[0], qxz = result.shapeAndOpacity[1], qzz = result.shapeAndOpacity[2];
        if (!(qxx > 0. && qzz > 0. && qxx * qzz > qxz * qxz)) return false;
        output = result;
        return true;
    }

std::uint32_t VehicleWindowGl50DS::racerGpuSkinMask50LA(std::uint32_t archetype) const noexcept {
        if (!initialized()) return 0;
        const auto& s = *storage_;
        if (!archetype || archetype > s.racerArchetypeCount || s.coverageDemand || !s.fixedSpecularBlend50LA ||
            !s.gl.blendFuncSeparate || !s.gl.uniform4fv || !s.gl.uniform3fv || !s.gl.uniform2fv) return 0;
        const auto& source = s.racerArchetypes[archetype - 1];
        std::uint32_t mask = 0;
        for (U c = 0; c < source.resources.componentCount; ++c)
            if (source.skin50LA[c].eligible) mask |= 1u << c;
        return mask;
    }

bool VehicleWindowGl50DS::Storage::begin(void* context, std::uint8_t bank) noexcept {
        auto& s = *static_cast<Storage*>(context);
        auto stageStart=s.rendererStageNow();
        s.active = true;s.activeFrameBank=bank;
        s.coveragePairRuns=s.coveragePairTriangles=s.coveragePairCoalescedTriangles=0;
        s.coveragePairEmptyTriangles=s.coveragePairFullSingletons=0;s.coveragePairBatchedGroups=0;
        s.coveragePairUnproven={};s.coveragePairUnprovenOperands=s.coveragePairBreakRegion=0;
        s.coveragePairBreakCap=s.coveragePairBreakEnd=0;s.coveragePairExactFallbackSpans=0;
        // Same-context GL ordering protects buffer/target reuse after prior
        // commands, including aborted work. No mapping or cross-context sharing.
        s.gl.bindFramebuffer(framebuffer, s.coverageDemand ? s.coverageLive.scene[bank] : s.targets[bank]);
        s.gl.drawBuffer(colorAttachment); s.gl.readBuffer(colorAttachment);
        s.gl.viewport(0, 0, s.width, s.height);
        // Solid, two-sided, opaque, LESS depth. This is diagnostic viewer state,
        // deliberately independent of original-game texture/material behavior.
        constexpr U disabled[] = {0x0BE2, 0x0B44, 0x0C11, 0x0B90, 0x8037, 0x0B20,
            0x8F9D, 0x0BD0, 0x809D, 0x80A0, 0x809E, 0x8C89, 0x8DB9, 0x864F};
        for (U capability : disabled) s.gl.disable(capability);
        for (I i = 0; i < s.clipDistances; ++i) s.gl.disable(0x3000 + static_cast<U>(i));
        s.gl.enable(0x0B71); s.gl.depthFunc(0x0201); s.gl.depthMask(1);
        s.gl.depthRange(0.0, 1.0); s.gl.clearDepth(1.0);
        s.gl.colorMask(1, 1, 1, 1); s.gl.polygonMode(0x0408, 0x1B02);
        s.gl.useProgram(s.shader->program());
        const auto& uniforms = s.shader->uniforms();
        s.gl.uniformMatrix4fv(uniforms.worldViewProjection, 1, 0, s.matrix);
        s.gl.uniform1i(uniforms.useTexture, 0); s.gl.uniform1i(uniforms.image, 0);
        s.gl.uniform1i(uniforms.effectMode, 0); s.gl.uniform1f(uniforms.alphaScale, 1.f);
        s.gl.uniform1i(uniforms.suppressDraw, 0); s.gl.uniform1i(uniforms.useUvTransform, 0);
        s.gl.uniform1i(uniforms.useLighting, 0);
        s.resetNativeGcLighting();
        if (s.usesTextures()) {
            s.gl.activeTexture(textureUnit0);
            s.gl.bindTexture(texture2D, 0);
            // A sampler object would override the immutable 50EB parameters.
            // Select the texture object's already-authored sampling policy.
            if (s.gl.bindSampler) s.gl.bindSampler(0, 0);
        }
        s.gl.clearColor(s.clearRgba[0], s.clearRgba[1], s.clearRgba[2], s.clearRgba[3]);
        s.gl.clear(colorBufferBit | depthBufferBit);
        if (!s.check("begin explicit private frame")) return false;
        s.traceRaStage("target_setup",stageStart);
        if (!s.uploadRcCrystalUvs() || !s.uploadDynamicWorld(bank) || !s.drawSky())return false;
        s.traceRaStage("sky",stageStart);
        if(!s.drawRaWorld(bank))return false;
        s.traceRaStage("ra_world",stageStart);
        if(!s.drawTrack(false)||!s.drawDynamicWorld(bank,false))return false;
        s.traceRaStage("legacy_world",stageStart);
        const bool accepted=s.uploadRacerNormals(bank, s.framePlayerBatch) && s.uploadSpecularUvs50LA(bank, 0, 0);
        if(accepted){
            s.traceRaStage("player_stream_prepare",stageStart);
            s.rendererPlayerStageStart=s.rendererStageNow();
        }
        return accepted;
    }

bool VehicleWindowGl50DS::Storage::bindMaterial(void* context, game::VehicleDrawComponent50CM component, U material) noexcept {
        if (!materialReady(context, component, material)) return false;
        auto& s = *static_cast<Storage*>(context);
        if (s.request.mode == VehicleWindowMode50DS::callerMaterialTextures) {
            const auto& binding = s.materials[s.materialOffsets[componentIndex(component)] + material];
            // 50CV keeps its exact original submission/provenance contract. A
            // selected effect issues a discard-only first-pass draw, then the
            // retained source range is rendered once at commit after opaques.
            return s.applyPodMaterial(binding, binding.policy.blend != VehicleWindowBlend50DS::opaque,
                nullptr, s.framePlayerLights->component(componentIndex(component)));
        }
        s.gl.uniform1i(s.shader->uniforms().useTexture, 0);
        s.gl.uniform1i(s.shader->uniforms().useNativeGcSelectedLights, 0);
        s.gl.uniform1i(s.shader->uniforms().useSourceDiffuseCompressed50LA, 0);
        return s.check("bind explicit neutral vehicle material policy");
    }

void VehicleWindowGl50DS::Storage::observePlayerTiming(void* context,VehicleGlTimingPhase50CV phase,U component,
        std::uint64_t ns,bool valid) noexcept {
        auto& s=*static_cast<Storage*>(context);const auto p=static_cast<unsigned>(phase);
        if(!s.request.enableGameplayDisplayWallTiming)return;
        if(!valid||component>=game::maxVehicleDrawComponents50CM||p>=static_cast<unsigned>(VehicleGlTimingPhase50CV::count)){
            s.playerTiming.invalid=true;return;
        }
        auto& counter=s.playerTiming.value[component][p];
        if(counter.calls==std::numeric_limits<std::uint64_t>::max()||
           ns>std::numeric_limits<std::uint64_t>::max()-counter.wallNs){s.playerTiming.invalid=true;return;}
        ++counter.calls;counter.wallNs+=ns;
    }

void VehicleWindowGl50DS::Storage::reportPlayerTiming(bool accepted) const noexcept {
        if(!request.enableGameplayDisplayWallTiming)return;
        std::fprintf(stderr,"player_submit_timing accepted=%u invalid=%u components=%u order=upload,geometry_bind,material_bind,span_draw,unpaired,base_only,paired units=calls:wall_ns",
            accepted?1u:0u,playerTiming.invalid?1u:0u,resources.componentCount);
        for(U c=0;c<resources.componentCount&&c<game::maxVehicleDrawComponents50CM;++c){
            std::fprintf(stderr," c%u=",c);
            for(unsigned p=0;p<static_cast<unsigned>(VehicleGlTimingPhase50CV::count);++p){
                const auto& counter=playerTiming.value[c][p];
                std::fprintf(stderr,"%s%llu:%llu",p?",":"",static_cast<unsigned long long>(counter.calls),
                    static_cast<unsigned long long>(counter.wallNs));
            }
        }
        std::fprintf(stderr,"\n");
    }

std::chrono::steady_clock::time_point VehicleWindowGl50DS::Storage::rendererStageNow() const noexcept {
        return request.enableGameplayDisplayWallTiming?std::chrono::steady_clock::now():
            std::chrono::steady_clock::time_point{};
    }

void VehicleWindowGl50DS::Storage::traceRaStage(const char* stage,std::chrono::steady_clock::time_point& start,bool nested) noexcept {
        if(!request.enableGameplayDisplayWallTiming)return;
        // Diagnostic only: serialize stage completion to locate GPU stalls.
        // These timings include submission/wait overhead, not GPU timer-query
        // durations, and this mode must not be used to claim production FPS.
        static const bool completedStages=[](){const char* value=std::getenv("RR_TRACE_GPU_STAGES");
            return value&&std::strcmp(value,"1")==0;}();
        if(completedStages)gl.finish();
        const auto now=std::chrono::steady_clock::now();
        const auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(now-start).count();
        if(completedStages)
            std::fprintf(stderr,"renderer_completed_stage name=%s nested_backend=%u wall_us=%lld\n",
                stage,nested?1u:0u,static_cast<long long>(elapsed));
        else if(request.raStaticWorld)
            std::fprintf(stderr,"ra_renderer_stage name=%s cpu_submission_wall_us=%lld\n",stage,static_cast<long long>(elapsed));
        else
            std::fprintf(stderr,"renderer_stage name=%s nested_backend=%u submission_wall_us=%lld\n",
                stage,nested?1u:0u,static_cast<long long>(elapsed));
        // Do not charge this diagnostic write to the following sibling stage.
        start=std::chrono::steady_clock::now();
    }

bool VehicleWindowGl50DS::Storage::commit(void* context, std::uint8_t bank) noexcept {
        auto& s = *static_cast<Storage*>(context);
        if (!s.active || bank != (s.published ^ 1u)) return false;
        s.traceRaStage("player_draw",s.rendererPlayerStageStart);
        auto stageStart=s.rendererStageNow();
        // One transaction and one clear/sky/track pass. Other actual opaque
        // racers join the player's pending framebuffer before ANY effects.
        for (U i = 0; i < s.frameOpponentCount; ++i) {
            s.diagnostic->opponentIndex = i;
            if (s.frameOpponentHidden[i]) continue; // destroyed racer: models hidden
            if (!s.drawOpaqueOpponent(bank, i)) return false;
        }
        s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        s.traceRaStage("opaque_opponents",stageStart);
        if(s.request.enableGameplayDisplayWallTiming)
            std::fprintf(stderr,"native_pair_batches scope=player_and_opaque_opponents runs=%llu triangles=%llu coalesced_triangles=%llu saved_bridges=%llu empty_triangles=%llu full_singletons=%llu batched_groups=%llu\n",
                static_cast<unsigned long long>(s.coveragePairRuns),
                static_cast<unsigned long long>(s.coveragePairTriangles),
                static_cast<unsigned long long>(s.coveragePairCoalescedTriangles),
                static_cast<unsigned long long>(s.coveragePairTriangles-s.coveragePairRuns),
                static_cast<unsigned long long>(s.coveragePairEmptyTriangles),
                static_cast<unsigned long long>(s.coveragePairFullSingletons),
                static_cast<unsigned long long>(s.coveragePairBatchedGroups));
        if(s.request.enableGameplayDisplayWallTiming)
            std::fprintf(stderr,"native_pair_breaks unproven_shared0=%llu unproven_shared1=%llu unproven_shared2=%llu unproven_operands=%llu region=%llu cap=%llu group_end=%llu\n",
                static_cast<unsigned long long>(s.coveragePairUnproven[0]),
                static_cast<unsigned long long>(s.coveragePairUnproven[1]),
                static_cast<unsigned long long>(s.coveragePairUnproven[2]),
                static_cast<unsigned long long>(s.coveragePairUnprovenOperands),
                static_cast<unsigned long long>(s.coveragePairBreakRegion),
                static_cast<unsigned long long>(s.coveragePairBreakCap),
                static_cast<unsigned long long>(s.coveragePairBreakEnd));
        if(s.request.enableGameplayDisplayWallTiming)
            std::fprintf(stderr,"native_fixed_specular enabled=%u exact_fallback_spans=%llu\n",
                s.fixedSpecularBlend50LA?1u:0u,static_cast<unsigned long long>(s.coveragePairExactFallbackSpans));
        // Cable/HUD failure still aborts the unpublished combined frame.
        s.resetPodState();
        if (!s.drawCables(s.frameCables, s.cableTextureName)) return false;
        for (U i = 0; i < s.frameOpponentCount; ++i) {
            s.diagnostic->opponentIndex = i;
            if (s.frameOpponentHidden[i]) continue;
            if (!s.drawCables(s.frameOpponentCables[i],
                s.racerView(s.frameOpponentArchetypes[i]).cableTextureName)) return false;
        }
        s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        s.traceRaStage("cables",stageStart);
        if (!s.drawTrack(true) || !s.drawDynamicWorld(bank, true)) return false;
        s.traceRaStage("transparent_world",stageStart);
        // The shared inactive VBOs currently hold the last opaque opponent.
        // Restore the player's independent stream before its deferred effects.
        if (s.frameOpponentCount && s.materials && !s.uploadRacer(bank, s.framePlayerBatch, 0, true)) return false;
        if (!s.drawTransparentPod(bank)) return false;
        if (s.materials) for (U i = 0; i < s.frameOpponentCount; ++i) {
            s.diagnostic->opponentIndex = i;
            if (s.frameOpponentHidden[i]) continue;
            if (!s.uploadRacer(bank, s.frameOpponentBatches[i], s.frameOpponentArchetypes[i], true) ||
                !s.drawTransparentPod(bank, i)) return false;
        }
        s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        s.resetPodState();
        s.traceRaStage("transparent_racers",stageStart);
        if (!s.drawGameplayBinders() || !s.drawGcParticles()) return false;
        s.traceRaStage("binders_particles",stageStart);
        if (s.hasSpecularPairs50LA || s.hasSingleDiffuse50LA || s.request.track.rcCrystal || s.frameGameplayBinderCount) {
            // All body/crystal Ad consumers have completed. Current Binder/GC
            // materials use source alpha and DATE0; the original HUD's private
            // scratch proves its own clear-before-Ad coverage. No later current
            // gameplay consumer needs these body/particle alpha bytes. Binder
            // now also stores source A even without a body/crystal owner; keep
            // that private alpha out of the opaque native presentation.
            s.unbindSpecularFeedback50LA();
            s.gl.disable(0x0C11); s.gl.colorMask(0, 0, 0, 1);
            s.gl.clearColor(0.f, 0.f, 0.f, 1.f); s.gl.clear(colorBufferBit);
            s.gl.colorMask(1, 1, 1, 1);
            if (!s.check("restore opaque presentation alpha after all body/crystal and particle consumers")) return false;
        }
        if (!s.resolveCoverage(bank)) return false;
        s.traceRaStage("coverage_resolve",stageStart);
        // PC-native FXAA smooths the completed 3D scene only; every HUD,
        // Results, F8 and Pause layer below is drawn afterwards, unsmoothed.
        if (!s.drawFxaa(bank)) return false;
        // 1AAE38/48/58/68 attach C8/CC/D0/C4 through141960 tail append;
        // 141D70 traverses firstChild18/next14. Preserve that HUD sibling order.
        // These explicit native joins do not claim recovered parent traversal.
        // Native world/HUD contribution has returned before the accepted Race
        // tail.1A8698 directly draws Results BEFORE1A86C8 visits F8 children.
        // Results admission rejects an active preceding source Pause; its
        // unrelated existing native Pause route remains unchanged otherwise.
        if (!s.drawHostHud() || !s.drawRaceHud(bank) || !s.drawDamage(bank) || !s.drawPositionStats(bank) || !s.drawTimeStats(bank) || !s.drawSpeedBoost(bank) || !s.drawGameplayBlur(bank) || !s.drawNativeResultsPackets(bank) || !s.drawNativeF8Bars(bank) || !s.drawPauseDerived(bank)) return false;
        s.traceRaStage("hud_blur",stageStart);
        if (!s.check("accept private vehicle frame")) return false;
        // 50CV publication is CPU-only. The fallible window copy is deliberately
        // OUTSIDE this transaction, after submitVehicleDrawBatch50CS succeeds.
        s.retainedRaceDisplayCompleted=false;
        s.pauseDisplayPublication={}; //A new race target is not the prior retained Pause publication.
        s.retainedRaceContainsPause=s.pauseDerived&&s.pauseDerived->frame;
        s.retainedRaceCameraPublication=s.pendingRaceCameraPublication;
        s.published = bank;
        s.committedWidth = s.width; s.committedHeight = s.height;
        s.raceWidth = s.width; s.raceHeight = s.height;
        s.displayedTarget = s.targets[bank];
        s.uiPrivatePublicationGeneration=0;
        s.uiSourcePublishedTargetGeneration=0;
        s.imageCommitted = true; s.active = false;
        return true;
    }

void VehicleWindowGl50DS::Storage::abort(void* context, std::uint8_t) noexcept {
        // Previous private target remains untouched. Never blit or swap here.
        auto& s = *static_cast<Storage*>(context);
        s.unbindSpecularFeedback50LA();
        s.coverageAa0();
        s.active = false;
    }

bool VehicleWindowGl50DS::Storage::copyToWindow() noexcept { return copyToWindow(width,height); }

bool VehicleWindowGl50DS::Storage::copyToWindow(I copyWidth,I copyHeight) noexcept {
        // An unsuccessful or superseded copy must not authorize a later swap.
        if(request.enableOriginalGameplayBlur)gameplayAwaitingTarget=0;
        gl.bindFramebuffer(readFramebuffer, displayedTarget);
        gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer, 0);
        gl.drawBuffer(backBuffer);
        if (!check("prepare default back-buffer blit")) return false;
        // No atomicity claim for default framebuffer writes. On any later
        // error the host must not swap this potentially partial back buffer.
        diagnostic->defaultFramebufferMayBeModified = true;
        gl.blitFramebuffer(0, 0, copyWidth, copyHeight, 0, 0, copyWidth, copyHeight, colorBufferBit, 0x2600); // NEAREST
        if (!check("blit vehicle color to default back buffer")) return false;
        gl.bindFramebuffer(framebuffer, 0);
        const bool accepted=check("restore default framebuffer bindings");
        if(accepted)noteGameplayDisplayPending(displayedTarget,copyWidth,copyHeight);
        return accepted;
    }

bool VehicleWindowGl50DS::Storage::copyRgba(std::uint8_t* destination,U target,I readWidth,I readHeight) noexcept {
        if (!target){target=displayedTarget;readWidth=committedWidth;readHeight=committedHeight;}
        if (!check("before committed RGBA readback")) return false;
        gl.bindFramebuffer(readFramebuffer,target);
        gl.readBuffer(colorAttachment);
        // A caller pointer must never be interpreted as an offset into a PBO.
        // Explicit pack state keeps all writes inside the validated tight extent.
        drawGl.bindBuffer(pixelPackBuffer, 0);
        gl.pixelStorei(0x0D05, 1); // GL_PACK_ALIGNMENT
        gl.pixelStorei(0x0D02, 0); // GL_PACK_ROW_LENGTH
        gl.pixelStorei(0x0D03, 0); // GL_PACK_SKIP_ROWS
        gl.pixelStorei(0x0D04, 0); // GL_PACK_SKIP_PIXELS
        gl.pixelStorei(0x0D00, 0); // GL_PACK_SWAP_BYTES
        gl.pixelStorei(0x0D01, 0); // GL_PACK_LSB_FIRST
        gl.pixelStorei(0x806C, 0); // GL_PACK_IMAGE_HEIGHT
        gl.pixelStorei(0x806B, 0); // GL_PACK_SKIP_IMAGES
        if (!check("configure committed RGBA readback")) return false;
        const U status = gl.checkFramebufferStatus(readFramebuffer);
        if (!check("check committed readback framebuffer")) return false;
        if (status != framebufferComplete) {
            diagnostic->framebufferStatus = status;
            return fail(*diagnostic, VehicleWindowError50DS::readback,
                "committed readback framebuffer is incomplete");
        }
        diagnostic->readbackBufferMayBeModified = true;
        gl.readPixels(0,0,readWidth,readHeight,rgba,unsignedByte,destination);
        return check("read committed viewport RGBA bytes");
    }

bool VehicleWindowGl50DS::Storage::hasGameplayBlurResources() const noexcept {
        return gameplaySceneTarget || gameplaySceneColor || gameplayHistoryTargets[0] ||
            gameplayHistoryTargets[1] || gameplayHistoryColors[0] || gameplayHistoryColors[1] ||
            gameplayBlurProgram || gameplayBlurVertexShader || gameplayBlurFragmentShader ||
            gameplayBlurVertexArray;
    }

bool VehicleWindowGl50DS::Storage::createGameplayBlurTarget(U& target,U& color) noexcept {
        gl.genFramebuffers(1,&target);
        if(!check("allocate gameplay history framebuffer") || !target)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero gameplay history framebuffer");
        gl.genTextures(1,&color);
        if(!check("allocate gameplay history texture") || !color)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero gameplay history texture");
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,color);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        // A null pointer must allocate storage, never name an unpack-PBO offset.
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
        if(!check("allocate cold maximum-size gameplay RGBA8 storage"))return false;
        I w=0,h=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
        gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if(!check("verify gameplay history texture storage"))return false;
        if(w!=maximumWidth || h!=maximumHeight || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"gameplay history texture storage mismatch");
        gl.bindFramebuffer(framebuffer,target);
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,color,0);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("verify gameplay history framebuffer"))return false;
        if(status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete gameplay history framebuffer");}
        return true;
    }

bool VehicleWindowGl50DS::Storage::createGameplayBlur() noexcept {
        if(!request.enableOriginalGameplayBlur)return true;
        I textureLimit=0,units=0;
        gl.getIntegerv(0x0D33,&textureLimit);gl.getIntegerv(0x8872,&units);
        if(!check("query gameplay blur native texture limits"))return false;
        if(textureLimit<maximumWidth || textureLimit<maximumHeight || units<2)
            return fail(*diagnostic,VehicleWindowError50DS::limits,"gameplay blur needs two fragment units and bounded texture dimensions");
        if(!createGameplayBlurTarget(gameplaySceneTarget,gameplaySceneColor))return false;
        for(U b=0;b<2;++b)
            if(!createGameplayBlurTarget(gameplayHistoryTargets[b],gameplayHistoryColors[b]))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::vertex;
        if(!compileTrackShader(0x8B31,native::gameplayBlurNativeVertex,gameplayBlurVertexShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::fragment;
        if(!compileTrackShader(0x8B30,native::gameplayBlurNativeFragment,gameplayBlurFragmentShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::link;
        gameplayBlurProgram=shaderGl.createProgram();
        if(!check("create gameplay blur program") || !gameplayBlurProgram)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"zero gameplay blur program");
        shaderGl.attachShader(gameplayBlurProgram,gameplayBlurVertexShader);
        shaderGl.attachShader(gameplayBlurProgram,gameplayBlurFragmentShader);
        shaderGl.bindFragDataLocation(gameplayBlurProgram,vehicleGlColorOutput50CZ,"composedColor");
        shaderGl.linkProgram(gameplayBlurProgram);
        I linked=0;shaderGl.getProgramiv(gameplayBlurProgram,0x8B82,&linked);
        if(!check("link gameplay blur program"))return false;
        if(!linked){
            shaderGl.getProgramInfoLog(gameplayBlurProgram,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read gameplay blur program log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"gameplay blur program link failed");
        }
        gameplayHistoryUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"historyImage");
        gameplayCurrentUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"currentImage");
        gameplayOriginalSizeUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"originalSize");
        gameplayNativeSizeUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"nativeViewportSize");
        gameplayHistorySizeUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"historyViewportSize");
        gameplayViewportUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"originalViewport");
        gameplayFixedUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"fixedCoefficient");
        if(!check("locate gameplay blur uniforms"))return false;
        if(gameplayHistoryUniform<0 || gameplayCurrentUniform<0 || gameplayOriginalSizeUniform<0 ||
            gameplayNativeSizeUniform<0 || gameplayHistorySizeUniform<0 || gameplayViewportUniform<0 || gameplayFixedUniform<0)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"missing gameplay blur uniform");
        gl.genVertexArrays(1,&gameplayBlurVertexArray);
        if(!check("allocate gameplay blur vertex array") || !gameplayBlurVertexArray)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero gameplay blur vertex array");
        gl.bindTexture(texture2D,0);
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::none;
        return check("finish cold gameplay blur resources");
    }

bool VehicleWindowGl50DS::Storage::validateGameplayBlurFrame(const GameplayBlurFrame50DS* input,VehicleWindowDiagnostic50DS& d) const noexcept {
        if(!input)return true;
        if(!request.enableOriginalGameplayBlur || !input->pending || input->logicalView!=0 || input->physicalView!=0)
            return fail(d,VehicleWindowError50DS::frame,"gameplay blur requires real producer queue and admitted single view0");
        const auto& r=input->cameraRectangle;
        const native::GameplayBlurNativePassParameters p{input->originalWidth,input->originalHeight,
            r.left,r.top,r.right,r.bottom,0};
        if(!native::admitGameplayBlurNativePass(p))
            return fail(d,VehicleWindowError50DS::frame,"gameplay blur original camera rectangle unavailable");
        // This join renders one original view. Never silently consume or discard
        // another view's real queue while pretending split-screen is supported.
        for(U i=1;i<4;++i)if(input->pending->requested[i] || gameplayBlurVideo.view[i].fixed)
            return fail(d,VehicleWindowError50DS::frame,"gameplay blur split-view owner is not joined");
        return true;
    }

bool VehicleWindowGl50DS::Storage::restoreGameplayBlurState(U target) noexcept {
        gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);
        if(gl.bindSampler)gl.bindSampler(1,0);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,0);
        if(gl.bindSampler)gl.bindSampler(0,0);
        drawGl.bindVertexArray(0);
        gl.bindFramebuffer(framebuffer,target);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        gl.viewport(0,0,width,height);gl.colorMask(1,1,1,1);
        resetPodState();
        return check("restore native scene state after gameplay blur without clearing depth");
    }

bool VehicleWindowGl50DS::Storage::drawGameplayBlur(std::uint8_t bank) noexcept {
        if(!frameGameplayBlurEnabled)return true;
        // Original manager consumption is a Render3D event. It does not advance
        // on simulation ticks, a readback, a pause menu or display fade samples.
        const auto result=game::consumeGameplayBlur(*frameGameplayBlur.pending,0,0,
            frameGameplayBlur.cameraRectangle,gameplayBlurVideo);
        if(result!=game::GameplayBlurConsumeResult::consumed && result!=game::GameplayBlurConsumeResult::no_request)
            return fail(*diagnostic,VehicleWindowError50DS::frame,"original gameplay blur queue consumption rejected");
        diagnostic->gameplayBlurRequestConsumed=result==game::GameplayBlurConsumeResult::consumed;
        const auto& video=gameplayBlurVideo.view[0];
        diagnostic->gameplayBlurFixed=video.fixed;
        if(!video.fixed)return true;
        if(!gameplayHistoryValid)
            return fail(*diagnostic,VehicleWindowError50DS::noImage,"positive gameplay blur has no actual completed display history");
        const auto& r=video.rectangle;
        const native::GameplayBlurNativePassParameters p{frameGameplayBlur.originalWidth,frameGameplayBlur.originalHeight,
            r.left,r.top,r.right,r.bottom,video.fixed};
        if(!native::admitGameplayBlurNativePass(p))
            return fail(*diagnostic,VehicleWindowError50DS::frame,"persistent gameplay blur rectangle exceeds current original video extent");
        // Renderbuffer scene -> distinct immutable texture. The output remains
        // the inactive native race FBO; neither sampled image is attached there.
        gl.disable(0x0C11);gl.disable(0x8DB9);
        gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,gameplaySceneTarget);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        bool accepted=check("snapshot completed Render3D scene for gameplay blur");
        if(accepted){
            gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
            if(gl.clipControl)gl.clipControl(0x8CA1,0x935E);
            gl.viewport(0,0,width,height);
            constexpr U disabled[]={0x0BE2,0x0B71,0x0B90,0x0C11,0x0B44,0x8C89,0x0BF2,0x0BD0,0x8DB9,
                0x8037,0x809D,0x80A0,0x809E};
            for(U capability:disabled)gl.disable(capability);
            for(I i=0;i<clipDistances;++i)gl.disable(0x3000+static_cast<U>(i));
            gl.depthMask(0);gl.colorMask(1,1,1,1);gl.polygonMode(0x0408,0x1B02);
            gl.useProgram(gameplayBlurProgram);drawGl.bindVertexArray(gameplayBlurVertexArray);
            gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,gameplayHistoryColors[gameplayHistoryPublished]);
            if(gl.bindSampler)gl.bindSampler(0,0);
            gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,gameplaySceneColor);
            if(gl.bindSampler)gl.bindSampler(1,0);
            gl.uniform1i(gameplayHistoryUniform,0);gl.uniform1i(gameplayCurrentUniform,1);
            gl.uniform2i(gameplayOriginalSizeUniform,p.originalWidth,p.originalHeight);
            gl.uniform2i(gameplayNativeSizeUniform,width,height);
            gl.uniform2i(gameplayHistorySizeUniform,gameplayHistoryWidth,gameplayHistoryHeight);
            gl.uniform4i(gameplayViewportUniform,p.left,p.top,p.right,p.bottom);
            gl.uniform1i(gameplayFixedUniform,p.fixed);
            accepted=check("bind native original gameplay blur inputs");
            if(accepted){gl.drawArrays(0x0004,0,3);accepted=check("draw original displayed-history gameplay blur");}
            if(accepted)++diagnostic->gameplayBlurDraws;
        }
        const bool restored=restoreGameplayBlurState(targets[bank]);
        return accepted && restored;
    }

void VehicleWindowGl50DS::Storage::noteGameplayDisplayPending(U target,I w,I h) noexcept {
        if(!request.enableOriginalGameplayBlur)return;
        gameplayAwaitingTarget=target;gameplayAwaitingWidth=w;gameplayAwaitingHeight=h;
    }

bool VehicleWindowGl50DS::Storage::stageGameplayDisplayedHistory(U target,I w,I h) noexcept {
        if(!request.enableOriginalGameplayBlur)return true;
        gameplayHistoryStaged=false;
        if(!target || target==gameplayHistoryTargets[0] || target==gameplayHistoryTargets[1] ||
            w<=0 || h<=0 || w>maximumWidth || h>maximumHeight ||
            gameplayHistoryGeneration==std::numeric_limits<std::uint64_t>::max())
            return fail(*diagnostic,VehicleWindowError50DS::frame,"actual display history source, extent or generation unavailable");
        const auto pending=static_cast<std::uint8_t>(gameplayHistoryPublished^1u);
        gl.disable(0x0C11);gl.disable(0x8DB9);
        gl.bindFramebuffer(readFramebuffer,target);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,gameplayHistoryTargets[pending]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,w,h,0,0,w,h,colorBufferBit,0x2600);
        if(!check("stage final actual display into inactive gameplay history"))return false;
        gameplayStagedWidth=w;gameplayStagedHeight=h;gameplayHistoryStaged=true;
        return true;
    }

void VehicleWindowGl50DS::Storage::acceptGameplayDisplayedHistory() noexcept {
        if(!request.enableOriginalGameplayBlur)return;
        // Caller has completed staging GPU work and the actual swap, or the
        // explicit headless equivalent. Nothing here can fail after that event.
        gameplayHistoryPublished^=1u;
        gameplayHistoryWidth=gameplayStagedWidth;gameplayHistoryHeight=gameplayStagedHeight;
        gameplayHistoryValid=true;gameplayHistoryStaged=false;++gameplayHistoryGeneration;
        gameplayAwaitingTarget=0;gameplayAwaitingWidth=gameplayAwaitingHeight=0;
    }

bool VehicleWindowGl50DS::Storage::deleteGameplayBlurNames(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!deleteShaderName(gameplayBlurProgram,true,d) ||
            !deleteShaderName(gameplayBlurVertexShader,false,d) ||
            !deleteShaderName(gameplayBlurFragmentShader,false,d) ||
            !deleteName(gameplayBlurVertexArray,gl.deleteVertexArrays,d))return false;
        for(U b=0;b<2;++b)
            if(!deleteName(gameplayHistoryTargets[b],gl.deleteFramebuffers,d) ||
                !deleteName(gameplayHistoryColors[b],gl.deleteTextures,d))return false;
        return deleteName(gameplaySceneTarget,gl.deleteFramebuffers,d) &&
            deleteName(gameplaySceneColor,gl.deleteTextures,d);
    }

bool VehicleWindowGl50DS::Storage::hasFxaaResources() const noexcept {
        return fxaaSceneTarget || fxaaSceneColor || fxaaProgram || fxaaVertexShader ||
            fxaaFragmentShader || fxaaVertexArray;
    }

bool VehicleWindowGl50DS::Storage::createFxaa() noexcept {
        fxaaEnabled=native::fxaaRequested50DS();
        std::fprintf(stderr,"native_fxaa requested=%u storage=%dx%d\n",fxaaEnabled?1u:0u,
            static_cast<int>(maximumWidth),static_cast<int>(maximumHeight));
        if(!fxaaEnabled)return true;
        // Same cold maximum-size RGBA8 texture/FBO pair as the blur snapshot.
        if(!createGameplayBlurTarget(fxaaSceneTarget,fxaaSceneColor))return false;
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,fxaaSceneColor);
        gl.texParameteri(texture2D,0x2801,0x2601);gl.texParameteri(texture2D,0x2800,0x2601); // LINEAR
        if(!check("select FXAA bilinear sampling"))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::vertex;
        if(!compileTrackShader(0x8B31,native::fxaaNativeVertex,fxaaVertexShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::fragment;
        if(!compileTrackShader(0x8B30,native::fxaaNativeFragment,fxaaFragmentShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::link;
        fxaaProgram=shaderGl.createProgram();
        if(!check("create FXAA program") || !fxaaProgram)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"zero FXAA program");
        shaderGl.attachShader(fxaaProgram,fxaaVertexShader);
        shaderGl.attachShader(fxaaProgram,fxaaFragmentShader);
        shaderGl.bindFragDataLocation(fxaaProgram,vehicleGlColorOutput50CZ,"smoothedColor");
        shaderGl.linkProgram(fxaaProgram);
        I linked=0;shaderGl.getProgramiv(fxaaProgram,0x8B82,&linked);
        if(!check("link FXAA program"))return false;
        if(!linked){
            shaderGl.getProgramInfoLog(fxaaProgram,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read FXAA program log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"FXAA program link failed");
        }
        fxaaImageUniform=shaderGl.getUniformLocation(fxaaProgram,"sceneImage");
        fxaaInverseSizeUniform=shaderGl.getUniformLocation(fxaaProgram,"inverseTextureSize");
        fxaaMaximumUvUniform=shaderGl.getUniformLocation(fxaaProgram,"maximumUv");
        if(!check("locate FXAA uniforms"))return false;
        if(fxaaImageUniform<0 || fxaaInverseSizeUniform<0 || fxaaMaximumUvUniform<0)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"missing FXAA uniform");
        gl.genVertexArrays(1,&fxaaVertexArray);
        if(!check("allocate FXAA vertex array") || !fxaaVertexArray)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero FXAA vertex array");
        gl.bindTexture(texture2D,0);
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::none;
        return check("finish cold FXAA resources");
    }

bool VehicleWindowGl50DS::Storage::drawFxaa(std::uint8_t bank) noexcept {
        if(!fxaaEnabled)return true;
        if(width<=0||height<=0||width>maximumWidth||height>maximumHeight)
            return fail(*diagnostic,VehicleWindowError50DS::frame,"FXAA frame extent outside cold storage");
        // Save every piece of state this pass touches; the HUD layers that
        // follow see exactly the state the 3D scene left behind.
        constexpr U capabilities[]={0x0BE2,0x0B71,0x0B90,0x0C11,0x0B44,0x8C89,0x0BF2,0x0BD0,0x8DB9,
            0x8037,0x809D,0x80A0,0x809E};
        constexpr U capabilityCount=sizeof(capabilities)/sizeof(capabilities[0]);
        I enabled[capabilityCount]{};
        for(U i=0;i<capabilityCount;++i)gl.getIntegerv(capabilities[i],&enabled[i]);
        I clipEnabled[8]{};
        const I clips=clipDistances<8?clipDistances:8;
        for(I i=0;i<clips;++i)gl.getIntegerv(0x3000+static_cast<U>(i),&clipEnabled[i]);
        I program=0,vertexArray=0,activeUnit=0,texture0=0,sampler0=0,drawTarget=0,readTarget=0;
        I drawBufferName=0,readBufferName=0,viewport[4]{},colorWrite[4]{},depthWrite=0;
        gl.getIntegerv(0x8B8D,&program);gl.getIntegerv(0x85B5,&vertexArray);
        gl.getIntegerv(0x84E0,&activeUnit);
        gl.activeTexture(textureUnit0);
        gl.getIntegerv(0x8069,&texture0);if(gl.bindSampler)gl.getIntegerv(0x8919,&sampler0);
        gl.getIntegerv(0x8CA6,&drawTarget);gl.getIntegerv(0x8CAA,&readTarget);
        gl.getIntegerv(0x0C01,&drawBufferName);gl.getIntegerv(0x0C02,&readBufferName);
        gl.getIntegerv(0x0BA2,viewport);gl.getIntegerv(0x0C23,colorWrite);gl.getIntegerv(0x0B72,&depthWrite);
        if(!check("save native scene state before FXAA"))return false;
        // Renderbuffer scene -> separate texture; the output is the same target.
        gl.disable(0x0C11);gl.disable(0x8DB9);
        gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,fxaaSceneTarget);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        bool accepted=check("snapshot completed 3D scene for FXAA");
        if(accepted){
            gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
            gl.viewport(0,0,width,height);
            for(U capability:capabilities)gl.disable(capability);
            for(I i=0;i<clips;++i)gl.disable(0x3000+static_cast<U>(i));
            gl.depthMask(0);gl.colorMask(1,1,1,1);gl.polygonMode(0x0408,0x1B02);
            gl.useProgram(fxaaProgram);drawGl.bindVertexArray(fxaaVertexArray);
            gl.bindTexture(texture2D,fxaaSceneColor);if(gl.bindSampler)gl.bindSampler(0,0);
            const float inverse[2]={1.f/static_cast<float>(maximumWidth),1.f/static_cast<float>(maximumHeight)};
            const float maximumUv[2]={(static_cast<float>(width)-.5f)*inverse[0],(static_cast<float>(height)-.5f)*inverse[1]};
            gl.uniform1i(fxaaImageUniform,0);gl.uniform2fv(fxaaInverseSizeUniform,1,inverse);
            gl.uniform2fv(fxaaMaximumUvUniform,1,maximumUv);
            accepted=check("bind FXAA inputs");
            if(accepted){gl.drawArrays(0x0004,0,3);accepted=check("draw FXAA");}
            if(accepted&&(++fxaaDraws==1||fxaaDraws%3125==0))
                std::fprintf(stderr,"native_fxaa draws=%llu extent=%dx%d\n",
                    static_cast<unsigned long long>(fxaaDraws),static_cast<int>(width),static_cast<int>(height));
            if(accepted&&fxaaDraws==1500){
                // One-time measurement of the actual output (no image kept):
                // how many pixels FXAA changed and by how much.
                const std::size_t count=static_cast<std::size_t>(width)*static_cast<std::size_t>(height);
                std::unique_ptr<std::uint8_t[]> before(new (std::nothrow) std::uint8_t[count*4]);
                std::unique_ptr<std::uint8_t[]> after(new (std::nothrow) std::uint8_t[count*4]);
                if(before&&after&&copyRgba(before.get(),fxaaSceneTarget,width,height)&&
                   copyRgba(after.get(),targets[bank],width,height)){
                    std::size_t changed=0;unsigned largest=0;unsigned long long total=0;
                    for(std::size_t i=0;i<count;++i){
                        unsigned delta=0;
                        for(unsigned c=0;c<3;++c){const int d=int(after[i*4+c])-int(before[i*4+c]);delta=std::max(delta,unsigned(d<0?-d:d));}
                        if(delta){++changed;total+=delta;largest=std::max(largest,delta);}
                    }
                    std::fprintf(stderr,"native_fxaa_check draw=%llu pixels=%zu changed=%zu changed_percent=%.2f mean_change=%.1f max_change=%u\n",
                        static_cast<unsigned long long>(fxaaDraws),count,changed,100.0*double(changed)/double(count),
                        changed?double(total)/double(changed):0.0,largest);
                }else std::fprintf(stderr,"native_fxaa_check unavailable\n");
                accepted=check("FXAA one-time output measurement");
            }
        }
        // Restore regardless of the draw outcome.
        gl.useProgram(static_cast<U>(program));drawGl.bindVertexArray(static_cast<U>(vertexArray));
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,static_cast<U>(texture0));
        if(gl.bindSampler)gl.bindSampler(0,static_cast<U>(sampler0));
        gl.activeTexture(static_cast<U>(activeUnit));
        gl.bindFramebuffer(drawFramebuffer,static_cast<U>(drawTarget));gl.bindFramebuffer(readFramebuffer,static_cast<U>(readTarget));
        gl.drawBuffer(static_cast<U>(drawBufferName));gl.readBuffer(static_cast<U>(readBufferName));
        gl.viewport(viewport[0],viewport[1],viewport[2],viewport[3]);
        gl.colorMask(colorWrite[0]?1:0,colorWrite[1]?1:0,colorWrite[2]?1:0,colorWrite[3]?1:0);
        gl.depthMask(depthWrite?1:0);
        for(U i=0;i<capabilityCount;++i){if(enabled[i])gl.enable(capabilities[i]);else gl.disable(capabilities[i]);}
        for(I i=0;i<clips;++i){const U c=0x3000+static_cast<U>(i);if(clipEnabled[i])gl.enable(c);else gl.disable(c);}
        const bool restored=check("restore native scene state after FXAA");
        return accepted&&restored;
    }

bool VehicleWindowGl50DS::Storage::deleteFxaaNames(VehicleWindowDiagnostic50DS& d) noexcept {
        return deleteShaderName(fxaaProgram,true,d) &&
            deleteShaderName(fxaaVertexShader,false,d) &&
            deleteShaderName(fxaaFragmentShader,false,d) &&
            deleteName(fxaaVertexArray,gl.deleteVertexArrays,d) &&
            deleteName(fxaaSceneTarget,gl.deleteFramebuffers,d) &&
            deleteName(fxaaSceneColor,gl.deleteTextures,d);
    }

} // namespace rr::platform
