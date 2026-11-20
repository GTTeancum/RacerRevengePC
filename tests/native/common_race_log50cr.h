#pragma once
// Common rewrite (W0 freeze): the ONE log vocabulary of the common race path.
// Plan: D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\PLAN.md section 4.1.
//
// Every line is one grep-friendly record "<key> name=value ...". The run checker
// (R11 check-round.ps1) and the host data check (R11 track_check.cpp) parse these
// exact keys and field names; do not add free-form text to them. Every common
// step names its original address, its role and its owning writer, so the first
// `result=refused` line answers "where did it stop and who owns it".
//
// Header-only. No guest memory, no allocation beyond std::string formatting.
// All writers emit through these helpers; no writer prints a common_* key by hand.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

namespace common_race_log50CR {

// ---------------------------------------------------------------- writers
// Owning writer of a step/role (PLAN 3.1). Logged so a failing run names its owner.
enum class Writer : std::uint8_t { W0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, I1, I2, I3, I4 };
inline const char* writerName(Writer w) noexcept {
    static constexpr const char* names[]={"W0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","I1","I2","I3","I4"};
    const auto i=static_cast<std::size_t>(w);
    return i<sizeof(names)/sizeof(names[0])?names[i]:"?";
}

// ---------------------------------------------------------------- course source
// Where a course or pod's files come from. Data only: never selects code.
enum class Source : std::uint8_t { stock, registry };
inline const char* sourceName(Source s) noexcept {return s==Source::registry?"registry":"stock";}

// ---------------------------------------------------------------- load steps
// One row per PLAN 2.1 step and per 1A9EE0 sub-step (PLAN 2.2), in the original
// order. `number` is the printed step=<NN> field ("05.03" = 1A9EE0 sub-step 3).
// The table is the single definition of each step's original address, role and
// owner; writers pass the Step, never their own pc/role strings.
enum class Step : std::uint8_t {
    race1A65C0,            // 01 1A65C0 race; 1A6AD4..1A6BA8 (2410E0,24B6E0,1AA5A0,2350D0)
    world1A6BC0,           // 02 1A6BC0 new 0x550, 1B2870 World at race+74
    cameraRoot1A9240,      // 03 1A9240 camera root
    music1A39F0,           // 03 1A39F0/1A3C00 music manager
    archive12CEA0,         // 04 1CC900 loading window, 187330 name, 12CEA0/132A20/132E40 mount
    trackLoad1A9EE0,       // 05 1A6D80 -> 1A9EE0 track load (summary line)
    scene270900,           // 05.01 1AA078 SCB bind, 270900 SceneInfo
    tob1B4330,             // 05.02 world TOB
    physicsIndex270CDC,    // 05.03 270CDC/27C110 physics index (+ storage sizing)
    collision271230,       // 05.04 271230/27BB40 collision zones
    fog273C30,             // 05.05 273C30 fog
    particles1B4DC0,       // 05.06 1B4DC0 particle manager, 274370 services
    instances274080,       // 05.07 274080 instances / standard models
    materials274160,       // 05.08 274160 materials, 14D370/29E340 textures
    triggers274E90,        // 05.09 274E90 trigger list
    water274570,           // 05.10 274570 (none) / 2745D4 (water factory)
    sky274AE0,             // 05.11 274AE0 sky
    aub271600,             // 05.12 271600 AUB
    lights,                // 05.13 SCB Lights/LightN
    animation101860,       // 05.14 101860 animation, ATM, 29B1D0 UV
    tail1AA400,            // 05.15 1AA400..1AA508 tail
    cameraSession1A95B0,   // 06 1A6DC0 -> 1A95B0 -> 174DF0/174E58 -> 174A30 (+174FA4)
    vcr287430,             // 07 287430/287710/2878F0 VCR channels
    navigation2A1260,      // 08 2A1160/2A1260 splines, 1ACF70 zone index
    cutResources175510,    // 09 175510 cut resources, 1AAB30
    roster1AA510,          // 10 1AA510 -> 1AD520 -> 1AE280 (humans 1ADF80, AI 1AE3CC -> 2129C0)
    speech1AF330,          // 11 1AF330, 2C2B50, 2C4B30 speech banks (1AED30 is the start lock, race start step; R9 checked)
    cameraRacers1750A0,    // 12 1750A0/1752D0 camera racer binding
    raceUi1A8A40,          // 13 race vtable+74 1A8A40: Pause100, Results104, F8, VCR78, HUD
    raceStart,             // 14 race start tail (1A6AA0 fresh / 1AC8E0 restart)
    endDispatch1A7DD0,     // 16 1A7DD0 -> 1AB520 -> 1ABBC0 / 1AB5BC
    pause1A87A0,           // 17 1A87A0 / 1A7C1C / 1BD7C0 / 1BD8CC / 1BDA74
    teardown,              // 18 26D070, 1A4AB0(0.5), 1A4B90(2), 267BE0, 2AED60, 1BE830, 17DE10
    count
};
struct StepRow { const char* number; std::uint32_t pc; const char* role; Writer owner; };
inline const StepRow& stepRow(Step s) noexcept {
    static constexpr StepRow rows[]={
        {"01",   0x1A65C0u,"race",           Writer::R1},
        {"02",   0x1A6BC0u,"world",          Writer::R1},
        {"03",   0x1A9240u,"camera_root",    Writer::R2},
        {"03",   0x1A39F0u,"music",          Writer::R1},
        {"04",   0x12CEA0u,"archive",        Writer::R3},
        {"05",   0x1A9EE0u,"track",          Writer::R3},
        {"05.01",0x270900u,"scene",          Writer::R3},
        {"05.02",0x1B4330u,"tob",            Writer::R3},
        {"05.03",0x270CDCu,"physics_index",  Writer::R3},
        {"05.04",0x271230u,"collision",      Writer::R3},
        {"05.05",0x273C30u,"fog",            Writer::R6},
        {"05.06",0x1B4DC0u,"particles",      Writer::R7},
        {"05.07",0x274080u,"instances",      Writer::R3},
        {"05.08",0x274160u,"materials",      Writer::R7},
        {"05.09",0x274E90u,"triggers",       Writer::R5},
        {"05.10",0x274570u,"water",          Writer::R7},
        {"05.11",0x274AE0u,"sky",            Writer::R6},
        {"05.12",0x271600u,"aub",            Writer::R7},
        {"05.13",0x2783D0u,"lights",         Writer::R6},   // the numeric SCB light reader (W0 round 2, r6 #2)
        {"05.14",0x101860u,"animation",      Writer::R5},
        {"05.15",0x1AA400u,"tail",           Writer::R3},
        {"06",   0x1A95B0u,"camera_session", Writer::R2},
        {"07",   0x287430u,"vcr",            Writer::R1},
        {"08",   0x2A1260u,"navigation",     Writer::R4},
        {"09",   0x175510u,"cut_resources",  Writer::R2},
        {"10",   0x1AA510u,"roster",         Writer::R10},
        {"11",   0x1AF330u,"speech",         Writer::I2},    // logged by the caller of the live-audio speech load (I2 world.cpp), W0 round 2
        {"12",   0x1750A0u,"camera_racers",  Writer::R2},
        {"13",   0x1A8A40u,"race_ui",        Writer::R1},
        {"14",   0x1AC8E0u,"race_start",     Writer::R9},
        {"16",   0x1A7DD0u,"race_end",       Writer::R9},
        {"17",   0x1A87A0u,"pause",          Writer::R9},
        {"18",   0x17DE10u,"teardown",       Writer::R9},
    };
    static_assert(sizeof(rows)/sizeof(rows[0])==static_cast<std::size_t>(Step::count));
    const auto i=static_cast<std::size_t>(s);
    return rows[i<static_cast<std::size_t>(Step::count)?i:0];
}

// ---------------------------------------------------------------- track-data roles
// PLAN 4.1 common_track_data role names (one line per role per race). `archive`
// is added for the archive bytes/SHA receipt (PLAN 4.2 last bullet).
enum class Role : std::uint8_t {
    fog,lights,cuts,triggers,nav_nodes,ai_nodes,lap_markers,zones,instances,collisions,
    storage,water,particles,sky,aub,atm,materials,archive,count
};
struct RoleRow { const char* name; Writer owner; };
inline const RoleRow& roleRow(Role r) noexcept {
    static constexpr RoleRow rows[]={
        {"fog",Writer::R6},{"lights",Writer::R6},{"cuts",Writer::R2},{"triggers",Writer::R5},
        {"nav_nodes",Writer::R4},{"ai_nodes",Writer::R4},{"lap_markers",Writer::R4},{"zones",Writer::R3},
        {"instances",Writer::R3},{"collisions",Writer::R3},{"storage",Writer::R3},{"water",Writer::R7},
        {"particles",Writer::R7},{"sky",Writer::R6},{"aub",Writer::R7},{"atm",Writer::R5},
        {"materials",Writer::R7},{"archive",Writer::R3},
    };
    static_assert(sizeof(rows)/sizeof(rows[0])==static_cast<std::size_t>(Role::count));
    const auto i=static_cast<std::size_t>(r);
    return rows[i<static_cast<std::size_t>(Role::count)?i:0];
}
inline const char* roleName(Role r) noexcept {return roleRow(r).name;}

// ---------------------------------------------------------------- run state
// First refusal of the process run, for common_run_summary first_refusal=.
// Host-side bookkeeping only (one mutex-guarded record); never game memory.
struct FirstRefusal {
    bool set=false;
    std::string step;     // "05.09"
    std::string role;     // "triggers"
    std::string owner;    // "R5"
    std::uint32_t pc=0;
    std::string reason;
};
inline std::mutex& runMutex() noexcept {static std::mutex m;return m;}
inline FirstRefusal& firstRefusalStorage() noexcept {static FirstRefusal f;return f;}
inline std::uint64_t& refusalCount() noexcept {static std::uint64_t n=0;return n;}
inline void noteRefusal(const char* step,const char* role,Writer owner,std::uint32_t pc,std::string_view reason){
    std::lock_guard<std::mutex> lock(runMutex());
    ++refusalCount();
    auto& f=firstRefusalStorage();
    if(f.set)return;
    f.set=true;f.step=step?step:"?";f.role=role?role:"?";f.owner=writerName(owner);f.pc=pc;f.reason=std::string(reason);
}
inline FirstRefusal firstRefusal(){std::lock_guard<std::mutex> lock(runMutex());return firstRefusalStorage();}

// ---------------------------------------------------------------- formatting
namespace detail {
inline void hex(std::ostream& o,std::uint32_t v){
    char b[16];std::snprintf(b,sizeof b,"%X",static_cast<unsigned>(v));o<<b;
}
// Quoted text for reason="..." fields: double quotes and newlines replaced.
inline void quoted(std::ostream& o,std::string_view s){
    o<<'"';
    for(char c:s)o<<((c=='"'||c=='\n'||c=='\r')?'\'':c);
    o<<'"';
}
inline void emit(std::ostream* sink,const std::ostringstream& line){
    std::ostream& o=sink?*sink:std::cerr;
    o<<line.str()<<'\n';o.flush();
}
}

// common_race_begin run=<n> course=<code> source=stock|registry mode34=<m> subtype=<11258> p34=<v> humans=<n> field=<n> pods=<id,id,...>
struct RaceBegin {
    std::uint64_t run=0;
    std::string_view course;      // upper-case code as data ("TA", "NV")
    Source source=Source::stock;
    std::uint32_t mode34=0;       // game34 (Application configuration34)
    std::uint32_t subtype11258=0; // launch subtype (1 race, 2 practice, 3 Time Trial)
    std::uint32_t p34=0;          // profile +34 (2 tournament)
    std::uint32_t humans34=0;
    std::uint32_t field=0;
    const std::uint32_t* pods=nullptr;std::size_t podCount=0; // racer60 per slot
};
inline void raceBegin(const RaceBegin& v,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_race_begin run="<<v.run<<" course="<<v.course<<" source="<<sourceName(v.source)
     <<" mode34="<<v.mode34<<" subtype="<<v.subtype11258<<" p34="<<v.p34<<" humans="<<v.humans34
     <<" field="<<v.field<<" pods=";
    for(std::size_t i=0;i<v.podCount;++i)o<<(i?",":"")<<v.pods[i];
    detail::emit(sink,o);
}

// common_load step=<NN> pc=<original> role=<role> owner=<writer> result=ok|refused value=<count/bytes> reason="<text>"
// A refused step is also recorded as the run's first refusal (if none yet).
inline void load(Step s,bool ok,std::uint64_t value,std::string_view reason={},std::ostream* sink=nullptr,
    std::uint32_t pcOverride=0){
    const auto& r=stepRow(s);
    const std::uint32_t pc=pcOverride?pcOverride:r.pc;
    std::ostringstream o;
    o<<"common_load step="<<r.number<<" pc=";detail::hex(o,pc);
    o<<" role="<<r.role<<" owner="<<writerName(r.owner)<<" result="<<(ok?"ok":"refused")<<" value="<<value<<" reason=";
    detail::quoted(o,reason);
    detail::emit(sink,o);
    if(!ok)noteRefusal(r.number,r.role,r.owner,pc,reason);
}

// common_track_data role=<role> course=<code> source=<file> value=<v> receipt=match|differs(<stock>)|none
// `stock` is the receipt value (stock_course_receipts50CR::find(course,role)->value)
// for a stock course; pass hasStock=false for an add-on course or a role without a
// receipt row. Receipts never size anything (PLAN 4.2).
// Receipt keys are stock_course_receipts50CR role strings: the base role name
// (Role below) or a dotted sub-role ("zones.portals", "ai_nodes.aspl_bytes");
// the archive role's receipts are "archive_size" (value) and "archive_sha256"
// (stock_course_receipts50CR::findHash). Use trackDataNamed for sub-roles.
inline void trackDataNamed(std::string_view role,std::string_view course,std::string_view file,std::uint64_t value,
    bool hasStock,std::uint64_t stock,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_track_data role="<<role<<" course="<<course<<" source="<<(file.empty()?"-":file)
     <<" value="<<value<<" receipt=";
    if(!hasStock)o<<"none";
    else if(stock==value)o<<"match";
    else o<<"differs("<<stock<<')';
    detail::emit(sink,o);
}
inline void trackData(Role role,std::string_view course,std::string_view file,std::uint64_t value,
    bool hasStock,std::uint64_t stock,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_track_data role="<<roleName(role)<<" course="<<course<<" source="<<(file.empty()?"-":file)
     <<" value="<<value<<" receipt=";
    if(!hasStock)o<<"none";
    else if(stock==value)o<<"match";
    else o<<"differs("<<stock<<')';
    detail::emit(sink,o);
}

// common_storage block=<name> count=<n> bytes=<n> limit=<n>
inline void storage(std::string_view block,std::size_t count,std::size_t bytes,std::size_t limit,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_storage block="<<block<<" count="<<count<<" bytes="<<bytes<<" limit="<<limit;
    detail::emit(sink,o);
}
// common_storage_refused block=<name> want=<n> have=<n> pc=<pc>
inline void storageRefused(std::string_view block,std::size_t want,std::size_t have,std::uint32_t pc,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_storage_refused block="<<block<<" want="<<want<<" have="<<have<<" pc=";detail::hex(o,pc);
    detail::emit(sink,o);
    noteRefusal("storage","storage",Writer::W0,pc,block);
}

// common_roster slot=<s> pod=<id> source=stock|registry human=0|1 path=1ADF80|1AE3CC result=ok|refused reason=
inline void roster(std::size_t slot,std::uint32_t pod,Source source,bool human,bool ok,std::string_view reason={},
    std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_roster slot="<<slot<<" pod="<<pod<<" source="<<sourceName(source)<<" human="<<(human?1:0)
     <<" path="<<(human?"1ADF80":"1AE3CC")<<" result="<<(ok?"ok":"refused")<<" reason=";
    detail::quoted(o,reason);
    detail::emit(sink,o);
    if(!ok)noteRefusal(stepRow(Step::roster1AA510).number,"roster",Writer::R10,human?0x1ADF80u:0x1AE3CCu,reason);
}
// common_pod_receipt pod=<id> file=<UIACCEPT|sounds.ini|...> receipt=match|differs|none
enum class Receipt : std::uint8_t { match, differs, none };
inline void podReceipt(std::uint32_t pod,std::string_view file,Receipt r,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_pod_receipt pod="<<pod<<" file="<<file<<" receipt="
     <<(r==Receipt::match?"match":r==Receipt::differs?"differs":"none");
    detail::emit(sink,o);
}

// common_race_start reason=fresh|restart pc=1A6AA0|1AC8E0 a1=<0|1> caller=<pc> best_lap_cb4=<t> racers=<n> grid=2A2880 lock=1AED30 result=ok|refused
struct RaceStart {
    bool restart=false;
    std::uint32_t a1=0;
    std::uint32_t caller=0;      // 1BD934 Pause, 1ABC0C TT finish, 1ABC1C practice, 1CA... Results replay, 1A6AA0 fresh
    float bestLapCB4=0.f;        // human slot 0 best lap after the reset (216690 keeps it when 11258==3)
    std::uint32_t racers=0;
    bool ok=false;
    std::uint32_t refusedPc=0;   // the step pc that refused (0 when ok)
    std::string_view reason;
};
inline void raceStart(const RaceStart& v,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_race_start reason="<<(v.restart?"restart":"fresh")<<" pc="<<(v.restart?"1AC8E0":"1A6AA0")
     <<" a1="<<v.a1<<" caller=";detail::hex(o,v.caller);
    o<<" best_lap_cb4="<<v.bestLapCB4<<" racers="<<v.racers<<" grid=2A2880 lock=1AED30 result="<<(v.ok?"ok":"refused");
    if(!v.ok){o<<" refused_pc=";detail::hex(o,v.refusedPc);o<<" reason=";detail::quoted(o,v.reason);}
    detail::emit(sink,o);
    if(!v.ok)noteRefusal(stepRow(Step::raceStart).number,"race_start",Writer::R9,v.refusedPc?v.refusedPc:0x1AC8E0u,v.reason);
}

// common_lap racer=<slot> lap=<n> time=<t> best=<cb4> marker=FF8
// Source (W0 round 2, r4): common_lap50CR::Tracker::applyCrossing prints it, one line
// per accepted crossing; the per-race marker count is common_track_data role=lap_markers
// (Track::logTrackData from Counts::lapMarkers, counted at step 08).
inline void lap(std::size_t slot,std::uint32_t lapCC8,float time,float bestCB4,std::uint32_t markerFF8,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_lap racer="<<slot<<" lap="<<lapCC8<<" time="<<time<<" best="<<bestCB4<<" marker="<<markerFF8;
    detail::emit(sink,o);
}

// common_race_end site=1A7DD0 subtype=<s> branch=1A7E18|1ABBE8|1ABC1C route=results|tt_restart|practice|spectator next=<row pc>
enum class EndRoute : std::uint8_t { results, tt_restart, practice, spectator, refused };
inline void raceEnd(std::uint32_t subtype11258,std::uint32_t branch,EndRoute route,std::uint32_t nextPc,
    std::string_view reason={},std::ostream* sink=nullptr){
    static constexpr const char* routes[]={"results","tt_restart","practice","spectator","refused"};
    std::ostringstream o;
    o<<"common_race_end site=1A7DD0 subtype="<<subtype11258<<" branch=";detail::hex(o,branch);
    o<<" route="<<routes[static_cast<std::size_t>(route)]<<" next=";detail::hex(o,nextPc);
    if(route==EndRoute::refused){o<<" reason=";detail::quoted(o,reason);}
    detail::emit(sink,o);
    if(route==EndRoute::refused)noteRefusal(stepRow(Step::endDispatch1A7DD0).number,"race_end",Writer::R9,branch,reason);
}
// "common_race_end refused ..." (PLAN 2.7: a rejected Results root departs to Course through step 18).
inline void raceEndRefused(std::uint32_t subtype11258,std::uint32_t pc,std::string_view reason,std::ostream* sink=nullptr){
    raceEnd(subtype11258,pc,EndRoute::refused,0x17DE10u,reason,sink);
}

// pause_quit_record site=1BDAB4 qualify_1ABFC0=<0|1> page=3|none
inline void pauseQuitRecord(bool qualifies,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"pause_quit_record site=1BDAB4 qualify_1ABFC0="<<(qualifies?1:0)<<" page="<<(qualifies?"3":"none");
    detail::emit(sink,o);
}
// pause_refused site=<pc> reason="..." (PLAN 2.8: host Pause fallback deleted; refusal logged, Start ignored that frame)
inline void pauseRefused(std::uint32_t pc,std::string_view reason,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"pause_refused site=";detail::hex(o,pc);o<<" reason=";detail::quoted(o,reason);
    detail::emit(sink,o);
}

// common_teardown role=<role> released=1|0
inline void teardownRole(std::string_view role,bool released,std::ostream* sink=nullptr){
    std::ostringstream o;
    o<<"common_teardown role="<<role<<" released="<<(released?1:0);
    detail::emit(sink,o);
    if(!released)noteRefusal(stepRow(Step::teardown).number,"teardown",Writer::R9,0x17DE10u,role);
}
// (course_archive_release released=1 is printed by native_course_archive_release50PD::Holder::release.)

// common_run_summary course=<code> mode=<..> pods=<..> result=pass|fail first_refusal=<step/role/owner> laps=<n> routes=<ack sequence> fps_avg=<v>
struct RunSummary {
    std::string_view course;
    std::string_view mode;        // "single", "time_trial", "tournament", "practice", "spectator"
    const std::uint32_t* pods=nullptr;std::size_t podCount=0;
    std::uint32_t laps=0;
    std::string_view routes;      // ack sequence, e.g. "results,hall"
    double fpsAverage=0.0;
};
inline void runSummary(const RunSummary& v,std::ostream* sink=nullptr){
    const auto f=firstRefusal();
    std::ostringstream o;
    o<<"common_run_summary course="<<v.course<<" mode="<<v.mode<<" pods=";
    for(std::size_t i=0;i<v.podCount;++i)o<<(i?",":"")<<v.pods[i];
    o<<" result="<<(f.set?"fail":"pass")<<" first_refusal=";
    if(f.set){o<<f.step<<'/'<<f.role<<'/'<<f.owner<<" first_refusal_pc=";detail::hex(o,f.pc);o<<" first_refusal_reason=";detail::quoted(o,f.reason);}
    else o<<"none";
    o<<" laps="<<v.laps<<" routes="<<(v.routes.empty()?"-":v.routes)<<" fps_avg="<<v.fpsAverage;
    detail::emit(sink,o);
}

} // namespace common_race_log50CR
