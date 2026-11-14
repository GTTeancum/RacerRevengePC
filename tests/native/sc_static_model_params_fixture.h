#pragma once
#include "sc_trigger_authoring_fixture.h"
#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace sc_static_model_params {
namespace a=rr::assets;
inline constexpr std::size_t stockCount=451; // receipt only; the TOB's own count is used
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
 std::vector<std::size_t> matched;
 std::size_t writes=0,unmatched=0;
};
class Owner final {
 std::vector<Source> source_;
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
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool load(const std::vector<std::uint8_t>& authenticatedTob,std::string& error){
  if(attempted_){error="SC static model parameter owner requires fresh lifetime";return false;}attempted_=true;
  try{using namespace ga_trigger_authoring::detail;
   // Read from the course's own TOB (stock identity is a receipt). The
   // original's 271A00 reads [StaticModelParams] NumberOfStaticModelParams
   // and each Model<n> SLT/AllowShadowReceiving; absent block = none.
   course_archive_receipt50CI::memberReceipt("sc_static_model_params","tracks\\sc.tob",authenticatedTob,0,0,
       sc_trigger_authoring::tobBytes,sc_trigger_authoring::tobSha);
   a::ParameterDocument48F document;a::DecodeError48C why{};
   need(a::decodeParameters48F(authenticatedTob,document,why),"SC static model TOB rejected");
   const a::ParameterBlock48F* header=nullptr;
   for(const auto& b:document.blocks)if(b.name=="staticmodelparams"){need(!header,"SC duplicate StaticModelParams");header=&b;}
   std::size_t count=0;
   if(header){const std::string* text=nullptr;
    for(const auto& f:header->fields)if(f.name=="numberofstaticmodelparams"){need(!text,"SC duplicate static model count");text=&f.value;}
    if(text)count=integer(*text);}
   need(count<=document.blocks.size(),"SC static model count exceeds the TOB's blocks");
   course_archive_receipt50CI::countReceipt("sc_static_model_params","rows",count,stockCount);
   std::vector<Source> next(count);
   for(std::size_t i=0;i<count;++i){const auto& b=block(document,"model"+std::to_string(i+1));auto& s=next[i];
    s.slt=field(b,"slt");
    //271A9C..AC calls1169D0 to find the LAST dot;271AB4..C8 clears its suffix.
    s.modelName=s.slt.substr(0,s.slt.rfind('.'));need(!s.modelName.empty(),"SC static model source name empty");
    // Absent AllowShadowReceiving keeps the 27A208/27A234 constructor default 1.
    const std::string* allow=nullptr;for(const auto& f:b.fields)if(f.name=="allowshadowreceiving")allow=&f.value;
    s.allowShadowReceiving148=allow?(boolean(*allow)?1u:0u):1u;
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
  Receipt next{};next.matched.assign(source_.size(),0);
  for(std::size_t s=0;s<source_.size();++s){
   for(std::size_t i=0;i<modelCount;++i)if(models[i].type==1&&equal(*models[i].sourceModelName,source_[s].modelName)){
    *models[i].allowShadowReceiving148=source_[s].allowShadowReceiving148;++next.matched[s];++next.writes;
   }
   if(!next.matched[s])++next.unmatched;
  }
  receipt=next;return true;
 }
 bool ready()const noexcept{return ready_;}
 const Source& source(std::size_t i)const{
  if(!ready_)throw std::runtime_error("SC static model source owner not ready");return source_.at(i);
 }
};
} // namespace sc_static_model_params
