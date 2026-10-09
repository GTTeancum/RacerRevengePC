#include "movement_hit_point50m.h"
#include <cmath>
#include <cfenv>
#include <cstring>
#include <limits>
namespace rr::game {
bool computeMovementHitPoint50M(const SpringVector4_48N&current,const SpringVector4_48N&point,
 float push,SpringVector4_48N&out) noexcept {
 if(std::fegetround()!=FE_TONEAREST||!std::isfinite(push))return false;
 float delta[3],square[3];
 for(unsigned i=0;i<3;++i){
  if(!std::isfinite(current.v[i])||!std::isfinite(point.v[i]))return false;
  volatile float d=point.v[i]-current.v[i];delta[i]=d;volatile float q=d*d;square[i]=q;
  if(!std::isfinite(d)||!std::isfinite(q))return false;
 }
 volatile float xy=square[0]+square[1];volatile float z=square[2]*1.f;volatile float sum=xy+z;
 if(!std::isfinite(sum))return false;
 float inverse;
 if(sum==0.f)inverse=std::numeric_limits<float>::max();
 else {if(std::fpclassify(sum)==FP_SUBNORMAL)return false;volatile float root=std::sqrt(sum);volatile float q=1.f/root;inverse=q;}
 SpringVector4_48N result{{0,0,0,1}};
 for(unsigned i=0;i<3;++i){volatile float normal=delta[i]*inverse;volatile float offset=push*normal;
  volatile float reconstructed=current.v[i]+delta[i];volatile float final=reconstructed+offset;
  if(!std::isfinite(normal)||!std::isfinite(offset)||!std::isfinite(reconstructed)||!std::isfinite(final))return false;
  result.v[i]=final;
 }
 std::memcpy(&out,&result,sizeof result);return true;
}
}
