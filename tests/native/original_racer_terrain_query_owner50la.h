#pragma once
#include "terrain_probe_authoring50fq_fixture.h"
#include "../../src/game/original_collision_base_owner50la.h"
#include "../../src/game/original_wreck_category_list50la.h"
#include "../../src/game/original_racer_main_body_owner50la.h"
#include <array>
#include <optional>
#include "../../src/game/original_wreck_type2_owners50la.h"
#include "native_terrain_result_owner50la.h"
#include <cfenv>
#include <cmath>

namespace native_racer_model50LA {class Owner;}
namespace rr::game {
// Counted source348. Numerical storage remains the existing registered Bank
// lanes; this owner supplies the missing Entity, source aliases and lifetime.
// All methods are private source callers, never a replacement public probe API.
class OriginalRacerTerrainQueryOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    using C=OriginalWreckComponent50LA;
    using S=OriginalWreckComponentStatus50LA;
    using R=OriginalWreckComponentResult50LA;
    using Stage=::terrain_probe_authoring50fq_fixture::SourceStage348;
    using Numeric=::terrain_probe_authoring50fq_fixture::Owner;
    OriginalCollisionBaseOwner50LA base_;
    OriginalWreckCategoryList50LA floorList_;
    OriginalWreckType2Owners50LA payload_;
    //Temporary113070 tree leaf and midpoint scratch survive rejected prefixes.
    struct SourceLeaf {std::optional<float> volume00;Type2Ray49X endpoints10;};
    std::unique_ptr<SourceLeaf> sourceLeaf_;
    std::unique_ptr<SpringVector4_48N> midpoint_;
    live_terrain_probe50fr::Owner* rowResource_=nullptr;
    native_terrain_result50LA::Owner* rowResult_=nullptr;
    bool shapeReadable_=false,sourceProjection_=false,nativeCancelled_=false;
    enum class SourceCallbackE4 {none,terrain1B0E50};
    SourceCallbackE4 sourceCallbackE4_=SourceCallbackE4::none;
    bool bindSelfCallback1B1478()noexcept {
        if(!payload_.loaded_||!sourceProjection_||retired_||sourceCallbackE4_!=SourceCallbackE4::none||
           !base_.callbacksE4_[0]||*base_.callbacksE4_[0]||!base_.owner9C_||*base_.owner9C_)return false;
        //Typed code identity, following the existing690/364 callback scheme.
        base_.callbacksE4_[0].reset();sourceCallbackE4_=SourceCallbackE4::terrain1B0E50; //1B1478
        base_.owner9C_=this; //1B147C actual concrete348 self, not Entity+8.
        return true;
    }
    bool callbackCurrent()const noexcept {
        for(unsigned i=1;i<4;++i)if(!base_.callbacksE4_[i]||*base_.callbacksE4_[i])return false;
        if(sourceCallbackE4_==SourceCallbackE4::none)return base_.callbacksE4_[0]&&
            !*base_.callbacksE4_[0]&&base_.owner9C_&&!*base_.owner9C_;
        return payload_.loaded_&&sourceProjection_&&!base_.callbacksE4_[0]&&
            base_.owner9C_&&*base_.owner9C_==this;
    }
    const void* shapeHistory_=nullptr;
    const void* resultHistory_=nullptr;
    std::uint16_t allocationMask_=0;
    void observePayloadCut()noexcept {
        allocationMask_=payload_.freshLiveAllocationMask();
        shapeHistory_=payload_.shape8C_.get();resultHistory_=payload_.result90_.get();
    }
    bool constructShape11C3F0(const LocalSegment48I& segment,
        live_terrain_probe50fr::Owner& rowResource,native_terrain_result50LA::Owner& rowResult)noexcept {
        if(!setupComplete_||retired_||payload_.attempted_||!base_.freshShapeNull()||
           !body128_||!*body128_||!(*body128_)->scene288()||
           !(*body128_)->entity().reference00||
           (*body128_)->entity().sessionGeneration!=base_.entity_.sessionGeneration||
           !(*body128_)->scene288()->constructed||(*body128_)->scene288()->released||
           !stage_||!stage_->live_||stage_->live_->initialized||
           rowResource.initialized||rowResource.sourceShape8C||rowResource.sourceWithdrawn||
           rowResult.constructed_||rowResult.revoked_||rowResult.sourceResult90_||rowResult.sourceWithdrawn_)
            return false;
        //11C424 calls11B600 first. Fresh known-null8C/90 takes its null path;
        //no prior legacy payload is accepted or reset here.
        //LocalSegment contains only XYZ. W comes from the actual registered
        //130/140 source lanes, not an out-of-bounds fourth segment word.
        if(!numeric_||numeric_->receipt.probe130.v[3]!=1.f||numeric_->receipt.probe140.v[3]!=1.f||
           std::memcmp(segment.first.v,numeric_->receipt.probe130.v,12)||
           std::memcmp(segment.second.v,numeric_->receipt.probe140.v,12))return false;
        for(unsigned i=0;i<16;++i)if(segment.worldMatrix.v[i]!=(i%5u==0?1.f:0.f))return false;
        rowResource_=&rowResource;rowResult_=&rowResult;
        payload_.attempted_=true;payload_.boundary_=0x11c42cu;
        try {
            payload_.shape8C_=std::make_unique<OriginalWreckType2Owners50LA::Shape>();
            base_.shape8C_=payload_.shape8C_.get(); //11C434
            base_.shape88_=2; //11C43C, before temporary leaf allocation.
            payload_.boundary_=0x11c44cu;
            sourceLeaf_=std::make_unique<SourceLeaf>();
            const SpringVector4_48N unit{{0,0,0,1}};
            sourceLeaf_->endpoints10.first=sourceLeaf_->endpoints10.second=unit;
            auto& s=*payload_.shape8C_;
            s.matrix00={};for(unsigned i=0;i<4;++i)s.matrix00.v[i*5]=1.f;
            payload_.boundary_=0x1130b0u;
            midpoint_=std::make_unique<SpringVector4_48N>();
            //Native count-one/identity lowering of113070+112D30. Admit only
            //ordinary finite nonnegative-zero lanes: avoid claiming VU NaN,
            //subnormal, signed-zero, overflow, or alternate rounding behavior.
            if(std::fegetround()!=FE_TONEAREST){observePayloadCut();return false;}
            for(unsigned i=0;i<3;++i){
                const float a=segment.first.v[i],b=segment.second.v[i];
                const auto ordinary=[](float x){return std::isfinite(x)&&
                    (x==0.f?!std::signbit(x):std::isnormal(x));};
                if(!ordinary(a)||!ordinary(b)){observePayloadCut();return false;}
                volatile float sum=a+b;volatile float half=.5f*sum;
                if(!ordinary(sum)||!ordinary(half)){observePayloadCut();return false;}
                midpoint_->v[i]=half;
            }
            midpoint_->v[3]=1;
            sourceLeaf_->volume00=-1.f; //112D8C count-one leaf marker.
            //Identity inverse preserves admitted normal/+zero actual130/140.
            //Only now does112D94/DA0 write leaf vectors, never during midpoint work.
            sourceLeaf_->endpoints10.first=numeric_->receipt.probe130;
            sourceLeaf_->endpoints10.second=numeric_->receipt.probe140;
            midpoint_.reset(); //113250 after112D30 leaf return.
            s.root40=type2Leaf49X;s.nodeCount=0;s.maxDepth=0; //11C50C/514
            s.racerModel44=(*body128_)->scene288(); //actual body128->288,11C51C
            s.normalMode54=1;s.count48=1; //11C524/52C
            payload_.boundary_=0x11c588u;
            s.rays50=std::make_unique<Type2Ray49X[]>(1);
            s.rays50[0]=sourceLeaf_->endpoints10; //113880 leaf, exact two128-bit copies.
            sourceLeaf_.reset(); //113290 leaf release, before result allocation.
            shapeReadable_=true;
            payload_.boundary_=0x11c5b4u;
            payload_.result90_=std::make_unique<OriginalWreckType2Owners50LA::Result>();
            auto& r=*payload_.result90_;
            r.first00=r.second10=r.normal20=r.point30=r.correction40=unit;
            r.count5C=3; //11C60C, NOT count60 reset or selectedFraction74.
            payload_.boundary_=0x11c614u;r.words64=std::make_unique<std::uint32_t[]>(3);
            payload_.boundary_=0x11c624u;r.flags68=std::make_unique<std::uint32_t[]>(3);
            payload_.boundary_=0x11c634u;r.fractions6C=std::make_unique<float[]>(3);
            payload_.boundary_=0x11c644u;r.tags70=std::make_unique<std::uint16_t[]>(3);
            payload_.boundary_=0x11c65cu;r.pairs50=std::make_unique<Type2Ray49X[]>(3);
            for(unsigned i=0;i<3;++i)r.pairs50[i].first=r.pairs50[i].second=unit;
            payload_.boundary_=0x11c68cu;r.normals54=std::make_unique<SpringVector4_48N[]>(3);
            for(unsigned i=0;i<3;++i)r.normals54[i]=unit;
            payload_.boundary_=0x11c6bcu;r.points58=std::make_unique<SpringVector4_48N[]>(3);
            for(unsigned i=0;i<3;++i)r.points58[i]=unit;
            base_.result90_=payload_.result90_.get(); //11C6E4 publication only now.
            payload_.loaded_=true;payload_.boundary_=0x11c6e4u;
        }catch(...){observePayloadCut();return false;}
        observePayloadCut();
        //Both retained-row and registered Bank resource are non-owning views
        //of ONE payload allocation. No copy to either embedded legacy pair.
        const auto project=[&](live_terrain_probe50fr::Owner& live){
            live.resource=payload_.resource();live.sourceShape8C=payload_.shape8C_.get();live.initialized=true;
        };
        project(rowResource);project(*stage_->live_);
        rowResult.sourceResult90_=payload_.result90_.get();sourceProjection_=true;
        return true;
    }
    bool payloadCurrent()const noexcept {
        if(!payload_.attempted_)return base_.freshShapeNull()&&!sourceProjection_&&
            !numeric_->prepared&&!stage_->live_->initialized;
        if(payload_.retired_||allocationMask_!=payload_.freshLiveAllocationMask()||
           shapeHistory_!=payload_.shape8C_.get()||resultHistory_!=payload_.result90_.get()||
           !base_.shape8C_||*base_.shape8C_!=shapeHistory_||!base_.result90_||
           *base_.result90_!=(payload_.loaded_?resultHistory_:nullptr)||
           base_.shape88_!=(shapeHistory_?2u:0u)||!rowResource_||!rowResult_)return false;
        if(shapeReadable_&&(!body128_||!*body128_||
           payload_.shape8C_->racerModel44!=(*body128_)->scene288()))return false;
        if(!sourceProjection_)return !payload_.loaded_&&!numeric_->prepared&&!rowResource_->initialized&&
            !stage_->live_->initialized&&!rowResult_->sourceResult90_;
        const auto matches=[&](const live_terrain_probe50fr::Owner& l){return
            l.initialized&&!l.sourceWithdrawn&&l.sourceShape8C==shapeHistory_&&
            l.resource.rays==payload_.shape8C_->rays50.get()&&l.resource.rayCount==1&&
            !l.resource.nodes&&!l.resource.nodeCount&&l.resource.root==type2Leaf49X;};
        return payload_.loaded_&&matches(*rowResource_)&&matches(*stage_->live_)&&
            !rowResult_->constructed_&&!rowResult_->revoked_&&!rowResult_->sourceWithdrawn_&&
            rowResult_->sourceResult90_==resultHistory_;
    }
    void retirePayload()noexcept {
        if(!payload_.attempted_)return;
        //Withdraw every native borrow BEFORE releasing allocations. Source
        //base aliases below remain historical on incomplete cancellation.
        const auto withdraw=[](live_terrain_probe50fr::Owner& l){l.resource={};
            l.initialized=false;l.sourceWithdrawn=true;l.sourceShape8C=nullptr;};
        withdraw(*rowResource_);withdraw(*stage_->live_);
        rowResult_->sourceResult90_=nullptr;rowResult_->sourceWithdrawn_=true;
        if(payload_.loaded_){
            if(!payload_.releaseShapeResult11B600())std::terminate();
            base_.shape8C_=nullptr;base_.result90_=nullptr;return;
        }
        const bool sourceShapeOnly=shapeReadable_&&!payload_.result90_;
        //Incomplete headers are NOT interpreted as source pointer/count fields.
        //Release only host allocations actually tracked at the stopped cut.
        midpoint_.reset();sourceLeaf_.reset();
        if(payload_.shape8C_){payload_.shape8C_->nodes4C.reset();payload_.shape8C_->rays50.reset();}
        if(payload_.result90_){auto& r=*payload_.result90_;
            r.fractions6C.reset();r.pairs50.reset();r.tags70.reset();r.normals54.reset();
            r.flags68.reset();r.words64.reset();r.points58.reset();}
        payload_.shape8C_.reset();payload_.result90_.reset();payload_.retired_=true;
        if(sourceShapeOnly){base_.shape8C_=nullptr;base_.result90_=nullptr;}
        else nativeCancelled_=true; //Not a claimed11B900/904 source store.
    }
    Stage* stage_=nullptr;
    Numeric* numeric_=nullptr; //dereference only inside its actual Bank slot
    std::size_t slot_=0;
    bool constructorAttempted_=false,constructorComplete_=false;
    bool setupAttempted_=false,setupComplete_=false,retired_=false;
    std::optional<const OriginalRacerMainBodyOwner50LA*> body128_;
    std::optional<const void*> terrain120_,water124_,owner12C_;
    //These are address borrows. Unknown48C is not consumed by212090/94.
    const std::optional<const void*>* listAddress1E0_=nullptr;
    const std::optional<std::uint32_t>* countAddress1E4_=nullptr;
    std::uint32_t enabled100_=0,word214_=0;
    std::array<std::uint32_t,8> bytes218_{};
    SpringVector4_48N unused1A0_{},unused1B0_{};
    //Only the otherwise unrepresented W companions live here; XYZ below use
    //existing numerical storage, never a second writable numeric projection.
    std::array<std::optional<float>,6> vectorW_{};
    static void xyz(Vector3_48C& to,const SpringVector4_48N& from)noexcept {
        for(unsigned n=0;n<3;++n)to.v[n]=from.v[n];
    }
    R construct1B0EE0(Stage& stage,OriginalApplicationEntities50LA& app,
        std::size_t slot,std::uint64_t generation,std::uint64_t key,
        const SpringVector4_48N& actual3E02C0,const SpringVector4_48N& actual434260,
        const SpringVector4_48N& actual434280)noexcept {
        //Native admission before counted effects. Do not adopt prepared legacy
        //storage or infer constructor fields from its zero-initialized backing.
        if(constructorAttempted_||stage.withdrawn_||!stage.reserved_||stage.constructorKnown_||!stage.owner_||
           !stage.live_||stage.owner_->prepared||stage.live_->initialized||
           stage.owner_->spring1F4||stage.owner_->damping1F8||stage.owner_->gravity1FC||
           stage.owner_->sideForce200)return {S::owner,0x1b0ee0u};
        constructorAttempted_=true;stage_=&stage;numeric_=stage.owner_;slot_=slot;
        //Explicit native opaque-bit admission, not recovered guest heap words.
        //Reuse the SAME immutable source namespace bindings as old numeric callers.
        base_.entity_.flags29=stage.binding_.flags31;
        base_.flagsF8_=stage.binding_.preservedFlagsF8;
        auto r=base_.construct11B190(app,generation,key,actual3E02C0);
        if(r.status!=S::complete)return r;
        base_.entity_.kind=OriginalWreckComponentKind50LA::terrain1B0EE0; //1B0F10/24
        auto& n=*numeric_;const SpringVector4_48N vf0{{0,0,0,1}};
        n.receipt.probe130=vf0; //2E8860 invokes1048E0 at130 then140.
        n.receipt.probe140=vf0;xyz(n.lowerEndpoint140,vf0);
        xyz(n.initial.terrain.surface.point150,vf0);vectorW_[0]=1;
        for(unsigned i=0;i<4;++i)n.initial.auxiliaryPoint160[i]=vf0.v[i];
        xyz(n.initial.terrain.surface.normal170,vf0);vectorW_[1]=1;
        xyz(n.initial.follow.previousPoint180,vf0);vectorW_[2]=1;
        xyz(n.initial.follow.previousNormal190,vf0);vectorW_[3]=1;
        unused1A0_=vf0;unused1B0_=vf0;
        xyz(n.follow.localOffset1c0,vf0);vectorW_[4]=1;
        xyz(n.initial.follow.largestVelocityChange1d0,vf0);vectorW_[5]=1;
        enabled100_=1;
        n.initial.follow.active104=0;n.initial.terrain.surface.forceAngleTest108=0;
        n.initial.follow.pending10c=0;n.initial.terrain.surface.hit110=0;
        n.initial.terrain.surface.hit114=0;n.initial.auxiliaryAbove118=0;
        n.initial.follow.detached11c=0;
        terrain120_=nullptr;water124_=nullptr;body128_=nullptr;owner12C_=nullptr;
        n.receipt.probe130=actual434260;
        n.receipt.probe140=actual434260;xyz(n.lowerEndpoint140,actual434260);
        xyz(n.follow.localOffset1c0,actual434260);vectorW_[4]=actual434260.v[3];
        xyz(n.initial.terrain.surface.point150,actual434260);vectorW_[0]=actual434260.v[3];
        for(unsigned i=0;i<4;++i)n.initial.auxiliaryPoint160[i]=actual434260.v[i];
        xyz(n.initial.terrain.surface.normal170,actual434280);vectorW_[1]=actual434280.v[3];
        xyz(n.initial.follow.previousNormal190,actual434260);vectorW_[3]=actual434260.v[3];
        xyz(n.initial.follow.previousPoint180,actual434260);vectorW_[2]=actual434260.v[3];
        xyz(n.initial.follow.largestVelocityChange1d0,actual434260);vectorW_[5]=actual434260.v[3];
        listAddress1E0_=nullptr;countAddress1E4_=nullptr;n.initial.terrain.collectedCount=0;
        //1EC constructor-null publication is separate from later native fixed
        //backing allocation admission. No collected array slot is initialized.
        collected1EC_=nullptr;
        n.lift1F0=0;n.spring1F4=0;n.damping1F8=0;n.gravity1FC=0;
        n.follow.detachAngle208=std::bit_cast<float>(0x3f1c61a9u);
        n.follow.detachDistance20c=5.f;n.sideForce200=0;n.follow.airborneTorque204=0;
        word214_=0;bytes218_.fill(0); //306478 actual32-byte zero fill.
        stage.constructorKnown_=true;constructorComplete_=true;
        return {S::complete,0x1b1128u};
    }
    //Setup/body binding are separate from constructor publication21206C.
    //Definition joins private mode0 source allocation before caller admission.
    R setup1B11B0(const OriginalRacerMainBodyOwner50LA& actualBody,
        const void* actualTerrain330,const void* actualWater334,
        std::uint32_t actualFloor4342A0,std::uint8_t& actualGlobal3E0730)noexcept {
        if(!constructorComplete_||setupAttempted_||retired_||!stage_||stage_->withdrawn_||
           !numeric_||numeric_->prepared||stage_->live_->initialized||!base_.freshShapeNull()||
           actualBody.entity().reference00!=1u||
           actualBody.entity().sessionGeneration!=base_.entity_.sessionGeneration||
           actualFloor4342A0!=stage_->binding_.floorCategory||!actualFloor4342A0)
            return {S::owner,0x1b11b0u};
        setupAttempted_=true;
        base_.flagsF8_=static_cast<std::uint8_t>(base_.flagsF8_&~8u); //11B594
        base_.flagsF8_=static_cast<std::uint8_t>(base_.flagsF8_|2u);  //11B5C0
        base_.flagsF8_=static_cast<std::uint8_t>(base_.flagsF8_|4u);  //11B5E4
        actualGlobal3E0730=0; //11B5F4; following141C20 is actual identity return.
        if(!floorList_.selectFreshMode0124160(base_.entity_,base_.listD8_,base_.countDC_,base_.modeD4_))
            return {S::owner,0x124160u};
        if(!floorList_.appendMode0124170(base_.listD8_,base_.countDC_,base_.modeD4_,actualFloor4342A0))
            return {S::capacity,0x124214u};
        body128_=&actualBody; //1B11FC
        terrain120_=actualTerrain330; //1B1200
        water124_=actualWater334; //1B120C delay slot, before allocator return
        //Native fixed backing policy: the exact registered16 words already
        //exist, but no word is source-written or consumable merely by this
        //publication. Not a literal guest allocation/failure equivalence.
        collected1EC_=&numeric_->initial.terrain.collectedIdentities; //1B1214
        setupComplete_=true;return {S::complete,0x1b1238u};
    }
    bool preflightClose(const C* racer,const C* previous,const C* next=nullptr)const noexcept {
        if(retired_)return true;
        if(!constructorComplete_||!stage_||!numeric_||!stage_->live_||
           !payloadCurrent()||!callbackCurrent()||
           !base_.membershipE0_.has_value()||*base_.membershipE0_||base_.listC0_||
           base_.listCC_||base_.countC4_||base_.countD0_||!collected1EC_.has_value()||
           (*collected1EC_&&*collected1EC_!=&numeric_->initial.terrain.collectedIdentities))return false;
        if(floorList_.mode0Route_){
            if(!floorList_.matchesMode0(base_.listD8_,base_.countDC_,base_.modeD4_))return false;
        }else if(!base_.constructorNullListsCurrent())return false;
        return base_.preflightClose142590(base_.appended_?racer:nullptr,
            base_.appended_?previous:nullptr,base_.appended_?next:nullptr);
    }
    R close142590(const C* racer,const C* previous)noexcept {
        if(retired_)return {S::complete,nativeCancelled_?payload_.boundary_:0x1b11a4u};
        if(!preflightClose(racer,previous))return {S::owner,0x142590u};
        stage_->withdrawn_=true; //Native one-way consumer withdrawal before reference drop.
        if(!base_.dropTailReference142590(base_.appended_?racer:nullptr,
            base_.appended_?previous:nullptr))return {S::topology,0x142590u};
        collectedCapabilityRetired_=true; //1B1164 fixed backing release; history untouched.
        base_.entity_.kind=OriginalWreckComponentKind50LA::collision11B190; //11B4A0 base vtable.
        R r;
        if(floorList_.mode0Route_){
            if(!floorList_.releaseMode0AfterReferenceDrop11B51C(base_.entity_,
                base_.listD8_,base_.countDC_,base_.modeD4_))return {S::owner,0x11b51cu};
            retirePayload();
            if(nativeCancelled_){
                //Native cancellation finalizes the counted owner after proven
                //tail-drop, filter retirement and disposal of every allocation.
                //Keep stale8C/90 history; do NOT certify source destructor return.
                if(base_.entity_.reference00||!payload_.retired_||payload_.shape8C_||payload_.result90_||
                   !floorList_.retiredMode0History(base_.entity_,base_.listD8_,base_.countDC_,base_.modeD4_))
                    std::terminate();
                base_.entity_.kind=OriginalWreckComponentKind50LA::entity141300;
                --base_.application_->objectCount423F60();
                base_.phase_=OriginalCollisionBaseOwner50LA::Phase::released;
                r={S::complete,payload_.boundary_};
            }else r=base_.retireBase1413E0AfterMode0Payload(floorList_);
        }else {retirePayload();r=base_.retireBase1413E0AfterPayload();}
        if(r.status!=S::complete)return r;
        retired_=true;return {S::complete,nativeCancelled_?payload_.boundary_:0x1b11a4u};
    }
    bool bindSourceList212090(const std::optional<const void*>& actual488,
        const std::optional<std::uint32_t>& actual48C)noexcept {
        if(!setupComplete_||listAddress1E0_||countAddress1E4_||retired_)return false;
        listAddress1E0_=&actual488; //212090; do not consume pointer value.
        countAddress1E4_=&actual48C; //212094; unknown count remains unknown.
        return true;
    }
    R append2120AC(const OriginalWreckComponentDomain50LA& domain,C& racer)noexcept {
        if(!setupComplete_||!listAddress1E0_||!countAddress1E4_||retired_)
            return {S::owner,0x2120acu};
        return base_.append141960(domain,racer,0u);
    }
    void withdrawNativeConsumers()noexcept{if(stage_)stage_->withdrawn_=true;}
    bool appended()const noexcept{return base_.appended_&&!retired_;}
    C* liveEntity()noexcept{return base_.entity_.reference00?&base_.entity_:nullptr;}
    std::optional<std::array<std::uint32_t,16>*> collected1EC_;
    bool collectedCapabilityRetired_=false;
    std::uint16_t collectedWritten_=0; //native guard, not source count1E8.
public:
    OriginalRacerTerrainQueryOwner50LA()=default;
    OriginalRacerTerrainQueryOwner50LA(const OriginalRacerTerrainQueryOwner50LA&)=delete;
    OriginalRacerTerrainQueryOwner50LA& operator=(const OriginalRacerTerrainQueryOwner50LA&)=delete;
    const C& entity()const noexcept{return base_.entity();}
};
}
