#pragma once
#include "native_asset48b.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rr::assets {
constexpr std::uint32_t maxMaterialTextureAssets50EA = 4096;
constexpr std::uint32_t maxMaterialTextureSlots50EA = 65536;
constexpr std::size_t maxMaterialTextureRGBABytes50EA = 256u * 1024u * 1024u;
constexpr std::size_t maxMaterialTextureSourceBytes50EA =
    maxMaterialTextureRGBABytes50EA + std::size_t(maxMaterialTextureAssets50EA) * 16u;
constexpr std::uint32_t unresolvedMaterialTexture50EA = 0xffffffffu;

// Opaque caller identity, not a filename hash or an inferred channel. Identity
// zero is valid. Distinct source assets must use distinct assetIdentity values
// if their material tables share this bank. pass and slot retain the caller's
// exact numbering (for Material48E, the original one-based pass/map numbers).
struct MaterialTextureSlotKey50EA {
    std::uint64_t assetIdentity = 0;
    std::uint32_t materialIndex = 0, pass = 0, slot = 0;
};
enum class MaterialTextureRequirement50EA : std::uint8_t {
    required, optionalUntextured
};
struct MaterialTextureAssetInput50EA {
    std::uint64_t identity = 0;
    // Actual resolved RRTEX001 bytes, borrowed during load only. An absent
    // resource is omitted from this table, never represented by null bytes.
    const std::vector<std::uint8_t>* textureBytes = nullptr;
};
struct MaterialTextureSlotInput50EA {
    MaterialTextureSlotKey50EA key{};
    std::uint64_t textureIdentity = 0;
    MaterialTextureRequirement50EA requirement = MaterialTextureRequirement50EA::required;
};
struct MaterialTextureBudgets50EA {
    // Caller-selected ceilings, not reservation requests. Zero permits no
    // resources of that class. dimensionCapacity must be positive when any
    // texture is supplied. All fields may only reduce the hard ceilings.
    std::uint32_t textureCapacity = 0, slotCapacity = 0;
    std::size_t sourceByteCapacity = 0, rgbaByteCapacity = 0;
    std::uint32_t dimensionCapacity = 0;
};
struct MaterialTextureAssetsInput50EA {
    const MaterialTextureAssetInput50EA* textures = nullptr;
    std::uint32_t textureCount = 0;
    const MaterialTextureSlotInput50EA* slots = nullptr;
    std::uint32_t slotCount = 0;
    MaterialTextureBudgets50EA budgets{};
};
struct MaterialTextureAsset50EA {
    std::uint64_t identity = 0;
    Texture48B texture;
};
struct MaterialTextureSlot50EA {
    MaterialTextureSlotKey50EA key{};
    std::uint64_t textureIdentity = 0;
    MaterialTextureRequirement50EA requirement = MaterialTextureRequirement50EA::required;
    // Index into the original input texture table. Missing optional entries
    // retain their key, requested identity and policy with this sentinel.
    std::uint32_t textureIndex = unresolvedMaterialTexture50EA;
};
enum class MaterialTextureAssetError50EA : std::uint8_t {
    none, alreadyLoaded, input, budget, serializedTexture,
    duplicateTextureIdentity, duplicateSlot, missingRequiredTexture,
    texture, allocation, unexpectedFailure
};
struct MaterialTextureAssetDiagnostic50EA {
    MaterialTextureAssetError50EA error = MaterialTextureAssetError50EA::none;
    // Original input-table indexes; the sentinel means no relevant index.
    std::uint32_t texture = unresolvedMaterialTexture50EA;
    std::uint32_t slot = unresolvedMaterialTexture50EA;
    std::size_t byteOffset = 0;
    std::string parserMessage;
};

// Cold, load-once owner of exact decoded Texture48B RGBA bytes and the complete
// declared slot table. No path search, source decoding, channel interpretation,
// fallback pixels, GL upload, sampling state or renderer calls occur here.
// All source byte totals, header lengths/dimensions and decoded RGBA totals
// pass before any bank allocation or parseTexture48B call. Present malformed
// textures always reject, even if referenced only by optional slots.
//
// Failure leaves an empty owner retryable; success publishes the whole bank.
// Reload rejects without changing published data or pointer identities.
class MaterialTextureAssets50EA final {
public:
    MaterialTextureAssets50EA() noexcept;
    ~MaterialTextureAssets50EA();
    MaterialTextureAssets50EA(const MaterialTextureAssets50EA&) = delete;
    MaterialTextureAssets50EA& operator=(const MaterialTextureAssets50EA&) = delete;
    MaterialTextureAssets50EA(MaterialTextureAssets50EA&&) = delete;
    MaterialTextureAssets50EA& operator=(MaterialTextureAssets50EA&&) = delete;

    // Serial, cold call. Input/diagnostic storage must be distinct and all
    // borrowed arrays/vectors must have stable contents and valid extents.
    // Empty and all-optional-unresolved banks are valid explicit inputs.
    bool load(const MaterialTextureAssetsInput50EA&,
        MaterialTextureAssetDiagnostic50EA&) noexcept;

    bool loaded() const noexcept;
    std::uint32_t textureCount() const noexcept;
    std::uint32_t slotCount() const noexcept;
    std::size_t sourceByteCount() const noexcept;
    std::size_t rgbaByteCount() const noexcept;
    // Immutable borrowed access, no allocation. Index getters preserve input
    // order. Lookups compare only exact caller keys/identities, in O(log N).
    // Null means unloaded, invalid index, unknown identity/key, or unresolved
    // optional slot for textureForSlot(). Keep the owner alive for all readers.
    const MaterialTextureAsset50EA* texture(std::uint32_t index) const noexcept;
    const MaterialTextureAsset50EA* findTexture(std::uint64_t identity) const noexcept;
    const MaterialTextureSlot50EA* slot(std::uint32_t index) const noexcept;
    const MaterialTextureSlot50EA* findSlot(const MaterialTextureSlotKey50EA&) const noexcept;
    const Texture48B* textureForSlot(std::uint32_t index) const noexcept;

private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
