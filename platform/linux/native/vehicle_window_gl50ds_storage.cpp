#include "vehicle_window_gl50ds_storage.h"
#include <cstdlib>
#include "../../../tests/native/ra_static_fire_visual50la.h"
#include "../../../tests/native/rc_particle_visual50rc.h"
#include "../../../tests/native/gb_particle_visual50gb.h"
#include "../../../tests/native/mc_particle_visual50mc.h"

namespace rr::platform {

bool VehicleWindowGl50DS::raSkySourceCurrent129280(const assets::Mesh48B& mesh,
    const assets::MaterialTextureAssets50EA& bank,const void* actualDisplay,
    VehicleWindowDiagnostic50DS& d)const noexcept {
    d={};
    if(!actualDisplay||sessionDisplayOwner50LA_!=actualDisplay||!storage_||
       !storage_->initialized||storage_->frontendOnly)
        return fail(d,VehicleWindowError50DS::notInitialized,"RA sky requires actual initialized gameplay Display");
    const auto& s=*storage_;
    if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"RA sky source inspection requires idle original renderer");
    // Identity comparison only. Never read through the saved CPU address.
    if(s.nativeSkyMeshIdentity129280!=&mesh||!s.skyVertexBuffer||!s.skyIndexBuffer||
       !s.skyVertexArray||s.skyIndexCount!=288||s.skySpanCount!=48u||!s.skySpans||
       !bank.loaded()||bank.textureCount()!=48u||bank.slotCount()!=48u||
       mesh.vertices.size()!=192u||mesh.indices.size()!=288u)
        return fail(d,VehicleWindowError50DS::assets,"RA sky differs from actual cold geometry publication");
    const auto* gpu=s.request.sky.textures;
    if(!gpu||!s.skyTexturePublication||gpu->textureCount()!=48u||
       !gpu->matchesSourceBank(bank,s.skyTexturePublication)||
       !gpu->validBorrow(s.request.context,s.request.isCurrent,s.skyTexturePublication))
        return fail(d,VehicleWindowError50DS::material,"RA sky requires same original bank/GPU/context publication");
    for(std::uint32_t i=0;i<48u;++i){const auto& span=s.skySpans[i];
        const auto& material=span.material;const auto* image=bank.texture(i);
        if(!image||image->identity!=i||span.indexStart!=6u*i||span.indexCount!=6u||
           material.textureIndex!=i||material.policy.textureIndex!=i||
           material.policy.mode!=VehicleWindowMaterialMode50DS::requiredTexture||
           material.policy.depthWrite||!material.policy.unlit||
           !material.name||material.name!=gpu->texture(i))
            return fail(d,VehicleWindowError50DS::material,"RA sky copied span/image differs from original upload");
    }
    return true;
}
void VehicleWindowGl50DS::Storage::retireRaStaticFirePublication() noexcept {
        if(raStaticFireOwner){
            raStaticFireOwner->retireRenderer(raStaticFireRenderer);
            raStaticFireOwner.reset();raStaticFireRenderer=nullptr;raStaticFirePublicationSerial=0;
        }
    }

bool VehicleWindowGl50DS::Storage::cancelRcParticleAttempt() noexcept {
        if(!rcParticleWindow||!rcParticlePendingAttempt)return true;
        if(rcParticleWindow->renderer_!=rcParticleRenderer||
           rcParticleWindow->lastAttempt_!=rcParticlePendingAttempt)return false;
        if(rcParticleWindow->attemptActive_&&
           !rcParticleWindow->failRenderAttempt(rcParticleRenderer,rcParticlePendingAttempt))return false;
        rcParticlePendingAttempt=0;return true;
    }

bool VehicleWindowGl50DS::Storage::beginRcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!rcParticleWindow)return true;
        if(!cancelRcParticleAttempt())return fail(d,VehicleWindowError50DS::frame,"RC prior particle draw attempt could not retire");
        if(rcParticleAttempt==std::numeric_limits<std::uint64_t>::max()){
            if(!rcParticleWindow->invalidateRenderPublication(rcParticleRenderer))d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::limits,"RC particle draw generation exhausted");
        }
        const auto attempt=++rcParticleAttempt;
        if(!rcParticleWindow->beginRenderAttempt(rcParticleRenderer,attempt))
            return fail(d,VehicleWindowError50DS::frame,"RC particle draw attempt owner rejected");
        rcParticlePendingAttempt=attempt;rcParticleCompletedBefore=trackVisibilityCompletedGeneration;return true;
    }

bool VehicleWindowGl50DS::Storage::completeRcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!rcParticleWindow||!rcParticlePendingAttempt)return true;
        if(active||frameSet||diagnostic||!current(d)||!texturesCurrent(d)||
           !trackVisibilityCompletedValid||trackVisibilityCompletedGeneration<=rcParticleCompletedBefore)
            return fail(d,VehicleWindowError50DS::frame,"RC particle completion requires the same idle current completed draw");
        std::array<bool,4> submitted{};
        for(std::size_t i=0;i<submitted.size();++i){
            if(rcParticleWindow->spans_[i].empty())return fail(d,VehicleWindowError50DS::frame,"RC source instance has no authenticated spans");
            submitted[i]=true;
            for(const auto span:rcParticleWindow->spans_[i]){
                if(span>=trackSpanCount)return fail(d,VehicleWindowError50DS::frame,"RC source instance span exceeds retained renderer table");
                submitted[i]=submitted[i]&&trackSpans[span].submittedCompletedGameplay;
            }
        }
        if(!rcParticleWindow->recordCompleted(rcParticleRenderer,rcParticlePendingAttempt,
            trackVisibilityCompletedGeneration,submitted))
            return fail(d,VehicleWindowError50DS::frame,"RC actual particle instance completion publication rejected");
        rcParticlePendingAttempt=0;return true;
    }

void VehicleWindowGl50DS::Storage::retireRcParticlePublication() noexcept {
        if(rcParticleWindow){
            if(!cancelRcParticleAttempt())std::terminate();
            rcParticleWindow->retireRenderer(rcParticleRenderer);
            rcParticleWindow.reset();rcParticleRenderer=nullptr;
        }
    }

bool VehicleWindowGl50DS::Storage::cancelGbParticleAttempt() noexcept {
        if(!gbParticleWindow||!gbParticlePendingAttempt)return true;
        if(gbParticleWindow->renderer_!=gbParticleRenderer||
           gbParticleWindow->lastAttempt_!=gbParticlePendingAttempt)return false;
        if(gbParticleWindow->attemptActive_&&
           !gbParticleWindow->failRenderAttempt(gbParticleRenderer,gbParticlePendingAttempt))return false;
        gbParticlePendingAttempt=0;return true;
    }

bool VehicleWindowGl50DS::Storage::beginGbParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!gbParticleWindow)return true;
        if(!cancelGbParticleAttempt())return fail(d,VehicleWindowError50DS::frame,"GB prior particle draw attempt could not retire");
        if(gbParticleAttempt==std::numeric_limits<std::uint64_t>::max()){
            if(!gbParticleWindow->invalidateRenderPublication(gbParticleRenderer))d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::limits,"GB particle draw generation exhausted");
        }
        const auto attempt=++gbParticleAttempt;
        if(!gbParticleWindow->beginRenderAttempt(gbParticleRenderer,attempt))
            return fail(d,VehicleWindowError50DS::frame,"GB particle draw attempt owner rejected");
        gbParticlePendingAttempt=attempt;gbParticleCompletedBefore=trackVisibilityCompletedGeneration;return true;
    }

bool VehicleWindowGl50DS::Storage::completeGbParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!gbParticleWindow||!gbParticlePendingAttempt)return true;
        if(active||frameSet||diagnostic||!current(d)||!texturesCurrent(d)||
           !trackVisibilityCompletedValid||trackVisibilityCompletedGeneration<=gbParticleCompletedBefore)
            return fail(d,VehicleWindowError50DS::frame,"GB particle completion requires the same idle current completed draw");
        std::array<bool,gb_particle_attachments50GB::placementCount> submitted{};
        for(std::size_t i=0;i<submitted.size();++i){
            const auto association=gbParticleWindow->representative_[i];
            const auto count=gbParticleWindow->attachments_->spanCount(association);
            const auto* spans=gbParticleWindow->attachments_->spanIndices(association);
            if(!count||!spans)return fail(d,VehicleWindowError50DS::frame,"GB source instance has no authenticated spans");
            submitted[i]=true;
            for(std::size_t n=0;n<count;++n){const auto span=spans[n];
                if(span>=trackSpanCount)return fail(d,VehicleWindowError50DS::frame,"GB source instance span exceeds retained renderer table");
                submitted[i]=submitted[i]&&trackSpans[span].submittedCompletedGameplay;
            }
        }
        if(!gbParticleWindow->recordCompleted(gbParticleRenderer,gbParticlePendingAttempt,
            trackVisibilityCompletedGeneration,submitted))
            return fail(d,VehicleWindowError50DS::frame,"GB actual particle instance completion publication rejected");
        gbParticlePendingAttempt=0;return true;
    }

void VehicleWindowGl50DS::Storage::retireGbParticlePublication() noexcept {
        if(gbParticleWindow){
            if(!cancelGbParticleAttempt())std::terminate();
            gbParticleWindow->retireRenderer(gbParticleRenderer);
            gbParticleWindow.reset();gbParticleRenderer=nullptr;
        }
    }

bool VehicleWindowGl50DS::Storage::cancelMcParticleAttempt() noexcept {
        if(!mcParticleWindow||!mcParticlePendingAttempt)return true;
        if(mcParticleWindow->renderer_!=mcParticleRenderer||
           mcParticleWindow->lastAttempt_!=mcParticlePendingAttempt)return false;
        if(mcParticleWindow->attemptActive_&&
           !mcParticleWindow->failRenderAttempt(mcParticleRenderer,mcParticlePendingAttempt))return false;
        mcParticlePendingAttempt=0;return true;
    }

bool VehicleWindowGl50DS::Storage::beginMcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!mcParticleWindow)return true;
        if(!cancelMcParticleAttempt())return fail(d,VehicleWindowError50DS::frame,"MC prior particle draw attempt could not retire");
        if(mcParticleAttempt==std::numeric_limits<std::uint64_t>::max()){
            if(!mcParticleWindow->invalidateRenderPublication(mcParticleRenderer))d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::limits,"MC particle draw generation exhausted");
        }
        const auto attempt=++mcParticleAttempt;
        if(!mcParticleWindow->beginRenderAttempt(mcParticleRenderer,attempt))
            return fail(d,VehicleWindowError50DS::frame,"MC particle draw attempt owner rejected");
        mcParticlePendingAttempt=attempt;mcParticleCompletedBefore=trackVisibilityCompletedGeneration;return true;
    }

bool VehicleWindowGl50DS::Storage::completeMcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!mcParticleWindow||!mcParticlePendingAttempt)return true;
        if(active||frameSet||diagnostic||!current(d)||!texturesCurrent(d)||
           !trackVisibilityCompletedValid||trackVisibilityCompletedGeneration<=mcParticleCompletedBefore)
            return fail(d,VehicleWindowError50DS::frame,"MC particle completion requires the same idle current completed draw");
        std::array<bool,mc_particle_attachments50MC::placementCount> submitted{};
        for(std::size_t i=0;i<submitted.size();++i){
            const auto association=mcParticleWindow->representative_[i];
            const auto count=mcParticleWindow->attachments_->spanCount(association);
            const auto* spans=mcParticleWindow->attachments_->spanIndices(association);
            if(!count||!spans)return fail(d,VehicleWindowError50DS::frame,"MC source instance has no authenticated spans");
            submitted[i]=true;
            for(std::size_t n=0;n<count;++n){const auto span=spans[n];
                if(span>=trackSpanCount)return fail(d,VehicleWindowError50DS::frame,"MC source instance span exceeds retained renderer table");
                submitted[i]=submitted[i]&&trackSpans[span].submittedCompletedGameplay;
            }
        }
        if(!mcParticleWindow->recordCompleted(mcParticleRenderer,mcParticlePendingAttempt,
            trackVisibilityCompletedGeneration,submitted))
            return fail(d,VehicleWindowError50DS::frame,"MC actual particle instance completion publication rejected");
        mcParticlePendingAttempt=0;return true;
    }

void VehicleWindowGl50DS::Storage::retireMcParticlePublication() noexcept {
        if(mcParticleWindow){
            if(!cancelMcParticleAttempt())std::terminate();
            mcParticleWindow->retireRenderer(mcParticleRenderer);
            mcParticleWindow.reset();mcParticleRenderer=nullptr;
        }
    }

bool VehicleWindowGl50DS::Storage::podMaterialUsesNormals(const VehicleWindowMaterialBinding50DS& policy) noexcept {
        return !policy.unlit && !policy.sourcePrelit && !policy.sourceDiffuseCompressed50LA &&
            policy.blend == VehicleWindowBlend50DS::opaque;
    }

VehicleWindowGl50DS::Storage::RacerView VehicleWindowGl50DS::Storage::racerView(U archetype) const noexcept {
        if (!archetype) return {meshes, scenes, &resources, racerNormalBuffers, materials.get(),
            materialOffsets, materialSummary, materialCount, cableTextureName, sourceSpecularGeometry50LA.data(),
            playerSkin50LA};
        const auto& source = racerArchetypes[archetype - 1];
        return {source.meshes, source.scenes, &source.resources, source.normalBuffers, source.materials.get(),
            source.materialOffsets, source.materialSummary, source.materialCount, source.cableTextureName,
            source.specularGeometry50LA.data(), source.skin50LA};
    }

bool VehicleWindowGl50DS::Storage::current(VehicleWindowDiagnostic50DS& d,
        const OriginalVideoBlockingNativeBackend* fadeOwner,
        const OriginalUiSharedDialogs50KG* menuOwner,
        const OriginalOptionsMutationInterlock50KG* optionsOwner,
        const NativeVideoInvocation50LA* invocation) const noexcept {
        if (thread != std::this_thread::get_id())
            return fail(d, VehicleWindowError50DS::wrongThread, "renderer requires creating thread");
        // Exclude ALL unrelated renderer context work, including Garage fence
        // allocation. Otherwise a failed foreign fence could block invocation
        // cancellation while its dependent pin simultaneously blocks close.
        if(nativeVideoInvocation50LA!=invocation)
            return fail(d,VehicleWindowError50DS::alreadyOwned,"native Video invocation owns renderer command custody");
        if(!OriginalOptionsMutationInterlock50KG::permitted(optionsOwner))
            return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes renderer mutation");
        // Every ordinary context-consuming renderer entry is excluded while
        // the exact menu raster contract is pending. Poll is explicitly exempt
        // because it never writes target, viewport or canvas. Only the sealed
        // same-owner source backend and actual retirement may otherwise enter.
        if(menuRasterShared50KG&&!menuRasterBackendActive50KG&&menuOwner!=menuRasterShared50KG)
            return fail(d,VehicleWindowError50DS::frame,"pending native menu raster excludes unrelated renderer work");
        if(displayFadeOwner && displayFadeOwner!=fadeOwner)
            return fail(d,VehicleWindowError50DS::frame,"blocking display fade owns renderer until retirement");
        return (request.isCurrent && request.isCurrent(request.context)&&
            nativeVideoInvocation50LA==invocation&&OriginalOptionsMutationInterlock50KG::permitted(optionsOwner)) ||
            fail(d, VehicleWindowError50DS::notCurrent, "caller context/host surface is not current or mutation owner changed");
    }

