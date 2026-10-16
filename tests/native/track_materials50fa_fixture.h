#pragma once
#include "pod_materials50ed_fixture.h"
#include "track_visual_profile_fixture.h"
#include "ta_original_members50la_fixture.h"
#include "original_racer_material_sources50la_fixture.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../src/assets/track_projected_uv.h"
#include "../../src/assets/import_decode48c.h"
#include <cfenv>
#include <cmath>
#include <locale>

// Cold retained-data bridge for an explicitly authenticated TA or GC mesh.
// It re-derives spans from source RRSCENE1 surfaces and the pinned placed export,
// verifies every live position/UV/color/index, and loads only real RRTEX bytes.
// No image generation, texture substitution, guessed material assignment,
// original shader emulation or GL calls occur here. Only the optional native
// source callback path decodes the bounded original archive.
namespace track_materials50fa_fixture {
namespace a = rr::assets;
constexpr std::uint32_t unresolved = a::unresolvedMaterialTexture50EA;
using MipMode = pod_materials50ed_fixture::MipMode;
using Choice = pod_materials50ed_fixture::Choice;
using TextureSelection = pod_materials50ed_fixture::TextureSelection;
// Caller supplies a reviewed original-format decoder. Input is the exact
// source member, authenticated here before the callback. Output must be a
// real decoded source mip/frame serialized as RRTEX001; never fallback pixels.
using SourceTextureDecoder = bool (*)(const char* sourceMember,
    const std::vector<std::uint8_t>& source, std::vector<std::uint8_t>& rrtex,
    std::uint32_t& mipIndex, std::string& error);
struct Request {
    std::filesystem::path root; // Retained visual49G directory.
    std::filesystem::path exportReport; // Matching 49G placed export report.
    Choice choice{}; // Explicit authored pass/slot and missing-texture policy.
    std::size_t coldSourceByteCapacity = 0;
    a::MaterialTextureBudgets50EA textureBudgets{};
    std::filesystem::path sourceArchive; // Optional exact retained TA.RES.
    SourceTextureDecoder decodeSourceTexture = nullptr;
    // Deliberate host display policy: use an actual decoded ATM start frame until
    // an animation owner exists. False leaves selected ATM sources unresolved.
    bool allowStaticAnimationFrame = false;
    track_source_fixture::Track track = track_source_fixture::Track::ta;
    // GC requires its complete original pass set. TA retains the historical
    // single-pass contract and never silently changes material scope.
    bool includeAllAuthoredPasses = false;
    // Optional original-Main TA/SA racer route. All three are required
    // together, from the SAME live Application/Video. TA alone admits its two
    // pinned world models; SA retains its authenticated all-pass world. Other tracks
    // retain their existing paths. No caller-set readiness or image callback.
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> canonicalDomain;
    std::shared_ptr<const a::OriginalStandardMaterialContext50LA> canonicalContext;
    a::OriginalStandardMaterialPath50LA canonicalMaterialPath;
    struct CanonicalRacerModel {
        const a::OriginalStandardModelResource50LA* resource=nullptr;
        const a::OriginalStandardMaterialReferences50LA* references=nullptr;
    };
    // Actual completed engine resource/ref owners, borrowed for this
    // cold scan. No alternate table or duplicate26E640 retain is introduced.
    std::vector<CanonicalRacerModel> canonicalRacerModels;
};
struct Selection {
    std::uint64_t assetIdentity = 0;
    std::uint32_t materialIndex = 0, pass = 0, slot = 0;
    std::uint32_t bankSlot = unresolved, textureIndex = unresolved, mipIndex = unresolved;
    std::string sceneKey, materialName, materialKey, program, textureName, textureKey;
    std::string unresolvedReason;
    std::string decodedSourceMember, decodedSourceSha256;
    bool staticAnimationFrame = false;
    track_source_fixture::Track track = track_source_fixture::Track::ta;
    std::uint32_t passCount = 1;
    bool projectedPrelit = false;
    bool clampU = false, clampV = false;
    std::array<double,16> texGenMatrix{};
    nlohmann::json sourcePassFields; // Exact authenticated original PSM fields.
    // Diagnostic identities read from the real model1C4 ->382B38 pass ->
    //383400 resource chain that selected this SAME submitted bank slot.
    std::int32_t canonicalMaterialRow = -1;
    std::int16_t canonicalTextureRow = -1;

};
struct Span {
    std::uint32_t indexStart = 0, indexCount = 0, vertexStart = 0, vertexCount = 0;
    std::uint32_t textureIndex = unresolved, materialSelection = unresolved;
    std::uint32_t worldInstanceIndex = 0, surfaceIndex = 0;
    bool hasOriginalColors = false;
    std::uint32_t sourcePass = 1, sourcePassCount = 1, projectionPass = 0;
};
struct Diagnostic {
    std::uint32_t instance = unresolved, surface = unresolved, material = unresolved;
    const char* stage = "input";
    std::string sourceMember;
    std::string message;
    a::OriginalStandardMaterialDiagnostic50LA canonical{};
    original_racer_material_sources50LA::Diagnostic racerSource{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
namespace shared = pod_materials50ed_fixture::detail;
using shared::Bytes; using shared::Json; using shared::Inputs;
using shared::need; using shared::number; using shared::hash; using shared::normalized;
using shared::sha256; using shared::add; using shared::u32;
constexpr const char* ta = "TRACKS/TA.RES";
// Fixed development fixture identity, not a general import format. The files
// are independently retained 49G source selection and exporter evidence.
constexpr const char* selectionSha = "12fad4579276c9fce30abd3009420a8529a033e1d7271ae6b0688927f8adf674";
constexpr const char* exportSha = "ef2bdffbb36f5669c0d84eb9b31e5a3f8f4853b5ab966898bb768efd42eb47c4";
constexpr const char* archiveSha = "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8";
inline bool sameFloat(float x, float y) noexcept { return std::memcmp(&x, &y, sizeof x) == 0; }
inline void resolution(const Json& row, const char* extension, const char* expected, const char* container = ta) {
    const auto name = row.at("name").get<std::string>();
    need(row.at("container") == container && shared::name(name), "Track exact resolution scope/name");
    const auto wanted = normalized("textures/" + name + extension);
    need(row.at("requested_member") == wanted && row.at("expected_status") == expected,
        "Track resolution member/type");
    const auto local = number(row, "local_candidates", 10000), global = number(row, "global_candidates", 10000);
    const auto& candidates = row.at("candidates");
    need(candidates.is_array() && local <= global && candidates.size() == (local ? local : global) &&
        row.at("scope") == (local ? "local" : "global"), "Track resolution candidate count/scope");
    std::set<std::string> sources; bool unsupported = false;
    for (const auto& c : candidates) {
        need(normalized(c.at("member")) == wanted && (!local || normalized(c.at("container")) == container),
            "Track exact local-first candidate");
        const auto source = c.at("source_sha256").get<std::string>();
        need(hash(source), "Candidate source hash"); sources.insert(source);
        unsupported |= c.at("status") != expected;
    }
    if (row.at("status") == "resolved") {
        const auto& selected = row.at("selected");
        need(!candidates.empty() && sources.size() == 1 && !unsupported &&
            std::find(candidates.begin(), candidates.end(), selected) != candidates.end() &&
            selected.at("key") == row.at("key") && hash(row.at("key")), "Track resolved candidate provenance");
    } else {
        const std::string why = row.at("reason");
        need(row.at("status") == "unresolved" &&
            ((candidates.empty() && why == "no_exact_member") ||
             (sources.size() == 1 && unsupported && why == "original_resource_unsupported")),
            "Track unresolved resource provenance");
    }
}
// These rows are parsed from the already hashed original Material48E record.
// Only affine XY ProjectedPrelit is admitted, as the original VU program does.
inline std::array<double,16> projection(const Json& fields) {
    std::array<double,16> matrix{};
    for (unsigned row = 0; row < 4; ++row) {
        const std::string name = "TexGenMatrixRow" + std::to_string(row+1);
        const Json* found = nullptr;
        for (const auto& field : fields.at("authored")) if (field.at("name") == name) {
            need(!found, "Duplicate authored texture matrix row"); found = &field;
        }
        need(found, "Missing authored texture matrix row");
        std::istringstream values(found->at("value").get<std::string>());
        values.imbue(std::locale::classic());
        for (unsigned col = 0; col < 4; ++col) {
            values >> matrix[row*4+col];
            need(bool(values) && std::isfinite(matrix[row*4+col]), "Invalid authored texture matrix scalar");
            if (col != 3) { char comma = 0; values >> comma; need(comma == ',', "Authored texture matrix separator"); }
        }
        values >> std::ws; need(values.eof(), "Authored texture matrix suffix");
    }
    need(matrix[3] == 0 && matrix[7] == 0 && matrix[11] == 0 && matrix[15] == 1,
        "Authored projected texture matrix must be affine");
    return matrix;
}
struct SourceScene {
    a::Scene48D scene;
    std::vector<std::vector<std::uint32_t>> materialSelections;
    const Json* request = nullptr;
    int canonicalTarget = -1;
};
} // namespace detail

class Owner final {
    struct CanonicalModel {
        std::uint32_t worldInstance=unresolved;
        std::shared_ptr<const a::OriginalStandardModelResource50LA> resource;
        // This is the actual26E640/1C0/1C4 facet of the existing visual
        // instance. It does not manufacture a Soultree class tag, unwritten
        // header bytes, a complete StandardModelPrefix or scene registration.
        a::OriginalStandardMaterialReferences50LA materials;
    };
    struct Storage {
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> canonicalDomain;
        std::array<std::unique_ptr<CanonicalModel>,2> canonicalModels;
        a::MaterialTextureAssets50EA bank;
        std::vector<Selection> selections;
        std::vector<Span> spans;
        std::vector<TextureSelection> textures;
        std::vector<a::TrackProjectedUv> projectionUvs;
        std::size_t coldBytes = 0;
    };
    // Source-neutral cold material operations. Admission remains in load();
    // these helpers do not select world targets or activate particle owners.
    // Keep the caller's source vectors alive through world refs/population.
    static void mountCanonicalArchive(Storage& next,const Request& request,
        const track_visual_profile_fixture::Profile& profile,
        const a::ResArchive48C& archive,Diagnostic& d) {
        using detail::need;
        auto& diagnostic=d.canonical;
        next.canonicalDomain=request.canonicalDomain;
        d.stage="canonical_archive_mounts";
        const auto& provider=next.canonicalDomain->archiveProvider3E1A70();
        a::OriginalStandardArchiveProvider50LA::Diagnostic providerDiagnostic;
        need(provider&&provider->mountAuthenticatedRes(profile.container,profile.archiveSha,
            profile.archiveBytes,archive,providerDiagnostic)&&provider->sealNativeMounts(providerDiagnostic),
            "Track complete native archive provider rejected");
        need(next.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,diagnostic.texture),
            "Track canonical resolver must borrow the same mounted provider");
    }
    static void appendCanonicalSources(Storage& next,const Request& request,
        std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>>& originalMaterials,
        std::vector<a::OriginalStandardTextureSourceInput50LA>& originalTextures,Diagnostic& d) {
        using detail::need;
        const auto& provider=next.canonicalDomain->archiveProvider3E1A70();
        for(const auto& racer:request.canonicalRacerModels){
            d.stage="canonical_racer_sources";
            need(racer.resource&&racer.references&&
                racer.references->table382B38()==next.canonicalDomain->materials382B38().get()&&
                original_racer_material_sources50LA::appendModelSources(*racer.resource,*racer.references,
                    *request.canonicalMaterialPath.actualPrefix36B970,*provider,
                    originalMaterials,originalTextures,d.racerSource),
                "Track original model material/texture closure rejected");
        }
        d.stage="canonical_sources";
        const bool bound=next.canonicalDomain->appendOriginalSources(originalMaterials,originalTextures,d.canonical);
        need(bound,"Track original canonical source domain rejected");
    }
    static void populateCanonicalMaterials(Storage& next,const Request& request,Diagnostic& d) {
        using detail::need;
        auto& diagnostic=d.canonical;
        d.stage="canonical_population";d.sourceMember.clear();
        const bool populated=next.canonicalDomain->populate(request.canonicalContext,diagnostic);
        need(populated,
            "Track canonical14D370/29E340 population rejected at sourcePC="+
            std::to_string(diagnostic.sourcePC?diagnostic.sourcePC:diagnostic.texture.sourcePC));
    }
    std::unique_ptr<Storage> storage_;
public:
    Owner() = default;
    Owner(const Owner&) = delete;
    Owner& operator=(const Owner&) = delete;
    Owner(Owner&&) = delete;
    Owner& operator=(Owner&&) = delete;
    bool loaded() const noexcept { return bool(storage_); }
    const a::MaterialTextureAssets50EA* bank() const noexcept { return storage_ ? &storage_->bank : nullptr; }
    const std::vector<Span>* spans() const noexcept { return storage_ ? &storage_->spans : nullptr; }
    const std::vector<Selection>* selections() const noexcept { return storage_ ? &storage_->selections : nullptr; }
    const std::vector<TextureSelection>* textures() const noexcept { return storage_ ? &storage_->textures : nullptr; }
    const std::vector<a::TrackProjectedUv>* projectionUvs() const noexcept { return storage_ ? &storage_->projectionUvs : nullptr; }
    std::size_t coldSourceBytes() const noexcept { return storage_ ? storage_->coldBytes : 0; }

