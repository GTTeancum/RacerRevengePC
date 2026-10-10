#pragma once
#include "body_scene48g.h"
#include <cstdint>
namespace rr::game {
struct BasePoseHistoryState50Y {
 std::uint32_t timerD2C,markerD30,positionC50[4],quaternionC60[4],velocityC70[4];
 std::uint32_t historyD50[20][8],currentFD0,nextFD4;
};
struct BasePoseHistoryInput50Y {
 BodyMatrix48G readyWorld100;
 std::uint32_t velocity70[4],positionAt228B98[4],outerDeltaBits;
 bool diagnosticsAbsent,worldReady;
};
enum class BasePoseHistoryStatus50Y {Complete,NeedsDiagnostics,Admission,Arithmetic};
struct BasePoseHistoryResult50Y {BasePoseHistoryState50Y state;BasePoseHistoryStatus50Y status;};
// Ordinary state3C=0 continuation228C14..228F00, before child dispatch.
// Prepared ready world matrix; caller preserves source cached-world semantics.
// Consumed arithmetic must be finite normal/zero. Copy-only words are opaque.
BasePoseHistoryResult50Y advanceBasePoseHistory50Y(const BasePoseHistoryState50Y&,const BasePoseHistoryInput50Y&) noexcept;
}
