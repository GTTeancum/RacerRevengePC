// NEW source-only reconstruction; not recovered original lost source.
#include "original_stream_metadata50kz.h"
#include <array>
#include <charconv>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <utility>
#include <vector>

namespace rr::assets {
namespace {
using Status = StreamMetadataStatus50KZ;
bool fail(StreamMetadataDiagnostic50KZ& d, Status status,
    std::size_t row = static_cast<std::size_t>(-1),
    std::size_t member = static_cast<std::size_t>(-1), std::size_t offset = 0) noexcept {
    d.status = status;
    d.row = row;
    d.member = member;
    d.byteOffset = offset;
    return false;
}
unsigned char lowerAscii(unsigned char c) noexcept {
    return c >= 'A' && c <= 'Z' ? static_cast<unsigned char>(c + ('a' - 'A')) : c;
}
bool sameSourceName(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (lowerAscii(static_cast<unsigned char>(a[i])) !=
            lowerAscii(static_cast<unsigned char>(b[i]))) return false;
    return true;
}
bool delimiter(unsigned char c) noexcept {
    // Same source298740 ResourceList separators as the ordinary directory.
    return c == ' ' || c == '\r' || c == '\n';
}
bool positiveInteger(std::string_view text, std::uint32_t maximum,
    std::uint32_t& result) noexcept {
    if (text.empty()) return false;
    std::uint32_t value = 0;
    for (const char c : text) {
        if (c < '0' || c > '9') return false;
        const auto digit = static_cast<std::uint32_t>(c - '0');
        if (value > maximum / 10u ||
            (value == maximum / 10u && digit > maximum % 10u)) return false;
        value = value * 10u + digit;
    }
    if (!value) return false;
    result = value;
    return true;
}
bool admittedDuration(const std::string& text, float& result) noexcept {
    // Deliberately admit the fixed unsigned decimal source shape, without
    // locale, signs, exponent, NaN/Inf, whitespace or partial-conversion rules.
    const auto dot = text.find('.');
    if (dot == std::string::npos || dot == 0 || dot + 1 == text.size()) return false;
    for (std::size_t i = 0; i < text.size(); ++i)
        if (i != dot && (text[i] < '0' || text[i] > '9')) return false;
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
        "IEEE binary32 required for admitted authored duration");
    float value = 0.f;
    const auto conversion = std::from_chars(text.data(), text.data() + text.size(),
        value, std::chars_format::fixed);
    if (conversion.ec != std::errc{} || conversion.ptr != text.data() + text.size() ||
        !std::isfinite(value) || value <= 0.f) return false;
    result = value;
    return true;
}
bool makePcmKey(const std::string& sourceName, std::string& result) {
    // Matches the existing AudioBank50HC key character/path contract. This
    // separate convenience string is NEVER used for source-name association.
    if (sourceName.empty() || sourceName.size() > 240) return false;
    result = sourceName;
    for (auto& c : result) {
        if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
            c == '_' || c == '-' || c == '.' || c == '/' || c == '\\')) return false;
        if (c == '\\') c = '/';
    }
    if (result.front() == '/' || result.back() == '/' ||
        result.find("//") != std::string::npos || result.size() <= 4 ||
        result.compare(result.size() - 4, 4, ".vag") != 0) return false;
    std::size_t begin = 0;
    while (begin < result.size()) {
        const auto separator = result.find('/', begin);
        const auto end = separator == std::string::npos ? result.size() : separator;
        const auto length = end - begin;
        if ((length == 1 && result[begin] == '.') ||
            (length == 2 && result[begin] == '.' && result[begin + 1] == '.')) return false;
        begin = end + 1;
    }
    return true;
}
bool authenticMember(OriginalUiMemberView50KG member, std::size_t ordinal,
    const char* name, std::uint32_t offset, std::size_t size, const char* digest) noexcept {
    constexpr auto appStart = OriginalUiArchiveId50KG::appStart;
    return member.data && member.identity &&
        member.identity == originalUiMemberIdentity50KG(appStart, ordinal) &&
        member.size == size && member.identity->bytes == size &&
        member.identity->logicalOffset == offset &&
        std::strcmp(member.identity->name, name) == 0 &&
        std::strcmp(member.identity->sha256, digest) == 0;
}
}

