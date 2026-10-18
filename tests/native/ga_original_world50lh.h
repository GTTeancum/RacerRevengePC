#pragma once
#include "track_materials50fa_fixture.h"
#include "material_alpha50gx_fixture.h"
#include "source_texture50fc_fixture.h"
#include "watchtower_source_fixture.h"
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

// Direct original-disc GA static ownership. No filesystem paths or generated
// asset files are read or emitted. The authenticated SCB and original PSG/PSM
// sources supply all geometry, normals, 308 materials and 1,241 draw spans.
// Both ProjectedPrelit passes retain their source-space texture generation.
// This owner supplies the exact authored ATM start frames; a separate owner
// must advance the 57 animated placements. No TOB motion/trigger registration,
// canonical static-world model, original VU/shader or pixel parity is claimed.
namespace ga_original_world50LH {
namespace a=rr::assets;
namespace shared=pod_materials50ed_fixture::detail;
namespace tm=track_materials50fa_fixture;
using Bytes=std::vector<std::uint8_t>;
using Json=nlohmann::json;
inline constexpr const char* archiveSha=watchtower_source_fixture::archiveSha;
inline constexpr std::size_t archiveBytes=watchtower_source_fixture::archiveBytes;
struct Diagnostic {
    const char* stage="input";
    std::string member,message;
    std::size_t scenes=0,placements=0,collisionOnly=0,surfaces=0,zeroNormals=0;
    std::size_t materials=0,selections=0,secondPasses=0;
    // A runtime digest records a decoded result; it is not an independent pin.
    std::size_t independentlyPinnedImages=0,runtimeIdentityImages=0;
    a::DecodeError48C decode{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
struct SourceImage {
    const char* member;const char* sourceSha256;
    std::uint32_t sourceBytes,width,height,mipIndex,kind,frameCount,initialFrame;
};
// Exact 79 live source-image identities and framing from authenticated GA.RES.
// These are source pins, not newly invented independent decoded-output hashes.
// The existing source_texture_ga_index.inc separately pins all four ATM start
// images and the five dynamic PST outputs (FIENCE is also used statically).
inline constexpr SourceImage sourceImages[]={
    {"TEXTURES/ARROWDROID01.PST","0cc7a8d515a392e195af12ee0c0f4610a19f8c91a0075b3a449f4005d88e4fce",2301u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/ARROWDROID03.PST","7e9021d48bbe4d3f09111710627d384888d959a7b4048abcf23f368a038e8056",2079u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/ARROWRED.ATM","dbfdaf3d44918c86254ae9f39f4146cc101c363e1fb9e2cda43bb8c0ea99b220",114312u,128u,128u,3u,1u,5u,0u},
    {"TEXTURES/BIG_WILO.PST","07dabd304bc83037c65ad0e3cd2fa7063d36038e6e9653e951e60ff7b50cbc7e",1357u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/BIG_WILO_LEAF.PST","ce76e380514da35925919fea8e359e7f2417ef7b72710c4e70579b596c5df998",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/CAMDROID03.PST","b7da07e4e9e3fe12fa47b9f9c06c5803534ae69c19d9904106060a84dbfa6710",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/CAVEWALL.PST","e5c6c05d0ade16ec9c678ad9a4cbb42e229cbd57083053a9cf536b31c305b86b",10921u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/CORD.PST","91cb74edd2d0cb4f3ca102abea2f06cc5e539f21f6b5fb42b54d42baafdc39a1",1233u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/FACTORY_A.PST","fbf266ce5bef67b77720df6635855e38957519c294a26b99faa985ffb2641aee",9620u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/FACTORY_B.PST","12ea298633a725d4ed622b6cdacd5a03270ed9271050e2ad2a428bcb9d35a518",8872u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/FACTORY_C.PST","ad6d34dc378bbf42f95305ae86e04c3433c93bc75a0b6b77f9dc4624d1b61289",9344u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/FACTORY_E.PST","2b07e7a94930bd8f66d73433d473caca90f20cf91d858f33bf8873f76ae3af2f",8882u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/FACTORY_F.PST","bf7052839190d5610d20084206782a218ef50d59d6281797c6da7a994ea108fa",2165u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/FIENCE.PST","c6c1f848a8efd2b2f7af8d3be5fa8d13f76fea11fd1ffadef4c460d29f30f00f",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/FIENCE01.PST","f9176e34712360b11373eaf86aa6db9633f7b227a231d5c46cf2dc274b09aca3",10851u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/FORTRESS_A.PST","1de729fa3e4a5cdaed0670f65364f048ca85f3ab5c0d9e4d56e088429d151368",9085u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/FORTRESS_B.PST","f0c59d40b425b6bcc874c311ac9a4a1f20fbcbf1da12da854bbed369e54b076e",9864u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/FORTRESS_C.PST","d8bca472099393b839d41f53bed9b8b6e41247c8c85ae7c615ff39a0b7af8f7d",10201u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/FORTRESS_D.PST","d57939c1dd162368744a3519d53d8157ccb8f9d81cd1d969821afbbadd95453d",6528u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/FORTRESS_F.PST","32b25b9b80745df8423e47f33d158fc37dbf10c7228724bf686af6706944cfdf",1642u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/FORTTRESS_TEX_A.PST","357354ef1a3d86429316c1f7961d1b39b33409458cd77103839f72cbc5c23737",47974u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/FORTTRESS_TEX_B.PST","c81c840b1e48d08d9467ccab47f8532c6a14c7e07a5abf90366816257778339b",42550u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/GAM_TREE_G.PST","c90d605453bbc2b777301ed6c31bc0e098f67a3600162b2ec439331601a74ae5",14328u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/GROUNDTEX.PST","07b539db3f785a3943e16daa1b3851900da5e64abd5449c61b9195301f932377",32184u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/GUN_A.PST","44b970fad0ceddcd714cc2a0bc8b65d5f189015feecec947e353e0fd6c36b5f5",8567u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/GUN_C.PST","c61a4497f1db2a385a713b96423fb2cde0dc3cc88ef8e422ed77ea1cc94337b5",7450u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/GUN_D.PST","227a4c0d708fddee5501e4480d127e081ef09ee5b85ad12b8721072cd4aaf0a8",528u,32u,32u,1u,2u,1u,0u},
    {"TEXTURES/HOUSE_STAND_A.PST","255ba73e703696f54d12409a690106057b3ca4b4efd9fc99794f5950338a31ea",8574u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/HOUSE_STAND_B.PST","d3687efd23769f34062e377df776f4ebff581164f6f4057fc890f9eeb5717595",9035u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/HOUSE_STAND_C.PST","55b7cc2de46dd17ad70bda09f6cec9211c4fbe6bc1a9bb4b87d209c35b13077a",8804u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/HOUSE_STAND_D.PST","db3c0e6449e5bbff5be29cb50d7f3d8c7fec369e1c1c46166f078a3d5930ab8c",9412u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/HOUSE_STAND_E.PST","fe193d5eea5c2e7750c1a29e881be3bada46a0590aab9bc695ded9efcb9313a9",2156u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/JAG_ROCK.PST","e5c6c05d0ade16ec9c678ad9a4cbb42e229cbd57083053a9cf536b31c305b86b",10921u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT.PST","5ab25e26356a00b8afd72e63e8dac92e0925a4108dd7aa1bfa5b2af35bea4d38",9033u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT01.PST","1623d8d37d7d1c655a9e789008386003bbc533d96cdb9f74005642c227cd059e",8373u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT02.PST","738f9957fc7dc79de8e9255827e6c9425a731210f4c470823d706e94be236619",1339u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT03.PST","a9b28e3c32cabb777aa70e4db4edef48129a98489620ee26f054fb0a58472c6f",8896u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT04.PST","70c26d6e16d613b233cd646ca5fe7f6d85d4cf8414b985599340f2fc7ade8101",8771u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT05.PST","2e13b96e6bc7c53a521fbb44138ee5078d7e892e6766212e2fa7b23fca8b3303",8830u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT06.PST","c46e2cc224caa55ee380e655231e2855deb70787e7aa2c1d7415d3d3f21a2c39",8075u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT_A.PST","cdc611f18debd46548862dfc490790261115905a22371bdaa90dcf64079d9d32",7888u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT_DOOR.PST","99460601bdea037f3485b559c4fdd22144f221d44f93b610bdef194550f9111f",3164u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MARKET_HUT_ROOF.PST","f409e942de70fc5e072f6700b38aa61db78ed0ef1e5366f849e592ff223097fe",10046u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/MARKET_TABLE.PST","77b93538187044b18da50389eb6f12850326dcbf44908de925318659508710fc",2435u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MARKET_TABLE_ROOF.PST","bba2c59e2cdc4e85ebc4fee0386e4fa550d1c1a5dd77466e84b28f558f11b12f",6840u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/METAL_HUT_SEP.PST","6a7fc0be2d4ea5f1fcbecdf535d05a5c716e7affd75e33ccbb81a67e8490345f",11649u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/METAL_HUT_SEP01.PST","e7f7cd61dc890d2c51fd58942203fedb8e8c747e1b9877fcd06aa2ce8d834208",9847u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/METAL_HUT_SEP02.PST","8bb73a28a35d2d7889afb7a26b669b8fd5804d0c61fd3a6d3b35b64aa2962ca3",2515u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/METAL_HUT_SEP03.PST","b3ae66a140fd997b2b45aa7aa930ed7c3f379b528b6e85ed8a18ea38886d72bb",2908u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_A.PST","78d1e22d4c7b2d98a1a68c8b392ca6c2e5e6f1613649312f5d25f1f629eeb052",2562u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_B.PST","e039b4a7b8417e9da2c8a2936afded67625310332585b0c792efada6a13fa290",3190u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_C.PST","8c62c4edac461a6287de87b148f92c2a3bf909cfc8c34addac165d88637c9f30",3167u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_D.PST","d944a3f839625bf71849481a53df52f88ce28c95535f8aa80c37072b161f25b7",3330u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_E.PST","7c32d6067b65245ce14e393b7de44dbd0304c8f704f8a97270201b8cfb909b53",2697u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_F.PST","2d3b6dea67e398b773959e68737127af4181e8eaa329e338aaae676163a78f14",2721u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_G.PST","049eb95c6d796ef716b02617c68bcd7b365737c903264472c0feb69c917c7fa4",3007u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_H.PST","23925eef1154172e7fb0d8e97376e6bf4ba231fc8aad947c77563ee6ee28f1f7",2098u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_I.PST","c8651f603780ec91d0dc9ba9a946e37f3b118c081b4521038b016b45778738aa",3075u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_J.PST","10d3284e656ce6d8f02807bce3ec19ae84d1d13783fdc8e43fa17a15ed8735e5",1860u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_K.PST","44c63e4331908d32584bc1f78424eb14152b556bbf4249a6b7c6bd2354da165c",2619u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_L.PST","c472e2684459294612e6784bc9852473a95e07f30bc512eebf9dca6d0126a484",2323u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_M.PST","419edf40d9b4f60538f1b59f63964b2ec24f27b30c6e34bca0e09c6e47ca94ce",2880u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/MINING_DROID_N.PST","788461ab09d79e58c35370d038331406c2c1c252ba5794163143982c98bcac81",2445u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/PIG_HEAD.PST","0f027aca3f9129cf15d0a0ed0cf2173731a58b867929801fa9806327578d6dc1",6398u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/PLANKS_WOOD_A.PST","29e0db6c3c4a87c7b9970d1a641efcd7387dcbb9e5c0cf6f4f24388ef30536bc",9850u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/ROADTEX.PST","ca6c91f431d51c413d260f7df822688830c7833716838fefec6066806e932651",23205u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/SMOKE_STACK_A.PST","a146ed57fe5f129273b59ece788e1a3885fbe818c3aa25ff9d1634a03d1700f4",8901u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/SMOKE_STACK_B.PST","3ff5e37841594216fc9e64b667d604f509b3ec80ca4d61adb0d336620fb4bad6",9557u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/SMOKE_STACK_C.PST","c2a5d67c5ee818aeebb9cf080b3bfe0b0998001d7788ac2d42d84f7ca2bd7572",8322u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/SMOKE_STACK_D.PST","ac6dfaa37c15ac7aa0bc568f947c6040ab0e46c264e96e5e220f18b00dcd8865",2099u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/SURFACETEX.PST","07b7a917a101687a1283351eab9925a531a70d8c9c8843e92bf770353df53b69",29642u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/S_FLAGA.ATM","bf418dae2372ddfdd3c5cbe7de9c3ab62efa60af841de7e1b87959271896904a",228612u,128u,128u,3u,1u,10u,1u},
    {"TEXTURES/S_FLAGB.ATM","96ab2ddbc4e174eca0edc6059db4123d6fea53125d13903d831ba2489687732c",228612u,128u,128u,3u,1u,10u,5u},
    {"TEXTURES/S_FLAGC.ATM","c4d8c0c15ce89d73f582eecf3b1c77549cbd437937f5981f978056dedc244fa2",228612u,128u,128u,3u,1u,10u,3u},
    {"TEXTURES/TREE_BRANCH_A.PST","d2220ac297dfd5eade6f1272b099e889d874437499e86d4882c9e066ace81a1b",7087u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/VILLAGE_A.PST","29557fafec7c4c60a4b6deace54dabc178829f4daae52f6a1fa1c9e3d0645187",6833u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/VILLAGE_B.PST","c7e2a984aa121e47e196e3a1143bf8ecb6cb7aa9952d5350e1baa8c0f319c2ce",2589u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/VILLAGE_C.PST","d69cf91ab82b9bfc1233a86b3a2a07c71c5dfbab0e702ae32e9ec8c569cd29a2",6831u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/VILLAGE_D.PST","550f48c6af73644d7c28396e5801a658d843b16dda88d0c0845275ef6e278ad2",2248u,64u,64u,2u,2u,1u,0u},
};
static_assert(std::size(sourceImages)==79,"Exact GA static image source closure");
inline std::string key(std::string name){return shared::normalized(std::move(name));}
inline Json fields(const a::MaterialFields48E& value){
    Json out={{"authored",Json::array()},{"unspecified",Json::array()}};
    for(const auto* list:{&value.authored,&value.unspecified})for(const auto& item:*list)
        out[list==&value.authored?"authored":"unspecified"].push_back(
            {{"name",item.name},{"value",item.value},{"source_offset",item.source_offset}});
    return out;
}
inline std::string field(const a::MaterialFields48E& value,const char* name){
    const auto wanted=key(name);std::string result;bool found=false;
    for(const auto& item:value.authored)if(key(item.name)==wanted){
        shared::need(!found,"Duplicate original GA material field");found=true;result=item.value;
    }
    return result;
}
inline bool clamp(const a::MaterialFields48E& value,const char* name){
    const auto text=field(value,name);
    shared::need(text.empty()||text=="0"||text=="1","Unsupported original GA texture wrap mode");
    return text=="1";
}
inline a::detail::OriginalIpumMipExtent50LA framePst(const Bytes& source,std::uint32_t& mipIndex){
    const auto range=[&](std::size_t at,std::size_t n){return at<=source.size()&&n<=source.size()-at;};
    const auto word=[&](std::size_t at){shared::need(range(at,4),"Truncated original GA IPUM PST");return shared::u32(source.data()+at);};
    const auto dimension=[](std::uint32_t n){return n>=16&&n<=1024&&!(n&(n-1));};
    shared::need(source.size()<=8u*1024u*1024u&&word(0)==1&&word(4)==2&&word(8)>=1&&word(8)<=5,
        "Original GA IPUM PST header rejected");
    const auto count=word(8);std::size_t at=12;std::uint32_t priorW=0,priorH=0;
    a::detail::OriginalIpumMipExtent50LA largest;
    for(std::uint32_t i=0;i<count;++i){
        shared::need(range(at,20),"Original GA IPUM mip header extent");
        const auto h=word(at),w=word(at+4),bytes=word(at+16);const auto p=at+20;
        shared::need(dimension(w)&&dimension(h)&&!word(at+8)&&!word(at+12)&&
            (!i||(w==priorW*2&&h==priorH*2))&&bytes>=25&&range(p,bytes),"Original GA IPUM mip extent/order");
        shared::need(!std::memcmp(source.data()+p,"ipum",4)&&word(p+4)==bytes-8&&
            (std::uint32_t(source[p+8])|(std::uint32_t(source[p+9])<<8))==w&&
            (std::uint32_t(source[p+10])|(std::uint32_t(source[p+11])<<8))==h&&
            word(p+12)==1&&source[p+16]==0,"Original GA IPUM frame header");
        constexpr std::uint8_t end[]{0,0,1,0xb0,0,0,1,0xb1};
        shared::need(!std::memcmp(source.data()+p+bytes-8,end,8),"Original GA IPUM terminators");
        largest={w,h,at,p,bytes};at=p+bytes;priorW=w;priorH=h;
    }
    shared::need(at==source.size(),"Original GA IPUM PST trailing bytes");mipIndex=count-1;return largest;
}
inline const SourceImage* sourceImage(const std::string& member){
    const auto name=key(member);const SourceImage* found=nullptr;
    for(const auto& entry:sourceImages)if(key(entry.member)==name){
        shared::need(!found,"Ambiguous original GA source image pin");found=&entry;
    }
    return found;
}
inline std::uint32_t initialFrame(const a::MaterialFields48E& value){
    const auto text=field(value,"animatedMapInitialFrame");if(text.empty())return 0;
    std::uint32_t result=0;const auto parsed=std::from_chars(text.data(),text.data()+text.size(),result);
    shared::need(parsed.ec==std::errc{}&&parsed.ptr==text.data()+text.size()&&result<64,
        "Original GA ATM initial frame syntax/extent");return result;
}
// Decode only supplied original bytes. The optional output records whether an
// already retained independent image hash was available and checked. Ordinary
// image hashes are recorded after decoding, never used to select pixel data.
inline Bytes image(const std::string& member,const Bytes& source,std::uint32_t& mipIndex,
    bool& animated,a::DecodeError48C& error,bool* independentlyPinned=nullptr){
    shared::need(source.size()>=12&&source.size()<=8u*1024u*1024u,"Original GA texture source extent");
    const auto name=key(member);const auto* pin=sourceImage(name);
    const source_texture50fc_fixture::detail::Entry* outputPin=nullptr;
    for(const auto& entry:source_texture50fc_fixture::detail::gaEntries)if(key(entry.member)==name){
        shared::need(!outputPin,"Ambiguous original GA decoded image pin");outputPin=&entry;
    }
    shared::need(pin||outputPin,"Original GA image has no authenticated source identity");
    const auto sourceSha=shared::sha256(source);
    if(pin)shared::need(source.size()==pin->sourceBytes&&sourceSha==pin->sourceSha256,
        "Original GA static image source identity differs");
    if(outputPin)shared::need(source.size()==outputPin->sourceBytes&&sourceSha==outputPin->sourceSha256,
        "Original GA exceptional image source identity differs");
    animated=name.ends_with(".ATM");
    shared::need(animated||name.ends_with(".PST"),"Unsupported original GA image extension");
    shared::need(!animated||(pin&&source.size()>=24&&pin->initialFrame<pin->frameCount),
        "Original GA ATM lacks its exact frame selection");
    const auto frameIndex=pin?pin->initialFrame:0u;
    const auto kind=shared::u32(source.data()+(animated?16:4));
    if(pin)shared::need(kind==pin->kind,"Original GA source image kind differs");
    Bytes output;
    if(kind==2){
        a::detail::OriginalIpumMipExtent50LA mip;
        if(animated){
            std::vector<a::detail::OriginalIpumFrameExtent50LA> frames;std::size_t bad=0;
            shared::need(a::detail::frameOriginalIpumAtm50LA(source,frames,bad)&&
                frames.size()==pin->frameCount&&frameIndex<frames.size(),
                "Original GA IPUM ATM framing failed at "+std::to_string(bad));
            const auto& frame=frames[frameIndex];mipIndex=frame.count-1;mip=frame.mips[mipIndex];
        }else mip=framePst(source,mipIndex);
        a::TextureMip48E decoded;decoded.width=mip.width;decoded.height=mip.height;std::string why;
        shared::need(a::decodeOriginalIpuRgba(source.data()+mip.payload,mip.bytes,mip.width,mip.height,decoded.rgba,why),
            "Original GA IPUM decode: "+why);
        shared::need(a::encodeTextureLevel48E(decoded,output,error),"Original GA IPUM output framing: "+error.code);
    }else{
        shared::need(kind==0||kind==1,"Original GA indexed texture kind");Bytes pst;
        if(animated){
            a::OriginalUiAtm50KG frames;
            shared::need(a::splitOriginalUiAtm50KG(source.data(),source.size(),frames,error)&&
                frames.frames.size()==pin->frameCount&&frameIndex<frames.frames.size(),
                "Original GA indexed ATM framing: "+error.code);
            const auto& frame=frames.frames[frameIndex];
            pst.assign(source.begin()+frame.pstOffset,source.begin()+frame.pstOffset+frame.pstBytes);
        }
        a::Texture48E decoded;
        shared::need(a::decodeTexture48E(animated?pst:source,decoded,error)&&!decoded.mips.empty(),
            "Original GA indexed PST decode: "+error.code);
        mipIndex=static_cast<std::uint32_t>(decoded.mips.size()-1);output=std::move(decoded.base_rrtex);
    }
    const auto w=pin?pin->width:outputPin->width,h=pin?pin->height:outputPin->height;
    const auto expectedMip=pin?pin->mipIndex:outputPin->mipIndex;
    shared::need(mipIndex==expectedMip&&output.size()==std::size_t(w)*h*4+16&&
        !std::memcmp(output.data(),"RRTEX001",8)&&shared::u32(output.data()+8)==w&&shared::u32(output.data()+12)==h,
        "Original GA decoded image source framing differs");
    if(outputPin)shared::need(mipIndex==outputPin->mipIndex&&w==outputPin->width&&h==outputPin->height&&
        shared::sha256(output)==outputPin->outputSha256,"Original GA independently pinned decoded image differs");
    if(independentlyPinned)*independentlyPinned=outputPin!=nullptr;
    return output;
}
// The retained original byte buffer alone does not authenticate mutable parsed
// fields or their cached typed order. Re-encode the entire parameter document,
// then compare every typed/source-provenance field with a fresh exact decode.
inline void validateBorrowedWorld(const Bytes& scb,const a::World48F& world,a::DecodeError48C& error){
    Bytes roundtrip;a::World48F canonical;
    shared::need(world.parameters.original_bytes==scb&&
        a::encodeParameters48F(world.parameters,roundtrip,error)&&roundtrip==scb&&
        a::decodeWorld48F(scb,canonical,error)&&a::validateWorldSemantics48F(world,error),
        "Original GA borrowed world fields differ from authenticated SCB");
    const auto& actual=world.parameters;const auto& source=canonical.parameters;
    shared::need(actual.version==source.version&&actual.header_table_offset==source.header_table_offset&&
        actual.preservesDuplicateLabels==source.preservesDuplicateLabels&&actual.blocks.size()==source.blocks.size(),
        "Original GA parameter provenance header differs");
    for(std::size_t i=0;i<source.blocks.size();++i){
        const auto& x=actual.blocks[i];const auto& y=source.blocks[i];
        shared::need(x.name==y.name&&x.name_offset==y.name_offset&&x.table_entry_offset==y.table_entry_offset&&
            x.offset==y.offset&&x.size_without_count==y.size_without_count&&x.fields.size()==y.fields.size(),
            "Original GA parameter block provenance differs");
        for(std::size_t j=0;j<y.fields.size();++j){const auto& f=x.fields[j];const auto& g=y.fields[j];
            shared::need(f.name==g.name&&f.value==g.value&&f.entry_offset==g.entry_offset&&
                f.value_offset==g.value_offset&&f.value_bytes==g.value_bytes,
                "Original GA parameter field provenance differs");
        }
    }
    shared::need(world.scene_info_block==canonical.scene_info_block&&world.instances.size()==753&&
        world.instances.size()==canonical.instances.size()&&world.zones.size()==12&&
        world.zones.size()==canonical.zones.size()&&world.portals.size()==18&&world.portals.size()==canonical.portals.size(),
        "Original GA typed world source extent differs");
    for(std::size_t i=0;i<canonical.zones.size();++i){const auto& x=world.zones[i];const auto& y=canonical.zones[i];
        shared::need(x.zone==y.zone&&x.models==y.models&&x.parameter_block==y.parameter_block,
            "Original GA typed zone order/provenance differs");
    }
    const auto same=[](const auto& x,const auto& y){
        return !std::memcmp(x.data(),y.data(),sizeof(x[0])*x.size());
    };
    for(std::size_t i=0;i<canonical.instances.size();++i){const auto& x=world.instances[i];const auto& y=canonical.instances[i];
        shared::need(x.section==y.section&&x.geometry_member==y.geometry_member&&x.collision_member==y.collision_member&&
            x.zone==y.zone&&x.ordinal==y.ordinal&&x.parameter_block==y.parameter_block&&
            x.source_block_offset==y.source_block_offset&&x.use_lighting==y.use_lighting&&
            same(x.position,y.position)&&same(x.look,y.look)&&same(x.up,y.up)&&same(x.matrix,y.matrix),
            "Original GA typed instance order/provenance differs");
    }
    for(std::size_t i=0;i<canonical.portals.size();++i){const auto& x=world.portals[i];const auto& y=canonical.portals[i];
        shared::need(x.section==y.section&&x.parameter_block==y.parameter_block&&x.zones==y.zones,
            "Original GA typed portal order/provenance differs");
        for(std::size_t j=0;j<y.vertices.size();++j)shared::need(same(x.vertices[j],y.vertices[j]),
            "Original GA typed portal vertices differ");
    }
}
} // namespace detail

// Compatible direct-source callback for separately owned GA dynamic materials.
// Never reads a retained image filename; failures preserve the caller's image.
inline bool decodeSourceTexture(const char* member,const Bytes& source,Bytes& output,
    std::uint32_t& mipIndex,std::string& error)noexcept {
    try{
        shared::need(member&&*member,"Original GA source texture member required");
        a::DecodeError48C diagnostic;std::uint32_t mip=0;bool animated=false;
        auto next=detail::image(member,source,mip,animated,diagnostic);
        output=std::move(next);mipIndex=mip;error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="Original GA source texture decode failed";}catch(...){}return false;}
}

class Owner final {
    struct Scene {a::Scene48D decoded;std::uint64_t identity=0;std::vector<std::uint32_t> materials;};
    struct Storage {
        a::ResArchive48C archive;
        std::map<std::string,std::size_t> entries;
        std::map<std::string,Scene> scenes;
        std::vector<a::Material48E> materials;
        std::vector<std::vector<std::uint32_t>> materialPasses;
        std::vector<std::uint8_t> independentlyPinnedImages;
        a::Mesh48B mesh;
        std::vector<a::WorldNormal50HK> normals;
        std::vector<a::TrackProjectedUv> projectionUvs;
        a::MaterialTextureAssets50EA bank;
        std::vector<tm::Span> spans;
        std::vector<tm::Selection> selections;
        std::vector<tm::TextureSelection> textures;
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> canonicalDomain;
        bool canonicalAttempted=false;
        const a::ResourceEntry48C* entry(const std::string& name)const{
            const auto found=entries.find(detail::key(name));
            return found==entries.end()?nullptr:&archive.entries[found->second];
        }
        Bytes member(const a::ResourceEntry48C& entry,a::DecodeError48C& error)const{
            Bytes bytes;shared::need(entry.bytes&&entry.bytes<=8u*1024u*1024u&&
                a::extractResMember48C(archive,entry.name,bytes,error,entry.bytes)&&bytes.size()==entry.bytes,
                "Original GA archive member extraction: "+entry.name+" "+error.code);return bytes;
        }
    };
    std::unique_ptr<Storage> storage_;
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return bool(storage_);}
    const a::Mesh48B* mesh()const noexcept{return storage_?&storage_->mesh:nullptr;}
    const std::vector<a::WorldNormal50HK>* normals()const noexcept{return storage_?&storage_->normals:nullptr;}
    const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
    const std::vector<tm::Span>* spans()const noexcept{return storage_?&storage_->spans:nullptr;}
    const std::vector<tm::Selection>* selections()const noexcept{return storage_?&storage_->selections:nullptr;}
    const std::vector<tm::TextureSelection>* textures()const noexcept{return storage_?&storage_->textures:nullptr;}
    const std::vector<a::Material48E>* materials()const noexcept{return storage_?&storage_->materials:nullptr;}
    const std::vector<std::uint8_t>* independentlyPinnedImages()const noexcept{return storage_?&storage_->independentlyPinnedImages:nullptr;}
    const std::vector<a::TrackProjectedUv>* projectionUvs()const noexcept{return storage_?&storage_->projectionUvs:nullptr;}

