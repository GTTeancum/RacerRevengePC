#include "../../src/game/pod_cables50fd.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <stdexcept>
using namespace rr::game;
namespace {
using I=SceneNodeIndex49P;
void need(bool b,const char* m){if(!b)throw std::runtime_error(m);}
bool close(float a,float b){return std::fabs(a-b)<.00002f;}
BodyMatrix48G identity(){BodyMatrix48G m{};for(unsigned i=0;i<4;++i)m.v[5*i]=1.f;return m;}
struct Fixture {
 VehicleFrameState49W state{};VehicleFrameBindings49W bindings{};
 PodCableBindings50FD cables{{I(35),I(36)},{I(8),I(23)}};
 Fixture(){
  for(auto& n:state.nodes){n.localC0=n.world100=identity();n.parent180=n.firstChild184=n.nextSibling188=I::none;n.sceneFlags190=0x80;}
  state.phase=VehicleFramePhase49W::Ready;
  bindings.lifecycle.model1B4=I(30);bindings.lifecycle.left1BC=I(0);bindings.lifecycle.right1B8=I(15);
  bindings.lifecycle.leftCable1E0=I(35);bindings.lifecycle.rightCable1E4=I(36);
  state.nodes[0].firstChild184=I(8);state.nodes[8].parent180=I(0);
  state.nodes[15].firstChild184=I(23);state.nodes[23].parent180=I(15);
  state.nodes[30].firstChild184=I(35);state.nodes[35].parent180=I(30);
  state.nodes[35].nextSibling188=I(36);state.nodes[36].parent180=I(30);
  state.nodes[0].localC0.v[12]=-4;state.nodes[15].localC0.v[12]=4;
  state.nodes[30].localC0.v[14]=-12;
  // Actual local Pod01 attachment translations. Root placement here is an
  // explicit unit-test arrangement, not an image or live-game placement.
  const float p[4][3]={{-2.421242f,-.048615f,5.083138f},{2.471886f,-.048615f,5.083138f},
    {.054054f,1.874547f,-2.578372f},{-.703685f,1.409562f,-2.578372f}};
  const unsigned ix[4]={35,36,8,23};
  for(unsigned i=0;i<4;++i)for(unsigned c=0;c<3;++c)state.nodes[ix[i]].localC0.v[12+c]=p[i][c];
 }
};
}
int main(){try{
 Fixture x;PodCableWorkspace50FD work{};PodCableMesh50FD mesh{},old{};PodCableDiagnostic50FD d{};
 const Vector3_48C camera{{.2f,-.25f,1.f}};
 const auto state=x.state;
 need(buildPodCables50FD(x.state,x.bindings,x.cables,camera,mesh,work,d),"initial source cable build");
 need(!std::memcmp(&state,&x.state,sizeof state),"canonical frame unchanged");
 need(close(mesh.vertices[0].position[0],-3.945946f)&&close(mesh.vertices[0].position[2],-2.578372f),"left actual engine endpoint");
 need(close(mesh.vertices[16].position[0],-2.421242f)&&close(mesh.vertices[16].position[2],-6.916862f),"left actual chariot endpoint");
 need(close(mesh.vertices[18].position[0],3.296315f)&&close(mesh.vertices[34].position[0],2.471886f),"right actual endpoints");
 for(unsigned side=0;side<2;++side)for(unsigned j=0;j<9;++j){
  const auto&a=mesh.vertices[side*18+j*2];const auto&b=mesh.vertices[side*18+j*2+1];
  float len=0;for(unsigned c=0;c<3;++c){const auto k=b.position[c]-a.position[c];len+=k*k;}
  need(close(len,.0625f),"source ribbon width0.25");
  need(a.uv[0]==0&&b.uv[0]==1&&a.uv[1]==j*1.25f&&a.uv[1]==b.uv[1],"source repeat10 UVs");
 }
 for(auto i:mesh.indices)need(i<36,"bounded triangle index");
 need(mesh.indices[0]==0&&mesh.indices[1]==1&&mesh.indices[2]==2&&mesh.indices[3]==2&&mesh.indices[4]==1&&mesh.indices[5]==3,"source strip winding");
 old=mesh;
 auto changed=x.state.nodes[0].localC0;changed.v[12]+=.375f;
 need(setLocalMatrix49P({x.state.nodes,53},I(0),changed),"engine movement");
 need(buildPodCables50FD(x.state,x.bindings,x.cables,camera,mesh,work,d),"moved engine cable");
 need(close(mesh.vertices[0].position[0]-old.vertices[0].position[0],.375f),"live engine endpoint follows");
 need(close(mesh.vertices[16].position[0],old.vertices[16].position[0]),"chariot endpoint stays attached");
 need(close(mesh.vertices[8].position[0]-old.vertices[8].position[0],.1875f),"source midpoint Hermite endpoint weight");
 old=mesh;x.state.bodies.main.cachedSpeed34=400.f;
 need(buildPodCables50FD(x.state,x.bindings,x.cables,camera,mesh,work,d),"source high-speed tangent flattening");
 need(mesh.vertices[8].position[1]<old.vertices[8].position[1],"speed changes source bow");
 old=mesh;x.state.bodies.main.vehicleState40=1;
 need(!buildPodCables50FD(x.state,x.bindings,x.cables,camera,mesh,work,d)&&d.error==PodCableError50FD::unsupportedState&&!std::memcmp(&mesh,&old,sizeof mesh),"unsupported detached state transactional");
 x.state.bodies.main.vehicleState40=0;x.cables.engine[0]=I(0);
 need(!buildPodCables50FD(x.state,x.bindings,x.cables,camera,mesh,work,d)&&d.error==PodCableError50FD::binding&&!std::memcmp(&mesh,&old,sizeof mesh),"model-root endpoint rejected");
 x.cables.engine[0]=I(8);x.state.bodies.main.cachedSpeed34=std::numeric_limits<float>::infinity();
 need(!buildPodCables50FD(x.state,x.bindings,x.cables,camera,mesh,work,d)&&!std::memcmp(&mesh,&old,sizeof mesh),"nonfinite speed transactional");
 std::puts("PASS: source 9-sample Hermite, current attachment endpoints, 0.25 ribbon width, repeat10 UVs, 96 bounded indices, speed bow, scene preservation and transactional rejection");
}catch(const std::exception&e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
