#include "../../src/game/original_garage_timed_actions50gn.h"
#include <array>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>
using namespace rr::game;
unsigned checks=0;void need(bool b,const char*s){++checks;if(!b){std::cerr<<"FAIL "<<s<<'\n';std::exit(1);}}
struct Actions{std::vector<GarageTimedAction50GN> rows;int failAt=-1;static bool dispatch(void* p,const GarageTimedAction50GN&a)noexcept {auto&self=*static_cast<Actions*>(p);if(int(self.rows.size())==self.failAt)return false;self.rows.push_back(a);return true;}};
int main(){
 CoupledNoiseContext49M raw;initializeOriginalSharedNoise50IU(raw);GarageTimedReceipt50GN r;OriginalGarageTimedActions50GN t;
 const auto initial=raw.sharedRandomState;need(!t.activateFreshZeroCash(1,raw,r)&&raw.sharedRandomState==initial,"nonfresh cash no activation or RNG");
 const auto first=initial*0x41c64e6du+0x3039u,second=first*0x41c64e6du+0x3039u;
 need(t.activateFreshZeroCash(0,raw,r)&&raw.sharedRandomState==second,"exact two borrowed raw RNG draws");need(r.activationDraws==std::array<std::uint32_t,2>{{first&0x7fffffff,second&0x7fffffff}},"activation draw receipts exact");
 need(t.state().fireEnabled34C8&&t.state().fallEnabled34CC&&t.state().fire34B4>=8&&t.state().fire34B4<13&&t.state().ext34B8==t.state().fire34B4+2.5f&&t.state().fall34BC>=8&&t.state().fall34BC<13,"zero digit sums enable BOTH real timer branches");
 need(!t.activateFreshZeroCash(0,raw,r)&&raw.sharedRandomState==second,"cannot reseed or reactivate fresh-only lifetime");Actions actions;
 const auto before=t.state();need(t.step(0,Actions::dispatch,&actions,r)&&actions.rows.empty()&&t.state().fire34B4==before.fire34B4,"zero page delta no trigger");
 need(!t.step(-1,Actions::dispatch,&actions,r)&&t.ready()&&t.state().fire34B4==before.fire34B4,"invalid delta leaves timer intact");
 need(t.step(20,Actions::dispatch,&actions,r)&&r.actions==23&&r.completedBranches==3&&r.lastEmitterLookup==6&&actions.rows.size()==23,"all crossed branches execute exact23 actions");
 const std::array<unsigned,23> indices{{5,1,1,1,2,2,2,3,3,3,4,4,4,6,8,7,5,5,5,9,6,6,6}};
 const std::array<unsigned,5> triggerPositions{{0,13,14,15,19}};
 unsigned phase=0;
 for(unsigned i=0;i<23;++i){need(actions.rows[i].index==indices[i],"exact authored ordinal order");bool trigger=false;for(auto p:triggerPositions)trigger|=i==p;
  if(trigger){need(actions.rows[i].kind==GarageTimedActionKind50GN::trigger,"genuine RUN/FALL order");phase=0;}
  else{const auto expected=std::array{GarageTimedActionKind50GN::reset88,GarageTimedActionKind50GN::enable20,GarageTimedActionKind50GN::start80}[phase++%3];need(actions.rows[i].kind==expected,"reset then enable then start");}}
 need(t.state().fire34B4==0&&t.state().ext34B8==0&&t.state().fall34BC==0&&raw.sharedRandomState==second,"completed timers zero; actions do not own/copy RNG");
 need(t.step(20,Actions::dispatch,&actions,r)&&r.actions==0&&r.lastEmitterLookup==-1&&actions.rows.size()==23,"zero timer no repeated callback");
 {OriginalGarageTimedActions50GN exact;need(exact.activateFreshZeroCash(0,raw,r),"second independent numeric fixture");Actions a;const auto delta=exact.state().fire34B4;need(exact.step(delta,Actions::dispatch,&a,r),"exact fire crossing");need(!a.rows.empty()&&a.rows.front().index==5&&exact.state().fire34B4==0,"new timer <=0 triggers at exact boundary");}
 {OriginalGarageTimedActions50GN failed;need(failed.activateFreshZeroCash(0,raw,r),"failure fixture");Actions a;a.failAt=2;need(!failed.step(20,Actions::dispatch,&a,r)&&a.rows.size()==2&&r.actions==2&&!failed.ready(),"failed callback preserves completed prefix and latches");const auto size=a.rows.size();need(!failed.step(20,Actions::dispatch,&a,r)&&a.rows.size()==size,"failure cannot replay actions");}
 std::cout<<"PASS "<<checks<<" Garage timed-action numeric checks; callbacks only record, no page/emitter admission\n";
}
