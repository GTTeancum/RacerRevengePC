#pragma once
#include "mc_cold_sources50la.h"
#include "ga_trigger_authoring_fixture.h"
#include <array>
#include <memory>
#include <optional>
#include <thread>

// Immutable original-authoring projections only. No source-frame sample,
// placement/attachment graph, collider, RNG state or runtime lease is issued.
namespace mc_scene_authoring50MC {
namespace a=rr::assets;namespace g=rr::game;
namespace detail {
using namespace ga_trigger_authoring::detail;
inline bool boolean(std::string s){for(auto& c:s)if(c>='a'&&c<='z')c=char(c-'a'+'A');return ga_trigger_authoring::detail::boolean(s);}
inline const a::ParameterDocument48F& document(const mc_cold_sources50LA::Owner& cold){
 need(cold.ready(),"MC actual cold owner required");const auto& p=cold.triggerParameters();
 need(!p.preservesDuplicateLabels&&p.original_bytes==cold.originalTob(),"MC held TOB document required");
 // Stock identity (1268 blocks, 186642 bytes, SHA) is a receipt, not a gate.
 course_archive_receipt50CI::memberReceipt("mc_scene_authoring","tracks\\mc.tob",p.original_bytes,0,0,186642,
   "7070700a884c24a98a0a89d4ea58e61e34f9a89e19af3ca239779c56c0dd14b0");
 course_archive_receipt50CI::countReceipt("mc_scene_authoring","tob_blocks",p.blocks.size(),1268);return p;
}
}
template<class Source,std::size_t Count> class OwnerBase {
protected:
 std::shared_ptr<const mc_cold_sources50LA::Owner> cold_;std::array<Source,Count> source_{};
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
 template<class Project> bool prepare(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,
   std::size_t available, std::string& error,Project project){
  if(thread_!=std::this_thread::get_id()){error="MC authoring thread mismatch";return false;}
  if(attempted_){error="MC authoring requires a fresh lifetime";return false;}attempted_=true;
  if(available>32u*1024u*1024u||available<maximumAdditionalPayloadBytes()){
   error="MC authoring reservation exceeds remaining allowance";return false;}
  try{detail::need(bool(cold),"MC cold source absent");const auto& p=detail::document(*cold);
   std::array<Source,Count> next{};project(p,next);source_=std::move(next);cold_=std::move(cold);
   ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
public:
 OwnerBase()=default;OwnerBase(const OwnerBase&)=delete;OwnerBase&operator=(const OwnerBase&)=delete;
 OwnerBase(OwnerBase&&)=delete;OwnerBase&operator=(OwnerBase&&)=delete;
 ~OwnerBase(){if(thread_!=std::this_thread::get_id())std::terminate();}
 // Fixed records borrow strings/blocks from separately charged immutable TOB.
 // Reserve staged array, authentication 7070700a884c24a98a0a89d4ea58e61e34f9a89e19af3ca239779c56c0dd14b0 copy/padding and bounded labels.
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept{
  return sizeof(OwnerBase)+sizeof(std::array<Source,Count>)+4u*(186642u+64u)+4096u;
 }
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(OwnerBase);}
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&cold_&&cold_->ready();}
 const Source& source(std::size_t i)const{if(!ready())throw std::runtime_error("MC authoring not ready");return source_.at(i);}
 const mc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
};
}
