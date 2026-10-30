#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_source_psg_missing50la.h"
#include <array>
#include <memory>
#include <optional>
#include <thread>
namespace rr::game {class OriginalSourcePhysicsCollision50LA;class OriginalWreckType0Owners50LA;}
namespace rr::assets {
//Exact COL t1=1 continuation; no PSG/native file ID is a source handle.
class OriginalSourceCollisionFile50LA final {
    friend class OriginalStandardTextureResolver50LA;
    friend class rr::game::OriginalSourcePhysicsCollision50LA;
    using Resource=OriginalStandardCollisionType0Resource50LA;
    using Payload=rr::game::OriginalWreckType0Owners50LA;
    using D=OriginalStandardTextureDiagnostic50LA;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalSourcePsgMissingLookup50LA> search_;
    const void* collisionOwner_=nullptr;
    const void* payloadBorrower_=nullptr; //Exact source type0 owner; never a source retain.
    const std::thread::id thread_=std::this_thread::get_id();
    OriginalUiMemberView50KG expected_{};
    std::int32_t row_=-1,handle_=-1;
    std::size_t mount_=static_cast<std::size_t>(-1);
    std::uint64_t generation_=0;
    bool warmObserved_=false,returnedCold_=false,nativeClosed_=false;
    bool outputHandleStored_=false,outputCacheStored_=false;
    std::optional<std::int64_t> sourceHandle148_;
    std::optional<const void*> sourceCache150_;
    Resource* associatedResource_=nullptr; //Raw source row1C; no resource retain.
    const Payload* resourceOwner_=nullptr;
    bool associationAttempted_=false,associationStored_=false,associationReturned_=false;
    bool associationWithdrawn_=false;
    std::uint8_t associationGuards_=0;
    std::optional<std::int32_t> associationReturnV0_;
    bool sourceCloseAttempted_=false,sourceCloseReturned_=false;
    std::optional<std::int32_t> sourceCloseResult_;
    std::uint32_t pc_=0;
    struct ReadReceipt {
        bool entered=false,returned=false,afterKnown=false;
        std::size_t before=0,after=0,requested=0,done=0;
        D diagnostic{};
    };
    OriginalSourceCollisionFile50LA()noexcept=default;
    static bool continueFound132988(std::shared_ptr<OriginalSourcePsgMissingLookup50LA>,
        const void* exactCollisionOwner,std::shared_ptr<OriginalSourceCollisionFile50LA>& pending,D&)noexcept;
    bool preflightNative()const noexcept;
    bool current()const noexcept;
    bool read12FB30(void*,std::size_t,ReadReceipt&,D&)noexcept;
    bool associateSource1326A0(Resource&,const Payload*,D&)noexcept;
    bool withdrawAssociationNative(D&)noexcept;
    bool payloadCustodyCurrentNative(const void*)const noexcept;
    //A genuine close can retain the exact source association/payload borrower.
    //Native cancellation never sets this original close return receipt.
    bool closeSource131040(D&)noexcept;
    bool closeNative(D&)noexcept;
public:
    ~OriginalSourceCollisionFile50LA()noexcept=default; //COL owns explicit withdrawal.
    OriginalSourceCollisionFile50LA(const OriginalSourceCollisionFile50LA&)=delete;
    OriginalSourceCollisionFile50LA& operator=(const OriginalSourceCollisionFile50LA&)=delete;
};
}
