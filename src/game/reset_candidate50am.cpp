#include "reset_candidate50am.h"
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
ResetTreeStatus50AK queryResetCandidate50AM(const SpringVector4_48N&start,const SpringVector4_48N&end,const BodyMatrix48G&m,const Vector3_48C&center,float radius,std::uint8_t flags,const assets::CollisionMesh48I&mesh,std::uint32_t mode,std::uint32_t*stack,std::size_t cap,ResetCandidateResult50AM&out)noexcept{
 using S=ResetTreeStatus50AK;Span ws,os;if(!span(stack,cap,sizeof(*stack),ws)||!span(&out,1,sizeof out,os)||overlap(ws,os)||(cap&&reinterpret_cast<std::uintptr_t>(stack)%alignof(std::uint32_t)))return S::InvalidInput;
 auto ro=[&](const void*p,std::size_t n,std::size_t bytes){Span x;return span(p,n,bytes,x)&&!overlap(x,ws)&&!overlap(x,os);};
 if(!ro(&start,1,sizeof start)||!ro(&end,1,sizeof end)||!ro(&m,1,sizeof m)||!ro(&center,1,sizeof center)||!ro(&mesh,1,sizeof mesh)||!ro(mesh.nodes.data(),mesh.nodes.size(),sizeof(mesh.nodes[0]))||!ro(mesh.faces.data(),mesh.faces.size(),sizeof(mesh.faces[0]))||!ro(mesh.vertices.data(),mesh.vertices.size(),sizeof(mesh.vertices[0]))||!ro(mesh.vertex_normals.data(),mesh.vertex_normals.size(),sizeof(mesh.vertex_normals[0])))return S::InvalidInput;
 auto result=out;result.hit=0;if(!(flags&1u)){out=result;return S::Complete;}
 if(!std::isfinite(radius)||radius<0.f)return S::InvalidInput;
 float delta[3],toCenter[3];for(unsigned i=0;i<3;++i){if(!std::isfinite(start.v[i])||!std::isfinite(end.v[i])||!std::isfinite(center.v[i]))return S::InvalidInput;delta[i]=sub(end.v[i],start.v[i]);toCenter[i]=sub(center.v[i],start.v[i]);}
 auto dot=[&](const float*a,const float*b){return add(add(mul(a[0],b[0]),mul(a[1],b[1])),mul(1.f,mul(a[2],b[2])));};
 const float squared=dot(delta,delta);if(!std::isfinite(squared))return S::InvalidInput;if(squared==0.f)return S::UnsupportedScope;
 float t=div(dot(delta,toCenter),squared);if(!std::isfinite(t))return S::InvalidInput;t=t<1.f?t:1.f;t=t>0.f?t:0.f;float distance[3];for(unsigned i=0;i<3;++i)distance[i]=sub(center.v[i],add(mul(t,delta[i]),start.v[i]));volatile float d=static_cast<float>(std::sqrt(static_cast<double>(dot(distance,distance))));
 if(!(d<radius)){out=result;return S::Complete;}
 BodyMatrix48G inverse{};inverse.v[15]=1.f;
 for(unsigned c=0;c<3;++c)for(unsigned i=0;i<3;++i){if(!std::isfinite(m.v[i*4+c]))return S::InvalidInput;inverse.v[c*4+i]=add(m.v[i*4+c],0.f);}
 for(unsigned i=0;i<3;++i){if(!std::isfinite(m.v[12+i]))return S::InvalidInput;inverse.v[12+i]=mul(add(add(mul(inverse.v[i],m.v[12]),mul(inverse.v[4+i],m.v[13])),mul(inverse.v[8+i],m.v[14])),-1.f);}
 ResetTreeResult50AK tree{1.f,{{0,0,0,1}},0};auto status=queryResetMesh50AK(start,end,inverse,mesh,mode,stack,cap,tree);if(status!=S::Complete)return status;
 if(tree.hit){result.hit=1;for(unsigned i=0;i<3;++i){result.point.v[i]=add(mul(tree.fraction,delta[i]),start.v[i]);result.normal.v[i]=add(add(add(mul(m.v[i],tree.normal.v[0]),mul(m.v[4+i],tree.normal.v[1])),mul(m.v[8+i],tree.normal.v[2])),0.f);}result.point.v[3]=1.f;result.normal.v[3]=2.f;}
 out=result;return S::Complete;
}
}
