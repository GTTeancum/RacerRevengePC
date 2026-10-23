#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_canonical_submission50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_frontend50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include <algorithm>
#include <cstdio>
#include <limits>
#include <thread>
namespace rr::platform {
namespace {using D=assets::OriginalStandardMaterialDiagnostic50LA;using S=assets::OriginalStandardMaterialStatus50LA;
bool reject(D& d,S status,std::uint32_t pc,const char* field)noexcept{d.status=status;d.sourcePC=pc;d.field=field;return false;}}
std::shared_ptr<::original_main_entry50KG::NativeCollectorFifo50LA> OriginalUiGarageDrawContribution50KG::fifoForBackendDrain()const noexcept {
    return identityCurrent()&&phase_==Phase::collectorDrain24B7B0?issuer_->nativeCollectorFifoOwner_:nullptr;
}
bool OriginalUiGarageDrawContribution50KG::frontendTextureServicesCurrent(const OriginalUiFrontend50KG& coordinator)const noexcept {
    const auto session=session_.lock();
    return identityCurrent()&&session&&session->entryFrontend_&&
        &session->entryFrontend_->coordinator==&coordinator&&
        session->entryFrontend_->garageTransferOwnerCurrent(session->grant_);
}
bool OriginalUiGarageDrawContribution50KG::frontendTextureRendererCurrent(const VehicleWindowGl50DS& renderer)const noexcept {
    return issuer_&&renderer_&&renderer_->currentFor(issuer_)&&renderer_->display_&&
        &renderer_->display_->renderer_==&renderer;
}
bool OriginalUiGarageDrawContribution50KG::prepareDrainResources24B130(D& d)noexcept {
    d={};if(drainResourcesBound_)return drainImageBank_&&drainGpu_&&drainGpu_->initialized();
    if(drainResourcesAttempted_||!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24B130u)
        return reject(d,S::phase,0x24B130,"actual first retained Garage resource preparation");
    const auto session=session_.lock();
    if(!session||!session->entryFrontend_)return reject(d,S::phase,0x24B130,"same retained Frontend");
    auto& coordinator=const_cast<::original_main_entry50KG::Frontend*>(session->entryFrontend_)->coordinator;
    VehicleWindowDiagnostic50DS native;
    if(!coordinator.borrowGarageTextureServices50KG(*this,drainTextureRequest_,drainAppStart_,drainIpum_,native))
        return reject(d,S::phase,0x24B130,native.operation);
    textureCoordinator_=&coordinator;drainResourcesAttempted_=true;
    if(!prepareDrainScreenPolicies24B130(d)||!prepareDrainParticlePolicy24B130(d)||!prepareDrainModelPolicies24B130(d))return false;
    const auto images=drainImageBank_?drainImageBank_->images():nullptr;
    if(!images||!images->textures()||drainSamplers_.size()!=images->textures()->textureCount())
        return reject(d,S::texture,0x249780,"actual complete model ImageBank/sampler publication");
    try {drainGpu_=std::make_shared<MaterialTextureGl50EB>();}
    catch(...){return reject(d,S::allocation,0x249780,"retained actual GPU bank owner");}
    auto request=drainTextureRequest_;request.samplers=drainSamplers_.data();
    request.samplerCount=static_cast<std::uint32_t>(drainSamplers_.size());
    MaterialTextureGlDiagnostic50EB upload;
    if(!drainGpu_->create(request,*images->textures(),upload))
        return reject(d,S::texture,0x249780,upload.operation?upload.operation:"actual Garage texture upload rejected");
    if(!coordinator.bindGarageTextureBank50KG(*this,*images->textures(),*drainGpu_,native))
        return reject(d,S::texture,0x249780,native.operation);
    drainResourcesBound_=true;return true;
}
bool OriginalUiGarageDrawContribution50KG::releaseDrainResourcesBeforeRetirement(VehicleWindowDiagnostic50DS& d)noexcept {
    if(issuer_&&issuer_->nativeCollectorFifoOwner_&&issuer_->nativeCollectorFifoOwner_->garageQueueOwnerCurrent(*this,publication_))return false;
    if(renderer_&&renderer_->ownsRenderPrelude(*this,publication_))return false;
    if(page_&&page_->scene4C0_&&page_->scene4C0_->sceneParticles_&&
       page_->scene4C0_->sceneParticles_->nativeGarageFifoPins_){
        d.error=VehicleWindowError50DS::cleanup;d.operation="actual particle105990 release pending";return false;
    }
    if(textureCoordinator_){if(!textureCoordinator_->releaseGarageTextureServices50KG(*this,d))return false;textureCoordinator_=nullptr;}
    if(drainGpu_&&drainGpu_->ownsResources()){
        MaterialTextureGlDiagnostic50EB close;
        if(!drainGpu_->close(close)){d.error=VehicleWindowError50DS::cleanup;d.operation=close.operation;return false;}
    }
    // Drop canonical/native views before their backing. All actual FIFO
    // records were already released by105990, and the renderer now binds the
    // retained prior Frontend bank rather than any of these resources.
    drainBatchCanonical_.reset();drainActiveGeometry_.reset();drainBatchNative_={};drainActiveSpan_={};
    drainParticleGeometry_.reset();drainParticlePolicy_.reset();drainParticleSpans_.clear();drainParticleRanges_.clear();drainParticleStates_.clear();
    drainGpu_.reset();drainImageBank_.reset();drainPolicies_.clear();drainModels_.clear();drainSamplers_.clear();
    drainScreenPolicies_.clear();drainScreenImages_.clear();drainScreenArchives_.clear();drainScreenSamplers_.clear();
    drainScreenVertices_.clear();drainScreenStq_.clear();drainScreenW_.clear();drainScreenFog_.clear();
    drainAppStart_.reset();drainIpum_.reset();drainTextureRequest_={};
    drainResourcesBound_=false;return true;
}
bool OriginalUiGarageDrawContribution50KG::drainCollector24B7B0(Diagnostic& d)noexcept {
    if(!identityCurrent()||phase_!=Phase::thirdPass||!issuer_->nativeCollectorFifoOwner_)return fail(d,0x24B7B0);
    const auto fifo=issuer_->nativeCollectorFifoOwner_;D material;
    const auto materialFailure=[&](const char* stage)noexcept {
        // Preserve the actual nested failure before the outer render adapter
        // reduces it to a source PC. Failure-only observations, no readiness
        // call, pointer disclosure, source write or cleanup completion.
        std::fprintf(stderr,"original_garage_collector_material_rejected stage=%s caller_pc=%u status=%u pc=%u row=%d slot=%zu pass=%zu map=%zu field=%.256s effects=%u texture_status=%u texture_pc=%u texture_row=%d texture_source=%zu texture_offset=%zu program_status=%u program_pc=%u program_row=%d publication=%llu epoch=%llu cursor=%zu record=%zu records=%zu draining=%u resources_attempted=%u resources_bound=%u image_bank=%u gpu_bank=%u models=%zu policies=%zu screen_policies=%zu samplers=%zu batch_pending=%u batch_submitted=%u batch_failed=%u\n",
            stage,sourcePC_,static_cast<unsigned>(material.status),material.sourcePC,
            static_cast<int>(material.row),material.slot,material.pass,material.map,material.field?material.field:"",
            material.sourceEffectsCommitted?1u:0u,static_cast<unsigned>(material.texture.status),
            material.texture.sourcePC,static_cast<int>(material.texture.row),material.texture.source,material.texture.byteOffset,
            static_cast<unsigned>(material.program.status),material.program.sourcePC,static_cast<int>(material.program.row),
            static_cast<unsigned long long>(publication_),static_cast<unsigned long long>(drainEpoch_),
            drainCursor_,drainActiveRecordOrdinal_,fifo->records_.size(),fifo->draining_?1u:0u,
            drainResourcesAttempted_?1u:0u,drainResourcesBound_?1u:0u,drainImageBank_?1u:0u,drainGpu_?1u:0u,
            drainModels_.size(),drainPolicies_.size(),drainScreenPolicies_.size(),drainSamplers_.size(),
            drainBatchPending_?1u:0u,drainBatchSubmitted_?1u:0u,drainBatchFailed_?1u:0u);
        return fail(d,material.sourcePC);
    };
    const auto nativeFailure=[&](const char* stage,std::uint32_t pc)noexcept {
        // Observe only the already-failed native boundary. No extra validation,
        // GL call, history mutation or complete-frame acceptance is performed.
        const auto& native=d.renderer;
        std::fprintf(stderr,"original_garage_collector_native_rejected stage=%s caller_pc=%u pc=%u renderer_error=%u operation=%.256s gl_error=%u cleanup_gl_error=%u publication=%llu epoch=%u cursor=%zu record=%zu records=%zu draining=%u committed_quads=%u model_quads=%u model_triangles=%u model_clipped=%u batch_quads=%u batch_spans=%u canonical=%u batch_pending=%u batch_submitted=%u batch_failed=%u ui_draws=%u ui_quads=%u ui_triangles=%u\n",
            stage,sourcePC_,pc,static_cast<unsigned>(native.error),native.operation?native.operation:"",
            native.glError,native.cleanupGlError,static_cast<unsigned long long>(publication_),drainEpoch_,
            drainCursor_,drainActiveRecordOrdinal_,fifo->records_.size(),fifo->draining_?1u:0u,drainCommittedQuads_,
            drainActiveGeometry_?drainActiveGeometry_->quadCount:0u,
            drainActiveGeometry_?drainActiveGeometry_->sourceTriangleCount:0u,
            drainActiveGeometry_?drainActiveGeometry_->clippedTriangleCount:0u,
            drainBatchNative_.draw.quadCount,drainBatchNative_.draw.spanCount,drainBatchCanonical_?1u:0u,
            drainBatchPending_?1u:0u,drainBatchSubmitted_?1u:0u,drainBatchFailed_?1u:0u,
            native.originalUiDraws,native.originalUiQuads,native.originalUiSourceTriangles);
        return fail(d,pc);
    };
    if(!fifo->retainGarageRegistry(*issuer_,material))return materialFailure("registry");
    if(fifo->draining_||!fifo->table_->coldCollectorCurrent(fifo->registry_))return fail(d,0x24B7B0);
    for(const auto& record:fifo->records_)if(record.garageDraw.get()!=this||record.garagePublication!=publication_||record.epoch!=fifo->epoch04_)
        return fail(d,0x24B9B0);
    //Native equivalent of the real source4010 parameter packet: capture the
    //same Camera/renderer/light rows, not a second context or default matrices.
    fifo->draining_=true;drainEpoch_=fifo->epoch04_;drainCursor_=0;drainOrder_.clear();
    phase_=Phase::collectorDrain24B7B0;sourcePC_=0x24BA50;
    if(!captureModelContext24BA50(drainContext_,material))return materialFailure("context24BA50");
    drainContextCaptured_=true;
    try {
        //24B8D4..930 visits canonical registration order. Within a leaf, retain
        //its actual source append order; NEVER move fonts behind model work.
        const auto count=fifo->table_->coldCollectorCount(fifo->registry_);
        for(std::size_t leaf=0;leaf<count;++leaf){
            assets::OriginalStandardMaterialCollectorEntryView50LA entry;
            if(!fifo->table_->coldCollectorEntry(fifo->registry_,leaf,entry)||!entry.material||!entry.material->constructed29B550())return fail(d,0x29BED0);
            //Actual29B550 constructor rejects Prologue/Epilogue publication,
            //so29BED0 cannot install entry00/08 actions. This is a concrete
            //constructor fact, not inferred from absence of a native callback.
            std::size_t ordinal=0;
            for(const auto& record:fifo->records_){if(record.finalRegistration==static_cast<std::int32_t>(leaf))drainOrder_.push_back(ordinal);++ordinal;}}
        if(drainOrder_.size()!=fifo->records_.size())return fail(d,0x24B8E4);
        // One bounded upload per sealed FIFO record, not one aggregate vertex
        // allocation. Bound cumulative accounting by that finite record extent.
        if(drainOrder_.size()>std::numeric_limits<std::uint32_t>::max()/originalUiMaximumSourceQuads50KM)
            return nativeFailure("collector_total_extent",0x24B26C);
        for(;drainCursor_<drainOrder_.size();++drainCursor_){
            drainActiveRecordOrdinal_=drainOrder_[drainCursor_];auto it=fifo->records_.begin();
            for(std::size_t i=0;i<drainActiveRecordOrdinal_;++i)++it;
            sourcePC_=0x24B130;
            if(!prepareDrainResources24B130(material))return materialFailure("resources24B130");
            drainBatchCanonical_.reset();drainActiveGeometry_.reset();drainParticleGeometry_.reset();drainBatchPending_=false;drainBatchSubmitted_=false;drainBatchFailed_=false;
            sourcePC_=0x24B26C;
            if(it->garageParticles){
                if(!lowerAndBindQueuedParticle24B26C(drainActiveRecordOrdinal_,material))return materialFailure("particle24B26C");
                drainBatchPending_=true;
                if(!renderer_->display_->renderer_.submitGarageCollectorBatch24B26C(*this,d.renderer))return nativeFailure("particle_submit24B26C",0x24B26C);
                drainCommittedQuads_+=drainBatchNative_.draw.quadCount;continue;
            }
            if(!it->garageModel){
                if(!lowerAndBindQueuedScreen24B26C(drainActiveRecordOrdinal_,material))return materialFailure("screen24B26C");
                drainBatchPending_=true;
                if(!renderer_->display_->renderer_.submitGarageCollectorBatch24B26C(*this,d.renderer))return nativeFailure("screen_submit24B26C",0x24B26C);
                drainCommittedQuads_+=drainBatchNative_.draw.quadCount;continue;
            }
            drainActiveGeometry_=std::make_unique<OriginalUiGarageOwnedModelGeometry50KG>();
            if(!lowerQueuedModel24B26C(drainActiveRecordOrdinal_,drainContext_,*drainActiveGeometry_,material))return materialFailure("model24B26C");
            if(drainActiveGeometry_->quadCount>originalUiMaximumSourceQuads50KM||
               drainCommittedQuads_>std::numeric_limits<std::uint32_t>::max()-drainActiveGeometry_->quadCount)
                return nativeFailure("model_quad_budget",0x24B26C);
            drainBatchCanonical_.reset();drainBatchPending_=false;drainBatchSubmitted_=false;drainBatchFailed_=false;
            sourcePC_=0x249610;
            if(!bindQueuedModel249610(*drainActiveGeometry_,drainActiveSpan_,material))return materialFailure("model249610");
            auto& geometry=*drainActiveGeometry_;auto& batch=drainBatchNative_;batch={};
            batch.opaquePresentationAlphaOnly=false;batch.draw.textures=drainGpu_.get();batch.draw.texturePublication=drainGpu_->publicationToken();
            batch.draw.canvasWidth=640.f;batch.draw.canvasHeight=448.f;
            batch.draw.vertices=geometry.vertices.data();batch.draw.quadCount=geometry.quadCount;
            batch.draw.spans=geometry.quadCount?&drainActiveSpan_.span:nullptr;batch.draw.spanCount=geometry.quadCount?1u:0u;
            batch.states=geometry.quadCount?&drainActiveSpan_.native:nullptr;batch.stateCount=batch.draw.spanCount;
            batch.sourcePacketStq=geometry.stq.data();batch.sourcePacketStqCount=static_cast<std::uint32_t>(geometry.stq.size());
            batch.sourceRasterW=geometry.rasterW.data();batch.sourceRasterWCount=static_cast<std::uint32_t>(geometry.rasterW.size());
            batch.particleFog=geometry.fog.data();batch.particleFogCount=static_cast<std::uint32_t>(geometry.fog.size());
            const auto& display=*renderer_->display_;
            if(drainActiveSpan_.native.sourceFogEnabled&&!display.sourceFogGsColor3DKnown_)return fail(d,0x244A40);
            const auto color=display.sourceFogGsColor3D_;batch.particleFogColor[0]=float((color>>16)&255u)/255.f;
            batch.particleFogColor[1]=float((color>>8)&255u)/255.f;batch.particleFogColor[2]=float(color&255u)/255.f;
            sourcePC_=0x24B26C;drainBatchPending_=true;
            if(!renderer_->display_->renderer_.submitGarageCollectorBatch24B26C(*this,d.renderer))return nativeFailure("model_submit24B26C",0x24B26C);
            drainCommittedQuads_+=geometry.quadCount;
        }
    }catch(...){return nativeFailure("collector_exception",0x24B26C);}
    sourcePC_=0x105990;
    if(!renderer_->display_->renderer_.completeGarageCollector24C160(*this,d.renderer))return fail(d,sourcePC_);
    phase_=Phase::thirdPass;sourcePC_=0x24B9A8;return true;
}
}
