#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_main_draw50kg.h"
#include "original_ui_options_draw_contribution50kg.h"
#include "../../../src/game/original_application_entities50la.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <bit>
#include <exception>
#include <limits>
#include <cstring>
#include "../../../src/assets/original_standard_archive_provider50la.h"
#include "../../../src/assets/original_standard_material_object50la.h"
#include "../../../src/assets/original_standard_material_source50la.h"
#include "../../../src/assets/original_standard_texture_table50la.h"
#include "../../../src/assets/original_standard_texture_resource50la.h"

namespace rr::platform {
namespace { using Kind=OriginalUiWarningOperationKind50KG; }

bool OriginalUiSharedDialogs50KG::menuWarningIdentityCurrent()const noexcept {
    const auto app=application_.lock();
    return menuWarningPhase_!=MenuWarningPhase::idle&&menuWarningPhase_!=MenuWarningPhase::failed&&
        menu470Current()&&app&&app->sharedDialogs.get()==this&&
        menuWarningEpoch_&&menuWarningEpoch_==menu470Epoch_&&
        menuWarningInvocation_&&menuWarningInvocation_==menu470Invocation_&&
        menuWarningUpdate_==menu470Update_&&menuWarningUpdate_==update470Count_&&!update470Pending_&&
        menuWarningCache_[0]==dialog470_.primaryPresent29C&&
        menuWarningCache_[1]==dialog470_.secondaryPresent2A0&&
        menuWarningPlan_.status==OriginalUiWarningGeometryStatus50KG::planned&&
        menuWarningPlan_.operationCount&&menuWarningPlan_.operationCount<=menuWarningPlan_.operations.size()&&
        menuWarningNext_<=menuWarningPlan_.operationCount;
}
bool OriginalUiSharedDialogs50KG::menuWarningBodyCurrent()const noexcept {
    // These are LIVE operands until the actual inherited +48 tail returns.
    // Never tolerate an enable-tuple change during an executing source body.
    const auto app=application_.lock();
    return (menuWarningPhase_==MenuWarningPhase::body||menuWarningPhase_==MenuWarningPhase::inheritedPending)&&
        menuWarningIdentityCurrent()&&app&&menuWarningCache_[2]==app->courseUi474&&
        menuWarningCache_[3]==app->courseUi478;
}
bool OriginalUiSharedDialogs50KG::menuWarningCompletedCurrent()const noexcept {
    // complete is issued ONLY by completeMenu470WarningTail after the real
    // inherited walk. The later Controls B8 callback may change474/478 in
    // this SAME source frame. They are now historical plan inputs, not live
    // inputs to a completed callback. Presence/update/owner identity stays live.
    return menuWarningPhase_==MenuWarningPhase::complete&&menuWarningIdentityCurrent()&&
        menu470CompletedSlots_==3u&&menuWarningNext_==menuWarningPlan_.operationCount&&
        menuWarningPlan_.operations[menuWarningPlan_.operationCount-1].kind==Kind::callInherited141D70;
}
bool OriginalUiSharedDialogs50KG::menuWarningOperationCurrent()const noexcept {
    return menuWarningBodyCurrent()&&menuWarningPhase_==MenuWarningPhase::body&&menu470RenderCurrent()&&
        menu470Phase_==Menu470Phase::callbacks&&menu470Slot_==0x48&&menu470Callback_==&dialog470_.entity&&
        menu470CompletedSlots_==1u&&menuWarningNext_<menuWarningPlan_.operationCount&&
        menuWarningSourcePC_==menuWarningPlan_.operations[menuWarningNext_].sourcePC;
}
bool OriginalUiSharedDialogs50KG::menuWarning470TextCurrent(const Text& text)const noexcept {
    if(!menuWarningOperationCurrent())return false;
    const auto& op=menuWarningPlan_.operations[menuWarningNext_];
    return op.kind==Kind::callText2E0170&&
        ((op.receiverOffset==0x1B0&&dialog470_.message1B0.get()==&text)||
         (op.receiverOffset==0x220&&dialog470_.alternate220.get()==&text));
}
bool OriginalUiSharedDialogs50KG::menuWarning470QuadCurrent(const Quad& quad)const noexcept {
    if(!menuWarningOperationCurrent())return false;
    const auto& op=menuWarningPlan_.operations[menuWarningNext_];
    if(op.kind!=Kind::callStrip2DF550)return false;
    for(std::size_t i=0;i<dialog470_.strips.size();++i)
        if(&dialog470_.strips[i]==&quad)return op.receiverOffset==0x40u+i*0x60u;
    return false;
}
bool OriginalUiSharedDialogs50KG::menuWarning470DimCurrent()const noexcept {
    if(!menuWarningOperationCurrent())return false;
    const auto& op=menuWarningPlan_.operations[menuWarningNext_];
    return op.kind==Kind::callDim2E21D0&&op.receiverOffset==0x160&&dialog470_.dim160;
}

bool OriginalUiSharedDialogs50KG::readMenuWarningTextInputs2E0170(
    const Text& text,MenuWarningTextInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x2E0F50;
    if(!menuWarning470TextCurrent(text)||!inputs_.root||!inputs_.root->entities()||
       text.authority_!=inputs_.root->entities()||!inputs_.root->entities()->screenTextEnabled383688_||
       !menu470RasterCurrent()||!inputs_.video||!inputs_.video->host.current()||
       inputs_.video->packetZMaximum<0||!menuRaster_.viewport[2]||!menuRaster_.viewport[3])return false;
    MenuWarningTextInputs next;
    next.viewportWidth1E8=menuRaster_.viewport[2];next.viewportHeight1EC=menuRaster_.viewport[3];
    next.videoDepthMaximum60=static_cast<float>(inputs_.video->packetZMaximum);
    out=next;pc=0;return true;
}
bool OriginalUiSharedDialogs50KG::readMenuWarningQuadInputs2DF550(
    const Quad& quad,MenuWarningQuadInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x2DF550;
    if(!menuWarning470QuadCurrent(quad)||!inputs_.root||!inputs_.root->entities())return false;
    MenuWarningQuadInputs next;
    next.sourceQuadEnabled383678=inputs_.root->entities()->screenQuadEnabled383678_!=0;
    // Keep the real disabled return ahead of every viewport operand read.
    if(!next.sourceQuadEnabled383678){out=next;pc=0x2DF8CC;return true;}
    pc=0x2DF608;
    if(!menu470RasterCurrent()||!menuRaster_.viewport[2]||!menuRaster_.viewport[3])return false;
    next.viewportWidth1E8=menuRaster_.viewport[2];next.viewportHeight1EC=menuRaster_.viewport[3];
    out=next;pc=0;return true;
}
bool OriginalUiSharedDialogs50KG::readMenuWarningDimInputs2E21D0(
    MenuWarningDimInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x2E21D0;
    if(!menuWarning470DimCurrent()||!inputs_.root||!inputs_.root->entities())return false;
    MenuWarningDimInputs next;
    next.sourceApertureEnabled383680=inputs_.root->entities()->apertureRenderEnabled383680_!=0;
    if(!next.sourceApertureEnabled383680){out=next;pc=0;return true;}
    if(!menu470RasterCurrent()||!inputs_.video||!inputs_.video->host.current()||
       inputs_.video->packetZMaximum<0||!menuRaster_.viewport[2]||!menuRaster_.viewport[3])return false;
    next.viewportWidth1E8=menuRaster_.viewport[2];next.viewportHeight1EC=menuRaster_.viewport[3];
    next.videoDepthMaximum60=inputs_.video->packetZMaximum;out=next;pc=0;return true;
}

bool OriginalUiSharedDialogs50KG::executeMenu470WarningBody(Diagnostic& d)noexcept {
    d={};
    const auto stop=[&](Status status,std::uint32_t pc)noexcept {
        menuWarningPhase_=MenuWarningPhase::failed;menu470Phase_=Menu470Phase::failed;
        d.status=status;d.sourcePC=pc;d.sourceEffectsCommitted=menuWarningEffects_;return false;
    };
    // Entry is single-use, including a no-packet branch and a failed prefix.
    if(menuWarningPhase_!=MenuWarningPhase::idle||menuWarningIssued_||menuWarningAppended_||
       nativeMenuWarning470Pins_||!menu470RenderCurrent()||menu470Phase_!=Menu470Phase::callbacks||
       menu470Slot_!=0x48||menu470Callback_!=&dialog470_.entity||menu470CompletedSlots_!=1u||
       update470Pending_||demand470Pending_||firstPass470Draw_||thirdPass470Draw_||nativeWarning470FifoPins_)
        return stop(Status::phase,0x204AC0);
    const auto app=application_.lock();
    if(!app||app->sharedDialogs.get()!=this)return stop(Status::owner,0x204AC0);
    // Copy exactly the operands consumed by the existing 204AC0 source plan.
    // Numeric planning grants neither callback ownership nor packet lifetime.
    OriginalUiWarningValues50KG values;
    values.delta290=dialog470_.delta290;values.timer294=dialog470_.timer294;values.active298=dialog470_.active298;
    values.primaryPresent29C=dialog470_.primaryPresent29C;values.secondaryPresent2A0=dialog470_.secondaryPresent2A0;
    values.primaryEnabled474=app->courseUi474;values.secondaryEnabled478=app->courseUi478;
    for(std::size_t i=0;i<dialog470_.strips.size();++i){const auto& q=dialog470_.strips[i];auto& v=values.strips[i];
        v.positionX10=q.position10[0];v.positionY14=q.position10[1];v.height38=q.uv30[2];
        v.rotation3C=q.uv30[3];v.flags5CValue=q.flags5CValue;
    }
    const auto plan=planOriginalUiWarning204AC050KG(values);
    if(plan.status!=OriginalUiWarningGeometryStatus50KG::planned||!plan.operationCount||
       plan.operationCount>plan.operations.size()||plan.operations[plan.operationCount-1].kind!=Kind::callInherited141D70)
        return stop(Status::source,plan.unsupportedPC?plan.unsupportedPC:0x204AC0);
    if(plan.operationCount>1){
        // No-effects retained-payload preflight. The positive reachability gates
        // remain at the caller; this executor never enables them itself.
        if(!warning470DimPacketCurrent2E21D0()||dialog470_.dim160->nativeGarageFifoPins_||
           dialog470_.dim160->nativeMenuWarningPins_||!dialog470_.message1B0||!dialog470_.alternate220||
           !dialog470_.message1B0->coldInputsCurrent()||!dialog470_.alternate220->coldInputsCurrent()||
           dialog470_.message1B0->nativeGarageFifoPins_||dialog470_.alternate220->nativeGarageFifoPins_||
           dialog470_.message1B0->nativeMenuWarningPins_||dialog470_.alternate220->nativeMenuWarningPins_)
            return stop(Status::resourcePending,0x2053F4);
        for(const auto& q:dialog470_.strips)
            if(!q.canMutatePacket2DF550()||(q.flags5CKnownMask&3u)!=3u)
                return stop(Status::resourcePending,0x204AC0);
    }
    menuWarningEpoch_=menu470Epoch_;menuWarningInvocation_=menu470Invocation_;menuWarningUpdate_=update470Count_;
    menuWarningCache_={{values.primaryPresent29C,values.secondaryPresent2A0,values.primaryEnabled474,values.secondaryEnabled478}};
    menuWarningPlan_=plan;menuWarningNext_=0;menuWarningEffects_=false;menuWarningPhase_=MenuWarningPhase::body;
    // Real native menu raster state is applied only for a positive plan, and
    // before its first source write. No guest camera or Garage lease is issued.
    if(plan.operationCount>1&&!applyMenu470RasterPrelude(d))return stop(d.status,d.sourcePC);
    for(;menuWarningNext_<menuWarningPlan_.operationCount;++menuWarningNext_){
        const auto& op=menuWarningPlan_.operations[menuWarningNext_];menuWarningSourcePC_=op.sourcePC;
        if(!menuWarningOperationCurrent())return stop(Status::owner,op.sourcePC);
        if(op.kind==Kind::callInherited141D70){
            if(menuWarningNext_+1!=menuWarningPlan_.operationCount)return stop(Status::source,op.sourcePC);
            ++menuWarningNext_;menuWarningPhase_=MenuWarningPhase::inheritedPending;
            d.sourceEffectsCommitted=menuWarningEffects_;d.sourcePC=op.sourcePC;return true;
        }
        if(op.kind==Kind::callDim2E21D0){
            const bool okay=drawMenuWarning470Dim2E21D0(d);menuWarningEffects_|=d.sourceEffectsCommitted;
            if(!okay)return stop(d.status,d.sourcePC);continue;
        }
        if(op.kind==Kind::callText2E0170){
            Text* text=op.receiverOffset==0x1B0?dialog470_.message1B0.get():
                op.receiverOffset==0x220?dialog470_.alternate220.get():nullptr;
            if(!text)return stop(Status::source,op.sourcePC);
            const bool okay=text->drawMenuWarning4702E0170(*this,d.text);menuWarningEffects_|=d.text.sourceEffectsCommitted;
            if(!okay)return stop(Status::resourcePending,d.text.sourcePC);continue;
        }
        if(op.kind==Kind::timer294){
            dialog470_.timer294=std::bit_cast<float>(op.valueBits);menuWarningEffects_=true;continue;
        }
        if(op.receiverOffset<0x40||op.receiverOffset>0x100||(op.receiverOffset-0x40)%0x60)
            return stop(Status::source,op.sourcePC);
        auto& quad=dialog470_.strips[(op.receiverOffset-0x40)/0x60];
        if(op.kind==Kind::callStrip2DF550){
            const bool okay=quad.drawMenuWarning4702DF550(*this,d.quad);menuWarningEffects_|=d.quad.sourceEffectsCommitted;
            if(!okay)return stop(Status::resourcePending,d.quad.sourcePC);continue;
        }
        if(!quad.canMutatePacket2DF550())return stop(Status::phase,op.sourcePC);
        switch(op.kind){
            case Kind::positionX10:quad.position10[0]=std::bit_cast<float>(op.valueBits);break;
            case Kind::positionY14:quad.position10[1]=std::bit_cast<float>(op.valueBits);break;
            case Kind::height38:quad.uv30[2]=std::bit_cast<float>(op.valueBits);break;
            case Kind::flags5C:quad.flags5CValue=static_cast<std::uint8_t>(op.valueBits);
                quad.flags5CKnownMask|=op.assertedDirtyBits;break;
            default:return stop(Status::source,op.sourcePC);
        }
        menuWarningEffects_=true;
    }
    return stop(Status::source,0x2054A4);
}
bool OriginalUiSharedDialogs50KG::completeMenu470WarningTail(Diagnostic& d)noexcept {
    // The caller must just have performed walkMenu470Inherited(8,141D70,d).
    const bool inherited=d.status==Status::complete&&d.sourcePC==0x141D70;
    d={};
    if(!inherited||!menuWarningBodyCurrent()||menuWarningPhase_!=MenuWarningPhase::inheritedPending||
       !menu470RenderCurrent()||menu470Phase_!=Menu470Phase::callbacks||menu470Slot_!=0x48||
       menu470Callback_!=&dialog470_.entity||menu470CompletedSlots_!=1u||
       menuWarningNext_!=menuWarningPlan_.operationCount||dialog470_.entity.firstChild18||
       !scope_->sharedDialogPrefixCurrent(*this,0,dialog470_.entity)){
        menuWarningPhase_=MenuWarningPhase::failed;menu470Phase_=Menu470Phase::failed;
        d.status=Status::owner;d.sourcePC=0x2054A4;d.sourceEffectsCommitted=menuWarningEffects_;return false;
    }
    menuWarningPhase_=MenuWarningPhase::complete;
    d.sourceEffectsCommitted=menuWarningEffects_;d.sourcePC=0x2054BC;return true;
}

bool OriginalUiSharedDialogs50KG::menuWarningPacketIdentity(
    const OriginalUiMenuWarningPacket50KG& receipt,std::size_t index)const noexcept {
    if(index>=menuWarningPackets_.size()||&receipt!=&menuWarningPackets_[index]||
       receipt.owner_!=this||!receipt.sourceOwner_||receipt.sourceOwner_!=menu470Owner()||
       !menuWarningEpoch_||receipt.warningEpoch_!=menuWarningEpoch_||menuWarningEpoch_!=menu470Epoch_||
       !menuWarningInvocation_||receipt.invocation_!=menuWarningInvocation_||menuWarningInvocation_!=menu470Invocation_||
       receipt.update_!=menuWarningUpdate_||receipt.update_!=menu470Update_||
       receipt.rasterInvocation_!=menuWarningInvocation_||!receipt.rasterInvocation_||
       menuWarningPlan_.status!=OriginalUiWarningGeometryStatus50KG::planned||
       menuWarningPlan_.operationCount>menuWarningPlan_.operations.size()||
       receipt.operation_>=menuWarningPlan_.operationCount)return false;
    const auto& op=menuWarningPlan_.operations[receipt.operation_];
    if(receipt.sourcePC_!=op.sourcePC)return false;
    using PacketKind=OriginalUiMenuWarningPacket50KG::Kind;
    switch(receipt.kind_){
        case PacketKind::text:return op.kind==Kind::callText2E0170&&
            (op.receiverOffset==0x1B0||op.receiverOffset==0x220);
        case PacketKind::quad:return op.kind==Kind::callStrip2DF550&&op.receiverOffset>=0x40&&
            op.receiverOffset<=0x100&&(op.receiverOffset-0x40)%0x60==0;
        case PacketKind::dim:return op.kind==Kind::callDim2E21D0&&op.receiverOffset==0x160;
        default:return false;
    }
}
bool OriginalUiSharedDialogs50KG::menuWarningPacketPayloadMatches(
    const OriginalUiMenuWarningPacket50KG& receipt)const noexcept {
    if(!receipt.active_||receipt.operation_>=menuWarningPlan_.operationCount||
       menuWarningPlan_.operationCount>menuWarningPlan_.operations.size())return false;
    const auto& op=menuWarningPlan_.operations[receipt.operation_];
    using PacketKind=OriginalUiMenuWarningPacket50KG::Kind;
    switch(receipt.kind_){
        case PacketKind::text:{
            const Text* text=op.receiverOffset==0x1B0?dialog470_.message1B0.get():
                op.receiverOffset==0x220?dialog470_.alternate220.get():nullptr;
            return text&&text->menuWarningPacketMatches(*this,receipt);
        }
        case PacketKind::quad:
            return op.receiverOffset>=0x40&&op.receiverOffset<=0x100&&(op.receiverOffset-0x40)%0x60==0&&
                dialog470_.strips[(op.receiverOffset-0x40)/0x60].menuWarningPacketMatches(*this,receipt);
        case PacketKind::dim:return menuWarningDimPacketMatches(receipt);
        default:return false;
    }
}
bool OriginalUiSharedDialogs50KG::prepareMenuWarningPacket(OriginalUiMenuWarningPacket50KG& receipt,
    OriginalUiMenuWarningPacket50KG::Kind kind)noexcept {
    if(!menuWarningOperationCurrent()||menuWarningIssued_>=menuWarningPackets_.size()||
       menuWarningIssued_!=menuWarningAppended_||nativeMenuWarning470Pins_!=menuWarningIssued_||
       &receipt!=&menuWarningPackets_[menuWarningIssued_]||receipt.active_||receipt.owner_)return false;
    receipt.owner_=this;receipt.sourceOwner_=menu470Owner();receipt.invocation_=menuWarningInvocation_;
    receipt.warningEpoch_=menuWarningEpoch_;receipt.update_=menuWarningUpdate_;
    receipt.rasterInvocation_=menuWarningInvocation_;receipt.kind_=kind;
    receipt.operation_=menuWarningNext_;receipt.sourcePC_=menuWarningSourcePC_;
    return menuWarning470PacketCaptureCurrent(receipt);
}
bool OriginalUiSharedDialogs50KG::menuWarning470PacketCaptureCurrent(
    const OriginalUiMenuWarningPacket50KG& receipt)const noexcept {
    return menuWarningOperationCurrent()&&menu470RasterCurrent()&&
        menuWarningIssued_<menuWarningPackets_.size()&&menuWarningIssued_==menuWarningAppended_&&
        nativeMenuWarning470Pins_==menuWarningIssued_&&
        menuWarningPacketIdentity(receipt,menuWarningIssued_)&&!receipt.active_&&
        !receipt.payload_&&!receipt.primitive_&&!receipt.packet_&&!receipt.packetRevision_&&
        receipt.materialToken_==-1&&receipt.operation_==menuWarningNext_;
}
bool OriginalUiSharedDialogs50KG::menuWarning470PacketCurrent(
    const OriginalUiMenuWarningPacket50KG& receipt)const noexcept {
    if(!(menuWarningPhase_==MenuWarningPhase::complete?menuWarningCompletedCurrent():menuWarningBodyCurrent())||
       !menu470RasterCurrent()||menuWarningIssued_>menuWarningPackets_.size()||
       nativeMenuWarning470Pins_!=menuWarningIssued_)return false;
    for(std::size_t i=0;i<menuWarningIssued_;++i)if(&receipt==&menuWarningPackets_[i])
        return menuWarningPacketIdentity(receipt,i)&&receipt.operation_<=menuWarningNext_&&
            menuWarningPacketPayloadMatches(receipt);
    return false;
}
bool OriginalUiSharedDialogs50KG::menuWarning470PacketOffsetCurrent(
    const OriginalUiMenuWarningPacket50KG& receipt,const std::array<std::uint32_t,2>& offset)const noexcept {
    return menuWarningOperationCurrent()&&menuWarningIssued_&&menuWarningIssued_<=menuWarningPackets_.size()&&
        menuWarningAppended_+1==menuWarningIssued_&& &receipt==&menuWarningPackets_[menuWarningIssued_-1]&&
        receipt.operation_==menuWarningNext_&&receipt.sourcePC_==menuWarningSourcePC_&&
        offset==menuRaster_.xyOffsetFixed4&&menuWarning470PacketCurrent(receipt);
}
bool OriginalUiSharedDialogs50KG::menuWarning470PacketAppendCurrent(const OriginalUiMainDraw50KG& draw,
    const OriginalUiMenuWarningPacket50KG& receipt)const noexcept {
    return menuMainDraw_==&draw&&!menuOptionsDraw_&&receipt.sourceOwner_==&draw&&
        draw.sharedMenuInvocationCurrent(*this,menuWarningInvocation_)&&
        menuWarning470PacketOffsetCurrent(receipt,menuRaster_.xyOffsetFixed4);
}
bool OriginalUiSharedDialogs50KG::menuWarning470PacketAppendCurrent(const OriginalUiOptionsDrawContribution50KG& draw,
    const OriginalUiMenuWarningPacket50KG& receipt)const noexcept {
    return menuOptionsDraw_==&draw&&!menuMainDraw_&&receipt.sourceOwner_==&draw&&
        draw.sharedMenuInvocationCurrent(*this,menuWarningInvocation_)&&
        menuWarning470PacketOffsetCurrent(receipt,menuRaster_.xyOffsetFixed4);
}
bool OriginalUiSharedDialogs50KG::appendMenuWarningPacket(OriginalUiMenuWarningPacket50KG& receipt,
    const OriginalUiMenuWarningGeometry50KG& geometry,Diagnostic& d)noexcept {
    d={};bool okay=false;
    if(menuMainDraw_){
        OriginalUiMainDrawDiagnostic50KG drawDiagnostic;
        okay=menuMainDraw_->appendMenuWarning470(*this,receipt,geometry,drawDiagnostic);
        d.sourcePC=drawDiagnostic.sourcePC;
    }else if(menuOptionsDraw_){
        OriginalUiOptionsContributionDiagnostic50KG drawDiagnostic;
        okay=menuOptionsDraw_->appendMenuWarning470(*this,receipt,geometry,drawDiagnostic);
        d.sourcePC=drawDiagnostic.sourcePC;
    }
    if(!okay){
        menuWarningPhase_=MenuWarningPhase::failed;menu470Phase_=Menu470Phase::failed;
        d.status=Status::resourcePending;if(!d.sourcePC)d.sourcePC=0x246760;return false;
    }
    // Append succeeds once only. Retained-prefix custody preceded all allocation.
    ++menuWarningAppended_;return true;
}
bool OriginalUiSharedDialogs50KG::collectMenuWarning470Text(Text& text,std::size_t index,Text::Diagnostic& d)noexcept {
    d={};
    const auto stop=[&](std::uint32_t pc)noexcept {
        menuWarningPhase_=MenuWarningPhase::failed;menu470Phase_=Menu470Phase::failed;
        if(d.status==OriginalUiGarageScreenTextStatus50KG::complete)d.status=OriginalUiGarageScreenTextStatus50KG::authority;
        d.sourcePC=pc;return false;
    };
    if(!menuWarning470TextCurrent(text)||index>=text.active5E_||index>=text.publishedPrimitives_||
       !text.primitives4_||!text.primitives4_[index])return stop(0x2E01F0);
    bool enabled=false;Diagnostic admission;
    if(!admitMenuWarningCollection(text.primitives4_[index]->material00,enabled,admission))return stop(admission.sourcePC);
    if(!enabled){d.sourcePC=0x24680C;return true;}
    if(menuWarningIssued_>=menuWarningPackets_.size())return stop(0x246760);
    auto& receipt=menuWarningPackets_[menuWarningIssued_];
    if(!prepareMenuWarningPacket(receipt,OriginalUiMenuWarningPacket50KG::Kind::text))return stop(0x246760);
    if(!text.retainMenuWarningPacket(*this,index,receipt,d))return stop(d.sourcePC?d.sourcePC:0x246760);
    // Absolutely no fallible operation between leaf acquisition and custody.
    ++menuWarningIssued_;++nativeMenuWarning470Pins_;
    try{
        OriginalUiMenuWarningGeometry50KG geometry;
        if(!text.lowerMenuWarningPacket(*this,receipt,menuRaster_.xyOffsetFixed4,geometry,d))
            return stop(d.sourcePC?d.sourcePC:0x327ED8);
        Diagnostic appendDiagnostic;
        if(!appendMenuWarningPacket(receipt,geometry,appendDiagnostic))return stop(appendDiagnostic.sourcePC);
        return true;
    }catch(...){d.status=OriginalUiGarageScreenTextStatus50KG::allocation;return stop(0x327ED8);}
}
bool OriginalUiSharedDialogs50KG::collectMenuWarning470Quad(Quad& quad,Quad::Diagnostic& d)noexcept {
    d={};
    const auto stop=[&](std::uint32_t pc)noexcept {
        menuWarningPhase_=MenuWarningPhase::failed;menu470Phase_=Menu470Phase::failed;
        if(d.status==Quad::Status::complete)d.status=Quad::Status::phase;
        d.sourcePC=pc;return false;
    };
    if(!menuWarning470QuadCurrent(quad)||!quad.packetCurrent2DF550()||!quad.material54||
       quad.material54!=quad.primitiveOwner_.get())return stop(0x2DF8B0);
    bool enabled=false;Diagnostic admission;
    if(!admitMenuWarningCollection(quad.material54->material00,enabled,admission))return stop(admission.sourcePC);
    if(!enabled){d.sourcePC=0x24680C;return true;}
    if(menuWarningIssued_>=menuWarningPackets_.size())return stop(0x246760);
    auto& receipt=menuWarningPackets_[menuWarningIssued_];
    if(!prepareMenuWarningPacket(receipt,OriginalUiMenuWarningPacket50KG::Kind::quad))return stop(0x246760);
    if(!quad.retainMenuWarningPacket(*this,receipt,d))return stop(d.sourcePC?d.sourcePC:0x246760);
    ++menuWarningIssued_;++nativeMenuWarning470Pins_;
    OriginalUiMenuWarningGeometry50KG geometry;
    if(!quad.lowerMenuWarningPacket(*this,receipt,menuRaster_.xyOffsetFixed4,geometry,d))
        return stop(d.sourcePC?d.sourcePC:0x3281E8);
    Diagnostic appendDiagnostic;
    if(!appendMenuWarningPacket(receipt,geometry,appendDiagnostic))return stop(appendDiagnostic.sourcePC);
    return true;
}
bool OriginalUiSharedDialogs50KG::collectMenuWarning470Dim(Diagnostic& d)noexcept {
    d={};
    const auto stop=[&](std::uint32_t pc)noexcept {
        menuWarningPhase_=MenuWarningPhase::failed;menu470Phase_=Menu470Phase::failed;
        if(d.status==Status::complete)d.status=Status::resourcePending;
        d.sourcePC=pc;return false;
    };
    if(!menuWarning470DimCurrent()||!warning470DimPacketCurrent2E21D0()||dialog470_.dim160->dirty3C)
        return stop(0x2E23DC);
    bool enabled=false;
    if(!admitMenuWarningCollection(dialog470_.dim160->primitives30[0]->material00,enabled,d))return stop(d.sourcePC);
    if(!enabled){d.sourcePC=0x24680C;return true;}
    if(menuWarningIssued_>=menuWarningPackets_.size())return stop(0x2E23DC);
    auto& receipt=menuWarningPackets_[menuWarningIssued_];
    if(!prepareMenuWarningPacket(receipt,OriginalUiMenuWarningPacket50KG::Kind::dim))return stop(0x2E23DC);
    if(!retainMenuWarningDimPacket(receipt,d))return stop(d.sourcePC?d.sourcePC:0x2E23DC);
    ++menuWarningIssued_;++nativeMenuWarning470Pins_;
    OriginalUiMenuWarningGeometry50KG geometry;
    if(!lowerMenuWarningDimPacket(receipt,menuRaster_.xyOffsetFixed4,geometry,d))
        return stop(d.sourcePC?d.sourcePC:0x3281E8);
    if(!appendMenuWarningPacket(receipt,geometry,d))return stop(d.sourcePC);
    return true;
}

bool OriginalUiSharedDialogs50KG::menuWarningPacketsReleasable()const noexcept {
    // Deliberately NO Session, current renderer, material-history or raster
    // predicate. Failed GL work/target-generation changes cannot strand pins.
    if(menuWarningIssued_>menuWarningPackets_.size()||menuWarningAppended_>menuWarningIssued_||
       nativeMenuWarning470Pins_!=menuWarningIssued_)return false;
    std::array<std::uint32_t,2> textPins{};std::array<std::uint32_t,3> quadPins{};std::uint32_t dimPins=0;
    using PacketKind=OriginalUiMenuWarningPacket50KG::Kind;
    for(std::size_t i=0;i<menuWarningPackets_.size();++i){const auto& receipt=menuWarningPackets_[i];
        if(i>=menuWarningIssued_){if(receipt.active_)return false;continue;}
        if(!menuWarningPacketIdentity(receipt,i)||!menuWarningPacketPayloadMatches(receipt))return false;
        const auto& op=menuWarningPlan_.operations[receipt.operation_];
        switch(receipt.kind_){
            case PacketKind::text:++textPins[op.receiverOffset==0x1B0?0:1];break;
            case PacketKind::quad:++quadPins[(op.receiverOffset-0x40)/0x60];break;
            case PacketKind::dim:++dimPins;break;
            default:return false;
        }
        // No duplicate retained primitive and strictly source-ordered custody.
        for(std::size_t j=0;j<i;++j){const auto& prior=menuWarningPackets_[j];
            if((prior.payload_==receipt.payload_&&prior.primitive_==receipt.primitive_)||
               prior.operation_>receipt.operation_||
               (prior.operation_==receipt.operation_&&prior.primitiveIndex_>=receipt.primitiveIndex_))return false;
        }
    }
    if((dialog470_.message1B0?dialog470_.message1B0->nativeMenuWarningPins_:0)!=textPins[0]||
       (dialog470_.alternate220?dialog470_.alternate220->nativeMenuWarningPins_:0)!=textPins[1]||
       (dialog470_.dim160?dialog470_.dim160->nativeMenuWarningPins_:0)!=dimPins)return false;
    for(std::size_t i=0;i<quadPins.size();++i)if(dialog470_.strips[i].nativeMenuWarningPins_!=quadPins[i])return false;
    return true;
}
bool OriginalUiSharedDialogs50KG::menuWarningPacketsCurrent()const noexcept {
    if(!menuWarningPacketsReleasable())return false;
    if(menuWarningPhase_==MenuWarningPhase::idle)return !menuWarningIssued_&&!menuWarningAppended_;
    if(!menuWarningCompletedCurrent()||menuWarningAppended_!=menuWarningIssued_)return false;
    if(menuWarningPlan_.operationCount==1)return !menuWarningIssued_&&!menuRasterAttempted_;
    if(!menu470RasterCurrent())return false;
    for(std::size_t i=0;i<menuWarningIssued_;++i)if(!menuWarning470PacketCurrent(menuWarningPackets_[i]))return false;
    return true;
}
void OriginalUiSharedDialogs50KG::releaseMenuWarningPackets()noexcept {
    // Only the enclosing accepted/retired lifecycle invokes this after its
    // actual renderer boundary. Preflight the ENTIRE prefix before releasing.
    if(!menuWarningPacketsReleasable())std::terminate();
    using PacketKind=OriginalUiMenuWarningPacket50KG::Kind;
    for(std::size_t i=0;i<menuWarningIssued_;++i){auto& receipt=menuWarningPackets_[i];bool released=false;
        const auto& op=menuWarningPlan_.operations[receipt.operation_];
        switch(receipt.kind_){
            case PacketKind::text:{
                Text* text=op.receiverOffset==0x1B0?dialog470_.message1B0.get():dialog470_.alternate220.get();
                released=text->releaseMenuWarningPacket(receipt);break;
            }
            case PacketKind::quad:released=dialog470_.strips[(op.receiverOffset-0x40)/0x60].releaseMenuWarningPacket(receipt);break;
            case PacketKind::dim:released=releaseMenuWarningDimPacket(receipt);break;
            default:std::terminate();
        }
        if(!released)std::terminate();
        --nativeMenuWarning470Pins_;
    }
    menuWarningIssued_=0;menuWarningAppended_=0;
}
void OriginalUiSharedDialogs50KG::resetMenuWarningBody()noexcept {
    if(menuWarningIssued_||menuWarningAppended_||nativeMenuWarning470Pins_)std::terminate();
    for(auto& receipt:menuWarningPackets_){
        if(receipt.active_)std::terminate();
        receipt.owner_=nullptr;receipt.sourceOwner_=nullptr;
        receipt.invocation_=0;receipt.warningEpoch_=0;receipt.update_=0;receipt.rasterInvocation_=0;
        receipt.kind_=OriginalUiMenuWarningPacket50KG::Kind::none;receipt.payload_=nullptr;
        receipt.primitive_=nullptr;receipt.packet_=nullptr;receipt.operation_=0;receipt.primitiveIndex_=0;
        receipt.packetRevision_=0;receipt.materialToken_=-1;receipt.sourcePC_=0;
    }
    menuWarningPhase_=MenuWarningPhase::idle;menuWarningPlan_={};menuWarningNext_=0;
    menuWarningEpoch_=0;menuWarningInvocation_=0;menuWarningUpdate_=0;menuWarningCache_={};
    menuWarningSourcePC_=0;menuWarningEffects_=false;
}
}

