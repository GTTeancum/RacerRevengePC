#include "base_pose_history50y.h"
#include <cstring>
#include <cfenv>
#include <cmath>
namespace rr::game {namespace {
float fp(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
struct Math {bool okay=true;float check(float f){const auto b=bits(f),e=b&0x7f800000u;if(e==0x7f800000u||(!e&&(b&0x7fffffu)))okay=false;return f;}float sub(float a,float b){check(a);check(b);volatile float v=a-b;return check(v);}float mul(float a,float b){check(a);check(b);volatile float v=a*b;return check(v);}float add(float a,float b){check(a);check(b);volatile float v=a+b;return check(v);}};
}
BasePoseHistoryResult50Y advanceBasePoseHistory50Y(const BasePoseHistoryState50Y&previous,const BasePoseHistoryInput50Y&in) noexcept {
 BasePoseHistoryResult50Y out{previous,BasePoseHistoryStatus50Y::Complete};
 if(!in.diagnosticsAbsent){out.status=BasePoseHistoryStatus50Y::NeedsDiagnostics;return out;}
 if(!in.worldReady||previous.currentFD0>=20||previous.nextFD4>=20||std::fegetround()!=FE_TONEAREST){out.status=BasePoseHistoryStatus50Y::Admission;return out;}
 auto&s=out.state;Math m;const auto timer=m.check(fp(s.timerD2C));
 if(!(timer<=0.f)){const float next=m.sub(timer,fp(in.outerDeltaBits));s.timerD2C=bits(next);if(next<=0.f){s.timerD2C=0;s.markerD30=0xffffffffu;}}
 Quaternion48C q{};if(!matrixToQuaternion48G(in.readyWorld100,q))m.okay=false;
 std::memcpy(s.quaternionC60,q.v,16);std::memcpy(s.positionC50,in.readyWorld100.v+12,12);std::memcpy(s.velocityC70,in.velocity70,16);
 float delta[3];for(unsigned i=0;i<3;++i)delta[i]=m.sub(fp(in.positionAt228B98[i]),fp(s.historyD50[s.currentFD0][i]));
 const float x=m.mul(delta[0],delta[0]),y=m.mul(delta[1],delta[1]),z=m.mul(delta[2],delta[2]);const float squared=m.add(m.add(x,y),z);float distance=0.f;
 if(squared!=0.f){volatile float reciprocal=static_cast<float>(1.0/std::sqrt(static_cast<double>(squared)));m.check(reciprocal);volatile float d=1.f/reciprocal;distance=m.check(d);}
 s.historyD50[s.currentFD0][4]=bits(distance);
 if(!(distance<=10.f)){s.currentFD0=s.nextFD4;std::memcpy(s.historyD50[s.currentFD0],in.positionAt228B98,16);s.historyD50[s.currentFD0][4]=0;s.nextFD4=s.nextFD4? s.nextFD4-1:19;}
 if(!m.okay){out.state=previous;out.status=BasePoseHistoryStatus50Y::Arithmetic;}return out;
}
}
