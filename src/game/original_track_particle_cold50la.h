#pragma once
#include "original_application_entities50la.h"
#include "native_directional_normalize106ea0_proposal.h"
#include "original_gc_particle_manager50kg.h"
#include "../assets/original_standard_particle_material_reference50la.h"
#include <exception>
#include <cstdio>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <optional>
#include <memory>
#include <new>
#include <utility>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__)
#error "Track particle bounded numeric adaptation requires strict floating point"
#endif
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_source_camera50LA {class OwnershipBridge;}
namespace rr::game {
static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559 &&
    std::numeric_limits<float>::radix==2 && std::numeric_limits<float>::digits==24,
    "Track particle bounded numeric adaptation requires IEEE binary32");
enum class OriginalTrackParticleColdStatus50LA : std::uint8_t {
    complete,unprepared,alreadyPrepared,input,component,particle,material,allocation
};
struct OriginalTrackParticleColdDiagnostic50LA {
    OriginalTrackParticleColdStatus50LA status=OriginalTrackParticleColdStatus50LA::unprepared;
    std::uint32_t sourcePC=0;
    OriginalWreckComponentResult50LA component{};
    OriginalGcParticleStatus50KG particle=OriginalGcParticleStatus50KG::unprepared;
    assets::OriginalStandardMaterialDiagnostic50LA material{};
    bool entityConstructed=false,depthGlobalWritten=false,materialRetained=false;
    bool okay()const noexcept{return status==OriginalTrackParticleColdStatus50LA::complete;}
};

// One real native scene particle owner: SAME5000-slot numeric provider,
// actual160B10->141300 Entity subobject and canonical2461E0 reference150.
// The application counter/depth global and material table are shared, pinned
// owners. This bounded cold path does not construct scene/manager ancestry,
// attach children, populate14D370, draw, demand textures, or certify the full
//160E00 auxiliary, renderer-scratch, bounding/transform or packet fields.
// The private RA World continuation below adds missing cold resources around
// this same owner, without widening public prepared() or enabling dispatch.
class OriginalTrackParticleCold50LA final {
    // Read-only provenance for the actual Application's native TA camera
    // issuer. Public component fields alone cannot establish generation.
    friend struct ::original_main_entry50KG::Application;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    std::size_t sourceConfigInvocationBorrowers_=0;
    std::size_t sourceParticleEmitterBorrowers_=0; //Actual27C7B0 emitter70 raw borrowers; no guest retain.
    std::size_t sourceSmokerManagerBorrowers_=0; //Native source70 borrowers; no guest retain/registration.
public:
    OriginalTrackParticleCold50LA() noexcept=default;
    OriginalTrackParticleCold50LA(const OriginalTrackParticleCold50LA&)=delete;
    OriginalTrackParticleCold50LA& operator=(const OriginalTrackParticleCold50LA&)=delete;
    ~OriginalTrackParticleCold50LA(){if(!close())std::terminate();}

    // The caller authenticates the PRT member and lends the actual36B970
    // temporarily selected from application's36B530. No file lookup, second
    // material table, GC fog/fire population or filename-based path guess.
    // Attempted source effects remain owned on failure until ordered close.
    bool prepareNativeTrack(const std::string& actualDirectory36B970,
        const std::string& actualScbParticleTextureMapName,
        const std::vector<std::uint8_t>& authenticatedPrt,
        std::shared_ptr<OriginalApplicationEntities50LA> application,
        std::shared_ptr<assets::OriginalStandardMaterialTable50LA> table,
        std::uint64_t generation,OriginalTrackParticleColdDiagnostic50LA& d) noexcept {
        using S=OriginalTrackParticleColdStatus50LA;
        d={};
        if(attempted_||closed_)return fail(d,S::alreadyPrepared,0x160b10u);
        if(!generation||!application||!application->application10()||
           !application->application26C()||!table||!table->prepared()||
           table->populationTerminal()||table->demandTerminal()||authenticatedPrt.empty()||
           !assets::OriginalStandardParticleMaterialReference50LA::acceptsTrackInput160F90(
               actualDirectory36B970,actualScbParticleTextureMapName))
            return fail(d,S::input,0x160b10u);
        try {
            application_=std::move(application);table_=std::move(table);
            particles_=std::make_unique<OriginalGcParticleManager50KG>();
            generation_=generation;attempted_=true;
            if(!construct160B10(d))return false;
            // Same constructor, followed ONLY HERE by the existing native
            // authenticated pool/PRT/material adaptation. Source callers stop
            // at constructor return and cannot inherit these prewarm effects.
            // Source130 receives the caller's5000;118 is signed division
            // toward zero.160E78 writes that value into the ONE application
            // global. Do not roll it back when later atlas/material work fails.
            capacity130_=static_cast<std::int32_t>(originalGcParticleCapacity50KG);
            depthBucketCount118_=capacity130_/2;
            std::int32_t previousDepth=0;
            if(!application_->writeParticleDepth160E78(depthBucketCount118_,previousDepth))
                return fail(d,S::input,0x160e78);
            depthGlobalBefore_=previousDepth;depthGlobalAfter_=depthBucketCount118_;
            depthWriteOrigin_=DepthWriteOrigin::nativePrewarm;
            depthGlobalWritten_=true;d.depthGlobalWritten=true;
            d.particle=particles_->prepareTrackAtlas160E00(authenticatedPrt);
            if(d.particle!=OriginalGcParticleStatus50KG::complete)
                return fail(d,S::particle,0x1617a0u);
            material150_=std::make_unique<assets::OriginalStandardParticleMaterialReference50LA>();
            if(!material150_->acquireTrack299BE0(table_,actualDirectory36B970,
                actualScbParticleTextureMapName,d.material))
                return fail(d,S::material,d.material.sourcePC);
            d.materialRetained=true;prepared_=true;
            // Native ready means these THREE actual owners only. It is not
            // a receipt that the remainder of source160E00 returned.
            d.status=S::complete;d.sourcePC=0x161008u;return true;
        }catch(const std::bad_alloc&){return fail(d,S::allocation,0);}
        catch(...){return fail(d,S::input,0);}
    }
    bool prepared() const noexcept {
        return prepared_&&!closed_&&entityConstructed_&&particles_&&particles_->prepared()&&
            material150_&&material150_->retained()&&material150_->table382B38()==table_.get();
    }
    OriginalGcParticleManager50KG* particles() noexcept{return prepared()?particles_.get():nullptr;}
    const OriginalGcParticleManager50KG* particles() const noexcept{return prepared()?particles_.get():nullptr;}
    OriginalWreckComponent50LA* component() noexcept{return entityConstructed_&&!closed_?&component_:nullptr;}
    const OriginalWreckComponent50LA* component() const noexcept{return entityConstructed_&&!closed_?&component_:nullptr;}
    const assets::OriginalStandardParticleMaterialReference50LA* material150() const noexcept{return material150_.get();}
    const std::int32_t& capacity130() const noexcept{return capacity130_;}
    const std::int32_t& depthBucketCount118() const noexcept{return depthBucketCount118_;}
    bool depthGlobalWritten() const noexcept{return depthGlobalWritten_;}
    bool closed() const noexcept{return closed_;}

