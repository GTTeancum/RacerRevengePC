// SOURCE-ONLY, UNCOMPILED cold extraction wrapper. No game/guest execution.
// Uses the existing authenticated archive owner, including its archive AND
// every member SHA256 checks. Writes only one fixed-name authenticated member.
#include "assets/original_ui_archive50kg.h"
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <fcntl.h>
#include <unistd.h>

namespace fs = std::filesystem;
namespace a = rr::assets;
static void need(bool okay, const char* why) {
    if (!okay) throw std::runtime_error(why);
}

int main(int argc, char** argv) {
    try {
        need(argc == 3, "Usage: extract_rider19_pst AUTHENTIC_UIRIDER.RES NEW_OUTPUT_DIRECTORY");
        const fs::path input(argv[1]), output(argv[2]);
        need(!fs::is_symlink(input) && fs::is_regular_file(input), "Regular archive required");
        need(!fs::exists(output) && !fs::is_symlink(output), "Output directory must be new");
        std::ifstream stream(input, std::ios::binary | std::ios::ate);
        need(bool(stream) && stream.tellg() == 5552683, "Exact UIRIDER.RES extent required");
        std::vector<std::uint8_t> raw(5552683);
        stream.seekg(0);
        stream.read(reinterpret_cast<char*>(raw.data()), raw.size());
        need(bool(stream), "Short archive read");
        a::OriginalUiArchive50KG archive;
        a::OriginalUiArchiveDiagnostic50KG diagnostic;
        need(archive.load(a::OriginalUiArchiveId50KG::rider, raw, diagnostic),
             "Original archive/member authentication or bounded RES decode failed");
        const auto member = archive.find("ui\\textures\\19c_1.pst");
        need(member.identity && member.data && member.size == 23096 &&
             member.identity->bytes == 23096 && member.identity->logicalOffset == 1228800 &&
             std::strcmp(member.identity->sha256,
                 "ad0bcd92f415d65d8bf68ae9e65fe355043e297e834cdffeae4ceac7cfa60cd3") == 0,
             "Exact authenticated Rider19 PST identity/layout unavailable");
        need(fs::create_directory(output), "Cannot create fresh output directory");
        const auto target = output / "19c_1.pst";
        const int fd = ::open(target.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
        need(fd >= 0, "Refusing existing or invalid output file");
        std::size_t at = 0;
        while (at < member.size) {
            const auto written = ::write(fd, member.data + at, member.size - at);
            if (written < 0 && errno == EINTR) continue;
            if (written <= 0) { ::close(fd); throw std::runtime_error("Short output write"); }
            at += static_cast<std::size_t>(written);
        }
        const bool synced = ::fsync(fd) == 0;
        const bool closed = ::close(fd) == 0;
        need(synced && closed, "Output flush/close failed");
        std::cout << "authenticated ui\\textures\\19c_1.pst: bytes=23096 logical_offset=1228800 "
                     "sha256=ad0bcd92f415d65d8bf68ae9e65fe355043e297e834cdffeae4ceac7cfa60cd3\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
