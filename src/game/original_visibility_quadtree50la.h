#pragma once
#include "original_selected_array_guards27ed40.h"
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
#include <cstring>
#include <new>

namespace ra_pod_scene50LA {class Owner;}
namespace rr::platform {class OriginalUiGarageVisibility50KG;}
namespace rr::game {
// Future registrations must retain their actual native object identity here.
// No guest pointer, render-mesh surrogate, or synthetic collection identity.
struct OriginalVisibilityMembership50LA;
class OriginalVisibilityQuadTree50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class rr::platform::OriginalUiGarageVisibility50KG;
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
    enum class EmptyQueryPhase {none,epochAdvanced,returned};
    EmptyQueryPhase emptyQueryPhase_=EmptyQueryPhase::none;
    std::uint64_t emptyQueryPublication_=0;

    static float binary32(float v)noexcept{volatile float rounded=v;return rounded;}
    //Source-only constructor custody on the SAME tree/backings. Unreached
    //native zero defaults are not source observations. The receipt's store
    //bits/known masks are the authority; native cleanup is not a destructor.
    struct SourceConstruction {
        //Private water caller admits only successful native backing. Refusal
        //does not stand in for2E8D90's mutable new-handler/throw/NULL behavior.
        bool requireNativeAllocationSuccess=false;
        enum class NativeAllocationRefusal:std::uint8_t {none,selected,root,scratch,items};
        NativeAllocationRefusal nativeAllocationRefusal=NativeAllocationRefusal::none;
        bool sourceMode=false,baseConstructed=false,selectedInitialized=false;
        bool selectedStored=false,innerPublished=false,constructorReturned=false;
        bool configurationEntered=false,resetStored=false,rootStored=false;
        bool boundsStored=false,scaleStored=false,quantaComplete=false;
        bool scratchResetStored=false,scratchPoolStored=false,itemsPoolStored=false;
        bool configurationReturned=false,capacityReturned=false,withdrawn=false;
        enum class VtableStage {none,searchTree27E9D0,quadTree2639B0,visibility2AEF70};
        VtableStage vtableStage=VtableStage::none;
        bool derivedRootConstructed=false;
        std::optional<std::uint32_t> rootVtable10,scratchPoolVtable8,itemsPoolVtable8;
        bool scratchPoolConstructed=false,itemsPoolConstructed=false;
        std::uint8_t typesWritten=0,selected28Known=0,selected28Bits=0;
        std::uint8_t destroyingA0Known=0,destroyingA0Bits=0,scratch16Known=0,scratch16Bits=0;
        std::uint16_t quantaWritten=0;
        std::array<std::uint32_t,4> boundsBits{};
        std::array<std::uint32_t,12> writtenQuanta{}; //Only mask-selected entries are known.
        std::uint32_t scaleBits=0,processQuantumBits=0,extentXBits=0,extentZBits=0;
        std::uint32_t selectedAllocationReturn=0,rootAllocationReturn=0;
        std::uint32_t scratchAllocationReturn=0,itemsAllocationReturn=0;
        std::uint32_t lastPc=0;
        selected_array_guards27ED40::History selectedGuards;
    } sourceConstruction_;
    static std::uint32_t sourceTreeFloatBits(float value)noexcept {
        std::uint32_t bits=0;std::memcpy(&bits,&value,sizeof bits);return bits;
    }
    static float sourceTreeFloat(std::uint32_t bits)noexcept {
        float value=0;std::memcpy(&value,&bits,sizeof value);return value;
    }
    bool constructSource2AEF70(OriginalApplicationEntities50LA& app,std::uint32_t& pc,
        bool requireNativeAllocationSuccess=false)noexcept {
        auto& r=sourceConstruction_;pc=0;
        //Fresh source route excludes legacy constructors and a foreign singleton.
        if(r.sourceMode||reference00_||application_||published_||configured_||
           selected18_||root94_||items98_||scratch9C_||app.visibility443A98_){pc=0x2aef70;return false;}
        r.sourceMode=true;r.requireNativeAllocationSuccess=requireNativeAllocationSuccess;application_=&app;
        reference00_=1;r.baseConstructed=true; //27E9DC ->1048F0.
        r.vtableStage=SourceConstruction::VtableStage::searchTree27E9D0; //27E9F0.
        selectedCount18_=selectedGrowth20_=selectedCapacity24_=0;selectedOwned28_=false;
        r.selected28Known=1;r.selected28Bits=0;r.selectedInitialized=true; //27ED10.
        usage10_=0; //27EA0C.
        if(!selected_array_guards27ED40::runFresh30(r.selectedGuards,pc)){r.lastPc=pc;return false;}
        //The existing particle adapter retains its nullable result policy.
        //Water refuses native failure before any source result/publication.
        //No fictitious selected entries are initialized.
        selected18_.reset(new(std::nothrow) OriginalVisibilityMembership50LA*[30]);
        if(!selected18_&&r.requireNativeAllocationSuccess){
            r.nativeAllocationRefusal=SourceConstruction::NativeAllocationRefusal::selected;
            pc=r.lastPc=0x27ee38;return false;
        }
        r.selectedAllocationReturn=selected18_?1u:0u;
        selectedGrowth20_=selectedCapacity24_=30;selectedOwned28_=true;
        r.selected28Bits=1;r.selectedStored=true;
        auto type=app.types383408().findOrAppend2A31E0("SoultreeObject");
        if(!type.okay()){pc=r.lastPc=type.sourcePC;return false;}
        soultreeType2C_=type.type;r.typesWritten|=1;
        type=app.types383408().findOrAppend2A31E0("SearchableGameObject");
        if(!type.okay()){pc=r.lastPc=type.sourcePC;return false;}
        searchableType2D_=type.type;r.typesWritten|=2;
        r.vtableStage=SourceConstruction::VtableStage::quadTree2639B0; //2AEF94.
        r.vtableStage=SourceConstruction::VtableStage::visibility2AEF70; //2AEFA8.
        //The caller admitted singletonNULL before entering this constructor;
        //therefore2AEFB0 takes its real non-assertion branch.
        if(app.visibility443A98_){pc=r.lastPc=0x2aefac;return false;}
        app.visibility443A98_=this;published_=true;r.innerPublished=true; //2AF034.
        nodeCount888_=0;root94_.reset();requestedCapacity8C0_=0; //2AF038/40/48.
        r.constructorReturned=true;pc=r.lastPc=0x2af058;return true;
    }
    bool configureSource2AF0D0(const std::array<std::uint32_t,4>& bounds,std::uint32_t& pc)noexcept {
        auto& r=sourceConstruction_;pc=0;
        if(!r.constructorReturned||r.configurationEntered||r.withdrawn||!application_||
           application_->visibility443A98_!=this||!preflightSourceConstructionNative()){
            pc=0x2af0d0;return false;
        }
        //Native arithmetic admission, NOT a source branch or allocation result.
        //The outer272FC0 owner supplies its actual authored Xmin,Xmax,Zmin,Zmax
        //words. Reject unsupported host/exceptional arithmetic before source
        //effects, without treating arbitrary zero defaults as known bounds.
        if(bounds!=std::array<std::uint32_t,4>{{0x80000000u,0x46f00000u,0x80000000u,0x46f00000u}}||
           std::fegetround()!=FE_TONEAREST){pc=0x263a78;return false;}
        const float xmin=sourceTreeFloat(bounds[0]),xmax=sourceTreeFloat(bounds[1]);
        const float zmin=sourceTreeFloat(bounds[2]),zmax=sourceTreeFloat(bounds[3]);
        if(!std::isfinite(xmin)||!std::isfinite(xmax)||!std::isfinite(zmin)||!std::isfinite(zmax)){
            pc=0x263a48;return false;
        }
        const float ex=binary32(xmax-xmin),ez=binary32(zmax-zmin);
        if(!std::isfinite(ex)||!std::isfinite(ez)||ex<=0.f||ez<=0.f){pc=0x263a78;return false;}
        const float maximum=ex>ez?ex:ez;
        const float scale=binary32(32768.f/maximum),quantum=binary32(1.f/scale);
        const float minimum=binary32(96.f*scale);
        if(!std::isfinite(scale)||scale<=0.f||!std::isfinite(quantum)||
           !std::isfinite(minimum)||minimum<0.f||minimum>2147483520.f){pc=0x263ac0;return false;}
        const auto converted=static_cast<std::int32_t>(std::nearbyint(minimum));
        r.configurationEntered=true;r.boundsBits=bounds;
        //2639B0 occurs before virtual4C/root allocation. Unknown upper byte
        //bits are retained abstractly: only bit0 is known to be cleared.
        application_->activeTree442E18_=this;nodeCount888_=0;field88C_=-1;
        destroyingA0_=false;r.destroyingA0Known=1;r.destroyingA0Bits=0;
        field890_=field894_=candidateCount8A0_=candidateCapacity8A4_=0;
        published898_=nullptr;candidate89C_.reset();minimumSize8B8_=0;r.resetStored=true;
        //Derived vtable3D4580+4C ->2AF160 ->2AF190 ->264CD0. Null
        //does not call node constructor/increment888, but stores null94.
        root94_.reset(new(std::nothrow) Node);
        if(!root94_&&r.requireNativeAllocationSuccess){
            r.nativeAllocationRefusal=SourceConstruction::NativeAllocationRefusal::root;
            pc=r.lastPc=0x2af168;return false;
        }
        r.rootAllocationReturn=root94_?1u:0u;
        if(root94_){++nodeCount888_;r.derivedRootConstructed=true;r.rootVtable10=0x3d4560u;}r.rootStored=true;
        originX30_=xmin;maximumX34_=xmax;originZ38_=zmin;maximumZ3C_=zmax;
        extentX48_=ex;extentZ4C_=ez;minimumSize8B8_=96.f;
        r.extentXBits=sourceTreeFloatBits(ex);r.extentZBits=sourceTreeFloatBits(ez);r.boundsStored=true;
        scale5C_=scale;application_->treeQuantum442E20_=quantum;
        r.scaleBits=sourceTreeFloatBits(scale);r.processQuantumBits=sourceTreeFloatBits(quantum);r.scaleStored=true;
        maximumLevel54_=0;std::uint32_t q=32768;
        do {
            const auto index=maximumLevel54_;q>>=1;++maximumLevel54_;
            quanta60_[index]=q;r.writtenQuanta[index]=q;r.quantaWritten|=std::uint16_t(1u<<index);
        }while(maximumLevel54_<12u&&converted<static_cast<std::int32_t>(q));
        --maximumLevel54_;smallestQuantum58_=quanta60_[maximumLevel54_];r.quantaComplete=true;
        scratchDirty16_=false;r.scratch16Known=1;r.scratch16Bits=0;field14_=0;
        headA4_=tailA8_=nullptr;countAC_=countB4_=0;r.scratchResetStored=true;
        //Independent allocations preserve earlier root/bounds/quanta effects.
        //Water's success-only policy stops on native refusal; the preexisting
        //particle adapter retains its nullable result policy.
        scratch9C_.reset(new(std::nothrow) Pool<ScratchItem,8>);
        if(!scratch9C_&&r.requireNativeAllocationSuccess){
            r.nativeAllocationRefusal=SourceConstruction::NativeAllocationRefusal::scratch;
            pc=r.lastPc=0x263b4c;return false;
        }
        r.scratchAllocationReturn=scratch9C_?1u:0u;
        if(scratch9C_){r.scratchPoolConstructed=true;r.scratchPoolVtable8=0x3cc9e0u;}r.scratchPoolStored=true;
        items98_.reset(new(std::nothrow) Pool<ItemBlock,20>);
        if(!items98_&&r.requireNativeAllocationSuccess){
            //263B74 is this call's delay slot: successful scratch+9C is
            //already published, but the later items+98 store is unreached.
            r.nativeAllocationRefusal=SourceConstruction::NativeAllocationRefusal::items;
            pc=r.lastPc=0x263b70;return false;
        }
        r.itemsAllocationReturn=items98_?1u:0u;
        if(items98_){r.itemsPoolConstructed=true;r.itemsPoolVtable8=0x3cc9e0u;}r.itemsPoolStored=true;
        configured_=true;r.configurationReturned=true;pc=r.lastPc=0x2af148;return true;
    }
    bool capacitySource2AF150(std::uint32_t& pc)noexcept {
        auto& r=sourceConstruction_;pc=0;
        if(!r.configurationReturned||r.capacityReturned||!preflightSourceConstructionNative()){
            pc=0x2af150;return false;
        }
        requestedCapacity8C0_=5000;r.capacityReturned=true;pc=r.lastPc=0x2af154;return true;
    }
    bool preflightSourceConstructionNative()const noexcept {
        const auto& r=sourceConstruction_;
        using Refusal=SourceConstruction::NativeAllocationRefusal;
        if(r.nativeAllocationRefusal!=Refusal::none&&!r.requireNativeAllocationSuccess)return false;
        if(r.requireNativeAllocationSuccess&&((r.selectedStored&&!r.selectedAllocationReturn)||
           (r.rootStored&&!r.rootAllocationReturn)||(r.scratchPoolStored&&!r.scratchAllocationReturn)||
           (r.itemsPoolStored&&!r.itemsAllocationReturn)))return false;
        switch(r.nativeAllocationRefusal){
        case Refusal::none:break;
        case Refusal::selected:
            if(r.lastPc!=0x27ee38u||!r.selectedGuards.returned||r.selectedStored||selected18_||r.typesWritten)return false;break;
        case Refusal::root:
            if(r.lastPc!=0x2af168u||!r.resetStored||r.rootStored||root94_||r.boundsStored)return false;break;
        case Refusal::scratch:
            if(r.lastPc!=0x263b4cu||!r.scratchResetStored||r.scratchPoolStored||scratch9C_||r.itemsPoolStored)return false;break;
        case Refusal::items:
            if(r.lastPc!=0x263b70u||!r.scratchPoolStored||r.itemsPoolStored||items98_)return false;break;
        default:return false;
        }
        if(!r.sourceMode||r.withdrawn||!r.baseConstructed||!application_||reference00_>3||
           !selected_array_guards27ED40::preflight(r.selectedGuards)||r.typesWritten>3||
           r.selected28Known!=1||r.selected28Bits>1||r.destroyingA0Known>1||r.scratch16Known>1||
           r.destroyingA0Bits||r.scratch16Bits||(r.quantaWritten&0xf000u))return false;
        if(!r.selectedInitialized||usage10_>2||selectedCount18_)return false;
        if(r.vtableStage!=(r.typesWritten==3?SourceConstruction::VtableStage::visibility2AEF70:
            SourceConstruction::VtableStage::searchTree27E9D0))return false;
        if(r.selectedStored){
            if(!r.selectedGuards.returned||selectedGrowth20_!=30||selectedCapacity24_!=30||
               !selectedOwned28_||r.selected28Bits!=1||bool(selected18_)!=bool(r.selectedAllocationReturn))return false;
        }else if(selected18_||selectedGrowth20_||selectedCapacity24_||selectedOwned28_||r.selected28Bits||r.typesWritten)return false;
        if(bool(soultreeType2C_)!=bool(r.typesWritten&1u)||bool(searchableType2D_)!=bool(r.typesWritten&2u)||
           ((r.typesWritten&2u)&&!(r.typesWritten&1u)))return false;
        if(r.innerPublished){
            if(r.typesWritten!=3||!published_||application_->visibility443A98_!=this)return false;
        }else if(published_||r.constructorReturned)return false;
        if(r.constructorReturned&&!r.innerPublished)return false;
        if(r.configurationEntered&&!r.constructorReturned)return false;
        if(r.resetStored){
            if(!r.configurationEntered||application_->activeTree442E18_!=this||r.destroyingA0Known!=1||
               destroyingA0_||field88C_!=-1||field890_||field894_||published898_||candidate89C_||
               candidateCount8A0_||candidateCapacity8A4_)return false;
        }else if(r.rootStored||r.boundsStored||r.scaleStored||r.quantaWritten||r.scratchResetStored)return false;
        if(r.rootVtable10!=(r.derivedRootConstructed?std::optional<std::uint32_t>(0x3d4560u):std::nullopt))return false;
        if(r.rootStored){
            if(bool(root94_)!=bool(r.rootAllocationReturn)||bool(root94_)!=r.derivedRootConstructed||nodeCount888_!=(root94_?1u:0u)||
               (root94_&&(root94_->children0||root94_->items4||root94_->field8||root94_->fieldCBits!=0xff7fffffu)))return false;
        }else if(r.constructorReturned&&(root94_||nodeCount888_))return false;
        if(r.boundsStored){
            if(!r.rootStored||sourceTreeFloatBits(originX30_)!=r.boundsBits[0]||sourceTreeFloatBits(maximumX34_)!=r.boundsBits[1]||
               sourceTreeFloatBits(originZ38_)!=r.boundsBits[2]||sourceTreeFloatBits(maximumZ3C_)!=r.boundsBits[3]||
               sourceTreeFloatBits(extentX48_)!=r.extentXBits||sourceTreeFloatBits(extentZ4C_)!=r.extentZBits||minimumSize8B8_!=96.f)return false;
        }else if(r.resetStored&&sourceTreeFloatBits(minimumSize8B8_))return false;
        if(r.scaleStored&&(!r.boundsStored||sourceTreeFloatBits(scale5C_)!=r.scaleBits||
           !application_->treeQuantum442E20_||sourceTreeFloatBits(*application_->treeQuantum442E20_)!=r.processQuantumBits))return false;
        if(r.quantaWritten&&!r.scaleStored)return false;
        for(unsigned i=0;i<12;++i)if((r.quantaWritten&(1u<<i))&&quanta60_[i]!=r.writtenQuanta[i])return false;
        if(r.quantaComplete&&(maximumLevel54_>=12||r.quantaWritten!=((1u<<(maximumLevel54_+1))-1u)||
           smallestQuantum58_!=quanta60_[maximumLevel54_]))return false;
        if(r.scratchResetStored&&(!r.quantaComplete||r.scratch16Known!=1||scratchDirty16_||field14_||headA4_||tailA8_||countAC_||countB4_))return false;
        if(r.scratchPoolConstructed!=bool(scratch9C_)||r.itemsPoolConstructed!=bool(items98_)||
           r.scratchPoolVtable8!=(r.scratchPoolConstructed?std::optional<std::uint32_t>(0x3cc9e0u):std::nullopt)||
           r.itemsPoolVtable8!=(r.itemsPoolConstructed?std::optional<std::uint32_t>(0x3cc9e0u):std::nullopt))return false;
        if(r.scratchPoolStored){if(!r.scratchResetStored||bool(scratch9C_)!=bool(r.scratchAllocationReturn)||
            (scratch9C_&&(!scratch9C_->fresh()||scratch9C_->field30||scratch9C_->bytes34)))return false;}
        else if(scratch9C_||r.itemsPoolStored)return false;
        if(r.itemsPoolStored){if(!r.scratchPoolStored||bool(items98_)!=bool(r.itemsAllocationReturn)||
            (items98_&&(!items98_->fresh()||items98_->field30||items98_->bytes34)))return false;}
        else if(items98_)return false;
        if(r.configurationReturned!=configured_||(r.configurationReturned&&!r.itemsPoolStored))return false;
        if(r.capacityReturned&&!r.configurationReturned)return false;
        if(r.constructorReturned&&requestedCapacity8C0_!=(r.capacityReturned?5000u:0u))return false;
        return true;
    }
    bool withdrawSourceConstructionNative()noexcept {
        auto& r=sourceConstruction_;
        //Parent has retired all SearchNode/outer references and borrowers.
        //This releases native backing only: no source destructor, ref decrement,
        //global reset, guard reset or constructor replay is claimed here.
        if(reference00_||!preflightSourceConstructionNative())return false;
        if((application_->visibility443A98_&&application_->visibility443A98_!=this)||
           (application_->activeTree442E18_&&application_->activeTree442E18_!=this))return false;
        if(application_->visibility443A98_==this)application_->visibility443A98_=nullptr;
        if(application_->activeTree442E18_==this)application_->activeTree442E18_=nullptr;
        selected18_.reset();root94_.reset();items98_.reset();scratch9C_.reset();candidate89C_.reset();
        published898_=nullptr;published_=configured_=false;application_=nullptr;r.withdrawn=true;return true;
    }

