#pragma once
#include "mc_cold_sources50la.h"
#include "../../src/game/original_camera_start_effects.h"

namespace mc_track_fog50MC {
// The same source-bound mutable storage is borrowed by the actual camera
// Session and retained by the MC renderer. Observations cannot replace it.
class Owner final {
 std::shared_ptr<const mc_cold_sources50LA::Owner> source_;
 rr::game::OriginalRaceFogFields fields_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,prepared_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool prepare(std::shared_ptr<const mc_cold_sources50LA::Owner> source,std::string& error) {
  if(thread_!=std::this_thread::get_id()||attempted_){error="MC fog requires a fresh same-thread owner";return false;}attempted_=true;
  if(!source||!source->ready()){error="MC fog requires its actual ready source";return false;}
  const auto& doc=source->world().parameters;const rr::assets::ParameterBlock48F* fog=nullptr;
  for(const auto& b:doc.blocks)if(b.name=="fog"){if(fog){error="MC duplicate Fog";return false;}fog=&b;}
  if(doc.original_bytes.size()!=187711u||!fog||fog->offset!=15148u||fog->fields.size()!=1){error="MC exact Fog source shape differs";return false;}
  const auto& color=fog->fields[0];
  if(color.name!="colorrgb"||color.value!="100,73,81"||color.value_offset!=15195u||
     pod_materials50ed_fixture::detail::sha256(doc.original_bytes)!=
     "26f5d637533ee9840e916e02d5cef7b06d68352f1b7e0b11a48bf050d73df51e"){
   error="MC exact authored Fog fields differ";return false;}
  // Original278430/278570: absent Visibility defaults to1; Haziness to0.5.
  // Getter158B80 missing-key writes preserved f12 at158BDC. No GB fog alias.
  rr::game::OriginalRaceFogFields next{};next.color30=0xff644951u;
  next.authoredFraction3C=1.f;next.authoredFraction40=.5f;
  std::uint32_t rendererColor=0;
  if(rr::game::restoreOriginalRaceFog1A98DC(next,rendererColor).status!=rr::game::OriginalCameraStartStatus::complete||rendererColor!=next.color30){error="MC source gameplay Fog range arithmetic rejected";return false;}
  fields_=next;source_=std::move(source);prepared_=true;error.clear();return true;
 }
 bool ready()const noexcept {
  return thread_==std::this_thread::get_id()&&prepared_&&source_&&source_->ready()&&
   fields_.color30==0xff644951u&&fields_.authoredFraction3C==1.f&&fields_.authoredFraction40==.5f;
 }
 const mc_cold_sources50LA::Owner* sourceOwner()const noexcept{return ready()?source_.get():nullptr;}
 const rr::game::OriginalRaceFogFields& fields()const noexcept{return fields_;}
 // Root-only mutable binding for genuine restore/display/camera operations.
 // Renderer admission additionally requires Session's private current revision
 // and exact pointer match; the pointer alone is not a draw authorization.
 rr::game::OriginalRaceFogFields* cameraFields()noexcept{return ready()?&fields_:nullptr;}
};
}
