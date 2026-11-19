#pragma once
#include "gb_cold_sources50la.h"
#include "../../src/game/original_camera_start_effects.h"
#include "../../src/game/original_track_fog50cr.h"
#include <iostream>

namespace gb_track_fog50GB {
// The same source-bound mutable storage is borrowed by the actual camera
// Session and retained by the GB renderer. Observations cannot replace it.
// The fields come from the course SCB's own [Fog] block through the one common
// fog reader (src\game\original_track_fog50cr.h: 278570 reader, then the
// 273C64/13E830 load the PodRaceEvent applies), as for every course. Stock GB
// (31,28,55 / 0.25 / 1 at byte 28448) is a receipt, not an admission gate.
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
  rr::game::OriginalTrackFog50CR fog;rr::game::OriginalTrackFogDiagnostic50CR diagnostic;
  rr::game::OriginalRaceFogFields next{};
  if(!rr::game::readOriginalTrackFog50CR(source->world(),fog,diagnostic)||
     !rr::game::loadOriginalRaceFogFields50CR(fog,next,diagnostic)){
   error=std::string("GB course Fog rejected by the common fog reader: ")+
       rr::game::originalTrackFogStatusName50CR(diagnostic.status);return false;}
  // Stock receipt: GB's shipped Fog block (color 31,28,55, Visibility .25,
  // Haziness 1, block at byte 28448 of the 401867-byte SCB).
  const bool stock=fog.scbBytes==401867u&&fog.fogBlockOffset==28448u&&next.color30==0xff1f1c37u&&
      next.authoredFraction3C==.25f&&next.authoredFraction40==1.f;
  std::cerr<<"gb_course_owner owner=fog present=1 color=0x"<<std::hex<<(next.color30&0xffffffu)<<std::dec
      <<" visibility="<<next.authoredFraction3C<<" haziness="<<next.authoredFraction40<<" stock="<<(stock?1:0)<<'\n';
  fields_=next;source_=std::move(source);prepared_=true;error.clear();return true;
 }
 bool ready()const noexcept {
  return thread_==std::this_thread::get_id()&&prepared_&&source_&&source_->ready();
 }
 const gb_cold_sources50LA::Owner* sourceOwner()const noexcept{return ready()?source_.get():nullptr;}
 const rr::game::OriginalRaceFogFields& fields()const noexcept{return fields_;}
 // Root-only mutable binding for genuine restore/display/camera operations.
 // Renderer admission additionally requires Session's private current revision
 // and exact pointer match; the pointer alone is not a draw authorization.
 rr::game::OriginalRaceFogFields* cameraFields()noexcept{return ready()?&fields_:nullptr;}
};
}
