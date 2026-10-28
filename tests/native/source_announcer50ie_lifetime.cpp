// Lifetime emission only: preserves the original native owner and cleanup order.
#include "source_announcer50ie_fixture.h"

namespace source_announcer50IE {
Owner::~Owner()noexcept {reset();}
} // namespace source_announcer50IE
