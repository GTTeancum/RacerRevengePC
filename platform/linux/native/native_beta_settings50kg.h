#pragma once
// Native beta preferences, deliberately separate from retail memory-card bytes.
#include "native_beta_file50kg.h"
#include "../../../src/game/original_ui_game_options_controller50kg.h"
#include "../../../src/game/control_presets49d.h"

namespace rr::platform::native_beta50kg {
class SettingsStore final {
public:
    SettingsStore()=default;
    SettingsStore(const SettingsStore&)=delete;
    SettingsStore& operator=(const SettingsStore&)=delete;
    ~SettingsStore()=default;
    // Startup only, before sound-mode/mixer construction. Parse into temporaries
    // and install together; corrupt/unreadable files remain untouched and read-only.
    Result load(const std::filesystem::path& directory,
        game::OriginalUiGameSettingsProfile50KG& settings,
        game::ControlPresets49D& controls) noexcept {
        if(attempted_)return Result::unavailable;
        attempted_=true;
        try {
            if(directory.empty())return Result::unavailable;
            path_=directory/"settings-v1.bin";
            std::error_code ec;std::filesystem::create_directories(directory,ec);
            if(ec)return Result::ioError;
            const auto locked=lock_.acquire(directory/"settings-v1.lock");
            if(locked!=Result::ok)return locked;
            Bytes bytes;const auto read=readFile(path_,bytes);
            if(read==Result::missing){writable_=true;return read;}
            if(read!=Result::ok)return read;
            if(bytes.size()!=52||word(bytes,0)!=0x53425252u||word(bytes,4)!=1||
                word(bytes,8)!=52||word(bytes,12)!=checksum(bytes,16))return Result::invalid;
            auto next=settings;auto nextControls=controls;
            const auto flags=word(bytes,16);
            if(flags>7)return Result::invalid;
            next.flags11050=static_cast<std::uint8_t>((next.flags11050&~7u)|flags);
            next.soundMode1101C=word(bytes,20);
            for(unsigned i=0;i<4;++i)next.volume11020[i]=word(bytes,24+i*4);
            next.savedGroup6E8=word(bytes,40);
            if(!game::validOriginalUiGameSettingsProfile50KG(next))return Result::invalid;
            for(unsigned i=0;i<2;++i){const auto preset=word(bytes,44+i*4);
                if(preset>=8||!game::selectControlPreset49D(nextControls,i,preset))return Result::invalid;}
            settings=next;controls=nextControls;writable_=true;return Result::ok;
        }catch(...){return Result::ioError;}
    }
    // Call only AFTER an actual accepted Game Options/Controls route committed.
    // Defaults edits and Cancel never reach this method. Auto-save governs career
    // progress, not a user's explicitly accepted preferences.
    Result save(const game::OriginalUiGameSettingsProfile50KG& settings,
        const game::ControlPresets49D& controls) noexcept {
        if(!writable_)return Result::unavailable;
        if(!game::validOriginalUiGameSettingsProfile50KG(settings)||
            controls.slots[0].preset11048>=8||controls.slots[1].preset11048>=8)return Result::invalid;
        try {
            Bytes bytes;bytes.reserve(52);
            put32(bytes,0x53425252u);put32(bytes,1);put32(bytes,52);put32(bytes,0);
            put32(bytes,settings.flags11050&7u);put32(bytes,settings.soundMode1101C);
            for(auto volume:settings.volume11020)put32(bytes,volume);
            put32(bytes,settings.savedGroup6E8);
            for(const auto& slot:controls.slots)put32(bytes,slot.preset11048);
            const auto crc=checksum(bytes,16);
            for(unsigned i=0;i<4;++i)bytes[12+i]=static_cast<std::uint8_t>(crc>>(8*i));
            return writeFile(path_,bytes);
        }catch(...){return Result::ioError;}
    }
private:
    static std::uint32_t word(const Bytes& b,std::size_t at) noexcept {
        return std::uint32_t(b[at])|(std::uint32_t(b[at+1])<<8)|
            (std::uint32_t(b[at+2])<<16)|(std::uint32_t(b[at+3])<<24);
    }
    std::filesystem::path path_;
    ProcessLock lock_;
    bool attempted_=false,writable_=false;
};
}
