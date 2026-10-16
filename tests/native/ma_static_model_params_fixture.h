#pragma once
#include "ma_trigger_authoring_fixture.h"
#include <array>
#include <string>
#include <vector>

namespace ma_static_model_params {
namespace a=rr::assets;
inline constexpr std::size_t count=145;
struct Source {
 std::string slt,modelName;
 std::uint32_t allowShadowReceiving148=1; //27A208/27A234 constructor default
};
// Borrow genuine existing static type1 model owners in zone/table order.
// Their source name is the model object's name at+0, reached from entry+10.
// No .SCB useLighting substitution and no renderer/shadow implementation.
struct ModelLease {
 const std::string* sourceModelName=nullptr;
 std::uint32_t* allowShadowReceiving148=nullptr;
 std::uint32_t type=1;
};
struct Receipt {
 std::array<std::size_t,count> matched{};
 std::size_t writes=0,unmatched=0;
};
class Owner final {
 std::array<Source,count> source_{};
 bool attempted_=false,ready_=false;
 static bool equal(const std::string& a,const std::string& b)noexcept {
  // Original308F00 is case-insensitive ASCII for these authenticated names.
  if(a.size()!=b.size())return false;
  for(std::size_t i=0;i<a.size();++i){auto x=a[i],y=b[i];
   if(x>='A'&&x<='Z')x=static_cast<char>(x+('a'-'A'));
   if(y>='A'&&y<='Z')y=static_cast<char>(y+('a'-'A'));
   if(x!=y)return false;
  }return true;
 }
public:
 bool load(const std::vector<std::uint8_t>& authenticatedTob,std::string& error){
  if(attempted_){error="MA static model parameter owner requires fresh lifetime";return false;}attempted_=true;
  try{using namespace ma_trigger_authoring::detail;
   need(authenticatedTob.size()==ma_trigger_authoring::tobBytes&&
      pod_materials50ed_fixture::detail::sha256(authenticatedTob)==ma_trigger_authoring::tobSha,"MA static model original TOB identity");
   a::ParameterDocument48F document;a::DecodeError48C why{};
   need(a::decodeParameters48F(authenticatedTob,document,why),"MA static model original TOB rejected");
   need(integer(field(block(document,"staticmodelparams"),"numberofstaticmodelparams"))==count,"MA static model original count changed");
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){const auto& b=block(document,"model"+std::to_string(i+1));auto& s=next[i];
    need(b.fields.size()==2,"MA static model source fields changed");s.slt=field(b,"slt");
    //271A9C..AC calls1169D0 to find the LAST dot;271AB4..C8 clears its suffix.
    s.modelName=s.slt.substr(0,s.slt.rfind('.'));need(!s.modelName.empty(),"MA static model source name empty");
    s.allowShadowReceiving148=boolean(field(b,"allowshadowreceiving"))?1u:0u;
    need(!s.allowShadowReceiving148,"MA static model authored flag changed");
   }
   source_=std::move(next);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool apply(const ModelLease* models,std::size_t modelCount,Receipt& receipt)const noexcept {
  if(!ready_||(modelCount&&!models)||modelCount>100000)return false;
  // Validate every writable owner first. Unmatched source rows are legal:
  //271A00 never loads/spawns a missing model or fabricates a new placement.
  for(std::size_t i=0;i<modelCount;++i)if(models[i].type==1&&
     (!models[i].sourceModelName||!models[i].allowShadowReceiving148))return false;
  Receipt next{};
  for(std::size_t s=0;s<count;++s){
   for(std::size_t i=0;i<modelCount;++i)if(models[i].type==1&&equal(*models[i].sourceModelName,source_[s].modelName)){
    *models[i].allowShadowReceiving148=source_[s].allowShadowReceiving148;++next.matched[s];++next.writes;
   }
   if(!next.matched[s])++next.unmatched;
  }
  receipt=next;return true;
 }
 bool ready()const noexcept{return ready_;}
 const Source& source(std::size_t i)const{return source_.at(i);}
};
} // namespace ma_static_model_params
