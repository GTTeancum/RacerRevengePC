#pragma once
// Common rewrite (R3, B-C11, PLAN 2.2 "instances, standard models"): the ONE 271A00
// [StaticModelParams] reader, for every course. Replaces the per-course readers
// ({mc,gb,tb,rc,tc}_static_model_params50xx.h, {sc,mb,ma}_static_model_params_fixture.h,
// gc_static_model_params50kg_fixture.h) and their pinned row counts, TOB SHAs and target
// names (receipts only now: stock_course_receipts50cr.h).
//
// Original 271A00 (RA's native chain, original_race_world_tob_prefix50la.cpp:1197):
//   [StaticModelParams] NumberOfStaticModelParams = N, then for i = 1..N:
//   271A74 157D00 looks up section "Model<i>"; a FAILED lookup is ignored and the previously
//   selected section stays selected (so a missing or misspelled section repeats the last
//   one; the first falls back to [StaticModelParams] itself) - TC's model4 / model17..19.
//   Fields are read with the packed 1583AC midpoint search (duplicate labels resolve the way
//   that search lands - TC's duplicate model3). "Slt" names the model (stem before the LAST
//   dot, 271AA0 -> 1169D0); "AllowShadowReceiving" F/T (158920) writes the standard model
//   descriptor's +148 (absent field: the descriptor keeps its constructor value 1).
//   271B4C then stores the value into every standard (type 1) model whose name matches.
//
// Rows are sized once from the authored count (ceiling below is an allocation guard);
// nothing names a course; nothing writes guest memory (ModelLease points at host-owned
// descriptor fields the caller owns, bounds-checked by count).
#include "common_track50cr.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace track_static_model_params50CR {
namespace a=rr::assets;namespace ct=common_track50CR;
// PORT STAND-IN: allocation guard against a corrupt TOB (stock max 1162, MC); not a gate.
inline constexpr std::size_t rowCeiling=16384;
struct Source {
 std::uint32_t requestedOrdinal=0,effectiveOrdinal=0;   // effective = the section the original actually read
 std::size_t sltFieldOrdinal=0,shadowFieldOrdinal=0;
 bool selectedRequestedBlock=false,shadowAuthored=false;
 std::string effectiveBlock,slt,modelName;
 std::uint32_t allowShadowReceiving148=1;
};
struct ModelLease {const std::string* sourceModelName=nullptr;std::uint32_t* allowShadowReceiving148=nullptr;std::uint32_t type=1;};
struct Receipt {std::vector<std::size_t> matched;std::size_t writes=0,unmatched=0;};
namespace detail {
inline const a::ParameterBlock48F* findBlock(const a::ParameterDocument48F& p,const std::string& name)noexcept {
 for(const auto& b:p.blocks)if(b.name==name)return &b;return nullptr;
}
// The packed 1583AC midpoint search with its final endpoint probes (duplicate labels
// resolve where this search lands; not a generic duplicate policy).
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
inline bool count(const std::string& text,std::size_t& out)noexcept {
 if(text.empty()||text.size()>9)return false;std::size_t v=0;
 for(char c:text){if(c<'0'||c>'9')return false;v=v*10+static_cast<std::size_t>(c-'0');}
 out=v;return true;
}
}
class Owner final {
 const ct::Track* track_=nullptr;
 std::vector<Source> rows_;bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 // Reads the Track's TOB (documents.tobDocument, decoded at 05.02). No [StaticModelParams]
 // block = no rows (nothing is stored).
 bool load(const ct::Track& track,std::string& error)noexcept {
  if(attempted_){error="static model parameters require a fresh lifetime";return false;}attempted_=true;
  try{
   const auto& p=track.documents.tobDocument;
   if(p.original_bytes.empty()||p.original_bytes!=track.documents.tob){error="static model parameters need the decoded TOB";return false;}
   const auto* selected=detail::findBlock(p,"staticmodelparams");
   std::vector<Source> next;
   if(selected){
    std::size_t ordinal=0,n=0;
    const auto* number=detail::packedField(*selected,"numberofstaticmodelparams",ordinal);
    if(!number||!detail::count(number->value,n)){error="[StaticModelParams] NumberOfStaticModelParams is not a count";return false;}
    if(n>rowCeiling){ct::log::storageRefused("static_model_params",n,rowCeiling,0x271A4Cu);error="static model parameter rows above the allocation guard";return false;}
    next.resize(n);std::uint32_t selectedOrdinal=0;
    for(std::size_t i=0;i<n;++i){auto& s=next[i];s.requestedOrdinal=static_cast<std::uint32_t>(i+1);
     const auto* requested=detail::findBlock(p,"model"+std::to_string(i+1));
     // 271A74 ignores a failed 157D00: the previously selected section stays selected.
     if(requested){selected=requested;selectedOrdinal=s.requestedOrdinal;s.selectedRequestedBlock=true;}
     s.effectiveOrdinal=selectedOrdinal;s.effectiveBlock=selected->name;
     const auto* slt=detail::packedField(*selected,"slt",s.sltFieldOrdinal);
     if(!slt||slt->value.empty()){error="static model row "+std::to_string(i+1)+" has no Slt";return false;}
     s.slt=slt->value;
     const auto dot=s.slt.rfind('.');s.modelName=dot==std::string::npos?s.slt:s.slt.substr(0,dot); // LAST dot (1169D0)
     if(s.modelName.empty()){error="static model row "+std::to_string(i+1)+" has an empty model stem";return false;}
     const auto* shadow=detail::packedField(*selected,"allowshadowreceiving",s.shadowFieldOrdinal);
     if(shadow){s.shadowAuthored=true;
      const auto& v=shadow->value;
      if(v=="F"||v=="f")s.allowShadowReceiving148=0;
      else if(v=="T"||v=="t")s.allowShadowReceiving148=1;
      else{error="static model row "+std::to_string(i+1)+" AllowShadowReceiving is not T/F";return false;}
     }
    }
   }
   ct::log::storage("static_model_params",next.size(),next.size()*sizeof(Source),rowCeiling);
   rows_=std::move(next);track_=&track;ready_=true;error.clear();return true;
  }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
   catch(...){try{error="static model parameter allocation failed";}catch(...){}return false;}
 }
 // 271B4C: every type-1 standard model whose name matches a row receives that row's value
 // (later rows overwrite earlier ones, as the original loop order does).
 bool apply(const ModelLease* models,std::size_t modelCount,Receipt& receipt)const noexcept {
  if(!ready_||(modelCount&&!models)||modelCount>100000)return false;
  for(std::size_t i=0;i<modelCount;++i)if(models[i].type==1&&(!models[i].sourceModelName||!models[i].allowShadowReceiving148))return false;
  try{
   Receipt next{};next.matched.assign(rows_.size(),0);
   for(std::size_t s=0;s<rows_.size();++s){
    for(std::size_t i=0;i<modelCount;++i)if(models[i].type==1&&detail::equal(*models[i].sourceModelName,rows_[s].modelName)){
     *models[i].allowShadowReceiving148=rows_[s].allowShadowReceiving148;++next.matched[s];++next.writes;
    }
    if(!next.matched[s])++next.unmatched;
   }
   receipt=std::move(next);return true;
  }catch(...){return false;}
 }
 bool ready()const noexcept{return ready_;}
 std::size_t count()const noexcept{return ready_?rows_.size():0u;}
 const Source& source(std::size_t i)const{if(!ready_||i>=rows_.size())throw std::out_of_range("static model row outside the authored rows");return rows_[i];}
};
} // namespace track_static_model_params50CR
