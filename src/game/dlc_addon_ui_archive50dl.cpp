#include "dlc_addon_ui_archive50dl.h"
// PORT EXTENSION, NOT ORIGINAL BEHAVIOUR: see dlc_addon_ui_archive50dl.h.
#include "../assets/dlc_files50dl.h"
#include "../assets/dlc_registry50dl.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <vector>

namespace rr::game::dlc50DL {
namespace reg=rr::assets::dlc50DL;
namespace {
std::string pick(const std::vector<std::string>& list,const char* extension) {
    for(const auto& m:list){
        const auto dot=m.find_last_of('.');if(dot==std::string::npos)continue;
        std::string e=m.substr(dot);for(auto& c:e)if(c>='A'&&c<='Z')c=char(c-'A'+'a');
        if(e==extension)return m;
    }
    return {};
}
}
void addonMemberStem50DL(const std::string& m,std::string& path,std::string& leaf) {
    const auto dot=m.find_last_of('.');const auto slash=m.find_last_of("\\/");
    const auto from=slash==std::string::npos?0:slash+1;
    path=m.substr(0,dot);leaf=m.substr(from,dot==std::string::npos?std::string::npos:dot-from);
}
bool loadAddonCareerArt50DL(unsigned number,AddonCareerArt50DL& out,std::string& why) noexcept {
    try {
        const auto* pod=reg::addonPod(number);
        if(!pod){why="not an admitted add-on pod";return false;}
        const auto path=reg::host(std::filesystem::path(pod->ui.archive));
        std::error_code e;const auto size=std::filesystem::file_size(path,e);
        if(e||!size||size>reg::RegistryLimits::uiArchiveBytes){why="menu archive outside the add-on UI archive capacity";return false;}
        std::ifstream in(path,std::ios::binary);
        if(!in){why="menu archive cannot be opened";return false;}
        std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
        if(!in.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()))){why="menu archive cannot be read";return false;}
        auto archive=std::make_shared<assets::OriginalUiArchive50KG>();assets::OriginalUiArchiveDiagnostic50KG ad;
        if(!archive->loadAddon(pod->ui.archive,std::move(bytes),reg::RegistryLimits::uiArchiveBytes,
               reg::RegistryLimits::uiDecodedBytes,reg::RegistryLimits::uiEntries,ad)){
            why="menu archive does not decode within its capacity (status "+std::to_string(unsigned(ad.status))+")";return false;}
        AddonCareerArt50DL next;next.archive=std::move(archive);
        next.portraitPst=pick(pod->ui.portraits,".pst");
        next.backgroundPst=pick(pod->ui.background,".pst");next.backgroundPsm=pick(pod->ui.background,".psm");
        next.garagePsg=pick(pod->ui.garageModel,".psg");
        out=std::move(next);return true;
    }catch(const std::exception& x){why=x.what();return false;}
     catch(...){why="menu archive cannot be read";return false;}
}
}
