#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#include "../../evidence/movementHitPoint50M/extended_oracle.h"
namespace f=frame49w_test;using namespace rr::game;
using O=movement50m_original::Machine;using U=std::uint32_t;
static U ptr(SceneNodeIndex49P i){return i==SceneNodeIndex49P::none?0:0x80000+static_cast<U>(i)*0x200;}
static std::unique_ptr<O> original(const VehicleFrameState49W&s,const VehicleFrameBindings49W&bindings,const MovementQueryCompletion50M&query){
 auto p=std::make_unique<O>();auto&m=*p;constexpr U vehicle=0x10000,body=0x20000,q=0x30000,result=0x31000,probe=0x40000,engine=0x41000,aero=0x42000,contact=0x43000;
 for(auto a:{vehicle,body,q,result,probe,engine,aero,contact})m.zero(a,0x1000);
 for(U i=0;i<53;++i){U a=ptr(static_cast<SceneNodeIndex49P>(i));m.zero(a,0x200);const auto&n=s.nodes[i];for(unsigned j=0;j<16;++j){m.putf(a+0xc0+j*4,n.localC0.v[j]);m.putf(a+0x100+j*4,n.world100.v[j]);}m.put(a+0x180,ptr(n.parent180));m.put(a+0x184,ptr(n.firstChild184));m.put(a+0x188,ptr(n.nextSibling188));m.put(a+0x190,n.sceneFlags190);}
 const U scene=ptr(bindings.lifecycle.main190);m.g[28][0]=0x100000;m.put(0xf8c34,0);m.putf(0x434710,query.pushDistance434710);m.put(vehicle+0x340,body);m.put(body+0x288,scene);m.put(q+0x90,result);
 m.put(vehicle+0x190,scene);m.put(vehicle+0x348,probe);m.put(vehicle+0x34c,engine);m.put(vehicle+0x350,aero);m.put(vehicle+0x354,contact);m.put(vehicle+0x694,0x50000);
 m.put(probe+0x104,s.bodies.main.components.probe348.field104);m.put(contact+0x3c,s.bodies.main.response354_3C);m.put(contact+0x44,s.bodies.main.response354_44);
 for(U i=0;i<4;++i){m.putf(vehicle+0x7b0+i*4,s.bodies.main.priorPoint7B0[i]);m.putf(vehicle+0x7c0+i*4,s.bodies.main.currentPoint7C0[i]);m.putf(result+0x30+i*4,query.selectedPoint.v[i]);m.putf(vehicle+0xba0+i*4,s.fields.impactBA0.v[i]);}
 m.init(query.resultFlagsF8&32?0x219230:0x219400);m.g[16][0]=vehicle;m.g[3][0]=q;m.put(m.stack+0x10,O::stop);m.put(m.stack,0);
 m.runUntil();m.init(0x219ea4);m.g[19][0]=vehicle;m.v[9][3]=-19;m.runUntil({0x11ff90});return p;
}
int main(){try{std::fesetround(FE_TONEAREST);f::Fixture x(200);x.beginFrame({128,128,128,128},.016f,60);auto&s=x.state;
 f::require(!s.bodies.vehicle838,"main-only controlled tail fixture");f::require(beginVehicleSubstep49W(s,x.bindings,x.selectors,x.workspace,x.diagnostic),"substep");PostWorld49W world{};world.point=WorldCut49W::First358And219420;world.effects=contactEffects49W(s);world.applicationGateByte58=2;
 f::require(produceVehicleForces49W(s,x.bindings,x.parameters,world,x.workspace,x.diagnostic),"forces");Post354_49W post{};post.effects=contactEffects49W(s);post.vehicle838=0;f::require(respondVehicle354_49W(s,x.bindings,x.parameters,post,x.secondaryInputs,x.workspace,x.diagnostic),"response");f::require(integrateVehicleMain49W(s,x.bindings,x.parameters,x.workspace,x.diagnostic),"integration");const auto base=s;unsigned cases=0,rejections=0;
 for(unsigned n=0;n<8;++n){s=base;MovementQueryCompletion50M query{};query.resultFlagsF8=n?0x26:0x06;query.representedStatePreserved=query.omittedBB0WNotConsumed=true;query.pushDistance434710=n&1?10.f:2.5f;for(unsigned i=0;i<3;++i)query.selectedPoint.v[i]=s.bodies.main.currentPoint7C0[i]+float(int(n)+int(i)-2);query.selectedPoint.v[3]=movement50m_original::fp(0x7f800001);
 auto ref=original(s,x.bindings,query);x.hot([&]{return finishVehicleMovementQuery50M(s,x.bindings,x.parameters,query,x.workspace,x.diagnostic);},"source-backed movement tail");
 for(unsigned i=0;i<53;++i){U a=ptr(static_cast<SceneNodeIndex49P>(i));for(unsigned j=0;j<16;++j){f::require(movement50m_original::bits(s.nodes[i].localC0.v[j])==ref->read(a+0xc0+j*4),"source local graph");f::require(movement50m_original::bits(s.nodes[i].world100.v[j])==ref->read(a+0x100+j*4),"source world graph");}f::require(s.nodes[i].sceneFlags190==ref->read(a+0x190),"source scene flags");}
 for(unsigned i=0;i<4;++i){f::require(movement50m_original::bits(s.bodies.main.priorPoint7B0[i])==ref->read(0x107b0+i*4),"source prior point");f::require(movement50m_original::bits(s.bodies.main.currentPoint7C0[i])==ref->read(0x107c0+i*4),"source current unchanged");}
 for(unsigned i=0;i<3;++i)f::require(movement50m_original::bits(s.fields.inverseImpactBB0XYZ.v[i])==ref->read(0x10bb0+i*4),"source inverse impact");
 f::words(&s.bodies.main.components.root.body,&base.bodies.main.components.root.body,sizeof s.bodies.main.components.root.body,"hit updates scene, not body histories");f::require(s.bodies.main.vehicle380==ref->read(0x10380),"source380");++cases;
  for(unsigned bad=0;bad<6;++bad){if(!n&&bad==0)continue;s=base;auto broken=x.parameters;auto local=query;
   if(bad==0)local.pushDistance434710=std::numeric_limits<float>::quiet_NaN();if(bad==1)local.representedStatePreserved=false;if(bad==2)local.omittedBB0WNotConsumed=false;
   if(bad==3)broken.shape.local40.v[0]=std::numeric_limits<float>::quiet_NaN();if(bad==4)s.bodies.main.domain.sceneObserverCount=1;if(bad==5)s.phase=VehicleFramePhase49W::Ready;
   const auto saved=s;const auto beforeAlloc=::allocations;
   const bool okay=finishVehicleMovementQuery50M(s,x.bindings,broken,local,x.workspace,x.diagnostic);
   f::require(!okay,"bad tail rejected");f::words(&s,&saved,sizeof s,"entire frame rollback");f::require(::allocations==beforeAlloc,"rejection no allocations");
   if(bad==3){f::require(x.diagnostic.error==VehicleFrameError49W::Shape,"late shape failure reached");if(n)f::require(std::memcmp(&x.workspace.next.nodes[50].localC0,&base.nodes[50].localC0,sizeof(BodyMatrix48G))!=0,"staged scene mutation not committed");}++rejections;
  }
 }
 std::cout<<"{\"cases\":"<<cases<<",\"rejections\":"<<rejections<<",\"checks\":"<<f::checks<<",\"hot_allocations\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
