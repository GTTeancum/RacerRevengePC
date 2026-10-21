#pragma once
#include "mc_animation_runtime50mc.h"
#include "mc_original_world50lm.h"

namespace mc_dynamic_materials50MC {
namespace a=rr::assets;using Selection=track_materials50fa_fixture::Selection;
struct Diagnostic {std::string message;std::size_t material=0;std::array<std::string,2> outputSha{};std::array<std::uint32_t,2> width{},height{},mip{};a::DecodeError48C source{};a::MaterialTextureAssetDiagnostic50EA bank{};};
namespace detail {
struct Pin {const char* name;std::size_t psmBytes;const char* psmSha;std::size_t pstBytes;const char* pstSha;std::uint32_t width,height,mip,kind;const char* nativeSha;};
// Native regression outputs measured by dynamic-materials-cold-v4, not independent pixel references.
inline constexpr std::array<Pin,2> pins{{
 {"mc_coral11",264,"aa9aafc9487487f813d90a3fb3ab166bf06c3039dc664eb2954b814530b3afbe",33112,"5bee916957b246d98b5e51d44b8cd3008c398b2b4a160b9694f508a4203a27c4",256,256,4,2,"c096413601b3c9646667449e815da9789b0f985a7fef896c53868b7d2b2408f5"},
 {"mb_ray",2621,"53796ed56017a30e569de209fb6faea3058d70d2344cb787ac69d51e5c60bc5a",2572,"31ded514edb34b484660ed88c7a6f5073f2faa2d903435699594aba214f5d6b9",64,64,2,2,"050b10d47e2fdb5fd317a714fbe11016066d4381454022c1d35f9287cf481b75"}
}};
inline constexpr std::array<std::uint32_t,14> materialPins{{0,1,1,1,1,1,1,1,1,1,1,1,1,1}};
inline constexpr std::array<std::uint32_t,14> owners{{0,1,2,3,4,5,6,7,8,9,10,11,12,13}};
inline constexpr std::array<std::uint32_t,14> localMaterials{};
}
class Owner final {
 struct Storage {a::MaterialTextureAssets50EA bank;std::array<Selection,14> selections;};
 std::unique_ptr<Storage> storage_;const mc_animation_source50MC::Owner* source_=nullptr;bool attempted_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 static constexpr std::size_t maximumPayloadReservation()noexcept{return sizeof(Owner)+sizeof(Storage)+4u*512u*1024u+65536u;}
 bool pixelsPinned()const noexcept{for(const auto& pin:detail::pins)if(!*pin.nativeSha)return false;return true;}
 bool loaded()const noexcept{return storage_&&source_&&source_->ready();}
 const a::MaterialTextureAssets50EA* bank()const noexcept{return loaded()?&storage_->bank:nullptr;}
 const std::array<Selection,14>* selections()const noexcept{return loaded()?&storage_->selections:nullptr;}
 const mc_animation_source50MC::Owner* sourceOwner()const noexcept{return source_;}
 bool load(const mc_animation_source50MC::Owner& source,Diagnostic& d)noexcept {
  namespace shared=pod_materials50ed_fixture::detail;using shared::need;
  try{
   d={};need(!attempted_,"MC dynamic material requires fresh lifetime");attempted_=true;
   need(source.ready()&&source.coldSource()&&source.coldSource()->ready(),"MC dynamic source unavailable");
   auto next=std::make_unique<Storage>();std::array<std::vector<std::uint8_t>,2> images;
   std::array<a::Material48E,2> materials;std::array<a::MaterialTextureAssetInput50EA,2> textures{};
   std::array<a::MaterialTextureSlotInput50EA,14> slots{};
   for(std::size_t i=0;i<detail::pins.size();++i){d.material=i;const auto& pin=detail::pins[i];
    const auto& psm=source.materialSource(i);
    need(psm.size()==pin.psmBytes&&shared::sha256(psm)==pin.psmSha,"MC exact dynamic PSM differs");
    need(a::decodeMaterial48E(psm,materials[i],d.source)&&materials[i].passes.size()==1,"MC dynamic material parser rejected");
    const auto& pass=materials[i].passes[0];need(pass.pass==1&&pass.microprogram=="DiffuseCompressed"&&pass.textures.size()==1&&pass.textures[0].slot==1&&pass.textures[0].name==pin.name,"MC dynamic authored map/program differs");
    std::vector<std::uint8_t> pst;const auto member=std::string("textures\\")+pin.name+".pst";
    need(source.coldSource()->extractMember(member,pst,d.source,pin.pstBytes)&&pst.size()==pin.pstBytes&&shared::sha256(pst)==pin.pstSha,"MC dynamic own texture differs");
    need(pin.kind==2&&pst.size()>=12&&shared::u32(pst.data()+4)==2,"MC exact dynamic kind2 texture");
    std::uint32_t mipIndex=0;auto mip=mc_original_world50LM::detail::framePst(pst,mipIndex);
    a::TextureMip48E decoded;decoded.width=mip.width;decoded.height=mip.height;std::string why;
    need(a::decodeOriginalIpuRgba(pst.data()+mip.payload,mip.bytes,mip.width,mip.height,decoded.rgba,why),why.c_str());
    need(a::encodeTextureLevel48E(decoded,images[i],d.source),"MC dynamic native texture framing");
    d.outputSha[i]=shared::sha256(images[i]);d.width[i]=mip.width;d.height[i]=mip.height;d.mip[i]=mipIndex;
    if(*pin.nativeSha)need(d.width[i]==pin.width&&d.height[i]==pin.height&&d.mip[i]==pin.mip&&d.outputSha[i]==pin.nativeSha,"MC dynamic measured native regression mismatch");
    textures[i]={i+1,&images[i]};
   }
   for(std::size_t i=0;i<14;++i){d.material=i;const auto pinIndex=detail::materialPins[i];const auto& pin=detail::pins[pinIndex];
    const auto owner=detail::owners[i],local=detail::localMaterials[i];const auto& scene=source.source(owner).scene;
    need(local<scene.materials.size()&&scene.materials[local]==pin.name&&scene.lods[0].surfaces[local].material_index==local,"MC fourteen-owner material/source order differs");
    const auto& pass=materials[pinIndex].passes[0];auto& selected=next->selections[i];
    selected.assetIdentity=0x4d43;selected.materialIndex=i;selected.materialName=pin.name;
    selected.materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+pin.psmSha);
    selected.textureName=pin.name;selected.textureKey=shared::sha256(std::string("rr-native-texture-48E-v1\n")+pin.pstSha);
    selected.textureIndex=pinIndex;selected.bankSlot=i;selected.mipIndex=d.mip[pinIndex];
    selected.pass=selected.slot=selected.passCount=1;selected.track=track_source_fixture::Track::mc;selected.program=pass.microprogram;
    selected.sourcePassFields=mc_original_world50LM::detail::fields(pass.fields);
    selected.clampU=mc_original_world50LM::detail::clamp(pass.fields,"wrapModeHorizontal");selected.clampV=mc_original_world50LM::detail::clamp(pass.fields,"wrapModeVertical");
    selected.decodedSourceMember=std::string("textures\\")+pin.name+".pst";selected.decodedSourceSha256=pin.pstSha;
    slots[i]={{selected.assetIdentity,static_cast<std::uint32_t>(i),1,1},pinIndex+1,a::MaterialTextureRequirement50EA::required};
   }
   const a::MaterialTextureBudgets50EA budget{2,14,512u*1024u,512u*1024u,256};
   need(next->bank.load({textures.data(),2,slots.data(),14,budget},d.bank),"MC dynamic source image bank rejected");
   source_=&source;storage_=std::move(next);return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.message="MC dynamic material preparation failure";}catch(...){}return false;}
 }
};
}