namespace rr::platform {
bool OriginalUiSharedDialogs50KG::menuWarning470PacketImageCurrent(
    const OriginalUiMenuWarningPacket50KG& receipt,const assets::OriginalUiMemberView50KG& member)const noexcept {
    if(!menuWarning470PacketCurrent(receipt)||!member.identity||!member.data||
       member.size!=member.identity->bytes||(receipt.materialToken_!=3229&&receipt.materialToken_!=3230))return false;
    std::array<const assets::OriginalStandardMaterialObject50LA*,6> canonical{};
    if(!sharedHistoryCanonicalOwnersCurrent(canonical))return false;
    const auto* object=canonical[static_cast<std::size_t>(receipt.materialToken_-3228)];
    assets::OriginalStandardMaterialPassView50LA pass;
    assets::OriginalStandardTextureRowView50LA row;
    if(!object||!object->passCount48()||*object->passCount48()!=1||!object->pass4C(0,pass)||
       !pass.mapCount20||*pass.mapCount20!=1||!pass.flags21||!(*pass.flags21&2u)||
       pass.animation10||pass.textureAnimation10||!pass.texture1A||(*pass.texture1A)[0]<0||
       object->textures383400()!=inputs_.domain->textures383400().get()||
       !inputs_.domain->textures383400()->row((*pass.texture1A)[0],row)||!row.object04||
       !row.references08||*row.references08<=0)return false;
    const auto* texture=row.object04;const auto* bytes=texture->originalBytes();
    if(!texture->sourceMember()||!bytes||bytes->size()!=member.size||
       std::memcmp(bytes->data(),member.data,member.size)!=0)return false;
    const auto& op=menuWarningPlan_.operations[receipt.operation_];
    if(receipt.kind_==OriginalUiMenuWarningPacket50KG::Kind::text){
        const Text* text=op.receiverOffset==0x1B0?dialog470_.message1B0.get():
            op.receiverOffset==0x220?dialog470_.alternate220.get():nullptr;
        Text::FontAtlasBorrow atlas;Text::Diagnostic diagnostic;
        return text&&text->borrowMenuWarningFontAtlas(*this,receipt,atlas,diagnostic)&&
            atlas.font&&atlas.archive&&atlas.archive->loaded()&&
            atlas.archive->identity()==assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::fonts)&&
            atlas.canonicalTexture==(*pass.texture1A)[0]&&atlas.pst.identity==member.identity&&
            atlas.pst.data&&atlas.pst.size==member.size&&
            (atlas.pst.data==member.data||std::memcmp(atlas.pst.data,member.data,member.size)==0);
    }
    if(receipt.kind_!=OriginalUiMenuWarningPacket50KG::Kind::quad||op.receiverOffset<0x40||
       op.receiverOffset>0x100||(op.receiverOffset-0x40)%0x60)return false;
    const auto& quad=dialog470_.strips[(op.receiverOffset-0x40)/0x60];
    if(quad.garage_!=inputs_.popup||!quad.garage_||!quad.garage_->loaded()||
       quad.garage_->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::controllerPopup)||
       quad.offset!=op.receiverOffset||!quad.material54||quad.material54->material00!=3229)return false;
    const auto* source=object->source();const auto* sourceBytes=source?source->originalBytes():nullptr;
    if(!source||!source->member()||!sourceBytes)return false;
    // Prove the actual retained Popup PSM and texture member, not simply the
    // receipt's integer row. Equal bytes permit authenticated archive reentry.
    assets::OriginalUiMemberView50KG psm,pst;
    for(std::size_t i=0;i<quad.garage_->memberCount();++i){const auto candidate=quad.garage_->member(i);
        if(!candidate.identity||!candidate.identity->name)continue;
        if(std::strcmp(candidate.identity->name,"ui\\textures\\overlay1popup.psm")==0)psm=candidate;
        if(candidate.identity==member.identity)pst=candidate;
    }
    return psm.identity&&psm.data&&psm.size==psm.identity->bytes&&sourceBytes->size()==psm.size&&
        assets::OriginalStandardArchiveProvider50LA::sameName(*source->member(),psm.identity->name)&&
        std::memcmp(sourceBytes->data(),psm.data,psm.size)==0&&pst.identity&&pst.data&&
        pst.size==member.size&&assets::OriginalStandardArchiveProvider50LA::sameName(*texture->sourceMember(),pst.identity->name)&&
        (pst.data==member.data||std::memcmp(pst.data,member.data,member.size)==0);
}
}

