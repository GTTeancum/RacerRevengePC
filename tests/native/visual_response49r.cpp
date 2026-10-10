#include "vehicle_visual49r_internal.h"
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>

namespace { bool watch = false; unsigned allocations = 0; }
void* operator new(std::size_t n) { if (watch) ++allocations; if (void* p = std::malloc(n ? n : 1)) return p; throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }

namespace {
using namespace rr::game;
unsigned checks = 0, cases = 0;
void require(bool x, const char* label) { ++checks; if (!x) { watch = false; std::fprintf(stderr, "FAIL %s\n", label); std::exit(1); } }
float fp(std::uint32_t word) { float x; std::memcpy(&x, &word, 4); return x; }
std::uint32_t bits(float x) { std::uint32_t word; std::memcpy(&word, &x, 4); return word; }
SceneNodeIndex49P idx(int i) { return static_cast<SceneNodeIndex49P>(i); }
BodyMatrix48G identity() { return BodyMatrix48G{{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}}; }
struct Fixture {
    SceneNode49P nodes[6]{};
    SceneGraph49P graph{nodes, 6};
    VehicleVisualInput49R in{};
    VehicleVisualParameters49R p{};
    VisualSceneBindings49R binding{idx(0), idx(1), idx(2), idx(3), false, false, 0};
    detail::VisualFrame49R frame{};
    VehicleVisualState49R state{};
    VisualShared49R shared{};
    CoupledNoiseContext49M noise{};
    VehicleVisualDiagnostic49R diagnostic{};
    Fixture() {
        for (auto& n : nodes) { n.localC0 = n.world100 = identity(); n.parent180 = n.firstChild184 = n.nextSibling188 = idx(-1); n.sceneFlags190 = 0x41; }
        nodes[0].firstChild184 = idx(1); nodes[1].parent180 = idx(0);
        nodes[2].firstChild184 = idx(4); nodes[4].parent180 = idx(2);
        nodes[3].firstChild184 = idx(5); nodes[5].parent180 = idx(3);
        in.phase = VehicleVisualPhase49R::afterAnimationBeforeChariotAndPhysics;
        in.frameFactor = .25f; in.cachedBodySpeed34 = 1; in.vehicle844 = 1;
        frame.mainUp = {{0,1,0,1}};
        p.effectiveSpeedSquaredA54 = 4; p.effectiveInputSpeedA68 = 1;
        p.minTimeA34 = p.maxTimeA38 = 1; p.damageTranslationA58 = p.damageOrientationA5C = 1;
        p.verticalLimit7EC = 100;
        p.rightBase2C0 = {{1,0,0,1}}; p.leftBase2B0 = {{-1,0,0,1}};
        state.right.position = p.rightBase2C0; state.left.position = p.leftBase2B0;
        state.right.orientation = state.left.orientation = {{1,0,0,0}};
        state.phase8B0 = 2; state.phase900 = 3; state.phase950 = 4; state.phase9A0 = 5;
        shared.zero434630 = {{0,0,0,1}};
        const std::uint32_t words[6] = {0x3f59999a, 0x3f19999a, 0, 0x3e4ccccd, 0x3e4ccccd, 0x40800000};
        for (unsigned i = 0; i < 6; ++i) shared.globals[i] = {1, words[i]};
        noise.sharedRandomState = 0xdeadbeefu;
    }
    bool run() {
        watch = true;
        const bool ok = detail::updateVisualResponses49R(in,p,binding,frame,graph,state,shared,noise,diagnostic);
        watch = false; ++cases; require(allocations == 0, "zero heap allocations"); return ok;
    }
};
float noiseValue(CoupledNoiseContext49M& context, float x) {
    float out; require(evaluateGradientNoise49M(context.noise,context.sharedRandomState,x,out), "noise leaf admitted"); return out;
}
float add(float a, float b) { volatile float out = a+b; return out; }
float mul(float a, float b) { volatile float out = a*b; return out; }
void basicEulerAndWords() {
    Fixture f; f.in.applicationByte58=2; f.in.engine74=2; f.in.engine70=3; f.p.thrust9FC=10;
    f.state.right.position.v[3]=fp(0x7fc12345); f.state.left.position.v[3]=fp(0xffc54321);
    f.state.right.linearVelocity.v[3]=fp(0x80000000); f.state.left.angularVelocity.v[3]=fp(0x7f800000);
    const auto oldWorld=f.nodes[2].world100;
    require(f.run(), "thrust admitted");
    require(f.state.right.linearVelocity.v[2]==5 && f.state.right.position.v[2]==1.25f, "right semi implicit Euler");
    require(f.state.left.linearVelocity.v[2]==7.5f && f.state.left.position.v[2]==1.875f, "left routed thrust");
    require(f.state.phase8B0==2.25f && f.state.phase900==3.25f && f.state.phase950==4.25f && f.state.phase9A0==5.25f, "four clocks including absent roots");
    require(bits(f.state.right.position.v[3])==0x7fc12345 && bits(f.state.left.position.v[3])==0xffc54321, "copy-only position W words");
    require(bits(f.state.right.linearVelocity.v[3])==0x80000000 && bits(f.state.left.angularVelocity.v[3])==0x7f800000, "untouched velocity W words");
    require(f.nodes[2].localC0.v[14]==1.25f && f.nodes[3].localC0.v[14]==1.875f, "actual root local outputs");
    require(std::memcmp(&oldWorld,&f.nodes[2].world100,sizeof(oldWorld))==0, "root worlds stay lazy");
    require(f.nodes[2].sceneFlags190==0xc0 && f.nodes[3].sceneFlags190==0xc0 && f.nodes[4].sceneFlags190==0x40 && f.nodes[5].sceneFlags190==0x40, "source setter invalidates ready descendants");
    require(f.shared.globals[6].guard==1 && f.shared.globals[6].word==0x41200000 && f.shared.globals[7].word==0x40000000 && f.shared.globals[8].word==0x41200000, "impact and empty tail lazy globals");
    require(!f.noise.noise.initialized && f.noise.sharedRandomState==0xdeadbeef, "844 gate preserves noise");
}
void binderAndConstructor() {
    Fixture f; f.in.applicationByte58=2; f.in.frameFactor=.5f; f.p.binderSpringA10=2; f.state.right.position.v[0]=2;
    require(f.run(), "binder admitted");
    require(f.state.right.position.v[0]==1.5f && f.state.left.position.v[0]==-.5f, "left uses old binder after right moves");
    Fixture zero; zero.in.applicationByte58=2; zero.p.binderSpringA10=20;
    zero.state.right.position={{0,0,0,1}}; zero.state.left.position={{0,0,0,1}};
    require(zero.run(), "constructor coincident roots use finite VU Q");
    require(zero.state.right.position.v[0]==0 && zero.state.left.position.v[0]==0, "zero separation multiplied by saturated Q");
    Fixture small; small.state.right.position.v[0]=0x1p-65f; small.state.left.position.v[0]=0;
    require(!small.run() && small.diagnostic.error==VehicleVisualError49R::arithmetic, "positive subnormal VU norm excluded");
}
void routesAndSnapshots() {
    Fixture f; f.in.frameFactor=1; f.in.engine68=1; f.in.engine6C=0; f.in.aeroE8=0; f.in.aeroE4=1; f.p.liftA00=2;
    require(f.run(), "differential tie admitted");
    require(f.state.right.linearVelocity.v[1]==2 && f.state.left.linearVelocity.v[1]==-2, "equal magnitude selects aero with opposite sign");
    Fixture bit0; bit0.in.applicationByte58=1; bit0.in.engine74=2; bit0.in.engine70=3; bit0.p.thrust9FC=1;
    require(bit0.run(), "bit0 alone admitted");
    require(bit0.state.right.linearVelocity.v[2]==2 && bit0.state.left.linearVelocity.v[2]==3, "bit1 extraction ignores bit0 and retains gain4");
    Fixture bit2; bit2.in.applicationByte58=4; bit2.p.springA08=4; bit2.state.right.position.v[0]=0;
    require(bit2.run(), "bit2 admitted"); require(bit2.state.right.linearVelocity.v[0]==1, "bit2 full spring without binder");
    Fixture reduced; reduced.p.springA08=4; reduced.state.right.position.v[0]=0;
    require(reduced.run(), "reduced spring admitted"); require(bits(reduced.state.right.linearVelocity.v[0])==bits(.2f), "neither bit gives source spring multiplier");
    Fixture raw; raw.in.applicationByte58=2; raw.in.aeroDC=1; raw.in.rawB5C=.5f; raw.in.cachedBodySpeed34=0; raw.p.thrust9FC=4; raw.p.dragA04=-2;
    require(raw.run(), "raw channels admitted at zero speed");
    require(raw.state.right.linearVelocity.v[2]==-.25f && raw.state.left.linearVelocity.v[2]==.5f, "raw channels are not speed scaled");
    Fixture angular; angular.in.applicationByte58=2; angular.in.frameFactor=.125f; angular.p.binderTorqueA30=2;
    angular.state.right.orientation={{.9f,.15f,.2f,.3f}}; angular.state.left.orientation={{.7f,-.25f,.1f,-.2f}};
    Vector3_48C r{},l{}; require(quaternionToAngles48G(angular.state.right.orientation,r)&&quaternionToAngles48G(angular.state.left.orientation,l), "old angle helper");
    volatile float dx=r.v[0]-l.v[0], dz=r.v[2]-l.v[2];
    const float expectedX=mul(.125f,mul(2,dx)), expectedZ=mul(.125f,mul(2,dz));
    require(angular.run(), "angle binder admitted");
    require(angular.state.right.angularVelocity.v[0]==expectedX && angular.state.left.angularVelocity.v[0]==-expectedX && angular.state.left.angularVelocity.v[2]==-expectedZ, "both torques use old quaternion angles");
    require(angular.state.right.angularVelocity.v[1]==0 && angular.state.left.angularVelocity.v[1]==0, "binder forcibly clears yaw");
}
void spacesAndImpact() {
    Fixture f; f.in.frameFactor=1; f.p.impact9F0=1;
    f.nodes[0].world100={{0,1,0,0,-1,0,0,0,0,0,1,0,8,9,10,1}};
    f.in.impactBA0={{0,-30,50,fp(0x7fc11111)}};
    f.nodes[1].world100.v[12]=20; f.nodes[1].world100.v[13]=30; f.nodes[1].world100.v[14]=40;
    require(f.run(), "rotated main impact admitted");
    require(f.state.right.linearVelocity.v[0]==-10 && f.state.left.linearVelocity.v[0]==0 && f.state.right.linearVelocity.v[2]==10, "main inverse and impact clamp10");
    require(f.nodes[2].localC0.v[12]==11 && f.nodes[2].localC0.v[14]==50, "graphics cached world composed into local");
    Fixture suppress; suppress.in.frameFactor=1; suppress.in.objectD34_10=.3f; suppress.in.vehicleCFC=1; suppress.in.impactBA0={{5,6,7,1}}; suppress.p.impact9F0=1;
    require(suppress.run(), "impact suppression admitted");
    require(suppress.state.left.linearVelocity.v[0]==0 && suppress.state.left.linearVelocity.v[1]==6 && suppress.state.left.linearVelocity.v[2]==7, "suppression clears only impact X");
    Fixture up; up.in.frameFactor=1; up.p.groundImpact9F4=3; up.in.probeVector1D0={{1,2,4,1}}; up.frame.mainUp={{0,0,1,1}}; up.frame.containerUp={{1,0,0,1}};
    require(up.run(), "ground dot admitted"); require(up.state.right.linearVelocity.v[1]==12 && up.state.left.linearVelocity.v[1]==12, "ground uses saved main up");
    Fixture warm; warm.shared.globals[6]={7,bits(3)}; warm.shared.globals[7]={9,0x7fc11111}; warm.shared.globals[8]={17,0xff800000}; warm.in.impactBA0={{20,20,20,1}}; warm.p.impact9F0=1;
    require(warm.run(), "warm store-only nonfinite globals admitted");
    require(warm.shared.globals[6].guard==7 && warm.shared.globals[7].word==0x7fc11111 && warm.shared.globals[8].word==0xff800000 && warm.state.left.linearVelocity.v[0]==.75f, "warm effective words preserved");
}
void phasesNoiseAndClamps() {
    Fixture cold; cold.in.vehicle844=0; cold.in.frameFactor=0; cold.p.orientationScaleA64=0;
    auto expected=cold.noise;
    const float coordinates[12]={3,5,5,20,8,4,7,9,13,15,9,3};
    for(float x:coordinates) (void)noiseValue(expected,x);
    require(cold.run(), "zero amplitude and frame noise admitted");
    require(cold.noise.noise.initialized && cold.noise.sharedRandomState==expected.sharedRandomState && cold.noise.noise.scratchCell==expected.noise.scratchCell, "zero amplitude still cold initializes and ends at twelfth call");
    require(cold.noise.sharedRandomState==0x8f0c8cce && std::memcmp(cold.noise.noise.permutation,expected.noise.permutation,sizeof(expected.noise.permutation))==0, "source1791 RNG draws shared tables");
    const auto warmSeed=cold.noise.sharedRandomState; require(cold.run(), "warm repeat admitted"); require(cold.noise.sharedRandomState==warmSeed, "no warm RNG draws");
    Fixture slide; slide.in.aeroDC=1; slide.in.rawB5C=1; slide.p.minTimeA34=1; slide.p.maxTimeA38=5;
    require(slide.run(), "positive slide phase admitted");
    require(slide.state.phase8B0==3.25f && slide.state.phase900==3.5f && slide.state.phase950==5.25f && slide.state.phase9A0==5.5f, "positive turn chooses left phase rate");
    Fixture negative; negative.in.aeroDC=1; negative.in.rawB5C=-1; negative.p.minTimeA34=1; negative.p.maxTimeA38=5;
    require(negative.run(), "negative slide phase admitted");
    require(negative.state.phase8B0==2.5f && negative.state.phase900==4.25f, "negative turn chooses right phase rate");
    Fixture denominator; denominator.p.effectiveSpeedSquaredA54=0; denominator.p.minTimeA34=7; denominator.p.maxTimeA38=-8;
    require(denominator.run(), "zero denominator admitted"); require(denominator.state.phase8B0==2 && denominator.state.phase9A0==5, "zero denominator uses zero rate");
    Fixture damage; damage.in.vehicle844=0; damage.in.frameFactor=0; damage.state.phase900=.375f; damage.p.minAmplitudeA44=damage.p.maxAmplitudeA48=1; damage.p.damageTranslationA58=3; damage.in.damage7D0=.5f; damage.p.orientationScaleA64=.3f;
    auto beforeNoise=damage.noise; const float nx=noiseValue(beforeNoise,.375f); const auto beforeQuaternion=damage.state.right.orientation;
    require(damage.run(), "damage noise admitted");
    require(bits(damage.nodes[2].localC0.v[12])==bits(add(1,mul(2,nx))), "source damage factor scales temporary X");
    require(damage.state.right.position.v[0]==1 && std::memcmp(&beforeQuaternion,&damage.state.right.orientation,sizeof(beforeQuaternion))==0, "noise does not feed persistent X or quaternion");
    require(damage.nodes[2].localC0.v[1]!=0 || damage.nodes[2].localC0.v[2]!=0, "visual orientation receives separate noise");
    Fixture clamp; clamp.in.vehicle844=0; clamp.in.frameFactor=0; clamp.state.phase900=.375f; clamp.p.minAmplitudeA44=clamp.p.maxAmplitudeA48=1; clamp.p.verticalLimit7EC=.25f;
    auto probe=clamp.noise; (void)noiseValue(probe,.375f); const float ny=noiseValue(probe,2.375f);
    require(ny!=0, "noise clamp fixture has displacement");
    clamp.state.right.position.v[1]=ny>0?.25f:-.25f; clamp.state.right.linearVelocity.v[1]=9;
    require(clamp.run(), "noise-induced clamp admitted");
    require(clamp.state.right.position.v[1]==(ny>0?.25f:-.25f) && bits(clamp.state.right.linearVelocity.v[1])==0, "noise clamp clears persistent Y velocity");
    Fixture equality; equality.in.frameFactor=0; equality.p.verticalLimit7EC=.25f; equality.state.right.position.v[1]=.25f; equality.state.right.linearVelocity.v[1]=9;
    require(equality.run(), "exact clamp equality admitted"); require(equality.state.right.linearVelocity.v[1]==9, "equality retains velocity");
    Fixture downward; downward.in.frameFactor=1; downward.shared.globals[2].word=bits(10); downward.p.verticalLimit7EC=2;
    require(downward.run(), "844 force and clamp admitted"); require(downward.state.right.position.v[1]==-2 && downward.state.right.linearVelocity.v[1]==0, "844 force lower clamp");
}
void quaternionAndAdmission() {
    Fixture q; q.in.frameFactor=0; q.state.right.orientation={{2,0,0,0}};
    require(q.run(), "zero factor normalizes quaternion"); require(q.state.right.orientation.v[0]==1 && q.nodes[2].localC0.v[0]==1, "normalization remains at zero factor");
    Fixture zero; zero.state.right.orientation={}; require(zero.run(), "zero norm quaternion admitted"); require(zero.state.right.orientation.v[0]==0 && zero.nodes[2].localC0.v[0]==1, "zero quaternion stays zero with identity matrix formula");
    // Independent original-word witness in response/tools/double_normalization.cpp.
    Fixture twice; twice.in.frameFactor=0; twice.state.right.orientation={{fp(0x3e9eb852),fp(0x3e2e147b),fp(0x3f35c28f),fp(0x3e6b851f)}};
    const std::uint32_t persistent[4]={0x3ec031a0,0x3e52cb13,0x3f5c17cc,0x3e8e986f};
    const std::uint32_t basis[12]={0xbf22288c,0x3f10240a,0xbf07e1f9,0,0x3e14584c,0x3f42970c,0x3f22288b,0,0x3f42970d,0x3ea60fea,0xbf10240c,0};
    require(twice.run(), "double normalization witness admitted");
    for(unsigned i=0;i<4;++i)require(bits(twice.state.right.orientation.v[i])==persistent[i], "first normalization persistent witness");
    for(unsigned i=0;i<12;++i)require(bits(twice.nodes[2].localC0.v[i])==basis[i], "second normalization original-word matrix witness");
    Fixture unused; unused.in.vehicle40=1; unused.in.damage7D0=unused.in.damage7D8=unused.in.damage7D4=unused.in.damage7DC=fp(0x7fc11111); unused.in.rawB5C=fp(0x7fc22222); unused.p.orientationScaleA64=fp(0x7fc33333);
    require(unused.run(), "unconsumed NaN damage and gated orientation scale admitted");
    Fixture invalid; invalid.in.engine68=std::numeric_limits<float>::infinity(); require(!invalid.run(), "consumed nonfinite differential rejected");
    require(std::fesetround(FE_DOWNWARD)==0, "set nonnearest"); Fixture rounding; require(!rounding.run()&&rounding.diagnostic.error==VehicleVisualError49R::rounding_mode, "nonnearest rejected"); require(std::fesetround(FE_TONEAREST)==0, "restore nearest");
}
void shapingBranches() {
    Fixture descending; descending.p.minTimeA34=5; descending.p.maxTimeA38=1;
    require(descending.run(), "descending time endpoints admitted"); require(descending.state.phase8B0==3 && descending.state.phase900==4, "descending interpolation retains order");
    Fixture timeClamp; timeClamp.in.cachedBodySpeed34=3; timeClamp.p.minTimeA34=5; timeClamp.p.maxTimeA38=1;
    require(timeClamp.run(), "descending time clamp admitted"); require(timeClamp.state.phase8B0==2.25f, "descending output clamp uses lower endpoint");
    Fixture state40; state40.in.vehicle40=1; state40.in.aeroDC=1; state40.in.rawB5C=1; state40.p.minTimeA34=1; state40.p.maxTimeA38=5;
    require(state40.run(), "state40 phase branch admitted"); require(state40.state.phase8B0==2.5f && state40.state.phase900==3.5f && state40.state.phase950==4.5f, "state40 bypasses slide phase selection");
    for(unsigned route=0;route<5;++route) {
        Fixture f; f.in.vehicle844=0; f.in.frameFactor=0; f.state.phase900=.375f; f.state.phase8B0=.125f;
        f.p.minAmplitudeA44=.25f; f.p.maxAmplitudeA48=.75f; f.in.aeroDC=1;
        float rightAmplitude=.375f,leftAmplitude=.375f;
        if(route==0) f.in.rawB5C=fp(0x3d4ccccd); // Exact0.05 is ordinary.
        if(route==1) { f.in.rawB5C=1; rightAmplitude=.25f;leftAmplitude=.75f; }
        if(route==2) { f.in.rawB5C=-1; rightAmplitude=.75f;leftAmplitude=.25f; }
        if(route==3) { f.in.vehicle40=1; f.p.damageTranslationA58=-2; rightAmplitude=leftAmplitude=-.75f; f.in.damage7D0=9; }
        if(route==4) { f.p.damageTranslationA58=-1; f.in.damage7D0=.75f; f.in.damage7D4=-2; rightAmplitude=-.1875f; }
        auto expected=f.noise; const float nr=noiseValue(expected,.375f),nl=noiseValue(expected,5.125f);
        require(f.run(), "amplitude route admitted");
        require(bits(f.nodes[2].localC0.v[12])==bits(add(1,mul(rightAmplitude,nr))), "right source amplitude branch");
        require(bits(f.nodes[3].localC0.v[12])==bits(add(-1,mul(leftAmplitude,nl))), "left source amplitude branch");
    }
}
}
int main() {
    require(std::fesetround(FE_TONEAREST)==0,"nearest");
    basicEulerAndWords(); binderAndConstructor(); routesAndSnapshots(); spacesAndImpact(); phasesNoiseAndClamps(); quaternionAndAdmission(); shapingBranches();
    std::printf("{\"suite\":\"visual_response49r\",\"cases\":%u,\"checks\":%u,\"hot_allocations\":%u,\"status\":\"pass\"}\n",cases,checks,allocations);
}
