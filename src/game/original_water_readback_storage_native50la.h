#pragma once
#include "original_water_readback_layout50la.h"
#include "original_water_readback_packet2b80d0_native50la.h"
#include "original_water_readback_fixup2b4778_native50la.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <optional>
#include <utility>

namespace rr::game {
// Native-only, independently owned backing. These are not retained source
// constructors: they publish no443D98/443CE8, provider140, callbackDC or PC.
// Their allocation failure is a native outcome, not a modeled305500/new-handler
// return. A host pointer is never converted to a guest address/uncached alias.
namespace water_readback_storage_detail50LA {
template<std::size_t Alignment> struct AlignedDelete {
    void operator()(std::uint8_t* p)const noexcept {
        if(p)::operator delete[](p,std::align_val_t{Alignment});
    }
};
}

// Separate lifetime from geometry depth storage: original443D98 is shared
// process packet storage created by2B1E60 and deleted by2B2230, not Video+88.
class OriginalWaterPacketBackingNative50LA final {
    using Delete=water_readback_storage_detail50LA::AlignedDelete<64>;
    std::unique_ptr<std::uint8_t[],Delete> bytes_;
    std::size_t prefixBytes_=0;
    OriginalWaterPacketBackingNative50LA()noexcept=default;
public:
    enum class CreateStatus : std::uint8_t {allocatedNative,budget,allocation};
    struct CreateResult {
        CreateStatus status=CreateStatus::allocation;
        std::unique_ptr<OriginalWaterPacketBackingNative50LA> storage;
    };
    ~OriginalWaterPacketBackingNative50LA()=default;
    OriginalWaterPacketBackingNative50LA(const OriginalWaterPacketBackingNative50LA&)=delete;
    OriginalWaterPacketBackingNative50LA& operator=(const OriginalWaterPacketBackingNative50LA&)=delete;
    static constexpr std::size_t capacityBytes()noexcept{return 1024;}
    static constexpr std::size_t alignmentBytes()noexcept{return 64;}
    static CreateResult allocateNative(std::size_t byteBudget)noexcept {
        if(byteBudget<capacityBytes())return {CreateStatus::budget,{}};
        std::unique_ptr<OriginalWaterPacketBackingNative50LA> owner(
            new(std::nothrow) OriginalWaterPacketBackingNative50LA);
        if(!owner)return {CreateStatus::allocation,{}};
        owner->bytes_.reset(static_cast<std::uint8_t*>(
            ::operator new[](capacityBytes(),std::align_val_t{64},std::nothrow)));
        if(!owner->bytes_)return {CreateStatus::allocation,{}};
        //2592D0(64,1):64qwords,64-aligned1024 bytes, memset before reset.
        //The native metadata object is not the guest32-byte object image.
        std::memset(owner->bytes_.get(),0,capacityBytes());
        return {CreateStatus::allocatedNative,std::move(owner)};
    }
    void stageNumeric(const OriginalWaterReadbackPacket2B80D0Native50LA& packet)noexcept {
        //An exact eight-qword prefix, never a submission or transfer grant.
        //Source2593F0 resets metadata only. Preserve bytes outside the prefix.
        std::memcpy(bytes_.get(),packet.data(),packet.sizeBytes());
        prefixBytes_=packet.sizeBytes();
    }
    void clearPreparedPrefix()noexcept {prefixBytes_=0;} //No backing-byte erasure.
    std::size_t preparedPrefixBytes()const noexcept{return prefixBytes_;}
    std::optional<std::uint8_t> numericByte(std::size_t offset)const noexcept {
        if(offset>=capacityBytes())return {};
        return bytes_[offset];
    }
    //No mutable span, callback installation, source return or DMA operation.
};

// Geometry allocation2B8740:128-aligned368*(columns+1)*bands bytes. The source
// does NOT initialize this buffer. Keep unknown bytes unreadable instead of
// treating allocator contents or host-zero initialization as readback data.
class OriginalWaterDepthBackingNative50LA final {
    using Delete=water_readback_storage_detail50LA::AlignedDelete<128>;
    using Fixup=OriginalWaterReadbackFixup2B4778Native50LA;
    using Packet=OriginalWaterReadbackPacket2B80D0Native50LA;
    const OriginalWaterReadbackLayout50LA layout_;
    std::unique_ptr<std::uint8_t[],Delete> bytes_;
    bool numericFixupApplied_=false;
    explicit OriginalWaterDepthBackingNative50LA(const OriginalWaterReadbackLayout50LA& layout)noexcept
        :layout_(layout){}
    bool byteKnown(std::size_t offset)const noexcept {
        const auto& extent=*layout_.checkedNativeExtent();
        if(!numericFixupApplied_||offset>=extent.bandRecordsBytes)return false;
        const auto record=offset/368u,lane=offset%368u;
        //Only the final three records in each ORIGINAL unrounded column row.
        //This can only be entered after applyNumeric admitted columns>=3.
        if(record%extent.columns<extent.columns-3u)return false;
        return (lane>=0x10u&&lane<0x100u)||(lane>=0x110u&&lane<0x160u);
    }
public:
    enum class CreateStatus : std::uint8_t {allocatedNative,unsupportedExtent,budget,allocation};
    struct CreateResult {
        CreateStatus status=CreateStatus::unsupportedExtent;
        std::unique_ptr<OriginalWaterDepthBackingNative50LA> storage;
    };
    ~OriginalWaterDepthBackingNative50LA()=default;
    OriginalWaterDepthBackingNative50LA(const OriginalWaterDepthBackingNative50LA&)=delete;
    OriginalWaterDepthBackingNative50LA& operator=(const OriginalWaterDepthBackingNative50LA&)=delete;
    static constexpr std::size_t alignmentBytes()noexcept{return 128;}
    static CreateResult allocateNative(const OriginalWaterReadbackLayout50LA& layout,
        std::size_t byteBudget)noexcept {
        const auto& extent=layout.checkedNativeExtent();
        if(!extent)return {CreateStatus::unsupportedExtent,{}};
        if(extent->allocationBytes>byteBudget)return {CreateStatus::budget,{}};
        std::unique_ptr<OriginalWaterDepthBackingNative50LA> owner(
            new(std::nothrow) OriginalWaterDepthBackingNative50LA(layout));
        if(!owner)return {CreateStatus::allocation,{}};
        owner->bytes_.reset(static_cast<std::uint8_t*>(
            ::operator new[](extent->allocationBytes,std::align_val_t{128},std::nothrow)));
        if(!owner->bytes_)return {CreateStatus::allocation,{}};
        //Deliberately no memset. numericByte never reads an unknown byte.
        return {CreateStatus::allocatedNative,std::move(owner)};
    }
    const OriginalWaterReadbackLayout50LA& numericLayout()const noexcept{return layout_;}
    std::size_t capacityBytes()const noexcept{return layout_.checkedNativeExtent()->allocationBytes;}
    //A count-only window in this native allocation. It is not a pointer,
    //callback index grant, DMA destination or proof of source443DC8 history.
    std::optional<Packet::NativeByteExtent> numericDepthWindow(std::uint32_t index)const noexcept {
        const auto& extent=*layout_.checkedNativeExtent();
        if(index>=extent.bands)return {};
        const auto packet=Packet::depthArguments(0,{},0,extent.columns,index);
        const auto& window=packet.checkedNativeByteExtent();
        if(!window||window->requiredBytes>extent.allocationBytes)return {};
        return window;
    }
    Fixup::Result applyNumericTerminalFixup()noexcept {
        const auto& extent=*layout_.checkedNativeExtent();
        //This intentionally invokes the pure CPU leaf on independent native
        //storage. It does NOT dispatch2B46D0 or assert earlier MMIO completed.
        const auto result=Fixup::applyNumeric(bytes_.get(),extent.allocationBytes,
            extent.bands,extent.columns);
        if(result.status==Fixup::Status::applied)numericFixupApplied_=true;
        return result;
    }
    std::optional<std::uint8_t> numericByte(std::size_t offset)const noexcept {
        if(!byteKnown(offset))return {};
        return bytes_[offset];
    }
    bool numericFixupApplied()const noexcept{return numericFixupApplied_;}
    //No writable/raw span is exposed. There is no API for caller-supplied
    //received/completed/ready flags. Source allocation/callback joins remain
    //absent, and destruction frees only this native backing. In particular
    //2B85B0 frees provider140 WITHOUT clearing global443CE8; we invent neither
    //that source clear nor an asynchronous source/GPU lifetime guarantee.
};
}
