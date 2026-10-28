#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>

namespace rr::platform { class OriginalUiRiderProfileOwner50KG; }
namespace rr::game {
// Synchronous facts from actual1AF330 manager order. No sorted display row,
// selected-player proxy, finish flag or earned-credit authority is implied.
struct OriginalRaceRivalRacerObservation50KG {
    std::uint32_t racer60,retirementCD4;
    std::int32_t placeCCC;
};

// Exact readonly1ABE70 predicate. It neither rewards/unlocks a rival nor writes
// profile7E8. Only the same concrete Profile owner can supply its owned course
// descriptor11280 and unlock10FAC. Ordinary rows/output are never admission to
// the private1AB520 continuation or its ordered7E4/7E8 stores.
class OriginalRaceRivalQualification50KG final {
public:
    enum class Status:std::uint8_t {ok,profile,course,missingRacer,unsupportedRival};
    struct Diagnostic {
        Status status=Status::ok;
        std::uint32_t sourcePC=0,managerIndex=0;
        //Present only for a missing scanned row. The actual caller may read
        //that row's racer60 first and skip CCC unless it matches this source
        //descriptor. Metadata is not an end/result authority or a rank value.
        std::optional<std::int32_t> scanRival11280{};
    };
    OriginalRaceRivalQualification50KG(const OriginalRaceRivalQualification50KG&)=default;
    OriginalRaceRivalQualification50KG& operator=(const OriginalRaceRivalQualification50KG&)=default;
    std::uint32_t originalV0()const noexcept{return originalV0_;}
    std::uint32_t course()const noexcept{return course_;}
    std::int32_t rival11280()const noexcept{return rival_;}
    std::uint64_t profileGeneration()const noexcept{return generation_;}
    // Absent means no matching row was reached, not a fabricated rival rank.
    const std::optional<std::uint32_t>& rivalManagerIndex()const noexcept{return matched_;}
private:
    friend class rr::platform::OriginalUiRiderProfileOwner50KG;
    OriginalRaceRivalQualification50KG() noexcept=default;
    static bool fail(Diagnostic& d,Status status,std::uint32_t pc,std::uint32_t index=0) noexcept {
        d={status,pc,index};return false;
    }
    static bool prepare(std::uint32_t course,std::int8_t rival,
        const std::array<std::uint32_t,27>& unlock10FAC,
        const OriginalRaceRivalRacerObservation50KG* rows,std::size_t availableRows,
        std::int32_t managerCount30,std::uint64_t generation,
        std::optional<OriginalRaceRivalQualification50KG>& out,Diagnostic& d) noexcept {
        d={};OriginalRaceRivalQualification50KG next;
        next.course_=course;next.rival_=rival;next.generation_=generation;
        //1ABED8/1ABEE8 obtain row0 separately before any manager-count scan.
        //One synchronous same-owner publication makes those two observations
        //the same actual row. Even count<=0 does not suppress either getter.
        if(!rows||availableRows==0)return fail(d,Status::missingRacer,0x1abed8u);
        const auto firstPlace=rows[0].placeCCC; //1ABEE4, signed LW/SLT.
        if(rows[0].retirementCD4!=0u){
            out=next;d={Status::ok,0x1abf9cu,0};return true;
        }
        //Source LB sign-extends11280 before indexing10FAC. Out-of-array
        //signed values are native unsupported storage, NOT source false.
        //Retirement above must skip this read and any unsupported-index gate.
        if(rival<0||static_cast<std::size_t>(rival)>=unlock10FAC.size())
            return fail(d,Status::unsupportedRival,0x1abf18u);
        if(unlock10FAC[static_cast<std::size_t>(rival)]!=0u){
            out=next;d={Status::ok,0x1abf98u,0};return true;
        }
        std::int32_t rivalPlace=std::numeric_limits<std::int32_t>::max(); //1ABE94..9C
        //1ABF7C is SIGNED SLT. No invented humans/1..8/count-positive gate.
        //Stop at the FIRST matching racer60; later duplicates are not read.
        for(std::int32_t index=1;index<managerCount30;++index){
            if(static_cast<std::size_t>(index)>=availableRows){
                fail(d,Status::missingRacer,0x1abf2cu,static_cast<std::uint32_t>(index));
                d.scanRival11280=next.rival_;return false;
            }
            if(rows[index].racer60!=static_cast<std::uint32_t>(next.rival_))continue;
            rivalPlace=rows[index].placeCCC; //1ABF50/5C; no rival CD4 gate.
            next.matched_=static_cast<std::uint32_t>(index);break;
        }
        //Absent rival intentionally keeps INT_MAX. No mode2 bypass, positive
        //rank restriction, tie win, or requirement that a rival was found.
        next.originalV0_=firstPlace<rivalPlace?1u:0u; //1ABF88..94
        out=next;d={Status::ok,0x1abf98u,0};return true;
    }
    std::uint32_t originalV0_=0,course_=0;
    std::int32_t rival_=0;
    std::uint64_t generation_=0;
    std::optional<std::uint32_t> matched_;
};
}
