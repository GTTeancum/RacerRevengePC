// Prospective source-only regression. Not executed by the implementation author.
// Arguments: authenticated retained APPSTART.RES, original extracted tracks/gc.aub.
// Reads only those inputs. No audio bank, PCM decoder, device or gameplay path.
#include "../../src/assets/original_audio_directory50kx.h"
#include "../../src/assets/material_audio50hi.h"
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <type_traits>

namespace a = rr::assets;
namespace {
std::size_t checks = 0;
void require(bool value, const char* label) {
    ++checks;
    if (!value) { std::cerr << label << '\n'; std::exit(1); }
}
std::vector<std::uint8_t> read(const char* path) {
    std::ifstream input(path, std::ios::binary);
    require(bool(input), "retained input opens");
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}
std::string folded(std::string value) {
    for (auto& c : value) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + 32);
    return value;
}
void inspectEveryManifestRow(const a::OriginalAudioDirectory50KX& directory,
    a::OriginalUiMemberView50KG manifest) {
    std::size_t start = 0, ordinal = 0;
    while (start < manifest.size) {
        const auto c = manifest.data[start];
        if (c == ' ' || c == '\r' || c == '\n') { ++start; continue; }
        std::size_t end = start;
        while (end < manifest.size && manifest.data[end] != ' ' &&
            manifest.data[end] != '\r' && manifest.data[end] != '\n') ++end;
        const std::string original(reinterpret_cast<const char*>(manifest.data + start), end - start);
        const auto* row = directory.row(ordinal);
        require(row && row->sourceIndex == ordinal && row->manifestByteOffset == start,
            "every actual token retains source ordinal and original byte offset");
        require(row->authoredName == original && row->sourceName == folded(original),
            "every actual token retains original spelling and source lowercase");
        require(directory.find(original) == row && directory.find(row->sourceName) == row,
            "every original token resolves without reordering or aliasing");
        ++ordinal;
        start = end;
    }
    require(ordinal == 267 && ordinal == directory.size(), "actual267-row source directory");
}
const a::ParameterField48F* sourceField(const a::ParameterDocument48F& parameters,
    const std::string& name) {
    for (const auto& block : parameters.blocks) if (block.name == name)
        for (const auto& field : block.fields) if (field.name == "soundresourcefile") return &field;
    return nullptr;
}
}
static_assert(!std::is_copy_constructible_v<a::OriginalAudioDirectory50KX>);
static_assert(!std::is_move_constructible_v<a::OriginalAudioDirectory50KX>);
static_assert(!std::is_copy_constructible_v<a::GcMaterialResourceBindings50KX>);
static_assert(!std::is_move_constructible_v<a::GcMaterialResourceBindings50KX>);

