#pragma once
#include <cstdint>
namespace rr::game {
// Prepared QuadTree fields. This leaf does not create/register a tree or object.
struct SpatialPointKeyInput50BN {
 float originX30=0,originZ38=0,extent48=0,scale5C=1;
 float pointX=0,pointZ=0;
 std::uint32_t quantum58=1,level54=0;
};
enum class SpatialPointKeyStatus50BN { Complete,InvalidInput,UnsupportedArithmetic };
// Original263D10..263E14, finite ordinary arithmetic and positive power-of-two
// quantum <=32768, level <=15. Original uses extent48 for BOTH axes here.
// Outside points naturally produce zero. Y is not consumed. Output is untouched
// on rejection. This is a packed source handle, not a native node-table index.
SpatialPointKeyStatus50BN spatialPointKey50BN(
 const SpatialPointKeyInput50BN&,std::uint32_t& output) noexcept;
}
