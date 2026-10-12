#pragma once
#include "gameplay_binder_candidate_fixture.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"

namespace gameplay_binder_window_candidate_fixture {
namespace a=rr::assets;namespace g=rr::game;namespace p=rr::platform;
namespace core=g::original_gameplay_binder_candidate;
// Borrow the actual application material mode plus authenticated DefaultMT.
// 29B950/299330 registers named Binder; 29AAA0 leaves unique-copy flag clear,
// 29B1D0 returns its base controller, and material registry42F9D0 resolves the
// same canonical textures\\binder entry for every ordinary racer.
struct SourceDependencies {
    const a::OriginalUiMaterialTemplates50KG& templates;
    const a::OriginalUiMaterialRuntime50KG& materialRuntime;
};
class Owner final {
    gameplay_binder_candidate_fixture::Owner source_;
    p::MaterialTextureGl50EB gpu_;
    std::array<core::Mesh,8> meshes_{};
    std::array<bool,8> ready_{};
    core::Workspace workspace_;
public:
    bool loaded()const noexcept{return source_.loaded();}
    bool ownsResources()const noexcept{return gpu_.ownsResources();}
    bool bindRacer(std::uint32_t slot,const native_racer50HW::Rig& rig,
        const g::VehicleFrameState49W& state,const g::VehicleFrameBindings49W& frame,
        const SourceDependencies& dependencies,gameplay_binder_candidate_fixture::Diagnostic& d)noexcept {
        return source_.bind(slot,rig,state,frame,dependencies.templates,dependencies.materialRuntime,d);
    }
    bool createGpu(const p::MaterialTextureGlRequest50EB& caller,p::MaterialTextureGlDiagnostic50EB& d)noexcept {
        if(!loaded())return false;
        for(unsigned i=0;i<8;++i)if(!source_.bindings(i))return false;
        std::array<p::MaterialTextureSampler50EB,10> samplers;
        for(auto& sampler:samplers)sampler={p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
            p::MaterialTextureWrap50EB::repeat,p::MaterialTextureWrap50EB::repeat};
        auto request=caller;request.samplers=samplers.data();request.samplerCount=10;
        request.maximumGpuBytes=10u*64u*64u*4u;
        return gpu_.create(request,*source_.bank(),d);
    }
    bool bindRequest(p::VehicleWindowRequest50DS& request)const noexcept {
        if(!gpu_.initialized())return false;
        request.gameplayBinderTextures=&gpu_;return true;
    }
    //24A758 material/render preparation consumes the actual application time
    // already published by the caller. Main/paused menus never reach this.
    // Native publication cadence is retained; console sample parity unclaimed.
    bool advanceRaceRender(std::uint32_t sourceApplicationTimestamp)noexcept {
        return source_.advance(sourceApplicationTimestamp);
    }
    // Called while the ACTUAL slot is visited, after its completed graphics
    // update. Output is retained if that native opponent later stops, matching
    // the harness's explicit last-complete geometry policy, not retail parity.
    bool stageRacer(std::uint32_t slot,const g::VehicleFrameState49W& state,
        const g::VehicleFrameBindings49W& frame,const g::Vector3_48C& cameraBackward,
        const g::original_gc_gameplay_lighting_candidate::Owner& lights)noexcept {
        if(slot>=8||!source_.bindings(slot))return false;
        std::array<float,4> ambient;
        if(!lights.currentSourceGlobalAmbient50(ambient))return false;
        g::OriginalUiRiderBinderQuad50KG quad;
        const std::array<float,3> camera{{cameraBackward.v[0],cameraBackward.v[1],cameraBackward.v[2]}};
        if(core::project(state,frame,*source_.bindings(slot),camera,quad,workspace_)!=core::Status::complete||
            core::shade(quad,ambient,meshes_[slot])!=core::Status::complete)return false;
        ready_[slot]=true;return true;
    }
    bool bindRaceFrame(p::VehicleWindowFrame50DS& frame)const noexcept {
        if(!gpu_.initialized()||frame.opponentCount!=7)return false;
        for(bool ready:ready_)if(!ready)return false;
        frame.gameplayBinders=meshes_.data();frame.gameplayBinderCount=8;
        frame.gameplayBinderFrame=source_.selectedFrame();return frame.gameplayBinderFrame<10;
    }
    void reset()noexcept{source_.reset();ready_.fill(false);}
    bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
}
