#pragma once
#include "mc_collision_world50mc.h"
#include "race_session50hm_fixture.h"
#include "../../src/game/ai_runtime50iv.h"
#include <string>
namespace mc_typed_navigation_test50MC {
// Invoke only against the real prepared111-candidate world. This helper cannot
// issue a world lease, invent callbacks, or substitute a host graph.
inline bool check(const std::shared_ptr<const mc_cold_sources50LA::Owner>& source,
 mc_collision_world50MC::Owner& world,std::string& error) {
 const auto need=[&](bool v,const char* why){if(!v)error=why;return v;};
 if(!need(source&&source->ready()&&world.geometryReady()&&world.coldSource()==source.get(),"actual MC source/world required"))return false;
 mc_navigation50MC::Owner navigation;mc_navigation50MC::Diagnostic diagnostic;
 if(!need(navigation.prepare(source,diagnostic)&&navigation.bindWorld(world.lease(),diagnostic),"actual MC navigation world binding"))return false;
 bool okay=true;
 {
  auto map=navigation.map(),ai=navigation.ai();auto mapMeta=navigation.mapMetadata(),aiMeta=navigation.aiMetadata();
  const auto borrowed=world.borrow();
  okay=need(borrowed&&borrowed.world()&&map.runtimeValid()&&ai.runtimeValid()&&map.count()==173&&ai.count()==173&&
   map.nodes()!=ai.nodes()&&map.track().nodes()!=ai.track().nodes()&&map.sameWorld(*borrowed.world())&&ai.sameWorld(*borrowed.world())&&
   mapMeta.matches(map)&&aiMeta.matches(ai)&&!mapMeta.matches(ai)&&!aiMeta.matches(map),"MC exact distinct full typed graphs and metadata");
  rr::game::LapProgressTrackView50MC laps;rr::game::LapProgressDiagnostic50GF lapDiagnostic;
  if(okay)okay=need(rr::game::prepareLapProgressTrack50GF(map,mapMeta,laps,lapDiagnostic)&&laps.matches(map)&&!laps.matches(ai),"MC actual lap markers");
  rr::game::LapProgressTrackView50MC rejected;
  if(okay)okay=need(!rr::game::prepareLapProgressTrack50GF(ai,aiMeta,rejected,lapDiagnostic),"AI role must not enter Map lap preparation");
  rr::game::AiRuntimeInput50MC input{};input.mapFF0=map;input.aiFF4=ai;input.orientationTrackBC=ai;input.aiMetadata=aiMeta;input.actualWorld=borrowed.world();
  if(okay)okay=need(rr::game::driveOriginalAiAdmission50MB(input),"MC real typed AI admission");
  input.mapFF0=ai;if(okay)okay=need(!rr::game::driveOriginalAiAdmission50MB(input),"swapped Map role rejected");input.mapFF0=map;
  input.aiMetadata=mapMeta;if(okay)okay=need(!rr::game::driveOriginalAiAdmission50MB(input),"swapped AI metadata rejected");
  race_session50HM::McSession race;
  if(okay)okay=need(race_session50HM::initialize(race,map,laps,8)&&race.runtimeValid()&&race.nodeCount==173&&race.racerCount==8,"MC full173 eight-peer race session");
  if(okay)okay=need(!navigation.close(),"retained typed consumers fence navigation close");
 }
 if(!need(navigation.close(),"typed consumers release genuine navigation lease"))return false;
 return okay;
}
} // namespace mc_typed_navigation_test50MC
