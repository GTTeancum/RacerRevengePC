#pragma once
#include "original_ui_archive50kg.h"
#include "original_ui_localization50kg.h"
#include <array>
#include <memory>

namespace rr::assets {
// Original 1BA390 -> 1B8810 requests. These identify cold source data, not
// material-to-image bindings or native draw packets. In particular the pause
// font PSM and the ingame font's CEL/PST are distinct original inputs.
enum class OriginalPauseSourceMember50LA : std::uint8_t {
    results1Pixels, results2Pixels, ingameFontCells, ingameFontPixels,
    letterBoxMaterial, iconMaterial, fontMaterial, leftArrowMaterial,
    rightArrowMaterial, count
};
enum class OriginalPauseSourceError50LA : std::uint8_t {
    none, alreadyLoaded, overlaysArchive, localizationArchive, member,
    localization, allocation
};
struct OriginalPauseSourceDiagnostic50LA {
    OriginalPauseSourceError50LA error=OriginalPauseSourceError50LA::none;
    OriginalPauseSourceMember50LA member=OriginalPauseSourceMember50LA::count;
    OriginalUiLocalizationDiagnostic50KG localization{};
};

// A bounded immutable source-data layer for the original Pause owner. The real
// application supplies its existing authenticated archives. Member views pin
// the SAME OVERLAYS owner; no private resource registry or archive clone is made.
// Localization is parsed/copied transactionally by the existing authentic
// GameText parser. Failure leaves an empty owner retryable, reload leaves an
// existing owner unchanged. Serial cold use only, with disjoint diagnostics.
//
// sourceLoaded() does NOT attest original Entity/load callbacks, ScreenObject
// geometry, PSM interpretation, canonical material admission, decoded pixels,
// renderer readiness or input routing. This layer never activates Pause.
class OriginalPauseSourceData50LA final {
    friend class OriginalPausePrimitiveReferences50LA;
public:
    OriginalPauseSourceData50LA() noexcept=default;
    OriginalPauseSourceData50LA(const OriginalPauseSourceData50LA&)=delete;
    OriginalPauseSourceData50LA& operator=(const OriginalPauseSourceData50LA&)=delete;

    bool load(std::shared_ptr<const OriginalUiArchive50KG> overlays,
              const OriginalUiArchive50KG& localization,
              OriginalPauseSourceDiagnostic50LA& diagnostic) noexcept {
        diagnostic={};
        if(sourceLoaded()) {
            diagnostic.error=OriginalPauseSourceError50LA::alreadyLoaded;
            return false;
        }
        if(!overlays || !overlays->loaded() || overlays->identity()!=
           originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)) {
            diagnostic.error=OriginalPauseSourceError50LA::overlaysArchive;
            return false;
        }
        if(!localization.loaded() || localization.identity()!=
           originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::localization)) {
            diagnostic.error=OriginalPauseSourceError50LA::localizationArchive;
            return false;
        }
        try {
            // Paths are exact source requests plus the original font resource
            // pair. No basename fallback, alternate archive or substitute art.
            static const char* const names[]={
                "ui\\textures\\results1.pst", "ui\\textures\\results2.pst",
                "font\\denmarkbold16ingame.cel", "font\\denmarkbold16ingame.pst",
                "ui\\textures\\letterboxpause.psm", "ui\\textures\\icon_xpause.psm",
                "font\\denmarkbold16pause.psm", "ui\\textures\\arrwleftPause.psm",
                "ui\\textures\\arrwrghtPause.psm"
            };
            std::array<OriginalUiMemberView50KG,memberCount> members{};
            for(std::size_t i=0;i<memberCount;++i) {
                members[i]=overlays->find(names[i]);
                if(!members[i].identity || !members[i].data ||
                   members[i].size!=members[i].identity->bytes) {
                    diagnostic.error=OriginalPauseSourceError50LA::member;
                    diagnostic.member=static_cast<OriginalPauseSourceMember50LA>(i);
                    return false;
                }
            }
            auto english=std::make_unique<OriginalUiEnglish50KG>();
            if(!readOriginalUiEnglish50KG(localization,*english,diagnostic.localization)) {
                diagnostic.error=OriginalPauseSourceError50LA::localization;
                return false;
            }
            // All potentially throwing work precedes this atomic publication.
            members_=members;
            english_=std::move(english);
            overlays_=std::move(overlays);
            return true;
        } catch(...) {
            diagnostic.error=OriginalPauseSourceError50LA::allocation;
            return false;
        }
    }

    bool sourceLoaded() const noexcept { return overlays_ && english_; }
    OriginalUiMemberView50KG member(OriginalPauseSourceMember50LA id) const noexcept {
        const auto i=static_cast<std::size_t>(id);
        return sourceLoaded() && i<memberCount ? members_[i] : OriginalUiMemberView50KG{};
    }
    const OriginalUiEnglish50KG* english() const noexcept { return english_.get(); }
    // Borrow only while this source-data owner is alive. Later PSM parsing must
    // resolve its actual references in this archive, rather than assume icon_x
    // or an ATM frame merely from the related catalog names.
    const OriginalUiArchive50KG* overlays() const noexcept { return overlays_.get(); }
private:
    static constexpr std::size_t memberCount=
        static_cast<std::size_t>(OriginalPauseSourceMember50LA::count);
    std::shared_ptr<const OriginalUiArchive50KG> overlays_;
    std::unique_ptr<OriginalUiEnglish50KG> english_;
    std::array<OriginalUiMemberView50KG,memberCount> members_{};
};
}
