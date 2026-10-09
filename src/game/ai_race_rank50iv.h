#pragma once
#include "ai_state_prefix50iv.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
// The source manager3C array contains vehicle+C90 records. Thus record34 is
// canonical vehicleCC4, record38 is CC8, record3C is CCC, record44 is CD4, and
// record48 is CD8. This is not HOST lap distance or HOST standings.
// Native admission capacity only, not a recovered original manager-size check.
inline constexpr std::size_t aiRaceRankCapacity50IV=8;
struct AiRaceRankInput50IV {
    float mapNode60; //2A2540(manager48->B8,vehicleC88), same actual map track.
    float mapNode10[3],mapNode20[3]; //2A2490 first and second output at that C88.
    float bodyScene288Position[3]; //Actual28B180(vehicle340->288) result.
    std::int32_t lapCC8;
    std::uint32_t vehicleState40,retirementCD4;
    std::uint8_t flagsCD8;
};
struct AiRaceRankState50IV {float remainingCC4;std::int32_t placeCCC;};
struct AiRaceRankWorkspace50IV {AiRaceRankState50IV next[aiRaceRankCapacity50IV];};
enum class AiRaceRankError50IV : std::uint8_t {None,Storage,RoundingMode,Arithmetic,RetirementBoundary,FinishBoundary};
struct AiRaceRankDiagnostic50IV {
    AiRaceRankError50IV error=AiRaceRankError50IV::None;
    std::uint32_t sourcePC=0;
    std::size_t racerIndex=0;
};
//1B0400 bounded all-active/not-finished manager pass. Supply EVERY actual racer
//in stable manager3C order, including human/HOST-controlled racers. Their genuine
//mapC88, source lapCC8, current physical scene and flag owners are mandatory.
//Call after ALL racer1AFF50 timing/event cuts and before1B02F0 finish processing.
//The owner of the original root scheduler remains outside this numeric leaf.
//
//Does not alter CC8, CD8, pose or navigation; publishes ONLY CC4 and CCC. It does
//not invent retirement lists40, finish lists44, or their counts50/54. Stops when
//such a branch is present, rather than retaining HOST ranks or forcing alive.
//All records/workspace/diagnostic disjoint. Failure preserves the output array;
//workspace may change. No allocation, external effects or original execution.
inline bool updateAiRaceRank50IV(const AiRaceRankInput50IV* in,std::size_t count,
    std::uint32_t actualFinishedCount54,AiRaceRankState50IV* out,AiRaceRankWorkspace50IV& workspace,
    AiRaceRankDiagnostic50IV& d) noexcept {
    d={};
    auto fail=[&](AiRaceRankError50IV e,std::uint32_t pc,std::size_t racer) noexcept {
        d={e,pc,racer};return false;};
    if(count>aiRaceRankCapacity50IV||(count&&(!in||!out)))return fail(AiRaceRankError50IV::Storage,0x1b0400,0);
    if(std::fegetround()!=FE_TONEAREST)return fail(AiRaceRankError50IV::RoundingMode,0x1b0400,0);
    if(actualFinishedCount54)return fail(AiRaceRankError50IV::FinishBoundary,0x1b07d0,0);
    // Ordinary route admission is explicit. No excluded actor is dropped from
    //the ranking population. Full1B0400 would also process its real list state.
    for(std::size_t i=0;i<count;++i){
        if(in[i].retirementCD4||in[i].vehicleState40)return fail(AiRaceRankError50IV::RetirementBoundary,0x1b06bc,i);
        if(in[i].flagsCD8&1u)return fail(AiRaceRankError50IV::FinishBoundary,0x1b07b0,i);
    }
    using namespace ai_prefix_detail50IV;
    for(std::size_t i=0;i<count;++i){
        const auto& a=in[i];Math f;
        //1B04A4/1B04A8/1B04D0 zero all three input Y lanes before subtraction.
        float segment[3]={f.sub(a.mapNode20[0],a.mapNode10[0]),f.sub(0.f,0.f),f.sub(a.mapNode20[2],a.mapNode10[2])};
        float relative[3]={f.sub(a.bodyScene288Position[0],a.mapNode10[0]),f.sub(0.f,0.f),f.sub(a.bodyScene288Position[2],a.mapNode10[2])};
        const float numerator=f.dot(segment,relative),denominator=f.dot(segment,segment);
        const float ratio=f.div(numerator,denominator);
        const float upper=detail::scalarMin48O(ratio,1.f);
        const float t=detail::scalarMax48O(upper,0.f);
        //Source computes XYZ products, then XYZ sums, then XYZ subtractions.
        const float px=f.mul(t,segment[0]),pz=f.mul(t,segment[2]),py=f.mul(t,segment[1]);
        const float qx=f.add(px,a.mapNode10[0]),qy=f.add(py,0.f),qz=f.add(pz,a.mapNode10[2]);
        const float dx=f.sub(a.bodyScene288Position[0],qx),dy=f.sub(0.f,qy),dz=f.sub(a.bodyScene288Position[2],qz);
        const float xx=f.mul(dx,dx),yy=f.mul(dy,dy),acc=f.add(xx,yy),zz=f.mul(dz,dz),square=f.add(acc,zz);
        if(!f.valid||square<0.f)return fail(AiRaceRankError50IV::Arithmetic,0x1b0630,i);
        volatile float root=static_cast<float>(std::sqrt(static_cast<double>(square)));
        float remaining=f.add(a.mapNode60,f.read(root));
        //1B065C..1B06A8: one ordered pass over PREVIOUS records, not a sort or
        //epsilon comparison; each encountered exact tie increments by1.0f.
        for(std::size_t earlier=0;earlier<i;++earlier)
            if(remaining==workspace.next[earlier].remainingCC4)remaining=f.add(remaining,1.f);
        if(!f.valid)return fail(AiRaceRankError50IV::Arithmetic,0x1b0690,i);
        workspace.next[i]={remaining,-1};
    }
    //No retired/finished lists are touched in this admitted branch. Source
    //1B0814..1B08D4 counts racers behind, then stores totalCount-behindCount.
    for(std::size_t i=0;i<count;++i){
        std::int32_t behind=0;
        for(std::size_t j=0;j<count;++j){
            if(in[j].lapCC8<in[i].lapCC8){++behind;continue;}
            if(in[j].lapCC8!=in[i].lapCC8)continue;
            if(workspace.next[i].remainingCC4<workspace.next[j].remainingCC4&&!(in[i].flagsCD8&4u))++behind;
        }
        workspace.next[i].placeCCC=static_cast<std::int32_t>(count)-behind;
    }
    for(std::size_t i=0;i<count;++i)out[i]=workspace.next[i];
    return true;
}
} // namespace rr::game
