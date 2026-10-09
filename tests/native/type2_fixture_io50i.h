#pragma once
// Cold diagnostic fixture readers; geometry is the verified complete source data.
#include "../../src/game/static_type2_producer49x.h"
#include "json.hpp"
#include <vector>
#include <cstring>
#include <stdexcept>
namespace cold50i {
using namespace rr::game;using Json=nlohmann::json;
inline void check(bool good,const std::string& message){if(!good)throw std::runtime_error(message);}
std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
float fp(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
void words(void* p,const Json& row){for(std::size_t i=0;i<row.size();++i){const auto u=row[i].get<std::uint32_t>();std::memcpy(static_cast<char*>(p)+i*4,&u,4);}}
void compareWords(const void* p,const Json& row,const std::string& name){for(std::size_t i=0;i<row.size();++i){std::uint32_t u;std::memcpy(&u,static_cast<const char*>(p)+i*4,4);const auto want=row[i].get<std::uint32_t>();check(u==want,name+" word"+std::to_string(i)+" got"+std::to_string(u)+" want"+std::to_string(want));}}
BodyMatrix48G matrix(const Json& j){BodyMatrix48G m{};for(unsigned i=0;i<16;++i)m.v[i]=j[i].get<float>();return m;}
CollisionSphere48J sphere(const Json& j){return {{{j[0].get<float>(),j[1].get<float>(),j[2].get<float>()}},j[3].get<float>()};}
CollisionFilter48J filter(const Json& j){CollisionFilter48J f;f.identity=j.at("identity");f.categoryA4=j.at("category");f.acceptMaskB8=j.at("accept_mask");f.flagsF8=j.at("flags");f.flags31=j.at("flags31");f.categoryModeD4=j.at("category_mode");f.categoryMasks=j.at("category_masks").get<std::vector<std::uint32_t>>();for(unsigned i=0;i<2;++i){f.identityLists[i].mode=j["identity_lists"][i]["mode"];f.identityLists[i].identities=j["identity_lists"][i]["ids"].get<std::vector<std::uint32_t>>();}return f;}
struct Shape {std::vector<Type2Node49X> nodes;std::vector<Type2Ray49X> rays;Type2ResourceView49X view;};
std::size_t depth(const std::vector<Type2Node49X>& nodes,std::uint16_t tag){if(tag&0x8000)return 0;return 1+std::max(depth(nodes,nodes.at(tag).children[0]),depth(nodes,nodes.at(tag).children[1]));}
Shape shape(const Json& j){Shape s;for(const auto& n:j["nodes"]){Type2Node49X x;x.volume=n["volume"];x.radius=n["radius"];x.center=n["center"].get<std::array<float,3>>();x.half=n["half"].get<std::array<float,3>>();x.children[0]=n["children"][0];x.children[1]=n["children"][1];s.nodes.push_back(x);}s.rays.resize(j["rays"].size());for(std::size_t i=0;i<s.rays.size();++i)words(&s.rays[i],j["rays"][i]);s.view.localMatrix.v[0]=s.view.localMatrix.v[5]=s.view.localMatrix.v[10]=s.view.localMatrix.v[15]=1.f;s.view.nodes=s.nodes.data();s.view.nodeCount=s.nodes.size();s.view.rays=s.rays.data();s.view.rayCount=s.rays.size();s.view.root=j["root"];s.view.maxDepth=depth(s.nodes,s.view.root);return s;}
struct Mesh {rr::assets::CollisionMesh48I mesh;std::vector<float> radii;std::size_t maxDepth=0;};
Mesh mesh(const Json& j){Mesh m;for(unsigned i:{0u,5u,10u,15u})m.mesh.local_matrix[i]=1.f;m.mesh.vertices=j["vertices"].get<std::vector<std::array<float,3>>>();m.mesh.vertex_normals=j["normals"].get<std::vector<std::array<float,3>>>();for(const auto& n:j["nodes"]){rr::assets::CollisionMeshNode48I x;x.volume=n["volume"];x.center=n["center"].get<std::array<float,3>>();x.half=n["half"].get<std::array<float,3>>();for(unsigned k=0;k<2;++k){auto tag=n["children"][k].get<std::uint32_t>();x.children[k]=(tag&0x7fff)|((tag&0x8000)?rr::assets::collision_mesh_leaf48i:0);}m.mesh.nodes.push_back(x);}for(const auto& f:j["faces"]){rr::assets::CollisionMeshFace48I x;x.d=f["d"];x.normal=f["normal"].get<std::array<std::int16_t,3>>();x.vertices=f["vertices"].get<std::array<std::uint16_t,3>>();m.mesh.faces.push_back(x);}m.radii.resize(m.mesh.nodes.size());std::vector<std::uint32_t> topology(m.mesh.nodes.size()+m.mesh.faces.size());check(prepareStaticTargetRadii49X(m.mesh,m.radii.data(),m.radii.size(),{topology.data(),topology.size()},m.maxDepth)==Type2ResourceStatus49X::Complete,"prepare mesh");for(std::size_t i=0;i<m.radii.size();++i)check(bits(m.radii[i])==bits(j["nodes"][i]["radius"].get<float>()),"original target radius");return m;}
std::string unhex(const std::string& hex){std::string out;for(std::size_t i=0;i<hex.size();i+=2)out.push_back(static_cast<char>(std::stoul(hex.substr(i,2),nullptr,16)));return out;}
}
