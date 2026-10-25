#pragma once
#include "../../platform/linux/native/loading_progress50du.h"
#include "track_materials50fa_fixture.h"
#include "material_alpha50gx_fixture.h"
#include "source_texture50fc_fixture.h"

#include "../../src/assets/import_world48f.h"
#include "../../src/assets/original_ta_encoded_archive50la.h"
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

// Reconstructed direct original-disc TA static owner, not a recovered old file.
// All geometry, source normals, materials and pixels come from authenticated
// TA.RES bytes. Indexed pixels have independent scalar Python output pins;
// exceptional images retain pre-existing source_texture50fc output pins.
// ATM start frames only. No TOB behavior, original shader/pixel parity or
// canonical static-model publication is claimed. Racers use the caller's
// real Application-owned domain, never a substitute domain.
namespace ta_original_world50LE {
namespace a=rr::assets;
namespace shared=pod_materials50ed_fixture::detail;
namespace tm=track_materials50fa_fixture;
using Bytes=std::vector<std::uint8_t>;
using Json=nlohmann::json;
inline constexpr const char* archiveSha="b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8";
inline constexpr std::size_t archiveBytes=3334562;
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
// Derived from authenticated TA.RES by analyze.py.
inline constexpr SourceImage sourceImages[]={
    {"TEXTURES/ARROWDROID01.PST","478676a45c866b654d12ad909ba042cc428ec96e1a462ff5824512df1518a5e3",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/ARROWDROID03.PST","abb0c6ad9fa0716e2ee5d4d4fd318cd05c5d1df8ef336acd46b933d884106e07",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/ARROWRED.ATM","dbfdaf3d44918c86254ae9f39f4146cc101c363e1fb9e2cda43bb8c0ea99b220",114312u,128u,128u,3u,1u,5u,0u},
    {"TEXTURES/BANTHA.PST","265aaf132776d1915fb2f69d4da43d475161579816f2f1b76caa2315a7146bb3",6460u,64u,64u,2u,1u,1u,0u},
    {"TEXTURES/BONE.PST","704b06ccc3b809c2e54f50ab48f732836f5735c4b9193845dac92d23d338f87b",6460u,64u,64u,2u,1u,1u,0u},
    {"TEXTURES/CAMDROID03.PST","b7da07e4e9e3fe12fa47b9f9c06c5803534ae69c19d9904106060a84dbfa6710",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/CORD.PST","91cb74edd2d0cb4f3ca102abea2f06cc5e539f21f6b5fb42b54d42baafdc39a1",1233u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/CRAWLERA.PST","0130a604d965a47948f36d02e1b6b64aec3ed4cebcffeb36a389a88454aee7e4",20602u,256u,256u,4u,2u,1u,0u},
    {"TEXTURES/CROWD.ATM","76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031",214578u,256u,256u,4u,2u,6u,0u},
    {"TEXTURES/HORN.PST","1aab8dcb5ecd1e028ce5dcc3bfb037ef3ce0521289f873b8199d3bf7f3d9f55e",2348u,32u,32u,1u,1u,1u,0u},
    {"TEXTURES/HUTR01.PST","80d64caad9b77732c1015eb7b2c5445329b1b41dda9ed190144f11b420a6e447",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/JABBA01.PST","7b01c82a7ed46f50c1070fa44816f265f190555f2eb258de806a989c5afc649f",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/MOSEISLEY1.PST","759e68f68303af6728116f8f98f7a2fce7f2fa65430190423e582b677dbaccfd",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/MOSEISLEY2.PST","f566aadca35803e3182f4a642d5e4dd3eda3ad4e06c9316716f4347e945e82f6",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/POLE01.PST","078ca20a235f83825fdc7672f3dd0f8e44ab03a9b427dc3aee20a17721ac04d1",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/RCLIGHTSTRIP.PST","7fb039aa70dc8a4726b6a09268a5ee6036ab52738a293246486f8b04175f1d86",22860u,128u,128u,3u,1u,1u,0u},
    {"TEXTURES/STSTAND1.PST","0a5ecf2895763813de4c96d67bd4fc268f426f8df34527a9e320c1b27ce5c0a7",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/STSTAND3.PST","5dbf916916a5b8d0e75e538a117ba5fae11c25fae8f6a4b442f28c4874509a22",2348u,32u,32u,1u,1u,1u,0u},
    {"TEXTURES/TAROCK.PST","736313718bcec566fd5a14d93315f16f2282e359855bb2f4b9b9a36502c0c133",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/TA_BLEND4.PST","946b80345e423e32e3898c5d8ed4125054ca8a4204263e0f7fb09c1bda40af14",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/TA_NEWBLND.PST","e6d06ebd30c6dda74593c852d3e322fc2e45364a73039e1a65e99c5dd93fce5d",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/TA_NEWRCK.PST","4fc5f5e41f2c47b3f0f863bdd480c6adf7fc38317ad279750ff5c573ce08aa00",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/TA_ROCK2.PST","74e54a341dd316f67185dc49ce52b3ff62f778aad0f3276b6eb452528c30cb27",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/TA_SAND8.PST","d43ee9e14e4bf4033248b298b4ff55f428394c77cf562a7b90f2ed22e849f63a",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/TA_SANDGROUND.PST","0cb7746e3baa25d49e0e94a4b524165d6c6b7a8b89c10a36afdf72ddb7d21352",88412u,256u,256u,4u,1u,1u,0u},
    {"TEXTURES/TRACK.PST","e27d767a7d2aaa595d97b86b7208658404389540d2265d1bfbcaf2bd09ccbfef",2319u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/TRACKDARK.PST","df48a11d8ca7ad3a4c53e0e2840c4c0df57ce01504751dd5ea62248bfcb6c0c2",2590u,64u,64u,2u,2u,1u,0u},
    {"TEXTURES/TRACKSIDE.PST","4db3e6270ec85d8ddd16af7445befd43bad7e03f61cc9399d9559095ae9d5d02",7302u,128u,128u,3u,2u,1u,0u},
    {"TEXTURES/TRACKWHEEL.PST","de447b0debdad77b00d3f95d0919c0ec9d2a169229a18a61a5df9c722e240265",2209u,64u,64u,2u,2u,1u,0u},
};
static_assert(std::size(sourceImages)==29,"Exact TA static image source closure");
inline constexpr source_texture50fc_fixture::detail::Entry indexedPins[]={
    {"TEXTURES/ARROWDROID01.PST","478676a45c866b654d12ad909ba042cc428ec96e1a462ff5824512df1518a5e3","", "08829e2e578edba9cca357035e8c7e79a2f2b2222b606fdf4e2681992dcfe8d0",22860u,128u,128u,3u},
    {"TEXTURES/ARROWDROID03.PST","abb0c6ad9fa0716e2ee5d4d4fd318cd05c5d1df8ef336acd46b933d884106e07","", "addbd144a72f6cf9f7b9bd763e6ff7c1ec18512c0b827a169e0cd48686f75711",22860u,128u,128u,3u},
    {"TEXTURES/ARROWRED.ATM","dbfdaf3d44918c86254ae9f39f4146cc101c363e1fb9e2cda43bb8c0ea99b220","", "483e1dcbee659193a7aacdd6ba8cca6fd685189ccd12e6ff467624eb74753e79",114312u,128u,128u,3u},
    {"TEXTURES/BANTHA.PST","265aaf132776d1915fb2f69d4da43d475161579816f2f1b76caa2315a7146bb3","", "b6d9352671fd5812871cc00b1c8eccc84d1494c542afc88faae8ed72602f856e",6460u,64u,64u,2u},
    {"TEXTURES/BONE.PST","704b06ccc3b809c2e54f50ab48f732836f5735c4b9193845dac92d23d338f87b","", "de3b84cecebd5baacf64c913aea4840496232254c06f29d15c9625599f31ce53",6460u,64u,64u,2u},
    {"TEXTURES/CAMDROID03.PST","b7da07e4e9e3fe12fa47b9f9c06c5803534ae69c19d9904106060a84dbfa6710","", "0fe0a87019b35dea4584d28bfa3d578d5191ec45e9fa454e3df549d7eedfd7f2",22860u,128u,128u,3u},
    {"TEXTURES/HORN.PST","1aab8dcb5ecd1e028ce5dcc3bfb037ef3ce0521289f873b8199d3bf7f3d9f55e","", "ea09a0f884cf3dda2271065537557e01515405b9ae44ab1049f96e5349e892f6",2348u,32u,32u,1u},
    {"TEXTURES/HUTR01.PST","80d64caad9b77732c1015eb7b2c5445329b1b41dda9ed190144f11b420a6e447","", "89938cd49688918bafc471d6a9db6f0d20c7e7f1699643bb1575a30808176614",88412u,256u,256u,4u},
    {"TEXTURES/JABBA01.PST","7b01c82a7ed46f50c1070fa44816f265f190555f2eb258de806a989c5afc649f","", "bc2887a9c3c7b60470c4d64ba4244e15bf7bdb03f866aaf4748625afa57b0fe2",22860u,128u,128u,3u},
    {"TEXTURES/MOSEISLEY1.PST","759e68f68303af6728116f8f98f7a2fce7f2fa65430190423e582b677dbaccfd","", "5718e081514d4438a32df6e18f5fa144a20f6b9012a8c8d80a07034cdbe1ba24",88412u,256u,256u,4u},
    {"TEXTURES/MOSEISLEY2.PST","f566aadca35803e3182f4a642d5e4dd3eda3ad4e06c9316716f4347e945e82f6","", "9ec118f0df1b07d26d81ac897c1ad9c6e0450dd72b3eebea6c04d370e0071524",88412u,256u,256u,4u},
    {"TEXTURES/POLE01.PST","078ca20a235f83825fdc7672f3dd0f8e44ab03a9b427dc3aee20a17721ac04d1","", "0d39df5eeeb08532264e1b10d036d79017b4b911e481ceeee4ee892e6c7549c9",22860u,128u,128u,3u},
    {"TEXTURES/RCLIGHTSTRIP.PST","7fb039aa70dc8a4726b6a09268a5ee6036ab52738a293246486f8b04175f1d86","", "fd58d018df9a1de2f5d91dc8b25cab31da743ad15f0789cc206bf7c9b76fec59",22860u,128u,128u,3u},
    {"TEXTURES/STSTAND1.PST","0a5ecf2895763813de4c96d67bd4fc268f426f8df34527a9e320c1b27ce5c0a7","", "a421c6e67879837985370d73a5ae7faf605d458875623dae59797e22b83b23a4",88412u,256u,256u,4u},
    {"TEXTURES/STSTAND3.PST","5dbf916916a5b8d0e75e538a117ba5fae11c25fae8f6a4b442f28c4874509a22","", "de04a9cac642238cf122408fb9583ad512409da95a72c52f65116fece8c70414",2348u,32u,32u,1u},
    {"TEXTURES/TAROCK.PST","736313718bcec566fd5a14d93315f16f2282e359855bb2f4b9b9a36502c0c133","", "a773b00f4406afd3b0293811b57632f62fc18c64336a65d66191886034004a6f",88412u,256u,256u,4u},
    {"TEXTURES/TA_BLEND4.PST","946b80345e423e32e3898c5d8ed4125054ca8a4204263e0f7fb09c1bda40af14","", "cf148032cfdbe804b586832fa4831b55284af2beac3343abd1ed02a3a39dae70",88412u,256u,256u,4u},
    {"TEXTURES/TA_NEWBLND.PST","e6d06ebd30c6dda74593c852d3e322fc2e45364a73039e1a65e99c5dd93fce5d","", "6900a6ecdaae7b54d694bbea5fe93c92537c7d7a102742dba6a80a567e0f1277",88412u,256u,256u,4u},
    {"TEXTURES/TA_NEWRCK.PST","4fc5f5e41f2c47b3f0f863bdd480c6adf7fc38317ad279750ff5c573ce08aa00","", "0f313e69fe5922495d6e2e54fa31d6f907e8dde198cfe0a5f77064097e961160",88412u,256u,256u,4u},
    {"TEXTURES/TA_ROCK2.PST","74e54a341dd316f67185dc49ce52b3ff62f778aad0f3276b6eb452528c30cb27","", "b98a11d47668713e0796e1123c52c51d1420ac13d0cd643bf70cc05f880a4669",88412u,256u,256u,4u},
    {"TEXTURES/TA_SAND8.PST","d43ee9e14e4bf4033248b298b4ff55f428394c77cf562a7b90f2ed22e849f63a","", "f9cf879fc2cb3b201ab0907830bfe9deba3465ad4ca2f0639cacbc960e301879",88412u,256u,256u,4u},
    {"TEXTURES/TA_SANDGROUND.PST","0cb7746e3baa25d49e0e94a4b524165d6c6b7a8b89c10a36afdf72ddb7d21352","", "7f6d33b714e19d263d9c0e82e6924c8ceb4fa9c6c43b5e5c7e217e11c5344f72",88412u,256u,256u,4u},
};
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
        shared::need(!found,"Duplicate original TA material field");found=true;result=item.value;
    }
    return result;
}
inline bool clamp(const a::MaterialFields48E& value,const char* name){
    const auto text=field(value,name);
    shared::need(text.empty()||text=="0"||text=="1","Unsupported original TA texture wrap mode");
    return text=="1";
}
inline a::detail::OriginalIpumMipExtent50LA framePst(const Bytes& source,std::uint32_t& mipIndex){
    const auto range=[&](std::size_t at,std::size_t n){return at<=source.size()&&n<=source.size()-at;};
    const auto word=[&](std::size_t at){shared::need(range(at,4),"Truncated original TA IPUM PST");return shared::u32(source.data()+at);};
    const auto dimension=[](std::uint32_t n){return n>=16&&n<=1024&&!(n&(n-1));};
    shared::need(source.size()<=8u*1024u*1024u&&word(0)==1&&word(4)==2&&word(8)>=1&&word(8)<=5,
        "Original TA IPUM PST header rejected");
    const auto count=word(8);std::size_t at=12;std::uint32_t priorW=0,priorH=0;
    a::detail::OriginalIpumMipExtent50LA largest;
    for(std::uint32_t i=0;i<count;++i){
        shared::need(range(at,20),"Original TA IPUM mip header extent");
        const auto h=word(at),w=word(at+4),bytes=word(at+16);const auto p=at+20;
        shared::need(dimension(w)&&dimension(h)&&!word(at+8)&&!word(at+12)&&
            (!i||(w==priorW*2&&h==priorH*2))&&bytes>=25&&range(p,bytes),"Original TA IPUM mip extent/order");
        shared::need(!std::memcmp(source.data()+p,"ipum",4)&&word(p+4)==bytes-8&&
            (std::uint32_t(source[p+8])|(std::uint32_t(source[p+9])<<8))==w&&
            (std::uint32_t(source[p+10])|(std::uint32_t(source[p+11])<<8))==h&&
            word(p+12)==1&&source[p+16]==0,"Original TA IPUM frame header");
        constexpr std::uint8_t end[]{0,0,1,0xb0,0,0,1,0xb1};
        shared::need(!std::memcmp(source.data()+p+bytes-8,end,8),"Original TA IPUM terminators");
        largest={w,h,at,p,bytes};at=p+bytes;priorW=w;priorH=h;
    }
    shared::need(at==source.size(),"Original TA IPUM PST trailing bytes");mipIndex=count-1;return largest;
}
inline const SourceImage* sourceImage(const std::string& member){
    const auto name=key(member);const SourceImage* found=nullptr;
    for(const auto& entry:sourceImages)if(key(entry.member)==name){
        shared::need(!found,"Ambiguous original TA source image pin");found=&entry;
    }
    return found;
}
inline std::uint32_t initialFrame(const a::MaterialFields48E& value){
    const auto text=field(value,"animatedMapInitialFrame");if(text.empty())return 0;
    std::uint32_t result=0;const auto parsed=std::from_chars(text.data(),text.data()+text.size(),result);
    shared::need(parsed.ec==std::errc{}&&parsed.ptr==text.data()+text.size()&&result<64,
        "Original TA ATM initial frame syntax/extent");return result;
}
// Decode only supplied original bytes. The optional output records whether an
// already retained independent image hash was available and checked. Ordinary
// image hashes are recorded after decoding, never used to select pixel data.
inline Bytes image(const std::string& member,const Bytes& source,std::uint32_t& mipIndex,
    bool& animated,a::DecodeError48C& error,bool* independentlyPinned=nullptr){
    shared::need(source.size()>=12&&source.size()<=8u*1024u*1024u,"Original TA texture source extent");
    const auto name=key(member);const auto* pin=sourceImage(name);
    const source_texture50fc_fixture::detail::Entry* outputPin=nullptr;
    for(const auto& entry:source_texture50fc_fixture::detail::entries)if(key(entry.member)==name){
        shared::need(!outputPin,"Ambiguous original TA decoded image pin");outputPin=&entry;
    }
    for(const auto& entry:indexedPins)if(key(entry.member)==name){
        if(outputPin)shared::need(std::string(outputPin->outputSha256)==entry.outputSha256,"TA independent pixel pin disagreement");
        outputPin=&entry;
    }
    shared::need(pin||outputPin,"Original TA image has no authenticated source identity");
    const auto sourceSha=shared::sha256(source);
    if(pin)shared::need(source.size()==pin->sourceBytes&&sourceSha==pin->sourceSha256,
        "Original TA static image source identity differs");
    if(outputPin)shared::need(source.size()==outputPin->sourceBytes&&sourceSha==outputPin->sourceSha256,
        "Original TA exceptional image source identity differs");
    animated=name.ends_with(".ATM");
    shared::need(animated||name.ends_with(".PST"),"Unsupported original TA image extension");
    shared::need(!animated||(pin&&source.size()>=24&&pin->initialFrame<pin->frameCount),
        "Original TA ATM lacks its exact frame selection");
    const auto frameIndex=pin?pin->initialFrame:0u;
    const auto kind=shared::u32(source.data()+(animated?16:4));
    if(pin)shared::need(kind==pin->kind,"Original TA source image kind differs");
    Bytes output;
    if(kind==2){
        a::detail::OriginalIpumMipExtent50LA mip;
        if(animated){
            std::vector<a::detail::OriginalIpumFrameExtent50LA> frames;std::size_t bad=0;
            shared::need(a::detail::frameOriginalIpumAtm50LA(source,frames,bad)&&
                frames.size()==pin->frameCount&&frameIndex<frames.size(),
                "Original TA IPUM ATM framing failed at "+std::to_string(bad));
            const auto& frame=frames[frameIndex];mipIndex=frame.count-1;mip=frame.mips[mipIndex];
        }else mip=framePst(source,mipIndex);
        a::TextureMip48E decoded;decoded.width=mip.width;decoded.height=mip.height;std::string why;
        shared::need(a::decodeOriginalIpuRgba(source.data()+mip.payload,mip.bytes,mip.width,mip.height,decoded.rgba,why),
            "Original TA IPUM decode: "+why);
        shared::need(a::encodeTextureLevel48E(decoded,output,error),"Original TA IPUM output framing: "+error.code);
    }else{
        shared::need(kind==0||kind==1,"Original TA indexed texture kind");Bytes pst;
        if(animated){
            a::OriginalUiAtm50KG frames;
            shared::need(a::splitOriginalUiAtm50KG(source.data(),source.size(),frames,error)&&
                frames.frames.size()==pin->frameCount&&frameIndex<frames.frames.size(),
                "Original TA indexed ATM framing: "+error.code);
            const auto& frame=frames.frames[frameIndex];
            pst.assign(source.begin()+frame.pstOffset,source.begin()+frame.pstOffset+frame.pstBytes);
        }
        a::Texture48E decoded;
        shared::need(a::decodeTexture48E(animated?pst:source,decoded,error)&&!decoded.mips.empty(),
            "Original TA indexed PST decode: "+error.code);
        mipIndex=static_cast<std::uint32_t>(decoded.mips.size()-1);output=std::move(decoded.base_rrtex);
    }
    const auto w=pin?pin->width:outputPin->width,h=pin?pin->height:outputPin->height;
    const auto expectedMip=pin?pin->mipIndex:outputPin->mipIndex;
    shared::need(mipIndex==expectedMip&&output.size()==std::size_t(w)*h*4+16&&
        !std::memcmp(output.data(),"RRTEX001",8)&&shared::u32(output.data()+8)==w&&shared::u32(output.data()+12)==h,
        "Original TA decoded image source framing differs");
    if(outputPin)shared::need(mipIndex==outputPin->mipIndex&&w==outputPin->width&&h==outputPin->height&&
        shared::sha256(output)==outputPin->outputSha256,"Original TA independently pinned decoded image differs");
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
        "Original TA borrowed world fields differ from authenticated SCB");
    const auto& actual=world.parameters;const auto& source=canonical.parameters;
    shared::need(actual.version==source.version&&actual.header_table_offset==source.header_table_offset&&
        actual.preservesDuplicateLabels==source.preservesDuplicateLabels&&actual.blocks.size()==source.blocks.size(),
        "Original TA parameter provenance header differs");
    for(std::size_t i=0;i<source.blocks.size();++i){
        const auto& x=actual.blocks[i];const auto& y=source.blocks[i];
        shared::need(x.name==y.name&&x.name_offset==y.name_offset&&x.table_entry_offset==y.table_entry_offset&&
            x.offset==y.offset&&x.size_without_count==y.size_without_count&&x.fields.size()==y.fields.size(),
            "Original TA parameter block provenance differs");
        for(std::size_t j=0;j<y.fields.size();++j){const auto& f=x.fields[j];const auto& g=y.fields[j];
            shared::need(f.name==g.name&&f.value==g.value&&f.entry_offset==g.entry_offset&&
                f.value_offset==g.value_offset&&f.value_bytes==g.value_bytes,
                "Original TA parameter field provenance differs");
        }
    }
    shared::need(world.scene_info_block==canonical.scene_info_block&&world.instances.size()==318&&
        world.instances.size()==canonical.instances.size()&&world.zones.size()==9&&
        world.zones.size()==canonical.zones.size()&&world.portals.size()==10&&world.portals.size()==canonical.portals.size(),
        "Original TA typed world source extent differs");
    for(std::size_t i=0;i<canonical.zones.size();++i){const auto& x=world.zones[i];const auto& y=canonical.zones[i];
        shared::need(x.zone==y.zone&&x.models==y.models&&x.parameter_block==y.parameter_block,
            "Original TA typed zone order/provenance differs");
    }
    const auto same=[](const auto& x,const auto& y){
        return !std::memcmp(x.data(),y.data(),sizeof(x[0])*x.size());
    };
    for(std::size_t i=0;i<canonical.instances.size();++i){const auto& x=world.instances[i];const auto& y=canonical.instances[i];
        shared::need(x.section==y.section&&x.geometry_member==y.geometry_member&&x.collision_member==y.collision_member&&
            x.zone==y.zone&&x.ordinal==y.ordinal&&x.parameter_block==y.parameter_block&&
            x.source_block_offset==y.source_block_offset&&x.use_lighting==y.use_lighting&&
            same(x.position,y.position)&&same(x.look,y.look)&&same(x.up,y.up)&&same(x.matrix,y.matrix),
            "Original TA typed instance order/provenance differs");
    }
    for(std::size_t i=0;i<canonical.portals.size();++i){const auto& x=world.portals[i];const auto& y=canonical.portals[i];
        shared::need(x.section==y.section&&x.parameter_block==y.parameter_block&&x.zones==y.zones,
            "Original TA typed portal order/provenance differs");
        for(std::size_t j=0;j<y.vertices.size();++j)shared::need(same(x.vertices[j],y.vertices[j]),
            "Original TA typed portal vertices differ");
    }
}
} // namespace detail

