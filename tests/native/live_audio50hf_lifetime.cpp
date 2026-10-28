// Lifetime emission only: preserves the original native owner and cleanup order.
#include "live_audio50hf_fixture.h"

namespace live_audio50HF {
Owner::~Owner()noexcept {rr::platform::AudioOutputDiagnostic50HD diagnostic;close(diagnostic);}
} // namespace live_audio50HF
