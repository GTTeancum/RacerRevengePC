#pragma once
#include "original_race_record_rule50kg.h"
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
    // The rule bodies live in race_record_rule50kg (original_race_record_rule50kg.h),
    // shared with the DLC progress side file. This stage keeps the owner,
    // identity, course and name gates and the before/after publication.
    static bool fail(Diagnostic& d,Status status,std::uint32_t pc) noexcept {
        d={status,pc};return false;
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
        //1AC51C..1AC5AC: shared rule body on the temporary after_ row.
        race_record_rule50kg::InsertionRanks ranks;std::uint32_t failPC=0;
        if(!race_record_rule50kg::insertSingleHuman1AC440(next.after_,currentSubtype11258,
            currentLapCount1125C,targetPacked1125C,racer,firstName,ranks,failPC))
            return fail(d,Status::unsupportedNumeric,failPC);
        next.ranks_={ranks.lap,ranks.race,ranks.knockouts};
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
        //1AC5B4..1AC8BC: shared rule body on the temporary after_ row.
        race_record_rule50kg::InsertionRanks first,second;std::uint32_t failPC=0;
        if(!race_record_rule50kg::insertTwoHumans1AC440(next.after_,currentSubtype11258,
            currentLapCount1125C,targetPacked1125C,rows,playerNames,first,second,failPC))
            return fail(d,Status::unsupportedNumeric,failPC);
        next.ranks_={first.lap,first.race,first.knockouts};
        secondRanks={second.lap,second.race,second.knockouts};
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
