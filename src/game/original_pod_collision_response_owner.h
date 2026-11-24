#pragma once
#include "original_ai_engine_emitter.h"
#include "original_pod_material_audio.h"
#include "original_human_engine_reset50lb.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Canonical represented D44 fields for the eight PodCollisionResponseEmitter
// instances owned by PodRaceAudioManager.1A3C98..1A3CE0 allocates them into
// manager.C0[0..7].2C20F0 establishes24C=0 at2C213C;1AE510..1AE56C binds the
// selected row to the actual AI vehicle. The pool lives outside racer snapshots.
//
// Each row now owns the canonical material arrays/23C/240/244/24C state.
// Separate cold native sound construction binds its actual array entries.
// Source2C2810(a1=0) still writes only the SAME24C and skips virtualA8; it
// never resets a parallel subscriber, array, selected key or playback state.
class OriginalPodCollisionResponseOwners final {
public:
    static constexpr std::size_t count=8;
    class Emitter final {
        friend class OriginalPodCollisionResponseOwners;
        const OriginalPodCollisionResponseOwners* pool_=nullptr;
        const OriginalAiEngineEmitter* engine_=nullptr;
        const OriginalHumanEngineResetOwner50LB* human_=nullptr;
        // Native identity of the actual vehicle assigned by2C2230 to+248.
        // The engine's identity is stable session-owned metadata for that SAME
        // racer, not a copy of its bank-switched physics/frame storage.
        OriginalPodMaterialAudioState material_{};
        std::uint32_t poolOrdinal_=0;
        Emitter() noexcept=default;
    public:
        Emitter(const Emitter&)=delete;Emitter& operator=(const Emitter&)=delete;
        Emitter(Emitter&&)=delete;Emitter& operator=(Emitter&&)=delete;
        std::uint32_t poolOrdinal()const noexcept{return poolOrdinal_;}
        const OriginalAiEmitterIdentity* vehicle248()const noexcept{return material_.vehicle248;}
        std::int32_t driveMaterial240()const noexcept{return material_.driveMaterial240;}
        std::int32_t driveKey244()const noexcept{return material_.driveKey244;}
        std::uint32_t disabled24C()const noexcept{return material_.disabled24C;}
        OriginalPodMaterialAudioState& material() noexcept{return material_;}
        const OriginalPodMaterialAudioState& material() const noexcept{return material_;}
    };
private:
    std::array<Emitter,count> emitters_{};
    std::uint64_t session_=0;
    std::uint32_t humanCount_=0,opponentCount_=0;
    bool closed_=false;
public:
    OriginalPodCollisionResponseOwners() noexcept=default;
    ~OriginalPodCollisionResponseOwners(){close();}
    OriginalPodCollisionResponseOwners(const OriginalPodCollisionResponseOwners&)=delete;
    OriginalPodCollisionResponseOwners& operator=(const OriginalPodCollisionResponseOwners&)=delete;
    OriginalPodCollisionResponseOwners(OriginalPodCollisionResponseOwners&&)=delete;
    OriginalPodCollisionResponseOwners& operator=(OriginalPodCollisionResponseOwners&&)=delete;

