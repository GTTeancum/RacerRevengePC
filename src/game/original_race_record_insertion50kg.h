#pragma once
#include "original_ui_hall_records50kg.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <optional>

namespace rr::platform { class OriginalUiRiderProfileOwner50KG; }
namespace rr::game {
// Per-profile native lifetime identity only. Retaining it prevents equal-row/
// equal-generation cross-profile reuse and address-reuse ABA. It is NOT an
// earned-result capability. Only the actual profile startup owner creates one.
class OriginalHallProfileIdentity50KG final {
public:
    ~OriginalHallProfileIdentity50KG()=default;
    OriginalHallProfileIdentity50KG(const OriginalHallProfileIdentity50KG&)=delete;
    OriginalHallProfileIdentity50KG& operator=(const OriginalHallProfileIdentity50KG&)=delete;
private:
    friend class rr::platform::OriginalUiRiderProfileOwner50KG;
    OriginalHallProfileIdentity50KG() noexcept=default;
};
// Values observed from the SAME actual 1AF330(managerB0,index) racer. This is
// ordinary data, NOT a proof of a finish, earned kills or permission to write.
// CB4/CB8 have live timing owners. Binding CD0 still requires the original
// destruction/credit owner; playerEventsCD0 is NOT an interchangeable source.
struct OriginalRaceRecordRacerObservation50KG {
    float bestLapCB4,timeCB8;
    std::uint32_t countCD0;
};

// Single/two-human 1AC440 preparation only. No application mutation, qualification
// query, dirty/save operation, Hall transition, callback or acknowledgment.
// Factory is private to the actual profile owner; snapshots do not confer live
// result authority. The output is ONE temporary course, not a new record table.
class OriginalRaceRecordInsertionStage50KG final {
public:
    enum class Status:std::uint8_t {
        ok,profile,unsupportedHumanCount,course,name,unsupportedNumeric
    };
    struct Diagnostic {Status status=Status::ok;std::uint32_t sourcePC=0;};
    struct Ranks {
        std::uint32_t lap11260=0,race11264=0,knockouts11268=0;
    };
    struct SecondHumanRanks {
        std::uint32_t lap1126C=0,race11270=0,knockouts11274=0;
    };
    OriginalRaceRecordInsertionStage50KG(const OriginalRaceRecordInsertionStage50KG&)=default;
    OriginalRaceRecordInsertionStage50KG& operator=(const OriginalRaceRecordInsertionStage50KG&)=default;
    std::uint32_t course()const noexcept{return course_;}
    std::uint64_t profileGeneration()const noexcept{return generation_;}
    std::uint32_t profileMode34()const noexcept{return mode34_;}
    const OriginalUiHallCourseRecords50KG& before()const noexcept{return before_;}
    const OriginalUiHallCourseRecords50KG& after()const noexcept{return after_;}
    const Ranks& ranks()const noexcept{return ranks_;}
    // Absent for single-human preparation: source does NOT clear these lanes
    // in that branch. Each present value is its original immediate helper
    // return, not a rank recomputed after the other human inserts a record.
    const std::optional<SecondHumanRanks>& secondHumanRanks()const noexcept {
        return secondHumanRanks_;
    }
private:
    friend class rr::platform::OriginalUiRiderProfileOwner50KG;
    OriginalRaceRecordInsertionStage50KG() noexcept=default;
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
        std::numeric_limits<float>::digits==24,"original binary32 record lanes");
    static bool fail(Diagnostic& d,Status status,std::uint32_t pc) noexcept {
        d={status,pc};return false;
    }
    static bool ordinary(float value) noexcept {
        return std::isfinite(value)&&(value==0.f||std::isnormal(value));
    }
    // 309980 strncpy(...,20). Shifted destinations keep their OWN byte20;
    // only a newly inserted name receives the explicit source byte20=0 store.
    // Copy via local bytes also avoids accidental aliasing of source/destination.
    static void copyName20(std::array<char,21>& destination,
        const std::array<char,21>& source) noexcept {
        std::array<char,20> prefix{};
        bool ended=false;
        for(std::size_t i=0;i<prefix.size();++i){
            if(!ended&&source[i]==0)ended=true;
            if(!ended)prefix[i]=source[i];
        }
        std::memcpy(destination.data(),prefix.data(),prefix.size());
    }
    template<class Value>
    static void insert(std::array<Value,3>& values,
        std::array<std::array<char,21>,3>& names,std::size_t rank,
        Value candidate,const std::array<char,21>& name) noexcept {
        // Source shifts rank2 -> rank3 BEFORE rank1 -> rank2. Do not sort the
        // row or re-rank ties. Preserve copied float representations exactly.
        for(std::size_t destination=2;destination>rank;--destination){
            std::memcpy(&values[destination],&values[destination-1],sizeof(Value));
            copyName20(names[destination],names[destination-1]);
        }
        std::memcpy(&values[rank],&candidate,sizeof(Value));
        copyName20(names[rank],name);names[rank][20]=0;
    }
    static bool timeRank(std::array<float,3>& values,
        std::array<std::array<char,21>,3>& names,float candidate,
        const std::array<char,21>& name,bool race,std::uint32_t& rank,
        Diagnostic& d) noexcept {
        const std::uint32_t lapPCs[]{0x1859acu,0x185a5cu,0x185ad8u};
        const std::uint32_t racePCs[]{0x185ca4u,0x185d54u,0x185dd0u};
        // Native ordinary-binary32 admission, NOT an original rejection or a
        // false qualification. NaN/infinity/subnormals stay unsupported. Leaf
        // gates precede this helper; timePair separately checks the two-human
        // outer comparison even when that leaf category will be skipped.
        if(!ordinary(candidate))return fail(d,Status::unsupportedNumeric,
            race?0x185c74u:0x18596cu);
        if(!race&&candidate<=0.f)return true; //185974 BC1F /18597C return.
        // 185C74 computes C.LE.S but NO branch consumes it. 185CA4 overwrites
        // the condition. Unlike readonly186280, race insertion has NO <=0 gate.
        for(std::size_t i=0;i<values.size();++i){
            if(!ordinary(values[i]))return fail(d,Status::unsupportedNumeric,
                race?racePCs[i]:lapPCs[i]);
            if(candidate<values[i]){
                insert(values,names,i,candidate,name);
                rank=static_cast<std::uint32_t>(i+1);return true;
            }
        }
        return true;
    }
    static void knockoutRank(std::array<std::uint32_t,3>& values,
        std::array<std::array<char,21>,3>& names,std::uint32_t candidate,
        const std::array<char,21>& name,std::uint32_t& rank) noexcept {
        for(std::size_t i=0;i<values.size();++i){
            //185F94/186040/1860B8 are SLTU, not signed compare. No cap7.
            if(values[i]<candidate){
                insert(values,names,i,candidate,name);
                rank=static_cast<std::uint32_t>(i+1);return;
            }
        }
    }
    static bool timePair(std::array<float,3>& values,
        std::array<std::array<char,21>,3>& names,float first,float second,
        const std::array<std::array<char,21>,2>& playerNames,bool race,
        bool leafEnabled,std::uint32_t& firstRank,std::uint32_t& secondRank,
        Diagnostic& d) noexcept {
        //1AC5E0/1AC6D8 compare BOTH human values before either writer checks
        //its gates. Unlike the single-human path, even a skipped category
        //requires supported operands for this reached outer C.LE.S.
        if(!ordinary(first)||!ordinary(second))return fail(d,Status::unsupportedNumeric,
            race?0x1ac6d8u:0x1ac5e0u);
        const bool firstBeforeSecond=first<=second; //Ties: manager row0 first.
        if(!leafEnabled)return true;
        if(firstBeforeSecond){
            if(!timeRank(values,names,first,playerNames[0],race,firstRank,d))return false;
            return timeRank(values,names,second,playerNames[1],race,secondRank,d);
        }
        if(!timeRank(values,names,second,playerNames[1],race,secondRank,d))return false;
        return timeRank(values,names,first,playerNames[0],race,firstRank,d);
    }
    // a2=0 automatic-insertion branch ONLY. The nonzero direct-slot seed
    // branches remain in1801D0's existing initializer; do not duplicate them.
    // Caller derives all profile/config/name arguments from its retained owner.
    // Failure leaves out unchanged; no temporary partial writes escape.
    static bool prepareSingleHuman(const OriginalUiHallRecords50KG& records,
        std::uint32_t currentSubtype11258,std::uint8_t currentLapCount1125C,
        std::uint16_t targetPacked1125C,std::uint8_t selected1125F,
        std::uint8_t race1B0,const OriginalRaceRecordRacerObservation50KG& racer,
        const std::array<char,21>& firstName,
        const std::shared_ptr<const OriginalHallProfileIdentity50KG>& identity,std::uint64_t generation,
        std::uint32_t mode34,std::optional<OriginalRaceRecordInsertionStage50KG>& out,
        Diagnostic& d) noexcept {
        d={};if(!identity)return fail(d,Status::profile,0x1ac440u);
        if((race1B0&3u)!=1u)return fail(d,Status::unsupportedHumanCount,0x1ac4ecu);
        const auto course=std::uint32_t(selected1125F&15u); //1AC47C
        if(course>=records.courses.size())return fail(d,Status::course,0x1ac47cu);
        if(!std::memchr(firstName.data(),0,firstName.size()))
            return fail(d,Status::name,mode34==2u?0x1ac49cu:0x1ac4b8u);
        OriginalRaceRecordInsertionStage50KG next;
        next.profileIdentity_=identity;
        next.course_=course;next.generation_=generation;next.mode34_=mode34;
        next.before_=records.courses[course];next.after_=next.before_;
        //1AC51C then11260;1AC554 then11264;1AC58C then11268. These are
        // proposed rank stores only. In particular CD4 is NOT a local gate:
        // eligibility/retirement belongs to preceding1ABFC0/1AB520.
        if(currentSubtype11258!=2u&&
           !timeRank(next.after_.lap102F0,next.after_.lapNames102FC,
             racer.bestLapCB4,firstName,false,next.ranks_.lap11260,d))return false;
        const auto humans=(targetPacked1125C>>5u)&3u;
        const auto opponents=(targetPacked1125C>>7u)&15u;
        const bool raceAndKnockouts=currentSubtype11258==1u&&
            (currentLapCount1125C&31u)==3u&&humans+opponents==8u;
        if(raceAndKnockouts){
            if(!timeRank(next.after_.race1033C,next.after_.raceNames10348,
                racer.timeCB8,firstName,true,next.ranks_.race11264,d))return false;
            knockoutRank(next.after_.knockouts10388,next.after_.knockoutNames10394,
                racer.countCD0,firstName,next.ranks_.knockouts11268);
        }
        out=next;d={Status::ok,0x1ac8bcu};return true;
    }
    //1AC5B4..1AC8BC. rows[0/1] MUST retain original manager getter order,
    //not position/rank order. Names likewise remain attached to player index.
    //Pure preparation only: these observations establish no earned ownership.
    static bool prepareTwoHumans(const OriginalUiHallRecords50KG& records,
        std::uint32_t currentSubtype11258,std::uint8_t currentLapCount1125C,
        std::uint16_t targetPacked1125C,std::uint8_t selected1125F,
        std::uint8_t race1B0,const std::array<OriginalRaceRecordRacerObservation50KG,2>& rows,
        const std::array<std::array<char,21>,2>& playerNames,
        const std::shared_ptr<const OriginalHallProfileIdentity50KG>& identity,std::uint64_t generation,
        std::uint32_t mode34,std::optional<OriginalRaceRecordInsertionStage50KG>& out,
        Diagnostic& d) noexcept {
        d={};if(!identity)return fail(d,Status::profile,0x1ac440u);
        if((race1B0&3u)!=2u)return fail(d,Status::unsupportedHumanCount,0x1ac5b4u);
        const auto course=std::uint32_t(selected1125F&15u);
        if(course>=records.courses.size())return fail(d,Status::course,0x1ac47cu);
        if(!std::memchr(playerNames[0].data(),0,playerNames[0].size()))
            return fail(d,Status::name,mode34==2u?0x1ac49cu:0x1ac4b8u);
        if(!std::memchr(playerNames[1].data(),0,playerNames[1].size()))
            return fail(d,Status::name,0x1ac4d8u);
        OriginalRaceRecordInsertionStage50KG next;
        next.profileIdentity_=identity;
        next.course_=course;next.generation_=generation;next.mode34_=mode34;
        next.before_=records.courses[course];next.after_=next.before_;
        next.secondHumanRanks_=SecondHumanRanks{};
        auto& secondRanks=*next.secondHumanRanks_;
        //First helper result is stored to its PLAYER lane before the second
        //insertion. Do not swap names/rank destinations with the sorted order.
        if(!timePair(next.after_.lap102F0,next.after_.lapNames102FC,
            rows[0].bestLapCB4,rows[1].bestLapCB4,playerNames,false,
            currentSubtype11258!=2u,next.ranks_.lap11260,secondRanks.lap1126C,d))return false;
        const auto humans=(targetPacked1125C>>5u)&3u;
        const auto opponents=(targetPacked1125C>>7u)&15u;
        const bool raceAndKnockouts=currentSubtype11258==1u&&
            (currentLapCount1125C&31u)==3u&&humans+opponents==8u;
        if(!timePair(next.after_.race1033C,next.after_.raceNames10348,
            rows[0].timeCB8,rows[1].timeCB8,playerNames,true,raceAndKnockouts,
            next.ranks_.race11264,secondRanks.race11270,d))return false;
        //1AC7D0 is signed SLT on the raw CD0 words, while185EC0 uses SLTU.
        //XOR sign-bit gives exact signed32 ordering without narrowing uint32
        //to host int32. Preserve this source distinction, even for high-bit
        //words; do not 'repair' already returned ranks after a later shift.
        const bool secondBeforeFirst=(rows[0].countCD0^0x80000000u)<
            (rows[1].countCD0^0x80000000u);
        if(raceAndKnockouts){
            const std::size_t order[]{secondBeforeFirst?1u:0u,secondBeforeFirst?0u:1u};
            for(const auto player:order){
                auto& rank=player==0?next.ranks_.knockouts11268:secondRanks.knockouts11274;
                knockoutRank(next.after_.knockouts10388,next.after_.knockoutNames10394,
                    rows[player].countCD0,playerNames[player],rank);
            }
        }
        out=next;d={Status::ok,0x1ac8bcu};return true;
    }
    std::uint32_t course_=0,mode34_=0;
    std::uint64_t generation_=0;
    OriginalUiHallCourseRecords50KG before_{},after_{};
    Ranks ranks_{};
    std::optional<SecondHumanRanks> secondHumanRanks_;
    std::shared_ptr<const OriginalHallProfileIdentity50KG> profileIdentity_;
};
}
