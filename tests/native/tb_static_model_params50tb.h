#pragma once
#include "tb_cold_sources50la.h"
#include <array>

namespace tb_static_model_params50TB {
namespace a=rr::assets;
inline constexpr std::size_t count=690;
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
// generic duplicate-label policy. It is used only after authentic TB admission.
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
 std::shared_ptr<const tb_cold_sources50LA::Owner> cold_;
 std::array<Source,count> source_{};bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 // Conservative retained payload reservation. Root charges this before load;
 // the source is shared and separately charged. String allocator overhead is
 // not represented as payload, and the fixed array is included by sizeof.
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept{return sizeof(Owner)+count*3u*33u;}
 std::size_t retainedPayloadBytes()const noexcept {
  std::size_t n=sizeof(Owner);for(const auto& s:source_)n+=s.effectiveBlock.capacity()+1+s.slt.capacity()+1+s.modelName.capacity()+1;return n;
 }
 bool load(std::shared_ptr<const tb_cold_sources50LA::Owner> cold,std::string& error){
  if(attempted_){error="TB static parameters require a fresh lifetime";return false;}attempted_=true;
  try{
   const auto need=[](bool v,const char* m){if(!v)throw std::runtime_error(m);};
   need(cold&&cold->ready()&&cold->originalTob().size()==106139&&
       pod_materials50ed_fixture::detail::sha256(cold->originalTob())=="12c908874736ea91dcd6d3f8af97413a0cfe3bcd87cfce0798e8ff86fe5fb0bc","TB static source identity");
   const auto& p=cold->triggerParameters();
   need(p.preservesDuplicateLabels&&p.original_bytes==cold->originalTob(),"TB exact retained parameter document");
   const auto* declared=detail::findBlock(p,"staticmodelparams");std::size_t ordinal=0;
   const auto* number=declared?detail::packedField(*declared,"numberofstaticmodelparams",ordinal):nullptr;
   need(number&&number->value=="690","TB static declared count");
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){auto& s=next[i];s.requestedOrdinal=static_cast<std::uint32_t>(i+1);
    const auto* selected=detail::findBlock(p,"model"+std::to_string(i+1));
    // Actual TB has every requested model block with no duplicate model fields. Do
    // not import TC's missing-section behavior or deduplicate repeated names.
    need(selected&&selected->fields.size()==2&&selected->fields[0].name=="allowshadowreceiving"&&selected->fields[1].name=="slt","TB exact ordered static fields");
    s.selectedRequestedBlock=true;s.effectiveOrdinal=s.requestedOrdinal;s.effectiveBlock=selected->name;
    const auto* slt=detail::packedField(*selected,"slt",s.sltFieldOrdinal);
    const auto* shadow=detail::packedField(*selected,"allowshadowreceiving",s.shadowFieldOrdinal);
    need(slt&&shadow&&s.sltFieldOrdinal==1&&s.shadowFieldOrdinal==0&&shadow->value=="F","TB consumed static fields");
    s.slt=slt->value;s.modelName=s.slt.substr(0,s.slt.rfind('.'));
    need(!s.modelName.empty()&&s.slt.size()<=29,"TB actual model spelling");s.allowShadowReceiving148=0;
    need(s.effectiveBlock.capacity()<=32&&s.slt.capacity()<=32&&s.modelName.capacity()<=32,"TB static string reservation exceeded");
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
 const Source& source(std::size_t i)const{if(!ready())throw std::runtime_error("TB static source not ready");return source_.at(i);}
 const tb_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
};
}
