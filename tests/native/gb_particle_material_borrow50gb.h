#pragma once
#include "gb_cold_sources50la.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/assets/original_standard_texture_resource50la.h"
#include <cstdint>
#include <memory>
#include <thread>

namespace gb_particle_visual50GB {
class Window;
// Observation of one actual Window-owned synchronous material/render borrow.
// Only Window can issue or invalidate it; no caller dimensions or ready flag.
class MaterialBorrow final {
 friend class Window;
 struct State {
  const Window* issuer=nullptr;
  std::thread::id thread=std::this_thread::get_id();
  std::uint64_t generation=0;
  bool active=false;
  std::shared_ptr<const gb_cold_sources50LA::Owner> source;
  std::shared_ptr<const rr::game::OriginalTrackParticleCold50LA> particles;
  std::shared_ptr<const rr::assets::OriginalStandardMaterialTable50LA> table;
  const rr::assets::OriginalStandardTextureResource50LA* texture=nullptr;
  std::int32_t row=-1;
  std::uint32_t width=0,height=0;
 };
 std::shared_ptr<const State> state_;
 const Window* issuer_=nullptr;
 std::uint64_t generation_=0;
 MaterialBorrow(std::shared_ptr<const State> state,const Window& issuer,std::uint64_t generation)noexcept
  :state_(std::move(state)),issuer_(&issuer),generation_(generation){}
 bool current()const noexcept {
  if(!state_||!issuer_||!generation_||state_->issuer!=issuer_||state_->generation!=generation_||
     !state_->active||state_->thread!=std::this_thread::get_id()||!state_->source||!state_->source->ready()||
     !state_->particles||!state_->particles->prepared()||!state_->table||!state_->texture||
     !state_->width||!state_->height)return false;
  const auto* reference=state_->particles->material150();
  return reference&&reference->retained()&&reference->table382B38()==state_->table.get()&&
      reference->material00()==state_->row;
 }
public:
 MaterialBorrow(const MaterialBorrow&)=delete;MaterialBorrow& operator=(const MaterialBorrow&)=delete;
 MaterialBorrow(MaterialBorrow&&)=delete;MaterialBorrow& operator=(MaterialBorrow&&)=delete;
 const rr::assets::OriginalStandardMaterialTable50LA* table382B38()const noexcept{return current()?state_->table.get():nullptr;}
 std::int32_t materialRow()const noexcept{return current()?state_->row:-1;}
 const rr::assets::OriginalStandardTextureResource50LA* texture()const noexcept{return current()?state_->texture:nullptr;}
 std::uint32_t width()const noexcept{return current()?state_->width:0;}
 std::uint32_t height()const noexcept{return current()?state_->height:0;}
 const gb_cold_sources50LA::Owner* coldSource()const noexcept{return current()?state_->source.get():nullptr;}
 const rr::game::OriginalTrackParticleCold50LA* trackParticles()const noexcept{return current()?state_->particles.get():nullptr;}
};
} // namespace gb_particle_visual50GB
