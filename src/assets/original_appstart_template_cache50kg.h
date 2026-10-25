#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr::assets {
struct OriginalUiMemberIdentity50KG;
class OriginalStandardTextureResolver50LA;
class OriginalUiMaterialTemplates50KG;

// 159440's actual line transformation. The input is the just-returned
// 12EE90 buffer, not an immutable archive member. CRLF normalization has
// already happened; all characters preceding its terminal LF keep their
// original byte positions. Stop at NUL as strlen159468 does. Unbalanced
// quotes/brackets are not a syntax error in this source function.
inline bool originalAppStartCleanLine159440(const char* input,std::size_t bytes,
    std::size_t memberOffset,std::string& cleaned,std::vector<std::size_t>& offsets)noexcept {
    if(!input&&bytes)return false;
    try {
        cleaned.clear();offsets.clear();
        bool quoted=false;std::int64_t brackets=0;
        for(std::size_t i=0;i<bytes&&input[i];++i){
            const char c=input[i];
            if(c=='"')quoted=!quoted;
            else if(c=='[')++brackets;
            else if(c==']')--brackets;
            const bool whitespace=c=='\r'||c=='\n'||c=='\t'||c==' ';
            if(whitespace&&brackets<=0&&!quoted)continue;
            cleaned.push_back(c);offsets.push_back(memberOffset+i);
        }
        return true;
    }catch(...){return false;}
}

// Actual temporary Config157320 state for the two bounded APPSTART text
// consumers. Only the same resolver can fill it, and only the template
// consumer can use its getters. No public raw-byte admission, generic file
// handle, or readiness/receipt API exists. The index and selected-section
// cache survive until AFTER the explicit 1573B0 member close.
class OriginalAppStartTemplateCache50KG final {
    friend class OriginalStandardTextureResolver50LA;
    friend class OriginalUiMaterialTemplates50KG;
    struct Line {std::string text;std::vector<std::size_t> offsets;};
    struct Section {std::string name;std::size_t bodyOffset=0,physicalLines=0;};
    const OriginalUiMemberIdentity50KG* identity=nullptr;
    // Reverse member destruction releases the selected cache before index,
    // matching the 157CB0 then section-table frees after source member close.
    std::vector<Section> sections;
    std::vector<Line> lines;
    bool sectionFound=false;
    // maximumBytes includes the NUL, or size_t(-1) for source a3=-1.
    bool getString158240(const char*,std::string&,std::size_t,std::size_t&)const;
    // False means no present matching key. Unsupported numeric range is a
    // separate native safety rejection, never a fabricated missing default.
    bool getInteger158920(const char*,std::int32_t&,std::size_t&,bool&)const;
    OriginalAppStartTemplateCache50KG()=default;
    OriginalAppStartTemplateCache50KG(const OriginalAppStartTemplateCache50KG&)=delete;
    OriginalAppStartTemplateCache50KG& operator=(const OriginalAppStartTemplateCache50KG&)=delete;
};
}
