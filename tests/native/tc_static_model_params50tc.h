#pragma once
#include "tc_trigger_authoring50tc.h"
#include <array>

namespace tc_static_model_params50TC {
namespace a=rr::assets;
inline constexpr std::size_t count=19;
struct Source {
 std::uint32_t requestedOrdinal=0,effectiveOrdinal=0;
 std::size_t sltFieldOrdinal=0,shadowFieldOrdinal=0;
 bool selectedRequestedBlock=false;
 std::string effectiveBlock,slt,modelName;
 std::uint32_t allowShadowReceiving148=1;
};
struct ModelLease {const std::string* sourceModelName=nullptr;std::uint32_t* allowShadowReceiving148=nullptr;std::uint32_t type=1;};
struct Receipt {std::array<std::size_t,count> matched{};std::size_t writes=0,unmatched=0;};
namespace detail {
inline const a::ParameterBlock48F* findBlock(const a::ParameterDocument48F& p,const std::string& name)noexcept {
 for(const auto& b:p.blocks)if(b.name==name)return &b;return nullptr;
}
// Exact packed1583AC midpoint search and final endpoint probes. This is not a
// generic duplicate-label policy. It is used only after authentic TC admission.
inline const a::ParameterField48F* packedField(const a::ParameterBlock48F& b,const std::string& name,std::size_t& ordinal)noexcept {
 if(b.fields.empty())return nullptr;
 if(b.fields.size()==1){if(b.fields[0].name!=name)return nullptr;ordinal=0;return &b.fields[0];}
 std::size_t low=0,high=b.fields.size()-1,middle=(high-low)/2;
 for(;;){const int comparison=name.compare(b.fields[middle].name);
  if(!comparison){ordinal=middle;return &b.fields[middle];}
  if(comparison>=0)low=middle;else high=middle;
  if(high-low<2)break;middle=low+(high-low)/2;
 }
 if(b.fields[low].name==name){ordinal=low;return &b.fields[low];}
 if(b.fields[high].name==name){ordinal=high;return &b.fields[high];}
 return nullptr;
}
inline bool equal(const std::string& a,const std::string& b)noexcept {
 if(a.size()!=b.size())return false;
 for(std::size_t i=0;i<a.size();++i){auto x=a[i],y=b[i];
  if(x>='A'&&x<='Z')x=static_cast<char>(x-'A'+'a');if(y>='A'&&y<='Z')y=static_cast<char>(y-'A'+'a');if(x!=y)return false;}
 return true;
}
}
class Owner final {
 std::shared_ptr<const tc_cold_sources50LA::Owner> cold_;
 std::array<Source,count> source_{};bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool decode(std::shared_ptr<const tc_cold_sources50LA::Owner> cold,std::string& error){
  if(attempted_){error="TC static parameters require a fresh lifetime";return false;}attempted_=true;
  try{using namespace tc_trigger_authoring50TC::detail;
   need(cold&&cold->ready()&&cold->originalTob().size()==tc_trigger_authoring50TC::tobBytes&&
       pod_materials50ed_fixture::detail::sha256(cold->originalTob())==tc_trigger_authoring50TC::tobSha,"TC static source identity");
   const auto& p=cold->triggerParameters();
   need(p.preservesDuplicateLabels&&p.original_bytes==cold->originalTob(),"TC static source retains ordered duplicate fields");
   const auto* selected=detail::findBlock(p,"staticmodelparams");
   need(selected&&integer(field(*selected,"numberofstaticmodelparams"))==count,"TC static declared count");
   const auto& duplicate=block(p,"model3");
   need(duplicate.fields.size()==4&&duplicate.fields[0].name=="allowshadowreceiving"&&duplicate.fields[0].value=="F"&&
       duplicate.fields[1].name=="allowshadowreceiving"&&duplicate.fields[1].value=="F"&&
       duplicate.fields[2].name=="slt"&&duplicate.fields[2].value=="tc_beam3"&&
       duplicate.fields[3].name=="slt"&&duplicate.fields[3].value=="tc_beam4","TC original duplicate model3 order");
   need(!detail::findBlock(p,"model4")&&!detail::findBlock(p,"model17")&&!detail::findBlock(p,"model18")&&!detail::findBlock(p,"model19")&&
       detail::findBlock(p,"mode17")&&detail::findBlock(p,"mode18")&&detail::findBlock(p,"mode19"),"TC original missing/misspelled row identity");
   constexpr std::uint32_t effective[count]={1,2,3,3,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16};
   std::array<Source,count> next{};std::uint32_t selectedOrdinal=0;
   for(std::size_t i=0;i<count;++i){auto& s=next[i];s.requestedOrdinal=static_cast<std::uint32_t>(i+1);
    const auto* requested=detail::findBlock(p,"model"+std::to_string(i+1));
    //271A74 ignores157D00 failure. Missing sections retain selected buffer.
    if(requested){selected=requested;selectedOrdinal=s.requestedOrdinal;s.selectedRequestedBlock=true;}
    s.effectiveOrdinal=selectedOrdinal;s.effectiveBlock=selected->name;
    need(s.effectiveOrdinal==effective[i],"TC original retained selector mapping");
    const auto* slt=detail::packedField(*selected,"slt",s.sltFieldOrdinal);
    const auto* shadow=detail::packedField(*selected,"allowshadowreceiving",s.shadowFieldOrdinal);
    need(slt&&shadow,"TC consumed static fields absent");s.slt=slt->value;
    s.modelName=s.slt.substr(0,s.slt.rfind('.'));need(!s.modelName.empty(),"TC static model stem empty");
    s.allowShadowReceiving148=boolean(shadow->value)?1u:0u;need(!s.allowShadowReceiving148,"TC authored shadow flag changed");
    if(s.effectiveOrdinal==3)need(s.sltFieldOrdinal==2&&s.shadowFieldOrdinal==1&&s.slt=="tc_beam3","TC duplicate midpoint selection");
    if(s.effectiveOrdinal==16)need(s.slt=="seat03.slt","TC retained final row selection");
   }
   source_=std::move(next);cold_=std::move(cold);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool apply(const ModelLease* models,std::size_t modelCount,Receipt& receipt)const noexcept {
  if(!ready()||(modelCount&&!models)||modelCount>100000)return false;
  for(std::size_t i=0;i<modelCount;++i)if(models[i].type==1&&(!models[i].sourceModelName||!models[i].allowShadowReceiving148))return false;
  Receipt next{};
  for(std::size_t s=0;s<count;++s){
   for(std::size_t i=0;i<modelCount;++i)if(models[i].type==1&&detail::equal(*models[i].sourceModelName,source_[s].modelName)){
    *models[i].allowShadowReceiving148=source_[s].allowShadowReceiving148;++next.matched[s];++next.writes;
   }
   if(!next.matched[s])++next.unmatched;
  }
  receipt=next;return true;
 }
 bool ready()const noexcept{return ready_&&cold_&&cold_->ready();}
 const Source& source(std::size_t i)const{if(!ready())throw std::runtime_error("TC static source not ready");return source_.at(i);}
 const tc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
};
}
