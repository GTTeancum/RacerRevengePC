#pragma once
// Common rewrite: the ONE original labelled-parameter field lookup and value rules, shared by
// every parameter reader (SCB, TOB, AUB, TUN, pod INIs decoded through decodeParameters48F).
//
// 158240 -> 15837C..1585B0 (1583AC): the original finds a field by a midpoint search over the
// block's sorted label table, then probes both endpoints. With duplicate labels (decoded with
// WorldLimits48F::retainDuplicateLabels) the copy it lands on is NOT necessarily the first:
// GC SoultreeSound15 (three copies of each field) and MC's SCB [Environment] ColorRGB resolve
// to a later copy. Every reader that looks up a field uses packedField1583AC.
//
// 158920 (integer/boolean read used by 158B30): "T*"/"F*" (first character, any case) -> 1/0,
// "ON"/"OFF" (any case) -> 1/0, "Y*"/"N*" -> 1/0, otherwise strtol(text, end, 0). A present
// field always yields a value; only an absent field leaves the caller's default (158B30).
#include "import_world48f.h"
#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>

namespace rr::assets {
inline const ParameterField48F* packedField1583AC(const ParameterBlock48F& b,std::string_view name)noexcept {
    if(b.fields.empty())return nullptr;
    if(b.fields.size()==1)return b.fields[0].name==name?&b.fields[0]:nullptr;
    std::size_t low=0,high=b.fields.size()-1,middle=(high-low)/2;
    for(;;){
        const int comparison=name.compare(b.fields[middle].name);
        if(!comparison)return &b.fields[middle];
        if(comparison>=0)low=middle;else high=middle;
        if(high-low<2)break;
        middle=low+(high-low)/2;
    }
    if(b.fields[low].name==name)return &b.fields[low];
    if(b.fields[high].name==name)return &b.fields[high];
    return nullptr;
}
inline const ParameterField48F* packedField1583AC(const ParameterBlock48F* b,std::string_view name)noexcept {
    return b?packedField1583AC(*b,name):nullptr;
}
namespace original_parameter_lookup50CR_detail {
inline char upper(char c)noexcept{return c>='a'&&c<='z'?static_cast<char>(c-'a'+'A'):c;}
inline bool sameFolded(const std::string& a,const char* b)noexcept{
    std::size_t i=0;for(;b[i];++i)if(i>=a.size()||upper(a[i])!=upper(b[i]))return false;return i==a.size();
}
}
inline std::int32_t integer158920(const std::string& t)noexcept {
    namespace d=original_parameter_lookup50CR_detail;
    const char first=t.empty()?'\0':d::upper(t[0]);
    if(first=='T')return 1;
    if(first=='F')return 0;
    if(d::sameFolded(t,"ON"))return 1;
    if(d::sameFolded(t,"OFF"))return 0;
    if(first=='Y')return 1;
    if(first=='N')return 0;
    return static_cast<std::int32_t>(std::strtol(t.c_str(),nullptr,0));
}
// 158B30: the field's 158920 value, or `fallback` when the field is absent.
inline std::int32_t integer158B30(const ParameterBlock48F* b,std::string_view name,std::int32_t fallback)noexcept {
    const auto* f=packedField1583AC(b,name);
    return f?integer158920(f->value):fallback;
}
inline const ParameterBlock48F* findBlock157D00(const ParameterDocument48F& p,std::string_view name)noexcept {
    for(const auto& b:p.blocks)if(b.name==name)return &b;
    return nullptr;
}
} // namespace rr::assets
