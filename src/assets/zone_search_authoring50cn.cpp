#include "zone_search_authoring50cn.h"
namespace rr::assets {
ZoneSearchReadResult50CN readZoneSearchSelection50CN(const World48F&world,
    std::size_t zoneIndex)noexcept{
    ZoneSearchReadResult50CN result{};
    if(zoneIndex>=world.zones.size())return result;
    const auto&zone=world.zones[zoneIndex];
    if(zone.parameter_block>=world.parameters.blocks.size())return result;
    const ParameterField48F*token=nullptr;
    for(const auto&field:world.parameters.blocks[zone.parameter_block].fields)
        if(field.name=="searchtype"){
            if(token){result.status=ZoneSearchReadStatus50CN::DuplicateField;return result;}
            token=&field;
        }
    if(!token){result.status=ZoneSearchReadStatus50CN::MissingGenericToken;return result;}
    ZoneSearchKind50CN kind;
    if(token->value=="List")kind=ZoneSearchKind50CN::List;
    else if(token->value=="QuadTree")kind=ZoneSearchKind50CN::QuadTree;
    else{result.status=ZoneSearchReadStatus50CN::UnsupportedToken;return result;}
    result.selection={zone.zone,zone.parameter_block,kind,kind,kind,kind};
    result.status=ZoneSearchReadStatus50CN::Complete;return result;
}
}
