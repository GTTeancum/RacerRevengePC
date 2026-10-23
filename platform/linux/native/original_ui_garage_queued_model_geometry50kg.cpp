#include "vehicle_window_gl50ds.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_model_resource50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include <cstring>
#include <limits>
#include <thread>
#include <utility>

namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::lowerQueuedModel24B26C(
    std::size_t ordinal,const OriginalUiGarageModelLowering50KG::Context& context,
    OriginalUiGarageOwnedModelGeometry50KG& out,assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    using S=assets::OriginalStandardMaterialStatus50LA;
    using Lower=OriginalUiGarageModelLowering50KG;
    using Program=assets::OriginalUiGarageProgram50KG;
    d={};const auto fail=[&](S status,std::uint32_t pc,const char* field){
        d.status=status;d.sourcePC=pc;d.field=field;return false;
    };
    if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24b26cu||
       !publication_||!renderer_||!renderer_->currentFor(issuer_)||
       !renderer_->ownsRenderPrelude(*this,publication_)||!cameraFrame_.current())
        return fail(S::phase,0x24b26c,"actual queued-model drain invocation");
    const auto fifo=issuer_->nativeCollectorFifoOwner_;
    if(!fifo||fifo->application_!=issuer_||fifo->closed_||!fifo->draining_||!fifo->uiConstructor1B73BC_||
       fifo->ownerThread_!=std::this_thread::get_id()||!fifo->table_||
       !fifo->table_->coldCollectorCurrent(fifo->registry_)||ordinal>=fifo->records_.size())
        return fail(S::collector,0x24b26c,"same live FIFO and actual record ordinal");
    auto it=fifo->records_.cbegin();for(std::size_t i=0;i<ordinal;++i)++it;
    const auto& record=*it;
    if(record.garageDraw.get()!=this||record.garagePublication!=publication_||record.epoch!=fifo->epoch04_||
       !record.garageModel||record.garageText||record.primitive04||record.camera||record.allocation||
       record.material||record.display||record.bank||record.cables)
        return fail(S::collector,0x24b26c,"actual model record and publication");
    const auto& model=*record.garageModel;
    const auto revision=model.nativeGaragePacketRevision();
    if(!model.current()||!model.nativeGarageFifoPins_||!revision||record.matrixRevision!=revision||
       model.nativeSharedSelf_.lock().get()!=&model||model.application_!=issuer_->entitiesOwner_||
       model.domain_!=issuer_->standardMaterialDomainOwner_||!model.domain_||
       model.domain_->materials382B38()!=fifo->table_||!model.geometry_||
       !model.geometry_->holdsModelReference(&model.scene_)||
       record.lod!=static_cast<std::size_t>(model.scene_.fields().field1B0)||
       record.lod>std::numeric_limits<std::uint32_t>::max()||
       record.surface>std::numeric_limits<std::uint32_t>::max()||
       record.packetIndex>std::numeric_limits<std::uint32_t>::max())
        return fail(S::phase,0x24b4a0,"same retained model, packet revision and LOD");
    const auto* surface=model.geometry_->surface246300(record.lod,record.surface);
    const auto* packet=model.packets_.packet(record.packetIndex);
    if(!surface||surface!=record.surface04||!surface->material00||*surface->material00<0||!packet||
       record.attributeArray1F0||record.attributes08!=packet||
       (model.geometry_->hasPalette20()?record.packetIndex!=surface->palette0E:record.packetIndex!=0u))
        return fail(S::source,0x24b4bc,"actual runtime Surface04 and direct attributes08");
    // The actual fast24178C record names FIRST Surface once and the sole
    // non-palette packet. There is no invented surface-count expansion here.
    if(model.scene_.fields().field1C8&&(model.geometry_->hasPalette20()||record.surface!=0u||
       model.packets_.count()!=1u||model.nativeCollector1C8Owner_.lock()!=fifo||
       model.nativeCollector1C8Registration_!=record.originRegistration||
       !fifo->table_->coldCollectorCurrent(model.nativeCollector1C8Receipt_)))
        return fail(S::collector,0x24178c,"real non-palette first-Surface cache record");
    assets::OriginalStandardMaterialCollectorEntryView50LA origin,final;
    if(record.originRegistration<0||record.finalRegistration<0||
       !fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.originRegistration),origin)||
       !fifo->table_->coldCollectorEntry(fifo->registry_,static_cast<std::size_t>(record.finalRegistration),final)||
       origin.registration!=record.originRegistration||origin.finalLeaf!=record.finalRegistration||
       final.registration!=record.finalRegistration||final.finalLeaf!=record.finalRegistration||final.redirect18!=-1||
       !origin.material||!final.material||origin.pass<0||final.pass<0)
        return fail(S::collector,0x24b26c,"actual canonical origin and final leaf");
    d.row=origin.materialRow;d.pass=static_cast<std::size_t>(origin.pass);
    const auto* handle=model.materials_.handle1C4(static_cast<std::size_t>(*surface->material00));
    if(!handle||(*handle==-1?*surface->material00:*handle)!=origin.materialRow)
        return fail(S::reference,0x2417d8,"actual Surface material override");
    assets::OriginalStandardMaterialPassView50LA originPass,finalPass;
    if(!origin.material->pass4C(static_cast<std::size_t>(origin.pass),originPass)||
       !final.material->pass4C(static_cast<std::size_t>(final.pass),finalPass)||
       record.originParameters!=originPass.parameters34||record.textureBase!=originPass.textureAnimation10||
       record.vertexBase||originPass.animation10||!originPass.parameterCount28||!originPass.parameterMask24)
        return fail(S::source,0x24b4a0,"retained originating pass operands");
    // Nonzero origin VU parameter overlays and vertex animation require their
    // own numeric source consumer. They cannot be silently replaced by the
    // final leaf's parameters or by the unmodified common packet rows.
    if(*originPass.parameterCount28!=0||*originPass.parameterMask24!=0||
       (record.originParameters&&!record.originParameters->empty()))
        return fail(S::unsupportedPass,0x24b4e4,"originating VU parameter overlay");
    const auto& programs=model.domain_->programs382B70();
    assets::OriginalStandardProgramRowView50LA program;
    if(!programs||!programs->prepared()||!finalPass.program18||
       !programs->row(*finalPass.program18,program)||!program.canonicalName)
        return fail(S::program,0x24b1c0,"actual final-leaf program18");
    Program family;
    if(std::strcmp(program.canonicalName,"PrelitCompressed")==0)family=Program::prelitCompressed;
    else if(std::strcmp(program.canonicalName,"Diffuse")==0)family=Program::diffuse;
    else if(std::strcmp(program.canonicalName,"DiffuseCompressed")==0)family=Program::diffuseCompressed;
    else if(std::strcmp(program.canonicalName,"SpecularMapCompressed")==0)family=Program::specularMapCompressed;
    else return fail(S::unsupportedPass,0x24b1c0,"unimplemented final-leaf model program");
    try {
        auto raw=std::shared_ptr<OriginalUiGarageRawSurface50KG>(new OriginalUiGarageRawSurface50KG());
        OriginalUiGarageRawDiagnostic50KG rawDiagnostic;
        if(!raw->prepare(record.garageModel,static_cast<std::uint32_t>(record.lod),
           static_cast<std::uint32_t>(record.surface),rawDiagnostic)) {
            d.slot=rawDiagnostic.sourceOffset;
            return fail(rawDiagnostic.status==OriginalUiGarageRawStatus50KG::allocation?S::allocation:S::source,
                rawDiagnostic.sourcePC,"actual raw Surface stream (slot is source byte offset)");
        }
        OriginalUiGarageRawSurface50KG::Metadata metadata;
        if(!raw->describe(metadata,rawDiagnostic)||metadata.runtimeSurface!=surface||
           metadata.materialSlot!=static_cast<std::uint32_t>(*surface->material00))
            return fail(S::source,rawDiagnostic.sourcePC,"same retained raw/runtime Surface identity");
        Lower lower;Lower::Diagnostic numeric;
        const auto lowerFailure=[&]() {
            const char* field="model numeric owner/packet";S status=S::source;
            if(numeric.status==Lower::Status::program){field="distinct model program unavailable";status=S::unsupportedPass;}
            else if(numeric.status==Lower::Status::numeric){
                switch(numeric.numeric){
                case game::garage_prelit50KG::Status::input:field="model triangle numeric input";status=S::input;break;
                case game::garage_prelit50KG::Status::arithmetic:field="model triangle arithmetic";status=S::arithmetic;break;
                case game::garage_prelit50KG::Status::unsupportedPacketRange:field="model triangle packet range";status=S::range;break;
                case game::garage_prelit50KG::Status::capacity:field="model triangle clip capacity";status=S::overflow;break;
                case game::garage_prelit50KG::Status::complete:break;
                }
            }else if(numeric.status==Lower::Status::raw){field="model triangle raw source";status=S::source;}
            return fail(status,numeric.sourcePC,field);
        };
        if(!lower.prepare(raw,static_cast<std::uint32_t>(record.packetIndex),*packet,family,context,numeric))
            return lowerFailure();
        OriginalUiGarageOwnedModelGeometry50KG next;
        if(!lower.readTriangleCount(next.sourceTriangleCount)||next.sourceTriangleCount!=metadata.triangleCount)
            return fail(S::phase,0x24b4a0,"actual complete triangle extent");
        for(std::uint32_t triangle=0;triangle<next.sourceTriangleCount;++triangle){
            d.slot=triangle;OriginalUiGaragePrelitPacket50KG polygon;
            if(!lower.projectTriangle(triangle,polygon,numeric))return lowerFailure();
            if(polygon.quadCount>originalUiMaximumSourceQuads50KM-next.quadCount)
                return fail(S::overflow,0x24b50c,"unchanged whole-frame model geometry capacity");
            if(!polygon.quadCount)++next.clippedTriangleCount;
            const auto count=polygon.quadCount*4u;
            next.vertices.insert(next.vertices.end(),polygon.vertices.begin(),polygon.vertices.begin()+count);
            next.stq.insert(next.stq.end(),polygon.stq.begin(),polygon.stq.begin()+count);
            next.rasterW.insert(next.rasterW.end(),polygon.rasterW.begin(),polygon.rasterW.begin()+count);
            next.fog.insert(next.fog.end(),polygon.fog.begin(),polygon.fog.begin()+count);
            next.quadCount+=polygon.quadCount;
        }
        if(!lower.current()||model.nativeGaragePacketRevision()!=record.matrixRevision||
           !model.nativeGarageFifoPins_||record.epoch!=fifo->epoch04_||
           next.vertices.size()!=std::size_t(next.quadCount)*4u||next.stq.size()!=next.vertices.size()||
           next.rasterW.size()!=next.vertices.size()||next.fog.size()!=next.vertices.size())
            return fail(S::phase,0x24b50c,"retained model and parallel output streams");
        next.recordOrdinal=ordinal;next.recordIdentity=&record;next.publication=publication_;
        next.epoch=record.epoch;next.matrixRevision=record.matrixRevision;
        next.lod=record.lod;next.surface=record.surface;next.packetIndex=record.packetIndex;
        next.materialSlot=metadata.materialSlot;next.originRegistration=record.originRegistration;
        next.finalRegistration=record.finalRegistration;next.program=family;
        // Includes the genuine all-clipped empty case only after EVERY source
        // triangle succeeded. Failure above leaves the caller's output intact.
        out=std::move(next);d.status=S::complete;d.sourcePC=0x24b50c;d.slot=ordinal;d.field=nullptr;return true;
    }catch(...){return fail(S::allocation,0x24b26c,"queued model geometry allocation");}
}
}