int main(int argc, char** argv) {
    require(argc == 3, "provide retained APPSTART.RES and original extracted GC AUB paths");
    const auto appBytes = read(argv[1]);
    auto gcBytes = read(argv[2]);
    a::OriginalAudioDirectory50KX directory;
    a::GcMaterialResourceBindings50KX binding;
    a::AudioDirectoryDiagnostic50KX diagnostic;
    require(!directory.prepared() && !directory.size() && !directory.row(0) &&
        !directory.find("audio\\sfx\\race\\pod_coll.vag"), "empty directory has no fabricated row0");
    require(!binding.prepare(directory, gcBytes, diagnostic) &&
        diagnostic.status == a::AudioDirectoryStatus50KX::directoryNotPrepared &&
        !binding.prepared() && !binding.rowCountPerKind() && !binding.collision(1),
        "unprepared directory cannot authorize GC binding");
    const a::OriginalAudioDirectoryRow50KX* retainedRow = nullptr;
    {
        a::OriginalUiArchive50KG archive;
        require(!directory.prepare(archive, diagnostic) &&
            diagnostic.status == a::AudioDirectoryStatus50KX::archiveNotPrepared && !directory.prepared(),
            "unprepared archive rejects without publication");
        a::OriginalUiArchiveDiagnostic50KG archiveDiagnostic;
        auto corrupt = appBytes;
        require(!corrupt.empty(), "nonempty original archive bytes");
        corrupt.back() ^= 1;
        require(!archive.load(a::OriginalUiArchiveId50KG::appStart, corrupt, archiveDiagnostic) &&
            !archive.loaded(), "changed archive cannot establish authenticated owner");
        require(archive.load(a::OriginalUiArchiveId50KG::appStart, appBytes, archiveDiagnostic),
            "retained archive authenticates with exact member identities");
        require(directory.prepare(archive, diagnostic) &&
            diagnostic.status == a::AudioDirectoryStatus50KX::none, "directory publishes once after real authentication");
        inspectEveryManifestRow(directory, archive.member(3));
        retainedRow = directory.row(109);
        require(!directory.prepare(archive, diagnostic) &&
            diagnostic.status == a::AudioDirectoryStatus50KX::alreadyPrepared && directory.row(109) == retainedRow,
            "repeated prepare preserves published directory addresses");
    }
    require(retainedRow && retainedRow == directory.row(109) &&
        retainedRow->sourceName == "audio\\sfx\\race\\pod_coll.vag",
        "directory owns names after authenticated archive is destroyed");
    require(directory.row(0)->sourceName == "audio\\sfx\\ui\\badcall.vag" &&
        directory.row(266)->sourceName == "audio\\sfx\\race\\cavecry1.vag" && !directory.row(267),
        "actual first and last ordinals, no synthetic extra row");
    require(directory.find("AUDIO\\SFX\\RACE\\POD_COLL.VAG") == retainedRow,
        "source case-insensitive key comparison");
    require(!directory.find("audio/sfx/race/pod_coll.vag") && !directory.find("pod_coll.vag") &&
        !directory.find("audio\\sfx\\race\\pod_coll") && !directory.find(""),
        "no slash normalization, basename, prefix or empty fallback");
    std::string embeddedNul = "audio\\sfx\\race\\pod_coll.vag";
    embeddedNul += '\0';
    embeddedNul += "suffix";
    require(!directory.find(embeddedNul), "embedded NUL does not truncate lookup into a match");

    auto corruptGc = gcBytes;
    require(corruptGc.size() == 32438u, "retained GC member byte count");
    corruptGc.back() ^= 1;
    require(!binding.prepare(directory, corruptGc, diagnostic) &&
        diagnostic.status == a::AudioDirectoryStatus50KX::gcAubIdentity && !binding.prepared(),
        "any changed GC byte rejects, including unrelated authoring");
    corruptGc.pop_back();
    require(!binding.prepare(directory, corruptGc, diagnostic) &&
        diagnostic.status == a::AudioDirectoryStatus50KX::gcAubIdentity && !binding.prepared(),
        "truncated GC member rejects and stays retryable");
    require(binding.prepare(directory, gcBytes, diagnostic) && binding.prepared() &&
        diagnostic.status == a::AudioDirectoryStatus50KX::none && binding.rowCountPerKind() == 12,
        "actual GC binding completes after earlier failures");

    constexpr std::uint32_t collisionIndices[] = {109,109,109,109,127,128,129,130,167,168,169,170};
    constexpr std::uint32_t driveIndices[] = {114,115,116,117,122,123,124,125,163,164,165,166};
    // Independently read original BINARY field offsets, retained in the source
    // reconstruction's GC-resource-ordinals.json; not another decoder result.
    constexpr std::uint32_t collisionNameOffsets[] = {4917,5177,5242,5307,5372,5437,5502,5567,5632,4982,5047,5112};
    constexpr std::uint32_t driveNameOffsets[] = {5955,7327,7670,8013,8356,8699,9042,9385,9728,6298,6641,6984};
    a::MaterialAudioCatalog50HI catalog;
    std::vector<a::AudioRequest50HC> requests;
    std::string error;
    require(a::readMaterialAudioCatalog50HI(gcBytes, catalog, requests, error) && requests.size() == 21,
        "existing catalog supplies actual24 rows and21 distinct requests");
    a::ParameterDocument48F parameters;
    a::DecodeError48C decode;
    a::WorldLimits48F limits{};
    limits.retainDuplicateLabels = true;
    require(a::decodeParameters48F(gcBytes, parameters, decode, nullptr, limits),
        "actual original BINARY authoring decodes with unrelated labels retained");
    for (std::uint32_t key = 1; key <= 12; ++key) {
        const auto* collision = binding.collision(key);
        const auto* drive = binding.drive(key);
        require(collision && drive && collision != drive && collision->sourceKey == key && drive->sourceKey == key,
            "collision and drive are distinct ordered immutable metadata records");
        require(collision->sourceResourceIndex == collisionIndices[key - 1] &&
            drive->sourceResourceIndex == driveIndices[key - 1], "actual original global resource ordinals");
        require(collision->aubValueByteOffset == collisionNameOffsets[key - 1] &&
            drive->aubValueByteOffset == driveNameOffsets[key - 1],
            "AUB filename byte offsets agree with independent retained source constants");
        require(collision->pcmRequestName == catalog.collision[key - 1] &&
            drive->pcmRequestName == catalog.drive[key - 1], "exact existing PCM catalog names retained");
        for (unsigned kind = 0; kind < 2; ++kind) {
            const auto* value = kind ? drive : collision;
            const auto* source = sourceField(parameters,
                std::string(kind ? "materialdriveonsound" : "materialcollisionsound") + std::to_string(key));
            require(source && value->aubValueByteOffset == source->value_offset &&
                value->sourceFactoryFilename == source->value + ".wav" &&
                value->sourceRegistryName == "audio\\" + source->value + ".vag",
                "factory .wav and registry .vag are separately derived from exact original AUB field");
            require(directory.find(value->sourceRegistryName) == directory.row(value->sourceResourceIndex),
                "every GC source key resolves its actual267-row entry");
        }
        const auto* controls = binding.driveAuthoring(key);
        require(controls && controls->sampleSpeed == 100 && controls->minVolume == 0 && controls->minSpeed == 1 &&
            controls->maxSpeed == 60 && controls->resonantSpeed == 100 && controls->frequencySlope == .75f,
            "six actual authored GC controls, no invented sample pitch field");
    }
    require(binding.collision(1) != binding.collision(4) &&
        binding.collision(1)->sourceResourceIndex == binding.collision(4)->sourceResourceIndex,
        "duplicate resource retains four separate authored collision records");
    require(!binding.collision(0) && !binding.collision(13) && !binding.drive(0) &&
        !binding.drive(13) && !binding.driveAuthoring(0) && !binding.driveAuthoring(13),
        "out-of-range material keys reject without default selection");
    const auto* saved = binding.drive(12);
    require(!binding.prepare(directory, corruptGc, diagnostic) &&
        diagnostic.status == a::AudioDirectoryStatus50KX::alreadyPrepared && binding.drive(12) == saved,
        "repeated GC prepare preserves immutable bindings even with invalid new input");
    a::GcMaterialResourceBindings50KX afterDirectory;
    const a::GcMaterialResourceBinding50KX* afterDirectoryAddress = nullptr;
    std::array<const a::GcMaterialResourceBinding50KX*, 24> afterDirectoryPointers{};
    {
        a::OriginalUiArchive50KG temporaryArchive;
        a::OriginalUiArchiveDiagnostic50KG archiveDiagnostic;
        a::OriginalAudioDirectory50KX temporaryDirectory;
        require(temporaryArchive.load(a::OriginalUiArchiveId50KG::appStart, appBytes, archiveDiagnostic) &&
            temporaryDirectory.prepare(temporaryArchive, diagnostic) &&
            afterDirectory.prepare(temporaryDirectory, gcBytes, diagnostic),
            "independent binding prepares from a separately owned temporary directory");
        afterDirectoryAddress = afterDirectory.drive(12);
        for (std::uint32_t key = 1; key <= 12; ++key) {
            afterDirectoryPointers[key - 1] = afterDirectory.collision(key);
            afterDirectoryPointers[12 + key - 1] = afterDirectory.drive(key);
        }
    }
    gcBytes.clear();
    gcBytes.shrink_to_fit();
    require(saved->sourceFactoryFilename == "sfx\\race\\wodscrp4.wav" &&
        saved->sourceRegistryName == "audio\\sfx\\race\\wodscrp4.vag",
        "binding names own their storage after original input is destroyed");
    require(afterDirectory.prepared() && afterDirectory.drive(12) == afterDirectoryAddress,
        "binding record address survives its supplying directory and archive destruction");
    for (std::uint32_t key = 1; key <= 12; ++key) {
        for (unsigned kind = 0; kind < 2; ++kind) {
            const auto* original = kind ? binding.drive(key) : binding.collision(key);
            const auto* survivor = kind ? afterDirectory.drive(key) : afterDirectory.collision(key);
            require(survivor && survivor == afterDirectoryPointers[kind * 12 + key - 1] &&
                survivor->sourceKey == original->sourceKey &&
                survivor->sourceResourceIndex == original->sourceResourceIndex &&
                survivor->aubValueByteOffset == original->aubValueByteOffset &&
                survivor->sourceFactoryFilename == original->sourceFactoryFilename &&
                survivor->sourceRegistryName == original->sourceRegistryName &&
                survivor->pcmRequestName == original->pcmRequestName,
                "all24 bindings own names and ordinals after directory/archive/input lifetime ends");
        }
    }
    std::cout << "original_audio_directory50kx: " << checks << " checks; metadata only\n";
}
