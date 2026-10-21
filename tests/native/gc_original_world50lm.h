#pragma once
#include "track_materials50fa_fixture.h"
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

// Direct original-disc GC static source profile. All geometry/pass/normal and
// image construction is shared in original_all_pass_world50lm.inl. Original
// ATM initial frames are retained; live animation is separately owned.
namespace gc_original_world50LM {
namespace a=rr::assets;
namespace shared=pod_materials50ed_fixture::detail;
namespace tm=track_materials50fa_fixture;
using Bytes=std::vector<std::uint8_t>;
using Json=nlohmann::json;
inline constexpr const char* archiveSha="69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9";
inline constexpr const char* scbSha="d47a83f4b50812609c923381f3a9038328c81d9a50563b68fb4de022a9be210d";
inline constexpr const char* archiveMember="TRACKS/GC.RES";
inline constexpr const char* scbMember="TRACKS/GC.SCB";
inline constexpr std::uint32_t archiveBytes=4205067u;
inline constexpr std::uint32_t decodedBytes=31580160u;
inline constexpr std::uint64_t textureByteBudget=8u*1024u*1024u;
inline constexpr std::uint32_t textureCountBudget=64u;
inline constexpr std::uint32_t entryCount=1190u;
inline constexpr std::uint32_t scbBytesCount=359830u;
inline constexpr std::uint32_t expectedInstances=999u;
inline constexpr std::uint32_t expectedZones=12u;
inline constexpr std::uint32_t expectedPortals=13u;
inline constexpr std::uint32_t expectedScenes=426u;
inline constexpr std::uint32_t expectedPlacements=801u;
inline constexpr std::uint32_t expectedCollisionOnly=198u;
inline constexpr std::uint32_t expectedSurfaces=1175u;
inline constexpr std::uint32_t expectedMaterials=375u;
inline constexpr std::uint32_t expectedSelections=693u;
inline constexpr std::uint32_t expectedImages=60u;
inline constexpr std::uint32_t expectedVertices=186184u;
inline constexpr std::uint32_t expectedIndices=345960u;
inline constexpr std::uint32_t expectedRgbaBytes=4276224u;
inline constexpr std::uint32_t expectedDrawSpans=1517u;
inline constexpr std::uint32_t expectedSecondPasses=342u;
inline constexpr std::uint32_t expectedPinnedImages=4u;
inline constexpr std::uint64_t assetIdentity=0x4743ull;
inline constexpr auto selectedTrack=track_source_fixture::Track::gc;
namespace detail {
struct SourceImage {const char* member;const char* sourceSha256;std::uint32_t sourceBytes,width,height,mipIndex,kind,frameCount,initialFrame;};
inline constexpr SourceImage sourceImages[]={
{"TEXTURES/ARROWDROID01.PST","478676a45c866b654d12ad909ba042cc428ec96e1a462ff5824512df1518a5e3",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/ARROWDROID03.PST","abb0c6ad9fa0716e2ee5d4d4fd318cd05c5d1df8ef336acd46b933d884106e07",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/ARROWRED.ATM","dbfdaf3d44918c86254ae9f39f4146cc101c363e1fb9e2cda43bb8c0ea99b220",114312u,128u,128u,3u,1u,5u,0u},
{"TEXTURES/BIG_WILO.PST","c838df418598b68e60abdcfb671fe9f8cc47e40483b893a31db1d6cd9297899e",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/BIG_WILO_LEAF.PST","2bfd9b0f789951c3bf8ace86a9d4b5de58e47534ce7742d6ed86486f77690bb0",17436u,128u,128u,0u,1u,1u,0u},
{"TEXTURES/CAMDROID03.PST","b7da07e4e9e3fe12fa47b9f9c06c5803534ae69c19d9904106060a84dbfa6710",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/CAVEWALL.PST","17f8a7aafb4d328edebe9ca93351c930f738b3bed148f9b50b735596afb0617a",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/CORD.PST","40a4839a7c8ced2fb435e3d73a357915aeacbbcf48bf11c3c992dbc27d5bc1b7",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/CROCK01.PST","29099b7f74f73fcf58c8ea4de9db6aacac9064cf80d7fab65893eb7412274be9",88412u,256u,256u,4u,1u,1u,0u},
{"TEXTURES/FIENCE.PST","85148a2653e50999f30019e7cf2b8fec2c0fa6e45dfe526fa6175c6f6b415052",17436u,128u,128u,0u,1u,1u,0u},
{"TEXTURES/FIENCE01.PST","26bbb5b65d335188f47d39ca33ccbf5a3dab7182b8e6f61cacce8ee073077322",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/FORTRESS_A.PST","12ddfb74b9c5243b681efddfbe92aa91eb7874792551f84824b645977fc0ea6b",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/FORTRESS_B.PST","549a78586a89350ff7a708153d47f4c1345dc817b05786eba0479c0d01ba6788",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/FORTRESS_C.PST","3f5b5f649ac60c4de3aca98186f8af3dd837a8cbd77e57b3c8d94a1d43dcabe5",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/FORTRESS_D.PST","a4d0277aa43192f33e1e776487d3c116aadf86db710f418bb88fdbac211182db",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/FORTRESS_E.PST","295c5a9f582e6f3fa95897259f8ceeea52bc74677d750b58b9ca675f2426570a",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/FORTRESS_F.PST","9cdce76ad6460143bcccb6e5aac77dcd828becf236c85260f886a6883ecbb550",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/FORTRESS_G.PST","157617fb3628fb35c6bf1790fc3f00332ef106ed0bd7bf120fe990256031dfe4",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/FORTTRESS_TEX_A.PST","18e43f36a340488c6fa0791198bcb857e27862d7468d2ec98bc3e2f6628deef2",88412u,256u,256u,4u,1u,1u,0u},
{"TEXTURES/FORTTRESS_TEX_B.PST","3b02d31d3bea77805f5e54e849d3af5ad94e210cec3ce844eb2d7b86f55693c2",88412u,256u,256u,4u,1u,1u,0u},
{"TEXTURES/GAM_BRIGE.PST","48814b7be8b80c4eb76f7e864275114beb0505f4453f4788c9ad5d10c696cb72",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/GAM_TREE_G.PST","2b4c58c1532875b2a2b54f4941c46533ed891efdffdca3d3b7dec43bbbe260af",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/GROUNDTEX.PST","0a7d945d250c865d1fa8dbe8049341448a06061244663260178cb0dc39a6659d",88412u,256u,256u,4u,1u,1u,0u},
{"TEXTURES/GUN_A.PST","f630c44468bffbeb185162f549d8c034ea41ec6246a16752292b6a5a88a0f25c",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/GUN_B.PST","adcd92096a9d95862b80536b861e9dcf6bcc63506553b009033e63fce163cfb1",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/GUN_C.PST","fd9b0b57e5ce060c02ddce825aa70882d7ae6106a8e60a1cc236ebbff81b0b85",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/GUN_D.PST","8c42cf01bd9209e16872deff5a97ad8fc99ef81fdb9753dee196e77941d1c087",2348u,32u,32u,1u,1u,1u,0u},
{"TEXTURES/HOUSE_STAND_A.PST","21c535e1e977ee283106587ffa573d8dc51867f079afad3282ebc86fff5c8324",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/HOUSE_STAND_B.PST","f936f84a23d1d40e7c935d6611925f037515bfb6ce345abaad37c0ea9e662cb0",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/HOUSE_STAND_C.PST","cb0c4b85e8f49b8c92e50b8f6d5ba906032458989ca68c34af1cd111783f19b9",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/HOUSE_STAND_D.PST","b1df57d9c4606f8348f5f27a4f177116f4286ed288f0f5461e5a913b1ab9d32d",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/HOUSE_STAND_E.PST","5e02fd6e8375dec49ce0a4065efc684f080ba7a701e567501958d61dd22317f8",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/JAG_ROCK.PST","17f8a7aafb4d328edebe9ca93351c930f738b3bed148f9b50b735596afb0617a",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/MARKET_HUT.PST","f7bfeeeb104ef31caec40ae2b00de9f07977df6cbc84326372e78d40789702fc",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/MARKET_HUT01.PST","45112545305bf97fef827df0e8207832d978fb47885daecb8119fab345f1a386",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/MARKET_HUT02.PST","a740e4b6cb8abc298ddfabc82d2ef1007d84d74a1516b5d1bcc7364472703b7f",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/MARKET_HUT03.PST","d18758976f8dfc8d2c315a36a7ca0051b3cc55698a9ac343b688cae1c8caf9e4",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/MARKET_HUT04.PST","5bbedd4da9c4a70d5ed143692d1d9a957ca07f64cb00359dfefb7fe5959a1c20",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/MARKET_HUT05.PST","1923234be3d7c964332a6411beb674fd738891e48faedd41b4afdf8d8cde9eef",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/MARKET_HUT06.PST","0396bc4d0cb157497a77c87d35ff298cff54b266af38390ac25a38c150eb18fc",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/MARKET_HUT_A.PST","1c7f08ef805c62f898efc8789789d7405dddca25f3b5b98af7440c1242618381",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/MARKET_HUT_ROOF.PST","d595cf6d7c48c356d83596e941a420bd1a5d58c00ab2e2a5d88c2fc6e816af73",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/MARKET_TABLE.PST","59b6f1bf3e856d9ea342265ce9e6ed3d477451e4ff0ecec170a38cb42cee7ae8",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/METAL_HUT_SEP.PST","c5625c733d393fdd3b90e8d4a7361ccf82e76253d06f5bfde15c60b785a5771e",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/METAL_HUT_SEP01.PST","8e6488fdf3fc7e62415644a34e6a448b96d019b83660d1a5a322fec238879578",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/METAL_HUT_SEP02.PST","e858fb4739c7f6763836d9951b1da080e72da12777533fbed53ff3300123ace3",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/METAL_HUT_SEP03.PST","229152a519c2d618450cb2a90f849c69ce86284f99707ec08e3dbdec3c5b205b",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/PIG_HEAD.PST","a821c73177521925a020320030e6c1cc4bee567019ae9235a706e01d131c2be5",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/PLANKS_WOOD_A.PST","6dc43028adbb1e5deaf0e1e7f3c0e8117c86eff88a7797e45dfada217da2b39b",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/ROADTEX.PST","06cdf8a58c615d67ceb4d725bf7c59ccfc1c289f96293a378bb90bd348d9c44b",88412u,256u,256u,4u,1u,1u,0u},
{"TEXTURES/SHADE.PST","a5a24dcfc87bd50935ed18ff94d298e45de052c823bbbd9a9dda9d8af2fb01be",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/SMOKE_STACK_A.PST","c27a8fd7437880a3a08b2adcbe40a34549bad47772f6f77f2d4ee56aee89456e",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/SMOKE_STACK_B.PST","9388d09deff7555b418dcc4f9c081222b95ad7335d9d7cb40a0814199214c4d1",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/SMOKE_STACK_C.PST","a4b68eb8df612c53b79f15d5978d52bb6ece73644c4659ab10636a208bed0879",22860u,128u,128u,3u,1u,1u,0u},
{"TEXTURES/SMOKE_STACK_D.PST","ab660b5fce1f7365c3ac800c937d8f9d6f1d6f7701ecb3c91abf8c4a751637bc",6460u,64u,64u,2u,1u,1u,0u},
{"TEXTURES/SURFACETEX.PST","f41107de305d51d8c54a19769a4bf8d929c7f25267eb3b7c4e05bfa76e8def76",88412u,256u,256u,4u,1u,1u,0u},
{"TEXTURES/S_FLAGA.ATM","b2a8e2bfdc8c8c19e1d8bdd1c2c0e045fb0f07dd215de06616fd94ee08dd1a66",23492u,32u,32u,1u,1u,10u,1u},
{"TEXTURES/S_FLAGB.ATM","7d4b88b2411ea69b1480c623de4f3f5e57198b25b87a44c39023068dd7272255",23492u,32u,32u,1u,1u,10u,5u},
{"TEXTURES/S_FLAGC.ATM","ff7d1665d5a4672eb095e8b241ed3c8e5d11dd35d8cf1e2f156a7b3d5137ed46",23492u,32u,32u,1u,1u,10u,3u},
{"TEXTURES/TREE_BRANCH_A.PST","6918a736316c5fa3497829045d54c507d2a9f8bd3bd93a315783d72002c2a9ab",22860u,128u,128u,3u,1u,1u,0u},
};
static_assert(std::size(sourceImages)==expectedImages);
inline constexpr auto& outputPins=source_texture50fc_fixture::detail::gcEntries;
} // namespace detail
#include "original_all_pass_world50lm.inl"
} // namespace gc_original_world50LM
