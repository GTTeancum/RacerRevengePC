#pragma once
// Development-client allocation instrumentation, independent of test/oracle
// fixtures. Counts only ordinary C++ allocations on the calling thread, exactly
// as the preceding client counter did; it does not count drivers, GPU or malloc.
// Include in the executable's single main translation unit only.
#include <cstdint>
#include <cstdlib>
#include <new>
inline thread_local std::uint64_t allocations=0;
[[gnu::noinline]] void* operator new(std::size_t n){
    void* p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;
}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void* p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void* p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void* p,std::size_t)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
