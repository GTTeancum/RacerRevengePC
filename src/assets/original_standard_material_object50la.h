#pragma once
#include "original_standard_material_source50la.h"
#include "original_standard_material_context50la.h"
#include "original_standard_material_parameters50la.h"
#include "original_standard_program_table50la.h"
#include "original_standard_texture_table50la.h"
#include "original_ui_material50kg.h"
#include <array>
#include <optional>

namespace rr::game { class OriginalApplicationAnimationClockView50KG; }
namespace rr::assets {
class OriginalStandardMaterialTable50LA;
class OriginalStandardMaterialAnimation50LA;
class OriginalStandardMaterialTextureAnimation50LA;
struct OriginalStandardMaterialDiagnostic50LA;
struct OriginalStandardMaterialPopulationInput50LA {
    // Exact members already selected/authenticated by the application resolver.
    // Missing here means unresolved, NOT proof that the original file is absent.
    const std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>>* sources=nullptr;
    std::shared_ptr<const OriginalStandardProgramTable50LA> actualPrograms382B70;
    std::shared_ptr<OriginalStandardTextureTable50LA> actualTextures383400;
    // Sealed capability from the actual native Video factory. Holds the real
    // display/application/runtime lifetimes and source-derived mode fields.
    std::shared_ptr<const OriginalStandardMaterialContext50LA> construction;
};
enum class OriginalStandardMaterialValueOrigin50LA : std::uint8_t {
    sourceDefault,templateValue,authored,runtime
};
struct OriginalStandardMaterialInteger50LA {
    std::int32_t value=0;
    OriginalStandardMaterialValueOrigin50LA origin=OriginalStandardMaterialValueOrigin50LA::sourceDefault;
    const OriginalUiMemberIdentity50KG* templateMember=nullptr;
    std::size_t byteOffset=0;
    std::uint32_t sourcePC=0;
};
// Typed native context state, replacing guest GS-packet allocation.
// Retains exact construction values/provenance; it neither emits guest words
// nor maps a source video base to host GPU storage. Native rendering/lowering
// and actual target binding remain separate, unsupported consumers.
struct OriginalStandardMaterialStateContext50LA {
    std::array<OriginalStandardMaterialInteger50LA,originalUiGsFieldCount50KG> fields{};
    std::array<OriginalStandardMaterialInteger50LA,6> sampler{};
    std::uint32_t videoDepthBase2E0=0,runtime435670=0;
    std::int32_t videoDepthFormat5C=0;
};
// First-context field names stay directly observable for existing single-map
// consumers. A dual-map2480D0 owner contains a distinct second context with
// its own authored _2 state; context selection is fixed0/1 by that constructor.
struct OriginalStandardMaterialState50LA : OriginalStandardMaterialStateContext50LA {
    std::optional<OriginalStandardMaterialStateContext50LA> secondContext;
    std::size_t contextCount() const noexcept {return secondContext?2u:1u;}
    const OriginalStandardMaterialStateContext50LA* context(std::size_t i) const noexcept {
        return i==0?this:(i==1&&secondContext?&*secondContext:nullptr);
    }
};
struct OriginalStandardMaterialPassView50LA {
    const std::int32_t* references04=nullptr;
    const std::int16_t* program18=nullptr;
    const std::array<std::int16_t,2>* texture1A=nullptr;
    const std::array<std::uint8_t,2>* uv1E=nullptr;
    const std::uint8_t* mapCount20=nullptr;
    const std::uint8_t* flags21=nullptr;
    const std::int16_t* group22=nullptr;
    const std::array<std::uint8_t,4>* fillRgba2C=nullptr;
    const OriginalUiMaterialTemplateState50KG* constructionTemplate=nullptr;
    const OriginalStandardMaterialState50LA* state30=nullptr;
    const std::uint32_t* parameterMask24=nullptr;
    const std::int32_t* parameterCount28=nullptr;
    const OriginalStandardMaterialParameters50LA* parameters34=nullptr;
    OriginalStandardMaterialAnimation50LA* animation10=nullptr;
    const OriginalStandardMaterialTextureAnimation50LA* textureAnimation10=nullptr;
    // Concrete construction owns every published dependency. No caller-set
    // readiness bit or substitute texture/program/animation object is accepted.
};
// Only the actual382B38 table may construct one. Each concrete owner has up to
// four actual reference-counted pass lifetimes, native state, and true dynamic
// texture refs; a recipe or externally supplied pointer cannot be published.
class OriginalStandardMaterialObject50LA final {
public:
    ~OriginalStandardMaterialObject50LA();
    OriginalStandardMaterialObject50LA(const OriginalStandardMaterialObject50LA&)=delete;
    OriginalStandardMaterialObject50LA& operator=(const OriginalStandardMaterialObject50LA&)=delete;
    bool constructed29B550() const noexcept;
    const std::int32_t* row40() const noexcept;
    const std::int32_t* passCount48() const noexcept;
    const std::string* name00() const noexcept;
    const OriginalStandardMaterialSource50LA* source() const noexcept;
    // Null for authored PSMs. Missing-PSM construction never invents source().
    const OriginalUiMaterialProvenance50KG* tournamentFallbackProvenance() const noexcept;
    const OriginalUiMaterialProvenance50KG* speedBoostFallbackProvenance() const noexcept;
    const OriginalUiMaterialProvenance50KG* damageFallbackProvenance() const noexcept;
    bool pass4C(std::size_t,OriginalStandardMaterialPassView50LA&) const noexcept;
    const std::int32_t* collector5C(std::size_t) const noexcept;
    const OriginalStandardTextureTable50LA* textures383400() const noexcept;
private:
    friend class OriginalStandardMaterialTable50LA;
    bool select29BFF0(OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool serviceCableBasePrototype29AC90(std::size_t,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool serviceTournamentMarker24A740(const OriginalStandardTextureTable50LA&,
        const OriginalUiMemberView50KG&,const OriginalUiMemberView50KG&,
        const rr::game::OriginalApplicationAnimationClockView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool serviceTexturePrototype29AC90(std::size_t,const OriginalStandardTextureTable50LA&,
        const rr::game::OriginalApplicationAnimationClockView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool selectModelPrototype24B26C(std::size_t,std::size_t,const OriginalStandardTextureResource50LA&,
        const OriginalStandardTextureResource50LA*&,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool selectTexturePrototype29AF90(std::size_t,std::size_t,const OriginalStandardTextureResource50LA&,
        const OriginalStandardTextureResource50LA*&,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool selectScreenPrototype24B278(std::size_t,std::size_t,
        const OriginalStandardTextureResource50LA&,
        const OriginalStandardTextureResource50LA*&,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool demandTournamentFallback2DF8A0(const OriginalStandardTextureTable50LA&,
        const OriginalUiArchive50KG&,std::uint32_t storePC,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool demandTournamentMarker2DF8A0(const OriginalStandardTextureTable50LA&,
        const OriginalUiMemberView50KG&,const OriginalUiMemberView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool demandTournamentOutline2DF8A0(const OriginalStandardTextureTable50LA&,
        const OriginalUiMemberView50KG&,const OriginalUiMemberView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    OriginalStandardMaterialObject50LA();
    // Only the canonical registry owner clears/publishes these source5C
    // mappings. Nodes borrow existing passes; these calls own no pass refs.
    void clearCollectors29BEA0() noexcept;
    void setCollector29BED0(std::size_t,std::int32_t) noexcept;
    bool construct29B550(std::int32_t canonicalRow,const std::string& canonicalName,
        std::shared_ptr<const OriginalUiMaterialTemplates50KG>,
        const OriginalUiMaterialTemplateState50KG* actualAssociation30,
        const OriginalStandardMaterialPopulationInput50LA&,
        std::shared_ptr<const OriginalUiArchive50KG> tournamentMissingPsm,
        std::shared_ptr<const OriginalUiArchive50KG> speedBoostMissingPsm,
        std::shared_ptr<const OriginalUiArchive50KG> damageMissingPsm,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    // Source24A430. Only the canonical manager supplies its one persistent
    // lazy application state. Surface values come from the retained PSG;
    // no caller-created texture pointer, image or readiness token is accepted.
    bool demand24A430(const float* uvSizes,std::uint8_t uvCount,float projectedRadius,
        float scale,float applicationScale4356E0,bool& firstTexture24A4EC,
        std::uint32_t& assertUv4356E8,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
