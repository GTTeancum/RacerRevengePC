#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr::assets {
class OriginalStandardMaterialObject50LA;
// Actual process-static382FC0 ->435690 name manager (299130/299190).
// Serial canonical construction only. Passes store borrowed numeric indices;
// 2475AC calls299330(name,0), without298A30 or per-pass release.
class OriginalShaderPassGroups50LA final {
    friend class OriginalStandardMaterialObject50LA;
    std::vector<std::string> names_;
    OriginalShaderPassGroups50LA()=default;
    static OriginalShaderPassGroups50LA& instance() {
        static OriginalShaderPassGroups50LA owner;
        return owner;
    }
    static char fold(char c)noexcept{return c>='A'&&c<='Z'?char(c+('a'-'A')):c;}
    bool acquire299330(const std::string& name,std::int16_t& index) {
        // Existing material string parser restricts this to printable ASCII.
        for(std::size_t i=0;i<names_.size();++i){
            const auto& prior=names_[i];
            if(prior.size()!=name.size())continue;
            std::size_t j=0;for(;j<name.size()&&fold(prior[j])==fold(name[j]);++j){}
            if(j==name.size()){index=static_cast<std::int16_t>(i);return true;}
        }
        // Source stores the returned index through signed SH at2474F8.
        // Refuse unrepresentable growth rather than alias a negative sentinel.
        if(names_.size()>=32768u)return false;
        if(names_.size()==names_.capacity())names_.reserve(names_.size()+16u);
        names_.push_back(name); // Native owned name; source value/ref remain zero.
        index=static_cast<std::int16_t>(names_.size()-1u);return true;
    }
public:
    static bool contains(std::int16_t index) noexcept {
        return index>=0&&static_cast<std::size_t>(index)<instance().names_.size();
    }
};
}
