#pragma once
// DLC registry (phase 2): the ONE place that defines add-on storage.
// NOT ORIGINAL BEHAVIOUR: the original game has no add-on content. PC enhancement.
// Design: work\scratch\dlc\DESIGN.md 4.1. Field contract: work\scratch\dlc\DLC-JSON.md
// ("Add-on items", dlc-contract-2).
//
// Layout. Two port-owned vectors (pods, courses) sized once at startup from the
// admitted packages, then frozen: no growth during play, so no pointer moves
// while menus or races hold rows. Add-on numbers start at 32 so that no add-on
// number can pass for an original value (course < 13, planet < 8, rider < 32):
// one written by mistake into an original field is refused by the encodings
// the port already checks. Racer numbers stop at 99 (the port refuses racer ids
// of 100 and above), so at most 68 add-on pods; add-on courses are capped at 64.
// Numbers follow the package order (priority high first, then id) and are
// never saved; saves use the item key "<package-id>/<item-id>".
//
// The registry never writes an original table, a profile field, a save byte or
// a stock file. With no admitted add-on item every list is empty and every
// accessor answers "not an add-on" (stock runs exactly as before).
//
// Narrow on purpose (standard headers only): menu and race sites include it.
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr::assets::dlc50DL {

struct RegistryLimits {
    static constexpr unsigned firstNumber=32;            // first add-on number (pods and courses)
    static constexpr unsigned lastRacer=99;              // racer ids >= 100 are refused by the port
    static constexpr std::size_t pods=lastRacer-firstNumber+1; // 68
    static constexpr std::size_t courses=64;             // 32..95
    static constexpr std::size_t stringKeys=4096,stringBytes=255,keyChars=47,languages=16;
    // Shared pod-archive capacity: the same values the pod identity reader uses
    // (tests\native\original_player_identity50ik_fixture.h limits(); FLAG: that
    // fixture keeps its own copy until its next header round, then reads these).
    static constexpr std::size_t podArchiveBytes=16u<<20,podDecodedBytes=16u<<20;
    static constexpr std::uint32_t podEntries=512,podNameBytes=256;
    static constexpr std::size_t textMemberBytes=8192;
    // Add-on UI archive capacity. PORT EXTENSION, NOT ORIGINAL: the original has no
    // per-pod menu archive; stock UI archives are pinned by the UI catalog instead.
    static constexpr std::size_t uiArchiveBytes=16u<<20,uiDecodedBytes=32u<<20;
    static constexpr std::uint32_t uiEntries=512;
    // Course archives use the shared course-archive capacity (DecodeLimits48C
    // defaults: 64 MiB file, 128 MiB decoded, 10000 entries).
    static constexpr std::size_t cameraBytes=64u<<10;    // <STEM>.CAM (stock PODnn.CAM is ~5.8 KiB)
    static constexpr std::size_t loosePstBytes=4u<<20;   // loading screen / preview PST
};

// Package-relative file addressed by the overlay: "dlc:/<package-id>/<path>".
// dlc50DL::host() (dlc_files50dl.h) turns such a path into the admitted
// package's listed file (one dlc_open line); any other dlc:/ path, or a file
// changed after startup, throws and never falls back to stock. FLAG: the
// comment on host() in dlc_files50dl.h still describes phase 1 only; it is
// left unchanged to avoid a ~124-TU rebuild for a comment (next header round).
inline constexpr const char* dlcScheme="dlc:/";

struct AddonPodUi {
    std::string archive;                 // dlc:/ path of the pod's own menu archive
    std::string riderModel,resultsIcon;  // member names exactly as dlc.json spells them
    std::vector<std::string> character,characterPages,podPages,shadow,portraits,background,garageModel;
    // Optional "layout_slot": the stock rider (1..26, a Rider-screen racer) whose
    // authored Rider pod and character widgets place this pod's menu models (the
    // slot its menu members were converted for). 0 = absent: the Rider screen uses
    // the anchor stock rider's widgets. PORT EXTENSION: a package has no widgets.
    unsigned layoutSlot=0;
    // Optional "huttese_name_key" (default name_key): the string the Rider
    // screen's large Huttese-font name widget draws, as a stock rider's two name
    // widgets both draw its one name string. Only dlcHutteseNameCharacter
    // characters (dlc_name_font50dl.h). hutteseName is its English text.
    std::string hutteseNameKey,hutteseName;
    // Optional "colors": {"accent": [r,g,b], "secondary": [r,g,b]}, whole numbers
    // 0..255, packed 0xRRGGBB like the stock profile colour words (17FA00 accent,
    // 17FA10 secondary; no alpha). Absent: the layout_slot rider's colours, else
    // the anchor's (PORT STAND-IN).
    bool colorsAuthored=false;
    std::uint32_t accentColor=0,secondaryColor=0;
};
struct AddonPod {
    unsigned number=0;                   // 32..99
    std::string key,packageId,packageName,itemId;
    std::string stem;                    // lower case, replaces Pod%02d in member names
    std::string podId;                   // upper-case stem: PODS/<podId>/<podId>.RES spelling
    std::string archive,camera;          // dlc:/ paths
    std::uint64_t archiveBytes=0;std::string archiveSha256;
    std::string nameKey,shortNameKey;    // package string keys (bare KEY)
    std::string name,shortName;          // English text of those keys
    AddonPodUi ui;
    bool voice=false;std::string voiceIni;unsigned voiceFirst=0,voiceCount=0; // voiceIni: dlc:/ path
    std::string announcer;               // dlc:/ path; empty = generic announcer lines only
    bool aiOnAddonCourses=true;          // "addon_courses" (true) or "never" (false); never on stock courses
    std::size_t podMembers=0,uiMembers=0;
    // Rider-screen stats bars: [BaseAbilities] / [MaxAbilities] of the PHY inside
    // `archive`, lanes in the original order (MaxThrust, TheoreticalTopSpeed,
    // SpoilerYawMoment, Defense, BaseRepairAmount, BaseCoolingTime); finite, >= 0.
    float baseAbilities[6]{},maxAbilities[6]{};
    // Material names this pod brings (folded, member path without extension,
    // e.g. "textures\\a2r_ark_1"): its own .psm definitions whose names start
    // with the stem. Each name belongs to exactly one admitted pod.
    std::vector<std::string> materials,uiMaterials;
};
struct AddonHallRow {double time=0;unsigned count=0;std::string name;}; // name: 1..20 characters
struct AddonCourse {
    unsigned number=0;                   // 32..95
    std::string key,packageId,packageName,itemId;
    std::string code;                    // 2 characters, lower case, never a stock code
    std::string archive;                 // dlc:/ path
    std::uint64_t archiveBytes=0;std::string archiveSha256;
    std::string nameKey,planetNameKey,name,planetName;
    char phyClass='B',aiClass='B';
    std::string rival;                   // as written: "stock:NN", "<pkg>/<item>" or "none"
    unsigned rivalRacer=0;               // resolved at freeze: 0 = none
    bool loadingStock=false;std::string loadingFile,loadingStockCode; // file: dlc:/ path
    std::string courseScreenTemplate="ta",coursePreview; // stock code; preview dlc:/ path or empty
    std::string inheritTuning,inheritParticles;          // stock codes or empty
    bool ownTuning=false,ownParticles=false;             // members present in the archive
    bool single=true,timeTrial=true,vs=true; // modes; "vs" has no effect until two-player races are built
    std::string recordsKey;
    bool hallDefaults=false;AddonHallRow lap[3],race[3],knockouts[3];
    std::vector<std::string> enhancements;
};

// ---- Read-only accessors (valid after dlc50DL::startup(); frozen afterwards).
// Every index and number is bounds-checked; a miss returns nullptr / false.
bool registryFrozen() noexcept;
// Packages admitted this session (any kind, replacements included); 0 = no DLC.
std::size_t admittedPackageCount() noexcept;
// HOOK (DLC save side file, helper 146s, src\assets\dlc_progress50dl.h ProgressCatalog):
// fill it once after startup from these accessors, nothing else:
//   admittedPackages            = admittedPackageCount()
//   courses[i].key/recordsKey   = addonCourseAt(i)->key / ->recordsKey
//   courses[i].archiveSha256    = addonCourseAt(i)->archiveSha256
//   courses[i].hallDefaults     = addonCourseAt(i)->lap/race/knockouts when ->hallDefaults
//                                 (names <= 20 characters, checked here)
//   pods[i].key                 = addonPodAt(i)->key
std::size_t addonPodCount() noexcept;
const AddonPod* addonPodAt(std::size_t index) noexcept;          // index 0..count-1, menu order
const AddonPod* addonPod(unsigned racer) noexcept;               // admitted add-on racer number only
const AddonPod* addonPodByPodId(const std::string& podId) noexcept; // case-insensitive stem/podId
inline constexpr bool addonNumberRange(unsigned n) noexcept {
    return n>=RegistryLimits::firstNumber&&n<=RegistryLimits::lastRacer;
}

// Course rows. API for the add-on course phase (another helper wires these into
// the Course screen and race load): courses are parsed, validated, numbered and
// listed here; nothing in the game reads them yet.
//   addonCourseCount()/addonCourseAt(i)  the "DLC" planet's list, in number order
//   addonCourse(number)                  32..95, nullptr for any other number
//   addonCourseByCode(code)              "nv" -> row (codes are unique)
//   courseRow(number,row)                0..12 = the stock descriptor row (code and
//                                        rival from the original table), 32.. = add-on
std::size_t addonCourseCount() noexcept;
const AddonCourse* addonCourseAt(std::size_t index) noexcept;
const AddonCourse* addonCourse(unsigned number) noexcept;
const AddonCourse* addonCourseByCode(const std::string& code) noexcept;

// pod(racer): the stock row (original Pod%02d naming, stock disc paths) or the
// add-on row (registry). A data lookup, not a code branch: the same pod load runs
// on whatever row comes back.
struct PodRow {
    unsigned racer=0;bool addon=false;
    std::string podId;        // "POD04" or "A2R_ARK"
    std::string memberStem;   // "pod04" or "a2r_ark" (folded member spelling)
    std::string archive;      // "PODS/POD04/POD04.RES" (stock-root relative) or dlc:/ path
    std::string camera;       // "PODS/POD04/POD04.CAM" or dlc:/ path
    const AddonPod* row=nullptr;
};
bool podRow(unsigned racer,PodRow& out);
struct CourseRow {
    unsigned number=0;bool addon=false;
    std::string code;         // lower case
    unsigned rivalRacer=0;
    const AddonCourse* row=nullptr;
};
bool courseRow(unsigned number,CourseRow& out);

// Every material name the admitted add-on pods bring (pod archives), and the
// menu material names (their UI archives). Empty without add-on pods.
const std::vector<std::string>& addonPodMaterialNames() noexcept;
const std::vector<std::string>& addonPodUiMaterialNames() noexcept;
// Owner of a material name ("textures\\a2r_ark_1"), case-insensitive; nullptr if none.
const AddonPod* addonPodOwningMaterial(const std::string& name) noexcept;

// Package strings ("strings" in dlc.json; keys namespaced by the package).
// Missing language -> English. nullptr when the package or key is unknown.
const std::string* packageString(const std::string& packageId,const std::string& key,
                                 const std::string& language="english") noexcept;

} // namespace rr::assets::dlc50DL
