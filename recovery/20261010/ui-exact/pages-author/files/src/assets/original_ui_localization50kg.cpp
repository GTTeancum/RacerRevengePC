#include "original_ui_localization50kg.h"
#include <utility>

namespace rr::assets {
namespace {
std::uint16_t u16(const std::uint8_t* p) noexcept {
    return std::uint16_t(p[0]) | (std::uint16_t(p[1])<<8);
}
std::uint32_t u32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1])<<8) |
        (std::uint32_t(p[2])<<16) | (std::uint32_t(p[3])<<24);
}
bool fail(OriginalUiLocalizationDiagnostic50KG& d,OriginalUiLocalizationError50KG e,
    std::size_t offset=0,std::size_t entry=0,std::uint32_t pc=0) noexcept {
    d={e,offset,entry,pc};return false;
}
}
const OriginalUiLocalizedEntry50KG* OriginalUiEnglish50KG::find(const std::string& name) const noexcept {
    if (loaded()) for (const auto& e:entries_) if (e.name==name) return &e;
    return nullptr;
}
const OriginalUiLocalizedEntry50KG* OriginalUiEnglish50KG::at(std::size_t index) const noexcept {
    return loaded() && index<entries_.size() ? &entries_[index] : nullptr;
}
bool readOriginalUiEnglish50KG(const OriginalUiArchive50KG& archive,
    OriginalUiEnglish50KG& output,OriginalUiLocalizationDiagnostic50KG& d) noexcept {
    if (!archive.loaded()) return fail(d,OriginalUiLocalizationError50KG::missingArchive);
    const auto names=archive.find("loc\\english\\UI.loc");
    const auto texts=archive.find("loc\\english\\L_UI_ENG.bin");
    if (!names.identity || !texts.identity || !names.data || !texts.data)
        return fail(d,OriginalUiLocalizationError50KG::missingMember);
    if (names.size<28 || !texts.size) return fail(d,OriginalUiLocalizationError50KG::header);
    if (names.size>1024*1024 || texts.size>1024*1024)
        return fail(d,OriginalUiLocalizationError50KG::capacity);
    if (u16(names.data+26)!=originalUiLocalizedStringCount50KG)
        return fail(d,OriginalUiLocalizationError50KG::count,26,0,0x14BDD8);
    const std::size_t firstName=u32(names.data+4),table=u32(names.data+8);
    if (firstName>=table || table>names.size ||
        originalUiLocalizedStringCount50KG>(names.size-table)/8)
        return fail(d,OriginalUiLocalizationError50KG::bounds,4,0,0x14C190);
    // Original reads table-firstName-1 bytes for its name buffer.
    const std::size_t namesEnd=table-1;
    try {
        OriginalUiEnglish50KG result;
        std::size_t na=firstName,ta=0;
        for (std::size_t i=0;i<originalUiLocalizedStringCount50KG;++i) {
            auto& entry=result.entries_[i];entry.nameOffset=na;entry.textOffset=ta;
            std::size_t ne=na;while (ne<namesEnd && names.data[ne]) ++ne;
            std::size_t te=ta;while (te<texts.size && texts.data[te]) ++te;
            if (ne==namesEnd || te==texts.size)
                return fail(d,OriginalUiLocalizationError50KG::unterminated,ne==namesEnd?na:ta,i,0x14C340);
            if (ne==na || ne-na>480 || te-ta>480)
                return fail(d,OriginalUiLocalizationError50KG::capacity,ne==na?na:ta,i,0x14C0EC);
            entry.name.assign(reinterpret_cast<const char*>(names.data+na),ne-na);
            entry.text.assign(reinterpret_cast<const char*>(texts.data+ta),te-ta);
            const auto maximum=u16(names.data+table+i*8+4);
            if (maximum>480) return fail(d,OriginalUiLocalizationError50KG::capacity,table+i*8+4,i,0x14C0EC);
            entry.sourceMaximumLength=static_cast<std::int16_t>(maximum);
            for (std::size_t previous=0;previous<i;++previous)
                if (result.entries_[previous].name==entry.name)
                    return fail(d,OriginalUiLocalizationError50KG::duplicateName,na,i,0x14C538);
            na=ne+1;ta=te+1;
        }
        // Source consumes only count entries. Authenticated trailing bytes are
        // retained by the archive but neither interpreted nor fabricated here.
        result.indexIdentity_=names.identity;result.textIdentity_=texts.identity;
        output=std::move(result);d={};return true;
    } catch (...) { return fail(d,OriginalUiLocalizationError50KG::allocation); }
}
}