    bool construct(OriginalApplicationEntities50LA& app,
        const original_world_environment50KG::SpatialBounds& bounds,std::uint32_t& pc)noexcept {
        pc=0;
        if(reference00_||app.visibility443A98_||app.environment443108_||
           !app.application10()||std::fegetround()!=FE_TONEAREST){pc=0x272fd4;return false;}
        selected_array_guards27ED40::invalidateNative();
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
    bool emptyGarageQueryStateCurrent()const noexcept {
        if(constructorStateOnly())return true;
        return configured_&&application_&&selected18_&&selectedOwned28_&&
            selectedGrowth20_==30&&selectedCapacity24_==30&&!selectedCount18_&&
            root94_&&!root94_->children0&&!root94_->items4&&nodeCount888_==1&&
            items98_&&items98_->fresh()&&scratch9C_&&scratch9C_->fresh()&&
            !headA4_&&!tailA8_&&!countAC_&&!countB4_&&!field890_&&
            requestedCapacity8C0_==5000&&!candidateCount8A0_&&candidateCapacity8A4_==5000&&
            emptyQueryPublication_&&
            ((emptyQueryPhase_==EmptyQueryPhase::returned&&candidate89C_&&
              published898_==candidate89C_.get()&&field894_==5000)||
             (emptyQueryPhase_==EmptyQueryPhase::epochAdvanced&&!candidate89C_&&!published898_&&!field894_));
    }
    //Only the real Garage2B00E0 caller can enter this reached empty-root branch.
    //The live root/pools, not an authored zero-count promise, prove the branch.
    bool queryGarageEmpty2AF1D0(float cameraX,float cameraZ,std::uint64_t publication,
        std::uint32_t& pc)noexcept {
        pc=0;
        if(!configured_||!application_||!application_->treeQuantum442E20_||!publication||!root94_||root94_->children0||
           root94_->items4||nodeCount888_!=1||!items98_||!items98_->fresh()||
           !scratch9C_||!scratch9C_->fresh()||headA4_||tailA8_||countAC_||countB4_||
           field890_||requestedCapacity8C0_!=5000||std::fegetround()!=FE_TONEAREST){pc=0x2AF1D0;return false;}
        const float x=binary32(binary32(cameraX-originX30_)*scale5C_);
        const float z=binary32(binary32(cameraZ-originZ38_)*scale5C_);
        if(!std::isfinite(x)||!std::isfinite(z)||x< -2147483648.f||z< -2147483648.f||
           x>2147483520.f||z>2147483520.f){pc=0x2AF354;return false;}
        const auto gridX=static_cast<std::int32_t>(std::nearbyint(x));
        const auto gridZ=static_cast<std::int32_t>(std::nearbyint(z));
        //2AF3C0 reaches the actual root, computes its bounds, calls2664D0 on
        //root.items4(null), then exits at2B0088 because root.children0 is null.
        const float quantum=*application_->treeQuantum442E20_;
        const float centerX=binary32(originX30_+binary32(16384.f*quantum));
        const float centerZ=binary32(originZ38_+binary32(16384.f*quantum));
        const float half=binary32(8192.f*quantum);
        if(!std::isfinite(centerX)||!std::isfinite(centerZ)||!std::isfinite(half)){pc=0x2AF454;return false;}
        (void)gridX;(void)gridZ;
        if(emptyQueryPublication_!=publication){
            if(emptyQueryPhase_!=EmptyQueryPhase::none&&emptyQueryPhase_!=EmptyQueryPhase::returned){pc=0x2AF1D0;return false;}
            if(scratchDirty16_&&!reset265C60(pc))return false;
            ++field14_; //2AF254/260, real16-bit query epoch.
            emptyQueryPublication_=publication;emptyQueryPhase_=EmptyQueryPhase::epochAdvanced;
        }
        if(emptyQueryPhase_==EmptyQueryPhase::returned)return true;
        candidateCapacity8A4_=requestedCapacity8C0_; //Nonzero input capacity,2AF2A8.
        if(!candidate89C_){
            try{candidate89C_.reset(new OriginalVisibilityMembership50LA*[candidateCapacity8A4_]);}
            catch(...){pc=0x2AF2BC;return false;}
            field894_=candidateCapacity8A4_;
        }else if(field894_!=5000){pc=0x2AF264;return false;}
        candidateCount8A0_=0;published898_=candidate89C_.get();scratchDirty16_=true;
        application_->activeTree442E18_=this; //2AF330, before actual empty-root visit.
        emptyQueryPhase_=EmptyQueryPhase::returned;return true;
    }
    bool closeGarageEmptyQuery2AF060()noexcept {
        if(!reference00_)return true;
        if(!emptyGarageQueryStateCurrent())return false;
        return closeEmptyBacking2AF060();
    }
    bool closeConstructor()noexcept {
        if(!reference00_)return true;
        if(!constructorStateOnly())return false;
        return closeEmptyBacking2AF060();
    }
    bool closeEmptyBacking2AF060()noexcept {
        if(!application_||reference00_!=1||
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
