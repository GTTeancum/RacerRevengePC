#pragma once
#include "original_pause_source_data50la.h"
#include "original_ui_material50kg.h"
#include <array>
#include <memory>
#include <utility>

namespace rr::assets {
enum class OriginalPauseAuthoredMaterial50LA : std::uint8_t {
    letterBox, icon, font, leftArrow, rightArrow, count
};
enum class OriginalPauseMaterialDataError50LA : std::uint8_t {
    none, alreadyParsed, source, templates, material, allocation
};
struct OriginalPauseMaterialDataDiagnostic50LA {
    OriginalPauseMaterialDataError50LA error=OriginalPauseMaterialDataError50LA::none;
    OriginalPauseAuthoredMaterial50LA material=OriginalPauseAuthoredMaterial50LA::count;
    OriginalUiMaterialDiagnostic50KG detail{};
};

// Parsed authored data only. No numeric canonical reference is acquired here:
// the original 299BE0/2DF0B0/2DFD90 owners acquire references PER primitive, not
// once per distinct PSM. That ownership cannot be replaced by this data cache.
// Zero maps stay zero; actual texture references, pass order, GS provenance and
// authored animation sections are retained by the existing authentic parser.
class OriginalPauseAuthoredMaterials50LA final {
public:
    OriginalPauseAuthoredMaterials50LA() noexcept=default;
    OriginalPauseAuthoredMaterials50LA(const OriginalPauseAuthoredMaterials50LA&)=delete;
    OriginalPauseAuthoredMaterials50LA& operator=(const OriginalPauseAuthoredMaterials50LA&)=delete;

    bool parse(std::shared_ptr<const OriginalPauseSourceData50LA> source,
               std::shared_ptr<const OriginalUiMaterialTemplates50KG> templates,
               const OriginalUiMaterialRuntime50KG& actualRuntime,
               OriginalPauseMaterialDataDiagnostic50LA& diagnostic) noexcept {
        diagnostic={};
        if(parsed()) {
            diagnostic.error=OriginalPauseMaterialDataError50LA::alreadyParsed;
            return false;
        }
        if(!source || !source->sourceLoaded() || !source->overlays()) {
            diagnostic.error=OriginalPauseMaterialDataError50LA::source;
            return false;
        }
        if(!templates || !templates->loaded() || !actualRuntime.overrideKnown) {
            diagnostic.error=OriginalPauseMaterialDataError50LA::templates;
            return false;
        }
        try {
            constexpr OriginalPauseSourceMember50LA members[]={
                OriginalPauseSourceMember50LA::letterBoxMaterial,
                OriginalPauseSourceMember50LA::iconMaterial,
                OriginalPauseSourceMember50LA::fontMaterial,
                OriginalPauseSourceMember50LA::leftArrowMaterial,
                OriginalPauseSourceMember50LA::rightArrowMaterial
            };
            auto next=std::make_unique<Storage>();
            for(std::size_t i=0;i<materialCount;++i) {
                const auto member=source->member(members[i]);
                if(!member.identity || !member.data || member.size!=member.identity->bytes) {
                    diagnostic.error=OriginalPauseMaterialDataError50LA::source;
                    diagnostic.material=static_cast<OriginalPauseAuthoredMaterial50LA>(i);
                    return false;
                }
                // ScreenTexture2DF10C / ScreenText2DFAA0 explicitly associate
                // ScreenMT. LetterBoxPause follows2E1AE0->299BE0->14D590, the
                // same screen association as the original RadarOverlay path.
                if(!readOriginalUiMaterial50KG(*source->overlays(),member.identity->name,
                       *templates,OriginalUiTemplate50KG::screenMt,actualRuntime,
                       next->materials[i],diagnostic.detail)) {
                    diagnostic.error=OriginalPauseMaterialDataError50LA::material;
                    diagnostic.material=static_cast<OriginalPauseAuthoredMaterial50LA>(i);
                    return false;
                }
                // Parser output must remain tied to the member supplied by the
                // source owner. No foreign same-name PSM can replace this identity.
                if(next->materials[i].identity!=member.identity) {
                    diagnostic.error=OriginalPauseMaterialDataError50LA::source;
                    diagnostic.material=static_cast<OriginalPauseAuthoredMaterial50LA>(i);
                    return false;
                }
            }
            next->source=std::move(source);
            next->templates=std::move(templates);
            next->runtime=actualRuntime;
            storage_=std::move(next);
            return true;
        } catch(...) {
            diagnostic.error=OriginalPauseMaterialDataError50LA::allocation;
            return false;
        }
    }
    bool parsed() const noexcept { return bool(storage_); }
    const OriginalUiMaterial50KG* material(OriginalPauseAuthoredMaterial50LA id) const noexcept {
        const auto i=static_cast<std::size_t>(id);
        return storage_ && i<materialCount ? &storage_->materials[i] : nullptr;
    }
    const OriginalPauseSourceData50LA* source() const noexcept {
        return storage_ ? storage_->source.get() : nullptr;
    }
    const OriginalUiMaterialRuntime50KG* constructionRuntime() const noexcept {
        return storage_ ? &storage_->runtime : nullptr;
    }
private:
    static constexpr std::size_t materialCount=
        static_cast<std::size_t>(OriginalPauseAuthoredMaterial50LA::count);
    struct Storage {
        std::shared_ptr<const OriginalPauseSourceData50LA> source;
        std::shared_ptr<const OriginalUiMaterialTemplates50KG> templates;
        OriginalUiMaterialRuntime50KG runtime{};
        std::array<OriginalUiMaterial50KG,materialCount> materials{};
    };
    std::unique_ptr<Storage> storage_;
};
}
