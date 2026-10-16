#pragma once
#include "original_standard_material_table50la.h"

namespace rr::assets {
// The actual material00 ownership of one AIDamage primitive. The application
// table retains the ONLY row04/ref08/associated30 state. Host shared ownership
// pins that table; it never substitutes for the one numeric298A30 reference.
// Noncopyable/nonmovable because the enclosing primitive has stable identity.
class OriginalStandardScreenMaterialReference50LA final {
public:
    OriginalStandardScreenMaterialReference50LA() noexcept=default;
    ~OriginalStandardScreenMaterialReference50LA();
    OriginalStandardScreenMaterialReference50LA(const OriginalStandardScreenMaterialReference50LA&)=delete;
    OriginalStandardScreenMaterialReference50LA& operator=(const OriginalStandardScreenMaterialReference50LA&)=delete;
    // Only the reached299BE0("AIDamage",actual36B970,actual3833F0) domain.
    // Literal textures with an optional final backslash, case-insensitive.
    // No UI separator folding, caller-supplied row or copied ScreenMT owner.
    static bool acceptsAIDamageDirectory299BE0(const char*,std::size_t capacity) noexcept;
    bool acquireAIDamage299BE0(std::shared_ptr<OriginalStandardMaterialTable50LA>,
        const char* actualDirectory36B970,std::size_t capacity,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool started() const noexcept{return started_;}
    bool retained() const noexcept{return retained_;}
    const std::int32_t& material00() const noexcept{return material00_;}
    const OriginalStandardMaterialTable50LA* table382B38() const noexcept{return table_.get();}
    // The proven299AC0 material-release segment only. Idempotence is native
    // lifetime bookkeeping, not a second source destructor call. This does
    // not perform renderer synchronization or entity1413E0 destruction.
    void release299AC0() noexcept;
private:
    std::shared_ptr<OriginalStandardMaterialTable50LA> table_;
    std::int32_t material00_=-1; //299AA0 constructor store.
    bool started_=false,retained_=false;
};
}
