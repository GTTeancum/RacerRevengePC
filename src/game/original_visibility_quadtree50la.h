#pragma once
#if defined(__FAST_MATH__)
#error "Visibility tree bounds require strict binary32 arithmetic"
#endif
#include "original_application_entities50la.h"
#include "original_world_environment50kg.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <memory>
#include <limits>
#include <optional>

namespace ra_pod_scene50LA {class Owner;}
namespace rr::game {
// Future registrations must retain their actual native object identity here.
// No guest pointer, render-mesh surrogate, or synthetic collection identity.
struct OriginalVisibilityMembership50LA;
class OriginalVisibilityQuadTree50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    struct ItemBlock {
        ItemBlock* next=nullptr;
        std::array<OriginalVisibilityMembership50LA*,4> objects{};
    };
    struct ScratchItem {ScratchItem* next=nullptr;OriginalVisibilityMembership50LA* object=nullptr;};
    template<class T,std::uint32_t SourceBytes> struct Pool {
        struct Chunk {Chunk* next=nullptr;std::array<T,(4096u-SourceBytes)/SourceBytes> slots;};
        std::uint64_t reference00=1; //1048F0 reached by105620.
        const std::uint32_t perBlock10=(4096u-SourceBytes)/SourceBytes;
        Chunk* active14=nullptr;Chunk* tail18=nullptr;Chunk* unused1C=nullptr;
        std::uint32_t used20=perBlock10;
        const std::uint32_t elementBytes24=SourceBytes,chunkBytes28=4096;
        T* free2C=nullptr;std::uint32_t field30=0;
        //105620 does not write34. Do not manufacture a source zero.
        std::optional<std::uint32_t> bytes34;
        bool fresh()const noexcept{return reference00==1&&!active14&&!tail18&&!unused1C&&!free2C&&used20==perBlock10;}
    };
    struct Node {
        std::unique_ptr<std::array<std::unique_ptr<Node>,4>> children0;
        ItemBlock* items4=nullptr;
        std::uint32_t field8=0,fieldCBits=0xff7fffffu;
    };
    OriginalApplicationEntities50LA* application_=nullptr;
    std::uint64_t reference00_=0;
    std::uint32_t usage10_=0;
    //27E9D0->27ED10/27ED40: actual selected list capacity30, growth30.
    std::unique_ptr<OriginalVisibilityMembership50LA*[]> selected18_;
    std::uint32_t selectedCount18_=0,selectedGrowth20_=0,selectedCapacity24_=0;
    bool selectedOwned28_=false;
    std::optional<std::uint8_t> soultreeType2C_,searchableType2D_;
    float originX30_=0,maximumX34_=0,originZ38_=0,maximumZ3C_=0,extentX48_=0,extentZ4C_=0;
    float scale5C_=0,minimumSize8B8_=0;
    std::uint32_t maximumLevel54_=0,smallestQuantum58_=0;
    std::array<std::uint32_t,12> quanta60_{};
    std::unique_ptr<Node> root94_;
    std::unique_ptr<Pool<ItemBlock,20>> items98_;
    std::unique_ptr<Pool<ScratchItem,8>> scratch9C_;
    bool scratchDirty16_=false,destroyingA0_=false;
    std::uint16_t field14_=0;
    ScratchItem* headA4_=nullptr;ScratchItem* tailA8_=nullptr;
    std::uint32_t countAC_=0,countB4_=0,nodeCount888_=0;
    std::int32_t field88C_=-1;
    std::uint32_t field890_=0,field894_=0;
    //Actual candidate buffer89C stays unallocated until2AF1D0. Capacity8C0
    //is a requested limit, never root94 or an allocation/completion proof.
    std::unique_ptr<OriginalVisibilityMembership50LA*[]> candidate89C_;
    OriginalVisibilityMembership50LA** published898_=nullptr;
    std::uint32_t candidateCount8A0_=0,candidateCapacity8A4_=0,requestedCapacity8C0_=0;
    std::array<std::optional<std::int32_t>,4> cached8A8_;
    bool published_=false,configured_=false;
    static float binary32(float v)noexcept{volatile float rounded=v;return rounded;}
    bool construct(OriginalApplicationEntities50LA& app,
        const original_world_environment50KG::SpatialBounds& bounds,std::uint32_t& pc)noexcept {
        pc=0;
        if(reference00_||app.visibility443A98_||app.environment443108_||
           !app.application10()||std::fegetround()!=FE_TONEAREST){pc=0x272fd4;return false;}
        application_=&app;reference00_=1; //27E9D0 ->1048F0.
        try {
            selected18_.reset(new OriginalVisibilityMembership50LA*[30]);
            selectedGrowth20_=selectedCapacity24_=30;selectedOwned28_=true;
            auto type=app.types383408().findOrAppend2A31E0("SoultreeObject");
            if(!type.okay()){pc=0x27ea28;return false;}soultreeType2C_=type.type;
            type=app.types383408().findOrAppend2A31E0("SearchableGameObject");
            if(!type.okay()){pc=0x27ea44;return false;}searchableType2D_=type.type;
            //2AEF70 itself publishes443A98 before its return to272FC0.
            app.visibility443A98_=this;published_=true;
            app.activeTree442E18_=this; //2639B0, before root264CD0.
            root94_.reset(new Node);++nodeCount888_;
            originX30_=bounds.xMin534;maximumX34_=bounds.xMax538;
            originZ38_=bounds.zMin53C;maximumZ3C_=bounds.zMax540;
            extentX48_=binary32(maximumX34_-originX30_);
            extentZ4C_=binary32(maximumZ3C_-originZ38_);minimumSize8B8_=96.f;
            const float maximum=extentX48_>extentZ4C_?extentX48_:extentZ4C_;
            if(!std::isfinite(maximum)||maximum<=0.f){pc=0x263a78;return false;}
            scale5C_=binary32(32768.f/maximum);
            app.treeQuantum442E20_=binary32(1.f/scale5C_);
            const float size=binary32(minimumSize8B8_*scale5C_);
            if(!std::isfinite(size)||size<0.f||size>2147483520.f){pc=0x263ac0;return false;}
            const auto integer=static_cast<std::int32_t>(std::nearbyint(size));
            std::uint32_t quantum=32768,levels=0;
            do {quantum>>=1;quanta60_[levels++]=quantum;}
            while(levels<12u&&integer<static_cast<std::int32_t>(quantum));
            maximumLevel54_=levels-1;smallestQuantum58_=quanta60_[maximumLevel54_];
            scratch9C_.reset(new Pool<ScratchItem,8>); //263B64, then263B88.
            items98_.reset(new Pool<ItemBlock,20>);
            requestedCapacity8C0_=5000; //2AF150 delay-slot store.
            configured_=true;return true;
        }catch(...){pc=0x272fe4;return false;}
    }
    //Actual slot40,265C60. This constructor-only owner has no query producer;
    //105990 retires its active typed chunk chain, without rebuilding root94.
    bool reset265C60(std::uint32_t& pc)noexcept {
        if(!configured_){pc=0x265c60;return false;}
        if(!scratchDirty16_){pc=0;return true;}
        if(!scratch9C_){pc=0x265c7c;return false;}
        auto& pool=*scratch9C_;
        if(pool.active14){
            if(pool.unused1C&&pool.tail18)pool.tail18->next=pool.unused1C;
            pool.unused1C=pool.active14;
        }
        pool.active14=pool.tail18=nullptr;pool.bytes34=0;pool.free2C=nullptr;pool.used20=pool.perBlock10;
        headA4_=tailA8_=nullptr;countAC_=countB4_=0;
        for(auto& value:cached8A8_)value=std::numeric_limits<std::int32_t>::min();
        scratchDirty16_=false;pc=0;return true;
    }
    bool constructorStateOnly()const noexcept {
        return !selectedCount18_&&!candidate89C_&&!published898_&&!candidateCount8A0_&&!candidateCapacity8A4_&&
            !scratchDirty16_&&!headA4_&&!tailA8_&&!countAC_&&!countB4_&&
            (!root94_||(!root94_->children0&&!root94_->items4&&nodeCount888_==1))&&
            (!items98_||items98_->fresh())&&(!scratch9C_||scratch9C_->fresh());
    }
    bool closeConstructor()noexcept {
        if(!reference00_)return true;
        if(!application_||!constructorStateOnly()||reference00_!=1||
           (published_&&application_->visibility443A98_!=this))return false;
        --reference00_; //104970 reaches2AF060, then263BF0.
        if(published_)application_->visibility443A98_=nullptr;
        application_->activeTree442E18_=this;destroyingA0_=true;
        candidate89C_.reset();
        if(root94_){root94_.reset();--nodeCount888_;}
        if(items98_){--items98_->reference00;items98_.reset();}
        if(scratch9C_){--scratch9C_->reference00;scratch9C_.reset();}
        selected18_.reset();
        //Native borrow invalidation, not a claimed additional source store.
        application_->activeTree442E18_=nullptr;application_=nullptr;return true;
    }
    OriginalVisibilityQuadTree50LA()=default;
public:
    ~OriginalVisibilityQuadTree50LA(){if(reference00_)std::terminate();}
    OriginalVisibilityQuadTree50LA(const OriginalVisibilityQuadTree50LA&)=delete;
    OriginalVisibilityQuadTree50LA& operator=(const OriginalVisibilityQuadTree50LA&)=delete;
};
}
