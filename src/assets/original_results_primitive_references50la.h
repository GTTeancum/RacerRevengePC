#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include "original_ui_material50kg.h"
namespace rr::game { class OriginalResults10450LA; }
namespace rr::assets {
class OriginalStandardMaterialDomain50LA;
class OriginalStandardMaterialContext50LA;
class OriginalUiArchive50KG;
struct OriginalStandardMaterialDiagnostic50LA;
class OriginalStandardTextureResource50LA;
class OriginalStandardMaterialCollectorReceipt50LA;
class OriginalUiScreenAssets50KG;
struct OriginalUiMemberIdentity50KG;
// Private material-reference facet of the connected Results104 loader. The
// source occurrence schedule, not a public name/certificate, selects each row.
class OriginalResultsPrimitiveReferences50LA final {
    friend class rr::game::OriginalResults10450LA;
public:
    enum class Role : std::uint8_t { baseResults1,baseResults2,letterBox,baseTextPause,
        iconSquare,iconTriangle,iconCross,iconCircle,denmark16,eurostile13,portrait,
        medalGold,medalSilver,medalBronze,separator };
    struct Reference {std::int32_t material00=-1;bool started=false,retained=false;};
    ~OriginalResultsPrimitiveReferences50LA();
    OriginalResultsPrimitiveReferences50LA(const OriginalResultsPrimitiveReferences50LA&)=delete;
    OriginalResultsPrimitiveReferences50LA& operator=(const OriginalResultsPrimitiveReferences50LA&)=delete;
private:
    OriginalResultsPrimitiveReferences50LA()noexcept=default;
    bool prepareCold(std::shared_ptr<OriginalStandardMaterialDomain50LA>,
        std::shared_ptr<const OriginalStandardMaterialContext50LA>,
        std::shared_ptr<const OriginalUiArchive50KG>,std::size_t actualRacerCount,
        OriginalStandardMaterialDiagnostic50LA&)noexcept;
    // Dynamic5E0/A94/480 carry their field offset plus actual element ordinal.
    // Embedded arrays carry the actual embedded offset and ordinal0. Portrait
    // racer60 is freshly read by the SAME connected owner before this call;
    // all other roles require0. No future racer list is retained here.
    bool acquireNext(Role,std::uint32_t callerPC,std::uint32_t objectOffset,
        std::size_t objectOrdinal,std::uint32_t actualRacer60,
        const std::string& actualDirectory36B970,
        OriginalStandardMaterialDiagnostic50LA&)noexcept;
    std::size_t associatedCount()const noexcept{return next_;}
    const Reference* reference(std::size_t i)const noexcept{return i<references_.size()?&references_[i]:nullptr;}
    void release299AC0(std::size_t)noexcept;
    // Concrete native pixels for the freshly selected FINAL map. The source
    // caller must service its reached primitive first; this never dispatches
    // animation, grants raster policy or acknowledges a source draw.
    struct NativeImage {
        std::shared_ptr<const OriginalUiScreenAssets50KG> images;
        const OriginalStandardTextureResource50LA* tableObject=nullptr;
        const OriginalStandardTextureResource50LA* selected=nullptr;
        const OriginalUiMemberIdentity50KG* member=nullptr;
        std::uint32_t image=0;
        std::size_t offset=0,bytes=0;
    };
    bool decodeCurrentNativeImage50LA(std::size_t reference,
        const OriginalStandardMaterialCollectorReceipt50LA&,NativeImage&,
        OriginalStandardMaterialDiagnostic50LA&)noexcept;
    struct NativePrimitive {
        NativeImage image;
        OriginalUiNativeMaterial50KG textured;
        OriginalUiNativeUntexturedMaterial50KG untextured;
        bool hasTexture=false;
    };
    // Only a reached source primitive, after its own CPU projection. No
    // queue/caller command is accepted. Still uninvoked until renderer custody.
    bool prepareNativePrimitive50LA(std::size_t reference,
        const OriginalStandardMaterialCollectorReceipt50LA&,NativePrimitive&,
        OriginalStandardMaterialDiagnostic50LA&)noexcept;
    struct NativeImageCache {
        const OriginalStandardTextureResource50LA* tableObject=nullptr;
        std::shared_ptr<const OriginalUiArchive50KG> archive;
        const OriginalUiMemberIdentity50KG* member=nullptr;
        std::vector<const OriginalStandardTextureResource50LA*> children;
        std::vector<std::size_t> offsets,extents;
        std::shared_ptr<const OriginalUiScreenAssets50KG> images;
    };
    const std::thread::id nativeImageThread_=std::this_thread::get_id();
    std::vector<NativeImageCache> nativeImages_;
    struct Source {Role role{};std::uint32_t caller=0,offset=0;std::size_t ordinal=0;const char* directory=nullptr;const char* name=nullptr;bool missingPsm=false;};
    Source source(std::size_t)const noexcept;
    std::shared_ptr<OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<const OriginalUiArchive50KG> overlays_;
    std::vector<Reference> references_;
    std::size_t racers_=0,next_=0;
    bool prepared_=false,failed_=false,closing_=false;
};
}