    bool load(const Bytes& original,const a::World48F& world,Diagnostic& d)noexcept{
        try{
            d={};shared::need(!storage_&&std::fegetround()==FE_TONEAREST,"GA direct cold owner already loaded or rounding mode differs");
            shared::need(original.size()==archiveBytes&&shared::sha256(original)==archiveSha,"Original GA archive identity differs");
            auto next=std::make_unique<Storage>();d.stage="archive";
            a::DecodeLimits48C limits;limits.resource_file_bytes=archiveBytes;
            limits.resource_decoded_bytes=watchtower_source_fixture::decodedBytes;
            limits.resource_entries=watchtower_source_fixture::archiveEntries;
            shared::need(a::decodeResource48C(original,next->archive,d.decode,nullptr,limits)&&next->archive.entries.size()==970&&next->archive.decoded.size()==26099712,
                "Original GA archive decode: "+d.decode.code);
            for(std::size_t i=0;i<next->archive.entries.size();++i)
                shared::need(next->entries.emplace(detail::key(next->archive.entries[i].name),i).second,"Ambiguous original GA member");
            d.stage="world";const auto* scb=next->entry("tracks\\ga.scb");
            shared::need(scb&&scb->bytes==watchtower_source_fixture::scene.bytes,"Original GA SCB extent differs");
            const auto scbBytes=next->member(*scb,d.decode);
            shared::need(shared::sha256(scbBytes)==watchtower_source_fixture::scene.sha,"Original GA SCB identity differs");
            detail::validateBorrowedWorld(scbBytes,world,d.decode);
            std::map<std::string,std::uint32_t> materialIndices,textureIndices;
            std::vector<Bytes> images;
            std::vector<a::MaterialTextureSlotInput50EA> slots;
            constexpr std::uint64_t textureBase=0x4741000000000000ull;
            std::size_t rgbaBytes=0,serializedBytes=0;
            auto material=[&](const std::string& materialName)->std::uint32_t{
                const auto normalized=detail::key(materialName);const auto existing=materialIndices.find(normalized);
                if(existing!=materialIndices.end())return existing->second;
                shared::need(shared::name(materialName),"Invalid original GA material basename");
                d.stage="material";const auto* psm=next->entry("textures\\"+materialName+".psm");
                shared::need(psm,"Original GA world material absent: "+materialName);d.member=psm->name;
                const auto source=next->member(*psm,d.decode);a::Material48E authored;
                shared::need(a::decodeMaterial48E(source,authored,d.decode)&&!authored.passes.empty()&&authored.passes.size()<=2,
                    "Original GA world material subset: "+d.member+" "+d.decode.code);
                const auto materialIndex=static_cast<std::uint32_t>(next->materials.size());
                shared::need(materialIndex<308,"Original GA PSM count bound");
                const auto materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+shared::sha256(source));
                std::vector<std::uint32_t> selectedPasses;
                for(std::size_t pi=0;pi<authored.passes.size();++pi){
                    const auto& pass=authored.passes[pi];const bool projected=pass.microprogram=="ProjectedPrelit";
                    shared::need(pass.pass==pi+1&&pass.textures.size()==1&&pass.textures[0].slot==1&&
                        ((authored.passes.size()==2&&projected)||
                         (authored.passes.size()==1&&!projected&&
                          (pass.microprogram=="PreLitCompressed"||pass.microprogram=="PrelitCompressed"||pass.microprogram=="DiffuseCompressed"))),
                        "Unjoined original GA pass/program/texture subset");
                    const auto& map=pass.textures[0];shared::need(shared::name(map.name),"Invalid original GA texture basename");
                    const a::ResourceEntry48C* texture=nullptr;
                    for(const char* extension:{".pst",".ipu",".tga",".atm"}){
                        texture=next->entry("textures\\"+map.name+extension);if(texture)break;
                    }
                    shared::need(texture,"Original GA required texture absent: "+map.name);
                    const auto textureKey=detail::key(texture->name);const auto* pin=detail::sourceImage(textureKey);
                    shared::need(pin,"Original GA static image lacks exact source pin");
                    const bool animated=textureKey.ends_with(".ATM");
                    if(animated)shared::need(authored.passes.size()==1&&detail::initialFrame(pass.fields)==pin->initialFrame,
                        "Original GA material/ATM initial frame differs");
                    auto found=textureIndices.find(textureKey);
                    if(found==textureIndices.end()){
                        d.stage="texture";d.member=texture->name;const auto bytes=next->member(*texture,d.decode);
                        std::uint32_t mip=0;bool decodedAnimated=false,independent=false;
                        auto image=detail::image(texture->name,bytes,mip,decodedAnimated,d.decode,&independent);
                        shared::need(decodedAnimated==animated,"Original GA texture animation source classification");
                        const auto index=static_cast<std::uint32_t>(images.size());
                        shared::need(index<79&&image.size()>=16,"GA original texture bank bound");
                        const auto w=shared::u32(image.data()+8),h=shared::u32(image.data()+12);
                        shared::need(w&&h&&w<=1024&&h<=1024&&image.size()==std::size_t(w)*h*4+16,"GA native image extent");
                        shared::add(rgbaBytes,std::size_t(w)*h*4,16u*1024u*1024u,"GA texture RGBA budget");
                        shared::add(serializedBytes,image.size(),16u*1024u*1024u,"GA texture framing budget");
                        tm::TextureSelection selected;selected.identity=textureBase+index+1;selected.bankIndex=index;
                        selected.mipIndex=mip;selected.width=w;selected.height=h;selected.path=texture->name;
                        selected.key="disc:"+textureKey+":"+pin->sourceSha256;
                        selected.sha256=shared::sha256(image); // Actual runtime output identity, not necessarily an independent pin.
                        next->textures.push_back(std::move(selected));images.push_back(std::move(image));
                        next->independentlyPinnedImages.push_back(independent?1:0);
                        if(independent)++d.independentlyPinnedImages;else ++d.runtimeIdentityImages;
                        found=textureIndices.emplace(textureKey,index).first;
                    }
                    const auto index=static_cast<std::uint32_t>(next->selections.size());
                    shared::need(index<542,"Original GA authored pass count bound");
                    const auto& image=next->textures[found->second];tm::Selection selected;
                    selected.assetIdentity=0x4741;selected.materialIndex=materialIndex;selected.pass=pass.pass;selected.slot=1;
                    selected.bankSlot=static_cast<std::uint32_t>(slots.size());selected.textureIndex=found->second;selected.mipIndex=image.mipIndex;
                    selected.materialName=materialName;selected.materialKey=materialKey;
                    selected.program=pass.microprogram;selected.textureName=map.name;selected.textureKey=image.key;
                    selected.track=track_source_fixture::Track::ga;selected.passCount=static_cast<std::uint32_t>(authored.passes.size());
                    selected.clampU=detail::clamp(pass.fields,"wrapModeHorizontal");selected.clampV=detail::clamp(pass.fields,"wrapModeVertical");
                    selected.sourcePassFields=detail::fields(pass.fields);selected.staticAnimationFrame=animated;
                    selected.projectedPrelit=projected;
                    if(projected)selected.texGenMatrix=tm::detail::projection(selected.sourcePassFields);
                    selected.decodedSourceMember=texture->name;selected.decodedSourceSha256=pin->sourceSha256;
                    slots.push_back({{selected.assetIdentity,materialIndex,pass.pass,1},image.identity,a::MaterialTextureRequirement50EA::required});
                    selectedPasses.push_back(index);next->selections.push_back(std::move(selected));
                }
                next->materials.push_back(std::move(authored));next->materialPasses.push_back(std::move(selectedPasses));
                materialIndices.emplace(normalized,materialIndex);return materialIndex;
            };
            constexpr std::array<double,16> identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
            for(std::size_t wi=0;wi<world.instances.size();++wi){
                const auto& placement=world.instances[wi];d.stage="scene";d.member=placement.geometry_member;
                const auto* entry=next->entry(placement.geometry_member);
                if(!entry){shared::need(next->entry(placement.collision_member),"GA placement lacks both original PSG and COL");++d.collisionOnly;continue;}
                const auto name=detail::key(entry->name);auto found=next->scenes.find(name);
                if(found==next->scenes.end()){
                    Scene scene;scene.identity=next->scenes.size()+1;
                    const auto originalScene=next->member(*entry,d.decode);
                    shared::need(a::decodeScene48D(originalScene,scene.decoded,d.decode),
                        "Original GA PSG decode: "+entry->name+" "+d.decode.code);
                    shared::need(scene.decoded.nodes.size()==1&&scene.decoded.nodes[0].parent==-1&&
                        !scene.decoded.identity_matrix_palette&&!scene.decoded.lods.empty(),"Original GA static hierarchy subset");
                    for(const auto& name:scene.decoded.materials)scene.materials.push_back(material(name));
                    found=next->scenes.emplace(name,std::move(scene)).first;
                }
                const auto& scene=found->second;const auto& matrix=scene.decoded.world_space?identity:placement.matrix;
                a::WorldNormalTransform50HK normalTransform;
                shared::need(a::prepareWorldNormalTransform50HK(matrix,normalTransform),"Original GA visual/normal transform");
                for(std::size_t si=0;si<scene.decoded.lods.front().surfaces.size();++si){
                    const auto& surface=scene.decoded.lods.front().surfaces[si];d.stage="surface";d.member=entry->name;
                    shared::need(surface.material_index<scene.materials.size()&&!surface.vertices.empty()&&!surface.indices.empty()&&
                        !(surface.indices.size()%3)&&surface.vertices.size()<=a::max_mesh_vertices-next->mesh.vertices.size()&&
                        surface.indices.size()<=a::max_mesh_indices-next->mesh.indices.size(),"GA original surface/material/geometry bounds");
                    tm::Span span;span.vertexStart=static_cast<std::uint32_t>(next->mesh.vertices.size());
                    span.vertexCount=static_cast<std::uint32_t>(surface.vertices.size());span.indexStart=static_cast<std::uint32_t>(next->mesh.indices.size());
                    span.indexCount=static_cast<std::uint32_t>(surface.indices.size());span.worldInstanceIndex=static_cast<std::uint32_t>(wi);
                    span.surfaceIndex=static_cast<std::uint32_t>(si);span.hasOriginalColors=surface.has_original_colors;
                    const auto& selectedPasses=next->materialPasses.at(scene.materials[surface.material_index]);
                    shared::need(!selectedPasses.empty()&&selectedPasses.size()<=2,"Original GA surface material pass closure");
                    for(const auto& source:surface.vertices){
                        shared::need(source.bone==0,"GA original static vertex bone");a::Vertex48B vertex{};
                        for(unsigned axis=0;axis<3;++axis){double value=0;
                            for(unsigned k=0;k<3;++k)value+=source.position_local[k]*matrix[k*4+axis];value+=matrix[12+axis];
                            vertex.position[axis]=static_cast<float>(value);shared::need(std::isfinite(value)&&std::isfinite(vertex.position[axis]),"GA nonfinite original vertex");
                        }
                        for(unsigned axis=0;axis<2;++axis){vertex.uv[axis]=static_cast<float>(source.uv[axis]);shared::need(std::isfinite(vertex.uv[axis]),"GA nonfinite original UV");}
                        for(unsigned axis=0;axis<4;++axis)vertex.color[axis]=surface.has_original_colors?source.color[axis]:255;
                        a::WorldNormal50HK normal;shared::need(a::transformPreparedWorldNormal50HK(source.normal_local,normalTransform,normal),"GA original normal transform");
                        if(!normal.xyz[0]&&!normal.xyz[1]&&!normal.xyz[2])++d.zeroNormals;
                        next->mesh.vertices.push_back(vertex);next->normals.push_back(normal);
                        // Original ProjectedPrelit consumes quantized source-space XYZ,
                        // before the SCB/world transform, for each authored pass.
                        a::TrackProjectedUv uv{{vertex.uv[0],vertex.uv[1],vertex.uv[0],vertex.uv[1]}};
                        for(const auto selectedPass:selectedPasses){
                            const auto& selection=next->selections[selectedPass];
                            if(!selection.projectedPrelit)continue;
                            for(unsigned axis=0;axis<2;++axis){double value=0;
                                for(unsigned k=0;k<3;++k)value+=(source.position_local[k]-scene.decoded.quantization[4+k])*selection.texGenMatrix[k*4+axis];
                                value+=selection.texGenMatrix[12+axis];
                                shared::need(std::isfinite(value)&&std::isfinite(static_cast<float>(value)),"GA projected source UV nonfinite");
                                uv.uv[(selection.pass-1)*2+axis]=static_cast<float>(value);
                            }
                        }
                        next->projectionUvs.push_back(uv);
                    }
                    for(const auto index:surface.indices){shared::need(index<surface.vertices.size(),"GA original surface index");next->mesh.indices.push_back(span.vertexStart+index);}
                    // Duplicate draw spans only. Vertex/index storage stays shared.
                    for(const auto selectedPass:selectedPasses){
                        const auto& selection=next->selections[selectedPass];
                        span.materialSelection=selectedPass;span.textureIndex=selection.textureIndex;
                        span.sourcePass=selection.pass;span.sourcePassCount=selection.passCount;
                        span.projectionPass=selection.projectedPrelit?selection.pass:0;
                        next->spans.push_back(span);if(selection.pass==2)++d.secondPasses;
                    }
                    ++d.surfaces;
                }
                ++d.placements;
            }
            d.scenes=next->scenes.size();d.stage="closure";d.member.clear();
            d.materials=next->materials.size();d.selections=next->selections.size();
            shared::need(d.scenes==298&&d.placements==563&&d.collisionOnly==190&&d.surfaces==993&&
                d.materials==308&&d.selections==542&&d.secondPasses==248&&next->spans.size()==1241&&
                next->mesh.vertices.size()==144521&&next->mesh.indices.size()==258507&&images.size()==79&&
                next->normals.size()==next->mesh.vertices.size()&&next->projectionUvs.size()==next->mesh.vertices.size()&&
                rgbaBytes==4673536&&d.independentlyPinnedImages==5&&d.runtimeIdentityImages==74,
                "Original GA full static source closure differs");
            for(unsigned axis=0;axis<3;++axis){next->mesh.bounds.minimum[axis]=std::numeric_limits<float>::infinity();
                next->mesh.bounds.maximum[axis]=-std::numeric_limits<float>::infinity();}
            for(const auto& vertex:next->mesh.vertices)for(unsigned axis=0;axis<3;++axis){
                next->mesh.bounds.minimum[axis]=std::min(next->mesh.bounds.minimum[axis],vertex.position[axis]);
                next->mesh.bounds.maximum[axis]=std::max(next->mesh.bounds.maximum[axis],vertex.position[axis]);}
            std::vector<a::MaterialTextureAssetInput50EA> textureInput;
            for(std::size_t i=0;i<images.size();++i)textureInput.push_back({next->textures[i].identity,&images[i]});
            a::MaterialTextureAssetsInput50EA input;input.textures=textureInput.data();input.textureCount=static_cast<std::uint32_t>(textureInput.size());
            input.slots=slots.data();input.slotCount=static_cast<std::uint32_t>(slots.size());
            input.budgets={79,542,16u*1024u*1024u,16u*1024u*1024u,1024};
            d.stage="pixel_bank";shared::need(next->bank.load(input,d.bank),"Original GA pixel bank rejected: "+d.bank.parserMessage);
            storage_=std::move(next);d.stage="complete";return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original GA native cold preparation failed";}catch(...){}return false;}
    }

