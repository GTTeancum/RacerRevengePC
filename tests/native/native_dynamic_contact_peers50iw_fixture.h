#pragma once
// Bounded serial owner for original self338/globalB0 manager identity and live
// cross-racer observations. This registry is independent of the bank's reused
// execution arena addresses. It does not invent original all-body scheduling.
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_observation50iz_fixture.h"
#include "original_all8_two_phase_schedule.h"
#include "../../src/game/dynamic_impact50iw.h"
#include "../../src/game/vehicle_frame49w.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

namespace native_dynamic_contact50IW {
using namespace rr::game;
inline constexpr std::size_t maximumRacers=native_racer50HN::maximumRacers;
// Ordered by actual source manager index, not sorted by body identity or58.
// The caller binds one retained source manager lifetime; bankSlot is its actual
// existing racer slot and mainBody340 is that racer's stable opaque body token.
struct ManagerBinding {
    std::size_t bankSlot=0;
    BodyContactIdentity48R mainBody340{};
};
enum class Error {
    None,Unprepared,Roster,Inactive,Phase,Owners,ObservationOverflow
};
// Native admission diagnostic, not a fabricated source return/status.
struct Diagnostic {
    Error error=Error::None;
    std::uint32_t sourcePC=0;
    std::size_t bankSlot=0;
    std::int32_t managerIndex=-1;
};
struct Snapshot {
    std::array<DynamicImpactPeer50IW,maximumRacers> peers{};
    std::int32_t total30=0,humanCount34=0,selfManagerIndex=-1;
    std::size_t selfBankSlot=0;
    std::uint64_t observationSerial=0;
    bool bound=false;
    // Refresh only the currently executing racer's copied manager row at the
    // actual impact cut. Others retain their immediate pre-frame observations;
    // no bank visit or cross-racer mutation is permitted inside this frame.
    // Identity/index and the successful pre-frame observation serial are kept.
    // Failed admission leaves all peer observations unchanged.
    bool refreshSelfAtImpact(const VehicleFrameState49W& live,
        Diagnostic& diagnostic) noexcept {
        diagnostic={};
        const auto reject=[&](Error error,std::uint32_t pc){
            diagnostic={error,pc,selfBankSlot,selfManagerIndex};return false;
        };
        if(!bound)return reject(Error::Unprepared,0x22a280u);
        if(total30<1||total30>static_cast<std::int32_t>(maximumRacers)||
           selfManagerIndex<0||selfManagerIndex>=total30||
           selfBankSlot>=static_cast<std::size_t>(total30))
            return reject(Error::Roster,0x1af330u);
        auto& peer=peers[static_cast<std::size_t>(selfManagerIndex)];
        if(!peer.present||peer.managerIndex!=selfManagerIndex||
           !peer.mainBody340.value||!peer.liveInputsBound)
            return reject(Error::Roster,0x1af330u);
        if(live.phase!=VehicleFramePhase49W::MainIntegration)
            return reject(Error::Phase,0x219d98u);
        if(!live.fields.dynamicOwnersBound50IW)
            return reject(Error::Owners,0x223110u);
        const auto& main=live.bodies.main;
        peer.vehicle40=main.vehicleState40;peer.flagsCD8=live.fields.impactFlagsCD8;
        peer.vehicle58=live.fields.vehicle58;
        peer.partnerStateD34=live.impactSupplement.partnerState;
        peer.smoothedChannel0D28=main.controls.bound.control.shaping.smoothedChannel0D28;
        peer.worldAngularVelocityA4=main.components.root.worldAngularVelocityA0.v[1];
        peer.cachedSpeed34=main.cachedSpeed34;
        return true;
    }
    DynamicImpactManagerView50IW managerView() const noexcept {
        DynamicImpactManagerView50IW out{};
        out.peers=peers.data();out.peerCount=static_cast<std::size_t>(total30);
        out.total30=total30;out.humanCount34=humanCount34;out.bound=bound;
        return out;
    }
};
class Registry {
public:
    Registry()=default;
    Registry(const Registry&)=delete;
    Registry& operator=(const Registry&)=delete;
    // Fresh session only. No reset/rebind API: existing210 identities and D34
    // numeric indices must not silently resolve against a replacement roster.
    // This registry must outlive every pending210/23C and rememberedD34 in its bank.
    // Source humanCount34 is supplied from the authentic manager owner; actual58
    // is never inferred from that boundary, bankSlot, or a HOST driving role.
    bool prepare(const ManagerBinding* entriesInSourceManagerOrder,
        std::size_t total30,std::uint32_t humanCount34,std::string& error) {
        if(ready_||!entriesInSourceManagerOrder||total30<1||total30>maximumRacers||
           humanCount34>total30)
            return fail(error,"invalid fresh dynamic-contact source manager binding");
        std::array<bool,maximumRacers> seenSlots{};
        for(std::size_t index=0;index<total30;++index){
            const auto& entry=entriesInSourceManagerOrder[index];
            if(entry.bankSlot>=total30||seenSlots[entry.bankSlot]||!entry.mainBody340.value)
                return fail(error,"dynamic-contact manager slot/body identity is missing or duplicated");
            seenSlots[entry.bankSlot]=true;
            for(std::size_t prior=0;prior<index;++prior)
                if(entriesInSourceManagerOrder[prior].mainBody340.value==entry.mainBody340.value)
                    return fail(error,"dynamic-contact manager body identities are not unique");
        }
        for(std::size_t index=0;index<total30;++index){
            bindings_[index]=entriesInSourceManagerOrder[index];
            indexBySlot_[bindings_[index].bankSlot]=static_cast<std::int32_t>(index);
        }
        count_=total30;humanCount_=static_cast<std::int32_t>(humanCount34);
        ready_=true;error.clear();return true;
    }
    bool prepared() const noexcept{return ready_;}
    std::size_t count() const noexcept{return count_;}
    std::int32_t humanCount34() const noexcept{return humanCount_;}
    std::int32_t managerIndexForSlot(std::size_t bankSlot) const noexcept {
        return ready_&&bankSlot<count_?indexBySlot_[bankSlot]:-1;
    }
    // target1008 stores SOURCE manager identity, not a bank slot. Resolve it
    // in this same retained session registry without borrowing the live arena.
    BodyContactIdentity48R bodyIdentityForManagerIndex(std::uint32_t managerIndex) const noexcept {
        return ready_&&managerIndex<count_?bindings_[managerIndex].mainBody340:BodyContactIdentity48R{};
    }
    BodyContactIdentity48R bodyIdentityForSlot(std::size_t bankSlot) const noexcept {
        const auto index=managerIndexForSlot(bankSlot);
        return index<0?BodyContactIdentity48R{}:bindings_[static_cast<std::size_t>(index)].mainBody340;
    }
    bool matchesContactIdentities(const std::uint32_t* identities,
        std::size_t count) const noexcept {
        if(!ready_||!identities||count!=count_)return false;
        for(std::size_t slot=0;slot<count_;++slot)
            if(identities[slot]!=bodyIdentityForSlot(slot).value)return false;
        return true;
    }
    // Call immediately before EACH racer's frame, outside Bank::visit(), with
    // no outstanding borrowed arena readers. Peers remain unchanged while that
    // single racer advances. Reusing a once-per-world or collision-time image is
    // invalid: a prior human frame may have changed its persistent D34.state3.
    // Self arithmetic inputs must be projected anew from the current live frame
    // at223030, not from this pre-frame copy. Snapshot contains no arena pointer.
    // The read handles bind once after bank seal, but all values below come
    // from the current live slot0 or latest saved peer bytes on every call.
    // On failure destination and successful-observation serial are unchanged.
    bool captureBeforeFrame(const native_racer50HN::Bank& bank,
        const native_racer50IZ::FrameReads& live,std::size_t selfBankSlot,
        Snapshot& destination,Diagnostic& diagnostic) {
        return captureAtBoundary(bank,live,selfBankSlot,destination,diagnostic,nullptr);
    }
    // The source two-phase adapter verifies the exact current mixed phases:
    // previous physics racers Ready, current/later racers AfterBodyChildren.
    // This uses the SAME actual registry identities and observation serial.
    // It is not a second peer owner or a broad relaxation of Ready admission.
    bool captureBeforeScheduledPhysics(const original_all8_frame::Schedule& schedule,
        const native_racer50HN::Bank& bank,const native_racer50IZ::FrameReads& live,
        std::size_t selfBankSlot,Snapshot& destination,Diagnostic& diagnostic) {
        return captureAtBoundary(bank,live,selfBankSlot,destination,diagnostic,&schedule);
    }
private:
    bool captureAtBoundary(const native_racer50HN::Bank& bank,
        const native_racer50IZ::FrameReads& live,std::size_t selfBankSlot,
        Snapshot& destination,Diagnostic& diagnostic,
        const original_all8_frame::Schedule* schedule) {
        diagnostic={};
        const auto reject=[&](Error error,std::uint32_t pc,std::size_t slot,
                              std::int32_t index){
            diagnostic={error,pc,slot,index};return false;
        };
        if(!ready_)return reject(Error::Unprepared,0x22a280u,selfBankSlot,-1);
        if(bank.count()!=count_||selfBankSlot>=count_)
            return reject(Error::Roster,0x1af330u,selfBankSlot,-1);
        if(schedule&&!schedule->admitsPhysicsObservation(bank,live,selfBankSlot))
            return reject(Error::Phase,0x219810u,selfBankSlot,indexBySlot_[selfBankSlot]);
        if(observationSerial_==std::numeric_limits<std::uint64_t>::max())
            return reject(Error::ObservationOverflow,0x22a280u,selfBankSlot,-1);
        Snapshot staged{};staged.total30=static_cast<std::int32_t>(count_);
        staged.humanCount34=humanCount_;staged.selfManagerIndex=indexBySlot_[selfBankSlot];
        staged.selfBankSlot=selfBankSlot;
        for(std::size_t index=0;index<count_;++index){
            const auto slot=bindings_[index].bankSlot;
            if(!bank.active(slot))return reject(Error::Inactive,0x1af330u,slot,static_cast<std::int32_t>(index));
            if(!schedule&&bank.read(slot,live.phase)!=VehicleFramePhase49W::Ready)
                return reject(Error::Phase,0x219d98u,slot,static_cast<std::int32_t>(index));
            if(!bank.read(slot,live.dynamicOwnersBound))
                return reject(Error::Owners,0x223110u,slot,static_cast<std::int32_t>(index));
            auto& peer=staged.peers[index];
            peer.present=true;peer.managerIndex=static_cast<std::int32_t>(index);
            peer.mainBody340=bindings_[index].mainBody340;
            peer.vehicle40=bank.read(slot,live.vehicle40);peer.flagsCD8=bank.read(slot,live.flagsCD8);
            peer.vehicle58=bank.read(slot,live.vehicle58);
            peer.partnerStateD34=bank.read(slot,live.partnerStateD34);
            peer.smoothedChannel0D28=bank.read(slot,live.smoothedChannel0D28);
            peer.worldAngularVelocityA4=bank.read(slot,live.worldAngularVelocityA4);
            peer.cachedSpeed34=bank.read(slot,live.cachedSpeed34);peer.liveInputsBound=true;
        }
        staged.observationSerial=observationSerial_+1;staged.bound=true;
        destination=staged;observationSerial_=staged.observationSerial;return true;
    }
private:
    static bool fail(std::string& error,const char* why){error=why;return false;}
    std::array<ManagerBinding,maximumRacers> bindings_{};
    std::array<std::int32_t,maximumRacers> indexBySlot_{};
    std::size_t count_=0;
    std::int32_t humanCount_=0;
    std::uint64_t observationSerial_=0;
    bool ready_=false;
};
} // namespace native_dynamic_contact50IW
