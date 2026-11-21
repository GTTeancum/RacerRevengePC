#pragma once
// DLC progress side file: <save folder>\dlc-progress-v1.json, schema rrpc-dlc-progress/1.
// PORT EXTENSION, NOT ORIGINAL: the original game has no add-on content.
// Design: work\scratch\dlc\DESIGN.md section 6 (6.1-6.3) and the end of DLC-JSON.md.
//
// - The stock save (progress-v1.bin, later the original card file) is never read
//   or written here. Nothing an add-on produces goes into the stock Hall.
// - Hall rows use the original row shape (OriginalUiHallCourseRecords50KG: 3
//   entries per table, 21-byte name lanes) and the original record rules 1ABFC0
//   and 1AC440, called from race_record_rule50kg (the same bodies the stock
//   Hall owner calls; nothing is copied).
// - The file is written through the stock save's writer (native_beta50kg::
//   writeFile: temporary file, flush, one same-directory replace), only when DLC
//   progress changed, only at the stock save moments, and never when no package
//   was admitted this session.
// - Loading is all-or-nothing: a malformed file is reported once, renamed to
//   dlc-progress-v1.bad.json and progress starts empty; it is never half-loaded.
//
// This header defines the one layout of DLC progress; nothing else invents it.
#include "../game/original_race_record_rule50kg.h"
#include "../game/original_ui_hall_records50kg.h"
#include "../game/original_tournament_result50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace rr::assets::dlc50DL {

inline constexpr const char* progressSchema="rrpc-dlc-progress/3";   // written
inline constexpr const char* progressSchemaV2="rrpc-dlc-progress/2"; // still read (careers without ai/name/boundary, no unlocks)
inline constexpr const char* progressSchemaV1="rrpc-dlc-progress/1"; // still read (no careers)
inline constexpr const char* progressFileName="dlc-progress-v1.json";
inline constexpr const char* progressBadFileName="dlc-progress-v1.bad.json";
inline constexpr const char* progressLockName="dlc-progress-v1.lock";

struct ProgressLimits {
    static constexpr std::size_t fileBytes=1u<<20;    // the stock writer's own 1 MiB cap
    static constexpr std::size_t courses=1024;        // course entries kept (shown or not)
    static constexpr std::size_t previousSets=16;     // retired records_key sets per course
    static constexpr std::size_t pods=1024;           // pod entries kept (shown or not)
    static constexpr std::size_t nameChars=20;        // visible chars; lane is 21 bytes with NUL
    static constexpr std::size_t entries=3;           // per table, as the original rows
    static constexpr std::size_t careers=1024;        // add-on pod careers kept (/2)
    static constexpr std::uint8_t progress=13;        // 219C: course order 0..13 (13 = completed)
    static constexpr std::uint8_t medal=8;            // 1938 holds the finish place 0..8 (0 = none)
};
// PORT EXTENSION: an empty time slot. The original rows are always seeded
// (1801D0); an add-on table without hall_defaults starts empty. 21474836 s is
// the value the original formatter 189000 already shows as "--:--.--", and any
// real time is lower, so the unchanged 1AC440 rule fills empty slots first.
inline constexpr float emptyRecordTime=21474836.f;
// Rows with every slot empty (times = emptyRecordTime, counts 0, names zero).
game::OriginalUiHallCourseRecords50KG emptyHallRows() noexcept;

// ---- What the DLC registry hands over (src\assets\dlc_registry50dl, helper 146p) ----
// Minimal interface type, filled once from the frozen registry:
//   ProgressCourseItem.key           = AddonCourse item key "<package-id>/<item-id>"
//   ProgressCourseItem.recordsKey    = the item's records_key (default: item id)
//   ProgressCourseItem.archiveSha256 = sha256 of the package files row named by
//                                      the item's "archive" (lower-case hex)
//   ProgressCourseItem.hallDefaults  = hall_defaults converted to original rows
//                                      (start from emptyHallRows()); absent = empty
//   ProgressPodItem.key              = AddonPod item key
//   admittedPackages                 = packages admitted this session (any kind)
struct ProgressCourseItem {
    std::string key,recordsKey,archiveSha256;
    std::optional<game::OriginalUiHallCourseRecords50KG> hallDefaults;
};
struct ProgressPodItem {std::string key;};
struct ProgressCatalog {
    std::size_t admittedPackages=0;
    std::vector<ProgressCourseItem> courses;
    std::vector<ProgressPodItem> pods;
};

// Key syntax (DLC-JSON.md). Item key "<package-id>/<item-id>".
bool validItemKey(const std::string&) noexcept;
bool validRecordsKey(const std::string&) noexcept;
// Racer reference: "stock:NN" (NN 01..26) or an item key.
bool validRacerReference(const std::string&) noexcept;
// Stock course code -> upper-case two-letter code from the 13 original course
// descriptors ("ta" -> "TA"); false for anything else.
bool stockCourseCode(const std::string& code,std::string& upper) noexcept;

