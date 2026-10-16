#pragma once
#include "track_source_fixture.h"
#include "ra_cold_sources50la.h"
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_world48f.h"
#include <array>
#include <charconv>
#include <cstdint>
#include <string>
#include <vector>

namespace original_track_sky50KX {
// Single-cube projection of276C00/275854..275964. Actual cold sky resources
// stay owned by sky50GW; this owner only projects the source enable bits.
// The native root samples at presentation from its current recovered camera,
// rather than claiming the complete retail world-service/render chronology.
class Owner final {
    std::array<std::int32_t,25> zoneCube_{};
    std::uint32_t zoneCount_=0;
    bool prepared_=false;
    static const std::string* field(const rr::assets::ParameterBlock48F& block,
        const std::string& key) noexcept {
        const std::string* value=nullptr;
        for(const auto& f:block.fields)if(f.name==key){if(value)return nullptr;value=&f.value;}
        return value;
    }
    static bool integer(const std::string* text,std::int32_t fallback,std::int32_t& value) noexcept {
        value=fallback;if(!text)return true;
        const auto r=std::from_chars(text->data(),text->data()+text->size(),value);
        return r.ec==std::errc{}&&r.ptr==text->data()+text->size();
    }
public:
    bool prepare(const rr::assets::World48F& world,track_source_fixture::Track track,
        std::string& error) {
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        const bool ma=track==track_source_fixture::Track::ma;
        const bool sa=track==track_source_fixture::Track::sa;
        const char* hash=ma?"5766af3e7a0bba0644058ae6f31eab802cadd9be7cc71911c349f603c7ca873b":
            "dd38f087f0438b2e1279651fe3b4f525b053c90d34cf5365d6bf4ec6c7d1d394";
        if(!ma&&!sa){error="source sky selection requires its actual authenticated MA/SA SCB";return false;}
        return prepareAuthenticated(world,ma?404992u:813441u,hash,ma?9u:25u,
            ma?"ma.cub":"sullust.cub",error);
    }
    // Cold source selection only. Uses the actual retained authenticated RA
    // decoder owner; neither a new Track enum nor host/Course admission follows.
    // The CUB identity authenticates the source resource, never decoded images
    // or GPU availability. Those remain the actual sky renderer's duties.
    bool prepareRaSource(const ra_cold_sources50LA::Owner& source,std::string& error){
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        const auto* decoded=source.sourcesDecoded();
        if(!decoded){error="actual RA decoded source owner required";return false;}
        std::vector<std::uint8_t> cube;rr::assets::DecodeError48C diagnostic;
        if(!rr::assets::extractResMember48C(decoded->archive,"skies\\ra.cub",cube,diagnostic,161825u)||
           cube.size()!=161825u||pod_materials50ed_fixture::detail::sha256(cube)!=
            "c497927d2fe78de6f2f9bbf9e319ac0ea2b4939e020df45a4ae570e5abb73960"){
            error="RA sky selection requires its authentic ra.cub source";return false;
        }
        return prepareAuthenticated(decoded->world,540795u,
            "57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81",
            10u,"ra.cub",error);
    }
private:
    bool prepareAuthenticated(const rr::assets::World48F& world,std::size_t bytes,
        const char* hash,std::size_t zones,const char* cubeName,std::string& error){
        if(world.parameters.original_bytes.size()!=bytes||
           pod_materials50ed_fixture::detail::sha256(world.parameters.original_bytes)!=hash||
           world.zones.size()!=zones){
            error="source sky selection requires its actual authenticated SCB";return false;
        }
        const rr::assets::ParameterBlock48F* environment=nullptr;
        for(const auto& b:world.parameters.blocks)if(b.name=="environment"){
            if(environment){error="duplicate source Environment";return false;}environment=&b;
        }
        std::int32_t count=0;
        if(!environment||!integer(field(*environment,"numberofskycubes"),1,count)||count!=1){
            error="source sky selection requires its single actual cube";return false;
        }
        const auto* cube=field(*environment,"cubefile1");
        if(!cube)cube=field(*environment,"cubefile"); //276CD8..276D1C first-cube legacy key.
        if(!cube||*cube!=cubeName){
            error="source sky selection cube identity differs";return false;
        }
        auto indices=zoneCube_;
        for(std::uint32_t i=0;i<world.zones.size();++i){
            //276D7C default1,276E24 subtract1: authored0 explicitly disables.
            std::int32_t authored=0;
            if(!integer(field(*environment,"cubefileforzone"+std::to_string(i+1)),1,authored)||
               authored<0||authored>count){error="source sky zone cube index rejected";return false;}
            indices[i]=authored-1;
        }
        zoneCube_=indices;zoneCount_=static_cast<std::uint32_t>(world.zones.size());prepared_=true;
        return true;
    }
public:
    bool select(std::uint32_t actualCameraZone48,bool& enabled)const noexcept {
        if(!prepared_||actualCameraZone48>=zoneCount_)return false;
        //275894 maps camera scene38->zone48;2758BC disables every cube,
        //275914 skips negative index;27591C..275964 enables the selected cube.
        enabled=zoneCube_[actualCameraZone48]==0;return true;
    }
};
}
