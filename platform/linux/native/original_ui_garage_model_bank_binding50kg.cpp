#include "vehicle_window_gl50ds.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_model_resource50kg.h"
#include "original_ui_garage_canonical_submission50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include <limits>
#include <thread>
#include <utility>

namespace rr::platform {
namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool bankFailure(D& d,S s,std::uint32_t pc,const char* field)noexcept {
    d.status=s;d.sourcePC=pc;d.field=field;return false;
}
bool asciiProgram(const std::string& value,const char* expected)noexcept {
    std::size_t i=0;
    for(;i<value.size()&&expected[i];++i){
        const auto fold=[](unsigned char c){return c>='A'&&c<='Z'?c+('a'-'A'):c;};
        if(fold(static_cast<unsigned char>(value[i]))!=fold(static_cast<unsigned char>(expected[i])))return false;
    }
    return i==value.size()&&!expected[i];
}
bool samplerFromRaster(const assets::OriginalUiNativeMaterial50KG& source,MaterialTextureSampler50EB& out)noexcept {
    const auto filter=[](assets::OriginalUiMaterialFilter50KG in,MaterialTextureFilter50EB& value){
        switch(in){case assets::OriginalUiMaterialFilter50KG::nearest:value=MaterialTextureFilter50EB::nearest;return true;
            case assets::OriginalUiMaterialFilter50KG::linear:value=MaterialTextureFilter50EB::linear;return true;default:return false;}
    };
    const auto wrap=[](assets::OriginalUiMaterialWrap50KG in,MaterialTextureWrap50EB& value){
        switch(in){case assets::OriginalUiMaterialWrap50KG::repeat:value=MaterialTextureWrap50EB::repeat;return true;
            case assets::OriginalUiMaterialWrap50KG::clampToEdge:value=MaterialTextureWrap50EB::clampToEdge;return true;default:return false;}
    };
    MaterialTextureSampler50EB next;
    if(!filter(source.minification,next.minification)||!filter(source.magnification,next.magnification)||
       !wrap(source.u,next.s)||!wrap(source.v,next.t))return false;
    out=next;return true;
}
}
bool OriginalUiGarageDrawContribution50KG::prepareDrainModelPolicies24B130(D& d)noexcept {
    d={};
    if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24b130u||
       !drainAppStart_||!drainAppStart_->loaded()||
       drainAppStart_->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::appStart)||
       !drainIpum_||!drainIpum_->loaded()||drainImageBank_||!drainPolicies_.empty()||
       !drainModels_.empty()||!drainSamplers_.empty())
        return bankFailure(d,S::phase,0x24b130u,"same retained Garage CPU image preparation");
    const auto session=session_.lock();const auto fifo=issuer_->nativeCollectorFifoOwner_;
    if(!session||session->issuer_!=issuer_||!session->resources_||!session->sceneAssets_||
       !session->sceneAssets_->loaded()||!session->wattoAssets_||!session->wattoAssets_->loaded()||
       !fifo||fifo->application_!=issuer_||fifo->closed_||!fifo->draining_||!fifo->uiConstructor1B73BC_||
       fifo->ownerThread_!=std::this_thread::get_id()||!fifo->table_||
       !fifo->table_->coldCollectorCurrent(fifo->registry_))
        return bankFailure(d,S::phase,0x24b130u,"actual Session population and native FIFO");
    const auto& resources=*session->resources_;
    if(resources.issuer_!=issuer_||resources.application_!=issuer_->entitiesOwner_||
       resources.domain_!=issuer_->standardMaterialDomainOwner_||!resources.generation_||
       resources.population_.size()!=42u||!resources.domain_||resources.domain_->materials382B38()!=fifo->table_)
        return bankFailure(d,S::source,0x26e820u,"complete actual 42-model construction population, not visibility");
    try {
        std::vector<std::shared_ptr<OriginalUiGarageModelResource50KG>> models;
        std::vector<std::shared_ptr<const assets::OriginalUiGarageModelMaterials50KG>> policies;
        models.reserve(resources.population_.size());policies.reserve(resources.population_.size());
        assets::OriginalUiImageBankInput50KG input;
        // DLC career: a package selected archive joins as an add-on archive of the bank.
        if(session->sceneAssets_->selectedArchive()&&session->sceneAssets_->selectedArchive()->addon()){
            input.archives={drainAppStart_,session->sceneAssets_->archive()};
            input.addonArchives={session->sceneAssets_->selectedArchive()};
        }else input.archives={drainAppStart_,session->sceneAssets_->archive(),session->sceneAssets_->selectedArchive()};
        if(session->residentPodTextures_)input.archives.push_back(session->residentPodTextures_);
        input.ipumCache=drainIpum_.get();input.owners.reserve(resources.population_.size());
        const auto* selected=session->sceneAssets_->selectedPod();
        if(!selected||!selected->model)return bankFailure(d,S::source,0x1d1cd0u,"actual selected19E source");
        // Original manager selected, manager Widget10 Watto,30 Scene statics,
        // then10 Toy Actors. Distinct live objects remain distinct even where
        // the authentic immutable pd.psg payload is shared between Actors.
        for(std::size_t i=0;i<resources.population_.size();++i){
            d.slot=i;const auto& model=resources.population_[i];
            if(!model||!model->current()||model->application_!=resources.application_||
               model->domain_!=resources.domain_||model->generation()!=resources.generation_||!model->source()||
               model->nativeSharedSelf_.lock()!=model||model->materials().table382B38()!=fifo->table_.get())
                return bankFailure(d,S::source,0x26e820u,"distinct retained population model identity");
            for(const auto& prior:models)if(prior==model)
                return bankFailure(d,S::duplicatePublication,0x26e820u,"duplicate live model in source population");
            auto policy=std::make_shared<assets::OriginalUiGarageModelMaterials50KG>();
            assets::OriginalUiGarageModelMaterialDiagnostic50KG md;bool loaded=false;
            if(i==0){
                if(model->source()!=selected->model.get())return bankFailure(d,S::source,0x1d1cd0u,"actual selected model population position");
                loaded=policy->loadSelected(session->sceneAssets_,drainAppStart_,issuer_->materialRuntime,drainIpum_.get(),md,
                    session->residentPodTextures_);
            }else if(i==1){
                if(model->source()->source.identity!=session->wattoAssets_->geometrySource()||
                   model->source()->archive!=session->sceneAssets_->archive())
                    return bankFailure(d,S::source,0x1d46b8u,"actual retained Watto model source");
                loaded=policy->loadWatto(session->wattoAssets_,session->sceneAssets_->archive(),drainAppStart_,issuer_->materialRuntime,drainIpum_.get(),md);
            }else if(i<32){
                const auto index=i-2;const auto* source=session->sceneAssets_->staticModel(index);
                if(!source||source->ordinal!=index+1||model->source()!=source->model.get())
                    return bankFailure(d,S::source,0x274160u,"actual authored Static model population order");
                loaded=policy->loadStatic(session->sceneAssets_,index,drainAppStart_,issuer_->materialRuntime,drainIpum_.get(),md);
            }else{
                const auto index=i-32;const auto* source=session->sceneAssets_->actor(index);
                if(!source||source->ordinal!=index+1||model->source()!=source->model.get())
                    return bankFailure(d,S::source,0x2742f0u,"actual authored Actor model population order");
                loaded=policy->loadActor(session->sceneAssets_,index,drainAppStart_,issuer_->materialRuntime,drainIpum_.get(),md);
            }
            if(!loaded||!policy->loaded()||policy->modelIdentity()!=model->source()->source.identity||
               !policy->images()||!policy->images()->loaded())
                return bankFailure(d,S::source,md.materialDetail.sourcePC?md.materialDetail.sourcePC:0x24b130u,
                    "authenticated model policy/images from same source owners");
            const auto* count=model->materials().count1C0();
            if(!count||*count!=policy->materialCount())
                return bankFailure(d,S::reference,0x26e640u,"actual PSG material slot extent");
            models.push_back(model);input.owners.push_back(policy->images());policies.push_back(std::move(policy));
        }
        // The same Draw has already authenticated each queued ScreenText
        // font/Quad owner. Preserve model image-owner indices0..41, then add
        // those REAL screen owners to the single shared GPU bank.
        if(drainScreenImages_.size()!=drainScreenSamplers_.size()||
           drainScreenImages_.size()>128u-policies.size())
            return bankFailure(d,S::source,0x24b130u,"actual screen image/sampler extent");
        for(const auto& archive:drainScreenArchives_){
            if(!archive||!archive->loaded())return bankFailure(d,S::archive,0x24b130u,"retained screen archive");
            bool present=false;
            for(const auto& prior:input.archives)if(prior->identity()==archive->identity()){
                // After a race the next frontend's Garage loads a fresh copy
                // of the same authenticated archive, while132820 keeps the
                // first equal-byte mount as its lookup winner. ImageBank
                // locates members by canonical identity and verifies pixels,
                // so the identical authenticated copy is the same source.
                if(prior!=archive&&(!prior->loaded()||prior->memberCount()!=archive->memberCount()))
                    return bankFailure(d,S::archive,0x24b130u,"different live archive owners with one identity");
                present=true;break;
            }
            if(!present){
                if(input.archives.size()==11u)return bankFailure(d,S::range,0x24b130u,"unchanged ImageBank archive capacity");
                input.archives.push_back(archive);
            }
        }
        for(const auto& image:drainScreenImages_){
            if(!image||!image->loaded()||!image->textures()||image->textures()->textureCount()!=1u)
                return bankFailure(d,S::source,0x24b130u,"exact single authenticated screen image owner");
            input.owners.push_back(image);
        }
        auto bank=std::make_shared<assets::OriginalUiImageBank50KG>();assets::OriginalUiImageBankDiagnostic50KG bd;
        if(!bank->load(input,bd)||!bank->loaded()||!bank->images()||!bank->images()->textures())
            return bankFailure(d,S::texture,0x24b254u,"authentic combined source model ImageBank");
        const auto count=bank->images()->textures()->textureCount();
        if(!count||count>assets::maxMaterialTextureAssets50EA)
            return bankFailure(d,S::range,0x24b254u,"unchanged native image bank capacity");
        std::vector<MaterialTextureSampler50EB> samplers(count);
        std::vector<bool> assigned(count,false);
        for(std::size_t owner=0;owner<policies.size();++owner){
            const auto& policy=*policies[owner];
            for(std::uint32_t slot=0;slot<policy.materialCount();++slot)
                for(std::uint32_t pass=0;pass<policy.passCount(slot);++pass){
                    const auto* authored=policy.pass(slot,pass);
                    if(!authored)return bankFailure(d,S::source,0x247110u,"actual model policy pass extent");
                    if(!authored->source.enabled)continue; // Only the original disabled-pass branch.
                    assets::OriginalUiNativeMaterial50KG raster;assets::OriginalUiMaterialDiagnostic50KG md;
                    bool admitted=false;
                    if(asciiProgram(authored->source.microprogram,"PrelitCompressed")){
                        assets::OriginalUiGaragePrelitMaterial50KG material;
                        admitted=assets::prepareOriginalUiGaragePrelitMaterial50KG(policy,slot,pass,material,md);
                        if(admitted)raster=material.raster;
                    }else{
                        assets::OriginalUiGarageLitMaterial50KG material;
                        admitted=assets::prepareOriginalUiGarageLitMaterial50KG(policy,slot,pass,material,md);
                        if(admitted)raster=material.raster;
                    }
                    if(!admitted)return bankFailure(d,S::unsupportedPass,md.sourcePC,"actual distinct model raster/sampler policy");
                    const auto* map=bank->mapping(static_cast<std::uint32_t>(owner),authored->imageIndex);
                    MaterialTextureSampler50EB sampler;
                    if(!map||map->combinedIndex>=count||!samplerFromRaster(raster,sampler))
                        return bankFailure(d,S::texture,0x249610u,"exact model image remap and sampler");
                    // Initialization uses the first authentic enabled owner
                    // of this image. Later actual spans keep THEIR own sampler;
                    // a shared image never aliases two material policies.
                    if(!assigned[map->combinedIndex]){samplers[map->combinedIndex]=sampler;assigned[map->combinedIndex]=true;}
                }
        }
        for(std::size_t i=0;i<drainScreenImages_.size();++i){
            const auto* map=bank->mapping(static_cast<std::uint32_t>(policies.size()+i),0);
            const auto& sampler=drainScreenSamplers_[i];
            const auto filter=[](MaterialTextureFilter50EB v){return v==MaterialTextureFilter50EB::nearest||v==MaterialTextureFilter50EB::linear;};
            const auto wrap=[](MaterialTextureWrap50EB v){return v==MaterialTextureWrap50EB::repeat||v==MaterialTextureWrap50EB::clampToEdge;};
            if(!map||map->combinedIndex>=count||!filter(sampler.minification)||!filter(sampler.magnification)||
               !wrap(sampler.s)||!wrap(sampler.t))
                return bankFailure(d,S::texture,0x249610u,"actual screen policy sampler and image remap");
            if(!assigned[map->combinedIndex]){samplers[map->combinedIndex]=sampler;assigned[map->combinedIndex]=true;}
        }
        for(bool known:assigned)if(!known)
            return bankFailure(d,S::texture,0x249610u,"bank image has no authenticated enabled sampler owner");
        drainModels_=std::move(models);drainPolicies_=std::move(policies);
        drainSamplers_=std::move(samplers);drainImageBank_=std::move(bank);
        d.status=S::complete;d.sourcePC=0x24b130u;return true;
    }catch(...){return bankFailure(d,S::allocation,0x24b130u,"model policy/image bank allocation");}
}

