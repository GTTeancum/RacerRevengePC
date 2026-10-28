// Lifetime emission only: preserves the original native owner and cleanup order.
#include "race_music50ie_fixture.h"

namespace race_music50IE {
Owner::~Owner()noexcept {reset();}
} // namespace race_music50IE