// Compatible direct-source callback for independently owned TA material consumers.
// Never reads a retained image filename; failures preserve the caller's image.
inline bool decodeSourceTexture(const char* member,const Bytes& source,Bytes& output,
    std::uint32_t& mipIndex,std::string& error)noexcept {
    try{
        shared::need(member&&*member,"Original TA source texture member required");
        a::DecodeError48C diagnostic;std::uint32_t mip=0;bool animated=false;
        auto next=detail::image(member,source,mip,animated,diagnostic);
        output=std::move(next);mipIndex=mip;error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="Original TA source texture decode failed";}catch(...){}return false;}
}

class Owner final {
    struct Scene {a::Scene48D decoded;std::uint64_t identity=0;std::vector<std::uint32_t> materials;};
    struct Storage {
        a::ResArchive48C archive;
        std::shared_ptr<const a::OriginalTaEncodedArchive50LA> encodedInput;
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
                "Original TA archive member extraction: "+entry.name+" "+error.code);return bytes;
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

    // Compatibility form copies original encoded bytes only on successful load.
    bool load(const Bytes& original,const a::World48F& world,Diagnostic& d)noexcept{
        return loadImpl(original,nullptr,world,d);
    }
    // Root's actual temporary transfers once after all fallible preparation.
    // Failure leaves input unchanged; success relinquishes mutable aliases.
    bool loadOwned(Bytes&& original,const a::World48F& world,Diagnostic& d)noexcept{
        return loadImpl(original,&original,world,d);
    }
private:
    bool loadImpl(const Bytes& original,Bytes* transfer,const a::World48F& world,Diagnostic& d)noexcept{
        try{
            d={};shared::need(!storage_&&std::fegetround()==FE_TONEAREST,"TA direct cold owner already loaded or rounding mode differs");
            shared::need(original.size()==archiveBytes&&shared::sha256(original)==archiveSha,"Original TA archive identity differs");
            auto next=std::make_unique<Storage>();d.stage="archive";
            a::DecodeLimits48C limits;limits.resource_file_bytes=archiveBytes;
            limits.resource_decoded_bytes=13099008;
            limits.resource_entries=383;
            shared::need(a::decodeResource48C(original,next->archive,d.decode,nullptr,limits)&&next->archive.entries.size()==383&&next->archive.decoded.size()==13099008,
                "Original TA archive decode: "+d.decode.code);
            for(std::size_t i=0;i<next->archive.entries.size();++i)
                shared::need(next->entries.emplace(detail::key(next->archive.entries[i].name),i).second,"Ambiguous original TA member");
            d.stage="world";const auto* scb=next->entry("tracks\\ta.scb");
            shared::need(scb&&scb->bytes==130511,"Original TA SCB extent differs");
            const auto scbBytes=next->member(*scb,d.decode);
            shared::need(shared::sha256(scbBytes)=="c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d","Original TA SCB identity differs");
            detail::validateBorrowedWorld(scbBytes,world,d.decode);
            std::map<std::string,std::uint32_t> materialIndices,textureIndices;
            std::vector<Bytes> images;
            std::vector<a::MaterialTextureSlotInput50EA> slots;
            constexpr std::uint64_t textureBase=0x5441000000000000ull;
            std::size_t rgbaBytes=0,serializedBytes=0;
            auto material=[&](const std::string& materialName)->std::uint32_t{
                const auto normalized=detail::key(materialName);const auto existing=materialIndices.find(normalized);
                if(existing!=materialIndices.end())return existing->second;
                shared::need(shared::name(materialName),"Invalid original TA material basename");
                d.stage="material";const auto* psm=next->entry("textures\\"+materialName+".psm");
                shared::need(psm,"Original TA world material absent: "+materialName);d.member=psm->name;
                const auto source=next->member(*psm,d.decode);a::Material48E authored;
                shared::need(a::decodeMaterial48E(source,authored,d.decode)&&!authored.passes.empty()&&authored.passes.size()==1,
                    "Original TA world material subset: "+d.member+" "+d.decode.code);
                const auto materialIndex=static_cast<std::uint32_t>(next->materials.size());
                shared::need(materialIndex<29,"Original TA PSM count bound");
                const auto materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+shared::sha256(source));
                std::vector<std::uint32_t> selectedPasses;
                for(std::size_t pi=0;pi<authored.passes.size();++pi){
                    const auto& pass=authored.passes[pi];const bool projected=pass.microprogram=="ProjectedPrelit";
                    shared::need(pass.pass==pi+1&&pass.textures.size()==1&&pass.textures[0].slot==1&&
                        (!projected&&
                         (pass.microprogram=="PreLitCompressed"||pass.microprogram=="PrelitCompressed"||
                          pass.microprogram=="DiffuseCompressed"||pass.microprogram=="diffuseCompressed")),
                        "Unjoined original TA pass/program/texture subset");
                    const auto& map=pass.textures[0];shared::need(shared::name(map.name),"Invalid original TA texture basename");
                    const a::ResourceEntry48C* texture=nullptr;
                    for(const char* extension:{".pst",".ipu",".tga",".atm"}){
                        texture=next->entry("textures\\"+map.name+extension);if(texture)break;
                    }
                    shared::need(texture,"Original TA required texture absent: "+map.name);
                    const auto textureKey=detail::key(texture->name);const auto* pin=detail::sourceImage(textureKey);
                    shared::need(pin,"Original TA static image lacks exact source pin");
                    const bool animated=textureKey.ends_with(".ATM");
                    if(animated)shared::need(authored.passes.size()==1&&detail::initialFrame(pass.fields)==pin->initialFrame,
                        "Original TA material/ATM initial frame differs");
                    auto found=textureIndices.find(textureKey);
                    if(found==textureIndices.end()){
                        d.stage="texture";d.member=texture->name;const auto bytes=next->member(*texture,d.decode);
                        std::uint32_t mip=0;bool decodedAnimated=false,independent=false;
                        auto image=detail::image(texture->name,bytes,mip,decodedAnimated,d.decode,&independent);
                        shared::need(decodedAnimated==animated,"Original TA texture animation source classification");
                        const auto index=static_cast<std::uint32_t>(images.size());
                        shared::need(index<29&&image.size()>=16,"TA original texture bank bound");
                        const auto w=shared::u32(image.data()+8),h=shared::u32(image.data()+12);
                        shared::need(w&&h&&w<=1024&&h<=1024&&image.size()==std::size_t(w)*h*4+16,"TA native image extent");
                        shared::add(rgbaBytes,std::size_t(w)*h*4,16u*1024u*1024u,"TA texture RGBA budget");
                        shared::add(serializedBytes,image.size(),16u*1024u*1024u,"TA texture framing budget");
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
                    shared::need(index<29,"Original TA authored pass count bound");
                    const auto& image=next->textures[found->second];tm::Selection selected;
                    selected.assetIdentity=0x5441;selected.materialIndex=materialIndex;selected.pass=pass.pass;selected.slot=1;
                    selected.bankSlot=static_cast<std::uint32_t>(slots.size());selected.textureIndex=found->second;selected.mipIndex=image.mipIndex;
                    selected.materialName=materialName;selected.materialKey=materialKey;
                    selected.program=pass.microprogram;selected.textureName=map.name;selected.textureKey=image.key;
                    selected.track=track_source_fixture::Track::ta;selected.passCount=static_cast<std::uint32_t>(authored.passes.size());
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
                rr::platform::loading_progress50DU::pulse();
                const auto& placement=world.instances[wi];d.stage="scene";d.member=placement.geometry_member;
                const auto* entry=next->entry(placement.geometry_member);
                if(!entry){shared::need(next->entry(placement.collision_member),"TA placement lacks both original PSG and COL");++d.collisionOnly;continue;}
                const auto name=detail::key(entry->name);auto found=next->scenes.find(name);
                if(found==next->scenes.end()){
                    Scene scene;scene.identity=next->scenes.size()+1;
                    const auto originalScene=next->member(*entry,d.decode);
                    shared::need(a::decodeScene48D(originalScene,scene.decoded,d.decode),
                        "Original TA PSG decode: "+entry->name+" "+d.decode.code);
                    shared::need(scene.decoded.nodes.size()==1&&scene.decoded.nodes[0].parent==-1&&
                        !scene.decoded.identity_matrix_palette&&!scene.decoded.lods.empty(),"Original TA static hierarchy subset");
                    for(const auto& name:scene.decoded.materials)scene.materials.push_back(material(name));
                    found=next->scenes.emplace(name,std::move(scene)).first;
                }
                const auto& scene=found->second;const auto& matrix=scene.decoded.world_space?identity:placement.matrix;
                a::WorldNormalTransform50HK normalTransform;
                shared::need(a::prepareWorldNormalTransform50HK(matrix,normalTransform),"Original TA visual/normal transform");
                for(std::size_t si=0;si<scene.decoded.lods.front().surfaces.size();++si){
                rr::platform::loading_progress50DU::pulse();
                    const auto& surface=scene.decoded.lods.front().surfaces[si];d.stage="surface";d.member=entry->name;
                    shared::need(surface.material_index<scene.materials.size()&&!surface.vertices.empty()&&!surface.indices.empty()&&
                        !(surface.indices.size()%3)&&surface.vertices.size()<=a::max_mesh_vertices-next->mesh.vertices.size()&&
                        surface.indices.size()<=a::max_mesh_indices-next->mesh.indices.size(),"TA original surface/material/geometry bounds");
                    tm::Span span;span.vertexStart=static_cast<std::uint32_t>(next->mesh.vertices.size());
                    span.vertexCount=static_cast<std::uint32_t>(surface.vertices.size());span.indexStart=static_cast<std::uint32_t>(next->mesh.indices.size());
                    span.indexCount=static_cast<std::uint32_t>(surface.indices.size());span.worldInstanceIndex=static_cast<std::uint32_t>(wi);
                    span.surfaceIndex=static_cast<std::uint32_t>(si);span.hasOriginalColors=surface.has_original_colors;
                    const auto& selectedPasses=next->materialPasses.at(scene.materials[surface.material_index]);
                    shared::need(!selectedPasses.empty()&&selectedPasses.size()==1,"Original TA surface material pass closure");
                    for(const auto& source:surface.vertices){
                        shared::need(source.bone==0,"TA original static vertex bone");a::Vertex48B vertex{};
                        for(unsigned axis=0;axis<3;++axis){double value=0;
                            for(unsigned k=0;k<3;++k)value+=source.position_local[k]*matrix[k*4+axis];value+=matrix[12+axis];
                            vertex.position[axis]=static_cast<float>(value);shared::need(std::isfinite(value)&&std::isfinite(vertex.position[axis]),"TA nonfinite original vertex");
                        }
                        for(unsigned axis=0;axis<2;++axis){vertex.uv[axis]=static_cast<float>(source.uv[axis]);shared::need(std::isfinite(vertex.uv[axis]),"TA nonfinite original UV");}
                        for(unsigned axis=0;axis<4;++axis)vertex.color[axis]=surface.has_original_colors?source.color[axis]:255;
                        a::WorldNormal50HK normal;shared::need(a::transformPreparedWorldNormal50HK(source.normal_local,normalTransform,normal),"TA original normal transform");
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
                                shared::need(std::isfinite(value)&&std::isfinite(static_cast<float>(value)),"TA projected source UV nonfinite");
                                uv.uv[(selection.pass-1)*2+axis]=static_cast<float>(value);
                            }
                        }
                        next->projectionUvs.push_back(uv);
                    }
                    for(const auto index:surface.indices){shared::need(index<surface.vertices.size(),"TA original surface index");next->mesh.indices.push_back(span.vertexStart+index);}
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
            shared::need(d.scenes==228&&d.placements==284&&d.collisionOnly==34&&d.surfaces==353&&
                d.materials==29&&d.selections==29&&d.secondPasses==0&&next->spans.size()==353&&
                next->mesh.vertices.size()==110196&&next->mesh.indices.size()==209514&&images.size()==29&&
                next->normals.size()==next->mesh.vertices.size()&&next->projectionUvs.size()==next->mesh.vertices.size()&&
                rgbaBytes==4038656&&d.independentlyPinnedImages==29&&d.runtimeIdentityImages==0,
                "Original TA full static source closure differs");
            for(unsigned axis=0;axis<3;++axis){next->mesh.bounds.minimum[axis]=std::numeric_limits<float>::infinity();
                next->mesh.bounds.maximum[axis]=-std::numeric_limits<float>::infinity();}
            for(const auto& vertex:next->mesh.vertices)for(unsigned axis=0;axis<3;++axis){
                next->mesh.bounds.minimum[axis]=std::min(next->mesh.bounds.minimum[axis],vertex.position[axis]);
                next->mesh.bounds.maximum[axis]=std::max(next->mesh.bounds.maximum[axis],vertex.position[axis]);}
            std::vector<a::MaterialTextureAssetInput50EA> textureInput;
            for(std::size_t i=0;i<images.size();++i)textureInput.push_back({next->textures[i].identity,&images[i]});
            a::MaterialTextureAssetsInput50EA input;input.textures=textureInput.data();input.textureCount=static_cast<std::uint32_t>(textureInput.size());
            input.slots=slots.data();input.slotCount=static_cast<std::uint32_t>(slots.size());
            input.budgets={29,29,16u*1024u*1024u,16u*1024u*1024u,1024};
            d.stage="pixel_bank";shared::need(next->bank.load(input,d.bank),"Original TA pixel bank rejected: "+d.bank.parserMessage);
            // One input-only owner. Its control block and real block metadata
            // allocate before transfer/publication; no provider/domain back-link.
            auto encoded=std::shared_ptr<a::OriginalTaEncodedArchive50LA>(new a::OriginalTaEncodedArchive50LA);
            encoded->blocks_=next->archive.blocks;
            if(transfer)encoded->bytes_.swap(*transfer);else encoded->bytes_=original;
            next->encodedInput=std::move(encoded);
            storage_=std::move(next);d.stage="complete";return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original TA native cold preparation failed";}catch(...){}return false;}
    }

public:
    // Both actual TA mount producers use this one complete decoded mount plus
    // immutable encoded-input binding. Original commit/revision semantics stay
    // with the existing provider; no alternate service or namespace is created.
    bool mountCanonicalArchive(a::OriginalStandardArchiveProvider50LA& provider,
        a::OriginalStandardArchiveProvider50LA::Diagnostic& d)noexcept {
        if(!storage_||!storage_->encodedInput){d={};d.status=a::OriginalStandardArchiveProvider50LA::Status::input;return false;}
        try {
            return provider.mountAuthenticatedRes("TRACKS/TA.RES",archiveSha,archiveBytes,storage_->archive,d)&&
                provider.retainTaEncodedInput(storage_->archive,storage_->encodedInput,d);
        }catch(...){d={};d.status=a::OriginalStandardArchiveProvider50LA::Status::allocation;return false;}
    }

    // Same Application-owned domain as the actual installed racers. This does
    // not register synthetic static models or claim source render dispatch for
    // the native world above. Mounting the real complete TA namespace preserves
    // the provider's local resource lookup before genuine racer population.
    bool populateCanonicalRacers(const tm::Request& request,tm::Diagnostic& d)noexcept{
        try{
            d={};shared::need(storage_&&!storage_->canonicalAttempted,"TA canonical population requires fresh native source owner");
            const bool canonical=bool(request.canonicalDomain);
            shared::need(canonical==bool(request.canonicalContext)&&canonical==(request.canonicalMaterialPath.actualPrefix36B970!=nullptr),
                "TA canonical Application/Video/path must be supplied together");
            if(!canonical){shared::need(request.canonicalRacerModels.empty(),"TA racer sources lack their canonical domain");return true;}
            shared::need(request.track==track_source_fixture::Track::ta&&request.canonicalDomain->prepared()&&
                request.canonicalContext->live()&&*request.canonicalMaterialPath.actualPrefix36B970=="textures\\",
                "TA canonical racers require their actual gameplay directory and live Video");
            storage_->canonicalAttempted=true;storage_->canonicalDomain=request.canonicalDomain;
            const auto& provider=request.canonicalDomain->archiveProvider3E1A70();
            a::OriginalStandardArchiveProvider50LA::Diagnostic mounted;d.stage="ta_original_archive_mount";
            shared::need(provider&&mountCanonicalArchive(*provider,mounted)&&
                provider->sealNativeMounts(mounted)&&request.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,d.canonical.texture),
                "Original TA canonical provider mount rejected");
            std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
            std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
            d.stage="ta_original_racer_sources";
            for(const auto& racer:request.canonicalRacerModels)
                shared::need(racer.resource&&racer.references&&racer.references->table382B38()==request.canonicalDomain->materials382B38().get()&&
                    original_racer_material_sources50LA::appendModelSources(*racer.resource,*racer.references,
                        *request.canonicalMaterialPath.actualPrefix36B970,*provider,materials,textures,d.racerSource),
                    "Original TA canonical racer material/texture closure rejected");
            shared::need(request.canonicalDomain->appendOriginalSources(materials,textures,d.canonical),"Original TA canonical racer source admission rejected");
            d.stage="ta_original_racer_population";
            shared::need(request.canonicalDomain->populate(request.canonicalContext,d.canonical),"Original TA canonical racer population rejected");
            return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original TA canonical racer preparation failed";}catch(...){}return false;}
    }

    // Preserve the established TA material-keyed single-pass alpha policy.
    bool alpha(std::size_t selection,const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,material_alpha50gx_fixture::Policy& output)const noexcept{
        try{return storage_&&selection<storage_->selections.size()&&
            material_alpha50gx_fixture::select(storage_->selections[selection],output);}
        catch(...){return false;}
    }
    // Borrow the same authenticated archive for a separate TA cold decoder. The
    // archive lifetime is the world lifetime and cannot be replaced by reload.
    const a::ResArchive48C* archive()const noexcept{return storage_?&storage_->archive:nullptr;}
};

} // namespace ta_original_world50LE
