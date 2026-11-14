#include "vehicle_window_gl50ds_storage.h"
#include "../../../tests/native/ra_static_fire_visual50la.h"
#include "../../../tests/native/rc_crystal50rc.h"
#include "../../../tests/native/tb_original_world50lm.h"

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
    return (componentCount == 3 || componentCount == 5) &&
        component >= 1 && component <= sourceBodyCount50LA && component < componentCount;
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
    if (material.sourceDiffuseCompressed50LA &&
        (material.mode != VehicleWindowMaterialMode50DS::requiredTexture ||
         material.blend != VehicleWindowBlend50DS::opaque || material.unlit ||
         material.sourcePrelit || material.fixedBlendAlpha || material.alphaScale != 1.f))
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
    bool anyPair = false;
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
                    "source compressed diffuse requires actual engine components in a three/five-model pod");
            if (binding.sourceSpecularPair50LA) {
                if (!sourceBodyComponent50LA(c, owner.componentCount()))
                    return fail(d, VehicleWindowError50DS::material,
                        "source specular pair requires actual engine components in a three/five-model pod");
                anyPair = true;
                const auto* map2 = input.specularTextures50LA ?
                    input.specularTextures50LA->asset(binding.specularTextureIndex50LA) : nullptr;
                if (!map2 || !map2->name)
                    return fail(d, VehicleWindowError50DS::material, "source pair has no actual published map 2");
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
    if (anyPair != bool(input.specularTextures50LA))
        return fail(d, VehicleWindowError50DS::material, "map-2 bank presence differs from source pair table");
    d.component = maxComponents;
    d.material = d.texture = assets::unresolvedMaterialTexture50EA;
    return true;
}

bool currentRaStaticFire(const VehicleWindowTrack50DS& track,VehicleWindowDiagnostic50DS& d) noexcept {
    if(!track.raStaticFire)return true;
    if(!track.textures||track.textures->raStaticFireOwner()!=track.raStaticFire.get())
        return fail(d,VehicleWindowError50DS::material,"FireP0t exact image publication lost");
    constexpr std::int32_t fields[]={1,0,255,255,0,0,1,0,1,128,0,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,1,6,0,0,0,1,2,0};
    constexpr std::int32_t sampler[]={0,0,1,5,0,-75};
    for(std::size_t i=0;i<4;++i){
        assets::OriginalStandardMaterialMapBindingView50LA map;
        assets::OriginalStandardMaterialDiagnostic50LA md;
        if(!track.raStaticFire->currentMap(i,map,md))
            return fail(d,VehicleWindowError50DS::material,"FireP0t canonical map/collector expired");
        if(map.contextAfter249610.fields.size()!=sizeof(fields)/sizeof(fields[0]))
            return fail(d,VehicleWindowError50DS::material,"FireP0t unsupported canonical field extent");
        for(std::size_t j=0;j<map.contextAfter249610.fields.size();++j)
            if(map.contextAfter249610.fields[j].value!=fields[j])
                return fail(d,VehicleWindowError50DS::material,"FireP0t unsupported canonical raster state");
        for(std::size_t j=0;j<6;++j)if(map.contextAfter249610.sampler[j].value!=sampler[j])
            return fail(d,VehicleWindowError50DS::material,"FireP0t canonical sampler changed");
    }
    return true;
}

bool validateRaStaticFire(const VehicleWindowTrack50DS& track,VehicleWindowDiagnostic50DS& d) noexcept {
    if(!track.raStaticFire){
        if(track.textures&&track.textures->raStaticFireOwner())
            return fail(d,VehicleWindowError50DS::material,"FireP0t image bank requires its typed track publication");
        return true;
    }
    if(track.mesh!=track.raStaticFire->mesh()||!track.mesh||
       track.mode!=VehicleWindowMode50DS::callerMaterialTextures||track.vertexColorScale!=1.f||
       track.spanCount!=4||!track.spans||track.sourceNormals||track.sourceNormalCount||
       track.sourceProjectionUvs||track.sourceProjectionCount)
        return fail(d,VehicleWindowError50DS::material,"FireP0t requires exact source mesh/four spans/authored RGB and UV");
    if(!currentRaStaticFire(track,d))return false;
    for(std::size_t i=0;i<4;++i){
        const auto* source=track.raStaticFire->span(i);const auto& span=track.spans[i];const auto& m=span.material;
        if(!source||source->vertexStart!=span.vertexStart||source->vertexCount!=span.vertexCount||
           source->indexStart!=span.indexStart||source->indexCount!=span.indexCount||
           m.mode!=VehicleWindowMaterialMode50DS::requiredTexture||m.textureIndex!=0||
           m.blend!=VehicleWindowBlend50DS::opaque||!m.depthWrite||!m.depthEqual||m.alphaScale!=1.f||
           m.fixedBlendAlpha||m.unlit||!m.sourcePrelit||m.sourceDiffuseCompressed50LA||m.sourceSpecularPair50LA||
           span.sourcePass!=1||span.sourcePassCount!=1||span.projectionPass||span.discardZeroAlpha||
           !span.sourceAlphaTest||span.sourceAlphaReference||span.sourceUvAnimation||
           !span.sourceFogKnown||!span.sourceFogEnabled)
            return fail(d,VehicleWindowError50DS::material,"FireP0t source span or exact canonical lowering differs");
    }
    return true;
}

