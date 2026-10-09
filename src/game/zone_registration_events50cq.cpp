#include "zone_registration_events50cq.h"
#include <limits>
namespace rr::game {namespace{
bool valid(const ZoneRegistrationQueue50CQ&q)noexcept{
    if(!q.capacity)return !q.head&&!q.count&&!q.commands;
    if(!q.commands||q.head>=q.capacity||q.count>q.capacity||q.capacity>std::numeric_limits<std::size_t>::max()/sizeof(*q.commands))return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(q.commands),bytes=q.capacity*sizeof(*q.commands);
    return begin%alignof(ZoneRegistrationCommand50CQ)==0&&bytes<=std::numeric_limits<std::uintptr_t>::max()-begin;
}
bool commandValid(const ZoneRegistrationCommand50CQ&c)noexcept{
    if(!c.object||c.object>(std::numeric_limits<std::uint32_t>::max()>>2))return false;
    if(c.operation==ZoneRegistrationOperation50CQ::Remove)return true;
    return c.operation==ZoneRegistrationOperation50CQ::Set&&c.zone&&
        (c.method==ZoneMethod50CL::First||c.method==ZoneMethod50CL::Second);
}
}
ZoneQueueStatus50CQ enqueueZoneRegistration50CQ(ZoneRegistrationQueue50CQ&q,
    const ZoneRegistrationCommand50CQ&command)noexcept{
    if(!valid(q)||!commandValid(command))return ZoneQueueStatus50CQ::InvalidInput;
    if(q.count==q.capacity)return ZoneQueueStatus50CQ::Full;
    const auto untilEnd=q.capacity-q.head;
    const auto tail=q.count<untilEnd?q.head+q.count:q.count-untilEnd;
    q.commands[tail]=command;++q.count;return ZoneQueueStatus50CQ::Complete;
}
ZoneQueueDrain50CQ drainZoneRegistrations50CQ(ZoneRegistrationQueue50CQ&q,
    ZoneLifecycle50CL&state,ZoneRegistrationScratch50CJ&scratch,std::size_t budget)noexcept{
    ZoneQueueDrain50CQ result{};result.remaining=q.count;
    if(!valid(q)){result.status=ZoneQueueStatus50CQ::InvalidInput;return result;}
    while(q.count&&result.applied<budget){
        const auto command=q.commands[q.head];
        if(!commandValid(command)){result.status=ZoneQueueStatus50CQ::InvalidInput;break;}
        result.registryStatus=command.operation==ZoneRegistrationOperation50CQ::Remove?
            removeZoneRegistration50CL(state,command.object,scratch):
            updateZoneRegistration50CL(state,command.object,command.zone,command.method,scratch);
        if(result.registryStatus!=SearchStorageStatus50BS::Complete){result.status=ZoneQueueStatus50CQ::RegistryRejected;break;}
        q.head=q.head==q.capacity-1?0:q.head+1;--q.count;++result.applied;
    }
    result.remaining=q.count;return result;
}
}
