// Lifetime emission only: preserves the original native owner and cleanup order.
#include "original_ai_audio_owner.h"

namespace native_original_ai_audio {
Owner::~Owner() {close();}
} // namespace native_original_ai_audio
