#pragma once
#include "original_ui_archive50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace rr::assets {
inline constexpr std::size_t originalAudioDirectoryRows50KX = 267;
inline constexpr std::size_t gcMaterialSoundRows50KX = 12;

enum class AudioDirectoryStatus50KX : std::uint8_t {
    none, alreadyPrepared, archiveNotPrepared, archiveIdentity, memberIdentity,
    manifestFormat, manifestCount, duplicateName, directoryNotPrepared,
    gcAubIdentity, gcAubFormat, gcCatalog, missingResource, allocation,
    unexpectedFailure
};
struct AudioDirectoryDiagnostic50KX {
    AudioDirectoryStatus50KX status = AudioDirectoryStatus50KX::none;
    std::size_t row = static_cast<std::size_t>(-1);
};

// Immutable source directory metadata only. No mutable row+4 value, reference
// count, sound owner, sample result, voice, group or audio service is implied.
struct OriginalAudioDirectoryRow50KX {
    std::uint32_t sourceIndex = 0;
    std::uint32_t manifestByteOffset = 0;
    std::string authoredName; // Exact AudioStr token spelling.
    std::string sourceName;   // Source2985B0 ASCII lowercase spelling.
};

// Cold load-once owner. Input must be the actual authenticated appStart owner;
// a caller-supplied member view or claimed digest is intentionally insufficient.
// Names are copied, so the archive may be destroyed after successful prepare.
// Failure leaves an empty owner retryable; repeated prepare preserves published
// storage. Returned const views remain stable until this owner is destroyed.
class OriginalAudioDirectory50KX final {
public:
    OriginalAudioDirectory50KX() noexcept;
    ~OriginalAudioDirectory50KX();
    OriginalAudioDirectory50KX(const OriginalAudioDirectory50KX&) = delete;
    OriginalAudioDirectory50KX& operator=(const OriginalAudioDirectory50KX&) = delete;
    OriginalAudioDirectory50KX(OriginalAudioDirectory50KX&&) = delete;
    OriginalAudioDirectory50KX& operator=(OriginalAudioDirectory50KX&&) = delete;
    bool prepare(const OriginalUiArchive50KG&, AudioDirectoryDiagnostic50KX&) noexcept;
    bool prepared() const noexcept;
    std::size_t size() const noexcept;
    const OriginalAudioDirectoryRow50KX* row(std::size_t sourceIndex) const noexcept;
    // Source308F00 full-name ASCII case-insensitive comparison. No separator
    // normalization, basename fallback, prefix matching, allocation or I/O.
    const OriginalAudioDirectoryRow50KX* find(std::string_view sourceName) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};

// The six authored controls only. A sample base pitch is deliberately absent:
// native normalized4096 is not evidence of source provider metadata.
struct GcMaterialDriveAuthoring50KX {
    std::int32_t sampleSpeed = 0, minVolume = 0, minSpeed = 0;
    std::int32_t maxSpeed = 0, resonantSpeed = 0;
    float frequencySlope = 0.f;
};
struct GcMaterialResourceBinding50KX {
    std::uint32_t sourceKey = 0;            // Source ordinal+1 within its kind.
    std::uint32_t sourceResourceIndex = 0;  // AudioStr ordinal, never a sample.
    std::uint32_t aubValueByteOffset = 0;
    std::string sourceFactoryFilename;     // Actual2787B0 name + ".wav".
    std::string sourceRegistryName;        // Actual233110 "audio\\" + .vag.
    std::string pcmRequestName;            // Existing50HI normalized .vag name.
};

// Independent immutable GC metadata. Exact AUB bytes are authenticated here;
// no claimed digest or unverified catalog can authorize publication. Existing
// decodeParameters48F/readMaterialAudioCatalog50HI provide the BINARY parser
// and normalized catalog. No PCM is loaded or decoded. Bindings own all names
// and numeric ordinals and do not borrow the directory or input vector.
class GcMaterialResourceBindings50KX final {
public:
    GcMaterialResourceBindings50KX() noexcept;
    ~GcMaterialResourceBindings50KX();
    GcMaterialResourceBindings50KX(const GcMaterialResourceBindings50KX&) = delete;
    GcMaterialResourceBindings50KX& operator=(const GcMaterialResourceBindings50KX&) = delete;
    GcMaterialResourceBindings50KX(GcMaterialResourceBindings50KX&&) = delete;
    GcMaterialResourceBindings50KX& operator=(GcMaterialResourceBindings50KX&&) = delete;
    bool prepare(const OriginalAudioDirectory50KX&,
        const std::vector<std::uint8_t>& originalGcAub,
        AudioDirectoryDiagnostic50KX&) noexcept;
    bool prepared() const noexcept;
    std::size_t rowCountPerKind() const noexcept;
    const GcMaterialResourceBinding50KX* collision(std::uint32_t sourceKey) const noexcept;
    const GcMaterialResourceBinding50KX* drive(std::uint32_t sourceKey) const noexcept;
    const GcMaterialDriveAuthoring50KX* driveAuthoring(std::uint32_t sourceKey) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
