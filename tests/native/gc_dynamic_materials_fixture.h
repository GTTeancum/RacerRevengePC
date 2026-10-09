#pragma once
#include "track_materials50fa_fixture.h"
#include "../../src/assets/import_texture48e.h"
#include <array>

// Cold material/texture owner for the exact original GC opening and windmill.
// Uses the existing native PSM/PST decoders on authenticated original bytes;
// no static-track same-name association, generated image or missing-texture fallback.
namespace gc_dynamic_materials_fixture {
namespace a = rr::assets;
using Selection = track_materials50fa_fixture::Selection;
struct Diagnostic {
    std::uint32_t material = a::unresolvedMaterialTexture50EA;
    std::string message;
    a::DecodeError48C source{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
namespace shared = pod_materials50ed_fixture::detail;
struct Entry {
    const char* name;
    const char* materialSha;
    const char* textureSha;
    const char* nativeTextureSha;
    std::uint32_t materialBytes, textureBytes, width, height, mip, bankIndex;
};
inline constexpr Entry entries[] = {
#include "gc_dynamic_materials_index.inc"
};
static_assert(sizeof(entries)/sizeof(entries[0]) == 6, "Exact original GC dynamic material closure");
inline nlohmann::json fields(const a::MaterialFields48E& source) {
    auto result = nlohmann::json::object();
    result["authored"] = nlohmann::json::array();
    result["unspecified"] = nlohmann::json::array();
    for (const auto& field : source.authored)
        result["authored"].push_back({{"name",field.name},{"value",field.value},{"source_offset",field.source_offset}});
    for (const auto& field : source.unspecified)
        result["unspecified"].push_back({{"name",field.name},{"value",field.value},{"source_offset",field.source_offset}});
    return result;
}
}
class Owner final {
    struct Storage {
        a::MaterialTextureAssets50EA bank;
        std::array<Selection,6> selections;
    };
    std::unique_ptr<Storage> storage_;
public:
    Owner() = default;
    Owner(const Owner&) = delete;
    Owner& operator=(const Owner&) = delete;
    bool loaded() const noexcept { return bool(storage_); }
    const a::MaterialTextureAssets50EA* bank() const noexcept { return storage_ ? &storage_->bank : nullptr; }
    const std::array<Selection,6>* selections() const noexcept { return storage_ ? &storage_->selections : nullptr; }
    // The scene owner authenticates and decodes each original PSG before this
    // call. Material tables must retain original order: opening4, windmill2.
    // Transactional, load once. Texture and material outputs are immutable.
    bool load(const std::filesystem::path& sourceArchive, const a::Scene48D& opening,
        const a::Scene48D& windmill, Diagnostic& d) noexcept {
        using namespace detail;
        using shared::need;
        try {
            d = {};
            need(!storage_, "GC dynamic materials already loaded");
            need(opening.materials.size() == 4 && windmill.materials.size() == 2,
                "GC dynamic original scene material counts");
            for (std::uint32_t i = 0; i < 6; ++i)
                need((i < 4 ? opening.materials[i] : windmill.materials[i-4]) == entries[i].name,
                    "GC dynamic original scene material identity/order mismatch");
            shared::Inputs inputs; inputs.capacity = 8u * 1024u * 1024u;
            auto archiveBytes = inputs.read(sourceArchive, 4205067);
            need(archiveBytes.size() == 4205067 && shared::sha256(archiveBytes) ==
                "69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9",
                "GC dynamic exact original archive identity");
            a::ResArchive48C archive;
            a::DecodeLimits48C limits;
            limits.resource_file_bytes = 4205067; limits.resource_decoded_bytes = 31580160;
            limits.resource_entries = 1190;
            need(a::decodeResource48C(archiveBytes, archive, d.source, nullptr, limits),
                "GC dynamic bounded original archive decode");
            std::unique_ptr<Storage> next(new Storage);
            std::array<std::vector<std::uint8_t>,6> textures;
            std::array<a::MaterialTextureAssetInput50EA,6> images{};
            std::array<a::MaterialTextureSlotInput50EA,6> slots{};
            for (std::uint32_t i = 0; i < 6; ++i) {
                d.material = i;
                const auto& entry = entries[i];
                const std::string stem = std::string("textures\\") + entry.name;
                std::vector<std::uint8_t> psm, pst;
                need(a::extractResMember48C(archive, stem+".psm", psm, d.source, 1024) &&
                    psm.size() == entry.materialBytes && shared::sha256(psm) == entry.materialSha,
                    "GC dynamic original PSM extraction/identity");
                need(a::extractResMember48C(archive, stem+".pst", pst, d.source, 32u*1024u) &&
                    pst.size() == entry.textureBytes && shared::sha256(pst) == entry.textureSha,
                    "GC dynamic original PST extraction/identity");
                a::Material48E material;
                need(a::decodeMaterial48E(psm, material, d.source) && material.passes.size() == 1,
                    "GC dynamic native material decode");
                const auto& pass = material.passes[0];
                need(pass.pass == 1 && pass.microprogram == "DiffuseCompressed" &&
                    pass.textures.size() == 1 && pass.textures[0].slot == 1 && pass.textures[0].name == entry.name,
                    "GC dynamic exact source program/texture map");
                a::Texture48E texture;
                need(a::decodeTexture48E(pst, texture, d.source) && texture.mips.size() == entry.mip+1 &&
                    texture.mips.back().width == entry.width && texture.mips.back().height == entry.height &&
                    shared::sha256(texture.base_rrtex) == entry.nativeTextureSha,
                    "GC dynamic exact native source-decoded base texture");
                textures[i] = std::move(texture.base_rrtex);
                auto& selection = next->selections[i];
                selection.assetIdentity = i < 4 ? 1 : 2;
                selection.materialIndex = i < 4 ? i : i-4;
                selection.materialName = entry.name;
                selection.materialKey = shared::sha256(std::string("rr-native-material-48E-v1\n")+entry.materialSha);
                selection.textureKey = shared::sha256(std::string("rr-native-texture-48E-v1\n")+entry.textureSha);
                selection.textureName = entry.name;
                selection.textureIndex = selection.bankSlot = entry.bankIndex;
                selection.mipIndex = entry.mip;
                selection.pass = selection.slot = selection.passCount = 1;
                selection.track = track_source_fixture::Track::gc;
                selection.program = pass.microprogram;
                selection.sourcePassFields = fields(pass.fields);
                for (const auto& field : pass.fields.authored) {
                    if (field.name != "wrapModeHorizontal" && field.name != "wrapModeVertical") continue;
                    need(field.value == "0" || field.value == "1", "GC dynamic wrap mode unsupported");
                    (field.name == "wrapModeHorizontal" ? selection.clampU : selection.clampV) = field.value == "1";
                }
                images[i] = {std::uint64_t(i)+1, &textures[i]};
                slots[i] = {{selection.assetIdentity, selection.materialIndex,1,1}, std::uint64_t(i)+1,
                    a::MaterialTextureRequirement50EA::required};
            }
            const a::MaterialTextureBudgets50EA budgets{6,6,256u*1024u,256u*1024u,128};
            const a::MaterialTextureAssetsInput50EA input{images.data(),6,slots.data(),6,budgets};
            need(next->bank.load(input, d.bank), "GC dynamic six-texture native bank rejected");
            storage_ = std::move(next); d.material = a::unresolvedMaterialTexture50EA; return true;
        } catch (const std::exception& e) { try { d.message=e.what(); } catch (...) {} return false; }
        catch (...) { try { d.message="Unexpected GC dynamic material preparation failure"; } catch (...) {} return false; }
    }
};
}
