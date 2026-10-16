#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace rr::game { class OriginalTimeStats50LA; class OriginalSpeedBoost50LA; class OriginalDamage50LA; class OriginalPositionStatsHud50LA; }
namespace rr::assets {
class OriginalStandardMaterialObject50LA;
class OriginalStandardTextureResource50LA;
class OriginalStandardTextureResolver50LA;
class OriginalStandardMaterialContext50LA;
enum class OriginalStandardTextureStatus50LA : std::uint8_t {
    complete,unprepared,alreadyPrepared,input,range,reference,overflow,allocation,
    source,unresolved,duplicateSource,unsupported,malformed,phase,video,number
};
struct OriginalStandardTextureDiagnostic50LA {
    OriginalStandardTextureStatus50LA status=OriginalStandardTextureStatus50LA::complete;
    std::uint32_t sourcePC=0;
    std::int32_t row=-1;
    std::size_t source=static_cast<std::size_t>(-1),byteOffset=0;
    std::size_t populationVisited=0,populationPublished=0;
    bool sourceEffectsCommitted=false;
};
struct OriginalStandardTextureRowView50LA {
    const std::string* name00=nullptr;
    const OriginalStandardTextureResource50LA* object04=nullptr;
    const std::int32_t* references08=nullptr;
};
// Fresh dynamic383400 manager, source3CAFF0 -> 299130. It is not an image bank
// and cannot adopt an existing manager or reorder/erase rows. Resource
// population is a distinct bounded29E340 call using the SAME resolved source
// cache and sealed actual Application texture-lifetime context. Positive refs
// alone still do not imply29E340/104A00 success or GPU residency.
// One shared owner must outlive its materials. Destruction with live refs is
// a lifetime error and terminates instead of silently discarding ownership.
// Serial/nonreentrant. Row views borrow until the next registry mutation.
class OriginalStandardTextureTable50LA final {
public:
    OriginalStandardTextureTable50LA() noexcept;
    ~OriginalStandardTextureTable50LA();
    OriginalStandardTextureTable50LA(const OriginalStandardTextureTable50LA&)=delete;
    OriginalStandardTextureTable50LA& operator=(const OriginalStandardTextureTable50LA&)=delete;
    // Default construction is unprepared. This explicitly takes the original
    // fresh zero-count/null-buffer constructor route; no table asset is read.
    bool prepareFresh(OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool prepared() const noexcept;
    std::size_t rowCount() const noexcept;
    bool row(std::int16_t,OriginalStandardTextureRowView50LA&) const noexcept;
    // Read-only first-match part of299330, with ASCII case folding only.
    // Slash spelling remains significant. A miss never inserts or retains.
    bool find(const std::string&,std::int16_t&) const noexcept;
    //Ascending positive-ref/null-resource scan, retaining genuine shared-cache
    //aliases104960. Immutable source provider and actual lifetime context are
    //pinned on the first call and cannot be swapped. Earlier complete rows
    //survive terminal failure; an actually started partial resource remains
    //owned but unpublished until final native cleanup. No fallback absence is
    //inferred when the provider lacks the first-priority PST member.
    bool populate29E340(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<const OriginalStandardMaterialContext50LA>,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    bool populationTerminal() const noexcept;
    const OriginalStandardTextureResource50LA* partialConstruction() const noexcept;
private:
    friend class OriginalStandardMaterialObject50LA;
    friend class OriginalApplicationFonts50LA;
    friend class rr::game::OriginalTimeStats50LA;
    friend class rr::game::OriginalSpeedBoost50LA;
    friend class rr::game::OriginalDamage50LA;
    friend class rr::game::OriginalPositionStatsHud50LA;
    //Host ownership admission only. The SAME context that constructs material
    //map references must later construct their texture resources.
    bool bindMaterialContext(std::shared_ptr<const OriginalStandardMaterialContext50LA>,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    // Source2472F0/247304: get-or-insert with null resource, then retain once.
    // Stores first-inserted spelling; returns the existing row on later case
    // variants. Embedded NUL is outside the represented C-string domain.
    // Native safety bounds keep every pass token nonnegative signed16 and
    // refs representable signed32. Failure does not acquire a reference.
    bool acquire299330AndRetain298A30(const std::string&,std::int16_t&,
        OriginalStandardTextureDiagnostic50LA&) noexcept;
    //Genuine298E60 object read; true with null is an authentic pending row.
    //Only a material consumer can obtain mutable demand access to that object.
    bool resolve298E60(std::int16_t,OriginalStandardTextureResource50LA*&,
        OriginalStandardTextureDiagnostic50LA&) const noexcept;
    //Source29E400 decrements row refs; zero releases its actual104970 object
    //reference and clears resource04 while preserving name and index.
    // False means an invalid ownership release; caller must not ignore it.
    bool release29E400(std::int16_t) noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
