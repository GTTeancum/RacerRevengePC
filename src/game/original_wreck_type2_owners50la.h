#pragma once
#include "type2_resource49x.h"
#include <cstring>
#include <exception>
#include <limits>
#include <memory>

namespace rr::game {
class OriginalWreckQueries50LA;
// Source allocations owned by ONE E0. This is separate from the native
// transaction banks in OriginalWreckType2Storage50LA. Only the actual query
// constructor/retirement join may advance these private lifetime methods.
class OriginalWreckType2Owners50LA final {
    friend class OriginalWreckQueries50LA;
    struct Shape {
        BodyMatrix48G matrix00{};
        std::uint16_t root40=0;
        SpringSceneId48N model44{};
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

    // Called at1253DC only after the caller has admitted the actual model
    // identity and retained COL occurrence. Does not publish a domain token.
    bool allocateShape1253DC(SpringSceneId48N actualModel)noexcept {
        if(attempted_||retired_||!actualModel.value)return false;
        attempted_=true;boundary_=0x1253dcu;
        try {shape8C_=std::make_unique<Shape>();}catch(...){return false;}
        shape8C_->model44=actualModel; //1253F0: caller must publish SAME identity
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
public:
    OriginalWreckType2Owners50LA()=default;
    ~OriginalWreckType2Owners50LA(){
        // A native destructor cannot silently substitute for11B600. Untouched
        // cold ownership is disposable; reached source allocation is retained
        // until its matching source release (partial failures remain gated).
        if(shape8C_||result90_)std::terminate();
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
