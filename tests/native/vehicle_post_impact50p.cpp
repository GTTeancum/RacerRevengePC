#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#include <memory>
namespace f=frame49w_test;using namespace rr::game;using J=nlohmann::json;
int main(int argc,char**argv){try{if(argc!=2)return 2;std::fesetround(FE_TONEAREST);f::Fixture x(4);auto&s=x.state;s.impactSupplement.partnerId=0xffffffffu;
 FrameReader49K read{};read.packet={128,128,128,128};read.preparedFactor=.016f;read.actions={nullptr,f::noAction};read.codes=x.actionCodes;read.context.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;
 unsigned b9dReaderCases=0,b9dTailCases=0;
 f::require(!s.fields.flagB9D0,"fresh constructor clears only represented B9D bit0");
 {
  const auto cold=std::make_unique<VehicleFrameState49W>(s);
  // Real reader writes B9C's low input bits. Both B9D states must survive;
  // no body phase is fabricated and each case starts from the cold fixture.
  for(unsigned bit=0;bit<2;++bit)for(unsigned byte=0;byte<256;++byte){
   s=*cold;s.fields.applicationByte58=0;s.fields.flagB9D0=bit!=0;
   s.bodies.main.controls.player.flagsB9C=static_cast<std::uint8_t>(byte);
   x.hot([&]{return readVehicleFrame49W(s,x.bindings,read,x.workspace,x.diagnostic);},"B9C reader with independent B9D bit");
   f::require(s.bodies.main.controls.player.flagsB9C==static_cast<std::uint8_t>(byte&0xf8u),"reader rewrites only source B9C input bits");
   f::require(s.fields.flagB9D0==(bit!=0),"B9C reader preserves independent B9D bit0");
   ++b9dReaderCases;
  }
  s=*cold;
 }
 x.hot([&]{return readVehicleFrame49W(s,x.bindings,read,x.workspace,x.diagnostic);},"reader");PostPlayerAt217DE0_49K post{};post.player=s.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=true;
 x.hot([&]{return bindVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,x.workspace,x.diagnostic);},"binding");x.hot([&]{return graphicsVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::Base2176E0BeforeGraphics),x.workspace,x.diagnostic);},"graphics");
 ImpactConsumerInput49Z consumer{};consumer.managerFlags58=2;consumer.profile34=2;consumer.finalDamageThreshold64=1;VehicleImpactEvents49Z consumerEvents{};
 x.hot([&]{return consumeVehicleImpacts49Z(s,x.bindings,consumer,consumerEvents,x.workspace,x.diagnostic);},"consumers");x.hot([&]{return repairVehicleImpacts50A(s,x.bindings,x.workspace,x.diagnostic);},"repair");
 // Explicit fixture owner set with no instantiated800/804/80C/D40 effects.
 ImpactEffectsInput50B owners{};owners.models[0].node=x.bindings.visual.left1BC;owners.models[1].node=x.bindings.visual.right1B8;owners.models[2].node=owners.models[3].node=SceneNodeIndex49P::none;ImpactEffectsWorkspace50B effectWorkspace{};std::size_t count=999;
 x.hot([&]{return effectsVehicleImpacts50O(s,x.bindings,owners,effectWorkspace,nullptr,0,count,x.workspace,x.diagnostic);},"actual absent-owner effects route");f::require(count==0,"no backend events for absent fixture effects");const auto base=s;
 J fixture;std::ifstream file(argv[1]);file>>fixture;unsigned optionalCases=0,decisionCases=0,rejections=0;
 for(const auto&row:fixture["cases"]){s=base;OptionalProbeOwners50P in{row["owner"],row["manager"],{row["ids"][0],row["ids"][1]}};s.bodies.main.components.probe348.field104=row["probe104"];s.bodies.main.components.probe348.field118=row["probe118"];auto bits=row["velocity"].get<std::uint32_t>();std::memcpy(&s.bodies.main.components.root.localVelocity80.v[2],&bits,4);const auto before=s;
  OptionalProbeResult50P result{};std::memset(&result,0xa5,sizeof result);const auto savedResult=result;const auto a=::allocations;
  const bool okay=optionalProbeVehicleImpacts50P(s,x.bindings,in,result,x.workspace,x.diagnostic);f::require(::allocations==a,"optional no allocation");
  if(row["status"]==0){f::require(okay&&s.phase==VehicleFramePhase49W::PostOptionalProbeEffects50P,"source optional complete");auto expected=before;expected.phase=VehicleFramePhase49W::PostOptionalProbeEffects50P;f::words(&s,&expected,sizeof s,"optional preserves gameplay state");f::require(result.count==row["calls"].size(),"source stop count");for(unsigned i=0;i<result.count;++i){f::require(result.events[i].target==row["calls"][i]["target"]&&result.events[i].sourcePC==row["calls"][i]["pc"]&&result.events[i].kind==ImpactEffectKind50B::Stop84,"source stop order/target");}
   const auto done=s;const auto events=result;f::require(!optionalProbeVehicleImpacts50P(s,x.bindings,in,result,x.workspace,x.diagnostic),"optional replay refused");f::words(&s,&done,sizeof s,"replay state");f::words(&result,&events,sizeof result,"replay events");++rejections;
  }else{f::require(!okay&&x.diagnostic.error==VehicleFrameError49W::ExternalCut&&x.diagnostic.sourceStage==0x217a40,"active original path stops before write");f::words(&s,&before,sizeof s,"unsupported optional state");f::words(&result,&savedResult,sizeof result,"unsupported optional output");++rejections;}++optionalCases;
 }
 s=base;OptionalProbeOwners50P absent{};OptionalProbeResult50P optional{};x.hot([&]{return optionalProbeVehicleImpacts50P(s,x.bindings,absent,optional,x.workspace,x.diagnostic);},"genuine no-owner fixture branch");const auto ready=s;
 for(unsigned n=0;n<5;++n){s=ready;std::uint32_t threshold=0x7f800001u;s.fields.flagB9D0=false;
  if(n==1)s.fields.flagB9D0=true;
  if(n>=2){s.fields.damage7D4=1.f;s.fields.impactBA0.v[2]=.2f;threshold=0x3f000000u;}
  if(n==3)s.fields.damage7D0=1.f;if(n==4)threshold=0x7f800001u;
  const auto before=s;ImpactTransitionInput50C input{};input.damage7D0[0]=s.fields.damage7D0;input.damage7D0[1]=s.fields.damage7D4;input.damage7D0[2]=s.fields.damage7D8;input.damage7D0[3]=s.fields.damage7DC;std::memcpy(&input.impactBA8,&s.fields.impactBA0.v[2],4);std::memcpy(&input.threshold50,&threshold,4);input.flagsB9D=static_cast<std::uint32_t>(s.fields.flagB9D0);const auto expected=inspectImpactTransition50C(input);
  ImpactTransitionResult50C result{};const auto a=::allocations;const bool okay=decideVehicleImpacts50P(s,x.bindings,threshold,result,x.workspace,x.diagnostic);f::require(::allocations==a,"decision no allocation");f::words(&result,&expected,sizeof result,"canonical transition inputs");
  if(!n){f::require(okay&&s.phase==VehicleFramePhase49W::AfterImpactBlock,"healthy original no-transition route");auto out=before;out.phase=VehicleFramePhase49W::AfterImpactBlock;f::words(&s,&out,sizeof s,"ordinary decision preserves gameplay");f::require(!refreshMainVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::BaseAfterGraphicsThroughBA0ResetBeforeChildren),x.workspace,x.diagnostic),"remaining base tail not bypassed");++rejections;}
  else{f::require(!okay,"selected handler/arithmetic must stop");f::words(&s,&before,sizeof s,"decision rollback");f::require(x.diagnostic.sourceStage==expected.sourcePC,"first source failure/call");++rejections;}++decisionCases;
 }
 {
  auto expected=std::make_unique<VehicleFrameState49W>(ready);
  // Reuse the genuinely reached post-optional phase. Controlled healthy
  // damage isolates the canonical B9D gate from all 256 independent B9C bytes.
  for(unsigned bit=0;bit<2;++bit)for(unsigned byte=0;byte<256;++byte){
   s=ready;s.fields.damage7D0=s.fields.damage7D4=s.fields.damage7D8=s.fields.damage7DC=0.f;
   s.fields.flagB9D0=bit!=0;s.bodies.main.controls.player.flagsB9C=static_cast<std::uint8_t>(byte);
   *expected=s;ImpactTransitionResult50C result{};const auto a=::allocations;
   const bool okay=decideVehicleImpacts50P(s,x.bindings,0x3f800000u,result,x.workspace,x.diagnostic);
   f::require(::allocations==a,"independent B9D decision no allocation");
   if(bit){
    f::require(!okay&&result.status==ImpactTransitionStatus50C::unsupportedTransition&&result.sourcePC==0x225d10&&result.vtableSlot==0x88&&result.originalTarget==0x2261b0,"canonical B9D always selects unsupported225D10");
    f::require(x.diagnostic.error==VehicleFrameError49W::ExternalCut&&x.diagnostic.sourceStage==0x225d10,"B9D keeps the genuine unsupported boundary");
   }else{
    f::require(okay&&result.status==ImpactTransitionStatus50C::noTransition,"B9C never substitutes for clear B9D bit0");
    expected->phase=VehicleFramePhase49W::AfterImpactBlock;
   }
   f::words(&s,expected.get(),sizeof s,"B9D decision preserves independent flags and canonical state");
   ++b9dTailCases;
  }
 }
 std::cout<<J{{"optional_cases",optionalCases},{"decision_cases",decisionCases},{"b9d_reader_cases",b9dReaderCases},{"b9d_tail_cases",b9dTailCases},{"rejections",rejections},{"checks",f::checks},{"hot_allocations",0}}.dump()<<'\n';return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