bool VehicleWindowGl50DS::Storage::texturesCurrent(VehicleWindowDiagnostic50DS& d) const noexcept {
        if(!currentRaStaticFire(request.track,d))return false;
        if(rcParticleWindow&&(!rcParticleWindow->ready()||
           !rcParticleWindow->gpu_.validBorrow(request.context,request.isCurrent,rcParticlePublication)||
           !rcParticleWindow->gpu_.matchesSourceBank(rcParticleWindow->bank_,rcParticlePublication)))
            return fail(d,VehicleWindowError50DS::material,"RC particle actual atlas publication/context changed");
        if(gbParticleWindow&&(!gbParticleWindow->ready()||
           !gbParticleWindow->gpu_.validBorrow(request.context,request.isCurrent,gbParticlePublication)||
           !gbParticleWindow->gpu_.matchesSourceBank(gbParticleWindow->bank_,gbParticlePublication)))
            return fail(d,VehicleWindowError50DS::material,"GB particle actual atlas publication/context changed");
        if(mcParticleWindow&&(!mcParticleWindow->ready()||
           !mcParticleWindow->gpu_.validBorrow(request.context,request.isCurrent,mcParticlePublication)||
           !mcParticleWindow->gpu_.matchesSourceBank(mcParticleWindow->bank_,mcParticlePublication)))
            return fail(d,VehicleWindowError50DS::material,"MC particle actual atlas publication/context changed");
        if(request.gcParticleTextures&&!request.gcParticleTextures->validBorrow(
            request.context,request.isCurrent,gcParticlePublication))
            return fail(d,VehicleWindowError50DS::material,"GC particle atlas publication/context changed");
        if(request.gameplayBinderTextures&&!request.gameplayBinderTextures->validBorrow(
            request.context,request.isCurrent,gameplayBinderPublication))
            return fail(d,VehicleWindowError50DS::material,"Binder immutable texture publication expired");
        if (request.mode == VehicleWindowMode50DS::callerMaterialTextures &&
            (!request.materials.textures || !request.materials.textures->validBorrow(
                request.context, request.isCurrent, texturePublication)))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed vehicle texture publication/context/thread is no longer valid");
        if (request.materials.specularTextures50LA && !request.materials.specularTextures50LA->validBorrow(
            request.context, request.isCurrent, specularPublication50LA))
            return fail(d, VehicleWindowError50DS::material, "player map-2 publication/context/thread changed");
        for (U m = 0; m < materialCount; ++m) {
            const auto& material = materials[m];
            if (!material.policy.sourceSpecularPair50LA) continue;
            const auto* base = request.materials.textures->asset(material.textureIndex);
            const auto* map2 = request.materials.specularTextures50LA->asset(material.policy.specularTextureIndex50LA);
            if (!base || base->name != material.name || base->identity != material.textureIdentity50LA ||
                !map2 || map2->name != material.specularName50LA || map2->identity != material.specularIdentity50LA)
                return fail(d, VehicleWindowError50DS::material, "player source pair texture identity changed");
        }
        for (U m = 0; m < materialCount; ++m) {
            const auto& material = materials[m];
            if (material.policy.extraPass50PP == VehicleWindowExtraPass50PP::none) continue;
            const auto* extra = request.materials.specularTextures50LA ?
                request.materials.specularTextures50LA->asset(material.policy.extraPassTextureIndex50PP) : nullptr;
            if (!extra || extra->name != material.extraPassName50PP || extra->identity != material.extraPassIdentity50PP)
                return fail(d, VehicleWindowError50DS::material, "player second-pass texture identity changed");
        }
        for (U i = 0; i < racerArchetypeCount; ++i) {
            const auto& archetype = racerArchetypes[i];
            if (request.mode == VehicleWindowMode50DS::callerMaterialTextures &&
                (!archetype.textures || !archetype.textures->validBorrow(
                    request.context, request.isCurrent, archetype.texturePublication)))
                return fail(d, VehicleWindowError50DS::material,
                    "borrowed racer texture publication/context/thread is no longer valid");
            if (archetype.specularTextures50LA && !archetype.specularTextures50LA->validBorrow(
                request.context, request.isCurrent, archetype.specularPublication50LA))
                return fail(d, VehicleWindowError50DS::material, "racer map-2 publication/context/thread changed");
            if (archetype.cables.textures && !archetype.cables.textures->validBorrow(
                request.context, request.isCurrent, archetype.cableTexturePublication))
                return fail(d, VehicleWindowError50DS::material,
                    "borrowed racer cable publication/context/thread is no longer valid");
            for (U m = 0; m < archetype.materialCount; ++m) {
                const auto& material = archetype.materials[m];
                if (!material.name) continue;
                const auto* texture = archetype.textures->asset(material.textureIndex);
                if (!texture || texture->name != material.name)
                    return fail(d, VehicleWindowError50DS::material,
                        "racer material texture is no longer the retained publication");
                if (material.policy.sourceSpecularPair50LA) {
                    const auto* map2 = archetype.specularTextures50LA->asset(material.policy.specularTextureIndex50LA);
                    if (texture->identity != material.textureIdentity50LA || !map2 ||
                        map2->name != material.specularName50LA || map2->identity != material.specularIdentity50LA)
                        return fail(d, VehicleWindowError50DS::material, "racer source pair texture identity changed");
                }
                if (material.policy.extraPass50PP != VehicleWindowExtraPass50PP::none) {
                    const auto* extra = archetype.specularTextures50LA ?
                        archetype.specularTextures50LA->asset(material.policy.extraPassTextureIndex50PP) : nullptr;
                    if (!extra || extra->name != material.extraPassName50PP || extra->identity != material.extraPassIdentity50PP)
                        return fail(d, VehicleWindowError50DS::material, "racer second-pass texture identity changed");
                }
            }
            if (archetype.cables.textures) {
                const auto* texture = archetype.cables.textures->asset(archetype.cables.textureIndex);
                if (!texture || texture->name != archetype.cableTextureName)
                    return fail(d, VehicleWindowError50DS::material,
                        "racer cable texture is no longer the retained publication");
            }
        }
        if(!raWorldCurrent(d))return false;
        if (request.track.mode == VehicleWindowMode50DS::callerMaterialTextures &&
            (!request.track.textures || !request.track.textures->validBorrow(
                request.context, request.isCurrent, trackTexturePublication)))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed track texture publication/context/thread is no longer valid");
        if (request.trackAnimationTextures && !request.trackAnimationTextures->validBorrow(
            request.context, request.isCurrent, trackAnimationTexturePublication))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed animation texture publication/context/thread is no longer valid");
        if (request.dynamicWorld.textures && !request.dynamicWorld.textures->validBorrow(
            request.context, request.isCurrent, dynamicWorldTexturePublication))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed dynamic-world texture publication/context/thread is no longer valid");
        if (request.sky.textures && !request.sky.textures->validBorrow(
            request.context, request.isCurrent, skyTexturePublication))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed sky texture publication/context/thread is no longer valid");
        if (request.cables.textures && !request.cables.textures->validBorrow(
            request.context, request.isCurrent, cableTexturePublication))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed cable texture publication/context/thread is no longer valid");
        return true;
    }

bool VehicleWindowGl50DS::Storage::usesTextures() const noexcept {
        if(hasNativeResultsResources())return true;
        if(request.raStaticWorld)return true;
        for (U i = 0; i < racerArchetypeCount; ++i)
            if (racerArchetypes[i].cables.textures) return true;
        return request.mode == VehicleWindowMode50DS::callerMaterialTextures ||
            request.track.mode == VehicleWindowMode50DS::callerMaterialTextures || request.cables.textures ||
            request.sky.textures || request.dynamicWorld.textures || request.enableOriginalUi ||
            request.enableOriginalGameplayBlur || request.gcParticleTextures || request.track.rcParticleWindow || request.track.gbParticleWindow || request.track.mcParticleWindow || (hasRaceHudResources() || hasRaceStartOverlayResources() || hasTimeStatsResources() || hasPositionStatsResources() || hasDamageResources() || hasPauseDerivedResources() || hasSpeedBoostResources());
    }

bool VehicleWindowGl50DS::Storage::usesTrackShader() const noexcept {
        return request.track.mode == VehicleWindowMode50DS::sourceVertexColors ||
            request.track.mode == VehicleWindowMode50DS::callerMaterialTextures ||
            request.dynamicWorld.mode == VehicleWindowMode50DS::callerMaterialTextures ||
            request.gameplayBinderTextures != nullptr;
    }

