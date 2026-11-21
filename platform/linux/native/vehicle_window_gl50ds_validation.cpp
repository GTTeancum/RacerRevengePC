#include "vehicle_window_gl50ds_storage.h"
#include "../../../tests/native/common_track50cr.h"

namespace rr::platform {
namespace vehicle_window_gl50ds_detail {
bool fail(VehicleWindowDiagnostic50DS& d, VehicleWindowError50DS error,
    const char* operation) noexcept {
    if (d.error == VehicleWindowError50DS::none) { d.error = error; d.operation = operation; }
    return false;
}

U componentIndex(game::VehicleDrawComponent50CM component) noexcept {
    for (U c = 0; c < maxComponents; ++c) if (components[c] == component) return c;
    return maxComponents;
}

bool sourceBodyComponent50LA(U component, U componentCount) noexcept {
    // The generic draw interface also accepts four components. Source body
    // admission requires a complete three-model pod or actual five-model pod.
    // Every model, chariot included, runs the same 241280 owner path; which
    // route it takes is decided by its material programs, not its index.
    return (componentCount == 3 || componentCount == 5) &&
        component < sourceBodyCount50LA && component < componentCount;
}

U trackSpanChunkCount(U indexCount, bool textured, U maximumTriangles) noexcept {
    if (!textured || !maximumTriangles) return 1;
    const U triangles = indexCount / 3;
    return triangles / maximumTriangles + (triangles % maximumTriangles != 0);
}

bool validateMaterialPolicy(const VehicleWindowMaterialBinding50DS& material,
    VehicleWindowDiagnostic50DS& d) noexcept {
    if ((material.blend != VehicleWindowBlend50DS::opaque &&
        material.blend != VehicleWindowBlend50DS::sourceAlpha &&
        material.blend != VehicleWindowBlend50DS::additive) ||
        !std::isfinite(material.alphaScale) || material.alphaScale < 0.f || material.alphaScale > 8.f ||
        (material.fixedBlendAlpha && material.blend != VehicleWindowBlend50DS::additive) ||
        (material.blend == VehicleWindowBlend50DS::opaque && material.alphaScale != 1.f))
        return fail(d, VehicleWindowError50DS::material, "invalid explicit blend/alpha material policy");
    // A single diffuse pass 1 may carry GS source alpha (A0 B1 C0 D1, e.g. the
    // pod canopy); its shader emits As/128 as the GL factor (alphaScale 1).
    if (material.sourceDiffuseCompressed50LA &&
        (material.mode != VehicleWindowMaterialMode50DS::requiredTexture ||
         (material.blend != VehicleWindowBlend50DS::opaque &&
          !(material.blend == VehicleWindowBlend50DS::sourceAlpha && material.sourceSingleDiffuseRgba50LA)) ||
         material.unlit || material.sourcePrelit || material.fixedBlendAlpha || material.alphaScale != 1.f))
        return fail(d, VehicleWindowError50DS::material,
            "source compressed diffuse requires a lit opaque base-map texture policy");
    if (material.sourceSpecularPair50LA) {
        if (!material.sourceDiffuseCompressed50LA || material.sourceSingleDiffuseRgba50LA || material.sourceBaseAlphaTest50LA || !material.depthWrite || !material.depthEqual ||
            material.specularTextureIndex50LA == assets::unresolvedMaterialTexture50EA)
            return fail(d, VehicleWindowError50DS::material,
                "source specular pair requires source diffuse, real map 2 and inclusive writing base depth");
    } else if (material.sourceSingleDiffuseRgba50LA) {
        if (!material.sourceDiffuseCompressed50LA || !material.depthWrite || !material.depthEqual ||
            material.specularTextureIndex50LA != assets::unresolvedMaterialTexture50EA ||
            material.sourceSpecularFog50LA || material.sourceSpecularAlphaTest50LA)
            return fail(d,VehicleWindowError50DS::material,"single diffuse requires its own source RGBA depth contract without map 2");
    } else if (material.specularTextureIndex50LA != assets::unresolvedMaterialTexture50EA ||
        material.sourceBaseFog50LA || material.sourceBaseAlphaTest50LA || material.sourceSpecularFog50LA || material.sourceSpecularAlphaTest50LA)
        return fail(d, VehicleWindowError50DS::material, "unpaired material must not carry specular state");
    if (material.extraPass50PP == VehicleWindowExtraPass50PP::none) {
        if (material.extraPassTextureIndex50PP != assets::unresolvedMaterialTexture50EA ||
            material.extraPassFixScale50PP != 0.f || material.extraPassFog50PP || material.extraPassAlphaTest50PP)
            return fail(d, VehicleWindowError50DS::material, "single-pass material must not carry second-pass state");
    } else if ((material.extraPass50PP != VehicleWindowExtraPass50PP::specularFixed &&
                material.extraPass50PP != VehicleWindowExtraPass50PP::specularDestinationAlpha) ||
        !material.sourceSingleDiffuseRgba50LA ||
        material.extraPassTextureIndex50PP == assets::unresolvedMaterialTexture50EA ||
        !std::isfinite(material.extraPassFixScale50PP) ||
        (material.extraPass50PP == VehicleWindowExtraPass50PP::specularFixed &&
         (material.extraPassFixScale50PP < 0.f || material.extraPassFixScale50PP > 255.f / 128.f)) ||
        (material.extraPass50PP == VehicleWindowExtraPass50PP::specularDestinationAlpha &&
         (material.extraPassFixScale50PP != 0.f || material.blend != VehicleWindowBlend50DS::opaque)))
        return fail(d, VehicleWindowError50DS::material,
            "second pass requires a single diffuse pass 1, a real map and its GS FIX or opaque Ad form");
    return true;
}

bool validateMaterials(const VehicleWindowRequest50DS& request,
    const assets::VehicleDrawAssets50CR& owner, VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& input = request.materials;
    if (input.componentCount != owner.componentCount())
        return fail(d, VehicleWindowError50DS::material, "material component count must match actual assets");
    if (request.mode == VehicleWindowMode50DS::viewerNeutralUntextured) {
        if (input.textures || input.specularTextures50LA)
            return fail(d, VehicleWindowError50DS::request, "neutral mode must not supply a texture bank");
        for (const auto& component : input.component)
            if (component.bindings || component.count)
                return fail(d, VehicleWindowError50DS::request, "neutral mode must not supply material bindings");
        return true;
    }
    if (!input.textures || !input.textures->validBorrow(request.context, request.isCurrent,
        input.textures->publicationToken()))
        return fail(d, VehicleWindowError50DS::material,
            "published texture bank with exact current context/callback/thread required");
    if (input.specularTextures50LA && !input.specularTextures50LA->validBorrow(
        request.context, request.isCurrent, input.specularTextures50LA->publicationToken()))
        return fail(d, VehicleWindowError50DS::material,
            "map-2 publication requires the same current context, callback and thread");
    bool anyPair = false, anyExtra = false;
    for (U c = 0; c < owner.componentCount(); ++c) {
        d.component = c;
        d.material = d.texture = assets::unresolvedMaterialTexture50EA;
        const auto* scene = owner.scene(components[c]);
        const auto& component = input.component[c];
        if (!scene || scene->materials.empty() || scene->materials.size() > maxVehicleWindowMaterials50DS ||
            component.count != scene->materials.size() || !component.bindings)
            return fail(d, VehicleWindowError50DS::material,
                "one explicit binding per original scene material within 4096-entry cap required");
        for (U m = 0; m < component.count; ++m) {
            d.material = m;
            const auto& binding = component.bindings[m];
            d.texture = binding.textureIndex;
            if (!validateMaterialPolicy(binding, d)) return false;
            if (binding.sourceDiffuseCompressed50LA &&
                !sourceBodyComponent50LA(c, owner.componentCount()))
                return fail(d, VehicleWindowError50DS::material,
                    "source compressed diffuse requires actual pod components in a three/five-model pod");
            if (binding.sourceSpecularPair50LA) {
                if (!sourceBodyComponent50LA(c, owner.componentCount()))
                    return fail(d, VehicleWindowError50DS::material,
                        "source specular pair requires actual pod components in a three/five-model pod");
                anyPair = true;
                const auto* map2 = input.specularTextures50LA ?
                    input.specularTextures50LA->asset(binding.specularTextureIndex50LA) : nullptr;
                if (!map2 || !map2->name)
                    return fail(d, VehicleWindowError50DS::material, "source pair has no actual published map 2");
            }
            if (binding.extraPass50PP != VehicleWindowExtraPass50PP::none) {
                anyExtra = true;
                const auto* extra = input.specularTextures50LA ?
                    input.specularTextures50LA->asset(binding.extraPassTextureIndex50PP) : nullptr;
                if (!extra || !extra->name)
                    return fail(d, VehicleWindowError50DS::material, "second pass has no actual published map");
            }
            if (binding.mode == VehicleWindowMaterialMode50DS::untextured) {
                if (binding.textureIndex != assets::unresolvedMaterialTexture50EA)
                    return fail(d, VehicleWindowError50DS::material,
                        "explicit untextured material must not name a texture");
            } else if (binding.mode == VehicleWindowMaterialMode50DS::requiredTexture) {
                const auto* texture = input.textures->asset(binding.textureIndex);
                if (!texture || !texture->name)
                    return fail(d, VehicleWindowError50DS::material, "required material texture is not published");
            } else {
                return fail(d, VehicleWindowError50DS::material, "material mode must be explicitly selected");
            }
        }
    }
    if ((anyPair || anyExtra) != bool(input.specularTextures50LA))
        return fail(d, VehicleWindowError50DS::material, "map-2 bank presence differs from source pair/second-pass table");
    d.component = maxComponents;
    d.material = d.texture = assets::unresolvedMaterialTexture50EA;
    return true;
}

bool validateCommonTrack(const VehicleWindowRequest50DS& request,VehicleWindowDiagnostic50DS& d) noexcept {
    // Frontend, viewer and host-check routes supply no race track load.
    if(!request.commonTrack)return true;
    const auto& track=*request.commonTrack;const auto& roles=track.roles;
    // One rule for every course: the lease is the loaded track whose static
    // world produced the textured mesh; each borrowed descriptor names data of
    // a role owner the same track load published. No course is consulted.
    if(!track.loaded()||!roles.staticWorld||!request.track.mesh||
       request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures)
        return fail(d,VehicleWindowError50DS::request,"common track lease requires its loaded static world and textured track mesh");
    if((request.sky.mesh&&!roles.sky)||(request.water.environmentTexture&&!roles.water)||
       ((request.particleTextures||request.particleCpuTextures||request.particleMaterial)&&!roles.particles)||
       (request.trackAnimationCount&&!roles.atm&&!roles.animation))
        return fail(d,VehicleWindowError50DS::request,"track descriptor borrows a role the common track load did not publish");
    return true;
}

bool validateTrack(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& track = request.track;
    const auto& fog = track.worldFog;
    if(track.environmentUv){
        // Producer sizes from its own data; both budgets are checked before
        // any allocation: float2 GPU stream per mesh vertex, compact staging.
        const auto& producer=*track.environmentUv;
        const std::uint64_t gpu=track.mesh?std::uint64_t(track.mesh->vertices.size())*sizeof(std::array<float,2>):0;
        const std::uint64_t staging=std::uint64_t(producer.uvCount())*sizeof(std::array<float,2>);
        std::uint64_t ranged=0;bool ordered=producer.ready()&&producer.ranges()&&producer.rangeCount();
        std::uint64_t nextVertex=0;
        for(std::size_t i=0;ordered&&i<producer.rangeCount();++i){const auto& r=producer.ranges()[i];
            ordered=r.vertexCount&&r.vertexStart>=nextVertex&&track.mesh&&
                std::uint64_t(r.vertexStart)+r.vertexCount<=track.mesh->vertices.size();
            nextVertex=std::uint64_t(r.vertexStart)+r.vertexCount;ranged+=r.vertexCount;}
        // The environment UVs and the authored projection share one float2
        // attribute stream, so a track supplies at most one of them.
        if(!ordered||ranged!=producer.uvCount()||track.sourceProjectionUvs||!producer.validate(track)||!gpu||gpu>track.maximumEnvironmentUvBytes||
           gpu>std::uint64_t(std::numeric_limits<std::intptr_t>::max())||!staging||staging>track.maximumEnvironmentStagingBytes)
            return fail(d,VehicleWindowError50DS::request,"environment-map UV producer association/projection budget rejected");
    }else if(track.maximumEnvironmentUvBytes||track.maximumEnvironmentStagingBytes)
        return fail(d,VehicleWindowError50DS::request,"environment-map UV budgets require its producer");
    if (fog.enabled) {
        if (!track.mesh || track.mode != VehicleWindowMode50DS::callerMaterialTextures ||
            !std::isfinite(fog.start) || !std::isfinite(fog.end) ||
            fog.start < 0.f || fog.end <= fog.start)
            return fail(d, VehicleWindowError50DS::request, "world fog requires textured world and finite ordered range");
        for (float value : fog.color)
            if (!std::isfinite(value) || value < 0.f || value > 1.f)
                return fail(d, VehicleWindowError50DS::request, "world fog color outside finite [0,1]");
    }
    // Original shadow receiver tables: both or neither, bounded.
    if (request.staticInstanceCount ? (!request.staticShadowReceiving || !request.staticInstanceZone ||
            request.staticInstanceCount > 100000u) :
        (request.staticShadowReceiving || request.staticInstanceZone))
        return fail(d, VehicleWindowError50DS::request, "original shadow receiver tables incomplete");
    if (request.staticPortalCount ? (!request.staticPortalZones || !request.staticPortalCorners ||
            request.staticPortalCount > 4096u) : (request.staticPortalZones || request.staticPortalCorners))
        return fail(d, VehicleWindowError50DS::request, "original shadow portal tables incomplete");
    if (!track.mesh)
        return (!track.maximumVertexBytes && !track.maximumIndexBytes &&
            track.mode == VehicleWindowMode50DS::unspecified && track.vertexColorScale == 1.f &&
            !track.textures && !track.spans && !track.spanCount && !track.enableFrustumCulling &&
            !track.cullingChunkTriangles && !track.sourceNormals && !track.sourceNormalCount &&
            !track.maximumNormalBytes && !track.sourceProjectionUvs && !track.sourceProjectionCount &&
            !track.maximumProjectionBytes && !track.environmentUv && !track.maximumEnvironmentUvBytes && !track.maximumEnvironmentStagingBytes) ||
            fail(d, VehicleWindowError50DS::request, "track mode/budgets/culling require an actual visual mesh");
    if ((track.mode != VehicleWindowMode50DS::viewerNeutralUntextured &&
            track.mode != VehicleWindowMode50DS::sourceVertexColors &&
            track.mode != VehicleWindowMode50DS::callerMaterialTextures) ||
        !track.maximumVertexBytes || !track.maximumIndexBytes)
        return fail(d, VehicleWindowError50DS::request, "track requires explicit shading mode and byte budgets");
    if (track.cullingChunkTriangles &&
        (!track.enableFrustumCulling || track.mode != VehicleWindowMode50DS::callerMaterialTextures))
        return fail(d, VehicleWindowError50DS::request,
            "track triangle chunks require textured track mode and enabled frustum culling");
    if (!std::isfinite(track.vertexColorScale) || track.vertexColorScale <= 0.f ||
        track.vertexColorScale > 8.f ||
        (track.mode == VehicleWindowMode50DS::viewerNeutralUntextured && track.vertexColorScale != 1.f))
        return fail(d, VehicleWindowError50DS::request, "track color scale requires source RGB mode and finite (0,8]");
    const auto& mesh = *track.mesh;
    if (mesh.vertices.empty() || mesh.vertices.size() > assets::max_mesh_vertices ||
        mesh.indices.empty() || mesh.indices.size() > assets::max_mesh_indices || mesh.indices.size() % 3 ||
        mesh.indices.size() > static_cast<std::size_t>(std::numeric_limits<I>::max()))
        return fail(d, VehicleWindowError50DS::assets, "track must be a bounded nonempty Mesh48B triangle list");
    const std::uint64_t vertexBytes = std::uint64_t(mesh.vertices.size()) * sizeof(assets::Vertex48B);
    const std::uint64_t indexBytes = std::uint64_t(mesh.indices.size()) * sizeof(U);
    if (vertexBytes > track.maximumVertexBytes || indexBytes > track.maximumIndexBytes ||
        vertexBytes > std::numeric_limits<std::size_t>::max() ||
        indexBytes > std::numeric_limits<std::size_t>::max() ||
        vertexBytes > std::uint64_t(std::numeric_limits<std::intptr_t>::max()) ||
        indexBytes > std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
        return fail(d, VehicleWindowError50DS::limits, "track exceeds explicit vertex/index byte budgets or GL size limits");
    if (track.sourceNormals) {
        const std::uint64_t normalBytes = std::uint64_t(track.sourceNormalCount) * sizeof(assets::WorldNormal50HK);
        if (track.mode == VehicleWindowMode50DS::viewerNeutralUntextured ||
            track.sourceNormalCount != mesh.vertices.size() || !track.maximumNormalBytes ||
            normalBytes > track.maximumNormalBytes ||
            normalBytes > std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
            return fail(d, VehicleWindowError50DS::limits, "source normals require source RGB mode, exact vertex count and byte budget");
        for (U n = 0; n < track.sourceNormalCount; ++n) {
            const auto& v = track.sourceNormals[n].xyz;
            const double length = std::hypot(v[0], v[1], v[2]);
            if (!std::isfinite(length) || (length != 0.0 && std::abs(length - 1.0) > 0.0001))
                return fail(d, VehicleWindowError50DS::assets, "source normals must be finite unit vectors or explicit zero");
        }
    } else if (track.sourceNormalCount || track.maximumNormalBytes) {
        return fail(d, VehicleWindowError50DS::request, "normal count/budget require an explicit source stream");
    }
    if (track.sourceProjectionUvs) {
        const std::uint64_t bytes = std::uint64_t(track.sourceProjectionCount) * sizeof(assets::TrackProjectedUv);
        if (track.mode != VehicleWindowMode50DS::callerMaterialTextures ||
            track.sourceProjectionCount != mesh.vertices.size() || !track.maximumProjectionBytes ||
            bytes > track.maximumProjectionBytes ||
            bytes > std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
            return fail(d, VehicleWindowError50DS::limits, "projected UV stream requires exact vertex count and explicit byte budget");
        for (U v = 0; v < track.sourceProjectionCount; ++v)
            for (float uv : track.sourceProjectionUvs[v].uv)
                if (!std::isfinite(uv))
                    return fail(d, VehicleWindowError50DS::assets, "nonfinite authored projected UV");
    } else if (track.sourceProjectionCount || track.maximumProjectionBytes) {
        return fail(d, VehicleWindowError50DS::request, "projected UV count/budget require a source stream");
    }
    // Mesh48B can be built or changed after parsing. Recheck all geometry
    // before allocating even the renderer owner; do not trust parser history.
    for (const auto& vertex : mesh.vertices) {
        for (float value : vertex.position)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::assets, "track contains nonfinite world position");
        for (float value : vertex.uv)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::assets, "track contains nonfinite UV");
    }
    for (U index : mesh.indices)
        if (index >= mesh.vertices.size())
            return fail(d, VehicleWindowError50DS::indices, "track index outside vertex array");
    if (track.mode == VehicleWindowMode50DS::callerMaterialTextures) {
        if (!track.textures || !track.textures->validBorrow(request.context, request.isCurrent,
            track.textures->publicationToken()) || !track.spans || !track.spanCount ||
            track.spanCount > maxVehicleWindowTrackSpans50DS)
            return fail(d, VehicleWindowError50DS::material,
                "track textures require current published bank and 1..65536 explicit source spans");
        U nextIndex = 0, nextVertex = 0;
        for (U i = 0; i < track.spanCount; ++i) {
            d.trackSpan = i;
            const auto& span = track.spans[i];
            d.texture = span.material.textureIndex;
            if (span.nativeCoverage != NativeTrackCoverage50DS::aa0 &&
                span.nativeCoverage != NativeTrackCoverage50DS::fourSampleAa1)
                return fail(d, VehicleWindowError50DS::material, "unknown native track coverage policy");
            if (!validateMaterialPolicy(span.material, d)) return false;
            // Common GS destination-alpha pair, keyed on the lowered material
            // data only: 1 must be followed, 2 preceded, by its partner pass.
            if(span.gsDestinationAlphaPass>2||
               (span.gsDestinationAlphaPass==1&&(i+1>=track.spanCount||
                   !validDestinationAlphaPair50DS(span,track.spans[i+1])))||
               (span.gsDestinationAlphaPass==2&&(!i||
                   !validDestinationAlphaPair50DS(track.spans[i-1],span))))
                return fail(d,VehicleWindowError50DS::material,"GS destination-alpha pass lacks its same-surface partner");
            if(span.sourceZone>track.zoneCount)
                return fail(d,VehicleWindowError50DS::material,"track span zone outside the world's authored zones");
            // The environment-map program is the pair's pass 2 and reads the
            // track's environment UV producer (any course whose data has it).
            if(span.sourceEnvironmentMap&&(!track.environmentUv||span.gsDestinationAlphaPass!=2))
                return fail(d,VehicleWindowError50DS::material,"environment-map pass lacks its projection producer");
            if (span.material.sourceDiffuseCompressed50LA)
                return fail(d, VehicleWindowError50DS::material,
                    "track, dynamic-world and sky spans cannot select racer source diffuse");
            if (!span.sourcePass || !span.sourcePassCount || span.sourcePassCount > 2 ||
                span.sourcePass > span.sourcePassCount || span.projectionPass > 2 ||
                (span.projectionPass && (!track.sourceProjectionUvs ||
                    span.projectionPass != span.sourcePass)))
                return fail(d, VehicleWindowError50DS::material, "invalid original track pass/projection binding");
            if (span.sourcePass == 1) {
                if ((i && track.spans[i-1].sourcePass != track.spans[i-1].sourcePassCount) ||
                    span.indexStart != nextIndex || span.vertexStart != nextVertex ||
                    !span.indexCount || span.indexCount % 3 || !span.vertexCount ||
                    span.indexCount > mesh.indices.size() - nextIndex ||
                    span.vertexCount > mesh.vertices.size() - nextVertex)
                    return fail(d, VehicleWindowError50DS::indices,
                        "first material passes must partition source mesh arrays in order");
                nextIndex += span.indexCount;
                nextVertex += span.vertexCount;
            } else {
                if (!i) return fail(d, VehicleWindowError50DS::indices, "material continuation without first pass");
                const auto& prior = track.spans[i-1];
                if (span.sourcePass != prior.sourcePass+1 || span.sourcePassCount != prior.sourcePassCount ||
                    span.indexStart != prior.indexStart || span.indexCount != prior.indexCount ||
                    span.vertexStart != prior.vertexStart || span.vertexCount != prior.vertexCount)
                    return fail(d, VehicleWindowError50DS::indices, "material passes must repeat the exact preceding source surface");
            }
            for (U n = span.indexStart; n < span.indexStart+span.indexCount; ++n)
                if (mesh.indices[n] < span.vertexStart || mesh.indices[n] >= span.vertexStart+span.vertexCount)
                    return fail(d, VehicleWindowError50DS::indices,
                        "track span index escapes its declared source vertex range");
            if (span.sourceUvPolicy != NativeTrackUvPolicy50DS::legacySinglePass &&
                span.sourceUvPolicy != NativeTrackUvPolicy50DS::uvScrollTwoPass)
                return fail(d,VehicleWindowError50DS::material,"unknown native track UV policy");
            const bool scrollPair=span.sourceUvPolicy==NativeTrackUvPolicy50DS::uvScrollTwoPass;
            if ((span.discardZeroAlpha && span.sourceAlphaTest) ||
                (!scrollPair && span.sourceUvAnimation && (span.sourcePass != 1 || span.sourcePassCount != 1 ||
                    span.projectionPass || span.material.mode != VehicleWindowMaterialMode50DS::requiredTexture)))
                return fail(d,VehicleWindowError50DS::material,
                    "source UV service requires one textured unprojected pass and one alpha-test mode");
            if(scrollPair) {
                // Both policies are checked before any GL allocation/publication.
                const U first=span.sourcePass==1?i:i-1;
                if(first+1>=track.spanCount ||
                    !validUvScrollTwoPassPair50DS(track.spans[first],track.spans[first+1]))
                    return fail(d,VehicleWindowError50DS::material,"SC lava exact adjacent two-pass UV/material pair differs");
            }
            if (span.material.mode == VehicleWindowMaterialMode50DS::untextured) {
                if (span.material.textureIndex != assets::unresolvedMaterialTexture50EA || span.discardZeroAlpha)
                    return fail(d, VehicleWindowError50DS::material,
                        "untextured track span must retain unresolved texture sentinel");
            } else if (span.material.mode == VehicleWindowMaterialMode50DS::requiredTexture) {
                const auto* texture = track.textures->asset(span.material.textureIndex);
                if (!texture || !texture->name)
                    return fail(d, VehicleWindowError50DS::material,
                        "required track span texture is not published");
            } else {
                return fail(d, VehicleWindowError50DS::material, "track span shading must be explicit");
            }
        }
        if (track.spans[track.spanCount-1].sourcePass != track.spans[track.spanCount-1].sourcePassCount ||
            nextIndex != mesh.indices.size() || nextVertex != mesh.vertices.size())
            return fail(d, VehicleWindowError50DS::indices, "track spans must cover the complete source mesh");
        if (track.cullingChunkTriangles) {
            // Keep the complete original source validation above. Bound the
            // separate candidate table before renderer allocation or GL work.
            std::uint64_t chunks = 0;
            for (U i = 0; i < track.spanCount; ++i) {
                const auto& span = track.spans[i];
                chunks += trackSpanChunkCount(span.indexCount,
                    span.material.mode == VehicleWindowMaterialMode50DS::requiredTexture,
                    track.cullingChunkTriangles);
                if (chunks > maxVehicleWindowTrackChunks50DS) {
                    d.trackSpan = i;
                    d.texture = span.material.textureIndex;
                    return fail(d, VehicleWindowError50DS::limits,
                        "track triangle chunks exceed bounded 65536-candidate capacity");
                }
            }
        }
        d.trackSpan = d.texture = assets::unresolvedMaterialTexture50EA;
    } else if (track.textures || track.spans || track.spanCount) {
        return fail(d, VehicleWindowError50DS::request,
            "neutral/source-color track must not supply texture bank or material spans");
    }
    // Serialized/display bounds are unused. Optional host culling derives its
    // own bounds from the actual indexed world-space geometry after validation.
    return true;
}

bool validateTrackAnimations(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    if (!request.trackAnimationCount)
        return (!request.trackAnimations && !request.trackAnimationTextures) ||
            fail(d, VehicleWindowError50DS::request, "animation bank/descriptors require a nonzero count");
    // Bounded by the data, not a fixed row cap: one animation row per distinct placed
    // span of the track table (duplicates are refused below), and the renderer's tables
    // are heap-sized from this count (createTrackAnimations). The former 128-row guard
    // (since removed from the header) refused RA's 346 crowd spans (every other stock course places 26..76).
    if (request.trackAnimationCount > request.track.spanCount)
        return fail(d, VehicleWindowError50DS::material, "track animation rows exceed the track's placed spans");
    if (!request.trackAnimations || !request.trackAnimationTextures ||
        request.track.mode != VehicleWindowMode50DS::callerMaterialTextures ||
        !request.trackAnimationTextures->validBorrow(request.context, request.isCurrent,
            request.trackAnimationTextures->publicationToken()))
        return fail(d, VehicleWindowError50DS::material, "track animation requires a bounded published source frame bank");
    for (U i = 0; i < request.trackAnimationCount; ++i) {
        const auto& animation = request.trackAnimations[i];
        if (!animation.frameCount || animation.frameCount > maxVehicleWindowAnimationFrames50DS ||
            animation.firstTextureIndex > request.trackAnimationTextures->textureCount() ||
            animation.frameCount > request.trackAnimationTextures->textureCount()-animation.firstTextureIndex)
            return fail(d, VehicleWindowError50DS::limits, "track animation frame range escapes actual published bank");
        for (U previous = 0; previous < i; ++previous)
            if (request.trackAnimations[previous].sourceSpanIndex == animation.sourceSpanIndex)
                return fail(d, VehicleWindowError50DS::material, "duplicate placed animation span association");
        if (animation.sourceSpanIndex >= request.track.spanCount)
            return fail(d, VehicleWindowError50DS::material, "animated source span outside original track table");
        const auto& span = request.track.spans[animation.sourceSpanIndex];
        if(span.sourceEnvironmentMap)
            return fail(d,VehicleWindowError50DS::material,"environment-map pass retains its actual static image; ATM override rejected");
        if(span.sourceUvPolicy==NativeTrackUvPolicy50DS::uvScrollTwoPass)
            return fail(d,VehicleWindowError50DS::material,"two-pass UV scroll retains its static original image; ATM override rejected");
        if (span.material.mode != VehicleWindowMaterialMode50DS::requiredTexture ||
            span.material.textureIndex != animation.sourceTextureIndex)
            return fail(d, VehicleWindowError50DS::material, "animated source span texture identity mismatch");
        for (U frame = 0; frame < animation.frameCount; ++frame) {
            const auto* image = request.trackAnimationTextures->asset(animation.firstTextureIndex+frame);
            if (!image || !image->name)
                return fail(d, VehicleWindowError50DS::material, "original animation frame is not published");
        }
    }
    return true;
}

bool validateDynamicWorld(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    auto selected = request;
    selected.track = request.dynamicWorld;
    selected.staticShadowReceiving = nullptr; selected.staticInstanceZone = nullptr; selected.staticInstanceCount = 0;
    selected.staticPortalZones = nullptr; selected.staticPortalCorners = nullptr; selected.staticPortalCount = 0;
    const auto& world = selected.track;
    if (world.mesh && (world.mode != VehicleWindowMode50DS::callerMaterialTextures ||
        world.mesh->vertices.size() > maxVehicleWindowDynamicVertices50DS ||
        world.mesh->indices.size() > maxVehicleWindowDynamicIndices50DS ||
        world.spanCount > maxVehicleWindowDynamicSpans50DS || world.enableFrustumCulling ||
        world.cullingChunkTriangles || world.sourceProjectionUvs ||
        world.sourceProjectionCount || world.maximumProjectionBytes || world.environmentUv ||
        world.maximumEnvironmentUvBytes || world.maximumEnvironmentStagingBytes))
        return fail(d, VehicleWindowError50DS::limits,
            "dynamic world requires bounded textured topology without static culling/projection");
    const bool result = validateTrack(selected, d);
    if(result)for(U i=0;i<world.spanCount;++i)if(world.spans[i].sourceUvAnimation||world.spans[i].gsDestinationAlphaPass||world.spans[i].sourceEnvironmentMap)
        return fail(d,VehicleWindowError50DS::material,"dynamic UV/crystal controller requires a separate owner binding");
    d.dynamicWorldSpan = d.trackSpan;
    d.trackSpan = assets::unresolvedMaterialTexture50EA;
    return result;
}

bool validateSky(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& sky = request.sky;
    if (!sky.mesh)
        return (!sky.maximumVertexBytes && !sky.maximumIndexBytes && !sky.textures &&
            !sky.spans && !sky.spanCount && !sky.sequenceTextures) ||
            fail(d, VehicleWindowError50DS::request, "sky fields require an actual authored sky mesh");
    VehicleWindowRequest50DS check = request;
    check.track = {};
    check.track.mesh = sky.mesh;
    check.track.maximumVertexBytes = sky.maximumVertexBytes;
    check.track.maximumIndexBytes = sky.maximumIndexBytes;
    check.track.mode = VehicleWindowMode50DS::callerMaterialTextures;
    check.track.textures = sky.textures;
    check.track.spans = sky.spans;
    check.track.spanCount = sky.spanCount;
    if (!validateTrack(check, d)) return false;
    for (U i = 0; i < sky.spanCount; ++i) {
        const auto& material = sky.spans[i].material;
        if (material.blend != VehicleWindowBlend50DS::opaque || material.depthWrite ||
            !material.unlit || sky.spans[i].discardZeroAlpha || sky.spans[i].sourceAlphaTest ||
            sky.spans[i].sourceUvAnimation || sky.spans[i].sourcePass != 1 ||
            sky.spans[i].sourcePassCount != 1 || sky.spans[i].projectionPass)
            return fail(d, VehicleWindowError50DS::material,
                "sky requires explicit unlit opaque non-depth-writing materials");
    }
    // Sky cube sequence bank (12C540 overlay tiles): a complete, current upload.
    if (const auto* seq = sky.sequenceTextures) {
        if (!sky.textures || !seq->textureCount() ||
            !seq->validBorrow(request.context, request.isCurrent, seq->publicationToken()))
            return fail(d, VehicleWindowError50DS::material,
                "sky sequence bank requires the sky bank and a current complete upload");
        for (U i = 0; i < seq->textureCount(); ++i)
            if (!seq->texture(i))
                return fail(d, VehicleWindowError50DS::material, "sky sequence image not uploaded");
    }
    return true;
}

bool validateCables(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& cables = request.cables;
    if (!cables.textures)
        return cables.textureIndex == assets::unresolvedMaterialTexture50EA ||
            fail(d, VehicleWindowError50DS::request, "cable texture index requires an actual texture bank");
    d.texture = cables.textureIndex;
    if (!cables.textures->validBorrow(request.context, request.isCurrent,
        cables.textures->publicationToken()))
        return fail(d, VehicleWindowError50DS::material,
            "cables require an exact current published texture bank");
    const auto* texture = cables.textures->asset(cables.textureIndex);
    if (!texture || !texture->name)
        return fail(d, VehicleWindowError50DS::material, "required actual cable texture is not published");
    d.texture = assets::unresolvedMaterialTexture50EA;
    return true;
}

U cableIndex50DS(U index) noexcept {
    // Canonical 50FD strip topology, up to four nine-sample cables, two triangles
    // per adjacent sample pair. No topology is generated from visual guesses.
    constexpr U offsets[] = {0, 1, 2, 2, 1, 3};
    const U trianglePair = index / 6;
    return (trianglePair / 8) * 18 + (trianglePair % 8) * 2 + offsets[index % 6];
}

bool validateCableFrame(const game::PodCableMesh50FD& mesh, VehicleWindowDiagnostic50DS& d) noexcept {
    static_assert(game::maxPodCableVertexCount50FD == 72 && game::maxPodCableIndexCount50FD == 192,
        "50DS fixed storage requires the admitted 50FD four-cable capacity");
    if (mesh.cableCount != 2 && mesh.cableCount != 4)
        return fail(d, VehicleWindowError50DS::frame, "cable count is outside the admitted source topology");
    const U vertexCount = mesh.cableCount * game::podCableVerticesPerCable50FD;
    const U indexCount = mesh.cableCount * game::podCableIndicesPerCable50FD;
    for (U i = 0; i < vertexCount; ++i) {
        const auto& vertex = mesh.vertices[i];
        for (float value : vertex.position)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::frame, "cable vertex contains nonfinite world position");
        for (float value : vertex.uv)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::frame, "cable vertex contains nonfinite source UV");
    }
    for (U i = 0; i < indexCount; ++i)
        if (mesh.indices[i] != cableIndex50DS(i))
            return fail(d, VehicleWindowError50DS::indices, "cable topology differs from canonical source strips");
    return true;
}

} // namespace vehicle_window_gl50ds_detail

} // namespace rr::platform
