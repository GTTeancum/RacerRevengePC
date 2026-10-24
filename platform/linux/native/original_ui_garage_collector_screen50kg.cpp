#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_canonical_submission50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_screen_text50kg.h"
#include "original_ui_garage_quad50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_widget1_owner50kg.h"
#include "original_ui_garage_text_overlay50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "../../../src/assets/original_standard_material_source50la.h"
#include "../../../src/assets/original_standard_texture_table50la.h"
#include "../../../src/assets/original_standard_texture_resource50la.h"
#include <algorithm>
#include <cstring>
#include <limits>
namespace rr::platform {
namespace {using D=assets::OriginalStandardMaterialDiagnostic50LA;using S=assets::OriginalStandardMaterialStatus50LA;
bool rejectScreen(D& d,S status,std::uint32_t pc,const char* field)noexcept{d.status=status;d.sourcePC=pc;d.field=field;return false;}}
bool OriginalUiGarageDrawContribution50KG::prepareDrainScreenPolicies24B130(D& d)noexcept {
 d={};if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24B130||
   !drainScreenImages_.empty()||!drainScreenPolicies_.empty())return rejectScreen(d,S::phase,0x24B130,"first actual queued screen policies");
 const auto fifo=issuer_->nativeCollectorFifoOwner_;const auto domain=issuer_->standardMaterialDomainOwner_;
 const auto provider=domain?domain->archiveProvider3E1A70():nullptr;
 if(!fifo||!fifo->draining_||!provider||!provider->sealed())return rejectScreen(d,S::phase,0x24B130,"same actual FIFO and archive provider");
 try {
  // Policy/image custody follows the actual constructed owners, not only
  // those whose visibility happened to append a record in the first frame.
  // This routine reads a real sealed registration; it never appends a FIFO
  // record or manufactures a pin for an owner which has not drawn yet.
  const auto preparePolicy=[&](const assets::OriginalStandardMaterialCollectorEntryView50LA& origin,
      const OriginalUiGarageScreenText50KG* text,const OriginalUiGarageQuad50KG* quad,bool queued)->bool {
   if(!origin.material||origin.pass<0||origin.finalLeaf<0||((text==nullptr)==(quad==nullptr)))
       return rejectScreen(d,S::source,0x24B278,"actual retained screen policy owner");
   if(std::any_of(drainScreenPolicies_.begin(),drainScreenPolicies_.end(),[&](const auto& p){return p.registration==origin.finalLeaf;}))return true;
   assets::OriginalStandardMaterialMapBindingView50LA map;
   if(!fifo->table_->prepareColdScreenPrototypeMap249610(fifo->registry_,origin.materialRow,static_cast<std::size_t>(origin.pass),0,map,d))return false;
   if(map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||!map.texture||
      map.representative.registration!=origin.finalLeaf||!map.texture->sourceMember())return rejectScreen(d,S::texture,0x24B278,"actual selected screen texture");
   const auto* source=map.representative.material->source();
   std::shared_ptr<const assets::OriginalUiArchive50KG> materialArchive,textureArchive;
   assets::OriginalUiMemberView50KG materialMember,textureMember;
   if(!source&&map.representative.material->capturedSharedIconName()){
    // Loaded objects retain their original texture winner. Current menu order
    // cannot relabel it or force a new132820 choice.
    if(!map.representative.material->borrowCapturedSharedIconTexture(*map.texture,textureArchive,textureMember,d))return false;
   }else if(!provider->borrowGarageUiMember132820(*map.texture->sourceMember(),textureArchive,textureMember))
      return rejectScreen(d,S::source,0x132820,"winning live canonical UI texture mount");
   if(source){
    if(!source->member()||!source->originalBytes()||
       !provider->borrowGarageUiMember132820(*source->member(),materialArchive,materialMember)||
       source->originalBytes()->size()!=materialMember.size||std::memcmp(source->originalBytes()->data(),materialMember.data,materialMember.size))
       return rejectScreen(d,S::source,0x132820,"winning live canonical UI source mount");
   }else{
    // The private conversion proves a real2475C0 fallback and retains
    // its first family. This is the actual texture archive, never a fake PSM.
    materialArchive=textureArchive;
   }
   const auto* original=map.texture->originalBytes();std::size_t offset=0,bytes=0;
   if(!original||original->size()!=textureMember.size||std::memcmp(original->data(),textureMember.data,textureMember.size)||
      !map.texture->pstExtent24E880(offset,bytes)||!bytes||offset>textureMember.size||bytes>textureMember.size-offset)
      return rejectScreen(d,S::texture,0x24E880,"actual retained PST extent");
   if(text){
    if(queued){OriginalUiGarageScreenText50KG::FontAtlasBorrow atlas;OriginalUiGarageScreenText50KG::Diagnostic td;
     if(!text->borrowQueuedFontAtlas(atlas,td)||atlas.pst.identity!=textureMember.identity||
        !atlas.pst.data||atlas.pst.size!=textureMember.size||atlas.canonicalTexture!=map.textureToken||
        (atlas.pst.data!=textureMember.data&&std::memcmp(atlas.pst.data,textureMember.data,textureMember.size)!=0))
        return rejectScreen(d,S::texture,td.sourcePC?td.sourcePC:0x281F90,"actual retained font atlas");
    }else if(text->phase_!=OriginalUiGarageScreenText50KG::Phase::coldLoaded||!text->coldInputsCurrent()||
        !text->font4C_||text->font4C_->atlasIdentity()!=textureMember.identity||
        text->font4C_->texture281F90()!=map.texture)
        return rejectScreen(d,S::texture,0x281F90,"actual constructed font atlas, not a queued borrow");
   }
   std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context=text?text->context_:quad->context_;
   if(!context||!context->live()||!context->materialApplication()||context->materialApplication()->appStart()!=drainAppStart_)
       return rejectScreen(d,S::phase,0x249BA0,"actual same Application screen template context");
   DrainScreenPolicy next;next.registration=origin.finalLeaf;next.materialArchive=materialArchive;
   assets::OriginalUiMaterialDiagnostic50KG material;
   if(source){
    if(!assets::readOriginalUiMaterial50KG(*materialArchive,materialMember.identity->name,context->materialApplication()->templates(),
        assets::OriginalUiTemplate50KG::screenMt,context->materialRuntime(),next.material,material))return rejectScreen(d,S::source,material.sourcePC,"actual screen PSM policy");
    if(map.representative.pass<0||static_cast<std::size_t>(map.representative.pass)>=next.material.passes.size())return rejectScreen(d,S::range,0x24B278,"actual screen pass ordinal");
   }else if(map.representative.pass!=0)return rejectScreen(d,S::range,0x2475C0,"actual single Garage fallback pass");
   // The immutable bank must contain every real ATM child before upload,
   // not just the child selected by the first frame's animation clock.
   assets::OriginalStandardTextureRowView50LA textureRow;
   const auto* textures=map.representative.material->textures383400();
   if(!textures||!textures->row(map.textureToken,textureRow)||!textureRow.references08||
      *textureRow.references08<=0||!textureRow.object04||!textureRow.object04->constructed24DA30()||
      !textureRow.object04->references00()||*textureRow.object04->references00()<=0)
      return rejectScreen(d,S::texture,0x24B254,"actual retained screen texture prototype");
   const auto* prototype=textureRow.object04;std::int32_t frames=1;
   if(prototype->animated102070()){
    const auto* count=prototype->frameCount20();
    if(!count||*count<=0||*count>64)return rejectScreen(d,S::range,0x102050,"bounded authored screen ATM children");
    frames=*count;
   }
   bool selected=false;
   for(std::int32_t frame=0;frame<frames;++frame){
    const auto* child=prototype->animated102070()?prototype->frame102050(frame):prototype;
    const auto* childBytes=child?child->originalBytes():nullptr;std::size_t childOffset=0,childExtent=0;
    if(!child||!child->constructed24DA30()||!child->references00()||*child->references00()<=0||
       !childBytes||childBytes->size()!=textureMember.size||
       std::memcmp(childBytes->data(),textureMember.data,textureMember.size)||
       !child->pstExtent24E880(childOffset,childExtent)||!childExtent||childOffset>textureMember.size||
       childExtent>textureMember.size-childOffset)
       return rejectScreen(d,S::texture,0x24E880,"actual authored screen child PST extent");
    std::size_t image=0;
    for(;image<drainScreenImages_.size();++image){const auto* p=drainScreenImages_[image]->sourceProvenance(0);
     if(p&&p->memberIdentity==textureMember.identity&&p->pstOffset==childOffset&&p->pstBytes==childExtent)break;}
    std::shared_ptr<assets::OriginalUiScreenAssets50KG> newImage;
    if(image==drainScreenImages_.size()){
     if(image>=86u)return rejectScreen(d,S::overflow,0x24B278,"unchanged128 owner bank capacity");
     newImage=std::make_shared<assets::OriginalUiScreenAssets50KG>();assets::OriginalUiImageSource50KG input;
     // Owner-local identity only. ImageBank authenticates the exact member and
     // child extent, then assigns its unchanged canonical global identity.
     input.identity=static_cast<std::uint64_t>(image+1);input.member=textureMember;
     input.pstOffset=childOffset;input.pstBytes=childExtent;
     assets::OriginalUiScreenAssetDiagnostic50KG decoded;
     if(!newImage->load({input},decoded,drainIpum_.get()))return rejectScreen(d,S::texture,0x24E880,"actual queued screen texture decode");
    }
    const auto images=newImage?std::shared_ptr<const assets::OriginalUiScreenAssets50KG>(newImage):drainScreenImages_[image];
    const auto* decoded=images->decodedTexture(0);
    if(!decoded)return rejectScreen(d,S::texture,0x24E880,"actual screen decoded image");
    assets::OriginalUiNativeMaterial50KG childPolicy;
    if(source){
     if(!assets::prepareOriginalUiNativeMaterial50KG(next.material.passes[static_cast<std::size_t>(map.representative.pass)],*decoded,childPolicy,material))
         return rejectScreen(d,S::unsupportedPass,material.sourcePC,"actual ScreenObject policy");
    }else if(!map.representative.material->prepareGarageScreenFallback2475C0(0,*child,*textureArchive,
        textureMember,*decoded,next.material,childPolicy,d))return false;
    OriginalUiMaterialSpan50KG scalar;OriginalUiMaterialSpanBinding50KG range;range.quadCount=range.drawQuadCount=range.textureCount=1;range.textureIndex=0;
    if(!convertOriginalUiMaterialSpan50KG(childPolicy,range,scalar))return rejectScreen(d,S::unsupportedPass,0x24B278,"screen sampler scalar conversion");
    if(newImage){drainScreenImages_.push_back(std::move(newImage));drainScreenSamplers_.push_back(scalar.native.sampler);}
    const auto owner=static_cast<std::uint32_t>(42u+image);
    if(child==map.texture){
     if(selected||childOffset!=offset||childExtent!=bytes)return rejectScreen(d,S::texture,0x102050,"unique actual selected screen child");
     selected=true;next.imageOwner=owner;next.policy=childPolicy;
    }
   }
   if(!selected)return rejectScreen(d,S::texture,0x102050,"selected screen texture belongs to actual prototype");
   drainScreenArchives_.push_back(std::move(textureArchive));drainScreenArchives_.push_back(std::move(materialArchive));
   drainScreenPolicies_.push_back(std::move(next));
   return true;
  };
  std::size_t ordinal=0;
  for(const auto& record:fifo->records_){const auto currentOrdinal=ordinal++;
   if(record.garageDraw.get()!=this||record.garagePublication!=publication_||record.epoch!=fifo->epoch04_)
       return rejectScreen(d,S::phase,0x24B9B0,"same queued screen lifetime");
   if(record.shared470Owner&&!fifo->shared470RecordCurrent(record))
       return rejectScreen(d,S::phase,0x24B130,"same retained470 payload, epoch and source pins");
   if(record.warning470Dim){if(!prepareDrainWarning470DimPolicy24B130(*record.shared470Owner,d))return false;continue;}
   if(record.garageAperturePage){if(!prepareDrainAperturePolicy24B130(currentOrdinal,d))return false;continue;}
   if(!record.garageText&&!record.garageQuad&&!record.warning470Text&&!record.warning470Quad)continue;
   assets::OriginalStandardMaterialCollectorEntryView50LA origin;
   if(record.originRegistration<0||!fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.originRegistration),origin)||
      origin.finalLeaf!=record.finalRegistration)return rejectScreen(d,S::collector,0x24B278,"actual origin registration");
   if(!preparePolicy(origin,record.warning470Text?record.warning470Text:record.garageText,
       record.warning470Quad?record.warning470Quad:record.garageQuad,true))return false;
  }
  // First-frame visibility is not a material population boundary. The same
  // Page already owns the prompt Quads and text which its opening hook later
  // reveals. Inventory their real primitives before the one immutable upload.
  if(!page_||!page_->sourceEntryReturned1FAE00()||!page_->widgets_||
     !page_->widgets_->pageEntryCustodyCurrent(*page_)||page_->quadCount_!=page_->quads_.size()||
     page_->textCount_!=2||!page_->sharedText440_.value||!page_->costText3340_.value)
      return rejectScreen(d,S::source,0x24B130,"complete same retained Page screen owner population");
  const auto preparePrimitive=[&](std::int32_t row,const OriginalUiGarageScreenText50KG* text,
      const OriginalUiGarageQuad50KG* quad)->bool {
   const assets::OriginalStandardMaterialObject50LA* material=nullptr;
   if(row<0||!fifo->table_->resolve298E60(row,material,d)||!material||!material->passCount48()||*material->passCount48()<0)
       return rejectScreen(d,S::reference,0x246798,"actual constructed screen material reference");
   for(std::int32_t passIndex=0;passIndex<*material->passCount48();++passIndex){
    assets::OriginalStandardMaterialPassView50LA pass;
    assets::OriginalStandardMaterialCollectorEntryView50LA entry;
    if(!material->pass4C(static_cast<std::size_t>(passIndex),pass)||!pass.flags21||
       !fifo->table_->coldCollectorPass(fifo->registry_,row,static_cast<std::size_t>(passIndex),entry))
        return rejectScreen(d,S::collector,0x24A728,"actual retained screen pass registration");
    if(!(*pass.flags21&2u))continue; // The same real disabled-pass branch as collection.
    if(!preparePolicy(entry,text,quad,false))return false;
   }
   return true;
  };
  const auto prepareText=[&](const OriginalUiGarageScreenText50KG* text)->bool {
   if(!text||text->issuer_!=issuer_||text->domain_!=domain||
      text->phase_!=OriginalUiGarageScreenText50KG::Phase::coldLoaded||!text->coldInputsCurrent()||
      !text->count0_||text->publishedPrimitives_!=text->count0_||text->count0_!=text->packetCount5D_||
      text->allocatedPackets_!=text->packetCount5D_||!text->primitives4_||!text->packets64_)
       return rejectScreen(d,S::source,0x2DFAF4,"actual constructed ScreenText primitive population");
   for(std::size_t i=0;i<text->publishedPrimitives_;++i){const auto& primitive=text->primitives4_[i];
    if(!primitive||!primitive->materialStored||!primitive->materialRetained||primitive->type0C!=15||
       primitive->optional04||primitive->optional14||(primitive->flags0F&1u)||
       !primitive->packet10||primitive->packet10!=text->packets64_[i].buffer00.get())
        return rejectScreen(d,S::source,0x246760,"same retained ScreenText primitive and packet");
    if(!preparePrimitive(primitive->material00,text,nullptr))return false;
   }
   return true;
  };
  std::size_t lines=0;
  for(const auto& row:page_->widgets_->rows_)for(const auto& line:row.lines){
   if(!line.owner)continue;
   if(++lines>37u||!line.owner->payloadCurrent())
       return rejectScreen(d,S::source,0x29C850,"same retained Widget TextOverlay payload");
   if(!prepareText(line.owner->screenText30_.get()))return false;
  }
  if(lines!=37u)return rejectScreen(d,S::source,0x1CDA34,"actual 37 retained Widget TextOverlay leaves");
  if(!prepareText(page_->sharedText440_.value)||!prepareText(page_->costText3340_.value))return false;
  for(std::size_t i=0;i<page_->quadCount_;++i){const auto& quad=page_->quads_[i];
   if(quad.domain_!=domain||!quad.packetCurrent2DF550())
       return rejectScreen(d,S::source,0x2DF0B0,"actual retained Page Quad population");
   if(!preparePrimitive(quad.material54->material00,nullptr,&quad))return false;
  }
  // UI470 is globally retained across this manager's frames. Inventory BOTH
  // actual text owners and all strips even if the first frame is connected;
  // the immutable GPU bank must not depend on first-warning visibility.
  const auto warning=issuer_->state?issuer_->state->sharedDialogs:nullptr;
  if(!warning||warning->inputs_.root!=issuer_||!warning->loadedCurrent()||
     warning->inputs_.domain!=domain||!prepareDrainWarning470DimPolicy24B130(*warning,d))
      return rejectScreen(d,S::source,d.sourcePC?d.sourcePC:0x24B130,"same retained470 resource population");
  if(!prepareText(warning->dialog470_.message1B0.get())||!prepareText(warning->dialog470_.alternate220.get()))return false;
  for(const auto& quad:warning->dialog470_.strips){
   if(quad.domain_!=domain||quad.context_!=warning->inputs_.context||!quad.packetCurrent2DF550())
       return rejectScreen(d,S::source,0x2DF0B0,"actual retained470 strip population");
   if(!preparePrimitive(quad.material54->material00,nullptr,&quad))return false;
  }
  return true;
 }catch(...){return rejectScreen(d,S::allocation,0x24B130,"retained screen policy/image owners");}
}
bool OriginalUiGarageDrawContribution50KG::warning470DrainReceiverCurrent(
    const void* receiver,std::int32_t origin,std::int32_t final)const noexcept {
 const auto fifo=issuer_?issuer_->nativeCollectorFifoOwner_:nullptr;
 if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||(sourcePC_!=0x24B26C&&sourcePC_!=0x249610&&sourcePC_!=0x249BA0)||!fifo||!fifo->draining_||
    drainActiveRecordOrdinal_>=fifo->records_.size())return false;
 auto it=fifo->records_.begin();for(std::size_t i=0;i<drainActiveRecordOrdinal_;++i)++it;const auto& record=*it;
 if(!record.shared470Owner)return false;
 if(!fifo->shared470RecordCurrent(record)||record.garageDraw.get()!=this||record.garagePublication!=publication_||
    record.epoch!=drainEpoch_||record.originRegistration!=origin||record.finalRegistration!=final)return false;
 const void* exact=record.warning470Text?static_cast<const void*>(record.warning470Text):
    record.warning470Quad?static_cast<const void*>(record.warning470Quad):
    static_cast<const void*>(record.shared470Owner->dialog470_.dim160.get());
 return receiver&&receiver==exact;
}
bool OriginalUiGarageDrawContribution50KG::warning470DrainBindingCurrent(
    const void* receiver,std::int32_t origin,std::int32_t final)const noexcept {
 const auto fifo=issuer_?issuer_->nativeCollectorFifoOwner_:nullptr;
 if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24B26C||!fifo||!fifo->draining_||
    drainActiveRecordOrdinal_>=fifo->records_.size())return false;
 auto it=fifo->records_.begin();for(std::size_t i=0;i<drainActiveRecordOrdinal_;++i)++it;const auto& record=*it;
 // Ordinary families keep their independent receiver contracts. This does
 // not make a Page/TextOverlay predicate accept an actual470 payload.
 if(!record.shared470Owner)return !record.warning470Text&&!record.warning470Quad&&!record.warning470Dim;
 return warning470DrainReceiverCurrent(receiver,origin,final);
}
bool OriginalUiGarageDrawContribution50KG::lowerAndBindQueuedScreen24B26C(std::size_t ordinal,D& d)noexcept {
 d={};const auto fifo=issuer_->nativeCollectorFifoOwner_;
 if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24B26C||!fifo||!fifo->draining_||
    ordinal>=fifo->records_.size()||!drainResourcesBound_||drainBatchPending_)return rejectScreen(d,S::phase,0x24B26C,"actual queued screen cursor");
 auto it=fifo->records_.begin();for(std::size_t i=0;i<ordinal;++i)++it;const auto& record=*it;
 if(record.garageDraw.get()!=this||record.garagePublication!=publication_||record.epoch!=drainEpoch_||
    record.garageModel||(!record.garageText&&!record.garageQuad&&!record.garageAperturePage&&!record.warning470Text&&!record.warning470Quad&&!record.warning470Dim))return rejectScreen(d,S::phase,0x24B26C,"actual ScreenObject record");
 if(record.shared470Owner&&!fifo->shared470RecordCurrent(record))
     return rejectScreen(d,S::phase,0x24B26C,"same retained470 payload, epoch and source pins");
 try {
  const void* receiver=nullptr;std::uint32_t quads=0;
  const auto* textOwner=record.warning470Text?record.warning470Text:record.garageText;
  const auto* quadOwner=record.warning470Quad?record.warning470Quad:record.garageQuad;
  if(textOwner){OriginalUiGarageScreenText50KG::LoweredPacket next;OriginalUiGarageScreenText50KG::Diagnostic text;
    if(!textOwner->lowerQueuedPacket(record.textPrimitive,sourceXyOffset_,next,text))return rejectScreen(d,S::source,text.sourcePC,"actual ScreenText packet lowering");
    receiver=textOwner;quads=next.quadCount;drainScreenVertices_=std::move(next.vertices);drainScreenStq_=std::move(next.stq);
    drainScreenW_=std::move(next.rasterW);drainScreenFog_=std::move(next.fog);
  }else if(quadOwner){OriginalUiGarageQuad50KG::LoweredPacket327E90 next;OriginalUiGarageQuad50KG::Diagnostic quad;
    if(!quadOwner->lowerPacket327E90(record.warning470Quad?record.warning470PacketRevision:record.quadPacketRevision,sourceXyOffset_,next,quad))return rejectScreen(d,S::source,quad.sourcePC,"actual Quad packet lowering");
    receiver=quadOwner;quads=next.quadCount;drainScreenVertices_.assign(next.vertices.begin(),next.vertices.end());
    drainScreenStq_.assign(next.stq.begin(),next.stq.end());drainScreenW_.assign(next.rasterW.begin(),next.rasterW.end());drainScreenFog_.assign(next.fog.begin(),next.fog.end());}
  if(record.garageAperturePage){OriginalUiGarageQuad50KG::LoweredPacket327E90 next;OriginalUiGaragePageOwner50KG::Diagnostic aperture;
    if(record.garageAperturePage!=page_||!page_->lowerAperture327E90(record.apertureOrdinal,record.apertureRevision,sourceXyOffset_,next,aperture))
        return rejectScreen(d,S::source,aperture.sourcePC?aperture.sourcePC:0x327E90,"actual pinned Aperture packet lowering");
    receiver=page_->apertures_[record.apertureOrdinal].get();quads=next.quadCount;
    drainScreenVertices_.assign(next.vertices.begin(),next.vertices.end());drainScreenStq_.assign(next.stq.begin(),next.stq.end());
    drainScreenW_.assign(next.rasterW.begin(),next.rasterW.end());drainScreenFog_.assign(next.fog.begin(),next.fog.end());}
  if(record.warning470Dim){OriginalUiGarageQuad50KG::LoweredPacket327E90 next;OriginalUiSharedDialogs50KG::Diagnostic dim;
    if(!record.shared470Owner->lowerWarning470Dim327E90(record.warning470PacketRevision,sourceXyOffset_,next,dim))
        return rejectScreen(d,S::source,dim.sourcePC?dim.sourcePC:0x327E90,"actual pinned470 Dim160 packet lowering");
    receiver=record.shared470Owner->dialog470_.dim160.get();quads=next.quadCount;
    drainScreenVertices_.assign(next.vertices.begin(),next.vertices.end());drainScreenStq_.assign(next.stq.begin(),next.stq.end());
    drainScreenW_.assign(next.rasterW.begin(),next.rasterW.end());drainScreenFog_.assign(next.fog.begin(),next.fog.end());}
  if(!quads||quads>originalUiMaximumSourceQuads50KM||
     drainCommittedQuads_>std::numeric_limits<std::uint32_t>::max()-quads)
      return rejectScreen(d,S::overflow,0x24B26C,"actual batch geometry and cumulative accounting bounds");
  const auto policy=std::find_if(drainScreenPolicies_.begin(),drainScreenPolicies_.end(),[&](const auto& p){return p.registration==record.finalRegistration;});
  if(policy==drainScreenPolicies_.end())return rejectScreen(d,S::source,0x249610,"same prepared screen material owner");
  assets::OriginalStandardMaterialCollectorEntryView50LA origin;
  if(record.originRegistration<0||!fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.originRegistration),origin))return rejectScreen(d,S::collector,0x24B278,"actual origin registration");
  const assets::OriginalUiImageBankMapping50KG* mapped=nullptr;
  assets::OriginalUiNativeMaterial50KG currentPolicy=policy->policy;
  if(!policy->untextured){
   assets::OriginalStandardMaterialMapBindingView50LA current;
   if(origin.pass<0||!fifo->table_->prepareColdScreenPrototypeMap249610(fifo->registry_,origin.materialRow,static_cast<std::size_t>(origin.pass),0,current,d))return false;
   assets::OriginalStandardTextureRowView50LA textureRow;
   const auto* textures=current.representative.material?current.representative.material->textures383400():nullptr;
   if(current.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||!current.texture||
      current.representative.registration!=record.finalRegistration||!textures||
      !textures->row(current.textureToken,textureRow)||!textureRow.references08||*textureRow.references08<=0||
      !textureRow.object04||!textureRow.object04->constructed24DA30()||
      !textureRow.object04->references00()||*textureRow.object04->references00()<=0)
      return rejectScreen(d,S::texture,0x24B254,"actual current screen texture prototype");
   const auto* prototype=textureRow.object04;std::int32_t frames=1;
   if(prototype->animated102070()){
    const auto* count=prototype->frameCount20();
    if(!count||*count<=0||*count>64)return rejectScreen(d,S::range,0x102050,"bounded current screen ATM children");
    frames=*count;
   }
   bool selected=false;
   for(std::int32_t frame=0;frame<frames;++frame){
    const auto* child=prototype->animated102070()?prototype->frame102050(frame):prototype;
    if(child==current.texture){
     if(selected)return rejectScreen(d,S::texture,0x102050,"unique current screen child");
     selected=true;
    }
   }
   if(!selected)return rejectScreen(d,S::texture,0x102050,"selected current screen child inventory");
   // The original policy owner pins the canonical member, not the first
   // selected child forever. Match the live child against cold-preloaded
   // one-image owners by that exact member identity and its full PST extent.
   const auto* anchor=drainImageBank_->mapping(policy->imageOwner,0);
   const auto member=anchor?drainImageBank_->sourceMember(anchor->combinedIndex):assets::OriginalUiMemberView50KG{};
   const auto* original=current.texture->originalBytes();std::size_t offset=0,bytes=0;
   if(!member.identity||!member.data||!original||original->size()!=member.size||
      std::memcmp(original->data(),member.data,member.size)||!current.texture->pstExtent24E880(offset,bytes)||
      !bytes||offset>member.size||bytes>member.size-offset||drainScreenImages_.size()>86u)
      return rejectScreen(d,S::texture,0x24B254,"same retained screen member and current child extent");
   for(std::size_t image=0;image<drainScreenImages_.size();++image){
    const auto* provenance=drainScreenImages_[image]->sourceProvenance(0);
    if(provenance&&provenance->memberIdentity==member.identity&&provenance->pstOffset==offset&&provenance->pstBytes==bytes){
     if(mapped)return rejectScreen(d,S::texture,0x102050,"unique retained current child image");
     mapped=drainImageBank_->mapping(static_cast<std::uint32_t>(42u+image),0);
     if(!mapped)return rejectScreen(d,S::texture,0x249610,"actual current child image remap");
    }
   }
   if(!mapped)return rejectScreen(d,S::texture,0x102050,"current child was authentically preloaded");
   // Re-derive the ordinary policy from the actual selected decoded child.
   // The following canonical stage still checks full member/extent/pixels and
   // every retained GS/sampler field against that same live selection.
   if(policy->material.identity){
    const auto bankImages=drainImageBank_->images();const auto* decoded=bankImages?bankImages->decodedTexture(mapped->combinedIndex):nullptr;
    assets::OriginalUiMaterialDiagnostic50KG material;
    if(!decoded||current.representative.pass<0||static_cast<std::size_t>(current.representative.pass)>=policy->material.passes.size()||
       !assets::prepareOriginalUiNativeMaterial50KG(policy->material.passes[static_cast<std::size_t>(current.representative.pass)],*decoded,currentPolicy,material))
       return rejectScreen(d,S::unsupportedPass,material.sourcePC,"actual current ScreenObject child policy");
   }
  }
  const auto images=drainImageBank_->images();
  if((!policy->untextured&&!mapped)||!images||!images->textures())return rejectScreen(d,S::texture,0x249610,"actual screen image remap");
  drainBatchCanonical_=std::shared_ptr<OriginalUiGarageCanonicalSubmission50KG>(new OriginalUiGarageCanonicalSubmission50KG);
  if(!drainBatchCanonical_->initialize(fifo->table_,fifo->registry_,drainImageBank_,*this,publication_,d))return false;
  OriginalUiMaterialSpanBinding50KG binding;binding.quadCount=binding.drawQuadCount=quads;binding.textureIndex=mapped?mapped->combinedIndex:0;binding.textureCount=images->textures()->textureCount();
  assets::OriginalStandardMaterialMapBindingView50LA map;sourcePC_=0x249610;
  assets::OriginalStandardMaterialPassView50LA originPass;
  if(origin.pass<0||!origin.material||!origin.material->pass4C(static_cast<std::size_t>(origin.pass),originPass)||
     record.originParameters!=originPass.parameters34||record.textureBase!=originPass.textureAnimation10||
     record.vertexBase||originPass.animation10||!originPass.parameterCount28||!originPass.parameterMask24||
     *originPass.parameterCount28||*originPass.parameterMask24||(record.originParameters&&!record.originParameters->empty()))
      return rejectScreen(d,S::unsupportedPass,0x24B4E4,"originating screen VU overlay or animation");
  if(policy->untextured){sourcePC_=0x249BA0;
    if(record.warning470Dim){
     if(!drainBatchCanonical_->stageWarning470UntexturedScreenPass(*this,origin.materialRow,static_cast<std::uint32_t>(origin.pass),
         receiver,*policy->materialArchive,policy->material,policy->untexturedPolicy,{0,quads,quads},drainActiveSpan_,map,d))return false;
    }else if(!record.garageAperturePage||!drainBatchCanonical_->stageUntexturedScreenPass(origin.materialRow,static_cast<std::uint32_t>(origin.pass),
        receiver,*policy->materialArchive,policy->material,policy->untexturedPolicy,{0,quads,quads},drainActiveSpan_,map,d))return false;
  }else if(record.shared470Owner){
    if(!drainBatchCanonical_->stageWarning470ScreenPass(*this,origin.materialRow,static_cast<std::uint32_t>(origin.pass),receiver,
        *policy->materialArchive,policy->material,currentPolicy,binding,drainActiveSpan_,map,d))return false;
  }else if(!drainBatchCanonical_->stageScreenPass(origin.materialRow,static_cast<std::uint32_t>(origin.pass),receiver,*policy->materialArchive,policy->material,currentPolicy,
      binding,drainActiveSpan_,map,d))return false;
  auto& batch=drainBatchNative_;batch={};batch.draw.textures=drainGpu_.get();batch.draw.texturePublication=drainGpu_->publicationToken();
  batch.draw.vertices=drainScreenVertices_.data();batch.draw.quadCount=quads;batch.draw.spans=&drainActiveSpan_.span;batch.draw.spanCount=1;
  batch.draw.canvasWidth=640;batch.draw.canvasHeight=448;batch.states=&drainActiveSpan_.native;batch.stateCount=1;
  //ScreenObject's actual Q1 reaches ordinary S/T interpolation. Do not opt a
  //font/sprite into the unrelated3D raw-STQ shader solely because it has STQ.
  sourcePC_=0x24B26C;return true;
 }catch(...){return rejectScreen(d,S::allocation,0x24B26C,"owned screen packet geometry");}
}
}
