#pragma once
#include "../../../src/assets/native_asset48b.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <vector>
namespace rr::platform {
class OriginalUiSharedDialogs50KG;
class OriginalUiGarageScreenText50KG;
class OriginalUiGarageQuad50KG;
class OriginalUiMainDraw50KG;
class OriginalUiOptionsDrawContribution50KG;
class OriginalUiSourceOwner50KM;
// Numeric lowered output only. It grants no source callback, payload lifetime,
// material registration, renderer submission or accepted-history authority.
struct OriginalUiMenuWarningGeometry50KG {
    std::vector<assets::Vertex48B> vertices;
    std::vector<std::array<float,3>> stq;
    std::vector<float> rasterW,fog;
    std::uint32_t quadCount=0;
};
// Retains an actual470 packet in a private Main/Options publication. Distinct
// from NativeCollectorFifo50LA and NEVER used to issue nativeGarageFifoPins_.
// Only Shared may issue/release a receipt in its fixed, address-stable array.
class OriginalUiMenuWarningPacket50KG final {
    friend class OriginalUiSharedDialogs50KG;
    friend class OriginalUiGarageScreenText50KG;
    friend class OriginalUiGarageQuad50KG;
    friend class OriginalUiMainDraw50KG;
    friend class OriginalUiOptionsDrawContribution50KG;
    enum class Kind:std::uint8_t {none,text,quad,dim};
    OriginalUiSharedDialogs50KG* owner_=nullptr;
    const OriginalUiSourceOwner50KM* sourceOwner_=nullptr;
    std::uint64_t invocation_=0,warningEpoch_=0,update_=0,rasterInvocation_=0;
    Kind kind_=Kind::none;
    const void* payload_=nullptr;
    const void* primitive_=nullptr;
    const void* packet_=nullptr;
    std::size_t operation_=0,primitiveIndex_=0;
    std::uint64_t packetRevision_=0;
    std::int32_t materialToken_=-1;
    std::uint32_t sourcePC_=0;
    bool active_=false;
public:
    OriginalUiMenuWarningPacket50KG()noexcept=default;
    ~OriginalUiMenuWarningPacket50KG()noexcept {if(active_)std::terminate();}
    OriginalUiMenuWarningPacket50KG(const OriginalUiMenuWarningPacket50KG&)=delete;
    OriginalUiMenuWarningPacket50KG& operator=(const OriginalUiMenuWarningPacket50KG&)=delete;
};
}
