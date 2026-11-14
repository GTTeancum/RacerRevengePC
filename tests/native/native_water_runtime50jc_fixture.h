#pragma once
#include "water_resources50jc_fixture.h"
#include "../../src/game/undertrack_reset50ew.h"
#include "../../src/game/water_wake50jc.h"
#include "../../src/game/water_presentation50jc.h"
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

// One canonical native water object for the entire selected GC session. This
// owner performs construction, time/wave commits, ordered force writes and
// signed-lattice commits. It is deliberately NOT copied into a racer bank.
// Construction alone does not authorize a vehicle334 attachment or declare
// an unrepresented22A620/19CED0/root-child continuation complete.
namespace native_water_runtime50JC {
using namespace rr::game;
enum class Policy : std::uint8_t { Disabled,OneNativeView };
enum class Stage : std::uint8_t { Idle,TimePrefix,Signatures,CompanionChildren,WakePrefix,WakeBuckets,WakeChildren,Complete };
enum class Status : std::uint8_t { Complete,Duplicate,Owner,Height,Wake };
struct Result {
    Status status=Status::Owner;
    std::uint32_t sourcePC=0;
    std::size_t index=0;
    WaterHeightResult50JC height{};
    WaterWakeResult50JC wake{};
};
// Native scheduling receipt, not source game data. The real root uses one
// monotonically increasing virtual-update sequence, independent of racer and
// rendered-frame counters. Retries retain the committed source prefix.
struct Update {
    Stage stage=Stage::Idle;
    std::uint64_t sequence=0,completed=0;
    float delta=0;
    std::uint32_t application11C=0;
    std::size_t signature=0,bucket=0;
    bool hasSequence=false;
};
class Owner;
// One application-lifetime source BSS owner, shared across fresh track loads.
// Provider construction must not restart the global swell clock or lazy guard.
class Application {
    friend class Owner;
    float wakeTime443BB0_=0;
    std::uint8_t lazy8D38_=0;
    std::int32_t sentinel443BB8_=0;
    const Owner* active_=nullptr;
public:
    Application()=default;
    Application(const Application&)=delete;Application& operator=(const Application&)=delete;
    Application(Application&&)=delete;Application& operator=(Application&&)=delete;
};
class Owner {
    Application* application_=nullptr;
    std::shared_ptr<const water_resources50JC::Owner> resources_;
    Policy policy_=Policy::Disabled;
    // Original274610..640 viewport-mode0 layout, explicitly selected for the
    // separately declared one-native-view composition. Never humanCount.
    static constexpr std::int32_t dimension_=120,blocks_=15;
    static constexpr std::size_t signedBytes_=120u*15u*64u,textureBytes_=128u*128u*4u;
    float cell_=8.f,inverseCell_=.125f,halfDimension_=60.f;
    float origin_[4]{0,0,0,1},anchor_[4]{0,0,0,1};
    std::int32_t active_=0;
    std::array<std::vector<std::uint8_t>,2> buffers_;
    std::vector<std::uint8_t> texture_,nextSigned_,nextTexture_;
    WaterBucket50JC queryBucket_{};
    WaterWakeBucket50JC wakeBucket_{};
    WaterHeightProvider50JC provider_{};
    WaterWakeProvider50JC wakeProvider_{};
    WaterHeightGlobals50JC globals_{};
    WaterWakeGlobals50JC wakeGlobals_{};
    WaterWakeForceGlobals50JC forceGlobals_{};
    WaterSignatureOwner50JC signatures_{};
    std::array<WaterSignature50JC,rr::assets::maxWaterSignatures50JC> queries_{};
    std::array<rr::assets::WaterSignatureCold50JC,rr::assets::maxWaterSignatures50JC> signatureState_{};
    std::array<WaterPresentationSignature50JC,rr::assets::maxWaterSignatures50JC> presentationSignatures_{};
    std::size_t signatureCount_=0;
    float baseHeight_=0,heightScale_=0,wakeScale_=0,heightRange_=0,poolBase_=0,poolDepth_=0;
    float swellHeight_=0,swellPeriod_=0,signatureTime_=0;
    float channelScale_[4]{},fps_[5]{},phase_[5]{},complement_[5]{1,1,1,1,0};
    // Channel4's phase/complement have no source cold writer and are never
    // exposed by this channels0..3 admission. The first time update writes all5.
    std::int32_t renderInfinite_=0;
    std::uint32_t application11C_=0,wakeEnabled_=1,renderFoam_=1;
    float direction_[4]{1,0,1,1},rebase_[4]{0,0,0,1};
    float forceScale_=0;
    std::int16_t forceShift_=0;
    std::array<std::array<std::int16_t,8>,11> coefficients_{};
    WaterWakeCoefficients50JC coefficientsView_{};
    std::array<WaterWakeForceWrite50JC,4> forceWrites_{};
    WaterTimeProposal50JC timeProposal_{};
    Update update_{};
    WaterPresentationOwner50JC presentation_{};
    bool initialized_=false;

