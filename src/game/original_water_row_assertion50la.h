#pragma once
#include <cstdint>
#include <optional>
namespace rr::game {
//Distinct startup BSS pair for299420; never reset by World withdrawal.
struct OriginalWaterRowGlobals50LA {
    std::uint8_t guard3D6644=0;
    std::uint32_t assertion443638=0;
    bool known=true;
};
struct OriginalWaterRowAssertion50LA {
    enum class Status {refused,invalidGlobals,accepted};
    Status status=Status::refused;
    std::uint32_t pc=0;
    std::optional<std::uint8_t> guardBefore;
    std::optional<std::uint32_t> assertionRead,assertionReturn;
    bool assertionStored=false,guardStored=false,assertionCalled=false;
    bool operator==(const OriginalWaterRowAssertion50LA&)const noexcept=default;
};
inline OriginalWaterRowAssertion50LA admitOriginalWaterRowOutput50LA(
    OriginalWaterRowGlobals50LA& globals,const void* output)noexcept {
    using R=OriginalWaterRowAssertion50LA;R result;
    if(!output)return result; //Native failure is not an originalNULL return.
    result.pc=0x299454u;
    if(!globals.known||globals.guard3D6644>1u||globals.assertion443638>1u||
       globals.guard3D6644!=globals.assertion443638){result.status=R::Status::invalidGlobals;return result;}
    result.guardBefore=globals.guard3D6644;
    if(!*result.guardBefore){
        result.pc=0x299468u;globals.assertion443638=1u;result.assertionStored=true;
        result.pc=0x29946cu;globals.guard3D6644=1u;result.guardStored=true;
    }
    result.pc=0x299474u;result.assertionRead=globals.assertion443638;
    //Both debug36C100 arms of133660 return0 for authenticated nonnull storage.
    result.pc=0x29948cu;result.assertionCalled=true;result.assertionReturn=0u;
    result.pc=0x2994bcu;result.status=R::Status::accepted;return result;
}
}
