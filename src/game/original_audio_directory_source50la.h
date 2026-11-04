#pragma once
#include <array>
#include <cstdint>
#include <exception>
#include <limits>
#include <string_view>
#include <thread>

namespace original_main_entry50KG {struct Application;}
namespace native_player_speech50LA {class LoadPrefix;}
namespace rr::game {
// Closed, process-shared projection of writable36AFE0, not an AUDIO path
// guessed from native mounts. The native Application constructor performs the
// represented17C7BC/C0/C4 stores. No whole17C650 completion is implied.
// Future writers/indirect aliases must join this owner or invalidate knownness.
class OriginalAudioDirectorySource50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class ::native_player_speech50LA::LoadPrefix;
    friend class OriginalRaceMusicManager50LA;
    struct State {
        const std::thread::id thread=std::this_thread::get_id();
        const OriginalAudioDirectorySource50LA* nativeOwner=nullptr;
        std::array<char,7> bytes; // Only the exact constructor-written prefix.
        std::uint64_t generation=0;
        bool known=false;
    };
    static State& state()noexcept {static State value;return value;}
    const ::original_main_entry50KG::Application* application_=nullptr;
    std::uint64_t generation_=0;
    bool attempted_=false,published_=false;
    bool write17C7BC(const ::original_main_entry50KG::Application& application)noexcept {
        if(attempted_)return false;attempted_=true;auto& shared=state();
        // Native bounded-domain admission, not a source allocation/null arm.
        if(shared.thread!=std::this_thread::get_id()||shared.nativeOwner||
           shared.generation==std::numeric_limits<std::uint64_t>::max())return false;
        application_=&application;
        // Original loads from literal3A85A8; little-endian word, halfword,
        // then NUL byte. No source read of initial empty36AFE0 is substituted.
        shared.bytes[0]='a';shared.bytes[1]='u';shared.bytes[2]='d';shared.bytes[3]='i'; //17C7BC.
        shared.bytes[4]='o';shared.bytes[5]='\\'; //17C7C0.
        shared.bytes[6]='\0'; //17C7C4.
        shared.known=true;generation_=++shared.generation;
        shared.nativeOwner=this;published_=true;return true;
    }
    bool read36AFE0(const ::original_main_entry50KG::Application& application,
        std::string_view& out)const noexcept {
        const auto& shared=state();
        if(!published_||application_!=&application||shared.thread!=std::this_thread::get_id()||
           !shared.known||shared.nativeOwner!=this||shared.generation!=generation_||shared.bytes[6])return false;
        out={shared.bytes.data(),6};return true;
    }
public:
    OriginalAudioDirectorySource50LA()noexcept=default;
    OriginalAudioDirectorySource50LA(const OriginalAudioDirectorySource50LA&)=delete;
    OriginalAudioDirectorySource50LA& operator=(const OriginalAudioDirectorySource50LA&)=delete;
    OriginalAudioDirectorySource50LA(OriginalAudioDirectorySource50LA&&)=delete;
    OriginalAudioDirectorySource50LA& operator=(OriginalAudioDirectorySource50LA&&)=delete;
    ~OriginalAudioDirectorySource50LA(){
        if(!published_)return;auto& shared=state();
        if(shared.thread!=std::this_thread::get_id()||shared.nativeOwner!=this||shared.generation!=generation_)
            std::terminate();
        // Native issuer withdrawal, including failed Application construction.
        // Original process bytes/history are NOT reset or cleared by teardown.
        shared.nativeOwner=nullptr;
    }
};
} // namespace rr::game
