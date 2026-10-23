#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_model_resource50kg.h"
#include "original_ui_garage_screen_text50kg.h"
#include "original_ui_garage_quad50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_text_overlay50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include <algorithm>
#include <limits>
namespace original_main_entry50KG {
namespace {
using D=rr::assets::OriginalStandardMaterialDiagnostic50LA;
using S=rr::assets::OriginalStandardMaterialStatus50LA;
bool reject(D& d,S s,std::uint32_t pc)noexcept{d.status=s;d.sourcePC=pc;return false;}
struct RetainAppendEffects {D& diagnostic;const std::size_t& count;std::size_t initial;
    ~RetainAppendEffects(){diagnostic.sourceEffectsCommitted|=count!=initial;}};
}
bool Application::constructUiCollector1B73BC(D& d)const noexcept {
    d={};const auto display=raceDisplayOwner50LA_.lock();
    if(uiCollector1B73BCAttempted_||nativeCollectorFifoOwner_||!entitiesOwner_||!entitiesOwner_->application10()||
       !display||display->application_!=this||!display->canRetain()||
       !display->ownerThread()||!standardMaterialDomainOwner_||
       !standardMaterialDomainOwner_->prepared())return reject(d,S::phase,0x1B73BC);
    const auto table=standardMaterialDomainOwner_->materials382B38();
    if(!table||!table->prepared()||!NativeCollectorFifo50LA::initialTableCurrent(*table))
        return reject(d,S::collector,0x1B73BC);
    // Explicit 1B7300 startup phase, never a lazy draw getter. Native canonical
    // entries may already exist without a FIFO: bind their actual current
    // receipt, with no old FIFO, pins or queued work. This native ordering does
    // not repeat 14D440, clear live entries, or invent file/cache work at132D50.
    uiCollector1B73BCAttempted_=true;
    try{
        auto next=std::shared_ptr<NativeCollectorFifo50LA>(new NativeCollectorFifo50LA(*this,table));
        if(!next->bindUiTable1B73BC())return reject(d,S::collector,0x1B73BC);
        next->uiConstructor1B73BC_=true;
        const auto receipt=standardMaterialDomainOwner_->coldCollector14D440();
        if(table->coldCollectorCurrent(receipt))next->registry_=receipt;
        nativeCollectorFifoOwner_=std::move(next);
        d.sourcePC=0x24B718;d.sourceEffectsCommitted=true;return true;
    }catch(...){return reject(d,S::allocation,0x1B73AC);}
}
bool NativeCollectorFifo50LA::retainGarageRegistry(const Application& app,D& d)noexcept {
    if(&app!=application_||app.nativeCollectorFifoOwner_.get()!=this||closed_||draining_||!uiConstructor1B73BC_||
       ownerThread_!=std::this_thread::get_id()||!table_||!app.standardMaterialDomainOwner_||
       app.standardMaterialDomainOwner_->materials382B38()!=table_||records_.size()!=garageRecordCount_)
        return reject(d,S::phase,0x24B9B0);
    const auto receipt=app.standardMaterialDomainOwner_->coldCollector14D440();
    if(!table_->coldCollectorCurrent(receipt)||(!records_.empty()&&!table_->coldCollectorCurrent(registry_)))
        return reject(d,S::collector,0x24A790);
    if(records_.empty())registry_=receipt;
    return true;
}
bool NativeCollectorFifo50LA::garageQueueOwnerCurrent(
    const rr::platform::OriginalUiGarageDrawContribution50KG& owner,std::uint64_t publication)const noexcept {
    for(const auto& r:records_)if(r.garageDraw.get()==&owner&&r.garagePublication==publication)return true;
    return false;
}
bool NativeCollectorFifo50LA::appendGarage24B9B0(Record&& pending,std::int32_t registration,D& d)noexcept {
    if(!application_||!retainGarageRegistry(*application_,d))return false;
    rr::assets::OriginalStandardMaterialCollectorEntryView50LA origin,final;
    if(!pending.garageDraw||!pending.garagePublication||registration<0||
       !table_->coldCollectorEntry(registry_,static_cast<std::size_t>(registration),origin)||origin.finalLeaf<0||
       !table_->coldCollectorEntry(registry_,static_cast<std::size_t>(origin.finalLeaf),final)||
       (unsigned(bool(pending.garageModel))+unsigned(bool(pending.garageText))+unsigned(bool(pending.garageQuad))+unsigned(bool(pending.garageAperturePage))+unsigned(bool(pending.garageParticles))!=1u))return reject(d,S::reference,0x24B9B0);
    for(const auto& r:records_)if(r.garageDraw!=pending.garageDraw||r.garagePublication!=pending.garagePublication)
        return reject(d,S::phase,0x24B9B0);
    if(pending.garageAperturePage&&(pending.apertureOrdinal>=pending.garageAperturePage->apertures_.size()||
       !pending.garageAperturePage->apertures_[pending.apertureOrdinal]||
       pending.garageAperturePage->apertures_[pending.apertureOrdinal]->nativeGarageFifoPins_==std::numeric_limits<std::uint32_t>::max()))return reject(d,S::overflow,0x105720);
    if(pending.garageParticles&&pending.garageParticles->nativeGarageFifoPins_==std::numeric_limits<std::uint32_t>::max())
        return reject(d,S::overflow,0x105720);
    if((pending.garageModel&&pending.garageModel->nativeGarageFifoPins_==std::numeric_limits<std::uint32_t>::max())||
       (pending.garageText&&pending.garageText->nativeGarageFifoPins_==std::numeric_limits<std::uint32_t>::max())||
       (pending.garageQuad&&pending.garageQuad->nativeGarageFifoPins_==std::numeric_limits<std::uint32_t>::max()))
        return reject(d,S::overflow,0x105720);
    if(!table_->pinNativeCollector(registry_))return reject(d,S::collector,0x29C150);
    try{records_.push_back(std::move(pending));}catch(...){table_->unpinNativeCollector();return reject(d,S::allocation,0x105720);}
    auto& record=records_.back();void* previous=nullptr;
    if(!table_->appendNativeCollector29C150(registry_,static_cast<std::size_t>(registration),epoch04_,&record,previous)){
        records_.pop_back();table_->unpinNativeCollector();return reject(d,S::collector,0x29C150);
    }
    //Actual29C218/21C allocation and final-leaf append. Redirects retain the
    //ORIGIN parameters at24BA0C, not a representative's foreign parameters.
    if(previous)static_cast<Record*>(previous)->next=&record;
    record.originRegistration=registration;record.finalRegistration=origin.finalLeaf;
    record.epoch=epoch04_;record.scissor18=scissor4014_.value_;
    if(record.garageParticles)++record.garageParticles->nativeGarageFifoPins_;
    if(record.garageModel)++record.garageModel->nativeGarageFifoPins_;
    if(record.garageText)++record.garageText->nativeGarageFifoPins_;
    if(record.garageQuad)++record.garageQuad->nativeGarageFifoPins_;
    if(record.garageAperturePage)++record.garageAperturePage->apertures_[record.apertureOrdinal]->nativeGarageFifoPins_;
    ++garageRecordCount_;d.sourceEffectsCommitted=true;d.sourcePC=0x24BA28;return true;
}
bool NativeCollectorFifo50LA::collectGarageModel241280(
    rr::platform::OriginalUiGarageDrawContribution50KG& draw,rr::platform::OriginalUiGarageModelResource50KG& model,D& d)noexcept {
    d={};RetainAppendEffects effects{d,garageRecordCount_,garageRecordCount_};
    if(!application_||!retainGarageRegistry(*application_,d))return false;
    const auto self=draw.retainSelfForFifo();const auto held=model.nativeSharedSelf_.lock();
    const auto revision=model.nativeGaragePacketRevision();auto& fields=model.scene_.fields();
    if(!self||!draw.identityCurrent()||draw.issuer_!=application_||held.get()!=&model||!revision||
       draw.phase_!=decltype(draw.phase_)::modelRender114E00||draw.sourcePC_!=0x114E00||
       model.application_!=application_->entitiesOwner_||model.domain_!=application_->standardMaterialDomainOwner_||
       model.nativeGarageFifoPins_||!model.geometry_||fields.materialCount1C0<=0||!model.materials_.count1C0())
        return reject(d,S::phase,0x2416A4);
    if(!application_->materialRuntime.callbackOverrideKnown||application_->materialRuntime.callbackOverride435678)
        return reject(d,S::runtime,0x24A674);
    const auto lod=static_cast<std::size_t>(fields.field1B0);
    const auto count=model.geometry_->surfaceCount26E2B8(lod);
    if(!count||!model.geometry_->surface246300(lod,0))return reject(d,S::source,0x24178C);
    const bool hasPalette=model.geometry_->hasPalette20();
    if(hasPalette&&fields.field1C8)return reject(d,S::source,0x241768);
    if(!(fields.flags1B3&8u)&&!hasPalette){
        const auto* first=model.materials_.handle1C4(0);if(!first)return reject(d,S::reference,0x2416C0);
        bool same=true;
        for(std::int32_t i=1;i<fields.materialCount1C0;++i){const auto* next=model.materials_.handle1C4(static_cast<std::size_t>(i));
            if(!next)return reject(d,S::reference,0x2416D8);if(*first!=*next){same=false;break;}}
        if(same){const rr::assets::OriginalStandardMaterialObject50LA* material=nullptr;
            if(!table_->resolve298E60(*first,material,d)||!material||!material->passCount48())return reject(d,S::reference,0x24170C);
            if(*material->passCount48()==1){rr::assets::OriginalStandardMaterialPassView50LA pass;
                if(!material->pass4C(0,pass))return reject(d,S::reference,0x241724);
                if(!pass.animation10&&!pass.textureAnimation10){rr::assets::OriginalStandardMaterialCollectorEntryView50LA entry;
                    if(!table_->coldCollectorPass(registry_,*first,0,entry)||entry.registration<0)return reject(d,S::collector,0x241748);
                    const auto target=table_->nativeCollectorEntry29C0B0(registry_,static_cast<std::size_t>(entry.registration));
                    if(!target)return reject(d,S::collector,0x241748);
                    model.nativeCollector1C8Owner_=application_->nativeCollectorFifoOwner_;model.nativeCollector1C8Receipt_=registry_;
                    model.nativeCollector1C8Registration_=entry.registration;fields.field1C8=target; //Real24174C.
                }
            }
        }
        fields.flags1B3=static_cast<std::uint8_t>(fields.flags1B3|8u); //241764, after packet20 capture.
    }else if(hasPalette){
        //2415B4..5DC only sets the cache bit then jumps241768. It NEVER
        //executes the direct-model24174C publication. Constructor1C8 is null.
        fields.flags1B3=static_cast<std::uint8_t>(fields.flags1B3|8u);
    }
    auto append=[&](std::size_t ordinal,std::size_t packet,std::int32_t registration,
        const rr::assets::OriginalStandardMaterialPassView50LA& pass,bool array)noexcept {
        const auto* surface=model.geometry_->surface246300(lod,ordinal);if(!surface)return reject(d,S::source,0x2417B0);
        Record record;record.garageDraw=self;record.garagePublication=draw.publication_;record.garageModel=held;
        record.matrixRevision=revision;record.lod=lod;record.surface=ordinal;record.packetIndex=packet;
        record.surface04=surface;record.attributeArray1F0=array;
        record.attributes08=array?static_cast<const void*>(&model.packets_):static_cast<const void*>(model.packets_.packet(packet));
        record.originParameters=pass.parameters34;record.textureBase=pass.textureAnimation10;
        if(!record.attributes08)return reject(d,S::source,0x241824);
        return appendGarage24B9B0(std::move(record),registration,d);
    };
    if(fields.field1C8){
        const auto owner=model.nativeCollector1C8Owner_.lock();const auto index=model.nativeCollector1C8Registration_;
        rr::assets::OriginalStandardMaterialCollectorEntryView50LA entry;rr::assets::OriginalStandardMaterialPassView50LA pass;
        if(owner.get()!=this||index<0||!table_->coldCollectorCurrent(model.nativeCollector1C8Receipt_)||
           table_->nativeCollectorEntry29C0B0(model.nativeCollector1C8Receipt_,static_cast<std::size_t>(index))!=fields.field1C8||
           !table_->coldCollectorEntry(registry_,static_cast<std::size_t>(index),entry)||!entry.material||
           !entry.material->pass4C(static_cast<std::size_t>(entry.pass),pass))return reject(d,S::collector,0x241798);
        //Exactly one FIRST runtime Surface and the actual sole direct packet.
        //Palette owners cannot publish1C8 under this genuine constructor.
        return append(0,0,index,pass,false);
    }
    for(std::size_t ordinal=0;ordinal<count;++ordinal){
        const auto* surface=model.geometry_->surface246300(lod,ordinal);
        if(!surface||!surface->material00||*surface->material00<0)return reject(d,S::source,0x2417C8);
        const auto* override=model.materials_.handle1C4(static_cast<std::size_t>(*surface->material00));
        if(!override)return reject(d,S::reference,0x2417D8);
        const auto row=*override==-1?*surface->material00:*override;
        const rr::assets::OriginalStandardMaterialObject50LA* material=nullptr;
        if(!table_->resolve298E60(row,material,d)||!material||!material->passCount48()||*material->passCount48()<0)
            return reject(d,S::reference,0x246798);
        const bool palette=model.geometry_->hasPalette20();
        if(palette&&!fields.distance1BC)return reject(d,S::source,0x2417E0);
        const bool firstOnly=palette?*fields.distance1BC>application_->sourceSecondMapDistance382FA0_:
            application_->sourceModelPassOverride4354F8_!=0;
        const auto passes=firstOnly?std::min<std::int32_t>(1,*material->passCount48()):*material->passCount48();
        const auto packet=palette?static_cast<std::size_t>(surface->palette0E):0u;
        if(!model.packets_.packet(packet))return reject(d,S::source,0x241824);
        for(std::int32_t i=0;i<passes;++i){rr::assets::OriginalStandardMaterialPassView50LA pass;
            rr::assets::OriginalStandardMaterialCollectorEntryView50LA entry;
            if(!material->pass4C(static_cast<std::size_t>(i),pass)||!pass.flags21||
               !table_->coldCollectorPass(registry_,row,static_cast<std::size_t>(i),entry))return reject(d,S::collector,0x24A728);
            if(!(*pass.flags21&2u))continue; //Actual disabled24A738 return-to-loop.
            if(pass.animation10||pass.textureAnimation10){
                if(!table_->serviceRetainedModelPrototype24A740(registry_,row,static_cast<std::size_t>(i),application_->animationClock(),d))return false;
                if(pass.animation10)return reject(d,S::unsupportedPass,0x24A76C);
            }
            if(!append(ordinal,packet,entry.registration,pass,false))return false;
        }
    }
    d.sourcePC=0x24186C;return true;
}
bool NativeCollectorFifo50LA::collectGarageText246760(
    rr::platform::OriginalUiGarageDrawContribution50KG& draw,rr::platform::OriginalUiGarageScreenText50KG& text,
    std::size_t primitiveIndex,D& d)noexcept {
    d={};RetainAppendEffects effects{d,garageRecordCount_,garageRecordCount_};
    if(!application_||!retainGarageRegistry(*application_,d))return false;
    const auto self=draw.retainSelfForFifo();
    if(!self||draw.issuer_!=application_||!draw.screenTextInvocation2E0170Current(text)||
       text.issuer_!=application_||text.domain_!=application_->standardMaterialDomainOwner_||
       !text.coldInputsCurrent()||text.phase_!=decltype(text.phase_)::coldLoaded||
       primitiveIndex>=text.active5E_||primitiveIndex>=text.publishedPrimitives_||
       !text.primitives4_||!text.primitives4_[primitiveIndex])return reject(d,S::phase,0x2E01F0);
    if(!application_->materialRuntime.callbackOverrideKnown||application_->materialRuntime.callbackOverride435678)
        return reject(d,S::runtime,0x24A674);
    auto& primitive=*text.primitives4_[primitiveIndex];
    if(!primitive.materialStored||!primitive.materialRetained||primitive.material00<0||!primitive.packet10||
       primitive.optional04||primitive.optional14||(primitive.flags0F&1u))return reject(d,S::source,0x246760);
    const rr::assets::OriginalStandardMaterialObject50LA* material=nullptr;
    if(!table_->resolve298E60(primitive.material00,material,d)||!material||!material->passCount48()||
       *material->passCount48()<0)return reject(d,S::reference,0x246798);
    for(std::int32_t i=0;i<*material->passCount48();++i){
        rr::assets::OriginalStandardMaterialPassView50LA pass;
        rr::assets::OriginalStandardMaterialCollectorEntryView50LA entry;
        if(!material->pass4C(static_cast<std::size_t>(i),pass)||!pass.flags21||
           !table_->coldCollectorPass(registry_,primitive.material00,static_cast<std::size_t>(i),entry))
            return reject(d,S::collector,0x24A728);
        if(!(*pass.flags21&2u))continue;
        if(!table_->serviceRetainedScreenPrototype24A740(registry_,primitive.material00,static_cast<std::size_t>(i),d))return false;
        Record record;record.garageDraw=self;record.garagePublication=draw.publication_;
        record.garageText=&text;record.textPrimitive=primitiveIndex;record.primitive04=&primitive;
        //2E02A0 passes no model attributes. The retained actual glyph packet
        //contains its own STQ/RGBA/XYZ aliases, never replacement host geometry.
        record.attributes08=nullptr;record.originParameters=pass.parameters34;
        record.textureBase=pass.textureAnimation10;
        if(!appendGarage24B9B0(std::move(record),entry.registration,d))return false;
    }
    d.sourcePC=0x24680C;return true;
}
bool NativeCollectorFifo50LA::collectGarageQuad246760(
    rr::platform::OriginalUiGarageDrawContribution50KG& draw,rr::platform::OriginalUiGarageQuad50KG& quad,D& d)noexcept {
    d={};RetainAppendEffects effects{d,garageRecordCount_,garageRecordCount_};
    if(!application_||!retainGarageRegistry(*application_,d))return false;
    const auto self=draw.retainSelfForFifo();
    if(!self||draw.issuer_!=application_||!draw.quadInvocation2DF550Current(quad)||
       quad.domain_!=application_->standardMaterialDomainOwner_||!quad.packetCurrent2DF550()||
       quad.nativeGarageFifoPins_||!quad.packetRevision_||!quad.material54||quad.material54!=quad.primitiveOwner_.get())
        return reject(d,S::phase,0x2DF8B0);
    if(!application_->materialRuntime.callbackOverrideKnown||application_->materialRuntime.callbackOverride435678)
        return reject(d,S::runtime,0x24A674);
    const auto& primitive=*quad.material54;
    if(!primitive.materialStored||!primitive.materialRetained||primitive.material00<0||
       primitive.packet10!=quad.resource58||primitive.optional04||primitive.optional14||(primitive.flags0F&1u))
        return reject(d,S::source,0x246760);
    const rr::assets::OriginalStandardMaterialObject50LA* material=nullptr;
    if(!table_->resolve298E60(primitive.material00,material,d)||!material||!material->passCount48()||*material->passCount48()<0)
        return reject(d,S::reference,0x246798);
    for(std::int32_t i=0;i<*material->passCount48();++i){
        rr::assets::OriginalStandardMaterialPassView50LA pass;rr::assets::OriginalStandardMaterialCollectorEntryView50LA entry;
        if(!material->pass4C(static_cast<std::size_t>(i),pass)||!pass.flags21||
           !table_->coldCollectorPass(registry_,primitive.material00,static_cast<std::size_t>(i),entry))
            return reject(d,S::collector,0x24A728);
        if(!(*pass.flags21&2u))continue;
        if(!table_->serviceRetainedScreenPrototype24A740(registry_,primitive.material00,static_cast<std::size_t>(i),d))return false;
        Record record;record.garageDraw=self;record.garagePublication=draw.publication_;
        record.garageQuad=&quad;record.quadPacketRevision=quad.packetRevision_;record.primitive04=&primitive;
        record.attributes08=nullptr;record.originParameters=pass.parameters34;record.textureBase=pass.textureAnimation10;
        if(!appendGarage24B9B0(std::move(record),entry.registration,d))return false;
    }
    d.sourcePC=0x24680C;return true;
}
bool NativeCollectorFifo50LA::publishGarageScissor280118(
    rr::platform::OriginalUiGarageDrawContribution50KG& draw,rr::platform::OriginalUiGarageVisibility50KG& visibility,
    std::shared_ptr<rr::game::OriginalZoneScissor50LA> zone,D& d)noexcept {
    if(!application_||!retainGarageRegistry(*application_,d))return false;
    if(!draw.identityCurrent()||draw.issuer_!=application_||draw.phase_!=decltype(draw.phase_)::modelRender114E00||
       draw.sourcePC_!=0x114E00||!zone||zone->garageRetired_||zone->garageOwner_!=&visibility||
       scissor4014_.application_!=application_->entitiesOwner_.get())return reject(d,S::phase,0x280118);
    scissor4014_.value_=std::move(zone);scissor4014_.owner_=nullptr;scissor4014_.cameraGeneration_=0;
    garageScissorOwner_=&visibility;garageScissorPublication_=draw.publication_;
    d.sourceEffectsCommitted=true;d.sourcePC=0x280118;return true;
}
void NativeCollectorFifo50LA::releaseGarageRecordAfter105990(Record& record)noexcept {
    if(!record.garageDraw||!garageRecordCount_)std::terminate();
    if(record.garageParticles){if(!record.garageParticles->nativeGarageFifoPins_)std::terminate();--record.garageParticles->nativeGarageFifoPins_;}
    if(record.garageModel){if(!record.garageModel->nativeGarageFifoPins_)std::terminate();--record.garageModel->nativeGarageFifoPins_;}
    if(record.garageText){if(!record.garageText->nativeGarageFifoPins_)std::terminate();--record.garageText->nativeGarageFifoPins_;}
    if(record.garageQuad){if(!record.garageQuad->nativeGarageFifoPins_)std::terminate();--record.garageQuad->nativeGarageFifoPins_;}
    if(record.garageAperturePage){auto& a=*record.garageAperturePage->apertures_[record.apertureOrdinal];
        if(!a.nativeGarageFifoPins_)std::terminate();--a.nativeGarageFifoPins_;}
    --garageRecordCount_;table_->unpinNativeCollector();
}
}

