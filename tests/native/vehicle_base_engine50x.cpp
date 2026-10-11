#include "../../src/game/vehicle_base_tail50x.h"
#include "../../src/assets/pod_flame50w.h"
#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
namespace f=frame49w_test;using namespace rr::game;using J=nlohmann::json;
int main(int argc,char**argv){try{if(argc!=3)return 2;std::fesetround(FE_TONEAREST);f::Fixture x(4);auto&s=x.state;s.impactSupplement.partnerId=0xffffffffu;
 
 rr::assets::PodFlameOwners50W coldFlame{};
 {using namespace rr::assets;std::ifstream input("work/native/project/disc/PODS/POD01/POD01.RES",std::ios::binary);std::vector<std::uint8_t> raw((std::istreambuf_iterator<char>(input)),{}),member;ResArchive48C res;DecodeError48C error;Scene48D left,right;Material48E shared,fl,fr;
 f::require(decodeResource48C(raw,res,error),"decode actual Pod01");
 auto scene=[&](const char*name,Scene48D&out){return extractResMember48C(res,name,member,error)&&decodeScene48D(member,out,error);};
 auto material=[&](const char*name,Material48E&out){return extractResMember48C(res,name,member,error)&&decodeMaterial48E(member,out,error);};
 f::require(scene("pods\\pod01\\englt1.psg",left)&&scene("pods\\pod01\\engrt1.psg",right),"actual engine scenes");
 f::require(material("textures\\01_1.psm",shared)&&material("textures\\flame01l.psm",fl)&&material("textures\\flame01r.psm",fr),"actual flame materials");PodFlameError50W diag{};
 f::require(buildPodFlameOwners50W(left,right,shared,fl,fr,static_cast<unsigned>(x.bindings.visual.left1BC)+1,static_cast<unsigned>(x.bindings.visual.right1B8)+1,coldFlame,diag),"cold flame owners");
 }

 FrameReader49K read{};read.packet={128,128,128,128};read.preparedFactor=.016f;read.actions={nullptr,f::noAction};read.codes=x.actionCodes;read.context.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;
 x.hot([&]{return readVehicleFrame49W(s,x.bindings,read,x.workspace,x.diagnostic);},"reader");PostPlayerAt217DE0_49K post{};post.player=s.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=true;
 x.hot([&]{return bindVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,x.workspace,x.diagnostic);},"binding");x.hot([&]{return graphicsVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::Base2176E0BeforeGraphics),x.workspace,x.diagnostic);},"graphics");
 ImpactConsumerInput49Z consumer{};consumer.managerFlags58=2;consumer.profile34=2;consumer.finalDamageThreshold64=1;VehicleImpactEvents49Z consumerEvents{};
 x.hot([&]{return consumeVehicleImpacts49Z(s,x.bindings,consumer,consumerEvents,x.workspace,x.diagnostic);},"consumers");x.hot([&]{return repairVehicleImpacts50A(s,x.bindings,x.workspace,x.diagnostic);},"repair");
 // Explicit fixture owner set with no instantiated800/804/80C/D40 effects.
 ImpactEffectsInput50B owners{};owners.models[0].node=x.bindings.visual.left1BC;owners.models[1].node=x.bindings.visual.right1B8;owners.models[2].node=owners.models[3].node=SceneNodeIndex49P::none;ImpactEffectsWorkspace50B effectWorkspace{};std::size_t count=999;
 x.hot([&]{return effectsVehicleImpacts50O(s,x.bindings,owners,effectWorkspace,nullptr,0,count,x.workspace,x.diagnostic);},"actual absent-owner effects route");f::require(count==0,"no backend events for absent fixture effects");const auto base=s;
 J fixture;std::ifstream file(argv[1]);file>>fixture;unsigned optionalCases=0,decisionCases=0,rejections=0,engineCases=0;
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
  if(!n){f::require(okay&&s.phase==VehicleFramePhase49W::AfterImpactBlock,"healthy original no-transition route");auto out=before;out.phase=VehicleFramePhase49W::AfterImpactBlock;f::words(&s,&out,sizeof s,"ordinary decision preserves gameplay");f::require(!refreshMainVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::BaseAfterGraphicsThroughBA0ResetBeforeChildren),x.workspace,x.diagnostic),"remaining base tail not bypassed");++rejections;
   const auto afterDecision=s;J plainFixture;std::ifstream plainFile(argv[2]);plainFile>>plainFixture;
   for(const auto&row:plainFixture["cases"]){s=afterDecision;PlainEffectOwners50Q owners{};for(unsigned i=0;i<12;++i)owners.slots[i]={row["ids"][i],row["flags"][i]};PlainEffectResult50Q plain{};
    x.hot([&]{return plainEffectsVehicleTail50Q(s,x.bindings,owners,plain,x.workspace,x.diagnostic);},"plain tail");
    auto expected=afterDecision;expected.phase=VehicleFramePhase49W::PostPlainEffects50Q;f::words(&s,&expected,sizeof s,"plain tail preserves canonical state");f::require(plain.count==row["calls"].size(),"plain count");for(unsigned i=0;i<plain.count;++i)f::require(plain.events[i].kind==ImpactEffectKind50B::Stop84&&plain.events[i].sourcePC==row["calls"][i]["pc"]&&plain.events[i].target==row["calls"][i]["target"],"plain original ordered calls");
    // Exercise distinct live vs requested drives and canonical packet inputs.
    s.bodies.main.components.engine.smoothedDrive[0]=.333f;s.bodies.main.components.engine.smoothedDrive[1]=.666f;s.bodies.main.components.bindings.engineDrive[0]=100.f;s.bodies.main.components.bindings.engineDrive[1]=100.f;
    s.bodies.main.components.bindings.aeroDrive[0]=1.f;s.bodies.main.components.bindings.aeroDrive[1]=0.f;s.bodies.main.controls.bound.control.filtered.values[1]=1.f;s.bodies.main.controls.bound.control.filtered.values[3]=1.f;s.bodies.main.controls.player.actionB80=2.f;s.bodies.main.controls.player.flagsB9C=3;
    s.fields.impactBA0={{3,4,0,123}};s.fields.damage7D0=.5f;s.fields.damage7D4=.25f;s.fields.damage7D8=.75f;s.fields.damage7DC=1.f;
    const auto beforePacket=s;const bool configured=plain.count%2!=0;
    PackedVehicleInput50R expectedInput{};expectedInput.drive[0]=.333f;expectedInput.drive[1]=.666f;expectedInput.drive[2]=1.f;expectedInput.drive[3]=0.f;expectedInput.impactBA0[0]=3;expectedInput.impactBA0[1]=4;expectedInput.damage7D0[0]=.5f;expectedInput.damage7D0[1]=.25f;expectedInput.damage7D0[2]=.75f;expectedInput.damage7D0[3]=1.f;expectedInput.filteredB70=expectedInput.filteredB78=1.f;expectedInput.actionB80=2.f;expectedInput.flagsB9C=3;expectedInput.configurationEntryNonzero=configured;
    const auto packet=packVehicleTail50R(expectedInput);f::require(packet.complete,"packet fixture domain");x.hot([&]{return packVehicleFrame50S(s,x.bindings,configured,x.workspace,x.diagnostic);},"canonical packet");auto packedExpected=beforePacket;packedExpected.phase=VehicleFramePhase49W::PostPackedBytes50S;std::memcpy(packedExpected.packedC00,packet.bytesC00,7);f::words(&s,&packedExpected,sizeof s,"canonical packet exact sole owner");f::require(!packVehicleFrame50S(s,x.bindings,configured,x.workspace,x.diagnostic),"packet replay refused");f::words(&s,&packedExpected,sizeof s,"packet replay rollback");
    

    if(!engineCases){const auto packed=s;J original;std::ifstream ef("evidence/baseTail50T/fixtures.json");ef>>original;
     for(const auto&row:original["cases"]){const auto&in=row["input"];if(in["state40"].get<unsigned>()!=0)continue;s=packed;auto flame=coldFlame;auto tail=coldBaseTailState50X(row["initial_thrust"][3],row["initial_thrust"][7]);tail.flagsACC=in["acc"];tail.flagsAD0=in["ad0"];tail.activeAE8=in["activeAE8"];tail.timerAE4=in["timerAE4"];for(unsigned side=0;side<2;++side){tail.countAD4[side]=in["counts"][side];tail.timerADC[side]=in["timers"][side];for(unsigned axis=0;axis<3;++axis){auto bits=row["initial_thrust"][side*4+axis].get<std::uint32_t>();std::memcpy(&s.bodies.main.components.engine.localThrust[side].v[axis],&bits,4);}}
      s.fields.vehicle844=in["field844"];s.fields.applicationByte58=in["application58"];s.noise.sharedRandomState=in["rng"];BaseTailGlobals50X globals{};BaseTailWorkspace50X scratch{};FrameTimingState49I clock{};auto dt=in["dt"].get<std::uint32_t>();std::memcpy(&clock.lastDispatched23C,&dt,4);BaseTailBindings50X binding{77,Probe128Binding50X::Null};
      x.hot([&]{return advanceVehicleBaseThroughEngine50X(s,x.bindings,clock,binding,tail,globals,flame,scratch,x.workspace,x.diagnostic);},"engine original-word integration");const auto&e=row["expected"];
      f::require(tail.flagsACC==e["0xacc"]&&tail.flagsAD0==e["0xad0"]&&tail.activeAE8==e["0xae8"]&&tail.timerAE4==e["0xae4"]&&s.fields.vehicle844==e["0x844"]&&s.noise.sharedRandomState==e["rng"],"original scalar and RNG owners");f::require(tail.countAD4[0]==e["0xad4"]&&tail.countAD4[1]==e["0xad8"]&&tail.timerADC[0]==e["0xadc"]&&tail.timerADC[1]==e["0xae0"],"original counters/timers");
      for(unsigned side=0;side<2;++side){for(unsigned axis=0;axis<3;++axis){std::uint32_t raw;std::memcpy(&raw,&s.bodies.main.components.engine.localThrust[side].v[axis],4);f::require(raw==e["thrust"][side*4+axis],"original thrust XYZ");}f::require(tail.engineThrustW[side]==e["thrust"][side*4+3],"original opaque/written W");}++engineCases;

     }
     for(unsigned bad=0;bad<7;++bad){s=packed;auto flame=coldFlame;auto tail=coldBaseTailState50X(0x7f800001u,0x80000000u);BaseTailGlobals50X globals{};BaseTailWorkspace50X scratch{};FrameTimingState49I clock{};clock.lastDispatched23C=1.f;BaseTailBindings50X binding{77,Probe128Binding50X::Null};
      if(bad==0)binding.vehicleOwnerId=0;
      if(bad==1)flame.models[0].modelId=999;
      if(bad==2)s.bodies.main.controls.player.flagsB9C|=16;
      if(bad==3)s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].localC0.v[13]=-1.f;
      if(bad>=4){tail.activeAE8=1;tail.timerAE4=0x3dcccccdu;if(bad==4){std::uint32_t nan=0x7fc00000u;std::memcpy(&clock.lastDispatched23C,&nan,4);}else binding.probe128=bad==5?Probe128Binding50X::Present:Probe128Binding50X::Unresolved;}
      const auto before=s;const auto bt=tail;const auto bg=globals;const auto bf=flame;auto allocationsBefore=::allocations;
      f::require(!advanceVehicleBaseThroughEngine50X(s,x.bindings,clock,binding,tail,globals,flame,scratch,x.workspace,x.diagnostic),"engine rejection control");f::require(::allocations==allocationsBefore,"engine reject no allocation");f::words(&s,&before,sizeof s,"engine reject frame rollback");f::words(&tail,&bt,sizeof tail,"engine reject supplementary rollback");f::words(&globals,&bg,sizeof globals,"engine reject global rollback");f::words(&flame,&bf,sizeof flame,"engine reject callback rollback");++rejections;
     }s=packed;
    }
    {auto saved=s;auto flame=coldFlame;auto tail=coldBaseTailState50X(0x7f800001u,0x80000000u);auto beforeTail=tail;BaseTailGlobals50X globals{};BaseTailWorkspace50X scratch{};FrameTimingState49I clock{};clock.lastDispatched23C=.016f;BaseTailBindings50X binding{77,Probe128Binding50X::Unresolved};
     x.hot([&]{return advanceVehicleBaseThroughEngine50X(s,x.bindings,clock,binding,tail,globals,flame,scratch,x.workspace,x.diagnostic);},"base engine connection");
     f::require(s.phase==VehicleFramePhase49W::PostEngineBase50X,"engine phase");f::require((s.bodies.main.controls.player.flagsB9C&16)!=0,"real flame binding flag");
     f::require(flame.records[2].words[0]==0x20f110&&flame.records[3].words[0]==0x20ee90&&flame.records[2].words[1]==77&&flame.records[3].words[1]==77,"real callback owners");
     f::require(globals.initialized434810&&globals.value434810==0xffffffffu,"lazy global init");f::require(tail.engineThrustW[0]==0x3f800000u&&tail.engineThrustW[1]==0x3f800000u,"original inactive thrust writes W one");
     const auto done=s;const auto doneTail=tail;const auto doneFlame=flame;f::require(!advanceVehicleBaseThroughEngine50X(s,x.bindings,clock,binding,tail,globals,flame,scratch,x.workspace,x.diagnostic),"engine replay refused");f::words(&s,&done,sizeof s,"engine replay frame rollback");f::words(&tail,&doneTail,sizeof tail,"engine replay supplement rollback");f::words(&flame,&doneFlame,sizeof flame,"engine replay materials rollback");s=saved;
    }

    const auto done=s;const auto output=plain;f::require(!plainEffectsVehicleTail50Q(s,x.bindings,owners,plain,x.workspace,x.diagnostic),"plain replay rejected");f::words(&s,&done,sizeof s,"plain replay rollback");f::words(&plain,&output,sizeof plain,"plain output rollback");
   }
   for(unsigned reject=0;reject<2;++reject){s=afterDecision;PlainEffectOwners50Q owners{};if(reject)owners.slots[0]=owners.slots[11]={1,2};else owners.attachment334=1;PlainEffectResult50Q plain{};std::memset(&plain,0xa5,sizeof plain);const auto saved=plain;
    f::require(!plainEffectsVehicleTail50Q(s,x.bindings,owners,plain,x.workspace,x.diagnostic),"plain unsupported/alias");f::words(&s,&afterDecision,sizeof s,"plain rejection state");f::words(&plain,&saved,sizeof plain,"plain rejection output");++rejections;
   }
  }
  else{f::require(!okay,"selected handler/arithmetic must stop");f::words(&s,&before,sizeof s,"decision rollback");f::require(x.diagnostic.sourceStage==expected.sourcePC,"first source failure/call");++rejections;}++decisionCases;
 }
 std::cout<<J{{"engine_original_cases",engineCases},{"plain_cases",37},{"optional_cases",optionalCases},{"decision_cases",decisionCases},{"rejections",rejections},{"checks",f::checks},{"hot_allocations",0}}.dump()<<'\n';return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
