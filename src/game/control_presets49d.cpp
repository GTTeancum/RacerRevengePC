#include "control_presets49d.h"

namespace rr::game {
namespace {
constexpr ControlActionRow49D rows[8]={
#include "control_preset_rows.inc"
};
}
bool selectControlPreset49D(ControlPresets49D& state,std::uint32_t slot,
                           std::uint32_t preset) noexcept {
    if(slot>=controlPresetSlots49D)return false;
    auto& s=state.slots[slot];
    s.preset11048=preset;
    if(preset>=8)return true;
    s.configuration11040=preset>=4?1u:0u;
    const auto& row=rows[preset];
    // Preserve source write order even though this serial typed API contains
    // no callback, volatile state or concurrent observer between these stores.
    s.actions.action694=row.action694;
    s.actions.action690=row.action690;
    s.actions.action6A8=row.action6A8;
    s.actions.flag698=row.flag698;
    s.actions.flag69C=row.flag69C;
    s.actions.action6AC=row.action6AC;
    s.actions.action6A0=row.action6A0;
    s.actions.action6A4=row.action6A4;
    s.actions.action6B0=row.action6B0;
    s.actions.flag6B4=row.flag6B4;
    return true;
}
void initializeControlPresets49D(ControlPresets49D& state) noexcept {
    for(std::uint32_t i=0;i<controlPresetSlots49D;++i){
        auto& s=state.slots[i];
        s.field10F94=7;
        s.field10F9C=7;
        s.field10FA4=1;
        s.configuration11040=0;
        s.preset11048=0;
        selectControlPreset49D(state,i,0);
        s.field11030=5;
        s.field11038=0;
    }
}
ControlActionRow49D previewControlPreset49D(ControlPresets49D& state,
                                          std::uint32_t preset) noexcept {
    const auto saved=state.slots[0].preset11048;
    selectControlPreset49D(state,0,preset);
    const auto result=state.slots[0].actions;
    selectControlPreset49D(state,0,saved);
    return result;
}
bool resolveControlPreset49D(const ControlPresets49D& state,std::uint32_t actionIndexFFC,
                            std::uint32_t configurationIndex5C,
                            SelectedControlPreset49D& out) noexcept {
    if(actionIndexFFC>=controlPresetSlots49D||configurationIndex5C>=controlPresetSlots49D)
        return false;
    const auto& r=state.slots[actionIndexFFC].actions;
    out={{r.action694,r.action690,r.flag698,r.flag69C,r.flag6B4},
         {r.action6AC},state.slots[configurationIndex5C].configuration11040!=0};
    return true;
}
PlayerEventPresetRows49D playerEventPresetRows49D(const ControlPresets49D& state) noexcept {
    return {{{state.slots[0].actions.action6AC},{state.slots[1].actions.action6AC}}};
}
bool stepReadyPresetPlayerControls49D(const ControlPresets49D& state,
    std::uint32_t actionIndexFFC,std::uint32_t configurationIndex5C,
    const DevicePacket48Y& packet,const DeviceButtonDescriptors48Y& descriptors,
    std::uint32_t activeCount,std::uint32_t deviceId,
    const DeviceButtonBank48Y& previousButtons,DeviceQueryContext48Y queryContext,
    const PlayerReadContext48X& readContext,float previousD28,
    const ControlChannels48B& previousFiltered,ControlRateLimits48B rates,float elapsedFactor,
    const ControlBindingInput48W& bindingContext,const ControlBindingParameters48W& parameters,
    const ControlModelBindings48W& previousBindings,HeldPlayerStep48Y& out) noexcept {
    SelectedControlPreset49D selected{};
    if(!resolveControlPreset49D(state,actionIndexFFC,configurationIndex5C,selected))return false;
    auto context=bindingContext;
    context.configurationEntryNonzero=selected.configurationEntryNonzero;
    return stepReadyHeldPlayerControls48Y(packet,descriptors,activeCount,deviceId,
        previousButtons,queryContext,readContext,selected.actions,previousD28,
        previousFiltered,rates,elapsedFactor,context,parameters,previousBindings,out);
}
}
