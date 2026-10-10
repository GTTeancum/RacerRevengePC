#pragma once
// Test-only bit-preserving relocation and accounting. Source fixture addresses
// remain opaque words: this loader never dereferences a guest address.
#include "group_shape48t.h"
#include "json.hpp"
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>
#include <string>
#include <vector>
#if defined(_MSC_VER)
#define RR48T_NOINLINE __declspec(noinline)
#else
#define RR48T_NOINLINE __attribute__((noinline))
#endif
namespace shape_test48t {
using namespace rr::game;
using Json=nlohmann::json;
inline bool watching=false;
inline std::size_t allocationKinds[4]{};
inline std::size_t checks=0,failures=0,differential=0,safety=0,overlaps=0;
inline std::string label;
inline void check(bool ok,const char* why){++checks;if(!ok){++failures;if(failures<100)std::cerr<<label<<": "<<why<<'\n';}}
inline std::size_t allocations(){std::size_t n=0;for(auto x:allocationKinds)n+=x;return n;}
inline void* allocate(std::size_t n,unsigned kind,std::size_t alignment=0){if(watching)++allocationKinds[kind];void* p=alignment?std::aligned_alloc(alignment,((n?n:1)+alignment-1)/alignment*alignment):std::malloc(n?n:1);if(p)return p;throw std::bad_alloc();}
inline float fp(std::uint32_t w){float f;std::memcpy(&f,&w,4);return f;}
inline std::uint32_t bits(float f){std::uint32_t w;std::memcpy(&w,&f,4);return w;}
template<class T>inline void fill(std::vector<T>& a,unsigned char c){if(!a.empty())std::memset(static_cast<void*>(a.data()),c,a.size()*sizeof(T));}
inline void matrix(BodyMatrix48G& m,const Json& words,std::size_t start){for(unsigned i=0;i<16;++i)m.v[i]=fp(words.at(start+i).get<std::uint32_t>());}
inline GroupShapeState48T state(const Json& words){GroupShapeState48T s{};matrix(s.composed00,words,0);matrix(s.world80,words,32);matrix(s.previousC0,words,48);return s;}
inline GroupShapeGeometry48T geometry(const Json& words){GroupShapeGeometry48T g{};for(unsigned k=0;k<3;++k){g.localCenter.v[k]=fp(words.at(3+k).get<std::uint32_t>());g.localHalf.v[k]=fp(words.at(6+k).get<std::uint32_t>());}return g;}
inline void project(std::vector<std::uint32_t>& words,const GroupShapeState48T& s){for(unsigned i=0;i<16;++i){words[i]=bits(s.composed00.v[i]);words[32+i]=bits(s.world80.v[i]);words[48+i]=bits(s.previousC0.v[i]);}}
struct Snapshot {
 const void* pointer;std::vector<unsigned char> bytes;
 Snapshot(const void* p,std::size_t n):pointer(p),bytes(n){if(n)std::memcpy(bytes.data(),p,n);}
 void verify()const{check(bytes.empty()||!std::memcmp(pointer,bytes.data(),bytes.size()),"entire source/destination/result byte span preserved");}
};
template<class T>inline void snap(std::vector<Snapshot>& s,const std::vector<T>& a){s.emplace_back(a.data(),a.size()*sizeof(T));}
template<class T>inline void tail(const std::vector<T>& a,std::size_t begin,unsigned char canary){const auto* p=reinterpret_cast<const unsigned char*>(a.data());for(std::size_t i=begin*sizeof(T);i<a.size()*sizeof(T);++i)check(p[i]==canary,"complete inactive capacity tail unchanged");}
inline Json accounting(){return {{"checks",checks},{"failures",failures},{"differential_cases",differential},{"native_safety_cases",safety},{"declared_span_overlap_cases",overlaps},{"observed_allocations",allocations()},{"ordinary_new",allocationKinds[0]},{"array_new",allocationKinds[1]},{"aligned_new",allocationKinds[2]},{"aligned_array_new",allocationKinds[3]}};}
}
RR48T_NOINLINE void* operator new(std::size_t n){return shape_test48t::allocate(n,0);}
RR48T_NOINLINE void* operator new[](std::size_t n){return shape_test48t::allocate(n,1);}
RR48T_NOINLINE void* operator new(std::size_t n,std::align_val_t a){return shape_test48t::allocate(n,2,static_cast<std::size_t>(a));}
RR48T_NOINLINE void* operator new[](std::size_t n,std::align_val_t a){return shape_test48t::allocate(n,3,static_cast<std::size_t>(a));}
RR48T_NOINLINE void operator delete(void* p)noexcept{std::free(p);}
RR48T_NOINLINE void operator delete[](void* p)noexcept{std::free(p);}
RR48T_NOINLINE void operator delete(void* p,std::size_t)noexcept{std::free(p);}
RR48T_NOINLINE void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
RR48T_NOINLINE void operator delete(void* p,std::align_val_t)noexcept{std::free(p);}
RR48T_NOINLINE void operator delete[](void* p,std::align_val_t)noexcept{std::free(p);}
RR48T_NOINLINE void operator delete(void* p,std::size_t,std::align_val_t)noexcept{std::free(p);}
RR48T_NOINLINE void operator delete[](void* p,std::size_t,std::align_val_t)noexcept{std::free(p);}
