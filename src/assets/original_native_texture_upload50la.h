#pragma once
#include "original_standard_texture_lifetime50la.h"
#include <memory>
#include <utility>
#include <thread>

namespace original_main_entry50KG { class RaceDisplayLifetime50LA; struct Frontend; struct Video; }
namespace rr::platform { class MaterialTextureGl50EB; class OriginalUiRiderResidentPool50KG; }
namespace rr::assets {
class OriginalNativeTextureUploadScope50LA;
// Typed lifetime of the actual Frontend member, independent of race display.
class OriginalNativeFrontendUploadLifetime50LA final {
    friend struct ::original_main_entry50KG::Frontend;
    friend class OriginalNativeTextureUploadScope50LA;
    const ::original_main_entry50KG::Frontend* const frontend_;
    const std::shared_ptr<const ::original_main_entry50KG::Video> video_;
    bool (*const live_)(const OriginalNativeFrontendUploadLifetime50LA&) noexcept;
    const std::thread::id thread_=std::this_thread::get_id();
    std::uint64_t active_=0;
    bool retired_=false;
    OriginalNativeFrontendUploadLifetime50LA(
        const ::original_main_entry50KG::Frontend& frontend,
        std::shared_ptr<const ::original_main_entry50KG::Video> video,
        bool (*live)(const OriginalNativeFrontendUploadLifetime50LA&) noexcept)
        :frontend_(&frontend),video_(std::move(video)),live_(live){}
public:
    OriginalNativeFrontendUploadLifetime50LA(const OriginalNativeFrontendUploadLifetime50LA&)=delete;
    ~OriginalNativeFrontendUploadLifetime50LA(){if(active_)std::terminate();}
};

// Sealed by the existing actual display. Copies keep no old display alive.
// This binds real direct uploads only; it is not full manager13C coverage.
class OriginalNativeTextureUploadBinding50LA final {
    friend class ::original_main_entry50KG::RaceDisplayLifetime50LA;
    friend struct ::original_main_entry50KG::Frontend;
    friend class OriginalNativeTextureUploadScope50LA;
    using Display=::original_main_entry50KG::RaceDisplayLifetime50LA;
    const std::weak_ptr<const Display> display_;
    const std::weak_ptr<OriginalNativeFrontendUploadLifetime50LA> frontend_;
    const std::shared_ptr<OriginalStandardTextureLifetime50LA> manager_;
    const std::uint64_t generation_;
    void* const context_;
    bool (*const current_)(void*) noexcept;
    bool (*const live_)(const Display&) noexcept;
    OriginalNativeTextureUploadBinding50LA(std::weak_ptr<const Display> display,
        std::shared_ptr<OriginalStandardTextureLifetime50LA> manager,void* context,
        bool (*current)(void*) noexcept,bool (*live)(const Display&) noexcept)
        :display_(std::move(display)),manager_(std::move(manager)),
         generation_(manager_?manager_->nativeUploadGeneration_:0),context_(context),
         current_(current),live_(live){}
    OriginalNativeTextureUploadBinding50LA(
        std::weak_ptr<OriginalNativeFrontendUploadLifetime50LA> frontend,
        std::shared_ptr<OriginalStandardTextureLifetime50LA> manager,void* context,
        bool (*current)(void*) noexcept)
        :frontend_(std::move(frontend)),manager_(std::move(manager)),
         generation_(manager_?manager_->nativeUploadGeneration_:0),context_(context),
         current_(current),live_(nullptr){}
public:
    OriginalNativeTextureUploadBinding50LA(const OriginalNativeTextureUploadBinding50LA&)=delete;
};
// Stack lifetime spans the actual GL upload, including failed partial uploads.
// Link identity is the actual MaterialTextureGl owner, never a caller receipt.
class OriginalNativeTextureUploadScope50LA final {
    friend class rr::platform::MaterialTextureGl50EB;
    friend class rr::platform::OriginalUiRiderResidentPool50KG;
    friend class OriginalStandardTextureLifetime50LA;
    const std::shared_ptr<const OriginalNativeTextureUploadBinding50LA> binding_;
    std::shared_ptr<const ::original_main_entry50KG::RaceDisplayLifetime50LA> display_;
    std::shared_ptr<OriginalNativeFrontendUploadLifetime50LA> frontend_;
    const rr::platform::MaterialTextureGl50EB* uploader_;
    OriginalNativeTextureUploadScope50LA* next_=nullptr;
    bool registered_=false,completed_=false;
    OriginalNativeTextureUploadScope50LA(
        std::shared_ptr<const OriginalNativeTextureUploadBinding50LA> binding,
        const rr::platform::MaterialTextureGl50EB& uploader,void* context,
        bool (*current)(void*) noexcept):binding_(std::move(binding)),uploader_(&uploader) {
        if(!binding_)return;
        display_=binding_->display_.lock();
        frontend_=binding_->frontend_.lock();
        if(!binding_->manager_||context!=binding_->context_||current!=binding_->current_)return;
        if(display_) {
            if(frontend_||!binding_->live_||!binding_->live_(*display_))return;
        } else if(!frontend_||frontend_->thread_!=std::this_thread::get_id()||
                  frontend_->retired_||!frontend_->live_||
                  !frontend_->live_(*frontend_)||
                  frontend_->active_==std::numeric_limits<std::uint64_t>::max())return;
        auto& m=*binding_->manager_;
        if(binding_->generation_!=m.nativeUploadGeneration_)return;
        if(m.nativeUploadAttempts_==std::numeric_limits<std::uint64_t>::max())return;
        for(auto* p=m.nativeUploads_;p;p=p->next_)if(p->uploader_==uploader_)return;
        next_=m.nativeUploads_;m.nativeUploads_=this;++m.nativeUploadAttempts_;
        registered_=true;
        if(frontend_)++frontend_->active_;
    }
    bool admitted()const noexcept{return !binding_||registered_;}
    void completed()noexcept{completed_=true;}
public:
    OriginalNativeTextureUploadScope50LA(const OriginalNativeTextureUploadScope50LA&)=delete;
    ~OriginalNativeTextureUploadScope50LA()noexcept {
        if(!registered_)return;
        auto& m=*binding_->manager_;
        auto** link=&m.nativeUploads_;
        while(*link&&*link!=this)link=&(*link)->next_;
        if(!*link)std::terminate();
        *link=next_;
        if(completed_)++m.nativeUploadCompletions_;
        if(frontend_){if(!frontend_->active_)std::terminate();--frontend_->active_;}
    }
};
}
