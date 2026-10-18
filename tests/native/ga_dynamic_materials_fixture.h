#pragma once
#include "track_materials50fa_fixture.h"
#include "source_texture50fc_fixture.h"
#include "watchtower_source_fixture.h"
#include "../../src/assets/import_texture48e.h"
#include <array>

// Exact GA dynamic material owner. Geometry equality with GC does not imply
// texture equality: three GA images are kind2, and all five original PSTs are
// authenticated independently. No same-name static/GC bank borrowing.
// Direct-disc callers explicitly inject their source decoder. The legacy
// overload retains its existing cold-prepared-file adapter.
namespace ga_dynamic_materials {
namespace a=rr::assets;
using Selection=track_materials50fa_fixture::Selection;
using SourceTextureDecoder=track_materials50fa_fixture::SourceTextureDecoder;
struct Diagnostic {
 std::uint32_t material=a::unresolvedMaterialTexture50EA;
 std::string message;a::DecodeError48C source{};a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
namespace shared=pod_materials50ed_fixture::detail;
struct Material {const char* name;const char* sha;std::uint32_t bytes,texture;};
struct Texture {const char* name;const char* sha;std::uint32_t bytes,width,height,mip;const char* nativeSha;};
inline constexpr Material materials[]={
 {"fience02","913cb65c8e2bc8c91f6cc99b1a1a6ad59694d94dd76d7c568a00a46b4dcf7e94",260,0},
 {"fience_opening","43c2defc92a04c61066e3f69138270603215f02f7bb104495fbb0efefa923eea",569,1},
 {"market_hut_a01","869194ef061f1f557407021956df0ccf22f7c006ee5886411a54a9cd544cf938",268,2},
 {"opening_a","6d49201a0a7b1af8bd37254217b941faecbe8256becf24df9fe233cf6d4e0f08",263,3},
 {"house_stand_e01","627b6c6b47ce6b45661b7cf413dc0cbf961928664a133a24ead2b14cdf1a8243",269,4},
 {"fience_opening","43c2defc92a04c61066e3f69138270603215f02f7bb104495fbb0efefa923eea",569,1}
};
// Existing source_texture_ga_index.inc output identities, also enforced at
// this owner boundary so decoder injection cannot weaken the reviewed pins.
inline constexpr Texture textures[]={
 {"fience","c6c1f848a8efd2b2f7af8d3be5fa8d13f76fea11fd1ffadef4c460d29f30f00f",22860,128,128,3,"17b2dbb87a1ff184d05f71e80bc3694de7cbb5611d74ad68a953167e63927323"},
 {"fience_opening","c6c1f848a8efd2b2f7af8d3be5fa8d13f76fea11fd1ffadef4c460d29f30f00f",22860,128,128,3,"17b2dbb87a1ff184d05f71e80bc3694de7cbb5611d74ad68a953167e63927323"},
 {"market_hut_a01","738f9957fc7dc79de8e9255827e6c9425a731210f4c470823d706e94be236619",1339,64,64,2,"259b1f2faa285b40205665822773afece66385873ddc3b4050ffc4fcb6d79d4e"},
 {"opening_a","d775490a47fd8332a5b81371a7725996215d7c716ff7ad88c9d8d7502b994dc3",9327,128,128,3,"d748d9935439a786e771af17e94f3d86788774907ed29a6c6f76f1fb160ebee1"},
 {"house_stand_e01","fe193d5eea5c2e7750c1a29e881be3bada46a0590aab9bc695ded9efcb9313a9",2156,64,64,2,"4fdc33f757d8facef2fa8a259d7876a879e94f7255a35f0077295a4496256eb9"}
};
inline nlohmann::json fields(const a::MaterialFields48E& source){
 auto out=nlohmann::json::object();out["authored"]=nlohmann::json::array();out["unspecified"]=nlohmann::json::array();
 for(const auto& f:source.authored)out["authored"].push_back({{"name",f.name},{"value",f.value},{"source_offset",f.source_offset}});
 for(const auto& f:source.unspecified)out["unspecified"].push_back({{"name",f.name},{"value",f.value},{"source_offset",f.source_offset}});
 return out;
}
}
class Owner final {
 struct Storage {a::MaterialTextureAssets50EA bank;std::array<Selection,6> selections;};
 std::unique_ptr<Storage> storage_;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 bool loaded()const noexcept{return bool(storage_);}
 const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
 const std::array<Selection,6>* selections()const noexcept{return storage_?&storage_->selections:nullptr;}
 bool load(const std::filesystem::path& archivePath,const a::Scene48D& defensegate,
     const a::Scene48D& opening,Diagnostic& d)noexcept {
  return load(archivePath,defensegate,opening,source_texture50fc_fixture::decodeSourceTextureGa,d);
 }
 // Input remains the exact authenticated GA member bytes. Null or failed
 // decoders are fatal and never fall back to the legacy retained-file route.
 bool load(const std::filesystem::path& archivePath,const a::Scene48D& defensegate,
     const a::Scene48D& opening,SourceTextureDecoder sourceDecoder,Diagnostic& d)noexcept {
  using namespace detail;using shared::need;
  try{
   d={};need(!storage_,"GA dynamic material owner is already loaded");
   need(sourceDecoder,"GA dynamic material owner requires an explicit source decoder");
   need(defensegate.materials.size()==2&&opening.materials.size()==4,"GA dynamic scene material counts");
   for(std::size_t i=0;i<6;++i)need((i<2?defensegate.materials[i]:opening.materials[i-2])==materials[i].name,"GA dynamic scene material order");
   shared::Inputs reader;reader.capacity=watchtower_source_fixture::archiveBytes;
   const auto bytes=reader.read(archivePath,watchtower_source_fixture::archiveBytes);
   need(bytes.size()==watchtower_source_fixture::archiveBytes&&shared::sha256(bytes)==watchtower_source_fixture::archiveSha,"GA dynamic archive identity");
   a::ResArchive48C archive;a::DecodeLimits48C limits;
   limits.resource_file_bytes=watchtower_source_fixture::archiveBytes;limits.resource_decoded_bytes=watchtower_source_fixture::decodedBytes;
   limits.resource_entries=watchtower_source_fixture::archiveEntries;
   need(a::decodeResource48C(bytes,archive,d.source,nullptr,limits),"GA dynamic archive decode");
   std::unique_ptr<Storage> next(new Storage);
   std::array<std::vector<std::uint8_t>,5> pixels;
   std::array<a::MaterialTextureAssetInput50EA,5> images{};
   std::array<a::MaterialTextureSlotInput50EA,6> slots{};
   for(std::size_t t=0;t<5;++t){
    const auto& entry=textures[t];const std::string member=std::string("textures\\")+entry.name+".pst";
    std::vector<std::uint8_t> original;
    need(a::extractResMember48C(archive,member,original,d.source,32768)&&original.size()==entry.bytes&&shared::sha256(original)==entry.sha,"GA dynamic original texture identity");
    a::Texture48E native;
    if(a::decodeTexture48E(original,native,d.source)){
     need(native.mips.size()==entry.mip+1&&native.mips.back().width==entry.width&&native.mips.back().height==entry.height,"GA indexed texture dimensions");
     pixels[t]=std::move(native.base_rrtex);
    }else{
     // Only authenticated kind2 reaches the explicitly selected decoder;
     // a failure never authorizes a different file, decoder or image.
     need(original.size()>=12&&shared::u32(original.data())==1&&
       shared::u32(original.data()+4)==2&&shared::u32(original.data()+8)==entry.mip+1,
       "GA dynamic original kind2 mip header");
     std::uint32_t mip=0;std::string error;
     need(sourceDecoder(member.c_str(),original,pixels[t],mip,error),error.empty()?"GA kind2 source texture decode failed":error);
     need(mip==entry.mip,"GA kind2 source mip differs");
    }
    need(pixels[t].size()==16u+std::size_t(entry.width)*entry.height*4u&&std::memcmp(pixels[t].data(),"RRTEX001",8)==0&&
      shared::u32(pixels[t].data()+8)==entry.width&&shared::u32(pixels[t].data()+12)==entry.height&&
      shared::sha256(pixels[t])==entry.nativeSha,"GA native texture framing/output identity");
    images[t]={std::uint64_t(t)+1,&pixels[t]};
   }
   for(std::uint32_t i=0;i<6;++i){
    d.material=i;const auto& entry=materials[i];const auto& texture=textures[entry.texture];
    std::vector<std::uint8_t> psm;const std::string member=std::string("textures\\")+entry.name+".psm";
    need(a::extractResMember48C(archive,member,psm,d.source,1024)&&psm.size()==entry.bytes&&shared::sha256(psm)==entry.sha,"GA dynamic original material identity");
    a::Material48E material;need(a::decodeMaterial48E(psm,material,d.source)&&material.passes.size()==1,"GA dynamic material decode");
    const auto& pass=material.passes.front();need(pass.pass==1&&pass.microprogram=="DiffuseCompressed"&&pass.textures.size()==1&&pass.textures[0].slot==1&&pass.textures[0].name==texture.name,"GA dynamic authored texture map");
    auto& selection=next->selections[i];selection.assetIdentity=i<2?1:2;selection.materialIndex=i<2?i:i-2;
    selection.materialName=entry.name;selection.materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+entry.sha);
    selection.textureName=texture.name;selection.textureKey=shared::sha256(std::string("rr-native-texture-48E-v1\n")+texture.sha);
    selection.textureIndex=entry.texture;selection.bankSlot=i;selection.mipIndex=texture.mip;
    selection.pass=selection.slot=selection.passCount=1;selection.track=track_source_fixture::Track::ga;
    selection.program=pass.microprogram;selection.sourcePassFields=fields(pass.fields);
    for(const auto& field:pass.fields.authored){
     if(field.name!="wrapModeHorizontal"&&field.name!="wrapModeVertical")continue;
     need(field.value=="0"||field.value=="1","GA dynamic source wrap mode");
     (field.name=="wrapModeHorizontal"?selection.clampU:selection.clampV)=field.value=="1";
    }
    slots[i]={{selection.assetIdentity,selection.materialIndex,1,1},std::uint64_t(entry.texture)+1,a::MaterialTextureRequirement50EA::required};
   }
   const a::MaterialTextureBudgets50EA budgets{5,6,256u*1024u,256u*1024u,128};
   const a::MaterialTextureAssetsInput50EA input{images.data(),5,slots.data(),6,budgets};
   need(next->bank.load(input,d.bank),"GA dynamic source texture bank rejected");
   storage_=std::move(next);d.material=a::unresolvedMaterialTexture50EA;d.source={};return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.message="GA dynamic material preparation failure";}catch(...){}return false;}
 }
};
}
