#include "original_ui_ipum_texture_cache50kg.h"
#include "original_ipu_decoder.h"
#include <algorithm>
#include <cstring>
#include <new>
#include <limits>
#include <utility>

namespace rr::assets {
namespace {
using Error=OriginalUiIpumCacheError50KG;
constexpr const char sourceMember[]="ui\\textures\\19c_1.pst";
constexpr const char sourceSha256[]="ad0bcd92f415d65d8bf68ae9e65fe355043e297e834cdffeae4ceac7cfa60cd3";
constexpr const char archiveSha256[]="112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856";
constexpr const char converterSha256[]="e482393f6b384863d17c7cc84aa930affd466eb6c6718dfd1292713408bf9efe";
constexpr const char commandArguments[]="-hide_banner -loglevel error -nostdin -cpuflags 0 -flags +bitexact -idct simple -threads 1 -err_detect explode -f ipu -i pipe:0 -frames:v 1 -pix_fmt yuv420p -f rawvideo pipe:1";
constexpr const char colorPolicyDescription[]="50FC integer BT.601; nearest 2x2 chroma; RGB16 dither; RGB5 high-bit expansion; TEXA TA0=TA1=255 AEM=1; no row flip; hardware pixel parity unverified";
constexpr std::size_t sourceBytes=23096,retainedDecodedBytes=611344;
struct SourceMipRecord {
    std::uint32_t width,height;
    std::size_t header,payload,bytes;
    const char* sha256;
    const char* filename;
};
constexpr SourceMipRecord sourceMips[originalUiIpumMipCount50KG]={
    {16,16,12,32,205,"30260bfadc135b0678023e198c919bbc76769808250bb1d64da95160a5dadc35","19c_1-mip0.rrtex"},
    {32,32,237,257,620,"0febb2bfc984a924942b49caefc5c1d024fd5b3fb54b0bf962f1ee40cf1f36a4","19c_1-mip1.rrtex"},
    {64,64,877,897,1850,"799b622653d923464c9a478e7aebfab286b7d135c5577d217840bb1f3eed7435","19c_1-mip2.rrtex"},
    {128,128,2747,2767,5469,"e017da2329dc9a9101ed1a33d32e87a3d9bb58e2ccd1055261073f6ee5bc5f7b","19c_1-mip3.rrtex"},
    {256,256,8236,8256,14840,"c8e6de01def72409f2cf85de2e6705b77261feb9194b33d50a5e4097e86474b5","19c_1-mip4.rrtex"}
};
struct RetainedMipRecord {
    const char* rrtexSha256;
    const char* rgbaSha256;
    const char* yuv420pSha256;
};
struct RetainedCodecRecord {
    const char* converterSha256;
    const char* generatorSha256;
    const char* ffmpegVersion;
    const char* ffmpegSha256;
    const char* ffmpegCommand;
};
#include "original_ui_ipum_texture_cache50kg_generated.inc"

struct AdditionalSourceRecord {
    const char* member;
    std::size_t sourceBytes;
    const char* sourceSha256;
    std::size_t mipCount;
    SourceMipRecord sourceMips[originalUiIpumMipCount50KG];
    RetainedMipRecord retainedMips[originalUiIpumMipCount50KG];
    RetainedCodecRecord codec;
};
#include "original_ui_ipum_texture_cache50kg_additional_generated.inc"
struct GarageSourceRecord {
    OriginalUiArchiveId50KG archive;
    std::size_t memberIndex;
    const char* archiveSha;
    AdditionalSourceRecord source;
};
// The 23 authenticated E archives carry the identical original spcgarge PST.
// Reuse its compressed-layout metadata, never its archive/member provenance.
#define garageSourceCount garageBaseSourceCount
#define garageSources garageBaseSources
#include "original_ui_ipum_texture_cache50kg_garage_generated.inc"
#undef garageSources
#undef garageSourceCount
constexpr std::size_t garageSourceCount=33;
const std::array<GarageSourceRecord,garageSourceCount> garageSources=[] {
    std::array<GarageSourceRecord,garageSourceCount> rows{};
    for(std::size_t i=0;i<garageBaseSourceCount;++i)rows[i]=garageBaseSources[i];
    constexpr std::uint32_t careers[]{1,2,3,4,7,8,9,10,11,12,14,15,16,17,18,20,21,22,23,24,25,26};
    std::size_t next=garageBaseSourceCount;
    for(const auto career:careers){
        const auto* archive=originalUiGaragePodArchive50KG(career);
        auto row=garageBaseSources[10];
        if(archive){row.archive=archive->id;row.archiveSha=archive->sha256;}
        rows[next++]=row;
    }
    return rows;
}();

// Independently retained original whole-member pins. Generated conversion
// records may describe only this finite source set. The Gasgano source pin
// alone supplies no converted pixels and cannot make a cache available.
// Exact original compressed layouts; no decoded-output hashes.
constexpr SourceMipRecord completePreviewSourceMips[][originalUiIpumMipCount50KG]={
    { // 04c_1
        {16,16,12,32,244,"064b4ca5f65497a405dc9925a6e3ff91bcc977339d63ddc1fa738a56d5ea7949","04c_1-mip0.rrtex"},
        {32,32,276,296,832,"9a491ee97b2e17741985bb8dffa36f6e09a3fe5ec86e198f2d91d3587c73998b","04c_1-mip1.rrtex"},
        {64,64,1128,1148,2883,"490b3414939b75d9e16debd0d4f37971ef397903e14b1060a2ffb6136ea373c3","04c_1-mip2.rrtex"},
        {128,128,4031,4051,9063,"7c1aea0b0bc918946855cdf8aaafd72b843fa34fc0e0506a74710b4585c65599","04c_1-mip3.rrtex"},
        {256,256,13114,13134,26307,"ada03cfc2aba63bc53ca8c545be3b9be4947a49d89fe6b41c02440b53b30cf20","04c_1-mip4.rrtex"},
    },
    { // 04c_2
        {16,16,12,32,230,"c8cbde141dc2a6f71f52a0ee182a0c0031f69923e031cd023487cf574c4b6a48","04c_2-mip0.rrtex"},
        {32,32,262,282,743,"8c35fd5933638443dbf0d200d781a4cb6e2f8fb0b13a5a110ad7ef865033cebb","04c_2-mip1.rrtex"},
        {64,64,1025,1045,2476,"7c1114e31d5a18f4a00e32497e4249370b3468ac0709f8b7181718d5f64f4322","04c_2-mip2.rrtex"},
        {128,128,3521,3541,7463,"ed1a614b2e8827c7a1061c64ddb61ebd21381caab86042e05add77027142ede4","04c_2-mip3.rrtex"},
        {256,256,11004,11024,22286,"623351be7ebc1c6833d1104dcf99708126a61b0295c2582e4f6f79a2fe50468b","04c_2-mip4.rrtex"},
    },
    { // 10c_1
        {16,16,12,32,220,"112569e2541fced9df0a877765da5677e3e3cbc53f6be523f6c662c228267aab","10c_1-mip0.rrtex"},
        {32,32,252,272,651,"069e0d01774efac41d4aff3321b94811e0f4efe41017f096fa3edd03f09fcf1c","10c_1-mip1.rrtex"},
        {64,64,923,943,1969,"f787c7e092af9646490a97260c1f1ac260ac8d03b348f499c1927866d9f89e34","10c_1-mip2.rrtex"},
        {128,128,2912,2932,5522,"9dbd9f3167a527e60fda04c7f389596e8cdf4e859fbbff548bec3ce54727ad41","10c_1-mip3.rrtex"},
        {256,256,8454,8474,14837,"a2a6578a40d4d31ca0accebacc4fa1a84f49913cbb986ec7d0c2b20fece52df5","10c_1-mip4.rrtex"},
    },
    { // 10c_2
        {16,16,12,32,177,"428fc57e65e8ae56408fabb1edd40d868080f7bb1a10adab5599b65431cba0cf","10c_2-mip0.rrtex"},
        {32,32,209,229,438,"a33c607667a73eea8e04440044cb4488bf6548ed039256df43d8608255f9036a","10c_2-mip1.rrtex"},
        {64,64,667,687,1359,"d3ea1a118647206bfcbc10a8c961f23f2ae6cd11b7c608dbe0dd02ac78b1032b","10c_2-mip2.rrtex"},
        {128,128,2046,2066,4101,"18f585985a66b160fbad9c714fa876010d5718f184c37a837890e6da8008b22d","10c_2-mip3.rrtex"},
        {256,256,6167,6187,12307,"5d1786675118a0f746d075c8f4886389daa6664bb95c0c301fb3c77e4bc3d5c8","10c_2-mip4.rrtex"},
    },
    { // 12c_1
        {16,16,12,32,169,"3dd44524376ca3df4d890d023052c809fe1f285b8f55fdde103540565d07fe14","12c_1-mip0.rrtex"},
        {32,32,201,221,492,"cf2c2cf2d8e8644ad1c6e0136fb458c1dafad450645f6a7215b163c50769bc92","12c_1-mip1.rrtex"},
        {64,64,713,733,1487,"3cd4f0b1ee1c04379a7709a1829c5ad4cd263c68611af93d66775e8bb46acfb1","12c_1-mip2.rrtex"},
        {128,128,2220,2240,4947,"0261e520ab702b88239b187a404aeecc10dc0fadbc6140bfbdf8ebb812830e1a","12c_1-mip3.rrtex"},
        {256,256,7187,7207,15374,"7b23ad7b6836896b7b1d5b1b1748f986128a910f6d0b7cbe0a5aebc3abe9aec2","12c_1-mip4.rrtex"},
    },
    { // 15c_1
        {16,16,12,32,190,"14e9fce2e84ca530ee87674895818b37fef2cc2148cc2fe66558a8cbcca82256","15c_1-mip0.rrtex"},
        {32,32,222,242,541,"e60aaedc04fd7137021c89e2cfd63f12aebb8d80bb7fb9c74e2bb151e347aeb5","15c_1-mip1.rrtex"},
        {64,64,783,803,1796,"9dc4d093d374752263f408953ff0d44f496b1e8c2b5db7d4772e41975be88424","15c_1-mip2.rrtex"},
        {128,128,2599,2619,5691,"d56bf2774be34c1f88d77eb6516ed2bb94e3606d9e7eadd131f361f80c22b095","15c_1-mip3.rrtex"},
        {256,256,8310,8330,17140,"073186ba4379c2766043729002deec7eb2aba0caff01397d15c5143569f55570","15c_1-mip4.rrtex"},
    },
    { // 15c_2
        {16,16,12,32,204,"088fa0ccf387498d7fd2bd0f6a47312d48bd27ced025460cf2ddc9581b606538","15c_2-mip0.rrtex"},
        {32,32,236,256,652,"8214047ecb31394ec19042bc4e30ca86deb6f318e1b9b341691452ab84b5d55d","15c_2-mip1.rrtex"},
        {64,64,908,928,2159,"863cc0d2b76c103f77608dd0d6032c7b8fa2c6a57d2b1cf8e733a5f1d4e1de90","15c_2-mip2.rrtex"},
        {128,128,3087,3107,6953,"4e2f9b6f21ca07081e3ad53309f0eb969b10076c848b9e6d1f475321b2909877","15c_2-mip3.rrtex"},
        {256,256,10060,10080,21602,"f39928eeb077db7be0f40b0699383d7439bb819007d1ddf705fdfbeb70140eda","15c_2-mip4.rrtex"},
    },
    { // 07c_2
        {16,16,12,32,216,"51c6ffdc77b9e50ffff5d9b7d00241f4da0c6a1d9de99cc84423f24e660321a2","07c_2-mip0.rrtex"},
        {32,32,248,268,776,"a93eec935f3de3fec135477ff4f22f9ebb9889acbeacc1d85f1197548ebef856","07c_2-mip1.rrtex"},
        {64,64,1044,1064,2392,"d525e93bf42b571ebaf2327a5f00bc4ab2c2b80b2dd242aeb43c79644cc2ba87","07c_2-mip2.rrtex"},
        {128,128,3456,3476,7101,"fbb471df280643efeb1afa45fdc8b9d29b44da9a12c270900ba83aa25e021424","07c_2-mip3.rrtex"},
        {256,256,10577,10597,18915,"89b135287cd4fbc7a0ecbeda1105396abe6eb8ca1dfab643553f8416fb7f3e6d","07c_2-mip4.rrtex"},
    },
    { // 03c_1
        {16,16,12,32,224,"4a53e555980dae8c11723806cc8b51da3ba474b64eb6c5ba1edcdef490b1675b","03c_1-mip0.rrtex"},
        {32,32,256,276,628,"66bb59bf9ab1089a29fc6cda009f938b1c4f6093e8b51e78f89ee1b2f804d327","03c_1-mip1.rrtex"},
        {64,64,904,924,1860,"770a5f874c1cf9077fdd3e45634f937f93b17cd5269b414e1d27ac35d18ab427","03c_1-mip2.rrtex"},
        {128,128,2784,2804,5001,"e773799648dde43f8f4785c435c2f83b5897c92e111bd4ea7effc3c17cdce726","03c_1-mip3.rrtex"},
        {256,256,7805,7825,13214,"b1fa6aabd54ade02cc17157a3e711791f7b03351e8174292a27090adee8528ec","03c_1-mip4.rrtex"},
    },
    { // 03c_2
        {16,16,12,32,185,"b95b149068cf247b47a96f8daaca6719851c8713c480614f98f54329b07ecdbd","03c_2-mip0.rrtex"},
        {32,32,217,237,610,"274762b0dd7bab0534eb92fccf77d57e748e92173b584d25be349ef92cb11d6c","03c_2-mip1.rrtex"},
        {64,64,847,867,1947,"f2f492634bbec0b5d122a3b7539420546a3af5cfffa512471bc4ec83e0d56369","03c_2-mip2.rrtex"},
        {128,128,2814,2834,5680,"0e2b4a5996a8cc6c9174d73700109c5c86956c18c9ebf899f6fa916653b39236","03c_2-mip3.rrtex"},
        {256,256,8514,8534,16650,"99453b54a9ed3114e879a459ba2cb62f1ac08ec0e6c98ba8ba04700e92325b4e","03c_2-mip4.rrtex"},
    },
    { // 01c_1
        {16,16,12,32,184,"841f94fbc864d392e53f102480086c31b03f63a03e5b58b8c176592a94fe7b29","01c_1-mip0.rrtex"},
        {32,32,216,236,544,"b5d1f269624d1555cd81ea49060431b7b275c930846cc6bfa2ad74db878d97f4","01c_1-mip1.rrtex"},
        {64,64,780,800,1673,"8da782b1630ec36acb801ec9cd6a8149802afe5cb71f52beed2657ea891243b3","01c_1-mip2.rrtex"},
        {128,128,2473,2493,4651,"54ca4ac51bd7b02521e3bd37887f3fb61ea50c96c085c56b0b95002016c57f9f","01c_1-mip3.rrtex"},
        {256,256,7144,7164,13038,"09d2839daf5ba34e433ed6327b051fc65067b837b9b131b3f3fa951e602f291d","01c_1-mip4.rrtex"},
    },
    { // 01c_2
        {16,16,12,32,188,"65256c33278a91e52c047b02f5711a6c18782dbc0521a6b5b2b6c36fc74b6b44","01c_2-mip0.rrtex"},
        {32,32,220,240,486,"802cf88138129e809ec5c522336989a71f1d75d764a4cbdb6ce1b67965709373","01c_2-mip1.rrtex"},
        {64,64,726,746,1416,"95fba8079da2919d1905555567718b827055ea9ac62c69241eb3ae12d2dcc5e2","01c_2-mip2.rrtex"},
        {128,128,2162,2182,4572,"ddb6422654e7e251503b1965a58951d95df73b4ab45981344901f6b9af784985","01c_2-mip3.rrtex"},
        {256,256,6754,6774,14957,"b48dc0171c56649c1b9e58dc0413364968fb7d7d0e47e50b7cba44619aa551c6","01c_2-mip4.rrtex"},
    },
    { // 08c_1
        {16,16,12,32,285,"159e55000aca2625cb2acd17b383ec67e3f9b404449b20975a93b677b8ee5568","08c_1-mip0.rrtex"},
        {32,32,317,337,879,"25038a6d42edb12b97935517c2a2b174b517aaa4eac1ccd8a89c99e617d49af4","08c_1-mip1.rrtex"},
        {64,64,1216,1236,2767,"29ad51212be026f7401e200da311a7e46680b0ab5647388fe2aa841b53c25d05","08c_1-mip2.rrtex"},
        {128,128,4003,4023,8821,"70ecc19bc778f77d8b4643263e11aa21ca9ce40e7ded5bc3bfdec7268f2c7f71","08c_1-mip3.rrtex"},
        {256,256,12844,12864,25484,"07d143ba1f953a251c7b6bdc503562d5a244278165275ba90e00f80360d4a1c0","08c_1-mip4.rrtex"},
    },
    { // 08c_2
        {16,16,12,32,214,"79d7acf913d8f173f01a7fe5e89e3ef62836624224bf2f9ec0c5294838ee098d","08c_2-mip0.rrtex"},
        {32,32,246,266,683,"ee8df29836f7dea032bad072a92e0ca9bd2487d7c2423767114457512aafb6e1","08c_2-mip1.rrtex"},
        {64,64,949,969,2258,"2c86ab06b6ab0f8afb3b0f6140e252d226feb0b72c8217ef735eac239bfb9f7b","08c_2-mip2.rrtex"},
        {128,128,3227,3247,7329,"d0d9a63651e78a56092a4fa90424a532516b918fbd041d4e5bd5aa8c31d586ae","08c_2-mip3.rrtex"},
        {256,256,10576,10596,21002,"2841153ba123749ffe80b1cd6446ef9d8e7080e9893d1b0278771b7463e6de0f","08c_2-mip4.rrtex"},
    },
    { // 09c_1
        {16,16,12,32,213,"dbf517782054d8d3bca31ecf7ed2d7a1fd71905e5ef1f843583b086a82986e9e","09c_1-mip0.rrtex"},
        {32,32,245,265,620,"66350afef20d90846897454a38c752d75f485c506238f15e7ff72a0190c00e03","09c_1-mip1.rrtex"},
        {64,64,885,905,1837,"7fee78c699be6808110f6d4cb597c7b14d4566e7400d13dafb40ac43c5152dd7","09c_1-mip2.rrtex"},
        {128,128,2742,2762,5715,"c4c7ee574ca52b163aad12e2daf68e3c3ac5eda432530c6f49e003b5b3c0e00d","09c_1-mip3.rrtex"},
        {256,256,8477,8497,17281,"6f67b318890dc22935ceeff8e7a3637f81a8f44fdd46e012a3dcf73d61ed56df","09c_1-mip4.rrtex"},
    },
    { // 14c_2
        {16,16,12,32,196,"eda0b81052ddd92697695a236ee6b635d100ec672fc58a49ce155a0ef6224292","14c_2-mip0.rrtex"},
        {32,32,228,248,658,"ed39b16d1d1d6546641e7c7d62d992ddb028ee4e16adf6b2e787ff6d09e4d0bc","14c_2-mip1.rrtex"},
        {64,64,906,926,2391,"15c961b47c75ee969fb13341531016b8558dfb14bd3365fa3a331cffc173fb88","14c_2-mip2.rrtex"},
        {128,128,3317,3337,7881,"06d469ef94c17d62398457129be440142675ffa0c87731eca885b08a58268062","14c_2-mip3.rrtex"},
        {256,256,11218,11238,24671,"dad1cc91d33e4d549d9a2f449cee2cea9313046e93b9d8d1cb5f3b85038c2aa6","14c_2-mip4.rrtex"},
    },
    { // 16c_1
        {16,16,12,32,270,"30048efde56a2c4442aaa2b1d0f94876ece01ef3260f950404b4b7ec36deb4cb","16c_1-mip0.rrtex"},
        {32,32,302,322,822,"992816b7910e93fa698c3a80b6446ee84cc577347fd16d32c1578ae6c27d6359","16c_1-mip1.rrtex"},
        {64,64,1144,1164,2755,"2e9daa117ea480a798480568a43e50ecdf1f64ad729b9a5b83c1b3cd660cf40e","16c_1-mip2.rrtex"},
        {128,128,3919,3939,9492,"ddbebb4e45037e33e20ff57f389aa6c8d8bf33a0e5a96d5345797cf5c669fdcc","16c_1-mip3.rrtex"},
        {256,256,13431,13451,31004,"054fc1a47d9c8215a3ec25628db52b100c5dbba0aa6ca9368235fd308a3ed977","16c_1-mip4.rrtex"},
    },
    { // 16c_2
        {16,16,12,32,205,"93278bc1bd915be95bad42bb77dd3eab64d64b3404723098e09f2dbb80ce0725","16c_2-mip0.rrtex"},
        {32,32,237,257,706,"7a1bc5d3b7648e1c930f7159e67c9b005229f506160feb9a6ea52f3f799a6566","16c_2-mip1.rrtex"},
        {64,64,963,983,2299,"48757ff14c6a431a17d07262649a6444c7cee69361a6048aecc7ee1cfda26687","16c_2-mip2.rrtex"},
        {128,128,3282,3302,7442,"4070ef250f23639f7ce277c3cea691891ce59f0796edea42b01397ed9093359d","16c_2-mip3.rrtex"},
        {256,256,10744,10764,24549,"778b28d4c186d3a29e33c0c69087f701f503a53153a1fb9f43f1408546ad57c3","16c_2-mip4.rrtex"},
    },
    { // 17c_1
        {16,16,12,32,204,"e030c6da277942b652b941669820bd69d4313e82f6418cb47421f5b1a6e04ab6","17c_1-mip0.rrtex"},
        {32,32,236,256,620,"d6fba88d47c7e6049a4146cb48dce6c6c6e601b430a5aebc6933f4a1d1e18f04","17c_1-mip1.rrtex"},
        {64,64,876,896,2260,"816d3a109ea477b08afd2d02020637814b32e5d47f6b9ce90adcfd073093ba82","17c_1-mip2.rrtex"},
        {128,128,3156,3176,8245,"4d4bcd5e62bd3bf9cad4f70df584560a84940ea336c518bba92a516315b1cfd7","17c_1-mip3.rrtex"},
        {256,256,11421,11441,27157,"c42ddf75497bf24b347d21d75fe563ab57d8a6234001c645c1bc04457ab37104","17c_1-mip4.rrtex"},
    },
    { // 17c_2
        {16,16,12,32,211,"4bbe8d9e3ffc9dbe1d748210f849d16e5e3eec714e7ecac08c38c91909620b4b","17c_2-mip0.rrtex"},
        {32,32,243,263,671,"19828db9bba4eb9a628ce82044627172f03841462a097579f9dd08f83c098b19","17c_2-mip1.rrtex"},
        {64,64,934,954,2204,"3cc3e1f82601deda14eec10a4279d54cecb2ab4983b4e1155fb8cd86af3c1f70","17c_2-mip2.rrtex"},
        {128,128,3158,3178,8018,"c280ee077086ef11ef9b0fa463bc82fc6dca312b74691d92c315849b280173aa","17c_2-mip3.rrtex"},
        {256,256,11196,11216,27545,"350c5fab2ccaf157c645043daddca718c74cde3cd525b65d5a16bcf2511be5b8","17c_2-mip4.rrtex"},
    },
    { // 18c_1
        {16,16,12,32,200,"a5a8ac1f00147310042e3ee815838f641da6c2ea3e7d0e8ddb1ac2ef911b74af","18c_1-mip0.rrtex"},
        {32,32,232,252,687,"b7a08c353ebe2f16d06b449544d81b5104b42b085c53bf4178928b958adc6e9e","18c_1-mip1.rrtex"},
        {64,64,939,959,2320,"6fd5eec31e2baf21e3aba880a0defc634fc7c8c068938a2445ca933703f2f131","18c_1-mip2.rrtex"},
        {128,128,3279,3299,7444,"aa4edbbc0c06da26b2f71282db6b9716cc0fef396d6c1d6a0a07663198600742","18c_1-mip3.rrtex"},
        {256,256,10743,10763,23263,"ce58fa0ddb9de24bcbe13cc2c8e469a6b6480e57c634156cf4b354211e77a633","18c_1-mip4.rrtex"},
    },
    { // 18c_2
        {16,16,12,32,190,"13184f5ece9ca405aef52fbecdc01aaf4fc533b49e382fc991ced4829140a960","18c_2-mip0.rrtex"},
        {32,32,222,242,648,"4cbfe2fd368c19c3833c065c9acb03c3be7a112473b15c8e55acf5394e857ee4","18c_2-mip1.rrtex"},
        {64,64,890,910,2348,"3bbf81ef9c95928b15b133c22708a742e99354cbd65f332d52651ea9f4761e75","18c_2-mip2.rrtex"},
        {128,128,3258,3278,7753,"c772f2f0bcaea9b90ea640fb9f8b2a7c61beb459d687240c250796fe0771a392","18c_2-mip3.rrtex"},
        {256,256,11031,11051,24340,"b833bb375f0784ccb6534178a65f835008dae6305e00ee21f881fed2e7b67514","18c_2-mip4.rrtex"},
    },
    { // 22c_1
        {16,16,12,32,230,"973b1871408390e8ead24725eec93e06257f01133e9f7356019446c0fc681dcd","22c_1-mip0.rrtex"},
        {32,32,262,282,744,"9c7c9fe81964c90b960f8fb2c4424be46f6ebf4fbdfc9025cdc08e88e2160659","22c_1-mip1.rrtex"},
        {64,64,1026,1046,2621,"e5a76b5e38c708e54e22ad9e170c57c3807210f2a65157277626963c8b5234e2","22c_1-mip2.rrtex"},
        {128,128,3667,3687,8948,"dfa372c18dd40cc84fd140da32e61c5369649a94c4fcbb5b0f04337ac0bfc884","22c_1-mip3.rrtex"},
        {256,256,12635,12655,28396,"40d172d86bfd525608fff5f9370470271e8b98950422f7051757a2b5943d29b5","22c_1-mip4.rrtex"},
    },
    { // 25c_1
        {16,16,12,32,230,"d32f8462e7b00c87c26ed6e253c1ceab3e832096977636babfa58eb913f1e5ea","25c_1-mip0.rrtex"},
        {32,32,262,282,722,"c117673c8879744e00b5607ef077515bc5e5daeeb146187f24c6133c3d4a74f3","25c_1-mip1.rrtex"},
        {64,64,1004,1024,2319,"1ae7885332bd3ed01be915ab92218627aa8718d246c622709470fb52c3fe3092","25c_1-mip2.rrtex"},
        {128,128,3343,3363,7309,"7e0e94224db5913ae91257416d1e9bb859c00f047168f1d78cbf092c51b38974","25c_1-mip3.rrtex"},
        {256,256,10672,10692,22320,"20c3ad57eb94661a92f8b58837ebf009b2fd02bf6a443ed3051e4c78f4a2b67a","25c_1-mip4.rrtex"},
    },
    { // 25c_2
        {16,16,12,32,265,"ebc242e34a3ea16a1b1e9ac8cef22be6e0224b84737f007d5402670eae83f5ee","25c_2-mip0.rrtex"},
        {32,32,297,317,832,"eeff56fd9956e0950aef8f3b01a8b39efa5626b12d3b48cd2764791ffd416552","25c_2-mip1.rrtex"},
        {64,64,1149,1169,2618,"87608430c9a4a7310038e6333e6dcf2ef1ec980a6aa10b15906b156c585fdaed","25c_2-mip2.rrtex"},
        {128,128,3787,3807,8208,"f1be06c7402c0916693af4427e31a9b7861088b275ea145b991a3b1bb63d0a8c","25c_2-mip3.rrtex"},
        {256,256,12015,12035,24222,"11e5cb781cf7a191568e751be4f7b4cb9e03066237836abe11f0cdf9ae1a681c","25c_2-mip4.rrtex"},
    },
    { // 25c_3
        {16,16,12,32,253,"cc5a76095859cb3466646abefcc318c8480d0a1d880604f1fd91548c720eacef","25c_3-mip0.rrtex"},
        {32,32,285,305,654,"64665b61275091715325de0a3d4646ea81cd320efc0eccdc3a6026e0e82883c8","25c_3-mip1.rrtex"},
        {64,64,959,979,1805,"3819b3d60ec85d7dbecb5449e5dda5cdfcc6ebed9f7091782de36d7958b0602a","25c_3-mip2.rrtex"},
        {128,128,2784,2804,5104,"abf24e948f0a087a5348b3f1623501cdf7804a1b6545f09c53b21ea0aaa90e75","25c_3-mip3.rrtex"},
    },
    { // 25c_4
        {16,16,12,32,188,"518eee389f6cefd2bc0d2f85a75dcf87393c0a0fbb7d7418e7218fd6283ac215","25c_4-mip0.rrtex"},
        {32,32,220,240,515,"eeba8ff5fd852911542c0ff4791f7b8af1a090adbeee5e5de01aa32a03937cc3","25c_4-mip1.rrtex"},
        {64,64,755,775,1369,"7ea87df0c122cb6754256b74e5cf254ac7cd53211093e233e654d3eecf5dad65","25c_4-mip2.rrtex"},
        {128,128,2144,2164,3661,"2a9039348e50d93e3a985bb9356af544cbef8bef2f6c20b63e60205c6374c325","25c_4-mip3.rrtex"},
    },
    { // 25c_5
        {16,16,12,32,146,"6b0ace3a5a9c16aa52eb289333e03cacb6fb0beb22812e03e3d822d5c721b72c","25c_5-mip0.rrtex"},
        {32,32,178,198,384,"cc2696254c2c9914149a08e19f1159a351a4a061c4d2c98fe1923d325803623a","25c_5-mip1.rrtex"},
        {64,64,582,602,970,"25078b81b1c9d5ccab0737d0d927cf68fe0cab4fedff584f6209f11a05db593c","25c_5-mip2.rrtex"},
    },
    { // 25c_7
        {16,16,12,32,187,"7703f5f9ac39c75bd3ea5688cdfc8eabf54f6350c33748cf396e816502d0df65","25c_7-mip0.rrtex"},
        {32,32,219,239,648,"ae1f69d452e98d20666c63ec4893cbba35385ce3c53a1c54080ba5f92ccfe0fd","25c_7-mip1.rrtex"},
        {64,64,887,907,1684,"ec3de160ca4e16fd5f42c9eb7484d170ae0e160e6823590d6f1e61460d45d0bd","25c_7-mip2.rrtex"},
    },
};
struct AdditionalPin {const char* member;std::size_t bytes;const char* sha;std::size_t mipCount;const SourceMipRecord* mips;};
constexpr AdditionalPin additionalPins[]={
    {"ui\\textures\\04c_1.pst",39441,"ca086fb878b398f6fccdcb2161d65ff38bacdb2ee5a3c96ff06e1ed814dbfd27",5,completePreviewSourceMips[0]},
    {"ui\\textures\\04c_2.pst",33310,"5f5979fa86f0043be411d84bfb33244921482290eda4c0f4df50e4af1faa32ff",5,completePreviewSourceMips[1]},
    {"ui\\textures\\10c_1.pst",23311,"99ccba918bd470d396ebb618b382a81907839a34d0b34b9beab8b5a09e9c0722",5,completePreviewSourceMips[2]},
    {"ui\\textures\\10c_2.pst",18494,"9efd4f229c3203e9a27252cac070a1107c83598a7bc536f51a9c46cbb4852ae5",5,completePreviewSourceMips[3]},
    {"ui\\textures\\12c_1.pst",22581,"eecda390eca1ea261f80ba992a37a225613f12d6bc9d0e9545d2380f64662b32",5,completePreviewSourceMips[4]},
    {"ui\\textures\\15c_1.pst",25470,"e02462b1c14fc1498a46b91b4245dc8453da45ffe674b70eae080828fdef557d",5,completePreviewSourceMips[5]},
    {"ui\\textures\\15c_2.pst",31682,"cf7093e713ad87e15301435a51beb6cd2cf958dfe74c72dc2657f7cbf1c1ccd5",5,completePreviewSourceMips[6]},
    {"ui\\textures\\07c_2.pst",29512,"cbd5e15d11e7b1b9336a1e8f965b18ecdb93d948e770baa2bc3005e3576b11ce",5,completePreviewSourceMips[7]},
    {"ui\\textures\\03c_1.pst",21039,"abfcdd11b1c519b9e5a66015016a7f93875b4050fd802e961a7ffc90f11b4293",5,completePreviewSourceMips[8]},
    {"ui\\textures\\03c_2.pst",25184,"8d1e23f9284bd6eb2b22b0d2e6e1ffce48d2474efecaa46a553086ddb8537916",5,completePreviewSourceMips[9]},
    {"ui\\textures\\01c_1.pst",20202,"6c6fbaa2447cdf6232532f12d07f6307a1ed9b073e0b4f7c32361706ae5d4aed",5,completePreviewSourceMips[10]},
    {"ui\\textures\\01c_2.pst",21731,"163fb5c6d28bff55e7eeed4f91adedb0402b0d5ad4e2f6be2a6a7e7856e8a154",5,completePreviewSourceMips[11]},
    {"ui\\textures\\08c_1.pst",38348,"bdbe402cf0569f79bace8e19b773b5ceeaa51e0792dccfab3739dfae8fdab33a",5,completePreviewSourceMips[12]},
    {"ui\\textures\\08c_2.pst",31598,"f9d8b1e23a8633342f7befd597b9f8ba8864d150b9087545a36da48d65225e65",5,completePreviewSourceMips[13]},
    {"ui\\textures\\09c_1.pst",25778,"3e59ec8fcb97d760a97c140dc49428c62feb72cc40f6e5166bcf7e7b83fe75d5",5,completePreviewSourceMips[14]},
    {"ui\\textures\\14c_2.pst",35909,"8d5e381345a903586560c00a977068011a348477ea81c47984bfb400f1386899",5,completePreviewSourceMips[15]},
    {"ui\\textures\\16c_1.pst",44455,"347dcbadc92150ef675a170fb61dc065d4dd63a75b3e518be84c84665f4aecf1",5,completePreviewSourceMips[16]},
    {"ui\\textures\\16c_2.pst",35313,"98714dbc1865359e959179503db9025c7f5f208760515188a43cff63bfd06ea5",5,completePreviewSourceMips[17]},
    {"ui\\textures\\17c_1.pst",38598,"250b5fc43687171fc04ca3477a4d1bd55881f778155b81f22ecc07e3e2cec2b7",5,completePreviewSourceMips[18]},
    {"ui\\textures\\17c_2.pst",38761,"9aad2297988bec55e8dca4d21664604661273da961d1ba2e7010724857fa6f60",5,completePreviewSourceMips[19]},
    {"ui\\textures\\18c_1.pst",34026,"70e8503a0e413489d8c6236a48847a21a82e7421dfc3351ad2e87b422206e0ff",5,completePreviewSourceMips[20]},
    {"ui\\textures\\18c_2.pst",35391,"c88c2dcd81b9bf680d8b0e91e064a9787c8a717e3a9b2425a82831ef3abb5b09",5,completePreviewSourceMips[21]},
    {"ui\\textures\\22c_1.pst",41051,"c189a488d0ad67d1a7a24ba97ec5f7777c2432c3019c7dcd1818b1fbe2d056da",5,completePreviewSourceMips[22]},
    {"ui\\textures\\25c_1.pst",33012,"3b34c48a254652b9f2d52d9425d3a7016a6e275c11b528cd7b9c16e64dad590a",5,completePreviewSourceMips[23]},
    {"ui\\textures\\25c_2.pst",36257,"1d461875c5f73d8acdd0a2c3a75dae62fc2a64328dc0ca43a514e45b9eff9613",5,completePreviewSourceMips[24]},
    {"ui\\textures\\25c_3.pst",7908,"9beeb102cd83c2c2efe8b9a4c75c4e0837132e5ba8981619a54e62fbba5f6f8e",4,completePreviewSourceMips[25]},
    {"ui\\textures\\25c_4.pst",5825,"3672ccb0eca3dc4f5c180f90f585d7a3f126bc6d8545a8250044f40b9961622e",4,completePreviewSourceMips[26]},
    {"ui\\textures\\25c_5.pst",1572,"2ade36706c9f0037265d9c46062fb6b67cf2fcace6a090a74fc4611c61b2b54c",3,completePreviewSourceMips[27]},
    {"ui\\textures\\25c_7.pst",2591,"1666b0d687043b785aad88cf78aa06aed446a10daa91a28f0cea0f561c0e0af5",3,completePreviewSourceMips[28]},
};

bool fail(OriginalUiIpumCacheDiagnostic50KG& d,Error error,std::size_t mip=0) noexcept {
    d.error=error;d.mip=mip;return false;
}
bool cancelled(const DecodeHooks48C* hooks) noexcept {
    return hooks&&hooks->cancelled&&hooks->cancelled(hooks->context);
}
bool hexHash(const char* s) noexcept {
    if(!s||std::strlen(s)!=64)return false;
    for(unsigned i=0;i<64;++i)if(!((s[i]>='0'&&s[i]<='9')||(s[i]>='a'&&s[i]<='f')))return false;
    return true;
}
bool catalogValid() noexcept {
    if(!retainedCatalogAvailable||!hexHash(retainedCodecRecord.converterSha256)||
        std::strcmp(retainedCodecRecord.converterSha256,converterSha256)!=0||
        !hexHash(retainedCodecRecord.generatorSha256)||!hexHash(retainedCodecRecord.ffmpegSha256)||
        !retainedCodecRecord.ffmpegVersion||!retainedCodecRecord.ffmpegVersion[0]||
        !retainedCodecRecord.ffmpegCommand||
        std::strcmp(retainedCodecRecord.ffmpegCommand,commandArguments)!=0)return false;
    for(const auto& mip:retainedMipRecords)
        if(!hexHash(mip.rrtexSha256)||!hexHash(mip.rgbaSha256)||!hexHash(mip.yuv420pSha256))return false;
    return true;
}
bool additionalCatalogValid() noexcept {
    if(!additionalCatalogAvailable||!additionalSourceCount||additionalSourceCount>originalUiIpumAdditionalCapacity50KG||
       additionalSourceCount>sizeof(additionalSources)/sizeof(additionalSources[0]))return false;
    for(std::size_t index=0;index<additionalSourceCount;++index){
        const auto& a=additionalSources[index];const auto& c=a.codec;
        if(!a.member||!hexHash(a.sourceSha256)||!a.mipCount||a.mipCount>originalUiIpumMipCount50KG)return false;
        const AdditionalPin* pinned=nullptr;
        for(const auto& pin:additionalPins)if(!std::strcmp(a.member,pin.member)&&a.sourceBytes==pin.bytes&&
            !std::strcmp(a.sourceSha256,pin.sha))pinned=&pin;
        if(!pinned||a.mipCount!=pinned->mipCount)return false;
        const SourceMipRecord* exactMips=pinned->mips;
        for(std::size_t j=0;j<index;++j)if(!std::strcmp(a.member,additionalSources[j].member))return false;
        if(!hexHash(c.converterSha256)||std::strcmp(c.converterSha256,converterSha256)||
           !hexHash(c.generatorSha256)||!hexHash(c.ffmpegSha256)||!c.ffmpegVersion||!c.ffmpegVersion[0]||
           !c.ffmpegCommand||std::strcmp(c.ffmpegCommand,commandArguments))return false;
        for(std::size_t i=0;i<a.mipCount;++i){const auto& m=a.sourceMips[i];const auto& r=a.retainedMips[i];
            if(m.width<16||m.height<16||m.width>1024||m.height>1024||
               (m.width&(m.width-1))||(m.height&(m.height-1))||
               (i&&(m.width<=a.sourceMips[i-1].width||m.height<=a.sourceMips[i-1].height))||
               !hexHash(m.sha256)||!hexHash(r.rrtexSha256)||!hexHash(r.rgbaSha256)||!hexHash(r.yuv420pSha256)||
               !m.filename||!m.filename[0]||std::strchr(m.filename,'/')||std::strchr(m.filename,'\\')||
               std::strstr(m.filename,".."))return false;
            if(exactMips){const auto& expected=exactMips[i];
                if(m.width!=expected.width||m.height!=expected.height||m.header!=expected.header||
                   m.payload!=expected.payload||m.bytes!=expected.bytes||
                   std::strcmp(m.sha256,expected.sha256)||std::strcmp(m.filename,expected.filename))return false;
            }
        }
    }
    return true;
}
bool garageCatalogValid() noexcept {
    if(!garageCatalogAvailable||garageSourceCount!=33)return false;
    for(std::size_t index=0;index<garageSourceCount;++index){
        const auto& binding=garageSources[index];const auto& a=binding.source;const auto& c=a.codec;
        if(binding.archive!=OriginalUiArchiveId50KG::garage&&originalUiGaragePodCareer50KG(binding.archive)<0)return false;
        const auto* archive=originalUiArchiveIdentity50KG(binding.archive);
        const auto* member=originalUiMemberIdentity50KG(binding.archive,binding.memberIndex);
        if(!archive||!member||!hexHash(binding.archiveSha)||std::strcmp(archive->sha256,binding.archiveSha)||
           std::strcmp(member->name,a.member)||member->bytes!=a.sourceBytes||std::strcmp(member->sha256,a.sourceSha256)||
           !a.mipCount||a.mipCount>originalUiIpumMipCount50KG)return false;
        for(std::size_t j=0;j<index;++j)if(binding.archive==garageSources[j].archive&&binding.memberIndex==garageSources[j].memberIndex)return false;
        if(!hexHash(c.converterSha256)||!hexHash(c.generatorSha256)||!hexHash(c.ffmpegSha256)||
           !c.ffmpegVersion||!c.ffmpegVersion[0]||!c.ffmpegCommand||!c.ffmpegCommand[0])return false;
        for(std::size_t i=0;i<a.mipCount;++i){const auto& m=a.sourceMips[i];const auto& r=a.retainedMips[i];
            if(m.width<16||m.height<16||m.width>1024||m.height>1024||(m.width&(m.width-1))||
               (m.height&(m.height-1))||!hexHash(m.sha256)||!hexHash(r.rrtexSha256)||!hexHash(r.rgbaSha256)||
               !m.filename||!m.filename[0])return false;
        }
    }
    return true;
}
// Retained payload only, not codec scratch, allocator overhead or GPU memory.
// The existing public Rider/standalone Garage APIs keep their16MiB ceiling.
// Only the private same-Frontend Garage append uses the complete authenticated
// corpus:30 Rider entries plus11 archive-qualified Garage entries. Recompute
// every mip and separate base RRTEX allocation, including its16-byte header.
bool addRetainedBytes(std::size_t& total,std::size_t bytes) noexcept {
    if(bytes>std::numeric_limits<std::size_t>::max()-total)return false;
    total+=bytes;return true;
}
bool sourceRetainedBytes(const SourceMipRecord* mips,std::size_t count,std::size_t& out) noexcept {
    out=0;if(!mips||!count||count>originalUiIpumMipCount50KG)return false;
    std::size_t base=0;
    for(std::size_t i=0;i<count;++i){
        const std::size_t width=mips[i].width,height=mips[i].height;
        if(!width||!height||width>std::numeric_limits<std::size_t>::max()/height)return false;
        const auto pixels=width*height;
        if(pixels>std::numeric_limits<std::size_t>::max()/4)return false;
        base=pixels*4;if(!addRetainedBytes(out,base))return false;
    }
    return addRetainedBytes(out,base)&&addRetainedBytes(out,16);
}
bool completeGarageCatalogRetainedLimit(std::size_t& out) noexcept {
    out=0;
    if(!catalogValid()||!additionalCatalogValid()||!garageCatalogValid()||
       additionalSourceCount!=29||garageSourceCount!=33)return false;
    std::size_t bytes=0;
    if(!sourceRetainedBytes(sourceMips,originalUiIpumMipCount50KG,bytes)||
       bytes!=retainedDecodedBytes||!addRetainedBytes(out,bytes))return false;
    for(const auto& record:additionalSources)
        if(!sourceRetainedBytes(record.sourceMips,record.mipCount,bytes)||!addRetainedBytes(out,bytes))return false;
    for(const auto& binding:garageSources){const auto& record=binding.source;
        if(!sourceRetainedBytes(record.sourceMips,record.mipCount,bytes)||!addRetainedBytes(out,bytes))return false;
    }
    // Exact current authored corpus, no rounded-up headroom or implicit growth.
    // 22 additional authenticated copies, each 611344 retained bytes.
    return out==std::size_t(33468400);
}
std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(std::uint16_t(p[0])|(std::uint16_t(p[1])<<8));
}
std::uint32_t u32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
}

