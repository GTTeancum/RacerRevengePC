#pragma once
#include "../../../src/assets/original_standard_archive_provider50la.h"
#include "../../../src/assets/original_ui_pages50kg.h"
#include <memory>
#include <string>
#include <vector>
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
// Native Config157320 is NOT an Entity. It owns an actual opened archive
// member, cursor, read buffer and newly decoded records until1573B0. None is
// borrowed from SceneAssets' preflight parser. No fabricated guest handle.
class OriginalUiGarageSceneConfig50KG final {
    friend class OriginalUiGaragePodScene50KG;
    using Provider=assets::OriginalStandardArchiveProvider50LA;
    enum class OpenResult {opened,absent,unsupported,stale,allocation};
    struct NativeOpenMember final {
        std::shared_ptr<Provider> provider;
        std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
        Provider::MemberView member{};
        assets::OriginalUiMemberView50KG authenticated{};
        std::string requested;
        std::size_t cursor=0;
        bool opened=false;
        bool current()const noexcept;
        bool read(void*,std::size_t,std::size_t&)noexcept;
        bool seek(std::size_t)noexcept;
        void close131040()noexcept;
    } file00_;
    assets::OriginalUiDat50KG records_;
    std::vector<std::uint8_t> readBuffer_;
    bool parsed_=false,closed_=false;
    OriginalUiGarageSceneConfig50KG()noexcept=default;
    OpenResult open1577D0(const std::shared_ptr<Provider>&,
        const std::shared_ptr<const assets::OriginalUiArchive50KG>&,
        const std::string&,assets::OriginalUiPageDiagnostic50KG&)noexcept;
    bool current()const noexcept;
    const assets::OriginalUiDat50KG* parsed()const noexcept;
    void close1573B0()noexcept;
public:
    ~OriginalUiGarageSceneConfig50KG()noexcept;
    OriginalUiGarageSceneConfig50KG(const OriginalUiGarageSceneConfig50KG&)=delete;
    OriginalUiGarageSceneConfig50KG& operator=(const OriginalUiGarageSceneConfig50KG&)=delete;
};
}
