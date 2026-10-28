#pragma once
#include <memory>
#include <type_traits>
#include <utility>

namespace interactive_frame50DU {
// A synchronous, non-owning call view. Only lvalues can bind: the original
// callable remains in runSession, before the FrameContext and its thin adapter.
// This view allocates nothing, copies no capture state, and never retains a
// callback supplied to an invocation. Exceptions propagate without translation.
template<class Signature> class BorrowedCall;
template<class R,class... Args> class BorrowedCall<R(Args...)> final {
    const void* object_;
    R (*invoke_)(const void*,Args...);
public:
    template<class F> explicit BorrowedCall(F& function) noexcept
        :object_(std::addressof(function)),
         invoke_([](const void* object,Args... args)->R {
             return (*static_cast<const F*>(object))(std::forward<Args>(args)...);
         }) {
        static_assert(std::is_same_v<R,std::invoke_result_t<const F&,Args...>>,
            "frame callback view must retain the exact result type");
    }
    R operator()(Args... args)const {
        return invoke_(object_,std::forward<Args>(args)...);
    }
};
// Explicit nonthrowing view for original noexcept callbacks. Keep that call
// contract rather than broadening the exception specification at the seam.
template<class R,class... Args> class BorrowedCall<R(Args...) noexcept> final {
    const void* object_;
    R (*invoke_)(const void*,Args...) noexcept;
public:
    template<class F> explicit BorrowedCall(F& function) noexcept
        :object_(std::addressof(function)),
         invoke_([](const void* object,Args... args) noexcept ->R {
             return (*static_cast<const F*>(object))(std::forward<Args>(args)...);
         }) {
        static_assert(std::is_same_v<R,std::invoke_result_t<const F&,Args...>>);
        static_assert(std::is_nothrow_invocable_v<const F&,Args...>);
    }
    R operator()(Args... args)const noexcept {
        return invoke_(object_,std::forward<Args>(args)...);
    }
};
} // namespace interactive_frame50DU
