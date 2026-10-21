#pragma once
#include "../../src/assets/audio_assets50hc.h"
#include <array>
#include <string>
namespace mc_material_audio_originals50MC {
struct Pin {const char* name;std::size_t bytes;const char* sha;std::uint32_t rate;};
inline constexpr std::array<Pin,15> pins{{
 {"sfx/race/fldhit1.vag",12336,"1c27d67c3713d551f6400072a122287b607863b18f155aea3d25e9cb4a5d477e",22050},
 {"sfx/race/fldhit2.vag",9632,"0d0671026a864ec56f2ffebfb79149b648299552279c59049bfbbb0216ce59bf",22050},
 {"sfx/race/pod_coll.vag",14704,"4bcd3f33e1fc5c47b921a565a334566464d649b7d69972bdc8d6d58dfc5669a7",44100},
 {"sfx/race/podscrp1.vag",6544,"546b0afbea0edab78ddafd2d37a987c314bed4ab93545c2162125f0c0d77b05e",18000},
 {"sfx/race/podscrp2.vag",8016,"6bb95c1d1225594b086619a791e9eba3b85674d7ce74c1b1f870b07e4fa897dd",19500},
 {"sfx/race/podscrp3.vag",8624,"adfc070933aa60444d290a62cbdf193408a803fe3b8b66e20c1f02d65f583448",18000},
 {"sfx/race/podscrp4.vag",6048,"7d52c97c392a0a0525ac78dc1096d77641aa0e9a78b79d620ef2bf0943bb5946",18000},
 {"sfx/race/rckscrp1.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rckscrp2.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rckscrp3.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rckscrp4.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rockhit1.vag",11664,"8549689b0eb7d1ec415d8ce092a6101927b19a324cf73cc2fe7a4011e4626912",18000},
 {"sfx/race/rockhit2.vag",10400,"17fb766a6f38c969c6726db2092222bc9e4a633810b9f7ba4c6ff17157ab63b0",18000},
 {"sfx/race/rockhit3.vag",12416,"465be3afd0b990d193871d3bd5b0d1cf63d9abb084abf2594daaa1fe04e4ffca",18000},
 {"sfx/race/rockhit4.vag",11664,"162885fe6bef97696a041b72ea7293654b8eac542d24429e7d56991c992f8d0d",18000},
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
