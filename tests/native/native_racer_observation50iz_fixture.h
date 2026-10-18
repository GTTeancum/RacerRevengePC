#pragma once
// Fresh scalar copies from the bank's registered owners. Handles cache only
// stable addresses/offsets, never racer values. No arena pointer is returned.
#include "native_racer_bank50hn_fixture.h"
#include "../../src/game/vehicle_frame49w.h"
#include <array>

namespace native_racer50IZ {
using Bank=native_racer50HN::Bank;
template<class T> using Read=Bank::ReadHandle<T>;
struct FrameReads {
    Read<rr::game::VehicleFramePhase49W> phase;
    Read<std::uint32_t> vehicle3C,vehicle58,vehicle40,partnerStateD34;
    Read<std::uint8_t> flagsB9C,flagsCD8;
    Read<bool> aiOwnersBound,dynamicOwnersBound;
    Read<float> cachedSpeed34,smoothedChannel0D28,worldAngularVelocityA4;
    std::array<std::array<Read<float>,3>,rr::game::vehicleFrameNodeCount49W> localPosition,localForward,localUp;
    std::array<Read<float>,3> velocity70,linearAccelerationC0;

    void bind(const Bank& bank,const rr::game::VehicleFrameState49W& frame) {
        phase=bank.bindRead(frame.phase);
        vehicle3C=bank.bindRead(frame.fields.vehicle3C);
        vehicle58=bank.bindRead(frame.fields.vehicle58);
        vehicle40=bank.bindRead(frame.bodies.main.vehicleState40);
        partnerStateD34=bank.bindRead(frame.impactSupplement.partnerState);
        flagsB9C=bank.bindRead(frame.bodies.main.controls.player.flagsB9C);
        flagsCD8=bank.bindRead(frame.fields.impactFlagsCD8);
        aiOwnersBound=bank.bindRead(frame.fields.nativeAiOwnersBound50IV);
        dynamicOwnersBound=bank.bindRead(frame.fields.dynamicOwnersBound50IW);
        cachedSpeed34=bank.bindRead(frame.bodies.main.cachedSpeed34);
        smoothedChannel0D28=bank.bindRead(frame.bodies.main.controls.bound.control.shaping.smoothedChannel0D28);
        worldAngularVelocityA4=bank.bindRead(frame.bodies.main.components.root.worldAngularVelocityA0.v[1]);
        // nodes is inline storage in VehicleFrameState49W's registered prefix.
        // Bind actual matrix lanes, never a copied graph/binding pointer.
        for(std::size_t node=0;node<localPosition.size();++node)
            for(unsigned axis=0;axis<3;++axis){
                localPosition[node][axis]=bank.bindRead(frame.nodes[node].localC0.v[12+axis]);
                localForward[node][axis]=bank.bindRead(frame.nodes[node].localC0.v[8+axis]);
                localUp[node][axis]=bank.bindRead(frame.nodes[node].localC0.v[4+axis]);
            }
        for(unsigned axis=0;axis<3;++axis){
            velocity70[axis]=bank.bindRead(frame.bodies.main.components.root.body.velocity.v[axis]);
            linearAccelerationC0[axis]=bank.bindRead(frame.bodies.main.components.root.body.linearAcceleration.v[axis]);
        }
    }
    float forward(const Bank& bank,std::size_t slot,
        rr::game::SceneNodeIndex49P node,unsigned axis) const {
        const auto index=static_cast<std::uint32_t>(node);
        if(index>=localForward.size()||axis>=3)
            throw std::runtime_error("racer observation inline forward lane is out of bounds");
        return bank.read(slot,localForward[index][axis]);
    }
    float up(const Bank& bank,std::size_t slot,
        rr::game::SceneNodeIndex49P node,unsigned axis) const {
        const auto index=static_cast<std::uint32_t>(node);
        if(index>=localUp.size()||axis>=3)
            throw std::runtime_error("racer observation inline up lane is out of bounds");
        return bank.read(slot,localUp[index][axis]);
    }
    float position(const Bank& bank,std::size_t slot,
        rr::game::SceneNodeIndex49P node,unsigned axis) const {
        const auto index=static_cast<std::uint32_t>(node);
        if(index>=localPosition.size()||axis>=3)
            throw std::runtime_error("racer observation inline scene lane is out of bounds");
        return bank.read(slot,localPosition[index][axis]);
    }
};
} // namespace native_racer50IZ
