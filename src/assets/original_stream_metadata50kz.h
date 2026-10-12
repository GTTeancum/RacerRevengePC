#pragma once
#include "original_ui_archive50kg.h"
#include "import_world48f.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace rr::assets {
inline constexpr std::size_t originalStreamMetadataRows50KZ = 1743;
inline constexpr std::size_t originalStreamDirectoryMember50KZ = 4;
inline constexpr std::size_t originalStreamParametersMember50KZ = 5;

enum class StreamMetadataStatus50KZ : std::uint8_t {
    none, alreadyPrepared, archiveNotPrepared, archiveIdentity, memberIdentity,
    directoryFormat, directoryCount, duplicateName, parameterFormat,
    songCoverage, songCount, fieldShape, filenameMismatch, numericValue,
    pcmKeyShape, allocation, unexpectedFailure
};
struct StreamMetadataDiagnostic50KZ {
    StreamMetadataStatus50KZ status = StreamMetadataStatus50KZ::none;
    std::size_t row = static_cast<std::size_t>(-1);
    std::size_t member = static_cast<std::size_t>(-1);
    std::size_t byteOffset = 0;
    DecodeError48C parameter;
};

// Immutable authoring only. Every offset is relative to its source member,
// never an archive-compressed byte offset. All five ParameterField48F values
// own the exact source text, lowercase label, label-table entry offset, value
// offset and NUL-inclusive value byte count from APPSTART member5.
struct OriginalStreamMetadataRow50KZ {
    std::uint32_t sourceIndex = 0; // TXT ordinal, NOT a sample handle.
    std::uint32_t songNumber = 0;  // Authenticated "song N", N=sourceIndex+1.
    std::size_t manifestByteOffset = 0; // APPSTART member4 token start.
    std::string authoredName; // Exact TXT token spelling.
    std::string sourceName;   // ASCII lowercase; original separators retained.
    std::string pcmRequestName; // Separate lowercase slash-normalized .vag key.
    std::string authoredBlockName; // Exact original label, including its space.
    std::size_t blockNameByteOffset = 0, blockTableEntryByteOffset = 0;
    std::size_t blockByteOffset = 0;
    std::uint32_t blockSizeWithoutCount = 0;
    ParameterField48F authoredFilename, authoredDuration, authoredPitch;
    ParameterField48F authoredSampleRate, authoredSize;
    // Host admission of authoredDuration.value via locale-independent
    // from_chars directly to IEEE binary32. Original302E10/310440 conversion
    // bit equivalence has NOT been established. Not a finite-PCM duration.
    float admittedDurationSeconds = 0.f;
    // Literal original integer authoring, NOT a normalized playback multiplier.
    std::uint32_t originalPitch = 0, originalSampleRate = 0, originalSize = 0;
};

// Cold, load-once, transactional immutable owner. Only an actually loaded,
// authenticated APPSTART archive can authorize preparation; no public raw
// bytes, caller-provided member view or claimed digest admission path exists.
// Names, source fields and offsets are copied. The archive can be destroyed
// after preparation. Published const row addresses stay stable for this
// owner's lifetime. Serial preparation; const lookup performs no allocation,
// I/O, decoder call, voice operation or mutable registry publication.
class OriginalStreamMetadata50KZ final {
public:
    OriginalStreamMetadata50KZ() noexcept;
    ~OriginalStreamMetadata50KZ();
    OriginalStreamMetadata50KZ(const OriginalStreamMetadata50KZ&) = delete;
    OriginalStreamMetadata50KZ& operator=(const OriginalStreamMetadata50KZ&) = delete;
    OriginalStreamMetadata50KZ(OriginalStreamMetadata50KZ&&) = delete;
    OriginalStreamMetadata50KZ& operator=(OriginalStreamMetadata50KZ&&) = delete;
    bool prepare(const OriginalUiArchive50KG&, StreamMetadataDiagnostic50KZ&) noexcept;
    bool prepared() const noexcept;
    std::size_t size() const noexcept;
    const OriginalStreamMetadataRow50KZ* row(std::size_t sourceIndex) const noexcept;
    // Full source key, ASCII case-insensitive only. No slash equivalence,
    // basename, extension, prefix, filesystem or PCM-key fallback. This is
    // already-normalized directory-key lookup, not original266C50's mutable
    // preload adapter, which strips exactly one final CR or LF from a request.
    const OriginalStreamMetadataRow50KZ* find(std::string_view sourceName) const noexcept;
    // Only an actually stream-marked authored request may use this convenience
    // lookup. The caller supplies its source-directory spelling, not a PCM key.
    // A returned row says nothing about sample availability or playback.
    const OriginalStreamMetadataRow50KZ* findForStreamRequest(
        bool authoredStream, std::string_view sourceName) const noexcept;
    // APPSTART member5 [songs] numberofsongs field, including original text
    // and member-relative field provenance; nullptr until successful prepare.
    const ParameterField48F* songCountField() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