// The repository's existing standard SHA-256 authentication algorithm, copied
// from original_ui_archive50kg.cpp. Fixed stack workspace; no codec behavior.
std::uint32_t rotateRight(std::uint32_t x,unsigned n) noexcept {
    return (x>>n)|(x<<(32u-n));
}
void shaBlock(const std::uint8_t* block,std::array<std::uint32_t,8>& h) noexcept {
    constexpr std::uint32_t k[64]={
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    std::uint32_t w[64]{};
    for(unsigned i=0;i<16;++i)for(unsigned j=0;j<4;++j)w[i]=(w[i]<<8)|block[i*4+j];
    for(unsigned i=16;i<64;++i){
        const auto s0=rotateRight(w[i-15],7)^rotateRight(w[i-15],18)^(w[i-15]>>3);
        const auto s1=rotateRight(w[i-2],17)^rotateRight(w[i-2],19)^(w[i-2]>>10);
        w[i]=w[i-16]+s0+w[i-7]+s1;
    }
    auto a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],v=h[7];
    for(unsigned i=0;i<64;++i){
        const auto s1=rotateRight(e,6)^rotateRight(e,11)^rotateRight(e,25);
        const auto ch=(e&f)^(~e&g);
        const auto t1=v+s1+ch+k[i]+w[i];
        const auto s0=rotateRight(a,2)^rotateRight(a,13)^rotateRight(a,22);
        const auto maj=(a&b)^(a&c)^(b&c);
        const auto t2=s0+maj;
        v=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
    }
    h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=v;
}
enum class HashResult { match,mismatch,cancelled };
HashResult hashMatches(const std::uint8_t* data,std::size_t size,const char* expected,
    const DecodeHooks48C* hooks) noexcept {
    std::array<std::uint32_t,8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at=0;
    for(;size-at>=64;at+=64){
        if(at%65536==0&&cancelled(hooks))return HashResult::cancelled;
        shaBlock(data+at,h);
    }
    std::array<std::uint8_t,128> tail{};
    const std::size_t remainder=size-at;
    if(remainder)std::memcpy(tail.data(),data+at,remainder);
    tail[remainder]=0x80;
    const std::size_t padded=remainder<56?64:128;
    const std::uint64_t bits=static_cast<std::uint64_t>(size)*8u;
    for(unsigned i=0;i<8;++i)tail[padded-1-i]=static_cast<std::uint8_t>(bits>>(i*8));
    shaBlock(tail.data(),h);if(padded==128)shaBlock(tail.data()+64,h);
    if(cancelled(hooks))return HashResult::cancelled;
    constexpr char hex[]="0123456789abcdef";
    for(unsigned i=0;i<64;++i)
        if(expected[i]!=hex[(h[i/8]>>((7u-i%8)*4))&15u])return HashResult::mismatch;
    return HashResult::match;
}
bool checkedHash(const std::uint8_t* data,std::size_t bytes,const char* expected,
    const DecodeHooks48C* hooks,OriginalUiIpumCacheDiagnostic50KG& d,Error error,std::size_t mip=0) noexcept {
    const auto result=hashMatches(data,bytes,expected,hooks);
    if(result==HashResult::cancelled)return fail(d,Error::cancelled,mip);
    return result==HashResult::match?true:fail(d,error,mip);
}
// One rule for every rider page, decided by the texture's own stored format:
// a member whose PST kind (u32 at +4) is not an IPU page (2) holds no IPU
// image, and the ordinary PST path reads it. Every stock source is kind 2, so
// stock bytes keep the exact IPU admission below. Read-only, bounds-checked.
bool notIpuPage(const std::shared_ptr<const OriginalUiArchive50KG>& archive,const char* member) { // callers hold the try
    if(!archive||!archive->loaded()||!member)return false;
    const auto view=archive->find(member);
    return view.data&&view.size>=8&&u32(view.data+4)!=2u;
}
bool decodeOriginalMips(const std::shared_ptr<const OriginalUiArchive50KG>& archive,
    const char* member,std::size_t expectedBytes,const char* expectedSha,
    const SourceMipRecord* mips,std::size_t count,
    std::vector<std::vector<std::uint8_t>>& images,OriginalUiIpumCacheDiagnostic50KG& d,
    const DecodeHooks48C* hooks,const TextureLimits48E& limits,
    OriginalUiArchiveId50KG archiveId=OriginalUiArchiveId50KG::rider,const char* expectedArchiveSha=archiveSha256) {
    if(!archive||!archive->loaded()||
       archive->identity()!=originalUiArchiveIdentity50KG(archiveId)||
       std::strcmp(archive->identity()->sha256,expectedArchiveSha)!=0)return fail(d,Error::archive);
    const auto source=archive->find(member);
    if(!source.identity||!source.data||source.size!=expectedBytes||
       source.identity->bytes!=expectedBytes||std::strcmp(source.identity->sha256,expectedSha)!=0)
        return fail(d,Error::source);
    if(expectedBytes>std::min(limits.input_bytes,std::size_t(8u*1024u*1024u))||
       count==0||count>originalUiIpumMipCount50KG)return fail(d,Error::limit);
    if(!checkedHash(source.data,source.size,expectedSha,hooks,d,Error::source))return false;
    std::size_t total=0;images.reserve(count);
    for(std::size_t i=0;i<count;++i){
        const auto& mip=mips[i];
        if(mip.payload>source.size||mip.bytes>source.size-mip.payload)
            return fail(d,Error::sourceLayout,i);
        if(!checkedHash(source.data+mip.payload,mip.bytes,mip.sha256,hooks,d,Error::encodedIdentity,i))return false;
        const auto bytes=std::size_t(mip.width)*mip.height*4u;
        const auto budget=std::min(limits.decoded_bytes,std::size_t(16u*1024u*1024u));
        if(total>budget||bytes>budget-total)return fail(d,Error::limit,i);
        total+=bytes;
        std::vector<std::uint8_t> rgba;
        if(!decodeOriginalIpuRgba(source.data+mip.payload,mip.bytes,mip.width,mip.height,rgba,d.detail))
            return fail(d,Error::codec,i);
        if(cancelled(hooks))return fail(d,Error::cancelled,i);
        if(rgba.size()!=bytes)return fail(d,Error::imageFormat,i);
        std::vector<std::uint8_t> rrtex{'R','R','T','E','X','0','0','1'};
        for(const auto dimension:{mip.width,mip.height})
            for(unsigned lane=0;lane<4;++lane)rrtex.push_back(static_cast<std::uint8_t>(dimension>>(lane*8)));
        rrtex.insert(rrtex.end(),rgba.begin(),rgba.end());images.push_back(std::move(rrtex));
    }
    return true;
}
} // namespace

