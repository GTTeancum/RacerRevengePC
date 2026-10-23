#pragma once
#include "original_ui_garage_raw_surface50kg.h"
#include "original_ui_garage_prelit_packet50kg.h"
#include "../../../src/assets/original_ui_material50kg.h"
#include "../../../src/game/original_ui_garage_diffuse50kg.h"
#include "../../../src/game/original_ui_garage_specular50kg.h"
#include "../../../src/game/original_ui_garage_palette_packet50kg.h"
#include <memory>
#include <optional>

namespace rr::game {struct OriginalRacerModelPacket50LA;}
namespace rr::platform {
class OriginalUiGarageDrawContribution50KG;
// One genuine queued Surface's numeric projection. Not a FIFO, material
// transaction, SourceOwner, visibility decision or frame-completion receipt.
// The complete composer retains queue order and its model pins while this
// immutable raw/packet borrow exists. Every triangle failure remains visible.
class OriginalUiGarageModelLowering50KG final {
    friend class OriginalUiGarageDrawContribution50KG;
    using Program=assets::OriginalUiGarageProgram50KG;
    using Numeric=game::garage_prelit50KG::Status;
    struct Context {
        // Actual Camera/renderer rows at the drain's retained publication.
        // Lights in this camera value are NEVER treated as implicit authority.
        game::OriginalUiLitContext50KM camera;
        float guardX334=0,guardY338=0;
        // Only the actual global1499A0 result may populate this. It is needed
        // for lit direct packets without their own two light-matrix slots.
        std::optional<game::original_racer_lighting50KM::LightMatrices> globalLights1499A0;
        std::array<std::uint32_t,2> sourceXyOffset{};
    };
    enum class Status:std::uint8_t {complete,phase,packet,program,numeric,raw};
    struct Diagnostic {
        Status status=Status::phase;
        std::uint32_t sourcePC=0;
        Numeric numeric=Numeric::input;
        OriginalUiGarageRawDiagnostic50KG raw{};
    };
    OriginalUiGarageModelLowering50KG() noexcept=default;
    // packetOrdinal is source Surface0E only in the actual palette fallback;
    // a non-palette direct/fast record has the sole genuine packet0. Pointer
    // equality below rejects a copied packet or a caller-selected replacement.
    bool prepare(std::shared_ptr<const OriginalUiGarageRawSurface50KG>,
        std::uint32_t packetOrdinal,const game::OriginalRacerModelPacket50LA&,
        Program,const Context&,Diagnostic&)noexcept;
    bool current()const noexcept;
    bool projectTriangle(std::uint32_t ordinal,OriginalUiGaragePrelitPacket50KG&,Diagnostic&)const noexcept;
    bool readTriangleCount(std::uint32_t&)const noexcept;
    enum class Kernel:std::uint8_t {none,prelit,diffuse,specular,palette};
    std::shared_ptr<const OriginalUiGarageRawSurface50KG> raw_;
    const game::OriginalRacerModelPacket50LA* packet_=nullptr;
    std::uint64_t matrixRevision_=0;
    std::uint32_t packetOrdinal_=0,triangleCount_=0;
    std::uint16_t sourceFlags20_=0;
    Kernel kernel_=Kernel::none;
    game::garage_prelit50KG::Preamble prelit_{};
    game::garage_diffuse50KG::Preamble diffuse_{};
    game::garage_specular50KG::Preamble specular_{};
    game::garage_palette50KG::Preamble palette_{};
public:
    ~OriginalUiGarageModelLowering50KG()=default;
    OriginalUiGarageModelLowering50KG(const OriginalUiGarageModelLowering50KG&)=delete;
    OriginalUiGarageModelLowering50KG& operator=(const OriginalUiGarageModelLowering50KG&)=delete;
};
}
