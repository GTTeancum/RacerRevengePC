#pragma once
#include "../assets/original_source_collision_file50la.h"
#include "original_source_physics_gravity50la.h"
#include "original_collision_base_owner50la.h"
#include "original_wreck_type0_owners50la.h"
#include "original_wreck_type2_owners50la.h"
#include "original_wreck_type3_owners50la.h"
#include "../assets/original_source_psg_missing50la.h"
#include <string_view>
#include <cstdio>
#include <vector>
namespace rr::game {
//Isolated source nullable100/11B190. E0 is the outer collision identity;
//only its embedded Entity+8 is enrolled/count-owned. No prepared COL facade.
class OriginalSourcePhysicsCollision50LA final {
    friend class OriginalSourcePhysicsContact50LA;
    friend class OriginalSourcePhysicsBody50LA;
    friend class OriginalSourcePhysicsDetector50LA;
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_static_model_cold50LA::Owner;
    using Base=OriginalSourcePhysicsObject50LA;
    using Gravity=OriginalSourcePhysicsGravity50LA;
    using Body=OriginalSourcePhysicsBody50LA;
    using B=OriginalCollisionBaseOwner50LA;
    using File=rr::assets::OriginalSourceCollisionFile50LA;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    enum class Status : std::uint8_t {reached,bounded,owner};
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;};
    const void* sourceContactBorrower_=nullptr;
    const void* sourceContactHistory_=nullptr;
    bool attachEntered_=false,attachReturned_=false,attached_=false,detachedNative_=false;
    std::optional<std::uint32_t> attachLock_;
    std::uint32_t attachPC_=0;
    bool proceduralLateEntered_=false,proceduralLateReturned_=false;
    std::uint32_t proceduralLatePC_=0;
    Base* base_=nullptr;
    Body* bodyBorrow_=nullptr;
    bool modeEntered_=false,modeReturned_=false,globalZeroStored_=false;
    bool categoryStored_=false,ownerStored_=false,bodyStored_=false;
    std::uint32_t category3E0718_=0;
    bool nameEntered_=false,requestCaptured_=false,directoryRead_=false,directoryNonempty_=false;
    bool pathCopied_=false,separatorAppended_=false,requestCopied_=false,dotReturned_=false,dotFound_=false;
    bool extensionStored_=false,sharedNameStored_=false,nameReturned_=false;
    const std::string* directoryIdentity_=nullptr; //Identity/history only, never reread by cleanup.
    std::string requestName_,lookupName_;
    std::array<char,0x108> path_{}; //Native safety backing; only reached logical prefix is known.
    std::size_t pathLength_=0,dotOffset_=0;
    std::optional<std::int64_t> sourceHandle148_;
    std::optional<const void*> sourceCache150_;
    std::shared_ptr<rr::assets::OriginalSourcePsgMissingLookup50LA> search_;
    bool searchAccepted_=false,searchBorrowed_=false;
    OriginalWreckType0Owners50LA sourceType0_;
    bool transformEntered_=false,transformWorldCallCompleted_=false,transformWorldReturned_=false;
    bool transformPassEntered_=false,transformPassCallCompleted_=false;
    bool transformNestedReturned_=false,transformDirectReturned_=false,transformReturned_=false;
    const OriginalRacerSceneObject50LA* transformModel_=nullptr;
    std::array<std::uint32_t,16> transformIncoming_{};
    std::uint16_t transformKnown_=0;
    std::uint32_t transformWorldPC_=0;
    std::optional<Body::Status> transformWorldStatus_;
    std::optional<OriginalWreckType0Owners50LA::SourcePrefixStatus> transformPassStatus_;
    OriginalWreckType2Owners50LA sourceResultOwner_; //Only its isolated raw source-result facet.
    bool resultEntered_=false,resultCallReturned_=false,resultBorrowed_=false,resultWithdrawn_=false;
    std::optional<OriginalWreckType2Owners50LA::SourceResultStatus> resultStatus_;
    bool resourceEntered_=false,resourceCallReturned_=false,associationEntered_=false;
    std::optional<OriginalWreckType0Owners50LA::SourcePrefixStatus> resourceStatus_;
    bool loader124F80Returned_=false,foundCloseEntered_=false,foundCloseReturned_=false;
    std::optional<std::int32_t> loader124F80ReturnV0_;
    bool nodeEntered_=false,nodeCallReturned_=false,nodeMetadataRefused_=false;
    std::optional<OriginalWreckType0Owners50LA::SourcePrefixStatus> nodeStatus_;
    std::vector<File::ReadReceipt> nodeReads_;
    std::size_t nodeRefusedIndex_=0,nodeRefusedRequested_=0;
    bool arrayEntered_=false,arrayCallReturned_=false,arrayMetadataRefused_=false;
    std::optional<OriginalWreckType0Owners50LA::SourcePrefixStatus> arrayStatus_;
    std::vector<File::ReadReceipt> arrayReads_;
    std::size_t arrayRefusedIndex_=0,arrayRefusedRequested_=0;
    bool type0Entered_=false,type0CallReturned_=false,type0Withdrawn_=false;
    const OriginalRacerSceneObject50LA* type0Model_=nullptr;
    std::optional<OriginalWreckType0Owners50LA::SourcePrefixStatus> type0Status_;
    std::array<File::ReadReceipt,18> type0Reads_{};
    std::size_t type0ReadCount_=0;
    bool foundEntered_=false,foundOutputReturned_=false;
    std::shared_ptr<File> foundFile_;
    std::optional<std::int64_t> foundHandle148_;
    std::optional<const void*> foundCache150_;
    std::array<std::array<std::uint8_t,4>,3> header_{}; //Storage only, not source-known zeroes.
    std::array<std::uint8_t,3> headerKnown_{};
    std::array<File::ReadReceipt,3> headerReads_{};
    bool headerReadsReturned_=false,headerMagicCompared_=false,headerMagicMatched_=false;
    bool headerTagEntered_=false,headerTagReturned_=false,headerTagMatched_=false;
    bool headerVersionCompared_=false,headerVersionMatched_=false;
    bool headerDecisionReached_=false,headerValid_=false,foundB0Stored_=false;
    bool missingEntered_=false,warningAttempted_=false,warningReturned_=false;
    std::optional<int> warningNativeResult_;
    bool loader11B920Returned_=false,callerShapeRead_=false,callerDelayStoreBound_=false;
    std::optional<std::int32_t> loader11B920ReturnV0_;
    std::optional<const void*> callerShape8C_;
    std::optional<std::uint32_t> resultAllocationArgument_;
    std::optional<std::int32_t> searchReturnV0_;
    std::unique_ptr<B> collision_;
    const std::array<std::uint32_t,4>* sentinel3E02C0_=nullptr;
    bool attempted_=false,counted_=false,constructorReturned_=false,published_=false;
    bool nativeRetired_=false,nativeWithdrawn_=false;
    B* publishedIdentity_=nullptr;
    std::optional<std::uint32_t> dispatchFC_,dispatch10_;
    std::optional<std::uint8_t> returnedType02_;
    std::uint8_t flags29Known_=0,flagsF8Known_=0;
    std::uint32_t pc_=0;
    Result at(Status s,std::uint32_t pc)noexcept {pc_=pc;return {s,pc};}
    enum class Occurrence:std::uint8_t {resultE0,directE4,proceduralE8};
    Occurrence occurrence_=Occurrence::resultE0;
    const void* proceduralSuccessorBorrower_=nullptr; //E4 entity custody, not a payload lease.
    OriginalSourcePhysicsCollision50LA* proceduralPredecessor_=nullptr;
    const void** directPredecessorToken_=nullptr;
    const void* directPredecessorHistory_=nullptr;
    const OriginalRacerSceneObject50LA* directModelArgument_=nullptr;
    bool directModelPinned_=false;
    bool directLateEntered_=false,directCallbackStored_=false,directOwnerStored_=false,directBodyStored_=false;
    bool directLateReturned_=false,directTransformEntered_=false,directTypeRead_=false,directShapeRead_=false;
    bool directNullModelBound_=false;
    std::optional<std::uint32_t> directCallbackE8PC_,directTransformType88_;
    std::optional<const void*> directTransformShape8C_;
    std::uint32_t directLatePC_=0,directTransformPC_=0;
    bool directFoundTransformEntered_=false,directWorldCallReached_=false;
    std::optional<std::uint32_t> directFoundType88_;
    std::optional<const void*> directFoundShape8C_;
    const OriginalRacerSceneObject50LA* directFoundModel110_=nullptr;
    std::uint32_t directFoundTransformPC_=0;
    struct DirectMatrixTransform {
        bool attempted=false,worldCompleted=false,worldReturned=false,passEntered=false,passCompleted=false;
        bool returned=false,leaseWithdrawn=false;
        std::array<std::uint32_t,16> incoming{};
        std::uint16_t known=0;
        std::optional<Body::Status> worldStatus;
        std::optional<OriginalWreckType0Owners50LA::SourcePrefixStatus> passStatus;
        std::uint32_t worldPC=0,pc=0;
    } directMatrix_;
    Result continueSourceDirectMatrix11F5B4()noexcept {
        if(directMatrix_.attempted||!isDirectE4()||!directWorldCallReached_||pc_!=0x28b550u||
           nativeWithdrawn_||nativeRetired_||!directFoundPayloadLiveNative()||!bodyBorrow_||
           !directModelPinned_||bodyBorrow_->directCollisionModelBorrower_!=this||
           !searchBorrowed_||!search_||search_->nativeClosed()||search_->collisionSearchBorrower_!=this||
           !search_->preflightNative()||!preflightNative())
            return {Status::owner,0x28b550u};
        //Install exact native custody before the second getter can make effects.
        if(!sourceType0_.pinSourceDirectTransform11F5B4(this,*directFoundModel110_))
            return {Status::owner,0x28b550u};
        auto& d=directMatrix_;d.attempted=true;d.pc=0x28b550u;
        const auto world=bodyBorrow_->copyDirectCollisionModelWorld28B550(*this,d.incoming,d.known);
        d.worldCompleted=true;d.worldStatus=world.status;d.worldPC=world.sourcePC;d.pc=world.sourcePC;
        if(world.status!=Body::Status::reached||world.sourcePC!=0x28b67cu||d.known!=0xffffu)
            return at(Status::bounded,d.pc);
        d.worldReturned=true;d.passEntered=true;d.pc=0x11f5c0u;
        const auto pass=sourceType0_.continueSourceDirectType0Transform11ED60(this,*directFoundModel110_,
            d.incoming,d.known,collision_->cachedCenter60_,collision_->radius80_);
        d.passCompleted=true;d.passStatus=pass.status;d.pc=pass.sourcePC;
        if(pass.status!=OriginalWreckType0Owners50LA::SourcePrefixStatus::reached||
           !sourceType0_.sourceTransform_.returned||sourceType0_.sourceTransform_.pc!=0x11fe28u)
            return at(Status::bounded,d.pc);
        //Same genuine 11F680,11ED60,11F570 control epilogues; no literal V0.
        d.returned=true;d.pc=0x11f678u;return at(Status::reached,d.pc);
    }
    bool directMatrixHistoryNative()const noexcept {
        const auto& d=directMatrix_;const auto& l=sourceType0_.sourceDirectTransform_;
        const auto& t=sourceType0_.sourceTransform_;
        if(!d.attempted)return !d.worldCompleted&&!d.worldReturned&&!d.passEntered&&!d.passCompleted&&
            !d.returned&&!d.leaseWithdrawn&&!d.known&&!d.worldStatus&&!d.passStatus&&!d.worldPC&&!d.pc&&!l.attempted;
        if(!isDirectE4()||!directWorldCallReached_||!directFoundModel110_||!l.attempted||
           l.occurrence!=this||l.model!=directFoundModel110_||l.nativeWithdrawn!=d.leaseWithdrawn||
           !sourceType0_.preflightSourceDirectTransformNative()||!sourceType0_.preflightSourceType0TransformNative())return false;
        if(!d.worldCompleted)return !d.worldReturned&&!d.passEntered&&!d.passCompleted&&!d.returned&&
            !d.worldStatus&&!d.passStatus&&!d.known&&!d.worldPC&&!t.attempted&&d.pc==0x28b550u&&pc_==d.pc;
        if(!d.worldStatus)return false;
        if(!d.worldReturned)return !d.passEntered&&!d.passCompleted&&!d.passStatus&&!d.returned&&!t.attempted&&
            !d.known&&d.pc==d.worldPC&&pc_==d.pc;
        if(d.worldStatus!=Body::Status::reached||d.worldPC!=0x28b67cu||d.known!=0xffffu||
           !d.passEntered||!d.passCompleted||!d.passStatus)return false;
        if(!t.attempted)return !d.returned&&d.passStatus==OriginalWreckType0Owners50LA::SourcePrefixStatus::owner&&
            d.pc==0x11ed60u&&pc_==d.pc;
        if(t.model!=directFoundModel110_||t.incoming!=d.incoming||t.incomingKnown!=d.known||
           (!t.nativeWithdrawn&&(t.center!=&collision_->cachedCenter60_||t.radius!=&collision_->radius80_))||
           (type0Withdrawn_&&!t.nativeWithdrawn)||!sourceType0_.sourceType0TransformOutputsCurrentNative(
            collision_->cachedCenter60_,collision_->radius80_))return false;
        if(!d.returned)return !t.returned&&d.passStatus==OriginalWreckType0Owners50LA::SourcePrefixStatus::bounded&&
            d.pc==t.pc&&pc_==d.pc;
        return t.returned&&t.pc==0x11fe28u&&d.passStatus==OriginalWreckType0Owners50LA::SourcePrefixStatus::reached&&
            d.pc==0x11f678u&&pc_==d.pc;
    }
    bool withdrawSourceDirectMatrixNative()noexcept {
        if(hasIncomingFiltersNative()||hasLiveOutgoingFiltersNative())return false;
        if(proceduralSuccessorBorrower_)return false;
        if(isDirectE4()&&attached_&&!detachedNative_)return false;
        auto& d=directMatrix_;if(!d.attempted||d.leaseWithdrawn)return true;
        if(!directMatrixHistoryNative()||!sourceType0_.unpinSourceDirectTransformNative(this))return false;
        d.leaseWithdrawn=true;return true;
    }
    bool directFoundTransformHistoryNative()const noexcept {
        if(!directFoundTransformEntered_)return !directWorldCallReached_&&!directFoundType88_&&
            !directFoundShape8C_&&!directFoundModel110_&&!directFoundTransformPC_;
        if(!isDirectE4()||!directLateReturned_||loader11B920ReturnV0_!=1||!foundCloseReturned_||
           directTransformEntered_||!directWorldCallReached_||directFoundType88_!=0||
           !directFoundShape8C_||!*directFoundShape8C_||!directFoundModel110_||
           directFoundModel110_!=type0Model_||directFoundModel110_!=directModelArgument_||
           directFoundTransformPC_!=0x28b550u||(!directMatrix_.attempted&&pc_!=directFoundTransformPC_)||
           *directFoundShape8C_!=sourceType0_.sourcePrefix_.publishedIdentity)return false;
        if(type0Withdrawn_)return sourceType0_.sourcePrefix_.nativeWithdrawn; //Opaque retained identities only.
        return sourceType0_.shape8C_&&collision_->shape88_==0&&collision_->shape8C_==directFoundShape8C_&&
            sourceType0_.shape8C_.get()==*directFoundShape8C_&&
            sourceType0_.shape8C_->racerModel110==directFoundModel110_;
    }
    bool directFoundPayloadLiveNative()const noexcept {
        return isDirectE4()&&foundCloseReturned_&&loader11B920Returned_&&loader11B920ReturnV0_==1&&
            !type0Withdrawn_&&foundFile_&&!foundFile_->nativeClosed_&&foundFile_->sourceCloseReturned_&&
            foundFile_->sourceCloseResult_==1&&foundFile_->associationReturned_&&!foundFile_->associationWithdrawn_&&
            foundFile_->payloadBorrower_==&sourceType0_&&foundFile_->payloadCustodyCurrentNative(&sourceType0_)&&
            !sourceType0_.sourcePrefix_.nativeWithdrawn&&sourceType0_.preflightSourceType0PrefixNative();
    }
    bool sourceDirectWorldCallAdmissionNative(const Body* actualBody)const noexcept {
        //Pure live issuer proof, not historical cleanup readiness. Original file
        //close is valid; native file/search/association withdrawal is not.
        return actualBody&&actualBody==bodyBorrow_&&isDirectE4()&&!nativeWithdrawn_&&!nativeRetired_&&
            directMatrix_.attempted&&!directMatrix_.worldCompleted&&!directMatrix_.leaseWithdrawn&&
            sourceType0_.sourceDirectTransform_.live&&sourceType0_.sourceDirectTransform_.occurrence==this&&
            directFoundTransformEntered_&&directWorldCallReached_&&directFoundTransformHistoryNative()&&directFoundPayloadLiveNative()&&
            directModelPinned_&&bodyBorrow_->directCollisionModelBorrower_==this&&
            bodyBorrow_->sourceModel288Native()==directFoundModel110_&&
            searchBorrowed_&&search_&&!search_->nativeClosed()&&search_->collisionSearchBorrower_==this&&
            search_->preflightNative()&&!type0Withdrawn_&&foundFile_&&!foundFile_->nativeClosed_&&
            foundFile_->sourceCloseReturned_&&foundFile_->sourceCloseResult_==1&&
            foundFile_->associationReturned_&&!foundFile_->associationWithdrawn_&&
            foundFile_->payloadBorrower_==&sourceType0_&&foundFile_->payloadCustodyCurrentNative(&sourceType0_)&&
            sourceType0_.preflightSourceType0PrefixNative()&&!sourceType0_.sourcePrefix_.nativeWithdrawn&&
            !resultEntered_&&collision_->result90_==std::optional<const void*>(nullptr);
    }
    Result beginSourceDirectFoundTransform11F570()noexcept {
        if(!isDirectE4()||nativeWithdrawn_||directFoundTransformEntered_||directTransformEntered_||
           !directLateReturned_||pc_!=0x1662c0u||!loader11B920Returned_||loader11B920ReturnV0_!=1||!directFoundPayloadLiveNative()||
           !searchBorrowed_||!search_||search_->nativeClosed()||search_->collisionSearchBorrower_!=this||
           !search_->preflightNative()||type0Withdrawn_||!foundFile_||foundFile_->nativeClosed_||
           !foundFile_->associationReturned_||foundFile_->associationWithdrawn_||
           !foundFile_->sourceCloseReturned_||!foundFile_->payloadCustodyCurrentNative(&sourceType0_)||
           !preflightNative()||!collision_||collision_->shape88_!=0||!sourceType0_.shape8C_||
           collision_->shape8C_!=std::optional<const void*>(sourceType0_.shape8C_.get())||
           sourceType0_.shape8C_->racerModel110!=directModelArgument_||!directModelArgument_)
            return {Status::owner,0x11f570u};
        directFoundTransformEntered_=true;directFoundTransformPC_=0x11f57cu;
        directFoundType88_=collision_->shape88_; //Actual unsigned dispatch input; proven type0 branch.
        directFoundTransformPC_=0x11f5a8u;directFoundShape8C_=*collision_->shape8C_;
        directFoundTransformPC_=0x11f5b0u;directFoundModel110_=sourceType0_.shape8C_->racerModel110;
        directWorldCallReached_=true;directFoundTransformPC_=0x28b550u;
        return at(Status::bounded,directFoundTransformPC_); //Reached call only; no getter or transform return.
    }
    bool directLateHistoryCurrentNative()const noexcept {
        if(!directLateEntered_)return !directCallbackStored_&&!directOwnerStored_&&!directBodyStored_&&
            !directLateReturned_&&!directTransformEntered_&&!directTypeRead_&&!directShapeRead_&&!directNullModelBound_&&
            !directCallbackE8PC_&&!directTransformType88_&&!directTransformShape8C_&&!directLatePC_&&!directTransformPC_;
        if(!isDirectE4()||!directModelPinned_||!loader11B920Returned_||!loader11B920ReturnV0_||
           !directCallbackStored_||directCallbackE8PC_!=std::optional<std::uint32_t>(0x165920u)||
           !directOwnerStored_||!directBodyStored_||!directLateReturned_||directLatePC_!=0x1662c0u||
           !collision_||!bodyBorrow_||!bodyBorrow_->body_||collision_->callbacksE4_[1]||
           collision_->owner9C_!=std::optional<const void*>(&base_->object_->entity)||
           collision_->bodyA0_!=std::optional<const void*>(&bodyBorrow_->body_->entity))return false;
        if(directFoundTransformEntered_)return directFoundTransformHistoryNative();
        if(!directTransformEntered_)return pc_==directLatePC_&&!directTypeRead_&&!directShapeRead_&&
            !directNullModelBound_&&!directTransformType88_&&!directTransformShape8C_&&!directTransformPC_;
        return loader11B920ReturnV0_==0&&missingEntered_&&directTypeRead_&&directTransformType88_==0&&
            collision_->shape88_==0&&directShapeRead_&&directTransformShape8C_==std::optional<const void*>(nullptr)&&
            collision_->shape8C_==directTransformShape8C_&&directNullModelBound_&&
            directTransformPC_==0x11f5b0u&&pc_==directTransformPC_;
    }
    Result publishSourceDirectCaller16629C()noexcept {
        if(!isDirectE4()||nativeWithdrawn_||directLateEntered_||
           (loader11B920ReturnV0_==1&&!directFoundPayloadLiveNative())||!searchBorrowed_||!search_||
           search_->nativeClosed()||search_->collisionSearchBorrower_!=this||!search_->preflightNative()||
           !preflightNative()||pc_!=0x11bbbcu||
           !loader11B920Returned_||!loader11B920ReturnV0_||!directModelPinned_||!collision_||
           !bodyBorrow_||!bodyBorrow_->body_||!base_->object_->bodyD0||
           *base_->object_->bodyD0!=&bodyBorrow_->body_->entity)return {Status::owner,0x16629cu};
        directLateEntered_=true;
        collision_->callbacksE4_[1].reset();directCallbackE8PC_=0x165920u;
        directCallbackStored_=true;directLatePC_=0x1662a8u; //Typed original code identity, no host function pointer.
        collision_->owner9C_=&base_->object_->entity;ownerStored_=true;
        directOwnerStored_=true;directLatePC_=0x1662b0u;
        collision_->bodyA0_=static_cast<const void*>(*base_->object_->bodyD0);bodyStored_=true;
        directBodyStored_=true;directLatePC_=0x1662bcu;
        directLateReturned_=true;directLatePC_=0x1662c0u;return at(Status::reached,directLatePC_);
    }
    Result continueSourceDirectMissingTransform11F570()noexcept {
        if(!isDirectE4()||nativeWithdrawn_||directTransformEntered_||!directLateReturned_||
           !searchBorrowed_||!search_||search_->nativeClosed()||search_->collisionSearchBorrower_!=this||
           !search_->preflightNative()||
           pc_!=0x1662c0u||!preflightNative()||!missingEntered_||!loader11B920Returned_||
           loader11B920ReturnV0_!=0||!collision_||collision_->shape88_!=0||
           collision_->shape8C_!=std::optional<const void*>(nullptr))return {Status::owner,0x11f570u};
        directTransformEntered_=true;directTransformPC_=0x11f57cu;
        directTransformType88_=collision_->shape88_;directTypeRead_=true;
        //Original jump-table[0] at3889E0 selects11F5A8, not a native shape dispatch.
        directTransformPC_=0x11f5a8u;directTransformShape8C_=*collision_->shape8C_;directShapeRead_=true;
        directTransformPC_=0x11f5b0u;directNullModelBound_=true;
        return at(Status::bounded,directTransformPC_); //Actual NULL shape+110 load; no28B550 call/return.
    }
    OriginalWreckType3Owners50LA sourceType3_;
    bool proceduralResultEntered_=false,proceduralResultCallCompleted_=false,proceduralResultReturned_=false;
    std::optional<OriginalWreckType3Owners50LA::SourceResultStatus> proceduralResultStatus_;
    std::optional<const void*> proceduralResultPayload58_;
    std::uint32_t proceduralResultPC_=0;
    Result continueSourceProceduralResult11C934()noexcept {
        if(!isProceduralE8()||proceduralResultEntered_||nativeWithdrawn_||nativeRetired_||detachedNative_||
           !attachReturned_||attachPC_!=0x16632cu||!proceduralLateReturned_||proceduralLatePC_!=0x166350u||
           !proceduralShapeCallCompleted_||proceduralShapeWithdrawn_||proceduralShapePC_!=0x11c934u||
           !sourceType3_.sourceShape_.boundaryReached||!preflightNative())return {Status::owner,0x11c934u};
        proceduralResultEntered_=true;
        const auto result=sourceType3_.continueSourceResult11C934(this);
        proceduralResultCallCompleted_=true;proceduralResultStatus_=result.status;proceduralResultPC_=result.sourcePC;
        if(result.status==OriginalWreckType3Owners50LA::SourceResultStatus::reached&&
           sourceType3_.sourceResultPrefix_.returned11C710&&result.sourcePC==0x11ca80u){
            proceduralResultReturned_=true;proceduralResultPayload58_=sourceType3_.sourceResultPrefix_.returnPayload58;
            return {Status::reached,proceduralResultPC_};
        }
        return {result.status==OriginalWreckType3Owners50LA::SourceResultStatus::owner?Status::owner:Status::bounded,
            proceduralResultPC_};
    }
    const void* proceduralResult90CurrentNative()const noexcept {
        const auto& r=sourceType3_.sourceResultPrefix_;
        return r.published90&&!r.nativeWithdrawn?r.resultIdentity:nullptr;
    }
    bool proceduralResultHistoryNative()const noexcept {
        const auto& r=sourceType3_.sourceResultPrefix_;
        if(!proceduralResultEntered_)return !proceduralResultCallCompleted_&&!proceduralResultReturned_&&
            !proceduralResultStatus_&&!proceduralResultPayload58_&&!proceduralResultPC_&&!r.attempted;
        if(!isProceduralE8()||!proceduralShapeEntered_||!proceduralShapeCallCompleted_||
           proceduralShapePC_!=0x11c934u||!sourceType3_.sourceShape_.boundaryReached||
           !proceduralResultCallCompleted_||!proceduralResultStatus_||!sourceType3_.preflightSourceResultPrefixNative())return false;
        if(!r.attempted)return !proceduralResultReturned_&&!proceduralResultPayload58_&&
            proceduralResultStatus_==OriginalWreckType3Owners50LA::SourceResultStatus::owner&&proceduralResultPC_==0x11c934u;
        if(r.occurrence!=this||proceduralResultPC_!=r.pc||proceduralResultReturned_!=r.returned11C710)return false;
        if(!r.returned11C710)return !proceduralResultPayload58_&&
            proceduralResultStatus_==OriginalWreckType3Owners50LA::SourceResultStatus::bounded&&r.pc==0x11c980u;
        return proceduralResultStatus_==OriginalWreckType3Owners50LA::SourceResultStatus::reached&&
            r.pc==0x11ca80u&&r.returnPayload58&&proceduralResultPayload58_==r.returnPayload58;
    }

