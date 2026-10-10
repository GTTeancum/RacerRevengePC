#include "reset_lifecycle50at.h"
#include <cstring>
namespace rr::game {
void initializedResetLifecycleGlobals50AT(ResetLifecycleGlobals50AT&g)noexcept{
 constexpr ResetLifecycleGlobals50AT initial{{0,0,0,0x3f800000u},{0,0,0x3f800000u,0x3f800000u},{0x3f800000u,0,0,0}};std::memcpy(&g,&initial,sizeof g);
}
bool resetLifecycleRecords50AT(std::uint32_t&active,ChariotLifecycleState49V&out,const ResetLifecycleGlobals50AT&g)noexcept{
 for(unsigned i=0;i<3;++i)if((g.forward434660[i]&0x7f800000u)==0x7f800000u)return false;
 auto s=out;std::uint32_t position[4]={g.forward434660[0]^0x80000000u,g.forward434660[1]^0x80000000u,g.forward434660[2]^0x80000000u,0x3f800000u};
 std::memcpy(s.response.position.v,position,16);std::memcpy(s.response.linearVelocity.v,g.zero434630,16);std::memcpy(s.response.angularVelocity.v,g.zero434630,16);std::memcpy(s.response.orientation.v,g.quaternion382B50,16);s.vehicle83C=0;const std::uint32_t zero=0;std::memcpy(&s.elapsed84C,&zero,4);out=s;active=1;return true;
}
}
