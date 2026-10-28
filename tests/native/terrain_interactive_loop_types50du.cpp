#include "terrain_interactive_loop_types50du.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <algorithm>
#include <iostream>
#include <limits>

namespace interactive_loop50DU {

void GameplayDisplayWallTotals50DU::Total::add(
    std::uint64_t& value,std::uint64_t amount) noexcept {
    const auto room=(std::numeric_limits<std::uint64_t>::max)()-value;
    if(amount>room){value+=room;overflow=true;}else value+=amount;
}

void GameplayDisplayWallTotals50DU::Total::record(
    const rr::platform::VehicleWindowWallSample50DS& sample) noexcept {
    if(!sample.attempted)return;
    add(attempted,1);
    if(!sample.valid)return;
    add(valid,1);add(wallNs,sample.wallNs);
    maxWallNs=std::max(maxWallNs,sample.wallNs);
}

void GameplayDisplayWallTotals50DU::record(bool isMenu,bool accepted,
    const rr::platform::VehicleWindowDiagnostic50DS& d) noexcept {
    if(!enabled)return;
    auto& group=isMenu?menu:gameplay;
    const auto increment=[&](std::uint64_t& value){
        if(value==(std::numeric_limits<std::uint64_t>::max)())group.overflow=true;
        else ++value;
    };
    increment(group.calls);if(!accepted)increment(group.failures);
    group.phase[0].record(d.gameplayHistoryStageWall);
    group.phase[1].record(d.gameplayFinishWall);
    group.phase[2].record(d.gameplaySwapWall);
}

void GameplayDisplayWallTotals50DU::reportAndReset(const char* boundary) {
    if(!enabled)return;
    const char* names[]{"history_stage","finish_wait","host_swap"};
    for(unsigned scope=0;scope<2;++scope){
        auto& group=scope?menu:gameplay;
        if(!group.calls)continue;
        for(unsigned i=0;i<3;++i){const auto& total=group.phase[i];
            std::cerr<<"display_wall_component "<<names[i]
                <<" parent=completion_wait nested=1 baseline_aligned=0 scope="<<(scope?"menu":"gameplay")
                <<" boundary="<<boundary<<" completion_calls="<<group.calls
                <<" completion_failures="<<group.failures
                <<" attempted="<<total.attempted<<" valid="<<total.valid
                <<" wall_ns="<<total.wallNs<<" max_wall_ns="<<total.maxWallNs
                <<" overflow="<<(group.overflow||total.overflow)<<'\n';
        }
        group={};
    }
}

} // namespace interactive_loop50DU
