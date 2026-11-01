#pragma once
#include "../assets/original_source_collision_file50la.h"
#include "original_collision_base_owner50la.h"
#include "type2_resource49x.h"
#include "original_collision_normal_guards50la.h"
#include "source_acos49t.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <optional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace rr::game {
class OriginalWorldCollisionObjects50LA;
// Dedicated 1B5590 call frame for 11B920(a0=outer04,a1=NULL,a2=folded name).
// Dedicated path/search/file custody; no Physics model or payload resource is manufactured.
class OriginalSourceCollisionPath50LA final {
    friend class OriginalWorldCollisionObjects50LA;
    struct Projection {
        std::array<char,0x108> path{}; //sp+40 ends before signed64 output at sp+148.
        std::size_t length=0;
        std::uint32_t pc=0x11b938u;
        bool directoryCopied=false,separator=false,requestCopied=false,extension=false;
    } projected_;
    bool entered_=false,directoryCaptured_=false,sharedNameStored_=false;
    const std::string* directoryIdentity_=nullptr;
    const void* sharedNameIdentity_=nullptr;
    std::string directoryRead_; //Historical read, not a live directory lease.
    std::optional<std::int64_t> handle148_;
    std::optional<const void*> cache150_;
    std::uint32_t pc_=0;
    using Search=assets::OriginalSourcePsgMissingLookup50LA;
    using File=assets::OriginalSourceCollisionFile50LA;
    using Diagnostic=assets::OriginalStandardTextureDiagnostic50LA;
    std::shared_ptr<Search> callbackSearch_;
    std::shared_ptr<File> callbackFile_;
    const assets::OriginalStandardTextureResolver50LA* resolverIdentity_=nullptr;
    const assets::OriginalRaTrackSourceFile50LA* trackIdentity_=nullptr;
    bool searchAttempted_=false,searchBorrowed_=false,fileAttempted_=false;
    Diagnostic searchDiagnostic_{},fileDiagnostic_{};
    //Independent NULL-model loader occurrence; no Physics payload/model lease.
    bool headerEntered_=false,headerComplete_=false,b0Stored_=false,emptyResetReturned_=false;
    OriginalCollisionBaseOwner50LA* headerOuter_=nullptr;
    std::array<std::array<std::uint8_t,4>,3> header_{}; //Unknown until receipt.done bytes.
    std::array<File::ReadReceipt,3> headerReads_{};
    std::uint32_t headerPC_=0;
    std::optional<unsigned> headerFailedRead_;
    Diagnostic headerReadDiagnostic_{};
    //Dedicated partial 0x130 allocation. Native backing bytes are NOT known
    //source values; typed field stores live in model110 and the raw payload.
    //No resource, prepared mesh or nonnull Physics model owner is borrowed.
    #include "original_source_collision_raw_payload50la.inl"
    //125014 raw malloc(0x70), NOT the model-bound Physics Resource class.
    //Word slots retain knownness; pointer-valued stores are separate native
    //aliases, never host pointers serialized into guest32 words. Resource
    //padding1E/1F and64..6F has no initializer in this source prefix.
    struct PartialType0Resource {
        File::CallbackResourceCustody custody;
        std::array<RawPayload::Word,28> words{};
        std::optional<const void*> nodes08,faces0C,vertices10,normals14;
    };
    struct PartialType0Shape {
        std::array<std::uint8_t,0x130> unknownBacking;
        std::optional<const void*> model110;
        RawPayload payload;
        //Actual mutable shape matrices. No constructor/parser store initializes
        //00/80/C0; zero native backing carries known=0 until11F680 writes.
        std::array<RawPayload::Word,16> matrix00{},matrix80{},matrixC0{};
        std::unique_ptr<PartialType0Resource> resource;
        std::optional<const void*> resource124;
        bool resourceAllocationAttempted=false,resourceStoresComplete=false;
    };
    std::unique_ptr<PartialType0Shape> partialType0_;
    bool associationEntered_=false,parserReturned_=false,loaderReturned_=false;
    std::optional<std::int32_t> parserReturnV0_,loaderReturnV0_;
    Diagnostic associationDiagnostic_{},closeDiagnostic_{};
    bool typeAttempted_=false,typeReadFailed_=false,typeAllocationAttempted_=false;
    bool typePublished_=false,nullModelStored_=false;
    std::array<std::uint8_t,4> typeBytes_{}; //11B190 actually initialized all of88 to0.
    File::ReadReceipt typeRead_{};
    Diagnostic typeReadDiagnostic_{};
    std::uint32_t typePC_=0;
    std::uint32_t typeValue()const noexcept {
        return std::uint32_t(typeBytes_[0])|(std::uint32_t(typeBytes_[1])<<8)|
            (std::uint32_t(typeBytes_[2])<<16)|(std::uint32_t(typeBytes_[3])<<24);
    }
    bool shapeCurrentNative(const OriginalCollisionBaseOwner50LA& outer)const noexcept {
        if(!typeAttempted_)return !partialType0_&&!typeReadFailed_&&!typeAllocationAttempted_&&
            !typePublished_&&!nullModelStored_&&!typeRead_.entered&&!typePC_&&outer.freshShapeNull();
        if(headerOuter_!=&outer||!headerComplete_||!b0Stored_||!emptyResetReturned_||
           headerDecisionPC()!=0x124fb8||outer.shape88_!=typeValue()||
           outer.shape8C_!=std::optional<const void*>(partialType0_.get())||
           outer.result90_!=std::optional<const void*>(nullptr)||pc_!=typePC_)return false;
        const auto& r=typeRead_;
        if(r.entered){
            if(r.requested!=4||r.done>4||
               (headerReads_[2].afterKnown&&r.before!=headerReads_[2].after)||
               (r.afterKnown&&(r.after<r.before||r.after-r.before!=r.done)))return false;
        }else if(r.returned||r.done||r.afterKnown)return false;
        //Unread high bytes retain the genuine constructor store, not unknown
        //stack bytes. A short successful type read still dispatches its value.
        for(std::size_t i=r.done;i<typeBytes_.size();++i)if(typeBytes_[i])return false;
        if(typeReadFailed_)return !r.returned&&!typeAllocationAttempted_&&!typePublished_&&
            !partialType0_&&!nullModelStored_&&typePC_==typeReadDiagnostic_.sourcePC&&
            (!r.entered||r.diagnostic.sourcePC==typePC_);
        if(!r.entered||!r.returned)return false;
        if(typeValue()!=0)return !typeAllocationAttempted_&&!typePublished_&&!partialType0_&&
            !nullModelStored_&&typePC_==(typeValue()==1?0x1250f8u:typeValue()==2?0x1253dcu:0x125414u);
        if(!typeAllocationAttempted_||!typePublished_||nullModelStored_!=bool(partialType0_))return false;
        if(!partialType0_)return typePC_==0x125004; //Published NULL; first dereference bounded.
        return partialType0_->model110==std::optional<const void*>(nullptr)&&
            partialType0_->payload.current(typeRead_)&&resourceCurrentNative()&&
            typePC_==(partialType0_->resourceAllocationAttempted?
                (partialType0_->resource?resourceContinuationPC():0x125030u):
                (partialType0_->payload.entered?partialType0_->payload.pc:0x12500cu));
    }
    bool continueType124FB8(OriginalCollisionBaseOwner50LA& outer,std::uint32_t& pc)noexcept {
        if(typeAttempted_||pc_!=0x124fb8||headerOuter_!=&outer||!headerCurrentNative()||
           !searchCurrentNative()||!outer.freshShapeNull())return false;
        typeAttempted_=true;
        //12FB30 writes directly into the constructor-known outer88. Preserve
        //partial bytes even on refusal; pre-entry refusal has its own receipt.
        const bool returned=callbackFile_->read12FB30(typeBytes_.data(),4,typeRead_,typeReadDiagnostic_);
        outer.shape88_=typeValue();
        if(!returned){typeReadFailed_=true;pc=pc_=typePC_=typeReadDiagnostic_.sourcePC;return false;}
        if(outer.shape88_!=0){pc=pc_=typePC_=outer.shape88_==1?0x1250f8u:outer.shape88_==2?0x1253dcu:0x125414u;return false;}
        typeAllocationAttempted_=true; //124FF0 nullable2E8D90(0x130).
        try{partialType0_=std::make_unique<PartialType0Shape>();}catch(...){partialType0_.reset();}
        outer.shape8C_=partialType0_.get();typePublished_=true; //124FF8 even when NULL.
        pc=pc_=typePC_=0x125004;
        if(!partialType0_)return false; //Never dereference NULL or invent a constructor.
        partialType0_->model110=nullptr;nullModelStored_=true; //125004 stores actual a1=NULL.
        pc=pc_=typePC_=0x12500c;
        return false; //125890 payload call not entered; no resource or loader return.
    }
    bool continuePayload125890(OriginalCollisionBaseOwner50LA& outer,
        OriginalApplicationEntities50LA& app,std::uint32_t& pc)noexcept {
        if(pc_!=0x12500c||!partialType0_||partialType0_->payload.entered||
           !shapeCurrentNative(outer)||!searchCurrentNative())return false;
        auto actualRead=[&](void* target,std::size_t bytes,File::ReadReceipt& receipt,Diagnostic& diagnostic)noexcept {
            return callbackFile_->read12FB30(target,bytes,receipt,diagnostic);
        };
        partialType0_->payload.begin125890(app,actualRead);
        pc=pc_=typePC_=partialType0_->payload.pc;
        return false; //Payload may return125890; resource125014/outer11B920 are not entered.
    }
    static std::array<RawPayload::Word,28> resourceWords(const RawPayload& raw)noexcept {
        std::array<RawPayload::Word,28> out{};
        out[0].store(0); //125030 resource type0.
        out[1]=raw.vertex11C; //125060 vertex count: copy unknown lanes, not a validated count.
        out[6]=raw.packed108; //125080 packed representation selector.
        //125088 LHU /125090 SH preserves unknown high half of resource1C.
        if(raw.records.root10C){out[7].bytes[0]=std::uint8_t(*raw.records.root10C);
            out[7].bytes[1]=std::uint8_t(*raw.records.root10C>>8);out[7].known=3;}
        for(unsigned i=0;i<16;++i)out[8+i]=raw.matrix40[i]; //Eight double-word loop iterations.
        out[24].store(1); //1250D4 source ref60 begins1; no native shared_ptr substitution.
        return out;
    }
    std::uint32_t resourceContinuationPC()const noexcept {
        if(!associationEntered_)return 0x1250e4u;
        if(!parserReturned_)return associationDiagnostic_.sourcePC;
        return loaderReturned_?0x11bbb8u:closeDiagnostic_.sourcePC;
    }
    bool resourceCurrentNative()const noexcept {
        const auto& shape=*partialType0_;const auto& raw=shape.payload;
        if(!shape.resourceAllocationAttempted)return !shape.resource&&!shape.resource124&&!shape.resourceStoresComplete;
        if(!raw.records.returned125890||raw.pc!=0x126410||
           shape.resource124!=std::optional<const void*>(shape.resource.get()))return false;
        if(!shape.resource)return !shape.resourceStoresComplete;
        if(!shape.resourceStoresComplete)return false;
        const auto& r=*shape.resource;const auto expected=resourceWords(raw);
        if(associationEntered_){
            if(!callbackFile_||r.custody.resource!=&r||r.custody.owner!=this||
               callbackFile_->callbackResource_!=&r.custody||!callbackFile_->associationAttempted_||
               r.custody.borrower!=(callbackFile_->associationWithdrawn_?nullptr:callbackFile_.get())||
               parserReturned_!=callbackFile_->associationReturned_)return false;
            if(parserReturned_){
                if(parserReturnV0_!=callbackFile_->associationReturnV0_||parserReturnV0_!=1||
                   !callbackFile_->sourceCloseAttempted_||
                   loaderReturned_!=callbackFile_->sourceCloseReturned_)return false;
            }else if(parserReturnV0_||loaderReturned_||loaderReturnV0_||callbackFile_->sourceCloseAttempted_)return false;
            if(loaderReturned_?loaderReturnV0_!=1:bool(loaderReturnV0_))return false;
        }else if(r.custody.resource||r.custody.owner||r.custody.borrower||parserReturned_||
                 parserReturnV0_||loaderReturned_||loaderReturnV0_)return false;
        for(unsigned i=0;i<expected.size();++i){
            if(r.words[i].known!=expected[i].known)return false;
            for(unsigned j=0;j<4;++j)if((expected[i].known&(1u<<j))&&
                r.words[i].bytes[j]!=expected[i].bytes[j])return false;
        }
        return r.nodes08==raw.records.node114&&r.faces0C==raw.records.face118&&
            r.vertices10==raw.vertex100&&r.normals14==raw.normal104;
    }
    bool continueResource125014(OriginalCollisionBaseOwner50LA& outer,std::uint32_t& pc)noexcept {
        if(pc_!=0x126410||!partialType0_||partialType0_->resourceAllocationAttempted||
           !shapeCurrentNative(outer)||!searchCurrentNative()||
           !partialType0_->payload.records.returned125890)return false;
        auto& shape=*partialType0_;auto& raw=shape.payload;
        shape.resourceAllocationAttempted=true; //125014 calls305700(0x70), no constructor.
        try{shape.resource=std::make_unique<PartialType0Resource>();}catch(...){shape.resource.reset();}
        shape.resource124=shape.resource.get(); //125024 publishes even the NULL result.
        pc=pc_=typePC_=0x125030;
        if(!shape.resource)return false; //First source dereference; never fabricate success.
        auto& r=*shape.resource;
        r.words=resourceWords(raw);
        r.nodes08=raw.records.node114;r.faces0C=raw.records.face118;
        r.vertices10=raw.vertex100;r.normals14=raw.normal104;
        shape.resourceStoresComplete=true;
        pc=pc_=typePC_=0x1250e4;
        //Stop this prefix before the dedicated callback association join.
        //continueAssociation1250E4 owns row custody, source close and loader
        //return; this function alone claims none of those or an outer90 result.
        return false;
    }
    bool continueAssociation1250E4(OriginalCollisionBaseOwner50LA& outer,std::uint32_t& pc)noexcept {
        if(pc_!=0x1250e4||associationEntered_||!shapeCurrentNative(outer)||!searchCurrentNative())return false;
        auto& r=*partialType0_->resource;
        r.custody.resource=&r;r.custody.owner=this;associationEntered_=true;
        if(!callbackFile_->associateCallback1326A0(r.custody,associationDiagnostic_)){
            pc=pc_=typePC_=associationDiagnostic_.sourcePC;return false;
        }
        //1250EC jumps to125478 epilogue: V0 is the actual1326A0 return,
        //not a new parser success assignment. Outer90 is still NULL.
        parserReturned_=true;parserReturnV0_=callbackFile_->associationReturnV0_;
        if(!callbackFile_->closeSource131040(closeDiagnostic_)){
            pc=pc_=typePC_=closeDiagnostic_.sourcePC;return false;
        }
        //11BB9C really returned;11BBA4 sets1 regardless of parser V0.
        loaderReturnV0_=1;loaderReturned_=true;pc=pc_=typePC_=0x11bbb8;
        return true; //Only11B920 returned; no1B5590 wrapper continuation claimed.
    }
    std::uint32_t headerDecisionPC()const noexcept {
        const auto word=[](const std::array<std::uint8_t,4>& b)noexcept {
            return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);
        };
        if(headerReads_[0].done!=4)return 0x11bac8;
        if(word(header_[0])!=1234)return 0x11bb00;
        constexpr std::uint8_t tag[4]={'c','o','l',0};
        for(unsigned i=0;i<4;++i){
            if(i>=headerReads_[1].done)return 0x308f00;
            auto c=header_[1][i];
            if(c>=128)return 0x308f00; //Unjoined ctype table, not an assertion result.
            if(c>='A'&&c<='Z')c=std::uint8_t(c+32);
            if(c!=tag[i])return 0x11bb00;
        }
        if(headerReads_[2].done!=4)return 0x11baf0;
        return word(header_[2])==1?0x124fb8u:0x11bb00u;
    }
    bool headerCurrentNative()const noexcept {
        if(!headerEntered_)return !headerOuter_&&!headerComplete_&&!b0Stored_&&!emptyResetReturned_&&!headerPC_&&!headerFailedRead_;
        if(!headerOuter_||!shapeCurrentNative(*headerOuter_)||!callbackFile_||!callbackFile_->returnedCold_||!callbackFile_->preflightNative()||
           !callbackFile_->sourceHandle148_||!callbackFile_->sourceCache150_||*callbackFile_->sourceCache150_)return false;
        bool all=true;const File::ReadReceipt* last=nullptr;
        for(unsigned i=0;i<3;++i){const auto& r=headerReads_[i];
            if(r.entered){
                if(r.requested!=4||r.done>4||(i&&!headerReads_[i-1].returned)||
                   (last&&last->afterKnown&&r.before!=last->after)||
                   (r.afterKnown&&(r.after<r.before||r.after-r.before!=r.done)))return false;
                last=&r;
            }else if(r.returned||r.done||r.afterKnown)return false;
            all=all&&r.returned;
        }
        if(headerComplete_!=all||pc_!=(typeAttempted_?typePC_:headerPC_))return false;
        if(!all){
            if(!headerFailedRead_||*headerFailedRead_>=headerReads_.size()||b0Stored_||emptyResetReturned_||
               headerPC_!=headerReadDiagnostic_.sourcePC)return false;
            const auto failed=*headerFailedRead_;
            for(unsigned i=0;i<headerReads_.size();++i){const auto& r=headerReads_[i];
                if(i<failed&&!r.returned)return false;
                if(i==failed&&(r.returned||(r.entered&&r.diagnostic.sourcePC!=headerPC_)))return false;
                if(i>failed&&r.entered)return false;
            }
            //Only this exact recorded call may have refused before receipt.entry.
            //Its earlier reads must have returned; later reads remain untouched.
            return true;
        }
        if(headerFailedRead_)return false;
        const auto expected=headerDecisionPC();
        if(headerPC_!=expected)return false;
        if(expected!=0x124fb8)return !b0Stored_&&!emptyResetReturned_&&
            headerOuter_->wordB0_==std::optional<std::uint64_t>(~std::uint64_t{0});
        return b0Stored_&&emptyResetReturned_&&shapeCurrentNative(*headerOuter_)&&
            headerOuter_->wordB0_==std::optional<std::uint64_t>(static_cast<std::uint64_t>(*callbackFile_->sourceHandle148_));
    }
    bool continueHeader11BA34(OriginalCollisionBaseOwner50LA& outer,std::uint32_t& pc)noexcept {
        if(headerEntered_||pc_!=0x11ba34||!searchCurrentNative()||!callbackFile_||
           !callbackFile_->returnedCold_||!callbackFile_->current()||!outer.freshShapeNull()||
           !callbackFile_->sourceHandle148_||!callbackFile_->sourceCache150_||*callbackFile_->sourceCache150_)return false;
        headerEntered_=true;headerOuter_=&outer;
        //11BA34/54: found and cache NULL. All three source read counts are ignored.
        for(unsigned i=0;i<3;++i){
            Diagnostic d;
            if(!callbackFile_->read12FB30(header_[i].data(),4,headerReads_[i],d)){
                headerFailedRead_=i;headerReadDiagnostic_=d;
                pc=pc_=headerPC_=d.sourcePC;return false;
            }
        }
        headerComplete_=true;pc=pc_=headerPC_=headerDecisionPC();
        if(pc_!=0x124fb8)return false; //Invalid: shared assertion11BB00, NOT false/close.
        outer.wordB0_=static_cast<std::uint64_t>(*callbackFile_->sourceHandle148_);b0Stored_=true; //11BB84.
        //124F9C calls11B600. This exact newly constructed outer has both
        //8C/90 NULL: branches11B614,11B8F0 then stores11B900/04, returns11B91C.
        outer.shape8C_=nullptr;outer.result90_=nullptr;emptyResetReturned_=true;
        return false; //Bound BEFORE type read124FB8; no shape/resource/loader return.
    }
    //Native custody only: source a1 remains NULL throughout this occurrence.
    bool searchCurrentNative()const noexcept {
        if(!searchAttempted_)return !callbackSearch_&&!callbackFile_&&!resolverIdentity_&&!trackIdentity_&&
            !searchBorrowed_&&!fileAttempted_;
        if(!resolverIdentity_||!trackIdentity_||!sharedNameStored_||projected_.pc!=0x11ba2c)return false;
        if(!callbackSearch_)return !searchBorrowed_&&!callbackFile_&&!fileAttempted_&&pc_==searchDiagnostic_.sourcePC;
        const auto& q=*callbackSearch_;
        if(!searchBorrowed_||q.nativeClosed_||!q.preflightNative()||q.collisionSearchBorrower_!=this||
           q.resolver()!=resolverIdentity_||q.root()!=trackIdentity_||
           q.requestedName()!=std::string_view(projected_.path.data(),projected_.length)||
           q.continuationAttempted_||q.fileBorrower_)return false;
        if(!fileAttempted_)return !callbackFile_&&!q.collisionContinuationAttempted_&&!q.collisionFileBorrower_&&
            pc_==(q.returnedMissing132820()?0x11ba34u:q.sourcePC_);
        if(!q.found_||q.sourcePC_!=0x132988)return false;
        if(!callbackFile_)return !q.collisionContinuationAttempted_&&!q.collisionFileBorrower_&&pc_==fileDiagnostic_.sourcePC;
        const auto& f=*callbackFile_;
        if(!f.preflightNative()||f.nativeClosed_||f.search_.get()!=callbackSearch_.get()||
           f.collisionOwner_!=this||q.collisionFileBorrower_!=&f||f.payloadBorrower_)return false;
        if(associationEntered_){
            if(!partialType0_||!partialType0_->resource||
               !f.canCloseCallbackNative(partialType0_->resource->custody))return false;
        }else if(!f.canCloseNative()||f.associationAttempted_||f.sourceCloseAttempted_)return false;
        return headerEntered_?headerCurrentNative():pc_==(f.returnedCold_?0x11ba34u:f.pc_);
    }
    bool continueSearch11BA2C(std::shared_ptr<assets::OriginalStandardTextureResolver50LA> resolver,
        std::shared_ptr<assets::OriginalRaTrackSourceFile50LA> track,std::uint32_t& pc)noexcept {
        if(searchAttempted_||pc_!=0x11ba2c||!sharedNameStored_||!resolver||!track)return false;
        std::string request;
        try{request.assign(projected_.path.data(),projected_.length);}catch(...){return false;}
        resolverIdentity_=resolver.get();trackIdentity_=track.get();searchAttempted_=true;
        Search::capture132820(std::move(resolver),std::move(track),request,callbackSearch_,searchDiagnostic_);
        if(!callbackSearch_){pc=pc_=searchDiagnostic_.sourcePC;return false;}
        //Capture issued here, once, into initially-empty owned storage. No
        //foreign search can be adopted. The exact callback frame pins root.
        callbackSearch_->collisionSearchBorrower_=this;searchBorrowed_=true;
        if(callbackSearch_->returnedMissing132820()){pc=pc_=0x11ba34;return false;}
        pc=pc_=callbackSearch_->sourcePC_;
        if(pc_!=0x132988)return false; //Nonzero mode miss remains12DCB0.
        fileAttempted_=true;
        File::continueFound132988(callbackSearch_,this,callbackFile_,fileDiagnostic_);
        pc=pc_=callbackFile_?callbackFile_->pc_:fileDiagnostic_.sourcePC;
        if(callbackFile_&&callbackFile_->returnedCold_)pc=pc_=0x11ba34;
        return false; //No11B920 return, header read, resource, or wrapper return.
    }
    bool withdrawSearchNative()noexcept {
        if(!searchCurrentNative()||!headerCurrentNative())return false;
        Diagnostic d;
        if(associationEntered_&&!callbackFile_->withdrawCallbackAssociationNative(d))return false;
        if(callbackFile_&&!callbackFile_->closeNative(d))return false;
        if(searchBorrowed_&&!callbackSearch_->closeCollisionSearchNative(this,d))return false;
        //Owner is destroyed immediately afterward; historical stack outputs
        //are not rewritten and native close is not source131040/11B920 return.
        callbackFile_.reset();callbackSearch_.reset();searchBorrowed_=false;
        //Dedicated raw payload children remain owned by its allocation. Retire
        //its alias before storage, after file/search, before counted outer.
        //This is native cancellation, NOT source11B600/125890 destructor work.
        if(typePublished_){headerOuter_->shape8C_=nullptr;partialType0_.reset();}
        return true;
    }
    static Projection project(std::string_view directory,std::string_view request,std::size_t sharedCapacity)noexcept {
        Projection p;
        if(directory.find('\0')!=std::string_view::npos)return p;
        if(!directory.empty()){
            p.pc=0x11b954;
            if(directory.size()>=p.path.size())return p;
            std::memcpy(p.path.data(),directory.data(),directory.size());
            p.length=directory.size();p.path[p.length]=0;p.directoryCopied=true;
            if(p.path[p.length-1]!='\\'){
                p.pc=0x11b990;if(p.length+2>p.path.size())return p;
                p.path[p.length++]='\\';p.path[p.length]=0;p.separator=true;
            }
            p.pc=0x11b99c;
        }else p.pc=0x11b9b0;
        if(request.size()>p.path.size()-p.length-1)return p;
        std::memcpy(p.path.data()+p.length,request.data(),request.size());
        p.length+=request.size();p.path[p.length]=0;p.requestCopied=true;
        //1169D0 searches the whole string, including directory dots.
        std::size_t dot=p.length;
        for(std::size_t i=0;i<p.length;++i)if(p.path[i]=='.')dot=i;
        p.pc=dot==p.length?0x11b9ecu:0x11b9d4u;
        if(dot+sizeof(".col")>p.path.size())return p;
        std::memcpy(p.path.data()+dot,".col",sizeof(".col"));p.length=dot+4;p.extension=true;
        p.pc=0x11b9fc;
        if(request.size()>=sharedCapacity)return p;
        p.pc=0x11ba2c;return p;
    }
    template<std::size_t N>
    bool begin11B920(const std::string& actualDirectory,std::array<std::uint8_t,N>& actualSharedName,
        std::string_view actualFoldedName,std::uint32_t& pc)noexcept {
        if(entered_||!actualFoldedName.data()||actualFoldedName.find('\0')!=std::string_view::npos)return false;
        entered_=true;directoryIdentity_=&actualDirectory;sharedNameIdentity_=&actualSharedName;
        //Native history allocation may refuse before the first source directory read.
        pc=pc_=0x11b938;
        try{directoryRead_=actualDirectory;}catch(...){return false;}
        directoryCaptured_=true;projected_=project(directoryRead_,actualFoldedName,N);pc=pc_=projected_.pc;
        if(pc_!=0x11ba2c)return false;
        //11B9FC copies ORIGINAL request (already folded by caller), not lookup path.
        std::memcpy(actualSharedName.data(),actualFoldedName.data(),actualFoldedName.size());
        actualSharedName[actualFoldedName.size()]=0;sharedNameStored_=true;
        handle148_=-1; //11BA10. This is a stack initializer, NOT an opened file.
        cache150_=nullptr; //11BA30 delay-slot output initializer, NOT a resource.
        return false; //132820 not called; no 11B920 return or wrapper continuation.
    }
    template<std::size_t N>
    bool currentNative(const std::string& directory,const std::array<std::uint8_t,N>& shared,
        std::string_view request,std::uint32_t callbackPC)const noexcept {
        if(!searchCurrentNative()||!headerCurrentNative())return false;
        if(!entered_)return !directoryCaptured_&&!directoryIdentity_&&!sharedNameIdentity_&&directoryRead_.empty()&&
            !sharedNameStored_&&!handle148_&&!cache150_&&!pc_;
        if(directoryIdentity_!=&directory||sharedNameIdentity_!=&shared||pc_!=callbackPC)return false;
        //Shared process memory may legitimately be changed by later source calls;
        //historical stores do not require mutable global bytes to remain equal.
        if(!directoryCaptured_)return pc_==0x11b938&&!sharedNameStored_&&!handle148_&&!cache150_;
        const auto expected=project(directoryRead_,request,N);
        if(projected_.path!=expected.path||projected_.length!=expected.length||projected_.pc!=expected.pc||
           projected_.directoryCopied!=expected.directoryCopied||projected_.separator!=expected.separator||
           projected_.requestCopied!=expected.requestCopied||projected_.extension!=expected.extension||(!searchAttempted_&&pc_!=expected.pc))return false;
        if(expected.pc!=0x11ba2c)return !sharedNameStored_&&!handle148_&&!cache150_;
        return sharedNameStored_&&handle148_&&*handle148_==-1&&cache150_&&!*cache150_;
    }
public:
    OriginalSourceCollisionPath50LA()=default;
};
}
