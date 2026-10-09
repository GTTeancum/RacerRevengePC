#pragma once
#include <cstdint>
namespace rr::game {
// Player subtype only: original217940 with vtable3CED50 slotA8=1A2B70.
// Present D40 must be the concrete19EBA0-created owner (table3CECA0).
// Pointer presence is supplied by the caller; no live owner discovery occurs here.
struct PlayerResetState50AX {std::uint32_t vehicle64,vehicle68,vehicle78;};
inline void resetPlayerEnd50AX(PlayerResetState50AX& s,std::uint32_t authoredCF8,
 std::uint8_t* concreteD40Byte22C) noexcept {
 if(concreteD40Byte22C)*concreteD40Byte22C=static_cast<std::uint8_t>(*concreteD40Byte22C&0xfdu);
 // Source player virtual A8 is a return+nop. Preserve the literal CF8 bits.
 s.vehicle68=0;s.vehicle64=0;s.vehicle78=authoredCF8;
}
}
