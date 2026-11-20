// Common rewrite (R5, PLAN B-C13 / 2.2): 1A9EE0 sub-step 05.14 for every course:
//   101860  the one track (scenery) animation player over the TOB's [Animations]
//           (absent block or NumberOfAnimations 0 = no owner, 2713E0);
//   29B1D0  the one texture-scroll owner over the static world's authored pass
//           fields (VertexAnimation=unique + uvs_animate, 29A310);
//   ATM     the one animated-texture reader over the static world's .ATM passes
//           (absent = no owner).
// All three read the Track's own documents, archive and static world (05.07,
// R3); nothing here names a course. GPU creation of the ATM frames and the UV
// owner's per-frame advance stay with their callers (I3 publication / I4 loop).
#include "common_track50cr.h"
#include "track_animation_course50an.h"
#include "track_uv_animation50uv_fixture.h"
#include "track_atm50cr.h"
#include "single_pass_world50lm.h"          // R3 NEW: single_pass_world50LM::Owner (roles.staticWorld)
#include "../../src/game/vehicle_animation49t.h"
#include <memory>
#include <string>

namespace common_track50CR {
bool prepareAnimation101860(Track& track,const LoadInputs& inputs,Diagnostic& d)noexcept {
 namespace L=common_race_log50CR;
 constexpr auto step=L::Step::animation101860;
 const auto refuse=[&](std::uint32_t pc,std::string why)noexcept{
  try{d.refuse(step,pc,std::move(why));}catch(...){d.step=step;d.pc=pc;d.refused=true;}
  L::load(step,false,d.value,d.reason,nullptr,pc);return false;
 };
 try{
  d.step=step;d.pc=0x101860u;d.value=0;
  if(track.roles.animation||track.roles.uvAnimation||track.roles.atm)return refuse(0x101860u,"track animation already prepared");
  if(!track.roles.staticWorld||!track.roles.staticWorld->loaded())return refuse(0x101860u,"static world (05.07) not loaded");
  const auto& world=*track.roles.staticWorld;

  // 101860: the track's [Animations] table. The motion/libm state is the
  // session's shared animation state (the same one the racers' animation uses).
  // W0 request r5 #1: LoadInputs::animationShared.
  if(!inputs.animationShared)return refuse(0x101860u,"no shared animation state (LoadInputs::animationShared)");
  {
   const Archive& archive=track.archive;
   track_animation_source50AN::Extract extract=[&archive](const std::string& name,std::vector<std::uint8_t>& bytes,std::size_t limit){
    Diagnostic md;return archive.member(name,bytes,limit,md);};
   auto loaded=track_animation_course50AN::load(track.documents.tobDocument,std::string(track.code()),track.counts.zones,
       std::move(extract),inputs.animationShared->libm,inputs.animationShared->motion);
   track.counts.animations=loaded.runtime?static_cast<std::uint32_t>(loaded.runtime->rowCount()):0u;
   track.roles.animation=std::move(loaded.runtime);   // null when the TOB authors no animations (data rule)
  }

  // 29B1D0: texture scroll over the static world's spans and selections.
  {
   const auto* spans=world.spans();const auto* selections=world.selections();
   if(!spans||!selections)return refuse(0x29B1D0u,"static world spans/selections missing for UV animation");
   auto uv=std::make_shared<track_uv_animation50uv_fixture::Owner>();
   track_uv_animation50uv_fixture::Diagnostic ud;
   if(!uv->load(*spans,*selections,ud))
    return refuse(0x29B1D0u,ud.message.empty()?std::string("track UV animation owner rejected"):ud.message);
   track.roles.uvAnimation=std::move(uv);

   // ATM: only when the static world authors an ATM pass.
   if(track_atm50CR::Owner::authored(*selections)){
    auto atm=std::make_shared<track_atm50CR::Owner>();
    track_atm50CR::Diagnostic ad;
    if(!atm->loadSource(track,world,ad))
     return refuse(0x29A020u,"ATM "+ad.member+": "+ad.message);
    track.counts.atmRows=atm->counts().animations;
    track.roles.atm=std::move(atm);
   }else track.counts.atmRows=0;
  }
  d.value=track.counts.animations;
  L::load(step,true,track.counts.animations,"animations="+std::to_string(track.counts.animations)+
      " atm_rows="+std::to_string(track.counts.atmRows));
  return true;
 }catch(const std::exception& e){return refuse(0x101860u,e.what());}
 catch(...){return refuse(0x101860u,"track animation allocation failure");}
}
} // namespace common_track50CR
