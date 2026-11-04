#pragma once
#include "../../src/game/original_camera_start_effects.h"
#include <cstdint>
#include <limits>
#include <optional>

namespace original_camera_session50KX {
using namespace rr::game;
// Native presentation policy: genuine260320/260610(async=true)/25B5D0 state,
// over one real retained race image while simulation/countdown are held.
// This is not retail live-underlay redraw chronology. The same actual renderer
// backend family that owns retained-image copies supplies every displayed sample
// and restore. Use its explicit asyncFrozenUnderlay25B5D0 policy instance;
// a blocking-only backend must reject this async request.
class AsyncRetainedFade final {
    OriginalVideoFadeState* video_=nullptr;
    OriginalVideoBlockingBackend* backend_=nullptr;
    HostMonotonicClock50ID* clock_=nullptr;
    std::chrono::steady_clock::time_point epoch_{};
    OriginalVideoFadeState expected_;
    std::optional<OriginalVideoFadeUpdate25B5D0> update_;
    unsigned phase_=0;
    std::uint64_t ordinal_=0,sourcePublication_=0;
    OriginalVideoBlockingSample sample_;
    bool complete_=false;
    OriginalVideoFadeDirection direction_=OriginalVideoFadeDirection::in261630;
public:
    AsyncRetainedFade()=default;
    AsyncRetainedFade(const AsyncRetainedFade&)=delete;
    AsyncRetainedFade& operator=(const AsyncRetainedFade&)=delete;
    bool complete()const noexcept{return complete_;}
    bool started()const noexcept{return video_!=nullptr;}
    OriginalCameraStartResult advance(OriginalVideoFadeState& video,HostMonotonicClock50ID& clock,
        std::uint64_t actualCameraPublication,OriginalVideoBlockingBackend* backend,
        OriginalVideoFadeDirection direction=OriginalVideoFadeDirection::in261630){
        using S=OriginalCameraStartStatus;
        if(complete_)return {direction_==direction?S::complete:S::owner,0x25b9b8};
        if(!backend)return {S::videoBackend,0x25e610};
        if(!actualCameraPublication)return {S::owner,0x25e610};
        if(!video_){
            if(direction!=OriginalVideoFadeDirection::in261630&&
               direction!=OriginalVideoFadeDirection::out260850)return {S::sourcePath,0x25b6cc};
            const auto expected=direction==OriginalVideoFadeDirection::out260850?5u:6u;
            if((video.flags404Low6&7u)!=expected)return {S::sourcePath,0x25b6cc};
            video_=&video;backend_=backend;clock_=&clock;epoch_=clock.epoch;expected_=video;
            sourcePublication_=actualCameraPublication;direction_=direction;
        }else if(video_!=&video||backend_!=backend||clock_!=&clock||epoch_!=clock.epoch||
                 sourcePublication_!=actualCameraPublication||direction_!=direction)
            return {S::owner,0x25e610};
        for(;;){
        // Recheck after every synchronous backend return, before more source
        // work. Shared lastNanoseconds may legitimately advance elsewhere.
        if(clock_!=&clock||epoch_!=clock.epoch||!OriginalVideoFadeUpdate25B5D0::sameFields(expected_,video))
            return {S::owner,0x25e610};
        switch(phase_){
        case 0:{
            const auto r=backend->begin(direction_,video);
            if(r.status!=S::complete)return r;phase_=1;break;}
        case 1:{
            if(ordinal_==std::numeric_limits<std::uint64_t>::max())return {S::owner,0x25e610};
            //The real new-video-frame numeric store25AAC8 clears drawn-bit3.
            //This native owner owns no other25A9C0 callbacks/hardware state.
            //The backend's successful begin owns the frozen image under its
            //exclusive renderer borrow.
            if(!update_){
                video.flags404Low6=static_cast<std::uint8_t>(video.flags404Low6&~8u);
                expected_=video;update_.emplace();
            }
            OriginalVideoFadeDraw draw;
            const auto r=update_->advance(video,clock,0,1,draw);
            expected_=video; //Includes real prefix stores even on numeric failure.
            if(r.status!=S::videoBackend)return r.status==S::complete?
                OriginalCameraStartResult{S::sourcePath,0x25e610}:r;
            if(!draw.reached25E610||!update_->current(video,clock))return {S::owner,0x25e610};
            const std::uint32_t argb=draw.argb;
            const std::uint64_t rgbaq=((argb>>16)&255u)|std::uint64_t(argb&0xff00u)|
                (std::uint64_t(argb&255u)<<16)|std::uint64_t(argb&0xff000000u)|
                (std::uint64_t(0x3f800000u)<<32);
            //Zero attributes the retained-image recopy to native policy; the
            //actual source draw is25E610. Async25B5D0 never calls262D80.
            sample_={direction_,ordinal_,rgbaq,
                static_cast<std::int32_t>(argb>>24),0};
            phase_=2;break;}
        case 2:{
            const auto r=backend->copyDrawAndComplete(sample_);
            if(r.status!=S::complete)return r;
            // Record actual backend completion before any fallible source
            // return guard. A retry must not redraw or present this sample.
            phase_=3;break;}
        case 3:{
            if(!update_)return {S::owner,0x25e610};
            const auto r=update_->afterNativeDrawComplete(video,clock);expected_=video;
            if(r.status!=S::complete)return r;
            ++ordinal_;
            const auto activeBit=direction_==OriginalVideoFadeDirection::out260850?1u:2u;
            if(video.flags404Low6&activeBit){update_.reset();phase_=1;return {S::videoClockWait,0x25b9b8};}
            phase_=4;break;}
        case 4:{
            // Async post-draw flags already belong to25B5D0's actual return.
            // Native restoration is separate and must not defer/undo them.
            const auto r=backend->restoreAndComplete(direction_);
            if(r.status!=S::complete)return r;
            phase_=5;break;}
        case 5:
            complete_=true;return {S::complete,0x25b9b8};
        default:return {S::owner,0x25e610};
        }}
    }
};
}
