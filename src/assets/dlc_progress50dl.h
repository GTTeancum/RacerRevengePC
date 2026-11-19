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

inline constexpr const char* progressSchema="rrpc-dlc-progress/1";
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