    static Result bad(std::uint32_t pc,std::size_t index=0) noexcept {return {Status::Owner,pc,index};}
    static Result heightError(const WaterHeightResult50JC& r) noexcept {
        Result out{Status::Height,r.sourcePC,r.index};out.height=r;return out;
    }
    static Result wakeError(const WaterWakeResult50JC& r) noexcept {
        Result out{Status::Wake,r.sourcePC,r.bucket};out.wake=r;return out;
    }
    bool idle() const noexcept {return update_.stage==Stage::Idle||update_.stage==Stage::Complete;}
    void refreshBucket() noexcept {
        queryBucket_.inverseScale04=inverseCell_;queryBucket_.originX10=origin_[0];
        queryBucket_.originZ18=origin_[2];queryBucket_.activeBuffer30=active_;
    }
    WaterTimeInput50JC timeInput() const noexcept {
        WaterTimeInput50JC in{};in.application11C=&application11C_;in.time388=&signatureTime_;
        for(unsigned c=0;c<5;++c)in.framesPerSecond84[c]=fps_+c;
        in.delta=update_.delta;in.sourceVirtual38DeltaBound=true;return in;
    }
public:
    Owner()=default;
    ~Owner(){if(application_&&application_->active_==this)application_->active_=nullptr;}
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    // Application must outlive this Owner. Cold-only allocation at this
    // owner's final address. Resources must be
    // the same authenticated GC WTT/PST/TGA owner retained by ColdScene.
    // The force/kernel constants below are original ELF initializers, not
    // observed values from a selected GC snapshot or caller-tuned substitutes.
    void initialize(Application& application,std::shared_ptr<const water_resources50JC::Owner> resources,
        Policy policy) {
        if(initialized_||application.active_||policy!=Policy::OneNativeView||!resources||!resources->coldResourcesBound||
           !resources->authored.bound||resources->authored.general.poolCount!=0||
           resources->signatures.size()!=static_cast<std::size_t>(resources->authored.general.signatureCount)||
           resources->signatures.size()>queries_.size())
            throw std::runtime_error("native water requires one authenticated cold GC resource owner and explicit one-view policy");
        if(std::fegetround()!=FE_TONEAREST)
            throw std::runtime_error("native water cold arithmetic rounding differs");
        water_height_detail50jc::Math math;
        const auto& a=resources->authored.general;
        baseHeight_=math.read(a.baseHeight,0x2b9370u);poolBase_=math.read(a.baseHeight,0x2b93a0u);
        wakeScale_=math.read(a.wakeScale,0x2b950cu);
        heightRange_=math.mul(a.heightRange,16.f,0x2b9898u);
        poolDepth_=math.read(a.poolDepth,0x2b9618u);
        swellHeight_=math.read(a.swellHeight,0x2b9634u);swellPeriod_=math.read(a.swellPeriod,0x2b9654u);
        //2B0EBC: real source383420 starts at1.0 in this fresh application
        // composition, so its product with original39800000 is exactly1/4096.
        heightScale_=math.mul(std::bit_cast<float>(0x39800000u),1.f,0x2b0ebcu);
        // Original383428=bits44FA0000 and LH(gp-32732) at3D5994=3.
        forceScale_=math.read(std::bit_cast<float>(0x44fa0000u),0x2b150cu);forceShift_=3;
        renderInfinite_=a.renderInfinitePlane;
        for(unsigned c=0;c<4;++c)channelScale_[c]=math.read(a.channelScale[c],0x2b93c0u+32u*c);
        for(unsigned c=0;c<5;++c)fps_[c]=math.read(a.framesPerSecond[c],0x2b9440u+32u*c);
        if(math.badPC)throw std::runtime_error("native water cold original scalar arithmetic rejected");
        resources_=std::move(resources);policy_=policy;signatureCount_=resources_->signatures.size();
        //2B0FA4/FC0 clear the actual two full signed images.2B0FF0..1010
        // writes FF7F7F7F words to the actual128x128 texture view.
        for(auto& b:buffers_)b.assign(signedBytes_,0);
        texture_.resize(textureBytes_);nextSigned_.resize(signedBytes_);nextTexture_.resize(textureBytes_);
        for(std::size_t p=0;p<texture_.size();p+=4u){texture_[p]=texture_[p+1u]=texture_[p+2u]=0x7fu;texture_[p+3u]=0xffu;}
        for(unsigned i=0;i<2;++i){const WaterBytes50JC bytes{buffers_[i].data(),buffers_[i].size(),true};
            queryBucket_.buffers38[i]=bytes;wakeBucket_.signedBuffers38[i]=bytes;}
        queryBucket_.bound=true;refreshBucket();
        wakeBucket_.cellSize00=&cell_;wakeBucket_.inverseCellSize04=&inverseCell_;
        wakeBucket_.origin10=origin_;wakeBucket_.anchor20=anchor_;wakeBucket_.activeBuffer30=&active_;
        wakeBucket_.texture34={texture_.data(),texture_.size(),true};wakeBucket_.sourceBound=true;
        wakeProvider_.buckets40=&wakeBucket_;wakeProvider_.count4C=1;
        wakeProvider_.dimensionB0=dimension_;wakeProvider_.blocksPerRowB4=blocks_;
        wakeProvider_.halfDimensionB8=&halfDimension_;wakeProvider_.first44=wakeProvider_.last48=0;
        wakeProvider_.selectedRangeBound=true;wakeProvider_.sourceBound=true;
        for(std::size_t i=0;i<signatureCount_;++i){
            const auto& source=resources_->signatures[i];
            if(!source.cold.bound||!source.cold.query.bound||source.cold.query.channelD8>3u||
               source.frames.size()!=source.cold.frameCount24)
                throw std::runtime_error("native water selected signature cold owner differs");
            signatureState_[i]=source.cold;queries_[i]=source.cold.query;
            // The complete constructor owns the actual2BE14C/150 zeros,
            // unlike an unjoined cold resource projection. Do not manufacture
            // an initial rendered frame or publish authored positions early.
            if(queries_[i].originPublication!=WaterOriginPublication50JC::Unbound||
               std::bit_cast<std::uint32_t>(queries_[i].originXC0)||
               std::bit_cast<std::uint32_t>(queries_[i].originZC4)||queries_[i].rowStride08)
                throw std::runtime_error("native water signature is not the genuine fresh cold state");
            queries_[i].originPublication=WaterOriginPublication50JC::OriginalCold2BE110;
            presentationSignatures_[i]={queries_.data()+i,signatureState_[i].authoredPosition40,
                &signatureState_[i].scrollB0,&signatureState_[i].scrollB4,true};
        }
        signatures_.signatures328=queries_.data();signatures_.count=signatureCount_;
        signatures_.completeListBound=true;signatures_.time388=&signatureTime_;
        signatures_.phase34C=phase_;signatures_.phase350=phase_+1;
        signatures_.complement360=complement_;signatures_.complement364=complement_+1;
        signatures_.sourceTimeStateBound=true;
        provider_.baseHeight3C=&baseHeight_;provider_.buckets40=&queryBucket_;provider_.count4C=1;
        provider_.dimensionB0=dimension_;provider_.blocksPerRowB4=blocks_;provider_.bucketsBound=true;
        provider_.heightScaleC0=&heightScale_;provider_.signaturesC4=&signatures_;
        const auto noise=rr::assets::prepareWaterNoiseCold50JC(resources_->noise,a.worldWidthHeight,provider_);
        if(noise.status!=WaterHeightStatus50JC::Complete)
            throw std::runtime_error("native water original noise field construction rejected");
        globals_={&renderInfinite_,&wakeScale_,&heightRange_,&poolBase_,&poolDepth_,
            channelScale_,channelScale_+1,channelScale_+2,channelScale_+3};
        wakeGlobals_={&application11C_,&wakeEnabled_,&application.lazy8D38_,&application.sentinel443BB8_,&application.wakeTime443BB0_,
            &swellPeriod_,&swellHeight_,&poolBase_,direction_,rebase_};
        forceGlobals_={&forceScale_,&forceShift_};
        constexpr std::int16_t original[11]{-127,127,-16000,16000,-5,5,-80,80,-1,75,-301};
        for(unsigned i=0;i<11;++i)coefficients_[i].fill(original[i]);
        coefficientsView_={coefficients_[0].data(),coefficients_[1].data(),coefficients_[2].data(),
            coefficients_[3].data(),coefficients_[4].data(),coefficients_[5].data(),coefficients_[6].data(),
            coefficients_[7].data(),coefficients_[8].data(),coefficients_[9].data(),coefficients_[10].data(),true};
        presentation_.policy=WaterPresentationPolicy50JC::OneNativeViewEveryRenderedFrame;
        application_=&application;application.active_=this;
        provider_.sourceStateBound=true;initialized_=true;
    }
    // Actual retained dynamic storage; inline state belongs to sizeof(Owner).
    std::size_t additionalPayloadBytes()const noexcept{
        return buffers_[0].capacity()+buffers_[1].capacity()+texture_.capacity()+
            nextSigned_.capacity()+nextTexture_.capacity();
    }
    bool initialized() const noexcept{return initialized_;}
    // Water surface draw (native_water_surface50ws.h): the same retained cold
    // resources, and the froth-layer frame phase h = provider+0x35C
    // (FramesPerSecond4, phase_[4]) the VU program reads from Water+0x210.
    const std::shared_ptr<const water_resources50JC::Owner>& resources() const noexcept{return resources_;}
    float frothPhase35C() const noexcept{return phase_[4];}
    const Update& update() const noexcept{return update_;}
    const WaterPresentationOwner50JC& presentation() const noexcept{return presentation_;}
    // These const views remain stable for this owner's entire lifetime. They
    // are exposed only at a completed update boundary; caller still owns the
    // actual source334 attachment,348 binding and ordered racer/root phases.
    const WaterHeightProvider50JC* provider() const noexcept{return initialized_&&idle()?&provider_:nullptr;}
    const WaterHeightGlobals50JC* globals() const noexcept{return initialized_&&idle()?&globals_:nullptr;}
    // Publish the actual completed runtime owner into a retained source domain.
    // Presence remains source-authored; callers retain this owner and the lift.
    bool bindRuntimeHeightDomain(const UndertrackPhysicsDomain50EW& source,
        const float& actual348Lift1F0,UndertrackPhysicsDomain50EW& output) const noexcept {
        if(source.terrain330!=UndertrackHeightOwner50EW::Absent||
           source.water334!=UndertrackHeightOwner50EW::Present||!provider()||!globals())return false;
        auto next=source;next.waterProvider334=provider();next.waterGlobals=globals();
        next.terrain348Lift1F0=&actual348Lift1F0;output=next;return true;
    }
    WaterHeightResult50JC query(float (&position)[3],std::uint32_t* effect=nullptr) const noexcept {
        if(!initialized_||!idle())return {WaterHeightStatus50JC::Owner,0x2b10f0u,0};
        return queryWaterHeight50JC(provider_,globals_,position,effect);
    }
    Result anchor(const WaterPresentationView50JC& view) noexcept {
        if(!initialized_||!idle())return bad(0x2b1690u);
        WaterWakeAnchorProposal50JC next{};
        const auto r=projectPresentationWaterAnchor50JC(wakeProvider_,wakeGlobals_,view,next);
        if(r.status!=WaterWakeStatus50JC::Complete)return wakeError(r);
        if(next.writesAnchor)std::memcpy(anchor_,next.anchor20,sizeof anchor_);
        return {Status::Complete,0x2b1744u};
    }
    Result sourceAnchor(std::int32_t index,const float (&point)[3]) noexcept {
        if(!initialized_||!idle())return bad(0x2b1690u);
        WaterWakeAnchorProposal50JC next{};
        const auto r=projectWaterWakeAnchor50JC(wakeProvider_,wakeGlobals_,index,point,next);
        if(r.status!=WaterWakeStatus50JC::Complete)return wakeError(r);
        if(next.writesAnchor)std::memcpy(anchor_,next.anchor20,sizeof anchor_);
        return {Status::Complete,0x2b1744u};
    }
    // Consume ONE actual source call immediately. Every SH, including a zero
    // delta, commits before the next call. No stale per-frame aggregation.
    Result force(const WaterWakeForceInput50JC& input) noexcept {
        if(!initialized_||!idle())return bad(0x2b1370u);
        WaterWakeForceProposal50JC plan{};
        const auto r=projectWaterWakeForce50JC(wakeProvider_,forceGlobals_,input,
            {forceWrites_.data(),forceWrites_.size()},plan);
        if(r.status!=WaterWakeStatus50JC::ForceProjected)return wakeError(r);
        for(std::size_t i=0;i<plan.writeCount;++i){const auto& write=forceWrites_[i];
            if(write.bucket||write.activeBuffer!=active_||write.byteOffset+2u>buffers_[active_].size())
                return bad(write.sourcePC,i);
            const auto* p=buffers_[active_].data()+write.byteOffset;
            const auto before=std::uint16_t(std::uint16_t(p[0])|(std::uint16_t(p[1])<<8u));
            if(before!=write.before)return bad(write.sourcePC,i);
        }
        for(std::size_t i=0;i<plan.writeCount;++i){const auto& write=forceWrites_[i];
            auto* p=buffers_[active_].data()+write.byteOffset;p[0]=std::uint8_t(write.after);p[1]=std::uint8_t(write.after>>8u);}
        return {Status::Complete,0x2b1668u};
    }
    // Begin/resume actual root water update after ALL actual racer children.
    // Time and signature prefixes commit in source order, then the original
    // companion141ED0 continuation is the source-proven fresh empty list.
    // No externally supplied 'children complete' boolean bypasses a child.
    Result advance(std::uint64_t sequence,float delta,std::uint32_t actualApplication11C) noexcept {
        if(!initialized_)return bad(0x2b6f00u);
        if(update_.hasSequence&&sequence<update_.sequence)return bad(0x2b6f00u);
        if(update_.hasSequence&&sequence==update_.sequence){
            if(std::bit_cast<std::uint32_t>(delta)!=std::bit_cast<std::uint32_t>(update_.delta)||
               actualApplication11C!=update_.application11C)return bad(0x2b6f00u);
            if(update_.stage==Stage::Complete)return {Status::Duplicate,0};
        }else{
            if(!idle()||update_.completed==std::numeric_limits<std::uint64_t>::max())return bad(0x2b6f00u);
            update_.hasSequence=true;update_.sequence=sequence;update_.delta=delta;
            update_.application11C=actualApplication11C;application11C_=actualApplication11C;
            update_.signature=update_.bucket=0;update_.stage=Stage::TimePrefix;
            // Already-borrowed terrain/reset descriptors must also reject a
            // partially committed root update, not merely new view requests.
            provider_.sourceStateBound=false;
        }
        for(;;)switch(update_.stage){
        case Stage::TimePrefix: {
            const auto tick=timeInput();const auto r=projectWaterTime50JC(tick,timeProposal_);
            if(r.status!=WaterHeightStatus50JC::Complete)return heightError(r);
            if(timeProposal_.paused){update_.stage=Stage::WakePrefix;break;}
            signatureTime_=timeProposal_.time388;
            for(unsigned c=0;c<5;++c){phase_[c]=timeProposal_.phase34C[c];complement_[c]=timeProposal_.complement360[c];}
            update_.stage=Stage::Signatures;break;
        }
        case Stage::Signatures:
            while(update_.signature<signatureCount_){const auto i=update_.signature;
                const auto& frames=resources_->signatures[i].frames;auto& live=signatureState_[i];
                WaterSignatureTimeInput50JC input{queries_.data()+i,frames.data(),live.frameCount24,
                    live.scrollB0,live.scrollB4,live.speedB8,live.speedBC,&renderFoam_,true,true};
                WaterSignatureTimeProposal50JC next{};
                const auto r=projectWaterSignatureTime50JC(input,timeInput(),timeProposal_,next);
                if(r.status!=WaterHeightStatus50JC::Complete)return heightError(r);
                if(next.advanced){queries_[i]=next.query;live.frame28=next.frame28;
                    live.scrollB0=next.scrollB0;live.scrollB4=next.scrollB4;}
                ++update_.signature;
            }
            update_.stage=Stage::CompanionChildren;break;
        case Stage::CompanionChildren:
            // Fresh GC source proof:2B1E60->2BA8C0->141300 clears child18
            // and descendantFlags24; complete2B2440 setup never appends a
            // child.1B4904 attaches THIS companion to the root, without
            // altering its own empty child list.141ED0 therefore returns.
            // This class exposes no child-attachment mutator or fake receipt.
            update_.stage=Stage::WakePrefix;break;
        case Stage::WakePrefix: {
            WaterWakeUpdateProposal50JC next{};
            const auto r=projectWaterWakeUpdate50JC(wakeProvider_,wakeGlobals_,update_.delta,true,next);
            if(r.status==WaterWakeStatus50JC::Paused){update_.stage=Stage::Complete;break;}
            if(r.status==WaterWakeStatus50JC::NeedsBaseChildren141ED0){update_.stage=Stage::WakeChildren;break;}
            if(r.status!=WaterWakeStatus50JC::NeedsLatticeKernel)return wakeError(r);
            if(next.writesLazySentinel){application_->lazy8D38_=next.lazy8D38;application_->sentinel443BB8_=next.sentinel443BB8;}
            if(next.writesSwell){application_->wakeTime443BB0_=next.time443BB0;baseHeight_=next.baseHeight3C;}
            update_.bucket=next.first44;update_.stage=Stage::WakeBuckets;break;
        }
        case Stage::WakeBuckets: {
            if(update_.bucket>wakeProvider_.last48){update_.stage=Stage::Complete;break;}
            WaterWakeBucketProposal50JC next{};
            const auto r=computeWaterWakeLattice50JC(wakeProvider_,wakeGlobals_,coefficientsView_,update_.bucket,
                {nextSigned_.data(),nextSigned_.size(),nextTexture_.data(),nextTexture_.size()},next);
            if(r.status!=WaterWakeStatus50JC::LatticeComputed)return wakeError(r);
            //2B18F0 rebase belongs before kernel writes. Source tail2B1D50
            // then publishes the completed texture/buffer, origin and selector.
            if(next.rebasesOriginBeforeKernel)std::memcpy(origin_,next.originBeforeKernel10,sizeof origin_);
            std::memcpy(buffers_[next.destinationBuffer].data(),nextSigned_.data(),signedBytes_);
            std::memcpy(texture_.data(),nextTexture_.data(),textureBytes_);
            std::memcpy(origin_,next.originAfterKernel10,sizeof origin_);active_=next.destinationBuffer;
            refreshBucket();++update_.bucket;break;
        }
        case Stage::WakeChildren:
            //2B0800->27E130->141300 gives the same genuinely empty fresh
            // provider child list; setup does not append. Normally unreachable
            // here because this bounded fresh application owns383418=1.
            update_.stage=Stage::Complete;break;
        case Stage::Complete:provider_.sourceStateBound=true;++update_.completed;return {Status::Complete,0x2b1e24u};
        default:return bad(0x2b6f00u);
        }
    }
    // Presentation is separate from root time. Origins keep the last actual
    // completed render until this native one-view policy publishes another.
    WaterPresentationResult50JC present(std::uint64_t completedFrame,bool actualNativeRenderCompleted) noexcept {
        if(!initialized_||!idle())return {WaterPresentationStatus50JC::Rejected,
            {WaterHeightStatus50JC::Owner,0x2b4cc8u,0}};
        WaterPresentationInput50JC input{};input.renderedFrame=completedFrame;input.viewCount=1;
        input.actualNativeRenderCompleted=actualNativeRenderCompleted;input.renderFoam3834F0=&renderFoam_;
        input.signatures=presentationSignatures_.data();input.count=signatureCount_;input.completeSignatureSetBound=true;
        return presentNativeWaterView50JC(presentation_,input);
    }
};
} // namespace native_water_runtime50JC
