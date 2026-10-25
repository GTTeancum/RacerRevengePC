#pragma once
// Isolated source-order adapter for the existing sealed 50HN arena and 49W
// frame. Install alongside the existing tests/native headers. No vehicle,
// pose-history, scene, query, contact, AI or random state is owned here.
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_observation50iz_fixture.h"
#include "../../src/game/advancement_selectors49k.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <utility>

namespace original_all8_frame {
using Bank=native_racer50HN::Bank;
using Reads=native_racer50IZ::FrameReads;
using Frame=rr::game::VehicleFrameState49W;
using Phase=rr::game::VehicleFramePhase49W;
inline constexpr std::size_t count=8;
// Mandatory on the new client entry. No choice certifies full original
// application/audio RNG chronology. The retained suffix policy is explicitly
// host-selected Player-derived audio over genuine unchanged ALL8 AI driving.
enum class AudioIntegrationPolicy : std::uint8_t {
    requireHostAudioDisabled,
    retainHostSelectedPlayerDerivedSuffixPolicy,
    nativeAiD40AtNativeAudioPhase
};
enum class Stage { Unbound,Idle,RacerChildren,RemainingRaceChildren,Physics,Complete,Blocked };
enum class Error { None,Binding,Order,Inactive,Phase,Factor,Operation,OneSubstepBoundary,Overflow };
struct Diagnostic {
    Error error=Error::None;
    std::uint32_t sourcePC=0;
    std::size_t managerIndex=0,bankSlot=0;
};
// Actual B0+44 manager order. Tokens identify real retained racer owners;
// neither manager index nor driving mode determines Player versus AI class.
struct RacerBinding {std::size_t bankSlot=0;std::uint64_t racerKey=0;};
class Schedule {
public:
    Schedule()=default;
    Schedule(const Schedule&)=delete;
    Schedule& operator=(const Schedule&)=delete;

    // Cold bind AFTER the existing bank is sealed and FrameReads is bound.
    // Actual manager34 determines only the proven child registration order:
    // 1AD684 appends [humanCount,total), then1AD6BC appends [0,humanCount).
    // Physics1A83D0 instead always walks manager order [0,total).
    bool bind(Bank& bank,Frame& arena,const Reads& reads,
        const std::array<RacerBinding,count>& managerOrder,
        std::uint32_t actualHumanCount34,std::uint64_t sessionGeneration) {
        if(advancing_||stage_!=Stage::Unbound||(!bank.count()||bank.count()>count)||!sessionGeneration||
           actualHumanCount34>bank.count())return reject(Error::Binding,0x1ad668u);
        const auto liveCount=bank.count();
        std::array<bool,count> seen{};
        for(std::size_t i=0;i<liveCount;++i){const auto& r=managerOrder[i];
            if(r.bankSlot>=liveCount||seen[r.bankSlot]||!r.racerKey)
                return reject(Error::Binding,0x1af330u,i,r.bankSlot);
            for(std::size_t j=0;j<i;++j)if(managerOrder[j].racerKey==r.racerKey)
                return reject(Error::Binding,0x1af330u,i,r.bankSlot);
            seen[r.bankSlot]=true;
        }
        // bindRead validates that these are the real registered arena fields;
        // this class must remain outside all banked owner spans.
        factor_=bank.bindRead(arena.bodies.main.frameFactor);
        substep_=bank.bindRead(arena.bodies.main.substepIndex);
        substepCount_=bank.bindRead(arena.bodies.main.schedule.count);
        bank_=&bank;arena_=&arena;reads_=&reads;manager_=managerOrder;
        session_=sessionGeneration;liveCount_=liveCount;
        std::size_t next=0;
        for(std::size_t i=actualHumanCount34;i<liveCount_;++i)children_[next++]=i;
        for(std::size_t i=0;i<actualHumanCount34;++i)children_[next++]=i;
        stage_=Stage::Idle;diagnostic_={};return true;
    }
    Stage stage() const noexcept{return stage_;}
    const Diagnostic& diagnostic() const noexcept{return diagnostic_;}
    std::uint64_t completedOuterFrames() const noexcept{return completed_;}
    std::uint64_t sessionGeneration() const noexcept{return session_;}
    std::size_t cameraRacer0BankSlot() const noexcept{return manager_[0].bankSlot;}
    std::size_t nextManagerIndex() const noexcept {
        return stage_==Stage::RacerChildren&&next_<liveCount_?children_[next_]:next_;
    }
    std::size_t nextBankSlot() const noexcept {
        const auto index=nextManagerIndex();return index<liveCount_?manager_[index].bankSlot:liveCount_;
    }

