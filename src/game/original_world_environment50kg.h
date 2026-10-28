#pragma once
#include "../assets/import_world48f.h"
#include "../assets/track_height_descriptor50jb.h"
#include <optional>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace rr::game::original_gc_gameplay_lighting_candidate {class Owner;}
namespace rr::game::original_world_environment50KG {
enum class Status : std::uint8_t {
    complete, unprepared, alreadyPrepared, sourceShape, missingEnvironment,
    duplicateField, integerSyntax, storageLimit, sourceRange, cameraZone,
    allocation, numberSyntax, arithmetic
};
struct Diagnostic {
    Status status=Status::unprepared;
    std::uint32_t sourcePC=0;
    std::size_t sourceOffset=0;
    assets::DecodeError48C decode;
};
struct SkySelection {
    // Camera's actual zero-based zone48, not an authored zone label or a
    // caller-chosen replacement. -1 means no enabled cube.
    std::uint32_t cameraZone48=0;
    std::int32_t cubeIndex=-1;
    bool enabled=false;
};
// Retained EnvironmentInfo520..540, in source read order. These are authoring
//fields, not a spatial tree, an admitted zone, or a query-completion token.
struct SpatialBounds {
    float terrainWidth520=0.f,terrainBreadth524=0.f;
    float terrainWidthScale528=0.f,terrainBreadthScale52C=0.f,border530=0.f;
    float xMin534=0.f,xMax538=0.f,zMin53C=0.f,zMax540=0.f;
};
// Native retained authoring portions of world3A4 EnvironmentInfo, from
// constructor276950/read276C00. This does not construct world3F8 sky
//nodes/resources, a spatial tree, terrain, water, or an Ecosystem.
class Owner final {
public:
    Owner() noexcept;
    ~Owner();
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    // One-shot, transactional. Caller must supply its selected, authenticated
    // original SCB World48F. Exact reversible bytes and typed world semantics
    // are checked here; that is consistency, not archive/hash authentication.
    // An immutable deep-copy retains the real source owner, including every
    // original parameter field and original_bytes. No mutable source alias is
    // exposed and subsequent caller edits cannot change a prepared owner.
    bool prepare(const assets::World48F& authenticatedWorld,Diagnostic&) noexcept;
    // Separate optional continuation of the SAME retained Environment block.
    // It preserves the sky-only preparation domain and accepts no replacement
    //bounds. Positive finite extents >=1 form the bounded native domain; source
    //diagnostic side effects and malformed numeric spellings remain unjoined.
    bool prepareSpatialBounds276F3C(Diagnostic&) noexcept;
    const SpatialBounds* spatialBounds534() const noexcept;
    bool prepared() const noexcept;
    const assets::World48F* world() const noexcept;
    std::size_t zoneCount() const noexcept;
    std::uint32_t cubeCount() const noexcept; // Actual effective count+0.
    const std::string* cubeFile(std::uint32_t cubeIndex) const noexcept;
    // Original+8 row, after subtract1. Row remains present after sole 'none'
    // collapses count+0. No rows exist when the initial count is zero.
    const std::int32_t* zoneCubeIndex(std::uint32_t cameraZone48) const noexcept;
    // Projection of275894/2758BC/275914..275964 only. Successful disabled
    // selection is distinct from failure. Output is unchanged on failure.
    bool selectCameraZone48(std::uint32_t actualCameraZone48,
        SkySelection&,Diagnostic&) const noexcept;
private:
    friend class ::rr::game::original_gc_gameplay_lighting_candidate::Owner;
    // Same EnvironmentInfo owner: bounded native metadata adoption at actual
    // water call, never a receipt that earlier276C00 was replayed.
    const assets::TrackHeightString50JB* nativeWaterFile110()noexcept;
    const assets::TrackHeightString50JB* nativeEcosystemFile318()noexcept;
    std::optional<assets::TrackHeightString50JB> nativeEcosystemFile_;
    std::optional<assets::TrackHeightDescriptor50JB> nativeHeightFields_;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
} // namespace rr::game::original_world_environment50KG
