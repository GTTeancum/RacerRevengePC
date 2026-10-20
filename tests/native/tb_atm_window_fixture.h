#pragma once
#include "tb_atm_animation_fixture.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"

// Main remains sole timestamp, pause, restart and GL lifecycle owner.
namespace tb_atm_window_fixture {
namespace p=rr::platform;
class Owner final {
    tb_atm_animation_fixture::Owner source_;
    p::MaterialTextureGl50EB gpu_;
    std::vector<p::VehicleWindowTrackAnimation50DS> bindings_;
    const std::thread::id thread_=std::this_thread::get_id();
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner()noexcept{if(thread_!=std::this_thread::get_id())std::terminate();}
    bool loaded()const noexcept{return source_.loaded();}
    bool ownsResources()const noexcept{return thread_==std::this_thread::get_id()&&gpu_.ownsResources();}
    // Retain the same actual typed TB world through frame/request borrowers.
    bool loadSource(track_source_fixture::Track selectedTrack,std::shared_ptr<const tb_original_world50LM::Owner> track,
        tb_atm_animation_fixture::Diagnostic& d)noexcept {
        return source_.load(selectedTrack,std::move(track),d);
    }
    bool createGpu(const p::MaterialTextureGlRequest50EB& caller,p::MaterialTextureGlDiagnostic50EB& d)noexcept {
        if(!source_.loaded()||!source_.allFramesPinned()){
            d={};d.error=p::MaterialTextureGlError50EB::assets;
            d.operation="TB ATM output pins require separate measured adoption";return false;
        }
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
            request.maximumGpuBytes=tb_atm_animation_fixture::maxRgbaBytes;
            if(!gpu_.create(request,*source_.bank(),d))return false;
            bindings_=std::move(bindings);return true;
        }catch(...){return false;}
    }
    bool bindRequest(p::VehicleWindowRequest50DS& request)const noexcept {
        if(!source_.loaded()||!source_.allFramesPinned()||!gpu_.initialized()||
            request.track.mesh!=source_.worldOwner()->mesh())return false;
        request.trackAnimationTextures=&gpu_;request.trackAnimations=bindings_.data();
        request.trackAnimationCount=static_cast<std::uint32_t>(bindings_.size());return true;
    }
    bool advance(std::uint32_t sourceApplicationTimestamp)noexcept{return source_.advance(sourceApplicationTimestamp);}
    bool bindFrame(p::VehicleWindowFrame50DS& frame)const noexcept {
        const auto* selected=source_.selectedFrames();if(!selected||!gpu_.initialized())return false;
        frame.trackAnimationFrames=selected->data();frame.trackAnimationFrameCount=static_cast<std::uint32_t>(selected->size());return true;
    }
    void reset()noexcept{source_.reset();}
    bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{
        if(thread_!=std::this_thread::get_id())return false;return gpu_.close(d);
    }
};
}
