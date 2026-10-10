#include "secondary_body49m.h"
#include "noise_state_equal49w.h"
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
using namespace rr::game;
static unsigned checks=0;static unsigned allocations=0;static bool countAllocations=false;
void* operator new(std::size_t n){if(countAllocations)++allocations;if(void*p=std::malloc(n))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void*p) noexcept {std::free(p);}void operator delete[](void*p) noexcept {std::free(p);}
void operator delete(void*p,std::size_t) noexcept {std::free(p);}void operator delete[](void*p,std::size_t) noexcept {std::free(p);}
static void check(bool ok,const char* what){++checks;if(!ok){std::fprintf(stderr,"check failed: %s\n",what);std::exit(1);}}
static std::uint32_t bits(float x){std::uint32_t u;std::memcpy(&u,&x,4);return u;}
template<class T> static bool same(const T&a,const T&b){return std::memcmp(&a,&b,sizeof(T))==0;}
struct Fixture{
 SecondaryBodyState49M s{};SecondaryBodyParameters49M p{};SecondaryForceContext49M c{};GradientNoiseState49M noise{};std::uint32_t rng=12345;
 Fixture(){
  s.root.scene.v[0]=s.root.scene.v[5]=s.root.scene.v[10]=s.root.scene.v[15]=1;
  s.root.scene.v[12]=3;s.root.body.orientation.v[0]=1;
  s.root.body.velocity={{2,-1,3}};s.root.loads.force={{1,3,5}};s.root.loads.torque={{1,2,3}};
  s.cachedSpeed34=50;s.cachedAnglesE0={{4,5,6}};s.sceneFlags190=0x25;s.scenePresent=s.enabled=true;
  s.body20C=0x12345678;s.velocity70W=3;s.angularVelocityB0W=4;s.force180W=5;s.torque190W=6;s.time864=.75f;
  p.body.mass=2;p.body.inverseMass=.5f;p.centerOffsetW=1;
  for(unsigned i=0;i<3;++i){p.body.inverseInertiaColumns[i].v[i]=1;p.body.translationEnabled[i]=p.body.rotationEnabled[i]=true;}
  p.inverseInertiaExtra[6]=1;
  p.body.linearAccelerationLimitSquared=p.body.angularAccelerationLimitSquared=p.body.linearVelocityLimitSquared=p.body.angularVelocityLimitSquared=1e10f;
  c.gravity378=7;c.gravity37C=11;c.targetRelative2F0={{0,999,0,1}};
  c.mainSceneWorld100.v[0]=c.mainSceneWorld100.v[5]=c.mainSceneWorld100.v[10]=c.mainSceneWorld100.v[15]=1;
  c.mainSceneFlags190=1;c.mainCachedSpeed34=10;initializedSecondaryGlobals49M(c);
 }
};
static void failure(const Fixture&f,SecondaryBodyState49M state,SecondaryForceContext49M context,float dt){
 auto noise=f.noise,oldNoise=noise;auto rng=f.rng;auto out=f.s,oldOut=out;
 check(!produceSecondaryForces49M(state,f.p,context,dt,noise,rng,out),"prefix rejection");
 check(same(noise,oldNoise)&&rng==f.rng&&same(out,oldOut),"prefix rejection atomic");
}
int main(){
 std::fesetround(FE_TONEAREST);Fixture f;countAllocations=true;
 check(bits(f.c.effective4346C8)==0x42820000u&&bits(f.c.effective4346D0)==0x43af0000u&&bits(f.c.effective4346D8)==0x44bb8000u,"initialized first globals");
 check(bits(f.c.effective4346E0)==0x43960000u&&bits(f.c.effective4346E8)==0x457a0000u&&bits(f.c.effective4346F0)==0x42480000u,"initialized middle globals");
 check(bits(f.c.effective4346F8)==0x42f00000u&&bits(f.c.effective434700)==0&&bits(f.c.effective434708)==0x40a00000u,"initialized noise globals");
 PodSphereBody49L view{};makeSecondarySphereBody49M(f.s,f.p,view);
 check(view.sceneFlags190==f.s.sceneFlags190&&view.scenePresent&&view.worldForce.v[3]==5&&view.localTorque.v[3]==6,"view flags and load padding");
 check(view.linearVelocity.v[3]==3&&view.angularVelocity.v[3]==4&&view.localCenter.v[3]==1,"view vector padding");
 constexpr unsigned extra[]={3,7,11,12,13,14,15};
 for(unsigned i=0;i<7;++i)f.p.inverseInertiaExtra[i]=float(i+4);
 makeSecondarySphereBody49M(f.s,f.p,view);
 for(unsigned i=0;i<7;++i)check(view.inverseInertia.v[extra[i]]==float(i+4),"view inertia extra");
 f=Fixture{};
 SecondaryBodyState49M out{};auto oldNoise=f.noise;auto oldRandom=f.rng;
 check(produceSecondaryForces49M(f.s,f.p,f.c,.01f,f.noise,f.rng,out),"state0 prefix");
 check(out.phase==SecondaryPhase49M::ConstraintPending,"prefix phase");
 check(out.root.loads.force.v[0]==-1029&&out.root.loads.force.v[1]==46&&out.root.loads.force.v[2]==-190,"state0 exact loads");
 check(same(out.root.loads.torque,f.s.root.loads.torque),"state0 aligned torque");
 check(same(f.noise,oldNoise)&&f.rng==oldRandom&&out.time864==f.s.time864,"state0 no noise effects");
 check(out.cachedSpeed34==f.s.cachedSpeed34&&same(out.cachedAnglesE0,f.s.cachedAnglesE0),"prefix preserves frame caches");
 check(out.force180W==5&&out.torque190W==6&&out.velocity70W==3&&out.angularVelocityB0W==4,"prefix preserves full lanes");
 auto aliased=f.s;check(produceSecondaryForces49M(aliased,f.p,f.c,.01f,f.noise,f.rng,aliased)&&same(aliased,out),"prefix alias");
 auto zero=f.s;zero.root.scene.v[12]=0;auto zc=f.c;zc.targetRelative2F0.v[0]=0;
 check(produceSecondaryForces49M(zero,f.p,zc,0,f.noise,f.rng,aliased),"zero restoration saturation");
 check(aliased.root.loads.force.v[0]==-129&&aliased.root.loads.force.v[2]==-190,"zero restoration adds zero");
 auto gravity=f.c;gravity.probe104=1;gravity.gravity37C=std::numeric_limits<float>::quiet_NaN();
 check(produceSecondaryForces49M(f.s,f.p,gravity,0,f.noise,f.rng,aliased)&&aliased.root.loads.force.v[1]==54,"gravity selector consumes only selected value");
 const float nan=std::numeric_limits<float>::quiet_NaN();
 auto bad=f.s;bad.phase=SecondaryPhase49M::ConstraintPending;failure(f,bad,f.c,.1f);
 bad=f.s;bad.sceneFlags190=0;failure(f,bad,f.c,.1f);
 bad=f.s;bad.scenePresent=false;failure(f,bad,f.c,.1f);
 bad=f.s;bad.cachedSpeed34=nan;failure(f,bad,f.c,.1f);
 auto bc=f.c;bc.mainSceneFlags190=0;failure(f,f.s,bc,.1f);
 bc=f.c;bc.targetRelative2F0.v[1]=nan;failure(f,f.s,bc,.1f);
 bc=f.c;bc.effective4346C8=nan;failure(f,f.s,bc,.1f);
 failure(f,f.s,f.c,nan);failure(f,f.s,f.c,-1);
 bc=f.c;bc.targetRelative2F0.v[3]=nan;bc.effective4346F0=nan;bc.effective4346F8=nan;bc.effective434700=nan;bc.effective434708=nan;bc.mainCachedSpeed34=nan;bc.gravity378=nan;
 bad=f.s;bad.time864=nan;bad.velocity70W=nan;bad.angularVelocityB0W=nan;
 check(produceSecondaryForces49M(bad,f.p,bc,0,f.noise,f.rng,aliased),"unused state0 values not demanded");
 std::fesetround(FE_DOWNWARD);failure(f,f.s,f.c,.1f);std::fesetround(FE_TONEAREST);
 auto refreshed=f.s;check(refreshSecondaryFrameCache49M(f.s,refreshed),"frame cache refresh");
 check(refreshed.cachedSpeed34!=f.s.cachedSpeed34&&refreshed.cachedAnglesE0.v[0]==0,"frame caches actually refresh");
 check(same(refreshed.root,f.s.root),"frame phase never integrates");
 bad=f.s;bad.phase=SecondaryPhase49M::ConstraintPending;auto saved=refreshed;
 check(!refreshSecondaryFrameCache49M(bad,refreshed)&&same(saved,refreshed),"cache wrong phase atomic");
 // Original noise routing, clock advancement, and shared RNG persistence.
 for(unsigned k=0;k<32;++k){
  Fixture n;n.c.vehicleState40=k%3+1;n.c.effective434700=3;n.s.time864=float(k)*.03125f;
  auto expectedNoise=n.noise;auto expectedRng=n.rng;float value=0;
  check(evaluateGradientNoise49M(expectedNoise,expectedRng,n.c.effective434708*n.s.time864,value),"expected original noise");
  check(produceSecondaryForces49M(n.s,n.p,n.c,.015625f,n.noise,n.rng,aliased),"nonzero prefix");
  check(rr::test::equalNoiseState49W(n.noise,expectedNoise)&&n.rng==expectedRng,"source noise state and RNG");
  check(aliased.time864==n.s.time864+.015625f,"source noise time");
  check(aliased.root.loads.torque.v[2]!=n.s.root.loads.torque.v[2],"noise torque observable");
 }
 // Rejection after the cold noise table was tentatively initialized must
 // preserve both caller-owned tables and the global RNG stream.
 bc=f.c;bc.vehicleState40=1;bc.mainCachedSpeed34=std::numeric_limits<float>::max();
 failure(f,f.s,bc,.01f);
 bc=f.c;bc.vehicleState40=1;bc.effective434708=std::numeric_limits<float>::max();
 failure(f,f.s,bc,.01f);
 // Joint setup is persistent and body B is read-only at all three stages.
 PodSphereBody49L a{},main{};makeSecondarySphereBody49M(f.s,f.p,a);main=a;main.sceneWorld100=f.c.mainSceneWorld100;
 PodSphereJoint49L joint{};check(initializePodSphere49L(joint),"joint initialize");
 PodSphereVector49L anchor{{0,0,0,1}};
 check(setupPodSphereA49L(joint,a,anchor)&&setupPodSphereB49L(joint,&main,anchor,3),"joint setup");
 auto beforeMain=main;SecondaryBodyState49M constrained{};PodSphereJoint49L nextJoint{};float magnitude=-99;
 check(applySecondaryConstraint49M(out,f.p,main,joint,constrained,nextJoint,magnitude),"ordered joint");
 check(constrained.phase==SecondaryPhase49M::Response368Pending&&same(main,beforeMain),"joint phase main read-only");
 check(nextJoint.cached100.v[0]==joint.cached100.v[0]&&same(nextJoint.cached140,joint.cached140),"setup cache retained");
 check(constrained.force180W==out.force180W&&constrained.torque190W==out.torque190W,"joint preserves W loads");
 auto unchanged=constrained;auto unchangedJoint=nextJoint;float oldMagnitude=magnitude;
 check(!applySecondaryConstraint49M(f.s,f.p,main,joint,constrained,nextJoint,magnitude),"joint wrong phase");
 check(same(unchanged,constrained)&&same(unchangedJoint,nextJoint)&&magnitude==oldMagnitude,"joint wrong phase atomic");
 // External response may replace every represented secondary field before48H.
 auto supplied=constrained;supplied.root.loads.force.v[0]+=20;supplied.root.positionCorrection1a0.v[1]=.01f;
 supplied.root.callbackState238=27;supplied.body20C=99;auto integrated=supplied;
 check(completeSecondaryResponse49M(supplied,f.p,.01f,integrated),"supplied contact completion");
 check(integrated.phase==SecondaryPhase49M::Ready&&integrated.root.callbackState238==0&&integrated.body20C==99,"secondary callback differs from main20C");
 check(integrated.sceneFlags190==(supplied.sceneFlags190|0x80u),"integration marks source scene bit7");
 check(integrated.force180W==1&&integrated.torque190W==1&&integrated.velocity70W==supplied.velocity70W&&integrated.angularVelocityB0W==supplied.angularVelocityB0W,"integration W semantics");
 check(same(integrated.cachedAnglesE0,supplied.cachedAnglesE0)&&integrated.cachedSpeed34==supplied.cachedSpeed34,"integration leaves frame caches");
 check(integrated.root.loads.force.v[0]==0&&integrated.root.loads.torque.v[2]==0&&integrated.root.positionCorrection1a0.v[1]==0,"integration clears accumulators");
 check(integrated.root.scene.v[12]!=supplied.root.scene.v[12],"integration actually advances");
 auto before=integrated;check(!completeSecondaryResponse49M(integrated,f.p,.01f,integrated)&&same(before,integrated),"integration wrong phase atomic");
 supplied.scenePresent=false;supplied.root.body.velocity.v[0]=nan;
 check(completeSecondaryResponse49M(supplied,f.p,.01f,integrated),"missing scene clears without demands");
 check(integrated.sceneFlags190==supplied.sceneFlags190,"missing scene preserves flags");
 check(integrated.force180W==1&&integrated.root.callbackState238==0&&std::isnan(integrated.root.body.velocity.v[0]),"missing scene preserved dynamics cleared loads");
 // Invalid demanded joint inputs fail atomically after prefix has succeeded.
 auto coincident=main;coincident.sceneWorld100=a.sceneWorld100;
 auto heldBody=constrained;auto heldJoint=nextJoint;float heldMagnitude=magnitude;
 check(!applySecondaryConstraint49M(out,f.p,coincident,joint,constrained,nextJoint,magnitude),"coincident constraint rejected");
 check(same(heldBody,constrained)&&same(heldJoint,nextJoint)&&magnitude==heldMagnitude,"constraint failure atomic");
 auto disabledJoint=joint;disabledJoint.enabled38=0;
 auto unusedMain=main;unusedMain.sceneWorld100.v[0]=nan;
 check(applySecondaryConstraint49M(out,f.p,unusedMain,disabledJoint,constrained,nextJoint,magnitude),"disabled joint no main demands");
 check(same(constrained.root.loads,out.root.loads)&&magnitude==0,"disabled joint loads preserved");
 // Persistent caller-owned wrapper state across repeated complete invocations.
 auto chain=f.s;auto jchain=disabledJoint;
 for(unsigned k=0;k<120;++k){
  chain.root.loads.force.v[0]=float(int(k%7)-3)*.25f;
  chain.root.loads.force.v[1]=float(k%5)*.125f;
  check(produceSecondaryForces49M(chain,f.p,f.c,.001f,f.noise,f.rng,chain),"persistent prefix");
  check(applySecondaryConstraint49M(chain,f.p,main,jchain,chain,jchain,magnitude),"persistent joint");
  check(completeSecondaryResponse49M(chain,f.p,.001f,chain),"persistent integration");
  check(chain.cachedSpeed34==50&&chain.body20C==0x12345678u,"persistent caches and20C");
  check(chain.force180W==1&&chain.torque190W==1&&chain.velocity70W==3&&chain.angularVelocityB0W==4,"persistent padding");
 }
 countAllocations=false;check(allocations==0,"zero allocations");
 std::printf("secondary_body_api49m: %u checks passed; %u allocations\n",checks,allocations);
 return 0;
}
