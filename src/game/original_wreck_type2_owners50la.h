#pragma once
#include "type2_resource49x.h"
#include "original_wreck_components50la.h"
#include <optional>
#include <cstring>
#include <exception>
#include <limits>
#include <memory>

namespace rr::game {
class OriginalWreckQueries50LA;
class OriginalSourcePhysicsCollision50LA;
class OriginalRacerQuery364Owner50LA;
class OriginalRacerQuery690Owner50LA;
struct OriginalRacerSceneObject50LA;
// Source allocations owned by ONE E0. This is separate from the native
// transaction banks in OriginalWreckType2Storage50LA. Only the actual query
// constructor/retirement join may advance these private lifetime methods.
class OriginalWreckType2Owners50LA final {
    friend class OriginalWreckQueries50LA;
    friend class OriginalSourcePhysicsCollision50LA;
    friend class OriginalRacerQuery364Owner50LA;
    friend class OriginalRacerQuery690Owner50LA;
    struct Shape {
        BodyMatrix48G matrix00{};
        std::uint16_t root40=0;
        SpringSceneId48N model44{};
        //Alternative concrete Racer190/19C or fresh Wreck14C source pointer;
        //never a fabricated numeric scene ID. Legacy wreck identity unchanged.
        const OriginalRacerSceneObject50LA* racerModel44=nullptr;
        std::uint32_t count48=0;
        std::unique_ptr<Type2Node49X[]> nodes4C;
        std::unique_ptr<Type2Ray49X[]> rays50;
        std::uint32_t normalMode54=0;
        std::size_t nodeCount=0,maxDepth=0;
    };
    struct Result {
        SpringVector4_48N first00{},second10{},normal20{},point30{},correction40{};
        std::unique_ptr<Type2Ray49X[]> pairs50;
        std::unique_ptr<SpringVector4_48N[]> normals54,points58;
        std::uint32_t count5C=0;
        // Native fresh backing is zero-initialized, but no source reset or
        // selected hit is certified by those initial values.
        std::uint32_t hits60=0;
        std::unique_ptr<std::uint32_t[]> words64,flags68;
        std::unique_ptr<float[]> fractions6C;
        std::unique_ptr<std::uint16_t[]> tags70;
        float selectedFraction74=0.f;
    };
    std::unique_ptr<Shape> shape8C_;
    // Owned local result until11BD18; loaded_ alone certifies publication
    // into source90. A failed allocation prefix is retained, never published.
    std::unique_ptr<Result> result90_;
    bool attempted_=false,loaded_=false,retired_=false;
    std::uint32_t boundary_=0;

