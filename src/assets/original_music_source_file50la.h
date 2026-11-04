#pragma once
#include "original_standard_texture_resource50la.h"
#include <filesystem>
#include <string_view>
namespace rr::game {class OriginalRaceMusicManager50LA;}
namespace rr::assets {
// Private single1578D0 occurrence. The actual manager supplies its same
// Application FileManager, audio root and automatic Config after validating
// its issuer. A path/name/byte match alone cannot mint this owner.
class OriginalMusicSourceFile50LA final {
    friend class rr::game::OriginalRaceMusicManager50LA;
    friend class OriginalStandardTextureResolver50LA;
    using D=OriginalStandardTextureDiagnostic50LA;
    using R=OriginalStandardTextureResolver50LA;
    const original_main_entry50KG::Application* const issuer_;
    const rr::game::OriginalRaceMusicManager50LA* const manager_;
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<R> resolver_;
    std::shared_ptr<const std::vector<std::uint8_t>> input_;
    const void* config_=nullptr;
    std::filesystem::path audioRoot_;
    struct Storage;
    std::unique_ptr<Storage> storage_;
    std::int32_t device_=-1,member_=-1,handle_=-1,winner_=-1;
    std::uint32_t observedMode170_=0;
    bool attempted_=false,acquired_=false,registered_=false,returned_=false,closed_=false,nativeClosed_=false;
    OriginalMusicSourceFile50LA(const original_main_entry50KG::Application*,
        const rr::game::OriginalRaceMusicManager50LA*)noexcept;
    bool begin131230(std::shared_ptr<R>,const void* exactConfig,const std::filesystem::path& actualAudioRoot,
        std::string_view exactName,std::int64_t& returnedSourceHandle,D&)noexcept;
    bool current(const void* exactConfig,std::int64_t handle)const noexcept;
    bool read12FB30(const void* exactConfig,std::int64_t handle,void*,std::size_t,std::size_t&,D&)noexcept;
    bool close131040(const void* exactConfig,std::int64_t handle,D&)noexcept;
    bool closeNative(D&)noexcept;
public:
    ~OriginalMusicSourceFile50LA()noexcept;
    OriginalMusicSourceFile50LA(const OriginalMusicSourceFile50LA&)=delete;
    OriginalMusicSourceFile50LA& operator=(const OriginalMusicSourceFile50LA&)=delete;
};
}
