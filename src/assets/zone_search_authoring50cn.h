#pragma once
#include "import_world48f.h"
namespace rr::assets {
enum class ZoneSearchKind50CN : std::uint32_t { List=1,QuadTree=2 };
enum class ZoneSearchReadStatus50CN { Complete,InvalidZone,MissingGenericToken,UnsupportedToken,DuplicateField };
struct ZoneSearchSelection50CN {
    std::uint32_t authoredZone=0,parameterBlock=0;
    ZoneSearchKind50CN visibilityStatic110=ZoneSearchKind50CN::List;
    ZoneSearchKind50CN visibilityDynamic114=ZoneSearchKind50CN::List;
    ZoneSearchKind50CN collisionStatic118=ZoneSearchKind50CN::List;
    ZoneSearchKind50CN collisionDynamic11C=ZoneSearchKind50CN::List;
};
struct ZoneSearchReadResult50CN {
    ZoneSearchReadStatus50CN status=ZoneSearchReadStatus50CN::InvalidZone;
    ZoneSearchSelection50CN selection{};
};
// Cold read of the generic-present275F74..275FC8 route only.276530 maps exact
// List/QuadTree tokens to1/2, then all four source fields receive the same kind.
// A missing token is an explicit fallback-reader dependency, never default List.
// Uses retained parsed SCB fields; no file I/O, allocation or source mutation.
ZoneSearchReadResult50CN readZoneSearchSelection50CN(const World48F&,
    std::size_t zoneIndex)noexcept;
}
