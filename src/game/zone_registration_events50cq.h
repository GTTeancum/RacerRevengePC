#pragma once
#include "zone_lifecycle50cl.h"
namespace rr::game {
enum class ZoneRegistrationOperation50CQ : std::uint8_t { Set,Remove };
struct ZoneRegistrationCommand50CQ {
    ZoneRegistrationOperation50CQ operation=ZoneRegistrationOperation50CQ::Set;
    CollectionRef50BX object=0,zone=0;
    ZoneMethod50CL method=ZoneMethod50CL::First;
};
struct ZoneRegistrationQueue50CQ {
    ZoneRegistrationCommand50CQ* commands=nullptr;
    std::size_t capacity=0,head=0,count=0;
};
enum class ZoneQueueStatus50CQ { Complete,InvalidInput,Full,RegistryRejected };
struct ZoneQueueDrain50CQ {
    ZoneQueueStatus50CQ status=ZoneQueueStatus50CQ::Complete;
    SearchStorageStatus50BS registryStatus=SearchStorageStatus50BS::Complete;
    std::size_t applied=0,remaining=0;
};
// Caller-owned serial ring. Explicit world/scene events choose destination;
// this queue never guesses a portal, zone, object identity or method.
// Queue, its backing and registry/scratch ownership must be disjoint and stable.
ZoneQueueStatus50CQ enqueueZoneRegistration50CQ(ZoneRegistrationQueue50CQ&,
    const ZoneRegistrationCommand50CQ&)noexcept;
// Drain BETWEEN collections, with no active SearchList cursor. Earlier successes
// commit in order. A failed command stays at the head, permitting an explicit
// repair/retry without replaying already-applied changes. budget bounds work.
// No heap allocation, background worker, sorting or silent event dropping.
ZoneQueueDrain50CQ drainZoneRegistrations50CQ(ZoneRegistrationQueue50CQ&,
    ZoneLifecycle50CL&,ZoneRegistrationScratch50CJ&,std::size_t budget)noexcept;
}