    // Serial cold load, transactional and retryable after failure. Existing
    // publication rejects reload without invalidating views. No references to
    // the caller mesh survive this call. Strict IEEE arithmetic is required.
    bool load(const Request& request, const a::Mesh48B& mesh, Diagnostic& d) noexcept {
        using namespace detail;
        try {
            d = {};
            const auto& profile = track_visual_profile_fixture::get(request.track);
            const bool ga = request.track == track_source_fixture::Track::ga;
            const bool ma = request.track == track_source_fixture::Track::ma;
            const bool sa = request.track == track_source_fixture::Track::sa;
            // These selected original profiles preserve world-space roots,
            // every authored pass and source wrap policy. Archive, profile and
            // source image identities remain independent for every track.
            const bool gc = request.track == track_source_fixture::Track::gc || ga || ma || sa;
            need(!gc || request.includeAllAuthoredPasses, "Selected track requires every authored material pass");
            need(!storage_, "Track materials already loaded");
            need(std::fegetround() == FE_TONEAREST, "Track parity requires FE_TONEAREST");
            need(!request.root.empty() && !request.exportReport.empty(), "Explicit retained TA paths required");
            need(request.coldSourceByteCapacity > 0 &&
                request.coldSourceByteCapacity <= ((ma||sa)?64u*1024u*1024u:pod_materials50ed_fixture::maxColdSourceBytes),
                "Explicit bounded selected-track cold source capacity required");
            const auto& choice = request.choice;
            const bool canonical=bool(request.canonicalDomain);
            const bool canonicalWorld=canonical&&request.track==track_source_fixture::Track::ta;
            need(canonical==bool(request.canonicalContext)&&
                canonical==(request.canonicalMaterialPath.actualPrefix36B970!=nullptr),
                "Track canonical Application/Video/path must be supplied together");
            need(!canonical||((request.track==track_source_fixture::Track::ta||sa)&&
                request.canonicalDomain->prepared()&&request.canonicalContext->live()&&
                *request.canonicalMaterialPath.actualPrefix36B970=="textures\\"&&
                choice.mipMode==MipMode::largestRetained&&request.decodeSourceTexture),
                "Track canonical racers require admitted track, actual gameplay directory, live Video and source archive");
            need(!gc || choice.requirement == a::MaterialTextureRequirement50EA::required,
                "Selected track requires actual textures for the entire authored material closure");
            // Caller selects authored pass/slot one. GC explicitly requests all
            // source passes below; no absent channel or dropped layer is inferred.
            need(choice.pass == 1 && choice.slot == 1 &&
                (choice.requirement == a::MaterialTextureRequirement50EA::required ||
                 choice.requirement == a::MaterialTextureRequirement50EA::optionalUntextured) &&
                (choice.mipMode == MipMode::exactIndex || choice.mipMode == MipMode::largestRetained) &&
                choice.mipIndex <= 6, "Explicit supported TA pass/slot/mip policy required");
            const auto& budget = request.textureBudgets;
            need(budget.textureCapacity <= a::maxMaterialTextureAssets50EA &&
                budget.slotCapacity <= a::maxMaterialTextureSlots50EA &&
                budget.sourceByteCapacity <= a::maxMaterialTextureSourceBytes50EA &&
                budget.rgbaByteCapacity <= a::maxMaterialTextureRGBABytes50EA &&
                budget.dimensionCapacity <= a::max_texture_dimension, "Invalid TA texture budgets");
            need(mesh.vertices.size() == profile.vertices && mesh.indices.size() == profile.indices, "Exact selected track mesh extent required");
            Inputs inputs; inputs.root = std::filesystem::canonical(request.root);
            inputs.capacity = request.coldSourceByteCapacity;
            need(!std::filesystem::is_symlink(inputs.root / "cacheobjects"), "Track cache directory symlink rejected");
            const auto selectedBytes = inputs.read(inputs.root / "asset-selection.json", 4u * 1024u * 1024u);
            const auto reportBytes = inputs.read(request.exportReport, 4u * 1024u * 1024u);
            need(sha256(selectedBytes) == profile.selectionSha && sha256(reportBytes) == profile.exportSha,
                "Track retained selection/export identity mismatch");
            const auto selected = Json::parse(selectedBytes.begin(), selectedBytes.end());
            const auto report = Json::parse(reportBytes.begin(), reportBytes.end());
            need(request.sourceArchive.empty() == (request.decodeSourceTexture == nullptr),
                "Track source archive and decoder must be supplied together");
            a::ResArchive48C archive;
            if (request.decodeSourceTexture) {
                d.stage="original_archive";
                const auto archiveBytes = inputs.read(request.sourceArchive, profile.archiveBytes);
                need(archiveBytes.size() == profile.archiveBytes && sha256(archiveBytes) == profile.archiveSha, "Track original archive identity mismatch");
                a::DecodeLimits48C limits; limits.resource_file_bytes = profile.archiveBytes;
                limits.resource_decoded_bytes = profile.archiveDecodedBytes; limits.resource_entries = profile.archiveEntries;
                a::DecodeError48C error;
                const bool decoded=a::decodeResource48C(archiveBytes,archive,error,nullptr,limits);
                need(decoded,"Track original archive decode: "+error.code+
                    " offset="+std::to_string(error.offset)+" detail="+error.detail);
            }
            d.stage="retained_selection";
            need(selected.at("schema") == "RR_VISUAL_SELECTION49G_V1" && selected.at("status") == "complete" &&
                report.at("schema") == "RR_AUTHORED_MESH49G_V1" && report.at("success") == true,
                "Track selection/export schema/status");
            for (const auto& row : selected.at("objects")) {
                const auto path = row.at("path").get<std::string>();
                need(shared::safeObjectPath(path) && hash(row.at("sha256")) &&
                    inputs.objects.emplace(path, row).second, "Track selected object path/hash/duplicate");
            }
            std::map<std::string, const Json*> materials, textureRows, sceneRows;
            for (const auto& row : selected.at("material_resolution")) if (row.at("container") == profile.container) {
                resolution(row, ".psm", "converted_material", profile.container);
                need(materials.emplace(row.at("name"), &row).second, "Duplicate TA material association");
            }
            for (const auto& row : selected.at("texture_resolution")) if (row.at("container") == profile.container) {
                resolution(row, ".pst", "converted_texture", profile.container);
                need(textureRows.emplace(row.at("name"), &row).second, "Duplicate TA texture association");
            }
            for (const auto& row : selected.at("scene_requests")) if (row.at("container") == profile.container)
                need(sceneRows.emplace(row.at("key"), &row).second, "Duplicate TA source scene");
            need(materials.size() == profile.materials && sceneRows.size() == profile.scenes, "Track retained material/scene extent");
            const auto& instances = report.at("world_instances");
            need(instances.size() == profile.instances && selected.at("visual_instances").size() == instances.size(),
                "Track source world instance extent");
            auto next = std::make_unique<Storage>(); next->spans.reserve(profile.drawSpans);
            if(canonical){
                d.stage="original_provider";
                ta_original_members50la_fixture::Sources sources;std::string why;
                if(canonicalWorld){
                    const bool prepared=sources.prepare(archive,selected,why);
                    need(prepared,"Track original TA provider: "+why);
                }else{
                    // SA supplies only actual installed racer model sources.
                    // Its world spans retain the separately authenticated full
                    // authored-pass path; no TA target/resource pins are reused.
                    need(sa&&!request.canonicalRacerModels.empty(),
                        "SA canonical closure requires actual installed racer models");
                }
                auto& diagnostic=d.canonical;
                mountCanonicalArchive(*next,request,profile,archive,d);
                auto originalMaterials=sources.materials();
                auto originalTextures=sources.textures();
                appendCanonicalSources(*next,request,originalMaterials,originalTextures,d);
                // TA world ownership stays separate from material initialization.
                // Acquire every admitted reference before the one population.
                for(std::size_t target=0;canonicalWorld&&target<next->canonicalModels.size();++target){
                    d.stage="canonical_model_references";
                    d.sourceMember=ta_original_members50la_fixture::targets[target].modelMember;
                    auto model=std::make_unique<CanonicalModel>();
                    model->worldInstance=ta_original_members50la_fixture::targets[target].worldInstance;
                    model->resource=sources.model(target);
                    need(model->resource&&model->materials.acquire26E640(*model->resource,
                        next->canonicalDomain->materials382B38(),request.canonicalMaterialPath,diagnostic),
                        "Track actual model26E640 material references rejected");
                    need(model->materials.complete()&&model->materials.count1C0()&&
                        *model->materials.count1C0()==1,"Track whole-model material count rejected");
                    next->canonicalModels[target]=std::move(model);
                }
                populateCanonicalMaterials(*next,request,d);
            }
            if (gc) next->projectionUvs.reserve(mesh.vertices.size());
            std::map<std::string, SourceScene> scenes;
            std::map<std::pair<std::string, std::uint32_t>, std::uint32_t> textureIndexes;
            std::map<std::uint32_t, std::pair<bool,bool>> samplerPolicies;
            // One callback invocation per original unresolved resource, even
            // when many scene materials use that source. Failed decodes retain
            // their precise reason and never synthesize a bank texture.
            struct DecodedSource { std::uint32_t texture = unresolved, mip = unresolved; std::string why; };
            std::map<std::string, DecodedSource> decodedSources;
            std::vector<Bytes> bytes;
            std::vector<a::MaterialTextureSlotInput50EA> slots;
            std::size_t rgbaBytes = 0, textureSourceBytes = 0;
            std::array<std::uint32_t,2> canonicalSubmittedSpans{};
            std::uint32_t nextVertex = 0, nextIndex = 0;
            for (std::uint32_t instanceIndex = 0; instanceIndex < instances.size(); ++instanceIndex) {
                d.instance = instanceIndex;
                d.surface=d.material=unresolved;d.stage="instance_selection";
                d.sourceMember.clear();d.canonical={};
                const auto& instance = instances.at(instanceIndex);
                const auto& selectedInstance = selected.at("visual_instances").at(instanceIndex);
                for (auto it = selectedInstance.begin(); it != selectedInstance.end(); ++it)
                    need(instance.at(it.key()) == it.value(), "Track exporter differs from selected source placement");
                const std::string key = instance.at("key");
                auto found = scenes.find(key);
                if (found == scenes.end()) {
                    d.stage="source_scene";
                    const auto rowFound = sceneRows.find(key);
                    need(rowFound != sceneRows.end(), "Track instance scene is unselected");
                    const auto& row = *rowFound->second;
                    d.sourceMember=row.at("member").get<std::string>();
                    const auto& record = inputs.record(key);
                    need(row.at("status") == "converted_geometry" && record == row.at("native") &&
                        record.at("source_sha256") == row.at("source_sha256") &&
                        row.at("rrscene") == "cacheobjects/" + key + ".rrscene", "Track scene provenance");
                    const auto sourceBytes = inputs.selected(row.at("rrscene"), a::max_scene_bytes48d);
                    need(record.at("scene_sha256") == sha256(sourceBytes), "Track scene record hash mismatch");
                    SourceScene source; source.request = &row; std::string why;
                    if(canonicalWorld)source.canonicalTarget=ta_original_members50la_fixture::targetIndex(row.at("member"));
                    const bool parsed=a::parseSceneAsset48D(sourceBytes,source.scene,why);
                    need(parsed,"Track source scene parse: "+why);
                    const auto& scene = source.scene;
                    need(scene.nodes.size() == 1 && !scene.identity_matrix_palette &&
                        record.at("materials") == Json(scene.materials), "Track single-root source scene/material subset");
                    const auto assetIdentity = static_cast<std::uint64_t>(scenes.size()) + 1;
                    for (std::uint32_t mi = 0; mi < scene.materials.size(); ++mi) {
                        d.stage="retained_material";
                        d.canonical={};
                        d.material = mi;
                        source.materialSelections.emplace_back();
                        const auto material = materials.find(scene.materials[mi]);
                        need(material != materials.end(), "Track exact scene material association absent");
                        const auto& mr = *material->second;
                        need(mr.at("status") == "resolved", "Track material unresolved");
                        const auto materialKey = mr.at("key").get<std::string>();
                        const auto& materialRecord = inputs.record(materialKey);
                        need(materialRecord.at("type") == "material" &&
                            materialRecord.at("converter") == "rr-native-material-48E-v1" &&
                            materialRecord.at("source_sha256") == mr.at("selected").at("source_sha256"),
                            "Track material source record mismatch");
                        bool requester = false;
                        for (const auto& r : mr.at("requested_by"))
                            if (r.at("scene_key") == key && r.at("scene_member") == row.at("member")) requester = true;
                        need(requester, "Track scene did not request this material");
                        const auto& passes = materialRecord.at("passes");
                        need(!passes.empty() && passes.size() <= (gc ? 2u : 1u),
                            "Material is outside authenticated one/two-pass corpus");
                        for (std::uint32_t pi = 0; pi < passes.size(); ++pi) {
                        const auto& pass = passes.at(pi);
                        need(pass.at("pass") == pi+1 && pass.at("textures").size() == 1 &&
                            pass.at("textures").at(0).at("slot") == 1,
                            "Original material pass/slot order unsupported");
                        need(slots.size() < budget.slotCapacity, "Track material slot budget");
                        Selection out; out.assetIdentity = assetIdentity; out.sceneKey = key;
                        out.materialIndex = mi; out.materialName = scene.materials[mi];
                        out.materialKey = materialKey; out.track = request.track;
                        out.pass = pi+1; out.slot = choice.slot; out.passCount = static_cast<std::uint32_t>(passes.size());
                        out.bankSlot = static_cast<std::uint32_t>(slots.size());
                        out.program = pass.at("program").get<std::string>();
                        out.sourcePassFields = pass.at("fields");
                        if (gc) for (const auto& field : out.sourcePassFields.at("authored")) {
                            if (field.at("name") == "wrapModeHorizontal" || field.at("name") == "wrapModeVertical") {
                                const auto wrap = field.at("value").get<std::string>();
                                need(wrap == "0" || wrap == "1", "Unsupported original texture wrap mode");
                                (field.at("name") == "wrapModeHorizontal" ? out.clampU : out.clampV) = wrap == "1";
                            }
                        }
                        out.projectedPrelit = out.program == "ProjectedPrelit";
                        need(!out.projectedPrelit || (gc && passes.size() == 2), "Unsupported projected material scope");
                        need(passes.size() == 1 || out.projectedPrelit, "Every layered pass requires recovered ProjectedPrelit");
                        if (out.projectedPrelit) out.texGenMatrix = projection(out.sourcePassFields);
                        out.textureName = pass.at("textures").at(0).at("name").get<std::string>();
                        const a::OriginalStandardTextureResource50LA* canonicalResource=nullptr;
                        a::OriginalStandardTextureLevelView50LA canonicalLevel;
                        if(source.canonicalTarget>=0){
                            d.stage="canonical_material_pass";
                            const auto target=static_cast<std::size_t>(source.canonicalTarget);
                            const auto& definition=ta_original_members50la_fixture::targets[target];
                            d.sourceMember=definition.materialMember;
                            const auto& model=*next->canonicalModels[target];
                            need(mi==0&&pi==0&&key==definition.sceneKey&&scene.materials.size()==1,
                                "Track canonical whole-model selection mismatch");
                            const auto* row1C4=model.materials.handle1C4(mi);
                            auto& diagnostic=d.canonical;
                            const a::OriginalStandardMaterialObject50LA* material=nullptr;
                            need(row1C4&&model.materials.table382B38()==next->canonicalDomain->materials382B38().get()&&
                                next->canonicalDomain->materials382B38()->resolve298E60(*row1C4,material,diagnostic)&&
                                material&&material->constructed29B550()&&material->passCount48()&&
                                *material->passCount48()==1&&material->source()&&material->source()->member()&&
                                *material->source()->member()==definition.materialMember&&material->source()->originalBytes()&&
                                sha256(*material->source()->originalBytes())==definition.materialSha,
                                "Track model1C4 canonical whole PSM publication missing");
                            a::OriginalStandardMaterialPassView50LA actualPass;
                            need(material->pass4C(pi,actualPass)&&actualPass.references04&&*actualPass.references04>0&&
                                actualPass.program18&&actualPass.mapCount20&&*actualPass.mapCount20==1&&
                                actualPass.texture1A&&actualPass.uv1E&&(*actualPass.uv1E)[0]==0&&
                                actualPass.flags21&&(*actualPass.flags21&2u)&&
                                material->textures383400()==next->canonicalDomain->textures383400().get(),
                                "Track canonical material pass/map ownership rejected");
                            std::int16_t retainedProgram=-1;
                            const bool programResolved=next->canonicalDomain->programs382B70()->find298C80(
                                out.program,retainedProgram,diagnostic.program);
                            need(programResolved&&retainedProgram==*actualPass.program18&&out.program=="PreLitCompressed",
                                "Track native retained rendering policy differs from canonical program");
                            // Source row39 spells this "PrelitCompressed";
                            // retained authored metadata spells "PreLitCompressed".
                            // The actual source lookup is case-insensitive, but
                            // retain the authenticated spelling for exact
                            // requester provenance and existing renderer policy.
                            d.stage="canonical_texture_resource";d.sourceMember=definition.textureMember;
                            a::OriginalStandardTextureRowView50LA textureRow;
                            const auto token=(*actualPass.texture1A)[0];
                            need(next->canonicalDomain->textures383400()->row(token,textureRow)&&
                                textureRow.name00&&textureRow.references08&&*textureRow.references08>0&&textureRow.object04&&
                                *textureRow.name00==std::string("textures\\")+out.textureName,
                                "Track canonical pass texture token is unresolved");
                            out.textureName=textureRow.name00->substr(textureRow.name00->find_last_of('\\')+1);
                            canonicalResource=textureRow.object04;
                            need(canonicalResource->constructed24DA30()&&canonicalResource->sourceMember()&&
                                *canonicalResource->sourceMember()==definition.textureMember&&canonicalResource->originalBytes()&&
                                sha256(*canonicalResource->originalBytes())==definition.textureSha&&
                                canonicalResource->sourceKind()&&*canonicalResource->sourceKind()==1&&
                                canonicalResource->formatAC()&&*canonicalResource->formatAC()==8&&
                                canonicalResource->bitsPerPixelE8()&&*canonicalResource->bitsPerPixelE8()==8&&
                                canonicalResource->palette208()&&!canonicalResource->palette204()&&
                                canonicalResource->levelCountB0()&&*canonicalResource->levelCountB0()==5&&
                                canonicalResource->uploadConversion382FC8()&&*canonicalResource->uploadConversion382FC8()==1&&
                                canonicalResource->level100(0,canonicalLevel)&&canonicalLevel.sourceMipOrdinal==4&&
                                canonicalLevel.width104==256&&canonicalLevel.height110==256&&
                                canonicalLevel.linearSourceIndices&&canonicalLevel.linearSourceBytes==256u*256u&&
                                (canonicalLevel.flags30KnownMask&4u)&&(canonicalLevel.flags30&4u),
                                "Track canonical original PST/native selected level rejected");
                            out.canonicalMaterialRow=*row1C4;out.canonicalTextureRow=token;
                        }
                        const auto texture = textureRows.find(out.textureName);
                        d.stage="retained_texture";
                        need(texture != textureRows.end(), "Track exact material texture association absent");
                        const auto& tr = *texture->second; requester = false;
                        for (const auto& r : tr.at("requested_by"))
                            if (r.at("material_key") == out.materialKey && r.at("material_name") == out.materialName &&
                                r.at("pass") == out.pass && r.at("slot") == out.slot && r.at("program") == out.program) requester = true;
                        need(requester, "Track material did not request this texture slot");
                        if (tr.at("status") != "resolved") {
                            out.unresolvedReason = tr.at("reason").get<std::string>();
                            const Json* source = nullptr;
                            if (request.decodeSourceTexture && out.unresolvedReason == "original_resource_unsupported") {
                                // The retained resolver's exact local PST is
                                // still authoritative; no global substitute.
                                const auto& candidates = tr.at("candidates");
                                if (candidates.size() == 1 && candidates.at(0).at("container") == profile.container)
                                    source = &candidates.at(0);
                            } else if (request.decodeSourceTexture && request.allowStaticAnimationFrame &&
                                out.unresolvedReason == "no_exact_member") {
                                // Only the independently retained original animated
                                // resources have an admitted source-frame route.
                                const auto textureName = normalized(out.textureName);
                                const bool supportedAtm = ma ?
                                    (textureName == "MA_CROWD" || textureName == "FLAG_A" ||
                                     textureName == "FLAG_ANI" || textureName == "FLAG_ARK" ||
                                     textureName == "FLAG_FUD" || textureName == "FLAG_SUB" ||
                                     textureName == "FLAG_BCD" || textureName == "FLAG_EFG" ||
                                     textureName == "FHOLOBEAM") : sa ?
                                    (textureName == "ARROWRED" || textureName == "MA_CROWD" ||
                                     textureName == "FHOLOBEAM") : gc ?
                                    (out.textureName == "arrowred" || out.textureName == "s_flaga" ||
                                     out.textureName == "s_flagb" || out.textureName == "s_flagc") :
                                    (out.textureName == "arrowred" || out.textureName == "crowd");
                                const std::string member = supportedAtm ? "textures/"+out.textureName+".atm" : "";
                                if (!member.empty()) for (const auto& candidate : tr.at("non_substitutable_same_stem"))
                                    if (candidate.at("container") == profile.container && normalized(candidate.at("member")) == normalized(member)) {
                                        need(!source, "Ambiguous TA animated source"); source = &candidate;
                                    }
                            }
                            if (source) {
                                out.decodedSourceMember = source->at("member").get<std::string>();
                                out.decodedSourceSha256 = source->at("source_sha256").get<std::string>();
                                out.staticAnimationFrame = out.unresolvedReason == "no_exact_member";
                                const auto sourceKey = normalized(out.decodedSourceMember);
                                auto decoded = decodedSources.find(sourceKey);
                                if (decoded == decodedSources.end()) {
                                    Bytes original; a::DecodeError48C error;
                                    need(a::extractResMember48C(archive, out.decodedSourceMember, original, error, 1024u * 1024u),
                                        "Track exact original texture extraction: " + error.code);
                                    need(original.size() == number(*source, "source_bytes", 1024u * 1024u) &&
                                        sha256(original) == out.decodedSourceSha256, "Track original texture member hash/extent mismatch");
                                    DecodedSource result; Bytes rrtex;
                                    if (request.decodeSourceTexture(out.decodedSourceMember.c_str(), original, rrtex, result.mip, result.why)) {
                                        need(result.mip <= 6 && rrtex.size() >= 16 && std::memcmp(rrtex.data(), "RRTEX001", 8) == 0,
                                            "Track source decoder output framing/mip");
                                        const auto width = u32(rrtex.data() + 8), height = u32(rrtex.data() + 12);
                                        need(width && height && width <= budget.dimensionCapacity && height <= budget.dimensionCapacity,
                                            "Track decoded source texture dimension budget");
                                        const auto rgba = std::size_t(width) * height * 4;
                                        need(rrtex.size() == rgba + 16 && bytes.size() < budget.textureCapacity,
                                            "Track decoded source texture extent/count budget");
                                        add(rgbaBytes, rgba, budget.rgbaByteCapacity, "Track decoded source RGBA budget");
                                        add(textureSourceBytes, rrtex.size(), budget.sourceByteCapacity, "Track decoded source RRTEX budget");
                                        TextureSelection meta; meta.key = out.decodedSourceSha256; meta.mipIndex = result.mip;
                                        meta.bankIndex = static_cast<std::uint32_t>(bytes.size()); meta.identity = std::uint64_t(meta.bankIndex) + 1;
                                        meta.width = width; meta.height = height;
                                        meta.path = request.sourceArchive.generic_string() + "::" + out.decodedSourceMember;
                                        meta.sha256 = sha256(rrtex); result.texture = meta.bankIndex; result.why.clear();
                                        next->textures.push_back(std::move(meta)); bytes.push_back(std::move(rrtex));
                                    } else {
                                        need(!result.why.empty(), "Track source decoder failure requires an explicit reason");
                                        result.why = "source_decoder_unsupported: " + result.why;
                                    }
                                    decoded = decodedSources.emplace(sourceKey, std::move(result)).first;
                                }
                                out.textureIndex = decoded->second.texture; out.mipIndex = decoded->second.mip;
                                out.unresolvedReason = decoded->second.why;
                                if (out.textureIndex != unresolved) out.textureKey = out.decodedSourceSha256;
                            }
                        }
                        else {
                            out.textureKey = tr.at("key").get<std::string>();
                            const auto& textureRecord = inputs.record(out.textureKey);
                            need(textureRecord == tr.at("native") && textureRecord.at("type") == "texture" &&
                                (textureRecord.at("converter") == "rr-native-texture-48E-v1" ||
                                 ((ga || ma || sa) && textureRecord.at("converter") == "rr-original-texture-50FC-v1")) &&
                                textureRecord.at("source_sha256") == tr.at("selected").at("source_sha256"),
                                "Track texture source record mismatch");
                            const auto& mips = textureRecord.at("mips");
                            need(!mips.empty() && mips.size() <= 7, "Track retained mip extent");
                            const Json* picked = nullptr;
                            std::uint32_t mipIndex = 0, lastW = 0, lastH = 0;
                            for (const auto& mip : mips) {
                                const auto index = number(mip, "index", 6);
                                const auto w = number(mip, "width", 1024), h = number(mip, "height", 1024);
                                need(index == mipIndex++ && w >= 16 && h >= 16 &&
                                    (!index || (w == lastW * 2 && h == lastH * 2)) && hash(mip.at("sha256")),
                                    "Track mip source order/dimensions");
                                lastW = static_cast<std::uint32_t>(w); lastH = static_cast<std::uint32_t>(h);
                                const auto path = "cacheobjects/" + out.textureKey + "-mip" + std::to_string(index) + ".rrtex";
                                const auto object = inputs.objects.find(path);
                                if (object != inputs.objects.end() && object->second.at("extracted") == true &&
                                    (canonicalResource?index==canonicalLevel.sourceMipOrdinal:
                                        (choice.mipMode == MipMode::largestRetained || index == choice.mipIndex))) picked = &mip;
                            }
                            // The canonical resource's real largest native
                            // level selects its source mip. Retained RGBA is a
                            // lowering of THAT original PST; no independent
                            // metadata selector or transfer-bytes-as-RGBA path.
                            if(canonicalResource)need(picked&&mips.size()==5&&
                                tr.at("selected").at("source_sha256")==sha256(*canonicalResource->originalBytes())&&
                                tr.at("selected").at("member")==*canonicalResource->sourceMember()&&
                                number(*picked,"width",1024)==static_cast<std::uint32_t>(canonicalLevel.width104)&&
                                number(*picked,"height",1024)==static_cast<std::uint32_t>(canonicalLevel.height110),
                                "Track canonical selected PST/mip has no matching retained RGBA");
                            if (!picked) out.unresolvedReason = "requested_mip_not_retained";
                            else {
                                out.mipIndex = static_cast<std::uint32_t>(number(*picked, "index", 6));
                                const auto textureKey = std::make_pair(out.textureKey, out.mipIndex);
                                const auto existing = textureIndexes.find(textureKey);
                                if (existing != textureIndexes.end()) out.textureIndex = existing->second;
                                else {
                                    need(bytes.size() < budget.textureCapacity, "Track texture count budget");
                                    TextureSelection meta; meta.key = out.textureKey; meta.mipIndex = out.mipIndex;
                                    meta.bankIndex = static_cast<std::uint32_t>(bytes.size()); meta.identity = std::uint64_t(meta.bankIndex) + 1;
                                    meta.width = static_cast<std::uint32_t>(number(*picked, "width", 1024));
                                    meta.height = static_cast<std::uint32_t>(number(*picked, "height", 1024));
                                    need(meta.width <= budget.dimensionCapacity && meta.height <= budget.dimensionCapacity,
                                        "Track texture dimension budget");
                                    const auto rgba = std::size_t(meta.width) * meta.height * 4;
                                    add(rgbaBytes, rgba, budget.rgbaByteCapacity, "Track RGBA byte budget");
                                    add(textureSourceBytes, rgba + 16, budget.sourceByteCapacity, "Track texture source byte budget");
                                    meta.path = "cacheobjects/" + out.textureKey + "-mip" + std::to_string(out.mipIndex) + ".rrtex";
                                    meta.sha256 = picked->at("sha256").get<std::string>();
                                    auto retained = inputs.selected(meta.path, rgba + 16);
                                    need(retained.size() == rgba + 16 && std::memcmp(retained.data(), "RRTEX001", 8) == 0 &&
                                        u32(retained.data() + 8) == meta.width && u32(retained.data() + 12) == meta.height &&
                                        sha256(retained) == meta.sha256, "Track actual texture framing/hash");
                                    out.textureIndex = meta.bankIndex; textureIndexes.emplace(textureKey, meta.bankIndex);
                                    next->textures.push_back(std::move(meta)); bytes.push_back(std::move(retained));
                                }
                                if(canonicalResource){
                                    const auto& retained=next->textures.at(out.textureIndex);
                                    need(retained.mipIndex==canonicalLevel.sourceMipOrdinal&&
                                        retained.width==static_cast<std::uint32_t>(canonicalLevel.width104)&&
                                        retained.height==static_cast<std::uint32_t>(canonicalLevel.height110)&&
                                        retained.sha256==picked->at("sha256").get<std::string>(),
                                        "Track shared bank entry differs from canonical texture level");
                                }
                            }
                        }
                        need(out.textureIndex != unresolved || (!canonicalResource&&
                            choice.requirement == a::MaterialTextureRequirement50EA::optionalUntextured),
                            "Required track texture unresolved: " + out.textureName + ": " + out.unresolvedReason);
                        if (gc && out.textureIndex != unresolved) {
                            const auto policy = std::make_pair(out.clampU, out.clampV);
                            const auto retained = samplerPolicies.emplace(out.textureIndex, policy);
                            need(retained.second || retained.first->second == policy,
                                "Shared original texture requires distinct sampler ownership");
                        }
                        const auto textureIdentity = out.textureIndex == unresolved ?
                            ((std::uint64_t(1) << 63) | out.bankSlot) : std::uint64_t(out.textureIndex) + 1;
                        slots.push_back({{out.assetIdentity, mi, out.pass, out.slot}, textureIdentity, choice.requirement});
                        source.materialSelections.back().push_back(static_cast<std::uint32_t>(next->selections.size()));
                        next->selections.push_back(std::move(out));
                        } // Every authored pass, in source order.
                    }
                    found = scenes.emplace(key, std::move(source)).first;
                }
                const auto& source = found->second;
                d.stage="mesh_span";
                d.sourceMember=source.request->at("member").get<std::string>();
                const auto& scene = source.scene;
                if(source.canonicalTarget>=0){
                    const auto& model=*next->canonicalModels[static_cast<std::size_t>(source.canonicalTarget)];
                    need(number(instance,"world_instance_index",profile.maxWorldInstance)==model.worldInstance&&
                        model.resource->scene()&&scene.materials==model.resource->scene()->materials&&
                        scene.lods.size()==1&&scene.lods[0].surfaces.size()==1&&scene.spans_lod0.size()==1,
                        "Track canonical instance whole-model closure mismatch");
                }
                need(instance.at("container") == profile.container &&
                    normalized(instance.at("geometry_member")) == normalized(source.request->at("member")),
                    "Track instance geometry/source mismatch");
                const auto vb = static_cast<std::uint32_t>(number(instance, "vertex_start", a::max_mesh_vertices));
                const auto ib = static_cast<std::uint32_t>(number(instance, "index_start", a::max_mesh_indices));
                need(vb == nextVertex && ib == nextIndex && instance.at("surfaces").size() == scene.spans_lod0.size(),
                    "Track source instance offset/surface order mismatch");
                auto matrix = instance.at("matrix").get<std::array<double, 16>>();
                if (gc && scene.world_space) matrix = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
                if (gc) need(instance.at("visual_matrix") == Json(matrix) &&
                    instance.at("world_space_root_bypassed") == scene.world_space,
                    "Original world-space visual root policy mismatch");
                for (auto v : matrix) need(std::isfinite(v), "Track source placement nonfinite");
                for (std::uint32_t si = 0; si < scene.spans_lod0.size(); ++si) {
                    d.surface = si;
                    const auto& span = scene.spans_lod0[si]; const auto& surface = scene.lods.at(0).surfaces.at(si);
                    Span out; out.indexStart = ib + span.index_start; out.indexCount = span.index_count;
                    out.vertexStart = vb + span.vertex_start; out.vertexCount = span.vertex_count;
                    const auto& materialPasses = source.materialSelections.at(span.material_index);
                    need(!materialPasses.empty(), "Source material has no selected pass");
                    out.worldInstanceIndex = static_cast<std::uint32_t>(number(instance, "world_instance_index", profile.maxWorldInstance));
                    out.surfaceIndex = si; out.hasOriginalColors = span.has_original_colors;
                    need(out.vertexStart == nextVertex && out.indexStart == nextIndex && out.vertexCount && out.indexCount &&
                        out.indexCount % 3 == 0 && out.vertexCount <= mesh.vertices.size() - std::min<std::size_t>(nextVertex, mesh.vertices.size()) &&
                        out.indexCount <= mesh.indices.size() - std::min<std::size_t>(nextIndex, mesh.indices.size()),
                        "Track source span order/extent mismatch");
                    const auto& recorded = instance.at("surfaces").at(si);
                    need(recorded.at("material") == scene.materials.at(span.material_index) &&
                        recorded.at("material_index") == span.material_index && recorded.at("vertex_start") == out.vertexStart &&
                        recorded.at("index_start") == out.indexStart && recorded.at("vertices") == out.vertexCount &&
                        recorded.at("indices") == out.indexCount && recorded.at("has_original_colors") == out.hasOriginalColors,
                        "Track exporter source material/span mismatch");
                    if (gc) need(next->projectionUvs.size() == out.vertexStart, "Source projection vertex order");
                    for (std::size_t vi = 0; vi < surface.vertices.size(); ++vi) {
                        const auto& authored = surface.vertices[vi]; const auto& live = mesh.vertices[out.vertexStart + vi];
                        need(authored.bone == 0, "Track source vertex bone unsupported");
                        if (gc) {
                            a::TrackProjectedUv uv{{static_cast<float>(authored.uv[0]), static_cast<float>(authored.uv[1]),
                                static_cast<float>(authored.uv[0]), static_cast<float>(authored.uv[1])}};
                            for (const auto selectedPass : materialPasses) {
                                const auto& selection = next->selections.at(selectedPass);
                                if (!selection.projectedPrelit) continue;
                                for (unsigned axis = 0; axis < 2; ++axis) {
                                    double value = 0;
                                    for (unsigned k = 0; k < 3; ++k)
                                        value += (authored.position_local[k] - scene.quantization[4+k]) * selection.texGenMatrix[k*4+axis];
                                    value += selection.texGenMatrix[12+axis];
                                    need(std::isfinite(value) && std::isfinite(static_cast<float>(value)),
                                        "Original projected texture coordinate nonfinite");
                                    uv.uv[(selection.pass-1)*2+axis] = static_cast<float>(value);
                                }
                            }
                            next->projectionUvs.push_back(uv);
                        }
                        for (unsigned c = 0; c < 2; ++c)
                            need(sameFloat(live.uv[c], static_cast<float>(authored.uv[c])), "Track source UV mismatch");
                        for (unsigned c = 0; c < 4; ++c)
                            need(live.color[c] == (surface.has_original_colors ? authored.color[c] : 255), "Track source color mismatch");
                        for (unsigned c = 0; c < 3; ++c) {
                            double position = 0;
                            for (unsigned q = 0; q < 3; ++q) position += authored.position_local[q] * matrix[q * 4 + c];
                            position += matrix[12 + c];
                            need(std::isfinite(position) && sameFloat(live.position[c], static_cast<float>(position)),
                                "Track source position/placement mismatch");
                        }
                    }
                    for (std::size_t ii = 0; ii < surface.indices.size(); ++ii)
                        need(mesh.indices[out.indexStart + ii] == out.vertexStart + surface.indices[ii], "Track source index order mismatch");
                    for (const auto selectedPass : materialPasses) {
                        const auto& selection = next->selections.at(selectedPass);
                        if(source.canonicalTarget>=0){
                            const auto target=static_cast<std::size_t>(source.canonicalTarget);
                            const auto* actual1C4=next->canonicalModels[target]->materials.handle1C4(si);
                            need(actual1C4&&selection.canonicalMaterialRow==*actual1C4&&
                                selection.canonicalTextureRow>=0&&selection.textureIndex!=unresolved&&
                                si==0&&materialPasses.size()==1,"Track submitted span lost its actual model1C4 slot");
                            ++canonicalSubmittedSpans[target];
                        }
                        out.materialSelection = selectedPass; out.textureIndex = selection.textureIndex;
                        out.sourcePass = selection.pass; out.sourcePassCount = selection.passCount;
                        out.projectionPass = selection.projectedPrelit ? selection.pass : 0;
                        next->spans.push_back(out);
                    }
                    nextVertex += out.vertexCount; nextIndex += out.indexCount;
                }
            }
            need(next->spans.size() == profile.drawSpans && (!gc || next->projectionUvs.size() == mesh.vertices.size()) && nextVertex == mesh.vertices.size() && nextIndex == mesh.indices.size(),
                "Track full source span coverage mismatch");
            if(canonicalWorld)for(const auto count:canonicalSubmittedSpans)
                need(count==1,"Track canonical whole-model submitted span missing or duplicated");
            std::vector<a::MaterialTextureAssetInput50EA> textureInput; textureInput.reserve(bytes.size());
            for (std::uint32_t i = 0; i < bytes.size(); ++i) textureInput.push_back({std::uint64_t(i) + 1, &bytes[i]});
            const a::MaterialTextureAssetsInput50EA bankInput{textureInput.data(), static_cast<std::uint32_t>(textureInput.size()),
                slots.data(), static_cast<std::uint32_t>(slots.size()), budget};
            d.stage="bank_publication";d.sourceMember.clear();
            need(next->bank.load(bankInput, d.bank), "Track real texture bank rejected");
            next->coldBytes = inputs.sourceBytes; storage_ = std::move(next);
            d.instance = d.surface = d.material = unresolved; d.stage="complete";return true;
        } catch (const std::exception& e) { try { d.message = e.what(); } catch (...) {} return false; }
        catch (...) { try { d.message = "Unexpected track cold material failure"; } catch (...) {} return false; }
    }
};
} // namespace track_materials50fa_fixture
