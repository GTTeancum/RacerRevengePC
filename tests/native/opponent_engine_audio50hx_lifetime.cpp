// Lifetime emission only: preserves the original native owner and cleanup order.
#include "opponent_engine_audio50hx_fixture.h"

namespace opponent_audio50HX {
Owner::~Owner()noexcept {close();}
} // namespace opponent_audio50HX
