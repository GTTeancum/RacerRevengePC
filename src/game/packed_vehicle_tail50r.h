#pragma once
#include <cstdint>
namespace rr::game {
struct PackedVehicleInput50R {
 float drive[4]; // engine70/74, aeroE4/E8
 float impactBA0[3],damage7D0[4],filteredB70,filteredB78,actionB80;
 std::uint32_t vehicle58;
 bool configurationEntryNonzero;
 std::uint8_t flagsB9C,state40;
};
struct PackedVehicleResult50R { bool complete=false;std::uint8_t bytesC00[7]{}; };
// Original2291D0 numeric packet, finite normal/zero consumed arithmetic domain.
// Nonrepresented global configuration resolution is explicit caller input.
// Returns privately staged bytes; no writes, allocation or fake geometry.
PackedVehicleResult50R packVehicleTail50R(const PackedVehicleInput50R&) noexcept;
}
