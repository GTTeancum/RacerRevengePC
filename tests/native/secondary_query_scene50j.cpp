#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#include "../../evidence/sceneProjection49P/source/scene_oracle.h"
#include "vehicle_secondary_contact50j.h"
namespace f=frame49w_test;using namespace rr::game;
int main(){try{
 std::fesetround(FE_TONEAREST);f::Fixture x(20);x.beginFrame({128,128,128,128},.016f,60);
 f::require(beginVehicleSubstep49W(x.state,x.bindings,x.selectors,x.workspace,x.diagnostic),"substep prefix");
 const auto base=x.state;const auto index=x.bindings.lifecycle.physical19C;const auto k=static_cast<unsigned>(index);
 MainStaticDetectorAdmission50H admission{true,true,true,false,false,false,true,true,2,0,index};unsigned cases=0,rejects=0;std::size_t instructions=0;
 for(unsigned dirty=0;dirty<2;++dirty){x.state=base;
  if(dirty){auto local=x.state.nodes[k].localC0;local.v[12]+=3;local.v[13]-=.25f;f::require(setLocalMatrix49P({x.state.nodes,53},index,local),"source local setter");f::project(x.state,x.bindings);}
  const auto before=x.state;auto expected=before;auto original=std::make_unique<scene49p_oracle::Machine>();constexpr std::uint32_t node=0x1000;
  for(unsigned j=0;j<16;++j){original->store(node+0xc0+j*4,before.nodes[k].localC0.v[j]);original->store(node+0x100+j*4,before.nodes[k].world100.v[j]);}
  original->write(node+0x180,0);original->write(node+0x190,0xa5a5a500u|before.nodes[k].sceneFlags190);original->call(0x2897b0,node);instructions+=original->trace.size();
  for(unsigned j=0;j<16;++j){auto word=original->read(node+0x100+j*4);std::memcpy(&expected.nodes[k].world100.v[j],&word,4);}
  expected.nodes[k].sceneFlags190=static_cast<std::uint8_t>(original->read(node+0x190));
  std::memcpy(&expected.bodies.secondary.root.scene,&expected.nodes[k].world100,64);expected.bodies.secondary.sceneFlags190=(expected.bodies.secondary.sceneFlags190&0xffffff00u)|expected.nodes[k].sceneFlags190;
  x.hot([&]{return prepareSecondaryQueryScene50J(x.state,x.bindings,admission,x.workspace)==SecondaryQuerySceneStatus50J::Ready;},"early secondary query getter");
  f::words(&x.state,&expected,sizeof expected,"exact original getter plus canonical projection");++cases;
  for(unsigned bad=0;bad<7;++bad){x.state=before;auto a=admission;
   if(bad==0)x.state.phase=VehicleFramePhase49W::Ready;
   if(bad==1)x.state.bodies.vehicle838=0;
   if(bad==2)x.state.bodies.main.substepIndex=1;
   if(bad==3)a.registrationE0=1;
   if(bad==4){a.providerPresent=true;a.pointContactsEnabled=true;}
   if(bad==5)a.queryScene=x.bindings.lifecycle.main190;
   if(bad==6)x.state.bodies.secondary.root.scene.v[12]+=1;
   const auto saved=x.state;const auto allocationsBefore=::allocations;
   f::require(prepareSecondaryQueryScene50J(x.state,x.bindings,a,x.workspace)!=SecondaryQuerySceneStatus50J::Ready,"invalid preparation rejected");f::words(&x.state,&saved,sizeof saved,"getter rejection atomic");f::require(::allocations==allocationsBefore,"getter rejection no allocation");++rejects;
  }
 }
 std::cout<<"{\"getter_cases\":"<<cases<<",\"rejections\":"<<rejects<<",\"original_words\":"<<instructions<<",\"checks\":"<<f::checks<<",\"hot_allocations\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
