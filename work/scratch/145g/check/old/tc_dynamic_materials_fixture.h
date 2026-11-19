#pragma once
#include "tc_gate_runtime50tc.h"
#include "tc_original_world50lm.h"

namespace tc_dynamic_materials50TC {
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
 bool load(const tc_gate_motion50TC::SourceOwner& source,Diagnostic& d)noexcept {
  namespace shared=pod_materials50ed_fixture::detail;using shared::need;
  try {
   d={};need(!attempted_,"TC gate material requires a fresh lifetime");attempted_=true;
   need(source.ready()&&source.coldSource()&&source.coldSource()->ready()&&
       source.scene().materials==std::vector<std::string>{"tc_gate"},"TC gate requires its actual source scene/material");
   const auto& archive=source.coldSource()->archive();
   std::vector<std::uint8_t> psm,pst,pixels;
   need(a::extractResMember48C(archive,"textures\\tc_gate.psm",psm,d.source,2622)&&psm.size()==2622&&
       shared::sha256(psm)=="4b3d1f1209d738210603de5bbe395bda9d0e90b6155d2dc243b08303b737fa01","TC gate PSM identity");
   need(a::extractResMember48C(archive,"textures\\tc_gate.pst",pst,d.source,6921)&&pst.size()==6921&&
       shared::sha256(pst)=="7793fd7b085f90f8b8b6b0697c80bd46e228566bfbb77e6840ffce29f0877099","TC gate PST identity");
   a::Material48E material;need(a::decodeMaterial48E(psm,material,d.source)&&material.passes.size()==1,"TC gate original material parser");
   const auto& pass=material.passes.front();need(pass.pass==1&&pass.microprogram=="DiffuseCompressed"&&
       pass.textures.size()==1&&pass.textures[0].slot==1&&pass.textures[0].name=="tc_gate","TC gate exact authored pass/map");
   std::uint32_t mipIndex=0;
   // Reuse the already admitted original PST framing algorithm only after the
   // distinct gate bytes have been authenticated; no static-image pin retag.
   const auto mip=tc_original_world50LM::detail::framePst(pst,mipIndex);
   need(mipIndex==3&&mip.width==128&&mip.height==128,"TC gate original mip dimensions");
   a::TextureMip48E decoded;decoded.width=mip.width;decoded.height=mip.height;std::string why;
   const bool decodedOk=a::decodeOriginalIpuRgba(pst.data()+mip.payload,mip.bytes,mip.width,mip.height,decoded.rgba,why);
   need(decodedOk,"TC gate own IPUM decode: "+why);
   need(a::encodeTextureLevel48E(decoded,pixels,d.source)&&pixels.size()==65552,"TC gate native pixel framing");
   auto next=std::make_unique<Storage>();auto& selected=next->selection;
   selected.assetIdentity=0x5443ull;selected.materialIndex=0;selected.materialName="tc_gate";
   selected.materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+shared::sha256(psm));
   selected.textureName="tc_gate";selected.textureKey=shared::sha256(std::string("rr-native-texture-48E-v1\n")+shared::sha256(pst));
   selected.textureIndex=selected.bankSlot=0;selected.mipIndex=3;selected.pass=selected.slot=selected.passCount=1;
   selected.track=track_source_fixture::Track::tc;selected.program=pass.microprogram;
   selected.sourcePassFields=tc_original_world50LM::detail::fields(pass.fields);
   selected.clampU=tc_original_world50LM::detail::clamp(pass.fields,"wrapModeHorizontal");
   selected.clampV=tc_original_world50LM::detail::clamp(pass.fields,"wrapModeVertical");
   selected.decodedSourceMember="textures\\tc_gate.pst";selected.decodedSourceSha256=shared::sha256(pst);
   next->measuredNativeSha=shared::sha256(pixels);
   const a::MaterialTextureAssetInput50EA image{1,&pixels};
   const a::MaterialTextureSlotInput50EA slot{{selected.assetIdentity,0,1,1},1,a::MaterialTextureRequirement50EA::required};
   const a::MaterialTextureBudgets50EA budgets{1,1,256u*1024u,256u*1024u,128};
   need(next->bank.load({&image,1,&slot,1,budgets},d.bank),"TC gate own native image bank");
   storage_=std::move(next);return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.message="TC gate material preparation failure";}catch(...){}return false;}
 }
};
}