bool validateTbWorld(const VehicleWindowRequest50DS& request,VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& track=request.track;if(!track.tbWorld)return true;
    const auto& owner=*track.tbWorld;const auto source=owner.sourceOwner();
    const auto* spans=owner.spans();const auto* selections=owner.selections();
    const auto* normals=owner.normals();const auto* projected=owner.projectionUvs();const auto* bank=owner.bank();
    if(!owner.loaded()||!source||!source->ready()||request.raStaticWorld||track.rcCrystal||
       track.raStaticFire||track.rcParticleWindow||track.gbParticleWindow||track.mcParticleWindow||track.mode!=VehicleWindowMode50DS::callerMaterialTextures||
       track.mesh!=owner.mesh()||!track.mesh||track.mesh->vertices.size()!=151225u||
       track.mesh->indices.size()!=286893u||!spans||spans->size()!=1376u||!selections||
       !normals||normals->size()!=track.mesh->vertices.size()||track.sourceNormals!=normals->data()||
       track.sourceNormalCount!=normals->size()||!projected||projected->size()!=track.mesh->vertices.size()||
       track.sourceProjectionUvs!=projected->data()||track.sourceProjectionCount!=projected->size()||
       !track.spans||track.spanCount!=spans->size()||track.vertexColorScale!=255.f/128.f||!bank||!track.textures||
       !track.textures->matchesSourceBank(*bank,track.textures->publicationToken())||track.textures->textureCount()!=103u)
        return fail(d,VehicleWindowError50DS::request,"TB retained world/source publication differs");
    std::size_t crowd=0;
    for(std::size_t i=0;i<spans->size();++i){const auto& a=(*spans)[i];const auto& b=track.spans[i];
        if(a.materialSelection>=selections->size()||a.vertexStart!=b.vertexStart||a.vertexCount!=b.vertexCount||
           a.indexStart!=b.indexStart||a.indexCount!=b.indexCount||a.textureIndex!=b.material.textureIndex||
           a.sourcePass!=b.sourcePass||a.sourcePassCount!=b.sourcePassCount||a.projectionPass!=b.projectionPass||
           b.gsDestinationAlphaPass||b.sourceEnvironmentMap||b.material.mode!=VehicleWindowMaterialMode50DS::requiredTexture)
            return fail(d,VehicleWindowError50DS::material,"TB ordered source span association differs");
        if(pod_materials50ed_fixture::detail::normalized((*selections)[a.materialSelection].materialName)=="CROWD"){
            if(b.nativeCoverage!=NativeTrackCoverage50DS::fourSampleAa1)
                return fail(d,VehicleWindowError50DS::material,"TB original CROWD AA1 lowering missing");
            ++crowd;
        }
    }
    if(crowd!=33u)return fail(d,VehicleWindowError50DS::material,"TB original CROWD span count differs");
    for(U i=0;i<103u;++i){const auto* cpu=bank->texture(i);const auto* gpu=track.textures->asset(i);
        if(!cpu||!gpu||!gpu->name||cpu->identity!=gpu->identity)
            return fail(d,VehicleWindowError50DS::material,"TB source image identity differs");
    }
    return true;
}

