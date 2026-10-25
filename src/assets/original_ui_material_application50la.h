#pragma once
#include "original_ui_material50kg.h"
#include <memory>
#include <utility>

namespace original_main_entry50KG {struct Application;}
namespace rr::assets {
// One native application lifetime for authenticated APPSTART and its genuine
// DefaultMT/ScreenMT template objects. No material-registry row, reference
// count, directory36B970, Entity counter, renderer or GPU state is fabricated.
// Keep this owner outside the frontend/race restart loop. It may be prepared
// lazily at the first actual material consumer, never reset between sessions.
class OriginalUiMaterialApplication50LA final {
    std::shared_ptr<const OriginalUiArchive50KG> appStart_;
    OriginalUiMaterialTemplates50KG templates_;
    std::shared_ptr<OriginalStandardTextureResolver50LA> sourceResolver_;
    friend struct ::original_main_entry50KG::Application;
    // Only the actual Application continuation can enter this source path.
    // Pin input and reader BEFORE work; partial template fields remain in the
    // same persistent templates_ on failure, and cannot fall back to legacy.
    bool prepareSourceApplication(std::shared_ptr<const OriginalUiArchive50KG> appStart,
        std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
        std::shared_ptr<OriginalStandardProgramTable50LA> programs,
        OriginalUiMaterialDiagnostic50KG& diagnostic) noexcept {
        diagnostic={};
        if(appStart_||sourceResolver_){diagnostic.error=OriginalUiMaterialError50KG::alreadyLoaded;return false;}
        if(!appStart||!appStart->loaded()||!resolver||!programs||
           appStart->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart)){
            diagnostic.error=OriginalUiMaterialError50KG::archive;return false;
        }
        appStart_=std::move(appStart);sourceResolver_=std::move(resolver);
        return templates_.loadSourceApplication(*appStart_,*sourceResolver_,std::move(programs),diagnostic);
    }
public:
    OriginalUiMaterialApplication50LA() noexcept=default;
    OriginalUiMaterialApplication50LA(const OriginalUiMaterialApplication50LA&)=delete;
    OriginalUiMaterialApplication50LA& operator=(const OriginalUiMaterialApplication50LA&)=delete;
    // Serial cold preparation. The input is already authenticated by the
    // existing archive owner; the unchanged loaders authenticate both PSMs.
    // Failed preparation publishes no application owner and is retryable.
    bool prepareFreshApplication(std::shared_ptr<const OriginalUiArchive50KG> appStart,
        OriginalUiMaterialDiagnostic50KG& diagnostic) noexcept {
        diagnostic={};
        if(appStart_){diagnostic.error=OriginalUiMaterialError50KG::alreadyLoaded;return false;}
        if(!appStart||!appStart->loaded()||
           appStart->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart)){
            diagnostic.error=OriginalUiMaterialError50KG::archive;return false;
        }
        OriginalUiMaterialTemplates50KG next;
        if(!next.load(*appStart,diagnostic))return false;
        templates_=std::move(next);appStart_=std::move(appStart);return true;
    }
    bool prepared()const noexcept{return appStart_&&templates_.loaded();}
    const std::shared_ptr<const OriginalUiArchive50KG>& appStart()const noexcept{return appStart_;}
    const OriginalUiMaterialTemplates50KG& templates()const noexcept{return templates_;}
};
}
