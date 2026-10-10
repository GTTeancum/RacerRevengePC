#pragma once
#include "original_ui_pages50kg.h"
#include <array>
#include <cstdint>

namespace rr::assets {
// Source input IDs emitted by 2A7410, in the order read by 2AB010.
// These are not host keyboard codes or raw PS2 bit masks.
enum class OriginalUiMovementInput50KG : std::uint8_t {
    lpadLeft=0,lpadRight=1,lpadUp=2,lpadDown=3,
    rpadDown=4,rpadLeft=5,rpadUp=6,rpadRight=7,
    start=8,select=9,l1=10,r1=11,l2=12,r2=13,l3=14,r3=15
};
struct OriginalUiMovementControl50KG {
    const OriginalUiMemberIdentity50KG* identity=nullptr;
    std::uint16_t widgetId=0;
    std::int32_t gridSize=0;
    std::array<std::int32_t,16> mappings{};
    std::array<bool,2> wantsJoystickInput{{false,false}};
    bool onTop=false;
    // Common UIWidget flags +3C/+40/+44/+48 remain constructor zero on this
    // factory path. Unlike Text/Icon, 2AB010 does not invoke/read UIWidget+74.
};
// Cold atomic descriptor read. Retains all 16 mappings, including mappings
// outside the bounded native Main navigation owner. Zero is the source default
// for GridSize and every mapping. Manager, rather than 2AB010, reads the input
// subscription and OnTop flags. Unknown present keys reject with their offset.
// The authenticated DAT and its owning archive must outlive this descriptor.
bool readOriginalUiMovementControl50KG(const OriginalUiDat50KG& widgets,
    std::uint16_t widgetId,OriginalUiMovementControl50KG& output,
    OriginalUiPageDiagnostic50KG& diagnostic) noexcept;
// Exact catalog identity check, not a basename/path heuristic or a parser run.
bool isOriginalUiMainMovement50KG(const OriginalUiMovementControl50KG&) noexcept;
}
