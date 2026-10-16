#pragma once
#include "../game/original_application_animation_clock50kg.h"
#include "original_ui_material_application50la.h"
#include "original_standard_texture_lifetime50la.h"
#include <cstdint>
#include <memory>
#include <utility>

namespace original_main_entry50KG { struct Video; }

namespace rr::assets {
// Construction-time projection of the ACTUAL native Video owner and the
// application's APPSTART/templates, immutable runtime and distinct443870 CPU
// texture lifetime. Only that Video's private backing can mint a context.
// The admitted original2379C0 mode is640x448x32;25B9F0 derives+2E0=280,
// +5C=0x31. These are source semantic values, never native/guest addresses.
// A context does not configure a display, own GS memory, or certify a draw.
class OriginalStandardMaterialContext50LA final {
    using Video=original_main_entry50KG::Video;
    using ValidatePin=bool (*)(const Video&) noexcept;
    friend struct original_main_entry50KG::Video;
    friend class OriginalStandardTextureResource50LA;

    std::shared_ptr<const Video> video_;
    std::shared_ptr<const OriginalUiMaterialApplication50LA> materialApplication_;
    std::shared_ptr<const OriginalUiMaterialRuntime50KG> materialRuntime_;
    std::shared_ptr<OriginalStandardTextureLifetime50LA> textureLifetime_;
    ValidatePin validatePin_;
    game::OriginalApplicationAnimationClockView50KG applicationAnimation_;

    OriginalStandardMaterialContext50LA(std::shared_ptr<const Video> video,
        std::shared_ptr<const OriginalUiMaterialApplication50LA> application,
        std::shared_ptr<const OriginalUiMaterialRuntime50KG> runtime,
        std::shared_ptr<OriginalStandardTextureLifetime50LA> textureLifetime,
        ValidatePin validate,game::OriginalApplicationAnimationClockView50KG animation) noexcept
        :video_(std::move(video)),materialApplication_(std::move(application)),
         materialRuntime_(std::move(runtime)),
         textureLifetime_(std::move(textureLifetime)),
         validatePin_(validate),applicationAnimation_(std::move(animation)) {}

    //29C780/29C7E0 reach original443870, the distinct Application-owned
    // texture manager. Only genuine resources can use this sealed capability.
    bool registerTexture29C780(OriginalStandardTextureLifetimeLink50LA& link) const noexcept {
        return live()&&textureLifetime_->register2557E0(link);
    }
    bool unregisterTexture29C7E0(OriginalStandardTextureLifetimeLink50LA& link) const noexcept {
        // Closing the display rejects new work, but cleanup still removes the
        // resource from the SAME retained manager after Video ceases to be live.
        return textureLifetime_&&textureLifetime_->unregister2557F0(link);
    }
public:
    OriginalStandardMaterialContext50LA()=delete;
    OriginalStandardMaterialContext50LA(const OriginalStandardMaterialContext50LA&)=delete;
    OriginalStandardMaterialContext50LA& operator=(const OriginalStandardMaterialContext50LA&)=delete;
    ~OriginalStandardMaterialContext50LA()=default;

    // A real platform observation installed by Video's private factory path,
    // not a caller-filled readiness flag or a success callback supplied to a
    // material consumer. Closing/releasing this host invalidates new work.
    // Application may reopen that SAME Video after complete session cleanup;
    // its immutable source mode and this CPU construction identity survive.
    // This never retains or revalidates an old GL texture/renderer publication.
    bool live()const noexcept {
        return video_&&textureLifetime_&&materialApplication_&&materialApplication_->prepared()&&
            materialRuntime_&&materialRuntime_->overrideKnown&&
            !materialRuntime_->override435670&&
            validatePin_&&validatePin_(*video_);
    }
    const Video* videoIdentity()const noexcept{return video_.get();}
    // Grant only through the existing live Video construction domain. The
    // returned capability observes time only; it cannot certify later GL work.
    game::OriginalApplicationAnimationClockView50KG applicationAnimationClock()const noexcept {
        return live()?applicationAnimation_:game::OriginalApplicationAnimationClockView50KG{};
    }
    const std::shared_ptr<const OriginalUiMaterialApplication50LA>&
        materialApplication()const noexcept{return materialApplication_;}

    // Exact positive640x448 arithmetic of25BAF4/25BB20:
    // each source framebuffer is(640*448)/2048=140; Z base follows two.
    std::uint32_t depthBufferBase2E0()const noexcept{return 280u;}
    std::uint32_t depthBufferFormat5C()const noexcept{return 0x31u;}
    // This is the SAME immutable backing read as Application.materialRuntime,
    // retained across construction. No independent normal-mode value exists.
    std::uint32_t runtime435670()const noexcept{return materialRuntime_->override435670?1u:0u;}
    const OriginalUiMaterialRuntime50KG& materialRuntime()const noexcept{return *materialRuntime_;}
};
}