bool OriginalUiGarageDrawContribution50KG::bindQueuedModel249610(
    const OriginalUiGarageOwnedModelGeometry50KG& geometry,OriginalUiMaterialSpan50KG& out,D& d)noexcept {
    d={};
    if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x249610u||
       !drainActiveGeometry_||drainActiveGeometry_.get()!=&geometry||
       drainActiveRecordOrdinal_!=geometry.recordOrdinal||drainBatchPending_||drainBatchCanonical_||
       !drainImageBank_||!drainImageBank_->loaded()||!drainGpu_||
       drainModels_.size()!=42u||drainPolicies_.size()!=drainModels_.size())
        return bankFailure(d,S::phase,0x249610u,"actual unbound one-record model drain batch");
    if(!geometry.quadCount&&(!geometry.sourceTriangleCount||geometry.clippedTriangleCount!=geometry.sourceTriangleCount))
        return bankFailure(d,S::source,0x24b26cu,"zero geometry requires actual complete clipping");
    if(geometry.firstQuad||geometry.quadCount>originalUiMaximumSourceQuads50KM||
       drainCommittedQuads_>std::numeric_limits<std::uint32_t>::max()-geometry.quadCount||
       geometry.vertices.size()!=std::size_t(geometry.quadCount)*4u||geometry.stq.size()!=geometry.vertices.size()||
       geometry.rasterW.size()!=geometry.vertices.size()||geometry.fog.size()!=geometry.vertices.size())
        return bankFailure(d,S::range,0x24b26cu,"actual bounded batch range and checked cumulative accounting");
    const auto fifo=issuer_->nativeCollectorFifoOwner_;
    if(!fifo||fifo->application_!=issuer_||fifo->closed_||!fifo->draining_||
       fifo->ownerThread_!=std::this_thread::get_id()||!fifo->table_||
       !fifo->table_->coldCollectorCurrent(fifo->registry_)||geometry.recordOrdinal>=fifo->records_.size())
        return bankFailure(d,S::collector,0x249610u,"same retained draining collector");
    auto it=fifo->records_.cbegin();for(std::size_t i=0;i<geometry.recordOrdinal;++i)++it;
    const auto& record=*it;
    if(&record!=geometry.recordIdentity||record.garageDraw.get()!=this||record.garagePublication!=publication_||
       geometry.publication!=publication_||record.epoch!=fifo->epoch04_||geometry.epoch!=record.epoch||
       !record.garageModel||!record.garageModel->current()||!record.garageModel->geometry()||
       record.matrixRevision!=geometry.matrixRevision||
       record.garageModel->nativeGaragePacketRevision()!=geometry.matrixRevision||!record.garageModel->nativeGarageFifoPins_||
       record.originRegistration!=geometry.originRegistration||record.finalRegistration!=geometry.finalRegistration||
       record.lod!=geometry.lod||record.surface!=geometry.surface||record.packetIndex!=geometry.packetIndex||
       record.garageModel->geometry()->surface246300(record.lod,record.surface)!=record.surface04)
        return bankFailure(d,S::collector,0x24b26cu,"same real record/model/revision and lowered geometry");
    const auto images=drainImageBank_->images();const auto textures=images?images->textures():nullptr;
    const auto publication=drainGpu_->publicationToken();
    if(!textures||!publication||!drainGpu_->matchesSourceBank(*textures,publication)||
       !drainGpu_->validBorrow(drainTextureRequest_.context,drainTextureRequest_.isCurrent,publication))
        return bankFailure(d,S::video,0x249610u,"same actual source image GPU publication");
    assets::OriginalStandardMaterialCollectorEntryView50LA origin,final;
    if(record.originRegistration<0||record.finalRegistration<0||
       !fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.originRegistration),origin)||
       !fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.finalRegistration),final)||
       origin.finalLeaf!=final.registration||final.registration!=record.finalRegistration||
       final.redirect18!=-1||!final.material||origin.pass<0||final.pass<0)
        return bankFailure(d,S::collector,0x29c490u,"actual canonical origin and final representative");
    std::size_t originIndex=drainModels_.size(),representative=drainModels_.size();std::uint32_t repSlot=0;
    for(std::size_t i=0;i<drainModels_.size();++i){
        const auto& model=drainModels_[i];
        if(!model||!model->current()||!drainPolicies_[i]||!drainPolicies_[i]->loaded())
            return bankFailure(d,S::phase,0x249610u,"retained model-policy lifetime");
        if(model==record.garageModel)originIndex=i;
        if(representative!=drainModels_.size())continue;
        for(std::uint32_t slot=0;slot<drainPolicies_[i]->materialCount();++slot){
            const auto* row=model->materials().handle1C4(slot);
            if(row&&*row==final.materialRow&&drainPolicies_[i]->pass(slot,static_cast<std::uint32_t>(final.pass))){
                representative=i;repSlot=slot;break;
            }
        }
    }
    if(originIndex==drainModels_.size()||representative==drainModels_.size())
        return bankFailure(d,S::unsupportedPass,0x29c490u,"final material has no actual retained model representative");
    const auto* originRow=record.garageModel->materials().handle1C4(geometry.materialSlot);
    if(!originRow||(*originRow==-1?static_cast<std::int32_t>(geometry.materialSlot):*originRow)!=origin.materialRow)
        return bankFailure(d,S::reference,0x246810u,"actual originating model material slot");
    const auto& policy=drainPolicies_[representative];
    const auto* pass=policy->pass(repSlot,static_cast<std::uint32_t>(final.pass));
    const auto* map=pass?drainImageBank_->mapping(static_cast<std::uint32_t>(representative),pass->imageIndex):nullptr;
    if(!map||map->combinedIndex>=textures->textureCount())
        return bankFailure(d,S::texture,0x249610u,"exact final representative ImageBank mapping");
    try {
        std::shared_ptr<OriginalUiGarageCanonicalSubmission50KG> canonical(new OriginalUiGarageCanonicalSubmission50KG());
        if(!canonical->initialize(fifo->table_,fifo->registry_,drainImageBank_,*this,publication_,d))return false;
        OriginalUiMaterialSpanBinding50KG binding;
        binding.firstQuad=0;binding.quadCount=binding.drawQuadCount=geometry.quadCount;
        binding.textureIndex=map->combinedIndex;binding.textureCount=textures->textureCount();
        OriginalUiMaterialSpan50KG next;assets::OriginalStandardMaterialMapBindingView50LA bound;
        if(!canonical->stageModelPass(record.garageModel,geometry.materialSlot,static_cast<std::uint32_t>(origin.pass),
           drainModels_[representative],repSlot,policy,static_cast<std::uint32_t>(representative),binding,next,bound,d))return false;
        using P=assets::OriginalUiGarageProgram50KG;
        OriginalUiMaterialMode50KG expected;
        switch(geometry.program){
        case P::prelitCompressed:expected=OriginalUiMaterialMode50KG::sourcePrelitCompressed;break;
        case P::diffuse:expected=OriginalUiMaterialMode50KG::sourceDiffuse;break;
        case P::diffuseCompressed:expected=OriginalUiMaterialMode50KG::sourceDiffuseCompressed;break;
        case P::specularMapCompressed:expected=OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;break;
        default:return bankFailure(d,S::unsupportedPass,0x24b1c0u,"actual model family unavailable");
        }
        if(next.span.materialMode!=expected||bound.origin.registration!=record.originRegistration||
           bound.representative.registration!=record.finalRegistration)
            return bankFailure(d,S::unsupportedPass,0x249610u,"same actual numeric program and material final leaf");
        // No accepted binding/history here. The real renderer batch owner is
        // solely responsible for begin/finish and exact target-start failure.
        // Zero-quads retains this exact map transaction, not a dummy draw.
        drainBatchCanonical_=std::move(canonical);out=next;d.status=S::complete;d.sourcePC=0x249610u;return true;
    }catch(...){return bankFailure(d,S::allocation,0x249610u,"one-record canonical model transaction allocation");}
}
}
