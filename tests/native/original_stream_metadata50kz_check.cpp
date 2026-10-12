// Prospective source-only regression. Not compiled or run by its author.
// Argument: exact retained APPSTART.RES. No audio bank/decoder/device or guest.
#include "../../src/assets/original_stream_metadata50kz.h"
#include <array>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <type_traits>
#include <vector>

namespace a = rr::assets;
namespace {
std::size_t checks = 0;
void require(bool condition, const char* label) {
    ++checks;
    if (!condition) { std::cerr << label << '\n'; std::exit(1); }
}
std::vector<std::uint8_t> read(const char* path) {
    std::ifstream input(path, std::ios::binary);
    require(bool(input), "retained APPSTART input opens");
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}
std::uint32_t word(a::OriginalUiMemberView50KG member, std::size_t at) {
    require(at <= member.size && member.size - at >= 4, "independent original word extent");
    return std::uint32_t(member.data[at]) | (std::uint32_t(member.data[at + 1]) << 8) |
        (std::uint32_t(member.data[at + 2]) << 16) | (std::uint32_t(member.data[at + 3]) << 24);
}
std::string originalText(a::OriginalUiMemberView50KG member, std::size_t at,
    std::size_t bytes, bool padded = false) {
    require(bytes && at <= member.size && bytes <= member.size - at,
        "independent original text extent");
    std::size_t length = 0;
    while (length < bytes && member.data[at + length]) ++length;
    require(length < bytes && (padded || length + 1 == bytes), "original source text terminator");
    for (std::size_t i = length; i < bytes; ++i)
        require(member.data[at + i] == 0, "original field text has zero suffix");
    return {reinterpret_cast<const char*>(member.data + at), length};
}
std::string lower(std::string text, bool slash = false) {
    for (auto& c : text) {
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + 32);
        if (slash && c == '\\') c = '/';
    }
    return text;
}
std::uint32_t integer(const std::string& text) {
    std::uint32_t n = 0;
    const auto result = std::from_chars(text.data(), text.data() + text.size(), n);
    require(result.ec == std::errc{} && result.ptr == text.data() + text.size(),
        "original integer literal completely converts");
    return n;
}
void fieldMatches(const a::ParameterField48F& field, a::OriginalUiMemberView50KG juk,
    std::size_t base, std::size_t ordinal, const char* expectedLabel) {
    const auto entry = base + ordinal * 43;
    const auto valueOffset = base + word(juk, entry + 35);
    const auto valueBytes = word(juk, entry + 39);
    require(field.name == expectedLabel && field.name == originalText(juk, entry, 35, true),
        "retained label matches original independent label-table bytes");
    require(field.entry_offset == entry && field.value_offset == valueOffset &&
        field.value_bytes == valueBytes, "retained source field offsets are original member-relative values");
    require(field.value == originalText(juk, valueOffset, valueBytes),
        "retained authored text matches exact original source bytes");
}
void inspectAll(const a::OriginalStreamMetadata50KZ& metadata,
    a::OriginalUiMemberView50KG manifest, a::OriginalUiMemberView50KG juk) {
    // Independent raw member inspection in this prospective test. This does
    // not call the implementation's BINARY parser or sort by Filename.
    std::size_t at = 0, ordinal = 0;
    while (at < manifest.size) {
        const auto c = manifest.data[at];
        if (c == ' ' || c == '\r' || c == '\n') { ++at; continue; }
        const auto begin = at;
        while (at < manifest.size && manifest.data[at] != ' ' &&
            manifest.data[at] != '\r' && manifest.data[at] != '\n') ++at;
        const auto* row = metadata.row(ordinal);
        const std::string token(reinterpret_cast<const char*>(manifest.data + begin), at - begin);
        require(row && row->sourceIndex == ordinal && row->songNumber == ordinal + 1 &&
            row->manifestByteOffset == begin, "all1743 TXT ordinals and byte offsets retained");
        require(row->authoredName == token && row->sourceName == lower(token) &&
            row->pcmRequestName == lower(token, true), "three source/PCM name forms remain distinct");
        require(metadata.find(token) == row && metadata.find(row->sourceName) == row &&
            metadata.findForStreamRequest(true, token) == row &&
            !metadata.findForStreamRequest(false, token), "actual stream marking gates request lookup");
        require(row->authoredBlockName == "song " + std::to_string(ordinal + 1),
            "numeric original song suffix associates with TXT ordinal, including exact space");
        require(lower(row->authoredFilename.value) == row->sourceName,
            "all1743 actual JUK Filename values agree with original TXT numeric order");
        require(std::isfinite(row->admittedDurationSeconds) && row->admittedDurationSeconds > 0.f,
            "every admitted host duration is positive finite binary32");
        float admitted = 0.f;
        const auto& duration = row->authoredDuration.value;
        const auto conversion = std::from_chars(duration.data(), duration.data() + duration.size(),
            admitted, std::chars_format::fixed);
        require(conversion.ec == std::errc{} && conversion.ptr == duration.data() + duration.size() &&
            admitted == row->admittedDurationSeconds, "documented host float admission, no original bit-parity claim");
        require(row->originalPitch == integer(row->authoredPitch.value) && row->originalPitch > 0 &&
            row->originalPitch <= 65535u &&
            row->originalSampleRate == integer(row->authoredSampleRate.value) && row->originalSampleRate > 0 &&
            row->originalSize == integer(row->authoredSize.value) && row->originalSize > 0,
            "all original32-bit integers preserved with explicit later halfword pitch bound");
        ++ordinal;
    }
    require(ordinal == 1743 && ordinal == metadata.size(), "actual complete1743 TXT inventory");
    require(juk.size >= 15 && std::memcmp(juk.data, "BINARY", 7) == 0 &&
        word(juk, 7) == 1 && word(juk, 11) == 1744, "independent original BINARY1 header");
    struct Header { std::string name; std::size_t nameAt; };
    std::vector<Header> headers;
    at = 15;
    for (std::size_t i = 0; i < 1744; ++i) {
        const auto size = word(juk, at);
        at += 4;
        headers.push_back({originalText(juk, at, size), at});
        at += size;
    }
    require(headers[0].name == "song 1" && headers[1].name == "song 10" &&
        headers[2].name == "song 100" && headers.back().name == "songs",
        "original lexical physical order is not source numeric order");
    const auto table = at;
    std::array<bool, 1743> seen{};
    bool countSeen = false;
    for (std::size_t i = 0; i < headers.size(); ++i) {
        const auto entry = table + i * 8;
        const auto block = word(juk, entry);
        const auto size = word(juk, entry + 4);
        const auto base = std::size_t(block) + 4;
        if (headers[i].name == "songs") {
            require(!countSeen && word(juk, block) == 1, "single actual songs count block");
            const auto* field = metadata.songCountField();
            require(field && field->value == "1743" && field->value_offset == 0x7e84au,
                "actual count text and independently read original byte offset");
            fieldMatches(*field, juk, base, 0, "numberofsongs");
            countSeen = true;
            continue;
        }
        require(headers[i].name.compare(0, 5, "song ") == 0, "actual original spaced song label");
        const auto number = integer(headers[i].name.substr(5));
        require(number >= 1 && number <= 1743 && !seen[number - 1], "complete unique numeric song coverage");
        seen[number - 1] = true;
        const auto* row = metadata.row(number - 1);
        require(row && row->authoredBlockName == headers[i].name &&
            row->blockNameByteOffset == headers[i].nameAt && row->blockTableEntryByteOffset == entry &&
            row->blockByteOffset == block && row->blockSizeWithoutCount == size,
            "every row owns actual original block provenance independent of lexical order");
        require(word(juk, block) == 5, "every original song has the five actual fields");
        fieldMatches(row->authoredDuration, juk, base, 0, "duration");
        fieldMatches(row->authoredFilename, juk, base, 1, "filename");
        fieldMatches(row->authoredPitch, juk, base, 2, "pitch");
        fieldMatches(row->authoredSampleRate, juk, base, 3, "samplerate");
        fieldMatches(row->authoredSize, juk, base, 4, "size");
    }
    require(countSeen, "count block actually inspected");
    for (const bool present : seen) require(present, "every numeric song actually inspected");
}
bool sameField(const a::ParameterField48F& x, const a::ParameterField48F& y) {
    return x.name == y.name && x.value == y.value && x.entry_offset == y.entry_offset &&
        x.value_offset == y.value_offset && x.value_bytes == y.value_bytes;
}
void example(const a::OriginalStreamMetadata50KZ& metadata, std::size_t index, const char* name,
    const char* duration, std::uint32_t pitch, std::uint32_t rate, std::uint32_t size,
    std::size_t filenameOffset, std::size_t durationOffset) {
    const auto* row = metadata.row(index);
    require(row && row == metadata.find(name) && row->authoredFilename.value == name &&
        row->authoredDuration.value == duration && row->originalPitch == pitch &&
        row->originalSampleRate == rate && row->originalSize == size &&
        row->authoredFilename.value_offset == filenameOffset &&
        row->authoredDuration.value_offset == durationOffset,
        "representative original constants from independent static text/byte inspection");
}
}
static_assert(!std::is_copy_constructible_v<a::OriginalStreamMetadata50KZ>);
static_assert(!std::is_move_constructible_v<a::OriginalStreamMetadata50KZ>);
static_assert(std::is_same_v<decltype(a::OriginalStreamMetadataRow50KZ::originalPitch), std::uint32_t>);
static_assert(std::is_same_v<decltype(a::OriginalStreamMetadataRow50KZ::originalSampleRate), std::uint32_t>);
static_assert(std::is_same_v<decltype(a::OriginalStreamMetadataRow50KZ::originalSize), std::uint32_t>);

