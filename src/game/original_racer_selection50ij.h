#pragma once
// HOST menu data for the eight retained original racers. Source constructor
// 21079C/21081C/210820 stores racer60 and formats that SAME value as Pod%02d.
// These IDs never denote the human/bot bank slot. Choosing a racer must move
// its whole immutable rig/collision/audio identity, never just its model/name.
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rr::game::original_racer_selection50IJ {
struct Entry {
    std::uint32_t racer60;
    std::string_view podId,name,retainedPodJson;
    bool humanJoinImplemented;
};
inline constexpr std::array<Entry,8> entries{{
    {1,"POD01","Anakin Skywalker","pod01/pod.json",true},
    {3,"POD03","Clegg","pod03/pod.json",true},
    {4,"POD04","Dudbolt","pod04/pod.json",false},
    {7,"POD07","Gasgano","pod07/pod.json",false},
    {8,"POD08","Mars Guo","pod08/pod.json",false},
    {9,"POD09","Mawhonic","pod09/pod.json",false},
    {10,"POD10","Alder Bedo","pod10/pod.json",false},
    {11,"POD11","Ody Mandell","pod11/pod.json",false}
}};
inline constexpr const Entry* findByRacer60(std::uint32_t value) noexcept {
    for(const auto& entry:entries)if(entry.racer60==value)return &entry;
    return nullptr;
}
inline constexpr const Entry* findByPodId(std::string_view value) noexcept {
    for(const auto& entry:entries)if(entry.podId==value)return &entry;
    return nullptr;
}
inline constexpr bool canSelectHuman(std::uint32_t value) noexcept {
    const auto* entry=findByRacer60(value);return entry&&entry->humanJoinImplemented;
}
struct Selection {
    std::size_t browsedIndex=0;
    std::uint32_t humanRacer60=1;
};
enum class ConfirmResult:std::uint8_t {Selected,HumanJoinPending,InvalidSelection};
inline constexpr bool browse(Selection& state,int direction) noexcept {
    if(state.browsedIndex>=entries.size()||(direction!=-1&&direction!=1))return false;
    state.browsedIndex=direction<0?(state.browsedIndex+entries.size()-1)%entries.size():
        (state.browsedIndex+1)%entries.size();return true;
}
inline constexpr ConfirmResult confirm(Selection& state) noexcept {
    if(state.browsedIndex>=entries.size()||!canSelectHuman(state.humanRacer60))
        return ConfirmResult::InvalidSelection;
    const auto& entry=entries[state.browsedIndex];
    if(!entry.humanJoinImplemented)return ConfirmResult::HumanJoinPending;
    state.humanRacer60=entry.racer60;return ConfirmResult::Selected;
}
inline constexpr bool cancel(Selection& state) noexcept {
    if(!canSelectHuman(state.humanRacer60))return false;
    for(std::size_t i=0;i<entries.size();++i)if(entries[i].racer60==state.humanRacer60){
        state.browsedIndex=i;return true;
    }
    return false;
}
// Build the slot -> original roster row permutation once at cold start. All
// eight originals occur exactly once. Runtime slot0 is human; racer60 remains
// the source ID, including POD01 when it moves into Clegg's former bot slot.
inline constexpr bool buildRosterOrder(std::uint32_t humanRacer60,
    std::array<std::size_t,entries.size()>& out) noexcept {
    if(!canSelectHuman(humanRacer60))return false;
    std::array<std::size_t,entries.size()> order{};std::size_t selected=0;
    for(std::size_t i=0;i<entries.size();++i){order[i]=i;if(entries[i].racer60==humanRacer60)selected=i;}
    order[0]=selected;order[selected]=0;out=order;return true;
}
} // namespace rr::game::original_racer_selection50IJ
