#pragma once
#include "original_race_record_insertion50kg.h"
#include "original_race_record_rule50kg.h"
#include <optional>

namespace rr::game {
// Pure readonly1ABFC0 projection. Does not write7E4,7E8, any Hall rank/name,
// dirty flag or save. No live caller is admitted. Only the actual profile owner
// may prepare it; an ordinary input/output snapshot confers no owner authority.
class OriginalRaceRecordQualification50KG final {
public:
    enum class Status:std::uint8_t {ok,profile,missingRacer,missingRaceFlags,storage,unsupportedNumeric};
    struct Diagnostic {
        Status status=Status::ok;
        std::uint32_t sourcePC=0,callPC=0,managerIndex=0;
    };
    struct Ranks {std::uint32_t lap=0,race=0,knockouts=0;};
    OriginalRaceRecordQualification50KG(const OriginalRaceRecordQualification50KG&)=default;
    OriginalRaceRecordQualification50KG& operator=(const OriginalRaceRecordQualification50KG&)=default;
    std::uint32_t originalV0()const noexcept{return originalV0_;}
    std::uint32_t selectorA1()const noexcept{return selector_;}
    std::uint32_t course()const noexcept{return course_;}
    std::uint64_t profileGeneration()const noexcept{return generation_;}
    // Absent means the corresponding source getter path was not represented
    // (guarded-out or not two-human), NOT a successfully read zero-stat racer.
    // Present ranks are query returns after source competition suppression;
    // these are NOT insertion ranks destined for11260..11274.
    const std::optional<Ranks>& firstRacerRanks()const noexcept{return first_;}
    const std::optional<Ranks>& secondRacerRanks()const noexcept{return second_;}
private:
    friend class rr::platform::OriginalUiRiderProfileOwner50KG;
    OriginalRaceRecordQualification50KG() noexcept=default;
    // The 1ABFC0 body lives in race_record_rule50kg (original_race_record_rule50kg.h),
    // shared with the DLC progress side file. This class keeps the owner gate.
    //Source a1:0 either player;1 first only;2 second only (see the rule).
    static bool prepare(const LapRecordConfig50ID& config,
        const OriginalUiHallRecords50KG& records,
        const std::array<std::uint32_t,27>& unlock10FAC,std::uint8_t selected1125F,
        std::optional<std::uint8_t> race1B0Low2,std::uint32_t selector,
        const OriginalRaceRecordQualificationRacerObservation50KG* rows,
        std::size_t rowCount,std::uint64_t generation,
        std::optional<OriginalRaceRecordQualification50KG>& out,Diagnostic& d) noexcept {
        namespace rule=race_record_rule50kg;
        d={};OriginalRaceRecordQualification50KG next;
        next.course_=std::uint32_t(selected1125F&15u);next.selector_=selector;
        next.generation_=generation;
        rule::QualificationResult result;rule::QualificationDiagnostic rd;
        const rule::HallRowTable table{records.courses.data(),records.courses.size()};
        if(!rule::qualify1ABFC0(config,table,unlock10FAC,next.course_,race1B0Low2,selector,
            rows,rowCount,result,rd)){
            Status status=Status::unsupportedNumeric;
            switch(rd.status){
            case rule::QualificationStatus::missingRacer:status=Status::missingRacer;break;
            case rule::QualificationStatus::missingRaceFlags:status=Status::missingRaceFlags;break;
            case rule::QualificationStatus::storage:status=Status::storage;break;
            default:break;
            }
            d={status,rd.sourcePC,rd.callPC,rd.managerIndex};return false;
        }
        next.originalV0_=result.originalV0;
        if(result.first)next.first_=Ranks{result.first->lap,result.first->race,result.first->knockouts};
        if(result.second)next.second_=Ranks{result.second->lap,result.second->race,result.second->knockouts};
        out=next;d={Status::ok,rd.sourcePC,0,0};return true;
    }
    std::uint32_t originalV0_=0,selector_=0,course_=0;
    std::uint64_t generation_=0;
    std::optional<Ranks> first_,second_;
};
}
