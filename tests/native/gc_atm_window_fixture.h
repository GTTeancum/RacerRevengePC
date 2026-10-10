#pragma once
#include "gc_atm_animation_fixture.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <array>

// Narrow main-loop join. Construct after the host and before its renderer;
// close renderer before this bank. Main remains the sole clock/lifecycle owner.
namespace gc_atm_window_fixture {
namespace p=rr::platform;
class Owner final {
    gc_atm_animation_fixture::Owner source_;
    p::MaterialTextureGl50EB gpu_;
    std::array<p::VehicleWindowTrackAnimation50DS,gc_atm_animation_fixture::animationCount> bindings_{};
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return source_.loaded();}
    bool ownsResources()const noexcept{return gpu_.ownsResources();}
    bool loadSource(const std::filesystem::path& archive,
        const track_materials50fa_fixture::Owner& track,gc_atm_animation_fixture::Diagnostic& d)noexcept {
        return source_.load(archive,track,d);
    }
    bool createGpu(const p::MaterialTextureGlRequest50EB& caller,
        p::MaterialTextureGlDiagnostic50EB& d)noexcept {
        // Exactly35 authenticated frame images. Repeated unique material owners
        // must agree on the shared source image's explicit sampling policy.
        if(!source_.loaded())return false;
        constexpr auto frames=gc_atm_animation_fixture::frameTextureCount;
        std::array<p::MaterialTextureSampler50EB,frames> samplers{};
        std::array<bool,frames> assigned{};
        const auto& animations=*source_.animations();
        for(std::uint32_t i=0;i<animations.size();++i){
            const auto& animation=animations[i];
            bindings_[i]={animation.sourceSpanIndex,animation.sourceTextureIndex,
                animation.firstTextureIndex,animation.frameCount};
            if(animation.firstTextureIndex>frames||animation.frameCount>frames-animation.firstTextureIndex)return false;
            const p::MaterialTextureSampler50EB sampler{
                p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
                animation.clampU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat,
                animation.clampV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat};
            for(std::uint32_t frame=0;frame<animation.frameCount;++frame){
                const auto index=animation.firstTextureIndex+frame;
                if(assigned[index]&&(samplers[index].s!=sampler.s||samplers[index].t!=sampler.t))return false;
                samplers[index]=sampler;assigned[index]=true;
            }
        }
        for(bool present:assigned)if(!present)return false;
        auto request=caller;request.samplers=samplers.data();request.samplerCount=frames;
        request.maximumGpuBytes=512u*1024u;
        return gpu_.create(request,*source_.bank(),d);
    }
    bool bindRequest(p::VehicleWindowRequest50DS& request)const noexcept {
        if(!source_.loaded()||!gpu_.initialized())return false;
        request.trackAnimationTextures=&gpu_;
        request.trackAnimations=bindings_.data();
        request.trackAnimationCount=static_cast<std::uint32_t>(bindings_.size());return true;
    }
    // Consume main's existing sourceApplicationTimestamp exactly once at its
    // admitted accepted-world boundary. No private clock or pause offset.
    bool advance(std::uint32_t sourceApplicationTimestamp)noexcept {
        return source_.advance(sourceApplicationTimestamp);
    }
    bool bindFrame(p::VehicleWindowFrame50DS& frame)const noexcept {
        const auto* selected=source_.selectedFrames();if(!selected||!gpu_.initialized())return false;
        frame.trackAnimationFrames=selected->data();
        frame.trackAnimationFrameCount=static_cast<std::uint32_t>(selected->size());return true;
    }
    void reset()noexcept{source_.reset();}
    bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
}