// Hall rows with the racer reference of every slot ("" = default row or empty).
struct ProgressHallView {
    game::OriginalUiHallCourseRecords50KG rows{};
    std::array<std::string,3> lapRacers,raceRacers,knockoutRacers;
};

// One finished race, as the Results flow already reads it for the stock Hall.
struct ProgressRaceResult {
    // Exactly one target: an add-on course item key, or a stock course code
    // (records of an add-on pod on a stock course, decision D7 b).
    std::string courseKey,stockCode;
    // Race facts the stock 1ABFC0/1AC440 read from the profile (read-only copies).
    std::uint32_t subtype11258=0;          // current mode (2 = Tournament)
    std::uint8_t lapCount1125C=0;          // playerBits1125C (lap count in bits 0-4)
    std::uint16_t packed1125C=0;           // 1125C | 1125D<<8 (humans, opponents)
    std::array<std::uint32_t,27> unlock10FAC{}; // stock cheat words 0/5/13 gate 1ABFC0
    std::uint8_t race1B0Low2=1;            // 1 = one human, 2 = two humans
    struct Racer {
        std::string racer;                 // "stock:NN" or pod item key
        std::string name;                  // persisted profile name, <= 20 printable chars
        game::OriginalRaceRecordQualificationRacerObservation50KG values{};
    };
    std::array<Racer,2> racers{};          // manager getter order; [1] used only for two humans
};
struct ProgressSubmitOutcome {
    enum class Status:std::uint8_t {
        recorded,        // qualified; rows changed or not (ranks say which)
        notQualified,    // 1ABFC0 returned 0 (Tournament, cheats, retired, no record)
        refused          // bad input or storage limit; nothing changed (reason)
    } status=Status::refused;
    game::race_record_rule50kg::InsertionRanks first,second;
    std::string reason;
};

// ---- Add-on pod tournament career (schema /2, /3). PORT EXTENSION, NOT ORIGINAL.
// The stock career shape (17FFB0/1875C0: OriginalTournamentCareer50KG) for an
// add-on pod: its scalars, its OWN pod row (the original diagonal) and (/3) the
// 27 stock pod rows of its AI opponents, the 219D name and the 102CC flag.
// Never stored in the profile's 27 career tables or the stock save.
// Bounds: abilities finite and >= 0, medals 0..8, progress 0..13, name at most
// 20 printable ASCII characters, boundary 0/1; the u32 / i16 lanes keep the
// stock storage widths. aiStored=false (a /2 entry, or a fresh career): the AI
// rows are seeded by the career owner (dlc_career50dl) from the stock base0180.
struct ProgressCareer {
    std::uint32_t earned2190=0,unspent2194=0,kills2198=0;
    std::uint8_t progress219C=0;
    game::OriginalTournamentPodCareer50KG pod{};
    bool aiStored=false;
    std::array<game::OriginalTournamentPodCareer50KG,27> ai{};   // /3
    std::string name;                                             // /3, 219D
    std::uint32_t boundary102CC=0;                                // /3
};
// ---- Stock unlocks granted by add-on careers (schema /3). PORT EXTENSION.
// The original grants (20B480) write the stock unlock words 10FAC and the
// gallery tier 11018 bits 5..6; a grant earned by an add-on career is stored
// HERE and merged at read time (stock | granted, max tier). Pods 0..26.
struct ProgressUnlocks {
    std::array<bool,27> stockPods{};
    std::uint8_t galleryTier=0;                                   // 0..3
};
bool validCareer(const ProgressCareer&) noexcept;
// A fresh career: abilities from the pod's PHY [BaseAbilities]
// (AddonPod::baseAbilities), remaining prizes from the stock prize table
// (originalTournamentPrizes50KG), everything else zero.
ProgressCareer seededCareer(const float (&baseAbilities)[6]) noexcept;

class DlcProgress50DL final {
public:
    enum class LoadStatus:std::uint8_t {
        notLoaded,
        noPackages,      // nothing admitted: file neither read nor created
        fresh,           // no file yet (created at the first change + save moment)
        loaded,
        setAside,        // malformed: renamed to dlc-progress-v1.bad.json, starts empty
        newerSchema,     // another schema version: left untouched, never written
        unavailable,     // no save folder, lock held elsewhere, read error, or the
                         // malformed file could not be set aside: never written
    };
    enum class SaveStatus:std::uint8_t {unchanged,saved,notWritable,tooLarge,writeError};

    DlcProgress50DL();
    DlcProgress50DL(const DlcProgress50DL&)=delete;
    DlcProgress50DL& operator=(const DlcProgress50DL&)=delete;
    ~DlcProgress50DL();