    bool proceduralShapeEntered_=false,proceduralShapeCallCompleted_=false,proceduralShapeWithdrawn_=false;
    std::optional<OriginalWreckType3Owners50LA::SourceShapeStatus> proceduralShapeStatus_;
    std::uint32_t proceduralShapePC_=0;
    Result beginSourceProceduralShape11C710()noexcept {
        if(!isProceduralE8()||proceduralShapeEntered_||nativeWithdrawn_||nativeRetired_||
           !proceduralLateReturned_||proceduralLatePC_!=0x166350u||!attachReturned_||attachPC_!=0x16632cu||
           detachedNative_||!preflightNative()||!collision_||!sentinel3E02C0_||
           !collision_->freshShapeNull())return {Status::owner,0x11c710u};
        proceduralShapeEntered_=true;
        const auto result=sourceType3_.beginSourceShape11C710(this,collision_->entity_,*sentinel3E02C0_,
            1u,nullptr,nullptr,collision_->shape8C_,collision_->shape88_,collision_->result90_);
        proceduralShapeCallCompleted_=true;proceduralShapeStatus_=result.status;proceduralShapePC_=result.sourcePC;
        return {result.status==OriginalWreckType3Owners50LA::SourceShapeStatus::owner?Status::owner:Status::bounded,
            proceduralShapePC_}; //Historical modePC166310 and latePC166350 remain unchanged.
    }
    bool proceduralShapeHistoryNative()const noexcept {
        const auto& s=sourceType3_.sourceShape_;
        if(!proceduralShapeEntered_)return !proceduralShapeCallCompleted_&&!proceduralShapeWithdrawn_&&
            !proceduralShapeStatus_&&!proceduralShapePC_&&!s.attempted;
        if(!isProceduralE8()||!proceduralLateReturned_||!proceduralShapeCallCompleted_||!proceduralShapeStatus_||
           !collision_||!sourceType3_.preflightSourceShapeNative())return false;
        if(!s.attempted)return !proceduralShapeWithdrawn_&&
            proceduralShapeStatus_==OriginalWreckType3Owners50LA::SourceShapeStatus::owner&&
            proceduralShapePC_==0x11c710u&&collision_->freshShapeNull();
        if(s.occurrence!=this||proceduralShapePC_!=s.pc||proceduralShapeWithdrawn_!=s.nativeWithdrawn||
           proceduralShapeStatus_!=OriginalWreckType3Owners50LA::SourceShapeStatus::bounded||
           collision_->shape88_!=3u||collision_->result90_!=std::optional<const void*>(proceduralResult90CurrentNative()))return false;
        if(s.nativeWithdrawn)return collision_->shape8C_==std::optional<const void*>(nullptr);
        return s.entity==&collision_->entity_&&s.sentinel==sentinel3E02C0_&&
            s.actual8C==&collision_->shape8C_&&s.actual88==&collision_->shape88_&&s.actual90==&collision_->result90_;
    }
    bool withdrawSourceProceduralShapeNative()noexcept {
        if(hasIncomingFiltersNative()||hasLiveOutgoingFiltersNative())return false;
        if(!proceduralShapeEntered_||proceduralShapeWithdrawn_)return true;
        if(attached_&&!detachedNative_)return false;
        if(!proceduralShapeHistoryNative())return false;
        if(!sourceType3_.sourceShape_.attempted)return true;
        if(!sourceType3_.withdrawSourceShapeNative(this))return false;
        proceduralShapeWithdrawn_=true;return true;
    }
    OriginalWreckCategoryList50LA sourceCategories_;
    OriginalCollisionIdentityList50LA sourceIdentities_;
    bool filterNativeWithdrawalStarted_=false,filterD4Returned_=false,filterCategoryEntered_=false;
    bool filterCategoryCallCompleted_=false,filterCategoryReturned_=false,filterEnabled_=false,filterBCReturned_=false;
    std::optional<std::uint32_t> filterCategoryArgument_;
    std::optional<std::uint8_t> filterFlagsBefore_;
    std::optional<OriginalWreckCategoryList50LA::SourceFilterStatus> filterCategoryStatus_;
    std::uint32_t filterD4PC_=0,filterCategoryPC_=0,filterEnablePC_=0,filterBCPC_=0;
    const void* filterContextE8History_=nullptr;
    struct SourceDirectedCall {
        OriginalSourcePhysicsCollision50LA* owner=nullptr;
        OriginalSourcePhysicsCollision50LA* target=nullptr;
        const B* rawTarget=nullptr;
        unsigned ownerIndex=0,incomingIndex=0;
        bool pinned=false,nativeUnpinned=false,callCompleted=false,returned=false;
        std::optional<OriginalCollisionIdentityList50LA::SourceFilterStatus> status;
        std::uint32_t pc=0;
    };
    std::array<SourceDirectedCall,2> filterEdges_;
    std::array<const SourceDirectedCall*,2> incomingFilters_{};
    unsigned filterEdgeCount_=0;
    bool hasIncomingFiltersNative()const noexcept {return incomingFilters_[0]||incomingFilters_[1];}
    bool hasLiveOutgoingFiltersNative()const noexcept {
        for(unsigned i=0;i<filterEdgeCount_;++i)if(filterEdges_[i].pinned)return true;
        return false;
    }
    bool sourceFilterLiveOwnerNative()const noexcept {
        if(filterNativeWithdrawalStarted_||nativeWithdrawn_||nativeRetired_||detachedNative_||!collision_||
           !constructorReturned_||!published_||!attachReturned_||!preflightNative())return false;
        if(isProceduralE8())return proceduralResultReturned_&&!proceduralShapeWithdrawn_&&
            !sourceType3_.sourceResultPrefix_.nativeWithdrawn;
        if(isDirectE4())return directMatrix_.returned&&!directMatrix_.leaseWithdrawn&&directFoundPayloadLiveNative();
        return transformReturned_&&!resultWithdrawn_&&!type0Withdrawn_&&foundFile_&&
            !foundFile_->nativeClosed_&&!foundFile_->associationWithdrawn_&&
            foundFile_->payloadCustodyCurrentNative(&sourceType0_);
    }
    Result configureSourceCategoryMode124160()noexcept {
        if(!isProceduralE8()||filterD4Returned_||!sourceFilterLiveOwnerNative())return {Status::owner,0x124160u};
        collision_->modeD4_=1;filterD4Returned_=true;filterD4PC_=0x166364u;
        return {Status::reached,filterD4PC_}; //Actual124164 JR-delay store, even priorvalue1.
    }
    Result appendSourceCategory124170()noexcept {
        if(!isProceduralE8()||filterCategoryEntered_||!filterD4Returned_||!sourceFilterLiveOwnerNative())
            return {Status::owner,0x124170u};
        filterCategoryEntered_=true;
        filterCategoryArgument_=base_->application_->collisionStartup3C81A0_.globals3E0708[2]; //166368, actual numeric category.
        const auto result=sourceCategories_.appendSource124170(collision_->entity_,collision_->listD8_,
            collision_->countDC_,*filterCategoryArgument_);
        filterCategoryCallCompleted_=true;filterCategoryStatus_=result.status;filterCategoryPC_=result.sourcePC;
        if(result.status==OriginalWreckCategoryList50LA::SourceFilterStatus::reached){
            filterCategoryReturned_=true;filterCategoryPC_=0x166374u;return {Status::reached,filterCategoryPC_};
        }
        return {result.status==OriginalWreckCategoryList50LA::SourceFilterStatus::owner?Status::owner:Status::bounded,filterCategoryPC_};
    }
    Result enableSourceProceduralCollision16638C()noexcept {
        if(!isProceduralE8()||filterEnabled_||!filterCategoryReturned_||!sourceFilterLiveOwnerNative())
            return {Status::owner,0x16638cu};
        filterFlagsBefore_=collision_->flagsF8_;collision_->flagsF8_=static_cast<std::uint8_t>(collision_->flagsF8_|1u);
        filterEnabled_=true;filterEnablePC_=0x166390u;return {Status::reached,filterEnablePC_};
    }
    Result setSourceFilterMode123EA0(OriginalSourcePhysicsCollision50LA& actualE8,
        OriginalSourcePhysicsCollision50LA& actualE0)noexcept {
        if(isProceduralE8()||filterBCReturned_||!sourceFilterLiveOwnerNative()||
           !actualE8.isProceduralE8()||actualE8.base_!=base_||!actualE8.filterEnabled_||!actualE8.sourceFilterLiveOwnerNative()||
           actualE0.occurrence_!=Occurrence::resultE0||actualE0.base_!=base_||
           (isDirectE4()?(!actualE0.filterBCReturned_||!actualE0.sourceFilterLiveOwnerNative()):&actualE0!=this))
            return {Status::owner,0x123ea0u};
        filterContextE8History_=&actualE8;collision_->modeBC_=1;filterBCReturned_=true;
        filterBCPC_=isDirectE4()?0x1663a8u:0x16639cu;return {Status::reached,filterBCPC_};
    }
    Result appendSourceExclusion123EB0(OriginalSourcePhysicsCollision50LA& actualTarget,
        OriginalSourcePhysicsCollision50LA& actualE8)noexcept {
        if(isProceduralE8()||filterEdgeCount_>=2||!filterBCReturned_||!sourceFilterLiveOwnerNative()||
           !actualE8.isProceduralE8()||actualE8.base_!=base_||filterContextE8History_!=&actualE8||
           !actualE8.filterEnabled_||!actualE8.sourceFilterLiveOwnerNative()||&actualTarget==this||
           actualTarget.base_!=base_||!actualTarget.sourceFilterLiveOwnerNative()||
           (filterEdgeCount_&&!filterEdges_[filterEdgeCount_-1].returned))return {Status::owner,0x123eb0u};
        const unsigned index=filterEdgeCount_;
        if(index==1){if(&actualTarget!=&actualE8)return {Status::owner,0x123eb0u};}
        else if(isDirectE4()){
            if(actualTarget.occurrence_!=Occurrence::resultE0||!actualTarget.filterBCReturned_||
               actualTarget.filterEdgeCount_!=2||!actualTarget.filterEdges_[0].returned||!actualTarget.filterEdges_[1].returned||
               actualTarget.filterEdges_[0].target!=this||actualTarget.filterEdges_[1].target!=&actualE8)
                return {Status::owner,0x123eb0u};
        }else if(!actualTarget.isDirectE4()||!actualTarget.filterBCReturned_||
                 actualTarget.filterContextE8History_!=&actualE8)return {Status::owner,0x123eb0u};
        unsigned incoming=0;while(incoming<2&&actualTarget.incomingFilters_[incoming])++incoming;
        if(incoming==2)return {Status::owner,0x123eb0u}; //Native receipt capacity, before source effects.
        auto& edge=filterEdges_[filterEdgeCount_++];edge.owner=this;edge.target=&actualTarget;
        edge.rawTarget=actualTarget.collision_.get();edge.ownerIndex=index;edge.incomingIndex=incoming;edge.pinned=true;
        actualTarget.incomingFilters_[incoming]=&edge; //Exact target custody BEFORE allocation/refusal.
        const auto result=sourceIdentities_.appendSource123EB0(collision_->entity_,collision_->listC0_,collision_->countC4_,edge.rawTarget);
        edge.callCompleted=true;edge.status=result.status;edge.pc=result.sourcePC;
        if(result.status==OriginalCollisionIdentityList50LA::SourceFilterStatus::reached){
            edge.returned=true;edge.pc=isDirectE4()?(index?0x1663d8u:0x1663ccu):(index?0x1663c0u:0x1663b4u);
            return {Status::reached,edge.pc};
        }
        return {result.status==OriginalCollisionIdentityList50LA::SourceFilterStatus::owner?Status::owner:Status::bounded,edge.pc};
    }
    bool sourceLocalListsCurrentNative()const noexcept {
        if(!sourceCategories_.preflightSourceFilterNative()||!sourceIdentities_.preflightSourceFilterNative())return false;
        const auto& cat=sourceCategories_.sourceFilter_;const auto& ids=sourceIdentities_.sourceFilter_;
        if(!collision_)return !cat.bound&&!ids.bound;
        if(cat.bound){
            if(cat.nativeWithdrawn){if(collision_->listD8_||collision_->countDC_!=cat.countHistory)return false;}
            else if(cat.entity!=&collision_->entity_||cat.actualAlias!=&collision_->listD8_||cat.actualCount!=&collision_->countDC_)return false;
        }else if(collision_->listD8_||collision_->countDC_)return false;
        if(ids.bound){
            if(ids.nativeWithdrawn){if(collision_->listC0_||collision_->countC4_!=ids.countHistory)return false;}
            else if(ids.entity!=&collision_->entity_||ids.actualAlias!=&collision_->listC0_||ids.actualCount!=&collision_->countC4_)return false;
        }else if(collision_->listC0_||collision_->countC4_)return false;
        return true;
    }
    bool filterPinsCurrentNative()const noexcept {
        if(filterEdgeCount_>2)return false;
        for(unsigned i=0;i<filterEdgeCount_;++i){const auto& edge=filterEdges_[i];
            if(edge.owner!=this||edge.ownerIndex!=i||!edge.rawTarget||!edge.callCompleted||!edge.status||
               edge.incomingIndex>=2||edge.pinned==edge.nativeUnpinned)return false;
            if(edge.nativeUnpinned){if(edge.target)return false;continue;}
            const auto* target=edge.target;
            if(!target||target==this||target->base_!=base_||target->nativeWithdrawn_||target->nativeRetired_||
               target->collision_.get()!=edge.rawTarget||!target->collision_||
               target->collision_->entity_.reference00!=1||target->collision_->application_!=base_->application_.get()||
               target->collision_->entity_.sessionGeneration!=base_->generation_||
               target->incomingFilters_[edge.incomingIndex]!=&edge)return false;
        }
        for(unsigned i=0;i<2;++i)if(const auto* edge=incomingFilters_[i]){
            const auto* owner=edge->owner;
            if(!owner||owner==this||owner->base_!=base_||owner->nativeWithdrawn_||owner->nativeRetired_||
               edge->ownerIndex>=owner->filterEdgeCount_||&owner->filterEdges_[edge->ownerIndex]!=edge||
               !edge->pinned||edge->nativeUnpinned||edge->target!=this||edge->incomingIndex!=i||
               edge->rawTarget!=collision_.get()||!owner->collision_||owner->collision_->entity_.reference00!=1)return false;
        }
        return true; //Structural only; never recursively preflight another owner.
    }
    bool sourceFilterHistoryNative()const noexcept {
        if(!sourceLocalListsCurrentNative()||!filterPinsCurrentNative())return false;
        if(filterD4Returned_){if(!isProceduralE8()||!proceduralResultReturned_||filterD4PC_!=0x166364u)return false;}
        else if(filterD4PC_||filterCategoryEntered_)return false;
        const auto& cat=sourceCategories_.sourceFilter_;
        if(filterCategoryEntered_){
            if(!filterCategoryCallCompleted_||!filterCategoryArgument_||!filterCategoryStatus_||!isProceduralE8())return false;
            if(!cat.callsEntered){if(filterCategoryReturned_||filterCategoryStatus_!=OriginalWreckCategoryList50LA::SourceFilterStatus::owner||
                filterCategoryPC_!=0x124170u)return false;}
            else {const auto& call=cat.calls[0];if(cat.callsEntered!=1||call.argument!=*filterCategoryArgument_||
                filterCategoryReturned_!=call.returned||filterCategoryPC_!=(call.returned?0x166374u:call.pc)||
                filterCategoryStatus_!=(call.returned?OriginalWreckCategoryList50LA::SourceFilterStatus::reached:
                    OriginalWreckCategoryList50LA::SourceFilterStatus::bounded))return false;}
        }else if(filterCategoryCallCompleted_||filterCategoryReturned_||filterCategoryArgument_||filterCategoryStatus_||filterCategoryPC_||cat.bound)return false;
        if(filterEnabled_){if(!filterCategoryReturned_||!filterFlagsBefore_||filterEnablePC_!=0x166390u||
            !collision_||collision_->flagsF8_!=std::uint8_t(*filterFlagsBefore_|1u))return false;}
        else if(filterFlagsBefore_||filterEnablePC_)return false;
        if(filterBCReturned_){if(isProceduralE8()||!filterContextE8History_||filterBCPC_!=(isDirectE4()?0x1663a8u:0x16639cu))return false;}
        else if(filterBCPC_||filterContextE8History_||filterEdgeCount_)return false;
        const auto& ids=sourceIdentities_.sourceFilter_;
        if(ids.callsEntered>filterEdgeCount_)return false;
        for(unsigned i=0;i<filterEdgeCount_;++i){const auto& edge=filterEdges_[i];
            if(i>=ids.callsEntered){if(edge.returned||edge.status!=OriginalCollisionIdentityList50LA::SourceFilterStatus::owner||
                edge.pc!=0x123eb0u||i+1!=filterEdgeCount_)return false;continue;}
            const auto& call=ids.calls[i];const auto returnPC=isDirectE4()?(i?0x1663d8u:0x1663ccu):(i?0x1663c0u:0x1663b4u);
            if(call.argument!=edge.rawTarget||edge.returned!=call.returned||edge.pc!=(call.returned?returnPC:call.pc)||
               edge.status!=(call.returned?OriginalCollisionIdentityList50LA::SourceFilterStatus::reached:
                 OriginalCollisionIdentityList50LA::SourceFilterStatus::bounded))return false;
        }
        return true;
    }
    bool withdrawSourceLocalFiltersNative()noexcept {
        if(!attempted_||nativeWithdrawn_||filterNativeWithdrawalStarted_)return true;
        if(!preflightNative())return false;
        //Incoming pins deliberately do not block outgoing withdrawal: E0/E4
        //cycles require this separate phase before any target detach/retirement.
        if(!sourceCategories_.withdrawSourceFilterNative()||!sourceIdentities_.withdrawSourceFilterNative())return false;
        for(unsigned i=0;i<filterEdgeCount_;++i){auto& edge=filterEdges_[i];
            edge.target->incomingFilters_[edge.incomingIndex]=nullptr;
            edge.target=nullptr;edge.pinned=false;edge.nativeUnpinned=true;
        }
        filterNativeWithdrawalStarted_=true;return true;
    }
    bool isProceduralE8()const noexcept{return occurrence_==Occurrence::proceduralE8;}
    bool proceduralHistoryCurrentNative()const noexcept {
        if(!isProceduralE8())return !proceduralPredecessor_;
        const auto* prior=proceduralPredecessor_;
        //Structural lifetime only: native E8 cancellation never needs E4 mesh/file readiness.
        return prior&&prior!=this&&prior->isDirectE4()&&prior->base_==base_&&
            prior->proceduralSuccessorBorrower_==this&&!prior->nativeWithdrawn_&&!prior->nativeRetired_&&
            prior->constructorReturned_&&prior->collision_&&prior->counted_&&prior->published_&&
            prior->attachReturned_&&prior->attachPC_==0x1662e4u&&!prior->detachedNative_&&
            prior->collision_->entity_.reference00==1&&prior->collision_->entity_.kind==K::collision11B190&&
            prior->collision_->application_==base_->application_.get()&&
            prior->collision_->entity_.sessionGeneration==base_->generation_&&
            prior->collision_->entity_.objectKey==Base::key(&prior->collision_->entity_)&&
            base_->object_->collisionE4==std::optional<B*>(prior->collision_.get())&&
            base_->directCollisionBorrower_==prior;
    }
    Result allocateSource1662E4(Base& actualBase,OriginalSourcePhysicsCollision50LA& actualE4)noexcept {
        if(attempted_||nativeWithdrawn_||&actualE4==this||!actualBase.preflightNative()||!actualBase.object_||
           actualBase.proceduralCollisionBorrower_||actualBase.object_->collisionE8||
           !actualE4.isDirectE4()||actualE4.base_!=&actualBase||actualE4.proceduralSuccessorBorrower_||
           !actualE4.attachReturned_||actualE4.attachPC_!=0x1662e4u||actualE4.detachedNative_||
           !actualE4.directMatrix_.returned||actualE4.directMatrix_.leaseWithdrawn||
           !actualE4.directFoundPayloadLiveNative()||!actualE4.preflightNative()||!actualE4.collision_||
           actualE4.nativeRetired_||actualE4.nativeWithdrawn_||
           actualBase.object_->collisionE4!=std::optional<B*>(actualE4.collision_.get())||
           actualBase.application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())
            return {Status::owner,0x1662e4u};
        occurrence_=Occurrence::proceduralE8;base_=&actualBase;attempted_=true;
        base_->proceduralCollisionBorrower_=this;proceduralPredecessor_=&actualE4;
        actualE4.proceduralSuccessorBorrower_=this;
        return constructSourceOuterNative(0x166304u); //Actual166300 nullable outerE8 publication.
    }
    Result configureSource166308()noexcept {
        if(!isProceduralE8()||nativeWithdrawn_||modeEntered_||!published_||pc_!=0x166304u||!preflightNative())
            return {Status::owner,0x166308u};
        modeEntered_=true;
        if(!collision_)return at(Status::bounded,0x11b574u);
        if(!collision_->setMode3_11B570(base_->application_->collisionGlobalValid3E0730_))
            return at(Status::bounded,0x11b570u);
        flagsF8Known_|=8u;modeReturned_=true;globalZeroStored_=true;
        //Original166324 attachment precedes9C/category/11C710. None is issued here.
        return at(Status::reached,0x166310u);
    }
    Result publishSourceProceduralCaller16632C()noexcept {
        if(!isProceduralE8()||proceduralLateEntered_||nativeWithdrawn_||nativeRetired_||
           !attachReturned_||attachPC_!=0x16632cu||detachedNative_||!preflightNative()||
           !base_||!base_->object_||!collision_||!counted_)return {Status::owner,0x16632cu};
        proceduralLateEntered_=true;proceduralLatePC_=0x16633cu;
        collision_->owner9C_=&base_->object_->entity;ownerStored_=true;
        proceduralLatePC_=0x166340u;
        category3E0718_=base_->application_->collisionStartup3C81A0_.globals3E0708[2];
        collision_->categoryA4_=category3E0718_;categoryStored_=true;
        proceduralLateReturned_=true;proceduralLatePC_=0x166350u;
        return {Status::reached,proceduralLatePC_}; //Historical modePC stays166310.
    }
    bool proceduralLateHistoryNative()const noexcept {
        if(!proceduralLateEntered_)return !proceduralLateReturned_&&!proceduralLatePC_&&
            (!isProceduralE8()||(!ownerStored_&&!categoryStored_));
        return isProceduralE8()&&attachReturned_&&attachPC_==0x16632cu&&modeReturned_&&
            proceduralLateReturned_&&proceduralLatePC_==0x166350u&&ownerStored_&&categoryStored_;
    }
    bool isDirectE4()const noexcept{return occurrence_==Occurrence::directE4;}
    std::optional<B*>& outerAliasNative()noexcept{return isProceduralE8()?base_->object_->collisionE8:isDirectE4()?base_->object_->collisionE4:base_->object_->collisionE0;}
    const std::optional<B*>& outerAliasNative()const noexcept{return isProceduralE8()?base_->object_->collisionE8:isDirectE4()?base_->object_->collisionE4:base_->object_->collisionE0;}
    const void*& baseBorrowerNative()noexcept{return isProceduralE8()?base_->proceduralCollisionBorrower_:isDirectE4()?base_->directCollisionBorrower_:base_->collisionBorrower_;}
    const void* baseBorrowerNative()const noexcept{return isProceduralE8()?base_->proceduralCollisionBorrower_:isDirectE4()?base_->directCollisionBorrower_:base_->collisionBorrower_;}
    bool directHistoryCurrentNative()const noexcept {
        if(!isDirectE4())return !directPredecessorToken_&&!directPredecessorHistory_&&!directModelArgument_&&!directModelPinned_;
        if(!directPredecessorHistory_||!directPredecessorToken_||*directPredecessorToken_!=this)return false;
        if(directModelPinned_)return bodyBorrow_&&directModelArgument_&&bodyBorrow_->base_==base_&&
            bodyBorrow_->directCollisionModelBorrower_==this&&bodyBorrow_->preflightNative()&&
            bodyBorrow_->sourceModel288Native()==directModelArgument_&&base_->object_->model14C==directModelArgument_;
        return !bodyBorrow_&&!directModelArgument_&&!nameEntered_;
    }
    template<class ActualDetector>
    Result allocateSource166250(Base& actualBase,ActualDetector& actualDetector)noexcept {
        if(attempted_||nativeWithdrawn_||!actualBase.preflightNative()||!actualBase.object_||
           actualBase.directCollisionBorrower_||actualBase.object_->collisionE4||
           actualDetector.base_!=&actualBase||actualDetector.directCollisionBorrower_||
           !actualDetector.attachReturned_||actualDetector.detachedNative_||!actualDetector.bindingReturned_||
           actualDetector.bindingNativeWithdrawn_||!actualDetector.preflightNative()||!actualDetector.storage_||
           actualDetector.nativeRetired_||actualDetector.nativeWithdrawn_||
           !actualBase.object_->detectorD8||*actualBase.object_->detectorD8!=&actualDetector.storage_->entity||
           actualBase.application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())
            return {Status::owner,0x166250u};
        occurrence_=Occurrence::directE4;base_=&actualBase;attempted_=true;base_->directCollisionBorrower_=this;
        directPredecessorToken_=&actualDetector.directCollisionBorrower_;directPredecessorHistory_=&actualDetector;
        *directPredecessorToken_=this;
        return constructSourceOuterNative(0x166270u); //Actual16626C nullable outerE4 publication.
    }
    Result configureSource166274()noexcept {
        if(!isDirectE4()||nativeWithdrawn_||modeEntered_||!published_||pc_!=0x166270u||!preflightNative())
            return {Status::owner,0x166274u};
        modeEntered_=true;
        if(!collision_)return at(Status::bounded,0x11b574u);
        if(!collision_->setMode5_11B570(base_->application_->collisionGlobalValid3E0730_))return at(Status::bounded,0x11b570u);
        flagsF8Known_|=8u;modeReturned_=true;globalZeroStored_=true;
        category3E0718_=base_->application_->collisionStartup3C81A0_.globals3E0708[2];
        collision_->categoryA4_=category3E0718_;categoryStored_=true; //166288; no9C/A0 source store.
        return at(Status::reached,0x16628cu);
    }
    Result prepareSourceDirectPath11B920(Body& actualBody,const void* actualModelBASE,std::string_view actualRequest)noexcept {
        if(!isDirectE4()||nativeWithdrawn_||directModelPinned_||nameEntered_||!modeReturned_||pc_!=0x16628cu||
           !preflightNative()||actualBody.base_!=base_||actualBody.directCollisionModelBorrower_||
           !actualBody.preflightNative()||actualBody.nativeWithdrawn_||actualBody.nativeRetired_||
           !actualBody.attachReturned_||actualBody.detachedNative_||!actualBody.sourceModel288Native()||
           actualModelBASE!=actualBody.sourceModel288Native()||base_->object_->model14C!=actualModelBASE||
           !actualRequest.data()||actualRequest.find('\0')!=std::string_view::npos)return {Status::owner,0x11b920u};
        bodyBorrow_=&actualBody;actualBody.directCollisionModelBorrower_=this;directModelPinned_=true;
        directModelArgument_=actualBody.sourceModel288Native(); //Native argument custody only; actualA0 remainsNULL.
        return prepareSourcePathBody11B920(actualRequest);
    }
    Result allocateSource166018(Base& actualBase,const Gravity& actualGravity)noexcept {
        if(attempted_||nativeWithdrawn_||!actualBase.preflightNative()||!actualBase.object_||
           actualBase.collisionBorrower_||actualBase.object_->collisionE0||
           actualGravity.base_!=&actualBase||!actualGravity.attachReturned_||actualGravity.detachedNative_||
           !actualGravity.preflightNative()||
           !actualGravity.gravity_||!actualGravity.targetReturned_||actualGravity.nativeRetired_||
           actualGravity.nativeWithdrawn_||!actualBase.object_->gravityD4||
           *actualBase.object_->gravityD4!=&actualGravity.gravity_->entity||
           actualBase.application_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())
            return {Status::owner,0x166018};
        //Same-owner prerequisite only, not proof of intervening141960 or
        //filename165FA0/166010 return. No root issues this constructor yet.
        base_=&actualBase;attempted_=true;base_->collisionBorrower_=this;
        return constructSourceOuterNative(0x166038u);
    }
    Result constructSourceOuterNative(std::uint32_t returnPC)noexcept {
        sentinel3E02C0_=&base_->application_->collisionStartup3C81A0_.vectors3E02C0[0];
        try{collision_=std::make_unique<B>();}catch(...){collision_.reset();}
        if(collision_){
            SpringVector4_48N sourceSentinel;
            std::memcpy(sourceSentinel.v,sentinel3E02C0_->data(),16);
            const auto r=collision_->construct11B190(*base_->application_,base_->generation_,
                Base::key(&collision_->entity_),sourceSentinel);
            counted_=collision_->phase_!=B::Phase::fresh;
            if(counted_){
                flags29Known_=0x0f;
                //Historical exact dispatch stores reached11B224/11B22C
                //before the constructor's fallible type-registry operation.
                dispatchFC_=0x3ccd00;dispatch10_=0x3ccd08;
            }
            if(r.status!=OriginalWreckComponentStatus50LA::complete)return at(Status::bounded,r.sourcePC);
            constructorReturned_=true;returnedType02_=collision_->type02_;flagsF8Known_=0x37;
        }
        publishedIdentity_=collision_.get();outerAliasNative()=publishedIdentity_;published_=true;
        return at(Status::reached,returnPC); //Nullable per-occurrence outer publication.
    }
    struct SourceDetectorBinding {
        const void* const* successorToken=nullptr; //Native Detector E4 pin; never source data.
        const void* occurrence=nullptr;
        const void* contact=nullptr;
        const void* contactOccurrence=nullptr;
        const void* body=nullptr;
        std::optional<const void*> beforeOwner,beforeBody,beforeCallback;
        std::uint32_t beforeMode=0;
        std::optional<std::uint32_t> callbackPC;
        bool attempted=false,ownerStored=false,bodyStored=false,callbackStored=false,modeStored=false;
        bool nativeWithdrawn=false;
    } detectorBinding_;
    bool detectorBindingCurrentNative()const noexcept {
        const auto& h=detectorBinding_;
        if(!h.attempted)return !h.successorToken&&!h.occurrence&&!h.contact&&!h.contactOccurrence&&!h.body&&!h.ownerStored&&!h.bodyStored&&
            !h.callbackStored&&!h.modeStored&&!h.nativeWithdrawn&&!h.callbackPC;
        if(!h.occurrence||!h.contact||!h.contactOccurrence||sourceContactHistory_!=h.contactOccurrence||!h.body||!collision_||!h.beforeOwner||!h.beforeBody||!h.beforeCallback||
           *h.beforeOwner!=&base_->object_->entity||*h.beforeBody!=h.body||*h.beforeCallback||h.beforeMode!=1||
           h.callbackPC!=(h.callbackStored?std::optional<std::uint32_t>(0x294bb0u):std::nullopt)||
           (h.bodyStored&&!h.ownerStored)||(h.callbackStored&&!h.bodyStored)||(h.modeStored&&!h.callbackStored))return false;
        if(h.nativeWithdrawn)return !h.successorToken&&collision_->owner9C_==h.beforeOwner&&collision_->bodyA0_==h.beforeBody&&
            collision_->callbacksE4_[0]==h.beforeCallback&&collision_->modeBC_==h.beforeMode;
        return sourceContactBorrower_==h.contactOccurrence&&collision_->owner9C_==(h.ownerStored?std::optional<const void*>(h.contact):h.beforeOwner)&&
            collision_->bodyA0_==(h.bodyStored?std::optional<const void*>(h.body):h.beforeBody)&&
            (h.callbackStored?!collision_->callbacksE4_[0]:collision_->callbacksE4_[0]==h.beforeCallback)&&
            collision_->modeBC_==(h.modeStored?1u:h.beforeMode);
    }
    bool pinSourceDetector2955F0(const void* occurrence,const void* contactOccurrence,const void* contact,const void* body)noexcept {
        auto& h=detectorBinding_;
        if(!occurrence||!contactOccurrence||!contact||!body||h.attempted||sourceContactBorrower_!=contactOccurrence||!attachReturned_||detachedNative_||
           !transformReturned_||!preflightNative()||!bodyBorrow_||body!=&bodyBorrow_->body_->entity)return false;
        h.occurrence=occurrence;h.contactOccurrence=contactOccurrence;h.contact=contact;h.body=body;h.beforeOwner=collision_->owner9C_;
        h.beforeBody=collision_->bodyA0_;h.beforeCallback=collision_->callbacksE4_[0];h.beforeMode=collision_->modeBC_;
        h.attempted=true;return true; //Native lifetime pin only, no source store/retain.
    }
    bool publishSourceDetector295604(const void* occurrence)noexcept {
        auto& h=detectorBinding_;
        if(h.occurrence!=occurrence||h.nativeWithdrawn||h.ownerStored||!detectorBindingCurrentNative())return false;
        collision_->owner9C_=h.contact;h.ownerStored=true; //295604
        collision_->bodyA0_=h.body;h.bodyStored=true; //295614, same raw BodyBASE
        collision_->callbacksE4_[0].reset();h.callbackPC=0x294bb0u;h.callbackStored=true; //29561C, typed source PC294BB0 below
        collision_->modeBC_=1;h.modeStored=true; //123EA4 JR-delay store
        return true;
    }
    bool withdrawSourceDetectorBindingNative(const void* occurrence)noexcept {
        auto& h=detectorBinding_;
        if(!h.attempted)return true;
        if(h.occurrence!=occurrence||!detectorBindingCurrentNative())return false;
        if(h.nativeWithdrawn)return true;
        if(h.successorToken&&*h.successorToken)return false;
        if(!preflightNative())return false;
        collision_->owner9C_=h.beforeOwner;collision_->bodyA0_=h.beforeBody;
        collision_->callbacksE4_[0]=h.beforeCallback;collision_->modeBC_=h.beforeMode;
        h.successorToken=nullptr;h.nativeWithdrawn=true;return true; //Native alias/callback withdrawal, never replayed source stores.
    }
    bool bodyBorrowCurrentNative()const noexcept {
        if(isDirectE4())return bodyStored_==directBodyStored_&&directHistoryCurrentNative()&&
            (!constructorReturned_||!collision_||collision_->bodyA0_==std::optional<const void*>(
                directBodyStored_?static_cast<const void*>(&bodyBorrow_->body_->entity):nullptr));
        if(!bodyBorrow_)return !bodyStored_;
        return bodyStored_&&bodyBorrow_->base_==base_&&bodyBorrow_->collisionBodyBorrower_==this&&
            bodyBorrow_->preflightNative()&&bodyBorrow_->body_&&bodyBorrow_->counted_&&
            !bodyBorrow_->nativeRetired_&&!bodyBorrow_->nativeWithdrawn_&&
            collision_->bodyA0_==std::optional<const void*>(&bodyBorrow_->body_->entity);
    }
    Result configureSource16603C(OriginalSourcePhysicsBody50LA& actualBody)noexcept {
        if(occurrence_!=Occurrence::resultE0||modeEntered_||!preflightNative()||pc_!=0x166038||!published_||
           actualBody.base_!=base_||!actualBody.attachReturned_||actualBody.detachedNative_||
           !actualBody.preflightNative()||!actualBody.body_||
           !actualBody.disableReturned_||!actualBody.counted_||actualBody.nativeRetired_||
           actualBody.nativeWithdrawn_||actualBody.collisionBodyBorrower_||
           !base_->object_->bodyD0||*base_->object_->bodyD0!=&actualBody.body_->entity)
            return {Status::owner,0x16603c};
        modeEntered_=true;
        if(!collision_)return at(Status::bounded,0x11b574); //First actualF8 read onNULL.
        //11B594/5C0/5E4 then jump-delay11B5F4 clears SAME process byte0.
        if(!collision_->setMode3_11B570(base_->application_->collisionGlobalValid3E0730_))
            return at(Status::bounded,0x11b570);
        flagsF8Known_|=8u;modeReturned_=true;globalZeroStored_=true;
        category3E0718_=base_->application_->collisionStartup3C81A0_.globals3E0708[2];
        collision_->categoryA4_=category3E0718_;categoryStored_=true; //166054, globals spaced8.
        collision_->owner9C_=&base_->object_->entity;ownerStored_=true; //16605C, BASE identity.
        bodyBorrow_=&actualBody;actualBody.collisionBodyBorrower_=this;
        collision_->bodyA0_=&actualBody.body_->entity;bodyStored_=true; //166068, rawD0/no retain.
        return at(Status::reached,0x166074); //Before11BBC0→11B920 path/file routine.
    }
    Result prepareSourcePath11B920(std::string_view actualRequest)noexcept {
        if(occurrence_!=Occurrence::resultE0||nameEntered_||!preflightNative()||!modeReturned_||!bodyStored_||pc_!=0x166074||
           !actualRequest.data()||actualRequest.find('\0')!=std::string_view::npos)
            return {Status::owner,0x11b920};
        return prepareSourcePathBody11B920(actualRequest);
    }
    Result prepareSourcePathBody11B920(std::string_view actualRequest)noexcept {
        nameEntered_=true;
        try{requestName_.assign(actualRequest.data(),actualRequest.size());}
        catch(...){return at(Status::bounded,0x11b934);} //Native capture before source path stores.
        requestCaptured_=true;
        const auto& directory=base_->application_->collisionDirectory36B0F0_;
        directoryIdentity_=&directory;directoryNonempty_=!directory.empty();directoryRead_=true;
        if(directory.find('\0')!=std::string::npos)return at(Status::bounded,0x11b938);
        if(directoryNonempty_){
            if(directory.size()+1>path_.size())return at(Status::bounded,0x11b954);
            std::memcpy(path_.data(),directory.c_str(),directory.size()+1);
            pathLength_=directory.size();pathCopied_=true;
            if(path_[pathLength_-1]!='\\'){
                if(pathLength_+2>path_.size())return at(Status::bounded,0x11b990);
                path_[pathLength_++]='\\';path_[pathLength_]='\0';separatorAppended_=true;
            }
            if(requestName_.size()>path_.size()-pathLength_-1)return at(Status::bounded,0x11b99c);
            std::memcpy(path_.data()+pathLength_,requestName_.c_str(),requestName_.size()+1);
            pathLength_+=requestName_.size();requestCopied_=true;
        }else {
            if(requestName_.size()+1>path_.size())return at(Status::bounded,0x11b9b0);
            std::memcpy(path_.data(),requestName_.c_str(),requestName_.size()+1);
            pathLength_=requestName_.size();pathCopied_=true;requestCopied_=true;
        }
        //1169D0 is last-dot over the entire source string, not basename-only.
        std::size_t dot=pathLength_;
        for(std::size_t i=0;i<pathLength_;++i)if(path_[i]=='.')dot=i;
        dotFound_=dot!=pathLength_;dotOffset_=dot;dotReturned_=true;
        const auto extensionAt=dotFound_?dot:pathLength_;
        if(extensionAt+sizeof(".col")>path_.size())
            return at(Status::bounded,dotFound_?0x11b9d4:0x11b9ec);
        std::memcpy(path_.data()+extensionAt,".col",sizeof(".col"));
        pathLength_=extensionAt+4;extensionStored_=true;
        //11B9FC copies ORIGINAL request, not normalized path, into shared
        //3E0730. Later methods may overwrite it; no restoration on cleanup.
        if(requestName_.size()+1>base_->application_->collisionName3E0730_.size())
            return at(Status::bounded,0x11b9fc);
        std::memcpy(base_->application_->collisionName3E0730_.data(),requestName_.c_str(),requestName_.size()+1);
        sharedNameStored_=true;
        sourceHandle148_=-1; //11BA10 signed64 stack output initializer.
        sourceCache150_=nullptr; //11BA30 call-delay initializer, not a source resource.
        try{lookupName_.assign(path_.data(),pathLength_);}
        catch(...){return at(Status::bounded,0x11ba2c);} //No source search performed.
        nameReturned_=true;return at(Status::reached,0x11ba2c);
    }
    Result acceptSourceSearch11BA2C(
        std::shared_ptr<rr::assets::OriginalSourcePsgMissingLookup50LA> actualSearch,
        const rr::assets::OriginalStandardTextureResolver50LA* actualResolver,
        const rr::assets::OriginalRaTrackSourceFile50LA* actualTrack)noexcept {
        if(searchAccepted_||!nameReturned_||pc_!=0x11ba2c||!preflightNative()||
           !actualSearch||actualSearch->nativeClosed()||!actualSearch->preflightNative()||
           actualSearch->continuationAttempted_||actualSearch->fileBorrower_||actualSearch->collisionSearchBorrower_||
           actualSearch->collisionContinuationAttempted_||actualSearch->collisionFileBorrower_||
           actualSearch->requestedName()!=lookupName_||actualSearch->resolver()!=actualResolver||
           actualSearch->root()!=actualTrack)return {Status::owner,0x11ba2c};
        //Root authenticates SAME Application/track/resolver and issues the
        //existing generic132820 search leaf once. The PSG-named service has
        //not opened/adopted any file/resource before its132988 found boundary.
        const bool actualMissing=actualSearch->liveMissingNative();
        search_=std::move(actualSearch);searchAccepted_=true;
        search_->collisionSearchBorrower_=this;searchBorrowed_=true;
        if(actualMissing){
            searchReturnV0_=0;return at(Status::reached,0x11ba34); //Only FileManager return, not11B920.
        }
        return at(Status::bounded,search_->sourcePC_); //Found132988 or unsupported nonzero mode.
    }
    Result continueSourceMissing11BA34()noexcept {
        if(missingEntered_||!preflightNative()||pc_!=0x11ba34||!searchAccepted_||!searchBorrowed_||
           !search_||search_->nativeClosed()||search_->collisionSearchBorrower_!=this||
           !search_->returnedMissing132820()||searchReturnV0_!=0||!collision_||
           !collision_->shape8C_||*collision_->shape8C_)
            return {Status::owner,0x11ba34};
        missingEntered_=true;warningAttempted_=true;
        //Native output adapter for actual1336C0; its result is NOT a source
        //condition. The source argument is the original11B920 request, not
        //the directory-prefixed/replaced lookup path or a provider alias.
        warningNativeResult_=std::fprintf(stderr,"Warning: No collision file for '%s'\n",requestName_.c_str());
        warningReturned_=true;
        loader11B920ReturnV0_=0;loader11B920Returned_=true; //11BA50→11BBA8 epilogue.
        if(isDirectE4())return at(Status::reached,0x11bbbcu); //Direct11B920 caller, no11BBC0 effects.
        //11BBC0 does NOT check the returnedzero before reading the shape.
        callerShape8C_=*collision_->shape8C_;callerShapeRead_=true; //11BBE0, actualNULL.
        resultAllocationArgument_=0x80; //11BBE4 register preparation, not allocation.
        //11BBEC is the JAL's delay-slot store throughNULL8C+54. Bound that
        //actual use BEFORE entering2E8D90: no result allocation or54 store.
        callerDelayStoreBound_=true;
        return at(Status::bounded,0x11bbec);
    }
    bool missingHistoryCurrentNative()const noexcept {
        if(!missingEntered_)return !warningAttempted_&&!warningReturned_&&!warningNativeResult_&&
            (foundCloseReturned_?(loader11B920Returned_&&loader11B920ReturnV0_==1):
             (!loader11B920Returned_&&!loader11B920ReturnV0_))&&!callerShapeRead_&&!callerShape8C_&&
            !resultAllocationArgument_&&!callerDelayStoreBound_;
        if(isDirectE4())return searchAccepted_&&search_&&search_->returnedMissing132820()&&searchReturnV0_==0&&
            warningAttempted_&&warningReturned_&&warningNativeResult_&&loader11B920Returned_&&loader11B920ReturnV0_==0&&
            !callerShapeRead_&&!callerShape8C_&&!resultAllocationArgument_&&!callerDelayStoreBound_&&
            (directLateEntered_?directLateHistoryCurrentNative():pc_==0x11bbbcu);
        return searchAccepted_&&search_&&search_->returnedMissing132820()&&searchReturnV0_==0&&
            warningAttempted_&&warningReturned_&&warningNativeResult_&&loader11B920Returned_&&
            loader11B920ReturnV0_==0&&callerShapeRead_&&callerShape8C_&&!*callerShape8C_&&
            resultAllocationArgument_==0x80&&callerDelayStoreBound_&&collision_&&
            collision_->shape8C_&&!*collision_->shape8C_&&pc_==0x11bbec;
    }
    Result continueSourceFound11BA34()noexcept {
        if(isDirectE4()&&(!directModelPinned_||!directModelArgument_||!bodyBorrow_||
           bodyBorrow_->sourceModel288Native()!=directModelArgument_))return {Status::owner,0x11ba34u};
        if(foundEntered_||missingEntered_||!searchAccepted_||!searchBorrowed_||searchReturnV0_||
           !search_||!search_->found_||pc_!=0x132988||!preflightNative())return {Status::owner,0x11ba34};
        foundEntered_=true;
        rr::assets::OriginalStandardTextureDiagnostic50LA d;
        if(!File::continueFound132988(search_,this,foundFile_,d))return at(Status::bounded,d.sourcePC);
        if(!foundFile_->returnedCold_||!foundFile_->sourceHandle148_||
           !foundFile_->outputCacheStored_)return at(Status::bounded,0x11ba34);
        foundOutputReturned_=true;foundHandle148_=foundFile_->sourceHandle148_;
        foundCache150_=foundFile_->sourceCache150_; //Distinct from original initializer history.
        //Original ignores all three read return COUNTS. Real EOF-short returns
        //continue to the next read. A native refusal is not an original return.
        static constexpr std::uint32_t calls[3]={0x11ba88,0x11baa4,0x11bac0};
        for(unsigned i=0;i<3;++i){
            pc_=calls[i];const bool returned=foundFile_->read12FB30(header_[i].data(),4,headerReads_[i],d);
            if(headerReads_[i].done>4)return at(Status::bounded,calls[i]);
            headerKnown_[i]=static_cast<std::uint8_t>(headerReads_[i].done);
            if(!returned)return at(Status::bounded,d.sourcePC);
        }
        headerReadsReturned_=true;
        //Stack154/158/15C are source-unknown until written. Native storage
        //zeroes are never evidence for a short read's untouched bytes.
        if(headerKnown_[0]!=4)return at(Status::bounded,0x11bac8);
        const auto word=[](const std::array<std::uint8_t,4>& b)noexcept{
            return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);
        };
        headerMagicCompared_=true;headerMagicMatched_=word(header_[0])==0x4d2u;
        bool okay=headerMagicMatched_;
        if(okay){
            constexpr std::uint8_t expected[4]={'c','o','l',0};
            headerTagEntered_=true;
            for(unsigned i=0;i<4;++i){
                if(i>=headerKnown_[1])return at(Status::bounded,0x308f00);
                auto actual=header_[1][i];
                //Actual308F00 ASCII case-fold. Non-ASCII ctype dependency is
                //bounded without claiming a mismatch/assertion return.
                if(actual>=0x80)return at(Status::bounded,0x308f00);
                if(actual>='A'&&actual<='Z')actual=std::uint8_t(actual+('a'-'A'));
                if(actual!=expected[i]){okay=false;break;}
            }
            headerTagReturned_=true;headerTagMatched_=okay;
        }
        if(okay){
            if(headerKnown_[2]!=4)return at(Status::bounded,0x11baf0);
            headerVersionCompared_=true;headerVersionMatched_=word(header_[2])==1u;okay=headerVersionMatched_;
        }
        headerDecisionReached_=true;headerValid_=okay;
        if(!okay){
            //Actual invalid-header route reaches shared assertion11BB00.
            //Its return/disabled policy is not implemented here. Do NOT fake
            //source0/close/repair; if it later returns,11BB74 closes then still
            //falls through B0 store and124F80 with a closed numeric handle.
            return at(Status::bounded,0x11bb00);
        }
        //Only actual valid-header branch reaches this store in this packet.
        collision_->wordB0_=static_cast<std::uint64_t>(*foundHandle148_);foundB0Stored_=true;
        return at(Status::bounded,0x124f80); //No loader/close/11B920 return yet.
    }
    bool foundHistoryCurrentNative()const noexcept {
        if(!foundEntered_)return !foundFile_&&!foundOutputReturned_&&!foundHandle148_&&!foundCache150_&&
            !headerReadsReturned_&&!headerMagicCompared_&&!headerTagEntered_&&!headerTagReturned_&&
            !headerVersionCompared_&&!headerDecisionReached_&&!foundB0Stored_;
        if(!searchAccepted_||!search_||!search_->found_||searchReturnV0_||missingEntered_)return false;
        if(!foundFile_)return !foundOutputReturned_&&!headerReadsReturned_&&!foundB0Stored_;
        if(!foundFile_->preflightNative()||foundFile_->search_.get()!=search_.get()||
           foundFile_->collisionOwner_!=this)return false;
        if(foundOutputReturned_){
            if(!foundFile_->returnedCold_||foundHandle148_!=foundFile_->sourceHandle148_||
               !foundCache150_||*foundCache150_||foundCache150_!=foundFile_->sourceCache150_)return false;
        }else if(foundHandle148_||foundCache150_||headerReadsReturned_||foundB0Stored_)return false;
        bool all=true;
        for(unsigned i=0;i<3;++i){const auto& r=headerReads_[i];
            if(headerKnown_[i]>4||headerKnown_[i]!=r.done||
               (r.entered&&(!foundOutputReturned_||r.requested!=4||(i&&!headerReads_[i-1].returned)))||
               (!r.entered&&(r.returned||r.done||r.afterKnown)))return false;
            all=all&&r.returned;
        }
        if(headerReadsReturned_!=all)return false;
        if(headerMagicCompared_&&(!all||headerKnown_[0]!=4))return false;
        if(headerTagEntered_&&!headerMagicMatched_)return false;
        if(headerTagReturned_&&!headerTagEntered_)return false;
        if(headerVersionCompared_&&(!headerTagReturned_||!headerTagMatched_||headerKnown_[2]!=4))return false;
        if(headerDecisionReached_&&!headerMagicCompared_)return false;
        if(foundB0Stored_&&(!headerDecisionReached_||!headerValid_||!headerVersionMatched_||
            !foundHandle148_||(!type0Entered_&&pc_!=0x124f80)))return false;
        return true;
    }
    Result continueSourceType0Prefix124F80()noexcept {
        if(type0Entered_||!foundB0Stored_||!headerValid_||pc_!=0x124f80||!preflightNative()||
           !foundFile_||!foundFile_->current()||foundFile_->payloadBorrower_||!bodyBorrow_)
            return {Status::owner,0x124f80};
        const auto* model=isDirectE4()?directModelArgument_:bodyBorrow_->sourceModel288Native();
        if(!model||base_->object_->model14C!=static_cast<const void*>(model)||
           (isDirectE4()&&(!directModelPinned_||bodyBorrow_->sourceModel288Native()!=model)))
            return {Status::owner,0x124f80};
        //The native interval pin precedes every reached cleanup/store/read or
        //nullable shape allocation. It is not a source file/resource retain.
        type0Entered_=true;type0Model_=model;foundFile_->payloadBorrower_=&sourceType0_;
        const auto read=[&](void* output,std::size_t requested,std::size_t& done)noexcept {
            done=0;
            if(type0ReadCount_>=type0Reads_.size()||foundFile_->payloadBorrower_!=&sourceType0_||
               !foundFile_->current())return false;
            auto& receipt=type0Reads_[type0ReadCount_++];
            rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
            const bool returned=foundFile_->read12FB30(output,requested,receipt,diagnostic);
            done=receipt.done;return returned; //Short genuine calls continue; no exactness test.
        };
        const auto result=sourceType0_.beginSourceType0Prefix124F80(foundFile_,*model,
            collision_->entity_,collision_->shape88_,collision_->shape8C_,collision_->result90_,read);
        type0CallReturned_=true;type0Status_=result.status;
        return at(Status::bounded,result.sourcePC); //No125890/124F80 source return exists yet.
    }
    Result continueSourceType0Arrays125AB4()noexcept {
        if(arrayEntered_||!type0Entered_||type0Withdrawn_||!sourceType0_.sourcePrefix_.scalarReturned||
           !preflightNative()||!foundFile_||!foundFile_->current()||foundFile_->payloadBorrower_!=&sourceType0_)
            return {Status::owner,0x125ab4};
        arrayEntered_=true;
        const auto read=[&](void* output,std::size_t requested,std::size_t& done)noexcept {
            done=0;
            //Native bookkeeping must not erase already committed source
            //allocation/cookie/publication effects if it cannot grow.
            try{arrayReads_.emplace_back();}
            catch(...){arrayMetadataRefused_=true;arrayRefusedIndex_=arrayReads_.size();
                arrayRefusedRequested_=requested;return false;}
            auto& receipt=arrayReads_.back();receipt.requested=requested;
            if(foundFile_->payloadBorrower_!=&sourceType0_||!foundFile_->current())return false;
            rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
            const bool returned=foundFile_->read12FB30(output,requested,receipt,diagnostic);
            receipt.diagnostic=diagnostic;done=receipt.done;return returned;
        };
        const auto result=sourceType0_.continueSourceType0Arrays125AB4(*base_->application_,read);
        arrayCallReturned_=true;arrayStatus_=result.status;
        return at(Status::bounded,result.sourcePC); //No source125890/124F80 return.
    }
    bool arrayHistoryCurrentNative()const noexcept {
        const auto& a=sourceType0_.sourceArrays_;
        if(!arrayEntered_)return !arrayCallReturned_&&!arrayStatus_&&!arrayMetadataRefused_&&
            !arrayRefusedIndex_&&!arrayRefusedRequested_&&arrayReads_.empty()&&!a.attempted;
        if(!type0Entered_||!sourceType0_.sourcePrefix_.scalarReturned||!arrayCallReturned_||!arrayStatus_)return false;
        if(!a.attempted)return arrayStatus_==OriginalWreckType0Owners50LA::SourcePrefixStatus::owner&&
            arrayReads_.empty()&&!arrayMetadataRefused_&&pc_==sourceType0_.sourcePrefix_.pc;
        if(!base_||a.application!=base_->application_.get()||
           arrayStatus_!=OriginalWreckType0Owners50LA::SourcePrefixStatus::bounded||(!nodeEntered_&&pc_!=a.pc)||
           a.readAttempts>a.reads.size()||arrayReads_.size()+(arrayMetadataRefused_?1u:0u)!=a.readAttempts||
           (type0Withdrawn_&&!a.nativeWithdrawn))return false;
        for(std::size_t i=0;i<arrayReads_.size();++i){const auto& r=arrayReads_[i];const auto& source=a.reads[i];
            if(r.requested!=source.requested||r.done!=source.done||r.returned!=source.returned||
               r.done>r.requested||(!r.entered&&(r.done||r.returned||r.afterKnown)))return false;
        }
        if(arrayMetadataRefused_){
            if(arrayRefusedIndex_!=arrayReads_.size()||arrayRefusedIndex_>=a.readAttempts)return false;
            const auto& source=a.reads[arrayRefusedIndex_];
            if(source.requested!=arrayRefusedRequested_||source.done||source.returned)return false;
        }else if(arrayRefusedIndex_||arrayRefusedRequested_)return false;
        return true;
    }
    Result continueSourceType0NodesFaces12602C()noexcept {
        if(nodeEntered_||!arrayEntered_||!arrayCallReturned_||type0Withdrawn_||
           (pc_!=0x12602c&&pc_!=0x126030)||
           !sourceType0_.sourceArrays_.normalsReturned||!preflightNative()||!foundFile_||
           !foundFile_->current()||foundFile_->payloadBorrower_!=&sourceType0_)
            return {Status::owner,0x12602c};
        nodeEntered_=true;
        const auto read=[&](void* output,std::size_t requested,std::size_t& done)noexcept {
            done=0;
            try{nodeReads_.emplace_back();}
            catch(...){nodeMetadataRefused_=true;nodeRefusedIndex_=nodeReads_.size();
                nodeRefusedRequested_=requested;return false;}
            auto& receipt=nodeReads_.back();receipt.requested=requested;
            if(foundFile_->payloadBorrower_!=&sourceType0_||!foundFile_->current())return false;
            rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
            const bool returned=foundFile_->read12FB30(output,requested,receipt,diagnostic);
            receipt.diagnostic=diagnostic;done=receipt.done;return returned;
        };
        const auto result=sourceType0_.continueSourceType0NodesFaces12602C(read);
        nodeCallReturned_=true;nodeStatus_=result.status;
        //Only the actual loop epilogue is a control return. No source V0
        //literal, resource125014 or overall collision setup return is minted.
        return at(result.status==OriginalWreckType0Owners50LA::SourcePrefixStatus::reached?
            Status::reached:Status::bounded,result.sourcePC);
    }
    bool nodeHistoryCurrentNative()const noexcept {
        const auto& n=sourceType0_.sourceNodes_;
        if(!nodeEntered_)return !nodeCallReturned_&&!nodeStatus_&&!nodeMetadataRefused_&&
            !nodeRefusedIndex_&&!nodeRefusedRequested_&&nodeReads_.empty()&&!n.attempted;
        if(!arrayEntered_||!sourceType0_.sourceArrays_.normalsReturned||!nodeCallReturned_||!nodeStatus_)return false;
        if(!n.attempted)return nodeStatus_==OriginalWreckType0Owners50LA::SourcePrefixStatus::owner&&
            nodeReads_.empty()&&!nodeMetadataRefused_&&pc_==sourceType0_.sourceArrays_.pc;
        if((!resourceEntered_&&pc_!=n.pc)||n.readAttempts>n.reads.size()||
           nodeReads_.size()+(nodeMetadataRefused_?1u:0u)!=n.readAttempts||
           (type0Withdrawn_&&!n.nativeWithdrawn))return false;
        if(n.returned125890){
            if(nodeStatus_!=OriginalWreckType0Owners50LA::SourcePrefixStatus::reached||(!resourceEntered_&&pc_!=0x126410))return false;
        }else if(nodeStatus_!=OriginalWreckType0Owners50LA::SourcePrefixStatus::bounded)return false;
        for(std::size_t i=0;i<nodeReads_.size();++i){const auto& r=nodeReads_[i];const auto& source=n.reads[i];
            if(r.requested!=source.requested||r.done!=source.done||r.returned!=source.returned||
               r.done>r.requested||(!r.entered&&(r.done||r.returned||r.afterKnown)))return false;
        }
        if(nodeMetadataRefused_){
            if(nodeRefusedIndex_!=nodeReads_.size()||nodeRefusedIndex_>=n.readAttempts)return false;
            const auto& source=n.reads[nodeRefusedIndex_];
            if(source.requested!=nodeRefusedRequested_||source.done||source.returned)return false;
        }else if(nodeRefusedIndex_||nodeRefusedRequested_)return false;
        return true;
    }
    Result continueSourceType0Resource125014()noexcept {
        if(resourceEntered_||!nodeCallReturned_||!sourceType0_.sourceNodes_.returned125890||
           pc_!=0x126410||type0Withdrawn_||!preflightNative()||!foundFile_||!foundFile_->current()||
           foundFile_->payloadBorrower_!=&sourceType0_)return {Status::owner,0x125014};
        resourceEntered_=true;
        const auto result=sourceType0_.continueSourceType0Resource125014();
        resourceCallReturned_=true;resourceStatus_=result.status;pc_=result.sourcePC;
        if(result.status!=OriginalWreckType0Owners50LA::SourcePrefixStatus::reached)
            return {result.status==OriginalWreckType0Owners50LA::SourcePrefixStatus::owner?Status::owner:Status::bounded,pc_};
        auto& r=sourceType0_.sourceResource_;
        if(!r.storesReturned||r.pc!=0x1250d8||!r.pending||
           !sourceType0_.preflightSourceType0PrefixNative())return at(Status::bounded,0x1250d8);
        associationEntered_=true;
        rr::assets::OriginalStandardTextureDiagnostic50LA d;
        if(!foundFile_->associateSource1326A0(*r.pending,&sourceType0_,d))return at(Status::bounded,d.sourcePC);
        //1250EC branches to the124F80 epilogue, preserving actual1326A0 V0.
        loader124F80ReturnV0_=foundFile_->associationReturnV0_;
        loader124F80Returned_=true;return at(Status::reached,0x125488);
    }
    Result closeSourceFound11BB9C()noexcept {
        if(foundCloseEntered_||!loader124F80Returned_||loader124F80ReturnV0_!=1||
           pc_!=0x125488||type0Withdrawn_||!preflightNative()||!foundFile_->current())
            return {Status::owner,0x11bb9c};
        foundCloseEntered_=true;
        rr::assets::OriginalStandardTextureDiagnostic50LA d;
        if(!foundFile_->closeSource131040(d))return at(Status::bounded,d.sourcePC);
        foundCloseReturned_=true;
        //The original close return is ignored;11BBA4 explicitly sets V0=1.
        loader11B920ReturnV0_=1;loader11B920Returned_=true;
        return at(Status::reached,0x11bbbc); //No11BBC0 result-buffer allocation yet.
    }
    bool resourceHistoryCurrentNative()const noexcept {
        const auto& r=sourceType0_.sourceResource_;
        if(!resourceEntered_)return !resourceCallReturned_&&!resourceStatus_&&!associationEntered_&&
            !loader124F80Returned_&&!loader124F80ReturnV0_&&!foundCloseEntered_&&!foundCloseReturned_&&!r.attempted;
        if(!nodeCallReturned_||!sourceType0_.sourceNodes_.returned125890||!resourceCallReturned_||
           !resourceStatus_||!sourceType0_.preflightSourceType0ResourceNative()||!foundFile_)return false;
        if(!r.attempted)return resourceStatus_==OriginalWreckType0Owners50LA::SourcePrefixStatus::owner&&
            !associationEntered_&&!loader124F80Returned_&&!foundCloseEntered_&&pc_==0x126410;
        if(type0Withdrawn_&&!r.nativeWithdrawn)return false;
        if(!r.storesReturned)return resourceStatus_==OriginalWreckType0Owners50LA::SourcePrefixStatus::bounded&&
            !associationEntered_&&!loader124F80Returned_&&!foundCloseEntered_&&pc_==r.pc;
        if(resourceStatus_!=OriginalWreckType0Owners50LA::SourcePrefixStatus::reached)return false;
        if(!associationEntered_)return !loader124F80Returned_&&!foundCloseEntered_&&pc_==0x1250d8;
        if(foundFile_->associationAttempted_&&
           (foundFile_->associatedResource_!=r.pending.get()||foundFile_->resourceOwner_!=&sourceType0_))return false;
        if(!loader124F80Returned_)return !loader124F80ReturnV0_&&!foundCloseEntered_&&
            !foundFile_->associationReturned_;
        if(loader124F80ReturnV0_!=1||!foundFile_->associationReturned_||foundFile_->associationReturnV0_!=1)return false;
        if(!foundCloseEntered_)return !foundCloseReturned_&&pc_==0x125488;
        if(!foundCloseReturned_)return !foundFile_->sourceCloseReturned_&&!loader11B920Returned_;
        return foundFile_->sourceCloseReturned_&&foundFile_->sourceCloseResult_==1&&
            loader11B920Returned_&&loader11B920ReturnV0_==1&&
            (isDirectE4()?(directLateEntered_?directLateHistoryCurrentNative():pc_==0x11bbbcu):
                (resultEntered_||pc_==0x11bbbcu));
    }
    Result continueSourceResult11BBE0(std::uint32_t actualArgument)noexcept {
        if(occurrence_!=Occurrence::resultE0||resultEntered_||actualArgument!=0||!foundCloseReturned_||!loader11B920Returned_||loader11B920ReturnV0_!=1||
           pc_!=0x11bbbc||type0Withdrawn_||!preflightNative()||!collision_||
           !collision_->shape8C_||!*collision_->shape8C_||!collision_->result90_||*collision_->result90_)
            return {Status::owner,0x11bbe0};
        if(!sourceType0_.pinSourceResult11BBE0(&sourceResultOwner_))return {Status::owner,0x11bbe0};
        resultEntered_=true;resultBorrowed_=true;
        const auto result=sourceResultOwner_.allocateSourceResult11BBE8(*collision_->shape8C_,actualArgument,
            [&](const void* shape,std::uint32_t argument)noexcept {
                return sourceType0_.storeSourceShape54_11BBEC(&sourceResultOwner_,shape,argument);},
            [&](const void* shape,std::uint32_t& count)noexcept {
                return sourceType0_.readSourceShape48_11BC3C(&sourceResultOwner_,shape,count);},
            [&](const void* resultIdentity)noexcept {
                return sourceType0_.publishSourceResult90_11BD18(&sourceResultOwner_,resultIdentity);});
        resultCallReturned_=true;resultStatus_=result.status;pc_=result.sourcePC;
        if(result.status==OriginalWreckType2Owners50LA::SourceResultStatus::reached)return {Status::reached,pc_};
        return {result.status==OriginalWreckType2Owners50LA::SourceResultStatus::owner?Status::owner:Status::bounded,pc_};
    }
    bool resultHistoryCurrentNative()const noexcept {
        if(isDirectE4()&&resultEntered_)return false; //Direct11B920 never authorizes11BBC0/result90.
        const auto& r=sourceResultOwner_.sourceResult_;const auto& l=sourceType0_.sourceResult_;
        if(!resultEntered_)return !resultCallReturned_&&!resultBorrowed_&&!resultWithdrawn_&&!resultStatus_&&
            !r.attempted&&!l.attempted;
        if(!foundCloseReturned_||!loader11B920Returned_||loader11B920ReturnV0_!=1||
           !resultCallReturned_||!resultStatus_||!sourceResultOwner_.preflightSourceResultNative()||
           !sourceType0_.preflightSourceResultShapeNative()||!l.attempted||l.occurrence!=&sourceResultOwner_||
           resultBorrowed_!=l.live||resultWithdrawn_!=l.nativeWithdrawn)return false;
        if(!r.attempted)return resultStatus_==OriginalWreckType2Owners50LA::SourceResultStatus::owner&&
            !l.store54Returned&&!l.read48Attempted&&!l.published90&&pc_==0x11bbe0;
        if((!transformEntered_&&pc_!=r.pc)||r.shapeIdentity!=l.shapeIdentity||r.rawArgument||r.shape54Stored!=l.store54Returned||
           (r.shape54Stored&&l.argument54!=r.rawArgument)||r.countRead!=l.read48Known||
           (r.countRead&&r.count!=l.value48)||r.published!=l.published90||
           (r.published&&r.outerIdentity!=l.result90History)||(resultWithdrawn_&&!r.nativeWithdrawn))return false;
        return resultStatus_==(r.returned?OriginalWreckType2Owners50LA::SourceResultStatus::reached:
            OriginalWreckType2Owners50LA::SourceResultStatus::bounded);
    }
    bool withdrawSourceResultNative()noexcept {
        if(hasIncomingFiltersNative()||hasLiveOutgoingFiltersNative())return false;
        if(sourceContactBorrower_||(detectorBinding_.attempted&&!detectorBinding_.nativeWithdrawn))return false;
        if(!resultEntered_||resultWithdrawn_)return true;
        if(!resultHistoryCurrentNative()||!sourceResultOwner_.withdrawSourceResultNative())return false;
        //The raw backing is gone, but opaque publication history/token remain
        //until this exact unpin succeeds; retries never inspect retired bytes.
        if(!sourceType0_.unpinSourceResultNative(&sourceResultOwner_))return false;
        resultBorrowed_=false;resultWithdrawn_=true;return true;
    }
    Result continueSourceTransform11F570()noexcept {
        if(transformEntered_||!resultCallReturned_||resultWithdrawn_||!resultBorrowed_||
           !sourceResultOwner_.sourceResult_.returned||sourceResultOwner_.sourceResult_.nativeWithdrawn||
           pc_!=0x11bd30||type0Withdrawn_||!preflightNative()||!collision_||collision_->shape88_!=0||
           !foundFile_||!foundFile_->associationReturned_||foundFile_->associationWithdrawn_||
           !sourceType0_.sourceResult_.live||!bodyBorrow_||bodyBorrow_->sourceModel288Native()!=type0Model_)
            return {Status::owner,0x11f570};
        transformEntered_=true;transformModel_=type0Model_;pc_=0x11f5b4;
        const auto world=bodyBorrow_->copySourceModelWorld28B550(transformIncoming_,transformKnown_);
        transformWorldCallCompleted_=true;transformWorldStatus_=world.status;transformWorldPC_=world.sourcePC;
        if(world.status!=Body::Status::reached||world.sourcePC!=0x28b67c||transformKnown_!=0xffffu)
            return at(Status::bounded,world.sourcePC);
        transformWorldReturned_=true;pc_=0x11f5c0;transformPassEntered_=true;
        const auto pass=sourceType0_.continueSourceType0Transform11ED60(*transformModel_,transformIncoming_,
            transformKnown_,collision_->cachedCenter60_,collision_->radius80_);
        transformPassCallCompleted_=true;transformPassStatus_=pass.status;pc_=pass.sourcePC;
        if(pass.status!=OriginalWreckType0Owners50LA::SourcePrefixStatus::reached)
            return {Status::bounded,pc_}; //Native refusal is not an original callee return.
        const auto& t=sourceType0_.sourceTransform_;
        if(!t.returned||t.pc!=0x11fe28)return at(Status::bounded,pass.sourcePC);
        //Only after actual second-pass completion:11F680 common epilogue,
        //11ED60 branch/epilogue, then11F570 branch/epilogue. No booleanV0.
        transformNestedReturned_=true; //11FE3C/11FE40.
        transformDirectReturned_=true; //11EF2C→11F55C/11F560.
        transformReturned_=true;return at(Status::reached,0x11f678);
    }
    bool transformHistoryCurrentNative()const noexcept {
        const auto& t=sourceType0_.sourceTransform_;
        if(!transformEntered_)return !transformModel_&&!transformWorldCallCompleted_&&!transformWorldStatus_&&
            !transformWorldPC_&&!transformWorldReturned_&&!transformKnown_&&!transformPassEntered_&&
            !transformPassCallCompleted_&&!transformPassStatus_&&!transformNestedReturned_&&
            !transformDirectReturned_&&!transformReturned_&&(!t.attempted||directMatrix_.attempted);
        if(!resultCallReturned_||!sourceResultOwner_.sourceResult_.returned||!transformModel_||
           transformModel_!=type0Model_||!transformWorldCallCompleted_||!transformWorldStatus_||
           !sourceType0_.preflightSourceType0TransformNative())return false;
        if(!transformWorldReturned_)return !transformPassEntered_&&!transformPassCallCompleted_&&!transformPassStatus_&&
            !transformNestedReturned_&&!transformDirectReturned_&&!transformReturned_&&!t.attempted&&
            !transformKnown_&&pc_==transformWorldPC_;
        if(transformWorldStatus_!=Body::Status::reached||transformWorldPC_!=0x28b67c||transformKnown_!=0xffffu||
           !transformPassEntered_||!transformPassCallCompleted_||!transformPassStatus_)return false;
        if(!t.attempted)return transformPassStatus_==OriginalWreckType0Owners50LA::SourcePrefixStatus::owner&&
            !transformNestedReturned_&&!transformDirectReturned_&&!transformReturned_&&pc_==0x11ed60;
        if(t.model!=transformModel_||t.incoming!=transformIncoming_||t.incomingKnown!=transformKnown_||
           (!t.nativeWithdrawn&&(t.center!=&collision_->cachedCenter60_||t.radius!=&collision_->radius80_))||
           (type0Withdrawn_&&!t.nativeWithdrawn)||!sourceType0_.sourceType0TransformOutputsCurrentNative(
               collision_->cachedCenter60_,collision_->radius80_))return false;
        if(!transformReturned_)return !transformNestedReturned_&&!transformDirectReturned_&&!t.returned&&
            transformPassStatus_==OriginalWreckType0Owners50LA::SourcePrefixStatus::bounded&&pc_==t.pc;
        return transformNestedReturned_&&transformDirectReturned_&&t.returned&&t.pc==0x11fe28&&
            transformPassStatus_==OriginalWreckType0Owners50LA::SourcePrefixStatus::reached&&pc_==0x11f678;
    }
    bool transformOutputsCurrentNative()const noexcept {
        if(!collision_)return false;
        if(sourceType0_.sourceTransform_.attempted)return sourceType0_.sourceType0TransformOutputsCurrentNative(
            collision_->cachedCenter60_,collision_->radius80_);
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        return !collision_->radius80_&&!std::memcmp(collision_->cachedCenter60_.v,vf0.data(),16);
    }
    bool sourceContactLeaseCurrentNative()const noexcept {
        return sourceContactBorrower_==sourceType0_.sourceContactBorrower_&&
            sourceContactHistory_==sourceType0_.sourceContactHistory_&&sourceType0_.preflightSourceContactLeaseNative();
    }
    bool pinSourceContactPoints296190(const void* occurrence,const OriginalRacerSceneObject50LA* model)noexcept {
        if(!occurrence||sourceContactHistory_||nativeWithdrawn_||!attachReturned_||detachedNative_||
           !transformReturned_||type0Withdrawn_||resultWithdrawn_||!bodyBorrow_||
           model!=type0Model_||bodyBorrow_->sourceModel288Native()!=model||!preflightNative()||
           !foundFile_||!foundFile_->associationReturned_||foundFile_->associationWithdrawn_)return false;
        if(!sourceType0_.pinSourceContactPoints296190(occurrence,model))return false;
        sourceContactBorrower_=sourceContactHistory_=occurrence;return true;
    }
    bool sourceContactPointCustodyNative(const void* occurrence)const noexcept {
        //Pinned immutable source owners: no Contact callback and no recursive
        //whole-loop validation on each raw point load.
        return occurrence&&sourceContactBorrower_==occurrence&&!nativeWithdrawn_&&base_&&bodyBorrow_&&
            !type0Withdrawn_&&bodyBorrow_->sourceModel288Native()==type0Model_&&
            foundFile_&&foundFile_->associationReturned_&&!foundFile_->associationWithdrawn_&&
            sourceType0_.sourceContactViewCurrentNative(occurrence);
    }
    bool readSourceContactCount2962F0(const void* occurrence,std::array<std::uint8_t,4>& bytes,std::uint8_t& known)const noexcept {
        OriginalWreckType0Owners50LA::SourcePrefixWord word;
        if(!sourceContactPointCustodyNative(occurrence)||!sourceType0_.readSourceContactCount2962F0(occurrence,word))return false;
        bytes=word.bytes;known=word.known;return true;
    }
    bool readSourceContactPacked296200(const void* occurrence,std::array<std::uint8_t,4>& bytes,std::uint8_t& known)const noexcept {
        OriginalWreckType0Owners50LA::SourcePrefixWord word;
        if(!sourceContactPointCustodyNative(occurrence)||!sourceType0_.readSourceContactPacked296200(occurrence,word))return false;
        bytes=word.bytes;known=word.known;return true;
    }
    bool readSourceContactVertex296224(const void* occurrence,std::uint32_t index,unsigned axis,bool packed,
        std::array<std::uint8_t,4>& bytes,std::uint8_t& known)const noexcept {
        OriginalWreckType0Owners50LA::SourcePrefixWord word;
        if(!sourceContactPointCustodyNative(occurrence)||!sourceType0_.readSourceContactVertex296224(occurrence,index,axis,packed,word))return false;
        bytes=word.bytes;known=word.known;return true;
    }
    bool unpinSourceContactPointsNative(const void* occurrence)noexcept {
        if(detectorBinding_.attempted&&!detectorBinding_.nativeWithdrawn)return false;
        if(!sourceContactHistory_)return true;
        if(occurrence!=sourceContactHistory_||!preflightNative())return false;
        if(!sourceType0_.unpinSourceContactPointsNative(occurrence))return false;
        sourceContactBorrower_=nullptr;return true;
    }
    bool type0HistoryCurrentNative()const noexcept {
        if(!type0Entered_)return !type0Model_&&!type0CallReturned_&&!type0Withdrawn_&&!type0Status_&&
            !type0ReadCount_&&!sourceType0_.sourcePrefix_.attempted&&
            (!foundFile_||!foundFile_->payloadBorrower_);
        if(!foundFile_||!collision_||!bodyBorrow_||!type0Model_||!type0CallReturned_||!type0Status_||
           !foundB0Stored_||!headerValid_||type0ReadCount_>type0Reads_.size()||
           !sourceType0_.preflightSourceType0PrefixNative())return false;
        const auto& s=sourceType0_.sourcePrefix_;
        if(type0Withdrawn_){
            if(foundFile_->payloadBorrower_||(s.attempted&&!s.nativeWithdrawn))return false;
        }else {
            if(!foundFile_->payloadCustodyCurrentNative(&sourceType0_)||
               bodyBorrow_->sourceModel288Native()!=type0Model_)return false;
            if(s.attempted&&(s.file!=foundFile_||s.model!=type0Model_||s.entity!=&collision_->entity_||
               s.actual88!=&collision_->shape88_||s.actual8C!=&collision_->shape8C_||s.actual90!=&collision_->result90_))return false;
        }
        if(s.attempted){
            if(collision_->shape88_!=s.discriminator.bits()||(!arrayEntered_&&pc_!=s.pc)||
               type0Status_!=OriginalWreckType0Owners50LA::SourcePrefixStatus::bounded||
               type0ReadCount_!=s.readsEntered)return false;
            for(unsigned i=0;i<type0Reads_.size();++i){const auto& r=type0Reads_[i];
                if(i<type0ReadCount_){
                    if(r.requested!=4||r.done!=s.reads[i].done||r.returned!=s.reads[i].returned||!r.entered)return false;
                }else if(r.entered||r.returned||r.done||r.requested||r.afterKnown)return false;
            }
        }else if(type0ReadCount_||type0Status_!=OriginalWreckType0Owners50LA::SourcePrefixStatus::owner||pc_!=0x124f80)return false;
        return true;
    }
    bool withdrawSourceType0Native()noexcept {
        if(hasIncomingFiltersNative()||hasLiveOutgoingFiltersNative())return false;
        if(proceduralSuccessorBorrower_)return false;
        if(sourceContactBorrower_||(detectorBinding_.attempted&&!detectorBinding_.nativeWithdrawn))return false;
        if(attached_&&!detachedNative_)return false;
        if(!type0Entered_||type0Withdrawn_)return true;
        if(!type0HistoryCurrentNative()||!arrayHistoryCurrentNative()||!nodeHistoryCurrentNative()||
           !resourceHistoryCurrentNative()||!transformHistoryCurrentNative()||!directMatrixHistoryNative()||
           !withdrawSourceResultNative()||!withdrawSourceDirectMatrixNative())return false;
        rr::assets::OriginalStandardTextureDiagnostic50LA d;
        if(!foundFile_->withdrawAssociationNative(d))return false;
        if(!sourceType0_.withdrawSourceType0PrefixNative())return false;
        //No refusal between native shape withdrawal and exact interval unpin.
        foundFile_->payloadBorrower_=nullptr;type0Withdrawn_=true;return true;
    }
    bool setupHistoryCurrentNative()const noexcept {
        if(!sourceFilterHistoryNative()||!proceduralResultHistoryNative()||!proceduralShapeHistoryNative()||!proceduralLateHistoryNative()||!proceduralHistoryCurrentNative()||!directMatrixHistoryNative()||!sourceContactLeaseCurrentNative()||!detectorBindingCurrentNative()||!directHistoryCurrentNative()||!directLateHistoryCurrentNative()||!directFoundTransformHistoryNative())return false;
        if(!bodyBorrowCurrentNative()||!missingHistoryCurrentNative()||!foundHistoryCurrentNative()||!type0HistoryCurrentNative()||!arrayHistoryCurrentNative()||!nodeHistoryCurrentNative()||!resourceHistoryCurrentNative()||!resultHistoryCurrentNative()||!transformHistoryCurrentNative())return false;
        if(isProceduralE8()){
            if(nameEntered_||search_||searchAccepted_||bodyBorrow_||bodyStored_||
               modeReturned_!=globalZeroStored_)return false;
            if(!modeEntered_)return !modeReturned_;
            if(!collision_)return !modeReturned_&&pc_==0x11b574u;
            return pc_==(modeReturned_?0x166310u:0x11b570u);
        }
        if(modeReturned_!=globalZeroStored_||modeReturned_!=categoryStored_)return false;
        if(isDirectE4()){if(ownerStored_!=directOwnerStored_||bodyStored_!=directBodyStored_)return false;}
        else if(modeReturned_!=ownerStored_||modeReturned_!=bodyStored_)return false;
        if(!modeEntered_)return !modeReturned_&&!nameEntered_&&!search_&&!searchAccepted_;
        if(!collision_)return !modeReturned_&&!nameEntered_&&!search_&&pc_==0x11b574;
        if(!modeReturned_)return !nameEntered_&&!search_&&pc_==0x11b570;
        if(!nameEntered_)return !search_&&!searchAccepted_&&pc_==(isDirectE4()?0x16628cu:0x166074u);
        if(directoryRead_&&(!requestCaptured_||directoryIdentity_!=&base_->application_->collisionDirectory36B0F0_))return false;
        if(pathCopied_&&(!directoryRead_||pathLength_>=path_.size()||path_[pathLength_]))return false;
        if(separatorAppended_&&(!pathCopied_||!directoryNonempty_))return false;
        if(requestCopied_&&!pathCopied_)return false;
        if(dotReturned_&&(!requestCopied_||dotOffset_>=path_.size()))return false;
        if(extensionStored_&&!dotReturned_)return false;
        if(sharedNameStored_&&(!extensionStored_||sourceHandle148_!=-1||
           !sourceCache150_||*sourceCache150_))return false;
        if(nameReturned_&&(!sharedNameStored_||lookupName_.size()!=pathLength_||
           std::memcmp(lookupName_.data(),path_.data(),pathLength_)))return false;
        if(searchAccepted_){
            if(!nameReturned_||!search_||!search_->preflightNative()||search_->requestedName()!=lookupName_||
               search_->continuationAttempted_||search_->fileBorrower_)return false;
            if(searchBorrowed_){if(search_->collisionSearchBorrower_!=this||search_->nativeClosed())return false;}
            else if(!search_->nativeClosed()||search_->collisionSearchBorrower_)return false;
            if(searchReturnV0_)return *searchReturnV0_==0&&search_->returnedMissing132820()&&
                (missingEntered_?(isDirectE4()?(directLateEntered_?directLateHistoryCurrentNative():pc_==0x11bbbcu):pc_==0x11bbecu):pc_==0x11ba34);
            return (foundEntered_||pc_==search_->sourcePC_)&&!search_->sourceReturnV0();
        }
        return !search_&&!searchReturnV0_&&!searchBorrowed_;
    }
    C*& sourceAttachmentChildNative()const noexcept {
        return isProceduralE8()?base_->proceduralCollisionChild141960_:isDirectE4()?base_->directCollisionChild141960_:base_->collisionChild141960_;
    }
    const B*& sourceAttachmentOuterNative()const noexcept {
        return isProceduralE8()?base_->proceduralCollisionChildOuter141960_:isDirectE4()?base_->directCollisionChildOuter141960_:base_->collisionChildOuter141960_;
    }
    template<class ActualAttach>
    Result attachSourceCollision141960(ActualAttach&& attach)noexcept {
        const auto callPC=isProceduralE8()?0x166324u:isDirectE4()?0x1662dcu:0x166098u;
        if(attachEntered_||nativeWithdrawn_||nativeRetired_||pc_!=(isProceduralE8()?0x166310u:0x11f678u)||type0Withdrawn_||
           (isProceduralE8()?(!modeReturned_||proceduralLateEntered_):isDirectE4()?(!directMatrix_.returned||directMatrix_.leaseWithdrawn||!directFoundPayloadLiveNative()||
              !searchBorrowed_||!search_||search_->nativeClosed()||search_->collisionSearchBorrower_!=this||
              !search_->preflightNative()):(!transformReturned_||resultWithdrawn_))||
           !preflightNative()||!base_||!base_->object_||!collision_||!counted_||!published_||
           sourceAttachmentChildNative()||sourceAttachmentOuterNative())return {Status::owner,callPC};
        auto& child=collision_->entity_;auto& parent=base_->object_->entity;
        if(child.parent1C||child.previous10||child.next14||child.firstChild18||child.mask20||child.descendants24)
            return {Status::owner,callPC};
        attachEntered_=true;attachLock_=base_->application_->attachmentLock423F58();attachPC_=0x141970;
        if(!*attachLock_){
            const auto result=attach(parent,child,0u,*attachLock_);
            if(result.status!=OriginalWreckComponentStatus50LA::complete){attachPC_=result.sourcePC;return {Status::bounded,attachPC_};}
            sourceAttachmentChildNative()=&child;sourceAttachmentOuterNative()=collision_.get();attached_=true;
        }
        //All three original callers ignore actual141960's locked NULL return.
        attachReturned_=true;attachPC_=isProceduralE8()?0x16632cu:isDirectE4()?0x1662e4u:0x1660a0u;return {Status::reached,attachPC_};
    }
    bool attachmentHistoryNative()const noexcept {
        if(!attachEntered_)return !attachReturned_&&!attached_&&!detachedNative_&&!attachLock_&&!attachPC_&&
            (!base_||(!sourceAttachmentChildNative()&&!sourceAttachmentOuterNative()));
        if(!base_||!base_->object_||!collision_||(isProceduralE8()?!modeReturned_:isDirectE4()?!directMatrix_.returned:!transformReturned_)||!attachLock_||
           (detachedNative_&&!attached_)||(*attachLock_&&attached_))return false;
        if(attachReturned_&&((*attachLock_?attached_:!attached_)||attachPC_!=(isProceduralE8()?0x16632cu:isDirectE4()?0x1662e4u:0x1660a0u)))return false;
        const auto& child=collision_->entity_;const bool live=attached_&&!detachedNative_;
        return sourceAttachmentChildNative()==(live?&child:nullptr)&&
            sourceAttachmentOuterNative()==(live?collision_.get():nullptr)&&base_->sourceChildChainCurrentNative()&&
            child.parent1C==(live?&base_->object_->entity:nullptr)&&
            (live||(!child.previous10&&!child.next14))&&!child.firstChild18&&!child.mask20&&!child.descendants24;
    }
    template<class ActualDetach>
    bool detachSourceCollisionNative(ActualDetach&& detach)noexcept {
        if(hasIncomingFiltersNative()||hasLiveOutgoingFiltersNative())return false;
        if(proceduralSuccessorBorrower_||sourceContactBorrower_||(detectorBinding_.attempted&&!detectorBinding_.nativeWithdrawn))return false;
        if(!attempted_||nativeWithdrawn_||!attached_||detachedNative_)return true;
        if(!base_||!base_->application_||base_->application_->attachmentLock423F58()||!preflightNative())return false;
        const auto result=detach(collision_->entity_,0u);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        sourceAttachmentChildNative()=nullptr;sourceAttachmentOuterNative()=nullptr;detachedNative_=true;return true;
    }
    C* countedEntityNative()const noexcept {
        return collision_&&counted_&&!nativeRetired_?&collision_->entity_:nullptr;
    }
    //One actual +78 call occurrence. NULL is observed, never source-cleared.
    struct SourceNullMembership120290 {
        const void* updateOccurrence=nullptr;
        const void* ownerIdentity=nullptr;
        bool entered=false,observedNull=false,returned=false;
        std::uint32_t pc=0;
    } nullMembership_;
    Result removeSourceNullMembership120290(const void* actualUpdateOccurrence)noexcept {
        if(!actualUpdateOccurrence||nullMembership_.entered||isProceduralE8()||
           !sourceFilterLiveOwnerNative()||!collision_->membershipE0_)
            return {Status::owner,0x120290u};
        auto& h=nullMembership_;h.updateOccurrence=actualUpdateOccurrence;
        h.ownerIdentity=collision_.get();h.entered=true;h.pc=0x1202a0u;
        //Actual LW E0 at1202A0. Current source constructor/custody proves known NULL.
        if(*collision_->membershipE0_)return {Status::bounded,h.pc=0x1202acu};
        h.observedNull=true;h.returned=true;h.pc=0x1202d4u;
        return {Status::reached,h.pc}; //JR1202CC/delay1202D0, no literal V0.
    }
    bool nullMembershipHistoryNative()const noexcept {
        const auto& h=nullMembership_;
        if(!h.entered)return !h.updateOccurrence&&!h.ownerIdentity&&!h.observedNull&&!h.returned&&!h.pc;
        if(!h.updateOccurrence||!h.ownerIdentity||isProceduralE8())return false;
        if(h.returned?(!h.observedNull||h.pc!=0x1202d4u):(h.observedNull||h.pc!=0x1202acu))return false;
        //After cancellation the retained identities are opaque, never dereferenced.
        if(nativeWithdrawn_)return true;
        return collision_&&h.ownerIdentity==collision_.get()&&collision_->membershipE0_&&
            (h.observedNull?*collision_->membershipE0_==nullptr:*collision_->membershipE0_!=nullptr);
    }
    bool preflightNative()const noexcept {
        if(!nullMembershipHistoryNative())return false;
        if(!attempted_)return !base_&&!collision_&&!counted_&&!published_&&!nativeWithdrawn_;
        if(nativeWithdrawn_)return !base_&&!collision_&&!proceduralPredecessor_&&!proceduralSuccessorBorrower_&&!sentinel3E02C0_&&!directPredecessorToken_&&!bodyBorrow_&&!search_&&!searchBorrowed_&&!foundFile_&&(!counted_||nativeRetired_);
        if(!attachmentHistoryNative())return false;
        if(!base_||baseBorrowerNative()!=this||!base_->preflightNative()||!base_->object_||
           sentinel3E02C0_!=&base_->application_->collisionStartup3C81A0_.vectors3E02C0[0])return false;
        if(published_&&(!outerAliasNative()||*outerAliasNative()!=publishedIdentity_))return false;
        if(!published_&&outerAliasNative())return false;
        if(!setupHistoryCurrentNative())return false;
        if(!collision_)return !counted_&&!constructorReturned_&&!nativeRetired_&&published_&&!publishedIdentity_;
        const auto& c=*collision_;const auto& e=c.entity_;
        if(c.application_!=base_->application_.get()||c.field00_||c.flags03_.knownMask!=7||
           (c.flags03_.value&7)||c.appended_)return false;
        if(!counted_)return !constructorReturned_&&!published_&&!nativeRetired_&&
            c.phase_==B::Phase::fresh&&!e.reference00&&e.kind==K::unbound&&
            c.type02_==0xff&&!dispatchFC_&&!dispatch10_&&!flags29Known_&&!flagsF8Known_&&!returnedType02_;
        if(nativeRetired_||e.reference00!=1||e.kind!=K::collision11B190||
           e.sessionGeneration!=base_->generation_||e.objectKey!=Base::key(&e)||
           ((!attached_||detachedNative_)&&(e.previous10||e.next14||e.parent1C))||
           e.firstChild18||e.mask20||e.descendants24||
           e.nativeFlags29||e.category28!=0xff||(e.flags29&0x0f)!=3||flags29Known_!=0x0f||
           dispatchFC_!=0x3ccd00||dispatch10_!=0x3ccd08||base_->application_->objectCount423F60()<2||
           c.wordB0_!=(foundB0Stored_?static_cast<std::uint64_t>(*foundHandle148_):~std::uint64_t{0}))return false;
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000}};
        if(!constructorReturned_)return c.phase_==B::Phase::entity&&!published_&&!returnedType02_&&
            !flagsF8Known_&&c.type02_==0xff&&!std::memcmp(c.copied40_.v,vf0.data(),16)&&
            !std::memcmp(c.copied50_.v,vf0.data(),16)&&!std::memcmp(c.cachedCenter60_.v,vf0.data(),16)&&
            !std::memcmp(c.previous70_.v,vf0.data(),16);
        if(c.phase_!=B::Phase::complete||!published_||publishedIdentity_!=&c||(!modeEntered_&&pc_!=(isProceduralE8()?0x166304u:isDirectE4()?0x166270u:0x166038u))||
           !returnedType02_||c.type02_!=*returnedType02_||flagsF8Known_!=(modeReturned_?0x3f:0x37)||
           (c.flagsF8_&flagsF8Known_)!=(modeReturned_?(isDirectE4()?4u:filterEnabled_?3u:2u):6u)||
           !transformOutputsCurrentNative()||(!type0Entered_&&!proceduralShapeEntered_&&c.shape88_)||!c.shape8C_||(!type0Entered_&&!proceduralShapeEntered_&&*c.shape8C_)||!c.result90_||
           *c.result90_!=(isProceduralE8()?proceduralResult90CurrentNative():resultEntered_&&!resultWithdrawn_&&sourceResultOwner_.sourceResult_.published?
               sourceResultOwner_.sourceResult_.outerIdentity:nullptr)||
           !c.owner9C_||*c.owner9C_!=(detectorBinding_.ownerStored&&!detectorBinding_.nativeWithdrawn?detectorBinding_.contact:(ownerStored_?static_cast<const void*>(&base_->object_->entity):nullptr))||
           !c.bodyA0_||(!bodyStored_&&*c.bodyA0_)||!c.membershipE0_||*c.membershipE0_||
           c.current94_||c.field98_||c.categoryA4_!=(categoryStored_?category3E0718_:0u)||c.fieldA8_||c.fieldA9_||c.fieldF4_||
           c.acceptB8_!=0xffffffffu||c.modeBC_!=1||c.modeC8_!=1||c.modeD4_!=1||
           c.listCC_||c.countD0_||!sourceLocalListsCurrentNative())return false;
        for(unsigned i=0;i<c.callbacksE4_.size();++i){const auto& callback=c.callbacksE4_[i];
            if(i==0&&detectorBinding_.callbackStored&&!detectorBinding_.nativeWithdrawn){
                if(callback||detectorBinding_.callbackPC!=std::optional<std::uint32_t>(0x294bb0u))return false;
            }else if(i==1&&directCallbackStored_){
                if(!isDirectE4()||callback||directCallbackE8PC_!=std::optional<std::uint32_t>(0x165920u))return false;
            }else if(!callback||*callback)return false;
        }
        std::uint32_t distance;std::memcpy(&distance,&c.distance84_,4);
        const std::array<std::uint32_t,4> previous{{0,0xc61c4000,0,0x3f800000}};
        return !distance&&!std::memcmp(c.copied40_.v,sentinel3E02C0_->data(),16)&&
            !std::memcmp(c.copied50_.v,sentinel3E02C0_->data(),16)&&
            !std::memcmp(c.previous70_.v,previous.data(),16);
    }
    template<class Retire>
    bool withdrawNative(Retire&& retire)noexcept {
        if(hasIncomingFiltersNative())return false;
        if(proceduralSuccessorBorrower_||sourceContactBorrower_||(detectorBinding_.attempted&&!detectorBinding_.nativeWithdrawn))return false;
        if(!attempted_||nativeWithdrawn_)return true;
        if((attached_&&!detachedNative_)||!preflightNative())return false;
        if(!withdrawSourceLocalFiltersNative())return false;
        //Native source shape/Model borrow leaves before numeric file close.
        if(!withdrawSourceProceduralShapeNative()||!withdrawSourceType0Native())return false;
        //Exact partial/source-open file interval leaves before its search pin.
        if(foundFile_){rr::assets::OriginalStandardTextureDiagnostic50LA d;
            if(!foundFile_->closeNative(d))return false;}
        //Search-only custody withdrawal, not original131040. A later ledger
        //refusal keeps its closed historical receipt and the Body pin valid.
        if(search_){rr::assets::OriginalStandardTextureDiagnostic50LA d;
            if(searchBorrowed_){if(!search_->closeCollisionSearchNative(this,d))return false;searchBorrowed_=false;}
            else if(!search_->closeNative(d))return false;}
        if(counted_&&!nativeRetired_){
            if(!retire(collision_->entity_))return false;
            collision_->entity_.reference00=0;--base_->application_->objectCount423F60();
            collision_->phase_=B::Phase::released;nativeRetired_=true;
        }
        if(bodyBorrow_){if(isDirectE4())bodyBorrow_->directCollisionModelBorrower_=nullptr;
            else bodyBorrow_->collisionBodyBorrower_=nullptr;bodyBorrow_=nullptr;}
        if(directPredecessorToken_){*directPredecessorToken_=nullptr;directPredecessorToken_=nullptr;}
        if(proceduralPredecessor_){proceduralPredecessor_->proceduralSuccessorBorrower_=nullptr;proceduralPredecessor_=nullptr;}
        foundFile_.reset();search_.reset(); //No source buffer restoration, handle or file-close receipt.
        if(published_){outerAliasNative()=nullptr;
            if(isProceduralE8())base_->object_->collisionE8NativeWithdrawn=true;else if(isDirectE4())base_->object_->collisionE4NativeWithdrawn=true;else base_->object_->collisionE0NativeWithdrawn=true;}
        collision_.reset();baseBorrowerNative()=nullptr;base_=nullptr;sentinel3E02C0_=nullptr;
        nativeWithdrawn_=true;return true; //Native cancellation, not source11B4A0/destructor dispatch.
    }
public:
    OriginalSourcePhysicsCollision50LA()=default;
    OriginalSourcePhysicsCollision50LA(const OriginalSourcePhysicsCollision50LA&)=delete;
    OriginalSourcePhysicsCollision50LA& operator=(const OriginalSourcePhysicsCollision50LA&)=delete;
    ~OriginalSourcePhysicsCollision50LA(){if(attempted_&&!nativeWithdrawn_)std::terminate();}
};
}
