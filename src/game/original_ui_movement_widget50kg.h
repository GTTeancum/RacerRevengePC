#pragma once
#include "../assets/original_ui_movement_control50kg.h"
#include "original_ui_navigation50kg.h"
#include <cstdint>

namespace rr::game {
enum class OriginalUiMovementStatus50KG : std::uint8_t {
    ok,noChange,notBound,alreadyBound,invalidDescriptor,unsupportedMapping,
    invalidOperation,invalidController,invalidInput,invalidPage,navigationFailure
};
struct OriginalUiMovementDiagnostic50KG {
    OriginalUiMovementStatus50KG status=OriginalUiMovementStatus50KG::ok;
    OriginalUiNavigationStatus50KG navigation=OriginalUiNavigationStatus50KG::ok;
    std::uint32_t sourcePC=0;
};
struct OriginalUiMovementDelivery50KG {
    // Native container must establish these at the original dispatch point.
    // The selected snapshot is taken AFTER the page callback, before ANY
    // registered widget callback (2A735C..2A7388). It is not a visibility test.
    // Main1E5E00 always returns1 at1E6284, even when its own action is blocked
    // or unhandled. Do not derive pageAccepted from native navigation status.
    bool containerReached=false,pageInputEnabled268=false,pageAccepted=false;
    bool selectedAtSnapshot=false;
};
struct OriginalUiMovementNotification50KG {
    bool issued=false;
    std::int32_t mapping=0;
    // 2AB490 calls manager+84(mapping,1,0,0). Actual PodUIMain vtable 3CFD20
    // selects 2A4330, whose body is JR RA / NOP. This records that proven
    // empty notification; no external callback or UI-manager transition occurs.
    std::uint32_t callPC=0,targetPC=0;
};
class OriginalUiMovementWidget50KG final {
public:
    // Native Main-only admission: authenticated Main widget3, zero GridSize,
    // and mappings in {0,-1,-2}. Other grid/selection operations are preserved
    // by the asset reader but remain unavailable here, never silently ignored.
    // Descriptor and original archive remain immutable/alive while bound.
    OriginalUiMovementStatus50KG bindMain(const assets::OriginalUiMovementControl50KG&) noexcept;
    bool bound() const noexcept {return descriptor_!=nullptr;}
    bool selected() const noexcept {return selected_;} // Source widget+38.
    bool shown() const noexcept {return shown_;}       // Entity flags29 bits0/1.
    const assets::OriginalUiMovementControl50KG* descriptor() const noexcept {return descriptor_;}
    // Constructor +38=0; manager then recursively hides every loaded widget.
    // Source select2AB4B0 always ends shown; deselect2AB530 always ends hidden.
    // clearSelection changes only+38; reset is inherited show-only2A79E0.
    // setEnabled is UIControl's actual2A7A00 call: updates+38 AND visibility.
    OriginalUiMovementStatus50KG apply(OriginalUiControlOperation50KG,bool value=false) noexcept;
    // Uses the controller-attached group from the actual current page, not a
    // hardcoded group1. Mapping0 leaves the group alone. Mapping-2 previous,
    // mapping-1 next, then the exact source-empty Main manager notification.
    // No group attached is source no-change. Rejection leaves nav/commands/
    // notification unchanged. No callbacks execute here; the owning mixed
    // page must preflight and commit the returned ordered widget commands.
    // Supply a private navigation copy if external callback admission can fail.
    OriginalUiMovementStatus50KG dispatchMain(OriginalUiNavigation50KG&,
        std::uint8_t controller,assets::OriginalUiMovementInput50KG,
        const OriginalUiMovementDelivery50KG&,OriginalUiCommands50KG&,
        OriginalUiMovementNotification50KG&,OriginalUiMovementDiagnostic50KG&) const noexcept;
    // Movement vtable+38=141ED0 and +48=141D70 only traverse existing children.
    // This authenticated widget factory creates none. It has no own vertices,
    // texture, transform, per-frame timer or camera dependency. Do not invent
    // a drawable or enable it merely to make the Main left/right mapping live.
private:
    const assets::OriginalUiMovementControl50KG* descriptor_=nullptr;
    bool selected_=false,shown_=false;
};
}
