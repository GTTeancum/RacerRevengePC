// Cold extraction only through the existing native RES reader. Never guest code.
// Caller authenticates/stages exact GA.RES; Python bridge authenticates EVERY
// output member before publishing a usable intake. Private trusted output parent.
// Windows caller must supply ASCII argument paths; the Python bridge enforces
// this before launch, avoiding narrow-main code-page loss. No Unicode claim.
#include "assets/import_decode48c.h"
#include "native-json/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
namespace fs = std::filesystem;
using Bytes = std::vector<std::uint8_t>;
using J = nlohmann::json;
static void need(bool okay, const std::string& why) { if (!okay) throw std::runtime_error(why); }
static void fresh(const fs::path& path, const Bytes& bytes) {
#ifdef _WIN32
    const int file = ::_wopen(path.c_str(), _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY, _S_IREAD | _S_IWRITE);
#else
    const int file = ::open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
#endif
    need(file >= 0, "Refusing existing/invalid output: " + path.string());
    std::size_t offset = 0;
    while (offset < bytes.size()) {
        const auto remaining = bytes.size() - offset;
        const auto chunk = static_cast<unsigned>(remaining > 1048576u ? 1048576u : remaining);
#ifdef _WIN32
        const auto wrote = ::_write(file, bytes.data() + offset, chunk);
#else
        const auto wrote = ::write(file, bytes.data() + offset, chunk);
#endif
        if (wrote < 0 && errno == EINTR) continue;
        if (wrote <= 0) {
#ifdef _WIN32
            ::_close(file);
#else
            ::close(file);
#endif
            throw std::runtime_error("Short member output write");
        }
        offset += static_cast<std::size_t>(wrote);
    }
#ifdef _WIN32
    need(::_close(file) == 0, "Output close failed");
#else
    need(::close(file) == 0, "Output close failed");
#endif
}
int main(int argc, char** argv) { try {
    need(argc == 3, "Usage: extract_ga_members EXACT_GA.RES NEW_OUTPUT");
    const fs::path source = argv[1], output = argv[2];
    need(!fs::exists(output) && !fs::is_symlink(output), "Output must be new");
    std::ifstream stream(source, std::ios::binary | std::ios::ate);
    need(bool(stream) && stream.tellg() == 2834942, "Exact GA resource byte extent");
    Bytes raw(2834942); stream.seekg(0);
    stream.read(reinterpret_cast<char*>(raw.data()), raw.size()); need(bool(stream), "Short source read");
    rr::assets::ResArchive48C archive; rr::assets::DecodeError48C error;
    need(rr::assets::decodeResource48C(raw, archive, error), "Native RES admission: " + error.code);
    // Exact authored extent from watchtower_source_fixture.h; the visual
    // profile's 32 MiB value is an admission ceiling, not the archive length.
    need(archive.entries.size() == 970 && archive.decoded.size() == 26099712,
         "Exact GA member/decoded extent differs");
    std::size_t total = 0;
    for (const auto& row : archive.entries) {
        need(row.bytes <= 8u*1024u*1024u, "Member byte bound"); total += row.bytes;
        need(total <= 128u*1024u*1024u, "Member aggregate bound");
    }
    need(fs::create_directory(output), "Cannot exclusively create output directory");
    J report = {{"schema", "RR_ORIGINAL_GA_EXTRACT_V1"}, {"members", J::array()}};
    for (std::size_t i = 0; i < archive.entries.size(); ++i) {
        const auto& row = archive.entries[i]; Bytes bytes;
        need(rr::assets::extractResMember48C(archive, row.name, bytes, error), "Native extraction: " + error.code);
        need(bytes.size() == row.bytes, "Extracted member extent differs");
        // Archive names never become native output paths at this boundary.
        const auto filename = std::to_string(i) + ".bin";
        fresh(output / filename, bytes);
        report["members"].push_back({{"name", row.name}, {"source_index", i},
            {"logical_offset", row.logical_offset}, {"bytes", row.bytes}, {"file", filename}});
    }
    const auto text = report.dump(2) + "\n";
    fresh(output / "extraction-index.json", Bytes(text.begin(), text.end()));
    return 0;
} catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; } }
