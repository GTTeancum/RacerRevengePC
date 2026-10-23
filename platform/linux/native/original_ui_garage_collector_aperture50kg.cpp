#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_session50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "../../../src/assets/original_standard_material_source50la.h"
#include <algorithm>
#include <cstring>
#include <iterator>
namespace rr::platform {
namespace {using D=assets::OriginalStandardMaterialDiagnostic50LA;using S=assets::OriginalStandardMaterialStatus50LA;
bool reject(D& d,S status,std::uint32_t pc,const char* field)noexcept{d.status=status;d.sourcePC=pc;d.field=field;return false;}}
bool OriginalUiGarageDrawContribution50KG::readApertureInputs2E21D0(const OriginalUiGaragePageOwner50KG& page,
    std::size_t ordinal,ApertureRenderInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x2E21D0;
    if(!identityCurrent()||phase_!=Phase::thirdPass||issuer_->sourceApplicationMode22C_!=5||
       page_.get()!=&page||callback_!=&page.entity_||!page.drawPending_||page.drawPublication_!=publication_||
       page.drawSourcePC_!=0x1D798C||page.sharedApertureOrdinal_!=ordinal||ordinal>=2||!page.apertures_[ordinal])return false;
    ApertureRenderInputs next;next.publication=publication_;
    next.sourceApertureEnabled383680=issuer_->entities()->apertureRenderEnabled383680_!=0;
    if(!next.sourceApertureEnabled383680){out=next;pc=0;return true;}
    if(!cameraFrame_.current()||!camera_->fields_.coreKnown||!renderer_->display_->video_||
       !renderer_->display_->video_->host.current())return false;
    const auto& f=camera_->fields_;if(!f.viewport1E0[2]||!f.viewport1E0[3])return false;
    next.viewportWidth1E8=f.viewport1E0[2];next.viewportHeight1EC=f.viewport1E0[3];
    next.videoDepthMaximum60=renderer_->display_->video_->packetZMaximum;out=next;pc=0;return true;
}
bool OriginalUiGarageDrawContribution50KG::collectAperturePrimitive246760(OriginalUiGaragePageOwner50KG& page,
    std::size_t ordinal,D& d)noexcept {
    d={};const auto fifo=issuer_?issuer_->nativeCollectorFifoOwner_:nullptr;
    if(!identityCurrent()||phase_!=Phase::thirdPass||issuer_->sourceApplicationMode22C_!=5||page_.get()!=&page||
       callback_!=&page.entity_||!page.drawPending_||page.drawPublication_!=publication_||page.drawSourcePC_!=0x1D798C||
       page.sharedApertureOrdinal_!=ordinal||page.apertureDrawOrdinal_!=ordinal||page.apertureDrawPublication_!=publication_||
       page.apertureDrawSourcePC_!=0x2E23DC||!fifo)return reject(d,S::phase,0x2E23DC,"actual retained Aperture call cursor");
    sourcePC_=0x2E23DC;return fifo->collectGarageAperture246760(*this,page,ordinal,d);
}
bool OriginalUiGarageDrawContribution50KG::prepareDrainAperturePolicy24B130(std::size_t ordinal,D& d)noexcept {
    d={};const auto fifo=issuer_->nativeCollectorFifoOwner_;
    if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24B130||!fifo||!fifo->draining_||ordinal>=fifo->records_.size())
        return reject(d,S::phase,0x24B130,"actual Aperture policy preparation");
    auto it=fifo->records_.begin();std::advance(it,ordinal);const auto& record=*it;
    if(record.garageAperturePage!=page_||record.garageDraw.get()!=this||record.garagePublication!=publication_||record.epoch!=fifo->epoch04_)
        return reject(d,S::phase,0x24B130,"same retained Aperture record");
    if(std::any_of(drainScreenPolicies_.begin(),drainScreenPolicies_.end(),[&](const auto& p){return p.registration==record.finalRegistration;}))return true;
    OriginalUiGaragePageOwner50KG::AperturePacketBorrow borrow;
    if(!page_->borrowAperturePacket2E21D0(record.apertureOrdinal,borrow)||borrow.primitive!=record.primitive04||borrow.revision!=record.apertureRevision)
        return reject(d,S::source,0x2E23DC,"actual queued Aperture packet");
    const auto& primitive=*borrow.primitive;const auto context=primitive.context;
    assets::OriginalStandardMaterialCollectorEntryView50LA final;
    if(record.finalRegistration<0||!fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.finalRegistration),final)||
       !final.material||final.pass<0||primitive.domain!=issuer_->standardMaterialDomainOwner_||!context||!context->live()||
       !context->materialApplication()||context->materialApplication()->appStart()!=drainAppStart_)
        return reject(d,S::reference,0x24B130,"actual NoTexture canonical owner");
    const auto* source=final.material->source();const auto provider=primitive.domain->archiveProvider3E1A70();
    DrainScreenPolicy next;next.registration=record.finalRegistration;next.untextured=true;assets::OriginalUiMemberView50KG member;
    if(!source||!source->member()||!source->originalBytes()||!provider||!provider->sealed()||
       !provider->borrowGarageUiMember132820(*source->member(),next.materialArchive,member)||
       source->originalBytes()->size()!=member.size||std::memcmp(source->originalBytes()->data(),member.data,member.size))
        return reject(d,S::source,0x132820,"winning NoTexture source PSM");
    assets::OriginalUiMaterialDiagnostic50KG material;
    if(!assets::readOriginalUiMaterial50KG(*next.materialArchive,member.identity->name,context->materialApplication()->templates(),
       assets::OriginalUiTemplate50KG::screenMt,context->materialRuntime(),next.material,material)||
       static_cast<std::size_t>(final.pass)>=next.material.passes.size()||
       !assets::prepareOriginalUiNativeUntexturedMaterial50KG(next.material.passes[static_cast<std::size_t>(final.pass)],next.untexturedPolicy,material))
        return reject(d,S::unsupportedPass,material.sourcePC?material.sourcePC:0x249BA0,"actual zero-map ScreenObject policy");
    try{drainScreenPolicies_.push_back(std::move(next));}catch(...){return reject(d,S::allocation,0x24B130,"retained NoTexture policy");}
    return true;
}
}
namespace original_main_entry50KG {
bool NativeCollectorFifo50LA::collectGarageAperture246760(rr::platform::OriginalUiGarageDrawContribution50KG& draw,
    rr::platform::OriginalUiGaragePageOwner50KG& page,std::size_t ordinal,rr::assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
    using S=rr::assets::OriginalStandardMaterialStatus50LA;d={};const auto fail=[&](S status,std::uint32_t pc){d.status=status;d.sourcePC=pc;return false;};
    if(!application_||!retainGarageRegistry(*application_,d))return false;
    auto self=draw.retainSelfForFifo();rr::platform::OriginalUiGaragePageOwner50KG::AperturePacketBorrow borrow;
    if(!self||draw.issuer_!=application_||draw.page_.get()!=&page||ordinal>=2||!page.apertures_[ordinal]||
       page.apertures_[ordinal]->nativeGarageFifoPins_||!page.borrowAperturePacket2E21D0(ordinal,borrow)||!borrow.primitive||!borrow.packet||!borrow.revision)
        return fail(S::phase,0x2E23DC);
    const auto& primitive=*borrow.primitive;
    if(primitive.domain!=application_->standardMaterialDomainOwner_||!primitive.materialStored||!primitive.materialRetained||primitive.material00<0||
       primitive.packet10!=borrow.packet||primitive.optional04||primitive.optional14||(primitive.flags0F&1u)||
       !application_->materialRuntime.callbackOverrideKnown||application_->materialRuntime.callbackOverride435678)
        return fail(S::source,0x246760);
    const rr::assets::OriginalStandardMaterialObject50LA* material=nullptr;
    if(!table_->resolve298E60(primitive.material00,material,d)||!material||!material->passCount48()||*material->passCount48()<0)return fail(S::reference,0x246798);
    const auto initial=garageRecordCount_;
    for(std::int32_t i=0;i<*material->passCount48();++i){
        rr::assets::OriginalStandardMaterialPassView50LA pass;rr::assets::OriginalStandardMaterialCollectorEntryView50LA entry;
        if(!material->pass4C(static_cast<std::size_t>(i),pass)||!pass.flags21||!table_->coldCollectorPass(registry_,primitive.material00,static_cast<std::size_t>(i),entry))return fail(S::collector,0x24A728);
        if(!(*pass.flags21&2u))continue;
        if(!table_->serviceRetainedScreenPrototype24A740(registry_,primitive.material00,static_cast<std::size_t>(i),d))return false;
        Record record;record.garageDraw=self;record.garagePublication=draw.publication_;record.garageAperturePage=draw.page_;
        record.apertureOrdinal=ordinal;record.apertureRevision=borrow.revision;record.primitive04=&primitive;
        record.originParameters=pass.parameters34;record.textureBase=pass.textureAnimation10;
        if(!appendGarage24B9B0(std::move(record),entry.registration,d))return false;
        d.sourceEffectsCommitted=garageRecordCount_!=initial;
    }
    d.sourcePC=0x24680C;return true;
}
}
