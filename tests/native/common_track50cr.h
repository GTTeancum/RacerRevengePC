#pragma once
// Common rewrite (W0 freeze): the ONE track of a race, for every course.
// Plan: D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\PLAN.md 2.1 steps 4-9, 2.2, 3.3.
// Governing rule: D:\Programming\GitHub\RacerRevengePC\CLAUDE.md (one common path; data differs, code does not).
//
// A Track is what the original's 1A6D80 -> 1A9EE0 track load produces for the
// race World at race+74 (1B2870): the course code, where its files come from
// (stock disc or a registry dlc:/ package), the mounted archive, the decoded
// SCB/TOB/AUB documents, every per-role count read from those files, and the
// one owner of each role. No member, function or branch here names a course or
// a pod; the course code is data (file names, receipts, the original's own A1
// PHY-class compare). Courses differ only in what their files hold.
//
// Storage rule (PLAN 2.2). Every count that sizes storage is read from the
// track's own files at cold. Storage is sized ONCE from that count through a
// Block below. Every write is bounds-checked first; a write that would not fit
// is refused and logged (common_storage_refused), never done and reported
// afterwards. Nothing here writes guest memory: the Track, the World and every
// owner below are port-owned host objects. This header is the ONE definition of
// the layout of every block it lists (BlockId); no other file invents a layout
// for these blocks. Stock values are receipts only
// (stock_course_receipts50cr.h), never sizes.
//
// Ownership (PLAN 3.1): declarations here are frozen by W0. Each declared
// function names its original address and the writer that implements it.
// Writers may add private helpers in their own files, never change these
// signatures (an interface change goes back through W0).
#include "common_race_log50cr.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/game/ai_cold_owners50iu.h"         // AiTrackNodeMetadata50IU (+ track_navigation50eu.h, undertrack_reposition50et.h)
#include "../../src/game/live_zone_membership50en.h"   // LiveZoneWorld50EN, LiveZonePortalInput50EN
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <new>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

// ---------------------------------------------------------------- role owner types
// The one owner type per role. Existing types are kept where the plan keeps the
// reader (PLAN 2.2 "Shared reader kept"); names marked NEW are produced by the
// owning writer by generalising the named existing type (W0-HANDOFF.txt).
namespace rr::assets { class OriginalStandardArchiveProvider50LA; class OriginalStandardTextureResolver50LA;
    class OriginalStandardMaterialTable50LA; struct OriginalPositionalSounds50SR; struct MaterialAudioCatalog50HI; }
namespace rr::game { class OriginalApplicationEntities50LA; class OriginalTrackFogRuntime50CR; class OriginalTrackParticleCold50LA;
    struct NativeTriggerGroup50MC; class OriginalRaceConstructor50LA; class OriginalRacerModelRenderGlobals50LA;
    struct VehicleAnimationShared49T; struct SourceLibmState49T; }
namespace rr::game::original_gc_gameplay_lighting_candidate { class Owner; }
namespace rr::platform { class AudioMixer50HD; class RaceAudioGroups50IC; }
namespace original_main_entry50KG { struct Application; }
namespace track_runtime_storage50IG { struct Storage; struct Counts; }   // R3: collision/zone storage (MC/GB aggregate shape)
namespace track_collision_world50CR { class Owner; }                     // R3 NEW: generalised mc/gb_collision_world Owner
namespace single_pass_world50LM { class Owner; }                         // R3 NEW: the one original_single_pass_world50lm.inl Owner
namespace original_track_sky50KX { class Owner; }                        // R6: zone->cube selection (prepareFromCourseData)
namespace course_trigger_sources50CT { class Owner; }                    // R5: 274E90 trigger list reader
namespace track_animation_runtime50AN { class Owner; }                   // R5: 101860 animation player
namespace track_uv_animation50uv_fixture { class Owner; }                // R5: 29B1D0/29B050 texture scroll
namespace track_atm50CR { class Owner; }                                 // R5 NEW: one ATM reader over authored data
namespace water_resources50JC { struct Owner; }                          // R7: 2745D4 water resources
namespace native_water_runtime50JC { class Owner; class Application; }   // R7: water runtime
namespace material_audio50HI { class Provider; }                         // R7: AUB 271600 provider
namespace positional_sound_rows50SR { class Owner; }                     // R7 NEW: one positional-sound owner
namespace course_trigger_actions50CT { class Owner; }                    // R5: the one trigger action owner (W0 round 2, r5 #2)
namespace track_particle_emitters50CR { class Owner; }                   // R7 NEW (W0 round 2): the one track ground-effects owner (track_particle_emitters50cr.h)
namespace common_race_world50CR { class World; }

