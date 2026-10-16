#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/game/original_wreck_components50la.h"
#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
namespace ra_pod_scene50LA {class Owner;}
namespace rr::game {
// Actual271230 per-zone27BB40 owners, with all source wrappers. Cold only:
// the later27BEA0/27B040 collision Entity construction has not run.
class OriginalWorldCollisionObjects50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    struct Descriptor {
        const assets::WorldInstance48F* source=nullptr;
        std::string name00;
        std::array<float,4> position110{0,0,0,1},look120{0,0,1,1},up130{0,1,0,1};
        std::array<float,4> smokeOffset150{0,0,0,1},smokeVelocity160{0,0,0,1};
        std::uint32_t useLighting140=1,useDynamicLighting144=0,field148=1,isSmoker14C=0;
        std::uint32_t field180=0,field184=0;
        //3CA8F0..3CA94C constructs actual442F80=(0,0,0,1).
    };
    struct Wrapper {
        std::uint32_t type00=1;
        OriginalWreckComponent50LA* collision04=nullptr; //27BF78, no live loader yet.
        std::unique_ptr<Descriptor> descriptor08;
        const ra_cold_sources50LA::CollisionOwner* decodedCollision=nullptr;
        // Null above means authenticated member absence, NOT successful27B040.
    };
    struct Zone {
        OriginalWreckComponent50LA entity;
        std::uint32_t count30=0;
        std::vector<Wrapper> entries34;
        bool counted=false;
    };
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::array<Zone,10> zones_;
    OriginalWorldCollisionObjects50LA()noexcept=default;
    bool prepareBacking(std::shared_ptr<const ra_cold_sources50LA::Owner> source)noexcept {
        if(source_||!source||!source->sourcesDecoded())return false;
        const auto& s=*source->sourcesDecoded();const auto& w=s.world;
        if(w.zones.size()!=10||w.instances.size()!=1261)return false;
        try {
            std::size_t wi=0,ci=0;
            for(std::size_t z=0;z<10;++z){const auto& authored=w.zones[z];
                if(authored.zone!=z+1||!authored.models||authored.parameter_block>=w.parameters.blocks.size())return false;
                // Actual RA fallback2764EC..276508: no explicit count means
                // NumberOfStaticModels and ModelN names, never ObjectN names.
                for(const auto& f:w.parameters.blocks[authored.parameter_block].fields)
                    if(f.name=="numberofcollisionobjects"||f.name=="collisionobjects")return false;
                auto& zone=zones_[z];zone.entries34.resize(authored.models);
                for(auto& wrapper:zone.entries34){
                    if(wi>=w.instances.size())return false;
                    const auto& in=w.instances[wi++];
                    if(in.zone!=z+1||in.ordinal!=&wrapper-zone.entries34.data()+1||
                       in.parameter_block>=w.parameters.blocks.size())return false;
                    const auto& b=w.parameters.blocks[in.parameter_block];if(b.name!=in.section)return false;
                    bool type=false,slt=false,lighting=false;
                    auto d=std::make_unique<Descriptor>();d->source=&in;
                    for(const auto& f:b.fields){
                        if(f.name=="statictype"){if(type||f.value!="Standard")return false;type=true;}
                        else if(f.name=="slt"){
                            if(slt)return false;slt=true;const auto dot=f.value.find('.');
                            if(dot==std::string::npos||!dot||dot>50)return false;
                            d->name00=f.value.substr(0,dot);
                        }else if(f.name=="uselighting"){
                            if(lighting||(f.value!="T"&&f.value!="F"))return false;
                            lighting=true;d->useLighting140=f.value=="T";
                        }else if(f.name=="col"||f.name=="usedynamiclighting"||f.name=="issmoker"||
                                 f.name=="smokeoffset"||f.name=="smokevelocity")return false;
                    }
                    if(!slt)return false;
                    for(unsigned a=0;a<3;++a){
                        const double values[]={in.position[a],in.look[a],in.up[a]};
                        float* dest[]={&d->position110[a],&d->look120[a],&d->up130[a]};
                        for(unsigned n=0;n<3;++n){const float v=static_cast<float>(values[n]);
                            if(!std::isfinite(v)||double(v)!=values[n])return false;*dest[n]=v;}
                    }
                    std::string member="geometry\\"+d->name00+".col";
                    for(char& c:member)if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
                    if(member!=in.collision_member)return false;
                    std::size_t archiveMatches=0;
                    for(const auto& entry:s.archive.entries){
                        if(entry.name.size()!=member.size())continue;
                        bool equal=true;
                        for(std::size_t n=0;n<member.size();++n){char c=entry.name[n];
                            if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
                            if(c!=member[n]){equal=false;break;}}
                        if(equal)++archiveMatches;
                    }
                    if(archiveMatches>1)return false;
                    if(ci<s.collisions.size()&&s.collisions[ci].zone==in.zone&&s.collisions[ci].ordinal==in.ordinal){
                        if(s.collisions[ci].member!=in.collision_member)return false;
                        wrapper.decodedCollision=&s.collisions[ci++];
                    }
                    if(bool(wrapper.decodedCollision)!=(archiveMatches==1))return false;
                    wrapper.descriptor08=std::move(d);
                }
            }
            if(wi!=w.instances.size()||ci!=s.collisions.size())return false;
        }catch(...){return false;}
        source_=std::move(source);return true;
    }
public:
    ~OriginalWorldCollisionObjects50LA()noexcept {
        for(const auto& z:zones_)if(z.counted)std::terminate();
    }
};
}
