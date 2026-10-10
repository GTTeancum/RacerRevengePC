#pragma once
#include "body_advancement49k.h"
#include "device_held48y.h"

namespace rr::game {
// One caller-owned, already-selected device. All initial record words are
// explicit: auxiliary fields have no invented clock/timestamp initialization.
// Hardware polling, device selection, descriptors and configuration are external.
// Stable storage is required because FrameReader49K borrows the member provider.
class ControllerIngress50CX {
public:
    explicit ControllerIngress50CX(const DeviceButtonBank48Y& initialButtons) noexcept;
    ControllerIngress50CX(const ControllerIngress50CX&) = delete;
    ControllerIngress50CX& operator=(const ControllerIngress50CX&) = delete;
    ControllerIngress50CX(ControllerIngress50CX&&) = delete;
    ControllerIngress50CX& operator=(ControllerIngress50CX&&) = delete;

    // Apply exactly one caller-supplied poll using48Y, retaining its complete
    // ordered transition batch. True includes NotReady and ReadFailed; inspect
    // deviceSample().status. NotReady preserves records; ReadFailed clears only
    // active held/value and emits no synthetic releases. Invalid48Y arguments
    // return false and preserve all state. Consume/copy the preceding transition
    // batch before another successful sample replaces it.
    bool samplePacket(bool deviceReady,bool readSucceeded,const DevicePacket48Y&,
        const DeviceButtonDescriptors48Y&,std::uint32_t activeCount,
        std::uint32_t deviceId) noexcept;

    // Only the latest Accepted packet can supply axes. No neutral/stale fallback
    // policy is inferred for other statuses. Call only after a successful
    // samplePacket and at the caller's actual frame-entry point. preparedFactor
    // must already be a Ready49I factor; this merely checks its numeric domain.
    // Codes/context are copied verbatim, without choosing a preset or remapping
    // unsupported codes. The eventual native reader retains its own validation.
    // False preserves out and this provider's context. No shaping/filter/binding
    // runs here.49W derives its canonical player context as before.
    bool prepareFrameReader(DeviceQueryContext48Y,const PlayerReadContext48X&,
        const PlayerActionCodes48X&,float preparedFactor,FrameReader49K& out) noexcept;

    // Directly accepted by appendDeviceTransitions48Z. Append at most once per
    // sample and handle queue failure before replacing this batch. Queue/context
    // rewriting, callbacks and dispatch order remain with their existing owner.
    const DeviceButtonSample48Y& deviceSample() const noexcept {return sample_;}

    // Dispatch48Z must resolve device records to this live bank, not a copied
    // sample. Its proven callbacks may update record auxiliaries. DeviceId and
    // the dispatcher's supported device-slot domain remain explicit caller data.
    DeviceButtonBank48Y& buttons() noexcept {return sample_.buttons;}
    const DeviceButtonBank48Y& buttons() const noexcept {return sample_.buttons;}

    // A prepared reader is a borrowed view, not a snapshot. Consume it while
    // this object lives at this address and before the next sample/prepare call.
    // Do not retain multiple differently configured outstanding readers or use
    // any operation concurrently/reentrantly. No packet/config pointer is kept.
private:
    DeviceButtonSample48Y sample_{};
    DualShockAxisBytes48X packet_{}; // Unavailable until an Accepted sample.
    DeviceHeldProvider48Y provider_{};
};
}
