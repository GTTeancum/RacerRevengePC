#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

namespace rr::assets {
enum class OriginalStandardProgramStatus50LA : std::uint8_t {
    complete,unprepared,alreadyPrepared,input,missingName,range
};
struct OriginalStandardProgramDiagnostic50LA {
    OriginalStandardProgramStatus50LA status=OriginalStandardProgramStatus50LA::complete;
    std::uint32_t sourcePC=0;
    std::int16_t row=-1;
};
struct OriginalStandardProgramRowView50LA {
    std::int16_t index=-1;
    const char* canonicalName=nullptr;
    std::uint32_t sourceRegistrationPC=0,sourceNameVA=0;
};
struct OriginalStandardProgramParameterRowView50LA {
    std::int16_t index=-1;
    const char* canonicalName=nullptr;
    std::int32_t value=0;
    std::uint32_t sourceRegistrationPC=0,sourceNameVA=0;
};
// Native canonical name/index subowner for the FRESH original382B70 registry.
// Original3C8C10 constructs singleton433310 through1505C0, which starts an
// empty298490 table and appends76 names through298900 in fixed source order.
// The source's immutable program payload entries are intentionally not copied
// or represented by invented pointers. This is not a program-object registry,
// native program lowering, GPU readiness receipt, or execution mechanism.
//
// Actual247110 only looks up the name and stores this signed16 row at pass+18;
// it neither retains nor dereferences a program object. A material owner must
// pin this exact shared application subowner for its construction lifecycle.
// The same original3C8C10 initializer also constructs companion382B78 with
// exactly five scalar parameter rows. prepareFresh owns both known fresh
// name domains together; absent-parameter admission must inspect these rows,
// not assume that an arbitrary live parameter registry has no extra names.
// No arbitrary registration, name-table reload, or adoption of a live foreign
// registry is exposed. Default construction remains UNPREPARED.
class OriginalStandardProgramTable50LA final {
public:
    OriginalStandardProgramTable50LA() noexcept=default;
    OriginalStandardProgramTable50LA(const OriginalStandardProgramTable50LA&)=delete;
    OriginalStandardProgramTable50LA& operator=(const OriginalStandardProgramTable50LA&)=delete;
    bool prepareFresh(OriginalStandardProgramDiagnostic50LA&) noexcept;
    bool prepared() const noexcept {return prepared_;}
    std::size_t rowCount() const noexcept;
    // Observations expose semantic names and provenance only. Failure leaves
    // the output unchanged. Source PCs/VAs are metadata, never host addresses.
    bool row(std::int32_t,OriginalStandardProgramRowView50LA&) const noexcept;
    std::size_t parameterRowCount382B78() const noexcept;
    bool parameterRow382B78(std::int32_t,
        OriginalStandardProgramParameterRowView50LA&) const noexcept;
    // Genuine298C80 first-match ASCII case-insensitive lookup. No separator
    // conversion or UI-program enum ordinals. Missing names never become0.
    // Inputs outside the bounded ASCII name domain reject without mutation.
    bool find298C80(const std::string&,std::int16_t&,
        OriginalStandardProgramDiagnostic50LA&) const noexcept;
private:
    bool prepared_=false;
};
}