    // The caller supplies the real prepared outer f20 and current source4188.
    // No schedule arithmetic, RNG, history initialization or phase write here.
    bool begin(float preparedOuterFactor,float liveScale4188) {
        if(advancing_||(stage_!=Stage::Idle&&stage_!=Stage::Complete)||!bank_)
            return reject(Error::Order,0x1a8304u);
        if(completed_==std::numeric_limits<std::uint64_t>::max())
            return reject(Error::Overflow,0x1a8304u);
        // Existing50DU client clocks, speech and first-substep routes are a
        // fixed16ms slice. Accept actual matching owners, not arbitrary times.
        if(!same(preparedOuterFactor,.016f)||!same(liveScale4188,60.f))
            return reject(Error::OneSubstepBoundary,0x1a8390u);
        for(std::size_t i=0;i<liveCount_;++i){const auto slot=manager_[i].bankSlot;
            if(!bank_->active(slot))return reject(Error::Inactive,0x1a8304u,i,slot);
            if(bank_->read(slot,reads_->phase)!=Phase::Ready)
                return reject(Error::Phase,0x1a8304u,i,slot);
        }
        factorValue_=preparedOuterFactor;scaleValue_=liveScale4188;
        next_=0;stage_=Stage::RacerChildren;diagnostic_={};return true;
    }

    // Exact mixed prefix admission, usable by the existing scalar AI observer
    // BEFORE entering Bank::visit. Already dispatched children are at the real
    // AfterBodyChildren cut; undispatched children remain Ready. No phase is
    // rewritten or inferred from a successful boolean receipt.
    bool admitsChildrenObservation(const Bank& bank,const Reads& reads,
        std::size_t selfSlot) const {
        if(&bank!=bank_||&reads!=reads_||stage_!=Stage::RacerChildren||
           next_>=liveCount_||selfSlot!=nextBankSlot())return false;
        return exactPhases();
    }
    bool admitsMiddleObservation(const Bank& bank,const Reads& reads) const {
        return &bank==bank_ && &reads==reads_ && stage_==Stage::RemainingRaceChildren&&exactPhases();
    }
    // Dynamic peers are copied afresh immediately BEFORE each physics suffix,
    // outside its lease. Earlier manager indices are Ready; current/later are
    // still AfterBodyChildren. All current child-updated values are observed.
    bool admitsPhysicsObservation(const Bank& bank,const Reads& reads,
        std::size_t selfSlot) const {
        if(&bank!=bank_||&reads!=reads_||stage_!=Stage::Physics||next_>=liveCount_||
           selfSlot!=manager_[next_].bankSlot)return false;
        return exactPhases();
    }

    // before() runs outside the lease: refresh genuine AI scalar observations
    // and actual input ownership here. operation() is the EXISTING50DU prefix,
    // split immediately after refreshSecondaryVehicleFrame49W. It must execute
    // the actual50Y writer exactly once, retaining each live history owner.
    template<class Before,class Operation>
    bool advanceNextChildren(Before&& before,Operation&& operation) {
        if(advancing_||stage_!=Stage::RacerChildren||next_>=liveCount_||!exactPhases())
            return block(Error::Order,0x1a8304u);
        AdvanceLease lease{advancing_};
        const auto index=children_[next_],slot=manager_[index].bankSlot;
        try {
            if(!before(index,slot))return block(Error::Operation,0x1a8304u,index,slot);
            if(!exactPhases())return block(Error::Phase,0x1a8304u,index,slot);
            const bool okay=bank_->visit(slot,[&](){
                if(arena_->phase!=Phase::Ready)return false;
                return operation(index,slot)&&arena_->phase==Phase::AfterBodyChildren&&
                    same(arena_->bodies.main.frameFactor,factorValue_);
            });
            if(!okay)return block(Error::Operation,0x228f04u,index,slot);
            ++next_;
            if(next_==liveCount_){next_=0;stage_=Stage::RemainingRaceChildren;}
            if(!exactPhases())return block(Error::Phase,0x1a8304u,index,slot);
            return true;
        }catch(...){stage_=Stage::Blocked;diagnostic_={Error::Operation,0x1a8304u,index,slot};throw;}
    }

    // Mandatory actual native remaining-child operation: recovered scene34,
    // timing/rank B4 and camera48 plus the existing native world services.
    // The explicitly selected root policy does not claim every retail HUD or
    // audio child is represented. No fabricated success callback is admitted.
    // Camera0 may borrow its actual arena via bank.visit(slot).
    // No outstanding arena reader/cursor may survive this operation.
    template<class Operation> bool advanceRemainingChildren(Operation&& operation) {
        if(advancing_||stage_!=Stage::RemainingRaceChildren||!exactPhases())
            return block(Error::Order,0x1a8304u);
        AdvanceLease lease{advancing_};
        try {
            if(!operation(*this)||!exactPhases())return block(Error::Operation,0x1a8304u);
            // Original1A8390 follows the completed child traversal. Reuse the
            // existing numeric selector; do not introduce alternate rounding.
            rr::game::Schedule49K selected{};
            if(!rr::game::selectSchedule49K(factorValue_,scaleValue_,selected))
                return block(Error::Factor,0x1a8390u);
            // The retained50DU suffix hard-codes FIRST-substep query routes,
            // one tail/audio completion and count==1. General49W supports more
            // substeps, but this existing client does not. Reject, never run
            // racer-major multi-substep physics under a source-order claim.
            if(selected.count!=1u)return block(Error::OneSubstepBoundary,0x1a83bcu);
            next_=0;stage_=Stage::Physics;return true;
        }catch(...){stage_=Stage::Blocked;diagnostic_={Error::Operation,0x1a8304u,0,0};throw;}
    }

