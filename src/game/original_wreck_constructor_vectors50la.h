#pragma once
#include <array>
namespace rr::game {
struct OriginalWreckConstructorVectors50LA {
    std::array<float,4> position434830{},look434860{},up434850{},zero433B10{};
};
// Actual file-backed static initializers, not reconstructed from zeroed BSS:
//3CA020..3CA09C builds four stack vectors and SQ stores434830/40/50/60;
//3C9210..3C928C independently builds/stores the zero sentinel433B10.
// All nonzero words are explicitly raw3F800000 and all zeros are SW zero.
// Return by value so the application owns this initialized source projection.
inline OriginalWreckConstructorVectors50LA initializedOriginalWreckAssetVectors50LA()noexcept {
    OriginalWreckConstructorVectors50LA out;
    out.position434830={{0.f,0.f,0.f,1.f}};
    out.look434860={{0.f,0.f,1.f,1.f}};
    out.up434850={{0.f,1.f,0.f,1.f}};
    out.zero433B10={{0.f,0.f,0.f,1.f}};
    return out;
}
}
