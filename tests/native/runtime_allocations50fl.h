#pragma once
// Development-client allocation instrumentation, independent of test/oracle
// fixtures. Counts only ordinary C++ allocations on the calling thread, exactly
// as the preceding client counter did; it does not count drivers, GPU or malloc.
// Include in the executable's single main translation unit only.
#include "runtime_allocation_counter50fl.h"
#include <cstdlib>
#include <new>
#if defined(_MSC_VER)
#define RR_ALLOCATION_NOINLINE50FL __declspec(noinline)
#else
#define RR_ALLOCATION_NOINLINE50FL [[gnu::noinline]]
#endif
RR_ALLOCATION_NOINLINE50FL void* operator new(std::size_t n){
    void* p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;
}
RR_ALLOCATION_NOINLINE50FL void* operator new[](std::size_t n){return ::operator new(n);}
RR_ALLOCATION_NOINLINE50FL void operator delete(void* p)noexcept{std::free(p);}
RR_ALLOCATION_NOINLINE50FL void operator delete[](void* p)noexcept{std::free(p);}
RR_ALLOCATION_NOINLINE50FL void operator delete(void* p,std::size_t)noexcept{std::free(p);}
RR_ALLOCATION_NOINLINE50FL void operator delete[](void* p,std::size_t)noexcept{std::free(p);}

#undef RR_ALLOCATION_NOINLINE50FL
