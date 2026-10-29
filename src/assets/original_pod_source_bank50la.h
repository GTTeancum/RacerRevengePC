#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_standard_archive_provider50la.h"
#include <memory>
#include <string>
#include <thread>
namespace original_racer_manager_entity {class Owner;}
namespace rr::assets {
// One exact manager-issued1ADF80 occurrence. Not a Garage lease, Track root,
// native prewarm receipt, or proof that the outer Race load has been reached.
class OriginalPodSourceBank50LA final {
    friend class OriginalStandardTextureResolver50LA;
    friend class ::original_racer_manager_entity::Owner;
    using D=OriginalStandardTextureDiagnostic50LA;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalUiArchive50KG> archive_;
    const std::thread::id thread_=std::this_thread::get_id();
    const original_racer_manager_entity::Owner* occurrence_=nullptr;
    std::string path_;
    bool attempted_=false,acquired_=false,joined_=false,nativeClosed_=false,mode1Reached_=false;
    OriginalPodSourceBank50LA()noexcept=default;
    bool mount12CEA0(const original_racer_manager_entity::Owner&,
        std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,
        std::shared_ptr<const OriginalUiArchive50KG>,const std::string&,D&)noexcept;
    bool enterArchiveOnly132A20(D&)noexcept;
    bool closeNative(D&)noexcept;
public:
    ~OriginalPodSourceBank50LA();
    OriginalPodSourceBank50LA(const OriginalPodSourceBank50LA&)=delete;
    OriginalPodSourceBank50LA& operator=(const OriginalPodSourceBank50LA&)=delete;
};
}
