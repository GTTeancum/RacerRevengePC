#pragma once
#include "original_ui_material50kg.h"
#include <memory>
#include <utility>

namespace rr::assets {
// One native application lifetime for authenticated APPSTART and its genuine
// DefaultMT/ScreenMT template objects. No material-registry row, reference
// count, directory36B970, Entity counter, renderer or GPU state is fabricated.
// Keep this owner outside the frontend/race restart loop. It may be prepared
// lazily at the first actual material consumer, never reset between sessions.
class OriginalUiMaterialApplication50LA final {
    std::shared_ptr<const OriginalUiArchive50KG> appStart_;
    OriginalUiMaterialTemplates50KG templates_;
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
