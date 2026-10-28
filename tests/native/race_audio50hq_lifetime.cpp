// Lifetime emission only: preserves the original native owner and cleanup order.
#include "race_audio50hq_fixture.h"

namespace race_audio50HQ {
Owner::~Owner()noexcept {reset();}
} // namespace race_audio50HQ