struct OriginalStreamMetadata50KZ::Storage {
    std::array<OriginalStreamMetadataRow50KZ, originalStreamMetadataRows50KZ> rows;
    ParameterField48F numberOfSongs;
};
OriginalStreamMetadata50KZ::OriginalStreamMetadata50KZ() noexcept = default;
OriginalStreamMetadata50KZ::~OriginalStreamMetadata50KZ() = default;
bool OriginalStreamMetadata50KZ::prepare(const OriginalUiArchive50KG& archive,
    StreamMetadataDiagnostic50KZ& diagnostic) noexcept {
    diagnostic = {};
    if (storage_) return fail(diagnostic, Status::alreadyPrepared);
    if (!archive.loaded()) return fail(diagnostic, Status::archiveNotPrepared);
    constexpr auto appStart = OriginalUiArchiveId50KG::appStart;
    if (archive.identity() != originalUiArchiveIdentity50KG(appStart))
        return fail(diagnostic, Status::archiveIdentity);
    const auto manifest = archive.member(originalStreamDirectoryMember50KZ);
    const auto juk = archive.member(originalStreamParametersMember50KZ);
    if (!authenticMember(manifest, 4, "audio\\Streams.txt", 147456u, 55535u,
        "f231c4a527bd6fc5b1ad39337736fa6e072f39cec0313080d0cb5c07c0bb8aa6"))
        return fail(diagnostic, Status::memberIdentity, static_cast<std::size_t>(-1), 4);
    if (!authenticMember(juk, 5, "audio\\streams.juk", 221184u, 518223u,
        "0cee7d838db0bc3fbfa03513f759ede05c746a0a66f1adf8f5c12d2c402eee7b"))
        return fail(diagnostic, Status::memberIdentity, static_cast<std::size_t>(-1), 5);
    try {
        auto next = std::make_unique<Storage>();
        std::size_t at = 0, count = 0;
        while (at < manifest.size) {
            if (delimiter(manifest.data[at])) { ++at; continue; }
            const auto begin = at;
            while (at < manifest.size && !delimiter(manifest.data[at])) {
                if (manifest.data[at] < 33 || manifest.data[at] >= 127)
                    return fail(diagnostic, Status::directoryFormat, count, 4, at);
                ++at;
            }
            if (count >= next->rows.size())
                return fail(diagnostic, Status::directoryCount, count, 4, begin);
            auto& row = next->rows[count];
            row.sourceIndex = static_cast<std::uint32_t>(count);
            row.songNumber = static_cast<std::uint32_t>(count + 1);
            row.manifestByteOffset = begin;
            row.authoredName.assign(reinterpret_cast<const char*>(manifest.data + begin), at - begin);
            row.sourceName = row.authoredName;
            for (auto& c : row.sourceName)
                c = static_cast<char>(lowerAscii(static_cast<unsigned char>(c)));
            for (std::size_t i = 0; i < count; ++i)
                if (next->rows[i].sourceName == row.sourceName)
                    return fail(diagnostic, Status::duplicateName, count, 4, begin);
            if (!makePcmKey(row.sourceName, row.pcmRequestName))
                return fail(diagnostic, Status::pcmKeyShape, count, 4, begin);
            ++count;
        }
        if (count != next->rows.size())
            return fail(diagnostic, Status::directoryCount, count, 4, manifest.size);

        const std::vector<std::uint8_t> bytes(juk.data, juk.data + juk.size);
        ParameterDocument48F parameters;
        WorldLimits48F limits;
        limits.input_bytes = 518223u;
        limits.blocks = 1744;
        limits.fields_per_block = 5;
        if (!decodeParameters48F(bytes, parameters, diagnostic.parameter, nullptr, limits))
            return fail(diagnostic, Status::parameterFormat, static_cast<std::size_t>(-1),
                5, diagnostic.parameter.offset);
        if (parameters.blocks.size() != next->rows.size() + 1)
            return fail(diagnostic, Status::songCoverage, static_cast<std::size_t>(-1), 5);
        std::array<bool, originalStreamMetadataRows50KZ> seen{};
        bool foundCount = false;
        for (const auto& block : parameters.blocks) {
            if (block.name == "songs") {
                if (foundCount || block.fields.size() != 1 ||
                    block.fields[0].name != "numberofsongs")
                    return fail(diagnostic, Status::fieldShape, static_cast<std::size_t>(-1), 5, block.offset);
                std::uint32_t songs = 0;
                if (!positiveInteger(block.fields[0].value, 1743u, songs) || songs != 1743u)
                    return fail(diagnostic, Status::songCount, static_cast<std::size_t>(-1),
                        5, block.fields[0].value_offset);
                next->numberOfSongs = block.fields[0];
                foundCount = true;
                continue;
            }
            std::uint32_t number = 0;
            if (block.name.compare(0, 5, "song ") != 0 || block.name.size() <= 5 ||
                block.name[5] == '0' || !positiveInteger(std::string_view(block.name).substr(5), 1743u, number))
                return fail(diagnostic, Status::songCoverage, static_cast<std::size_t>(-1), 5, block.name_offset);
            const auto index = static_cast<std::size_t>(number - 1);
            if (seen[index]) return fail(diagnostic, Status::songCoverage, index, 5, block.name_offset);
            seen[index] = true;
            constexpr const char* names[] = {"duration", "filename", "pitch", "samplerate", "size"};
            if (block.fields.size() != 5)
                return fail(diagnostic, Status::fieldShape, index, 5, block.offset);
            for (std::size_t i = 0; i < 5; ++i)
                if (block.fields[i].name != names[i])
                    return fail(diagnostic, Status::fieldShape, index, 5, block.fields[i].entry_offset);
            auto& row = next->rows[index];
            if (!sameSourceName(row.sourceName, block.fields[1].value))
                return fail(diagnostic, Status::filenameMismatch, index, 5, block.fields[1].value_offset);
            row.authoredBlockName = block.name;
            row.blockNameByteOffset = block.name_offset;
            row.blockTableEntryByteOffset = block.table_entry_offset;
            row.blockByteOffset = block.offset;
            row.blockSizeWithoutCount = block.size_without_count;
            row.authoredDuration = block.fields[0];
            row.authoredFilename = block.fields[1];
            row.authoredPitch = block.fields[2];
            row.authoredSampleRate = block.fields[3];
            row.authoredSize = block.fields[4];
            if (!admittedDuration(row.authoredDuration.value, row.admittedDurationSeconds))
                return fail(diagnostic, Status::numericValue, index, 5, row.authoredDuration.value_offset);
            // Preserve full uint32 source type, while rejecting any value that
            // could lose bits at the original later stream-backing halfword store.
            if (!positiveInteger(row.authoredPitch.value, 65535u, row.originalPitch))
                return fail(diagnostic, Status::numericValue, index, 5, row.authoredPitch.value_offset);
            if (!positiveInteger(row.authoredSampleRate.value,
                    std::numeric_limits<std::uint32_t>::max(), row.originalSampleRate))
                return fail(diagnostic, Status::numericValue, index, 5, row.authoredSampleRate.value_offset);
            if (!positiveInteger(row.authoredSize.value,
                    std::numeric_limits<std::uint32_t>::max(), row.originalSize))
                return fail(diagnostic, Status::numericValue, index, 5, row.authoredSize.value_offset);
        }
        if (!foundCount) return fail(diagnostic, Status::songCount, static_cast<std::size_t>(-1), 5);
        for (std::size_t i = 0; i < seen.size(); ++i)
            if (!seen[i]) return fail(diagnostic, Status::songCoverage, i, 5);
        storage_ = std::move(next);
        return true;
    } catch (const std::bad_alloc&) { return fail(diagnostic, Status::allocation); }
    catch (...) { return fail(diagnostic, Status::unexpectedFailure); }
}
bool OriginalStreamMetadata50KZ::prepared() const noexcept { return bool(storage_); }
std::size_t OriginalStreamMetadata50KZ::size() const noexcept {
    return storage_ ? storage_->rows.size() : 0;
}
const OriginalStreamMetadataRow50KZ* OriginalStreamMetadata50KZ::row(std::size_t index) const noexcept {
    return storage_ && index < storage_->rows.size() ? &storage_->rows[index] : nullptr;
}
const OriginalStreamMetadataRow50KZ* OriginalStreamMetadata50KZ::find(std::string_view name) const noexcept {
    if (storage_) for (const auto& row : storage_->rows)
        if (sameSourceName(row.sourceName, name)) return &row;
    return nullptr;
}
const OriginalStreamMetadataRow50KZ* OriginalStreamMetadata50KZ::findForStreamRequest(
    bool authoredStream, std::string_view name) const noexcept {
    return authoredStream ? find(name) : nullptr;
}
const ParameterField48F* OriginalStreamMetadata50KZ::songCountField() const noexcept {
    return storage_ ? &storage_->numberOfSongs : nullptr;
}
}
