#include "grid_center50ac.h"
#include <cmath>
#include <cfenv>
#include <cstring>
namespace rr::game {namespace {
float fp(std::uint32_t u){float x;std::memcpy(&x,&u,4);return x;}std::uint32_t bits(float x){std::uint32_t u;std::memcpy(&u,&x,4);return u;}
struct Math{bool valid=true;float keep(float x){const auto u=bits(x),e=u&0x7f800000u;if(e==0x7f800000u||(!e&&(u&0x7fffffu)))valid=false;return x;}float sub(float a,float b){keep(a);keep(b);volatile float x=a-b;return keep(x);}float mul(float a,float b){keep(a);keep(b);volatile float x=a*b;return keep(x);}float add(float a,float b){keep(a);keep(b);volatile float x=a+b;return keep(x);}};
}
bool prepareGridCenter50AC(const GridCenterInput50AC&in,GridCenterResult50AC&out) noexcept {
 if((in.rank!=1&&in.rank!=4)||std::fegetround()!=FE_TONEAREST)return false;
 Math m;float d[3];for(unsigned i=0;i<3;++i)d[i]=m.sub(fp(in.current[i]),fp(in.previous[i]));
 const float x=m.mul(d[0],d[0]),y=m.mul(d[1],d[1]),z=m.mul(d[2],d[2]);const float square=m.add(m.add(x,y),z);if(!m.valid||!(square>0))return false;
 volatile float root=static_cast<float>(std::sqrt(static_cast<double>(square)));m.keep(root);volatile float reciprocal=1.f/root;m.keep(reciprocal);
 GridCenterResult50AC result{};result.position[3]=in.current[3];result.heading[3]=0x3f800000u;result.nextNode=in.nextNode;
 const float distance=static_cast<float>((in.rank/3u)*100u);
 for(unsigned i=0;i<3;++i){const float n=m.mul(d[i],reciprocal);result.heading[i]=bits(n);result.position[i]=bits(m.sub(fp(in.current[i]),m.mul(distance,n)));}
 if(!m.valid)return false;out=result;return true;
}
bool prepareCenterPose50AC(const GridCenterInput50AC&in,float height,const Vector3_48C&up,BodyMatrix48G&out) noexcept {
 GridCenterResult50AC center{};if(!prepareGridCenter50AC(in,center))return false;Math m;
 auto cross=[&](const Vector3_48C&a,const Vector3_48C&b){Vector3_48C v{};for(unsigned i=0;i<3;++i)v.v[i]=m.sub(m.mul(a.v[(i+1)%3],b.v[(i+2)%3]),m.mul(a.v[(i+2)%3],b.v[(i+1)%3]));return v;};
 auto normalize=[&](const Vector3_48C&a,Vector3_48C&v){const auto x=m.mul(a.v[0],a.v[0]),y=m.mul(a.v[1],a.v[1]),z=m.mul(a.v[2],a.v[2]);const auto square=m.add(m.add(x,y),z);if(!m.valid||!(square>0))return false;volatile float root=static_cast<float>(std::sqrt(static_cast<double>(square)));m.keep(root);volatile float q=1.f/root;m.keep(q);for(unsigned i=0;i<3;++i)v.v[i]=m.mul(a.v[i],q);return m.valid;};
 Vector3_48C heading{},normalizedUp{},normalizedHeading{};for(unsigned i=0;i<3;++i)heading.v[i]=fp(center.heading[i]);
 const auto initialRight=cross(up,heading);const auto preparedUp=cross(heading,initialRight);
 if(!normalize(preparedUp,normalizedUp)||!normalize(heading,normalizedHeading))return false;
 const auto right=cross(normalizedUp,normalizedHeading);BodyMatrix48G matrix{};
 for(unsigned i=0;i<3;++i){matrix.v[i]=right.v[i];matrix.v[4+i]=normalizedUp.v[i];matrix.v[8+i]=normalizedHeading.v[i];matrix.v[12+i]=m.add(fp(center.position[i]),i==1?height:0.f);}matrix.v[15]=1.f;
 if(!m.valid)return false;out=matrix;return true;
}

}
