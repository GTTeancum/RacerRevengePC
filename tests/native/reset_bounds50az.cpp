#define main retained_model_bounds49o_main
#include "model_bounds49o.cpp"
#undef main
#include "json.hpp"
int main(){try{
 require(std::fesetround(FE_TONEAREST)==0,"rounding");
 std::ifstream in("evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg",std::ios::binary);std::vector<char>b((std::istreambuf_iterator<char>(in)),{});require(b.size()>12+18*224,"PSG fixture missing");auto rd=[&](unsigned offset){U w;std::memcpy(&w,b.data()+offset,4);return w;};require(rd(8)==18,"PSG node count");N nodes[18]{};
 for(unsigned i=0;i<18;++i){unsigned at=12+224*i;auto&n=nodes[i];for(unsigned j=0;j<16;++j)n.local.v[j]=fp(bounds49o_fixture::liveLocal[i][j]);for(unsigned j=0;j<3;++j){n.center140.v[j]=fp(rd(at+192+j*4));n.halfExtent150.v[j]=fp(rd(at+204+j*4));}n.parent=static_cast<std::int32_t>(rd(at+220));n.sceneFlags190=0xa4;}
 nlohmann::json poses;std::ifstream poseFile("evidence/gridCenter50AC/pose-fixtures.json");poseFile>>poses;
 for(const auto&row:poses["cases"]){ModelBoundsContext49O context{};for(unsigned j=0;j<16;++j)context.rootWorld100.v[j]=fp(row["matrix"][j]);context.rootSceneFlags190=0x80;fixture(nodes,18,context);}
 std::cout<<"PASS reset model bounds50AZ: "<<cases<<" cases, "<<checks<<" checks, "<<instructions<<" original instructions, zero hot allocations\n";
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
