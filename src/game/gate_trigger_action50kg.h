#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::game {

// GC Trigger9 only. Provenance: SLUS_202.68 SHA-256
// c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30;
// executable bytes were inspected as data, never executed. Original gc.toy
// SHA-256 a08d92a07942411dcbc8f487e0a25d717677f264f486725696e9749b44be8244.
//
// Preconditions: the caller has established an actual eligible contact with
// the authored Gate primitive. This is solely the action boundary; it neither
// detects a collision nor substitutes elapsed time, proximity, or race phase.
// A new state belongs to a new track instance, not a new frame or racer.
struct GateTriggerAction50KG {
    bool enabled=true; // Trigger+29 bit0; StartEnabled defaults true, 2DBE48.
    // 2A0290 constructor: exit callbacks bit0, enter callbacks bit1 enabled.
    // Trigger9's OneTimeOnly adds bit2 at 2DC03C. Bit3 is committed/fired,
    // bit4 current-contact, bit5 previous-contact. Bits4/5 are observational
    // for Gate's probability100 path, never another animation activation gate.
    std::uint8_t flags1B8=0x07;
    std::uint32_t lastCollider=0; // Native identity standing for Trigger+1B4.
};

struct GateTriggerCallbacks50KG {
    void* context=nullptr;
    // Source callback101170 always selects sequence0 of authored animation1.
    // Native bool returns are diagnostics; the original callbacks return void.
    bool (*activateAnimation)(void*,std::size_t ownerIndex)=nullptr;
    // Optional boundary for original action2, PodSound bigdoor.vag. A null
    // callback explicitly records unavailable sound, never fabricated success.
    bool (*podSound)(void*)=nullptr;
};

enum class GateTriggerStatus50KG : std::uint8_t {
    Disabled, AlreadyFired, AlreadyPaired, EnterCallbacksDisabled,
    Dispatched, ActionCallbackFailed
};

struct GateTriggerReceipt50KG {
    GateTriggerStatus50KG status=GateTriggerStatus50KG::Disabled;
    bool committed=false;
    bool animationDispatched=false;
    bool animationActivated=false;
    bool soundDispatched=false;
    bool soundUnavailable=false;
    bool soundSucceeded=false;
};

// Mirrors Gate's 2A00A0 -> 2A0E20 path. AlreadyPaired corresponds to the
// source primitive+CC/+D0 partner search, not a distance estimate. It is false
// for the pristine Gate's first contact. Parent owns any partner-list upkeep;
// one-shot admission makes subsequent enter dispatches impossible here.
inline GateTriggerReceipt50KG dispatchGateContact50KG(
        GateTriggerAction50KG& state,std::uint32_t collider,
        bool alreadyPaired,const GateTriggerCallbacks50KG& callbacks) noexcept {
    GateTriggerReceipt50KG receipt{};
    if(!state.enabled)return receipt; // 2A00BC..C8.
    state.flags1B8=static_cast<std::uint8_t>(state.flags1B8|0x10u); // 2A00CC..E0.
    if((state.flags1B8&0x0cu)==0x0cu){ // 2A00E4..110.
        receipt.status=GateTriggerStatus50KG::AlreadyFired;
        return receipt;
    }
    // RandomTriggerPercent defaults100 (2DBE84); 2A0114..120 bypasses
    // all RNG, including lazy seed/advance, for that exact value.
    state.flags1B8=static_cast<std::uint8_t>(state.flags1B8|0x08u); // 2A0244..268.
    receipt.committed=true;
    state.lastCollider=collider; // 2A0E40, before the partner search.
    if(alreadyPaired){
        receipt.status=GateTriggerStatus50KG::AlreadyPaired;
        return receipt;
    }
    if(!(state.flags1B8&0x02u)){
        receipt.status=GateTriggerStatus50KG::EnterCallbacksDisabled;
        return receipt;
    }
    // Action registration2DC2B8..2E8, append2A0A30, then ordered
    // dispatch2A0EAC..2AEC: animation first, PodSound second.
    if(callbacks.activateAnimation){
        receipt.animationDispatched=true;
        receipt.animationActivated=callbacks.activateAnimation(callbacks.context,0u);
    }
    if(callbacks.podSound){
        receipt.soundDispatched=true;
        receipt.soundSucceeded=callbacks.podSound(callbacks.context);
    }else receipt.soundUnavailable=true;
    // Keep source action order even if a native diagnostic reports failure.
    // Never roll back fired, requeue either callback, or retry a committed event.
    receipt.status=receipt.animationActivated&&
        (!receipt.soundDispatched||receipt.soundSucceeded)
        ?GateTriggerStatus50KG::Dispatched:GateTriggerStatus50KG::ActionCallbackFailed;
    return receipt;
}

struct GateTriggerBinding50KG {
    GateTriggerAction50KG state{};
    GateTriggerCallbacks50KG callbacks{};
    GateTriggerReceipt50KG lastReceipt{};
    std::uint64_t contacts=0,dispatches=0,soundUnavailable=0;
};

// The caller invokes this only after real contact admission. Counters describe
// native observations, not source state; they saturate rather than wrap.
inline GateTriggerReceipt50KG dispatchGateContact50KG(
        GateTriggerBinding50KG& binding,std::uint32_t collider,
        bool alreadyPaired=false) noexcept {
    constexpr auto max=~std::uint64_t{0};
    if(binding.contacts!=max)++binding.contacts;
    binding.lastReceipt=dispatchGateContact50KG(binding.state,collider,
        alreadyPaired,binding.callbacks);
    const auto& receipt=binding.lastReceipt;
    const bool enteredActions=receipt.status==GateTriggerStatus50KG::Dispatched||
        receipt.status==GateTriggerStatus50KG::ActionCallbackFailed;
    if(enteredActions&&binding.dispatches!=max)++binding.dispatches;
    if(receipt.soundUnavailable&&binding.soundUnavailable!=max)++binding.soundUnavailable;
    return receipt;
}

} // namespace rr::game