    enum class FreshWreckRelease : std::uint8_t {
        none,sourceShapeOnly11B600,sourceFull11B600,nativeIncompleteCancellation
    };
    bool freshWreck_=false,freshLoadAttempted_=false,freshResultAttempted_=false;
    const OriginalWreckComponent50LA* freshE0_=nullptr;
    const OriginalRacerSceneObject50LA* freshModel14C_=nullptr; //Caller pins SAME Model through release.
    std::optional<const void*>* freshShapeField8C_=nullptr;
    std::optional<const void*>* freshResultField90_=nullptr;
    const void* freshShapeHistory_=nullptr;
    const void* freshResultHistory_=nullptr;
    std::uint16_t freshAllocatedMask_=0; //Only actual successful allocations, never source readiness.
    std::uint32_t freshStoppedPC_=0;
    FreshWreckRelease freshRelease_=FreshWreckRelease::none;
    std::uint16_t freshLiveAllocationMask()const noexcept {
        std::uint16_t m=shape8C_?1u:0u;
        if(shape8C_){if(shape8C_->nodes4C)m|=2u;if(shape8C_->rays50)m|=4u;}
        if(result90_){m|=8u;const auto& r=*result90_;
            if(r.words64)m|=16u;if(r.flags68)m|=32u;if(r.fractions6C)m|=64u;
            if(r.tags70)m|=128u;if(r.pairs50)m|=256u;if(r.normals54)m|=512u;if(r.points58)m|=1024u;}
        return m;
    }
    void observeFreshWreckAllocationCut()noexcept {
        freshAllocatedMask_=freshLiveAllocationMask();freshStoppedPC_=boundary_;
        if(shape8C_)freshShapeHistory_=shape8C_.get();
        if(result90_)freshResultHistory_=result90_.get();
    }
    bool allocateFreshWreckShape1253DC(const OriginalRacerSceneObject50LA& model,
        std::optional<const void*>& actual8C,const OriginalWreckComponent50LA& actualE0)noexcept {
        if(attempted_||retired_||freshWreck_||!actual8C||*actual8C||actualE0.reference00!=1u||
           actualE0.kind!=OriginalWreckComponentKind50LA::collision11B190||actualE0.parent1C||
           actualE0.previous10||actualE0.next14||actualE0.firstChild18)return false;
        freshWreck_=true;freshE0_=&actualE0;freshModel14C_=&model;freshShapeField8C_=&actual8C;
        attempted_=true;boundary_=0x1253dcu;
        try {shape8C_=std::make_unique<Shape>();}
        catch(...){observeFreshWreckAllocationCut();return false;}
        actual8C=shape8C_.get(); //1253E4 BEFORE source Model44 and normalMode54.
        shape8C_->racerModel44=&model; //1253F0, raw borrow, no extra Model retain.
        shape8C_->normalMode54=0;boundary_=0x125400u;
        observeFreshWreckAllocationCut();return true;
    }
    bool loadFreshWreckShape126640(const Type2ResourceView49X& retained)noexcept {
        if(!freshWreck_||freshLoadAttempted_||retired_||boundary_!=0x125400u)return false;
        freshLoadAttempted_=true;
        const bool done=loadShape126640(retained);observeFreshWreckAllocationCut();return done;
    }
    bool allocateFreshWreckResult11BBE8(std::uint32_t normalMode,
        std::optional<const void*>& actual90)noexcept {
        if(!freshWreck_||!freshLoadAttempted_||freshResultAttempted_||retired_||
           boundary_!=0x11bbe0u||normalMode!=0u||!actual90||*actual90)return false;
        freshResultAttempted_=true;freshResultField90_=&actual90;
        const bool done=allocateResult11BBE8(normalMode);
        if(done)actual90=result90_.get(); //11BD18; failed local allocations never publish90.
        observeFreshWreckAllocationCut();return done;
    }
    bool freshWreckCutCurrent(const OriginalRacerSceneObject50LA& model,
        const OriginalWreckComponent50LA& e0,const std::optional<const void*>& actual8C,
        const std::optional<const void*>& actual90,std::uint32_t expectedReference,
        const OriginalWreckComponent50LA* parent=nullptr,const OriginalWreckComponent50LA* previous=nullptr,
        const OriginalWreckComponent50LA* expectedNext=nullptr,
        const OriginalWreckComponent50LA* expectedAfterNext=nullptr,
        const OriginalWreckComponent50LA* expectedAfterDetectorE4=nullptr,
        const OriginalWreckComponent50LA* expectedAfterE4E8=nullptr)const noexcept {
        if(!freshWreck_||!attempted_||retired_||freshRelease_!=FreshWreckRelease::none||
           freshE0_!=&e0||freshModel14C_!=&model||freshShapeField8C_!=&actual8C||&actual8C==&actual90||
           (freshResultField90_&&freshResultField90_!=&actual90)||!actual8C||!actual90||
           e0.reference00!=expectedReference||e0.kind!=OriginalWreckComponentKind50LA::collision11B190||
           e0.parent1C!=parent||e0.previous10!=previous||e0.next14!=expectedNext||e0.firstChild18||
           bool(parent)!=bool(previous)||
           freshStoppedPC_!=boundary_||freshAllocatedMask_!=freshLiveAllocationMask())return false;
        if(expectedNext&&(!expectedReference||!parent||expectedNext->reference00!=1u||
           expectedNext->kind!=OriginalWreckComponentKind50LA::contact295A40||
           expectedNext->parent1C!=parent||expectedNext->previous10!=&e0||expectedNext->next14!=expectedAfterNext||
           expectedNext->sessionGeneration!=e0.sessionGeneration))return false;
        if(expectedAfterNext&&(!expectedReference||!expectedNext||expectedAfterNext->reference00!=1u||
           expectedAfterNext->kind!=OriginalWreckComponentKind50LA::detector295290||
           expectedAfterNext->parent1C!=parent||expectedAfterNext->previous10!=expectedNext||expectedAfterNext->next14!=expectedAfterDetectorE4||
           expectedAfterNext->sessionGeneration!=e0.sessionGeneration))return false;
        if(expectedAfterDetectorE4&&(!expectedReference||!expectedAfterNext||expectedAfterDetectorE4->reference00!=1u||
           expectedAfterDetectorE4->kind!=OriginalWreckComponentKind50LA::collision11B190||
           expectedAfterDetectorE4->parent1C!=parent||expectedAfterDetectorE4->previous10!=expectedAfterNext||
           expectedAfterDetectorE4->next14!=expectedAfterE4E8||expectedAfterDetectorE4->firstChild18||
           expectedAfterDetectorE4->sessionGeneration!=e0.sessionGeneration))return false;
        if(expectedAfterE4E8&&(!expectedReference||!expectedAfterDetectorE4||expectedAfterE4E8->reference00!=1u||
           expectedAfterE4E8->kind!=OriginalWreckComponentKind50LA::collision11B190||
           expectedAfterE4E8->parent1C!=parent||expectedAfterE4E8->previous10!=expectedAfterDetectorE4||
           expectedAfterE4E8->next14||expectedAfterE4E8->firstChild18||
           expectedAfterE4E8->sessionGeneration!=e0.sessionGeneration))return false;
        if(parent&&(!parent->reference00||!previous->reference00||previous->parent1C!=parent||
           previous->next14!=(expectedReference?&e0:nullptr)||parent->sessionGeneration!=e0.sessionGeneration||
           previous->sessionGeneration!=e0.sessionGeneration||
           parent->kind!=OriginalWreckComponentKind50LA::piece2CB4B0||
           previous->kind!=OriginalWreckComponentKind50LA::gravity282670))return false;
        if(*actual8C!=shape8C_.get()||*actual90!=(loaded_?result90_.get():nullptr))return false;
        if(shape8C_&&(shape8C_->racerModel44!=&model||shape8C_->model44.value))return false;
        //Each supported failure boundary has exactly the owners that really
        //precede that allocation. Native zero defaults are not source writes.
        switch(boundary_){
        case 0x1253dcu:return freshAllocatedMask_==0u&&!loaded_;
        case 0x125400u:return freshAllocatedMask_==1u&&!loaded_;
        case 0x12687cu:return freshAllocatedMask_==1u&&!loaded_;
        case 0x126d3cu:return freshAllocatedMask_==3u&&!loaded_;
        case 0x11bbe0u:return freshAllocatedMask_==7u&&!loaded_;
        case 0x11bbe8u:return freshAllocatedMask_==7u&&!loaded_;
        case 0x11bc48u:return freshAllocatedMask_==15u&&!loaded_;
        case 0x11bc58u:return freshAllocatedMask_==31u&&!loaded_;
        case 0x11bc68u:return freshAllocatedMask_==63u&&!loaded_;
        case 0x11bc78u:return freshAllocatedMask_==127u&&!loaded_;
        case 0x11bc90u:return freshAllocatedMask_==255u&&!loaded_;
        case 0x11bcc0u:return freshAllocatedMask_==511u&&!loaded_;
        case 0x11bcf0u:return freshAllocatedMask_==1023u&&!loaded_;
        case 0x11bd18u:return freshAllocatedMask_==2047u&&loaded_&&freshResultField90_;
        default:return false;
        }
    }
    bool preflightFreshWreckPayload(const OriginalRacerSceneObject50LA& model,
        const OriginalWreckComponent50LA& e0,const std::optional<const void*>& actual8C,
        const std::optional<const void*>& actual90,const OriginalWreckComponent50LA* parent=nullptr,
        const OriginalWreckComponent50LA* previous=nullptr,
        const OriginalWreckComponent50LA* expectedNext=nullptr,
        const OriginalWreckComponent50LA* expectedAfterNext=nullptr,
        const OriginalWreckComponent50LA* expectedAfterDetectorE4=nullptr,
        const OriginalWreckComponent50LA* expectedAfterE4E8=nullptr)const noexcept {
        return freshWreckCutCurrent(model,e0,actual8C,actual90,1u,parent,previous,expectedNext,expectedAfterNext,expectedAfterDetectorE4,expectedAfterE4E8);
    }
    bool releaseFreshWreckPayloadAfterReferenceDrop(const OriginalRacerSceneObject50LA& model,
        const OriginalWreckComponent50LA& e0,std::optional<const void*>& actual8C,
        std::optional<const void*>& actual90,const OriginalWreckComponent50LA* parent=nullptr,
        const OriginalWreckComponent50LA* previous=nullptr)noexcept {
        if(!freshWreckCutCurrent(model,e0,actual8C,actual90,0u,parent,previous))return false;
        if(loaded_){
            //All source-read pointer/count fields are initialized. Reuse exact
            //11B600 order, then actual NULL stores11B900/11B904.
            if(!releaseShapeResult11B600())std::terminate();
            actual8C=nullptr;actual90=nullptr;freshRelease_=FreshWreckRelease::sourceFull11B600;
            return true;
        }
        const bool sourceShapeOnly=freshAllocatedMask_==7u;
        //Readable fully loaded shape with source90=NULL uses the actual
        //11B814/818 null-result branch. Other cuts lack readable guest fields:
        //bounded NATIVE cancellation frees only tracked actual allocations.
        if(shape8C_){shape8C_->nodes4C.reset();shape8C_->rays50.reset();}
        if(result90_){auto& r=*result90_;
            r.fractions6C.reset();r.pairs50.reset();r.tags70.reset();r.normals54.reset();
            r.flags68.reset();r.words64.reset();r.points58.reset();}
        shape8C_.reset();result90_.reset();loaded_=false;retired_=true;
        if(sourceShapeOnly){actual8C=nullptr;actual90=nullptr;boundary_=0x11b904u;
            freshRelease_=FreshWreckRelease::sourceShapeOnly11B600;}
        else {freshRelease_=FreshWreckRelease::nativeIncompleteCancellation;}
        //Native cancellation preserves actual8C/90 values and stoppedPC;
        //history pointers/mask never authorize dereferencing retired storage.
        return true;
    }
    FreshWreckRelease freshWreckReleaseKind()const noexcept{return freshRelease_;}

