#pragma once
#include "../../src/assets/audio_assets50hc.h"
#include <array>
#include <string>
namespace rc_audio_originals50RC {
struct Pin {const char* name;std::size_t bytes;const char* sha;std::uint32_t rate;};
inline constexpr std::array<Pin,26> pins{{
 {"sfx/race/cavecry2.vag",34048,"f7c1a7bda3d872cae84a8211447b3752a448a9e5b7d44427f06647224427cdbb",22050},
 {"sfx/race/cnyncry1.vag",39904,"fd33a11bd1bb48e5e291fcc4afa1860ffcd892a168f265a4e52787736548e661",22050},
 {"sfx/race/cnyncry8.vag",31776,"f304d9161aab1f8c2fad95b1e3eecbcdf5e3239464d2b02f2a5053898a31eb39",22050},
 {"sfx/race/flies.vag",39488,"56f6a5343af96bf3c79abd03c8370b4a6105c7201e6af451468a8980f219ace9",22050},
 {"sfx/race/headston.vag",45312,"0a0578363967428a6c92b3f9983771a665da12b1a8d06dd7d300e0dd778e048e",22050},
 {"sfx/race/lighthum.vag",22208,"13a391aca9334779b12bcb80b6eb89be106702d2aaab422157779a891cc2e633",22050},
 {"sfx/race/pod_coll.vag",14704,"4bcd3f33e1fc5c47b921a565a334566464d649b7d69972bdc8d6d58dfc5669a7",44100},
 {"sfx/race/podscrp1.vag",6544,"546b0afbea0edab78ddafd2d37a987c314bed4ab93545c2162125f0c0d77b05e",18000},
 {"sfx/race/podscrp2.vag",8016,"6bb95c1d1225594b086619a791e9eba3b85674d7ce74c1b1f870b07e4fa897dd",19500},
 {"sfx/race/podscrp3.vag",8624,"adfc070933aa60444d290a62cbdf193408a803fe3b8b66e20c1f02d65f583448",18000},
 {"sfx/race/podscrp4.vag",6048,"7d52c97c392a0a0525ac78dc1096d77641aa0e9a78b79d620ef2bf0943bb5946",18000},
 {"sfx/race/pwrhum1.vag",17504,"0f2539a6da20cd1c26d4266d81dcdffe8bfbd0a054fc1f2d29e7fbc53a6ea055",22050},
 {"sfx/race/pwrhum2.vag",17440,"d65c22b42216ce5b53e92fbcc98c47cd3384151fda7679802e219824de66851a",22050},
 {"sfx/race/rckscrp1.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rckscrp2.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rckscrp3.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rckscrp4.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rockhit1.vag",11664,"8549689b0eb7d1ec415d8ce092a6101927b19a324cf73cc2fe7a4011e4626912",18000},
 {"sfx/race/rockhit2.vag",10400,"17fb766a6f38c969c6726db2092222bc9e4a633810b9f7ba4c6ff17157ab63b0",18000},
 {"sfx/race/rockhit3.vag",12416,"465be3afd0b990d193871d3bd5b0d1cf63d9abb084abf2594daaa1fe04e4ffca",18000},
 {"sfx/race/rockhit4.vag",11664,"162885fe6bef97696a041b72ea7293654b8eac542d24429e7d56991c992f8d0d",18000},
 {"sfx/race/rockhum.vag",13856,"e0455ae2be460e59bdb5372a9e6d0043d9891d6639f007e4a9f03fb9d140649b",8611},
 {"sfx/race/rockwsh1.vag",4544,"af59577c39beed190e49493c24063a9f2a9fc3f31236e7bc3e8f985c180a3ccd",22050},
 {"sfx/race/rockwsh2.vag",7536,"4c8c50b7e835c9bf49223d37a916cf2963f159901c3ba59a6a818a540140d43d",22050},
 {"sfx/race/rylcrowd.vag",41120,"29b870df4cb3fb0a9395b9e49be7359dff23c17531513453d99871305009e57d",16000},
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
