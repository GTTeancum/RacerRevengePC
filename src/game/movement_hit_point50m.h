#pragma once
#include "spring_records48n.h"
namespace rr::game {
// Original219230..219338, stopping before the scene setter. Consumes XYZ only;
// fourth input words may be opaque. Output W is the source-created1. Output may
// alias an input. Failure leaves output unchanged. Nearest/even, finite ordinary
// binary32 intermediates; nonzero subnormal norm-squared is outside the scope.
// The source initialized push distance is10; a supplied live value is explicit.
bool computeMovementHitPoint50M(const SpringVector4_48N& current,
 const SpringVector4_48N& selectedPoint,float pushDistance,
 SpringVector4_48N& correctedPrior) noexcept;
}
