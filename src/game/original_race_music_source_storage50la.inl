#pragma once
// Included before the music manager Impl. Private source-owned backing only;
// none of these fields is a public readiness or caller-supplied completion token.
#include <cstring>
namespace rr::game::original_race_music_source50LA {
struct VectorAssertions final {
    const std::thread::id thread=std::this_thread::get_id();
    bool guard8834=false,guard8838=false;
    std::uint32_t assertion434AB8=0,assertion434AC0=0;
};
inline VectorAssertions& vectorAssertions()noexcept {
    // Original lazy BSS guards persist across serial managers. Other original
    //235CB0 callers must join this SAME projection, not seed a per-child copy.
    static VectorAssertions state;return state;
}
struct Config final {
    std::int64_t file00;
    std::uint32_t binary08,opened0C,mode10,count14,index18;
    const void* text1C;std::uint32_t selected20;const void* child24;
    std::array<char,1024> name28;
    std::uint32_t field428,field42C,field430,field434,cacheCount438;
    std::array<std::uint32_t,32> cache43C;
    const void *names4CC,*index4BC;
    std::uint32_t indexBytes4C0;const void* block4C4;std::uint32_t field4C8;
    Config()noexcept {
        file00=-1;binary08=0;opened0C=0;mode10=0;count14=0;index18=0;
        text1C=nullptr;selected20=0;child24=nullptr;name28[0]=0;
        field428=0;field42C=0;field430=0;field434=0;cacheCount438=0;
        cache43C.fill(0);names4CC=nullptr;index4BC=nullptr;
        indexBytes4C0=0;block4C4=nullptr;field4C8=0;
    }
};
struct Child final {
    OriginalWreckComponent50LA entity;
    bool counted=false,constructorReturned=false,published164=false,loadEntered=false;
    // Knownness is independent of contents on partially reached constructors.
    std::optional<std::uint32_t> dispatch08,word30,count34,group38,mode40;
    const void* backend3C=nullptr;
    std::optional<std::uint32_t> vectorCount44,growth4C,capacity50;
    // The source array's contents are unwritten. nullopt means UNKNOWN, not0.
    std::unique_ptr<std::optional<std::uint32_t>[]> storage48;
    std::uint8_t flags54KnownMask=0,flags54KnownValue=0;
    std::optional<std::uint32_t> word58,word5C,word60,word64,word68,word6C,word70,word78;
    std::uint8_t flags74KnownMask=0,flags74KnownValue=0;
    std::optional<Config> config60;
    std::array<char,0x110> filename640,filename750; // Unknown beyond reached NUL.
    std::size_t filename640Known=0,filename750Known=0;
    std::uint32_t pc=0x1a3c34;
    Child()noexcept {} // Do not value-initialize unknown source stack buffers.
    Child(const Child&)=delete;Child& operator=(const Child&)=delete;
};
}
