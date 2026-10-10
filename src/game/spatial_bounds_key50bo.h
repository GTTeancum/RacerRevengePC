#pragma once
#include <cstdint>
namespace rr::game {
struct SpatialBoundsKeyInput50BO {
 float originX30=0,maximumX34=0,originZ38=0,maximumZ3C=0;
 float extentX48=0,extentZ4C=0,scale5C=1;
 float centerX=0,centerZ=0,halfX=0,halfZ=0;
 std::uint32_t quanta60[16]{},maximumLevel54=0;
};
enum class SpatialBoundsKeyStatus50BO{Complete,InvalidInput,UnsupportedArithmetic};
// Original263E20..264084. Prepared tree, nearest ordinary finite binary32,
// nonnegative extents/halves, <=16 descending power-of-two quanta<=32768.
// No tree mutation or live registration. Returns source packed handle; zero is
// the original outside result. Y/W and the source's previous handle are unread.
// Unsupported/invalid inputs preserve output. Unused quanta are not consumed.
SpatialBoundsKeyStatus50BO spatialBoundsKey50BO(
 const SpatialBoundsKeyInput50BO&,std::uint32_t&) noexcept;
}
