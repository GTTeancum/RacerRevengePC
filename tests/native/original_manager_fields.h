#pragma once
#include "native_racer_bank50hn_fixture.h"
#include "lap_drive50gj_fixture.h"

// Six audited manager passes use these exact fields and stable shared owners.
// Keep their existing living addresses (including timingCursor.owner), but move
// only these fields between Bank snapshots. Unselected arena bytes stay player0.
// This is not a general partial vehicle execution API. No physics, audio service,
// scene query, other Bank visit, or retained arena reference is admitted here.
namespace original_manager_fields {
class Access final {
    struct Field {void* live=nullptr;std::size_t bytes=0,offset=0;};
    std::array<Field,8> fields_{};
    native_racer50HN::Bank* bank_=nullptr;
    rr::game::VehicleFrameState49W* state_=nullptr;
    lap_drive50GJ::Continuity* lap_=nullptr;
    std::size_t bytes_=0;
    void bind(native_racer50HN::Bank& bank,rr::game::VehicleFrameState49W& state,
        lap_drive50GJ::Continuity& lap){
        std::array<Field,8> fields{};std::size_t index=0,bytes=0;
        const auto add=[&](auto& value){
            static_assert(std::is_trivially_copyable_v<std::remove_reference_t<decltype(value)>>);
            const auto address=reinterpret_cast<std::uintptr_t>(&value);
            for(const auto& span:bank.spans_){
                const auto first=reinterpret_cast<std::uintptr_t>(span.address);
                if(address<first)continue;
                const auto within=address-first;
                if(within>span.bytes||sizeof(value)>span.bytes-within)continue;
                fields[index++]={&value,sizeof(value),span.offset+within};bytes+=sizeof(value);return;
            }
            throw std::runtime_error("manager field is not registered in this Bank");
        };
        add(state.fields.vehicle3C);add(state.fields.vehicle58);
        add(state.fields.impactFlagsCD8);add(state.fields.applicationByte58);
        add(lap.state.vehicleCC8);add(lap.state.vehicleCD8);
        add(lap.timing);add(lap.timingCursor);
        for(std::size_t i=0;i<fields.size();++i)for(std::size_t j=0;j<i;++j)
            if(fields[i].offset<fields[j].offset+fields[j].bytes&&
               fields[j].offset<fields[i].offset+fields[i].bytes)
                throw std::runtime_error("overlapping manager fields");
        fields_=fields;bytes_=bytes;bank_=&bank;state_=&state;lap_=&lap;
    }
    void save(std::size_t slot)noexcept{
        for(const auto& field:fields_)std::memcpy(bank_->slots_[slot].bytes.data()+field.offset,field.live,field.bytes);
    }
    void restore(std::size_t slot)noexcept{
        for(const auto& field:fields_)std::memcpy(field.live,bank_->slots_[slot].bytes.data()+field.offset,field.bytes);
    }
public:
    Access()=default;
    Access(const Access&)=delete;Access& operator=(const Access&)=delete;
    std::size_t bytesPerRacer()const noexcept{return bytes_;}
    template<class Operation>void visitAll(native_racer50HN::Bank& bank,
        rr::game::VehicleFrameState49W& state,lap_drive50GJ::Continuity& lap,Operation&& operation){
        // This access is only at an ordinary idle manager boundary, never a
        // retirement/animation exception or a nested current-arena callback.
        if(bank.retired_||!bank.sealed_||!bank.count_||bank.visiting_||bank.readBlocked_||
           bank.visitDepth_||bank.currentSlot_||bank.pendingToken_||bank.managerFieldsVisit_||
           bank.animationBlocked_||bank.queryActive_||
           bank.terminalBlocked_||bank.retirementVisit_)
            throw std::runtime_error("manager fields require an idle sealed Bank");
        if(!bank_)bind(bank,state,lap);
        if(bank_!=&bank||state_!=&state||lap_!=&lap)
            throw std::runtime_error("manager field owner changed");
        bank.visiting_=bank.readBlocked_=bank.managerFieldsVisit_=true;++bank.visitDepth_;
        std::size_t current=0;
        struct Restore {
            Access& access;native_racer50HN::Bank& bank;std::size_t& current;
            ~Restore()noexcept{
                // Same partial-state-on-throw rule as full Bank visits. Never
                // save unrelated player bytes into an opponent snapshot.
                access.save(current);if(current)access.restore(0);
                bank.currentSlot_=0;--bank.visitDepth_;
                bank.visiting_=bank.readBlocked_=bank.managerFieldsVisit_=false;
            }
        } scope{*this,bank,current};
        for(std::size_t slot=0;slot<bank.count_;++slot){
            if(slot!=current){save(current);restore(slot);current=slot;}
            bank.currentSlot_=slot;operation(slot);
        }
    }
};
}
