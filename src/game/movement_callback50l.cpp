#include "movement_callback50l.h"
#include <cmath>
#include <cfenv>
#include <cstddef>
namespace rr::game {
namespace {
bool aliases(const void*a,std::size_t n,const void*b,std::size_t m) noexcept {
 const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
 return x<y?y-x<n:x-y<m;
}
}
MovementCallbackStatus50L applyMovementCallback50L(std::uint32_t category,std::uint32_t mask,
 const Vector3_48C* other,const Vector3_48C* self,std::uint32_t& flag) noexcept {
 using S=MovementCallbackStatus50L;
 if(!(category&mask)){flag=1;return S::Complete;}
 if(std::fegetround()!=FE_TONEAREST||!other||!self||reinterpret_cast<std::uintptr_t>(other)%alignof(Vector3_48C)||
    reinterpret_cast<std::uintptr_t>(self)%alignof(Vector3_48C)||
    aliases(&flag,sizeof flag,other,sizeof *other)||aliases(&flag,sizeof flag,self,sizeof *self))return S::InvalidInput;
 float products[3];
 for(unsigned i=0;i<3;++i){
  if(!std::isfinite(other->v[i])||!std::isfinite(self->v[i]))return S::InvalidInput;
  volatile float product=other->v[i]*self->v[i];products[i]=product;
  if(!std::isfinite(products[i]))return S::InvalidInput;
 }
 volatile float xy=products[0]+products[1];volatile float xyz=xy+products[2];
 if(!std::isfinite(xy)||!std::isfinite(xyz))return S::InvalidInput;
 if(xyz<0.f)flag=1;
 return S::Complete;
}
}