int main(int argc, char** argv) {
    require(argc == 2, "provide exact retained APPSTART.RES path");
    auto original = read(argv[1]);
    a::OriginalStreamMetadata50KZ metadata;
    a::StreamMetadataDiagnostic50KZ diagnostic;
    require(!metadata.prepared() && !metadata.size() && !metadata.row(0) &&
        !metadata.find("speech\\Pod01\\AN1RDL01.VAG") && !metadata.songCountField(),
        "empty metadata owner fabricates no records");
    std::array<const a::OriginalStreamMetadataRow50KZ*, 1743> pointers{};
    std::vector<a::OriginalStreamMetadataRow50KZ> snapshots;
    a::ParameterField48F savedCount;
    {
        a::OriginalUiArchive50KG archive;
        a::OriginalUiArchiveDiagnostic50KG archiveDiagnostic;
        require(!metadata.prepare(archive, diagnostic) &&
            diagnostic.status == a::StreamMetadataStatus50KZ::archiveNotPrepared && !metadata.prepared(),
            "unloaded archive rejects without publishing and remains retryable");
        require(!original.empty(), "nonempty original APPSTART bytes");
        auto changed = original;
        changed.back() ^= 1;
        require(!archive.load(a::OriginalUiArchiveId50KG::appStart, changed, archiveDiagnostic) &&
            !archive.loaded(), "changed archive bytes cannot establish authentic archive owner");
        require(!metadata.prepare(archive, diagnostic) && !metadata.prepared(),
            "failed authentication cannot authorize metadata publication");
        require(archive.load(a::OriginalUiArchiveId50KG::appStart, original, archiveDiagnostic),
            "exact original archive authenticates");
        require(metadata.prepare(archive, diagnostic) && metadata.prepared() && metadata.size() == 1743 &&
            diagnostic.status == a::StreamMetadataStatus50KZ::none,
            "actual authenticated original directory and JUK admit transactionally");
        inspectAll(metadata, archive.member(4), archive.member(5));
        for (std::size_t i = 0; i < pointers.size(); ++i) {
            pointers[i] = metadata.row(i);
            snapshots.push_back(*pointers[i]);
        }
        savedCount = *metadata.songCountField();
        require(!metadata.prepare(archive, diagnostic) &&
            diagnostic.status == a::StreamMetadataStatus50KZ::alreadyPrepared && metadata.row(0) == pointers[0],
            "repeated prepare preserves published immutable storage");
    }
    original.clear();
    original.shrink_to_fit();
    for (std::size_t i = 0; i < pointers.size(); ++i) {
        const auto* row = metadata.row(i);
        const auto& saved = snapshots[i];
        require(row == pointers[i] && row->sourceIndex == saved.sourceIndex &&
            row->songNumber == saved.songNumber && row->manifestByteOffset == saved.manifestByteOffset &&
            row->authoredName == saved.authoredName && row->sourceName == saved.sourceName &&
            row->pcmRequestName == saved.pcmRequestName && row->authoredBlockName == saved.authoredBlockName &&
            row->blockNameByteOffset == saved.blockNameByteOffset &&
            row->blockTableEntryByteOffset == saved.blockTableEntryByteOffset &&
            row->blockByteOffset == saved.blockByteOffset && row->blockSizeWithoutCount == saved.blockSizeWithoutCount &&
            row->admittedDurationSeconds == saved.admittedDurationSeconds && row->originalPitch == saved.originalPitch &&
            row->originalSampleRate == saved.originalSampleRate && row->originalSize == saved.originalSize &&
            sameField(row->authoredFilename, saved.authoredFilename) &&
            sameField(row->authoredDuration, saved.authoredDuration) &&
            sameField(row->authoredPitch, saved.authoredPitch) &&
            sameField(row->authoredSampleRate, saved.authoredSampleRate) &&
            sameField(row->authoredSize, saved.authoredSize),
            "all1743 rows own stable names, integers, floats and provenance after archive/input destruction");
    }
    require(sameField(*metadata.songCountField(), savedCount), "count field owns source text and provenance");
    example(metadata, 0, "speech\\Dax\\Tracks\\Coruscnt\\DAXCRA23.VAG", "13.055238", 1881, 22050, 164496,
        0x9273u, 0x92a8u);
    example(metadata, 6, "speech\\Dax\\Tracks\\Tatooine\\DAXTAA01.VAG", "14.933333", 1881, 22050, 188160,
        0x682b8u, 0x682edu);
    example(metadata, 1309, "speech\\pod13\\BENRBA01.VAG", "1.222857", 3763, 44100, 30816,
        0x20335u, 0x2035bu);
    example(metadata, 1657, "speech\\Pod01\\AN1RDL01.VAG", "0.632381", 1881, 22050, 7968,
        0x39c1du, 0x39c42u);
    example(metadata, 1692, "Music\\UI\\Screens\\ui_main.vag", "17.420952", 1881, 22050, 219504,
        0x3c5aeu, 0x3c5d8u);
    example(metadata, 1735, "Music\\Race\\Combat\\combat01.vag", "20.389841", 1881, 22050, 256912,
        0x3f9b1u, 0x3f9ddu);
    const auto* voice = metadata.row(1657);
    require(voice->admittedDurationSeconds > 13916.f / 22050.f,
        "authored duration remains distinct from retained documented finite PCM endpoint; no decoding");
    require(metadata.row(1742)->authoredName == "Music\\Race\\Practice\\practc1.vag" &&
        !metadata.row(1743) && !metadata.row(std::numeric_limits<std::size_t>::max()),
        "original final ordinal and out-of-range rejection");
    require(metadata.find("SPEECH\\POD01\\AN1RDL01.VAG") == voice,
        "source full-key ASCII case folding");
    require(!metadata.find(voice->pcmRequestName) && !metadata.find("AN1RDL01.VAG") &&
        !metadata.find("audio\\speech\\Pod01\\AN1RDL01.VAG") &&
        !metadata.find("speech\\Pod01\\AN1RDL01") && !metadata.find(""),
        "no PCM-separator, basename, audio-prefix, extension or empty fallback");
    require(!metadata.find(voice->sourceName + '\r') &&
        !metadata.find(voice->sourceName + '\n') &&
        !metadata.find(voice->sourceName + "\r\n"),
        "strict directory-key lookup does not implement mutable preload request CR/LF trimming");
    std::string nulName = voice->authoredName;
    nulName += '\0';
    nulName += "suffix";
    require(!metadata.find(nulName), "embedded NUL cannot truncate lookup into a match");
    a::OriginalUiArchive50KG emptyArchive;
    require(!metadata.prepare(emptyArchive, diagnostic) &&
        diagnostic.status == a::StreamMetadataStatus50KZ::alreadyPrepared && metadata.row(1657) == voice,
        "invalid later input cannot replace published metadata");
    std::cout << "original_stream_metadata50kz: " << checks << " checks; authored metadata only\n";
}
