#pragma once
#include "original_hall_record_queries50kg.h"
#include "original_ui_hall_records50kg.h"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <optional>

namespace rr::game {
// Values observed from the SAME actual 1AF330(managerB0,index) racer. This is
// ordinary data, NOT a proof of a finish, earned kills or permission to write.
// CB4/CB8 have live timing owners. Binding CD0 still requires the original
// destruction/credit owner; playerEventsCD0 is NOT an interchangeable source.
struct OriginalRaceRecordRacerObservation50KG {
    float bestLapCB4,timeCB8;
    std::uint32_t countCD0;
};
// Same manager getter-index observations as insertion, with genuine CD4. A
// value is NOT a finished-race receipt or proof of earned CD0. In particular,
// no HOST clock, displayed rank or playerEventsCD0 may substitute here.
struct OriginalRaceRecordQualificationRacerObservation50KG {
    OriginalRaceRecordRacerObservation50KG records;
    std::uint32_t retirementCD4;
};

// The ONE copy of the source record rules 1ABFC0 (qualification) and 1AC440
// (insertion, a2=0 automatic branch), applied to Hall rows of the original
// OriginalUiHallCourseRecords50KG shape. Pure functions over caller-owned
// rows: no profile, owner, identity, dirty flag, save or generation. The stock
// Hall owners (OriginalRaceRecordQualification50KG and
// OriginalRaceRecordInsertionStage50KG) keep their owner/identity gates and
// call these bodies; the DLC progress side file (src\assets\dlc_progress50dl)
// calls the same bodies on its own port-owned rows. Nothing here may be copied.
namespace race_record_rule50kg {
using Name=std::array<char,21>;
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
    std::numeric_limits<float>::digits==24,"original binary32 record lanes");

inline bool ordinary(float value) noexcept {
    return std::isfinite(value)&&(value==0.f||std::isnormal(value));
}
// 309980 strncpy(...,20). Shifted destinations keep their OWN byte20;
// only a newly inserted name receives the explicit source byte20=0 store.
// Copy via local bytes also avoids accidental aliasing of source/destination.
inline void copyName20(Name& destination,const Name& source) noexcept {
    std::array<char,20> prefix{};
    bool ended=false;
    for(std::size_t i=0;i<prefix.size();++i){
        if(!ended&&source[i]==0)ended=true;
        if(!ended)prefix[i]=source[i];
    }
    std::memcpy(destination.data(),prefix.data(),prefix.size());
}
template<class Value>
inline void insert(std::array<Value,3>& values,std::array<Name,3>& names,
    std::size_t rank,Value candidate,const Name& name) noexcept {
    // Source shifts rank2 -> rank3 BEFORE rank1 -> rank2. Do not sort the
    // row or re-rank ties. Preserve copied float representations exactly.
    for(std::size_t destination=2;destination>rank;--destination){
        std::memcpy(&values[destination],&values[destination-1],sizeof(Value));
        copyName20(names[destination],names[destination-1]);
    }
    std::memcpy(&values[rank],&candidate,sizeof(Value));
    copyName20(names[rank],name);names[rank][20]=0;
}
// Returns false only for unsupported numerics; failPC names the source site.
inline bool timeRank(std::array<float,3>& values,std::array<Name,3>& names,
    float candidate,const Name& name,bool race,std::uint32_t& rank,
    std::uint32_t& failPC) noexcept {
    const std::uint32_t lapPCs[]{0x1859acu,0x185a5cu,0x185ad8u};
    const std::uint32_t racePCs[]{0x185ca4u,0x185d54u,0x185dd0u};
    // Native ordinary-binary32 admission, NOT an original rejection or a
    // false qualification. NaN/infinity/subnormals stay unsupported. Leaf
    // gates precede this helper; timePair separately checks the two-human
    // outer comparison even when that leaf category will be skipped.
    if(!ordinary(candidate)){failPC=race?0x185c74u:0x18596cu;return false;}
    if(!race&&candidate<=0.f)return true; //185974 BC1F /18597C return.
    // 185C74 computes C.LE.S but NO branch consumes it. 185CA4 overwrites
    // the condition. Unlike readonly186280, race insertion has NO <=0 gate.
    for(std::size_t i=0;i<values.size();++i){
        if(!ordinary(values[i])){failPC=race?racePCs[i]:lapPCs[i];return false;}
        if(candidate<values[i]){
            insert(values,names,i,candidate,name);
            rank=static_cast<std::uint32_t>(i+1);return true;
        }
    }
    return true;
}
inline void knockoutRank(std::array<std::uint32_t,3>& values,
    std::array<Name,3>& names,std::uint32_t candidate,const Name& name,
    std::uint32_t& rank) noexcept {
    for(std::size_t i=0;i<values.size();++i){
        //185F94/186040/1860B8 are SLTU, not signed compare. No cap7.
        if(values[i]<candidate){
            insert(values,names,i,candidate,name);
            rank=static_cast<std::uint32_t>(i+1);return;
        }
    }
}
inline bool timePair(std::array<float,3>& values,std::array<Name,3>& names,
    float first,float second,const std::array<Name,2>& playerNames,bool race,
    bool leafEnabled,std::uint32_t& firstRank,std::uint32_t& secondRank,
    std::uint32_t& failPC) noexcept {
    //1AC5E0/1AC6D8 compare BOTH human values before either writer checks
    //its gates. Unlike the single-human path, even a skipped category
    //requires supported operands for this reached outer C.LE.S.
    if(!ordinary(first)||!ordinary(second)){failPC=race?0x1ac6d8u:0x1ac5e0u;return false;}
    const bool firstBeforeSecond=first<=second; //Ties: manager row0 first.
    if(!leafEnabled)return true;
    if(firstBeforeSecond){
        if(!timeRank(values,names,first,playerNames[0],race,firstRank,failPC))return false;
        return timeRank(values,names,second,playerNames[1],race,secondRank,failPC);
    }
    if(!timeRank(values,names,second,playerNames[1],race,secondRank,failPC))return false;
    return timeRank(values,names,first,playerNames[0],race,firstRank,failPC);
}
struct InsertionRanks {std::uint32_t lap=0,race=0,knockouts=0;};
// Race and knockout categories: Single Race subtype 1, 3 laps, full field of 8.
inline bool raceAndKnockoutsEnabled1AC440(std::uint32_t currentSubtype11258,
    std::uint8_t currentLapCount1125C,std::uint16_t targetPacked1125C) noexcept {
    const auto humans=(targetPacked1125C>>5u)&3u;
    const auto opponents=(targetPacked1125C>>7u)&15u;
    return currentSubtype11258==1u&&(currentLapCount1125C&31u)==3u&&humans+opponents==8u;
}
// 1AC51C..1AC5AC on ONE row. On false the row may hold a partial effect:
// callers apply this to a temporary copy and publish only on true.
inline bool insertSingleHuman1AC440(OriginalUiHallCourseRecords50KG& row,
    std::uint32_t currentSubtype11258,std::uint8_t currentLapCount1125C,
    std::uint16_t targetPacked1125C,const OriginalRaceRecordRacerObservation50KG& racer,
    const Name& firstName,InsertionRanks& ranks,std::uint32_t& failPC) noexcept {
    //1AC51C then11260;1AC554 then11264;1AC58C then11268. These are
    // proposed rank stores only. In particular CD4 is NOT a local gate:
    // eligibility/retirement belongs to preceding1ABFC0/1AB520.
    if(currentSubtype11258!=2u&&
       !timeRank(row.lap102F0,row.lapNames102FC,racer.bestLapCB4,firstName,false,
           ranks.lap,failPC))return false;
    if(raceAndKnockoutsEnabled1AC440(currentSubtype11258,currentLapCount1125C,targetPacked1125C)){
        if(!timeRank(row.race1033C,row.raceNames10348,racer.timeCB8,firstName,true,
            ranks.race,failPC))return false;
        knockoutRank(row.knockouts10388,row.knockoutNames10394,racer.countCD0,
            firstName,ranks.knockouts);
    }
    return true;
}
//1AC5B4..1AC8BC on ONE row. rows[0/1] MUST retain original manager getter
//order, not position/rank order. Names likewise remain attached to player index.
inline bool insertTwoHumans1AC440(OriginalUiHallCourseRecords50KG& row,
    std::uint32_t currentSubtype11258,std::uint8_t currentLapCount1125C,
    std::uint16_t targetPacked1125C,
    const std::array<OriginalRaceRecordRacerObservation50KG,2>& rows,
    const std::array<Name,2>& playerNames,InsertionRanks& first,InsertionRanks& second,
    std::uint32_t& failPC) noexcept {
    //First helper result is stored to its PLAYER lane before the second
    //insertion. Do not swap names/rank destinations with the sorted order.
    if(!timePair(row.lap102F0,row.lapNames102FC,rows[0].bestLapCB4,rows[1].bestLapCB4,
        playerNames,false,currentSubtype11258!=2u,first.lap,second.lap,failPC))return false;
    const bool raceAndKnockouts=raceAndKnockoutsEnabled1AC440(currentSubtype11258,
        currentLapCount1125C,targetPacked1125C);
    if(!timePair(row.race1033C,row.raceNames10348,rows[0].timeCB8,rows[1].timeCB8,
        playerNames,true,raceAndKnockouts,first.race,second.race,failPC))return false;
    //1AC7D0 is signed SLT on the raw CD0 words, while185EC0 uses SLTU.
    //XOR sign-bit gives exact signed32 ordering without narrowing uint32
    //to host int32. Preserve this source distinction, even for high-bit
    //words; do not 'repair' already returned ranks after a later shift.
    const bool secondBeforeFirst=(rows[0].countCD0^0x80000000u)<
        (rows[1].countCD0^0x80000000u);
    if(raceAndKnockouts){
        const std::size_t order[]{secondBeforeFirst?1u:0u,secondBeforeFirst?0u:1u};
        for(const auto player:order){
            auto& rank=player==0?first.knockouts:second.knockouts;
            knockoutRank(row.knockouts10388,row.knockoutNames10394,
                rows[player].countCD0,playerNames[player],rank);
        }
    }
    return true;
}

// ---- 1ABFC0 qualification (readonly) ----
enum class QualificationStatus:std::uint8_t {ok,missingRacer,missingRaceFlags,storage,unsupportedNumeric};
struct QualificationDiagnostic {
    QualificationStatus status=QualificationStatus::ok;
    std::uint32_t sourcePC=0,callPC=0,managerIndex=0;
};
struct QualificationRanks {std::uint32_t lap=0,race=0,knockouts=0;};
struct QualificationResult {
    std::uint32_t originalV0=0;
    // Absent means the corresponding source getter path was not represented
    // (guarded-out or not two-human), NOT a successfully read zero-stat racer.
    std::optional<QualificationRanks> first,second;
};
using HallRowTable=LapRecordTableView50ID<OriginalUiHallCourseRecords50KG>;
inline bool qualificationFail(QualificationDiagnostic& d,QualificationStatus status,
    std::uint32_t pc,std::uint32_t callPC=0,std::uint32_t index=0) noexcept {
    d={status,pc,callPC,index};return false;
}
inline bool retainQuery(LapRecordQueryResult50ID query,std::uint32_t callPC,
    std::uint32_t index,std::uint32_t& out,QualificationDiagnostic& d) noexcept {
    if(!query.completed())return qualificationFail(d,
        query.status==LapRecordQueryStatus50ID::Storage?QualificationStatus::storage:
            QualificationStatus::unsupportedNumeric,query.sourcePC,callPC,index);
    out=query.originalV0;return true;
}
inline bool queryRacer(const LapRecordConfig50ID& config,const HallRowTable& table,
    std::uint32_t course,const OriginalRaceRecordQualificationRacerObservation50KG& racer,
    std::uint32_t index,QualificationRanks& ranks,QualificationDiagnostic& d) noexcept {
    //1AC0A4/1AC128: retirement skips all three queries and their operands.
    if(racer.retirementCD4!=0u)return true;
    if(!retainQuery(queryLapRecord50ID(config,table,course,racer.records.bestLapCB4),
        index==0u?0x1ac0bcu:0x1ac140u,index,ranks.lap,d))return false;
    if(!retainQuery(queryRaceRecord50ID(config,table,course,racer.records.timeCB8),
        index==0u?0x1ac0d8u:0x1ac15cu,index,ranks.race,d))return false;
    return retainQuery(queryOriginalHallKnockoutRow50KG(config,table,course,
        racer.records.countCD0),index==0u?0x1ac0f4u:0x1ac178u,index,ranks.knockouts,d);
}
inline bool competeTimes(float first,float second,float threshold,bool race,
    std::uint32_t& firstRank,std::uint32_t& secondRank,QualificationDiagnostic& d) noexcept {
    //Reached only when BOTH original queries returned nonzero. Native
    //ordinary-float admission is distinct from source unordered behavior.
    if(!ordinary(first)||!ordinary(second))return qualificationFail(d,
        QualificationStatus::unsupportedNumeric,race?0x1ac254u:0x1ac1a0u);
    const bool firstBeforeSecond=first<=second;
    if(!ordinary(threshold))return qualificationFail(d,QualificationStatus::unsupportedNumeric,
        race?(firstBeforeSecond?0x1ac294u:0x1ac2dcu):
             (firstBeforeSecond?0x1ac1e0u:0x1ac228u));
    //Original compares the losing candidate with the STORED SECOND slot,
    //not a hypothetical inserted table, the third slot, or query-rank math.
    if(firstBeforeSecond){if(!(second<threshold))secondRank=0;}
    else if(!(first<threshold))firstRank=0;
    return true;
}
inline bool anyRank(const QualificationRanks& ranks) noexcept {
    return ranks.lap>0u||ranks.race>0u||ranks.knockouts>0u;
}
// 1AC034,1AC03C/44,1AC058/60,1AC074/7C early exclusion: Tournament subtype 2
// or any of the cheat words 10FAC[0], [5], [13] returns 0 before any query.
inline bool qualificationExcluded1ABFC0(std::uint32_t currentMode11258,
    const std::array<std::uint32_t,27>& unlock10FAC) noexcept {
    return currentMode11258==2u||unlock10FAC[0]!=0u||unlock10FAC[5]!=0u||unlock10FAC[13]!=0u;
}
//Source a1:0 either player;1 first only;2 second only. All other uint32
//selectors return0, but ONLY AFTER the same queries/competition are reached.
//race1B0 low2==2 selects the second getter. Other values still read row0;
//do not copy insertion's stricter1-or2 branch behavior into this query.
inline bool qualify1ABFC0(const LapRecordConfig50ID& config,const HallRowTable& table,
    const std::array<std::uint32_t,27>& unlock10FAC,std::uint32_t course,
    std::optional<std::uint8_t> race1B0Low2,std::uint32_t selector,
    const OriginalRaceRecordQualificationRacerObservation50KG* rows,std::size_t rowCount,
    QualificationResult& out,QualificationDiagnostic& d) noexcept {
    d={};QualificationResult next;
    //a1=0 in EACH profile helper: 188EC0/188F10/188FB0 therefore return raw
    //10FAC/10FC0/10FE0 even when profile.mode34==2. The a1=1 Tournament
    //bypass does NOT apply.
    if(qualificationExcluded1ABFC0(config.currentMode11258,unlock10FAC)){
        out=next;d={QualificationStatus::ok,0x1ac40cu,0,0};return true;
    }
    if(!rows||rowCount<1u)return qualificationFail(d,QualificationStatus::missingRacer,0x1ac094u,0,0);
    next.first=QualificationRanks{};
    if(!queryRacer(config,table,course,rows[0],0,*next.first,d))return false;
    //Original reaches LBU1B0 only here, AFTER row0 queries. Missing
    //canonical low2 is not numeric1 from Camera or a source false result;
    //early exclusions above legitimately skip this dependency entirely.
    if(!race1B0Low2)return qualificationFail(d,QualificationStatus::missingRaceFlags,0x1ac100u);
    if((*race1B0Low2&3u)==2u){
        if(rowCount<2u)return qualificationFail(d,QualificationStatus::missingRacer,0x1ac118u,0,1);
        next.second=QualificationRanks{};
        if(!queryRacer(config,table,course,rows[1],1,*next.second,d))return false;
        auto& first=*next.first;auto& second=*next.second;
        //Nonzero queries have already bounded course against THIS table.
        //Retired/gated-out branches never read a competition operand.
        if(first.lap&&second.lap&&
           !competeTimes(rows[0].records.bestLapCB4,rows[1].records.bestLapCB4,
               table.rows[course].lap102F0[1],false,first.lap,second.lap,d))return false;
        if(first.race&&second.race&&
           !competeTimes(rows[0].records.timeCB8,rows[1].records.timeCB8,
               table.rows[course].race1033C[1],true,first.race,second.race,d))return false;
        if(first.knockouts&&second.knockouts){
            const auto a=rows[0].records.countCD0,b=rows[1].records.countCD0;
            const auto threshold=table.rows[course].knockouts10388[1];
            //1AC308 signed SLT chooses losing player;1AC344/388 SLTU
            //then compares its RAW word with stored rank2. Ties favor0.
            if((a^0x80000000u)<(b^0x80000000u)){
                if(!(threshold<a))first.knockouts=0;
            }else if(!(threshold<b))second.knockouts=0;
        }
    }
    if(selector<2u&&anyRank(*next.first))next.originalV0=1u;
    else if((selector==0u||selector==2u)&&next.second&&anyRank(*next.second))
        next.originalV0=1u;
    out=next;d={QualificationStatus::ok,0x1ac408u,0,0};return true;
}
} // namespace race_record_rule50kg
}