namespace rr::platform {
bool OriginalUiSharedDialogs50KG::admitMenuWarningCollection(
    std::int32_t token,bool& enabled,Diagnostic& d)noexcept {
    d={};enabled=false;
    const auto stop=[&](std::uint32_t pc)noexcept {d.status=Status::resourcePending;d.sourcePC=pc;return false;};
    if(!menuWarningOperationCurrent()||!inputs_.root||!inputs_.domain||!inputs_.context||
       !inputs_.context->live()||token<3228||token>3230)return stop(0x246760);
    const auto& op=menuWarningPlan_.operations[menuWarningNext_];
    const auto expected=op.kind==Kind::callDim2E21D0?3228:op.kind==Kind::callStrip2DF550?3229:
        op.kind==Kind::callText2E0170?3230:-1;
    if(token!=expected)return stop(0x246798);
    const auto& runtime=inputs_.root->materialRuntime;
    if(!runtime.callbackOverrideKnown||runtime.callbackOverride435678)return stop(0x24A674);
    const auto& retainedRuntime=inputs_.context->materialRuntime();
    if(!retainedRuntime.callbackOverrideKnown||retainedRuntime.callbackOverride435678)return stop(0x24A674);
    std::array<const assets::OriginalStandardMaterialObject50LA*,6> canonical{};
    const auto table=inputs_.domain->materials382B38();
    const assets::OriginalStandardMaterialObject50LA* material=nullptr;
    assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
    if(!table||!sharedHistoryCanonicalOwnersCurrent(canonical)||
       !table->resolve298E60(token,material,materialDiagnostic)||
       material!=canonical[static_cast<std::size_t>(token-3228)]||!material||
       !material->passCount48()||*material->passCount48()!=1)return stop(0x246798);
    assets::OriginalStandardMaterialPassView50LA pass;
    if(!material->pass4C(0,pass)||!pass.flags21||!pass.references04||*pass.references04<=0||
       !pass.mapCount20||*pass.mapCount20!=(token==3228?0u:1u))return stop(0x24A728);
    // Actual disabled material-pass branch precedes prototype work and never
    // acquires a packet pin or appends geometry, even though leaf regeneration
    // already committed its source writes before reaching246760.
    if(!(*pass.flags21&2u)){d.sourcePC=0x24680C;return true;}
    // Bootstrap authenticates these six roles as static. Re-prove the actual
    // enabled pass has neither vertex nor texture prototype: its24A750 service
    // is genuinely empty. Nonempty controllers need their own future consumer;
    // do not skip them, call Garage service, or manufacture a collector receipt.
    if(pass.animation10||pass.textureAnimation10)return stop(0x24A750);
    enabled=true;d.sourcePC=0x24A790;return true;
}
}