bool VehicleWindowGl50DS::Storage::createTrackMaterials(const VehicleWindowTrack50DS& input) noexcept {
        if (input.mode != VehicleWindowMode50DS::callerMaterialTextures) return true;
        trackSpans.reset(new (std::nothrow) TrackSpan[input.spanCount]);
        if (!trackSpans)
            return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded source-ordered track span table");
        trackSpanCount = input.spanCount;
        for (U i = 0; i < trackSpanCount; ++i) {
            const auto& source = input.spans[i];
            auto& target = trackSpans[i];
            target.indexStart = source.indexStart;
            target.indexCount = source.indexCount;
            target.discardZeroAlpha = source.discardZeroAlpha;
            target.nativeCoverage = source.nativeCoverage;
            // The exact destination-alpha bridge (fallback and comparison route)
            // composes per sample, so any GS C=Ad pair also needs these targets.
            coverageDemand = coverageDemand || source.nativeCoverage == NativeTrackCoverage50DS::fourSampleAa1 ||
                source.gsDestinationAlphaPass != 0;
            target.sourceAlphaTest = source.sourceAlphaTest;
            target.sourceAlphaReference = source.sourceAlphaReference;
            target.sourceUvAnimation = source.sourceUvAnimation;
            target.allowPodShadow = source.allowPodShadow;
            target.worldInstance = source.worldInstance;
            target.sourcePass = source.sourcePass;
            target.sourcePassCount = source.sourcePassCount;
            target.projectionPass = source.projectionPass;
            target.gsDestinationAlphaPass = source.gsDestinationAlphaPass;
            target.sourceEnvironmentMap = source.sourceEnvironmentMap;
            target.sourceZone = source.sourceZone;
            if (source.gsDestinationAlphaPass == 2 && !source.sourceEnvironmentMap) {
                // Cold bound for the fixed route's pass-2 admission: largest
                // RGB vertex byte this span can feed into texel x byte / 128.
                if (!gsSpanMaxVertexByte) {
                    gsSpanMaxVertexByte.reset(new (std::nothrow) std::uint8_t[input.spanCount]());
                    if (!gsSpanMaxVertexByte)
                        return fail(*diagnostic, VehicleWindowError50DS::allocation,
                            "allocate destination-alpha vertex byte bounds");
                }
                std::uint8_t maximum = 0;
                if (!input.mesh || source.vertexStart > input.mesh->vertices.size() ||
                    input.mesh->vertices.size() - source.vertexStart < source.vertexCount) maximum = 255;
                else for (U v = source.vertexStart; v < source.vertexStart + source.vertexCount; ++v)
                    for (U c = 0; c < 3; ++c) maximum = std::max(maximum, input.mesh->vertices[v].color[c]);
                gsSpanMaxVertexByte[i] = maximum;
            }
            // All source ranges/pass continuity were validated before creation.
            // For alpha-over pairs, fogging both colors with the same factor
            // yields fog(alpha*layer + (1-alpha)*base), without fogging alpha.
            target.worldFogEligible = source.sourceFogKnown ? source.sourceFogEnabled :
                (source.sourcePassCount == 1 && source.material.blend == VehicleWindowBlend50DS::opaque && source.material.depthWrite);
            if (source.sourcePassCount == 2 && !source.sourceFogKnown) {
                const U first = i - (source.sourcePass - 1);
                const auto& base = input.spans[first];
                const auto& layer = input.spans[first + 1];
                target.worldFogEligible = base.projectionPass == 1 && layer.projectionPass == 2 &&
                    base.sourcePass == 1 && layer.sourcePass == 2 &&
                    base.material.mode == VehicleWindowMaterialMode50DS::requiredTexture &&
                    layer.material.mode == VehicleWindowMaterialMode50DS::requiredTexture &&
                    base.material.blend == VehicleWindowBlend50DS::opaque && base.material.depthWrite &&
                    layer.material.blend == VehicleWindowBlend50DS::sourceAlpha &&
                    !base.material.fixedBlendAlpha && !layer.material.fixedBlendAlpha &&
                    base.material.unlit && layer.material.unlit &&
                    !base.discardZeroAlpha && !layer.discardZeroAlpha;
            }
            target.material.textureIndex = source.material.textureIndex;
            target.material.policy = source.material;
            target.material.name = source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                input.textures->texture(source.material.textureIndex) : 0;
            if (source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture &&
                !target.material.name) {
                diagnostic->trackSpan = i;
                diagnostic->texture = source.material.textureIndex;
                return fail(*diagnostic, VehicleWindowError50DS::material,
                    "required track texture disappeared during cold preparation");
            }
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::createTrackAnimations(const VehicleWindowRequest50DS& input) noexcept {
        if (!input.trackAnimationCount) return true;
        trackAnimations.reset(new (std::nothrow) TrackAnimation[input.trackAnimationCount]);
        if (!trackAnimations) return fail(*diagnostic, VehicleWindowError50DS::allocation,
            "allocate bounded cold original track-animation table");
        trackAnimationCount = input.trackAnimationCount;
        for (U i = 0; i < trackAnimationCount; ++i) {
            const auto& source = input.trackAnimations[i]; auto& target = trackAnimations[i];
            target.sourceTextureIndex = source.sourceTextureIndex;
            target.firstTextureIndex = source.firstTextureIndex; target.frameCount = source.frameCount;
            for (U f = 0; f < target.frameCount; ++f) {
                target.names[f] = input.trackAnimationTextures->texture(target.firstTextureIndex+f);
                if (!target.names[f]) return fail(*diagnostic, VehicleWindowError50DS::material,
                    "original animation frame disappeared during cold preparation");
            }
            trackSpans[source.sourceSpanIndex].animation = i;
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::createDynamicWorldMaterials(const VehicleWindowTrack50DS& input) noexcept {
        if (input.mode != VehicleWindowMode50DS::callerMaterialTextures) return true;
        dynamicWorldSpans.reset(new (std::nothrow) TrackSpan[input.spanCount]);
        if (!dynamicWorldSpans)
            return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded source-ordered dynamic-world span table");
        dynamicWorldSpanCount = input.spanCount;
        for (U i = 0; i < dynamicWorldSpanCount; ++i) {
            const auto& source = input.spans[i];
            auto& target = dynamicWorldSpans[i];
            target.indexStart = source.indexStart;
            target.indexCount = source.indexCount;
            target.discardZeroAlpha = source.discardZeroAlpha;
            target.sourceAlphaTest = source.sourceAlphaTest;
            target.sourceAlphaReference = source.sourceAlphaReference;
            target.sourceUvAnimation = source.sourceUvAnimation;
            target.allowPodShadow = source.allowPodShadow;
            target.sourcePass = source.sourcePass;
            target.sourcePassCount = source.sourcePassCount;
            target.projectionPass = source.projectionPass;
            target.material.textureIndex = source.material.textureIndex;
            target.material.policy = source.material;
            target.material.name = source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                input.textures->texture(source.material.textureIndex) : 0;
            if (source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture &&
                !target.material.name) {
                diagnostic->dynamicWorldSpan = i;
                diagnostic->texture = source.material.textureIndex;
                return fail(*diagnostic, VehicleWindowError50DS::material,
                    "required dynamic-world texture disappeared during cold preparation");
            }
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::createSkyMaterials(const VehicleWindowSky50DS& input) noexcept {
        if (!input.mesh) return true;
        skySpans.reset(new (std::nothrow) TrackSpan[input.spanCount]);
        if (!skySpans) return fail(*diagnostic, VehicleWindowError50DS::allocation,
            "allocate bounded authored sky span table");
        skySpanCount = input.spanCount;
        for (U i = 0; i < skySpanCount; ++i) {
            const auto& source = input.spans[i];
            auto& target = skySpans[i];
            target.indexStart = source.indexStart;
            target.indexCount = source.indexCount;
            target.material.textureIndex = source.material.textureIndex;
            target.material.policy = source.material;
            target.material.name = source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                input.textures->texture(source.material.textureIndex) : 0;
            if (source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture && !target.material.name)
                return fail(*diagnostic, VehicleWindowError50DS::material,
                    "required sky texture disappeared during cold preparation");
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::createTrackVisibility(const VehicleWindowTrack50DS& input) noexcept {
        if (!input.mesh) return true;
        trackDrawCount = trackSpanCount ? trackSpanCount : 1;
        trackCandidateIndexCount = trackSpanCount ? 0 : static_cast<U>(input.mesh->indices.size());
        for (U i = 0; i < trackSpanCount; ++i) trackCandidateIndexCount += trackSpans[i].indexCount;
        if (!input.enableFrustumCulling) return true;
        if (input.cullingChunkTriangles) {
            // Validation already bounded the total, with the original material
            // and complete source-span partition requirements left unchanged.
            for (U i = 0; i < trackSpanCount; ++i)
                trackChunkCount += trackSpanChunkCount(trackSpans[i].indexCount,
                    trackSpans[i].material.name != 0, input.cullingChunkTriangles);
            trackChunks.reset(new (std::nothrow) TrackChunk[trackChunkCount]);
            if (!trackChunks)
                return fail(*diagnostic, VehicleWindowError50DS::allocation,
                    "allocate bounded source-ordered track triangle chunks");
            U nextChunk = 0;
            for (U i = 0; i < trackSpanCount; ++i) {
                const auto& span = trackSpans[i];
                U start = span.indexStart, remaining = span.indexCount;
                while (remaining) {
                    // Bound before multiplication, so even UINT32_MAX as the
                    // requested triangle ceiling cannot overflow an index count.
                    const U count = span.material.name ?
                        std::min(remaining / 3, input.cullingChunkTriangles) * 3 : remaining;
                    trackChunks[nextChunk++] = {i, start, count};
                    start += count;
                    remaining -= count;
                }
            }
            trackDrawCount = trackChunkCount;
        }
        // The validated candidates have at most 65536 draws. Allocate the complete
        // possible retained list once; submit never grows or allocates storage.
        trackDrawBounds.reset(new (std::nothrow) game::StaticDrawBounds50EI[trackDrawCount]);
        trackVisibleDrawIds.reset(new (std::nothrow) U[trackDrawCount]);
        if (!trackDrawBounds || !trackVisibleDrawIds)
            return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded static track bounds and retained draw IDs");
        const auto& mesh = *input.mesh;
        for (U i = 0; i < trackDrawCount; ++i) {
            auto& draw = trackDrawBounds[i];
            draw.drawId = i;
            const U start = trackChunkCount ? trackChunks[i].indexStart :
                (trackSpanCount ? trackSpans[i].indexStart : 0);
            const U count = trackChunkCount ? trackChunks[i].indexCount :
                (trackSpanCount ? trackSpans[i].indexCount : static_cast<U>(mesh.indices.size()));
            // Index ranges are nonempty and validated before renderer ownership
            // or allocation. Ignore unreferenced vertices and untrusted mesh
            // display bounds; no root or model transform is applied a second time.
            const auto& first = mesh.vertices[mesh.indices[start]];
            for (U axis = 0; axis < 3; ++axis)
                draw.worldBounds.minimum[axis] = draw.worldBounds.maximum[axis] = first.position[axis];
            for (U n = start + 1; n < start + count; ++n) {
                const auto& vertex = mesh.vertices[mesh.indices[n]];
                for (U axis = 0; axis < 3; ++axis) {
                    draw.worldBounds.minimum[axis] = std::min(draw.worldBounds.minimum[axis], vertex.position[axis]);
                    draw.worldBounds.maximum[axis] = std::max(draw.worldBounds.maximum[axis], vertex.position[axis]);
                }
            }
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::createMaterials(const VehicleWindowMaterials50DS& input) noexcept {
        if (request.mode != VehicleWindowMode50DS::callerMaterialTextures) return true;
        U total = 0;
        for (U c = 0; c < resources.componentCount; ++c) {
            materialOffsets[c] = total;
            total += input.component[c].count; // Already bounded to 5 * 4096.
        }
        materialCount = total;
        materials.reset(new (std::nothrow) Material[total]);
        if (!materials)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate bounded cold material table");
        for (U c = 0; c < resources.componentCount; ++c) {
            for (U m = 0; m < input.component[c].count; ++m) {
                const auto& binding = input.component[c].bindings[m];
                auto& resolved = materials[materialOffsets[c] + m];
                resolved.textureIndex = binding.textureIndex;
                resolved.policy = binding;
                materialSummary[c].sourceDiffuse |= binding.sourceDiffuseCompressed50LA;
                materialSummary[c].sourcePair |= binding.sourceSpecularPair50LA;
                materialSummary[c].normalDependent |= podMaterialUsesNormals(binding);
                materialSummary[c].hasDeferredEffects |= binding.blend != VehicleWindowBlend50DS::opaque;
                resolved.name = binding.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                    input.textures->texture(binding.textureIndex) : 0;
                if (binding.sourceSpecularPair50LA) {
                    const auto* base = input.textures->asset(binding.textureIndex);
                    const auto* map2 = input.specularTextures50LA->asset(binding.specularTextureIndex50LA);
                    if (!base || !base->name || !map2 || !map2->name)
                        return fail(*diagnostic, VehicleWindowError50DS::material, "source pair disappeared during create");
                    resolved.textureIdentity50LA = base->identity;
                    resolved.specularName50LA = map2->name; resolved.specularIdentity50LA = map2->identity;
                    hasSpecularPairs50LA = true;
                }
                if (binding.extraPass50PP != VehicleWindowExtraPass50PP::none) {
                    const auto* extra = input.specularTextures50LA ?
                        input.specularTextures50LA->asset(binding.extraPassTextureIndex50PP) : nullptr;
                    if (!extra || !extra->name)
                        return fail(*diagnostic, VehicleWindowError50DS::material, "second-pass texture disappeared during create");
                    resolved.extraPassName50PP = extra->name; resolved.extraPassIdentity50PP = extra->identity;
                    // The additive pass draws with the deferred effects of its racer.
                    materialSummary[c].hasDeferredEffects = true;
                    hasExtraPasses50PP = true;
                }
                if (binding.mode == VehicleWindowMaterialMode50DS::requiredTexture && !resolved.name) {
                    diagnostic->component = c;
                    diagnostic->material = m;
                    diagnostic->texture = binding.textureIndex;
                    return fail(*diagnostic, VehicleWindowError50DS::material,
                        "required texture disappeared during cold material preparation");
                }
            }
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::hasDisplayFadeNames() const noexcept {
        return displayFadeSourceTarget || displayFadeSourceColor ||
            displayFadeTargets[0] || displayFadeTargets[1] || displayFadeColors[0] || displayFadeColors[1] ||
            displayFadeProgram || displayFadeVertexShader || displayFadeFragmentShader || displayFadeVertexArray;
    }

bool VehicleWindowGl50DS::Storage::loadDisplayFadeFunctions() noexcept {
        // Dormant on normal create/submit, independent of every UI opt-in.
        // All required pointers exist before the first name is allocated.
        return load(gl.activeTexture,"glActiveTexture") && load(gl.bindTexture,"glBindTexture") &&
            load(gl.genTextures,"glGenTextures") && load(gl.deleteTextures,"glDeleteTextures") &&
            load(gl.texImage2D,"glTexImage2D") && load(gl.texParameteri,"glTexParameteri") &&
            load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv") &&
            load(gl.framebufferTexture2D,"glFramebufferTexture2D") &&
            load(gl.blitFramebuffer,"glBlitFramebuffer") && load(gl.drawArrays,"glDrawArrays");
    }

bool VehicleWindowGl50DS::Storage::createDisplayFadeTarget(U& target,U& color,I w,I h) noexcept {
        gl.genFramebuffers(1,&target);
        if(!check("allocate display-fade framebuffer") || !target)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero display-fade framebuffer");
        gl.genTextures(1,&color);
        if(!check("allocate display-fade texture") || !color)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero display-fade texture");
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,color);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,rgba8,w,h,0,rgba,unsignedByte,nullptr);
        if(!check("allocate actual-size display-fade RGBA8 storage"))return false;
        I actualW=0,actualH=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&actualW);
        gl.getTexLevelParameteriv(texture2D,0,0x1001,&actualH);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if(!check("verify display-fade texture storage"))return false;
        if(actualW!=w || actualH!=h || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"display-fade texture storage mismatch");
        gl.bindFramebuffer(framebuffer,target);
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,color,0);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("verify display-fade framebuffer"))return false;
        if(status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete display-fade framebuffer");}
        return true;
    }

bool VehicleWindowGl50DS::Storage::createDisplayFadeProgram() noexcept {
        static const char* vertex=R"GLSL(#version 150 core
void main() {
    vec2 p=gl_VertexID==0 ? vec2(-1.0,-1.0) :
           gl_VertexID==1 ? vec2(3.0,-1.0) : vec2(-1.0,3.0);
    gl_Position=vec4(p,0.0,1.0);
}
)GLSL";
        static const char* fragment=R"GLSL(#version 150 core
uniform sampler2D immutableRaceImage;
uniform vec4 rgbaqBytes;
out vec4 viewerOutput;
int signedFloor128(int value) {
    return value>=0 ? value/128 : -((-value+127)/128);
}
void main() {
    // Source PRIM0x46 is an untextured full-size sprite. ALPHA0x8000000044
    // supplies (Cs-Cd)*As/128+Cd. RGBAQ low bytes are used verbatim, including
    // source packing's signed-lane OR effects; Q is irrelevant to this sprite.
    // Integer byte policy avoids normalized-GL-alpha255 interpolation.
    ivec4 source=ivec4(rgbaqBytes);
    ivec3 destination=ivec3(floor(texelFetch(immutableRaceImage,
        ivec2(gl_FragCoord.xy),0).rgb*255.0+0.5));
    ivec3 product=(source.rgb-destination)*source.a;
    ivec3 rgb=destination+ivec3(signedFloor128(product.r),
        signedFloor128(product.g),signedFloor128(product.b));
    viewerOutput=vec4(vec3(clamp(rgb,ivec3(0),ivec3(255))),float(source.a))/255.0;
}
)GLSL";
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::vertex;
        if(!compileTrackShader(0x8B31,vertex,displayFadeVertexShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::fragment;
        if(!compileTrackShader(0x8B30,fragment,displayFadeFragmentShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::link;
        displayFadeProgram=shaderGl.createProgram();
        if(!check("create display-fade program") || !displayFadeProgram)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"zero display-fade program");
        shaderGl.attachShader(displayFadeProgram,displayFadeVertexShader);
        shaderGl.attachShader(displayFadeProgram,displayFadeFragmentShader);
        shaderGl.bindFragDataLocation(displayFadeProgram,vehicleGlColorOutput50CZ,"viewerOutput");
        shaderGl.linkProgram(displayFadeProgram);
        I linked=0;shaderGl.getProgramiv(displayFadeProgram,0x8B82,&linked);
        if(!check("link display-fade program"))return false;
        if(!linked){
            shaderGl.getProgramInfoLog(displayFadeProgram,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read display-fade program log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"display-fade program link failed");
        }
        displayFadeImageUniform=shaderGl.getUniformLocation(displayFadeProgram,"immutableRaceImage");
        displayFadeColorUniform=shaderGl.getUniformLocation(displayFadeProgram,"rgbaqBytes");
        if(!check("locate display-fade uniforms"))return false;
        if(displayFadeImageUniform<0 || displayFadeColorUniform<0)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"missing display-fade uniform");
        gl.genVertexArrays(1,&displayFadeVertexArray);
        if(!check("allocate display-fade vertex array") || !displayFadeVertexArray)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero display-fade vertex array");
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::none;return true;
    }

bool VehicleWindowGl50DS::Storage::restoreDisplayFadeBindings(I w,I h) noexcept {
        // The existing native renderer forbids unrelated foreign GL state.
        // Restore its normal binding/depth baseline, never clear source depth.
        gl.useProgram(0);drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,0);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.bindFramebuffer(framebuffer,0);gl.viewport(0,0,w,h);
        gl.disable(0x0BE2);gl.disable(0x0C11);gl.enable(0x0B71);
        gl.depthMask(1);gl.depthFunc(0x0201);gl.depthRange(0,1);gl.clearDepth(1);
        gl.colorMask(1,1,1,1);
        return check("restore native display-fade renderer state without clearing depth");
    }

bool VehicleWindowGl50DS::Storage::deleteDisplayFadeNames(VehicleWindowDiagnostic50DS& d) noexcept {
        // Last complete publication may point at an effect surface. Call only
        // after a newer admitted race/Pause publication or after close has invalidated it.
        displayFadeDeletionPending=true;
        if(!deleteShaderName(displayFadeProgram,true,d) ||
           !deleteShaderName(displayFadeVertexShader,false,d) ||
           !deleteShaderName(displayFadeFragmentShader,false,d) ||
           !deleteName(displayFadeVertexArray,gl.deleteVertexArrays,d))return false;
        for(U i=0;i<2;++i)
            if(!deleteName(displayFadeTargets[i],gl.deleteFramebuffers,d) ||
               !deleteName(displayFadeColors[i],gl.deleteTextures,d))return false;
        if(!deleteName(displayFadeSourceTarget,gl.deleteFramebuffers,d) ||
           !deleteName(displayFadeSourceColor,gl.deleteTextures,d))return false;
        displayFadeDeletionPending=false;return true;
    }

bool VehicleWindowGl50DS::Storage::hasGlResources() const noexcept {
        if(nativeVideoFinishFence50LA)return true;
        if(nativeVideoOverlayTextures50LA[0]||nativeVideoOverlayTextures50LA[1]||nativeVideoOverlayProgram50LA||nativeVideoOverlayShader50LA)return true;
        for(U b=0;b<2;++b)if(nativeVideoPacketTargets50LA[b]||nativeVideoPacketColors50LA[b]||
            nativeVideoSnapshotTargets50LA[b]||nativeVideoSnapshotColors50LA[b])return true;
        if(hasNativeResultsResources())return true;
        if(hasNativeF8BarResources())return true;
        if(garagePollFence50KG||garageTargetFence50KG)return true;
        for(U name:garageSamplers50KG)if(name)return true;
        if(hasRaWorldResources())return true;
        if(hasCoverageResources()||uiCoverage.owns())return true;
        if(hasRaceHudResources() || hasRaceStartOverlayResources() || hasTimeStatsResources() || hasPositionStatsResources() || hasDamageResources() || hasPauseDerivedResources() || hasSpeedBoostResources())return true;
        if(hasDisplayFadeNames())return true;
        if (hasGameplayBlurResources() || hasFxaaResources() || hasDebrisResources() || hasEffectResources() || hasWaterResources()) return true;
        if (hasShadowResources()) return true;
        if (shader && shader->hasResources()) return true;
        if (trackProgram || trackVertexShader || trackFragmentShader) return true;
        if (hostHudProgram || hostHudVertexShader || hostHudFragmentShader ||
            hostHudVertexBuffer || hostHudIndexBuffer || hostHudVertexArray) return true;
        if (uiProgram || uiVertexShader || uiFragmentShader ||
            uiVertexBuffer || uiIndexBuffer || uiVertexArray || uiParticleFogBuffer || uiSourceRasterWBuffer || uiSourcePacketStqBuffer ||
            uiSourceIndexBuffer) return true;
        for (U b = 0; b < 2; ++b)
            if (uiTargets[b] || uiColors[b] || uiDepths[b]) return true;
        if (uiSourceTarget || uiSourceColor || uiSourceDepth || uiRetainedTarget || uiRetainedColor) return true;
        if (uiBlendIncomingTarget || uiBlendIncomingColor || uiBlendEffectTarget || uiBlendEffectColor ||
            uiBlendProgram || uiBlendVertexShader || uiBlendFragmentShader || uiBlendVertexArray) return true;
        if (uiPromotedIncomingTarget || uiPromotedIncomingColor) return true;
        if (cableVertexBuffer || cableIndexBuffer || cableVertexArray) return true;
        if (trackVertexBuffer || trackIndexBuffer || trackVertexArray || trackNormalBuffer || trackProjectionBuffer) return true;
        if (dynamicWorldIndexBuffer) return true;
        for (U b = 0; b < 2; ++b)
            if (dynamicWorldVertexBuffers[b] || dynamicWorldNormalBuffers[b] || dynamicWorldArrays[b]) return true;
        if (skyVertexBuffer || skyIndexBuffer || skyVertexArray) return true;
        for (U b = 0; b < 2; ++b) {
            if (sourcePairColors50LA[b]) return true;
            for (const auto& body : sourceSpecularGeometry50LA)
                if (body.buffers[b] || body.fogBuffers[b]) return true;
        }
        for (U b = 0; b < 2; ++b) {
            if (targets[b] || colors[b] || depths[b] || menuTargets[b] || menuColors[b]) return true;
            for (U c = 0; c < maxComponents; ++c)
                if (resources.bank[b].vertexBuffer[c] || resources.bank[b].vertexArray[c] ||
                    racerNormalBuffers[b][c]) return true;
        }
        for (const auto& index : resources.indices) if (index.buffer) return true;
        for (const auto& archetype : racerArchetypes) {
            for (const auto& body : archetype.specularGeometry50LA) {
                for (U name : body.buffers) if (name) return true;
                for (U name : body.fogBuffers) if (name) return true;
            }
            for (const auto& bank : archetype.normalBuffers)
                for (U normal : bank) if (normal) return true;
            for (const auto& bank : archetype.resources.bank)
                for (U c = 0; c < maxComponents; ++c)
                    if (bank.vertexBuffer[c] || bank.vertexArray[c]) return true;
            for (const auto& index : archetype.resources.indices) if (index.buffer) return true;
        }
        return false;
    }

bool VehicleWindowGl50DS::Storage::check(const char* operation) noexcept {
        const U error = drawGl.getError();
        if (!error) return true;
        nativeGlFaultObserved50KG=true;
        if (!diagnostic->glError) diagnostic->glError = error;
        return fail(*diagnostic, VehicleWindowError50DS::gl, operation);
    }

bool VehicleWindowGl50DS::Storage::loadFunctions() noexcept {
#define LOAD(group, member, suffix) if (!load(group.member, "gl" #suffix)) return false
        LOAD(drawGl, bindBuffer, BindBuffer); LOAD(drawGl, bufferSubData, BufferSubData);
        LOAD(drawGl, bindVertexArray, BindVertexArray);
        LOAD(drawGl, drawElementsBaseVertex, DrawElementsBaseVertex);
        LOAD(drawGl, isBuffer, IsBuffer); LOAD(drawGl, isVertexArray, IsVertexArray);
        LOAD(drawGl, getError, GetError);
        LOAD(shaderGl, createShader, CreateShader); LOAD(shaderGl, shaderSource, ShaderSource);
        LOAD(shaderGl, compileShader, CompileShader); LOAD(shaderGl, getShaderiv, GetShaderiv);
        LOAD(shaderGl, getShaderInfoLog, GetShaderInfoLog); LOAD(shaderGl, deleteShader, DeleteShader);
        LOAD(shaderGl, createProgram, CreateProgram); LOAD(shaderGl, attachShader, AttachShader);
        LOAD(shaderGl, bindAttribLocation, BindAttribLocation);
        LOAD(shaderGl, bindFragDataLocation, BindFragDataLocation);
        LOAD(shaderGl, linkProgram, LinkProgram); LOAD(shaderGl, getProgramiv, GetProgramiv);
        LOAD(shaderGl, getProgramInfoLog, GetProgramInfoLog);
        LOAD(shaderGl, getUniformLocation, GetUniformLocation);
        LOAD(shaderGl, deleteProgram, DeleteProgram); shaderGl.getError = drawGl.getError;
        LOAD(gl, genBuffers, GenBuffers); LOAD(gl, deleteBuffers, DeleteBuffers);
        LOAD(gl, bufferData, BufferData); LOAD(gl, getBufferParameteri64v, GetBufferParameteri64v);
        LOAD(gl, getBufferParameteriv, GetBufferParameteriv); LOAD(gl, getBufferSubData, GetBufferSubData);
        LOAD(gl, genVertexArrays, GenVertexArrays); LOAD(gl, deleteVertexArrays, DeleteVertexArrays);
        LOAD(gl, enableVertexAttribArray, EnableVertexAttribArray);
        LOAD(gl, vertexAttribPointer, VertexAttribPointer); LOAD(gl, getVertexAttribiv, GetVertexAttribiv);
        LOAD(gl, getVertexAttribPointerv, GetVertexAttribPointerv);
        LOAD(gl, genFramebuffers, GenFramebuffers); LOAD(gl, deleteFramebuffers, DeleteFramebuffers);
        LOAD(gl, bindFramebuffer, BindFramebuffer); LOAD(gl, checkFramebufferStatus, CheckFramebufferStatus);
        LOAD(gl, framebufferRenderbuffer, FramebufferRenderbuffer);
        LOAD(gl, genRenderbuffers, GenRenderbuffers); LOAD(gl, deleteRenderbuffers, DeleteRenderbuffers);
        LOAD(gl, bindRenderbuffer, BindRenderbuffer); LOAD(gl, renderbufferStorage, RenderbufferStorage);
        LOAD(gl, getRenderbufferParameteriv, GetRenderbufferParameteriv);
        LOAD(gl, getIntegerv, GetIntegerv); LOAD(gl, useProgram, UseProgram);
        LOAD(gl, uniformMatrix4fv, UniformMatrix4fv); LOAD(gl, uniform1i, Uniform1i);
        LOAD(gl, uniform1f, Uniform1f); LOAD(gl, uniform4f, Uniform4f);
        LOAD(gl, uniform4fv, Uniform4fv); LOAD(gl, uniform3fv, Uniform3fv); LOAD(gl, uniform2fv, Uniform2fv);
        LOAD(gl, blendFunc, BlendFunc); LOAD(gl, blendEquation, BlendEquation);
        LOAD(gl, blendFuncSeparate, BlendFuncSeparate);
        {
            const char* exact = std::getenv("RR_EXACT_PS2_SPECULAR");
            fixedSpecularBlend50LA = !(exact && exact[0] == '1' && !exact[1]);
            // DELIBERATE, USER-APPROVED DEVIATION (Nov 12 2026): under AA1
            // coverage (RC/TB) pod map 2 uses the fixed-function blend and GPU
            // pod skinning instead of the exact per-sample bridge. Measured by
            // pair_route_compare: up to 2 colour levels on about 30% of pod
            // highlight pixels (rounding: the bridge floors, the GPU blend
            // rounds), for about 62 FPS instead of about 40 on RC and TB.
            // RR_COVERAGE_PAIR_EXACT=1 restores the exact bridge route.
            const char* exactCoverage = std::getenv("RR_COVERAGE_PAIR_EXACT");
            coverageFixedSpecular50LA = !(exactCoverage && exactCoverage[0] == '1' && !exactCoverage[1]);
            const char* compare = std::getenv("RR_TEST_PAIR_ROUTE_COMPARE");
            pairCompare.enabled = compare && compare[0] == '1' && !compare[1];
            const char* finish = std::getenv("RR_GPU_FINISH_EACH_FRAME");
            finishEachGameplayFrame = finish && finish[0] == '1' && !finish[1];
            const char* cpuPods = std::getenv("RR_CPU_POD_VERTICES");
            cpuPodVertices50LA = cpuPods && cpuPods[0] == '1' && !cpuPods[1];
            const char* vertexCheck = std::getenv("RR_GPU_POD_VERTEX_CHECK");
            gpuSkinCheck50LA.requested = vertexCheck && std::atoll(vertexCheck) > 0;
        }
        LOAD(gl, viewport, Viewport); LOAD(gl, enable, Enable); LOAD(gl, disable, Disable);
        LOAD(gl, depthFunc, DepthFunc); LOAD(gl, depthMask, DepthMask);
        LOAD(gl, depthRange, DepthRange); LOAD(gl, clearDepth, ClearDepth);
        LOAD(gl, colorMask, ColorMask); LOAD(gl, polygonMode, PolygonMode);
        LOAD(gl, clearColor, ClearColor); LOAD(gl, clear, Clear);
        LOAD(gl, drawBuffer, DrawBuffer); LOAD(gl, readBuffer, ReadBuffer);
        LOAD(gl, pixelStorei, PixelStorei); LOAD(gl, readPixels, ReadPixels);
        LOAD(gl, finish, Finish); LOAD(gl, flush, Flush);
        LOAD(gl, getString, GetString);
        // Host pause menus copy the retained race image between private FBOs
        // in headless mode too. Resolve before exposing either blit path.
        if (request.output == VehicleWindowOutput50DS::defaultBackBuffer || request.enableHostHud || request.enableOriginalUi || request.enableOriginalGameplayBlur) {
            LOAD(gl, blitFramebuffer, BlitFramebuffer);
        }
        if (request.enableOriginalUi || request.gcParticleTextures || request.track.rcParticleWindow || request.track.gbParticleWindow || request.track.mcParticleWindow) {
            LOAD(gl, scissor, Scissor); LOAD(gl, getTexParameteriv, GetTexParameteriv);
        }
        if (usesTextures()) {
            LOAD(gl, activeTexture, ActiveTexture); LOAD(gl, bindTexture, BindTexture);
        }
        if (request.enableOriginalUiSourceRgba || request.enableOriginalGameplayBlur || hasSpecularPairs50LA) {
            LOAD(gl, genTextures, GenTextures); LOAD(gl, deleteTextures, DeleteTextures);
            LOAD(gl, texImage2D, TexImage2D); LOAD(gl, texParameteri, TexParameteri);
            LOAD(gl, getTexLevelParameteriv, GetTexLevelParameteriv);
            LOAD(gl, framebufferTexture2D, FramebufferTexture2D);
        }
        if (request.enableOriginalGameplayBlur) {
            LOAD(gl, drawArrays, DrawArrays); LOAD(gl, uniform2i, Uniform2i); LOAD(gl, uniform4i, Uniform4i);
        }
#undef LOAD
        return true;
    }

bool VehicleWindowGl50DS::Storage::compileTrackShader(U type, const char* source, U& name) noexcept {
        name = shaderGl.createShader(type);
        if (!check("create source-color track shader")) return false;
        if (!name) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero track shader name");
        shaderGl.shaderSource(name, 1, &source, nullptr);
        shaderGl.compileShader(name);
        I compiled = 0;
        shaderGl.getShaderiv(name, 0x8B81, &compiled); // GL_COMPILE_STATUS
        if (!check("compile source-color track shader")) return false;
        if (compiled) return true;
        shaderGl.getShaderInfoLog(name, static_cast<I>(sizeof(diagnostic->shader.log)),
            nullptr, diagnostic->shader.log);
        diagnostic->shader.log[sizeof(diagnostic->shader.log) - 1] = '\0';
        check("read source-color track shader log");
        return fail(*diagnostic, VehicleWindowError50DS::shader, "source-color track shader compilation failed");
    }

bool VehicleWindowGl50DS::Storage::createTrackShader() noexcept {
        if (!usesTrackShader()) return true;
        // Unchanged positions/UV/RGB/index stream, optional authenticated
        // normals in a separate VBO. No mesh welding or fabricated detail.
        // Missing normals preserve the prior derivative-flat display policy.
        // This explicit host illumination is not original-game shader parity.
        constexpr const char* vertexSource = R"GLSL(#version 150 core
in vec3 rrPosition;
in vec2 rrUv;
in vec4 rrColor;
in vec3 rrWorldNormal;
in vec4 rrProjectedUv;
uniform int projectionPass;
uniform int environmentMap;
uniform vec4 trackUvTranslation;
uniform int worldFogEnabled;
uniform vec4 worldFogCamera;
uniform vec4 worldFogRange;
out float trackFogAmount;
uniform mat4 worldViewProjection;
out vec3 trackWorldNormal;
out vec3 trackWorldPosition;
out vec4 trackSourceColor;
noperspective out vec4 rcSourceColor;
out vec2 trackSourceUv;
void main() {
    gl_Position = vec4(rrPosition, 1.0) * worldViewProjection;
    trackWorldPosition = rrPosition;
    // Native radial world distance per vertex. No clip-W/perspective scale.
    trackFogAmount = worldFogEnabled != 0 ? clamp(
        (length(rrPosition - worldFogCamera.xyz) - worldFogRange.x) /
        (worldFogRange.y - worldFogRange.x), 0.0, 1.0) : 0.0;
    trackWorldNormal = rrWorldNormal;
    trackSourceColor = rrColor;
    rcSourceColor = rrColor;
    trackSourceUv = environmentMap != 0 ? rrProjectedUv.xy : projectionPass == 1 ? rrProjectedUv.xy :
        (projectionPass == 2 ? rrProjectedUv.zw : rrUv);
    trackSourceUv += trackUvTranslation.xy;
}
)GLSL";
        constexpr const char* fragmentSource = R"GLSL(#version 150 core
in vec3 trackWorldPosition;
in float trackFogAmount;
uniform int worldFogEnabled;
uniform vec4 worldFogColor;
in vec3 trackWorldNormal;
in vec4 trackSourceColor;
noperspective in vec4 rcSourceColor;
in vec2 trackSourceUv;
uniform int gsAlphaPass;
uniform int gsAlphaPayload;
uniform int environmentMap;
uniform vec4 environmentAmbient;
out float rcCenterDepth;
uniform float vertexColorScale;
uniform int useTexture;
uniform int discardZeroAlpha;
uniform float sourceAlphaReference;
uniform int effectMode;
uniform float alphaScale;
uniform int unlit;
uniform int sourceColorAbsent;
uniform int useSourceNormals;
uniform sampler2D image;
out vec4 viewerOutput;
void main() {
    if(gsAlphaPass!=0){
        // GS byte source colour: texel x vertex byte / 128 (prelit vertex
        // bytes, or the ambient row for the environment-map program).
        vec4 vertexByte=environmentMap!=0?environmentAmbient:clamp(rcSourceColor*255.0,0.0,255.0);
        vec4 source=floor(clamp(texture(image,trackSourceUv)*255.0*vertexByte/128.0,0.0,255.0));
        if(discardZeroAlpha==2&&source.a<=sourceAlphaReference)discard;
        // Explicit existing native radial-fog presentation, not GS raster parity.
        if(worldFogEnabled!=0)source.rgb=floor(mix(source.rgb,worldFogColor.rgb*255.0,trackFogAmount));
        rcCenterDepth=gl_FragCoord.z;
        if(gsAlphaPayload==2){
            // Fixed-function GS A=Cs,B=0,D=Cd (see drawDestinationAlphaRange).
            // Pass 1 (C=As): this fragment's own floor(Cs*As/128), added by
            // ONE,ONE. Pass 2 (C=Ad): Cs/128 times the DST_ALPHA factor. Both
            // write source alpha independently (blend A: ONE,ZERO), as the GS does.
            viewerOutput=gsAlphaPass==1?
                vec4(min(floor(source.rgb*source.a/128.0),255.0)/255.0,source.a/255.0):
                vec4(source.rgb/128.0,source.a/255.0);
            return;
        }
        viewerOutput=gsAlphaPayload!=0?source:vec4(0.0);
        return;
    }
    vec3 plane = cross(dFdx(trackWorldPosition), dFdy(trackWorldPosition));
    float squaredLength = dot(plane, plane);
    float illumination = 0.45;
    vec3 normal = vec3(0.0, 1.0, 0.0);
    vec3 lightDirection = normalize(vec3(0.35, 0.85, 0.30));
    if (squaredLength > 0.0) {
        normal = plane * inversesqrt(squaredLength);
        illumination += 0.55 * abs(dot(normal, lightDirection));
    }
    if (useSourceNormals != 0) {
        float authoredLength = dot(trackWorldNormal, trackWorldNormal);
        if (authoredLength > 0.000001) {
            normal = trackWorldNormal * inversesqrt(authoredLength);
        } else if (!gl_FrontFacing) {
            // Correct derivative orientation only for the explicit zero-normal
            // fallback. Authored normal signs do not follow the camera.
            normal = -normal;
        }
        float skyAmbient = 0.32 + 0.16 * max(normal.y, 0.0);
        illumination = skyAmbient + 0.52 * max(dot(normal, lightDirection), 0.0);
    }
    // A surface without a source colour stream carries the export's diagnostic
    // white. The original program emits light-row bytes instead: RGB at the GS
    // neutral 128 (host illumination stands in for the rows), alpha = the
    // zone-ambient .w 64. Same rule for every course; keyed on geometry data.
    vec4 sourceColor = sourceColorAbsent != 0 ?
        vec4(128.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0, 64.0 / 255.0) : trackSourceColor;
    vec3 color = sourceColor.rgb * vertexColorScale * (unlit != 0 ? 1.0 : illumination);
    float alpha = sourceColor.a;
    if (useTexture != 0) {
        vec4 texel = texture(image, trackSourceUv);
        if (discardZeroAlpha == 1 && texel.a <= 0.0) discard;
        color *= texel.rgb;
        alpha *= texel.a;
    }
    // Declared native GS-unit alpha policy. Original bytes remain unchanged;
    // native interpolation/raster precision is not a PS2 fragment-parity claim.
    float sourceAlphaByte = floor(clamp(alpha * (255.0 * 255.0 / 128.0), 0.0, 255.0));
    if (discardZeroAlpha == 2 && sourceAlphaByte <= sourceAlphaReference) discard;
    // Binder's independent source-A store uses the same texture sample and
    // byte alpha test as its RGB pass. ALPHA blends RGB, never stored alpha.
    if (effectMode == 4) {
        viewerOutput = vec4(0.0, 0.0, 0.0, sourceAlphaByte / 255.0);
        return;
    }
    if (effectMode == 2) color *= alphaScale;
    if (worldFogEnabled != 0) color = mix(color, worldFogColor.rgb, trackFogAmount);
    viewerOutput = vec4(color, effectMode == 3 ? sourceAlphaByte / 128.0 :
        effectMode == 1 ? clamp(alpha * alphaScale, 0.0, 1.0) : 1.0);
}
)GLSL";
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::vertex;
        if (!compileTrackShader(0x8B31, vertexSource, trackVertexShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::fragment;
        if (!compileTrackShader(0x8B30, fragmentSource, trackFragmentShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::link;
        trackProgram = shaderGl.createProgram();
        if (!check("create source-color track program")) return false;
        if (!trackProgram) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero track program name");
        shaderGl.attachShader(trackProgram, trackVertexShader);
        shaderGl.attachShader(trackProgram, trackFragmentShader);
        shaderGl.bindAttribLocation(trackProgram, vehicleGlPositionAttribute50CZ, "rrPosition");
        shaderGl.bindAttribLocation(trackProgram, vehicleGlUvAttribute50CZ, "rrUv");
        shaderGl.bindAttribLocation(trackProgram, vehicleGlColorAttribute50CZ, "rrColor");
        shaderGl.bindAttribLocation(trackProgram, 3, "rrWorldNormal");
        shaderGl.bindAttribLocation(trackProgram, 4, "rrProjectedUv");
        shaderGl.bindFragDataLocation(trackProgram, vehicleGlColorOutput50CZ, "viewerOutput");
        shaderGl.bindFragDataLocation(trackProgram,1,"rcCenterDepth");
        shaderGl.linkProgram(trackProgram);
        I linked = 0;
        shaderGl.getProgramiv(trackProgram, 0x8B82, &linked); // GL_LINK_STATUS
        if (!check("link source-color track program")) return false;
        if (!linked) {
            shaderGl.getProgramInfoLog(trackProgram, static_cast<I>(sizeof(diagnostic->shader.log)),
                nullptr, diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log) - 1] = '\0';
            check("read source-color track link log");
            return fail(*diagnostic, VehicleWindowError50DS::shader, "source-color track program link failed");
        }
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::uniforms;
        gsAlphaPassUniform=shaderGl.getUniformLocation(trackProgram,"gsAlphaPass");
        gsAlphaPayloadUniform=shaderGl.getUniformLocation(trackProgram,"gsAlphaPayload");
        environmentMapUniform=shaderGl.getUniformLocation(trackProgram,"environmentMap");
        environmentAmbientUniform=shaderGl.getUniformLocation(trackProgram,"environmentAmbient");
        worldFogEnabledUniform = shaderGl.getUniformLocation(trackProgram, "worldFogEnabled");
        worldFogCameraUniform = shaderGl.getUniformLocation(trackProgram, "worldFogCamera");
        worldFogColorUniform = shaderGl.getUniformLocation(trackProgram, "worldFogColor");
        worldFogRangeUniform = shaderGl.getUniformLocation(trackProgram, "worldFogRange");
        trackMatrixUniform = shaderGl.getUniformLocation(trackProgram, "worldViewProjection");
        trackColorScaleUniform = shaderGl.getUniformLocation(trackProgram, "vertexColorScale");
        trackUseTextureUniform = shaderGl.getUniformLocation(trackProgram, "useTexture");
        trackImageUniform = shaderGl.getUniformLocation(trackProgram, "image");
        trackDiscardZeroAlphaUniform = shaderGl.getUniformLocation(trackProgram, "discardZeroAlpha");
        trackEffectModeUniform = shaderGl.getUniformLocation(trackProgram, "effectMode");
        trackAlphaScaleUniform = shaderGl.getUniformLocation(trackProgram, "alphaScale");
        trackUnlitUniform = shaderGl.getUniformLocation(trackProgram, "unlit");
        trackSourceColorAbsentUniform = shaderGl.getUniformLocation(trackProgram, "sourceColorAbsent");
        trackUseSourceNormalsUniform = shaderGl.getUniformLocation(trackProgram, "useSourceNormals");
        trackProjectionPassUniform = shaderGl.getUniformLocation(trackProgram, "projectionPass");
        trackUvTranslationUniform = shaderGl.getUniformLocation(trackProgram, "trackUvTranslation");
        trackAlphaReferenceUniform = shaderGl.getUniformLocation(trackProgram, "sourceAlphaReference");
        if (!check("query source-color track uniforms")) return false;
        if (gsAlphaPassUniform<0||gsAlphaPayloadUniform<0||environmentAmbientUniform<0||environmentMapUniform<0||
            worldFogEnabledUniform < 0 || worldFogCameraUniform < 0 ||
            worldFogColorUniform < 0 || worldFogRangeUniform < 0 ||
            trackMatrixUniform < 0 || trackColorScaleUniform < 0 ||
            trackUseTextureUniform < 0 || trackImageUniform < 0 || trackDiscardZeroAlphaUniform < 0 ||
            trackEffectModeUniform < 0 || trackAlphaScaleUniform < 0 || trackUnlitUniform < 0 ||
            trackUseSourceNormalsUniform < 0 || trackProjectionPassUniform < 0 || trackUvTranslationUniform < 0 || trackAlphaReferenceUniform < 0)
            return fail(*diagnostic, VehicleWindowError50DS::shader, "source-color track uniform missing");
        if (trackSourceColorAbsentUniform < 0)
            return fail(*diagnostic, VehicleWindowError50DS::shader, "source colour-stream uniform missing");
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::none;
        return true;
    }

bool VehicleWindowGl50DS::Storage::verifyBuffer(U target, std::size_t bytes, U usage) noexcept {
        std::int64_t actualBytes = 0;
        I actualUsage = 0;
        gl.getBufferParameteri64v(target, bufferSize, &actualBytes);
        gl.getBufferParameteriv(target, bufferUsage, &actualUsage);
        if (!check("verify buffer allocation")) return false;
        return (actualBytes == static_cast<std::int64_t>(bytes) && actualUsage == static_cast<I>(usage)) ||
            fail(*diagnostic, VehicleWindowError50DS::allocation, "buffer size/usage mismatch");
    }

bool VehicleWindowGl50DS::Storage::verifyLayout(U vertexBuffer, U indices) noexcept {
        constexpr I sizes[] = {3, 2, 4};
        constexpr U types[] = {floatType, floatType, unsignedByte};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        constexpr U properties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
        // enabled, size, stride, type, normalized, VBO association, integer.
        for (U a = 0; a < 3; ++a) {
            const I expected[] = {1, sizes[a], 24, static_cast<I>(types[a]), a == 2 ? 1 : 0,
                static_cast<I>(vertexBuffer), 0};
            for (U p = 0; p < 7; ++p) {
                I value = 0;
                gl.getVertexAttribiv(a, properties[p], &value);
                if (!check("glGetVertexAttribiv")) return false;
                if (value != expected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout,
                    "vertex attribute layout mismatch");
            }
            void* pointer = nullptr;
            gl.getVertexAttribPointerv(a, 0x8645, &pointer);
            if (!check("glGetVertexAttribPointerv")) return false;
            if (reinterpret_cast<std::uintptr_t>(pointer) != offsets[a])
                return fail(*diagnostic, VehicleWindowError50DS::layout, "vertex attribute offset mismatch");
        }
        I element = 0;
        gl.getIntegerv(0x8895, &element);
        if (!check("verify VAO element buffer")) return false;
        return static_cast<U>(element) == indices ||
            fail(*diagnostic, VehicleWindowError50DS::layout, "VAO element association mismatch");
    }

bool VehicleWindowGl50DS::Storage::createGeometry(VehicleGlResources50CV& resources,
        const game::VehicleDrawMesh50CM* const* meshes) noexcept {
        for (U c = 0; c < resources.componentCount; ++c) {
            diagnostic->component = c;
            const auto& mesh = *meshes[c];
            auto& index = resources.indices[c];
            index.data = mesh.indices; index.count = mesh.indexCount;
            const std::size_t indexBytes = std::size_t(index.count) * sizeof(U);
            gl.genBuffers(1, &index.buffer);
            // ARRAY_BUFFER can initialize an EBO without changing VAO state.
            drawGl.bindBuffer(arrayBuffer, index.buffer);
            gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(indexBytes), index.data, staticDraw);
            if (!check("create immutable index buffer")) return false;
            if (!index.buffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero index buffer name");
            if (!verifyBuffer(arrayBuffer, indexBytes, staticDraw)) return false;
            // Cold, exact GPU-side contents check; only 1 KiB stack scratch.
            U words[256];
            for (U start = 0; start < index.count;) {
                const U count = std::min<U>(256, index.count - start);
                gl.getBufferSubData(arrayBuffer, std::intptr_t(start) * sizeof(U),
                    std::intptr_t(count) * sizeof(U), words);
                if (!check("verify retained index bytes")) return false;
                if (std::memcmp(words, index.data + start, std::size_t(count) * sizeof(U)))
                    return fail(*diagnostic, VehicleWindowError50DS::indices, "GPU index contents mismatch");
                start += count;
            }
            for (auto& bank : resources.bank) {
                bank.vertexCapacity[c] = mesh.vertexCount;
                gl.genBuffers(1, &bank.vertexBuffer[c]);
                drawGl.bindBuffer(arrayBuffer, bank.vertexBuffer[c]);
                const std::size_t bytes = std::size_t(mesh.vertexCount) * sizeof(assets::Vertex48B);
                gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), nullptr, dynamicDraw);
                if (!check("allocate inactive vertex bank")) return false;
                if (!bank.vertexBuffer[c]) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero vertex buffer name");
                if (!verifyBuffer(arrayBuffer, bytes, dynamicDraw)) return false;
                gl.genVertexArrays(1, &bank.vertexArray[c]);
                drawGl.bindVertexArray(bank.vertexArray[c]);
                drawGl.bindBuffer(elementBuffer, index.buffer);
                constexpr I sizes[] = {3, 2, 4};
                constexpr std::uintptr_t offsets[] = {0, 12, 20};
                for (U a = 0; a < 3; ++a) {
                    gl.enableVertexAttribArray(a);
                    gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                        a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
                }
                if (!check("create Vertex48B VAO")) return false;
                if (!bank.vertexArray[c]) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero vertex array name");
                if (!verifyLayout(bank.vertexBuffer[c], index.buffer)) return false;
            }
        }
        diagnostic->component = maxComponents;
        return true;
    }

bool VehicleWindowGl50DS::Storage::createStaticGeometry(const assets::Mesh48B* mesh, U& trackVertexBuffer,
        U& trackIndexBuffer, U& trackVertexArray, I& trackIndexCount,
        U vertexUsage, bool reuseIndices) noexcept {
        if (!mesh) return true;
        const std::size_t vertexBytes = mesh->vertices.size() * sizeof(assets::Vertex48B);
        const std::size_t indexBytes = mesh->indices.size() * sizeof(U);
        // The complete input was validated before any renderer/GL allocation.
        // Use ARRAY_BUFFER for both uploads so no other VAO's EBO is changed.
        U* const buffers[] = {&trackVertexBuffer, &trackIndexBuffer};
        const void* const inputs[] = {mesh->vertices.data(), mesh->indices.data()};
        const std::size_t bufferBytes[] = {vertexBytes, indexBytes};
        std::uint8_t scratch[1024];
        for (U b = 0; b < 2; ++b) {
            if (b == 1 && reuseIndices) continue;
            const U usage = b == 0 ? vertexUsage : staticDraw;
            gl.genBuffers(1, buffers[b]);
            drawGl.bindBuffer(arrayBuffer, *buffers[b]);
            gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bufferBytes[b]), inputs[b], usage);
            if (!check("upload static world-space track buffer")) return false;
            if (!*buffers[b])
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero track buffer name");
            if (!verifyBuffer(arrayBuffer, bufferBytes[b], usage)) return false;
            const auto* source = static_cast<const std::uint8_t*>(inputs[b]);
            for (std::size_t offset = 0; offset < bufferBytes[b];) {
                const std::size_t count = std::min(sizeof(scratch), bufferBytes[b] - offset);
                gl.getBufferSubData(arrayBuffer, static_cast<std::intptr_t>(offset),
                    static_cast<std::intptr_t>(count), scratch);
                if (!check("verify uploaded track bytes")) return false;
                if (std::memcmp(scratch, source + offset, count))
                    return fail(*diagnostic, b == 0 ? VehicleWindowError50DS::assets :
                        VehicleWindowError50DS::indices, "GPU track contents mismatch");
                offset += count;
            }
        }
        gl.genVertexArrays(1, &trackVertexArray);
        drawGl.bindVertexArray(trackVertexArray);
        drawGl.bindBuffer(arrayBuffer, trackVertexBuffer);
        drawGl.bindBuffer(elementBuffer, trackIndexBuffer);
        constexpr I sizes[] = {3, 2, 4};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        for (U a = 0; a < 3; ++a) {
            gl.enableVertexAttribArray(a);
            gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
        }
        if (!check("create static track Vertex48B VAO")) return false;
        if (!trackVertexArray)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero track vertex array name");
        if (!verifyLayout(trackVertexBuffer, trackIndexBuffer)) return false;
        trackIndexCount = static_cast<I>(mesh->indices.size());
        return true;
    }

bool VehicleWindowGl50DS::Storage::createWorldNormals(const VehicleWindowTrack50DS& track, U& trackNormalBuffer,
        U trackVertexArray, U usage) noexcept {
        if (!track.sourceNormals) return true;
        const std::size_t bytes = std::size_t(track.sourceNormalCount) * sizeof(assets::WorldNormal50HK);
        gl.genBuffers(1, &trackNormalBuffer);
        drawGl.bindBuffer(arrayBuffer, trackNormalBuffer);
        gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), track.sourceNormals, usage);
        if (!check("upload authenticated track source normals")) return false;
        if (!trackNormalBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero source normal buffer name");
        if (!verifyBuffer(arrayBuffer, bytes, usage)) return false;
        std::uint8_t scratch[1024];
        const auto* source = reinterpret_cast<const std::uint8_t*>(track.sourceNormals);
        for (std::size_t offset = 0; offset < bytes;) {
            const auto count = std::min(sizeof(scratch), bytes - offset);
            gl.getBufferSubData(arrayBuffer, static_cast<std::intptr_t>(offset),
                static_cast<std::intptr_t>(count), scratch);
            if (!check("verify authenticated track normal bytes")) return false;
            if (std::memcmp(scratch, source + offset, count))
                return fail(*diagnostic, VehicleWindowError50DS::assets, "GPU track source normal mismatch");
            offset += count;
        }
        drawGl.bindVertexArray(trackVertexArray);
        gl.enableVertexAttribArray(3);
        gl.vertexAttribPointer(3, 3, floatType, 0, sizeof(assets::WorldNormal50HK), nullptr);
        if (!check("bind authenticated track normal attribute")) return false;
        constexpr U properties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
        const I expected[] = {1, 3, sizeof(assets::WorldNormal50HK), static_cast<I>(floatType), 0,
            static_cast<I>(trackNormalBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(3, properties[p], &value);
            if (!check("verify authenticated track normal attribute")) return false;
            if (value != expected[p])
                return fail(*diagnostic, VehicleWindowError50DS::layout, "source normal attribute layout mismatch");
        }
        void* pointer = nullptr; gl.getVertexAttribPointerv(3, 0x8645, &pointer);
        if (!check("verify authenticated normal offset")) return false;
        return pointer == nullptr || fail(*diagnostic, VehicleWindowError50DS::layout, "source normal attribute offset mismatch");
    }

bool VehicleWindowGl50DS::Storage::createSpecularGroups50LA(U archetype, U body, SpecularGeometry50LA& out) noexcept {
        const auto selected = racerView(archetype);
        if (!selected.materials) return true;
        if (body >= sourceBodyCount50LA)
            return fail(*diagnostic, VehicleWindowError50DS::assets, "source pair requires a pod body slot");
        const U c = body;
        // Three-model pods have no lower scenes, meshes or material offsets.
        // Leave their two absent body slots empty before indexing any of them.
        if (c >= selected.resources->componentCount) return true;
        const auto& scene = *selected.scenes[c];
        const auto& mesh = *selected.meshes[c];
        if(scene.materials.size()>maxVehicleWindowMaterials50DS)return false;
        std::array<std::uint8_t,maxVehicleWindowMaterials50DS> singleModes{};
        for (U m = 0; m < scene.materials.size(); ++m) {
            const auto& policy=selected.materials[selected.materialOffsets[c]+m].policy;
            out.enabled = out.enabled || policy.sourceSpecularPair50LA;
            out.extraPassUv50PP = out.extraPassUv50PP || policy.extraPass50PP != VehicleWindowExtraPass50PP::none;
            singleModes[m]=policy.sourceSingleDiffuseRgba50LA?1u:0u;
            out.singleDiffuse=out.singleDiffuse||policy.sourceSingleDiffuseRgba50LA;
        }
        if(out.singleDiffuse){
            if(!game::original_racer_diffuse50LA::prepareCold(mesh,singleModes.data(),scene.materials.size(),out.diffuseGeometry))
                return fail(*diagnostic,VehicleWindowError50DS::assets,"single diffuse original group/quantized palette inputs rejected");
            hasSingleDiffuse50LA=true;
        }
        if (!out.enabled && !out.singleDiffuse && !out.extraPassUv50PP) return true;
        if (!sourceBodyComponent50LA(c, selected.resources->componentCount))
            return fail(*diagnostic, VehicleWindowError50DS::assets,
                "source pair requires an actual pod model in a three/five-model pod");
        if (mesh.semanticScene != &scene || scene.lods.empty() ||
            scene.lods[0].surfaces.size() != mesh.spanCount || mesh.spanCount > game::maxVehicleDrawSpans50CM)
            return fail(*diagnostic, VehicleWindowError50DS::assets, "source pair requires exact semantic LOD0 spans");
        const U capacity = mesh.vertexCount / 3; // Every original group owns at least three vertices.
        out.groups.reset(new (std::nothrow) SpecularGroup50LA[capacity]);
        if (!out.groups) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate bounded original body group table");
        for (U spanIndex = 0; spanIndex < mesh.spanCount; ++spanIndex) {
            const auto& span = mesh.spans[spanIndex];
            const auto& spanPolicy=selected.materials[selected.materialOffsets[c]+span.material_index].policy;
            if (!spanPolicy.sourceSpecularPair50LA && !spanPolicy.sourceSingleDiffuseRgba50LA) continue;
            const auto& surface = scene.lods[0].surfaces[spanIndex];
            if (surface.material_index != span.material_index || surface.vertices.size() != span.vertex_count ||
                surface.indices.size() != span.index_count || surface.strips.empty() || !surface.groups ||
                surface.groups > capacity || span.index_start > mesh.indexCount ||
                span.index_count > mesh.indexCount - span.index_start || span.vertex_start > mesh.vertexCount ||
                span.vertex_count > mesh.vertexCount - span.vertex_start)
                return fail(*diagnostic, VehicleWindowError50DS::assets, "source body surface/group extent differs");
            out.spanFirst[spanIndex] = out.groupCount;
            U index = 0, vertexBase = 0, stripIndex = 0;
            for (U group = 0; group < surface.groups; ++group) {
                if (out.groupCount >= capacity || stripIndex >= surface.strips.size())
                    return fail(*diagnostic, VehicleWindowError50DS::assets, "source body group count escapes topology");
                const U firstIndex = index;
                U nextVertex = 0;
                while (stripIndex < surface.strips.size() && surface.strips[stripIndex].group == group) {
                    const auto& strip = surface.strips[stripIndex++];
                    if (strip.start != nextVertex || strip.end < strip.start + 3 || strip.end > 16 ||
                        strip.vertex_start != vertexBase + strip.start || strip.vertex_start > surface.vertices.size() ||
                        strip.end - strip.start > surface.vertices.size() - strip.vertex_start)
                        return fail(*diagnostic, VehicleWindowError50DS::assets, "source body strip order/3..16 vertex group differs");
                    for (U v = strip.start + 2; v < strip.end; ++v) {
                        U a = vertexBase + v - 2, b = vertexBase + v - 1;
                        if ((v - strip.start) & 1u) std::swap(a, b);
                        const U triangle[] = {a, b, vertexBase + v};
                        for (U expected : triangle) {
                            if (index >= surface.indices.size() || surface.indices[index] != expected ||
                                mesh.indices[span.index_start + index] != span.vertex_start + expected)
                                return fail(*diagnostic, VehicleWindowError50DS::indices, "source body group triangle order differs from retained EBO");
                            ++index;
                        }
                    }
                    nextVertex = strip.end;
                }
                if (nextVertex < 3 || index == firstIndex)
                    return fail(*diagnostic, VehicleWindowError50DS::assets, "empty or missing original body group");
                out.groups[out.groupCount++] = {span.index_start + firstIndex, index - firstIndex};
                ++out.spanCount[spanIndex];
                vertexBase += nextVertex;
            }
            if (stripIndex != surface.strips.size() || vertexBase != surface.vertices.size() || index != span.index_count)
                return fail(*diagnostic, VehicleWindowError50DS::assets, "source body groups do not partition the retained surface");
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::createDiffuseFogBuffers50LA(U archetype,U body,SpecularGeometry50LA& target) noexcept {
        if(!target.singleDiffuse)return true;
        const auto selected=racerView(archetype);const U c=body;
        const auto bytes=std::size_t(selected.meshes[c]->vertexCount)*sizeof(float);
        constexpr U attribute=vehicleGlSourceFogAttribute50LA;
        for(U bank=0;bank<2;++bank){
            gl.genBuffers(1,&target.fogBuffers[bank]);drawGl.bindBuffer(arrayBuffer,target.fogBuffers[bank]);
            gl.bufferData(arrayBuffer,static_cast<std::intptr_t>(bytes),nullptr,dynamicDraw);
            if(!check("allocate single diffuse fog stream")||!target.fogBuffers[bank]||
               !verifyBuffer(arrayBuffer,bytes,dynamicDraw))return false;
            drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[c]);
            gl.enableVertexAttribArray(attribute);gl.vertexAttribPointer(attribute,1,floatType,0,sizeof(float),nullptr);
            constexpr U properties[]={0x8622,0x8623,0x8624,0x8625,0x886A,0x889F,0x88FD};
            const I expected[]={1,1,sizeof(float),static_cast<I>(floatType),0,static_cast<I>(target.fogBuffers[bank]),0};
            for(U p=0;p<7;++p){I actual=0;gl.getVertexAttribiv(attribute,properties[p],&actual);
                if(!check("verify single diffuse fog attribute")||actual!=expected[p])return false;}
            void* offset=nullptr;gl.getVertexAttribPointerv(attribute,0x8645,&offset);
            if(!check("verify single diffuse fog offset")||offset)return false;
        }return true;
    }

bool VehicleWindowGl50DS::Storage::createSpecularUvBuffers50LA(U archetype, U body, SpecularGeometry50LA& target) noexcept {
        if(!createDiffuseFogBuffers50LA(archetype,body,target))return false;
        if (!target.needsUv50PP()) return true;
        if (body >= sourceBodyCount50LA)
            return fail(*diagnostic, VehicleWindowError50DS::assets, "map-2 UVs require a pod body slot");
        const auto selected = racerView(archetype);
        const U c = body;
        if (!sourceBodyComponent50LA(c, selected.resources->componentCount))
            return fail(*diagnostic, VehicleWindowError50DS::assets,
                "map-2 UVs require an actual pod model in a three/five-model pod");
        constexpr U attribute = vehicleGlSpecularUvAttribute50LA;
        const std::size_t bytes = std::size_t(selected.meshes[c]->vertexCount) * sizeof(std::array<float, 2>);
        for (U bank = 0; bank < 2; ++bank) {
            gl.genBuffers(1, &target.buffers[bank]);
            drawGl.bindBuffer(arrayBuffer, target.buffers[bank]);
            gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), nullptr, dynamicDraw);
            if (!check("allocate private bounded map-2 UV stream") || !target.buffers[bank])
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "map-2 UV buffer allocation failed");
            if (!verifyBuffer(arrayBuffer, bytes, dynamicDraw)) return false;
            drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[c]);
            gl.enableVertexAttribArray(attribute);
            gl.vertexAttribPointer(attribute, 2, floatType, 0, sizeof(std::array<float, 2>), nullptr);
            if (!check("bind source map-2 UV attribute")) return false;
            constexpr U properties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
            const I expected[] = {1, 2, static_cast<I>(sizeof(std::array<float, 2>)),
                static_cast<I>(floatType), 0, static_cast<I>(target.buffers[bank]), 0};
            for (U p = 0; p < 7; ++p) {
                I actual = 0; gl.getVertexAttribiv(attribute, properties[p], &actual);
                if (!check("verify source map-2 UV layout")) return false;
                if (actual != expected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout, "map-2 UV layout differs");
            }
            void* offset = nullptr; gl.getVertexAttribPointerv(attribute, 0x8645, &offset);
            if (!check("verify source map-2 UV offset")) return false;
            if (offset) return fail(*diagnostic, VehicleWindowError50DS::layout, "map-2 UV offset differs");
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::createRacerSkin50LA(const game::VehicleDrawMesh50CM* const* meshes,
        VehicleGlResources50CV& resources, const Material* materials, const U* materialOffsets,
        const SpecularGeometry50LA* specular, RacerSkin50LA* skins) noexcept {
        // Cold only. A component that cannot satisfy every admission keeps the
        // CPU vertex path; nothing here changes the CPU path's own buffers.
        // Shared by the player pod (archetype 0) and every opponent archetype.
        for (U c = 0; c < resources.componentCount; ++c) {
            auto& skin = skins[c];
            skin = {};
            const auto* mesh = meshes[c];
            if (!mesh || !mesh->localVertices || !mesh->vertexTemplate || !mesh->semanticScene ||
                !mesh->vertexCount || mesh->vertexCount > game::maxRigidVertices49S ||
                !mesh->sourceNodeCount || mesh->sourceNodeCount > game::maxRigidPalette49S ||
                mesh->spanCount > game::maxVehicleDrawSpans50CM) continue;
            if (c < sourceBodyCount50LA && specular[c].singleDiffuse) continue;
            const auto& scene = *mesh->semanticScene;
            if (scene.lods.empty() || scene.lods[0].surfaces.size() != mesh->spanCount) continue;
            const double q = scene.quantization[1];
            if (!std::isfinite(q) || q <= 0.0) continue;
            std::vector<VehicleGlSkinVertex50LA> vertices(mesh->vertexCount);
            bool admitted = true;
            U vertex = 0;
            for (const auto& surface : scene.lods[0].surfaces) {
                for (const auto& source : surface.vertices) {
                    if (vertex >= mesh->vertexCount) { admitted = false; break; }
                    const auto& local = mesh->localVertices[vertex];
                    if (local.paletteIndex != source.bone || local.paletteIndex >= mesh->sourceNodeCount) {
                        admitted = false; break;
                    }
                    game::original_racer_lighting50KM::CompressedNormal normal{};
                    if (mesh->compressedNormals50LA && mesh->compressedNormalCount50LA == mesh->vertexCount &&
                        mesh->compressedNormalScene50LA == &scene && mesh->compressedNormalQ50LA == q) {
                        if (!mesh->compressedNormals50LA[vertex].valid) { admitted = false; break; }
                        normal = mesh->compressedNormals50LA[vertex].value;
                    } else if (!game::vehicle_draw_detail50LA::decodeCompressedNormal(source.normal_local, q, normal)) {
                        admitted = false; break;
                    }
                    auto& out = vertices[vertex];
                    for (U axis = 0; axis < 3; ++axis) {
                        out.position[axis] = static_cast<float>(local.positionLocal[axis]);
                        out.compressedNormal[axis] = static_cast<float>(normal[axis]);
                        out.localNormal[axis] = static_cast<float>(source.normal_local[axis]);
                        if (!std::isfinite(out.position[axis]) || !std::isfinite(out.localNormal[axis])) admitted = false;
                    }
                    out.bone = static_cast<float>(local.paletteIndex);
                    std::memcpy(out.uv, mesh->vertexTemplate[vertex].uv, sizeof(out.uv));
                    std::memcpy(out.color, mesh->vertexTemplate[vertex].color, sizeof(out.color));
                    ++vertex;
                }
                if (!admitted) break;
            }
            if (!admitted || vertex != mesh->vertexCount) continue;
            // Per-bone opaque-span statistics (formerly the retired native
            // footprint shadow input); retained unchanged, not read for shadows.
            for (U span = 0; span < mesh->spanCount; ++span) {
                const auto& range = mesh->spans[span];
                bool casts = true;
                if (request.mode == VehicleWindowMode50DS::callerMaterialTextures && materials) {
                    const auto& policy = materials[materialOffsets[c] + range.material_index].policy;
                    casts = policy.blend == VehicleWindowBlend50DS::opaque && policy.depthWrite;
                }
                std::uint8_t maximumAlpha = 0;
                for (U n = range.vertex_start; n < range.vertex_start + range.vertex_count && n < mesh->vertexCount; ++n) {
                    maximumAlpha = std::max(maximumAlpha, mesh->vertexTemplate[n].color[3]);
                    if (!casts) continue;
                    const U bone = mesh->localVertices[n].paletteIndex;
                    const double p[3] = {mesh->localVertices[n].positionLocal[0],
                        mesh->localVertices[n].positionLocal[1], mesh->localVertices[n].positionLocal[2]};
                    auto& b = skin.bounds[bone];
                    for (U axis = 0; axis < 3; ++axis) {
                        const float value = static_cast<float>(p[axis]);
                        if (!skin.count[bone] || value < b[axis]) b[axis] = value;
                        if (!skin.count[bone] || value > b[axis + 3]) b[axis + 3] = value;
                    }
                    skin.count[bone] += 1.0;
                    for (U axis = 0; axis < 3; ++axis) skin.sum[bone][axis] += p[axis];
                    skin.square[bone][0] += p[0] * p[0]; skin.square[bone][1] += p[0] * p[1];
                    skin.square[bone][2] += p[0] * p[2]; skin.square[bone][3] += p[1] * p[1];
                    skin.square[bone][4] += p[1] * p[2]; skin.square[bone][5] += p[2] * p[2];
                }
                skin.templateAlphaAdmitted[span] = maximumAlpha <= 64 ? 1 : 0;
            }
            gl.genBuffers(1, &skin.buffer);
            drawGl.bindBuffer(arrayBuffer, skin.buffer);
            gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(vertices.size() * sizeof(VehicleGlSkinVertex50LA)),
                vertices.data(), staticDraw);
            if (!check("create static racer skin vertex buffer") || !skin.buffer)
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "racer skin vertex buffer");
            for (auto& bank : resources.bank) {
                drawGl.bindVertexArray(bank.vertexArray[c]);
                drawGl.bindBuffer(arrayBuffer, skin.buffer);
                constexpr I stride = sizeof(VehicleGlSkinVertex50LA);
                const struct { U index; I size; U type; std::uint8_t normalized; std::size_t offset; } attributes[] = {
                    {vehicleGlSkinPositionAttribute50LA, 3, floatType, 0, offsetof(VehicleGlSkinVertex50LA, position)},
                    {vehicleGlSkinBoneAttribute50LA, 1, floatType, 0, offsetof(VehicleGlSkinVertex50LA, bone)},
                    {vehicleGlSkinNormalAttribute50LA, 3, floatType, 0, offsetof(VehicleGlSkinVertex50LA, compressedNormal)},
                    {vehicleGlSkinLocalNormalAttribute50LA, 3, floatType, 0, offsetof(VehicleGlSkinVertex50LA, localNormal)},
                    {vehicleGlSkinUvAttribute50LA, 2, floatType, 0, offsetof(VehicleGlSkinVertex50LA, uv)},
                    {vehicleGlSkinColorAttribute50LA, 4, 0x1401, 1, offsetof(VehicleGlSkinVertex50LA, color)},
                };
                for (const auto& attribute : attributes) {
                    gl.enableVertexAttribArray(attribute.index);
                    gl.vertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized,
                        stride, reinterpret_cast<const void*>(attribute.offset));
                }
                if (!check("bind racer skin attributes")) return false;
            }
            drawGl.bindVertexArray(0);
            skin.boneCount = mesh->sourceNodeCount;
            skin.eligible = true;
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::createRacerNormals(const VehicleGlResources50CV& geometry,
        U (&buffers)[2][maxComponents]) noexcept {
        // Keep the compatibility mesh and all UV/color bytes unchanged. Every
        // private VAO gets a bounded independent normal stream, initially zero.
        static constexpr assets::WorldNormal50HK absent[game::maxRigidVertices49S]{};
        for (U bank = 0; bank < 2; ++bank) for (U c = 0; c < geometry.componentCount; ++c) {
            VehicleWindowTrack50DS source{};
            source.sourceNormals = absent;
            source.sourceNormalCount = geometry.bank[bank].vertexCapacity[c];
            if (!createWorldNormals(source, buffers[bank][c],
                geometry.bank[bank].vertexArray[c], dynamicDraw)) return false;
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::createTrackProjection(const VehicleWindowTrack50DS& track) noexcept {
        if(track.rcCrystal)return createRcCrystalProjection(track);
        if (!track.sourceProjectionUvs) return true;
        const std::size_t bytes = std::size_t(track.sourceProjectionCount) * sizeof(assets::TrackProjectedUv);
        gl.genBuffers(1, &trackProjectionBuffer);
        drawGl.bindBuffer(arrayBuffer, trackProjectionBuffer);
        gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), track.sourceProjectionUvs, staticDraw);
        if (!check("upload authenticated track source projection UVs")) return false;
        if (!trackProjectionBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero source projection UV buffer name");
        if (!verifyBuffer(arrayBuffer, bytes, staticDraw)) return false;
        std::uint8_t scratch[1024];
        const auto* source = reinterpret_cast<const std::uint8_t*>(track.sourceProjectionUvs);
        for (std::size_t offset = 0; offset < bytes;) {
            const auto count = std::min(sizeof(scratch), bytes - offset);
            gl.getBufferSubData(arrayBuffer, static_cast<std::intptr_t>(offset),
                static_cast<std::intptr_t>(count), scratch);
            if (!check("verify authenticated track projection UV bytes")) return false;
            if (std::memcmp(scratch, source + offset, count))
                return fail(*diagnostic, VehicleWindowError50DS::assets, "GPU track source projection UV mismatch");
            offset += count;
        }
        drawGl.bindVertexArray(trackVertexArray);
        gl.enableVertexAttribArray(4);
        gl.vertexAttribPointer(4, 4, floatType, 0, sizeof(assets::TrackProjectedUv), nullptr);
        if (!check("bind authenticated track projection UV attribute")) return false;
        constexpr U properties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
        const I expected[] = {1, 4, sizeof(assets::TrackProjectedUv), static_cast<I>(floatType), 0,
            static_cast<I>(trackProjectionBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(4, properties[p], &value);
            if (!check("verify authenticated track projection UV attribute")) return false;
            if (value != expected[p])
                return fail(*diagnostic, VehicleWindowError50DS::layout, "source projection UV attribute layout mismatch");
        }
        void* pointer = nullptr; gl.getVertexAttribPointerv(4, 0x8645, &pointer);
        if (!check("verify authenticated projection UV offset")) return false;
        return pointer == nullptr || fail(*diagnostic, VehicleWindowError50DS::layout, "source projection UV attribute offset mismatch");
    }

bool VehicleWindowGl50DS::Storage::createDynamicWorldGeometry(const VehicleWindowTrack50DS& world) noexcept {
        if (!world.mesh) return true;
        dynamicWorldVertexCount = static_cast<U>(world.mesh->vertices.size());
        dynamicWorldNormalCount = world.sourceNormalCount;
        dynamicWorldSourceVertices.reset(new (std::nothrow) assets::Vertex48B[dynamicWorldVertexCount]);
        if (!dynamicWorldSourceVertices)
            return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded dynamic-world source UV/color provenance");
        std::memcpy(dynamicWorldSourceVertices.get(), world.mesh->vertices.data(),
            std::size_t(dynamicWorldVertexCount) * sizeof(assets::Vertex48B));
        for (U b = 0; b < 2; ++b)
            if (!createStaticGeometry(world.mesh, dynamicWorldVertexBuffers[b], dynamicWorldIndexBuffer,
                    dynamicWorldArrays[b], dynamicWorldIndexCount, dynamicDraw, b != 0) ||
                !createWorldNormals(world, dynamicWorldNormalBuffers[b], dynamicWorldArrays[b], dynamicDraw))
                return false;
        return true;
    }

bool VehicleWindowGl50DS::Storage::createCableGeometry() noexcept {
        bool needed = request.cables.textures != nullptr;
        for (U i = 0; i < racerArchetypeCount; ++i)
            needed = needed || racerArchetypes[i].cables.textures;
        if (!needed) return true;
        constexpr std::size_t vertexBytes = sizeof(game::PodCableMesh50FD::vertices);
        constexpr std::size_t indexBytes = sizeof(game::PodCableMesh50FD::indices);
        U indices[game::maxPodCableIndexCount50FD];
        for (U i = 0; i < game::maxPodCableIndexCount50FD; ++i) indices[i] = cableIndex50DS(i);
        gl.genBuffers(1, &cableVertexBuffer);
        drawGl.bindBuffer(arrayBuffer, cableVertexBuffer);
        gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(vertexBytes), nullptr, dynamicDraw);
        if (!check("allocate fixed dynamic cable vertices")) return false;
        if (!cableVertexBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero cable vertex buffer");
        if (!verifyBuffer(arrayBuffer, vertexBytes, dynamicDraw)) return false;
        gl.genBuffers(1, &cableIndexBuffer);
        drawGl.bindBuffer(arrayBuffer, cableIndexBuffer);
        gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(indexBytes), indices, staticDraw);
        if (!check("upload canonical source cable indices")) return false;
        if (!cableIndexBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero cable index buffer");
        if (!verifyBuffer(arrayBuffer, indexBytes, staticDraw)) return false;
        U uploaded[game::maxPodCableIndexCount50FD];
        gl.getBufferSubData(arrayBuffer, 0, static_cast<std::intptr_t>(indexBytes), uploaded);
        if (!check("verify canonical cable index bytes")) return false;
        if (std::memcmp(indices, uploaded, indexBytes))
            return fail(*diagnostic, VehicleWindowError50DS::indices, "GPU cable index contents mismatch");
        gl.genVertexArrays(1, &cableVertexArray);
        drawGl.bindVertexArray(cableVertexArray);
        drawGl.bindBuffer(arrayBuffer, cableVertexBuffer);
        drawGl.bindBuffer(elementBuffer, cableIndexBuffer);
        constexpr I sizes[] = {3, 2, 4};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        for (U a = 0; a < 3; ++a) {
            gl.enableVertexAttribArray(a);
            gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
        }
        if (!check("create fixed cable Vertex48B layout")) return false;
        if (!cableVertexArray)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero cable vertex array");
        return verifyLayout(cableVertexBuffer, cableIndexBuffer);
    }

bool VehicleWindowGl50DS::Storage::createAttachment(U& name, U format, U attachment) noexcept {
        gl.genRenderbuffers(1, &name);
        gl.bindRenderbuffer(renderbuffer, name);
        gl.renderbufferStorage(renderbuffer, format, maximumWidth, maximumHeight);
        if (!check("allocate private staging renderbuffer")) return false;
        if (!name) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero renderbuffer name");
        I actualWidth = 0, actualHeight = 0, actualFormat = 0, actualSamples = 0;
        gl.getRenderbufferParameteriv(renderbuffer, 0x8D42, &actualWidth);
        gl.getRenderbufferParameteriv(renderbuffer, 0x8D43, &actualHeight);
        gl.getRenderbufferParameteriv(renderbuffer, 0x8D44, &actualFormat);
        gl.getRenderbufferParameteriv(renderbuffer, 0x8CAB, &actualSamples);
        if (!check("verify private staging renderbuffer")) return false;
        if (actualWidth != maximumWidth || actualHeight != maximumHeight || actualFormat != static_cast<I>(format) || actualSamples)
            return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "renderbuffer storage mismatch");
        gl.framebufferRenderbuffer(framebuffer, attachment, renderbuffer, name);
        return check("attach private staging renderbuffer");
    }

bool VehicleWindowGl50DS::Storage::createSpecularColorAttachment50LA(U& name) noexcept {
        // Gameplay banks keep their existing transaction/depth owners. Only
        // authenticated body-pair capability changes color storage to RGBA8
        // textures; UI targets, receipts and source-alpha owners are unrelated.
        I maximum = 0; gl.getIntegerv(0x0D33, &maximum);
        if (!check("query gameplay pair texture dimensions")) return false;
        if (maximumWidth > maximum || maximumHeight > maximum)
            return fail(*diagnostic, VehicleWindowError50DS::limits, "gameplay pair texture exceeds context limit");
        gl.genTextures(1, &name); gl.activeTexture(textureUnit0); gl.bindTexture(texture2D, name);
        if (gl.bindSampler) gl.bindSampler(0, 0);
        gl.texParameteri(texture2D, 0x2801, 0x2600); gl.texParameteri(texture2D, 0x2800, 0x2600);
        gl.texParameteri(texture2D, 0x2802, 0x812F); gl.texParameteri(texture2D, 0x2803, 0x812F);
        gl.texParameteri(texture2D, 0x813C, 0); gl.texParameteri(texture2D, 0x813D, 0);
        drawGl.bindBuffer(0x88EC, 0); // No inherited unpack-PBO offset.
        gl.texImage2D(texture2D, 0, rgba8, maximumWidth, maximumHeight, 0, rgba, unsignedByte, nullptr);
        if (!check("allocate private gameplay source-pair RGBA8 color") || !name)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "gameplay source-pair texture allocation failed");
        I w = 0, h = 0, format = 0;
        gl.getTexLevelParameteriv(texture2D, 0, 0x1000, &w); gl.getTexLevelParameteriv(texture2D, 0, 0x1001, &h);
        gl.getTexLevelParameteriv(texture2D, 0, 0x1003, &format);
        if (!check("verify private gameplay source-pair texture")) return false;
        if (w != maximumWidth || h != maximumHeight || format != static_cast<I>(rgba8))
            return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "gameplay source-pair texture storage mismatch");
        gl.framebufferTexture2D(framebuffer, colorAttachment, texture2D, name, 0);
        gl.bindTexture(texture2D, 0);
        return check("attach private gameplay source-pair color");
    }

bool VehicleWindowGl50DS::Storage::createTargets() noexcept {
        I maximum = 0, viewportLimits[2]{};
        gl.getIntegerv(0x84E8, &maximum); // GL_MAX_RENDERBUFFER_SIZE
        gl.getIntegerv(0x0D3A, viewportLimits); // GL_MAX_VIEWPORT_DIMS
        if (!check("query target limits")) return false;
        if (maximumWidth > maximum || maximumHeight > maximum ||
            maximumWidth > viewportLimits[0] || maximumHeight > viewportLimits[1] ||
            (request.enableOriginalUiSourceRgba && (maximum<640 || viewportLimits[0]<640 || viewportLimits[1]<448)))
            return fail(*diagnostic, VehicleWindowError50DS::limits, "explicit resolution exceeds GL limits");
        for (U b = 0; b < 2; ++b) {
            if(!frontendOnly){
            gl.genFramebuffers(1, &targets[b]);
            gl.bindFramebuffer(framebuffer, targets[b]);
            if (!check("create unpublished framebuffer")) return false;
            if (!targets[b]) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero framebuffer name");
            if (!(hasSpecularPairs50LA ? createSpecularColorAttachment50LA(sourcePairColors50LA[b]) :
                    createAttachment(colors[b], rgba8, colorAttachment)) ||
                !createAttachment(depths[b], depth24, depthAttachment)) return false;
            gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
            const U status = gl.checkFramebufferStatus(framebuffer);
            if (!check("check unpublished framebuffer")) return false;
            if (status != framebufferComplete) {
                diagnostic->framebufferStatus = status;
                return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "incomplete unpublished framebuffer");
            }
            }
            if (request.enableOriginalUi) {
                gl.genFramebuffers(1, &uiTargets[b]);
                gl.bindFramebuffer(framebuffer, uiTargets[b]);
                if (!check("create private original UI framebuffer")) return false;
                if (!uiTargets[b]) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI framebuffer");
                if (!createAttachment(uiColors[b], rgba8, colorAttachment) ||
                    !createAttachment(uiDepths[b], depth24, depthAttachment)) return false;
                gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
                const U uiStatus = gl.checkFramebufferStatus(framebuffer);
                if (!check("check private original UI framebuffer")) return false;
                if (uiStatus != framebufferComplete) {
                    diagnostic->framebufferStatus = uiStatus;
                    return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "incomplete original UI framebuffer");
                }
            }
            if (request.enableHostHud) {
                gl.genFramebuffers(1, &menuTargets[b]);
                gl.bindFramebuffer(framebuffer, menuTargets[b]);
                if (!check("create private host menu framebuffer")) return false;
                if (!menuTargets[b])
                    return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero host menu framebuffer");
                if (!createAttachment(menuColors[b], rgba8, colorAttachment)) return false;
                gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
                const U menuStatus = gl.checkFramebufferStatus(framebuffer);
                if (!check("check private host menu framebuffer")) return false;
                if (menuStatus != framebufferComplete) {
                    diagnostic->framebufferStatus = menuStatus;
                    return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "incomplete host menu framebuffer");
                }
            }
        }
        return createOriginalUiSourceTarget();
    }

