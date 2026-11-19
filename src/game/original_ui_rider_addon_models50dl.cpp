#include "original_ui_rider_addon_models50dl.h"
// PORT EXTENSION, NOT ORIGINAL BEHAVIOUR: see original_ui_rider_addon_models50dl.h.
#include "../assets/dlc_files50dl.h"
#include "../assets/dlc_registry50dl.h"
#include <cstdio>
#include <fstream>
#include <system_error>
#include <filesystem>

namespace rr::game {
namespace reg=rr::assets::dlc50DL;
namespace {
// Reads the package file the overlay names, within the add-on UI archive capacity.
bool readBounded(const std::string& dlcPath,std::vector<std::uint8_t>& out,std::string& why) {
    try {
        const auto path=reg::host(std::filesystem::path(dlcPath));
        std::error_code e;const auto size=std::filesystem::file_size(path,e);
        if(e||!size||size>reg::RegistryLimits::uiArchiveBytes){why="outside the add-on UI archive capacity";return false;}
        std::ifstream in(path,std::ios::binary);
        if(!in){why="cannot be opened";return false;}
        out.resize(static_cast<std::size_t>(size));
        if(!in.read(reinterpret_cast<char*>(out.data()),static_cast<std::streamsize>(out.size()))){why="cannot be read";return false;}
        return true;
    }catch(const std::exception& x){why=x.what();return false;}
     catch(...){why="cannot be read";return false;}
}
}
bool OriginalUiRiderAddonModels50DL::load(const Input& input) noexcept {
    if(loaded_||!input.stockRider||!input.appStart||!input.podTextures||!input.layoutWidget)return false;
    try {
        const auto count=reg::addonPodCount();
        if(count>reg::RegistryLimits::pods)return false;
        std::vector<Row> rows;rows.reserve(count);
        for(std::size_t i=0;i<count;++i){
            const auto* pod=reg::addonPodAt(i);
            rows.emplace_back();auto& row=rows.back();
            if(!pod){row.refused="registry row absent";continue;}
            row.number=pod->number;row.key=pod->key;
            std::vector<std::uint8_t> bytes;std::string why;
            auto archive=std::make_shared<assets::OriginalUiArchive50KG>();
            assets::OriginalUiArchiveDiagnostic50KG ad;
            if(!readBounded(pod->ui.archive,bytes,why))row.refused="menu archive "+why;
            else if(!archive->loadAddon(pod->ui.archive,std::move(bytes),reg::RegistryLimits::uiArchiveBytes,
                reg::RegistryLimits::uiDecodedBytes,reg::RegistryLimits::uiEntries,ad))
                row.refused="menu archive does not decode within its capacity (status "+std::to_string(unsigned(ad.status))+")";
            else{
                row.archive=archive;
                auto podAssets=std::make_shared<assets::OriginalUiRiderPodAssets50KG>();
                assets::OriginalUiRiderPodDiagnostic50KG pd;
                if(!podAssets->loadAddon(input.stockRider,input.layoutWidget,archive,pod->ui.riderModel,pd))
                    row.refused="rider_model "+pod->ui.riderModel+" does not load (error "+std::to_string(unsigned(pd.error))+")";
                else{
                    auto materials=std::make_shared<assets::OriginalUiRiderModelMaterials50KG>();
                    assets::OriginalUiRiderModelMaterialDiagnostic50KG md;
                    if(!materials->loadAddonPod(podAssets,input.stockRider,input.appStart,input.podTextures,
                        input.materialRuntime,input.ipumCache,md))
                        row.refused="model material "+std::to_string(md.material)+" pass "+std::to_string(md.pass)+
                            " does not load (error "+std::to_string(unsigned(md.error))+")";
                    else if(!materials->images())row.refused="model has no textures of its own";
                    else{
                        auto scene=std::make_unique<OriginalUiRiderPodScene50KG>();OriginalUiRiderModelDiagnostic50KG sd;
                        if(!scene->loadAddon(podAssets,sd))row.refused="model graph does not build";
                        else{row.pod=std::move(podAssets);row.materials=std::move(materials);row.scene=std::move(scene);row.ready=true;}
                    }
                }
            }
            std::fprintf(stderr,"dlc_rider_model_prepare key=%s number=%u model=%s status=%s%s%s%s\n",
                row.key.c_str(),row.number,pod->ui.riderModel.c_str(),row.ready?"ready":"refused",
                row.ready?"":" reason=\"",row.refused.c_str(),row.ready?"":"\"");
        }
        rows_=std::move(rows);loaded_=true;return true;
    }catch(...){return false;}
}
const OriginalUiRiderAddonModels50DL::Row* OriginalUiRiderAddonModels50DL::byNumber(unsigned number) const noexcept {
    if(!reg::addonNumberRange(number))return nullptr;
    for(const auto& row:rows_)if(row.number==number)return &row;
    return nullptr;
}
OriginalUiRiderAddonModels50DL::Row* OriginalUiRiderAddonModels50DL::byNumber(unsigned number) noexcept {
    if(!reg::addonNumberRange(number))return nullptr;
    for(auto& row:rows_)if(row.number==number)return &row;
    return nullptr;
}
bool OriginalUiRiderAddonModels50DL::setImageOwner(std::size_t index,std::uint32_t owner) noexcept {
    if(index>=rows_.size()||!rows_[index].ready||rows_[index].imageOwner!=noOwner)return false;
    rows_[index].imageOwner=owner;return true;
}
void OriginalUiRiderAddonModels50DL::drain() noexcept {
    for(auto& row:rows_)if(row.scene){OriginalUiRiderModelDiagnostic50KG d;(void)row.scene->modelDestroyedAfterDetach(d);}
}

} // namespace rr::game
