#include "material_texture_assets50ea.h"
#include <algorithm>
#include <cstring>
#include <new>
#include <stdexcept>

namespace rr::assets {
namespace {
bool reject(MaterialTextureAssetDiagnostic50EA& d, MaterialTextureAssetError50EA error) noexcept {
    d.error = error;
    return false;
}
void clearLocation(MaterialTextureAssetDiagnostic50EA& d) noexcept {
    d.texture = d.slot = unresolvedMaterialTexture50EA;
    d.byteOffset = 0;
}
std::uint32_t readU32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
        (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
bool addBounded(std::size_t& total, std::size_t count, std::size_t capacity) noexcept {
    if (total > capacity || count > capacity - total) return false;
    total += count;
    return true;
}
bool keyLess(const MaterialTextureSlotKey50EA& a, const MaterialTextureSlotKey50EA& b) noexcept {
    if (a.assetIdentity != b.assetIdentity) return a.assetIdentity < b.assetIdentity;
    if (a.materialIndex != b.materialIndex) return a.materialIndex < b.materialIndex;
    if (a.pass != b.pass) return a.pass < b.pass;
    return a.slot < b.slot;
}
bool keyEqual(const MaterialTextureSlotKey50EA& a, const MaterialTextureSlotKey50EA& b) noexcept {
    return a.assetIdentity == b.assetIdentity && a.materialIndex == b.materialIndex &&
        a.pass == b.pass && a.slot == b.slot;
}
bool budgetsValid(const MaterialTextureBudgets50EA& b) noexcept {
    return b.textureCapacity <= maxMaterialTextureAssets50EA &&
        b.slotCapacity <= maxMaterialTextureSlots50EA &&
        b.sourceByteCapacity <= maxMaterialTextureSourceBytes50EA &&
        b.rgbaByteCapacity <= maxMaterialTextureRGBABytes50EA &&
        b.dimensionCapacity <= max_texture_dimension;
}

// Nonallocating validation of the complete native texture framing, not a
// dimension guess from unchecked bytes. The existing parser remains the sole
// constructor of Texture48B and preserves the original RGBA payload verbatim.
bool scanTexture(const std::vector<std::uint8_t>& bytes,
    const MaterialTextureBudgets50EA& budgets, std::size_t& rgbaBytes,
    MaterialTextureAssetDiagnostic50EA& d) noexcept {
    if (bytes.size() < 16 || bytes.size() > max_asset_file_bytes ||
        std::memcmp(bytes.data(), "RRTEX001", 8) != 0)
        return reject(d, MaterialTextureAssetError50EA::serializedTexture);
    const auto width = readU32(bytes.data() + 8), height = readU32(bytes.data() + 12);
    if (!width || width > max_texture_dimension) {
        d.byteOffset = 8;
        return reject(d, MaterialTextureAssetError50EA::serializedTexture);
    }
    if (!height || height > max_texture_dimension) {
        d.byteOffset = 12;
        return reject(d, MaterialTextureAssetError50EA::serializedTexture);
    }
    const std::uint64_t pixels = 4ull * width * height;
    if (16ull + pixels != bytes.size()) {
        d.byteOffset = 16;
        return reject(d, MaterialTextureAssetError50EA::serializedTexture);
    }
    if (width > budgets.dimensionCapacity || height > budgets.dimensionCapacity) {
        d.byteOffset = width > budgets.dimensionCapacity ? 8 : 12;
        return reject(d, MaterialTextureAssetError50EA::budget);
    }
    if (!addBounded(rgbaBytes, static_cast<std::size_t>(pixels), budgets.rgbaByteCapacity)) {
        d.byteOffset = 16;
        return reject(d, MaterialTextureAssetError50EA::budget);
    }
    return true;
}
}

struct MaterialTextureAssets50EA::Storage {
    std::vector<MaterialTextureAsset50EA> textures;
    std::vector<MaterialTextureSlot50EA> slots;
    // Lookup indexes sort independently; public table order never changes.
    std::vector<std::uint32_t> textureLookup, slotLookup;
    std::size_t sourceBytes = 0, rgbaBytes = 0;

