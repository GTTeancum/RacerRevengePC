#pragma once
#include <cstdint>

namespace rr::game {
// Inputs must eventually come from the actual314B58/314A20 completion for the
// same port/slot request. Plain structs do NOT attest device/target identity.
struct OriginalSaveDeviceObservation50KG {
    OriginalSaveDeviceObservation50KG(std::int32_t observedType,std::int32_t observedFree,
        std::int32_t observedFormatted,std::int32_t observedResult) noexcept
        :type(observedType),freeBlocks(observedFree),formatted(observedFormatted),completionResult(observedResult) {}
    std::int32_t type,freeBlocks,formatted,completionResult;
};
struct OriginalSaveDeviceClassification50KG {
    std::int32_t rawStatus=0;
    bool writesFreeBlocks=false;
    std::int32_t freeBlocks=0;
    bool writesSeenCard=false;
    std::uint32_t seenCard3C8=0;
};
// Exact23AF30 classification only. No RPC, files, fallback observation, state
// mutation, device-presence assertion or provider capability is created here.
// There is no default observation constructor or missing-IO fallback.
OriginalSaveDeviceClassification50KG classifyOriginalSaveDevice23AF3050KG(
    const OriginalSaveDeviceObservation50KG&,std::uint32_t currentSeenCard3C8,
    bool callerRequestedFreeBlocks) noexcept;
}
