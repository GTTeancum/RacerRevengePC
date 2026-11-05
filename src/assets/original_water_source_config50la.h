#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_standard_archive_provider50la.h"
#include "original_config_process_globals50la.h"
#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::assets {
//The distinct2B9314 stack+240 Config. Neither a Particle occurrence nor a
//prepared parser/file receipt can enter it. Source bytes survive native abort.
class OriginalWaterSourceConfig50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    using D=OriginalStandardTextureDiagnostic50LA;
    struct Source {
        std::int64_t file00=-1;
        std::uint32_t binary08=0,opened0C=0,mode10=0,count14=0,index18=0;
        const void* text1C=nullptr;std::uint32_t selected20=0;const void* child24=nullptr;
        std::array<char,1024> name28; //Only byte0 is written by157354.
        std::uint32_t field428=0,field42C=0,field430=0,field434=0,cacheCount438=0;
        std::array<std::uint32_t,32> cache43C{}; //Actual306478(...,0,128).
        const void* names4CC=nullptr;const void* index4BC=nullptr;
        std::uint32_t indexBytes4C0=0;const void* block4C4=nullptr;std::uint32_t field4C8=0;
        Source()noexcept {name28[0]=0;}
    } source_;
    const void* const waterReceiver_; //Exact already-constructed source Entity.
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<OriginalRaTrackSourceFile50LA> track_;
    const void* selectedRoot_=nullptr;
    std::string filename_;
    std::int32_t handle_=-1;
    bool attempted_=false,borrowed_=false,nativeClosed_=false;
    std::uint64_t trackGeneration_=0;
    std::uint32_t pc_=0x1573a4;
    std::optional<std::int64_t> before168_,after168_,managerHandle_;
    std::optional<std::uint32_t> missMode170_;
    std::int32_t memberCount_=0;std::uint32_t rowsCompared_=0;
    bool searchReturned_=false,found_=false,managerReturned_=false;
    std::optional<bool> openReturned_;
    struct Read {
        bool attempted=false,returned=false;
        std::size_t copied=0;
        std::optional<std::size_t> before,after;
        std::array<std::uint8_t,7> bytes; //Only copied prefix is known.
    } magic_,version_,count_;
    std::optional<std::size_t> expectedCursor_;
    bool magicTerminated_=false;
    std::optional<std::int32_t> magicCompare_;
    std::optional<std::uint32_t> versionWord_,countWord_,allocationLow32_;
    std::optional<std::int64_t> allocationArgument_;
    bool seekAttempted_=false,seekReturned_=false,textCallReached_=false;
    bool rejectionCloseAttempted_=false,rejectionCloseReturned_=false;
    bool destructorEntered_=false,selectionCleared_=false,destructorReturned_=false;
    explicit OriginalWaterSourceConfig50LA(const void* receiver)noexcept:waterReceiver_(receiver){}
    bool begin1577D0(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,std::shared_ptr<OriginalRaTrackSourceFile50LA>,
        const std::string&,D&)noexcept;
    bool compareName308F00(std::string_view,std::string_view,bool&)noexcept;
    bool read12FB30(Read&,std::size_t,std::uint32_t,D&)noexcept;
    bool rejectBinary157968(std::uint32_t,D&)noexcept;
    bool closeRejectedStack1573B0(D&)noexcept;
    bool preflightNative(const void*)const noexcept;
    bool abortNative(D&)noexcept;
public:
    ~OriginalWaterSourceConfig50LA(){if(borrowed_)std::terminate();}
    OriginalWaterSourceConfig50LA(const OriginalWaterSourceConfig50LA&)=delete;
    OriginalWaterSourceConfig50LA& operator=(const OriginalWaterSourceConfig50LA&)=delete;
};
}
