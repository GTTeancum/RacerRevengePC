#pragma once
#include "original_standard_texture_resource50la.h"
#include <memory>
#include <string>
#include <thread>
#include <optional>
namespace ra_static_model_cold50LA {class Owner;}
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {class OriginalSourcePhysicsModel50LA;class OriginalSourcePhysicsCollision50LA;}
namespace rr::assets {
class OriginalRaTrackSourceFile50LA;
//Conditional source lookup only: no provider absence, nativeFileId, fopen,
//resource pointer/output handle store, or synthetic source file-open interval.
class OriginalSourcePsgMissingLookup50LA final {
    friend class OriginalStandardTextureResolver50LA;
    friend class OriginalSourcePsgFile50LA;
    friend class OriginalSourceCollisionFile50LA;
    friend class rr::game::OriginalSourcePhysicsModel50LA;
    friend class rr::game::OriginalSourcePhysicsCollision50LA;
    friend class ::ra_static_model_cold50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    using D=OriginalStandardTextureDiagnostic50LA;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalRaTrackSourceFile50LA> root_;
    std::string name_;
    const std::thread::id thread_=std::this_thread::get_id();
    std::int64_t before160_=-1,before168_=-1,after160_=-1,after168_=-1;
    std::int32_t memberCount_=0;
    std::size_t mount_=static_cast<std::size_t>(-1);
    std::uint64_t generation_=0;
    std::optional<std::uint32_t> observed170_;
    std::optional<std::int32_t> returnV0_;
    std::uint32_t sourcePC_=0;
    bool nativeBorrowed_=false,nativeClosed_=false,searchReached_=false,searchReturned_=false;
    bool fastMatch_=false,found_=false;
    const void* foundMemberIdentity_=nullptr; //Observed actual row identity, not guest address.
    const char* foundFilenameIdentity_=nullptr;
    const void* collisionSearchBorrower_=nullptr; //Exclusive search-only occurrence, never a source retain.
    const OriginalSourcePsgFile50LA* fileBorrower_=nullptr;
    const OriginalSourceCollisionFile50LA* collisionFileBorrower_=nullptr;
    bool collisionContinuationAttempted_=false;
    bool continuationAttempted_=false; //One-shot132988, never replay132820.
    OriginalSourcePsgMissingLookup50LA()noexcept=default;
    static bool capture132820(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalRaTrackSourceFile50LA>,const std::string& exactName,
        std::shared_ptr<OriginalSourcePsgMissingLookup50LA>& pending,D&)noexcept;
    bool returnedMissing132820()const noexcept{return returnV0_&&*returnV0_==0&&sourcePC_==0x132a0cu;}
    std::optional<std::int32_t> sourceReturnV0()const noexcept{return returnV0_;}
    //Private typed caller custody, separate from historical sourceV0.
    const OriginalStandardTextureResolver50LA* resolver()const noexcept{return resolver_.get();}
    const OriginalRaTrackSourceFile50LA* root()const noexcept{return root_.get();}
    const std::string& requestedName()const noexcept{return name_;}
    bool nativeClosed()const noexcept{return nativeClosed_;}
    bool liveMissingNative()const noexcept {
        return !collisionSearchBorrower_&&!nativeClosed_&&nativeBorrowed_&&returnedMissing132820()&&preflightNative();
    }
    bool preflightNative()const noexcept;
    bool closeNative(D&)noexcept;
    bool closeCollisionSearchNative(const void* expected,D& d)noexcept {
        d={};if(!expected||collisionSearchBorrower_!=expected||nativeClosed_||
            continuationAttempted_||fileBorrower_||collisionFileBorrower_||!preflightNative())return false;
        //Synchronous callback-free close has no effects on refusal. No other
        //issuer can run during this temporary native-token withdrawal.
        collisionSearchBorrower_=nullptr;
        if(closeNative(d))return true;
        collisionSearchBorrower_=expected;return false;
    }
public:
    ~OriginalSourcePsgMissingLookup50LA()noexcept=default; //Root must explicitly withdraw attempt.
    OriginalSourcePsgMissingLookup50LA(const OriginalSourcePsgMissingLookup50LA&)=delete;
    OriginalSourcePsgMissingLookup50LA& operator=(const OriginalSourcePsgMissingLookup50LA&)=delete;
};
}
