#pragma once
#include <cstdint>
namespace race_music50IE { class Owner; }
namespace live_audio50HF { class Owner; }
namespace interactive_publication50DU {
// Same original callback payload/layout; the instance remains in runSession.
struct StartMusicContext {
  race_music50IE::Owner* music;
  live_audio50HF::Owner* audio;
  const std::uint32_t* sourceEventType;
 };
} // namespace interactive_publication50DU