bool validateTrack(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    if(!validateRaStaticFire(request.track,d)||!validateTbWorld(request,d))return false;
    const auto& track = request.track;
    const auto& fog = track.worldFog;
    if(track.rcCrystal){
        const std::uint64_t gpu=track.mesh?std::uint64_t(track.mesh->vertices.size())*sizeof(std::array<float,2>):0;
        if(!track.rcCrystal->validate(track)||!gpu||gpu>track.maximumRcProjectionBytes||
           gpu>std::uint64_t(std::numeric_limits<std::intptr_t>::max())||
           rc_crystal50RC::Owner::uvCount*sizeof(std::array<float,2>)>track.maximumRcStagingBytes)
            return fail(d,VehicleWindowError50DS::request,"RC crystal source association/projection budget rejected");
    }else if(track.maximumRcProjectionBytes||track.maximumRcStagingBytes)
        return fail(d,VehicleWindowError50DS::request,"RC projection budgets require typed source owner");
    if (fog.enabled) {
        if (!track.mesh || track.mode != VehicleWindowMode50DS::callerMaterialTextures ||
            !std::isfinite(fog.start) || !std::isfinite(fog.end) ||
            fog.start < 0.f || fog.end <= fog.start)
            return fail(d, VehicleWindowError50DS::request, "world fog requires textured world and finite ordered range");
        for (float value : fog.color)
            if (!std::isfinite(value) || value < 0.f || value > 1.f)
                return fail(d, VehicleWindowError50DS::request, "world fog color outside finite [0,1]");
    }
    if (request.enableNativePodShadows && (!track.mesh ||
        (track.mode != VehicleWindowMode50DS::sourceVertexColors &&
         track.mode != VehicleWindowMode50DS::callerMaterialTextures)))
        return fail(d, VehicleWindowError50DS::request, "native pod shadows require actual source-color/textured track");
    if (!track.mesh)
        return (!track.maximumVertexBytes && !track.maximumIndexBytes &&
            track.mode == VehicleWindowMode50DS::unspecified && track.vertexColorScale == 1.f &&
            !track.textures && !track.spans && !track.spanCount && !track.enableFrustumCulling &&
            !track.cullingChunkTriangles && !track.sourceNormals && !track.sourceNormalCount &&
            !track.maximumNormalBytes && !track.sourceProjectionUvs && !track.sourceProjectionCount &&
            !track.maximumProjectionBytes && !track.rcCrystal && !track.tbWorld && !track.maximumRcProjectionBytes && !track.maximumRcStagingBytes && !track.rcParticleWindow && !track.gbParticleWindow && !track.mcParticleWindow) ||
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
        if(track.textures&&track.textures->raStaticWorldOwner())
            return fail(d,VehicleWindowError50DS::material,
                "full RA image publication requires pending typed world renderer/light join");
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
            // Stand-in: the retained RC owner is the only environment-map UV producer.
            if(span.sourceEnvironmentMap&&(!track.rcCrystal||span.gsDestinationAlphaPass!=2))
                return fail(d,VehicleWindowError50DS::material,"environment-map pass lacks its projection owner");
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
                span.sourceUvPolicy != NativeTrackUvPolicy50DS::scLavaTwoPass)
                return fail(d,VehicleWindowError50DS::material,"unknown native track UV policy");
            const bool scLava=span.sourceUvPolicy==NativeTrackUvPolicy50DS::scLavaTwoPass;
            if ((span.discardZeroAlpha && span.sourceAlphaTest) ||
                (!scLava && span.sourceUvAnimation && (span.sourcePass != 1 || span.sourcePassCount != 1 ||
                    span.projectionPass || span.material.mode != VehicleWindowMaterialMode50DS::requiredTexture)))
                return fail(d,VehicleWindowError50DS::material,
                    "source UV service requires one textured unprojected pass and one alpha-test mode");
            if(scLava) {
                // Both policies are checked before any GL allocation/publication.
                const U first=span.sourcePass==1?i:i-1;
                if(first+1>=track.spanCount ||
                    !validScLavaUvPair50DS(track.spans[first],track.spans[first+1]))
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
    if (request.trackAnimationCount > maxVehicleWindowTrackAnimations50DS ||
        !request.trackAnimations || !request.trackAnimationTextures ||
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
        if(span.sourceUvPolicy==NativeTrackUvPolicy50DS::scLavaTwoPass)
            return fail(d,VehicleWindowError50DS::material,"SC lava retains its static original image; ATM override rejected");
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
    selected.enableNativePodShadows = false;
    const auto& world = selected.track;
    if (world.mesh && (world.mode != VehicleWindowMode50DS::callerMaterialTextures ||
        world.mesh->vertices.size() > maxVehicleWindowDynamicVertices50DS ||
        world.mesh->indices.size() > maxVehicleWindowDynamicIndices50DS ||
        world.spanCount > maxVehicleWindowDynamicSpans50DS || world.enableFrustumCulling ||
        world.cullingChunkTriangles || world.sourceProjectionUvs ||
        world.sourceProjectionCount || world.maximumProjectionBytes || world.rcCrystal || world.tbWorld ||
        world.maximumRcProjectionBytes || world.maximumRcStagingBytes || world.rcParticleWindow || world.gbParticleWindow || world.mcParticleWindow))
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
            !sky.spans && !sky.spanCount) ||
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
