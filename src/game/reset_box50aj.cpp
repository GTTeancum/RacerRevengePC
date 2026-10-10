#include "reset_box50aj.h"
#include <cmath>
namespace rr::game {namespace {
float add(float a,float b)noexcept{volatile float x=a+b;return x;}
float sub(float a,float b)noexcept{volatile float x=a-b;return x;}
float mul(float a,float b)noexcept{volatile float x=a*b;return x;}
float transform(const SpringVector4_48N&p,const BodyMatrix48G&m,unsigned i)noexcept{
 return add(add(add(mul(m.v[i],p.v[0]),mul(m.v[4+i],p.v[1])),mul(m.v[8+i],p.v[2])),m.v[12+i]);
}
}
Type3NodeStatus48P resetSegmentBox50AJ(const SpringVector4_48N&start,
 const SpringVector4_48N&end,const BodyMatrix48G&m,const Vector3_48C&center,const Vector3_48C&half)noexcept{
 for(unsigned i=0;i<3;++i){if(!std::isfinite(start.v[i])||!std::isfinite(end.v[i])||!std::isfinite(center.v[i])||!std::isfinite(half.v[i])||half.v[i]<0.f)return Type3NodeStatus48P::InvalidInput;for(unsigned j=0;j<4;++j)if(!std::isfinite(m.v[j*4+i]))return Type3NodeStatus48P::InvalidInput;}
 float c[3],d[3],a[3];
 for(unsigned i=0;i<3;++i){const float p=sub(transform(start,m,i),center.v[i]);const float q=sub(transform(end,m,i),center.v[i]);c[i]=mul(.5f,add(p,q));d[i]=sub(c[i],p);a[i]=std::fabs(d[i]);}
 for(unsigned i=0;i<3;++i)if(!(sub(sub(std::fabs(c[i]),half.v[i]),a[i])<=0.f))return Type3NodeStatus48P::Separated;
 for(unsigned k=0;k<3;++k){const unsigned i=k==0?1:0,j=k==2?1:2;const float cross=std::fabs(sub(mul(c[i],d[j]),mul(c[j],d[i])));const float bound=add(mul(half.v[i],a[j]),mul(half.v[j],a[i]));if(!(cross<=bound))return Type3NodeStatus48P::Separated;}
 return Type3NodeStatus48P::Overlap;
}
}