    // Same Application-owned domain as the actual installed racers. This does
    // not register synthetic static models or claim source render dispatch for
    // the native world above. Mounting the real complete GA namespace preserves
    // the provider's local resource lookup before genuine racer population.
    bool populateCanonicalRacers(const tm::Request& request,tm::Diagnostic& d)noexcept{
        try{
            d={};shared::need(storage_&&!storage_->canonicalAttempted,"GA canonical population requires fresh native source owner");
            const bool canonical=bool(request.canonicalDomain);
            shared::need(canonical==bool(request.canonicalContext)&&canonical==(request.canonicalMaterialPath.actualPrefix36B970!=nullptr),
                "GA canonical Application/Video/path must be supplied together");
            if(!canonical){shared::need(request.canonicalRacerModels.empty(),"GA racer sources lack their canonical domain");return true;}
            shared::need(request.track==track_source_fixture::Track::ga&&request.canonicalDomain->prepared()&&
                request.canonicalContext->live()&&*request.canonicalMaterialPath.actualPrefix36B970=="textures\\",
                "GA canonical racers require their actual gameplay directory and live Video");
            storage_->canonicalAttempted=true;storage_->canonicalDomain=request.canonicalDomain;
            const auto& provider=request.canonicalDomain->archiveProvider3E1A70();
            a::OriginalStandardArchiveProvider50LA::Diagnostic mounted;d.stage="ga_original_archive_mount";
            shared::need(provider&&provider->mountAuthenticatedRes("TRACKS/GA.RES",archiveSha,archiveBytes,storage_->archive,mounted)&&
                provider->sealNativeMounts(mounted)&&request.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,d.canonical.texture),
                "Original GA canonical provider mount rejected");
            std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
            std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
            d.stage="ga_original_racer_sources";
            for(const auto& racer:request.canonicalRacerModels)
                shared::need(racer.resource&&racer.references&&racer.references->table382B38()==request.canonicalDomain->materials382B38().get()&&
                    original_racer_material_sources50LA::appendModelSources(*racer.resource,*racer.references,
                        *request.canonicalMaterialPath.actualPrefix36B970,*provider,materials,textures,d.racerSource),
                    "Original GA canonical racer material/texture closure rejected");
            shared::need(request.canonicalDomain->appendOriginalSources(materials,textures,d.canonical),"Original GA canonical racer source admission rejected");
            d.stage="ga_original_racer_population";
            shared::need(request.canonicalDomain->populate(request.canonicalContext,d.canonical),"Original GA canonical racer population rejected");
            return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original GA canonical racer preparation failed";}catch(...){}return false;}
    }

    // Preserve the existing GA one/two-pass policy and actual APPSTART
    // DefaultMT alpha test. Flag omissions inherit that original template.
    bool alpha(std::size_t selection,const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,material_alpha50gx_fixture::Policy& output)const noexcept{
        try{return storage_&&selection<storage_->selections.size()&&
            material_alpha50gx_fixture::selectGa(storage_->selections[selection],templates,runtime,output);}
        catch(...){return false;}
    }
    // Borrow the same authenticated archive for a separate GA ATM cold decoder. The
    // archive lifetime is the world lifetime and cannot be replaced by reload.
    const a::ResArchive48C* archive()const noexcept{return storage_?&storage_->archive:nullptr;}
};

} // namespace ga_original_world50LH
