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
    const char* menuLabel;
};
inline constexpr std::array<Entry,8> entries{{
    {1,"POD01","Anakin Skywalker","pod01/pod.json",true,"RACER: ANAKIN SKYWALKER"},
    {3,"POD03","Clegg","pod03/pod.json",true,"RACER: CLEGG"},
    {4,"POD04","Dudbolt","pod04/pod.json",true,"RACER: DUDBOLT"},
    {7,"POD07","Gasgano","pod07/pod.json",true,"RACER: GASGANO"},
    {8,"POD08","Mars Guo","pod08/pod.json",true,"RACER: MARS GUO"},
    {9,"POD09","Mawhonic","pod09/pod.json",true,"RACER: MAWHONIC"},
    {10,"POD10","Alder Bedo","pod10/pod.json",true,"RACER: ALDER BEDO"},
    {11,"POD11","Ody Mandell","pod11/pod.json",true,"RACER: ODY MANDELL"}
}};
// The Course handoff can select this ninth resource identity while retaining
// an eight-racer field. It is deliberately outside the legacy host-menu cycle
// and permutation; the cold Course owner replaces only camera slot0.
inline constexpr Entry coursePod19{19,"POD19","Anakin Skywalker","pod19/pod.json",
    true,"RACER: ANAKIN SKYWALKER"};
// Genuine supplemental Course identities, outside the retained eight-entry
// host-menu cycle. Names are the originals' own PHY Name fields. Every cold
// selection still requires that pod's authenticated RES/import/PHY owners.
inline constexpr std::array<Entry,9> additionalCoursePods{{
    {2,"POD02","Ben Quad","pod02/pod.json",true,"RACER: BEN QUAD"},
    {12,"POD12","Shrivel Braittrand","pod12/pod.json",true,"RACER: SHRIVEL BRAITTRAND"},
    {15,"POD15","Teemto","pod15/pod.json",true,"RACER: TEEMTO"},
    {20,"POD20","Occo Ninebar","pod20/pod.json",true,"RACER: OCCO NINEBAR"},
    {16,"POD16","Was Sandage","pod16/pod.json",true,"RACER: WAS SANDAGE"},
    {17,"POD17","Scorch Zanales","pod17/pod.json",true,"RACER: SCORCH ZANALES"},
    {18,"POD18","Sebulba 2","pod18/pod.json",true,"RACER: SEBULBA 2"},
    {24,"POD24","Tzidik Wrantojo","pod24/pod.json",true,"RACER: TZIDIK WRANTOJO"},
    {22,"POD22","Knire Dark","pod22/pod.json",true,"RACER: KNIRE DARK"}
}};
inline constexpr const Entry* findByRacer60(std::uint32_t value) noexcept {
    for(const auto& entry:entries)if(entry.racer60==value)return &entry;
    if(value==coursePod19.racer60)return &coursePod19;
    for(const auto& entry:additionalCoursePods)if(entry.racer60==value)return &entry;
    return nullptr;
}
inline constexpr const Entry* findByPodId(std::string_view value) noexcept {
    for(const auto& entry:entries)if(entry.podId==value)return &entry;
    if(value==coursePod19.podId)return &coursePod19;
    for(const auto& entry:additionalCoursePods)if(entry.podId==value)return &entry;
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
// the source ID, including POD01 when it moves into the selected pod's bot slot.
inline constexpr bool buildRosterOrder(std::uint32_t humanRacer60,
    std::array<std::size_t,entries.size()>& out) noexcept {
    if(!canSelectHuman(humanRacer60))return false;
    std::array<std::size_t,entries.size()> order{};std::size_t selected=entries.size();
    for(std::size_t i=0;i<entries.size();++i){order[i]=i;if(entries[i].racer60==humanRacer60)selected=i;}
    // Supplemental Course pods require their real replacement row, not a
    // fabricated permutation of the eight default originals.
    if(selected==entries.size())return false;
    order[0]=selected;order[selected]=0;out=order;return true;
}
} // namespace rr::game::original_racer_selection50IJ
