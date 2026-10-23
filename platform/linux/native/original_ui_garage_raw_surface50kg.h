#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace rr::assets {class OriginalStandardModelResource50LA;struct OriginalStandardSurface246300_50LA;}
namespace rr::platform {
class OriginalUiGarageModelResource50KG;
class OriginalUiGarageDrawContribution50KG;
enum class OriginalUiGarageRawStatus50KG:std::uint8_t {complete,phase,source,capacity,allocation};
struct OriginalUiGarageRawDiagnostic50KG {
    OriginalUiGarageRawStatus50KG status=OriginalUiGarageRawStatus50KG::phase;
    std::uint32_t sourcePC=0;
    std::size_t sourceOffset=0;
};
// Immutable original source-operand borrow owned by the genuine Garage
// composer. It neither selects a model nor owns a SourceFrame capability.
class OriginalUiGarageRawSurface50KG final {
public:
    struct Vertex {
        std::array<std::int16_t,3> position{};
        std::array<std::int8_t,3> normal{};
        std::array<std::int16_t,2> uv{};
        std::array<std::uint8_t,4> rgba{};
        std::int16_t paletteAdcWord=0;
    };
    ~OriginalUiGarageRawSurface50KG()=default;
    OriginalUiGarageRawSurface50KG(const OriginalUiGarageRawSurface50KG&)=delete;
    OriginalUiGarageRawSurface50KG& operator=(const OriginalUiGarageRawSurface50KG&)=delete;
private:
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiGarageModelLowering50KG;
    using D=OriginalUiGarageRawDiagnostic50KG;
    OriginalUiGarageRawSurface50KG() noexcept=default;
    bool prepare(std::shared_ptr<const OriginalUiGarageModelResource50KG>,
        std::uint32_t lod,std::uint32_t surface,D&)noexcept;
    bool current()const noexcept;
    struct Metadata {
        std::uint32_t triangleCount=0,materialSlot=0;bool hasStoredColors=false;
        const assets::OriginalStandardSurface246300_50LA* runtimeSurface=nullptr;
    };
    bool describe(Metadata&,D&)const noexcept;
    bool triangle(std::uint32_t ordinal,std::array<Vertex,3>&,D&)const noexcept;
    std::shared_ptr<const OriginalUiGarageModelResource50KG> model_;
    const assets::OriginalStandardModelResource50LA* payload_=nullptr;
    const assets::OriginalStandardSurface246300_50LA* runtimeSurface_=nullptr;
    std::vector<Vertex> vertices_;
    std::vector<std::array<std::uint32_t,3>> triangles_;
    std::uint64_t modelGeneration_=0;
    std::uint32_t lod_=0,surface_=0,materialSlot_=0;
    std::size_t sourceOffset_=0;
    bool hasStoredColors_=false;
};
}
