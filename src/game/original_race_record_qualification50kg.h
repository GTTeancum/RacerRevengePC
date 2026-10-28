#pragma once
#include "original_hall_record_queries50kg.h"
#include "original_race_record_insertion50kg.h"
#include <optional>

namespace rr::game {
// Same manager getter-index observations as insertion, with genuine CD4. A
// value is NOT a finished-race receipt or proof of earned CD0. In particular,
// no HOST clock, displayed rank or playerEventsCD0 may substitute here.
struct OriginalRaceRecordQualificationRacerObservation50KG {
    OriginalRaceRecordRacerObservation50KG records;
    std::uint32_t retirementCD4;
};

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
    static bool fail(Diagnostic& d,Status status,std::uint32_t pc,
        std::uint32_t callPC=0,std::uint32_t index=0) noexcept {
        d={status,pc,callPC,index};return false;
    }
    static bool retainQuery(LapRecordQueryResult50ID query,std::uint32_t callPC,
        std::uint32_t index,std::uint32_t& out,Diagnostic& d) noexcept {
        if(!query.completed())return fail(d,
            query.status==LapRecordQueryStatus50ID::Storage?Status::storage:Status::unsupportedNumeric,
            query.sourcePC,callPC,index);
        out=query.originalV0;return true;
    }
    static bool queryRacer(const LapRecordConfig50ID& config,
        const OriginalUiHallRecords50KG& records,std::uint32_t course,
        const OriginalRaceRecordQualificationRacerObservation50KG& racer,
        std::uint32_t index,Ranks& ranks,Diagnostic& d) noexcept {
        //1AC0A4/1AC128: retirement skips all three queries and their operands.
        if(racer.retirementCD4!=0u)return true;
        const LapRecordTableView50ID<OriginalUiHallCourseRecords50KG> table{
            records.courses.data(),records.courses.size()};
        if(!retainQuery(queryLapRecord50ID(config,table,course,racer.records.bestLapCB4),
            index==0u?0x1ac0bcu:0x1ac140u,index,ranks.lap,d))return false;
        if(!retainQuery(queryRaceRecord50ID(config,table,course,racer.records.timeCB8),
            index==0u?0x1ac0d8u:0x1ac15cu,index,ranks.race,d))return false;
        return retainQuery(queryOriginalHallKnockoutRecord50KG(config,records,course,
            racer.records.countCD0),index==0u?0x1ac0f4u:0x1ac178u,index,ranks.knockouts,d);
    }
    static bool competeTimes(float first,float second,float threshold,
        bool race,std::uint32_t& firstRank,std::uint32_t& secondRank,
        Diagnostic& d) noexcept {
        using lap_record_detail50ID::ordinary;
        //Reached only when BOTH original queries returned nonzero. Native
        //ordinary-float admission is distinct from source unordered behavior.
        if(!ordinary(first)||!ordinary(second))return fail(d,Status::unsupportedNumeric,
            race?0x1ac254u:0x1ac1a0u);
        const bool firstBeforeSecond=first<=second;
        if(!ordinary(threshold))return fail(d,Status::unsupportedNumeric,
            race?(firstBeforeSecond?0x1ac294u:0x1ac2dcu):
                 (firstBeforeSecond?0x1ac1e0u:0x1ac228u));
        //Original compares the losing candidate with the STORED SECOND slot,
        //not a hypothetical inserted table, the third slot, or query-rank math.
        if(firstBeforeSecond){if(!(second<threshold))secondRank=0;}
        else if(!(first<threshold))firstRank=0;
        return true;
    }
    static bool any(const Ranks& ranks) noexcept {
        return ranks.lap>0u||ranks.race>0u||ranks.knockouts>0u;
    }
    //Source a1:0 either player;1 first only;2 second only. All other uint32
    //selectors return0, but ONLY AFTER the same queries/competition are reached.
    //race1B0 low2==2 selects the second getter. Other values still read row0;
    //do not copy insertion's stricter1-or2 branch behavior into this query.
    static bool prepare(const LapRecordConfig50ID& config,
        const OriginalUiHallRecords50KG& records,
        const std::array<std::uint32_t,27>& unlock10FAC,std::uint8_t selected1125F,
        std::optional<std::uint8_t> race1B0Low2,std::uint32_t selector,
        const OriginalRaceRecordQualificationRacerObservation50KG* rows,
        std::size_t rowCount,std::uint64_t generation,
        std::optional<OriginalRaceRecordQualification50KG>& out,Diagnostic& d) noexcept {
        d={};OriginalRaceRecordQualification50KG next;
        next.course_=std::uint32_t(selected1125F&15u);next.selector_=selector;
        next.generation_=generation;
        //1AC034,1AC03C/44,1AC058/60,1AC074/7C. a1=0 in EACH profile helper:
        //188EC0/188F10/188FB0 therefore return raw10FAC/10FC0/10FE0 even
        //when profile.mode34==2. The a1=1 Tournament bypass does NOT apply.
        if(config.currentMode11258==2u||unlock10FAC[0]!=0u||
           unlock10FAC[5]!=0u||unlock10FAC[13]!=0u){
            out=next;d={Status::ok,0x1ac40cu,0,0};return true;
        }
        if(!rows||rowCount<1u)return fail(d,Status::missingRacer,0x1ac094u,0,0);
        next.first_=Ranks{};
        if(!queryRacer(config,records,next.course_,rows[0],0,*next.first_,d))return false;
        //Original reaches LBU1B0 only here, AFTER row0 queries. Missing
        //canonical low2 is not numeric1 from Camera or a source false result;
        //early exclusions above legitimately skip this dependency entirely.
        if(!race1B0Low2)return fail(d,Status::missingRaceFlags,0x1ac100u);
        if((*race1B0Low2&3u)==2u){
            if(rowCount<2u)return fail(d,Status::missingRacer,0x1ac118u,0,1);
            next.second_=Ranks{};
            if(!queryRacer(config,records,next.course_,rows[1],1,*next.second_,d))return false;
            auto& first=*next.first_;auto& second=*next.second_;
            //Nonzero queries have already bounded course against THIS table.
            //Retired/gated-out branches never read a competition operand.
            if(first.lap&&second.lap&&
               !competeTimes(rows[0].records.bestLapCB4,rows[1].records.bestLapCB4,
                   records.courses[next.course_].lap102F0[1],false,
                   first.lap,second.lap,d))return false;
            if(first.race&&second.race&&
               !competeTimes(rows[0].records.timeCB8,rows[1].records.timeCB8,
                   records.courses[next.course_].race1033C[1],true,
                   first.race,second.race,d))return false;
            if(first.knockouts&&second.knockouts){
                const auto a=rows[0].records.countCD0,b=rows[1].records.countCD0;
                const auto threshold=records.courses[next.course_].knockouts10388[1];
                //1AC308 signed SLT chooses losing player;1AC344/388 SLTU
                //then compares its RAW word with stored rank2. Ties favor0.
                if((a^0x80000000u)<(b^0x80000000u)){
                    if(!(threshold<a))first.knockouts=0;
                }else if(!(threshold<b))second.knockouts=0;
            }
        }
        if(selector<2u&&any(*next.first_))next.originalV0_=1u;
        else if((selector==0u||selector==2u)&&next.second_&&any(*next.second_))
            next.originalV0_=1u;
        out=next;d={Status::ok,0x1ac408u,0,0};return true;
    }
    std::uint32_t originalV0_=0,selector_=0,course_=0;
    std::uint64_t generation_=0;
    std::optional<Ranks> first_,second_;
};
}
