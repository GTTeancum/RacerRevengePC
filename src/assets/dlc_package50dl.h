#pragma once
// DLC packages: dlc.json reader and package checker (phases 1-2).
// NOT ORIGINAL BEHAVIOUR: the original game has no add-on content. This is a PC
// enhancement. Packages carry data only; nothing here runs package code.
// Design: work\scratch\dlc\DESIGN.md sections 2 and 5. Field reference: same
// folder, DLC-JSON.md (the converter contract).
// This header defines the one in-memory layout of a package; nothing else
// invents package fields. No global state: dlc_files50dl.cpp owns the session.
#include "dlc_registry50dl.h"
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace rr::assets::dlc50DL {

inline constexpr const char* schemaName="rrpc-dlc/1";
inline constexpr const char* portContract="dlc-contract-1";      // replacement packages
inline constexpr const char* itemsContract="dlc-contract-2";     // add-on items (courses, pods)
inline constexpr const char* personalNoteDefault=
    "built from your own copy of the source games; do not share";

struct Limits {
    static constexpr std::uint64_t manifestBytes=1u<<20;           // dlc.json <= 1 MiB
    static constexpr std::size_t files=4096;                        // files per package
    static constexpr std::size_t items=256;                         // add-on items per package
    static constexpr std::size_t replacements=4096;
    static constexpr std::uint64_t packageBytes=512ull<<20;         // all files <= 512 MiB
    static constexpr std::size_t nameChars=63,descriptionChars=1024,noteChars=255,reasonChars=255;
    static constexpr std::size_t pathChars=240,pathDepth=8;
    static constexpr int priorityLow=-1000,priorityHigh=1000;
    static constexpr std::size_t dependencies=64,conflicts=64;
};

struct Version {int major=0,minor=0,patch=0;};
bool parseVersion(const std::string& text,Version& out);
int compareVersions(const Version& a,const Version& b);
// Range: space-separated comparators (">=1.0.0 <2.0.0", "=1.2.0", "1.2.0"); empty = any.
bool versionRangeValid(const std::string& range);
bool versionSatisfies(const Version& version,const std::string& range);

// "TRACKS\\sc.res" -> "TRACKS/sc.res". Refuses empty, absolute, drive, "..", ".",
// control characters, ':' and paths longer/deeper than the limits.
bool normalisePackagePath(const std::string& input,std::string& out);
std::string foldCase(std::string text); // ASCII lower case (paths compare case-insensitively)
bool refusedFileType(const std::string& normalisedPath); // executable/script extensions

struct FileRow {std::string path;std::uint64_t bytes=0;std::string sha256;};
struct Replacement {
    std::string target;              // normalised stock path, original spelling
    std::uint64_t expectBytes=0;std::string expectSha256; // the stock file it was built for
    std::string with;                // normalised package path (a listed file)
};
struct Dependency {std::string id,range;bool optional=false;};
struct Conflict {std::string id,reason;};

struct Package {
    std::filesystem::path folder;
    std::string id,name,author,versionText,description,kind,personalNote;
    Version version;
    bool enabled=false,replace=false,personal=false;
    int priority=0;
    std::vector<FileRow> files;
    std::vector<Replacement> replacements;
    std::vector<Dependency> dependencies;
    std::vector<Conflict> conflicts;
    std::uint64_t totalBytes=0;
    std::size_t itemCount=0;             // entries of "items" (checked by readItems)
    std::string manifestText;            // the dlc.json text (<= 1 MiB), read again by readItems
};

// dlc.json only: size, UTF-8 JSON, schema, fields, types, ranges, folder name.
// Reason is written for a player. A disabled package (enabled=false) still
// passes this step so the caller can report it as disabled.
bool readManifest(const std::filesystem::path& folder,Package& out,std::string& reason);
// Folder contents: every listed file present with its size and SHA-256, nothing
// unlisted, no links/junctions, no executable content.
bool checkFiles(const Package& package,std::string& reason);
// Streaming SHA-256 of a whole file (lower-case hex). False if unreadable.
bool sha256File(const std::filesystem::path& path,std::uint64_t& bytes,std::string& hex);

// ---- Add-on items (dlc-contract-2). Implemented in dlc_registry50dl.cpp, the
// one owner of add-on storage; called only by dlc50DL::startup().
struct PackageItems {
    std::vector<AddonPod> pods;
    std::vector<AddonCourse> courses;
    std::map<std::string,std::map<std::string,std::string>> strings; // language -> KEY -> text
};
// Items and strings of a package whose dlc.json passed readManifest (json rules).
bool readItems(const Package& package,PackageItems& out,std::string& reason);
// Item content: archives decode within their capacity, members found by name
// (stem / code rule), PHY prefix, menu members named exactly once, material
// names either shared originals or stem-named. `stock` is the install root
// (shared original names are read from the stock pod and garage archives).
bool checkItems(const Package& package,PackageItems& items,const std::filesystem::path& stock,std::string& reason);
// True when two packages claim the same pod stem, course code or material name.
bool itemsCollide(const PackageItems& a,const PackageItems& b,std::string& what);
struct AdmittedItems {const Package* package=nullptr;PackageItems items;};
// Numbers in package order (priority high first, then id), publishes and
// freezes the registry; appends one dlc_item line per item to `lines`.
void publishRegistry(std::vector<AdmittedItems>& admittedInOrder,std::size_t admittedPackages,std::vector<std::string>& lines);

} // namespace rr::assets::dlc50DL
