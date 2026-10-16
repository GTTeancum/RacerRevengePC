#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/game/original_wreck_components50la.h"
#include <array>
#include <memory>
#include <vector>
namespace ra_pod_scene50LA {class Owner;}
namespace rr::game {
// World3D4's actual PhysicsObject model index. This bounded RA route proves
// every authored StaticType resolves Standard; it is not an empty-world claim.
class OriginalWorldPhysicsIndex50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    struct Zone {
        std::uint32_t capacity04=0,used08=0;
        std::vector<const OriginalWreckComponent50LA*> models0C;
    };
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::vector<Zone> zones_;
    bool constructed_=false,closed_=false;
    OriginalWorldPhysicsIndex50LA()noexcept=default; //27C010: count/arrays absent.
    bool prepare27C110(std::shared_ptr<const ra_cold_sources50LA::Owner> source)noexcept {
        if(constructed_||closed_||!source||!source->sourcesDecoded())return false;
        const auto& world=source->sourcesDecoded()->world;
        if(world.zones.size()!=10||world.instances.size()!=1261)return false;
        std::array<std::uint32_t,10> ordinals{};
        for(std::size_t i=0;i<world.zones.size();++i)
            if(world.zones[i].zone!=i+1||!world.zones[i].models||world.zones[i].models>1261)return false;
        // All1261 authored records participate, including geometry-less ones.
        // Do not infer this from the smaller178 decoded collision instances.
        for(const auto& instance:world.instances){
            if(!instance.zone||instance.zone>10||instance.parameter_block>=world.parameters.blocks.size())return false;
            const auto z=instance.zone-1;
            if(instance.ordinal!=++ordinals[z]||instance.ordinal>world.zones[z].models)return false;
            const auto& block=world.parameters.blocks[instance.parameter_block];
            if(block.name!=instance.section)return false;
            bool seen=false;
            for(const auto& field:block.fields)if(field.name=="statictype"){
                if(seen||field.value!="Standard")return false;
                seen=true;
            }
            //27B834 default Standard; factory27BB08/0C resolves it to type1.
            //27B89C..B8AC counts only type2;27B8DC skips27C1D0 for zero.
        }
        for(std::size_t i=0;i<ordinals.size();++i)if(ordinals[i]!=world.zones[i].models)return false;
        try {zones_.resize(world.zones.size());}catch(...){return false;}
        source_=std::move(source);constructed_=true;return true;
    }
    bool canRelease27C030()const noexcept {
        if(closed_)return true;
        if(!constructed_)return false;
        for(const auto& zone:zones_)
            if(zone.capacity04||zone.used08||!zone.models0C.empty())return false;
        return true;
    }
    bool release27C030()noexcept {
        if(!canRelease27C030())return false;
        if(closed_)return true;
        //27C030 releases per-zone backing, then the three index arrays. No
        //model reference is released by this owner; admitted per-zone backing
        //was never allocated, proven from SAME immutable authored source.
        std::vector<Zone>().swap(zones_);source_.reset();closed_=true;return true;
    }
public:
    OriginalWorldPhysicsIndex50LA(const OriginalWorldPhysicsIndex50LA&)=delete;
    OriginalWorldPhysicsIndex50LA& operator=(const OriginalWorldPhysicsIndex50LA&)=delete;
    ~OriginalWorldPhysicsIndex50LA()noexcept {
        if(constructed_&&!closed_&&!release27C030())std::terminate();
    }
};
}
