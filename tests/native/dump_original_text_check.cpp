// Prints selected original English localization entries from LOC.RES.
// Argument: original asset root. Read-only development aid.
#include "../../src/assets/original_ui_archive50kg.h"
#include "../../src/assets/original_ui_localization50kg.h"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <vector>
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    std::ifstream file(std::string(argv[1]) + "/LOC.RES", std::ios::binary);
    const std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(file), {}};
    rr::assets::OriginalUiArchive50KG archive;
    rr::assets::OriginalUiArchiveDiagnostic50KG archiveDiagnostic;
    if (!archive.load(rr::assets::OriginalUiArchiveId50KG::localization, bytes, archiveDiagnostic)) return 3;
    rr::assets::OriginalUiEnglish50KG english;
    rr::assets::OriginalUiLocalizationDiagnostic50KG diagnostic;
    if (!rr::assets::readOriginalUiEnglish50KG(archive, english, diagnostic)) return 4;
    const unsigned ids[] = {4, 8, 172, 173, 401, 424, 425, 106, 107, 108, 109, 110, 111, 112,
        114, 115, 116, 117, 118, 119};
    for (unsigned id : ids)
        if (const auto* entry = english.at(id)) std::printf("%u=%s\n", id, entry->text.c_str());
    return 0;
}