    // Called at1253DC only after the caller has admitted the actual model
    // identity and retained COL occurrence. Does not publish a domain token.
    bool allocateShape1253DC(SpringSceneId48N actualModel)noexcept {
        if(attempted_||retired_||!actualModel.value)return false;
        attempted_=true;boundary_=0x1253dcu;
        try {shape8C_=std::make_unique<Shape>();}catch(...){return false;}
        shape8C_->model44=actualModel; //1253F0: caller must publish SAME identity
        shape8C_->normalMode54=0u;boundary_=0x125400u;return true;
    }
    //Concrete690 caller has already proved its SAME live Racer190 owner.
    bool allocateRacerShape1253DC(const OriginalRacerSceneObject50LA& actualModel)noexcept {
        if(attempted_||retired_)return false;
        attempted_=true;boundary_=0x1253dcu;
        try {shape8C_=std::make_unique<Shape>();}catch(...){return false;}
        shape8C_->racerModel44=&actualModel; //1253F0, no source retain.
        shape8C_->normalMode54=0u;boundary_=0x125400u;return true;
    }
    //126640 is represented using the SAME already decoded/authenticated COL
    // occurrence, never decoding again or manufacturing endpoint fourth words.
    // Failure retains the reached allocation prefix. No transactional rollback
    // is mislabeled as original11B600 retirement.
    bool loadShape126640(const Type2ResourceView49X& retained)noexcept {
        if(!shape8C_||boundary_!=0x125400u||!retained.nodes||!retained.rays||
           !retained.nodeCount||!retained.rayCount||retained.root!=0u||
           retained.nodeCount>32767u||retained.rayCount!=retained.nodeCount+1u||
           retained.rayCount>std::numeric_limits<std::uint32_t>::max())return false;
        auto& s=*shape8C_;s.matrix00=retained.localMatrix;
        s.count48=static_cast<std::uint32_t>(retained.rayCount);
        boundary_=0x12687cu;
        try {
            s.nodes4C=std::make_unique<Type2Node49X[]>(retained.nodeCount);
            s.nodeCount=retained.nodeCount;
            std::memcpy(s.nodes4C.get(),retained.nodes,retained.nodeCount*sizeof(Type2Node49X));
            boundary_=0x126d3cu;
            s.rays50=std::make_unique<Type2Ray49X[]>(retained.rayCount);
            std::memcpy(s.rays50.get(),retained.rays,retained.rayCount*sizeof(Type2Ray49X));
        }catch(...){return false;}
        s.root40=0u;s.maxDepth=retained.maxDepth;boundary_=0x11bbe0u;return true;
    }
    //11BBC0 stores its actual normal-mode argument AFTER the loader returns.
    // Result scalar arrays precede vector arrays; vector ownership is typed
    // host allocation, not a guest16-byte allocator-header reproduction.
    bool allocateResult11BBE8(std::uint32_t actualNormalMode)noexcept {
        if(!shape8C_||boundary_!=0x11bbe0u||result90_)return false;
        shape8C_->normalMode54=actualNormalMode;boundary_=0x11bbe8u;
        try {
            result90_=std::make_unique<Result>();auto& r=*result90_;
            const SpringVector4_48N vf0{{0.f,0.f,0.f,1.f}};
            r.first00=vf0;r.second10=vf0;r.normal20=vf0;r.point30=vf0;r.correction40=vf0;
            r.count5C=shape8C_->count48;const auto n=r.count5C;
            boundary_=0x11bc48u;r.words64=std::make_unique<std::uint32_t[]>(n);
            boundary_=0x11bc58u;r.flags68=std::make_unique<std::uint32_t[]>(n);
            boundary_=0x11bc68u;r.fractions6C=std::make_unique<float[]>(n);
            boundary_=0x11bc78u;r.tags70=std::make_unique<std::uint16_t[]>(n);
            boundary_=0x11bc90u;r.pairs50=std::make_unique<Type2Ray49X[]>(n);
            for(std::size_t i=0;i<n;++i){r.pairs50[i].first=vf0;r.pairs50[i].second=vf0;}
            boundary_=0x11bcc0u;r.normals54=std::make_unique<SpringVector4_48N[]>(n);
            for(std::size_t i=0;i<n;++i)r.normals54[i]=vf0;
            boundary_=0x11bcf0u;r.points58=std::make_unique<SpringVector4_48N[]>(n);
            for(std::size_t i=0;i<n;++i)r.points58[i]=vf0;
        }catch(...){return false;}
        loaded_=true;boundary_=0x11bd18u;return true;
    }
    // The enclosing query must first prove true11B4A0 eligibility: reference
    // zero, no parent/children, no live registration, and retired filters.
    // This handles allocation ownership only, never canonical row deletion.
    bool releaseShapeResult11B600()noexcept {
        if(!loaded_||retired_||!shape8C_||!result90_)return false;
        shape8C_->nodes4C.reset(); //11B7E4..11B7F8
        shape8C_->rays50.reset(); //11B7FC..11B810
        auto& r=*result90_;
        r.fractions6C.reset();r.selectedFraction74=0.f; //11B81C..11B834
        // Source does NOT clear field6C: its freed pointer remains stale until
        // result90 dies below. unique_ptr is the allocation owner, not that
        // source stale pointer field; no observer can read it during this call.
        r.pairs50.reset();r.tags70.reset();r.normals54.reset();
        r.flags68.reset();r.words64.reset();r.points58.reset();
        shape8C_.reset();result90_.reset(); //11B8E0..11B904, in this order
        loaded_=false;retired_=true;boundary_=0x11b904u;return true;
    }
#include "original_source_collision_result50la.inl"
public:
    OriginalWreckType2Owners50LA()=default;
    ~OriginalWreckType2Owners50LA(){
        // A native destructor cannot silently substitute for11B600. Untouched
        // cold ownership is disposable; reached source allocation is retained
        // until its matching source release (partial failures remain gated).
        if(shape8C_||result90_||(sourceResult_.attempted&&!sourceResult_.nativeWithdrawn))std::terminate();
    }
    OriginalWreckType2Owners50LA(const OriginalWreckType2Owners50LA&)=delete;
    OriginalWreckType2Owners50LA& operator=(const OriginalWreckType2Owners50LA&)=delete;
    bool attempted()const noexcept{return attempted_;}
    bool loaded()const noexcept{return loaded_;}
    bool retired()const noexcept{return retired_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
    // Read-only view for the eventual genuine11F570 continuation. No model
    // preparation is implied by successful allocation/loading.
    Type2ResourceView49X resource()const noexcept {
        if(!loaded_)return {};
        const auto& s=*shape8C_;return {s.matrix00,s.nodes4C.get(),s.nodeCount,
            s.rays50.get(),s.count48,s.root40,s.maxDepth};
    }
};
} // namespace rr::game
