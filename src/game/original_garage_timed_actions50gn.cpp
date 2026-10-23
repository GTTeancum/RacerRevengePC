#include "original_garage_timed_actions50gn.h"
#include <cfenv>
#include <cmath>
namespace rr::game {namespace {
float add(float a,float b)noexcept{volatile float c=a+b;return c;}
float sub(float a,float b)noexcept{volatile float c=a-b;return c;}
float mul(float a,float b)noexcept{volatile float c=a*b;return c;}
float cvtPositive(std::uint32_t word)noexcept {
 unsigned shift=0;for(auto m=word;m>=0x1000000u;m>>=1)++shift;
 return static_cast<float>((word>>shift)<<shift); //EE positive CVT.S.W.
}
using K=GarageTimedActionKind50GN;using A=GarageTimedAction50GN;
constexpr std::array<A,13> fire{{
 {K::trigger,5,0x1fb708},{K::reset88,1,0x1fb740},{K::enable20,1,0x1fb750},{K::start80,1,0x1fb760},
 {K::reset88,2,0x1fb798},{K::enable20,2,0x1fb7a8},{K::start80,2,0x1fb7b8},
 {K::reset88,3,0x1fb7f0},{K::enable20,3,0x1fb800},{K::start80,3,0x1fb810},
 {K::reset88,4,0x1fb848},{K::enable20,4,0x1fb858},{K::start80,4,0x1fb868}}};
constexpr std::array<A,6> ext{{
 {K::trigger,6,0x1fb8b8},{K::trigger,8,0x1fb8d4},{K::trigger,7,0x1fb8f0},
 {K::reset88,5,0x1fb928},{K::enable20,5,0x1fb938},{K::start80,5,0x1fb948}}};
constexpr std::array<A,4> fall{{
 {K::trigger,9,0x1fb9a4},{K::reset88,6,0x1fb9dc},{K::enable20,6,0x1fb9ec},{K::start80,6,0x1fb9fc}}};
}
bool OriginalGarageTimedActions50GN::activateFreshZeroCash(std::int32_t cash,CoupledNoiseContext49M& raw,GarageTimedReceipt50GN& r)noexcept {
 r={};r.sourcePC=0x1faaf0;
 if(activated_||failed_||inCall_||cash!=0||std::fegetround()!=FE_TONEAREST)return false;
 // Digit sums1FAE10 and1FAE90 both equal0, hence both source tests succeed.
 state_.fireEnabled34C8=state_.fallEnabled34CC=true;
 r.activationDraws[0]=drawOriginalSharedRng50IU(raw);
 state_.fire34B4=add(8.f,mul(5.f,mul(0x1p-31f,cvtPositive(r.activationDraws[0]))));
 state_.ext34B8=add(2.5f,state_.fire34B4);
 r.activationDraws[1]=drawOriginalSharedRng50IU(raw);
 state_.fall34BC=add(8.f,mul(5.f,mul(0x1p-31f,cvtPositive(r.activationDraws[1]))));
 activated_=true;r.sourcePC=0x1fad84;return true;
}
bool OriginalGarageTimedActions50GN::step(float dt,GarageTimedDispatch50GN dispatch,void* context,GarageTimedReceipt50GN& r)noexcept {
 r={};r.sourcePC=0x1fb6a0;
 if(!ready()||inCall_||!dispatch||!std::isfinite(dt)||dt<0||std::fpclassify(dt)==FP_SUBNORMAL||std::fegetround()!=FE_TONEAREST)return false;
 inCall_=true;struct Guard{bool& b;~Guard(){b=false;}}g{inCall_};
 auto branch=[&](float& timer,const auto& actions,std::uint32_t done)noexcept {
  if(timer<=0.f)return true;
  timer=sub(timer,dt);if(timer>0.f)return true;
  for(const auto& a:actions){r.sourcePC=a.sourcePC;if(a.kind==K::reset88)r.lastEmitterLookup=a.index;if(!dispatch(context,a)){failed_=true;return false;}++r.actions;}
  timer=0.f;++r.completedBranches;r.sourcePC=done;return true;
 };
 if(state_.fireEnabled34C8&&(!branch(state_.fire34B4,fire,0x1fb870)||!branch(state_.ext34B8,ext,0x1fb950)))return false;
 if(state_.fallEnabled34CC&&!branch(state_.fall34BC,fall,0x1fba04))return false;
 r.sourcePC=0x1fba08;return true;
}
}
