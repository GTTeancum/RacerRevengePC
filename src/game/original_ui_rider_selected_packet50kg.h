#pragma once
#include "original_ui_rider_backend50kg.h"
#include "original_ui_rider_model_packet50kg.h"
#include "original_ui_light_manager_lifetime50kg.h"
namespace rr::game {
struct OriginalUiRiderSelectedPacketDiagnostic50KG {
    enum class Error:std::uint8_t {none,input,membership,lighting,packet} error=Error::none;
    OriginalUiRiderVisibilityDiagnostic50KG membership;
    OriginalUiLightManagerLifetimeDiagnostic50KG lighting;
    OriginalUiRiderPacketDiagnostic50KG packet;
};
struct OriginalUiRiderSelectedPacket50KG {
    OriginalUiRiderModelSelection50KG selection{};
    OriginalUiRiderModelPacket50KG packet{};
    bool emitsGeometry=false;
};
// Called in actual enabled widget/model traversal order, after prepareFrame.
// A genuine culled member produces no packet; a failed selection is an error.
// The character reads the shared application's actual LightManager history.
// Backend bank, model graph and assets must remain live for this borrowed packet.
bool prepareOriginalUiRiderSelectedPacket50KG(std::uint16_t widget,
    OriginalUiRiderDynamicOwner50KG&,OriginalUiRiderBackend50KG&,
    const OriginalUiRiderRenderContext50KG&,const OriginalUiRiderLighting50KG&,
    OriginalUiLightManagerLifetime50KG&,OriginalUiRiderSelectedPacket50KG&,
    OriginalUiRiderSelectedPacketDiagnostic50KG&) noexcept;
}
