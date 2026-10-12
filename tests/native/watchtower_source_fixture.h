#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/import_world48f.h"
#include <array>
#include <cstring>
#include <string>
#include <vector>

// Cold authenticated original Watchtower Run resource owner. It deliberately
// does not publish a runnable session or manufacture unjoined dynamic owners.
namespace watchtower_source_fixture {
namespace a=rr::assets;
using Bytes=std::vector<std::uint8_t>;
inline constexpr std::size_t archiveBytes=2834942,decodedBytes=26099712;
inline constexpr std::uint32_t archiveEntries=970,courseIndex=8;
inline constexpr const char* archiveSha=
    "063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9";
struct Member {
    const char* name;std::size_t bytes;const char* sha;
};
inline constexpr Member scene{"tracks\\ga.scb",292650,
    "ad38b0d47efdda92e36bf36f9b380f7f3111698cb068532068a19229097de5d7"};
inline constexpr Member cube{"skies\\ga.cub",193885,
    "a7a39f4efab6ff032293846f90429d4235cfc3ec9d16aea178b59108faf67141"};
inline constexpr Member mapSpline{"tracks\\mga.spl",18403,
    "e970addc78c6acee941c3b3cb237a0d3322a857b39165719f5eacd0eb3757eb3"};
inline constexpr Member aiSpline{"tracks\\aga.spl",22380,
    "81033b6deef3c7da03325f2d467b5132ae608889aa91c94cc800abea99e95786"};
inline constexpr Member tuning{"tracks\\ga.tun",978,
    "70cc374b434029360c36c3aa5595bcdb7425fd5c2e73443752a43bb7a68f83e5"};
inline constexpr Member objects{"tracks\\ga.tob",9689,
    "e96450990e23c97f6d686707a183789b08d45857187d5c8463960cb323e75aa3"};
inline constexpr Member audio{"tracks\\ga.aub",41363,
    "8302ea938033d539feece2a13ebc2f3c2171938be13770afdebc980940a043ec"};
inline constexpr std::size_t zones=12,portals=18,staticPlacements=753,
    staticCollisionPlacements=450,staticDrawablePlacements=563;
struct Atmosphere {
    std::array<std::uint8_t,3> fogColor{{117,107,66}};
    float visibility=.4f,haziness=.5f;
    // Original Light::Init divides source RGB by2. These are source channels,
    // not normalized shader values and not a full-screen color grade.
    std::array<std::int32_t,3> ambientColor{{108,108,83}},sunColor{{93,93,53}};
    std::uint32_t authoredLights=38;
};
struct Storage {
    a::ResArchive48C archive;
    a::World48F world;
    Bytes sky,map,ai,tun,tob,aub;
    Atmosphere atmosphere{};
};
inline bool load(const Bytes& original,Storage& output,std::string& error) {
    using namespace pod_materials50ed_fixture::detail;
    try {
        need(original.size()==archiveBytes&&sha256(original)==archiveSha,
            "Watchtower requires the exact original GA.RES");
        Storage next;
        a::DecodeError48C d;a::DecodeLimits48C limits;
        limits.resource_file_bytes=archiveBytes;
        // Resource reader's block-count bound must admit the final exact block.
        limits.resource_decoded_bytes=decodedBytes;limits.resource_entries=archiveEntries;
        need(a::decodeResource48C(original,next.archive,d,nullptr,limits),
            "Original GA archive decode: "+d.code);
        need(next.archive.entries.size()==archiveEntries&&next.archive.decoded.size()==decodedBytes,
            "Original GA archive table extent");
        const auto extract=[&](const Member& member,Bytes& bytes){
            need(a::extractResMember48C(next.archive,member.name,bytes,d,member.bytes),
                std::string("Original GA member unavailable: ")+member.name);
            need(bytes.size()==member.bytes&&sha256(bytes)==member.sha,
                std::string("Original GA member identity differs: ")+member.name);
        };
        Bytes scb;extract(scene,scb);
        extract(cube,next.sky);extract(mapSpline,next.map);extract(aiSpline,next.ai);
        extract(tuning,next.tun);extract(objects,next.tob);extract(audio,next.aub);
        need(a::decodeWorld48F(scb,next.world,d),"Original GA scene decode: "+d.code);
        need(next.world.zones.size()==zones&&next.world.portals.size()==portals&&
            next.world.instances.size()==staticPlacements,"Original GA world extent");
        const auto field=[&](std::size_t at,const char* literal){
            const auto size=std::strlen(literal)+1u;
            need(at<=scb.size()&&size<=scb.size()-at&&
                std::memcmp(scb.data()+at,literal,size)==0,"Original GA atmosphere field differs");
        };
        field(21785,"ga.cub");field(21908,"117,107,66");field(21919,"0.4");
        field(22121,"108,108,83");field(26939,"93,93,53");field(38121,"38");
        // The malformed original section is retained by World48F. Do not move
        // its position/look/zone fields into Light9 or synthesize 36 valid
        // spotlight poses. Original Light9 default construction is a separate
        // source join, as are both .TOB animation owners and LostAI triggers.
        std::size_t collisions=0,drawables=0;
        const auto contains=[&](const std::string& name){
            const auto wanted=normalized(name);
            for(const auto& member:next.archive.entries)
                if(normalized(member.name)==wanted)return true;
            return false;
        };
        for(const auto& placement:next.world.instances){
            collisions+=contains(placement.collision_member);
            drawables+=contains(placement.geometry_member);
        }
        need(collisions==staticCollisionPlacements&&drawables==staticDrawablePlacements,
            "Original GA static source-member joins differ");
        output=std::move(next);error.clear();return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
} // namespace watchtower_source_fixture
