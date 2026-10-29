#pragma once
#include "original_wreck_components50la.h"
#include <memory>
#include <optional>
#include <cstdint>
#include <cstddef>
#include <thread>
#include <exception>
#include <utility>

namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_cold_roster50LA {class WreckBacking;}
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace original_camera_session50KX {class Session;}
namespace rr::game {
class OriginalRacerEntityCold50LA;
//Read-through lease of the SAME counted World and its source3FC/400 lanes.
//Only the actual aggregate can mint it; immutable authoring/height receipts
//cannot construct it. It owns no replacement water/terrain field or provider.
class OriginalRacerWorldBorrow50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class OriginalRacerEntityCold50LA;
    friend class ::ra_cold_roster50LA::WreckBacking;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    friend class ::original_camera_session50KX::Session;
    using IncomingZonesReader=bool(*)(const void*,const void*&,const void*&)noexcept;
    ::ra_cold_aggregate50LA::Owner* actualOwner_=nullptr;
    IncomingZonesReader incomingZonesReader_=nullptr;
    bool readIncoming210770(std::uint64_t generation,const std::uint32_t* actualObjectCount,
        const void*& visibility,const void*& collision)const noexcept {
        if(generation!=generation_||actualObjectCount!=objectCount_||!current()||
           !actualOwner_||actualOwner_!=keepalive_.get()||!incomingZonesReader_)return false;
        return incomingZonesReader_(keepalive_.get(),visibility,collision);
    }
    //Same aggregate identity minted by the private factory, not its Entity address.
    const ::ra_cold_aggregate50LA::Owner* actualCollectionWorld40()const noexcept {
        return current()?actualOwner_:nullptr;
    }
    std::shared_ptr<const void> keepalive_;
    const OriginalWreckComponent50LA* world_=nullptr;
    const std::optional<const void*> *terrain3FC_=nullptr,*water400_=nullptr;
    const bool *constructed_=nullptr,*resetComplete_=nullptr,*terminal_=nullptr,*closed_=nullptr,*revoked_=nullptr;
    const std::uint32_t *firstNoneReturn_=nullptr,*objectCount_=nullptr;
    const std::optional<std::uint32_t>* secondNoneReturn_=nullptr;
    std::size_t* borrowers_=nullptr;
    const std::uint64_t generation_;
    const std::thread::id thread_;
    OriginalRacerWorldBorrow50LA(std::shared_ptr<const void> keepalive,
        const OriginalWreckComponent50LA& world,
        const std::optional<const void*>& terrain3FC,const std::optional<const void*>& water400,
        const bool& constructed,const bool& resetComplete,const bool& terminal,const bool& closed,const bool& revoked,
        const std::uint32_t& firstNoneReturn,const std::optional<std::uint32_t>& secondNoneReturn,
        const std::uint32_t& objectCount,std::size_t& borrowers,std::thread::id thread,
        ::ra_cold_aggregate50LA::Owner& actualOwner,IncomingZonesReader incomingReader)
        :actualOwner_(&actualOwner),incomingZonesReader_(incomingReader),keepalive_(std::move(keepalive)),world_(&world),terrain3FC_(&terrain3FC),water400_(&water400),
         constructed_(&constructed),resetComplete_(&resetComplete),terminal_(&terminal),closed_(&closed),revoked_(&revoked),
         firstNoneReturn_(&firstNoneReturn),objectCount_(&objectCount),secondNoneReturn_(&secondNoneReturn),
         borrowers_(&borrowers),generation_(world.sessionGeneration),thread_(thread) {
        //The private factory preflights all identities before allocation. This
        //increment pins the real source owner before any Racer field effects.
        ++*borrowers_;
    }
    bool read2107FC(std::uint64_t generation,const std::uint32_t* actualObjectCount,
        const void*& terrain,const void*& water)const noexcept {
        if(generation!=generation_||actualObjectCount!=objectCount_||!current())return false;
        terrain=**terrain3FC_;water=**water400_;return true;
    }
public:
    OriginalRacerWorldBorrow50LA(const OriginalRacerWorldBorrow50LA&)=delete;
    OriginalRacerWorldBorrow50LA& operator=(const OriginalRacerWorldBorrow50LA&)=delete;
    ~OriginalRacerWorldBorrow50LA(){
        //Session/partial-constructor retirement must precede this lease. World
        //retirement is separately blocked by the same counter while it lives.
        if(thread_!=std::this_thread::get_id()||!borrowers_||!*borrowers_)std::terminate();
        --*borrowers_;
    }
    bool current()const noexcept {
        if(thread_!=std::this_thread::get_id()||!keepalive_||!world_||!generation_||
           !borrowers_||!*borrowers_||!constructed_||!*constructed_||!resetComplete_||!*resetComplete_||
           !terminal_||*terminal_||!closed_||*closed_||!revoked_||*revoked_||
           world_->reference00!=1u||world_->sessionGeneration!=generation_||!world_->objectKey||
           world_->kind!=OriginalWreckComponentKind50LA::entity141300||
           !terrain3FC_||!terrain3FC_->has_value()||**terrain3FC_||
           !water400_||!water400_->has_value()||**water400_||
           !firstNoneReturn_||*firstNoneReturn_!=1u||!secondNoneReturn_||
           !secondNoneReturn_->has_value()||**secondNoneReturn_!=1u)return false;
        return true; //Only the actually completed NONE source branch is admitted.
    }
    std::uint64_t generation()const noexcept{return generation_;}
};
}
