#pragma once
#include "../../src/assets/audio_assets50hc.h"
#include <array>
#include <string>
namespace mc_audio_originals50MC {
struct Pin {const char* name;std::size_t bytes;const char* sha;std::uint32_t rate;};
inline constexpr std::array<Pin,10> pins{{
 {"sfx/race/calcrowd.vag",43280,"b86fc6e1dd03e6e7d6b3352264f209bc83cd7a0223bb31065a71a3a910b2526f",22050},
 {"sfx/race/camera.vag",25120,"8bc151e8e662b7904f93d4152ef4d4469ffd6988ea9723b3423da805a6e174a9",22050},
 {"sfx/race/cnyncry9.vag",20512,"5b1dc35c8debfd0dc4356987ec9392c3f1599f835f46fa56ee49a708be871d63",22050},
 {"sfx/race/electric.vag",28512,"2d669051f4c22400dbaa85258bede99c2ab1208bddf329aa5c29cde79b6b1165",22050},
 {"sfx/race/metlwsh1.vag",6016,"8b146edbee3426e83f07c1c663bcea26690e6b30006e0494f123c2c8e70082f0",22050},
 {"sfx/race/metlwsh2.vag",5088,"196db3d760409f0669d4d8c9bbf296e5266eb461e3b26b505f38ef4da5187445",22050},
 {"sfx/race/rockhum.vag",13856,"e0455ae2be460e59bdb5372a9e6d0043d9891d6639f007e4a9f03fb9d140649b",8611},
 {"sfx/race/rockwsh1.vag",4544,"af59577c39beed190e49493c24063a9f2a9fc3f31236e7bc3e8f985c180a3ccd",22050},
 {"sfx/race/rockwsh2.vag",7536,"4c8c50b7e835c9bf49223d37a916cf2963f159901c3ba59a6a818a540140d43d",22050},
 {"sfx/race/torch.vag",32320,"2be8564dd9b2e14a6bc364e3db805ce53b1a24ae402fbee864cee9123ea42381",22050},
}};
inline bool authentic(const rr::assets::AudioClip50HC& clip)noexcept {
 for(const auto& pin:pins)if(clip.request.name==pin.name)return
  clip.state==rr::assets::AudioAssetState50HC::ready&&clip.sourceBytes==pin.bytes&&
  clip.sourceSha256==pin.sha&&clip.pcm.sourceSha256==pin.sha&&
  clip.pcm.sampleRate==pin.rate&&clip.pcm.channels==1&&
  clip.pcm.frames>0&&clip.pcm.frames==clip.pcm.samples.size();
 return false;
}
}
