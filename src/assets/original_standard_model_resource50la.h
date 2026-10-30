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
    // Private authenticated RA path: input may be consumed on a later decode failure.
    bool prepareOriginalLimits(const std::string&,std::vector<std::uint8_t>&&,
        const SceneLimits48D&,OriginalStandardResourceDiagnostic50LA&) noexcept;
    template<class Bytes>
    bool prepareOriginalLimitsImpl(const std::string&,Bytes&&,
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
// Retained native identity for one actually loaded2461E0/246300 Surface.
// These are source scalar operands, not a guest ABI/DMA packet.26E170 owns a
// distinct contiguous array per LOD (source stride36); native sizeof differs.
struct OriginalStandardSurface246300_50LA final {
    const std::int32_t* material00=nullptr; // Same actual mutable geometry slot.
    std::uint8_t attributes0C=0,uvCount0D=0,palette0E=0xff;
    std::array<float,2> uvScale18{{1.f,1.f}}; //246218/21C; source reads0D entries.
    std::uint16_t vertexCount20=0,stripCount22=0;
    std::uint32_t lodOrdinal=0,surfaceOrdinal=0;
    std::size_t sourceOffset=0;
};

class OriginalSourcePsgFile50LA;
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
    // Only present after the genuine resource constructor publishes. Immutable
    // identity survives warm instances and is retired with the actual array.
    const OriginalStandardSurface246300_50LA* surface246300(std::size_t lod,std::size_t surface)const noexcept;
    std::size_t surfaceCount26E2B8(std::size_t lod)const noexcept;
    bool retain104960(const rr::game::OriginalRacerModelSceneOwner50LA*,
        OriginalStandardResourceDiagnostic50LA&) noexcept;
    bool holdsModelReference(const rr::game::OriginalRacerModelSceneOwner50LA*) const noexcept;
    bool canReleaseForModel(const rr::game::OriginalRacerModelSceneOwner50LA*) const noexcept;
    //After this model's actual26E4C0 material release: when last, reset every
    //surface00 to-1, execute resource104970/240150/26E0A0, then1327D0.
    bool release26E598(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const OriginalStandardMaterialReferences50LA&,OriginalStandardResourceDiagnostic50LA&) noexcept;
private:
    friend class rr::game::OriginalRacerModelSceneOwner50LA;
    friend class OriginalStandardTextureResolver50LA;
    friend class OriginalSourcePsgFile50LA;
    bool publishSourcePrototype26EE74(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const OriginalSourcePsgFile50LA&,OriginalStandardResourceDiagnostic50LA&)noexcept;
    bool sourceWarmLedgerCurrent()const noexcept;
    bool registerSourceWarmBorrow(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const std::shared_ptr<OriginalSourcePsgFile50LA>&,OriginalStandardResourceDiagnostic50LA&)noexcept;
    bool retainSourceWarm104960(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const OriginalSourcePsgFile50LA*,OriginalStandardResourceDiagnostic50LA&)noexcept;
    bool sourceWarmBorrowCurrent(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const OriginalSourcePsgFile50LA*,bool retained)const noexcept;
    bool withdrawSourceWarmBorrowNative(const rr::game::OriginalRacerModelSceneOwner50LA*,
        OriginalSourcePsgFile50LA*)noexcept;
    bool sourcePrototypeIdentityCurrent(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const OriginalSourcePsgFile50LA*)const noexcept;
    bool sourcePrototypeCurrent(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const OriginalSourcePsgFile50LA*)const noexcept;
    bool cachePublicationReady1326A0()const noexcept;
    bool loadSourceOrdinary26E170(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const std::vector<std::uint8_t>&,std::size_t&,OriginalSourcePsgFile50LA&,OriginalStandardResourceDiagnostic50LA&)noexcept;
    bool sourceLoadCurrent()const noexcept;
    bool loadSourcePalette240210(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const std::vector<std::uint8_t>&,std::size_t&,OriginalSourcePsgFile50LA&,OriginalStandardResourceDiagnostic50LA&)noexcept;
    bool sourcePaletteCurrent()const noexcept;
    bool constructSource240110(std::shared_ptr<OriginalStandardTextureResolver50LA>,OriginalStandardModelFileId50LA,
        const rr::game::OriginalRacerModelSceneOwner50LA*,OriginalStandardResourceDiagnostic50LA&)noexcept;
    bool sourceConstructorCurrent(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const OriginalStandardTextureResolver50LA*,OriginalStandardModelFileId50LA)const noexcept;
    bool cancelSourceConstructorNative(const rr::game::OriginalRacerModelSceneOwner50LA*,
        const OriginalStandardTextureResolver50LA*,OriginalStandardModelFileId50LA)noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
