#include "body_contact_list48r.h"
#include <cstddef>
#include <type_traits>

namespace rr::game {
static_assert(sizeof(BodyContactIdentity48R)==4,"Identity must preserve one word");
static_assert(std::is_trivially_copyable<BodyContactListState48R>::value,
    "Contact state is bounded caller-owned storage");
static_assert(offsetof(BodyContactListState48R,slots210)==4&&
    sizeof(BodyContactListState48R)==44,"Count and all ten slots are explicit");
BodyContactListStatus48R applyBodyContactList48R(
    BodyContactIdentity48R other,BodyContactListState48R& state) noexcept {
    const auto count=state.count20C;
    if(count>=static_cast<std::int32_t>(BodyContactCapacity48R))
        return BodyContactListStatus48R::Full;
    // Original signed count<10 is not a sufficient memory-safety condition:
    // negative counts bypass its search and index before or wrap around slots.
    if(count<0)return BodyContactListStatus48R::InvalidInput;
    for(std::int32_t i=0;i<count;++i)
        if(state.slots210[static_cast<std::size_t>(i)].value==other.value)
            return BodyContactListStatus48R::Duplicate;
    state.slots210[static_cast<std::size_t>(count)]=other;
    state.count20C=count+1;
    return BodyContactListStatus48R::Appended;
}
void resetBodyContactList48R(BodyContactListState48R& state) noexcept {
    state.count20C=0;
}
}
