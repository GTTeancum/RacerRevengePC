#include "reset_tree50ak.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <cfenv>
#include <bit>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=3)return 2;std::fesetround(FE_TONEAREST);nlohmann::json j,source;std::ifstream f(argv[1]);f>>j;std::ifstream mf(argv[2]);mf>>source;unsigned cases=0,checks=0,hits=0;
for(const auto&row:j["cases"]){const auto&mj=source["meshes"][row["mesh"].get<unsigned>()];rr::assets::CollisionMesh48I mesh{};for(const auto&v:mj["vertices"])mesh.vertices.push_back({v[0],v[1],v[2]});for(const auto&v:mj["normals"])mesh.vertex_normals.push_back({v[0],v[1],v[2]});for(const auto&n:mj["nodes"]){rr::assets::CollisionMeshNode48I node{};node.volume=n["volume"];for(unsigned i=0;i<3;++i){node.center[i]=n["center"][i];node.half[i]=n["half"][i];}for(unsigned i=0;i<2;++i){auto c=n["children"][i].get<std::uint32_t>();node.children[i]=(c&0x7fff)|((c&0x8000)?rr::assets::collision_mesh_leaf48i:0);}mesh.nodes.push_back(node);}for(const auto&f:mj["faces"]){rr::assets::CollisionMeshFace48I face{};face.d=f["d"];for(unsigned i=0;i<3;++i){face.vertices[i]=f["vertices"][i];face.normal[i]=f["normal"][i];}mesh.faces.push_back(face);}
SpringVector4_48N p{},q{};BodyMatrix48G m{};for(unsigned i=0;i<4;++i){p.v[i]=std::bit_cast<float>(row["start"][i].get<std::uint32_t>());q.v[i]=std::bit_cast<float>(row["end"][i].get<std::uint32_t>());}for(unsigned i=0;i<16;++i)m.v[i]=std::bit_cast<float>(row["matrix"][i].get<std::uint32_t>());std::vector<std::uint32_t> stack(mesh.nodes.size()+mesh.faces.size()+1);ResetTreeResult50AK out{1.f,{{.125f,.25f,.5f,std::bit_cast<float>(0x7fc01234u)}},0};auto count=allocations;auto mode=row["mode"].get<std::uint32_t>();auto status=queryResetMesh50AK(p,q,m,mesh,mode,stack.data(),stack.size(),out);if(status!=ResetTreeStatus50AK::Complete||allocations!=count){std::cerr<<"case "<<cases<<" status "<<int(status)<<'\n';return 1;}if(out.hit!=row["hit"]||std::bit_cast<std::uint32_t>(out.fraction)!=row["fraction"]){std::cerr<<"case "<<cases<<" hit/fraction "<<out.hit<<" "<<std::hex<<std::bit_cast<std::uint32_t>(out.fraction)<<" expected "<<row["fraction"]<<'\n';return 1;}for(unsigned i=0;i<4;++i){if(std::bit_cast<std::uint32_t>(out.normal.v[i])!=row["normal"][i]){std::cerr<<"case "<<cases<<" normal "<<i<<'\n';return 1;}++checks;}checks+=2;hits+=out.hit;++cases;
if(cases==1){auto saved=out;if(queryResetMesh50AK(p,q,m,mesh,mode,stack.data(),1,out)!=ResetTreeStatus50AK::InsufficientCapacity||std::memcmp(&saved,&out,sizeof out))return 1;
if(queryResetMesh50AK(p,q,m,mesh,3,stack.data(),stack.size(),out)!=ResetTreeStatus50AK::UnsupportedScope||std::memcmp(&saved,&out,sizeof out))return 1;
auto child=mesh.nodes[0].children[0];mesh.nodes[0].children[0]=0;if(queryResetMesh50AK(p,q,m,mesh,mode,stack.data(),stack.size(),out)!=ResetTreeStatus50AK::InvalidInput||std::memcmp(&saved,&out,sizeof out))return 1;mesh.nodes[0].children[0]=child;
auto vertex=mesh.faces[0].vertices[0];mesh.faces[0].vertices[0]=65535;if(queryResetMesh50AK(p,q,m,mesh,mode,stack.data(),stack.size(),out)!=ResetTreeStatus50AK::InvalidInput||std::memcmp(&saved,&out,sizeof out))return 1;mesh.faces[0].vertices[0]=vertex;
if(queryResetMesh50AK(p,q,m,mesh,mode,nullptr,0,out)!=ResetTreeStatus50AK::InsufficientCapacity||std::memcmp(&saved,&out,sizeof out))return 1;if(queryResetMesh50AK(p,q,m,mesh,mode,reinterpret_cast<std::uint32_t*>(&out),sizeof out/4,out)!=ResetTreeStatus50AK::InvalidInput||std::memcmp(&saved,&out,sizeof out))return 1;}}
std::cout<<nlohmann::json{{"cases",cases},{"hits",hits},{"word_checks",checks},{"hot_allocations",0},{"result_bytes",sizeof(ResetTreeResult50AK)}}.dump()<<'\n';}
