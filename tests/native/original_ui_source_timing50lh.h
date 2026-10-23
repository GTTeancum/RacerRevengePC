#pragma once
#include "../../src/game/frame_timing49i.h"
#include "../../src/game/lap_host_clock50id.h"
#include <cstdint>
#include <memory>
#include <limits>

// Bounded native UI orchestration of the recovered140AC0 numeric leaves.
// Not an Application owner, timer sampler, root scheduler or PCM clock.
namespace original_ui_source_timing50LH {
struct Sample {
    std::uint32_t timestamp=0,deltaMilliseconds=0;
    float raw250=0,updateSeconds=0;
};
class Owner;
// Same owner-thread-only contract as Owner. Retained const access is not
// concurrent publication/read synchronization.
class PublicationView final {
    friend class Owner;
    struct Publication {
        Sample sample{};
        std::uint64_t serial=0;
        bool alive=true,prepared=false;
    };
    std::shared_ptr<const Publication> owner_;
public:
    PublicationView()noexcept=default;
    bool sameOwner(const PublicationView& other)const noexcept {
        return owner_&&other.owner_&&owner_.get()==other.owner_.get()&&
            !owner_.owner_before(other.owner_)&&!other.owner_.owner_before(owner_)&&owner_->alive;
    }
    bool readPrepared(Sample& sample,std::uint64_t& publication)const noexcept {
        if(!owner_||!owner_->alive||!owner_->prepared||!owner_->serial)return false;
        sample=owner_->sample;publication=owner_->serial;return true;
    }
};
class Owner final {
    rr::game::FrameTimingState49I committed_{},pending_{};
    Sample pendingSample_{};
    std::uint64_t publication_=0;
    std::shared_ptr<PublicationView::Publication> publicationOwner_;
    bool initialized_=false,prepared_=false;
    std::uint64_t committedNanoseconds_=0,pendingNanoseconds_=0;
    // Original423ED8 starts zero but is mutable through a debug matcher. This
    // bounded native UI has no such binding; do not claim immutable source state.
    const std::uint32_t optionalCapWord423ED8_=0;
public:
    Owner()noexcept=default;
    ~Owner()noexcept {if(publicationOwner_)publicationOwner_->alive=false;}
    // Explicitly fallible publication retention, never a hidden allocation in
    // this numeric owner's noexcept constructor or prepare/commit path.
    bool retainPublication(PublicationView& out)noexcept {
        try {
            if(!publicationOwner_){
                auto next=std::make_shared<PublicationView::Publication>();
                next->sample=pendingSample_;next->serial=publication_;next->prepared=prepared_;
                publicationOwner_=std::move(next);
            }
            out.owner_=publicationOwner_;return true;
        }catch(...){return false;}
    }
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    bool prepare(std::uint64_t retainedActiveTimelineNanoseconds,bool hostFirstOrResumed,Sample& out)noexcept {
        if(publication_==std::numeric_limits<std::uint64_t>::max()||prepared_||(!initialized_&&!hostFirstOrResumed)||
           (initialized_&&retainedActiveTimelineNanoseconds<committedNanoseconds_))return false;
        std::uint32_t quantizedTimestamp=0;
        if(!rr::game::hostNanosecondsToSourceTimestamp50ID(retainedActiveTimelineNanoseconds,quantizedTimestamp))return false;
        auto staged=committed_;
        // Existing native first/resume zero-delta policy only. This is not a
        // claim that original cold/focus transitions invoke140D70 here.
        if(hostFirstOrResumed)rr::game::resetFrameTimestamps49I(staged,quantizedTimestamp);
        rr::game::FrameElapsed49I elapsed{};
        if(!rr::game::beginFrameTiming49I(staged,quantizedTimestamp,optionalCapWord423ED8_,elapsed))return false;
        pending_=staged;pendingNanoseconds_=retainedActiveTimelineNanoseconds;prepared_=true;
        out={staged.current230,elapsed.deltaMilliseconds,staged.raw250,elapsed.elapsedSeconds};
        pendingSample_=out;++publication_;
        if(publicationOwner_){publicationOwner_->sample=out;publicationOwner_->serial=publication_;publicationOwner_->prepared=true;}
        return true;
    }
    // Caller commits only after its ONE accepted page update and PCM service.
    // Failed updates retain pending admission and cannot replay consumed effects.
    bool commit()noexcept {
        if(!prepared_)return false;
        rr::game::finishFrameTiming49I(pending_);
        committed_=pending_;committedNanoseconds_=pendingNanoseconds_;prepared_=false;initialized_=true;
        if(publicationOwner_)publicationOwner_->prepared=false;
        return true;
    }
};
}