    std::uint32_t textureIndex(std::uint64_t identity) const noexcept {
        const auto it = std::lower_bound(textureLookup.begin(), textureLookup.end(), identity,
            [this](std::uint32_t index, std::uint64_t key) noexcept {
                return textures[index].identity < key;
            });
        return it != textureLookup.end() && textures[*it].identity == identity ?
            *it : unresolvedMaterialTexture50EA;
    }
};

MaterialTextureAssets50EA::MaterialTextureAssets50EA() noexcept = default;
MaterialTextureAssets50EA::~MaterialTextureAssets50EA() = default;

bool MaterialTextureAssets50EA::load(const MaterialTextureAssetsInput50EA& input,
    MaterialTextureAssetDiagnostic50EA& d) noexcept {
    d.error = MaterialTextureAssetError50EA::none;
    clearLocation(d);
    d.parserMessage.clear();
    if (storage_) return reject(d, MaterialTextureAssetError50EA::alreadyLoaded);
    if ((input.textureCount && !input.textures) || (input.slotCount && !input.slots))
        return reject(d, MaterialTextureAssetError50EA::input);
    const auto& budgets = input.budgets;
    if (!budgetsValid(budgets) || input.textureCount > budgets.textureCapacity ||
        input.slotCount > budgets.slotCapacity || (input.textureCount && !budgets.dimensionCapacity))
        return reject(d, MaterialTextureAssetError50EA::budget);

    // Check ALL source extents before headers, and ALL validated decoded
    // extents before allocating even the cold metadata/lookup tables.
    std::size_t sourceBytes = 0, rgbaBytes = 0;
    for (std::uint32_t t = 0; t < input.textureCount; ++t) {
        d.texture = t;
        const auto* bytes = input.textures[t].textureBytes;
        if (!bytes) return reject(d, MaterialTextureAssetError50EA::input);
        if (!addBounded(sourceBytes, bytes->size(), budgets.sourceByteCapacity))
            return reject(d, MaterialTextureAssetError50EA::budget);
    }
    for (std::uint32_t t = 0; t < input.textureCount; ++t) {
        d.texture = t;
        if (!scanTexture(*input.textures[t].textureBytes, budgets, rgbaBytes, d)) return false;
    }
    clearLocation(d);
    for (std::uint32_t s = 0; s < input.slotCount; ++s) {
        d.slot = s;
        const auto requirement = input.slots[s].requirement;
        if (requirement != MaterialTextureRequirement50EA::required &&
            requirement != MaterialTextureRequirement50EA::optionalUntextured)
            return reject(d, MaterialTextureAssetError50EA::input);
    }
    clearLocation(d);

    try {
        std::unique_ptr<Storage> next(new Storage);
        next->textures.resize(input.textureCount);
        next->textureLookup.resize(input.textureCount);
        next->slots.resize(input.slotCount);
        next->slotLookup.resize(input.slotCount);
        next->sourceBytes = sourceBytes;
        next->rgbaBytes = rgbaBytes;
        for (std::uint32_t t = 0; t < input.textureCount; ++t) {
            next->textures[t].identity = input.textures[t].identity;
            next->textureLookup[t] = t;
        }
        std::sort(next->textureLookup.begin(), next->textureLookup.end(),
            [&next](std::uint32_t a, std::uint32_t b) noexcept {
                const auto ai = next->textures[a].identity, bi = next->textures[b].identity;
                return ai != bi ? ai < bi : a < b;
            });
        for (std::uint32_t n = 1; n < input.textureCount; ++n) {
            const auto previous = next->textureLookup[n - 1], current = next->textureLookup[n];
            if (next->textures[previous].identity == next->textures[current].identity) {
                d.texture = current;
                return reject(d, MaterialTextureAssetError50EA::duplicateTextureIdentity);
            }
        }
        for (std::uint32_t s = 0; s < input.slotCount; ++s) {
            d.slot = s;
            const auto& source = input.slots[s];
            const auto index = next->textureIndex(source.textureIdentity);
            if (index == unresolvedMaterialTexture50EA &&
                source.requirement == MaterialTextureRequirement50EA::required)
                return reject(d, MaterialTextureAssetError50EA::missingRequiredTexture);
            next->slots[s] = {source.key, source.textureIdentity, source.requirement, index};
            next->slotLookup[s] = s;
        }
        clearLocation(d);
        std::sort(next->slotLookup.begin(), next->slotLookup.end(),
            [&next](std::uint32_t a, std::uint32_t b) noexcept {
                const auto& ak = next->slots[a].key;
                const auto& bk = next->slots[b].key;
                return keyEqual(ak, bk) ? a < b : keyLess(ak, bk);
            });
        for (std::uint32_t n = 1; n < input.slotCount; ++n) {
            const auto previous = next->slotLookup[n - 1], current = next->slotLookup[n];
            if (keyEqual(next->slots[previous].key, next->slots[current].key)) {
                d.slot = current;
                return reject(d, MaterialTextureAssetError50EA::duplicateSlot);
            }
        }

        // Every input, budget, identity and slot policy is now admitted. All
        // actual pixel allocation belongs to the existing native parser.
        for (std::uint32_t t = 0; t < input.textureCount; ++t) {
            d.texture = t;
            const auto& bytes = *input.textures[t].textureBytes;
            auto& texture = next->textures[t].texture;
            if (!parseTexture48B(bytes.data(), bytes.size(), texture, d.parserMessage))
                return reject(d, MaterialTextureAssetError50EA::texture);
            if (texture.width != readU32(bytes.data() + 8) ||
                texture.height != readU32(bytes.data() + 12) || texture.rgba.size() != bytes.size() - 16)
                return reject(d, MaterialTextureAssetError50EA::texture);
        }
        storage_.swap(next);
        clearLocation(d);
        return true;
    } catch (const std::bad_alloc&) {
        return reject(d, MaterialTextureAssetError50EA::allocation);
    } catch (const std::length_error&) {
        return reject(d, MaterialTextureAssetError50EA::allocation);
    } catch (...) {
        return reject(d, MaterialTextureAssetError50EA::unexpectedFailure);
    }
}

bool MaterialTextureAssets50EA::loaded() const noexcept { return bool(storage_); }
std::uint32_t MaterialTextureAssets50EA::textureCount() const noexcept {
    return storage_ ? static_cast<std::uint32_t>(storage_->textures.size()) : 0;
}
std::uint32_t MaterialTextureAssets50EA::slotCount() const noexcept {
    return storage_ ? static_cast<std::uint32_t>(storage_->slots.size()) : 0;
}
std::size_t MaterialTextureAssets50EA::sourceByteCount() const noexcept {
    return storage_ ? storage_->sourceBytes : 0;
}
std::size_t MaterialTextureAssets50EA::rgbaByteCount() const noexcept {
    return storage_ ? storage_->rgbaBytes : 0;
}
const MaterialTextureAsset50EA* MaterialTextureAssets50EA::texture(std::uint32_t index) const noexcept {
    return storage_ && index < storage_->textures.size() ? &storage_->textures[index] : nullptr;
}
const MaterialTextureAsset50EA* MaterialTextureAssets50EA::findTexture(std::uint64_t identity) const noexcept {
    return storage_ ? texture(storage_->textureIndex(identity)) : nullptr;
}
const MaterialTextureSlot50EA* MaterialTextureAssets50EA::slot(std::uint32_t index) const noexcept {
    return storage_ && index < storage_->slots.size() ? &storage_->slots[index] : nullptr;
}
const MaterialTextureSlot50EA* MaterialTextureAssets50EA::findSlot(
    const MaterialTextureSlotKey50EA& key) const noexcept {
    if (!storage_) return nullptr;
    const auto it = std::lower_bound(storage_->slotLookup.begin(), storage_->slotLookup.end(), key,
        [this](std::uint32_t index, const MaterialTextureSlotKey50EA& wanted) noexcept {
            return keyLess(storage_->slots[index].key, wanted);
        });
    return it != storage_->slotLookup.end() && keyEqual(storage_->slots[*it].key, key) ?
        &storage_->slots[*it] : nullptr;
}
const Texture48B* MaterialTextureAssets50EA::textureForSlot(std::uint32_t index) const noexcept {
    const auto* binding = slot(index);
    const auto* asset = binding ? texture(binding->textureIndex) : nullptr;
    return asset ? &asset->texture : nullptr;
}
}
