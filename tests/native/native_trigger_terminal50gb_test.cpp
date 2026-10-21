#include "../../src/game/native_trigger_retirement50gb.h"
#include <cassert>
#include <cstring>
#include <iostream>
namespace g=rr::game;
static g::NativeTriggerGroup50GB graph(){
 g::NativeTriggerGroup50GB g{};
 for(std::size_t t=0;t<18;++t){g.triggers[t].collider.identity=0x40000000+t;g.triggers[t].collider.highWaterD0=16;
  for(std::size_t c=0;c<16;++c)g.triggers[t].collider.membersCC[c]=100+c;}
 for(std::size_t c=0;c<16;++c){g.colliders[c].identity=100+c;g.colliders[c].highWaterD0=18;
  for(std::size_t t=0;t<18;++t)g.colliders[c].membersCC[t]=0x40000000+t;}
 return g;
}
struct Actions {unsigned completed=0;bool fail=false;};
static bool preflight(void*,const g::SourceTriggerActionCall50GB&)noexcept{return true;}
static g::SourceTriggerActionResult50GB action(void* p,const g::SourceTriggerActionCall50GB& c)noexcept {
 auto& a=*static_cast<Actions*>(p);
 if(a.fail&&c.source.actionIndex==1)return {g::SourceTriggerActionStatus50GB::Blocked,0x1b64b0,0x128};
 ++a.completed;return {g::SourceTriggerActionStatus50GB::Complete,c.source.sourceSite,0};
}
int main(){
 auto full=graph();full.ready=false;full.actionsFailed=true;auto before=full;
 auto r=g::validateNativeTriggerTerminalGraph50GB(full);assert(r.valid&&r.pairs==288);assert(!std::memcmp(&full,&before,sizeof full));
 {auto v=full;v.triggers[4].collider.membersCC[31]=100;assert(!g::validateNativeTriggerTerminalGraph50GB(v).valid);}
 {auto v=full;v.colliders[2].membersCC[31]=0x40000000;assert(!g::validateNativeTriggerTerminalGraph50GB(v).valid);}
 {auto v=full;v.triggers[3].collider.membersCC[0]=101;assert(!g::validateNativeTriggerTerminalGraph50GB(v).valid);}
 {auto v=full;v.colliders[3].membersCC[0]=0;assert(!g::validateNativeTriggerTerminalGraph50GB(v).valid);}
 {auto v=full;v.colliders[3].identity=v.colliders[4].identity;assert(!g::validateNativeTriggerTerminalGraph50GB(v).valid);}
 {auto v=full;v.triggers[1].collider.highWaterD0=33;assert(!g::validateNativeTriggerTerminalGraph50GB(v).valid);}
 {auto v=full;v.triggers[0].collider.membersCC[0]=v.triggers[0].collider.identity;assert(!g::validateNativeTriggerTerminalGraph50GB(v).valid);}
 {auto v=full;v.triggers[0].collider.membersCC[0]=999;assert(!g::validateNativeTriggerTerminalGraph50GB(v).valid);}
 {auto v=full;for(auto& t:v.triggers)t.collider.membersCC.fill(0);for(auto& c:v.colliders)c.membersCC.fill(0);
  assert(g::validateNativeTriggerTerminalGraph50GB(v).valid);assert(g::validateNativeTriggerTerminalGraph50GB(v).pairs==0);}
 {
  auto v=full;for(auto& t:v.triggers){t.collider.membersCC.fill(0);t.collider.highWaterD0=0;t.flags29=3;t.flags1B8=3;t.randomTriggerPercent90=100;}
  for(std::size_t i=0;i<18;++i)v.triggers[i].callbackCount98=i==15?2:i==17?5:1;
  for(auto& c:v.colliders){c.membersCC.fill(0);c.highWaterD0=0;}
  Actions target{};v.actions={&target,&preflight,&action};v.ready=true;v.actionsFailed=false;v.queryActive=true;v.querySerial=1;v.queryCollider=100;
  assert(g::validNativeTriggerGroup50GB(v));
  const auto completed=g::dispatchNativeTriggerContact50GB(v,0,100,4);assert(completed.status==g::SourceTriggerContactStatus50GB::Complete);
  assert(target.completed==1&&v.contact.phase==g::SourceTriggerContactPhase50GB::Complete);
  target.fail=true;const auto failed=g::dispatchNativeTriggerContact50GB(v,17,100,7);
  assert(failed.status==g::SourceTriggerContactStatus50GB::ActionBlocked&&target.completed==2);
  assert(v.actionsFailed&&v.contact.phase==g::SourceTriggerContactPhase50GB::Actions&&v.contact.nextAction==1&&!v.contact.dispatchCounted);
  assert(v.triggers[17].collider.highWaterD0==0&&v.colliders[0].highWaterD0==1);
  const auto retained=v;assert(g::validateNativeTriggerTerminalGraph50GB(v).valid&&g::validateNativeTriggerTerminalGraph50GB(v).pairs==1);
  assert(!g::releaseCompletedSourceContact50GB(v.contact));assert(!std::memcmp(&retained,&v,sizeof v));
 }
 std::cout<<"GB terminal graph: full 288 reciprocal pairs, failed-ready state, poisoned tails, duplicates, missing reciprocal, alias, bounds, self/unknown, empty graph and native failed-action prefix passed\n";
}
