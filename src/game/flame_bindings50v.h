#pragma once
#include <cstdint>
#include <cstddef>
namespace rr::game {
constexpr std::uint32_t noFlameRecord50V=0xffffffffu;
struct FlameRecord50V { std::uint32_t words[8]; }; // opaque native record; words0/1 callback tag/owner
struct FlameInstance50V {std::uint32_t modelId,record;};
struct FlamePass50V {std::uint32_t prototype,firstInstance,instanceCount;bool instanceTablePresent;};
struct FlameMaterial50V {std::uint32_t firstPass,passCount;};
struct FlameModel50V {std::uint32_t modelId,firstReference,referenceCount,flameFilter;};
struct FlameViews50V {
 const FlameMaterial50V*materials;std::size_t materialCount;
 const FlamePass50V*passes;std::size_t passCount;
 const FlameInstance50V*instances;std::size_t instanceCount;
 const std::uint32_t*references;std::size_t referenceCount;
};
struct FlameWrite50V {std::uint32_t record,callbackTag,ownerId;};
struct FlameWorkspace50V {FlameRecord50V next[128];FlameWrite50V writes[256];};
enum class FlameStatus50V {Complete,Storage,Limit,InvalidBinding,NeedsColdInstance,UnsupportedDiagnostic};
struct FlameResult50V {FlameStatus50V status;std::uint32_t sourcePC,writes;};
// Prepared native equivalent of26EF10 then26F3F0, model order1BC/1B8/1C4/1C0.
// The caller has resolved actual named flame material IDs and created all needed
// native per-model uniform instances. No guest pointers are dereferenced; tags
//20F110/20EE90 identify future native handlers. No B9C flag is fabricated here.
// Only callback/owner words change, atomically. Workspace may change on failure.
FlameResult50V bindPreparedFlameModels50V(const FlameModel50V(&models)[4],
 std::uint32_t vehicleOwnerId,const FlameViews50V&,FlameRecord50V*records,
 std::size_t recordCount,FlameWorkspace50V&) noexcept;
}
