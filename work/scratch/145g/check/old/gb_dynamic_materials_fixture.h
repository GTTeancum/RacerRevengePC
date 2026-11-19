#pragma once
#include "gb_animation_runtime50gb.h"
#include "gb_original_world50lm.h"

namespace gb_dynamic_materials50GB {
namespace a=rr::assets;using Selection=track_materials50fa_fixture::Selection;
struct Diagnostic {std::string message;std::size_t material=0;a::DecodeError48C source{};a::MaterialTextureAssetDiagnostic50EA bank{};};
namespace detail {
struct Pin {const char* name;std::size_t psmBytes;const char* psmSha;std::size_t pstBytes;const char* pstSha;std::uint32_t width,height,mip,kind;const char* nativeSha;};
inline constexpr std::array<Pin,7> pins{{
#include "gb_dynamic_materials_index.inc"
}};
inline constexpr std::array<std::uint32_t,10> materialPins{{0,1,2,3,4,1,0,1,5,6}};
inline constexpr std::array<std::uint32_t,10> owners{{0,0,1,1,1,1,2,2,3,3}};
inline constexpr std::array<std::uint32_t,10> localMaterials{{0,1,0,1,2,3,0,1,0,1}};
}
class Owner final {
 struct Storage {a::MaterialTextureAssets50EA bank;std::array<Selection,10> selections;};
 std::unique_ptr<Storage> storage_;const gb_animation_source50GB::Owner* source_=nullptr;bool attempted_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 bool loaded()const noexcept{return storage_&&source_&&source_->ready();}
 const a::MaterialTextureAssets50EA* bank()const noexcept{return loaded()?&storage_->bank:nullptr;}
 const std::array<Selection,10>* selections()const noexcept{return loaded()?&storage_->selections:nullptr;}
 const gb_animation_source50GB::Owner* sourceOwner()const noexcept{return source_;}
 bool load(const gb_animation_source50GB::Owner& source,Diagnostic& d)noexcept {
  namespace shared=pod_materials50ed_fixture::detail;using shared::need;
  try{
   d={};need(!attempted_,"GB dynamic material requires fresh lifetime");attempted_=true;
   need(source.ready()&&source.coldSource()&&source.coldSource()->ready(),"GB dynamic source unavailable");
   auto next=std::make_unique<Storage>();std::array<std::vector<std::uint8_t>,7> images;
   std::array<a::Material48E,7> materials;std::array<a::MaterialTextureAssetInput50EA,7> textures{};
   std::array<a::MaterialTextureSlotInput50EA,10> slots{};
   for(std::size_t i=0;i<detail::pins.size();++i){d.material=i;const auto& pin=detail::pins[i];
    const auto& psm=source.materialSource(i);
    need(psm.size()==pin.psmBytes&&shared::sha256(psm)==pin.psmSha,"GB exact dynamic PSM differs");
    need(a::decodeMaterial48E(psm,materials[i],d.source)&&materials[i].passes.size()==1,"GB dynamic material parser rejected");
    const auto& pass=materials[i].passes[0];need(pass.pass==1&&pass.microprogram=="DiffuseCompressed"&&pass.textures.size()==1&&pass.textures[0].slot==1&&pass.textures[0].name==pin.name,"GB dynamic authored map/program differs");
    std::vector<std::uint8_t> pst;const auto member=std::string("textures\\")+pin.name+".pst";
    need(source.coldSource()->extractMember(member,pst,d.source,pin.pstBytes)&&pst.size()==pin.pstBytes&&shared::sha256(pst)==pin.pstSha,"GB dynamic own texture differs");
    // All seven authentic GB dynamic PSTs are indexed kind1, not GA's
    // differently encoded same-name files or an injected fallback decoder.
    need(pin.kind==1&&pst.size()>=12&&shared::u32(pst.data()+4)==1,"GB dynamic indexed texture kind differs");
    a::Texture48E decoded;need(a::decodeTexture48E(pst,decoded,d.source)&&decoded.mips.size()==pin.mip+1&&decoded.mips.back().width==pin.width&&decoded.mips.back().height==pin.height,"GB dynamic indexed decode rejected");
    images[i]=std::move(decoded.base_rrtex);
    need(images[i].size()==16u+std::size_t(pin.width)*pin.height*4&&shared::sha256(images[i])==pin.nativeSha,"GB independently pinned dynamic pixels differ");
    textures[i]={i+1,&images[i]};
   }
   for(std::size_t i=0;i<10;++i){d.material=i;const auto pinIndex=detail::materialPins[i];const auto& pin=detail::pins[pinIndex];
    const auto owner=detail::owners[i],local=detail::localMaterials[i];const auto& scene=source.source(owner).scene;
    need(local<scene.materials.size()&&scene.materials[local]==pin.name&&scene.lods[0].surfaces[local].material_index==local,"GB four-owner material/source order differs");
    const auto& pass=materials[pinIndex].passes[0];auto& selected=next->selections[i];
    selected.assetIdentity=0x4742;selected.materialIndex=i;selected.materialName=pin.name;
    selected.materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+pin.psmSha);
    selected.textureName=pin.name;selected.textureKey=shared::sha256(std::string("rr-native-texture-48E-v1\n")+pin.pstSha);
    selected.textureIndex=pinIndex;selected.bankSlot=i;selected.mipIndex=pin.mip;
    selected.pass=selected.slot=selected.passCount=1;selected.track=track_source_fixture::Track::gb;selected.program=pass.microprogram;
    selected.sourcePassFields=gb_original_world50LM::detail::fields(pass.fields);
    selected.clampU=gb_original_world50LM::detail::clamp(pass.fields,"wrapModeHorizontal");selected.clampV=gb_original_world50LM::detail::clamp(pass.fields,"wrapModeVertical");
    selected.decodedSourceMember=std::string("textures\\")+pin.name+".pst";selected.decodedSourceSha256=pin.pstSha;
    slots[i]={{selected.assetIdentity,static_cast<std::uint32_t>(i),1,1},pinIndex+1,a::MaterialTextureRequirement50EA::required};
   }
   const a::MaterialTextureBudgets50EA budget{7,10,512u*1024u,512u*1024u,128};
   need(next->bank.load({textures.data(),7,slots.data(),10,budget},d.bank),"GB dynamic source image bank rejected");
   source_=&source;storage_=std::move(next);return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.message="GB dynamic material preparation failure";}catch(...){}return false;}
 }
};
}
