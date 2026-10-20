#pragma once
#include "track_materials50fa_fixture.h"
#include "rc_cold_sources50la.h"
#include "../../src/assets/original_tga_conversion50la.h"
#include "../../src/game/original_track_environment50rc.h"
#include "material_alpha50gx_fixture.h"
#include "source_texture50fc_fixture.h"

#include "../../src/assets/import_world48f.h"
#include "../../src/assets/original_ipu_decoder.h"
#include "../../src/assets/original_standard_ipum_framing50la.h"
#include "../../src/assets/original_ui_atm50kg.h"
#include "../../src/assets/world_normals50hk.h"
#include <algorithm>
#include <charconv>
#include <iterator>
#include <limits>
#include <map>
#include <memory>

// Cold RC owner proposal; strict geometry acceptance remains independently gated.
namespace rc_original_world50LM {
namespace a=rr::assets;
namespace shared=pod_materials50ed_fixture::detail;
namespace tm=track_materials50fa_fixture;
using Bytes=std::vector<std::uint8_t>;
using Json=nlohmann::json;
inline constexpr const char* archiveSha="ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b";
inline constexpr const char* scbSha="1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a";
inline constexpr const char* scbMember="TRACKS/RC.SCB";
inline constexpr const char* archiveMember="TRACKS/RC.RES";
inline constexpr std::uint32_t archiveBytes=4655141u;
inline constexpr std::uint32_t decodedBytes=31703040u;
inline constexpr std::uint32_t entryCount=1121u;
inline constexpr std::uint32_t scbBytesCount=468918u;
inline constexpr std::uint32_t expectedInstances=1277u;
inline constexpr std::uint32_t expectedZones=15u;
inline constexpr std::uint32_t expectedPortals=15u;
inline constexpr std::uint32_t expectedScenes=912u;
inline constexpr std::uint32_t expectedPlacements=1170u;
inline constexpr std::uint32_t expectedCollisionOnly=107u;
inline constexpr std::uint32_t expectedSurfaces=1688u;
inline constexpr std::uint32_t expectedMaterials=43u;
inline constexpr std::uint32_t expectedVertices=271949u;
inline constexpr std::uint32_t expectedIndices=475035u;
inline constexpr std::uint32_t expectedRgbaBytes=3903488u;
inline constexpr std::uint32_t expectedPinnedImages=18u;
inline constexpr std::uint64_t assetIdentity=0x5243ull;
inline constexpr auto selectedTrack=track_source_fixture::Track::rc;
inline constexpr std::uint32_t expectedSelections=44,expectedImages=44,expectedSecondPasses=79;
struct CrystalSource {
 std::vector<rr::game::original_track_environment50RC::Vertex> vertices;
 float qPosition=0,qNormal=0;std::string member,sourceSha;
};
struct CrystalPlacement {
 std::uint32_t sourceIndex=0,firstSpan=0,vertexStart=0,vertexCount=0,indexStart=0,indexCount=0;
 std::array<double,16> matrix{};
};
namespace detail {
struct SourceImage {const char* member;const char* sourceSha256;std::uint32_t sourceBytes,width,height,mipIndex,kind,frameCount,initialFrame;};
inline constexpr SourceImage sourceImages[]={
{"TEXTURES/ARENATILE.PST","709a54f496ddfcdb54b00e786be35cc014f72a153da5ff8738d6203d72102f40",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/ARROWDROID01.TGA","06e5ceebbefa498778fabb8f975b919b77262f2c9d38b643df971d7ddc864faf",49196u,128u,128u,0u,3u,1u,0u},
{"TEXTURES/ARROWDROID03.TGA","efe13aee3c3ab36aeb8b9208fc0794d85b3056c77ff9017517c621aea8d64d15",49196u,128u,128u,0u,3u,1u,0u},
{"TEXTURES/ARROWRED.ATM","dbfdaf3d44918c86254ae9f39f4146cc101c363e1fb9e2cda43bb8c0ea99b220",114312u,128u,128u,3u,1u,5u,0u},
{"TEXTURES/BLOCKWALL.PST","21d5056d73e9f59d3a60005f5dbc77f5f07495ef2a0207bee61c2f398d93cc14",4437u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/BUILDING01.PST","5dff386796b0c7261e9e713abca4df0d5c8b4429f6a05f3bdffb0178d0871f52",3233u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/BULBBASE3.PST","4c3e9a7819e35290de58f7faa6f28a839be596689d9457387e0175db7978a3d5",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/CAMDROID03.PST","b7da07e4e9e3fe12fa47b9f9c06c5803534ae69c19d9904106060a84dbfa6710",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/CORD.PST","91cb74edd2d0cb4f3ca102abea2f06cc5e539f21f6b5fb42b54d42baafdc39a1",1233u,64u,64u,2u,2u,1u,0u},
{"TEXTURES/CROWD.ATM","054ce086e15886a331f0da2fb610479e665413c917a9f9ab634ab133fc582b38",49861u,128u,128u,3u,2u,4u,0u},
{"TEXTURES/CRYSTAL01.PST","1a9bca8cb30af8ee09c09b6c597b7fbb5c09dfaa138fcd9881b7894904027600",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/DARKTUSKS.PST","71102b85f2b220eccb3769a641f60bfc100d0b9c974daf1fe587690c763f622f",3323u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/DARKTUSKSDESIGNS.PST","df6fdaceac16e6ab1d941ad20503af50016db62162c96da25834eb57e7414988",5491u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/DARKTUSKSDESIGNS02.PST","31e786c6cae12683439e8d0a13e5b2ef646bcd9faec52fe82164c7c8ef95edd8",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/GARGOYLEBODY.PST","a72f848331bae9be135ce772a35a69bdaf1bda6e98de69b3ae7e589430a25f43",5782u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/GARGOYLEFACE.PST","a6e8af21befe0a4e39e2ba6a3272a8eb1f66e9f923a0b268fbcaf4dbe48b710a",9093u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/GREENWATER01.PST","15f9771417047623c7b5c28b7fb01543197b22cdacc27665110dcb4c3f7efff3",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/LARGEFLAG.PST","9c82b04bf9aceee7f881e76b1e7cfcd431901c83cbd829f1f2d1b718525d622e",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/LARGEPILLARBASE02.PST","d7b76af0e1fe6feccb3840a236c04b6e3b1569139e3b14e1edc2e507b3df8b84",5155u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/LEAF01.PST","86592eb1b4b706e1a897712c2d94a972c31e11d489f576797b8d4e2e33933cc6",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/LIGHTINGRIGLIGHTBOTTOM.PST","f3ae291f8887019c0098cdf50de287f226b9e9c37bb3bacd8b6abe4b47dfd0f9",2193u,64u,64u,2u,2u,1u,0u},
{"TEXTURES/LIGHTINGRIGLIGHTSIDE.PST","bde4f1e341110fa77daa3c8957ba3d2391c7acd2dac9a331c2f3fc1bf2d89e0e",4203u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/LIGHTINGRIGSIDE.PST","59f11ee833cda543ed5a1eadebe7806b5af40db9d8105f16714d9215fe2fffba",2509u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/MUSHROOM06TOP.PST","9e68db94d7ebc615cfca78e62cff9adf4dd54dab54fb6577c049dcd1072b1a8a",88412u,256u,256u,4u,1u,1u,0u},
{"TEXTURES/MUSHROOM07TOP.PST","f7566afdd9ca18a0920b83819104ca3de5a3a3f3e3a59d163b41ecd0c369d605",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/MUSHTRUNK.PST","ff3b9bdeea4807f44c016226e284d2ad2f523e14d157cf4b658daefcd34c28df",88412u,256u,256u,4u,1u,1u,0u},
{"TEXTURES/MUSHTRUNKX64.PST","44238f02bbe8b8c17ac410a0690f97bc4e53f7168ba6f08e61bb2202e21fddb5",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/PLANTSTEM.TGA","f84e083c073aa756b3739acf7e1d05630d4098a1a3f51b15c6612cb6e409ed2d",3116u,32u,32u,0u,3u,1u,0u},
{"TEXTURES/PLANTTOP.TGA","0deb48ef798cdb2d1587eec48bd47258b4249ff843982894a05aeb72dd0f56d1",16428u,64u,64u,0u,3u,1u,0u},
{"TEXTURES/PLANTTOP02.TGA","618bb0cca7f95193a43708bb0438734aca844eef2acd2b9e3dbefd0a21d44dd9",16428u,64u,64u,0u,3u,1u,0u},
{"TEXTURES/RCLIGHTSTRIP.PST","7fb039aa70dc8a4726b6a09268a5ee6036ab52738a293246486f8b04175f1d86",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/RCPOOLS.PST","3d67ef27c2fcbc62042a8e2faecfb34c637e542936796e8cf6ed12d8e21f8a44",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/RC_CLIFF01.PST","0c9ebd69892d5030c3d68fd6fd0069981b2f0be137a8c8cbb828dae06e56bdcb",8148u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/RC_GRASS01.PST","6c33db299e77757e76aa28f16f99936f7081bbc6aceb7de7fa450f9dd5f85264",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/RC_GRASS02.PST","800bc90894114197956c73a3fe1e53e8a6d8b94260e060a72425912e88dc62ec",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/RC_NEWROCK.PST","186200a3f27b2eb8a7f0f1b9e16ed0733bf74a7e48bda4c0f32c77b7b386245b",42391u,256u,256u,4u,2u,1u,0u},
{"TEXTURES/RC_SAND02.PST","0c3a5abd729f999debeb00e9adc90ae9516522ac6508a3d439bb5b27de32c8a9",38015u,256u,256u,4u,2u,1u,0u},
{"TEXTURES/RC_TRACK4.PST","3c5df581af47780e26cab024cc348bdcef4aa29415aa4eb2ce898f85a7b0cc37",31204u,256u,256u,4u,2u,1u,0u},
{"TEXTURES/RC_WALLBLEND.PST","f5ab8d212661e31c6fc87994212cdaa02c50e38c5c36a12b8ab2dfa593426cbe",32168u,256u,256u,4u,2u,1u,0u},
{"TEXTURES/RC_WALLSIDE01.PST","b1a619c80a83278753be90d617bd4a9c2020e02b75adb108334c69a18565c607",35426u,256u,256u,4u,2u,1u,0u},
{"TEXTURES/SPEAR.PST","2e86ede0b92202716910288ae6a4e79fb86edb12455a10fffde218016ce97ba8",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/SPECMAP5.TGA","0c9838ec2d3a1936b6dc1a5e218accfd29f47307c5a7d9801614a2d1858bc7ea",196652u,256u,256u,0u,3u,1u,0u},
{"TEXTURES/STAIRS01.PST","b73db8ca765f38950320917f0f7424eb6e7eb805e5448f86a1cc3190c42589ef",6047u,128u,128u,3u,2u,1u,0u},
{"TEXTURES/TWILEKFLAG01.PST","ba9fc527c7549f3907fdbbeb0350a79f88cbdd49998f2b00718239e6906716a5",6966u,128u,128u,3u,2u,1u,0u},
};
inline constexpr source_texture50fc_fixture::detail::Entry indexedPins[]={
{"TEXTURES/ARENATILE.PST","709a54f496ddfcdb54b00e786be35cc014f72a153da5ff8738d6203d72102f40","","05bf49426e1f8a027c04a8821f370f753db1037327b08a2a30a8c831cb265d6b",6460u,64u,64u,2u},
{"TEXTURES/ARROWRED.ATM","dbfdaf3d44918c86254ae9f39f4146cc101c363e1fb9e2cda43bb8c0ea99b220","","483e1dcbee659193a7aacdd6ba8cca6fd685189ccd12e6ff467624eb74753e79",114312u,128u,128u,3u},
{"TEXTURES/BULBBASE3.PST","4c3e9a7819e35290de58f7faa6f28a839be596689d9457387e0175db7978a3d5","","b6a661e47737378da65fc80587f7614d26d82a0c9d27433935d04abfbe93c620",22860u,128u,128u,3u},
{"TEXTURES/CAMDROID03.PST","b7da07e4e9e3fe12fa47b9f9c06c5803534ae69c19d9904106060a84dbfa6710","","0fe0a87019b35dea4584d28bfa3d578d5191ec45e9fa454e3df549d7eedfd7f2",22860u,128u,128u,3u},
{"TEXTURES/CRYSTAL01.PST","1a9bca8cb30af8ee09c09b6c597b7fbb5c09dfaa138fcd9881b7894904027600","","ec4940b591f8af5085486bab0c44ad76bc710407897e95e38f98f5b5f4268fe0",6460u,64u,64u,2u},
{"TEXTURES/DARKTUSKSDESIGNS02.PST","31e786c6cae12683439e8d0a13e5b2ef646bcd9faec52fe82164c7c8ef95edd8","","b3edc630ad8af3db47eeb70edf8af85c0f129473933013741a3a9996b96275f2",22860u,128u,128u,3u},
{"TEXTURES/GREENWATER01.PST","15f9771417047623c7b5c28b7fb01543197b22cdacc27665110dcb4c3f7efff3","","11fd2f42de0ac6bedcb43d2cd470ee25c3d531ed0ca77b213a49370656737e66",6460u,64u,64u,2u},
{"TEXTURES/LARGEFLAG.PST","9c82b04bf9aceee7f881e76b1e7cfcd431901c83cbd829f1f2d1b718525d622e","","6b55ca01b5ce5f4191bc2c34d61e257637ca9a564f43f8af8650fd8f380399c1",22860u,128u,128u,3u},
{"TEXTURES/LEAF01.PST","86592eb1b4b706e1a897712c2d94a972c31e11d489f576797b8d4e2e33933cc6","","4592bef09dd40c153cf1d705ff3ac633b3a18fb5c66afde564c328962a217b15",22860u,128u,128u,3u},
{"TEXTURES/MUSHROOM06TOP.PST","9e68db94d7ebc615cfca78e62cff9adf4dd54dab54fb6577c049dcd1072b1a8a","","87794337110ffe1ea009b143bfa67a4d10a91eddf28399396e707afc3f37c867",88412u,256u,256u,4u},
{"TEXTURES/MUSHROOM07TOP.PST","f7566afdd9ca18a0920b83819104ca3de5a3a3f3e3a59d163b41ecd0c369d605","","6fee093859d4484790bb9ccf28a71e82d74a9cbc3be073e5ca970ca24e3cee72",6460u,64u,64u,2u},
{"TEXTURES/MUSHTRUNK.PST","ff3b9bdeea4807f44c016226e284d2ad2f523e14d157cf4b658daefcd34c28df","","baecc7ac90b1afe096a1372c0fcca204f5fd616aa47c8fc419064daea64fb11d",88412u,256u,256u,4u},
{"TEXTURES/MUSHTRUNKX64.PST","44238f02bbe8b8c17ac410a0690f97bc4e53f7168ba6f08e61bb2202e21fddb5","","c7b9bcf8ae351f504a027b0a63b3b444f2a20f5a7051c6568205c3be02413260",6460u,64u,64u,2u},
{"TEXTURES/RCLIGHTSTRIP.PST","7fb039aa70dc8a4726b6a09268a5ee6036ab52738a293246486f8b04175f1d86","","fd58d018df9a1de2f5d91dc8b25cab31da743ad15f0789cc206bf7c9b76fec59",22860u,128u,128u,3u},
{"TEXTURES/RCPOOLS.PST","3d67ef27c2fcbc62042a8e2faecfb34c637e542936796e8cf6ed12d8e21f8a44","","47a40177f820ad4b57b59a67ef1ec7c6499b7c3a2ca8105a217ec70f401bf168",22860u,128u,128u,3u},
{"TEXTURES/RC_GRASS01.PST","6c33db299e77757e76aa28f16f99936f7081bbc6aceb7de7fa450f9dd5f85264","","2e33dd28db3a518be207ddad78b01e086d452cbed9454155a9f5d94e6a497edd",22860u,128u,128u,3u},
{"TEXTURES/RC_GRASS02.PST","800bc90894114197956c73a3fe1e53e8a6d8b94260e060a72425912e88dc62ec","","edca486ad82357d3e54f0c4188c72e173a8db596f809e944e8316288411c3a3c",22860u,128u,128u,3u},
{"TEXTURES/SPEAR.PST","2e86ede0b92202716910288ae6a4e79fb86edb12455a10fffde218016ce97ba8","","3e019d12d4a2a984baf516d2dd0a3a35f9641d0756b2cef0319ff69d299b2570",6460u,64u,64u,2u},
};
inline constexpr std::array<source_texture50fc_fixture::detail::Entry,0> exceptionalImages{};
struct CrystalPin {const char* member;const char* sha;std::uint32_t bytes,vertices;float qPosition;};
inline constexpr CrystalPin crystalPins[]={
{"GEOMETRY/CRYSTAL_INSTANCE.PSG","3a8bc4b25606014722cdfc6fc47d0beb69286e57426da9ac88664f02854b2b0d",9760u,430u,0.0055763693526387215f},
{"GEOMETRY/CRYSTALCLUMP01.PSG","a8a2cf3fa92d1448147cd77dfeac0d53c2fbd2c08516a283d2442b4695a182df",4608u,194u,0.005940180271863937f},
{"GEOMETRY/NEWCRYSTAL03.PSG","51921a216fd3eaf93544848045fe77efd6d300dbd3b5919d4319ae73f0205032",2992u,120u,0.002124525373801589f},
{"GEOMETRY/SINGLECRYSTAL.PSG","093cc5469289b953fd6508cf09899f14469815505ed217b3815304a1c400f864",1104u,32u,0.0012033276725560427f},
};
}
#include "rc_world50lm.inl"
}