    // Fresh detached owning-reference cleanup only. Real borrowers must be closed
    // first. Refuse foreign references/links or an attachment lock before any
    // release.1B4E88..98 dispatches slot10; actual3CDE40 is142590. Its
    // childless/unlinked branch reaches142654->104970, whose zero reference
    // dispatches slot08=160C70. No guessed tree or141A70 unlink/reset.
    // Release the numeric pool before246260->299AC0->14D500; then the reached
    //1413E0 decrements the SAME423F60 exactly once while both pins are live.
    bool close() noexcept {
        if(closed_)return true;
        if(nativeWreckBorrower_||sourceConfigInvocationBorrowers_||sourceSmokerManagerBorrowers_||sourceParticleEmitterBorrowers_)return false;
        if(entityConstructed_){
            if(!application_||application_->attachmentLock423F58()||
               component_.kind!=OriginalWreckComponentKind50LA::particleManager160B10||
               component_.reference00!=1u||component_.sessionGeneration!=generation_||
               component_.objectKey!=key(&component_)||component_.parent1C||component_.previous10||
               component_.next14||component_.firstChild18||component_.mask20||component_.descendants24)
                return false;
        }
        if(!canRetireNativeResources160E00())return false;
        if(sourceEntityEnrolled_&&(!application_||!application_->entityAuthority_.canRetire(component_)))return false;
        if(sourceEntityEnrolled_){
            if(!application_->entityAuthority_.retire(component_))return false;
            sourceEntityEnrolled_=false;
        }
        prepared_=false;resourcesComplete_=false;
        // Actual142654->104970 consumes this owner's single reference
        // before the concrete160C70 destructor, not inside1413E0 itself.
        if(entityConstructed_)--component_.reference00;
        // Withdraw the raw auxiliary borrow before the numeric domain dies;
        // retain its Reference metadata until source160D9C after buckets.
        if(auxiliary1C0_)auxiliary1C0_->numericChild=nullptr;
        particles_.reset();
        //160D10..1C releases the SAME actual source110 frame allocation first.
        sourceFrameStorage110_.reset();sourceFrameRows_.reset();
        //160D24..30 releases the SAME actual source13C allocation before150.
        //No source NULL store is invented; closed owner retains historical lane.
        sourceGroupStorage13C_.reset();sourceGroupRows_.reset();
        material150_.reset();
        scratch15C_.reset();scratchBytes15C_=0; //160D68/70
        std::vector<DepthBucket160E00>().swap(buckets11C_); //160D84 releases backing now
        if(auxiliary1C0_){
            --auxiliary1C0_->reference00; //104970->163060, not Entity1413E0
            auxiliary1C0_->primitiveE0.releaseCold133FA0();
            auxiliary1C0_->primitiveB0.releaseCold133FA0();
            auxiliary1C0_.reset();
        }
        primitive190_.releaseCold133FA0();primitive160_.releaseCold133FA0();
        if(entityConstructed_){
            --application_->objectCount423F60();
            //160DBC..DC4->1413E0->104910 performs the actual global store
            // after storage/material destruction. No second reference release.
        }
        closed_=true;return true;
    }
private:
    // Private actual World1B4E4C constructor branch. Native prewarm cannot
    // call this method; the ordered source caller alone owns it. It cannot load
    // a PRT, publish World4A0, or claim160E00 has returned.
    //Same concrete owner validates its authority membership; no new broad
    //friendship to Application's private entity ledger is required.
    bool sourceEntityCountedNative(const OriginalApplicationEntities50LA* expected,std::uint64_t generation,
        std::size_t& count)const noexcept {
        count=0;
        if(closed_||application_.get()!=expected||!expected||generation_!=generation||!attempted_||prepared_||
           nativeWreckBorrower_||!canRetireNativeResources160E00())return false;
        const auto& e=component_;const auto& authority=application_->entityAuthority_;
        if(entityConstructed_){
            if(e.reference00!=1||e.kind!=OriginalWreckComponentKind50LA::particleManager160B10||
               e.sessionGeneration!=generation||e.objectKey!=key(&e)||
               e.category28!=0xff||(e.flags29&15u)!=3u||e.nativeFlags29||
               e.parent1C||e.previous10||e.next14||e.firstChild18||e.mask20||e.descendants24)return false;
            count=1;
        }else if(e.reference00)return false;
        if(sourceEntityEnrolled_)return entityConstructed_&&authority.canRetire(e);
        for(std::size_t i=0;i<authority.count_;++i)if(authority.entries_[i].node==&e)return false;
        return true;
    }
    bool constructSource160B10(std::shared_ptr<OriginalApplicationEntities50LA> application,
        std::uint64_t generation,OriginalTrackParticleColdDiagnostic50LA& d)noexcept {
        using S=OriginalTrackParticleColdStatus50LA;d={};
        if(attempted_||closed_)return fail(d,S::alreadyPrepared,0x160b10);
        if(!application||!generation||!application->application10()||!application->application26C()||
           application->attachmentLock423F58())return fail(d,S::input,0x160b10);
        auto& authority=application->entityAuthority_;
        if(!authority.reserveAdditional(1)||!authority.canEnroll(component_,key(&component_)))
            return fail(d,S::allocation,0x160b10);
        application_=std::move(application);generation_=generation;attempted_=true;
        if(!construct160B10(d))return false;
        if(!authority.enroll(component_))return fail(d,S::component,0x160c68);
        sourceEntityEnrolled_=true;d.status=S::complete;d.sourcePC=0x160c68;return true;
    }
    // Conditional original1B4E6C virtual+74 entry for its fixed5000 argument.
    // Actual source World/environment/order owns this private invocation.
    // No filename, atlas/pool or native prewarm is adopted.
    bool beginSourceLoad160E00(std::uint32_t& pc)noexcept {
        pc=sourceLoadPC_;
        if(!constructorReturned_||!sourceEntityEnrolled_||sourceLoadAttempted_||closed_||
           !application_||application_->attachmentLock423F58()||
           !application_->entityAuthority_.canRetire(component_)||particles_||material150_||table_||
           auxiliary1C0_||!buckets11C_.empty()||scratch15C_||std::fegetround()!=FE_TONEAREST)return false;
        sourceLoadAttempted_=true;pc=sourceLoadPC_=0x160e28;
        capacity130_=5000; //160E2C call delay store precedes nullable allocation.
        auxiliary1C0_.reset(new(std::nothrow) Auxiliary162E70);
        if(auxiliary1C0_){auto& child=*auxiliary1C0_;
            //Same typed auxiliary owner; no second numeric auxiliary is made.
            child.sourceDispatch08=0x3cdeb0;child.sourceEnabled18=0;
            const std::uint32_t bits=0x3c95e9e2u;std::memcpy(&child.field20,&bits,4);
            child.inverseDimensions70={{div160E00(32767,90),div160E00(32767,35),div160E00(32767,90),1}};
            //Other162E70 scalar/vector/133F80 stores are this actual owner's
            //constructor fields; untouched buffer/110..11C/80..8C/13C lanes
            //remain optional unknown, never initialized from host zeros.
        }
        sourceAuxiliaryPublished_=true; //160E40 publishes even NULL.
        depthBucketCount118_=capacity130_/2; //Positive original fixed count.
        pc=sourceLoadPC_=0x160e5c;
        try {buckets11C_.resize(static_cast<std::size_t>(depthBucketCount118_));}
        catch(...){sourceBucketAllocationFailed_=true;}
        sourceBucketsPublished_=true; //160E64 nullable native allocation adaptation.
        pc=sourceLoadPC_=0x160e78;
        std::int32_t previousDepth=0;
        if(!application_->writeParticleDepth160E78(depthBucketCount118_,previousDepth))return false;
        depthGlobalBefore_=previousDepth;depthGlobalAfter_=depthBucketCount118_;
        depthWriteOrigin_=DepthWriteOrigin::source160E78;
        depthGlobalWritten_=true; //160E78 occurs even after NULL allocation.
        pc=sourceLoadPC_=0x160e8c;
        if(sourceBucketAllocationFailed_)return false; //Before first NULL epoch store.
        for(auto& bucket:buckets11C_){
            bucket.epoch00=0;++sourceBucketWordsInitialized_;
            bucket.head04=nullptr;++sourceBucketWordsInitialized_;
        }
        //Original signed division is exact for this5000 caller; no generalized
        //capacity or native pool count is substituted for130.
        scratchBytes15C_=static_cast<std::size_t>(capacity130_+capacity130_/12+1)*64u+16u;
        pc=sourceLoadPC_=0x160ee8;
        scratch15C_.reset(static_cast<std::uint8_t*>(::operator new[](
            scratchBytes15C_,std::align_val_t(64),std::nothrow)));
        sourceScratchPublished_=true;pc=sourceLoadPC_=0x160efc;
        if(!scratch15C_)return false; //Before source memset(NULL,0,346704).
        std::memset(scratch15C_.get(),0,scratchBytes15C_);sourceScratchCleared_=true;
        pc=sourceLoadPC_=0x160f04;return false;
        //Before allocation of130 pointer slots/5000 particle records and
        //1617A0 PRT Config/file path. No160E00 return/material/atlas receipt.
    }
    bool continueSourcePool160F04(std::uint32_t& pc)noexcept {
        pc=sourceLoadPC_;
        if(sourceLoadPC_!=0x160f04||!sourceLoadAttempted_||!sourceEntityEnrolled_||closed_||
           !sourceScratchCleared_||sourcePoolAttempted_||particles_||!application_||
           !application_->entityAuthority_.canRetire(component_)||capacity130_!=5000)return false;
        //Native control object only; NOT another counted particle manager.
        particles_.reset(new(std::nothrow) OriginalGcParticleManager50KG);
        if(!particles_)return false;
        sourcePoolAttempted_=true;
        if(!particles_->allocateSourcePointers160F08(pc)){sourceLoadPC_=pc;return false;}
        sourceParticlePointers138_=particles_->sourcePointerPublication138(); //160F10 evenNULL.
        sourceParticlePointersPublished_=true;
        if(!particles_->constructSourceRecords160F1C(pc)){sourceLoadPC_=pc;return false;}
        pc=sourceLoadPC_=0x160f60;return false;
        //Before1617A0 consumes the true incoming environment filename and
        //same FileManager/Config custody. No source loader return/readiness.
    }
    bool construct160B10(OriginalTrackParticleColdDiagnostic50LA& d)noexcept {
        using S=OriginalTrackParticleColdStatus50LA;
        if(constructorReturned_||entityConstructed_||!application_||!generation_||closed_)
            return fail(d,S::input,0x160b10);
        d.component=initializeOriginalWreckComponent50LA(component_,
            OriginalWreckComponentKind50LA::particleManager160B10,generation_,
            key(&component_),true,application_->objectCount423F60());
        entityConstructed_=component_.reference00!=0;d.entityConstructed=entityConstructed_;
        if(d.component.status!=OriginalWreckComponentStatus50LA::complete)
            return fail(d,S::component,d.component.sourcePC);
        sourceDispatch08_=0x3cde30;
        position30_={{0,0,0,1}};gravity40_={{0,0,0,1}};
        //2E8860 invokes1048E0 four times at50, stride10: actual VF0.
        for(auto& normal:normals50_)normal={{0,0,0,1}};
        //The four12ACC0 calls at90, stride20, are identity returns with NO
        //stores. All32 words stay source-unknown, despite safe host backing.
        quadPositions90Known_=false;
        for(auto& record:quadRemainder90_)for(auto& word:record)word.reset();
        primitive160_=Buffer160E00{};primitive190_=Buffer160E00{};
        //133F80 writes only+20/+24/+28=0 and+2C=-1;00..1C stay unknown.
        sourceParticles128_=nullptr;field12C_=0;capacity130_=0;
        gravity40_[0]=0;gravity40_[1]=-64;gravity40_[2]=0;
        sourceAtlas110_=nullptr;sourceNumFrames154_=0;field158_=0;field134_=0;
        sourceParticlePointers138_=nullptr;sourceGroups13C_=nullptr;sourceNumGroups140_=0;
        field144_=1;field14C_=nullptr;epoch114_=0;depthBucketCount118_=0;
        //11C/150/15C/1C0 are the existing actual empty owned resources.
        //No numeric provider/material/auxiliary/scratch is allocated here.
        glowScalar148_=255;
        constructorReturned_=true;d.status=S::complete;d.sourcePC=0x160c68;return true;
    }
    struct SourceGroup1618C4 {std::array<std::optional<std::int32_t>,3> lanes;};
    struct SourceGroupFree {void operator()(void* p)const noexcept {std::free(p);}};
    std::unique_ptr<void,SourceGroupFree> sourceGroupStorage13C_;
    std::unique_ptr<SourceGroup1618C4[]> sourceGroupRows_;
    bool sourceGroupAllocationAttempted_=false,sourceGroupAllocationPublished_=false;
    std::uint32_t sourceGroupCapacity_=0,sourceGroupIndex_=0,sourceGroupLane_=0;
    bool beginSourceGroups1618C4(std::uint32_t& pc)noexcept {
        pc=0x1618B0;
        if(sourceGroupAllocationAttempted_||!sourceEntityEnrolled_||closed_||
           !sourceConfigInvocationBorrowers_)return false;
        sourceGroupAllocationAttempted_=true;
        if(sourceNumGroups140_<=0){pc=0x161968;return true;}
        const auto count=static_cast<std::uint32_t>(sourceNumGroups140_);
        //Original32-bit multiply/shift can wrap. This native storage adapter
        //bounds oversized extents BEFORE allocation; it does not alter count.
        pc=0x1618C4;if(count>0xffffffffu/12u)return false;
        try{sourceGroupRows_.reset(new SourceGroup1618C4[count]);}
        catch(...){return false;} //Sidecar admission is NOT source mallocNULL.
        sourceGroupCapacity_=count;
        sourceGroupStorage13C_.reset(std::malloc(std::size_t(count)*12u));
        sourceGroups13C_=sourceGroupStorage13C_.get(); //1618CC includingNULL.
        sourceGroupAllocationPublished_=true;sourceGroupIndex_=0;sourceGroupLane_=0;
        pc=0x1618DC;return true;
    }
    struct SourceFrame161974 {std::array<std::optional<std::uint32_t>,8> words;};
    std::unique_ptr<void,SourceGroupFree> sourceFrameStorage110_;
    std::unique_ptr<SourceFrame161974[]> sourceFrameRows_;
    bool sourceFrameAllocationAttempted_=false,sourceFrameAllocationPublished_=false;
    std::uint32_t sourceFrameCapacity_=0,sourceFrameIndex_=0,sourceFrameOffset_=0,sourceFrameWordsWritten_=0;
    bool beginSourceFrames161974(std::uint32_t& pc)noexcept {
        pc=0x161968;
        if(sourceFrameAllocationAttempted_||!sourceGroupAllocationAttempted_||!sourceEntityEnrolled_||
           closed_||!sourceConfigInvocationBorrowers_)return false;
        sourceFrameAllocationAttempted_=true;
        if(sourceNumFrames154_<=0){pc=0x161AAC;return true;}
        const auto count=static_cast<std::uint32_t>(sourceNumFrames154_);
        pc=0x161974;if(count>0xffffffffu/32u)return false; //Native wrap/extent boundary.
        try{sourceFrameRows_.reset(new SourceFrame161974[count]);}
        catch(...){return false;} //Native provenance sidecar, not sourceNULL.
        sourceFrameCapacity_=count;
        sourceFrameStorage110_.reset(std::malloc(std::size_t(count)*32u));
        sourceAtlas110_=sourceFrameStorage110_.get(); //16197C, includingNULL.
        sourceFrameAllocationPublished_=true;sourceFrameIndex_=0;sourceFrameOffset_=0;sourceFrameWordsWritten_=0;
        pc=0x16198C;return true;
    }
    //Actual missing-member branch only; caller proves Config open returned0.
    //No text/group parse or default substitutions run on this branch.
    bool constructSourceFallback161AC0(const std::string& filename,std::uint32_t& pc)noexcept {
        if(sourceFallbackAttempted_||sourceFrameAllocationAttempted_||sourceGroupAllocationAttempted_||
           !sourceEntityEnrolled_||closed_||!sourceConfigInvocationBorrowers_)return false;
        sourceFallbackAttempted_=true;pc=0x161ac8;
        try {
            //Explicit native1336C0 sink, matching other source diagnostic joins.
            //Original ignores return; retain even short/error output as evidence.
            const std::string message="Error reading (or could not find) particle descriptor file "+
                filename+"\nusing default layout\n";
            sourceFallbackLogAttempted_=true;
            sourceFallbackLogBytes_=std::fwrite(message.data(),1,message.size(),stdout);
            sourceFallbackLogError_=std::ferror(stdout)!=0;
        }catch(...){return false;}
        pc=0x161ad4;sourceNumFrames154_=61;
        pc=0x161adc;sourceFrameAllocationAttempted_=true;
        try{sourceFrameRows_.reset(new SourceFrame161974[61]);}catch(...){return false;}
        sourceFrameCapacity_=61;
        sourceFrameStorage110_.reset(std::malloc(61u*32u));
        pc=0x161ae4;sourceAtlas110_=sourceFrameStorage110_.get();
        sourceFrameAllocationPublished_=true;sourceFrameIndex_=0;sourceFrameOffset_=0;sourceFrameWordsWritten_=0;
        //Exact small integers and power-of-two fractions only. Preserve source
        //CVT.S.W/mul/div/add order using strict binary32 helpers, not a guessed
        //fully-populated atlas. Native FP admission does not change process mode.
        pc=0x161b10;if(std::fegetround()!=FE_TONEAREST)return false;
        constexpr std::uint32_t stores[4][8]={
            {0x161b40,0x161b50,0x161b5c,0x161b68,0x161b74,0x161b80,0x161b8c,0x161ba0},
            {0x161c28,0x161c38,0x161c44,0x161c50,0x161c5c,0x161c68,0x161c74,0x161c88},
            {0x161d18,0x161d28,0x161d34,0x161d40,0x161d4c,0x161d58,0x161d64,0x161d78},
            {0x161e04,0x161e14,0x161e20,0x161e2c,0x161e38,0x161e44,0x161e50,0x161e64}
        };
        constexpr unsigned starts[4]={0,14,29,45},ends[4]={14,18,45,61};
        for(unsigned region=0;region<4;++region){
            //161C8C deliberately jumps18->29: frame18..28 are never written.
            sourceFrameOffset_=starts[region]*32u;
            for(unsigned frame=starts[region];frame<ends[region];++frame){
                const auto relative=frame-starts[region];
                const unsigned divisor=region==1?2u:4u;
                const float row=static_cast<float>(relative/divisor);
                const float column=static_cast<float>(relative%divisor);
                float left,top,right=0.f,bottom; //right only becomes source-known at its arithmetic.
                if(region==0){
                    top=mul160E00(.25f,row);bottom=add160E00(.25f,top);
                    left=mul160E00(.25f,column);
                }else if(region==1){
                    const float y=div160E00(mul160E00(.25f,row),2.f);
                    top=add160E00(.75f,y);bottom=add160E00(.125f,top);
                    const float x=div160E00(mul160E00(.25f,column),2.f);
                    left=add160E00(.25f,x);
                }else{
                    const float y=div160E00(mul160E00(.25f,row),4.f);
                    top=add160E00(.75f,y);bottom=add160E00(.0625f,top);
                    const float x=div160E00(mul160E00(.25f,column),4.f);
                    left=add160E00(region==2?.5f:.75f,x);
                }
                //Each loop increments its frame index BEFORE first store.
                sourceFrameIndex_=frame+1;sourceFrameWordsWritten_=0;
                for(unsigned word=0;word<8;++word){
                    //Compute right AFTER first left store, before top, at
                    //161B48/161C30/161D20/161E0C respectively.
                    if(word==1)right=add160E00(region==0?.25f:region==1?.125f:.0625f,left);
                    const float values[8]={left,top,right,top,right,bottom,left,bottom};
                    if(word==7)sourceFrameOffset_+=32; //Before branch delay-slot store.
                    pc=stores[region][word];
                    if(!sourceAtlas110_||sourceAtlas110_!=sourceFrameStorage110_.get()||
                       !sourceFrameRows_||frame>=sourceFrameCapacity_)return false;
                    std::uint32_t bits;std::memcpy(&bits,&values[word],sizeof(bits));
                    auto* out=static_cast<std::uint8_t*>(sourceFrameStorage110_.get())+frame*32u+word*4u;
                    for(unsigned byte=0;byte<4;++byte)out[byte]=static_cast<std::uint8_t>(bits>>(8u*byte));
                    sourceFrameRows_[frame].words[word]=bits;++sourceFrameWordsWritten_;
                }
            }
        }
        sourceFallbackFramesReturned_=true;pc=0x161e68;return true;
    }
    bool storeSourceFrame161A0C(std::uint32_t index,const std::array<float,4>& uv,
        std::uint32_t& pc)noexcept {
        //Original S2 increments161A14 BEFORE the first source store. S1 offset
        //still addresses this index. Keep that partial iterator progress.
        sourceFrameIndex_=index+1;sourceFrameWordsWritten_=0;
        constexpr std::array<std::uint32_t,8> pcs{{0x161A1C,0x161A2C,0x161A3C,0x161A4C,
            0x161A5C,0x161A6C,0x161A7C,0x161A90}};
        constexpr std::array<std::size_t,8> lanes{{0,2,1,2,1,3,0,3}};
        for(std::size_t word=0;word<8;++word){
            if(word==7)sourceFrameOffset_+=32; //161A8C before final delay-slot store.
            pc=pcs[word];
            if(!sourceFrameAllocationPublished_||!sourceAtlas110_||
               sourceAtlas110_!=sourceFrameStorage110_.get()||!sourceFrameRows_||index>=sourceFrameCapacity_)return false;
            std::uint32_t bits;std::memcpy(&bits,&uv[lanes[word]],sizeof(bits));
            auto* destination=static_cast<std::uint8_t*>(sourceFrameStorage110_.get())+std::size_t(index)*32u+word*4u;
            for(std::size_t byte=0;byte<4;++byte)destination[byte]=static_cast<std::uint8_t>(bits>>(8*byte));
            sourceFrameRows_[index].words[word]=bits;++sourceFrameWordsWritten_;
        }
        pc=0x161A98;return true;
    }
    using Vec160E00=std::array<float,4>;
    //133F80 writes only20/24/28/2C. No packet allocation or renderer callback.
    struct Buffer160E00 {
        std::array<std::optional<std::uint32_t>,8> unwritten00To1C;
        void* buffer20=nullptr;
        std::uint32_t count24=0,count28=0;
        std::int32_t field2C=-1;
        bool released133FA0=false;
        bool cold()const noexcept{return !buffer20&&!count24&&!count28&&field2C==-1&&!released133FA0;}
        void releaseCold133FA0()noexcept{released133FA0=true;}
    };
    struct Auxiliary162E70 {
        std::uint64_t reference00=1;
        std::uint32_t sourceDispatch08=0;
        std::optional<std::uint32_t> sourceEnabled18;
        std::array<std::optional<std::uint32_t>,4> unwritten80To8C;
        std::optional<std::uint32_t> unwritten13C;
        // In the legacy native path, enabled18 remains in the actual numeric owner.
        // sourceEnabled18 is engaged ONLY by genuine conditional162E70 construction.
        const OriginalGcParticleAuxiliaryChild50KG* numericChild=nullptr;
        std::uint32_t field10=0,field14=0,field1C=0,field24=0;
        float field20=0,minimum28=0,maximum2C=190;
        Vec160E00 position30{{0,-20,0,1}},color40{{128,128,128,128}};
        Vec160E00 position50{{0,0,0,1}},scale60{{1,1,0,1}};
        Vec160E00 inverseDimensions70{},field90{{-100000,-100000,-100000,1}};
        std::uint32_t fieldA0=0,fieldA4=0,fieldAC=0;
        Buffer160E00 primitiveB0,primitiveE0;
        std::array<std::optional<std::uint32_t>,4> unwritten110To11C;
        std::uint32_t field120=0,field124=0,field128=0,field12C=0,field134=0,field138=0;
        bool coldResources()const noexcept{return reference00==1&&!field10&&!fieldAC&&!field120&&
            !field124&&!field128&&!fieldA0&&primitiveB0.cold()&&primitiveE0.cold();}
    };
    // Two source words, widened pointer lane in the native representation.
    struct DepthBucket160E00 {std::uint32_t epoch00=0;const OriginalGcParticle50KG* head04=nullptr;};
    struct ScratchDelete160E00 {
        void operator()(std::uint8_t* p)const noexcept{::operator delete[](p,std::align_val_t(64));}
    };
    static float add160E00(float a,float b)noexcept{volatile float r=a+b;return r;}
    static float mul160E00(float a,float b)noexcept{volatile float r=a*b;return r;}
    static float div160E00(float a,float b)noexcept{volatile float r=a/b;return r;}
    // PRIVATE native adoption: original resources existed before the pool; this
    // native path completes missing custody around the SAME already-live pool,
    // atlas/material/count/global. It does not replay160E00 or reset live state.
    // Caller authenticates GlowScalar from that SAME retained PRT General Info.
    bool completeNativeResources160E00(float authenticatedGlowScalar,
        OriginalTrackParticleColdDiagnostic50LA& d)noexcept {
        using S=OriginalTrackParticleColdStatus50LA;
        d={};
        if(resourcesAttempted_)return fail(d,S::alreadyPrepared,resourcesFailurePC_?resourcesFailurePC_:0x161174u);
        if(!prepared()||capacity130_!=5000||depthBucketCount118_!=2500||
           authenticatedGlowScalar!=128.f||std::fegetround()!=FE_TONEAREST)
            return fail(d,S::input,0x160e00u);
        const auto* child=particles_->auxiliaryChild();
        if(!child||!child->constructed)return fail(d,S::particle,0x160e38u);
        // No enabled18=0 admission/reset: existing live auxiliary state is
        // adopted, not retrospectively certified as newly constructed.
        resourcesAttempted_=true;resourcesFailurePC_=0x160e28u;
        try {
            auxiliary1C0_=std::make_unique<Auxiliary162E70>();
            auto& auxiliary=*auxiliary1C0_;auxiliary.numericChild=child;
            const std::uint32_t raw=0x3c95e9e2u;
            std::memcpy(&auxiliary.field20,&raw,sizeof raw);
            auxiliary.inverseDimensions70={{div160E00(32767,90),div160E00(32767,35),div160E00(32767,90),1}};
            resourcesFailurePC_=0x160e5cu;
            buckets11C_.resize(static_cast<std::size_t>(depthBucketCount118_));
            resourcesFailurePC_=0x160ee8u;
            scratchBytes15C_=static_cast<std::size_t>(capacity130_+capacity130_/12+1)*64u+16u;
            scratch15C_.reset(static_cast<std::uint8_t*>(::operator new[](scratchBytes15C_,std::align_val_t(64))));
            std::memset(scratch15C_.get(),0,scratchBytes15C_);
            // Atlas110/13C and their counts remain represented by the existing
            // numeric vectors. Pool138/order and actual material150 untouched.
            glowScalar148_=authenticatedGlowScalar;field144_=1;
            quadPositions90Known_=true;
            quadPositions90_={{{{-1,1,0}},{{-1,-1,0}},{{1,1,0}},{{1,-1,0}}}};
            resourcesFailurePC_=0x161078u;
            // Bounded native Math, NOT EE/VU reciprocal-square-root parity.
            // Source VMUL/MADDA sum XYZ; VADDx.w separately copies input W1.
            for(std::size_t i=0;i<4;++i){
                auto& normal=normals50_[i];normal={{quadPositions90_[i][0],quadPositions90_[i][1],.25f,1}};
                const float squared=add160E00(add160E00(mul160E00(normal[0],normal[0]),
                    mul160E00(normal[1],normal[1])),mul160E00(normal[2],normal[2]));
                volatile float length=std::sqrt(squared);
                if(!std::isfinite(length)||!(length>0))return fail(d,S::input,resourcesFailurePC_);
                const float inverse=div160E00(1,length);
                for(unsigned axis=0;axis<3;++axis)normal[axis]=mul160E00(normal[axis],inverse);
            }
            resourcesComplete_=true;resourcesFailurePC_=0;
            d.status=S::complete;d.sourcePC=0x161174u;return true;
        }catch(const std::bad_alloc&){return fail(d,S::allocation,resourcesFailurePC_);}
        catch(...){return fail(d,S::input,resourcesFailurePC_);}
    }
    bool nativeResourcesCurrent160E00()const noexcept {
        return resourcesComplete_&&prepared()&&auxiliary1C0_&&auxiliary1C0_->coldResources()&&
            auxiliary1C0_->numericChild==particles_->auxiliaryChild()&&
            auxiliary1C0_->numericChild&&auxiliary1C0_->numericChild->constructed&&
            buckets11C_.size()==2500&&scratch15C_&&scratchBytes15C_==346704&&
            primitive160_.cold()&&primitive190_.cold();
    }
    bool matchesNativeResources160E00(const OriginalApplicationEntities50LA* application,
        const assets::OriginalStandardMaterialTable50LA* table,std::uint64_t generation,
        const OriginalGcParticleManager50KG* numeric)const noexcept {
        return nativeResourcesCurrent160E00()&&application_.get()==application&&table_.get()==table&&
            generation_==generation&&generation&&particles_.get()==numeric&&
            component_.kind==OriginalWreckComponentKind50LA::particleManager160B10&&
            component_.sessionGeneration==generation&&component_.objectKey==key(&component_);
    }
    bool canRetireNativeResources160E00()const noexcept {
        // Private cold resources have no exported packet/pointer borrower.
        return (!auxiliary1C0_||auxiliary1C0_->coldResources())&&primitive160_.cold()&&primitive190_.cold();
    }
    std::unique_ptr<Auxiliary162E70> auxiliary1C0_;
    std::vector<DepthBucket160E00> buckets11C_;
    std::unique_ptr<std::uint8_t,ScratchDelete160E00> scratch15C_;
    std::size_t scratchBytes15C_=0;
    Vec160E00 position30_{{0,0,0,1}},gravity40_{{0,-64,0,1}};
    std::array<Vec160E00,4> normals50_{};
    std::array<std::array<float,3>,4> quadPositions90_{};
    bool quadPositions90Known_=false;
    std::uint32_t sourceDispatch08_=0;
    // Source constructor pointer lanes, distinct from the native numeric
    // provider's internal storage. No160E00 publication is inferred from it.
    const void *sourceAtlas110_=nullptr,*sourceParticles128_=nullptr,*sourceParticlePointers138_=nullptr,
        *sourceGroups13C_=nullptr;
    //160BEC/160C14 zero stores;161870/161888 pass these SAME integer lanes
    //to158B30. They are signed counts, not constructor-NULL pointer owners.
    std::int32_t sourceNumFrames154_=0,sourceNumGroups140_=0;
    bool constructorReturned_=false,sourceEntityEnrolled_=false;
    enum class DepthWriteOrigin {none,nativePrewarm,source160E78};
    DepthWriteOrigin depthWriteOrigin_=DepthWriteOrigin::none;
    std::optional<std::int32_t> depthGlobalBefore_,depthGlobalAfter_;
    bool sourcePoolAttempted_=false,sourceParticlePointersPublished_=false;
    bool sourceLoadAttempted_=false,sourceAuxiliaryPublished_=false,sourceBucketsPublished_=false,
        sourceBucketAllocationFailed_=false,sourceScratchPublished_=false,sourceScratchCleared_=false;
    std::size_t sourceBucketWordsInitialized_=0;
    std::uint32_t sourceLoadPC_=0x160e00;
    //Each full SQ commits independently. Earlier source outputs survive a
    //later strict-native numerical refusal; constructor VF0 stays otherwise.
    bool sourceFallbackAttempted_=false,sourceFallbackLogAttempted_=false,
        sourceFallbackLogError_=false,sourceFallbackFramesReturned_=false;
    std::size_t sourceFallbackLogBytes_=0;
    std::uint32_t sourceNormalsStored_=0;
    bool sourceLoadReturned160E00_=false;
    //Native observation of the one-shot source continuation, never readiness.
    bool sourceMaterialDirectorySelected_=false,sourceMaterialDirectoryRestored161014_=false,
        sourceMaterialDirectoryNativeUnwound_=false,sourceMaterialAllocationAttempted_=false,
        sourceMaterialPublished150_=false,sourceMaterialReturned2465B0_=false;
    std::optional<bool> sourceConfigReturn1617A0_; //Native reached-return evidence, not a source field.
    std::array<std::array<std::optional<std::uint32_t>,5>,4> quadRemainder90_;
    Buffer160E00 primitive160_,primitive190_;
    std::optional<std::int32_t> minimum120_,maximum124_;
    std::uint32_t epoch114_=0,field12C_=0,field134_=0,field158_=0;
    const void* field14C_=nullptr;
    std::int32_t field144_=1;float glowScalar148_=255;
    bool resourcesAttempted_=false,resourcesComplete_=false;
    std::uint32_t resourcesFailurePC_=0;
    static std::uint64_t key(const void* p) noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    static bool fail(OriginalTrackParticleColdDiagnostic50LA& d,
        OriginalTrackParticleColdStatus50LA status,std::uint32_t pc) noexcept {
        d.status=status;d.sourcePC=pc;return false;
    }
    // Declaration order keeps both application/table pins alive until after
    // provider/material teardown even when a partially prepared owner dies.
    std::shared_ptr<OriginalApplicationEntities50LA> application_;
    std::shared_ptr<assets::OriginalStandardMaterialTable50LA> table_;
    OriginalWreckComponent50LA component_;
    std::unique_ptr<OriginalGcParticleManager50KG> particles_;
    std::unique_ptr<assets::OriginalStandardParticleMaterialReference50LA> material150_;
    std::uint64_t generation_=0;
    const ::ra_source_camera50LA::OwnershipBridge* nativeWreckBorrower_=nullptr;
    std::int32_t capacity130_=0,depthBucketCount118_=0;
    bool attempted_=false,entityConstructed_=false,depthGlobalWritten_=false;
    bool prepared_=false,closed_=false;
};
}