namespace common_track50CR {
namespace log=common_race_log50CR;
using Source=common_race_log50CR::Source;
using Bytes=std::vector<std::uint8_t>;

// ---------------------------------------------------------------- course code
// Upper-case course code as data (stock "TA".."RA", add-on codes such as "NV").
// Two letters on every shipped and registered course; three bytes keep the NUL.
struct Code final {
    std::array<char,3> text{};
    static bool parse(std::string_view s,Code& out)noexcept {
        if(s.size()!=2u)return false;
        Code c;
        for(std::size_t i=0;i<2u;++i){
            char ch=s[i];
            if(ch>='a'&&ch<='z')ch=static_cast<char>(ch-'a'+'A');
            if(!((ch>='A'&&ch<='Z')||(ch>='0'&&ch<='9')))return false;
            c.text[i]=ch;
        }
        out=c;return true;
    }
    std::string_view view()const noexcept {return {text.data(),text[0]?(text[1]?2u:1u):0u};}
    // Lower-case form used by the original's file names (tracks\<code>.scb, m<code>.spl).
    std::string lower()const {
        std::string s(view());
        for(char& ch:s)if(ch>='A'&&ch<='Z')ch=static_cast<char>(ch-'A'+'a');
        return s;
    }
    bool valid()const noexcept {return view().size()==2u;}
    friend bool operator==(const Code& a,const Code& b)noexcept {return a.text==b.text;}
};

// ---------------------------------------------------------------- identity (step 4 input)
// Where this course's files are. Built once from the launch's course row
// (stock session row or registry row); nothing downstream reads which it was
// except receipt lookup and file-path building.
struct Identity final {
    Code code;
    Source source=Source::stock;
    unsigned courseNumber=0;        // Hall row order: stock 0..12, registry >=32 (PORT EXTENSION A12)
    std::string archivePath;        // TRACKS\<CODE>.RES, or dlc:/<pkg>/TRACKS/<CODE>.RES
    std::string listedSha256;       // stock: receipt only; registry: the listed digest the overlay checks
    std::uint64_t listedBytes=0;
    std::string scbMember;          // tracks\<code>.scb
    std::string tobMember;          // tracks\<code>.tob
    std::string aubMember;          // tracks\<code>.aub
    std::string mapSpline;          // tracks\m<code>.spl  (2A1160 map track)
    std::string aiSpline;           // tracks\a<code>.spl  (2A1160 AI track)
    std::string tuning;             // tracks\<code>.tun   (in-archive or the row's inherited tuning archive)
    std::string loadingPst;         // LD_<CODE>.PST (1CC900 loading window)
    std::string registryPackage;    // registry rows only: package id (data)
};
// R3: fills Identity from the launch's course row. Stock rows come from
// track_session_assets50IG::SessionAssets, registry rows from the DLC course row;
// both produce the same fields. Refuses an unknown code (log step 04).
bool identityForCourse(std::string_view code,bool registryRow,Identity& out,std::string& error)noexcept; // R3, 187330 name

// ---------------------------------------------------------------- diagnostics
struct Diagnostic final {
    log::Step step=log::Step::race1A65C0;
    std::uint32_t pc=0;             // the original address reached/refused
    bool refused=false;
    std::uint64_t value=0;          // count or bytes for the common_load line
    std::string reason;
    rr::assets::DecodeError48C decode{};
    void refuse(log::Step s,std::uint32_t at,std::string why){step=s;pc=at;refused=true;reason=std::move(why);}
};

// ---------------------------------------------------------------- blocks (reserved-block layouts)
// Every sized store of the common race path. One enumerator per block; the
// table below is the ONE definition of its name and its allocation ceiling.
// PORT STAND-IN (flagged): the ceilings are allocation guards against a
// corrupt or hostile file, not admission gates and not sizes. Each one is above
// the largest stock value plus the largest add-on value seen (the receipts) and
// is logged on every sizing (common_storage ... limit=). Where the original has
// its own limit (for example 2A1260's node limit) the owning writer replaces the
// ceiling with that limit and says so in the row comment.
enum class BlockId : std::uint8_t {
    navMapNodes,        // R4  UndertrackNode50ET per map-spline node (2A1260)        stock max 293 (MB/TC)
    navAiNodes,         // R4  UndertrackNode50ET per AI-spline node                  stock max 293
    navMapGates,        // R4  TrackNavigationNode50EU per map node (gates 2A1980)
    navAiGates,         // R4  TrackNavigationNode50EU per AI node
    navMapIndex,        // R4  nodeIndex78 per map node (1ACF70 zone index)
    navAiIndex,         // R4  nodeIndex78 per AI node
    navMapMetadata,     // R4  AiTrackNodeMetadata50IU per map node
    navAiMetadata,      // R4  AiTrackNodeMetadata50IU per AI node
    navPortals,         // R4  LiveZonePortalInput50EN per SCB portal                 stock max 256 (MB shared store)
    navFieldMask,       // R4  per map node reader field mask
    triggers,           // R5  TriggerEntryState50IN per numberoftriggers row (274E90)  stock max 22 (TC)
    triggerIgnore,      // R5  ignore-collider lists per trigger
    triggerMemberships, // R5/I2 one live-zone membership per trigger (track_runtime_storage50IG::Membership)
    lights,             // R6  one row per SCB LightN (NumberOfLights)
    fogZones,           // R6  OriginalTrackFogZone50CR per SCB zone (273C30)
    particleEmitters,   // R7  particle emitter table rows (27DE00)                     original capacity 40 (particle_emitters50PE)
    positionalSounds,   // R7  sound rows of the track (AUB/SCB sound rows)
    particleAttachments,// R7  ground-effect placements: one per TOB emitter instance (SoulTree attachment)  W0 round 2
    particleDefinitions,// R7  ground-effect definitions: one per distinct EmitterFilename (EMT)            W0 round 2
    count
};
struct BlockRow { const char* name; std::size_t ceiling; log::Writer owner; };
inline const BlockRow& blockRow(BlockId b)noexcept {
    static constexpr BlockRow rows[]={
        // nav_*: the 2A1260 reader's own guard rr::game::navigationNodeLimit50ET (1024,
        // undertrack_reposition50et.h) is stricter and wins; readNavigation2A1260 refuses (logged)
        // if a ceiling here ever drops below it.
        {"nav_map_nodes",4096u,log::Writer::R4},{"nav_ai_nodes",4096u,log::Writer::R4},
        {"nav_map_gates",4096u,log::Writer::R4},{"nav_ai_gates",4096u,log::Writer::R4},
        {"nav_map_index",4096u,log::Writer::R4},{"nav_ai_index",4096u,log::Writer::R4},
        {"nav_map_metadata",4096u,log::Writer::R4},{"nav_ai_metadata",4096u,log::Writer::R4},
        {"nav_portals",4096u,log::Writer::R4},{"nav_field_mask",4096u,log::Writer::R4},
        // triggers: allocation guard only; the original 274E90 has no limit of its own
        // (NumberOfTriggers is read as-is). NativeTriggerGroup50MC sizes with the same ceiling (block=trigger_group).
        {"triggers",1024u,log::Writer::R5},{"trigger_ignore",1024u,log::Writer::R5},{"trigger_memberships",1024u,log::Writer::R5},
        {"lights",1024u,log::Writer::R6},{"fog_zones",256u,log::Writer::R6}, // 256 = originalTrackFogZoneLimit50CR
        {"particle_emitters",40u,log::Writer::R7},{"positional_sounds",4096u,log::Writer::R7},
        // definitions <= emitter rows (27DE00 capacity 40); attachments: allocation guard (instances of those rows).
        {"particle_attachments",4096u,log::Writer::R7},{"particle_definitions",40u,log::Writer::R7},
    };
    static_assert(sizeof(rows)/sizeof(rows[0])==static_cast<std::size_t>(BlockId::count));
    const auto i=static_cast<std::size_t>(b);
    return rows[i<static_cast<std::size_t>(BlockId::count)?i:0];
}

// Storage sized once from a data count; every write bounds-checked first.
// T must be default-constructible. Not copyable (one owner per block).
template<class T> class Block final {
    std::unique_ptr<T[]> data_;
    std::size_t capacity_=0,used_=0;
    BlockId id_=BlockId::count;
    bool sized_=false;
public:
    Block()=default;
    Block(const Block&)=delete;Block& operator=(const Block&)=delete;
    Block(Block&&)noexcept=default;Block& operator=(Block&&)noexcept=default;
    // Size once from the count read from the track's files. Refuses a second
    // sizing and a count above the block's ceiling. Logs common_storage.
    bool size(BlockId id,std::size_t count,std::uint32_t pc)noexcept {
        const auto& row=blockRow(id);
        if(sized_||count>row.ceiling){
            log::storageRefused(row.name,count,sized_?capacity_:row.ceiling,pc);
            return false;
        }
        std::unique_ptr<T[]> d;
        if(count){d.reset(new(std::nothrow) T[count]());if(!d){log::storageRefused(row.name,count,0,pc);return false;}}
        data_=std::move(d);capacity_=count;used_=0;id_=id;sized_=true;
        log::storage(row.name,count,count*sizeof(T),row.ceiling);
        return true;
    }
    bool sized()const noexcept {return sized_;}
    std::size_t capacity()const noexcept {return capacity_;}
    std::size_t size()const noexcept {return used_;}
    // Bounds-checked write at index; extends the live prefix. Refused (logged) when it would not fit.
    bool write(std::size_t index,const T& value,std::uint32_t pc)noexcept(std::is_nothrow_copy_assignable_v<T>) {
        if(!sized_||index>=capacity_){
            log::storageRefused(sized_?blockRow(id_).name:"unsized",index+1u,capacity_,pc);
            return false;
        }
        data_[index]=value;
        if(index>=used_)used_=index+1u;
        return true;
    }
    bool append(const T& value,std::uint32_t pc)noexcept(std::is_nothrow_copy_assignable_v<T>) {return write(used_,value,pc);}
    // Bounds-checked slot access for in-place construction of non-copyable rows.
    T* slot(std::size_t index,std::uint32_t pc)noexcept {
        if(!sized_||index>=capacity_){
            log::storageRefused(sized_?blockRow(id_).name:"unsized",index+1u,capacity_,pc);
            return nullptr;
        }
        if(index>=used_)used_=index+1u;
        return &data_[index];
    }
    const T* at(std::size_t index)const noexcept {return index<used_?&data_[index]:nullptr;}
    T* at(std::size_t index)noexcept {return index<used_?&data_[index]:nullptr;}
    const T* data()const noexcept {return data_.get();}
    T* data()noexcept {return data_.get();}
    std::span<const T> live()const noexcept {return {data_.get(),used_};}
    std::span<T> live()noexcept {return {data_.get(),used_};}
    void release()noexcept {data_.reset();capacity_=used_=0;sized_=false;}
};

// ---------------------------------------------------------------- counts
// Every per-role count, read from the track's own files at cold (2.2). These
// size the blocks and the role owners; they are also the values printed on the
// common_track_data lines and compared with the stock receipts.
struct Counts final {
    std::uint32_t zones=0;            // SCB zones (270900)
    std::uint32_t portals=0;          // SCB portals
    std::uint32_t portalLinks=0;      // SCB portal links
    std::uint32_t instances=0;        // SCB instances / placements (274080)
    std::uint32_t collisions=0;       // SCB collision objects (271230)
    std::uint32_t terrain=0;          // terrain owners (271370)
    std::uint32_t physicsObjects=0;   // TOB physics objects (2718C0)
    std::uint32_t staticModels=0;     // TOB StaticModelParams rows (271A00)
    std::uint32_t triggers=0;         // TOB numberoftriggers (274E90)
    std::uint32_t triggerActions=0;   // total triggerNactionM rows
    std::uint32_t cuts=0;             // TOB [CSCameras].NumberOfCameras (World+3E4, 174E58); absent block = 0; read at 05.02
    std::uint32_t lights=0;           // SCB NumberOfLights
    std::uint32_t navMapNodes=0;      // m<code>.spl nodes (2A1260)
    std::uint32_t navAiNodes=0;       // a<code>.spl nodes
    std::uint32_t lapMarkers=0;       // ProgressNode markers (2A1980..19C0); counted at step 08, printed by Track::logTrackData
    std::uint32_t particleEmitters=0; // particle emitter rows (27DE00)
    std::uint32_t particleAttachments=0; // ground-effect placements (emitter instances), 05.06 (W0 round 2)
    std::uint32_t aubRows=0;          // AUB catalog rows (271600)
    std::uint32_t positionalSounds=0; // track sound rows
    std::uint32_t materials=0;        // materials (274160)
    std::uint32_t textures=0;         // textures (14D370/29E340)
    std::uint32_t atmRows=0;          // animated-texture rows
    std::uint32_t animations=0;       // track animation rows (101860), read at 05.02 ([Animations] NumberOfAnimations) so 05.13 can size its lane
    std::uint32_t skyCubes=0;         // SCB Environment sky cubes (274AE0)
    std::uint64_t storageBytes=0;     // collision/zone storage bytes (track_runtime_storage50IG)
    bool water=false;                 // SCB Environment names water (A9: 274570 none / 2745D4 factory)
};

// ---------------------------------------------------------------- documents
// Decoded members of the archive, read once at cold and borrowed by readers.
struct Documents final {
    Bytes scb,tob,aub,mapSpline,aiSpline,tuning;
    rr::assets::World48F world;                    // SCB (270900 SceneInfo onward)
    rr::assets::ParameterDocument48F tobDocument;  // TOB parameters
    rr::assets::ParameterDocument48F aubDocument;  // AUB parameters
    std::string waterName;                         // SCB Environment water name; empty = no water (A9)
    std::uint32_t waterLogicalOffset=0;
    // 1B44A0..1B4528: TOB [PodRacer] and [Rain], read at 05.02 with 158B30 (158920 rule,
    // T/F/ON/OFF/Y/N or strtol base 0; absent field = the original default).
    std::uint32_t trackEmitsDust51C=0;   // [PodRacer] TrackEmitsDust -> World+51C (default 0)
    std::int32_t ipuCacheSize524=3;      // [PodRacer] IPUCacheSize   -> World+524 (default 3)
    bool podRacerBlock=false;            // [PodRacer] present (157D00)
    bool rainActive=false;               // [Rain] RainActive (default 0): 2D0290 rain owner -> World+534
};

// ---------------------------------------------------------------- archive (step 4)
// The mounted course archive: TRACKS\<CODE>.RES or the registry dlc:/ overlay,
// mounted through the Application's standard archive provider (12CEA0/132A20/
// 132E40) under a race mount lease, and released at teardown through 1316A0
// (native_course_archive_release50PD, which prints course_archive_release).
// The archive's own bytes/SHA are receipts for a stock course (role=archive).
class Archive final {
public:
    struct State;                               // R3 defines (common_track_load50cr.cpp)
    Archive()noexcept;
    ~Archive();
    Archive(const Archive&)=delete;Archive& operator=(const Archive&)=delete;
    // R3, 1CC900/187330/12CEA0/132A20/132E40. Reads, decodes and mounts the archive
    // named by `identity`; logs the archive receipt; refuses on read/decode/mount failure.
    bool open(const Identity& identity,
        const std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA>& provider,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        Diagnostic& d)noexcept;
    bool opened()const noexcept;                                            // R3
    // R3, 132820 lookup by member name. `limit` bounds the copy (refused when larger).
    bool member(std::string_view name,Bytes& out,std::size_t limit,Diagnostic& d)const noexcept;
    bool exists(std::string_view name)const noexcept;                       // R3
    const rr::assets::ResArchive48C* decoded()const noexcept;               // R3
    std::uint64_t bytes()const noexcept;                                    // R3, actual archive bytes
    const std::string& sha256()const noexcept;                              // R3, actual archive SHA-256
    std::size_t memberCount()const noexcept;                                // R3
    // R3, teardown 1316A0: retire this race's mounts. Refused (logged) while a
    // borrower of the bank's files is alive; the bank then stays mounted.
    bool release()noexcept;
private:
    std::unique_ptr<State> state_;
};

// ---------------------------------------------------------------- navigation (step 8)
// One navigation store for every course, sized from the spline files' node
// counts (2A1160/2A1260), with the 1ACF70 zone index bound to the collision
// world. Replaces TrackNavigation50EU fixed arrays and the five per-course
// navigation_storage_detail50{MB,TC,TB,GB,MC} stores and views. Layout owned here.
enum class NavigationRole : std::uint8_t { map, ai };
struct NavigationGraph final {
    Block<rr::game::UndertrackNode50ET> nodes;          // 2A1260 nodes (live count = file's node count)
    Block<rr::game::TrackNavigationNode50EU> gates;     // gate10/20/30/40, height50, left54/right58, branch68, flags79
    Block<std::uint8_t> nodeIndex78;                    // 1ACF70 zone index per node
    Block<rr::game::AiTrackNodeMetadata50IU> metadata;  // AI metadata per node
    float trackLength3C=0.f;
    std::uint32_t count=0;
    bool gatesPrepared=false,indicesPrepared=false,metadataBound=false;
};
struct NavigationStorage final {
    NavigationGraph map,ai;
    Block<rr::game::LiveZonePortalInput50EN> authoredPortals;  // SCB portals for the zone index
    Block<std::uint32_t> mapFieldMask;
    std::uint32_t authoredZoneCount=0,authoredPortalCount=0;
    // 1ACF70 binding to the one collision world. The lease keeps the world alive
    // while any view is runtime-valid; generation detects a rebuilt world.
    std::shared_ptr<const void> worldLease;
    const rr::game::LiveZoneWorld50EN* world=nullptr;
    std::uint64_t generation=0;
    bool gatesReady=false;
};
// The one read view (map or AI) handed to AI, lap, wrong-way, camera radar and
// race-session code. Same accessor set as rr::game::TrackNavigationView50EU.
class NavigationView final {
    std::shared_ptr<const NavigationStorage> storage_;
    NavigationRole role_=NavigationRole::map;
    const NavigationGraph* graph()const noexcept {
        return storage_?(role_==NavigationRole::map?&storage_->map:&storage_->ai):nullptr;
    }
public:
    NavigationView()=default;
    NavigationView(std::shared_ptr<const NavigationStorage> s,NavigationRole role)noexcept:storage_(std::move(s)),role_(role){}
    NavigationRole role()const noexcept {return role_;}
    std::uint32_t count()const noexcept {const auto* g=graph();return g?g->count:0u;}
    std::size_t capacity()const noexcept {const auto* g=graph();return g?g->nodes.capacity():0u;}
    const rr::game::UndertrackNode50ET* nodes()const noexcept {const auto* g=graph();return g?g->nodes.data():nullptr;}
    const rr::game::TrackNavigationNode50EU* gates()const noexcept {return gatesPrepared()?graph()->gates.data():nullptr;}
    const std::uint8_t* nodeIndex78()const noexcept {return indicesPrepared()?graph()->nodeIndex78.data():nullptr;}
    const rr::game::AiTrackNodeMetadata50IU* metadata()const noexcept {
        const auto* g=graph();return g&&g->metadataBound?g->metadata.data():nullptr;
    }
    float trackLength3C()const noexcept {const auto* g=graph();return g?g->trackLength3C:0.f;}
    bool gatesPrepared()const noexcept {const auto* g=graph();return g&&storage_->gatesReady&&g->gatesPrepared;}
    bool indicesPrepared()const noexcept {
        const auto* g=graph();return gatesPrepared()&&g->indicesPrepared&&storage_->world&&storage_->worldLease;
    }
    bool runtimeValid()const noexcept {return indicesPrepared()&&count()&&count()<=capacity();}
    std::uint64_t generation()const noexcept {return indicesPrepared()?storage_->generation:0u;}
    bool sameOwner(const NavigationView& o)const noexcept {return runtimeValid()&&o.runtimeValid()&&storage_==o.storage_;}
    bool sameWorld(const rr::game::LiveZoneWorld50EN& w)const noexcept {return runtimeValid()&&storage_->world==&w;}
    const NavigationStorage* storage()const noexcept {return storage_.get();}
};

// ---------------------------------------------------------------- role owners
// The one owner of each role for this race's track. Shared pointers so the
// World, the camera session, the renderer request and the frame owners borrow
// the same object; the Track's release order (Track::release) is the one closer.
struct Roles final {
    // R3: published after the collision world's prepareStorage (I2, after step 10) by
    // track_collision_world50CR::publishStorageRole(track): an alias of the collision world's
    // one Storage (its racer rows are sized from the roster). 05.03 sizes the plan and logs it.
    std::shared_ptr<track_runtime_storage50IG::Storage> storage;
    std::shared_ptr<track_collision_world50CR::Owner> collision;                     // R3  05.03/05.04 one collision world, lease, query bracket, lift bind
    std::shared_ptr<single_pass_world50LM::Owner> staticWorld;                       // R3  05.07 instances + single-pass decoder (mesh, bank, spans, selections)
    std::shared_ptr<rr::game::OriginalTrackFogRuntime50CR> fog;                      // R6  05.05 273C30 (borrowed by camera + renderer)
    std::shared_ptr<rr::game::original_gc_gameplay_lighting_candidate::Owner> lights; // R6 05.13 one prepare over SCB Lights
    std::shared_ptr<original_track_sky50KX::Owner> sky;                              // R6  05.11 274AE0
    std::shared_ptr<const course_trigger_sources50CT::Owner> triggerSource;          // R5  05.09 274E90 rows
    std::shared_ptr<rr::game::NativeTriggerGroup50MC> triggers;                      // R5  05.09 2A00A0/2A0E20/2A0B30 one group (50MC shape)
    // R5 type, created by I2 after step 08 (needs the AI node count) and bound after step 10
    // (racer identities); null when the track has no triggers. The collision world borrows the
    // same pointer (StorageRequest.actions). Track::release drops this reference first; the
    // collision world closes the owner at its retirement (W0 round 2, r5 #2 / r3 #3).
    std::shared_ptr<course_trigger_actions50CT::Owner> triggerActions;
    std::shared_ptr<track_animation_runtime50AN::Owner> animation;                   // R5  05.14 101860 family
    std::shared_ptr<track_uv_animation50uv_fixture::Owner> uvAnimation;              // R5  05.14 29B1D0
    std::shared_ptr<track_atm50CR::Owner> atm;                                       // R5  05.14 authored animated textures
    std::shared_ptr<rr::game::OriginalTrackParticleCold50LA> particles;              // R7  05.06 1B4DC0 manager (renderer particle window reads it)
    // R7 05.06 (W0 round 2): the one ground-effects owner over the TOB [ParticleEmitters] rows
    // (27DE00/27C600/27C7B0, EMT definitions, SoulTree attachments, 15AEA0), emitting into
    // `particles` by each row's TypeID (the original's switch: data). Null when the TOB has no rows.
    std::shared_ptr<track_particle_emitters50CR::Owner> emitters;
    std::shared_ptr<const water_resources50JC::Owner> waterResources;                // R7  05.10 null when the SCB names no water (A9)
    std::shared_ptr<native_water_runtime50JC::Owner> water;                          // R7  05.10 runtime (null when no water)
    std::shared_ptr<material_audio50HI::Provider> audio;                             // R7  05.12 AUB catalog + material audio
    std::shared_ptr<positional_sound_rows50SR::Owner> sounds;                        // R7  05.12 one positional-sound owner
    std::shared_ptr<NavigationStorage> navigation;                                   // R4  08 one navigation store
};

// ---------------------------------------------------------------- load inputs
// What the 1A9EE0 chain borrows from its caller (I2). Synchronous borrows only.
// W0 round 2 (r11 #1, i2 #2): pointers, so the host data check (runHostDataCheck)
// can run the SAME readers with no Application, race or World. In a race all three
// are non-null (Load::run refuses otherwise); only Load::run reads hostDataCheck and
// decides which publishing parts to skip; a reader never branches on it.
struct LoadInputs final {
    const original_main_entry50KG::Application* application=nullptr;
    rr::game::OriginalRaceConstructor50LA* race=nullptr;
    const common_race_world50CR::World* world=nullptr;       // race+74 (step 2 already ran); read only
    bool hostDataCheck=false;                                 // R11 track_check.cpp only; never set in a race
    std::shared_ptr<rr::game::OriginalApplicationEntities50LA> entities;
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> materials382B38;
    rr::platform::AudioMixer50HD* mixer=nullptr;
    rr::platform::RaceAudioGroups50IC* audioGroups=nullptr;
    std::uint32_t racers=0;                                   // field size (1AD520: 1..8)
    std::uint64_t generation=0;                               // session generation
    std::string audioRoot;                                    // AUB PCM root (material audio)
    // The session frame's ONE shared animation state (libm + motion), the same objects the
    // racers' animation uses (errno/motion parity). I2 fills &s.animationShared. 05.14 refuses
    // (logged) when null. W0 round 2 (r5 #1).
    rr::game::VehicleAnimationShared49T* animationShared=nullptr;
};

class Track;
// ---------------------------------------------------------------- role readers
// The 1A9EE0 sub-steps (PLAN 2.2), each implemented by its role writer and called
// in this order by Load::run (R3). Each one reads the Track's documents, sizes its
// storage from the counts it reads, publishes its owner into Track::roles, logs
// its common_load and common_track_data lines, and refuses (Diagnostic) rather
// than guessing. None may branch on the course code.
bool readScene270900(Track&,const LoadInputs&,Diagnostic&)noexcept;          // R3  05.01 1AA078 SCB bind, 270900 SceneInfo; zones/portals/instances/collisions counts
bool readTob1B4330(Track&,const LoadInputs&,Diagnostic&)noexcept;            // R3  05.02 TOB decode; cuts/triggers/physics/static-model counts
bool buildPhysicsIndex270CDC(Track&,const LoadInputs&,Diagnostic&)noexcept;  // R3  05.03 270CDC/27C110 + storage sized from Counts
bool buildCollision271230(Track&,const LoadInputs&,Diagnostic&)noexcept;     // R3  05.04 271230/27BB40 collision zones; publishes roles.collision
bool readFog273C30(Track&,const LoadInputs&,Diagnostic&)noexcept;            // R6  05.05 273C30 (original_track_fog50cr.h)
bool prepareParticles1B4DC0(Track&,const LoadInputs&,Diagnostic&)noexcept;   // R7  05.06 1B4DC0 + 274370 over the 27DE00 reader
bool readInstances274080(Track&,const LoadInputs&,Diagnostic&)noexcept;      // R3  05.07 274080 instances, standard models
bool readMaterials274160(Track&,const LoadInputs&,Diagnostic&)noexcept;      // R7  05.08 274160, 14D370/29E340; selectMaSa + authored AA1 on every pass
bool readTriggers274E90(Track&,const LoadInputs&,Diagnostic&)noexcept;       // R5  05.09 274E90 -> one NativeTriggerGroup50MC sized from numberoftriggers
bool prepareWater274570(Track&,const LoadInputs&,Diagnostic&)noexcept;       // R7  05.10 274570 (no water, returns ok) / 2745D4 water factory
bool readSky274AE0(Track&,const LoadInputs&,Diagnostic&)noexcept;            // R6  05.11 274AE0 zone->cube selection
bool readAub271600(Track&,const LoadInputs&,Diagnostic&)noexcept;            // R7  05.12 271600/271674..271804 one AUB catalog + positional sounds
bool prepareLights(Track&,const LoadInputs&,Diagnostic&)noexcept;            // R6  05.13 SCB Lights/LightN, all light types and flashers (prepareMaSa path)
bool prepareAnimation101860(Track&,const LoadInputs&,Diagnostic&)noexcept;   // R5  05.14 101860 animation, ATM, 29B1D0 UV
bool finishTail1AA400(Track&,const LoadInputs&,Diagnostic&)noexcept;         // R3  05.15 1AA400..1AA508 tail (restores model directory)
// Step 8, R4. Cold order (W0 round 2, i2 #6): readNavigation2A1260 runs right after the
// load; grid input, roster (10) and collision storage come next, then bindNavigation1ACF70,
// all before the camera session (06), which needs a runtime-valid map view. The log steps
// keep their PLAN numbers.
// `libm` is the session's ONE serial libm owner (s.animationShared.libm): the 2A1D80 gate
// build runs the source acos on it (W0 round 2, r4 #1).
bool readNavigation2A1260(Track&,rr::game::SourceLibmState49T& libm,Diagnostic&)noexcept; // R4  08 2A1160/2A1260 both splines; sizes nav blocks from node counts
bool bindNavigation1ACF70(Track&,Diagnostic&)noexcept;                       // R4  08 1ACF70 zone index against roles.collision's live world
NavigationView navigationView(const Track&,NavigationRole)noexcept;          // R4  view over roles.navigation

// ---------------------------------------------------------------- the track
class Track final {
public:
    Identity identity;
    Archive archive;
    Documents documents;
    Counts counts;
    Roles roles;
    Track()=default;
    Track(const Track&)=delete;Track& operator=(const Track&)=delete;
    ~Track();                                                  // R3 (calls release when still loaded)
    // Course code as data; for file names, receipts and the A1 PHY-class compare only.
    std::string_view code()const noexcept {return identity.code.view();}
    Source source()const noexcept {return identity.source;}
    bool loaded()const noexcept {return loaded_;}
    bool hasWater()const noexcept {return counts.water;}       // A9: the original branches on this data
    // The cut count World+3E4 reads (174E58): TOB [CSCameras].NumberOfCameras.
    std::uint32_t cutCount3E4()const noexcept {return counts.cuts;}
    // R3: reverse-order release of every role owner (teardown step 18), then the
    // archive (1316A0). Logs common_teardown role=<role> released=1|0 per role.
    bool release()noexcept;
    // R3: one common_track_data line per role (PLAN 4.1), looking up the stock
    // receipt for a stock course (stock_course_receipts50CR::find). Called once
    // per race after Load::run and step 8.
    void logTrackData()const;
private:
    friend class Load;
    bool loaded_=false;
};

// ---------------------------------------------------------------- the load (steps 4-5)
// R3 (common_track_load50cr.cpp): 1A6D80 -> 1A9EE0, the generic part of today's
// RA chain (original_race_world_load_prefix50la.cpp, original_race_world_tob_prefix50la.cpp,
// original_race_world_native_tail50la.inl) with every RA constant replaced by a
// data read. Runs step 04 then the 05.xx readers above in order and logs one
// common_load line per step. Stops at the first refusal.
class Load final {
public:
    bool run(Track& track,const Identity& identity,
        const std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA>& provider,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        const LoadInputs& inputs,Diagnostic& d)noexcept;       // R3, 1A6D80/1A9EE0
    log::Step reached()const noexcept {return reached_;}
    std::uint32_t pc()const noexcept {return pc_;}
private:
    log::Step reached_=log::Step::archive12CEA0;
    std::uint32_t pc_=0;
};

// ---------------------------------------------------------------- host data check (PLAN 4.3)
// R3 (common_track_load50cr.cpp), W0 round 2 (r11 #1): the entry R11's track_check.cpp
// calls. Builds the host archive provider / texture resolver pair the same way the
// Application's fresh native service does, then runs Load::run with
// LoadInputs{.hostDataCheck=true} (steps 04, 05.01..05.15), readNavigation2A1260 (08,
// with one check-local libm state), and track.logTrackData(). The SAME reader functions
// as a race; no second count reader. Stops at the first refusal (d). Never used in a race.
bool runHostDataCheck(Track& track,const Identity& identity,Diagnostic& d)noexcept;

} // namespace common_track50CR
