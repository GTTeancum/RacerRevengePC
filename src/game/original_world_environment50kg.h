#pragma once
#include "../assets/import_world48f.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace rr::game::original_world_environment50KG {
enum class Status : std::uint8_t {
    complete, unprepared, alreadyPrepared, sourceShape, missingEnvironment,
    duplicateField, integerSyntax, storageLimit, sourceRange, cameraZone,
    allocation
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
// Native retained sky-selection portion of world3A4 EnvironmentInfo, from
// constructor276950/read276C00. This does not impersonate its other fields or
// construct world3F8 sky nodes/resources, terrain, water, or an Ecosystem.
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
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
} // namespace rr::game::original_world_environment50KG
