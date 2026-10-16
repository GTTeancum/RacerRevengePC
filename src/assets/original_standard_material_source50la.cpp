#include "original_standard_material_source50la.h"
#include <utility>

namespace rr::assets {
namespace {
char lower(char c) noexcept {return c>='A'&&c<='Z'?char(c+('a'-'A')):c;}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)if(lower(a[i])!=lower(b[i]))return false;
    return true;
}
bool space(char c) noexcept {return c==' '||c=='\t'||c=='\r';}
bool fail(OriginalUiPageDiagnostic50KG& d,OriginalUiPageError50KG e,
    std::size_t at=0,std::uint32_t pc=0) noexcept {d={e,at,pc};return false;}
const OriginalUiDatSection50KG* findSection(const std::vector<OriginalUiDatSection50KG>& sections,
    const std::string& name) noexcept {
    for(const auto& section:sections)if(same(section.name,name))return &section;
    return nullptr;
}
}
struct OriginalStandardMaterialSource50LA::Storage {
    std::string member;
    std::vector<std::uint8_t> bytes;
    std::vector<OriginalUiDatSection50KG> sections;
};
OriginalStandardMaterialSource50LA::OriginalStandardMaterialSource50LA() noexcept=default;
OriginalStandardMaterialSource50LA::~OriginalStandardMaterialSource50LA()=default;
bool OriginalStandardMaterialSource50LA::prepared() const noexcept {return bool(storage_);}
const std::string* OriginalStandardMaterialSource50LA::member() const noexcept {return storage_?&storage_->member:nullptr;}
const std::vector<std::uint8_t>* OriginalStandardMaterialSource50LA::originalBytes() const noexcept {return storage_?&storage_->bytes:nullptr;}
const std::vector<OriginalUiDatSection50KG>* OriginalStandardMaterialSource50LA::sections() const noexcept {return storage_?&storage_->sections:nullptr;}
const OriginalUiDatSection50KG* OriginalStandardMaterialSource50LA::section(const std::string& name) const noexcept {
    return storage_?findSection(storage_->sections,name):nullptr;
}
// The native lexical subset is deliberately the existing original_ui_pages50kg
// source157548/158824/159440 parser, with first-present repeated keys. Only its
// authenticated-UI input wrapper is replaced; this owner retains gameplay bytes
// and never assigns a UI identity. Numeric getters reuse that same native code.
bool OriginalStandardMaterialSource50LA::prepare(const std::string& name,
    const std::vector<std::uint8_t>& bytes,OriginalUiPageDiagnostic50KG& d) noexcept {
    d={};
    if(storage_)return fail(d,OriginalUiPageError50KG::unsupportedValue,0,0x29B550);
    if(name.empty()||name.size()>63||bytes.empty())
        return fail(d,OriginalUiPageError50KG::missingMember,0,0x29B628);
    for(unsigned char c:name)if(c<32||c>=127||c=='/')
        return fail(d,OriginalUiPageError50KG::unsupportedValue,0,0x29B5F8);
    if(name.size()<4||name[name.size()-4]!='.'||lower(name[name.size()-3])!='p'||
       lower(name[name.size()-2])!='s'||lower(name[name.size()-1])!='m')
        return fail(d,OriginalUiPageError50KG::unsupportedValue,0,0x29B5F8);
    if(bytes.size()>originalUiDatByteLimit50KG)return fail(d,OriginalUiPageError50KG::capacity);
    const auto* p=bytes.data();
    std::size_t size=bytes.size();
    if (!p[size-1]) --size;
    for (std::size_t i=0;i<size;++i)
        if ((p[i]<32 && p[i]!=9 && p[i]!=10 && p[i]!=13) || p[i]>126)
            return fail(d,OriginalUiPageError50KG::unsupportedEncoding,i);
    try {
        auto next=std::make_unique<Storage>();auto& result=*next;
        result.member=name;result.bytes=bytes;
        std::size_t at=0, keyCount=0;
        while (at<size) {
            const std::size_t lineStart=at;
            while (at<size && p[at]!='\n') ++at;
            auto lineEnd=at;
            if (at<size) {
                ++at;
                // File::ReadLine 12F224..244 normalizes CRLF to LF before
                // cleaning. GetString's newline delimiter removes that LF,
                // even when an authored value has no closing quote.
                if (lineEnd>lineStart && p[lineEnd-1]=='\r') --lineEnd;
            }
            if (lineEnd-lineStart>2048) return fail(d,OriginalUiPageError50KG::capacity,lineStart);
            std::size_t first=lineStart;
            while (first<lineEnd && space(char(p[first]))) ++first;
            if (first==lineEnd || p[first]==';') continue;
            // Keep an offset for every retained character so diagnostics still
            // name bytes in the authenticated member after source cleaning.
            std::string line;std::vector<std::size_t> offsets;
            const bool sectionHeader=p[first]=='[';
            bool quoted=false;int brackets=0;
            for (std::size_t i=lineStart;i<lineEnd;++i) {
                const char c=char(p[i]);
                // 0x157548..74 extracts the name through the first ']'. Admit
                // its authored trailing comment only, before comment quotes or
                // brackets can affect syntax; never strip a value semicolon.
                if (sectionHeader && c==';' && !quoted && brackets==0 &&
                    !line.empty() && line.back()==']') break;
                if (c=='"') quoted=!quoted;
                else if (c=='[') ++brackets;
                else if (c==']') --brackets;
                if (brackets<0 || brackets>16) return fail(d,OriginalUiPageError50KG::textSyntax,i,0x159440);
                if (space(c) && !quoted && brackets==0) continue;
                line.push_back(c);offsets.push_back(i);
            }
            if (line.empty() || line[0]==';') continue;
            // 159440 tracks quotes only to preserve whitespace within this
            // line; it does not require a closing quote. GetString 158824
            // and 158858 strip leading/trailing quotes independently. Actual
            // Options Widgets311/251 author opening-only Text quotes.
            if (brackets!=0) return fail(d,OriginalUiPageError50KG::textSyntax,lineStart,0x159440);
            if (line[0]=='[') {
                if (line.size()<3 || line.back()!=']') return fail(d,OriginalUiPageError50KG::textSyntax,lineStart);
                const std::string sectionName=line.substr(1,line.size()-2);
                if (sectionName.size()>127 || sectionName.find_first_of("[]=\"")!=std::string::npos)
                    return fail(d,OriginalUiPageError50KG::textSyntax,offsets[1]);
                if (findSection(result.sections,sectionName)) return fail(d,OriginalUiPageError50KG::duplicateSection,offsets[1]);
                if (result.sections.size()==512) return fail(d,OriginalUiPageError50KG::capacity,lineStart);
                result.sections.push_back({sectionName,offsets[1],{}});continue;
            }
            if (result.sections.empty()) return fail(d,OriginalUiPageError50KG::textSyntax,lineStart);
            const auto eq=line.find('=');
            if (eq==std::string::npos || eq==0 || eq>127)
                return fail(d,OriginalUiPageError50KG::textSyntax,lineStart);
            std::string key=line.substr(0,eq);
            if (key.find_first_of("[];\"")!=std::string::npos)
                return fail(d,OriginalUiPageError50KG::textSyntax,offsets[0]);
            const bool present=eq+1<line.size();
            std::size_t va=eq+1,vb=line.size();
            if (va<vb && line[va]=='"') ++va;
            if (va<vb && line[vb-1]=='"') --vb;
            auto& section=result.sections.back();
            if (++keyCount>8192) return fail(d,OriginalUiPageError50KG::capacity,offsets[0]);
            const auto valueOffset=va<offsets.size()?offsets[va]:offsets[eq]+1;
            section.values.push_back({std::move(key),line.substr(va,vb-va),offsets[0],valueOffset,present});
        }
        if (result.sections.empty()) return fail(d,OriginalUiPageError50KG::textSyntax);
        storage_=std::move(next);d={};return true;
    } catch (...) { return fail(d,OriginalUiPageError50KG::allocation); }
}
}
