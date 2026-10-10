#include "original_ui_main_material_schedule50kg.h"

namespace rr::assets {
namespace {
struct SourceEntry {
    const char* name;
    const char* psm;
    std::uint32_t token;
    std::size_t offset;
    std::uint8_t maps;
};
// Derived identities/order, not redistributed original material text.
constexpr std::array<SourceEntry, originalUiMainScheduledMaterialCount50KG> source{{
    {"particle/mmprt", "particle/mmprt.psm", 2902, 64764, 2},
    {"ui/textures/mmbkg", "ui/textures/mmbkg.psm", 2956, 65593, 1},
    {"ui/textures/mmlogo", nullptr, 2957, 65612, 1},
    {"ui/textures/overlay1", "ui/textures/overlay1.psm", 2962, 65677, 1},
    {"ui/textures/arrwleft", "ui/textures/arrwleft.psm", 2996, 66421, 1},
    {"ui/textures/arrwrght", "ui/textures/arrwrght.psm", 2999, 66491, 1},
    {"font/venus16", "font/venus16.psm", 3006, 66635, 1},
    {"font/denmrk20", "font/denmrk20.psm", 3016, 66799, 1},
    {"font/helvcondblack14", "font/helvcondblack14.psm", 3019, 66858, 1},
    {"ui/textures/notexture", "ui/textures/notexture.psm", 3155, 69494, 0},
    {"font/venus16top", "font/venus16top.psm", 3178, 69882, 1},
    { "font/denmrk16top", "font/denmrk16top.psm", 3180, 69917, 1},
    {"ui/textures/icon_cir", nullptr, 3214, 70499, 1},
    {"ui/textures/icon_sqr", nullptr, 3215, 70521, 1},
    {"ui/textures/icon_tri", nullptr, 3216, 70543, 1},
    {"ui/textures/icon_x", nullptr, 3217, 70565, 1}
}};
unsigned char normalized(unsigned char c) noexcept {
    if (c == '\\') return '/';
    return c >= 'A' && c <= 'Z' ? static_cast<unsigned char>(c + ('a' - 'A')) : c;
}
bool equalName(const std::uint8_t* bytes, std::size_t count, const char* expected) noexcept {
    if (!bytes || !expected) return false;
    std::size_t n = 0;
    while (expected[n]) {
        if (n >= count || normalized(bytes[n]) != static_cast<unsigned char>(expected[n])) return false;
        ++n;
    }
    return n == count;
}
// 298724..298788: ONLY space, CR and LF delimit Mat_Stbl tokens. Semicolons
// and words on comment-looking lines are real table tokens. Tabs are not split.
bool delimiter(std::uint8_t c) noexcept { return c == ' ' || c == '\r' || c == '\n'; }
bool fail(OriginalUiMainMaterialScheduleDiagnostic50KG& d,
    OriginalUiMainMaterialScheduleError50KG error, std::size_t entry,
    std::uint32_t pc) noexcept {
    d.error = error; d.entry = entry; d.sourcePC = pc; return false;
}
}

bool OriginalUiMainMaterialSchedule50KG::load(const OriginalUiArchive50KG& appStart,
    const OriginalUiArchive50KG& mainMenu,const OriginalUiArchive50KG& overlays,
    OriginalUiMainMaterialScheduleDiagnostic50KG& d) noexcept {
    d = {};
    if (loaded_) return fail(d, OriginalUiMainMaterialScheduleError50KG::alreadyLoaded, 0, 0);
    if (!appStart.loaded() || appStart.identity() != originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart) ||
        !mainMenu.loaded() || mainMenu.identity() != originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::mainMenu) ||
        !overlays.loaded() || overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
        return fail(d, OriginalUiMainMaterialScheduleError50KG::archive, 0, 0x17CAD4);
    try {
        const auto table = appStart.find("AppStart/Mat_Stbl.txt");
        if (!table.identity || !table.data || table.size != 70962)
            return fail(d, OriginalUiMainMaterialScheduleError50KG::member, 0, 0x17CAD4);
        std::array<OriginalUiMainMaterialScheduleEntry50KG, originalUiMainScheduledMaterialCount50KG> next{};
        std::size_t cursor = 0, selected = 0;
        std::uint32_t token = 0;
        while (cursor < table.size) {
            while (cursor < table.size && delimiter(table.data[cursor])) ++cursor;
            if (cursor == table.size) break;
            const auto start = cursor;
            while (cursor < table.size && !delimiter(table.data[cursor])) ++cursor;
            if (selected < source.size() && token == source[selected].token) {
                const auto& original = source[selected];
                if (start != original.offset || !equalName(table.data + start, cursor - start, original.name))
                    return fail(d, OriginalUiMainMaterialScheduleError50KG::registryToken, selected, 0x29887C);
                auto& entry = next[selected];
                entry.registeredName = original.name;
                entry.registryIdentity = table.identity;
                entry.registryTokenOrdinal = token;
                entry.registryByteOffset = start;
                entry.projectedLeafOrdinal = static_cast<std::uint8_t>(selected);
                entry.textureMapCount = original.maps;
                if (original.psm) {
                    entry.psmIdentity = mainMenu.find(original.psm).identity;
                    if (!entry.psmIdentity)
                        return fail(d, OriginalUiMainMaterialScheduleError50KG::member, selected, 0x29B628);
                } else {
                    const bool logo=selected==2;
                    const auto& textureOwner=logo?mainMenu:overlays;
                    const std::string base(original.name);
                    if(textureOwner.find(base+".psm").identity || mainMenu.find(base+".psm").identity)
                        return fail(d,OriginalUiMainMaterialScheduleError50KG::member,selected,0x29B630);
                    entry.fallbackTextureIdentity=textureOwner.find(base+".pst").identity;
                    if (!entry.fallbackTextureIdentity)
                        return fail(d, OriginalUiMainMaterialScheduleError50KG::member, selected, 0x247648);
                }
                ++selected;
            }
            ++token;
        }
        if (selected != source.size() || token != 3235)
            return fail(d, OriginalUiMainMaterialScheduleError50KG::registryToken, selected, 0x2987C0);
        entries_ = next;
        loaded_ = true;
        return true;
    } catch (...) {
        return fail(d, OriginalUiMainMaterialScheduleError50KG::allocation, 0, 0);
    }
}

const OriginalUiMainMaterialScheduleEntry50KG* OriginalUiMainMaterialSchedule50KG::find(
    const std::string& name) const noexcept {
    if (!loaded_) return nullptr;
    for (const auto& entry : entries_)
        if (equalName(reinterpret_cast<const std::uint8_t*>(name.data()), name.size(), entry.registeredName))
            return &entry;
    return nullptr;
}
}