bool VehicleWindowGl50DS::Storage::checkDefaultFramebuffer() noexcept {
        gl.bindFramebuffer(framebuffer, 0);
        gl.drawBuffer(backBuffer); gl.readBuffer(backBuffer);
        const U status = gl.checkFramebufferStatus(framebuffer);
        if (!check("select default window back buffer")) return false;
        if (status != framebufferComplete) {
            diagnostic->framebufferStatus = status;
            return fail(*diagnostic, VehicleWindowError50DS::framebuffer,
                "default window framebuffer is incomplete");
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::checkTextureSampling(I major, I minor) noexcept {
        if (!usesTextures()) return true;
        I profile = 0, imageUnits = 0;
        gl.getIntegerv(0x9126, &profile); // GL_CONTEXT_PROFILE_MASK
        gl.getIntegerv(0x8872, &imageUnits); // GL_MAX_TEXTURE_IMAGE_UNITS
        if (!check("query caller-material texture support")) return false;
        if (!(profile & 1) || imageUnits < 1)
            return fail(*diagnostic, VehicleWindowError50DS::context,
                "caller material textures require desktop core GL and texture unit zero");
        bool samplerObjects = major > 3 || (major == 3 && minor >= 3);
        if (!samplerObjects) {
            // GL 3.2 has no core sampler objects; its optional ARB extension
            // does. A nonnull resolver result alone is not capability evidence.
            if (!load(gl.getStringi, "glGetStringi")) return false;
            I count = 0;
            gl.getIntegerv(0x821D, &count); // GL_NUM_EXTENSIONS
            if (!check("query GL 3.2 sampler extension count")) return false;
            if (count < 0 || count > 65536)
                return fail(*diagnostic, VehicleWindowError50DS::limits, "invalid bounded GL extension count");
            for (I i = 0; i < count; ++i) {
                const auto* extension = gl.getStringi(0x1F03, static_cast<U>(i));
                if (!check("query GL 3.2 sampler extension")) return false;
                if (!extension)
                    return fail(*diagnostic, VehicleWindowError50DS::context, "null GL extension name");
                if (!std::strcmp(reinterpret_cast<const char*>(extension), "GL_ARB_sampler_objects")) {
                    samplerObjects = true;
                    break;
                }
            }
        }
        return !samplerObjects || load(gl.bindSampler, "glBindSampler");
    }

bool VehicleWindowGl50DS::Storage::checkSpecularPairSupport50LA(I major, I minor) noexcept {
        if (!hasSpecularPairs50LA) return true;
        I units = 0; gl.getIntegerv(0x8872, &units);
        if (!check("query body-pair fragment texture units")) return false;
        if (units < 2) return fail(*diagnostic, VehicleWindowError50DS::context, "body pairs need two fragment texture units");
        bool coreOrArb = major > 4 || (major == 4 && minor >= 5), nv = false;
        if (!load(gl.getStringi, "glGetStringi")) return false;
        I count = 0; gl.getIntegerv(0x821D, &count);
        if (!check("query body-pair texture-barrier extensions")) return false;
        if (count < 0 || count > 65536)
            return fail(*diagnostic, VehicleWindowError50DS::limits, "invalid bounded body-pair extension count");
        for (I i = 0; i < count; ++i) {
            const auto* extension = gl.getStringi(0x1F03, static_cast<U>(i));
            if (!check("read body-pair texture-barrier capability")) return false;
            if (!extension) return fail(*diagnostic, VehicleWindowError50DS::context, "null body-pair extension name");
            const auto* name = reinterpret_cast<const char*>(extension);
            if (!std::strcmp(name, "GL_ARB_texture_barrier")) coreOrArb = true;
            if (!std::strcmp(name, "GL_NV_texture_barrier")) nv = true;
        }
        if (!coreOrArb && !nv)
            return fail(*diagnostic, VehicleWindowError50DS::context, "body pairs require GL4.5, ARB or NV texture barrier");
        return load(gl.textureBarrier, coreOrArb ? "glTextureBarrier" : "glTextureBarrierNV");
    }

bool VehicleWindowGl50DS::Storage::checkContext() noexcept {
        const auto* version = gl.getString(0x1F02); // GL_VERSION
        I major = 0, minor = 0;
        gl.getIntegerv(0x821B, &major); gl.getIntegerv(0x821C, &minor);
        if (!check("query desktop GL version")) return false;
        if (!version || std::strncmp(reinterpret_cast<const char*>(version), "OpenGL ES", 9) == 0 ||
            major < 3 || (major == 3 && minor < 2))
            return fail(*diagnostic, VehicleWindowError50DS::context, "desktop OpenGL 3.2 or newer required");
        if (!checkCoverageSupport(major, minor)) return false;
        if (!checkTextureSampling(major, minor) || !checkOriginalUiSourceSupport(major,minor) ||
            !checkSpecularPairSupport50LA(major, minor)) return false;
        if (request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
            if (!checkDefaultFramebuffer()) return false;
            I doubleBuffered = 0, stereo = 0, sampleBuffers = 0, samples = 0;
            gl.getIntegerv(0x0C32, &doubleBuffered); // GL_DOUBLEBUFFER
            gl.getIntegerv(0x0C33, &stereo); // GL_STEREO
            gl.getIntegerv(0x80A8, &sampleBuffers); gl.getIntegerv(0x80A9, &samples);
            if (!check("query default window framebuffer format")) return false;
            if (!doubleBuffered || stereo || sampleBuffers || samples)
                return fail(*diagnostic, VehicleWindowError50DS::context,
                    "nonstereo double-buffered nonmultisampled window required");
        }
        gl.getIntegerv(0x0D32, &clipDistances); // GL_MAX_CLIP_DISTANCES
        if (!check("query GL clip-distance limit")) return false;
        // GL 3.2 guarantees at least eight. Keep state work bounded even if a
        // broken resolver or driver returns an implausible capability count.
        if (clipDistances < 8 || clipDistances > 64)
            return fail(*diagnostic, VehicleWindowError50DS::limits, "invalid GL clip-distance limit");
        return true;
    }

bool VehicleWindowGl50DS::Storage::createRacerArchetypes(const VehicleWindowRequest50DS& input) noexcept {
        racerArchetypeCount = input.racerArchetypeCount;
        opponentMaterialStride = materialCount;
        for (U a = 0; a < racerArchetypeCount; ++a) {
            const auto& source = input.racerArchetypes[a];
            auto& target = racerArchetypes[a];
            target.resources.componentCount = source.assets->componentCount();
            target.textures = source.materials.textures;
            target.texturePublication = target.textures ? target.textures->publicationToken() : 0;
            target.specularTextures50LA = source.materials.specularTextures50LA;
            target.specularPublication50LA = target.specularTextures50LA ? target.specularTextures50LA->publicationToken() : 0;
            target.cables = source.cables;
            target.cableTexturePublication = target.cables.textures ?
                target.cables.textures->publicationToken() : 0;
            target.cableTextureName = target.cables.textures ?
                target.cables.textures->texture(target.cables.textureIndex) : 0;
            if (target.cables.textures && !target.cableTextureName)
                return fail(*diagnostic, VehicleWindowError50DS::material,
                    "required racer cable texture disappeared during create");
            for (U c = 0; c < target.resources.componentCount; ++c) {
                diagnostic->component = c;
                const auto* mesh = target.meshes[c] = source.assets->preparedMesh(components[c]);
                const auto* scene = target.scenes[c] = source.assets->scene(components[c]);
                if (!mesh || !scene || !mesh->indices || !mesh->spans ||
                    !mesh->vertexCount || !mesh->indexCount || !mesh->spanCount ||
                    mesh->vertexCount > game::maxRigidVertices49S ||
                    mesh->indexCount > assets::max_mesh_indices ||
                    mesh->spanCount > game::maxVehicleDrawSpans50CM ||
                    std::uint64_t(mesh->vertexCount) * sizeof(assets::Vertex48B) >
                        std::uint64_t(std::numeric_limits<std::intptr_t>::max()) ||
                    std::uint64_t(mesh->indexCount) * sizeof(U) >
                        std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
                    return fail(*diagnostic, VehicleWindowError50DS::assets,
                        "invalid prepared racer 50CR mesh");
                for (U i = 0; i < mesh->spanCount; ++i)
                    if (mesh->spans[i].material_index >= scene->materials.size())
                        return fail(*diagnostic, VehicleWindowError50DS::assets,
                            "racer original material index outside loaded scene");
                target.materialOffsets[c] = target.materialCount;
                target.materialCount += source.materials.component[c].count;
            }
            if (request.mode != VehicleWindowMode50DS::callerMaterialTextures) continue;
            target.materials.reset(new (std::nothrow) Material[target.materialCount]);
            if (!target.materials)
                return fail(*diagnostic, VehicleWindowError50DS::allocation,
                    "allocate bounded cold racer material table");
            for (U c = 0; c < target.resources.componentCount; ++c)
                for (U m = 0; m < source.materials.component[c].count; ++m) {
                    const auto& binding = source.materials.component[c].bindings[m];
                    auto& material = target.materials[target.materialOffsets[c] + m];
                    material.policy = binding;
                    target.materialSummary[c].sourceDiffuse |= binding.sourceDiffuseCompressed50LA;
                    target.materialSummary[c].sourcePair |= binding.sourceSpecularPair50LA;
                    target.materialSummary[c].normalDependent |= podMaterialUsesNormals(binding);
                    target.materialSummary[c].hasDeferredEffects |= binding.blend != VehicleWindowBlend50DS::opaque;
                    material.textureIndex = binding.textureIndex;
                    material.name = binding.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                        target.textures->texture(binding.textureIndex) : 0;
                    if (binding.sourceSpecularPair50LA) {
                        const auto* base = target.textures->asset(binding.textureIndex);
                        const auto* map2 = target.specularTextures50LA->asset(binding.specularTextureIndex50LA);
                        if (!base || !base->name || !map2 || !map2->name)
                            return fail(*diagnostic, VehicleWindowError50DS::material, "racer source pair disappeared during create");
                        material.textureIdentity50LA = base->identity;
                        material.specularName50LA = map2->name; material.specularIdentity50LA = map2->identity;
                        hasSpecularPairs50LA = true;
                    }
                    if (binding.extraPass50PP != VehicleWindowExtraPass50PP::none) {
                        const auto* extra = target.specularTextures50LA ?
                            target.specularTextures50LA->asset(binding.extraPassTextureIndex50PP) : nullptr;
                        if (!extra || !extra->name)
                            return fail(*diagnostic, VehicleWindowError50DS::material, "racer second-pass texture disappeared during create");
                        material.extraPassName50PP = extra->name; material.extraPassIdentity50PP = extra->identity;
                        target.materialSummary[c].hasDeferredEffects = true;
                        hasExtraPasses50PP = true;
                    }
                    if (binding.mode == VehicleWindowMaterialMode50DS::requiredTexture && !material.name)
                        return fail(*diagnostic, VehicleWindowError50DS::material,
                            "required racer material texture disappeared during create");
                }
            opponentMaterialStride = std::max(opponentMaterialStride, target.materialCount);
        }
        if (opponentMaterialStride) {
            opponentMaterialUvs.reset(new (std::nothrow)
                MaterialUv[opponentMaterialStride * maxVehicleWindowOpponents50DS]);
            if (!opponentMaterialUvs)
                return fail(*diagnostic, VehicleWindowError50DS::allocation,
                    "allocate bounded per-racer UV transform storage");
        }
        diagnostic->component = maxComponents;
        return texturesCurrent(*diagnostic);
    }

bool VehicleWindowGl50DS::Storage::initialize(const VehicleWindowRequest50DS& input,
        const assets::VehicleDrawAssets50CR& owner) noexcept {
        // Capture before the ordinary cold-pointer clearing below. This local
        // is not a publication; every early return leaves the token null.
        const auto* const originalSkyMesh=input.sky.mesh;
        nativeSkyMeshIdentity129280=nullptr;
        request = input;
        // Coverage is currently admitted only for static track spans. Never
        // silently consume a requested dynamic policy without binding it.
        for (U i=0;i<input.dynamicWorld.spanCount;++i)
            if(input.dynamicWorld.spans[i].nativeCoverage!=NativeTrackCoverage50DS::aa0)
                return fail(*diagnostic,VehicleWindowError50DS::material,"dynamic coverage is not admitted");
        resources.componentCount = owner.componentCount();
        if (input.enableOriginalUiSourceRgba && !input.enableOriginalUi)
            return fail(*diagnostic,VehicleWindowError50DS::request,"source UI requires original UI opt-in");
        if (!validateOriginalUiBank(input) || !validateGcParticleBank(input) || !validateRcParticleBank(input) || !validateGbParticleBank(input) || !validateMcParticleBank(input)) return false;
        uiTexturePublication = input.originalUiTextures ? input.originalUiTextures->publicationToken() : 0;
        request.racerArchetypes = nullptr; // Cold descriptors are never retained.
        request.trackAnimations = nullptr;
        // Track CPU bytes are borrowed only for this cold synchronous call.
        // No retained pointer, including after a failed cleanup, is needed.
        request.track.mesh = nullptr;
        request.track.spans = nullptr;
        request.track.sourceNormals = nullptr;
        request.track.sourceProjectionUvs = nullptr;
        request.dynamicWorld.mesh = nullptr;
        request.dynamicWorld.spans = nullptr;
        request.dynamicWorld.sourceNormals = nullptr;
        request.dynamicWorld.sourceProjectionUvs = nullptr;
        request.sky.mesh = nullptr;
        request.sky.spans = nullptr;
        // Mapping arrays are cold-only; retain just the 50EB owner and its
        // publication token, never pointers into caller mapping storage.
        for (auto& component : request.materials.component) component = {};
        texturePublication = input.materials.textures ? input.materials.textures->publicationToken() : 0;
        specularPublication50LA = input.materials.specularTextures50LA ? input.materials.specularTextures50LA->publicationToken() : 0;
        trackTexturePublication = input.track.textures ? input.track.textures->publicationToken() : 0;
        dynamicWorldTexturePublication = input.dynamicWorld.textures ? input.dynamicWorld.textures->publicationToken() : 0;
        trackAnimationTexturePublication = input.trackAnimationTextures ? input.trackAnimationTextures->publicationToken() : 0;
        cableTexturePublication = input.cables.textures ? input.cables.textures->publicationToken() : 0;
        gameplayBinderPublication=input.gameplayBinderTextures?input.gameplayBinderTextures->publicationToken():0;
        skyTexturePublication = input.sky.textures ? input.sky.textures->publicationToken() : 0;
        cableTextureName = input.cables.textures ? input.cables.textures->texture(input.cables.textureIndex) : 0;
        maximumWidth = request.maximumWidth; maximumHeight = request.maximumHeight;
        if (!current(*diagnostic) || !texturesCurrent(*diagnostic)) return false;
        if (input.cables.textures && !cableTextureName)
            return fail(*diagnostic, VehicleWindowError50DS::material, "required cable texture disappeared during create");
        for (U c = 0; c < resources.componentCount; ++c) {
            diagnostic->component = c;
            meshes[c] = owner.preparedMesh(components[c]); scenes[c] = owner.scene(components[c]);
            if (!meshes[c] || !scenes[c] || !meshes[c]->indices || !meshes[c]->spans ||
                !meshes[c]->vertexCount || !meshes[c]->indexCount || !meshes[c]->spanCount ||
                meshes[c]->vertexCount > game::maxRigidVertices49S ||
                meshes[c]->indexCount > assets::max_mesh_indices ||
                meshes[c]->spanCount > game::maxVehicleDrawSpans50CM ||
                std::uint64_t(meshes[c]->vertexCount) * sizeof(assets::Vertex48B) >
                    std::uint64_t(std::numeric_limits<std::intptr_t>::max()) ||
                std::uint64_t(meshes[c]->indexCount) * sizeof(U) >
                    std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
                return fail(*diagnostic, VehicleWindowError50DS::assets, "invalid prepared 50CR mesh");
            for (U i = 0; i < meshes[c]->spanCount; ++i)
                if (meshes[c]->spans[i].material_index >= scenes[c]->materials.size())
                    return fail(*diagnostic, VehicleWindowError50DS::assets,
                        "original material index outside loaded scene");
        }
        diagnostic->component = maxComponents;
        if (!createMaterials(input.materials) || !createRacerArchetypes(input) ||
            !createTrackMaterials(input.track) || !createTrackAnimations(input) ||
            !createDynamicWorldMaterials(input.dynamicWorld) ||
            !createTrackVisibility(input.track) || !createSkyMaterials(input.sky)) return false;
        for (U body = 0; body < sourceBodyCount50LA; ++body)
            if (!createSpecularGroups50LA(0, body, sourceSpecularGeometry50LA[body])) return false;
        for (U i = 0; i < racerArchetypeCount; ++i)
            for (U body = 0; body < sourceBodyCount50LA; ++body)
                if (!createSpecularGroups50LA(i + 1, body, racerArchetypes[i].specularGeometry50LA[body])) return false;
        if (hasSpecularPairs50LA) {
            frameSpecularUvs50LA.reset(new (std::nothrow) std::array<float, 2>[
                (1 + maxVehicleWindowOpponents50DS) * sourceBodyCount50LA * game::maxRigidVertices49S]);
            specularWorkspace50LA.reset(new (std::nothrow) game::OriginalRacerSpecularWorkspace50LA);
            if (!frameSpecularUvs50LA || !specularWorkspace50LA)
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate bounded current-camera specular workspace");
        }
        if(hasSingleDiffuse50LA){
            frameDiffuseFog50LA.reset(new(std::nothrow) float[(1+maxVehicleWindowOpponents50DS)*sourceBodyCount50LA*game::maxRigidVertices49S]);
            diffuseWorkspace50LA.reset(new(std::nothrow) game::original_racer_diffuse50LA::Workspace);
            if(!frameDiffuseFog50LA||!diffuseWorkspace50LA)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate bounded single diffuse fog workspace");
        }
        coverageDemand = coverageDemand || bool(request.raStaticWorld);
        if (!loadFunctions() || !check("initial GL error state") || !checkContext()) return false;
        shader.reset(new (std::nothrow) VehicleGlShader50CZ(shaderGl));
        if (!shader) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate shader owner");
        if (!shader->initialize(diagnostic->shader, coverageDemand && hasSpecularPairs50LA))
            return fail(*diagnostic, VehicleWindowError50DS::shader, "initialize 50CZ viewer shader");
        if (!createTrackShader() || !createGeometry(resources, meshes) ||
            !createRacerNormals(resources, racerNormalBuffers)) return false;
        for (U body = 0; body < sourceBodyCount50LA; ++body)
            if (!createSpecularUvBuffers50LA(0, body, sourceSpecularGeometry50LA[body])) return false;
        for (U i = 0; i < racerArchetypeCount; ++i) {
            if (!createGeometry(racerArchetypes[i].resources, racerArchetypes[i].meshes) ||
                !createRacerNormals(racerArchetypes[i].resources, racerArchetypes[i].normalBuffers)) return false;
            for (U body = 0; body < sourceBodyCount50LA; ++body)
                if (!createSpecularUvBuffers50LA(i + 1, body, racerArchetypes[i].specularGeometry50LA[body])) return false;
            auto& archetype = racerArchetypes[i];
            if (!createRacerSkin50LA(archetype.meshes, archetype.resources, archetype.materials.get(),
                archetype.materialOffsets, archetype.specularGeometry50LA.data(), archetype.skin50LA)) return false;
        }
        if (!createRacerSkin50LA(meshes, resources, materials.get(), materialOffsets,
            sourceSpecularGeometry50LA.data(), playerSkin50LA)) return false;
        if (
            !createStaticGeometry(input.track.mesh, trackVertexBuffer, trackIndexBuffer, trackVertexArray, trackIndexCount) ||
            !createWorldNormals(input.track, trackNormalBuffer, trackVertexArray) ||
            !createDynamicWorldGeometry(input.dynamicWorld) ||
            !createTrackProjection(input.track) ||
            !createStaticGeometry(input.sky.mesh, skyVertexBuffer, skyIndexBuffer, skyVertexArray, skyIndexCount) ||
            !createCableGeometry() || !createHostHud() || !createOriginalUi() ||
            !createTargets() || !createGameplayBlur() || !createFxaa() || !createDebris(input.debris) || !createEffects(input.effects) || !createWater(input.water) || !createCoverageProgram() || !createRaWorld() || !createOriginalShadows()) return false;
        VehicleGlHooks50CV hooks{};
        hooks.context = this; hooks.ready = ready; hooks.materialReady = materialReady;
        if(request.enableGameplayDisplayWallTiming)hooks.timingObserver=observePlayerTiming;
        hooks.begin = begin; hooks.bindMaterial = bindMaterial; hooks.commit = commit; hooks.abort = abort;
        // The player's GPU vertex path binds its per-part inputs in this hook,
        // which otherwise issues the same single indexed draw as 50CV.
        bool playerSkinned = false;
        for (U c = 0; c < resources.componentCount; ++c) playerSkinned = playerSkinned || playerSkin50LA[c].eligible;
        if (hasSpecularPairs50LA || hasSingleDiffuse50LA || playerSkinned) hooks.drawSpan = drawSpan50LA;
        adapter.reset(new (std::nothrow) VehicleGlBackend50CV(drawGl, resources, hooks, published));
        if (!adapter) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate 50CV adapter");
        if (request.output == VehicleWindowOutput50DS::defaultBackBuffer)
            gl.bindFramebuffer(framebuffer, 0);
        if (!check("finish cold renderer creation")) return false;
        initialized = true;
        nativeSkyMeshIdentity129280=originalSkyMesh;
        diagnostic->originalUiCoherentFramebufferFetch=uiSourceCoherentFramebufferFetch;
        return true;
    }

bool VehicleWindowGl50DS::Storage::drainCleanupErrors(VehicleWindowDiagnostic50DS& d) noexcept {
        for (unsigned i = 0; i < 32; ++i) {
            const U error = drawGl.getError();
            if (!error) return true;
            nativeGlFaultObserved50KG=true;
            if (!d.cleanupGlError) d.cleanupGlError = error;
        }
        return false;
    }

bool VehicleWindowGl50DS::Storage::deleteName(U& name, void (*deleter)(I, const U*), VehicleWindowDiagnostic50DS& d) noexcept {
        if (!name) return true;
        deleter(1, &name);
        const U error = drawGl.getError();
        if (!error) { name = 0; return true; }
        nativeGlFaultObserved50KG=true;
            if (!d.cleanupGlError) d.cleanupGlError = error;
        // Retain the name conservatively until deletion is acknowledged. The
        // host may not create/reuse GL objects between unsuccessful close calls.
        return false;
    }

bool VehicleWindowGl50DS::Storage::deleteShaderName(U& name, bool program, VehicleWindowDiagnostic50DS& d) noexcept {
        if (!name) return true;
        if (program) shaderGl.deleteProgram(name);
        else shaderGl.deleteShader(name);
        const U error = drawGl.getError();
        if (!error) { name = 0; return true; }
        nativeGlFaultObserved50KG=true;
            if (!d.cleanupGlError) d.cleanupGlError = error;
        return false;
    }

bool VehicleWindowGl50DS::Storage::cleanup(VehicleWindowDiagnostic50DS& d) noexcept {
        if(thread==std::this_thread::get_id()&&nativeVideoInvocation50LA){d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::cleanup,"native packet invocation must retire before renderer cleanup");}

        nativeSkyMeshIdentity129280=nullptr; // Even retained failed cleanup cannot certify old upload.
        if(frameNativeResults||nativeResultsFrame||nativeResultsRenderer){d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::cleanup,"Results native Frame borrow must end before cleanup");}
        if(frameNativeF8Bars||nativeF8BarFrame||nativeF8BarRenderer){d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::cleanup,"F8 synchronous native frame borrow must end before cleanup");}
        if(menuRasterShared50KG){d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::cleanup,"native menu raster requires actual SourceOwner retirement before cleanup");}
        if(garagePreludeOwner50KG){d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::cleanup,"Garage source-target prelude must retire its actual owner before renderer cleanup");}
        if (thread != std::this_thread::get_id()) {
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::wrongThread, "cleanup requires creating thread");
        }
        if(rcParticleWindow && (rcParticleWindow->material_->active || active || frameSet || diagnostic)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"RC renderer close requires ended synchronous draw/material borrowers");
        }
        if(!cancelRcParticleAttempt()){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"RC pending draw attempt could not retire before cleanup");
        }
        if(rcParticleWindow&&!rcParticleWindow->invalidateRenderPublication(rcParticleRenderer)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"RC prior particle completion could not invalidate");
        }
        if(gbParticleWindow && (gbParticleWindow->material_->active || active || frameSet || diagnostic)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"GB renderer close requires ended synchronous draw/material borrowers");
        }
        if(!cancelGbParticleAttempt()){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"GB pending draw attempt could not retire before cleanup");
        }
        if(gbParticleWindow&&!gbParticleWindow->invalidateRenderPublication(gbParticleRenderer)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"GB prior particle completion could not invalidate");
        }
        if(mcParticleWindow && (mcParticleWindow->material_->active || active || frameSet || diagnostic)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"MC renderer close requires ended synchronous draw/material borrowers");
        }
        if(!cancelMcParticleAttempt()){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"MC pending draw attempt could not retire before cleanup");
        }
        if(mcParticleWindow&&!mcParticleWindow->invalidateRenderPublication(mcParticleRenderer)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"MC prior particle completion could not invalidate");
        }
        if (hasGlResources() && !current(d)) { d.cleanupIncomplete = true; return false; }
        if(raStaticFireOwner)raStaticFireOwner->nativeCompletionLive_=false;
        initialized = false; active = frameSet = imageCommitted = false;
        // Invalidate native observations, but retain the exact sync for cleanup.
        nativeVideoFinishState50LA=NativeVideoPacketFinishState50LA::cancelled;
        nativeVideoPacketReady50LA=nativeVideoPacketImage50LA=false;nativeVideoPacketSerial50LA=0;
        nativeVideoSnapshotReady50LA=nativeVideoSnapshotImage50LA=false;
        nativeVideoSnapshotSerial50LA=nativeVideoSnapshotImageSerial50LA=nativeVideoSnapshotClearSerial50LA=0;
        nativeVideoPacketClearAncestor50LA=0;nativeVideoPacketLastWrite50LA=NativeVideoPacketWrite50LA::none;
        nativeVideoOverlayReady50LA=false;nativeVideoOverlayImageSerial50LA=nativeVideoOverlayCaptureSerial50LA=0;
        frameCables = nullptr; frameHostHud = false;frameGcParticles=nullptr;
        committedWidth = committedHeight = raceWidth = raceHeight = 0;
        retainedRaceDisplayCompleted=retainedRaceContainsPause=false;
        pauseDisplayPublication={};
        pendingRaceCameraPublication=retainedRaceCameraPublication=0;
        displayedTarget = 0;
        trackVisibilityPendingTarget=0;trackVisibilityCompletedValid=false;
        gameplayHistoryValid=gameplayHistoryStaged=frameGameplayBlurEnabled=false;
        gameplayAwaitingTarget=0;frameGameplayBlur={};
        game::clearGameplayBlurVideoStrengths(gameplayBlurVideo);
        uiPrivatePublicationGeneration=0;
        adapter.reset();
        if (!hasGlResources()) { shader.reset(); retireRaWorldPublication(); retireRaStaticFirePublication(); retireRcParticlePublication();retireGbParticlePublication();retireMcParticlePublication(); d.cleanupIncomplete = false; return true; }
        // All functions were loaded before the first name could be allocated.
        if (!drainCleanupErrors(d)) {
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::cleanup, "GL error state will not clear for cleanup");
        }
        // The packet marker is independent of Garage and survives all packet
        // metadata resets above. Cancellation never reports GPU completion.
        if(!retireNativeVideoPacketFinish50LA(d))return false;
        // A failed nonblocking Garage observation can retain its one native
        // fence. Query actual ownership before deletion, including a prior
        // delete whose diagnostic outcome was uncertain. No replayed draw.
        if(garagePollFence50KG){
            if(!gl.isSync||!gl.deleteSync){d.cleanupIncomplete=true;
                return fail(d,VehicleWindowError50DS::cleanup,"Garage poll fence lacks its exact GL release owner");}
            const auto alive=gl.isSync(garagePollFence50KG);
            U error=drawGl.getError();
            if(error){d.cleanupIncomplete=true;d.cleanupGlError=error;return false;}
            if(alive){gl.deleteSync(garagePollFence50KG);error=drawGl.getError();
                if(error){d.cleanupIncomplete=true;d.cleanupGlError=error;return false;}}
            garagePollFence50KG=nullptr;
        }
        // The last Garage target's completion marker: the same wait the former
        // completion glFinish performed, then its release.
        if(garageTargetFence50KG){
            waitGarageTargetFence50KG();
            const U error=drawGl.getError();
            if(error){d.cleanupIncomplete=true;d.cleanupGlError=error;return false;}
        }
        // Explicit source/display leases add a real completion obligation. Never
        // confuse a CPU generation increment or deferred GL deletion with it.
        if (hasNativeResultsResources() || hasNativeF8BarResources() || riderTextureBorrow || raWorld || raStaticFireOwner || rcParticleWindow || gbParticleWindow || mcParticleWindow || hasDisplayFadeNames() || uiCoverage.owns() || uiSourceLeaseWork || uiRetainedTarget || uiRetainedColor || uiBlendOldSnapshot ||
            uiPromotedIncomingTarget || uiPromotedIncomingColor || hasGameplayBlurResources() || hasFxaaResources() || (hasRaceHudResources() || hasRaceStartOverlayResources() || hasTimeStatsResources() || hasPositionStatsResources() || hasDamageResources() || hasPauseDerivedResources() || hasSpeedBoostResources())){
            gl.finish();const U error=drawGl.getError();
            if (error){nativeGlFaultObserved50KG=true;if(!d.cleanupGlError)d.cleanupGlError=error;d.cleanupIncomplete=true;
                return fail(d,VehicleWindowError50DS::cleanup,"complete source leases before cleanup");}
            uiSourceLeaseWork=false;
        }
        uiSourceReceiptLifetime.reset();uiSnapshotGeneration=0;
        uiBlendGeneration=uiBlendSampleGeneration=0;
        uiPromotedIncomingGeneration=0;
        gl.useProgram(0); drawGl.bindVertexArray(0); drawGl.bindBuffer(arrayBuffer, 0);
        gl.bindFramebuffer(framebuffer, 0); gl.bindRenderbuffer(renderbuffer, 0);
        if(uiCoverage.owns()){unbindUiCoverageInputs();gl.disable(0x809D);}
        if (coverageDemand) {
            coverageAa0();
            gl.activeTexture(textureUnit0+2);gl.bindTexture(coverageTexture,0);
            if(gl.bindSampler)gl.bindSampler(2,0);
            gl.activeTexture(textureUnit0);gl.bindTexture(coverageTexture,0);
        }
        if (usesTextures() || hasDisplayFadeNames()) {
            if (request.enableOriginalUiSourceRgba || request.enableOriginalGameplayBlur || (hasRaceHudResources() || hasRaceStartOverlayResources() || hasTimeStatsResources() || hasPositionStatsResources() || hasDamageResources() || hasPauseDerivedResources() || hasSpeedBoostResources()) || hasSpecularPairs50LA || raWorld) {
                gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);
                if (gl.bindSampler)gl.bindSampler(1,0);
            }
            gl.activeTexture(textureUnit0); gl.bindTexture(texture2D, 0);
            if (gl.bindSampler) gl.bindSampler(0, 0);
        }
        const U unbindError = drawGl.getError();
        if (unbindError) {
            nativeGlFaultObserved50KG=true;
            if (!d.cleanupGlError) d.cleanupGlError = unbindError;
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::cleanup, "unbind renderer resources before cleanup");
        }
        bool complete = true;
        if(!deleteNativeVideoOverlay50LA(d))goto incomplete;
        if(!deleteNativeVideoPacketTargets50LA(d))goto incomplete;
        if(!deleteNativeResultsResources(d))goto incomplete;
        if(!deleteNativeF8BarNames(d))goto incomplete;
        if(!deleteSpeedBoostNames(d))goto incomplete;
        if(!deletePauseDerivedNames(d))goto incomplete;
        if(!deleteDamageNames(d))goto incomplete;
        if(!deletePositionStatsNames(d))goto incomplete;
        if(!deleteTimeStatsNames(d))goto incomplete;
        if(!deleteRaceHudNames(d))goto incomplete;
        if(!deleteRaceStartOverlayNames(d))goto incomplete;
        if(!deleteGameplayBlurNames(d))goto incomplete;
        if(!deleteFxaaNames(d))goto incomplete;
        if(!deleteDebrisNames(d))goto incomplete;
        if(!deleteEffectNames(d))goto incomplete;
        if(!deleteWaterNames(d))goto incomplete;
        if(!deleteShadowNames(d))goto incomplete;
        if (shader && !shader->release(d.cleanupShader)) {
            if (!d.cleanupGlError) d.cleanupGlError = d.cleanupShader.glError ?
                d.cleanupShader.glError : d.cleanupShader.cleanupGlError;
            complete = false;
        }
        // Only proceed after an acknowledged clean error state. Stop at a
        // failing deletion instead of conflating it with subsequent objects.
        if (!drainCleanupErrors(d)) complete = false;
        else {
            if(!deleteDisplayFadeNames(d))goto incomplete;
            if (!deleteUiByteBlendNames(d) || !deleteUiCoverage(d))goto incomplete;
            garageSamplersReady50KG=false;
            for(auto& name:garageSamplers50KG)if(!deleteName(name,gl.deleteSamplers,d))goto incomplete;
            uiBlendOldSnapshot=uiBlendIncomingSource=0;
            if (!deleteName(uiPromotedIncomingTarget,gl.deleteFramebuffers,d) ||
                !deleteName(uiPromotedIncomingColor,gl.deleteTextures,d))goto incomplete;
            if (!deleteShaderName(uiProgram, true, d) ||
                !deleteShaderName(uiVertexShader, false, d) ||
                !deleteShaderName(uiFragmentShader, false, d) ||
                !deleteName(uiVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(uiVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiParticleFogBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiSourceRasterWBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiSourcePacketStqBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiSourceIndexBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            for (U b = 0; b < 2; ++b)
                if (!deleteName(uiTargets[b], gl.deleteFramebuffers, d) ||
                    !deleteName(uiColors[b], gl.deleteRenderbuffers, d) ||
                    !deleteName(uiDepths[b], gl.deleteRenderbuffers, d)) goto incomplete;
            if (!deleteName(uiRetainedTarget,gl.deleteFramebuffers,d) ||
                !deleteName(uiRetainedColor,gl.deleteTextures,d))goto incomplete;
            if (!deleteName(uiSourceTarget,gl.deleteFramebuffers,d) ||
                !deleteName(uiSourceColor,gl.deleteTextures,d) ||
                !deleteName(uiSourceDepth,gl.deleteRenderbuffers,d))goto incomplete;
            if (!deleteShaderName(trackProgram, true, d) ||
                !deleteShaderName(trackVertexShader, false, d) ||
                !deleteShaderName(trackFragmentShader, false, d)) goto incomplete;
            if (!deleteShaderName(hostHudProgram, true, d) ||
                !deleteShaderName(hostHudVertexShader, false, d) ||
                !deleteShaderName(hostHudFragmentShader, false, d)) goto incomplete;
            for (auto& bank : resources.bank) {
                for (U c = 0; c < maxComponents; ++c) {
                    if (!deleteName(bank.vertexArray[c], gl.deleteVertexArrays, d) ||
                        !deleteName(bank.vertexBuffer[c], gl.deleteBuffers, d)) goto incomplete;
                }
            }
            for (auto& body : sourceSpecularGeometry50LA) {
                for (auto& buffer : body.buffers)if (!deleteName(buffer, gl.deleteBuffers, d)) goto incomplete;
                for (auto& buffer : body.fogBuffers)if (!deleteName(buffer, gl.deleteBuffers, d)) goto incomplete;
            }
            for (auto& bank : racerNormalBuffers)
                for (auto& normal : bank)
                    if (!deleteName(normal, gl.deleteBuffers, d)) goto incomplete;
            for (auto& index : resources.indices)
                if (!deleteName(index.buffer, gl.deleteBuffers, d)) goto incomplete;
            for (auto& skin : playerSkin50LA)
                if (!deleteName(skin.buffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(gpuSkinCheck50LA.buffer, gl.deleteBuffers, d)) goto incomplete;
            if (gpuSkinCheck50LA.program) { shaderGl.deleteProgram(gpuSkinCheck50LA.program); gpuSkinCheck50LA.program = 0; }
            if (gpuSkinCheck50LA.vertexShader) { shaderGl.deleteShader(gpuSkinCheck50LA.vertexShader); gpuSkinCheck50LA.vertexShader = 0; }
            for (auto& archetype : racerArchetypes) {
                for (auto& skin : archetype.skin50LA)
                    if (!deleteName(skin.buffer, gl.deleteBuffers, d)) goto incomplete;
                for (auto& body : archetype.specularGeometry50LA) {
                    for (auto& buffer : body.buffers)if (!deleteName(buffer, gl.deleteBuffers, d)) goto incomplete;
                    for (auto& buffer : body.fogBuffers)if (!deleteName(buffer, gl.deleteBuffers, d)) goto incomplete;
                }
                for (auto& bank : archetype.normalBuffers)
                    for (auto& normal : bank)
                        if (!deleteName(normal, gl.deleteBuffers, d)) goto incomplete;
                for (auto& bank : archetype.resources.bank)
                    for (U c = 0; c < maxComponents; ++c)
                        if (!deleteName(bank.vertexArray[c], gl.deleteVertexArrays, d) ||
                            !deleteName(bank.vertexBuffer[c], gl.deleteBuffers, d)) goto incomplete;
                for (auto& index : archetype.resources.indices)
                    if (!deleteName(index.buffer, gl.deleteBuffers, d)) goto incomplete;
            }
            if (!deleteName(cableVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(cableVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(cableIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(hostHudVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(hostHudVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(hostHudIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(trackVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(trackVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(trackNormalBuffer, gl.deleteBuffers, d) ||
                !deleteName(trackProjectionBuffer, gl.deleteBuffers, d) ||
                !deleteName(trackIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            for (U b = 0; b < 2; ++b)
                if (!deleteName(dynamicWorldArrays[b], gl.deleteVertexArrays, d) ||
                    !deleteName(dynamicWorldVertexBuffers[b], gl.deleteBuffers, d) ||
                    !deleteName(dynamicWorldNormalBuffers[b], gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(dynamicWorldIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(skyVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(skyVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(skyIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteRaWorldResources(d) || !deleteCoverageResources(d)) goto incomplete;
            for (U b = 0; b < 2; ++b)
                if (!deleteName(targets[b], gl.deleteFramebuffers, d) ||
                    !deleteName(colors[b], gl.deleteRenderbuffers, d) ||
                    !deleteName(sourcePairColors50LA[b], gl.deleteTextures, d) ||
                    !deleteName(depths[b], gl.deleteRenderbuffers, d) ||
                    !deleteName(menuTargets[b], gl.deleteFramebuffers, d) ||
                    !deleteName(menuColors[b], gl.deleteRenderbuffers, d)) goto incomplete;
        }
        d.cleanupIncomplete = hasGlResources();
        if(!d.cleanupIncomplete){retireRaStaticFirePublication();retireRaWorldPublication();retireRcParticlePublication();retireGbParticlePublication();retireMcParticlePublication();}
        if (d.cleanupIncomplete || !complete)
            return fail(d, VehicleWindowError50DS::cleanup, "renderer cleanup incomplete");
        shader.reset();
        if (d.cleanupGlError)
            return fail(d, VehicleWindowError50DS::cleanup, "cleanup completed after GL errors");
        return true;
    incomplete:
        d.cleanupIncomplete = true;
        return fail(d, VehicleWindowError50DS::cleanup, "GL resource deletion failed");
    }

} // namespace rr::platform
