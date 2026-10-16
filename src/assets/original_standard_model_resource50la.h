#pragma once
#include "import_scene48d.h"
#include "model_geometry49o.h"
#include "original_standard_texture_resource50la.h"
#include <array>
#include <memory>

namespace ra_static_world50LA {class Owner;}
namespace rr::game { class OriginalRacerModelSceneOwner50LA; }
namespace rr::assets {
class OriginalStandardMaterialReferences50LA;
enum class OriginalStandardResourceStatus50LA : std::uint8_t {
    complete,alreadyPrepared,input,hierarchy,geometry,allocation,phase,reference,overflow,cache
};
struct OriginalStandardResourceDiagnostic50LA {
    OriginalStandardResourceStatus50LA status=OriginalStandardResourceStatus50LA::complete;
    std::uint32_t sourcePC=0;
    GeometryDiagnostic49O hierarchy{};
    DecodeError48C geometry{};
    OriginalStandardTextureDiagnostic50LA cache{};
};
// Immutable native payload for the original ordinary PSG3 load route. This
// owns the genuine hierarchy rows and geometry; it is NOT model2403A0, the
// application's resource cache, material handles1C4, or render packets1F0.
// Multiple Standard instances may share this payload while retaining distinct
// canonical model fields, graphs, bounds and observers.
class OriginalStandardModelResource50LA final {
public:
    OriginalStandardModelResource50LA() noexcept;
    ~OriginalStandardModelResource50LA();
    OriginalStandardModelResource50LA(const OriginalStandardModelResource50LA&)=delete;
    OriginalStandardModelResource50LA& operator=(const OriginalStandardModelResource50LA&)=delete;
    // Caller has authenticated the original archive/member and supplies its
    // exact resolved name and decompressed PSG bytes. No file/path resolution
    // or authentication claim is made here. Copies bytes; input is borrowed
    // only for this cold call. Unsupported loaders/assets fail explicitly.
    // Decoders run only when a future native caller invokes this method.
    bool prepare(const std::string& member,const std::vector<std::uint8_t>&,
        OriginalStandardResourceDiagnostic50LA&) noexcept;
    bool prepared() const noexcept;
    const std::string* member() const noexcept;
    const std::vector<std::uint8_t>* originalBytes() const noexcept;
    const Scene48D* scene() const noexcept;
    const PsgNode49O* sourceNodes() const noexcept;
    std::uint32_t nodeCount() const noexcept;
    // Ordinary native resource26E050+14, initialized to1 at26E078/26E07C.
    // This is not quantization, a decoded cutoff or a caller LOD preference.
    const float* sourceLodScale14() const noexcept;
private:
    friend class ::ra_static_world50LA::Owner;
    // Only the authenticated RA source owner may select its pinned signed24
    // decoder limits. Public/default resource preparation remains unchanged.
    bool prepareOriginalLimits(const std::string&,const std::vector<std::uint8_t>&,
        const SceneLimits48D&,OriginalStandardResourceDiagnostic50LA&) noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};

//The actual240110/26E050 CPU resource facet used by model1B4. Its independent
//signed64 source reference is not shared_ptr::use_count and not423F60. Exact
//immutable PSG payload may be shared with native geometry/visual consumers.
//The creator prototype1C is borrowed, never cloned or silently retargeted.
//Serial callers must release warm users before this prototype; the explicit
//release admission prevents leaving a cache hit with a destroyed prototype.
class OriginalStandardModelGeometry50LA final {
public:
    OriginalStandardModelGeometry50LA() noexcept;
    ~OriginalStandardModelGeometry50LA();
    OriginalStandardModelGeometry50LA(const OriginalStandardModelGeometry50LA&)=delete;
    OriginalStandardModelGeometry50LA& operator=(const OriginalStandardModelGeometry50LA&)=delete;
    //Constructs the ordinary identity-palette PSG3 resource and its one fresh
    //source reference. Publication remains the caller's26EE80 operation.
    //No20 geometry is only admitted when an actual source zero count is
    //present; absent bytes are not invented as the source read's result.
    bool construct240110(std::shared_ptr<const OriginalStandardModelResource50LA>,
        std::shared_ptr<OriginalStandardTextureResolver50LA>,OriginalStandardModelFileId50LA,
        const rr::game::OriginalRacerModelSceneOwner50LA*,OriginalStandardResourceDiagnostic50LA&) noexcept;
    bool constructed240110() const noexcept;
    const std::int64_t* references00() const noexcept;
    const float* scale14() const noexcept;
    const rr::game::OriginalRacerModelSceneOwner50LA* prototype1C() const noexcept;
    const OriginalStandardModelResource50LA* payload() const noexcept;
    OriginalStandardModelFileId50LA fileId() const noexcept;
    bool hasPalette20() const noexcept;
    std::size_t paletteCount20() const noexcept;
    const std::vector<std::uint8_t>* palette20(std::size_t) const noexcept;
    const std::int32_t* surfaceMaterial00(std::size_t lod,std::size_t surface) const noexcept;
    bool retain104960(const rr::game::OriginalRacerModelSceneOwner50LA*,
        OriginalStandardResourceDiagnostic50LA&) noexcept;
    bool canReleaseForModel(const rr::game::OriginalRacerModelSceneOwner50LA*) const noexcept;
    //After this model's actual26E4C0 material release: when last, reset every
    //surface00 to-1, execute resource104970/240150/26E0A0, then1327D0.
    bool release26E598(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const OriginalStandardMaterialReferences50LA&,OriginalStandardResourceDiagnostic50LA&) noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