    // Once, at startup after the registry is frozen. saveDirectory = the folder
    // of the stock save (the profile owner's progress-v1.bin folder, or
    // native_beta50kg::defaultDirectory()). Applies 6.3 against the catalog and
    // logs one line per course entry it keeps but does not show.
    LoadStatus load(const std::filesystem::path& saveDirectory,const ProgressCatalog& catalog) noexcept;
    LoadStatus loadStatus() const noexcept {return status_;}

    // Hall rows of an admitted add-on course (defaults when none were set).
    // False for a key the catalog does not hold (removed package: not shown).
    bool courseHall(const std::string& courseKey,ProgressHallView& out) const noexcept;
    // Add-on pod records on a stock course ("TA"); empty rows when none.
    bool stockCourseAddonPodHall(const std::string& stockCode,ProgressHallView& out) const noexcept;
    // Contract 2: every admitted add-on pod is unlocked ("unlock": "always").
    bool podUnlocked(const std::string& podKey) const noexcept;

    // Add-on pod career (schema /2). careerSnapshot: the stored career of an
    // admitted add-on pod, or a fresh seeded one (registry baseAbilities) when
    // none is stored yet; false for a pod the catalog does not hold.
    // commitCareer: bounds-checked replace of that pod's career (refused, and
    // nothing changes, for a bad value, an unknown pod or the career limit);
    // marks progress changed. Written at the next stock save moment.
    bool careerSnapshot(const std::string& podKey,ProgressCareer& out) const noexcept;
    bool commitCareer(const std::string& podKey,const ProgressCareer& career) noexcept;
    // Stock unlocks granted by add-on careers (/3): read for the merge, and the
    // bounds-checked grant (pod 0..26; tier 0..3, never lowered). Marks changed.
    const ProgressUnlocks& addonCareerUnlocks() const noexcept {return state_.unlocks;}
    bool grantStockPod(unsigned pod) noexcept;
    bool grantGalleryTier(std::uint8_t tier) noexcept;

    // Applies 1ABFC0 (selector 0) and, when it returns 1, 1AC440 to the target
    // rows. Never touches the stock Hall. Marks progress changed on any effect.
    ProgressSubmitOutcome submit(const ProgressRaceResult&) noexcept;

    // Call right after each stock save write (same moments, same autosave
    // condition). Writes only when DLC progress changed since the last write.
    SaveStatus saveIfChanged() noexcept;
    bool changed() const noexcept {return dirty_;}

    // The one in-memory layout (public for the host unit check).
    struct Table {
        game::OriginalUiHallCourseRecords50KG rows{};
        std::array<std::string,3> lapRacers,raceRacers,knockoutRacers;
    };
    struct RecordSet {std::string archiveSha256;Table table;};
    struct CourseEntry {
        std::string recordsKey;RecordSet active;
        std::map<std::string,RecordSet> previous;   // retired records_key -> set (D10)
    };
    struct State {
        std::map<std::string,CourseEntry> courses;          // item key
        std::map<std::string,Table> stockCourseAddonPod;    // upper-case stock code
        std::map<std::string,bool> pods;                    // pod item key -> unlocked
        std::map<std::string,ProgressCareer> careers;       // pod item key -> career (/2)
        ProgressUnlocks unlocks;                            // /3
    };
    const State& state() const noexcept {return state_;}
    // Serialise / parse the whole file (bounds-checked, all or nothing).
    static bool serialise(const State&,std::string& text,std::string& reason);
    static bool parse(const std::string& text,State& out,bool& newerSchema,std::string& reason);

private:
    struct Lock;
    LoadStatus status_=LoadStatus::notLoaded;
    bool writable_=false,dirty_=false;
    std::filesystem::path directory_,path_;
    std::unique_ptr<Lock> lock_;
    ProgressCatalog catalog_;
    std::set<std::string> blocked_;   // catalog courses whose entry could not be reconciled (not shown)
    bool notWritableLogged_=false;
    State state_;
};

// ---- Process hook-up (146p round 3). PORT EXTENSION, NOT ORIGINAL.
// The one DLC progress owner of the process.
DlcProgress50DL& progressOwner() noexcept;
// The catalog filled only from the frozen registry accessors (the HOOK in
// dlc_registry50dl.h): course keys, records keys, archive digests, hall
// defaults converted to original rows, pod keys, admitted package count.
ProgressCatalog progressCatalogFromRegistry();
// Once, after dlc50DL::startup(). With no admitted package nothing is read,
// created or logged (stock runs exactly as before); otherwise loads the side
// file next to the stock save (saveDirectory).
void loadProgressFromRegistry(const std::filesystem::path& saveDirectory) noexcept;

} // namespace rr::assets::dlc50DL
