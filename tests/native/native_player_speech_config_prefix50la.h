#pragma once
#include "../../src/game/original_audio_directory_source50la.h"
#include "../../src/assets/dlc_registry50dl.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

namespace live_audio50HF {class Owner;}
namespace native_player_speech50LA {
// One actual automatic Config at2C119C, retained across the pending native
// continuation. It is not World34/38, a particle emitter, or a file handle.
class LoadPrefix final {
    friend class ::live_audio50HF::Owner;
    struct Config157320 {
        std::int64_t file00;
        std::uint32_t binary08,opened0C,mode10,count14,index18;
        const void* text1C;std::uint32_t selected20;const void* child24;
        std::array<char,1024> name28; //29..427 remain unknown/unread.
        std::uint32_t field428,field42C,field430,field434,cacheCount438;
        std::array<std::uint32_t,32> cache43C;
        const void *names4CC,*index4BC;
        std::uint32_t indexBytes4C0;
        const void* block4C4;std::uint32_t field4C8;
        Config157320()noexcept {
            file00=-1; //157330 SD, including high word04.
            binary08=0;opened0C=0;mode10=0;count14=0;index18=0;
            text1C=nullptr;selected20=0;child24=nullptr; //157334..350.
            name28[0]=0; //157354 writes ONE byte, not the array.
            field428=0;field42C=0;field430=0;field434=0;cacheCount438=0;
            cache43C.fill(0); //157378 ->306478 zeroes exactly43C..4BB.
            names4CC=nullptr;index4BC=nullptr;indexBytes4C0=0;block4C4=nullptr;field4C8=0;
        }
    };
    std::optional<Config157320> config_;
    std::array<char,0x110> filename70_; //No invented initialization of source stack tail.
    std::size_t filenameKnownBytes_=0;
    std::optional<std::uint32_t> racer60_;
    const void* racer_=nullptr;const void* speech_=nullptr;
    std::uint32_t pc_=0x2c1170u;
    bool attempted_=false,constructorReturned_=false,filenameReturned_=false,nativeAbandoned_=false;
    bool begin2C1190(const ::original_main_entry50KG::Application& app,
        const rr::game::OriginalAudioDirectorySource50LA& directory,
        const void* racer,const void* speech,std::uint32_t actualRacer60)noexcept {
        if(attempted_||nativeAbandoned_||!racer||!speech)return false;
        attempted_=true;racer_=racer;speech_=speech;
        pc_=0x2c1190u;racer60_=actualRacer60; //Actual LW from SAME racer+60, not1E0 or host slot.
        pc_=0x2c119cu;config_.emplace(); //Automatic storage; no guest allocator/null branch.
        pc_=0x1573a0u;constructorReturned_=true; //Same Config returned, unknown name tail retained.
        pc_=0x2c11b8u;
        std::string_view sourceDirectory;
        if(!directory.read36AFE0(app,sourceDirectory))return false;
        // Exact positive two-digit subset already authenticated by the caller.
        // %02d is minimum width, not modulo100. Other signed values are pending.
        if(!actualRacer60||actualRacer60>=100u)return false;
        // PORT EXTENSION, NOT ORIGINAL: an add-on racer (32..99) spells its
        // stem where the original formats pod%02d ("speech\<stem>\sounds.ini").
        const auto* addon=actualRacer60>=rr::assets::dlc50DL::RegistryLimits::firstNumber?
            rr::assets::dlc50DL::addonPod(actualRacer60):nullptr;
        if(actualRacer60>=rr::assets::dlc50DL::RegistryLimits::firstNumber&&!addon)return false;
        constexpr std::string_view middle="speech\\pod",suffix="\\sounds.ini";
        constexpr std::string_view addonMiddle="speech\\";
        const auto length=addon?sourceDirectory.size()+addonMiddle.size()+addon->stem.size()+suffix.size():
            sourceDirectory.size()+middle.size()+2u+suffix.size();
        if(length>=filename70_.size())return false; //Native extent guard; no source snprintf truncation.
        std::size_t at=0;
        for(const auto c:sourceDirectory)filename70_[at++]=c;
        if(addon){
            for(const auto c:addonMiddle)filename70_[at++]=c;
            for(const auto c:addon->stem)filename70_[at++]=c;
        }else{
            for(const auto c:middle)filename70_[at++]=c;
            filename70_[at++]=static_cast<char>('0'+actualRacer60/10u);
            filename70_[at++]=static_cast<char>('0'+actualRacer60%10u);
        }
        for(const auto c:suffix)filename70_[at++]=c;
        filename70_[at++]=0;filenameKnownBytes_=at;filenameReturned_=true;
        pc_=0x2c11c8u;return true;
        //1577D0 is NOT called. Config.file00=-1 remains constructor state,
        //not a failed-open result. The caller ignores open's result in retail,
        //so unopened assertion/default-count behavior must not be fabricated.
    }
    void abandonNative()noexcept {
        if(nativeAbandoned_)return;
        // No file/Config destructor source effects have been implemented or
        // reached. Release native stack custody only, never claim1573B0 return.
        racer_=nullptr;speech_=nullptr;nativeAbandoned_=true;
    }
public:
    // User-provided constructor avoids value-initializing the unknown stack
    // byte array when optional::emplace constructs this parked occurrence.
    LoadPrefix()noexcept {}
    LoadPrefix(const LoadPrefix&)=delete;LoadPrefix& operator=(const LoadPrefix&)=delete;
    LoadPrefix(LoadPrefix&&)=delete;LoadPrefix& operator=(LoadPrefix&&)=delete;
    ~LoadPrefix()=default;
};
} // namespace native_player_speech50LA
