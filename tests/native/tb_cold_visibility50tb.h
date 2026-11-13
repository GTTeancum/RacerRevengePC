#pragma once
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "../../src/game/original_shared_rng50iu.h"

namespace tb_cold_visibility50TB {
// Terminal native cold lifetime bridge only. It cannot bind/start/complete a
// source query, create a reset lease, or issue a source retirement receipt.
class Retirement final {
    using LightOwner=rr::game::original_gc_gameplay_lighting_candidate::Owner;
    using LightDiagnostic=rr::game::original_gc_gameplay_lighting_candidate::Diagnostic;
    using LightStatus=rr::game::original_gc_gameplay_lighting_candidate::Status;
    static bool reject(std::size_t slot,LightDiagnostic& d) noexcept {
        d={};d.racer=static_cast<std::uint32_t>(slot);
        d.status=LightStatus::identity;d.sourcePC=0x216a90;return false;
    }
    static bool selected(const native_racer50HN::Bank& bank,std::size_t slot) noexcept {
        return !bank.retired_&&bank.sealed_&&slot<bank.count_&&
            (slot?bank.visiting_:bank.readBlocked_)&&
            bank.visitDepth_==1&&bank.currentSlot_==slot&&
            !bank.pendingToken_&&!bank.sourceIssuer_&&!bank.retirementVisit_&&
            !bank.nativeFifoPins_&&!bank.raWorldLifetime_&&
            bank.queryState_==native_racer50HN::Bank::QueryState::idle;
    }
    static bool exactSpan(const native_racer50HN::Bank& bank,const void* address,
        std::size_t bytes) noexcept {
        if(!address||!bytes)return false;
        for(const auto& span:bank.spans_)if(span.address==address&&span.bytes==bytes)return true;
        return false;
    }
    static bool actualArena(const native_racer50HN::Bank& bank,
        const rr::game::VehicleFrameState49W& frame,const rr::game::VehicleFrameBindings49W& bindings,
        const rr::game::VehicleFrameParameters49W& parameters) noexcept {
        // Every course saves the racer frame without the process-shared motion
        // and 308030 noise (one rule, original_shared_rng50iu.h), as GB/MC do.
        const bool frameRegistered=rr::game::racerFrameSharedExclusion50GB(frame,
            [&](const void* p,std::size_t bytes){return exactSpan(bank,p,bytes);},
            [&](const void* p,std::size_t bytes){
                const auto begin=reinterpret_cast<std::uintptr_t>(p),finish=begin+bytes;
                for(const auto& span:bank.spans_){const auto other=reinterpret_cast<std::uintptr_t>(span.address);
                    if(begin<other+span.bytes&&other<finish)return false;}
                return true;
            });
        return frameRegistered&&exactSpan(bank,&bindings,sizeof(bindings))&&
            exactSpan(bank,&parameters,sizeof(parameters));
    }
    static bool actualCables(const native_racer50HW::Rig& rig,
        const native_racer50HW::Cables& cables,const rr::game::VehicleFrameState49W& frame,
        const rr::game::VehicleFrameBindings49W& bindings) noexcept {
        using Rig=native_racer50HW::Rig;using Index=rr::game::SceneNodeIndex49P;
        if(!rig.directOriginal||cables.sourceRig_!=&rig||cables.nativeFifoPins_||cables.cameraPins_||
           frame.sceneNodeCount!=rig.nodeCount||bindings.sceneNodeCount!=rig.nodeCount||
           bindings.lifecycle.model1B4!=rig.roots[Rig::chariot]||
           bindings.lifecycle.left1BC!=rig.roots[Rig::left]||bindings.lifecycle.right1B8!=rig.roots[Rig::right]||
           bindings.visual.bottomLeft1C4!=(rig.modelCount==5?rig.roots[Rig::bottomLeft]:Index::none)||
           bindings.visual.bottomRight1C0!=(rig.modelCount==5?rig.roots[Rig::bottomRight]:Index::none))return false;
        // These immutable motion-view pointers are installed by this same Rig
        // and survive native model retirement. They distinguish source owners
        // even when their local root indices or authored bytes happen to match.
        const auto& a=bindings.animation;
        return a.left2A0==rig.motion(Rig::EngineLeft)&&a.right29C==rig.motion(Rig::EngineRight)&&
            a.brakeLeft28C==rig.motion(Rig::BrakeLeft)&&a.brakeRight290==rig.motion(Rig::BrakeRight)&&
            a.forward280==rig.motion(Rig::Forward)&&a.back284==rig.motion(Rig::Back)&&
            a.mid288==rig.motion(Rig::Mid)&&a.rightMid294==rig.motion(Rig::EngineRightMid)&&
            a.leftMid298==rig.motion(Rig::EngineLeftMid);
    }
public:
    Retirement()=delete;
    // Both calls occur synchronously inside the actual Bank.visit(slot).
    // Missing Rig/Cables may use only this read-only never-touched proof.
    static bool untouchedSlot(native_racer50HN::Bank& bank,std::size_t slot,
        LightOwner& visibility,LightDiagnostic& diagnostic) noexcept {
        if(!selected(bank,slot))return reject(slot,diagnostic);
        return visibility.tbColdVehicleUntouched(static_cast<std::uint32_t>(slot),diagnostic);
    }
    static bool retireSlot(native_racer50HN::Bank& bank,std::size_t slot,
        const rr::game::VehicleFrameState49W& frame,const rr::game::VehicleFrameBindings49W& bindings,
        const rr::game::VehicleFrameParameters49W& parameters,const native_racer50HW::Rig& rig,
        native_racer50HW::Cables& cables,LightOwner& visibility,LightDiagnostic& diagnostic) noexcept {
        if(!selected(bank,slot)||!actualArena(bank,frame,bindings,parameters)||
           !actualCables(rig,cables,frame,bindings))return reject(slot,diagnostic);
        return visibility.retireTbColdVehicle(static_cast<std::uint32_t>(slot),frame,bindings,parameters,
            &cables.visibility50(),diagnostic);
    }
};
} // namespace tb_cold_visibility50TB
