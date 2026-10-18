#pragma once
#include "sa_animated_world_fixture.h"
#include "track_materials50fa_fixture.h"
#include "source_texture50fc_fixture.h"
#include "../../src/assets/import_texture48e.h"
#include <array>
#include <memory>

// Genuine SA dynamic material/texture owner: eight distinct original PSM/PSTs,
// twenty-one source-ordered owner/material slots. Kind2 pixels are admitted
// only through the SA source-byte/output-hash-pinned cold preparation callback.
namespace sa_dynamic_materials {
namespace a=rr::assets;
using Selection=track_materials50fa_fixture::Selection;
using SourceTextureDecoder=track_materials50fa_fixture::SourceTextureDecoder;
inline constexpr std::size_t textureCount=8,slotCount=sa_animated_world::surfaceCount;
struct Diagnostic {
 std::uint32_t material=a::unresolvedMaterialTexture50EA;
 std::string message;a::DecodeError48C source{};a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
namespace shared=pod_materials50ed_fixture::detail;
struct Texture {const char* name;const char* sha;std::uint32_t bytes,width,height,mip;const char* nativeSha;};
inline constexpr std::array<Texture,textureCount> textures{{
 {"robotred01","95f56ca5a5aea03b086893f40df472a89662f8c173170c745b8356f883c9323e",6055,128,128,3,"2d2ce6623b1953a489b26736ba0e32ec97c226adcdaa8ea9622391ea31d6cf63"},
 {"robotred02","8a2068db39028c5de8e318891cc6690101d4dead7502c6e68eb167372e0676de",1539,64,64,2,"16765460b2d849bb16491705cf87521930e15a40d389c08ad7d08f4aeee65642"},
 {"robox","86306f7133d667793c75aa87d235fe0495e53ec01409e0b44efd1fdb75b306a3",2085,64,64,2,"b1e4a0087f75410d9886b46e456f0ac89953b109b7e2695aee116ad8550bf038"},
 {"slide02","61a6bf79f12274b312c78f9f8c857f134d8831da618e75caf57a439bf6f51113",403,32,32,1,"267218c4553f28933dd57888a9c727cd27604c2ad726f0e6060cfdb2cdce9060"},
 {"larobo02","ec864ab3403de5c2f2a435867f601d3e13301ed10c1389d1059e55664f743b67",2030,64,64,2,"e75f84dc4dcdd1420f5732755231936146d4ebe41a2eecb463afe50fe269bf37"},
 {"larobo01","0f543c972303bf5391c464636e1c2422b8e0135dff38e853b345361a066c64b9",2041,64,64,2,"b3d144412891ff3210691eb9313448b315aff42c3a2e1233dbe80736825becd0"},
 {"larobo03","ce2769acc9ab9887e617ba83a5f908c52ff020a2c79b2e1b9a467d58a0b17a2a",2415,64,64,2,"28bf08f99d6010c7aab7533ed0c7bee8c691785a72903db18a67af391c493f03"},
 {"lazerrobo01","f97aa5b62efeafb1c74e59a7b7801a5466dd7e9e23f65a9b75b21ad92e535649",1782,64,64,2,"f85b280dd0f008aa090df0e1122a1b78b5e3ed8fd3b98f7e4cc461ef92f85daa"},
}};
inline nlohmann::json fields(const a::MaterialFields48E& source){
 auto out=nlohmann::json::object();out["authored"]=nlohmann::json::array();out["unspecified"]=nlohmann::json::array();
 for(const auto& f:source.authored)out["authored"].push_back({{"name",f.name},{"value",f.value},{"source_offset",f.source_offset}});
 for(const auto& f:source.unspecified)out["unspecified"].push_back({{"name",f.name},{"value",f.value},{"source_offset",f.source_offset}});
 return out;
}
}
class Owner final {
 struct Storage {a::MaterialTextureAssets50EA bank;std::array<Selection,slotCount> selections;};
 std::unique_ptr<Storage> storage_;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool loaded()const noexcept{return bool(storage_);}
 const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
 const std::array<Selection,slotCount>* selections()const noexcept{return storage_?&storage_->selections:nullptr;}
 bool load(const std::filesystem::path& archivePath,const sa_animated_world::Owner& world,Diagnostic& d)noexcept {
  return load(archivePath,world,source_texture50fc_fixture::decodeSourceTextureSa,d);
 }
 // Direct-disc callers explicitly choose their source decoder. Null or
 // failed callbacks are fatal; no fallback to retained files is attempted.
 bool load(const std::filesystem::path& archivePath,const sa_animated_world::Owner& world,
     SourceTextureDecoder sourceDecoder,Diagnostic& d)noexcept {
  using namespace detail;using shared::need;
  try{
   d={};need(!storage_&&world.ready(),"SA dynamic material owner requires a fresh loaded world");
   need(sourceDecoder,"SA dynamic material owner requires an explicit source decoder");
   shared::Inputs reader;reader.capacity=sa_animated_world::archiveBytes;
   const auto bytes=reader.read(archivePath,sa_animated_world::archiveBytes);
   need(bytes.size()==sa_animated_world::archiveBytes&&shared::sha256(bytes)==sa_animated_world::archiveSha,"SA dynamic archive identity");
   a::ResArchive48C archive;a::DecodeLimits48C limits{};
   limits.resource_file_bytes=sa_animated_world::archiveBytes;limits.resource_decoded_bytes=sa_animated_world::decodedBytes;limits.resource_entries=sa_animated_world::archiveEntries;
   need(a::decodeResource48C(bytes,archive,d.source,nullptr,limits)&&archive.entries.size()==sa_animated_world::archiveEntries&&archive.decoded.size()==sa_animated_world::decodedBytes,"SA dynamic archive decode/extent");
   std::unique_ptr<Storage> next(new Storage);
   std::array<std::vector<std::uint8_t>,textureCount> pixels;
   std::array<a::MaterialTextureAssetInput50EA,textureCount> images{};
   std::array<a::MaterialTextureSlotInput50EA,slotCount> slots{};
   std::array<a::Material48E,textureCount> materials{};
   for(std::size_t t=0;t<textureCount;++t){
    const auto& entry=textures[t];const std::string member=std::string("textures\\")+entry.name+".pst";
    std::vector<std::uint8_t> original;
    need(a::extractResMember48C(archive,member,original,d.source,32768)&&original.size()==entry.bytes&&shared::sha256(original)==entry.sha,"SA dynamic original texture identity");
    // All eight actual PSTs are kind2. No generic decoder rejection is used
    // as permission to select a different file, texture or fallback pixels.
    need(original.size()>=12&&shared::u32(original.data())==1&&shared::u32(original.data()+4)==2&&shared::u32(original.data()+8)==entry.mip+1,"SA original dynamic kind2 mip header");
    std::uint32_t mip=0;std::string error;
    need(sourceDecoder(member.c_str(),original,pixels[t],mip,error),error.empty()?"SA kind2 source texture decode failed":error);
    need(mip==entry.mip&&pixels[t].size()==16u+std::size_t(entry.width)*entry.height*4u&&std::memcmp(pixels[t].data(),"RRTEX001",8)==0&&
      shared::u32(pixels[t].data()+8)==entry.width&&shared::u32(pixels[t].data()+12)==entry.height&&
      shared::sha256(pixels[t])==entry.nativeSha,"SA native source texture extent/framing/output identity");
    images[t]={std::uint64_t(t)+1,&pixels[t]};
    const auto& originalMaterial=sa_animated_world::detail::materialResources[t];
    std::vector<std::uint8_t> psm;
    need(a::extractResMember48C(archive,originalMaterial.name,psm,d.source,1024)&&psm.size()==originalMaterial.bytes&&shared::sha256(psm)==originalMaterial.sha,"SA dynamic original material identity");
    auto& material=materials[t];need(a::decodeMaterial48E(psm,material,d.source)&&material.passes.size()==1,"SA dynamic material decode");
    const auto& pass=material.passes.front();need(pass.pass==1&&pass.microprogram=="DiffuseCompressed"&&pass.textures.size()==1&&pass.textures[0].slot==1&&pass.textures[0].name==entry.name,"SA dynamic authored texture map");
    // The authenticated eight files author only program and TextureMap_1.
    // Missing GS fields are retained as omissions. The window joins these
    // selections to the actual authenticated DefaultMT owner, not literals.
    need(pass.fields.unspecified.empty()&&pass.fields.authored.size()==2,"SA dynamic material render-field scope");
    for(const auto& f:pass.fields.authored)need(f.name=="microProgramName"||f.name=="TextureMap_1","SA dynamic unjoined render field");
   }
   for(std::uint32_t i=0;i<slotCount;++i){d.material=i;const auto& span=world.spans()[i];
    need(span.material==i&&span.owner<sa_animated_world::ownerCount,"SA dynamic original span owner");
    const auto& scene=world.source(span.owner).scene;need(span.sourceMaterial<scene.materials.size(),"SA dynamic original material index");
    const auto& name=scene.materials[span.sourceMaterial];std::size_t t=0;for(;t<textureCount&&name!=textures[t].name;++t){}
    need(t<textureCount,"SA dynamic unlisted source material");const auto& texture=textures[t];const auto& originalMaterial=sa_animated_world::detail::materialResources[t];
    const auto& pass=materials[t].passes.front();auto& selection=next->selections[i];
    selection.assetIdentity=std::uint64_t(span.owner)+1;selection.materialIndex=span.sourceMaterial;
    selection.materialName=name;selection.materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+originalMaterial.sha);
    selection.decodedSourceMember=std::string("textures\\")+texture.name+".pst";selection.decodedSourceSha256=texture.sha;
    selection.textureName=texture.name;selection.textureKey=shared::sha256(std::string("rr-native-texture-48E-v1\n")+texture.sha);
    selection.textureIndex=static_cast<std::uint32_t>(t);selection.bankSlot=i;selection.mipIndex=texture.mip;
    selection.pass=selection.slot=selection.passCount=1;selection.track=track_source_fixture::Track::sa;
    selection.program=pass.microprogram;selection.sourcePassFields=fields(pass.fields);
    slots[i]={{selection.assetIdentity,selection.materialIndex,1,1},std::uint64_t(t)+1,a::MaterialTextureRequirement50EA::required};
   }
   const a::MaterialTextureBudgets50EA budgets{textureCount,slotCount,256u*1024u,256u*1024u,128};
   const a::MaterialTextureAssetsInput50EA input{images.data(),textureCount,slots.data(),slotCount,budgets};
   need(next->bank.load(input,d.bank),"SA dynamic source texture bank rejected");
   storage_=std::move(next);d.material=a::unresolvedMaterialTexture50EA;d.source={};return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.message="SA dynamic material preparation failure";}catch(...){}return false;}
 }
};
} // namespace sa_dynamic_materials
