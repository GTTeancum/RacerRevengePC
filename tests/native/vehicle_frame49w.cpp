#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#include <limits>
using namespace frame49w_test;
static std::uint64_t rejects=0,reverseCases=0,persistentCases=0,traceChecks=0;
template<class F>void reject(Fixture&f,F&&call,const char*why){const auto before=f.state;const auto n=allocations;bool ok=call();require(!ok,why);require(n==allocations,"rejection allocated");words(&f.state,&before,sizeof before,"atomic rejected framecall");++rejects;}
void traceOrder(){
 Fixture f(80);
#ifdef RR_VEHICLE_FRAME49W_STAGE
 frame49w_trace::clear();
#endif
 f.advanceFrame({128,128,128,128},.03f,60);
#ifdef RR_VEHICLE_FRAME49W_STAGE
 require(!frame49w_trace::overflow,"frame trace overflow");
 std::vector<U> actual(frame49w_trace::events,frame49w_trace::events+frame49w_trace::count);
 const std::vector<U> prefix={0x1a15c0,0x217de0,0x21a000,0x21c120,0x21a660,0x228b8c,0x167bb4,0x167bb4,0x1a8390};
 require(actual.size()>prefix.size()&&std::equal(prefix.begin(),prefix.end(),actual.begin()),"source graphics/cache/schedule order");
 const std::vector<U> step={0x21990c,0x1b1800,0x2e7720,0x2827d0,0x2e7c3c,0x2e7c98,0x16cf58,0x16d7c0,0x1b1a30,0x218a3c,0x218c60,0x218ce0,0x218e30,0x218e44,0x218eb8,0x2190ac,0x2190fc,0x219e80,0x219e94,0x219170,0x219400,0x219f80,0x219f90,0x219fd8};
 require(actual.size()==prefix.size()+2*step.size(),"source substep trace length");for(unsigned n=0;n<2;++n)require(std::equal(step.begin(),step.end(),actual.begin()+prefix.size()+n*step.size()),"source substep getter/force/body/tail order");++traceChecks;
#endif
}
void frameCases(){
 for(float speed:{80.f,170.f}){Fixture f(speed);const auto pose=f.state.nodes[50].localC0;for(unsigned i=0;i<16;++i)f.advanceFrame({128,std::uint8_t(120+i%13),std::uint8_t(115+i%25),128},.016f,60);require(std::memcmp(&pose,&f.state.nodes[50].localC0,64)!=0,"persistent main pose didnot advance");require(f.state.bodies.main.components.root.body.previousPositionDerivative.v[2]!=0,"persistent main derivative missing");++persistentCases;}
 // One initial state; declared354 forces slow the main body. Cached34 retains
 // its source lifetime, so the actual virtual→physical reversal is delayed.
 {Fixture f(170);unsigned reversals=0;for(unsigned i=0;i<20;++i){const U physical=f.state.bodies.vehicle838;const auto prior=f.state.bodies.main.components.root.body.velocity;
  f.advanceFrame({128,128,128,128},.03f,60,[](ContactEffects49W&e,unsigned frame,unsigned step){if(frame<3){e.main.loads.force.v[2]-=60000;e.main.loads.force.v[0]+=float(step+1)*2;}e.main.positionCorrection1A0={{.0001f,0,.0002f}};e.main.rotationCorrection1B0={{0,.00001f,0}};e.main.body20C=17;e.secondary.body20C=0x12345678;e.impactBA0={{1,2,3,-9}};});
  if(!physical&&f.state.bodies.vehicle838)++reversals;
  if(i<3)require(f.state.bodies.main.components.root.body.velocity.v[2]!=prior.v[2],"source-backed velocity stayed fixed");
 }require(reversals>0,"no actual reverse followedbyphysics");require(f.state.secondarySupplement.previousVelocity90W==f.state.bodies.secondary.velocity70W,"secondary supplement notadvanced afterreverse");reverseCases+=reversals;++persistentCases;}
 // Rounded/truncated exact selector counts1..4, zero-delta preserved histories,
 // and persistent next graphics all consume the previous terminal body outputs.
 {Fixture f(80);bool counts[4]{};for(unsigned i=0;i<25;++i){const float dt=i%5==0?0.f:(i%5==1?.016f:(i%5==2?.02f:(i%5==3?.034f:.05f)));f.advanceFrame({128,128,128,128},dt,60);counts[f.state.bodies.main.schedule.count-1]=true;}for(bool x:counts)require(x,"missing source substep count");++persistentCases;}
 // Ready-cache/local distinction survives through graphics; source getters
 // consume cachedWorld without silently replacing it with current local.
 {Fixture f(80);f.state.nodes[50].localC0=c::v::matrix(.4f,900,800,700);f.state.nodes[49].localC0=c::v::matrix(.2f,12,19,24);f.state.nodes[49].sceneFlags190&=0xfe;frame49w_test::project(f.state,f.bindings);f.advanceFrame();f.advanceFrame();++persistentCases;}
 {Fixture f(80);f.secondaryInputs.gravity378=1000;f.secondaryInputs.gravity37C=-500;f.advanceFrame();require(f.state.fields.gravity378==f.selectors.vehicle378&&f.state.fields.gravity37C==f.selectors.vehicle37C,"canonicalgravity owner");}
 traceOrder();
}
void admission(){
 Fixture f(80);f.compareOriginal=false;FrameReader49K read{};read.packet={128,128,128,128};read.preparedFactor=.016f;read.actions={nullptr,noAction};read.codes=f.actionCodes;
 const auto initial=f.state;for(unsigned n=0;n<16;++n){f.state=initial;switch(n){case 0:f.state.fields.vehicle58=1;break;case 1:f.state.fields.vehicle3C=1;break;case 2:f.state.bodies.main.vehicleState40=1;break;case 3:f.state.bodies.main.applicationActive11C=1;break;case 4:f.state.bodies.main.domain.body320=0;break;case 5:f.state.bodies.main.domain.bodyChildCount=1;break;case 6:f.state.bodies.main.domain.sceneChildCount=0;break;case 7:f.state.shape.registrationE0=1;break;case 8:f.state.nodes[50].firstChild184=Index::none;break;case 9:f.state.bodies.secondary.root.scene.v[0]+=1;break;case 10:f.state.bodies.mainSphere.sceneFlags190^=1;break;case 11:f.state.bodies.main.phase=MainBodyPhase49K::AwaitingPostBase;break;case 12:f.state.bodies.secondary.phase=SecondaryPhase49M::ConstraintPending;break;case 13:f.state.shape.type88=1;break;case 14:f.state.nodes[50].sceneFlags190&=0xfe;break;case 15:f.state.bodies.secondaryDomain.sceneObserverCount=1;break;}
 reject(f,[&]{return readVehicleFrame49W(f.state,f.bindings,read,f.workspace,f.diagnostic);},"unsupported domain accepted");}
 f.state=initial;auto saved=f.bindings.mainDomain;f.bindings.mainDomain.rootObserverPresent19C=true;reject(f,[&]{return readVehicleFrame49W(f.state,f.bindings,read,f.workspace,f.diagnostic);},"nonnull main observer");f.bindings.mainDomain=saved;
 read.context.source7D0=std::numeric_limits<float>::quiet_NaN();read.context.previousFlagsB9C=0;read.context.gateByte58=0xff;f.hot([&]{return readVehicleFrame49W(f.state,f.bindings,read,f.workspace,f.diagnostic);},"read for cuts");require(f.state.bodies.main.controls.player.flagsB9C==0x80,"reader duplicateflags overwrotecanonical");PostPlayerAt217DE0_49K post{};post.player=f.state.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=true;
 reject(f,[&]{return bindVehicleFrame49W(f.state,f.bindings,f.parameters,{PreservingCut49W::PlayerAndBaseBefore217DE0,false,true},post,f.workspace,f.diagnostic);},"unpreserved externalcut");
 post.player.flagsB9C&=0x7f;reject(f,[&]{return bindVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,f.workspace,f.diagnostic);},"unrecovered firstbase");post.player.flagsB9C|=0x80;
 f.hot([&]{return bindVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,f.workspace,f.diagnostic);},"binding for cuts");
 const auto par=f.parameters.activation;f.parameters.activation.vector433B80[0]=c::v::bits(1);reject(f,[&]{return graphicsVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::Base2176E0BeforeGraphics),f.workspace,f.diagnostic);},"different shared433B80");f.parameters.activation=par;
 reject(f,[&]{return graphicsVehicleFrame49W(f.state,f.bindings,f.parameters,{PreservingCut49W::Base2176E0BeforeGraphics,true,false},f.workspace,f.diagnostic);},"unproved BB0W consumer");
 // Late native graphics arithmetic failure rolls back graph, bodies, noise and
 // all preceding successful49T/49R scratch changes together.
 const float duration=f.state.lifecycle.duration848;f.state.lifecycle.duration848=0;reject(f,[&]{return graphicsVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::Base2176E0BeforeGraphics),f.workspace,f.diagnostic);},"late graphics failure");f.state.lifecycle.duration848=duration;
 f.state=initial;f.beginFrame({128,128,128,128},.03f,60);
 reject(f,[&]{return finishVehicleQuery49W(f.state,f.bindings,f.parameters,{0,true,true},f.workspace,f.diagnostic);},"query outofphase");
 f.hot([&]{return beginVehicleSubstep49W(f.state,f.bindings,f.selectors,f.workspace,f.diagnostic);},"begin boundary");PostWorld49W world{WorldCut49W::First358And219420,contactEffects49W(f.state),false,{},2};
 reject(f,[&]{return produceVehicleForces49W(f.state,f.bindings,f.parameters,world,f.workspace,f.diagnostic);},"missing firstsecondary36C");world.point=WorldCut49W::First358Secondary36CAnd219420;
 f.hot([&]{return produceVehicleForces49W(f.state,f.bindings,f.parameters,world,f.workspace,f.diagnostic);},"world boundary");Post354_49W response{contactEffects49W(f.state),0,0,0};f.hot([&]{return respondVehicle354_49W(f.state,f.bindings,f.parameters,response,f.secondaryInputs,f.workspace,f.diagnostic);},"skip secondary");
 f.state.bodies.main.components.root.accumulator1c0.v[0]=1;f.parameters.bodies.main.effectiveImpactZero434630.v[0]=1;reject(f,[&]{return integrateVehicleMain49W(f.state,f.bindings,f.parameters,f.workspace,f.diagnostic);},"duplicate zero bypassed impact");f.state.bodies.main.components.root.accumulator1c0.v[0]=0;
 f.hot([&]{return integrateVehicleMain49W(f.state,f.bindings,f.parameters,f.workspace,f.diagnostic);},"main toquery");reject(f,[&]{return finishVehicleQuery49W(f.state,f.bindings,f.parameters,{0x20,true,true},f.workspace,f.diagnostic);},"hit query accepted");
 f.hot([&]{return finishVehicleQuery49W(f.state,f.bindings,f.parameters,{0,true,true},f.workspace,f.diagnostic);},"finish first");f.hot([&]{return beginVehicleSubstep49W(f.state,f.bindings,f.selectors,f.workspace,f.diagnostic);},"later skipped");world.point=WorldCut49W::LaterSkipped;world.effects=contactEffects49W(f.state);world.effects.main.loads.force.v[0]=1;reject(f,[&]{return produceVehicleForces49W(f.state,f.bindings,f.parameters,world,f.workspace,f.diagnostic);},"later skipped accepted changes");world.effects=contactEffects49W(f.state);
 // Padding is not represented source state. Equivalent projections remain valid.
 const auto pad=offsetof(PodSphereJoint49L,bodyBPresent48)+sizeof(bool);auto*raw=reinterpret_cast<unsigned char*>(&world.effects.joint);for(std::size_t i=pad;i<offsetof(PodSphereJoint49L,length4C);++i)raw[i]=0xa5;
 f.hot([&]{return produceVehicleForces49W(f.state,f.bindings,f.parameters,world,f.workspace,f.diagnostic);},"padding-independent skippedboundary");
 f.state=initial;const int mode=std::fegetround();std::fesetround(FE_UPWARD);reject(f,[&]{return readVehicleFrame49W(f.state,f.bindings,read,f.workspace,f.diagnostic);},"rounding accepted");std::fesetround(mode);
}
void noiseStateComparison() {
 GradientNoiseState49M a{}, b{};
 auto* raw=reinterpret_cast<unsigned char*>(&b);
 for(std::size_t i=offsetof(GradientNoiseState49M,initialized)+sizeof(bool);i<sizeof b;++i)raw[i]=0xa5;
 require(rr::test::equalNoiseState49W(a,b),"noise comparison interpreted padding");
 b.permutation[255]=1;require(!rr::test::equalNoiseState49W(a,b),"noise comparison lost permutation");b.permutation[255]=0;
 const std::uint32_t minusZero=0x80000000u;std::memcpy(&b.gradient1D[255],&minusZero,4);
 require(!rr::test::equalNoiseState49W(a,b),"noise comparison lost gradient sign bit");b.gradient1D[255]=0;
 b.scratchCell=1;require(!rr::test::equalNoiseState49W(a,b),"noise comparison lost scratch");b.scratchCell=0;
 b.initialized=true;require(!rr::test::equalNoiseState49W(a,b),"noise comparison lost readiness");
}
int main(int argc,char**argv){try{if(argc>1)std::filesystem::current_path(argv[1]);std::fesetround(FE_TONEAREST);noiseStateComparison();frameCases();admission();require(shapeComparisons==substeps,"missing originalshape substepcomparison");std::cout<<"{\"passed\":true,\"frames\":"<<frames<<",\"substeps\":"<<substeps<<",\"graphics_comparisons\":"<<graphicsComparisons<<",\"cache_comparisons\":"<<cacheComparisons<<",\"tail_comparisons\":"<<tailComparisons<<",\"shape_original_comparisons\":"<<shapeComparisons<<",\"body_comparisons\":"<<bodyComparisons<<",\"force_comparisons\":"<<forceComparisons<<",\"secondary_comparisons\":"<<secondaryComparisons<<",\"input_comparisons\":"<<inputComparisons<<",\"reverse_cases\":"<<reverseCases<<",\"persistent_cases\":"<<persistentCases<<",\"rejections\":"<<rejects<<",\"trace_checks\":"<<traceChecks<<",\"native_calls\":"<<nativeCalls<<",\"native_allocations\":0,\"checks\":"<<frame49w_test::checks<<"}\n";return 0;}catch(const std::exception&e){std::cerr<<"FAIL frame49W: "<<e.what()<<'\n';return 1;}}
