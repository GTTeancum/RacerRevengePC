#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_canonical_submission50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_main_particle_adapter50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "../../../src/assets/original_standard_material_source50la.h"
#include <algorithm>
#include <cstring>
#include <iterator>
#include <limits>
namespace rr::platform {
namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool reject(D& d,S status,std::uint32_t pc,const char* field)noexcept {
    d.status=status;d.sourcePC=pc;d.field=field;return false;
}
}
bool OriginalUiGarageDrawContribution50KG::prepareDrainParticlePolicy24B130(D& d)noexcept {
    d={};const auto fifo=issuer_?issuer_->nativeCollectorFifoOwner_:nullptr;
    if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24B130||
       !fifo||!fifo->draining_||drainParticlePolicy_||!drainAppStart_||!drainIpum_)
        return reject(d,S::phase,0x24B130,"first actual particle policy preparation");
    const auto domain=issuer_->standardMaterialDomainOwner_;
    const auto context=issuer_->materialContextOwner_;
    const auto provider=domain?domain->archiveProvider3E1A70():nullptr;
    if(!context||!context->live()||!context->materialApplication()||
       context->materialApplication()->appStart()!=drainAppStart_||!provider||!provider->sealed())
        return reject(d,S::phase,0x24B130,"same live Garage material context");
    try {
        if(!page_||!page_->scene4C0_||page_->sceneBacking_.get()!=page_->scene4C0_||
           !page_->scene4C0_->sceneParticles_)
            return reject(d,S::phase,0x24B130,"actual retained Scene particle owner");
        const auto& pool=*page_->scene4C0_->sceneParticles_;
        if(!pool.prepared||!pool.entityLive||pool.retiring||pool.retired||
           page_->scenePageChildren_[9]!=&pool.entity||!pool.material150||!pool.material150->retained()||
           pool.material150->table382B38()!=fifo->table_.get())
            return reject(d,S::phase,0x24B130,"actual constructed Material150");
        //The bank persists across frames. Retain the real constructed atlas
        //even when this first frame's source count is zero; no draw or source
        //249610 history is claimed by this read-only CPU image preparation.
        for(const auto& record:fifo->records_){
            if(!record.garageParticles)continue;
            if(record.garageParticles!=&pool||record.garageDraw.get()!=this||
               record.garagePublication!=publication_||record.epoch!=fifo->epoch04_)
                return reject(d,S::phase,0x162E50,"same actual Scene particle pool record");
            OriginalUiGarageSceneParticles50KG::PacketBorrow packet;
            if(!pool.borrowPacket162E50(packet)||!pool.nativeGarageFifoPins_||
               packet.revision!=record.particlePacketRevision||
               packet.material150!=record.primitive04||packet.primitive160!=record.attributes08)
                return reject(d,S::source,0x162E50,"actual pinned Material150 and Primitive160");
        }
        {
            auto next=std::make_unique<DrainParticlePolicy>();
            next->materialRow=pool.material150->material00();
            assets::OriginalStandardMaterialCollectorEntryView50LA entry;
            if(!fifo->table_->coldCollectorPass(fifo->registry_,next->materialRow,0,entry)||entry.finalLeaf<0)
                return reject(d,S::collector,0x24B130,"actual populated ParticleCode registration");
            next->registration=entry.finalLeaf;
            for(std::size_t state=0;state<2;++state){
                assets::OriginalStandardMaterialMapBindingView50LA map;
                if(!fifo->table_->prepareGarageParticleMap249610(fifo->registry_,next->materialRow,state,map,d))return false;
                if(map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||
                   map.representative.registration!=next->registration||!map.texture||!map.texture->sourceMember())
                    return reject(d,S::texture,0x24B254,"actual particle context texture");
                const auto* source=map.representative.material->source();
                const auto* materialBytes=source?source->originalBytes():nullptr;
                std::shared_ptr<const assets::OriginalUiArchive50KG> materialArchive,textureArchive;
                assets::OriginalUiMemberView50KG materialMember,textureMember;
                if(!source||!source->member()||!materialBytes||
                   !provider->borrowGarageUiMember132820(*source->member(),materialArchive,materialMember)||
                   !provider->borrowGarageUiMember132820(*map.texture->sourceMember(),textureArchive,textureMember)||
                   materialBytes->size()!=materialMember.size||
                   std::memcmp(materialBytes->data(),materialMember.data,materialMember.size))
                    return reject(d,S::source,0x132820,"winning particle material source");
                const auto* original=map.texture->originalBytes();std::size_t offset=0,bytes=0;
                if(!original||original->size()!=textureMember.size||
                   std::memcmp(original->data(),textureMember.data,textureMember.size)||
                   !map.texture->pstExtent24E880(offset,bytes)||!bytes||
                   offset>textureMember.size||bytes>textureMember.size-offset)
                    return reject(d,S::texture,0x24E880,"actual retained particle PST extent");
                std::size_t image=0;
                for(;image<drainScreenImages_.size();++image){
                    const auto* p=drainScreenImages_[image]->sourceProvenance(0);
                    if(p&&p->memberIdentity==textureMember.identity&&p->pstOffset==offset&&p->pstBytes==bytes)break;
                }
                std::shared_ptr<assets::OriginalUiScreenAssets50KG> added;
                if(image==drainScreenImages_.size()){
                    if(image>=86)return reject(d,S::overflow,0x24B130,"unchanged128 image-owner capacity");
                    added=std::make_shared<assets::OriginalUiScreenAssets50KG>();
                    assets::OriginalUiImageSource50KG input;
                    input.identity=static_cast<std::uint64_t>(image+1);input.member=textureMember;
                    input.pstOffset=offset;input.pstBytes=bytes;
                    assets::OriginalUiScreenAssetDiagnostic50KG decoded;
                    if(!added->load({input},decoded,drainIpum_.get()))
                        return reject(d,S::texture,0x24E880,"actual particle atlas decode");
                }
                const auto images=added?std::shared_ptr<const assets::OriginalUiScreenAssets50KG>(added):drainScreenImages_[image];
                const auto* texture=images->decodedTexture(0);
                assets::OriginalUiMainParticleMaterial50KG actual;assets::OriginalUiMaterialDiagnostic50KG material;
                if(!texture||!assets::prepareOriginalUiGarageParticleMaterial50KG(*materialArchive,
                   context->materialApplication()->templates(),context->materialRuntime(),*texture,actual,material)||
                   actual.identity!=materialMember.identity||actual.textureIdentity!=textureMember.identity)
                    return reject(d,S::unsupportedPass,material.sourcePC?material.sourcePC:0x2480D0,"authenticated Garage two-context particle policy");
                if(state&&next->archive!=materialArchive)
                    return reject(d,S::source,0x24B218,"same actual representative source for both contexts");
                next->archive=materialArchive;next->material=actual;
                next->imageOwners[state]=static_cast<std::uint32_t>(42+image);
                OriginalUiMaterialSpanBinding50KG binding;binding.quadCount=binding.drawQuadCount=binding.textureCount=1;
                binding.textureIndex=0; // The authenticated local owner above exposes decodedTexture(0).
                OriginalUiMaterialSpan50KG scalar;
                if(!convertOriginalUiMainParticleSpan50KG(actual,static_cast<std::uint8_t>(state),binding,scalar))
                    return reject(d,S::unsupportedPass,0x249610,"actual particle sampler conversion");
                if(added){drainScreenImages_.push_back(std::move(added));drainScreenSamplers_.push_back(scalar.native.sampler);}
                drainScreenArchives_.push_back(std::move(textureArchive));
            }
            drainParticlePolicy_=std::move(next);
        }
        return true;
    }catch(...){return reject(d,S::allocation,0x24B130,"retained particle image/policy owners");}
}
bool OriginalUiGarageDrawContribution50KG::drainParticleRecordCurrent()const noexcept {
    const auto fifo=issuer_?issuer_->nativeCollectorFifoOwner_:nullptr;
    if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||!fifo||!fifo->draining_||
       fifo->epoch04_!=drainEpoch_||drainActiveRecordOrdinal_>=fifo->records_.size()||
       !page_||!page_->scene4C0_||page_->sceneBacking_.get()!=page_->scene4C0_)return false;
    auto it=fifo->records_.begin();std::advance(it,drainActiveRecordOrdinal_);const auto& record=*it;
    const auto* pool=record.garageParticles;
    if(!pool||page_->scene4C0_->sceneParticles_.get()!=pool||page_->scenePageChildren_[9]!=&pool->entity||
       record.garageDraw.get()!=this||record.garagePublication!=publication_||record.epoch!=drainEpoch_||
       !pool->nativeGarageFifoPins_||pool->leaf14CRegistration<0||pool->leaf14COwner!=fifo||pool->leaf14CRegistration!=record.originRegistration||
       !fifo->table_->coldCollectorCurrent(pool->leaf14CReceipt)||
       fifo->table_->nativeCollectorEntry29C0B0(pool->leaf14CReceipt,static_cast<std::size_t>(pool->leaf14CRegistration))!=pool->field14C)
        return false;
    OriginalUiGarageSceneParticles50KG::PacketBorrow packet;
    if(drainParticleGeometry_&&(drainParticleGeometry_->revision!=record.particlePacketRevision||
       drainParticleGeometry_->publication!=publication_))return false;
    return pool->borrowPacket162E50(packet)&&packet.publication==publication_&&packet.revision==record.particlePacketRevision&&
        packet.material150==record.primitive04&&packet.primitive160==record.attributes08;
}
bool OriginalUiGarageDrawContribution50KG::drainParticleClippedEmptyCurrent()const noexcept {
    return drainParticleGeometry_&&drainParticleRecordCurrent()&&!drainParticleGeometry_->rows.empty()&&
        !drainParticleGeometry_->visibleQuads&&drainParticleGeometry_->publication==publication_&&
        std::all_of(drainParticleGeometry_->rows.begin(),drainParticleGeometry_->rows.end(),
            [](const auto& row){return !row.projected.visible;});
}
bool OriginalUiGarageDrawContribution50KG::lowerAndBindQueuedParticle24B26C(std::size_t ordinal,D& d)noexcept {
    d={};
    if(sourcePC_!=0x24B26C||ordinal!=drainActiveRecordOrdinal_||!drainParticleRecordCurrent()||
       !drainContextCaptured_||!drainResourcesBound_||!drainParticlePolicy_||drainBatchPending_||
       !cameraFrame_.current()||!renderer_||!renderer_->ownsRenderPrelude(*this,publication_))
        return reject(d,S::phase,0x24B26C,"actual retained particle drain cursor");
    const auto fifo=issuer_->nativeCollectorFifoOwner_;auto it=fifo->records_.begin();std::advance(it,ordinal);
    const auto& record=*it;const auto& policy=*drainParticlePolicy_;
    assets::OriginalStandardMaterialCollectorEntryView50LA origin;assets::OriginalStandardMaterialPassView50LA pass;
    if(policy.registration!=record.finalRegistration||
       !fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.originRegistration),origin)||
       !origin.material||origin.pass!=0||!origin.material->pass4C(0,pass)||
       record.originParameters!=pass.parameters34||record.textureBase||record.vertexBase||
       pass.animation10||pass.textureAnimation10||!pass.parameterCount28||!pass.parameterMask24||
       *pass.parameterCount28||*pass.parameterMask24||(record.originParameters&&!record.originParameters->empty()))
        return reject(d,S::unsupportedPass,0x24B4E4,"actual particle origin overlay/animation operands");
    OriginalUiGarageCameraFrame50KG::Fields fields;std::uint32_t pending=0;
    if(!cameraFrame_.read(fields,pending))return reject(d,S::phase,pending?pending:0x24BBAC,"same live particle Camera");
    game::OriginalUiMainParticleCamera50KG camera;
    camera.sourceScreen140=fields.screen140;camera.combinedSourceScreen250=fields.combinedScreen250;
    camera.focalPixels1D8=fields.focal1D8;
    game::OriginalUiMainParticleRenderConstants50KG constants;
    constants.combinedClip100=fields.combined100;
    constants.fogStart33Z=drainContext_.camera.fogEnd33Z;constants.fogScale33W=drainContext_.camera.fogSlope33W;
    constants.sourceCanvasOffset=drainContext_.camera.canvasOffset;
    try {
        drainParticleGeometry_=std::make_unique<OriginalUiGarageParticlePacketGeometry50KG>();
        if(!record.garageParticles->lowerPacket3212C8(camera,constants,drainContext_.camera.cameraPosition180,*drainParticleGeometry_,d))return false;
        const auto& geometry=*drainParticleGeometry_;
        if(geometry.revision!=record.particlePacketRevision||geometry.publication!=publication_||
           geometry.visibleQuads>originalUiMaximumSourceQuads50KM||
           drainCommittedQuads_>std::numeric_limits<std::uint32_t>::max()-geometry.visibleQuads)
            return reject(d,S::overflow,0x24B26C,"unchanged batch geometry and checked cumulative accounting");
        const auto images=drainImageBank_->images();std::array<std::uint32_t,2> indices{};
        if(!images||!images->textures())return reject(d,S::texture,0x249610,"actual particle image bank");
        for(std::size_t state=0;state<2;++state){
            const auto* mapping=drainImageBank_->mapping(policy.imageOwners[state],0);
            if(!mapping)return reject(d,S::texture,0x249610,"actual context atlas mapping");
            indices[state]=mapping->combinedIndex;
        }
        drainScreenVertices_.clear();drainScreenStq_.clear();drainScreenW_.clear();drainScreenFog_.clear();
        drainParticleSpans_.clear();drainParticleRanges_.clear();drainParticleStates_.clear();
        std::uint32_t first=0,count=0;std::uint8_t state=0;bool started=false;
        const auto finishRun=[&]() {
            if(!count)return true;
            OriginalUiMaterialSpanBinding50KG binding;binding.firstQuad=first;binding.quadCount=count;
            binding.drawQuadCount=geometry.visibleQuads;binding.textureCount=images->textures()->textureCount();
            binding.textureIndex=indices[state];OriginalUiMaterialSpan50KG span;
            if(!convertOriginalUiMainParticleSpan50KG(policy.material,state,binding,span))return false;
            drainParticleSpans_.push_back(span);drainParticleRanges_.push_back(span.span);drainParticleStates_.push_back(span.native);
            first+=count;count=0;return true;
        };
        for(const auto& row:geometry.rows){
            if(row.firstInGroup||row.firstInRun){
                if(!finishRun())return reject(d,S::unsupportedPass,0x3213A8,"actual signed-run particle state");
                state=row.projected.stateIndex;started=true;
            }
            if(!started||state>1||row.projected.stateIndex!=state)
                return reject(d,S::source,0x3213A8,"retained source run order");
            if(!row.projected.visible)continue; //The actual clipped row remains in geometry.rows.
            drainScreenVertices_.insert(drainScreenVertices_.end(),row.projected.vertices.begin(),row.projected.vertices.end());
            drainScreenStq_.insert(drainScreenStq_.end(),row.sourceSTQ.begin(),row.sourceSTQ.end());
            drainScreenW_.insert(drainScreenW_.end(),row.sourceW.begin(),row.sourceW.end());
            drainScreenFog_.insert(drainScreenFog_.end(),row.projected.fogFactors.begin(),row.projected.fogFactors.end());++count;
        }
        if(!finishRun()||first!=geometry.visibleQuads)
            return reject(d,S::source,0x3217A0,"complete source particle run extent");
        drainBatchCanonical_=std::shared_ptr<OriginalUiGarageCanonicalSubmission50KG>(new OriginalUiGarageCanonicalSubmission50KG);
        if(!drainBatchCanonical_->initialize(fifo->table_,fifo->registry_,drainImageBank_,*this,publication_,d)||
           !drainBatchCanonical_->stageParticleContexts249610(policy.materialRow,record.garageParticles,*policy.archive,
               policy.material,indices,drainParticleSpans_,d))return false;
        auto& batch=drainBatchNative_;batch={};batch.opaquePresentationAlphaOnly=false;
        batch.draw.textures=drainGpu_.get();batch.draw.texturePublication=drainGpu_->publicationToken();
        batch.draw.canvasWidth=640;batch.draw.canvasHeight=448;batch.draw.quadCount=geometry.visibleQuads;
        batch.draw.vertices=geometry.visibleQuads?drainScreenVertices_.data():nullptr;
        batch.draw.spans=drainParticleRanges_.empty()?nullptr:drainParticleRanges_.data();
        batch.draw.spanCount=static_cast<std::uint32_t>(drainParticleRanges_.size());
        batch.states=drainParticleStates_.empty()?nullptr:drainParticleStates_.data();batch.stateCount=batch.draw.spanCount;
        batch.sourcePacketStq=geometry.visibleQuads?drainScreenStq_.data():nullptr;
        batch.sourcePacketStqCount=static_cast<std::uint32_t>(drainScreenStq_.size());
        batch.sourceRasterW=geometry.visibleQuads?drainScreenW_.data():nullptr;
        batch.sourceRasterWCount=static_cast<std::uint32_t>(drainScreenW_.size());
        batch.particleFog=geometry.visibleQuads?drainScreenFog_.data():nullptr;
        batch.particleFogCount=static_cast<std::uint32_t>(drainScreenFog_.size());
        const auto& display=*renderer_->display_;
        if(!display.sourceFogGsColor3DKnown_)return reject(d,S::phase,0x244A40,"actual retained source fog color");
        const auto rgb=display.sourceFogGsColor3D_;
        batch.particleFogColor[0]=float((rgb>>16)&255u)/255.f;batch.particleFogColor[1]=float((rgb>>8)&255u)/255.f;
        batch.particleFogColor[2]=float(rgb&255u)/255.f;
        if(!drainParticleRecordCurrent())return reject(d,S::phase,0x24B26C,"particle record changed during lowering");
        return true;
    }catch(...){return reject(d,S::allocation,0x24B26C,"owned actual particle run geometry");}
}
}