    // Bounded actual roster: zero through two humans, one through eight total.
    // The actual packed profile is mandatory; unused fixed pool rows are not racers.
    // The source creates all eight rows in both cases;1AE524..1AE540 selects
    // each AI row by its opponent ordinal plus profile human count. A human
    // prefix is never rebound to an AI D40 or claimed as a completed AI owner.
    bool prepare(std::uint64_t session,std::uint16_t actualPacked1125C) noexcept {
        const auto humans=std::uint32_t((actualPacked1125C>>5u)&3u);
        const auto opponents=std::uint32_t((actualPacked1125C>>7u)&15u);
        if(closed_||session_||!session||humans>2u||! (humans+opponents)||humans+opponents>count)return false;
        session_=session;humanCount_=humans;opponentCount_=opponents;
        for(std::size_t i=0;i<count;++i){auto& row=emitters_[i];
            row.pool_=this;row.poolOrdinal_=static_cast<std::uint32_t>(i);
            //2C2130..213C. These are actual constructor fields, not material
            // playback results. Reset2C2810(0) preserves240/244/248.
            row.material_={};row.material_.vehicle248=nullptr;
            // Each genuine manager row receives the SAME manager+50 numeric
            // packet before2C2230 assigns a vehicle. This is not full source
            // GameObject/Aural registration or a fabricated sample lookup.
            if(!projectOriginalPodMaterialSetup2C21C0(row.material_))return false;
        }
        return true;
    }
    bool ownsAiSlot(std::size_t actualManagerOrdinal)const noexcept {
        return !closed_&&session_&&actualManagerOrdinal>=humanCount_&&
            actualManagerOrdinal-humanCount_<opponentCount_;
    }
    std::uint32_t humanCount()const noexcept{return humanCount_;}
    std::uint32_t racerCount()const noexcept{return humanCount_+opponentCount_;}
    // Retains the native lifetime/vehicle identity of the same selected row.
    // This does not execute source GameObject virtual0C or child traversal.
    // The source row is borrowed, never cloned into a racer-bank snapshot.
    Emitter* bindAiRacer(const OriginalAiEngineEmitter& actualD40,
        std::uint32_t actualOpponentOrdinal) noexcept {
        const auto& id=actualD40.identity;
        if(closed_||!session_||id.session!=session_||!id.racer||
           !actualD40.constructed||!actualD40.prepared||actualD40.blocked||
           actualOpponentOrdinal>=opponentCount_||
           id.managerOrdinal!=actualOpponentOrdinal+humanCount_||
           !ownsAiSlot(id.managerOrdinal))return nullptr;
        auto& row=emitters_[id.managerOrdinal];
        if(row.pool_!=this||row.engine_||row.human_||row.material_.vehicle248)return nullptr;
        row.engine_=&actualD40;row.material_.vehicle248=&id;return &row;
    }
    // The genuine human prefix row is already constructed by1A3C98. Bind
    // its actual native vehicle identity, without constructing an AI emitter
    // or claiming human material playback/source scheduling is implemented.
    Emitter* bindHumanRacer(const OriginalHumanEngineResetOwner50LB& actualD40) noexcept {
        const auto& id=actualD40.identity();
        if(closed_||!session_||!actualD40.bound()||
           id.session!=session_||id.managerOrdinal>=humanCount_||!id.racer)return nullptr;
        auto& row=emitters_[id.managerOrdinal];
        if(row.pool_!=this||row.engine_||row.human_||row.material_.vehicle248)return nullptr;
        row.human_=&actualD40;row.material_.vehicle248=&id;return &row;
    }
    std::uint32_t* resetHumanField24C(Emitter* actualD44,
        const OriginalHumanEngineResetOwner50LB& actualD40) noexcept {
        const auto& id=actualD40.identity();
        if(closed_||!session_||!actualD40.bound()||
           id.session!=session_||id.managerOrdinal>=humanCount_||!id.racer)return nullptr;
        auto& row=emitters_[id.managerOrdinal];
        if(actualD44!=&row||row.pool_!=this||row.human_!=&actualD40||
           row.engine_||row.material_.vehicle248!=&id)return nullptr;
        return &row.material_.disabled24C;
    }
    // Borrow only inside this actual racer's reset dispatch. This returns the
    // sole24C storage, suitable for resetUndertrackBasePrefix50ET beside the
    // SAME actualD40.flags22CLow4. Borrow ends before close/pool destruction.
    std::uint32_t* resetField24C(Emitter* actualD44,
        const OriginalAiEngineEmitter& actualD40) noexcept {
        const auto& id=actualD40.identity;
        if(id.session!=session_||!ownsAiSlot(id.managerOrdinal)||
           !actualD40.constructed||!actualD40.prepared||actualD40.blocked)return nullptr;
        auto& row=emitters_[id.managerOrdinal];
        if(actualD44!=&row||row.pool_!=this||row.engine_!=&actualD40||
           row.material_.vehicle248!=&id||!id.racer)return nullptr;
        return &row.material_.disabled24C;
    }
    // Native ownership release. All racer D44 borrows must already be gone;
    // close is terminal and cannot reconstruct a new zero-valued live pool.
    void close() noexcept {
        if(closed_)return;
        closed_=true;
        for(auto& row:emitters_){
            for(auto& sound:row.material_.collision){sound.nativeOwner=nullptr;sound.nativeSound=nullptr;sound.groupState44=nullptr;}
            for(auto& drive:row.material_.drive){drive.sound.nativeOwner=nullptr;drive.sound.nativeSound=nullptr;drive.sound.groupState44=nullptr;}
            row.material_.vehicle248=nullptr;row.engine_=nullptr;row.human_=nullptr;row.pool_=nullptr;
        }
        session_=0;humanCount_=0;opponentCount_=0;
    }
};
}
