#pragma once
#include "gb_cold_sources50la.h"
#include "../../src/game/original_camera_start_effects.h"

namespace gb_track_fog50GB {
// The same source-bound mutable storage is borrowed by the actual camera
// Session and retained by the GB renderer. Observations cannot replace it.
class Owner final {
 std::shared_ptr<const gb_cold_sources50LA::Owner> source_;
 rr::game::OriginalRaceFogFields fields_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,prepared_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool prepare(std::shared_ptr<const gb_cold_sources50LA::Owner> source,std::string& error) {
  if(thread_!=std::this_thread::get_id()||attempted_){error="GB fog requires a fresh same-thread owner";return false;}attempted_=true;
  if(!source||!source->ready()){error="GB fog requires its actual ready source";return false;}
  const auto& doc=source->world().parameters;const rr::assets::ParameterBlock48F* fog=nullptr;
  for(const auto& b:doc.blocks)if(b.name=="fog"){if(fog){error="GB duplicate Fog";return false;}fog=&b;}
  if(doc.original_bytes.size()!=401867u||!fog||fog->offset!=28448u||fog->fields.size()!=3){error="GB exact Fog source shape differs";return false;}
  const auto& color=fog->fields[0];const auto& haze=fog->fields[1];const auto& visibility=fog->fields[2];
  if(color.name!="colorrgb"||color.value!="31,28,55"||color.value_offset!=28581u||
     haze.name!="haziness"||haze.value!="1"||haze.value_offset!=28595u||
     visibility.name!="visibility"||visibility.value!="0.25"||visibility.value_offset!=28590u){error="GB exact authored Fog fields differ";return false;}
  rr::game::OriginalRaceFogFields next{};next.color30=0xff1f1c37u;next.authoredFraction3C=.25f;next.authoredFraction40=1.f;
  std::uint32_t rendererColor=0;
  if(rr::game::restoreOriginalRaceFog1A98DC(next,rendererColor).status!=rr::game::OriginalCameraStartStatus::complete||rendererColor!=next.color30){error="GB source gameplay Fog range arithmetic rejected";return false;}
  fields_=next;source_=std::move(source);prepared_=true;error.clear();return true;
 }
 bool ready()const noexcept {
  return thread_==std::this_thread::get_id()&&prepared_&&source_&&source_->ready()&&
   fields_.color30==0xff1f1c37u&&fields_.authoredFraction3C==.25f&&fields_.authoredFraction40==1.f;
 }
 const gb_cold_sources50LA::Owner* sourceOwner()const noexcept{return ready()?source_.get():nullptr;}
 const rr::game::OriginalRaceFogFields& fields()const noexcept{return fields_;}
 // Root-only mutable binding for genuine restore/display/camera operations.
 // Renderer admission additionally requires Session's private current revision
 // and exact pointer match; the pointer alone is not a draw authorization.
 rr::game::OriginalRaceFogFields* cameraFields()noexcept{return ready()?&fields_:nullptr;}
};
}