namespace rr::platform {
namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool reject(D& d,S s,std::uint32_t pc)noexcept{d.status=s;d.sourcePC=pc;return false;}
}
std::shared_ptr<OriginalUiGarageDrawContribution50KG> OriginalUiGarageDrawContribution50KG::retainSelfForFifo()const noexcept {
    const auto s=session_.lock();if(!s||s->drawContribution_.get()!=this)return {};
    return s->drawContribution_;
}
bool OriginalUiGarageDrawContribution50KG::collectModel241280(OriginalUiGarageModelResource50KG& model,D& d)noexcept {
    d={};const auto visibility=page_?page_->renderVisibility114D28():nullptr;
    if(!visibility||!cameraFrame_.renderInvocation114E00Current(*page_,*visibility)||!issuer_->nativeCollectorFifoOwner_)
        return reject(d,S::phase,0x241798);
    return issuer_->nativeCollectorFifoOwner_->collectGarageModel241280(*this,model,d);
}
bool OriginalUiGarageCameraFrame50KG::collectGarageModel241280(OriginalUiGarageModelResource50KG& model,D& d)const noexcept {
    if(!cursor_)return reject(d,S::phase,0x241280);
    return const_cast<OriginalUiGarageDrawContribution50KG*>(cursor_)->collectModel241280(model,d);
}
bool OriginalUiGarageCameraFrame50KG::publishModelScissor280118(OriginalUiGarageVisibility50KG& visibility,
    std::shared_ptr<game::OriginalZoneScissor50LA> zone,D& d)const noexcept {
    if(!cursor_)return reject(d,S::phase,0x280118);
    return const_cast<OriginalUiGarageDrawContribution50KG*>(cursor_)->publishModelScissor280118(visibility,std::move(zone),d);
}
bool OriginalUiGarageDrawContribution50KG::publishModelScissor280118(OriginalUiGarageVisibility50KG& visibility,
    std::shared_ptr<game::OriginalZoneScissor50LA> zone,D& d)noexcept {
    d={};if(!cameraFrame_.renderInvocation114E00Current(*page_,visibility))return reject(d,S::phase,0x280118);
    const auto fifo=issuer_->nativeCollectorFifoOwner_;
    if(!fifo)return true; //Exact280108 null branch, not permission to collect models without443848.
    return fifo->publishGarageScissor280118(*this,visibility,std::move(zone),d);
}
bool OriginalUiGarageDrawContribution50KG::screenTextInvocation2E0170Current(const OriginalUiGarageScreenText50KG& text)const noexcept {
    return (activeText_&&textInvocation29D3F0Current(*activeText_)&&activeText_->screenText30_.get()==&text)||
        (identityCurrent()&&phase_==Phase::thirdPass&&issuer_->sourceApplicationMode22C_==5&&
         page_&&activePageCostText_==&text&&page_->drawReceiverCurrent(*this,&text));
}
bool OriginalUiGarageDrawContribution50KG::collectScreenTextPrimitive246760(
    OriginalUiGarageScreenText50KG& text,std::size_t primitive,OriginalUiGarageScreenTextDiagnostic50KG& d)noexcept {
    if(!screenTextInvocation2E0170Current(text)||!issuer_->nativeCollectorFifoOwner_){
        d.status=OriginalUiGarageScreenTextStatus50KG::authority;d.sourcePC=0x246760;return false;
    }
    if(!issuer_->nativeCollectorFifoOwner_->collectGarageText246760(*this,text,primitive,d.material)){
        d.status=OriginalUiGarageScreenTextStatus50KG::material;d.sourcePC=d.material.sourcePC;
        d.sourceEffectsCommitted=d.sourceEffectsCommitted||d.material.sourceEffectsCommitted;return false;
    }
    d.sourceEffectsCommitted=d.sourceEffectsCommitted||d.material.sourceEffectsCommitted;return true;
}
bool OriginalUiGarageDrawContribution50KG::textInvocation29D3F0Current(const OriginalUiGarageTextOverlay50KG& text)const noexcept {
    return identityCurrent()&&phase_==Phase::thirdPass&&sourcePC_==0x29D3F0&&
        issuer_->sourceApplicationMode22C_==5&&callback_==&text.entity_&&text.issuer_==issuer_&&
        text.grant_==page_->grant_&&text.identityCurrent()&&text.payloadCurrent();
}
bool OriginalUiGarageDrawContribution50KG::readTextInputs29D3F0(const OriginalUiGarageTextOverlay50KG& text,
    TextRenderInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x2E0F50;
    if(!textInvocation29D3F0Current(text)||!cameraFrame_.current()||!camera_->fields_.coreKnown||
       !renderer_->display_->video_||!renderer_->display_->video_->host.current())return false;
    const auto& f=camera_->fields_;const auto depth=renderer_->display_->video_->packetZMaximum;
    if(!f.viewport1E0[2]||!f.viewport1E0[3]||depth<0)return false;
    TextRenderInputs next;next.viewportWidth1E8=f.viewport1E0[2];next.viewportHeight1EC=f.viewport1E0[3];
    next.videoDepthMaximum60=static_cast<float>(depth);next.publication=publication_;out=next;pc=0;return true;
}
bool OriginalUiGarageDrawContribution50KG::restoreTextState29D8E8(const OriginalUiGarageTextOverlay50KG& text,
    std::uint32_t tag,std::uint32_t value,std::uint32_t flags,std::uint32_t& pc)noexcept {
    pc=textRestoreStep_?0x29D908:0x29D8E8;
    if(!textInvocation29D3F0Current(text)||textRestoreStep_>1||tag!=(textRestoreStep_?7u:14u)||value!=1u||flags)
        return false;
    //Actual renderer3D2050+30=245140. Only tags24/25 mutate B0/B4/B8;
    //these exact14/7 calls reach245200 with no render-state mutation.
    ++textRestoreStep_;pc=0x245200;return true;
}
}

namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::renderReturned140230Current(const Application& root)const noexcept {
    //Historical completed render receipt survives the later SourceTiming commit.
    //No setter and no prepared-sample requirement after the real return. The
    //current bounded loader never reaches this phase until every render tail exists.
    const auto session=session_.lock();
    return nativeRetirementPhase_==NativeRetirementPhase::notStarted&&issuer_==&root&&publication_&&phase_==Phase::renderReturned140230&&
        sourcePC_==0x140A68u&&targetCompleted140A38_&&finalPassReturned140844_&&rendererEndReturned2442CC_&&
        terminalQueue105990Current()&&session&&session->issuer_==issuer_&&session->drawContribution_.get()==this&&
        session->page_==page_&&!session->retiring_&&renderer_&&renderer_->currentFor(issuer_)&&
        renderer_->ownsRenderPrelude(*this,publication_)&&page_&&camera_&&
        page_->sharedCamera_==camera_&&camera_->nativeRegistrationIdentityCurrent();
}
}
namespace original_main_entry50KG {
bool RaceDisplayLifetime50LA::advanceGarageFrame26F910(
    const rr::platform::OriginalUiGarageDrawContribution50KG& draw,
    rr::platform::VehicleWindowDiagnostic50DS& d)noexcept {
    d={};
    if(!frontendOwnerCurrent50KG()||!canRetain()||!sourceBufferCount0CInitialized_||
       !draw.renderReturned140230Current(*application_)||
       !renderer_.ownsGarageSourcePrelude50KG(draw,draw.publication_)) {
        d.error=rr::platform::VehicleWindowError50DS::frame;
        d.operation="26F910 requires actual returned same-Display Garage render";return false;
    }
    const auto previous=garageAdvancedDraw26F910_.lock();
    if(previous.get()==&draw&&garageAdvancedPublication26F910_==draw.publication_)return true;
    const auto held=draw.retainSelfForFifo();
    if(!held||held.get()!=&draw){d.error=rr::platform::VehicleWindowError50DS::frame;
        d.operation="26F910 requires retained exact Draw publication";return false;}
    //Unsigned source add, equality wrap ONLY, then independent frame word.
    //No modulo, host buffer substitution, presentation or timing commit.
    ++sourceCurrentBuffer10_;if(sourceCurrentBuffer10_==sourceBufferCount0C_)sourceCurrentBuffer10_=0;
    ++sourceFrame14_;
    garageAdvancedDraw26F910_=held;garageAdvancedPublication26F910_=draw.publication_;
    return true;
}
}

namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::quadInvocation2DF550Current(const OriginalUiGarageQuad50KG& quad)const noexcept {
    return identityCurrent()&&phase_==Phase::thirdPass&&issuer_->sourceApplicationMode22C_==5&&
        page_&&activePageQuad_==&quad&&page_->drawReceiverCurrent(*this,&quad);
}
bool OriginalUiGarageDrawContribution50KG::readQuadInputs2DF550(const OriginalUiGarageQuad50KG& quad,
    QuadRenderInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x2DF550;if(!quadInvocation2DF550Current(quad))return false;
    QuadRenderInputs next;next.publication=publication_;
    next.sourceQuadEnabled383678=issuer_->entities()->screenQuadEnabled383678_!=0;
    //2DF5EC's real disabled branch does not consume Camera dimensions.
    if(!next.sourceQuadEnabled383678){out=next;pc=0x2DF8CC;return true;}
    if(!cameraFrame_.current()){pc=0x2DF608;return false;}
    next.viewportWidth1E8=camera_->fields_.viewport1E0[2];next.viewportHeight1EC=camera_->fields_.viewport1E0[3];
    if(!next.viewportWidth1E8||!next.viewportHeight1EC){pc=0x2DF608;return false;}
    out=next;pc=0;return true;
}
bool OriginalUiGarageDrawContribution50KG::collectQuadPrimitive246760(OriginalUiGarageQuad50KG& quad,D& d)noexcept {
    if(!quadInvocation2DF550Current(quad)||!issuer_->nativeCollectorFifoOwner_)return reject(d,S::phase,0x246760);
    return issuer_->nativeCollectorFifoOwner_->collectGarageQuad246760(*this,quad,d);
}
}

namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::readPageTextInputs2E0170(const OriginalUiGarageScreenText50KG& text,
    TextRenderInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x2E0F50;
    if(activePageCostText_!=&text||!screenTextInvocation2E0170Current(text)||!cameraFrame_.current()||
       !camera_->fields_.coreKnown||!renderer_->display_->video_||!renderer_->display_->video_->host.current())return false;
    const auto& f=camera_->fields_;const auto depth=renderer_->display_->video_->packetZMaximum;
    if(!f.viewport1E0[2]||!f.viewport1E0[3]||depth<0)return false;
    TextRenderInputs next;next.viewportWidth1E8=f.viewport1E0[2];next.viewportHeight1EC=f.viewport1E0[3];
    next.videoDepthMaximum60=static_cast<float>(depth);next.publication=publication_;out=next;pc=0;return true;
}
}