    // before() performs the genuine scheduled dynamic-peer capture (and its
    // actor projection) outside the lease. operation() runs the EXISTING50DU
    // scheduleVehicleFrame49W + one219810 suffix. Bank::step records one whole
    // completed opponent frame only here, never once for each half.
    template<class Before,class Operation>
    bool advanceNextPhysics(Before&& before,Operation&& operation) {
        if(advancing_||stage_!=Stage::Physics||next_>=liveCount_||!exactPhases())
            return block(Error::Order,0x1a83d0u);
        AdvanceLease lease{advancing_};
        const auto index=next_,slot=manager_[index].bankSlot;
        try {
            if(!before(index,slot)||!exactPhases())return block(Error::Operation,0x1a83d0u,index,slot);
            const auto execute=[&](){
                if(arena_->phase!=Phase::AfterBodyChildren)return false;
                return operation(index,slot,factorValue_,scaleValue_)&&arena_->phase==Phase::Ready&&
                    same(arena_->bodies.main.frameFactor,factorValue_)&&
                    arena_->bodies.main.schedule.count==1u&&arena_->bodies.main.substepIndex==1u;
            };
            const bool okay=slot?bank_->step(slot,true,execute):bank_->visit(slot,execute);
            if(!okay){bank_->stop(slot);return block(Error::Operation,0x219810u,index,slot);}
            ++next_;
            if(next_==liveCount_)stage_=Stage::Complete;
            if(!exactPhases())return block(Error::Phase,0x219810u,index,slot);
            if(stage_==Stage::Complete)++completed_;
            return true;
        }catch(...){stage_=Stage::Blocked;diagnostic_={Error::Operation,0x219810u,index,slot};throw;}
    }
private:
    struct AdvanceLease {
        bool& busy;
        explicit AdvanceLease(bool& value) noexcept:busy(value){busy=true;}
        ~AdvanceLease(){busy=false;}
    };
    template<class T> using Read=Bank::ReadHandle<T>;
    Bank* bank_=nullptr;Frame* arena_=nullptr;const Reads* reads_=nullptr;
    Read<float> factor_{};Read<std::uint32_t> substep_{},substepCount_{};
    std::array<RacerBinding,count> manager_{};
    std::array<std::size_t,count> children_{};
    Stage stage_=Stage::Unbound;Diagnostic diagnostic_{};bool advancing_=false;
    std::size_t next_=0,liveCount_=0;std::uint64_t session_=0,completed_=0;
    float factorValue_=0,scaleValue_=0;
    static bool same(float a,float b) noexcept {return std::memcmp(&a,&b,sizeof a)==0;}
    bool reject(Error e,std::uint32_t pc,std::size_t i=0,std::size_t slot=0) noexcept {
        diagnostic_={e,pc,i,slot};return false;
    }
    bool block(Error e,std::uint32_t pc,std::size_t i=0,std::size_t slot=0) noexcept {
        stage_=Stage::Blocked;return reject(e,pc,i,slot);
    }
    bool exactPhases() const {
        if(!bank_||bank_->count()!=liveCount_)return false;
        for(std::size_t index=0;index<liveCount_;++index){
            const auto slot=manager_[index].bankSlot;
            if(!bank_->active(slot))return false;
            Phase expected=Phase::Ready;bool prefixDone=false,physicsDone=false;
            switch(stage_){
            case Stage::RacerChildren:
                for(std::size_t p=0;p<next_;++p)if(children_[p]==index)prefixDone=true;
                expected=prefixDone?Phase::AfterBodyChildren:Phase::Ready;break;
            case Stage::RemainingRaceChildren:prefixDone=true;expected=Phase::AfterBodyChildren;break;
            case Stage::Physics:prefixDone=true;physicsDone=index<next_;
                expected=physicsDone?Phase::Ready:Phase::AfterBodyChildren;break;
            case Stage::Complete:prefixDone=physicsDone=true;expected=Phase::Ready;break;
            default:return false;
            }
            if(bank_->read(slot,reads_->phase)!=expected)return false;
            if(prefixDone&&!same(bank_->read(slot,factor_),factorValue_))return false;
            if(physicsDone&&(bank_->read(slot,substep_)!=1u||bank_->read(slot,substepCount_)!=1u))return false;
        }
        return true;
    }
};
} // namespace original_all8_frame
