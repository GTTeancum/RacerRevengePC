#pragma once
#include "original_shared_rng50iu.h"
#include <array>
#include <cstdint>
namespace rr::game {
// Source page1FB6A0, before its child traversal. These are original actions,
// not stand-in animations or particles. The enclosing scene must dispatch each
// to its real sequencer/emitter owner in this exact order, sharing one RNG.
enum class GarageTimedActionKind50GN : std::uint8_t {trigger,reset88,enable20,start80};
struct GarageTimedAction50GN {
 GarageTimedActionKind50GN kind;std::uint8_t index;std::uint32_t sourcePC;
 // trigger index is authored actor ordinal; other indices are emitter ordinals.
};
using GarageTimedDispatch50GN=bool(*)(void*,const GarageTimedAction50GN&)noexcept;
struct GarageTimedState50GN {float fire34B4=0,ext34B8=0,fall34BC=0;bool fireEnabled34C8=false,fallEnabled34CC=false;};
struct GarageTimedReceipt50GN {std::array<std::uint32_t,2> activationDraws{};std::uint32_t actions=0,completedBranches=0,sourcePC=0;std::int32_t lastEmitterLookup=-1;};
class OriginalGarageTimedActions50GN final {
 GarageTimedState50GN state_{};bool activated_=false,failed_=false,inCall_=false;
public:
 // Authenticated fresh zero-cash profile only. No purchase/progression write,
 // no synthetic seed and no manager lazy-seed update. Cold garageSparks setup
 // consumes its actual RNG draw BEFORE this operation in the enclosing scene.
 bool activateFreshZeroCash(std::int32_t actualCash,CoupledNoiseContext49M&,GarageTimedReceipt50GN&)noexcept;
 // No local pause check exists at1FB6A0. Caller must invoke only when the actual
 // page Entity traversal reaches it. It must not substitute app pause for this.
 // Rejection after a callback preserves its completed prefix and latches closed;
 // neither the frame nor shared random draws may be retried.
 bool step(float actualPageDelta,GarageTimedDispatch50GN,void*,GarageTimedReceipt50GN&)noexcept;
 const GarageTimedState50GN& state()const noexcept{return state_;}
 bool ready()const noexcept{return activated_&&!failed_;}
};
}
