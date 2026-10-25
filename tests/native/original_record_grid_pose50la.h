#pragma once
#include "selected_grid_navigation50ig.h"
#include "../../src/game/undertrack_reposition50et.h"
#include <bit>
#include <cfenv>
#include <cmath>

namespace original_record_grid50LA {
// Cold geometry for1AE9EC's actual2A2880(rank0), followed by the existing
//2169C0 lift and216AF0/28A090 basis projection. This never copies a player
//matrix, chooses an axis fallback, changes a Scene, or runs a reset/ground
//query. The caller supplies the actual selected rig lift and shared434650 up.
//SelectedMapGrid retains the original typed MB/TC/TB/GB/MC view or legacy map
//capability (including RA and TA). Those source owners must remain alive.
inline bool prepareRankZeroPose(const source_grid50IG::SelectedMapGrid& selected,
    float actualSelectedLift,const float (&sharedUp434650)[4],
    rr::game::BodyMatrix48G& output,rr::game::UndertrackDiagnostic50ET& diagnostic)noexcept {
    namespace g=rr::game;
    diagnostic={};
    if(std::fegetround()!=FE_TONEAREST){
        diagnostic={g::UndertrackError50ET::RoundingMode,0x2a2880u,0};return false;
    }
    if(!std::isfinite(actualSelectedLift)||actualSelectedLift<0.f||
       std::fpclassify(actualSelectedLift)==FP_SUBNORMAL){
        diagnostic={g::UndertrackError50ET::Input,0x2169c0u,0};return false;
    }
    g::GridCenterResult50AC grid{};
    if(!selected.prepare(0u,grid)){
        diagnostic={g::UndertrackError50ET::Track,0x2a2880u,0};return false;
    }
    float position[4]{},heading[4]{},up[4]{};
    for(unsigned i=0;i<4;++i){
        position[i]=std::bit_cast<float>(grid.position[i]);
        heading[i]=std::bit_cast<float>(grid.heading[i]);up[i]=sharedUp434650[i];
    }
    // Source scalar single-precision add, once only. Ground adjustment, when
    //required by the actual native placement owner, occurs after this result.
    volatile float raised=position[1]+actualSelectedLift;
    position[1]=raised;
    g::BodyMatrix48G staged{};
    if(!g::prepareUndertrackResetPose50ET(position,heading,up,staged,diagnostic))return false;
    output=staged;return true;
}
} // namespace original_record_grid50LA
