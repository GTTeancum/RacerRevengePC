#include "original_ui_tournament_clean_lifecycle50kg.h"
#include <cmath>
namespace rr::game {
bool planOriginalUiTournamentCleanPhase50KG(OriginalUiTournamentCleanPhase50KG phase,
 bool dirty,bool camera,OriginalUiTournamentCleanPlan50KG& out) noexcept {
 using P=OriginalUiTournamentCleanPhase50KG;using E=OriginalUiTournamentCleanEffect50KG;
 OriginalUiTournamentCleanPlan50KG p;
 const auto add=[&](E kind,std::uint32_t argument,std::uint32_t value,std::uint32_t pc){
  p.steps[p.count++]={kind,argument,value,pc};
 };
 switch(phase){
 case P::entry74:
  if(dirty)return false; // Native unsupported branch, no effects published.
  add(E::promptMask,0,0,0x1EE9EC);
  add(E::groupEnabled,2,0,0x1EE9FC);
  add(E::groupEnabled,1,0,0x1EEA0C);
  add(E::widgetEnabled,27,0,0x1EEA20);
  add(E::widgetEnabled,28,0,0x1EEA34);
  add(E::widgetEnabled,29,0,0x1EEA48);
  add(E::hidePopup438,0,0,0x1EEAC8);
  add(E::resetExit4060,0,0,0x1EEAD0);
  add(E::resetTimer4064,0,0,0x1EEAD4);
  if(camera){add(E::showCamera4BC,0,0,0x1EEAEC);add(E::activateCamera4BC,0,0,0x1EEB10);}
  add(E::resetSharedPopupResult3D8,0,0,0x1EEB2C);
  add(E::applyBackground430,0,0,0x1EEB38);
  break;
 case P::openingAC:
  add(E::groupEnabled,2,1,0x1EEB68);
  add(E::startSong11,11,1,0x1EEB80);break;
 case P::openedB0:
  add(E::groupEnabled,1,1,0x1EEBB8);
  add(E::promptMask,0,7,0x1EEBC4);break;
 case P::closingB4:
  add(E::promptMask,0,0,0x1EEBEC);
  add(E::groupEnabled,1,0,0x1EEBFC);
  add(E::fadeMusic1,0,0x3f800000,0x1EEC14);break;
 case P::closedB8:break; // Source1EEC30 returns without effects.
 default:return false;
 }
 out=p;return true;
}
bool updateOriginalUiTournamentTimer50KG(float timer,float delta,float& out) noexcept {
 if(!std::isfinite(timer)||!std::isfinite(delta)||delta<0)return false;
 const float next=timer>0?timer-delta:timer;
 if(!std::isfinite(next))return false;
 out=next;return true;
}
}
