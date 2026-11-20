// HOST scalar regression inputs only. No gameplay, registered collider, Bank,
// application RNG chronology, source-live callbacks or query admission claim.
#include "../../src/game/native_trigger_group50mc.h"
#include <iostream>
#include <stdexcept>
namespace g=rr::game;
static void need(bool v,const char* why){if(!v)throw std::runtime_error(why);}
struct Calls {unsigned count=0;bool block=false;};
int main(){try{
 need(g::originalTriggerPercent50MC(0)==1&&g::originalTriggerPercent50MC(0x7fffffffu)==100,"chance endpoints");
 for(std::uint32_t sample=0;sample<0x7fffffffu-1048576u;sample+=1048576u){const auto p=g::originalTriggerPercent50MC(sample);need(p>=1&&p<=100,"chance range");}
 g::NativeTriggerGroup50MC group;g::SharedRandomOwner50ID shared;g::HostMonotonicClock50ID clock;std::uint32_t raw=1;
 shared.seed.lazy80A8=true;shared.seed.seed3D6B30=1;group.chance={&raw,&shared,&clock};
 Calls calls;group.actions={&calls,[](void*,const g::SourceTriggerActionCall50MC&)noexcept{return true;},
  [](void* p,const g::SourceTriggerActionCall50MC& c)noexcept{auto& s=*static_cast<Calls*>(p);++s.count;return g::SourceTriggerActionResult50MC{s.block?g::SourceTriggerActionStatus50MC::Blocked:g::SourceTriggerActionStatus50MC::Complete,c.source.sourceSite,0};}};
 need(g::sizeNativeTriggerGroup50MC(group,21,1024),"group sized once from the live count"); // stock MC live count (receipt)
 for(std::size_t i=0;i<21;++i){auto& s=group.triggers[i];s.collider.identity=1000+i;s.callbackCount98=1;s.randomTriggerPercent90=i==20?50:(i==1||i==2||i==19)?100:99;if(i==20)s.flags1B8|=4;}
 for(std::size_t i=0;i<16;++i)group.colliders[i].identity=2000+i;
 group.ready=true;group.queryActive=true;group.querySerial=1;group.queryCollider=2000;
 need(g::validNativeTriggerGroup50MC(group),"exact source-shaped group");
 auto result=g::dispatchNativeTriggerContact50MC(group,1,2000,90);
 need(result.status==g::SourceTriggerContactStatus50MC::Complete&&!result.chance.drawn&&raw==1&&calls.count==1&&result.addedPairs==1,"100 chance bypass");
 ++group.querySerial;result=g::dispatchNativeTriggerContact50MC(group,1,2000,90);
 need(result.status==g::SourceTriggerContactStatus50MC::Complete&&!result.chance.drawn&&calls.count==1&&!result.addedPairs,"duplicate no callback replay");
 ++group.querySerial;result=g::dispatchNativeTriggerContact50MC(group,20,2000,109);
 need(result.status==g::SourceTriggerContactStatus50MC::Complete&&result.chance.drawn&&result.chance.percent>50&&!result.addedPairs&&calls.count==1&&(group.triggers[20].flags1B8&0x10)&&!(group.triggers[20].flags1B8&8),"50 rejection keeps touched but not fired");
 group.triggers[20].flags1B8|=0x20;const auto previous=raw;++group.querySerial;
 result=g::dispatchNativeTriggerContact50MC(group,20,2000,109);
 need(result.status==g::SourceTriggerContactStatus50MC::Complete&&!result.chance.drawn&&raw==previous,"previous interval suppresses RNG");
 raw=0x8dbdbb1eu;const auto callbacks=calls.count;++group.querySerial;
 result=g::dispatchNativeTriggerContact50MC(group,0,2000,89);
 need(result.status==g::SourceTriggerContactStatus50MC::Complete&&result.chance.drawn&&result.chance.percent==100&&!result.addedPairs&&calls.count==callbacks,"99 rejects actual100 draw");
 group.triggers[0].flags1B8|=0x20;need(g::validNativeTriggerGroup50MC(group),"valid chance state");
 std::cout<<"MC HOST chance/group scalar regression only\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
