#pragma once
#include <array>
#include <atomic>
#include <cstdint>
namespace rr::game {
//Source process pairs: GP3DD970+signed8BAC /443050 and
//GP3DD970+signed8C28 /4433D0; GP-0x74A0 /442F68 bounds profile.
//GP8C34 /4433E8 is the position setter lane.
//Startup BSS clearing gives zero guards.
//No App/World reset, public setter or claim that arbitrary guest aliases are
//disproved. All source callers must share this owner and serial native domain.
class OriginalRacerModelSceneOwner50LA;
class OriginalSourcePhysicsModel50LA;
class OriginalStandardPlacementGlobals50LA final {
    friend class OriginalRacerModelSceneOwner50LA;
    friend class OriginalSourcePhysicsModel50LA;
    struct Pair {std::atomic<std::uint8_t> guard{0};std::atomic<std::int32_t> word{0};};
    static std::array<Pair,4>& process()noexcept {static std::array<Pair,4> state;return state;}
public:
    struct History {
        bool entered=false,wordStored=false,guardStored=false;
        std::uint8_t guardRead=0;
        std::int32_t wordValue=0; //Parent source read; orientation native-history observation only.
    };
private:
    static bool enter(unsigned lane,std::int32_t initial,History& h)noexcept {
        if(h.entered)return false;auto& p=process()[lane];
        const auto guard=p.guard.load(std::memory_order_acquire);if(guard>1)return false;
        h.entered=true;h.guardRead=guard;
        if(!guard){
            p.word.store(initial,std::memory_order_relaxed);h.wordStored=true; //Actual word BEFORE guard.
            p.guard.store(1,std::memory_order_release);h.guardStored=true;
        }
        h.wordValue=p.word.load(std::memory_order_acquire);
        return h.wordValue==initial; //Other source writers are not silently assumed absent/adopted.
    }
    static bool current(unsigned lane,std::int32_t initial,const History& h)noexcept {
        if(!h.entered)return !h.wordStored&&!h.guardStored;
        const auto& p=process()[lane];
        return h.guardRead<=1&&h.wordStored==!h.guardRead&&h.guardStored==!h.guardRead&&
            h.wordValue==initial&&p.guard.load(std::memory_order_acquire)==1&&
            p.word.load(std::memory_order_acquire)==initial;
    }
public:
    static bool parent27AD48(History& h)noexcept{return enter(0,1,h);}
    static bool orientation289ECC(History& h)noexcept{return enter(1,-1,h);}
    static bool bounds26F5E4(History& h)noexcept{return enter(2,-1,h);}
    static void boundsNativeRouteUnknown()noexcept {
        //Native provenance marker2, not an original guard byte store. It
        //prevents a later source call from inventing startup-zero history.
        process()[2].guard.store(2,std::memory_order_release);
    }
    static bool boundsCurrent(const History& h)noexcept {
        if(!h.entered)return !h.wordStored&&!h.guardStored;
        //A later native adapter cannot erase earlier exact source history
        //or make native cancellation unsafe. Unknown blocks NEW enter only.
        const auto guard=process()[2].guard.load(std::memory_order_acquire);
        if(guard==2)return h.guardRead<=1&&h.wordStored==!h.guardRead&&
            h.guardStored==!h.guardRead&&h.wordValue==-1;
        return current(2,-1,h);
    }
private:
    //Same process certainty lane for actual28ADDC/gp8C34 and4433E8.
    //No per-App/global projection is adopted as source initialization proof.
    static bool enterPosition28ADD0(History& h)noexcept{return enter(3,-1,h);}
    static bool positionCurrent(const History& h)noexcept {
        if(!h.entered)return !h.wordStored&&!h.guardStored;
        const auto guard=process()[3].guard.load(std::memory_order_acquire);
        if(guard==2)return h.guardRead<=1&&h.wordStored==!h.guardRead&&
            h.guardStored==!h.guardRead&&h.wordValue==-1;
        return current(3,-1,h);
    }
public:
    static void positionNativeRouteUnknown()noexcept {
        //Native provenance marker, NOT a guest guard store or a reset of the
        //actual flight/nonflight projection. Those existing writes remain
        //unchanged. A reached native setter invalidates future source entry
        //even if its own projected guard was already warm. Earlier source
        //receipts remain valid historical evidence for refusal/retirement.
        process()[3].guard.store(2,std::memory_order_release);
    }
    static bool parentCurrent(const History& h)noexcept{return current(0,1,h);}
    static bool orientationCurrent(const History& h)noexcept{return current(1,-1,h);}
};
}
