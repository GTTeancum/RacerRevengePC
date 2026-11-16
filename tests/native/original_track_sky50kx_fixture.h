#pragma once
#include "track_source_fixture.h"
#include "ra_cold_sources50la.h"
#include "mb_original_world50lm.h"
#include "tc_original_world50lm.h"
#include "rc_cold_sources50la.h"
#include "tb_cold_sources50la.h"
#include "gb_cold_sources50la.h"
#include "mc_cold_sources50la.h"
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
    std::array<std::int32_t,256> zoneCube_{}; // zone-row capacity (live-world zone bound)
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
    // Cold metadata from the SAME loaded MB world owner. Re-decode its exact
    // authenticated SCB locally; no unrelated mutable World48F is accepted and
    // no second world/archive is retained after these 16 enable bits publish.
    bool prepareMbSource(const mb_original_world50LM::Owner& source,std::string& error){
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        const auto* archive=source.archive();
        if(!source.loaded()||!archive||archive->entries.size()!=487u||archive->decoded.size()!=16392192u){
            error="actual loaded MB world/archive owner required";return false;
        }
        std::vector<std::uint8_t> cube,scene;rr::assets::DecodeError48C diagnostic;
        if(!rr::assets::extractResMember48C(*archive,"skies\\mb.cub",cube,diagnostic,210066u)||
           cube.size()!=210066u||pod_materials50ed_fixture::detail::sha256(cube)!=
            "31cff4858fd5132ab838102877e21c2d14077e12e1a86fae77dbbf9587efcdd0"){
            error="MB sky selection requires its authentic mb.cub source";return false;
        }
        if(!rr::assets::extractResMember48C(*archive,"tracks\\mb.scb",scene,diagnostic,
                mb_original_world50LM::scbBytesCount)||scene.size()!=mb_original_world50LM::scbBytesCount||
           pod_materials50ed_fixture::detail::sha256(scene)!=mb_original_world50LM::scbSha){
            error="MB sky selection requires its authentic retained mb.scb source";return false;
        }
        rr::assets::World48F world;
        if(!rr::assets::decodeWorld48F(scene,world,diagnostic)){
            error="MB sky selection original SCB decode failed";return false;
        }
        return prepareAuthenticated(world,mb_original_world50LM::scbBytesCount,
            mb_original_world50LM::scbSha,16u,"mb.cub",error);
    }
    // Cold source selection from the loaded TC world only. Authenticate TC's
    // own ta.cub and TC SCB before decoding the local parameter view. No world
    // or archive borrow survives publication of the actual 25 zone indices.
    bool prepareTcSource(const tc_original_world50LM::Owner& source,std::string& error){
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        const auto* archive=source.archive();
        if(!source.loaded()||!archive||archive->entries.size()!=509u||archive->decoded.size()!=16048128u){
            error="actual loaded TC world/archive owner required";return false;
        }
        std::vector<std::uint8_t> cube,scene;rr::assets::DecodeError48C diagnostic;
        if(!rr::assets::extractResMember48C(*archive,"skies\\ta.cub",cube,diagnostic,164187u)||
           cube.size()!=164187u||pod_materials50ed_fixture::detail::sha256(cube)!=
            "27e43db810442e9fd90fda8551e6804ab68d48e8398992a1b2997945e88d68f4"){
            error="TC sky selection requires its own authentic ta.cub source";return false;
        }
        if(!rr::assets::extractResMember48C(*archive,"tracks\\tc.scb",scene,diagnostic,
                tc_original_world50LM::scbBytesCount)||scene.size()!=tc_original_world50LM::scbBytesCount||
           pod_materials50ed_fixture::detail::sha256(scene)!=tc_original_world50LM::scbSha){
            error="TC sky selection requires its authentic retained tc.scb source";return false;
        }
        rr::assets::World48F world;
        if(!rr::assets::decodeWorld48F(scene,world,diagnostic)){
            error="TC sky selection original SCB decode failed";return false;
        }
        return prepareAuthenticated(world,tc_original_world50LM::scbBytesCount,
            tc_original_world50LM::scbSha,25u,"ta.cub",error);
    }
    // RC uses its existing authenticated compact source. Only the selected
    // zone indices are retained here; no second archive or World is decoded.
    bool prepareRcSource(const rc_cold_sources50LA::Owner& source,std::string& error){
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        if(!source.ready()){error="actual authenticated RC source owner required";return false;}
        std::vector<std::uint8_t> cube;rr::assets::DecodeError48C diagnostic;
        if(!source.extractMember("skies\\rcsky.cub",cube,diagnostic,167607u)||cube.size()!=167607u||
            pod_materials50ed_fixture::detail::sha256(cube)!=
                "3f8a28b1d91d127638cb8e70cb996107968e135a0e9704bfba4cd98931697436"){
            error="RC sky selection requires its authentic rcsky.cub source";return false;
        }
        return prepareAuthenticated(source.world(),468918u,
            "1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a",
            15u,"rcSky.cub",error);
    }
    // Same authentic TB compact source; only the ten source enable bits survive.
    bool prepareTbSource(const tb_cold_sources50LA::Owner& source,std::string& error){
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        if(!source.ready()){error="actual authenticated TB source owner required";return false;}
        std::vector<std::uint8_t> cube;rr::assets::DecodeError48C diagnostic;
        if(!source.extractMember("skies\\tb.cub",cube,diagnostic,184730u)||cube.size()!=184730u||
            pod_materials50ed_fixture::detail::sha256(cube)!=
                "fe992b1ccf89f28fa1f47ebe1ad1958e9f0e459a408e9341edf732ae95d95330"){
            error="TB sky selection requires its authentic tb.cub source";return false;
        }
        return prepareAuthenticated(source.world(),448107u,
            "c3536c6e76dc4d6836ee154aabc80f5e73bd80e85189b841538b3a3ce53e8184",
            10u,"tb.cub",error);
    }
    // GB selects its one authored file; its seven internal cube payloads are
    // distinct from EnvironmentInfo's single authored cube-file selection.
    bool prepareGbSource(const gb_cold_sources50LA::Owner& source,std::string& error){
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        if(!source.ready()){error="actual authenticated GB source owner required";return false;}
        std::vector<std::uint8_t> cube;rr::assets::DecodeError48C diagnostic;
        if(!source.extractMember("skies\\gb.cub",cube,diagnostic,304110u)||cube.size()!=304110u||
           pod_materials50ed_fixture::detail::sha256(cube)!="6b97ffd55d8aad11819d2696cc2366bf0303a42ebc65219b9ef4f4214a94a59c"){
            error="GB sky selection requires its authentic gb.cub source";return false;
        }
        return prepareAuthenticated(source.world(),401867u,
            "ed0eb494f8c91df5264e120bad3780016f67f297f259572d48a3e7a56aad27bb",
            13u,"gb.cub",error);
    }
    bool prepareMcSource(const mc_cold_sources50LA::Owner& source,std::string& error){
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        if(!source.ready()){error="actual authenticated MC source owner required";return false;}
        std::vector<std::uint8_t> cube;rr::assets::DecodeError48C diagnostic;
        if(!source.archive().extractMember("skies\\mcset4.cub",cube,diagnostic,194342u)||cube.size()!=194342u||
           pod_materials50ed_fixture::detail::sha256(cube)!="ff3eb5a3e3480a04fe3bf3a85ab2d9113803f6fbd2b5301be43a2ff471a64383"){
            error="MC sky selection requires its authentic mcset4.cub source";return false;
        }
        return prepareAuthenticated(source.world(),187711u,
            "26f5d637533ee9840e916e02d5cef7b06d68352f1b7e0b11a48bf050d73df51e",
            28u,"mcset4.cub",error);
    }
    // Course-neutral: the same reader on a World48F its course's cold owner
    // already decoded from that course's own archive. No identity pin; cube
    // count, cube name and zone rows are the data's (the sky images themselves
    // are loaded by sky50gw_fixture::Owner::loadFromCourseData from the same
    // world, so both read one [Environment] block).
    bool prepareFromCourseData(const rr::assets::World48F& world,std::string& error){
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        return prepareSelection(world,nullptr,error);
    }
private:
    bool prepareAuthenticated(const rr::assets::World48F& world,std::size_t bytes,
        const char* hash,std::size_t zones,const char* cubeName,std::string& error){
        if(world.parameters.original_bytes.size()!=bytes||
           pod_materials50ed_fixture::detail::sha256(world.parameters.original_bytes)!=hash||
           world.zones.size()!=zones||zones>zoneCube_.size()){
            error="source sky selection requires its actual authenticated SCB";return false;
        }
        return prepareSelection(world,cubeName,error);
    }
    // The 276C00 Environment read shared by every course. expectedCube is the
    // stock rows' identity cross-check; null for a course read from its data.
    bool prepareSelection(const rr::assets::World48F& world,const char* expectedCube,std::string& error){
        // Bounded by this owner's fixed zone-row store; a larger world is refused.
        if(world.zones.empty()||world.zones.size()>zoneCube_.size()){
            error="source sky selection zone count exceeds its row store";return false;
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
        if(!cube||(expectedCube&&*cube!=expectedCube)){
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
