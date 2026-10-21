#pragma once
#include "../../src/assets/audio_assets50hc.h"
#include <array>
#include <string>
namespace gb_audio_originals50GB {
struct Pin {const char* name;std::size_t bytes;const char* sha;std::uint32_t rate;};
inline constexpr std::array<Pin,45> pins{{
 {"sfx/race/animal1.vag",36528,"8a5d540321263fa06c841610a0f45ed84472a504d0cc79be0976b2733c951add",22050},
 {"sfx/race/bshscrp1.vag",30160,"744438eb2f483682675934dcfb1703825056406c0c99c8f9dd18f388253e37af",19500},
 {"sfx/race/bshscrp2.vag",30416,"8ee8989cc3666f1c7946fa9ce95f341e9cd20a5e84062e93143c268006dad797",19500},
 {"sfx/race/bshscrp3.vag",37888,"5a6170ba2d26e76cec301004a78bf3fac91c5d87643c244d91a1d6a298332e90",19500},
 {"sfx/race/bshscrp4.vag",42336,"1d4bcfb58e36016d33ec051f11d01eb663abafc5955e90e3de1a8da66df0d0cd",19500},
 {"sfx/race/bushhit1.vag",12944,"ab79779c19358f039b26b4812606c23c3d52101e440fd5270fce387de582c206",19500},
 {"sfx/race/bushhit2.vag",12208,"719fa50f7a7d644bf93be3b4c08f011f04917f0d30763f0b3bca80790cf8717c",19500},
 {"sfx/race/bushhit3.vag",13024,"a9eaebe99df9f1679633c2d36a3905daa6f8efd71588bebfffafddc37d21a5d1",19500},
 {"sfx/race/bushhit4.vag",18912,"4ce9d1535d1a05d38d2ff9929647c556064625bce25278c0be09abf99bc1ca1e",19500},
 {"sfx/race/creaking.vag",43968,"4dec87e4702f5e9893d530d71ba061950beb065aea621e33b54a054e57d697aa",22050},
 {"sfx/race/gamcrowd.vag",30528,"6d0b9e80bd2597a7aed66e5b250cfb1eb86944c6545b2ff522ccca4cead18c41",16000},
 {"sfx/race/metlhit1.vag",5888,"7670d1e0c33ae3662ce221329adeff5201000ece5964e1d335af9386740b4698",19500},
 {"sfx/race/metlhit2.vag",7040,"b0a8a062db4f0294dca2d9a95e0130dacc9a885a0564dee606a2a85b73d52577",19500},
 {"sfx/race/metlhit3.vag",6720,"75481691c79fa4f07917649f1042325d6754ff619d1dfb23e5d6adf2a864e401",19500},
 {"sfx/race/metlhit4.vag",8208,"7ad246b513b721e24e0aa894d9b1eaa4e227f213c44220485e9a0591e83de03f",19500},
 {"sfx/race/mtlscrp1.vag",28800,"ad176b5a9cb3302f0f8ac727a33da1e5662e7db38da65e2f01963d89c4bde452",19000},
 {"sfx/race/mtlscrp2.vag",25760,"b0233fbb2b5aa3df74e3747ad79d63e2781d73df42132ce63a605497edaa1304",18000},
 {"sfx/race/mtlscrp3.vag",31552,"d13aa67eed6adc6c111fcb6bbc6c16aec5b771c5bd77a5025df2f32767013c1f",22050},
 {"sfx/race/mtlscrp4.vag",31552,"d13aa67eed6adc6c111fcb6bbc6c16aec5b771c5bd77a5025df2f32767013c1f",22050},
 {"sfx/race/pod_coll.vag",14704,"4bcd3f33e1fc5c47b921a565a334566464d649b7d69972bdc8d6d58dfc5669a7",44100},
 {"sfx/race/podscrp1.vag",6544,"546b0afbea0edab78ddafd2d37a987c314bed4ab93545c2162125f0c0d77b05e",18000},
 {"sfx/race/podscrp2.vag",8016,"6bb95c1d1225594b086619a791e9eba3b85674d7ce74c1b1f870b07e4fa897dd",19500},
 {"sfx/race/podscrp3.vag",8624,"adfc070933aa60444d290a62cbdf193408a803fe3b8b66e20c1f02d65f583448",18000},
 {"sfx/race/podscrp4.vag",6048,"7d52c97c392a0a0525ac78dc1096d77641aa0e9a78b79d620ef2bf0943bb5946",18000},
 {"sfx/race/rain.vag",33648,"b2596622f50b56312de35db7bbcef6be936603a4a5c64ab93eb1c582d555ea02",16000},
 {"sfx/race/rckscrp1.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rckscrp2.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rckscrp3.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rckscrp4.vag",37376,"848272a0ab9a381e47865dec41f17b0d504f713e982957e727579b4554013ab3",22050},
 {"sfx/race/rockhit1.vag",11664,"8549689b0eb7d1ec415d8ce092a6101927b19a324cf73cc2fe7a4011e4626912",18000},
 {"sfx/race/rockhit2.vag",10400,"17fb766a6f38c969c6726db2092222bc9e4a633810b9f7ba4c6ff17157ab63b0",18000},
 {"sfx/race/rockhit3.vag",12416,"465be3afd0b990d193871d3bd5b0d1cf63d9abb084abf2594daaa1fe04e4ffca",18000},
 {"sfx/race/rockhit4.vag",11664,"162885fe6bef97696a041b72ea7293654b8eac542d24429e7d56991c992f8d0d",18000},
 {"sfx/race/thunder1.vag",20288,"55d1c72233b983c9b8a16581ffb06d9a57661fdbe7b2f0e3f84cf11ceadf77ea",18500},
 {"sfx/race/thunder2.vag",20320,"622ca9aa7a8a0c86a4d15e1d833a57fd468d3972b2abcfb3bfab693934bfa44e",11025},
 {"sfx/race/thunder3.vag",16784,"81e8044810f877f7862ab0cb950df966894f2a33fb931aac8a87036ccf52e478",11025},
 {"sfx/race/torch.vag",32320,"2be8564dd9b2e14a6bc364e3db805ce53b1a24ae402fbee864cee9123ea42381",22050},
 {"sfx/race/wodscrp1.vag",26624,"3d6a092a70b185969d2467c62796779bec471a16763eb09b2364708a978b9d12",18000},
 {"sfx/race/wodscrp2.vag",11008,"738726bf3c511f62842b1f1102d70b24ffded53f164d773992d960021650e113",18000},
 {"sfx/race/wodscrp3.vag",23968,"a8528aa242f697469983705cbf197df5e81c66e35c59e9e0313bb9c870510c98",18000},
 {"sfx/race/wodscrp4.vag",24224,"df294db9152e09db7aa347e48607850c2233ef5190cb8ca573088e6fbcc7b683",18000},
 {"sfx/race/woodhit1.vag",5936,"0e3aad8ca1be41d412176825089e8d7a7d95ecc1021ef9738e1d144213241726",18000},
 {"sfx/race/woodhit2.vag",9328,"ccbd313e0ba2bbcc4af00b9a6926525d81a0fa030c4019418aa6850379e4940e",18000},
 {"sfx/race/woodhit3.vag",5024,"96f9cdbaad8e3c924096cd206e394c2929b81cfa3e7e5a5c4485337e3a1e105e",18000},
 {"sfx/race/woodhit4.vag",8016,"e311085cd4b913b434961ff6227ec45ec1eb7c7e1a4132668874f71c702a4f81",18000},
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
