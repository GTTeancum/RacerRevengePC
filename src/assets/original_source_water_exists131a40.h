#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <thread>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::assets {
class OriginalStandardTextureResolver50LA;
// Private one-shot World2746E4 existence call. No open handle, provider member,
// nativeFileId or read/constructor capability. Scalars are historical after
// the synchronous call; no row/filename pointer is retained or dereferenced.
class OriginalSourceWaterExists131A40 final {
    friend class OriginalStandardTextureResolver50LA;
    friend class ::ra_cold_aggregate50LA::Owner;
    const std::thread::id thread_=std::this_thread::get_id();
    const void* resolverIdentity_=nullptr;
    const void* trackIdentity_=nullptr;
    std::uint64_t trackGeneration_=0;
    std::string name_;
    bool entered_=false,searchReturned_=false,found_=false;
    std::int64_t before160_=-1,after160_=-1,before168_=-1,after168_=-1;
    std::int32_t memberCount_=0;
    std::uint32_t rowsCompared_=0,pc_=0x131a40;
    std::optional<std::uint32_t> mode170_,returnV0_;
    OriginalSourceWaterExists131A40()noexcept=default;
    bool preflightHistory()const noexcept;
public:
    ~OriginalSourceWaterExists131A40()=default;
    OriginalSourceWaterExists131A40(const OriginalSourceWaterExists131A40&)=delete;
    OriginalSourceWaterExists131A40& operator=(const OriginalSourceWaterExists131A40&)=delete;
};
}
