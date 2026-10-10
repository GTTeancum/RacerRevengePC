#include "pod_cables50fd_fixture.h"
#include <iostream>
using namespace rr::game;
int main(){try{
 namespace a=rr::assets;namespace p=pod_cables50fd_fixture;
 const char* keys[]={"cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5","210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3","f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60"};
 const unsigned bases[]={31,1,16};const unsigned roots[]={30,0,15};
 std::vector<std::uint8_t> bytes[3];std::vector<SceneNodeIndex49P> mappings[3];std::vector<std::uint8_t> palettes[3];
 a::VehicleDrawAssets50CR assets;a::VehicleDrawAssetsInput50CR input{};a::VehicleDrawAssetDiagnostic50CR assetDiag{};
 VehicleFrameState49W state{};VehicleFrameBindings49W bindings{};
 for(auto&n:state.nodes){for(unsigned i=0;i<4;++i)n.localC0.v[i*5]=n.world100.v[i*5]=1;n.parent180=n.firstChild184=n.nextSibling188=SceneNodeIndex49P::none;n.sceneFlags190=0x80;}
 state.phase=VehicleFramePhase49W::Ready;
 for(unsigned component=0;component<3;++component){
  const std::string path=std::string("evidence/visual49G/cacheobjects/")+keys[component]+".rrscene";
  std::ifstream f(path,std::ios::binary);p::detail::need(bool(f),"retained actual scene read");
  bytes[component]=std::vector<std::uint8_t>((std::istreambuf_iterator<char>(f)),{});
  a::Scene48D scene;std::string error;p::detail::need(a::parseSceneAsset48D(bytes[component],scene,error),"retained actual scene parse");
  for(unsigned j=0;j<scene.nodes.size();++j){
   const unsigned index=bases[component]+j;mappings[component].push_back(SceneNodeIndex49P(index));palettes[component].push_back(j);
   auto&node=state.nodes[index];for(unsigned k=0;k<16;++k)node.localC0.v[k]=static_cast<float>(scene.nodes[j].local_matrix[k]);
   const unsigned parent=scene.nodes[j].parent<0?roots[component]:bases[component]+scene.nodes[j].parent;
   node.parent180=SceneNodeIndex49P(parent);
   auto&first=state.nodes[parent].firstChild184;if(first==SceneNodeIndex49P::none)first=SceneNodeIndex49P(index);
   else{auto n=first;while(state.nodes[static_cast<unsigned>(n)].nextSibling188!=SceneNodeIndex49P::none)n=state.nodes[static_cast<unsigned>(n)].nextSibling188;state.nodes[static_cast<unsigned>(n)].nextSibling188=SceneNodeIndex49P(index);}
  }
  input.component[component]={&bytes[component],mappings[component].data(),static_cast<std::uint32_t>(mappings[component].size()),palettes[component].data(),static_cast<std::uint32_t>(palettes[component].size())};
 }
 p::detail::need(assets.load(input,assetDiag),"actual cold draw load");
 bindings.lifecycle.model1B4=SceneNodeIndex49P(30);bindings.lifecycle.left1BC=SceneNodeIndex49P(0);bindings.lifecycle.right1B8=SceneNodeIndex49P(15);
 bindings.lifecycle.leftCable1E0=SceneNodeIndex49P(35);bindings.lifecycle.rightCable1E4=SceneNodeIndex49P(36);
 p::Owner owner;p::Diagnostic d;
 p::detail::need(owner.load(assets,bindings,state,d),d.message.empty()?"cable actual cold owner load":d.message);
 p::detail::need(owner.bindings().chariot[0]==SceneNodeIndex49P(35)&&owner.bindings().chariot[1]==SceneNodeIndex49P(36)&&owner.bindings().engine[0]==SceneNodeIndex49P(8)&&owner.bindings().engine[1]==SceneNodeIndex49P(23),"actual cable endpoint resolution");
 p::detail::need(d.width==64&&d.height==64&&owner.bank()->textureCount()==1&&owner.bank()->slotCount()==1,"actual cable texture bank");
 const auto&texture=owner.bank()->texture(0)->texture;
 p::detail::need(p::detail::sha256(texture.rgba)=="83c16fad7b74a3639226111f69777d9e334c0083bda52615a0e7164761dcd546","actual cable exact RGBA hash");
 PodCableMesh50FD mesh;PodCableWorkspace50FD workspace;PodCableDiagnostic50FD diagnostic;
 const auto before=state;
 p::detail::need(buildPodCables50FD(state,bindings,owner.bindings(),{{.2f,-.25f,1.f}},mesh,workspace,diagnostic),"actual retained graph cable generation");
 p::detail::need(!std::memcmp(&before,&state,sizeof state),"actual retained graph stays unchanged");
 std::cout<<"PASS: actual RRSCENE endpoint35/36 to8/23, original64x64 cable texture, exact16384RGBA bytes, one-texture50EA bank, current-graph native cable generation\n";
}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
