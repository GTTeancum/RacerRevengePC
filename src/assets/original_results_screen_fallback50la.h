#pragma once
#include "original_standard_archive_provider50la.h"
#include "original_standard_material_context50la.h"
#include <memory>
#include <string>
#include <vector>
namespace rr::assets {
class OriginalResultsPrimitiveReferences50LA;
class OriginalStandardMaterialTable50LA;
class OriginalStandardMaterialObject50LA;
class OriginalStandardTextureTable50LA;
// Private source-occurrence authority minted only by the connected Results
// loader's material-reference facet. Not a public missing-file/name switch,
// renderer receipt, HUD proof, or a claim that Results constructed an old row.
class OriginalResultsScreenFallback50LA final {
    friend class OriginalResultsPrimitiveReferences50LA;
    friend class OriginalStandardMaterialTable50LA;
    friend class OriginalStandardMaterialObject50LA;
    OriginalResultsScreenFallback50LA()=default;
    std::int32_t row_=-1;
    std::uint32_t caller_=0,offset_=0,racer60_=0;
    std::size_t occurrence_=0,ordinal_=0;
    std::string name_,pstMember_;
    std::vector<std::uint8_t> pstBytes_;
    std::shared_ptr<const OriginalUiArchive50KG> overlays_;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures_;
    // Authentic selected132820 winner, not a preferred archive alias. This
    // records first-construction history and pins its original bank backing.
    OriginalUiEncodedArchiveView50KG selectedArchive_;
    std::size_t mount_=static_cast<std::size_t>(-1),member_=static_cast<std::size_t>(-1);
    std::uint64_t revision_=0,bankGeneration_=0;
    bool current(const std::string& materialName,
        const std::shared_ptr<const OriginalStandardMaterialContext50LA>&,
        const std::shared_ptr<OriginalStandardTextureTable50LA>&,
        const std::shared_ptr<const OriginalStandardArchiveProvider50LA>&)const noexcept;
public:
    ~OriginalResultsScreenFallback50LA()=default;
    OriginalResultsScreenFallback50LA(const OriginalResultsScreenFallback50LA&)=delete;
    OriginalResultsScreenFallback50LA& operator=(const OriginalResultsScreenFallback50LA&)=delete;
};
}
