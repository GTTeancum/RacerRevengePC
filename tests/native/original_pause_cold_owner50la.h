#pragma once
#include "../../src/game/original_pause_native_resources50la.h"
#include <exception>
#include <memory>
namespace original_main_entry50KG { struct Application; }
namespace rr::game { class OriginalPauseNativeDraw50LA; }

namespace original_pause_cold50LA {
class Owner final {
    friend struct original_main_entry50KG::Application;
    const original_main_entry50KG::Application* application_=nullptr;
    std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays_;
    std::shared_ptr<rr::assets::OriginalPauseSourceData50LA> data_;
    rr::assets::OriginalPausePrimitiveReferences50LA prewarm_;
    std::shared_ptr<rr::game::OriginalPauseStorage50LA> storage_;
    std::shared_ptr<rr::game::OriginalPauseNativeDraw50LA> drawing_;
    bool attempted_=false,prewarmed_=false,loaded_=false,drawAttempted_=false,closed_=false;
public:
    rr::assets::OriginalPauseSourceDiagnostic50LA source;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalUiPageDiagnostic50KG parse;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalApplicationFontDiagnostic50LA font;
    rr::game::OriginalPauseStorage50LA::Diagnostic storageDiagnostic;
    rr::game::OriginalPauseNativeResources50LA::Diagnostic nativeDiagnostic;
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){if(!close())std::terminate();}
    const std::shared_ptr<rr::game::OriginalPauseStorage50LA>& storage()const noexcept{return storage_;}
    const std::shared_ptr<rr::game::OriginalPauseNativeDraw50LA>& drawing()const noexcept{return drawing_;}
    // Renderer owns drawing_ strongly until its real GPU retirement completes.
    bool retireDrawing()noexcept {
        if(drawing_&&drawing_.use_count()!=1)return false;
        drawing_.reset();return true;
    }
    bool close()noexcept {
        if(closed_)return true;
        if(!retireDrawing())return false;
        storage_.reset(); // Actual counted race retains its Pause until graph cleanup.
        for(std::size_t i=rr::assets::OriginalPausePrimitiveReferences50LA::count;i;--i)
            prewarm_.release299AC0(i-1);
        data_.reset();overlays_.reset();context_.reset();domain_.reset();
        closed_=true;return true;
    }
};
}
