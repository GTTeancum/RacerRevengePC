// Common rewrite (R5, PLAN B-C13): 1A9EE0 sub-step 05.09, 274E90 trigger list,
// for every course. One reader (course_trigger_sources50CT) over the Track's
// own decoded TOB, then the one trigger group (rr::game::NativeTriggerGroup50MC)
// sized once from the live trigger count. The action owner
// (course_trigger_actions50CT::Owner) binds later, once the AI spline (step 8)
// and the race field are known; see work\scratch\common-rewrite\writer-notes\r5.txt.
#include "common_track50cr.h"
#include "course_trigger_sources50ct.h"
#include "../../src/game/native_trigger_group50mc.h"
#include <memory>
#include <string>

namespace common_track50CR {
bool readTriggers274E90(Track& track,const LoadInputs&,Diagnostic& d)noexcept {
 namespace L=common_race_log50CR;
 constexpr auto step=L::Step::triggers274E90;
 const auto refuse=[&](std::uint32_t pc,std::string why)noexcept{
  try{d.refuse(step,pc,std::move(why));}catch(...){d.step=step;d.pc=pc;d.refused=true;}
  L::load(step,false,d.value,d.reason,nullptr,pc);return false;
 };
 try{
  d.step=step;d.pc=0x274E90u;d.value=0;
  if(track.roles.triggerSource||track.roles.triggers)return refuse(0x274E90u,"trigger list already read for this track");
  auto source=std::make_shared<course_trigger_sources50CT::Owner>();
  course_trigger_sources50CT::Diagnostic sd;
  if(!source->load(track,sd))
   return refuse(sd.trigger?0x2A00A0u:0x274E90u,(sd.trigger?"trigger"+std::to_string(sd.trigger)+": ":std::string())+sd.message);
  const auto live=source->liveCount();
  auto group=std::make_shared<rr::game::NativeTriggerGroup50MC>();
  const auto& row=blockRow(BlockId::triggers);
  if(!rr::game::sizeNativeTriggerGroup50MC(*group,live,row.ceiling)){
   L::storageRefused("trigger_group",live,row.ceiling,0x2A00A0u);
   return refuse(0x2A00A0u,"trigger group could not be sized from the live trigger count");
  }
  L::storage("trigger_group",live,live*sizeof(rr::game::TriggerEntryState50IN),row.ceiling);
  track.counts.triggers=static_cast<std::uint32_t>(live);
  track.counts.triggerActions=source->actionTotal();
  track.roles.triggerSource=std::move(source);
  track.roles.triggers=std::move(group);
  d.value=live;
  L::load(step,true,live,"numberoftriggers");
  return true;
 }catch(const std::exception& e){return refuse(0x274E90u,e.what());}
 catch(...){return refuse(0x274E90u,"trigger list allocation failure");}
}
} // namespace common_track50CR
