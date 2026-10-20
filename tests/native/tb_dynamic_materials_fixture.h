#pragma once
#include "tb_door_runtime50tb.h"
#include "tb_original_world50lm.h"

namespace tb_dynamic_materials50TB {
namespace a=rr::assets;
using Selection=track_materials50fa_fixture::Selection;
struct Diagnostic {std::string message;a::DecodeError48C source{};a::MaterialTextureAssetDiagnostic50EA bank{};};
class Owner final {
 struct Storage {a::MaterialTextureAssets50EA bank;Selection selection;std::string measuredNativeSha;};
 std::unique_ptr<Storage> storage_;
 bool attempted_=false;
public:
 bool loaded()const noexcept{return bool(storage_);}
 const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
 const Selection* selection()const noexcept{return storage_?&storage_->selection:nullptr;}
 const std::string* measuredNativeSha()const noexcept{return storage_?&storage_->measuredNativeSha:nullptr;}
 bool load(const tb_door_motion50TB::SourceOwner& source,Diagnostic& d)noexcept {
  namespace shared=pod_materials50ed_fixture::detail;using shared::need;
  try {
   d={};need(!attempted_,"TB door material requires a fresh lifetime");attempted_=true;
   need(source.ready()&&source.coldSource()&&source.coldSource()->ready()&&
       source.scene().materials==std::vector<std::string>{"tb_door"},"TB door requires its actual source scene/material");
   std::vector<std::uint8_t> psm,pst,pixels;
   need(source.coldSource()->extractMember("textures\\tb_door.psm",psm,d.source,2580)&&psm.size()==2580&&
       shared::sha256(psm)=="9c0e98e6e9cfad9d63fdbc27c087375094eb281851ffc72172509ac1e36996ad","TB door PSM identity");
   need(source.coldSource()->extractMember("textures\\tb_door.pst",pst,d.source,8375)&&pst.size()==8375&&
       shared::sha256(pst)=="ea77b224cf4067cf0161472d85bb8b718064404a42aff0435d6d928d61d3d7f9","TB door PST identity");
   a::Material48E material;need(a::decodeMaterial48E(psm,material,d.source)&&material.passes.size()==1,"TB door original material parser");
   const auto& pass=material.passes.front();need(pass.pass==1&&pass.microprogram=="DiffuseCompressed"&&
       pass.textures.size()==1&&pass.textures[0].slot==1&&pass.textures[0].name=="tb_door","TB door exact authored pass/map");
   std::uint32_t mipIndex=0;
   // Reuse the already admitted original PST framing algorithm only after the
   // distinct gate bytes have been authenticated; no static-image pin retag.
   const auto mip=tb_original_world50LM::detail::framePst(pst,mipIndex);
   need(mipIndex==3&&mip.width==128&&mip.height==128,"TB door original mip dimensions");
   a::TextureMip48E decoded;decoded.width=mip.width;decoded.height=mip.height;std::string why;
   const bool decodedOk=a::decodeOriginalIpuRgba(pst.data()+mip.payload,mip.bytes,mip.width,mip.height,decoded.rgba,why);
   need(decodedOk,"TB door own IPUM decode: "+why);
   need(a::encodeTextureLevel48E(decoded,pixels,d.source)&&pixels.size()==65552,"TB door native pixel framing");
   auto next=std::make_unique<Storage>();auto& selected=next->selection;
   selected.assetIdentity=0x5442ull;selected.materialIndex=0;selected.materialName="tb_door";
   selected.materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+shared::sha256(psm));
   selected.textureName="tb_door";selected.textureKey=shared::sha256(std::string("rr-native-texture-48E-v1\n")+shared::sha256(pst));
   selected.textureIndex=selected.bankSlot=0;selected.mipIndex=3;selected.pass=selected.slot=selected.passCount=1;
   selected.track=track_source_fixture::Track::tb;selected.program=pass.microprogram;
   selected.sourcePassFields=tb_original_world50LM::detail::fields(pass.fields);
   selected.clampU=tb_original_world50LM::detail::clamp(pass.fields,"wrapModeHorizontal");
   selected.clampV=tb_original_world50LM::detail::clamp(pass.fields,"wrapModeVertical");
   selected.decodedSourceMember="textures\\tb_door.pst";selected.decodedSourceSha256=shared::sha256(pst);
   next->measuredNativeSha=shared::sha256(pixels);
   const a::MaterialTextureAssetInput50EA image{1,&pixels};
   const a::MaterialTextureSlotInput50EA slot{{selected.assetIdentity,0,1,1},1,a::MaterialTextureRequirement50EA::required};
   const a::MaterialTextureBudgets50EA budgets{1,1,256u*1024u,256u*1024u,128};
   need(next->bank.load({&image,1,&slot,1,budgets},d.bank),"TB door own native image bank");
   storage_=std::move(next);return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.message="TB door material preparation failure";}catch(...){}return false;}
 }
};
}
