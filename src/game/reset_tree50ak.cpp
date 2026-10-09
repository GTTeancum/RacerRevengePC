#include "reset_tree50ak.h"
#include <cmath>
#include <limits>
#include <cstdint>
namespace rr::game {namespace {
float add(float a,float b)noexcept{volatile float x=a+b;return x;}
float sub(float a,float b)noexcept{volatile float x=a-b;return x;}
float mul(float a,float b)noexcept{volatile float x=a*b;return x;}
float div(float a,float b)noexcept{volatile float x=a/b;return x;}
struct Span{std::uintptr_t a=0,b=0;};
bool span(const void*p,std::size_t n,std::size_t size,Span&s)noexcept{if(!n){s={};return true;}if(!p||n>std::numeric_limits<std::size_t>::max()/size)return false;s.a=reinterpret_cast<std::uintptr_t>(p);const auto bytes=n*size;if(bytes>std::numeric_limits<std::uintptr_t>::max()-s.a)return false;s.b=s.a+bytes;return true;}
bool overlap(Span a,Span b)noexcept{return a.a<b.b&&b.a<a.b;}
}
ResetTreeStatus50AK queryResetMesh50AK(const SpringVector4_48N&start,const SpringVector4_48N&end,const BodyMatrix48G&m,const assets::CollisionMesh48I&mesh,std::uint32_t mode,std::uint32_t*stack,std::size_t cap,ResetTreeResult50AK&out)noexcept{
 using S=ResetTreeStatus50AK;Span ws,os;if(!span(stack,cap,sizeof(*stack),ws)||!span(&out,1,sizeof out,os)||overlap(ws,os)||(cap&&reinterpret_cast<std::uintptr_t>(stack)%alignof(std::uint32_t)))return S::InvalidInput;
 auto ro=[&](const void*p,std::size_t n,std::size_t bytes){Span x;return span(p,n,bytes,x)&&!overlap(x,ws)&&!overlap(x,os);};
 if(!ro(&start,1,sizeof start)||!ro(&end,1,sizeof end)||!ro(&m,1,sizeof m)||!ro(&mesh,1,sizeof mesh)||!ro(mesh.nodes.data(),mesh.nodes.size(),sizeof(mesh.nodes[0]))||!ro(mesh.faces.data(),mesh.faces.size(),sizeof(mesh.faces[0]))||!ro(mesh.vertices.data(),mesh.vertices.size(),sizeof(mesh.vertices[0]))||!ro(mesh.vertex_normals.data(),mesh.vertex_normals.size(),sizeof(mesh.vertex_normals[0])))return S::InvalidInput;
 if(mesh.reverse_mode||mode>1)return S::UnsupportedScope;
 if(mesh.nodes.empty()||mesh.faces.empty()||mesh.vertices.empty()||mesh.nodes.size()>32768||mesh.faces.size()>32768||(!mesh.vertex_normals.empty()&&mesh.vertex_normals.size()!=mesh.vertices.size())||!std::isfinite(out.fraction))return S::InvalidInput;
 Type3Segment48P ray{};float delta[3];
 for(unsigned i=0;i<3;++i){if(!std::isfinite(start.v[i])||!std::isfinite(end.v[i]))return S::InvalidInput;for(unsigned j=0;j<4;++j)if(!std::isfinite(m.v[j*4+i]))return S::InvalidInput;
 auto transform=[&](const SpringVector4_48N&p){return add(add(add(mul(m.v[i],p.v[0]),mul(m.v[4+i],p.v[1])),mul(m.v[8+i],p.v[2])),m.v[12+i]);};ray.start.v[i]=transform(start);delta[i]=sub(transform(end),ray.start.v[i]);}
 ray.start.v[3]=ray.direction.v[3]=1.f;const float squared=add(add(mul(delta[0],delta[0]),mul(delta[1],delta[1])),mul(delta[2],delta[2]));if(!std::isfinite(squared))return S::InvalidInput;
 auto result=out;result.hit=0;if(squared==0.f){out=result;return S::Complete;}
 volatile float reciprocalRoot=static_cast<float>(1.0/std::sqrt(static_cast<double>(squared)));ray.length=div(1.f,reciprocalRoot);const float reciprocal=div(1.f,ray.length);for(unsigned i=0;i<3;++i)ray.direction.v[i]=mul(delta[i],reciprocal);
 if(!cap)return S::InsufficientCapacity;std::size_t used=1,visits=0;stack[0]=0;const auto limit=mesh.nodes.size()+mesh.faces.size();constexpr auto leaf=assets::collision_mesh_leaf48i;
 while(used){if(++visits>limit)return S::InvalidInput;auto tag=stack[--used];if(tag&~(leaf|0x7fffu))return S::InvalidInput;auto index=tag&0x7fffu;
 if(tag&leaf){if(index>=mesh.faces.size())return S::InvalidInput;const auto&face=mesh.faces[index];Type3Triangle48P t{};t.hasVertexNormals=!mesh.vertex_normals.empty();t.normalMode=mode;for(unsigned k=0;k<3;++k){auto v=face.vertices[k];if(v>=mesh.vertices.size())return S::InvalidInput;for(unsigned i=0;i<3;++i){t.vertices[k].v[i]=mesh.vertices[v][i];if(t.hasVertexNormals)t.vertexNormals[k].v[i]=mesh.vertex_normals[v][i];}}
 ResetLeafResult50AH hit{};auto status=intersectResetTriangle50AI(ray,t,hit);if(status==Type3LeafStatus48P::InvalidInput)return S::InvalidInput;if(status==Type3LeafStatus48P::UnsupportedNormalMode)return S::UnsupportedScope;if(status==Type3LeafStatus48P::UnsupportedInterpolationBoundary)return S::UnsupportedInterpolationBoundary;
 if(status==Type3LeafStatus48P::Hit){result.hit=1;if(hit.fraction<result.fraction){result.fraction=hit.fraction;result.normal=hit.normal;}}
 }else{if(index>=mesh.nodes.size())return S::InvalidInput;const auto&n=mesh.nodes[index];Vector3_48C center{{n.center[0],n.center[1],n.center[2]}},half{{n.half[0],n.half[1],n.half[2]}};auto status=resetSegmentBox50AJ(start,end,m,center,half);if(status==Type3NodeStatus48P::InvalidInput)return S::InvalidInput;if(status==Type3NodeStatus48P::Separated)continue;if(cap-used<2)return S::InsufficientCapacity;stack[used++]=n.children[1];stack[used++]=n.children[0];}}
 out=result;return S::Complete;
}
}
