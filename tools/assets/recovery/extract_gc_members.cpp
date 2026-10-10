// Cold, bounded RES member extraction. No guest execution or emulation.
#include "assets/import_decode48c.h"
#include "native-json/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
namespace fs = std::filesystem;
namespace a = rr::assets;
using Bytes = std::vector<std::uint8_t>;
using J = nlohmann::json;
static void need(bool ok, const std::string& why) { if (!ok) throw std::runtime_error(why); }
static std::string safe(std::string s) {
    need(!s.empty() && s.size() <= 1024, "Member name bounds");
    for (auto& c : s) {
        if (c == '\\') c = '/';
        if (c >= 'A' && c <= 'Z') c += 32;
        need((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '/' || c == '_' || c == '-' || c == '.', "Unsafe member character");
    }
    need(s.front() != '/' && s.back() != '/', "Absolute/empty member path");
    std::size_t at = 0;
    while (at < s.size()) {
        auto end = s.find('/', at); if (end == std::string::npos) end = s.size();
        const auto part = s.substr(at, end-at);
        need(!part.empty() && part != "." && part != "..", "Unsafe member component");
        at = end + 1;
    }
    return s;
}
static void writeNew(const fs::path& p, const Bytes& b) {
    const int fd = ::open(p.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
    need(fd >= 0, "Refusing existing/invalid output: " + p.string());
    std::size_t at = 0;
    while (at < b.size()) {
        const auto n = ::write(fd, b.data()+at, b.size()-at);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) { ::close(fd); throw std::runtime_error("Short output write"); }
        at += static_cast<std::size_t>(n);
    }
    need(::close(fd) == 0, "Output close failed");
}
int main(int argc, char** argv) { try {
    need(argc == 3, "Usage: extract_gc_members GC.RES NEW_INTAKE_DIRECTORY");
    const fs::path input = argv[1], out = argv[2];
    need(!fs::exists(out) && !fs::is_symlink(out), "Output must be new");
    std::ifstream f(input, std::ios::binary | std::ios::ate);
    need(bool(f), "Cannot read original resource"); const auto n = f.tellg();
    need(n == 4205067, "Exact GC resource byte bound");
    Bytes raw(static_cast<std::size_t>(n)); f.seekg(0);
    f.read(reinterpret_cast<char*>(raw.data()), raw.size()); need(bool(f), "Short resource read");
    a::ResArchive48C res; a::DecodeError48C e;
    need(a::decodeResource48C(raw, res, e), "RES decode: " + e.code + " " + e.detail);
    need(!res.entries.empty() && res.entries.size() <= 10000, "Member count bound");
    std::set<std::string> names; std::vector<std::string> paths; std::size_t total = 0;
    for (const auto& row : res.entries) {
        auto p = safe(row.name); need(names.insert(p).second, "Case-folded member collision: " + p);
        need(row.bytes <= 8u*1024u*1024u, "Member byte bound"); total += row.bytes;
        need(total <= 128u*1024u*1024u, "Aggregate member byte bound"); paths.push_back(p);
    }
    for (const auto& p : paths) {
        auto parent = fs::path(p).parent_path();
        while (!parent.empty()) { need(!names.count(parent.generic_string()), "File/directory collision"); parent = parent.parent_path(); }
    }
    need(fs::create_directory(out), "Cannot create exclusive intake directory");
    fs::permissions(out, fs::perms::owner_all, fs::perm_options::replace);
    fs::create_directory(out/"original-members");
    J manifest = {{"schema", "RR_ORIGINAL_GC_MEMBER_INVENTORY_V1"}, {"members", J::array()}};
    for (std::size_t i = 0; i < res.entries.size(); ++i) {
        const auto& row = res.entries[i]; Bytes bytes;
        need(a::extractResMember48C(res, row.name, bytes, e), "Extract: " + row.name + " " + e.code);
        need(bytes.size() == row.bytes, "Member size mismatch");
        auto target = out/"original-members"/paths[i]; fs::create_directories(target.parent_path());
        writeNew(target, bytes);
        manifest["members"].push_back({{"name", row.name}, {"path", paths[i]}, {"bytes", row.bytes}, {"logical_offset", row.logical_offset}, {"source_index", i}});
    }
    const auto json = manifest.dump(2) + "\n";
    writeNew(out/"extraction-index.json", Bytes(json.begin(), json.end()));
    std::cout << "Extracted " << res.entries.size() << " original members\n"; return 0;
} catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; } }
