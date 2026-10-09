#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
namespace f=frame49w_test;using namespace rr::game;
static void bindCanonical(ImpactEffectsInput50B&in,const VehicleFrameState49W&s){
 const float d[]={s.fields.damage7D0,s.fields.damage7D4,s.fields.damage7D8,s.fields.damage7DC};std::memcpy(in.damage7D0,d,16);std::memcpy(in.timers820,s.impactSupplement.timers820,20);std::memcpy(in.bodyVelocity70,s.bodies.main.components.root.body.velocity.v,12);std::memcpy(in.bodyVelocity70+3,&s.bodies.mainSphere.velocity70W,4);
}
int main(){try{std::fesetround(FE_TONEAREST);f::Fixture x(4);auto&s=x.state;s.impactSupplement.partnerId=0xffffffffu;
 FrameReader49K read{};read.packet={128,128,128,128};read.preparedFactor=.016f;read.actions={nullptr,f::noAction};read.codes=x.actionCodes;read.context.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;
 x.hot([&]{return readVehicleFrame49W(s,x.bindings,read,x.workspace,x.diagnostic);},"reader");PostPlayerAt217DE0_49K post{};post.player=s.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=true;
 x.hot([&]{return bindVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,x.workspace,x.diagnostic);},"binding");x.hot([&]{return graphicsVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::Base2176E0BeforeGraphics),x.workspace,x.diagnostic);},"graphics");
 ImpactConsumerInput49Z consumer{};consumer.managerFlags58=2;consumer.profile34=2;consumer.finalDamageThreshold64=1;VehicleImpactEvents49Z consumerEvents{};
 x.hot([&]{return consumeVehicleImpacts49Z(s,x.bindings,consumer,consumerEvents,x.workspace,x.diagnostic);},"actual impact consumers");x.hot([&]{return repairVehicleImpacts50A(s,x.bindings,x.workspace,x.diagnostic);},"actual repair");const auto base=s;
 std::uint8_t matches[53]{};
 // Authored PSG child names; wrapper roots are explicitly nonmatching fixture names.
 for(unsigned side=0;side<2;++side)for(unsigned n=0;n<x.assets.hierarchy[side].nodeCount;++n){rr::assets::PsgNode49O node{};f::require(rr::assets::readPsgNode49O(x.assets.hierarchy[side],n,node),"authored node name");bool same=true;const char expected[]="damage00";for(unsigned k=0;k<sizeof expected;++k){unsigned char ch=node.name[k];if(ch>='A'&&ch<='Z')ch=static_cast<unsigned char>(ch+32);same&=ch==static_cast<unsigned char>(expected[k]);}matches[(side?16:1)+n]=same;}
 f::require(matches[5]&&matches[20],"actual engine damage00 anchors");
 ImpactEffectsInput50B owners{};owners.models[0]={x.bindings.visual.left1BC,0,{1,0},{2,0},{3,0}};owners.models[1]={x.bindings.visual.right1B8,0,{4,0},{5,0},{6,0}};owners.models[2].node=owners.models[3].node=SceneNodeIndex49P::none;owners.effectD40=7;owners.damage00Matches=matches;owners.nameCount=53;
 const float poison=std::numeric_limits<float>::quiet_NaN();for(float&v:owners.damage7D0)v=poison;for(float&v:owners.timers820)v=poison;for(float&v:owners.bodyVelocity70)v=poison;
 unsigned cases=0,rejections=0;std::size_t maximumEvents=0;
 for(unsigned n=0;n<6;++n){s=base;s.fields.damage7D0=.1f*n;s.fields.damage7D4=.2f*n;for(unsigned i=0;i<5;++i)s.impactSupplement.timers820[i]=(n&1)?.2f:0.f;s.noise.sharedRandomState=0x12345678+n;
  auto expected=s;const auto before=s;auto input=owners;bindCanonical(input,s);ImpactEffectsWorkspace50B referenceWorkspace{};ImpactEffectEvent50B referenceEvents[41]{};std::size_t expectedCount=0;
  f::require(updateImpactEffects50B({expected.nodes,53},input,expected.noise.sharedRandomState,referenceWorkspace,referenceEvents,41,expectedCount)==ImpactEffectsStatus50B::applied,"source-verified reference leaf");f::project(expected,x.bindings);expected.phase=VehicleFramePhase49W::PostImpactEffects50O;
  ImpactEffectsWorkspace50B work{};ImpactEffectEvent50B events[41]{};std::size_t count=0;
  x.hot([&]{return effectsVehicleImpacts50O(s,x.bindings,owners,work,events,41,count,x.workspace,x.diagnostic);},"canonical effects composition");f::words(&s,&expected,sizeof s,"full frame effects projection");f::require(count==expectedCount,"ordered event count");f::words(events,referenceEvents,count*sizeof(*events),"ordered effect calls and arguments");maximumEvents=std::max(maximumEvents,count);++cases;
  {const auto finished=s;ImpactEffectEvent50B emitted[41];std::memcpy(emitted,events,sizeof events);const auto emittedCount=count;const auto a=::allocations;
   f::require(!effectsVehicleImpacts50O(s,x.bindings,owners,work,events,41,count,x.workspace,x.diagnostic)&&x.diagnostic.error==VehicleFrameError49W::Phase,"effects replay refused");f::words(&s,&finished,sizeof s,"effects replay state");f::words(events,emitted,sizeof events,"effects replay events");f::require(count==emittedCount&&::allocations==a,"no replay count or allocations");
   f::require(!refreshMainVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::BaseAfterGraphicsThroughBA0ResetBeforeChildren),x.workspace,x.diagnostic),"cannot bypass remaining base work");f::words(&s,&finished,sizeof s,"base bypass state rollback");rejections+=2;
  }
  for(unsigned bad=0;bad<6;++bad){s=before;auto invalid=owners;auto bindings=x.bindings;std::size_t capacity=41;
   if(bad==0)capacity=0;if(bad==1)invalid.models[0].node=bindings.visual.right1B8;if(bad==2)s.phase=VehicleFramePhase49W::PostGraphics;if(bad==3)invalid.models[1].effect800.id=invalid.models[0].effect800.id;if(bad==4)invalid.damage00Matches=reinterpret_cast<const std::uint8_t*>(&x.workspace);if(bad==5)bindings.visual.extra1C0Present=true;
   std::memset(events,0xa5,sizeof events);ImpactEffectEvent50B savedEvents[41];std::memcpy(savedEvents,events,sizeof events);count=123;const auto saved=s;const auto allocBefore=::allocations;
   const bool okay=effectsVehicleImpacts50O(s,bindings,invalid,work,events,capacity,count,x.workspace,x.diagnostic);f::require(!okay,"invalid effects connection rejected");f::words(&s,&saved,sizeof s,"effects frame rollback");f::words(events,savedEvents,sizeof events,"effects event rollback");f::require(count==123&&::allocations==allocBefore,"event count and allocations unchanged");++rejections;
  }
 }
 std::cout<<"{\"cases\":"<<cases<<",\"rejections\":"<<rejections<<",\"checks\":"<<f::checks<<",\"maximum_events\":"<<maximumEvents<<",\"hot_allocations\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
