#pragma once
#include "../../src/assets/original_track_cut_identity50la.h"
#include "track_materials50fa_fixture.h"
#include "original_world_material_admission50cm.h"
#include "../../src/assets/original_texture_candidate50tc.h"
#include "material_alpha50gx_fixture.h"
#include "source_texture50fc_fixture.h"

#include "../../src/assets/import_world48f.h"
#include "../../src/assets/original_ipu_decoder.h"
#include "../../src/assets/original_standard_ipum_framing50la.h"
#include "../../src/assets/original_ui_atm50kg.h"
#include "../../src/assets/world_normals50hk.h"
#include <functional>
#include <algorithm>
#include <charconv>
#include <iterator>
#include <limits>
#include <map>
#include <memory>

// Cold TC owner proposal; strict geometry acceptance remains independently gated.
namespace tc_original_world50LM {
namespace a=rr::assets;
namespace shared=pod_materials50ed_fixture::detail;
namespace tm=track_materials50fa_fixture;
using Bytes=std::vector<std::uint8_t>;
using Json=nlohmann::json;
inline constexpr const char* archiveSha="7b5486f4029dabe137919127c879faef92b8bd2fb9df5c0d1a1d122e92731490";
inline constexpr const char* scbSha="795bbbf9882baf233b9d67f777d0bb1e36506dda0f1ad7e71a303faae3993d24";
inline constexpr const char* scbMember="TRACKS/TC.SCB";
inline constexpr const char* archiveMember="TRACKS/TC.RES";
inline constexpr std::uint32_t archiveBytes=3607868u;
inline constexpr std::uint32_t decodedBytes=16048128u;
inline constexpr std::uint32_t entryCount=509u;
inline constexpr std::uint32_t scbBytesCount=232284u;
inline constexpr std::uint32_t expectedInstances=600u;
inline constexpr std::uint32_t expectedZones=25u;
inline constexpr std::uint32_t expectedPortals=26u;
inline constexpr std::uint32_t expectedScenes=254u;
inline constexpr std::uint32_t expectedPlacements=531u;
inline constexpr std::uint32_t expectedCollisionOnly=69u;
inline constexpr std::uint32_t expectedSurfaces=738u;
inline constexpr std::uint32_t expectedMaterials=69u;
inline constexpr std::uint32_t expectedVertices=261995u;
inline constexpr std::uint32_t expectedIndices=477729u;
inline constexpr std::uint32_t expectedRgbaBytes=7262208u;
inline constexpr std::uint32_t expectedPinnedImages=7u;
inline constexpr std::uint64_t assetIdentity=0x5443ull;
inline constexpr auto selectedTrack=track_source_fixture::Track::tc;
namespace detail {
struct SourceImage { const char* member;const char* sourceSha256;std::uint32_t sourceBytes,width,height,mipIndex,kind,frameCount,initialFrame; };
inline constexpr SourceImage sourceImages[]={
    {"TEXTURES/ACAM01.PST","df778d48b79c2728febe8d6b2efc7309c528031940bf7be8bafa3370ad9f5a1a",757u,32u,32u,1u,2u,1u,0u},
    {"TEXTURES/AHUTS01.PST","7a384b82fc8567484642243cb83683d19fb21d4e721f01bc5246fdf0f2dd206e",213u,16u,16u,0u,2u,1u,0u},
    {"TEXTURES/AHUTS02.PST","fcdf85d0830771516fbb062fdd30f093c7b21360b3c1e54c1f74ae0c82d254b4",176u,16u,16u,0u,2u,1u,0u},
    {"TEXTURES/AHUTS03.PST","7e03df8953f376586c53d25a8ca409ee60b051c7cf7709783f1824d82e155412",179u,16u,16u,0u,2u,1u,0u},
    {"TEXTURES/AHUTS04.PST","f26535ca2f9a0a0d4bfd6651e133b682246ea822af1a01845e4d91aee2252359",166u,16u,16u,0u,2u,1u,0u},
    {"TEXTURES/AHUTS05.PST","28ec9821ec922e5cfc516e03715baf5bc0a458fe7fb3af05d1a4ff007c82accb",195u,16u,16u,0u,2u,1u,0u},
    {"TEXTURES/AHUTS06.PST","09d4896293dc1f9ed378a5b14f3160e6e12b9786a4fcea8943f87a3455b70454",198u,16u,16u,0u,2u,1u,0u},
    {"TEXTURES/ARCH2.PST","cb51a37b5f1cf91b53be279a73756dd72c2dde2b27cc6c01d172757c12e891b9",7474u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/ARINAC01.PST","828d588ec78ec276790a5026be819573abf1593ec56c2c010684a08a232f23ee",23772u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/ARROWDROID01.PST","0cc7a8d515a392e195af12ee0c0f4610a19f8c91a0075b3a449f4005d88e4fce",2301u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/ARROWDROID03.PST","7e9021d48bbe4d3f09111710627d384888d959a7b4048abcf23f368a038e8056",2079u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/ARROWRED.ATM","dbfdaf3d44918c86254ae9f39f4146cc101c363e1fb9e2cda43bb8c0ea99b220",114312u,128u,128u,3u,1u,5u,0u},
    {"TEXTURES/CAMDROID03.PST","b7da07e4e9e3fe12fa47b9f9c06c5803534ae69c19d9904106060a84dbfa6710",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/CAVEDIRT.PST","5b427222a1706d61372da18ce5ff055f8a77ea2e669a27e88b9ba76add84eeef",30250u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/CNSESNS1.PST","11b988779036a6adf920d33dcaa2d911871ee59ca8480108add7eb7443b35154",4650u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/CORD.PST","91cb74edd2d0cb4f3ca102abea2f06cc5e539f21f6b5fb42b54d42baafdc39a1",1233u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/CRAWLERA.PST","55adb22e376c5f92968b37d6d85ac30eba23d864a1b5682af4e738719722de66",20762u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/CROWD.ATM","76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031",214578u,256u,256u,4u,2u,6u,0u},
    {"TEXTURES/DBLEND.PST","103be850a8371620fe1353800919a93e2e0f38c1b14118f1542d2207b2135813",25972u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/FENCE02.PST","b71931c958e36a3afe174d152c565a54b33456a5043251a4a5d50fe648966ab9",1905u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/FIX01.PST","3d57b48faa3b9ac354446694de52cd9369eb9e46f09ca7c292fe171c680bdf01",2086u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/FLOAT01.PST","f3592b1820fcd955ca12969f0a33b5ed30ac9f0cfae4c000bc2026665b23c29e",3198u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/FLOAT02.PST","2343fde19681f55c8ff907a7c961e7579119e4d9244ec079eedf990085551406",2814u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/FORCEFIELD_DOME.PST","d94cabd76761a8217d8f4574ade0bbabb8034da10798e3cf6fc61e2773d0131f",2348u,32u,32u,1u,1u,1u,0u},
    {"TEXTURES/HUTR01.PST","416bac074c6b865a50a797aa4eb7d29b2fc0ded38fc0fe2144cf75de38446f60",22634u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/JABARB01.PST","11df7aff9204566590b35fb3a2b06fa346ab3a8fe183f93701065337c8c26780",30315u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/JAWA.PST","39e0c7425ffc849b30937c0b062c99420f865f2f7e72de691c39015f0d3eb98d",6460u,64u,64u,2u,1u,1u,0u},
    {"TEXTURES/JUNK02.PST","a9f188feecb0933757365d213d4f5ed7a7ec090c2cc3f448c4dfa133e767e35f",1602u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/JUNK03.PST","53b74a536b34370ff67c03b4cfd2f5ad479f656a5d28246556051f55d06bd898",3025u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/JUNK04.PST","13b12f5e95aead2eb48a1c3c2719975866c6036b22e8e9a5fca23ed4a53fd182",1666u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/JUNK05.PST","762b50e1415a81b89d02588c56bad69b073a88db2811c69b6ed802013a62fc58",2601u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/JUNK06.PST","e51d8e1c55427e8a55566154577863c792d69c4207be57d235d7f6a338b92e1a",2227u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/JUNK07.PST","6a10879d0ef5c3369ad430b2461f5fbf60f117457bfc0fbc6d174fcad49b2c75",2422u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/JUNK13.PST","5b0df64817a62bb6dd68918f7e90d750c08a88e9914db8f6284b4226c8067dc5",2317u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MUSH_1A.PST","a172be707f5d1a056ee5c4636ae016f9e2631300ca43cb8289488426c75de248",29817u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/MUSH_2A.PST","1a98d9dccba02ad7127172f93191cae852f6757c8240f25c2353db0e2a336452",31304u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/MUSH_3A.PST","d8f96c1c7240483aa988f8f8ab25253060f5919e321795352ca1560d0f2838ad",8654u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/PITHAN01.PST","4befc1cc04eca217c5c19f804bbfaaa4a0b8f431b03547ddfda933717e384bee",24879u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/POLE01.PST","1850c927af158e75ec63bdf9c37fe13ecb68cdd56919cf0ec187efc1bf281633",4953u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/POLE02.PST","9667193f3f954bba023bea55969caf0dae6ccd760d5d5a0a82cf68d8dda6ed59",4580u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/ROCK_STRATAB-1.PST","fd657aee97634d4db5229e110bfbdfa4d00d82aae2309b0fe2b9952752812478",37062u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/SEATING1.PST","69fb246e2b8adc4e439a115b164178077b510dff3beb8720de72bcb332a49a54",27891u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/SEATING2.PST","8e57276b4ee07123b255f330620cc237b8ad0eca45389c44c59aa011df8b8d66",28651u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/SHADE01.PST","17d2aba48656bd40bf0c3ad24fb6dc8be51a818ae76e8536655d44b402c339b7",8838u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/SHADE02.PST","cc195bbcb2674b7a33c2d44e7444a2c6771c97c1d26b10518ff4257c871cf63f",179u,16u,16u,0u,2u,1u,0u},
    {"TEXTURES/SHADE04.PST","7db5ad82bb544cce8478c4f0bb95ef5eb4d412532a0674698ff42179ad963c83",140u,16u,16u,0u,2u,1u,0u},
    {"TEXTURES/STALAG1.PST","89df4cf4d260745288c3b1ca88fd445bef32e76a16a38babe3598f333d56a03f",21846u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/STARTL01.PST","fafd913d56b264a14c2a13b456ecabc56241c219e82e75e888370e328c30befb",10069u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/STARTL02.PST","5735ce997aa3fa39c13d3bf981fa43499ae58464b7e3add9ab0e8a6b711e94eb",2057u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/STOP01.PST","5d969201966fa87911136a85f08fdafad26c118ab2c9436d63f531a54da7f6a4",1765u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/STSTAND1.PST","321169e9325d470c0d00542cb922272126f765f772f826d7569d0570d8c4f1f1",15868u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/STSTAND2.PST","4c59fc4f28f86ad4ab9767415ea12c0e4f47669e2ba48534c3396540ccefd40c",13941u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/STSTAND3.PST","a46edd20287e95ae2b6265049567502f41217c12978715c1ab23992c2ceb4b43",692u,32u,32u,1u,2u,1u,0u},
    {"TEXTURES/TA_BLEND2.PST","0d0e6e8b03b7ae5d35120d23d7923bad20f002c05cba48392cb905b4e4b2a92d",24231u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/TA_BLEND9.PST","1ec07ee40934ba45d577fb4852f6548db2ee99d762e09b3a3c881d8bb4ab678a",21954u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/TA_ROCK2.PST","89b635e42249bfaf66af7f9b7c891928cd9f9b3a71de456401dd8b29f0b9fd28",31522u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/TA_SAND8.PST","6b28ec986ae037cf4fe4beaaf2ca1f2e668bdfa744286e09113b95949366db8a",26567u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/TCSUN.PST","7fb039aa70dc8a4726b6a09268a5ee6036ab52738a293246486f8b04175f1d86",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/TC_BLEND5.PST","9487245dbea157e02aaf4405e2e6e3ff5074ee0fa345f0253954fa429a779e5b",26751u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/TC_BLEND8.PST","f31535111cd89e91242545b65ffadb37ac542f798d530c6e5accaa4be1b58342",31497u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/TC_SHIELD.PST","5dafb1b7425f3367997b408ae23e22d59bc40e32ca3b1ad6b1aa1c3ce807cee3",11020u,128u,128u,3u,0u,1u,0u},
    {"TEXTURES/TC_TUSK.PST","a472b6921f431d83863ae19fcef4772a24ae2d3e9b28eebf75242791682596de",18864u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/TOP01.PST","a56bd05f5000d1ea35e5dda73c4bd5055a6742644b96d419f2ee56aa58ebefae",2299u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/TOWER.PST","c1873504ec386e554a4ffddd12b31d1549d8e50339f6bf232f18ba8b51343033",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/TRACK.PST","e27d767a7d2aaa595d97b86b7208658404389540d2265d1bfbcaf2bd09ccbfef",2319u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/TRACKDARK.PST","df48a11d8ca7ad3a4c53e0e2840c4c0df57ce01504751dd5ea62248bfcb6c0c2",2590u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/TRACKSIDE.PST","4db3e6270ec85d8ddd16af7445befd43bad7e03f61cc9399d9559095ae9d5d02",7302u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/TRACKWHEEL.PST","de447b0debdad77b00d3f95d0919c0ec9d2a169229a18a61a5df9c722e240265",2209u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/VIEW01.PST","d447c5b06e21209eaed2c201d6d7a2041356b5d215cf5a3bd69d14e8682c5c6e",1793u,64u,64u,2u,2u,1u,0u},
};
inline constexpr source_texture50fc_fixture::detail::Entry indexedPins[]={
    {"TEXTURES/ARROWRED.ATM","dbfdaf3d44918c86254ae9f39f4146cc101c363e1fb9e2cda43bb8c0ea99b220","","483e1dcbee659193a7aacdd6ba8cca6fd685189ccd12e6ff467624eb74753e79",114312u,128u,128u,3u},
    {"TEXTURES/CAMDROID03.PST","b7da07e4e9e3fe12fa47b9f9c06c5803534ae69c19d9904106060a84dbfa6710","","0fe0a87019b35dea4584d28bfa3d578d5191ec45e9fa454e3df549d7eedfd7f2",22860u,128u,128u,3u},
    {"TEXTURES/FORCEFIELD_DOME.PST","d94cabd76761a8217d8f4574ade0bbabb8034da10798e3cf6fc61e2773d0131f","","b5570881cebe3b22cde64936cf35d205fc50cebc1b724f02ee50705e18e67633",2348u,32u,32u,1u},
    {"TEXTURES/JAWA.PST","39e0c7425ffc849b30937c0b062c99420f865f2f7e72de691c39015f0d3eb98d","","8fa2ffbdd5e18f39e4cb1ed65bc892974865d98c5828830a3dcaa9f7b48bc08e",6460u,64u,64u,2u},
    {"TEXTURES/TCSUN.PST","7fb039aa70dc8a4726b6a09268a5ee6036ab52738a293246486f8b04175f1d86","","fd58d018df9a1de2f5d91dc8b25cab31da743ad15f0789cc206bf7c9b76fec59",22860u,128u,128u,3u},
    {"TEXTURES/TC_SHIELD.PST","5dafb1b7425f3367997b408ae23e22d59bc40e32ca3b1ad6b1aa1c3ce807cee3","","eafabe17d9434de9967749059f17fbde7f5068733fe07229d285dad87cfa8d49",11020u,128u,128u,3u},
    {"TEXTURES/TOWER.PST","c1873504ec386e554a4ffddd12b31d1549d8e50339f6bf232f18ba8b51343033","","e10fd69b2fe3cc9f798b53ed3731dcb10aef2abc661c89821113408859ad7952",22860u,128u,128u,3u},
};
inline constexpr std::array<source_texture50fc_fixture::detail::Entry,0> exceptionalImages{};
}
#include "tc_world50lm.inl"
}
