#pragma once
#include "terrain_interactive_frame50du.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace interactive_frame50DU {
// Stack-only replacement for advance's original generic runNative closure.
// No callback is retained. The phase body completes synchronously against the
// same Context, and a throwing operation still leaves nativeCalls untouched.
struct NativeCalls final {
    Context& context;
    template<class Operation> void operator()(Operation&& operation,const char* label) const {
        const bool ok=operation();
        context.recordNativeResult(ok,label);
    }
};
namespace detail {
inline void require50DU(bool ok,const char* message) {
    if (!ok) throw std::runtime_error(message);
}
inline void addCounter50DU(std::uint64_t& counter,std::uint64_t amount=1) noexcept {
    const auto room=std::numeric_limits<std::uint64_t>::max()-counter;
    counter+=std::min(room,amount);
}
} // namespace detail
} // namespace interactive_frame50DU
