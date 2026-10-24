#pragma once
#include "original_standard_archive_provider50la.h"
#include "original_standard_material_context50la.h"
#include <memory>
#include <vector>
namespace rr::platform {class OriginalUiMemoryCardConstructor50KG;}
namespace rr::assets {
class OriginalStandardMaterialTable50LA;
class OriginalStandardMaterialObject50LA;
class OriginalStandardTextureTable50LA;
class OriginalStandardProgramTable50LA;
// Pending source-construction capability, NOT historical origin or page authority.
// Only the real retained X/Triangle occurrence owner can mint it. It never owns its issuer/table/domain.
class OriginalMemoryCardScreenFallback50KG final {
    friend class rr::platform::OriginalUiMemoryCardConstructor50KG;
    friend class OriginalStandardMaterialTable50LA;
    friend class OriginalStandardMaterialObject50LA;
    OriginalMemoryCardScreenFallback50KG()=default;
    struct Observation {
        std::size_t roleIndex=static_cast<std::size_t>(-1),constructionIndex=static_cast<std::size_t>(-1);
        std::uint32_t componentOffset=0,sourceCallerPC=0;
        std::shared_ptr<const OriginalUiArchive50KG> options;
        OriginalUiEncodedArchiveView50KG selectedArchive;
        std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider;
        const std::uint8_t* selectedBytes=nullptr;
        std::vector<std::uint8_t> pstBytes;
        std::size_t mount=static_cast<std::size_t>(-1),ordinal=static_cast<std::size_t>(-1);
        std::uint64_t revision=0,bankGeneration=0;
    };
    std::weak_ptr<const rr::platform::OriginalUiMemoryCardConstructor50KG> issuer_;
    const OriginalStandardMaterialTable50LA* table_=nullptr;
    const void* primitive_=nullptr;
    std::int32_t row_=-1;
    std::string name_;
    std::size_t roleIndex_=static_cast<std::size_t>(-1),constructionIndex_=static_cast<std::size_t>(-1);
    std::uint32_t componentOffset_=0,sourceCallerPC_=0;
    std::shared_ptr<const OriginalUiArchive50KG> options_;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures_;
    std::shared_ptr<const OriginalStandardProgramTable50LA> programs_;
    bool occurrenceCurrent()const noexcept;
    bool captureAt29B638(const std::string&,
        const std::shared_ptr<const OriginalStandardMaterialContext50LA>&,
        const std::shared_ptr<OriginalStandardTextureTable50LA>&,
        const std::shared_ptr<const OriginalStandardProgramTable50LA>&,Observation&)const noexcept;
public:
    ~OriginalMemoryCardScreenFallback50KG()=default;
    OriginalMemoryCardScreenFallback50KG(const OriginalMemoryCardScreenFallback50KG&)=delete;
    OriginalMemoryCardScreenFallback50KG& operator=(const OriginalMemoryCardScreenFallback50KG&)=delete;
};
}
