#pragma once
#include "ma_sa_atm_animation_fixture.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"

// Main remains sole timestamp, pause, restart and GL lifecycle owner.
namespace ma_sa_atm_window_fixture {
namespace p=rr::platform;
class Owner final {
    ma_sa_atm_animation_fixture::Owner source_;
    p::MaterialTextureGl50EB gpu_;
    std::vector<p::VehicleWindowTrackAnimation50DS> bindings_;
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return source_.loaded();}
    bool ownsResources()const noexcept{return gpu_.ownsResources();}
    bool loadSource(const std::filesystem::path& archive,const std::filesystem::path& frameRoot,
        track_source_fixture::Track selectedTrack,const track_materials50fa_fixture::Owner& track,
        ma_sa_atm_animation_fixture::Diagnostic& d)noexcept {
        return source_.load(archive,frameRoot,selectedTrack,track,d);
    }
    bool createGpu(const p::MaterialTextureGlRequest50EB& caller,p::MaterialTextureGlDiagnostic50EB& d)noexcept {
        if(!source_.loaded())return false;
        try{
            const auto& animations=*source_.animations();const auto& policies=*source_.sampling();
            std::vector<p::MaterialTextureSampler50EB> samplers(policies.size());
            std::vector<p::VehicleWindowTrackAnimation50DS> bindings;bindings.reserve(animations.size());
            for(std::size_t i=0;i<policies.size();++i){
                samplers[i]={p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
                    policies[i].clampU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat,
                    policies[i].clampV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat};
            }
            for(const auto& animation:animations){
                if(animation.firstTextureIndex>policies.size()||animation.frameCount>policies.size()-animation.firstTextureIndex)return false;
                bindings.push_back({animation.sourceSpanIndex,animation.sourceTextureIndex,animation.firstTextureIndex,animation.frameCount});
            }
            auto request=caller;request.samplers=samplers.data();request.samplerCount=static_cast<std::uint32_t>(samplers.size());
            request.maximumGpuBytes=ma_sa_atm_animation_fixture::maxRgbaBytes;
            if(!gpu_.create(request,*source_.bank(),d))return false;
            bindings_=std::move(bindings);return true;
        }catch(...){return false;}
    }
    bool bindRequest(p::VehicleWindowRequest50DS& request)const noexcept {
        if(!source_.loaded()||!gpu_.initialized())return false;
        request.trackAnimationTextures=&gpu_;request.trackAnimations=bindings_.data();
        request.trackAnimationCount=static_cast<std::uint32_t>(bindings_.size());return true;
    }
    bool advance(std::uint32_t sourceApplicationTimestamp)noexcept{return source_.advance(sourceApplicationTimestamp);}
    bool bindFrame(p::VehicleWindowFrame50DS& frame)const noexcept {
        const auto* selected=source_.selectedFrames();if(!selected||!gpu_.initialized())return false;
        frame.trackAnimationFrames=selected->data();frame.trackAnimationFrameCount=static_cast<std::uint32_t>(selected->size());return true;
    }
    void reset()noexcept{source_.reset();}
    bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
}
