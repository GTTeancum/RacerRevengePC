#include "original_ui_pages50kg.h"
#include "pod_physics49n.h"
#include <limits>
#include <utility>

namespace rr::assets {
namespace {
char lower(char c) noexcept { return c >= 'A' && c <= 'Z' ? char(c + 'a' - 'A') : c; }
bool same(const std::string& a, const std::string& b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i=0;i<a.size();++i) if (lower(a[i]) != lower(b[i])) return false;
    return true;
}
bool space(char c) noexcept { return c==' ' || c=='\t' || c=='\r'; }
bool fail(OriginalUiPageDiagnostic50KG& d, OriginalUiPageError50KG e,
    std::size_t at=0, std::uint32_t pc=0) noexcept { d={e,at,pc}; return false; }
bool integer(const std::string& text, std::int32_t& out) noexcept {
    if (text.empty()) return false;
    std::size_t at=0;
    const bool neg=text[0]=='-';
    if (neg || text[0]=='+') ++at;
    if (at==text.size()) return false;
    const std::uint64_t limit=neg ? 2147483648ull : 2147483647ull;
    std::uint64_t n=0;
    for (;at<text.size();++at) {
        if (text[at]<'0' || text[at]>'9') return false;
        const unsigned digit=unsigned(text[at]-'0');
        if (n>(limit-digit)/10) return false;
        n=n*10+digit;
    }
    out=neg ? static_cast<std::int32_t>(-static_cast<std::int64_t>(n))
            : static_cast<std::int32_t>(n);
    return true;
}
// Source DAT numbers admit an omitted integral zero (.6, -.25). This
// bounded lexical expansion retains the existing exact decimal arithmetic;
// source member bytes and offsets are never changed.
bool number(const char* text, std::size_t bytes, float& out) noexcept {
    if (!text || !bytes) return false;
    const std::size_t sign=(text[0]=='-' || text[0]=='+') ? 1u : 0u;
    if (sign<bytes && text[sign]=='.') {
        if (bytes<=sign+1 || text[sign+1]<'0' || text[sign+1]>'9' || bytes>10) return false;
        char expanded[12]{};
        if (sign) expanded[0]=text[0];
        expanded[sign]='0';
        for (std::size_t i=sign;i<bytes;++i) expanded[i+1]=text[i];
        return parsePodPhysicsNumber49N(expanded,bytes+1,out);
    }
    return parsePodPhysicsNumber49N(text,bytes,out);
}
bool count(const OriginalUiDatSection50KG& s, const char* key,
    std::size_t limit, std::int32_t& out, OriginalUiPageDiagnostic50KG& d,
    std::uint32_t pc) noexcept {
    std::int32_t n=0;
    if (!originalUiInteger50KG(s,key,0,n,d,pc)) return false;
    if (n<0 || std::uint64_t(n)>limit) {
        const auto* v=s.find(key);
        return fail(d,OriginalUiPageError50KG::capacity,v?v->valueOffset:s.sourceOffset,pc);
    }
    out=n;return true;
}
std::size_t offset(const OriginalUiDatSection50KG& s, const std::string& key) noexcept {
    const auto* v=s.find(key);return v?v->valueOffset:s.sourceOffset;
}
bool vector3(const OriginalUiDatValue50KG& value, std::array<float,3>& out,
    OriginalUiPageDiagnostic50KG& d, std::uint32_t pc) noexcept {
    const auto& s=value.text; std::array<float,3> v{};std::size_t at=0;
    for (unsigned i=0;i<3;++i) {
        std::size_t end=at;while (end<s.size() && s[end]!=',') ++end;
        if ((i<2 && end==s.size()) || (i==2 && end!=s.size()))
            return fail(d,OriginalUiPageError50KG::numberSyntax,value.valueOffset+at,pc);
        std::size_t first=at,last=end;
        while (first<last && space(s[first])) ++first;
        while (last>first && space(s[last-1])) --last;
        if (!number(s.data()+first,last-first,v[i]))
            return fail(d,OriginalUiPageError50KG::numberSyntax,value.valueOffset+first,pc);
        at=end+1;
    }
    out=v;return true;
}
}
const OriginalUiDatValue50KG* OriginalUiDatSection50KG::find(const std::string& key) const noexcept {
    for (const auto& v:values) if (v.sourceValuePresent && same(v.key,key)) return &v;
    return nullptr;
}
const OriginalUiDatSection50KG* OriginalUiDat50KG::section(const std::string& name) const noexcept {
    for (const auto& s:sections_) if (same(s.name,name)) return &s;
    return nullptr;
}
const OriginalUiDatValue50KG* OriginalUiDat50KG::find(const std::string& name,
    const std::string& key) const noexcept {
    const auto* s=section(name);return s?s->find(key):nullptr;
}
bool originalUiInteger50KG(const OriginalUiDatSection50KG& s,const std::string& key,
    std::int32_t fallback,std::int32_t& output,OriginalUiPageDiagnostic50KG& d,
    std::uint32_t pc) noexcept {
    const auto* v=s.find(key);std::int32_t n=fallback;
    if (v && !integer(v->text,n)) return fail(d,OriginalUiPageError50KG::integerSyntax,v->valueOffset,pc);
    output=n;d={};return true;
}
bool originalUiNumber50KG(const OriginalUiDatSection50KG& s,const std::string& key,
    float fallback,float& output,OriginalUiPageDiagnostic50KG& d,std::uint32_t pc) noexcept {
    const auto* v=s.find(key);float n=fallback;
    if (v && !number(v->text.data(),v->text.size(),n))
        return fail(d,OriginalUiPageError50KG::numberSyntax,v->valueOffset,pc);
    output=n;d={};return true;
}
bool originalUiTriple50KG(const OriginalUiDatSection50KG& s,const std::string& key,
    const std::array<float,3>& fallback,std::array<float,3>& output,
    OriginalUiPageDiagnostic50KG& d,std::uint32_t pc) noexcept {
    const auto* v=s.find(key);auto result=fallback;
    if (v && !vector3(*v,result,d,pc)) return false;
    output=result;d={};return true;
}
bool readOriginalUiDat50KG(const OriginalUiArchive50KG& archive,const std::string& name,
    OriginalUiDat50KG& output,OriginalUiPageDiagnostic50KG& d,
    OriginalUiRepeatedKeyPolicy50KG repeatedKeys) noexcept {
    if (!archive.loaded()) return fail(d,OriginalUiPageError50KG::missingArchive);
    const auto member=archive.find(name);
    if (!member.identity || !member.data || !member.size)
        return fail(d,OriginalUiPageError50KG::missingMember);
    if (member.size>originalUiDatByteLimit50KG) return fail(d,OriginalUiPageError50KG::capacity);
    const auto* p=member.data;
    std::size_t size=member.size;
    if (!p[size-1]) --size;
    for (std::size_t i=0;i<size;++i)
        if ((p[i]<32 && p[i]!=9 && p[i]!=10 && p[i]!=13) || p[i]>126)
            return fail(d,OriginalUiPageError50KG::unsupportedEncoding,i);
    try {
        OriginalUiDat50KG result;result.identity_=member.identity;
        std::size_t at=0, keyCount=0;
        while (at<size) {
            const std::size_t lineStart=at;
            while (at<size && p[at]!='\n') ++at;
            const auto lineEnd=at;if (at<size) ++at;
            if (lineEnd-lineStart>2048) return fail(d,OriginalUiPageError50KG::capacity,lineStart);
            std::size_t first=lineStart;
            while (first<lineEnd && space(char(p[first]))) ++first;
            if (first==lineEnd || p[first]==';') continue;
            // Keep an offset for every retained character so diagnostics still
            // name bytes in the authenticated member after source cleaning.
            std::string line;std::vector<std::size_t> offsets;
            bool quoted=false;int brackets=0;
            for (std::size_t i=lineStart;i<lineEnd;++i) {
                const char c=char(p[i]);
                if (c=='"') quoted=!quoted;
                else if (c=='[') ++brackets;
                else if (c==']') --brackets;
                if (brackets<0 || brackets>16) return fail(d,OriginalUiPageError50KG::textSyntax,i,0x159440);
                if (space(c) && !quoted && brackets==0) continue;
                line.push_back(c);offsets.push_back(i);
            }
            if (line.empty() || line[0]==';') continue;
            if (quoted || brackets!=0) return fail(d,OriginalUiPageError50KG::textSyntax,lineStart,0x159440);
            if (line[0]=='[') {
                if (line.size()<3 || line.back()!=']') return fail(d,OriginalUiPageError50KG::textSyntax,lineStart);
                const std::string sectionName=line.substr(1,line.size()-2);
                if (sectionName.size()>127 || sectionName.find_first_of("[]=\"")!=std::string::npos)
                    return fail(d,OriginalUiPageError50KG::textSyntax,offsets[1]);
                if (result.section(sectionName)) return fail(d,OriginalUiPageError50KG::duplicateSection,offsets[1]);
                if (result.sections_.size()==512) return fail(d,OriginalUiPageError50KG::capacity,lineStart);
                result.sections_.push_back({sectionName,offsets[1],{}});continue;
            }
            if (result.sections_.empty()) return fail(d,OriginalUiPageError50KG::textSyntax,lineStart);
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
            auto& section=result.sections_.back();
            if (repeatedKeys==OriginalUiRepeatedKeyPolicy50KG::reject)
                for (const auto& old:section.values) if (same(old.key,key))
                    return fail(d,OriginalUiPageError50KG::duplicateKey,offsets[0]);
            if (++keyCount>8192) return fail(d,OriginalUiPageError50KG::capacity,offsets[0]);
            const auto valueOffset=va<offsets.size()?offsets[va]:offsets[eq]+1;
            section.values.push_back({std::move(key),line.substr(va,vb-va),offsets[0],valueOffset,present});
        }
        if (result.sections_.empty()) return fail(d,OriginalUiPageError50KG::textSyntax);
        output=std::move(result);d={};return true;
    } catch (...) { return fail(d,OriginalUiPageError50KG::allocation); }
}
bool readOriginalUiPage50KG(const OriginalUiDat50KG& page,const OriginalUiDat50KG& widgets,
    OriginalUiPage50KG& output,OriginalUiPageDiagnostic50KG& d) noexcept {
    if (!page.identity() || !widgets.identity()) return fail(d,OriginalUiPageError50KG::missingMember);
    const auto* info=page.section("PageInfo");const auto* general=widgets.section("General");
    if (!info || !general) return fail(d,OriginalUiPageError50KG::missingSection,0,0x1D64CC);
    try {
        OriginalUiPage50KG result;result.identity=page.identity();
        std::int32_t wc=0,cc=0,gc=0,n=0;
        if (!count(*general,"NumberOfWidgets",originalUiWidgetLimit50KG,wc,d,0x2A4404) ||
            !count(*info,"NumberOfControls",originalUiControlLimit50KG-1,cc,d,0x1D6E04) ||
            !count(*info,"NumberOfGroups",originalUiGroupLimit50KG-1,gc,d,0x1D6E30) ||
            !count(*info,"NumberOfStaticWidgets",originalUiWidgetLimit50KG,result.staticWidgetCount,d,0x1D6DEC) ||
            !originalUiInteger50KG(*info,"PageNo",0,result.pageNo,d,0x1D6D94) ||
            !originalUiInteger50KG(*info,"OpenSize",224,result.openSize,d,0x1D64DC) ||
            !originalUiNumber50KG(*info,"Time",1.0f,result.openTime,d,0x1D64F4) ||
            !originalUiInteger50KG(*info,"UseVideoAsTexture",0,n,d,0x1D6D74)) return false;
        result.widgetCount=static_cast<std::uint16_t>(wc);result.useVideoAsTexture=n!=0;
        if (result.openSize<0 || result.openSize>224 || result.openTime<=0.0f)
            return fail(d,OriginalUiPageError50KG::unsupportedValue,info->sourceOffset,0x1D67E4);
        if (const auto* v=info->find("Background")) result.background=v->text;
        if (const auto* v=info->find("Start")) result.start=v->text;
        if (const auto* v=info->find("BackgroundColor"))
            if (!vector3(*v,result.backgroundColor,d,0x1D6D1C)) return false;
        if (!originalUiInteger50KG(*info,"HasPopup",0,n,d,0x1D6570)) return false;
        result.hasPopup=n!=0;
        constexpr const char* keys[]={"PopupConfirmWidget","PopupFormatWidget","PopupFormatSuccessWidget",
            "PopupFormatFailWidget","PopupYesWidget","PopupNoWidget","PopupAskFormatWidget",
            "PopupNoMemcardWidget","PopupFullMemcardWidget"};
        if (result.hasPopup) for (std::size_t i=0;i<result.popupWidgets.size();++i) {
            if (!originalUiInteger50KG(*info,keys[i],-1,result.popupWidgets[i],d,0x1D6598+std::uint32_t(i*24))) return false;
            if (result.popupWidgets[i]<-1 || result.popupWidgets[i]>wc)
                return fail(d,OriginalUiPageError50KG::reference,offset(*info,keys[i]),0x1D6698);
        }
        result.controls.resize(std::size_t(cc)+1);result.groups.resize(std::size_t(gc)+1);
        std::size_t memberships=0;
        for (std::size_t i=0;i<result.controls.size();++i) {
            const auto* section=page.section("Control "+std::to_string(i));
            if (!section) continue;
            auto& control=result.controls[i];control.present=true;
            if (!count(*section,"NumberOfWidgets",originalUiMembershipLimit50KG,n,d,0x1D71FC)) return false;
            memberships+=std::size_t(n);
            if (memberships>originalUiMembershipLimit50KG) return fail(d,OriginalUiPageError50KG::capacity,section->sourceOffset);
            for (std::int32_t j=1;j<=n;++j) {
                const auto key="WidgetID"+std::to_string(j);std::int32_t id=0;
                if (!originalUiInteger50KG(*section,key,0,id,d,0x1D7254)) return false;
                if (id<1 || id>wc) return fail(d,OriginalUiPageError50KG::reference,offset(*section,key),0x1D7284);
                control.widgets.push_back(static_cast<std::uint16_t>(id));
            }
        }
        for (std::size_t i=0;i<result.groups.size();++i) {
            const auto* section=page.section("Group "+std::to_string(i));
            if (!section) continue; // Original missing section yields zero memberships/default KeepEnabled.
            auto& group=result.groups[i];
            if (!count(*section,"NumberOfControls",originalUiMembershipLimit50KG,n,d,0x1D6F6C)) return false;
            memberships+=std::size_t(n);
            if (memberships>originalUiMembershipLimit50KG) return fail(d,OriginalUiPageError50KG::capacity,section->sourceOffset);
            for (std::int32_t j=1;j<=n;++j) {
                const auto key="ControlID"+std::to_string(j);std::int32_t id=-1;
                if (!originalUiInteger50KG(*section,key,-1,id,d,0x1D6FC0)) return false;
                if (id<-1 || id>cc) return fail(d,OriginalUiPageError50KG::reference,offset(*section,key),0x1D6FFC);
                group.controls.push_back(static_cast<std::int16_t>(id));
            }
            if (!count(*section,"NumberOfAssociations",originalUiMembershipLimit50KG,n,d,0x1D7060)) return false;
            memberships+=std::size_t(n);
            if (memberships>originalUiMembershipLimit50KG) return fail(d,OriginalUiPageError50KG::capacity,section->sourceOffset);
            for (std::int32_t j=1;j<=n;++j) {
                const auto key="AssociationID"+std::to_string(j);std::int32_t id=0;
                if (!originalUiInteger50KG(*section,key,0,id,d,0x1D70B4)) return false;
                if (id<0 || id>cc) return fail(d,OriginalUiPageError50KG::reference,offset(*section,key),0x1D70E8);
                group.associations.push_back(static_cast<std::uint16_t>(id));
            }
            if (!originalUiInteger50KG(*section,"KeepEnabled",1,n,d,0x1D7110)) return false;
            group.keepEnabled=n!=0;
        }
        output=std::move(result);d={};return true;
    } catch (...) { return fail(d,OriginalUiPageError50KG::allocation); }
}
bool readOriginalUiPageIndex50KG(const OriginalUiDat50KG& ui,OriginalUiPageIndex50KG& output,
    OriginalUiPageDiagnostic50KG& d) noexcept {
    if (!ui.identity()) return fail(d,OriginalUiPageError50KG::missingMember);
    const auto* base=ui.section("UIBase");
    if (!base) return fail(d,OriginalUiPageError50KG::missingSection,0,0x2A4168);
    try {
        OriginalUiPageIndex50KG result;result.identity=ui.identity();std::int32_t n=0;
        if (!count(*base,"TotalPages",64,n,d,0x2A417C)) return false;
        for (std::int32_t i=1;i<=n;++i) {
            const auto* section=ui.section("Page "+std::to_string(i));
            if (!section) return fail(d,OriginalUiPageError50KG::missingSection,base->sourceOffset,0x2A41B8);
            const auto* value=section->find("PageFile");
            if (!value || value->text.empty()) return fail(d,OriginalUiPageError50KG::missingKey,section->sourceOffset,0x2A41D0);
            if (value->text.size()>255) return fail(d,OriginalUiPageError50KG::capacity,value->valueOffset,0x2A41D0);
            result.pageFiles.push_back(value->text);
        }
        output=std::move(result);d={};return true;
    } catch (...) { return fail(d,OriginalUiPageError50KG::allocation); }
}
}