struct OriginalUiIpumTextureCache50KG::Storage {
    std::shared_ptr<const OriginalUiArchive50KG> archive;
    OriginalUiMemberView50KG source{};
    Texture48E texture;
    OriginalUiIpumProvenance50KG evidence;
    std::size_t decodedBytes=0;
};
OriginalUiIpumTextureCache50KG::OriginalUiIpumTextureCache50KG() noexcept=default;
OriginalUiIpumTextureCache50KG::~OriginalUiIpumTextureCache50KG()=default;
bool OriginalUiIpumTextureCache50KG::catalogAvailable() noexcept{return catalogValid();}
const char* OriginalUiIpumTextureCache50KG::retainedMipFilename(std::size_t mip) noexcept {
    return mip<originalUiIpumMipCount50KG?sourceMips[mip].filename:nullptr;
}
bool OriginalUiIpumTextureCache50KG::loadOriginal(std::shared_ptr<const OriginalUiArchive50KG> archive,
    OriginalUiIpumCacheDiagnostic50KG& d,const DecodeHooks48C* hooks,const TextureLimits48E& limits) noexcept {
    d={};
    try {
        if(storage_)return fail(d,Error::alreadyLoaded);
        if(!catalogValid())return fail(d,Error::cacheUnavailable);
        // Same general rule as every additional rider page (no slot-19 contract).
        if(notIpuPage(archive,sourceMember)){d={};return true;}
        std::vector<std::vector<std::uint8_t>> images;
        if(!decodeOriginalMips(archive,sourceMember,sourceBytes,sourceSha256,sourceMips,
            originalUiIpumMipCount50KG,images,d,hooks,limits)||!load(std::move(archive),images,d,hooks,limits))return false;
        storage_->evidence.decodedInProcess=true;return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiIpumTextureCache50KG::loadAdditionalOriginal(std::size_t sourceIndex,
    std::shared_ptr<const OriginalUiArchive50KG> archive,OriginalUiIpumCacheDiagnostic50KG& d,
    const DecodeHooks48C* hooks,const TextureLimits48E& limits) noexcept {
    d={};
    try {
        if(!additionalCatalogValid()||sourceIndex>=additionalSourceCount)return fail(d,Error::cacheUnavailable);
        if(additional_[sourceIndex])return fail(d,Error::alreadyLoaded);
        const auto& record=additionalSources[sourceIndex];
        if(notIpuPage(archive,record.member)){d={};return true;} // general rule, see notIpuPage
        std::vector<std::vector<std::uint8_t>> images;
        if(!decodeOriginalMips(archive,record.member,record.sourceBytes,record.sourceSha256,
            record.sourceMips,record.mipCount,images,d,hooks,limits)||
            !loadAdditional(sourceIndex,std::move(archive),images,d,hooks,limits))return false;
        additional_[sourceIndex]->evidence.decodedInProcess=true;return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiIpumTextureCache50KG::load(std::shared_ptr<const OriginalUiArchive50KG> archive,
    const std::vector<std::vector<std::uint8_t>>& retained,OriginalUiIpumCacheDiagnostic50KG& d,
    const DecodeHooks48C* hooks,const TextureLimits48E& limits) noexcept {
    d={};
    try {
        if(storage_)return fail(d,Error::alreadyLoaded);
        if(cancelled(hooks))return fail(d,Error::cancelled);
        if(!catalogValid())return fail(d,Error::cacheUnavailable);
        if(notIpuPage(archive,sourceMember)){d={};return true;} // general rule, see notIpuPage
        if(!archive||!archive->loaded()||
            archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)||
            std::strcmp(archive->identity()->sha256,archiveSha256)!=0)return fail(d,Error::archive);
        const auto source=archive->find(sourceMember);
        if(!source.identity||!source.data||source.size!=sourceBytes||
            source.identity->bytes!=sourceBytes||
            std::strcmp(source.identity->sha256,sourceSha256)!=0)return fail(d,Error::source);
        std::size_t totalDecodedBytes=retainedDecodedBytes;
        if(storage_)totalDecodedBytes+=storage_->decodedBytes;
        for(const auto& existing:additional_)if(existing)totalDecodedBytes+=existing->decodedBytes;
        for(const auto& existing:garage_)if(existing)totalDecodedBytes+=existing->decodedBytes;
        if(sourceBytes>std::min(limits.input_bytes,std::size_t(8u*1024u*1024u))||
            totalDecodedBytes>std::min(limits.decoded_bytes,std::size_t(16u*1024u*1024u)))
            return fail(d,Error::limit);
        if(retained.size()!=originalUiIpumMipCount50KG)return fail(d,Error::imageCount);
        if(!checkedHash(source.data,source.size,sourceSha256,hooks,d,Error::source))return false;
        if(u16(source.data)!=1||u16(source.data+2)!=0||u32(source.data+4)!=2||
            u32(source.data+8)!=originalUiIpumMipCount50KG)return fail(d,Error::sourceLayout);
        // Validate every source extent and every retained output before any
        // decoded pixel allocation/publication. Constants are exact originals.
        constexpr std::uint8_t terminators[]{0,0,1,0xb0,0,0,1,0xb1};
        for(std::size_t i=0;i<originalUiIpumMipCount50KG;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            if(s.header>source.size||20>source.size-s.header||s.payload!=s.header+20||
                s.payload>source.size||s.bytes>source.size-s.payload||s.bytes<25||
                (i==0?s.header!=12:s.header!=sourceMips[i-1].payload+sourceMips[i-1].bytes)||
                (i+1==originalUiIpumMipCount50KG&&s.payload+s.bytes!=source.size))
                return fail(d,Error::sourceLayout,i);
            const auto* h=source.data+s.header;const auto* p=source.data+s.payload;
            if(u32(h)!=s.height||u32(h+4)!=s.width||u32(h+8)||u32(h+12)||u32(h+16)!=s.bytes||
                std::memcmp(p,"ipum",4)||u32(p+4)!=s.bytes-8||u16(p+8)!=s.width||
                u16(p+10)!=s.height||u32(p+12)!=1||p[16]!=0||
                std::memcmp(p+s.bytes-8,terminators,8))return fail(d,Error::sourceLayout,i);
            if(!checkedHash(p,s.bytes,s.sha256,hooks,d,Error::encodedIdentity,i))return false;
            const std::size_t rgbaBytes=std::size_t(s.width)*s.height*4;
            if(image.size()!=rgbaBytes+16)return fail(d,Error::imageFormat,i);
            if(!checkedHash(image.data(),image.size(),r.rrtexSha256,hooks,d,Error::imageIdentity,i)||
                !checkedHash(image.data()+16,rgbaBytes,r.rgbaSha256,hooks,d,Error::imageIdentity,i))return false;
            if(std::memcmp(image.data(),"RRTEX001",8)||u32(image.data()+8)!=s.width||
                u32(image.data()+12)!=s.height)return fail(d,Error::imageFormat,i);
            for(std::size_t pixelAt=16;pixelAt<image.size();pixelAt+=4){
                if((pixelAt-16)%65536==0&&cancelled(hooks))return fail(d,Error::cancelled,i);
                if((image[pixelAt]&7u)||(image[pixelAt+1]&7u)||(image[pixelAt+2]&7u)||
                    image[pixelAt+3]!=(image[pixelAt]||image[pixelAt+1]||image[pixelAt+2]?255:0))
                    return fail(d,Error::imageFormat,i);
            }
        }
        auto next=std::make_unique<Storage>();next->archive=std::move(archive);next->source=source;
        next->decodedBytes=retainedDecodedBytes;
        auto& texture=next->texture;texture.revision=1;texture.flags=0;texture.kind=2;
        // No authored palette exists for kind2; zero storage is not a palette.
        texture.mips.reserve(originalUiIpumMipCount50KG);
        auto& evidence=next->evidence;
        evidence.mipCount=originalUiIpumMipCount50KG;
        evidence.colorPolicyKind=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1;
        evidence.sourceMember=sourceMember;
        evidence.sourcePstSha256=sourceSha256;evidence.sourceArchiveSha256=archiveSha256;
        evidence.converterSha256=retainedCodecRecord.converterSha256;
        evidence.generatorSha256=retainedCodecRecord.generatorSha256;
        evidence.ffmpegVersion=retainedCodecRecord.ffmpegVersion;
        evidence.ffmpegSha256=retainedCodecRecord.ffmpegSha256;
        evidence.ffmpegCommand=retainedCodecRecord.ffmpegCommand;evidence.colorPolicy=colorPolicyDescription;
        if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",0,originalUiIpumMipCount50KG);
        for(std::size_t i=0;i<originalUiIpumMipCount50KG;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            TextureMip48E mip;mip.width=s.width;mip.height=s.height;
            mip.source_header_offset=s.header;mip.source_indices_offset=s.payload;
            mip.rgba.assign(image.begin()+16,image.end());texture.mips.push_back(std::move(mip));
            auto& e=evidence.mips[i];e.width=s.width;e.height=s.height;e.sourceHeaderOffset=s.header;
            e.sourcePayloadOffset=s.payload;e.sourcePayloadBytes=s.bytes;e.sourcePayloadSha256=s.sha256;
            e.rrtexSha256=r.rrtexSha256;e.rgbaSha256=r.rgbaSha256;e.yuv420pSha256=r.yuv420pSha256;
            e.retainedFilename=s.filename;
            if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",
                static_cast<std::uint32_t>(i+1),originalUiIpumMipCount50KG);
        }
        texture.base_rrtex=retained.back();
        if(cancelled(hooks))return fail(d,Error::cancelled,originalUiIpumMipCount50KG-1);
        storage_=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiIpumTextureCache50KG::loadAdditional(std::size_t sourceIndex,std::shared_ptr<const OriginalUiArchive50KG> archive,
    const std::vector<std::vector<std::uint8_t>>& retained,OriginalUiIpumCacheDiagnostic50KG& d,
    const DecodeHooks48C* hooks,const TextureLimits48E& limits) noexcept {
    d={};
    try {
        if(!additionalCatalogValid()||sourceIndex>=additionalSourceCount)return fail(d,Error::cacheUnavailable);
        if(additional_[sourceIndex])return fail(d,Error::alreadyLoaded);
        const auto& record=additionalSources[sourceIndex];
        if(notIpuPage(archive,record.member)){d={};return true;} // Not an IPU page: nothing to retain.
        const auto* sourceMember=record.member;const auto* sourceSha256=record.sourceSha256;
        const auto sourceBytes=record.sourceBytes;const auto mipCount=record.mipCount;
        const auto& sourceMips=record.sourceMips;const auto& retainedMipRecords=record.retainedMips;
        const auto& retainedCodecRecord=record.codec;
        std::size_t retainedDecodedBytes=0;
        for(std::size_t i=0;i<mipCount;++i){
            const auto pixels=std::size_t(sourceMips[i].width)*sourceMips[i].height;
            retainedDecodedBytes+=pixels*4;
        }
        // Mip RGBA plus independently retained full base RRTEX allocation.
        retainedDecodedBytes+=std::size_t(sourceMips[mipCount-1].width)*sourceMips[mipCount-1].height*4+16;
        if(cancelled(hooks))return fail(d,Error::cancelled);
        if(!archive||!archive->loaded()||
            archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)||
            std::strcmp(archive->identity()->sha256,archiveSha256)!=0)return fail(d,Error::archive);
        const auto source=archive->find(sourceMember);
        if(!source.identity||!source.data||source.size!=sourceBytes||
            source.identity->bytes!=sourceBytes||
            std::strcmp(source.identity->sha256,sourceSha256)!=0)return fail(d,Error::source);
        std::size_t totalDecodedBytes=retainedDecodedBytes;
        if(storage_)totalDecodedBytes+=storage_->decodedBytes;
        for(const auto& existing:additional_)if(existing)totalDecodedBytes+=existing->decodedBytes;
        for(const auto& existing:garage_)if(existing)totalDecodedBytes+=existing->decodedBytes;
        if(sourceBytes>std::min(limits.input_bytes,std::size_t(8u*1024u*1024u))||
            totalDecodedBytes>std::min(limits.decoded_bytes,std::size_t(16u*1024u*1024u)))
            return fail(d,Error::limit);
        if(retained.size()!=mipCount)return fail(d,Error::imageCount);
        if(!checkedHash(source.data,source.size,sourceSha256,hooks,d,Error::source))return false;
        if(u16(source.data)!=1||u16(source.data+2)!=0||u32(source.data+4)!=2||
            u32(source.data+8)!=mipCount)return fail(d,Error::sourceLayout);
        // Validate every source extent and every retained output before any
        // decoded pixel allocation/publication. Constants are exact originals.
        constexpr std::uint8_t terminators[]{0,0,1,0xb0,0,0,1,0xb1};
        for(std::size_t i=0;i<mipCount;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            if(s.header>source.size||20>source.size-s.header||s.payload!=s.header+20||
                s.payload>source.size||s.bytes>source.size-s.payload||s.bytes<25||
                (i==0?s.header!=12:s.header!=sourceMips[i-1].payload+sourceMips[i-1].bytes)||
                (i+1==mipCount&&s.payload+s.bytes!=source.size))
                return fail(d,Error::sourceLayout,i);
            const auto* h=source.data+s.header;const auto* p=source.data+s.payload;
            if(u32(h)!=s.height||u32(h+4)!=s.width||u32(h+8)||u32(h+12)||u32(h+16)!=s.bytes||
                std::memcmp(p,"ipum",4)||u32(p+4)!=s.bytes-8||u16(p+8)!=s.width||
                u16(p+10)!=s.height||u32(p+12)!=1||p[16]!=0||
                std::memcmp(p+s.bytes-8,terminators,8))return fail(d,Error::sourceLayout,i);
            if(!checkedHash(p,s.bytes,s.sha256,hooks,d,Error::encodedIdentity,i))return false;
            const std::size_t rgbaBytes=std::size_t(s.width)*s.height*4;
            if(image.size()!=rgbaBytes+16)return fail(d,Error::imageFormat,i);
            if(!checkedHash(image.data(),image.size(),r.rrtexSha256,hooks,d,Error::imageIdentity,i)||
                !checkedHash(image.data()+16,rgbaBytes,r.rgbaSha256,hooks,d,Error::imageIdentity,i))return false;
            if(std::memcmp(image.data(),"RRTEX001",8)||u32(image.data()+8)!=s.width||
                u32(image.data()+12)!=s.height)return fail(d,Error::imageFormat,i);
            for(std::size_t pixelAt=16;pixelAt<image.size();pixelAt+=4){
                if((pixelAt-16)%65536==0&&cancelled(hooks))return fail(d,Error::cancelled,i);
                if((image[pixelAt]&7u)||(image[pixelAt+1]&7u)||(image[pixelAt+2]&7u)||
                    image[pixelAt+3]!=(image[pixelAt]||image[pixelAt+1]||image[pixelAt+2]?255:0))
                    return fail(d,Error::imageFormat,i);
            }
        }
        auto next=std::make_unique<Storage>();next->archive=std::move(archive);next->source=source;
        next->decodedBytes=retainedDecodedBytes;
        auto& texture=next->texture;texture.revision=1;texture.flags=0;texture.kind=2;
        // No authored palette exists for kind2; zero storage is not a palette.
        texture.mips.reserve(mipCount);
        auto& evidence=next->evidence;
        evidence.mipCount=mipCount;
        evidence.colorPolicyKind=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1;
        evidence.sourceMember=sourceMember;
        evidence.sourcePstSha256=sourceSha256;evidence.sourceArchiveSha256=archiveSha256;
        evidence.converterSha256=retainedCodecRecord.converterSha256;
        evidence.generatorSha256=retainedCodecRecord.generatorSha256;
        evidence.ffmpegVersion=retainedCodecRecord.ffmpegVersion;
        evidence.ffmpegSha256=retainedCodecRecord.ffmpegSha256;
        evidence.ffmpegCommand=retainedCodecRecord.ffmpegCommand;evidence.colorPolicy=colorPolicyDescription;
        if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",0,mipCount);
        for(std::size_t i=0;i<mipCount;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            TextureMip48E mip;mip.width=s.width;mip.height=s.height;
            mip.source_header_offset=s.header;mip.source_indices_offset=s.payload;
            mip.rgba.assign(image.begin()+16,image.end());texture.mips.push_back(std::move(mip));
            auto& e=evidence.mips[i];e.width=s.width;e.height=s.height;e.sourceHeaderOffset=s.header;
            e.sourcePayloadOffset=s.payload;e.sourcePayloadBytes=s.bytes;e.sourcePayloadSha256=s.sha256;
            e.rrtexSha256=r.rrtexSha256;e.rgbaSha256=r.rgbaSha256;e.yuv420pSha256=r.yuv420pSha256;
            e.retainedFilename=s.filename;
            if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",
                static_cast<std::uint32_t>(i+1),mipCount);
        }
        texture.base_rrtex=retained.back();
        if(cancelled(hooks))return fail(d,Error::cancelled,mipCount-1);
        additional_[sourceIndex]=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiIpumTextureCache50KG::appendGarageArchive12CEA0(
    const std::shared_ptr<const OriginalUiArchive50KG>& archive,OriginalUiIpumCacheDiagnostic50KG& d)const noexcept {
    d={};
    try {
    if(!archive||!archive->loaded()||!garageCatalogValid())return fail(d,Error::archive);
    std::size_t retainedLimit=0;
    if(!completeGarageCatalogRetainedLimit(retainedLimit))return fail(d,Error::cacheUnavailable);
    const TextureLimits48E nativeLimits; // Per-source decoder limits stay unchanged.
    bool found=false;
    for(std::size_t i=0;i<garageSourceCount;++i){
        const auto& binding=garageSources[i];
        if(archive->identity()!=originalUiArchiveIdentity50KG(binding.archive))continue;
        found=true;d={};d.sourceIndex=i;const auto& existing=garage_[i];
        if(existing){
            const auto source=archive->find(binding.source.member);
            // Reentry retains the genuine original immutable publication; no
            // replacement pixels or rebinding to a same-name allocation.
            if(existing->archive!=archive||source.identity!=existing->source.identity||
               source.data!=existing->source.data||source.size!=existing->source.size||
               !existing->evidence.decodedInProcess)
                return fail(d,Error::source);
            continue;
        }
        if(!loadGarageOriginalImpl(i,archive,d,nullptr,nativeLimits,retainedLimit))return false;
    }
    return found?true:fail(d,Error::archive);
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiIpumTextureCache50KG::appendUnlockArchive20A850(
    const std::shared_ptr<const OriginalUiArchive50KG>& archive,OriginalUiIpumCacheDiagnostic50KG& d)const noexcept {
    d={};
    try {
    if(!archive||!archive->loaded()||archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::unlock)||
       !additionalCatalogValid())return fail(d,Error::archive);
    std::size_t retainedLimit=0,bytes=0;
    if(!completeGarageCatalogRetainedLimit(retainedLimit))return fail(d,Error::cacheUnavailable);
    for(const auto& record:additionalSources)
        if(!sourceRetainedBytes(record.sourceMips,record.mipCount,bytes)||!addRetainedBytes(retainedLimit,bytes))
            return fail(d,Error::limit);
    const TextureLimits48E nativeLimits; // Per-source decoder limits stay unchanged.
    for(std::size_t i=0;i<additionalSourceCount;++i){
        const auto& record=additionalSources[i];
        const auto source=archive->find(record.member);
        // Only members UIUNLOCK actually holds with the Rider record's exact bytes.
        if(!source.identity||source.identity->bytes!=record.sourceBytes||
           std::strcmp(source.identity->sha256,record.sourceSha256))continue;
        d={};d.sourceIndex=i;
        if(const auto& existing=unlock_[i]){
            if(existing->archive!=archive||source.identity!=existing->source.identity||
               source.data!=existing->source.data||source.size!=existing->source.size||
               !existing->evidence.decodedInProcess)return fail(d,Error::source);
            continue;
        }
        if(!loadGarageOriginalImpl(i,archive,d,nullptr,nativeLimits,retainedLimit,true))return false;
    }
    d={};return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiIpumTextureCache50KG::loadGarageOriginal(std::size_t sourceIndex,std::shared_ptr<const OriginalUiArchive50KG> archive,
    OriginalUiIpumCacheDiagnostic50KG& d,
    const DecodeHooks48C* hooks,const TextureLimits48E& limits) noexcept {
    return loadGarageOriginalImpl(sourceIndex,std::move(archive),d,hooks,limits,
        std::min(limits.decoded_bytes,std::size_t(16u*1024u*1024u)));
}
bool OriginalUiIpumTextureCache50KG::loadGarageOriginalImpl(std::size_t sourceIndex,std::shared_ptr<const OriginalUiArchive50KG> archive,
    OriginalUiIpumCacheDiagnostic50KG& d,
    const DecodeHooks48C* hooks,const TextureLimits48E& limits,std::size_t retainedByteCeiling,bool unlock) const noexcept {
    d={};d.sourceIndex=sourceIndex;
    try {
        GarageSourceRecord binding{};
        if(unlock){
            // The UIUNLOCK member of the same name, required byte-identical to
            // the Rider record (name, size and SHA-256 of the catalog member).
            if(!additionalCatalogValid()||sourceIndex>=additionalSourceCount)return fail(d,Error::cacheUnavailable);
            if(unlock_[sourceIndex])return fail(d,Error::alreadyLoaded);
            const auto* unlockArchive=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::unlock);
            if(!unlockArchive||!archive||!archive->loaded()||archive->identity()!=unlockArchive)return fail(d,Error::archive);
            const auto& record=additionalSources[sourceIndex];
            std::size_t index=archive->memberCount();
            for(std::size_t i=0;i<archive->memberCount();++i){const auto view=archive->member(i);
                if(view.identity&&!std::strcmp(view.identity->name,record.member)){index=i;break;}}
            const auto* member=index<archive->memberCount()?originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::unlock,index):nullptr;
            if(!member||member->bytes!=record.sourceBytes||std::strcmp(member->sha256,record.sourceSha256))return fail(d,Error::source);
            binding.archive=OriginalUiArchiveId50KG::unlock;binding.memberIndex=index;
            binding.archiveSha=unlockArchive->sha256;binding.source=record;
        }else{
            if(!garageCatalogValid()||sourceIndex>=garageSourceCount)return fail(d,Error::cacheUnavailable);
            if(garage_[sourceIndex])return fail(d,Error::alreadyLoaded);
            binding=garageSources[sourceIndex];
        }
        const auto& record=binding.source;
        if(!archive||!archive->loaded()||archive->identity()!=originalUiArchiveIdentity50KG(binding.archive)||
           archive->member(binding.memberIndex).identity!=originalUiMemberIdentity50KG(binding.archive,binding.memberIndex))return fail(d,Error::archive);
        std::vector<std::vector<std::uint8_t>> retained;
        const auto* sourceMember=record.member;const auto* sourceSha256=record.sourceSha256;
        const auto sourceBytes=record.sourceBytes;const auto mipCount=record.mipCount;
        const auto& sourceMips=record.sourceMips;const auto& retainedMipRecords=record.retainedMips;
        const auto& retainedCodecRecord=record.codec;
        std::size_t retainedDecodedBytes=0;
        if(!sourceRetainedBytes(sourceMips,mipCount,retainedDecodedBytes))return fail(d,Error::limit);
        if(cancelled(hooks))return fail(d,Error::cancelled);
        if(!archive||!archive->loaded()||
            archive->identity()!=originalUiArchiveIdentity50KG(binding.archive)||
            std::strcmp(archive->identity()->sha256,binding.archiveSha)!=0)return fail(d,Error::archive);
        const auto source=archive->find(sourceMember);
        if(!source.identity||!source.data||source.size!=sourceBytes||
            source.identity->bytes!=sourceBytes||
            std::strcmp(source.identity->sha256,sourceSha256)!=0)return fail(d,Error::source);
        d.inputBytes=sourceBytes;
        d.inputByteLimit=std::min(limits.input_bytes,std::size_t(8u*1024u*1024u));
        d.requestedBytes=retainedDecodedBytes;
        // Aggregate retained cache capacity is distinct from per-source decode bounds.
        d.decodedByteLimit=retainedByteCeiling;
        if(storage_&&!addRetainedBytes(d.retainedBytes,storage_->decodedBytes))return fail(d,Error::limit);
        for(const auto& existing:additional_)
            if(existing&&!addRetainedBytes(d.retainedBytes,existing->decodedBytes))return fail(d,Error::limit);
        for(const auto& existing:garage_)
            if(existing&&!addRetainedBytes(d.retainedBytes,existing->decodedBytes))return fail(d,Error::limit);
        // The Unlock copies have their own budget (the caller's ceiling adds
        // the whole Rider additional corpus); the Garage corpus proof is unchanged.
        if(unlock)for(const auto& existing:unlock_)
            if(existing&&!addRetainedBytes(d.retainedBytes,existing->decodedBytes))return fail(d,Error::limit);
        std::size_t totalDecodedBytes=d.retainedBytes;
        if(!addRetainedBytes(totalDecodedBytes,retainedDecodedBytes)||
           sourceBytes>d.inputByteLimit||totalDecodedBytes>d.decodedByteLimit)
            return fail(d,Error::limit);
        if(!decodeOriginalMips(archive,record.member,record.sourceBytes,record.sourceSha256,record.sourceMips,
            record.mipCount,retained,d,hooks,limits,binding.archive,binding.archiveSha))return false;
        if(retained.size()!=mipCount)return fail(d,Error::imageCount);
        if(!checkedHash(source.data,source.size,sourceSha256,hooks,d,Error::source))return false;
        if(u16(source.data)!=1||u16(source.data+2)!=0||u32(source.data+4)!=2||
            u32(source.data+8)!=mipCount)return fail(d,Error::sourceLayout);
        // Validate every source extent and every retained output before any
        // decoded pixel allocation/publication. Constants are exact originals.
        constexpr std::uint8_t terminators[]{0,0,1,0xb0,0,0,1,0xb1};
        for(std::size_t i=0;i<mipCount;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            if(s.header>source.size||20>source.size-s.header||s.payload!=s.header+20||
                s.payload>source.size||s.bytes>source.size-s.payload||s.bytes<25||
                (i==0?s.header!=12:s.header!=sourceMips[i-1].payload+sourceMips[i-1].bytes)||
                (i+1==mipCount&&s.payload+s.bytes!=source.size))
                return fail(d,Error::sourceLayout,i);
            const auto* h=source.data+s.header;const auto* p=source.data+s.payload;
            if(u32(h)!=s.height||u32(h+4)!=s.width||u32(h+8)||u32(h+12)||u32(h+16)!=s.bytes||
                std::memcmp(p,"ipum",4)||u32(p+4)!=s.bytes-8||u16(p+8)!=s.width||
                u16(p+10)!=s.height||u32(p+12)!=1||p[16]!=0||
                std::memcmp(p+s.bytes-8,terminators,8))return fail(d,Error::sourceLayout,i);
            if(!checkedHash(p,s.bytes,s.sha256,hooks,d,Error::encodedIdentity,i))return false;
            const std::size_t rgbaBytes=std::size_t(s.width)*s.height*4;
            if(image.size()!=rgbaBytes+16)return fail(d,Error::imageFormat,i);
            if(!checkedHash(image.data(),image.size(),r.rrtexSha256,hooks,d,Error::imageIdentity,i)||
                !checkedHash(image.data()+16,rgbaBytes,r.rgbaSha256,hooks,d,Error::imageIdentity,i))return false;
            if(std::memcmp(image.data(),"RRTEX001",8)||u32(image.data()+8)!=s.width||
                u32(image.data()+12)!=s.height)return fail(d,Error::imageFormat,i);
            for(std::size_t pixelAt=16;pixelAt<image.size();pixelAt+=4){
                if((pixelAt-16)%65536==0&&cancelled(hooks))return fail(d,Error::cancelled,i);
                if((image[pixelAt]&7u)||(image[pixelAt+1]&7u)||(image[pixelAt+2]&7u)||
                    image[pixelAt+3]!=(image[pixelAt]||image[pixelAt+1]||image[pixelAt+2]?255:0))
                    return fail(d,Error::imageFormat,i);
            }
        }
        auto next=std::make_unique<Storage>();next->archive=std::move(archive);next->source=source;
        next->decodedBytes=retainedDecodedBytes;
        auto& texture=next->texture;texture.revision=1;texture.flags=0;texture.kind=2;
        // No authored palette exists for kind2; zero storage is not a palette.
        texture.mips.reserve(mipCount);
        auto& evidence=next->evidence;
        evidence.mipCount=mipCount;
        evidence.colorPolicyKind=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1;
        evidence.sourceMember=sourceMember;
        evidence.sourcePstSha256=sourceSha256;evidence.sourceArchiveSha256=binding.archiveSha;
        evidence.decodedInProcess=true;
        evidence.converterSha256=retainedCodecRecord.converterSha256;
        evidence.generatorSha256=retainedCodecRecord.generatorSha256;
        evidence.ffmpegVersion=retainedCodecRecord.ffmpegVersion;
        evidence.ffmpegSha256=retainedCodecRecord.ffmpegSha256;
        evidence.ffmpegCommand=retainedCodecRecord.ffmpegCommand;evidence.colorPolicy=colorPolicyDescription;
        if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",0,mipCount);
        for(std::size_t i=0;i<mipCount;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            TextureMip48E mip;mip.width=s.width;mip.height=s.height;
            mip.source_header_offset=s.header;mip.source_indices_offset=s.payload;
            mip.rgba.assign(image.begin()+16,image.end());texture.mips.push_back(std::move(mip));
            auto& e=evidence.mips[i];e.width=s.width;e.height=s.height;e.sourceHeaderOffset=s.header;
            e.sourcePayloadOffset=s.payload;e.sourcePayloadBytes=s.bytes;e.sourcePayloadSha256=s.sha256;
            e.rrtexSha256=r.rrtexSha256;e.rgbaSha256=r.rgbaSha256;e.yuv420pSha256=r.yuv420pSha256;
            e.retainedFilename=s.filename;
            if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",
                static_cast<std::uint32_t>(i+1),mipCount);
        }
        texture.base_rrtex=retained.back();
        if(cancelled(hooks))return fail(d,Error::cancelled,mipCount-1);
        (unlock?unlock_[sourceIndex]:garage_[sourceIndex])=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiIpumTextureCache50KG::loaded() const noexcept {
    if(storage_)return true;
    for(const auto& entry:additional_)if(entry)return true;
    for(const auto& entry:garage_)if(entry)return true;
    for(const auto& entry:unlock_)if(entry)return true;
    return false;
}
OriginalUiIpumColorPolicy50KG OriginalUiIpumTextureCache50KG::colorPolicy() const noexcept {
    return loaded()?OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1:
        OriginalUiIpumColorPolicy50KG::unavailable;
}
const Texture48E* OriginalUiIpumTextureCache50KG::textureFor(const OriginalUiMemberView50KG& source,
    std::size_t offset,std::size_t bytes) const noexcept {
    if(offset)return nullptr;
    const auto matches=[&](const Storage* entry){return entry&&bytes==entry->source.size&&
        source.identity==entry->source.identity&&source.data==entry->source.data&&source.size==entry->source.size;};
    if(matches(storage_.get()))return &storage_->texture;
    for(const auto& entry:additional_)if(matches(entry.get()))return &entry->texture;
    for(const auto& entry:garage_)if(matches(entry.get()))return &entry->texture;
    for(const auto& entry:unlock_)if(matches(entry.get()))return &entry->texture;
    return nullptr;
}
const OriginalUiIpumProvenance50KG* OriginalUiIpumTextureCache50KG::provenance() const noexcept {
    return storage_?&storage_->evidence:nullptr;
}
std::size_t OriginalUiIpumTextureCache50KG::garageCatalogSourceCount() noexcept {
    return garageCatalogValid()?garageSourceCount:0;
}
const char* OriginalUiIpumTextureCache50KG::garageSourceMember(std::size_t source) noexcept {
    return source<garageCatalogSourceCount()?garageSources[source].source.member:nullptr;
}
const OriginalUiArchiveIdentity50KG* OriginalUiIpumTextureCache50KG::garageSourceArchive(std::size_t source) noexcept {
    return source<garageCatalogSourceCount()?originalUiArchiveIdentity50KG(garageSources[source].archive):nullptr;
}
std::size_t OriginalUiIpumTextureCache50KG::additionalCatalogSourceCount() noexcept {
    return additionalCatalogValid()?additionalSourceCount:0;
}
const char* OriginalUiIpumTextureCache50KG::additionalSourceMember(std::size_t source) noexcept {
    return source<additionalCatalogSourceCount()?additionalSources[source].member:nullptr;
}
std::size_t OriginalUiIpumTextureCache50KG::additionalMipCount(std::size_t source) noexcept {
    return source<additionalCatalogSourceCount()?additionalSources[source].mipCount:0;
}
const char* OriginalUiIpumTextureCache50KG::additionalRetainedMipFilename(std::size_t source,std::size_t mip) noexcept {
    return mip<additionalMipCount(source)?additionalSources[source].sourceMips[mip].filename:nullptr;
}
const OriginalUiIpumProvenance50KG* OriginalUiIpumTextureCache50KG::provenanceFor(
    const OriginalUiMemberView50KG& source,std::size_t offset,std::size_t bytes) const noexcept {
    if(offset)return nullptr;
    const auto matches=[&](const Storage* entry){return entry&&bytes==entry->source.size&&
        source.identity==entry->source.identity&&source.data==entry->source.data&&source.size==entry->source.size;};
    if(matches(storage_.get()))return &storage_->evidence;
    for(const auto& entry:additional_)if(matches(entry.get()))return &entry->evidence;
    for(const auto& entry:garage_)if(matches(entry.get()))return &entry->evidence;
    for(const auto& entry:unlock_)if(matches(entry.get()))return &entry->evidence;
    return nullptr;
}
} // namespace rr::assets
