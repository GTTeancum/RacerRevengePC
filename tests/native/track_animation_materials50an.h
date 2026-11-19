#pragma once
// Common track-animation MATERIALS (CPU side of piece 4, ANIM.md): for every span of the one
// dynamic mesh, the surface's PSG material name -> textures\<name>.psm (Material48E) -> pass 1
// TextureMap_1 -> textures\<map>.pst, decoded with the port's own decoders (kind0/1 indexed:
// decodeTexture48E; kind2 IPUM: decodeOriginalIpuRgba on the largest mip). One texture bank
// for the course; nothing is keyed on a course, a name list or a hash.
#include "track_animation_runtime50an.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/material_texture_assets50ea.h"
#include "../../src/assets/original_ipu_decoder.h"
#include <map>

namespace track_animation_materials50AN {
namespace a=rr::assets;namespace rt=track_animation_runtime50AN;namespace src=track_animation_source50AN;
struct Diagnostic {std::string message;std::size_t span=0;std::string member;a::DecodeError48C source{};a::MaterialTextureAssetDiagnostic50EA bank{};};
struct Material {
 std::string name,program,texture;            // PSG material, pass-1 microprogram, TextureMap_1
 a::MaterialFields48E fields;                 // pass-1 authored/unspecified fields (alpha policy input)
 std::uint32_t passes=0,textureIndex=a::unresolvedMaterialTexture50EA;
 bool clampU=false,clampV=false;
};
namespace detail {
using src::detail::need;using src::detail::lowered;using src::detail::u32;
inline std::string field(const a::MaterialFields48E& f,const char* name){
 std::string out;bool found=false;
 for(const auto& x:f.authored)if(lowered(x.name)==lowered(name)){need(!found,std::string("duplicate material field ")+name);found=true;out=x.value;}
 return out;}
// Largest mip of an original kind2 (IPUM) PST: header 1,2,count; per mip h,w,0,0,bytes then "ipum".
// PORT STAND-IN (flagged): the same framing exists as per-course copies (framePst in
// gb/mc/mb/ga/all-pass world loaders); converge them on one shared reader.
struct Mip {std::uint32_t width=0,height=0;std::size_t payload=0,bytes=0;std::uint32_t index=0;};
inline Mip frameIpum(const std::vector<std::uint8_t>& s){
 const auto range=[&](std::size_t at,std::size_t n){return at<=s.size()&&n<=s.size()-at;};
 const auto word=[&](std::size_t at){need(range(at,4),"truncated IPUM PST");return u32(s.data()+at);};
 const auto dimension=[](std::uint32_t n){return n>=16&&n<=1024&&!(n&(n-1));};
 need(s.size()<=8u*1024u*1024u&&word(0)==1&&word(4)==2&&word(8)>=1&&word(8)<=5,"IPUM PST header rejected");
 const auto count=word(8);std::size_t at=12;std::uint32_t pw=0,ph=0;Mip largest{};
 for(std::uint32_t i=0;i<count;++i){
  need(range(at,20),"IPUM mip header extent");
  const auto h=word(at),w=word(at+4),bytes=word(at+16);const auto p=at+20;
  need(dimension(w)&&dimension(h)&&!word(at+8)&&!word(at+12)&&(!i||(w==pw*2&&h==ph*2))&&bytes>=25&&range(p,bytes),"IPUM mip extent/order");
  need(!std::memcmp(s.data()+p,"ipum",4)&&word(p+4)==bytes-8,"IPUM frame header");
  largest={w,h,p,bytes,i};at=p+bytes;pw=w;ph=h;
 }
 need(at==s.size(),"IPUM PST trailing bytes");return largest;
}
} // namespace detail

class Owner final {
 struct Storage {a::MaterialTextureAssets50EA bank;std::vector<Material> materials;std::vector<std::uint32_t> spanMaterial;std::uint32_t textures=0;};
 std::unique_ptr<Storage> storage_;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool loaded()const noexcept{return bool(storage_);}
 const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
 std::size_t textureCount()const noexcept{return storage_?storage_->textures:0;}
 const Material& spanMaterial(std::size_t span)const{return storage_->materials.at(storage_->spanMaterial.at(span));}
 std::size_t materialCount()const noexcept{return storage_?storage_->materials.size():0;}
 const Material& material(std::size_t i)const{return storage_->materials.at(i);}
 bool load(const rt::Owner& runtime,const src::Extract& extract,Diagnostic& d)noexcept{
  try{using namespace detail;d={};need(!storage_&&runtime.ready()&&static_cast<bool>(extract),"track animation materials need a fresh owner and a ready runtime");
   auto next=std::make_unique<Storage>();
   std::map<std::string,std::uint32_t> byMaterial,byTexture;std::vector<std::vector<std::uint8_t>> images;
   const auto& spans=runtime.spans();
   for(std::size_t i=0;i<spans.size();++i){d.span=i;const auto& span=spans[i];
    const auto& name=runtime.source().row(span.row).scene.materials.at(span.material);
    const auto key=lowered(name);auto found=byMaterial.find(key);
    if(found==byMaterial.end()){
     Material m{};m.name=name;std::vector<std::uint8_t> psm;d.member="textures\\"+name+".psm";
     need(extract(d.member,psm,64u*1024u)&&!psm.empty(),"animation material PSM missing: "+d.member);
     a::Material48E decoded;need(a::decodeMaterial48E(psm,decoded,d.source),"animation material PSM rejected: "+d.member);
     m.passes=static_cast<std::uint32_t>(decoded.passes.size());
     need(m.passes>=1,"animation material without a shader pass: "+d.member);
     // PORT STAND-IN (flagged): one textured pass per span, as every per-course dynamic window drew.
     need(m.passes==1,"animation material with more than one shader pass is not drawn yet: "+d.member);
     const auto& pass=decoded.passes[0];m.program=pass.microprogram;m.fields=pass.fields;
     const a::MaterialTexture48E* map=nullptr;for(const auto& t:pass.textures)if(t.slot==1)map=&t;
     need(map&&!map->name.empty(),"animation material pass 1 has no TextureMap_1: "+d.member);m.texture=map->name;
     const auto wrapU=field(pass.fields,"wrapModeHorizontal"),wrapV=field(pass.fields,"wrapModeVertical");
     need((wrapU.empty()||wrapU=="0"||wrapU=="1")&&(wrapV.empty()||wrapV=="0"||wrapV=="1"),"animation material wrap mode unsupported: "+d.member);
     m.clampU=wrapU=="1";m.clampV=wrapV=="1";
     const auto tkey=lowered(m.texture);auto t=byTexture.find(tkey);
     if(t==byTexture.end()){
      std::vector<std::uint8_t> pst,rrtex;d.member="textures\\"+m.texture+".pst";
      need(extract(d.member,pst,8u*1024u*1024u)&&pst.size()>=12,"animation texture PST missing: "+d.member);
      const auto kind=u32(pst.data()+4);
      if(kind==2){const auto mip=frameIpum(pst);a::TextureMip48E level;level.width=mip.width;level.height=mip.height;std::string why;
       need(a::decodeOriginalIpuRgba(pst.data()+mip.payload,mip.bytes,mip.width,mip.height,level.rgba,why),"animation IPUM texture decode: "+why);
       need(a::encodeTextureLevel48E(level,rrtex,d.source),"animation IPUM texture framing rejected: "+d.member);
      }else{a::Texture48E texture;need((kind==0||kind==1)&&a::decodeTexture48E(pst,texture,d.source)&&!texture.base_rrtex.empty(),"animation indexed texture rejected: "+d.member);
       rrtex=std::move(texture.base_rrtex);}
      t=byTexture.emplace(tkey,static_cast<std::uint32_t>(images.size())).first;images.push_back(std::move(rrtex));
     }
     m.textureIndex=t->second;
     found=byMaterial.emplace(key,static_cast<std::uint32_t>(next->materials.size())).first;next->materials.push_back(std::move(m));
    }
    next->spanMaterial.push_back(found->second);
   }
   d.member.clear();
   std::vector<a::MaterialTextureAssetInput50EA> textures(images.size());std::size_t bytes=0;
   for(std::size_t i=0;i<images.size();++i){textures[i]={i+1,&images[i]};bytes+=images[i].size();}
   std::vector<a::MaterialTextureSlotInput50EA> slots(spans.size());
   for(std::size_t i=0;i<spans.size();++i){const auto& m=next->materials[next->spanMaterial[i]];
    slots[i]={{0x414eu,static_cast<std::uint32_t>(i),1,1},std::uint64_t(m.textureIndex)+1,a::MaterialTextureRequirement50EA::required};}
   const a::MaterialTextureBudgets50EA budgets{static_cast<std::uint32_t>(images.size()),static_cast<std::uint32_t>(slots.size()),bytes,bytes,1024};
   need(next->bank.load({textures.data(),static_cast<std::uint32_t>(textures.size()),slots.data(),static_cast<std::uint32_t>(slots.size()),budgets},d.bank),
     "animation texture bank rejected");
   next->textures=static_cast<std::uint32_t>(images.size());storage_=std::move(next);return true;
  }catch(const src::detail::Fail& f){try{d.message=f.message;}catch(...){}}
   catch(const std::exception& e){try{d.message=e.what();}catch(...){}}
   catch(...){}
  if(d.message.empty())d.message="track animation material preparation failed";return false;
 }
};
} // namespace track_animation_materials50AN
