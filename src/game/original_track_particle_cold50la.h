#pragma once
#include "original_application_entities50la.h"
#include "native_directional_normalize106ea0_proposal.h"
#include "original_gc_particle_manager50kg.h"
#include "original_particle_fog_velocity50la.h"
#include "original_particle_fog_lifetime50la.h"
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
    // Common rewrite (area E, W0 round 2 R7 owner gaps): the PSM/PST atlas members
    // the 05.06 loader read from the SAME archive closure as the PRT above
    // (1617A0 names all three from the SCB ParticleTextureMapName). Retained once,
    // after prepareNativeTrack succeeded, for the one particle material owner
    // (gc_particle_materials50kg_fixture). Host memory; nothing is written elsewhere.
    bool retainAtlasMaterialSources(std::vector<std::uint8_t> psm,std::vector<std::uint8_t> pst) noexcept {
        if(!prepared()||atlasSourcesRetained_||psm.empty()||pst.empty())return false;
        particlePsm_=std::move(psm);particlePst_=std::move(pst);atlasSourcesRetained_=true;return true;
    }
    const std::vector<std::uint8_t>& particlePsm() const noexcept{return particlePsm_;}
    const std::vector<std::uint8_t>& particlePst() const noexcept{return particlePst_;}
    // 1AC8E0 restart, World+4A0 particles: the manager's own 161F60 removal for the
    // one ground-effects owner (track_particle_emitters50CR::Owner::restart161F60).
    // `pc` receives the original address reached. Refused when not prepared.
    bool restartRemoval161F60(std::uint32_t& pc) noexcept {
        pc=0x161f60u;
        return prepared()&&particles_->removeRestartParticles161F60(pc);
    }

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
    //The first source Fog count store has separate native custody from its
    //caller receipt. No emitter/occurrence pointer below is a live capability.
    //Both abort orders are valid: emitter before Track, or Track before the
    //withdrawn descriptor. Structural history never dereferences these IDs.
    struct SourceFogCount160994 {
        const OriginalTrackParticleCold50LA* manager=nullptr;
        const OriginalGcParticleManager50KG* pool=nullptr;
        const OriginalGcParticle50KG* record=nullptr;
        const void* publication138=nullptr;
        const void* emitter=nullptr;
        const void* occurrence=nullptr;
        std::uint64_t generation=0;
        std::int32_t index128=0,before128=0,after128=0;
        std::uint32_t pc=0;
        bool stored=false;
    };
    static bool sourceFogCountHistoryNative(const SourceFogCount160994& h)noexcept {
        if(!h.stored)return !h.manager&&!h.pool&&!h.record&&!h.publication138&&
            !h.emitter&&!h.occurrence&&!h.generation&&!h.index128&&!h.before128&&!h.after128&&
            (h.pc==0||h.pc==0x160994u);
        return h.manager&&h.pool&&h.record&&h.publication138&&h.emitter&&h.occurrence&&
            h.generation&&h.index128==0&&h.before128==0&&h.after128==1&&h.pc==0x160994u;
    }
    static bool sourceFogSuccessorCountHistoryNative(const SourceFogCount160994& h)noexcept {
        if(!h.stored)return sourceFogCountHistoryNative(h);
        return h.manager&&h.pool&&h.record&&h.publication138&&h.emitter&&h.occurrence&&
            h.generation&&h.index128==1&&h.before128==1&&h.after128==2&&h.pc==0x160994u;
    }
    bool sourceActiveCountAuthorityNative()const noexcept {
        const auto& h=sourceFogCountPublication_;const auto& next=sourceFogSuccessorCountPublication_;
        if(sourceActiveCount128Known_!=constructorReturned_||!sourceFogCountHistoryNative(h)||
           !sourceFogSuccessorCountHistoryNative(next))return false;
        if(!h.stored)return !h.pc&&!next.pc&&!next.stored&&(!sourceActiveCount128Known_||sourceActiveCount128_==0);
        //Both immutable events outlive their emitters. No dereference of
        //borrowed identities, legacy count, or caller receipt is needed here.
        if(!sourceActiveCount128Known_||h.manager!=this||h.generation!=generation_||!particles_||
           h.pool!=particles_.get()||!sourceParticlePointersPublished_||h.publication138!=sourceParticlePointers138_||
           !sourceLoadReturned160E00_||!sourcePoolAttempted_||capacity130_!=5000)return false;
        if(!next.stored)return !next.pc&&sourceActiveCount128_==1;
        return sourceActiveCount128_==2&&next.manager==this&&next.generation==h.generation&&
            next.pool==h.pool&&next.publication138==h.publication138&&next.record!=h.record&&
            next.emitter!=h.emitter&&next.occurrence!=h.occurrence&&
            next.emitter==sourceFogSuccessorEmitter_&&next.occurrence==sourceFogSuccessorOccurrence_&&
            sourceFogSuccessorPosition_.stored&&sourceFogSuccessorPosition_.index128==1&&
            next.record==sourceFogSuccessorPosition_.record&&next.pool==sourceFogSuccessorPosition_.pool&&
            next.publication138==sourceFogSuccessorPosition_.publication138;
    }
    bool sourceFogCountObservationCurrentNative(std::int32_t observed,const void* occurrence)const noexcept {
        //1605AC,1605C8 and1606C4 belong to epoch0. Their retained values
        //stay zero after this SAME occurrence publishes epoch1, including
        //after the separate authenticated successor publishes epoch2.
        return occurrence&&observed==0&&sourceActiveCount128Known_&&sourceActiveCountAuthorityNative()&&
            (!sourceFogCountPublication_.stored||sourceFogCountPublication_.occurrence==occurrence);
    }
    bool sourceFogCountReceiptCurrentNative(const SourceFogCount160994& h,
        const void* exactEmitter,const void* occurrence)const noexcept {
        if(!sourceFogCountHistoryNative(h)||!exactEmitter||!occurrence||
           !sourceActiveCount128Known_||!sourceActiveCountAuthorityNative())return false;
        const auto& actual=sourceFogCountPublication_;
        if(h.stored!=actual.stored)return false;
        if(!h.stored)return true;
        return h.manager==actual.manager&&h.pool==actual.pool&&h.record==actual.record&&
            h.publication138==actual.publication138&&h.emitter==actual.emitter&&h.emitter==exactEmitter&&
            h.occurrence==actual.occurrence&&h.occurrence==occurrence&&h.generation==actual.generation&&
            h.index128==actual.index128&&h.before128==actual.before128&&h.after128==actual.after128&&h.pc==actual.pc;
    }
    //One distinct successor's read-only retirement scan. This Track-issued
    //ledger retains the first count publication independently of epoch2.
    //Copied identities are inert history after native borrower withdrawal.
    struct SourceFogRetirement160570 {
        SourceFogCount160994 publication;
        const void* readerEmitter=nullptr;
        const void* readerOccurrence=nullptr;
        const void* publication138=nullptr;
        const OriginalGcParticle50KG* record=nullptr; //A completed slot read may be NULL.
        const void* owner48=nullptr;
        std::array<const OriginalTrackParticleCold50LA*,3> managers70{};
        std::optional<std::int32_t> first128,again128;
        std::uint32_t index=0;
        std::uint8_t step=0; //5A8,AC,55C,564,56C,570,578,5A8,5AC,5B8 completed.
    };
    static bool sameSourceFogCountNative(const SourceFogCount160994& a,const SourceFogCount160994& b)noexcept {
        return a.manager==b.manager&&a.pool==b.pool&&a.record==b.record&&a.publication138==b.publication138&&
            a.emitter==b.emitter&&a.occurrence==b.occurrence&&a.generation==b.generation&&
            a.index128==b.index128&&a.before128==b.before128&&a.after128==b.after128&&a.pc==b.pc&&a.stored==b.stored;
    }
    static bool sourceFogRetirementHistoryNative(const SourceFogRetirement160570& h)noexcept {
        if(h.step>10||!sourceFogCountHistoryNative(h.publication))return false;
        if(!h.step)return !h.publication.stored&&!h.publication.pc&&!h.readerEmitter&&!h.readerOccurrence&&
            !h.publication138&&!h.record&&!h.owner48&&!h.managers70[0]&&!h.managers70[1]&&!h.managers70[2]&&
            !h.first128&&!h.again128&&!h.index;
        if(!h.publication.stored||!h.readerEmitter||!h.readerOccurrence||
           h.readerEmitter==h.publication.emitter||h.readerOccurrence==h.publication.occurrence||
           h.first128.has_value()!=(h.step>=2)||(h.first128&&*h.first128!=1)||
           h.managers70[0]!=h.publication.manager||
           h.managers70[1]!=(h.step>=3?h.publication.manager:nullptr)||
           h.managers70[2]!=(h.step>=8?h.publication.manager:nullptr)||
           h.publication138!=(h.step>=4?h.publication.publication138:nullptr)||
           (h.step<5&&h.record)||h.owner48!=(h.step>=6?h.publication.emitter:nullptr)||
           (h.step>=6&&h.record!=h.publication.record)||h.index!=(h.step>=7?1u:0u)||
           h.again128.has_value()!=(h.step>=9)||(h.again128&&*h.again128!=1))return false;
        return true;
    }
    bool sourceFogRetirementCurrentNative(const SourceFogRetirement160570& h,
        const void* reader,const void* occurrence)const noexcept {
        if(!sourceFogRetirementHistoryNative(h)||!h.step||!reader||!occurrence||
           h.readerEmitter!=reader||h.readerOccurrence!=occurrence||closed_||prepared_||
           !constructorReturned_||!sourceEntityEnrolled_||sourceParticleEmitterBorrowers_<2||
           !sourceLoadReturned160E00_||!particles_||!sourcePoolAttempted_||capacity130_!=5000||
           !sourceFogCountReceiptCurrentNative(h.publication,h.publication.emitter,h.publication.occurrence))return false;
        const auto& actual=sourceFogRetirementObservation_;
        if(!sourceFogRetirementHistoryNative(actual)||!sameSourceFogCountNative(h.publication,actual.publication)||
           h.readerEmitter!=actual.readerEmitter||h.readerOccurrence!=actual.readerOccurrence||
           h.publication138!=actual.publication138||h.record!=actual.record||h.owner48!=actual.owner48||
           h.managers70!=actual.managers70||h.first128!=actual.first128||h.again128!=actual.again128||
           h.index!=actual.index||h.step!=actual.step)return false;
        if(h.step>=4&&h.publication138!=sourceParticlePointers138_)return false;
        if(h.step>=5&&!particles_->sourcePositionSlotCurrentNative(h.publication138,0,h.record,nullptr))return false;
        if(h.step>=6){const void* owner=nullptr;
            if(!particles_->readSourceFogOwner160570(h.publication138,0,h.record,owner)||owner!=h.owner48)return false;
        }
        return true;
    }
    bool stepSourceFogRetirement160570(std::uint32_t pc,const OriginalTrackParticleCold50LA* freshManager70,
        const void* reader,const void* occurrence,std::uint64_t generation,SourceFogRetirement160570& h)noexcept {
        if(sourceFogSuccessorCountPublication_.stored||sourceActiveCount128_!=1)return false;
        constexpr std::uint32_t pcs[]={0x1605a8,0x1605ac,0x16055c,0x160564,0x16056c,0x160570,0x160578,0x1605a8,0x1605ac,0x1605b8};
        if(!sourceFogRetirementHistoryNative(h)||h.step>=10||pc!=pcs[h.step]||freshManager70!=this||
           !reader||!occurrence||generation!=generation_)return false;
        if(!h.step){
            if(sourceFogRetirementObservation_.step||!sourceFogRetirementHistoryNative(sourceFogRetirementObservation_)||
               closed_||prepared_||!constructorReturned_||!sourceEntityEnrolled_||sourceParticleEmitterBorrowers_<2||
               !sourceLoadReturned160E00_||!particles_||!sourcePoolAttempted_||capacity130_!=5000||
               !sourceActiveCountAuthorityNative()||!sourceFogCountPublication_.stored||
               sourceActiveCount128_!=1||sourceFogCountPublication_.emitter==reader||
               sourceFogCountPublication_.occurrence==occurrence)return false;
        }else if(!sourceFogRetirementCurrentNative(h,reader,occurrence))return false;
        auto next=h;
        switch(h.step){
            case 0:next.publication=sourceFogCountPublication_;next.readerEmitter=reader;next.readerOccurrence=occurrence;
                next.managers70[0]=freshManager70;break; //Actual initial1605A8 read; no count read claimed yet.
            case 1:next.first128=sourceActiveCount128_;break; //Actual first1605AC read, not epoch0 replay.
            case 2:if(!h.first128||static_cast<std::int32_t>(h.index)>=*h.first128)return false;
                next.managers70[1]=freshManager70;break; //Reached only through the actual true signed comparison.
            case 3:next.publication138=sourceParticlePointers138_;break;
            case 4:if(!particles_->readSourceFogRetirementSlot16056C(h.publication138,next.record))return false;break;
            case 5:
                if(h.record!=h.publication.record||!particles_->readSourceFogOwner160570(
                    h.publication138,0,h.record,next.owner48)||next.owner48!=h.publication.emitter)return false;
                break;
            case 6:if(h.owner48==reader)return false;next.index=1;break; //Taken BNEL delay only.
            case 7:next.managers70[2]=freshManager70;break; //Fresh second1605A8.
            case 8:if(sourceActiveCount128_!=1)return false;next.again128=sourceActiveCount128_;break;
            case 9:if(!h.again128||static_cast<std::int32_t>(h.index)<*h.again128)return false;break;
        }
        ++next.step;if(!sourceFogRetirementHistoryNative(next))return false;
        sourceFogRetirementObservation_=next;h=next;return true;
    }
    bool sourceEntityCountedNative(const OriginalApplicationEntities50LA* expected,std::uint64_t generation,
        std::size_t& count)const noexcept {
        count=0;
        if(closed_||application_.get()!=expected||!expected||generation_!=generation||!attempted_||prepared_||
           nativeWreckBorrower_||!canRetireNativeResources160E00())return false;
        //Constructor epoch0, or the two independently authenticated160994 events.
        //This fixed Track-owned history outlives the emitter on direct abort.
        if(!sourceActiveCountAuthorityNative())return false;
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
    //One source1606E4 slot load and optional1606FC position SQ only. These
    //native receipt pointers never replace source fields or claim owner48.
    struct SourceFogPosition1606FC {
        const OriginalGcParticleManager50KG* pool=nullptr;
        const OriginalGcParticle50KG* record=nullptr;
        const void* publication138=nullptr;
        std::optional<std::int32_t> index128;
        std::array<std::uint32_t,4> before{},after{};
        bool slotRead=false,stored=false;
        std::uint32_t pc=0;
    };
    static bool sourceFogPositionHistoryNative(const SourceFogPosition1606FC& h)noexcept {
        const std::array<std::uint32_t,4> empty{};
        //Structural history only. In particular, do not dereference these
        //historical pointers after native emitter-borrow withdrawal.
        if(!h.slotRead)return !h.pool&&!h.record&&!h.publication138&&!h.index128&&
            !h.stored&&h.before==empty&&h.after==empty&&(h.pc==0||h.pc==0x1606e4u);
        if(!h.pool||!h.publication138||!h.index128||*h.index128<0||
           static_cast<std::uint32_t>(*h.index128)>=originalGcParticleCapacity50KG||
           (h.pc!=0x1606e4u&&h.pc!=0x1606fcu))return false;
        if(!h.stored)return h.before==empty&&h.after==empty;
        return h.record&&h.pc==0x1606fcu&&h.before[3]==0x3f800000u&&h.after[3]==0x3f800000u;
    }
    bool sourceFogPositionCurrentNative(const SourceFogPosition1606FC& h)const noexcept {
        if(!sourceFogPositionHistoryNative(h))return false;
        if(!h.slotRead)return true;
        if(closed_||prepared_||!constructorReturned_||!sourceEntityEnrolled_||
           !sourceLoadReturned160E00_||!sourcePoolAttempted_||!sourceParticlePointersPublished_||
           !sourceParticleEmitterBorrowers_||!particles_||particles_.get()!=h.pool||
           sourceParticlePointers138_!=h.publication138||!sourceActiveCount128Known_||
           *h.index128!=0||!sourceActiveCountAuthorityNative()||capacity130_!=5000)return false;
        //The reached1606C4 index remains epoch0. Only this same stable slot
        //may survive the sole count publication; no later creator is admitted.
        const auto& count=sourceFogCountPublication_;
        if(count.stored&&(count.pool!=h.pool||count.publication138!=h.publication138||
           count.record!=h.record||count.index128!=*h.index128))return false;
        return particles_->sourcePositionSlotCurrentNative(h.publication138,*h.index128,h.record,
            h.stored?&h.after:nullptr);
    }
    bool readSourceFogPositionSlot1606E4(std::int32_t reachedIndex128,
        const void* reachedPublication138,SourceFogPosition1606FC& h)noexcept {
        if(!sourceFogPositionHistoryNative(h)||h.pc||h.slotRead)return false;
        h.pc=0x1606e4; //Failed native admission leaves the real source load pending.
        if(closed_||prepared_||!constructorReturned_||!sourceEntityEnrolled_||
           !sourceLoadReturned160E00_||!sourcePoolAttempted_||!sourceParticlePointersPublished_||
           !sourceParticleEmitterBorrowers_||!particles_||!sourceActiveCount128Known_||
           !sourceActiveCountAuthorityNative()||sourceFogCountPublication_.stored||
           sourceActiveCount128_!=reachedIndex128||
           sourceParticlePointers138_!=reachedPublication138||capacity130_!=5000)return false;
        const OriginalGcParticle50KG* record=nullptr;
        //Use the caller's actual1606D0 operand, not a fresh source-field read.
        //The native equality guard above authenticates that retained identity.
        if(!particles_->readSourcePositionSlot1606E4(reachedPublication138,reachedIndex128,record))return false;
        h.pool=particles_.get();h.publication138=reachedPublication138;
        h.index128=reachedIndex128;h.record=record;h.slotRead=true;
        return true; //NULL is retained; its reached store is bounded separately.
    }
    bool storeSourceFogPosition1606FC(const std::array<std::uint32_t,4>& after,
        SourceFogPosition1606FC& h)noexcept {
        if(!sourceFogPositionHistoryNative(h)||!h.slotRead||h.stored||h.pc!=0x1606e4u)return false;
        h.pc=0x1606fc;
        if(!sourceFogPositionCurrentNative(h))return false;
        std::array<std::uint32_t,4> before{};
        if(!particles_->storeSourcePosition1606FC(h.publication138,*h.index128,h.record,after,before))return false;
        h.before=before;h.after=after;h.stored=true;
        return true; //Before160700: no later source22C/velocity/48 access or128 write.
    }
    //Actual epoch1 reads by the exact scanned successor. After its own
    //publication they remain historical1, not a fresh epoch2 read or replay.
    bool sourceFogSuccessorCountObservationCurrentNative(std::int32_t observed,
        const SourceFogRetirement160570& scan,const void* reader,const void* occurrence)const noexcept {
        return observed==1&&scan.step==10&&sourceFogRetirementCurrentNative(scan,reader,occurrence)&&
            sourceActiveCount128Known_&&(!sourceFogSuccessorCountPublication_.stored?
                sourceActiveCount128_==1:(sourceActiveCount128_==2&&
                sourceFogSuccessorCountPublication_.emitter==reader&&sourceFogSuccessorCountPublication_.occurrence==occurrence));
    }
    static bool sourceFogSuccessorPositionHistoryNative(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& h)noexcept {
        if(!sourceFogRetirementHistoryNative(scan)||!sourceFogPositionHistoryNative(h))return false;
        if(!h.pc&&!h.slotRead)return true;
        if(scan.step!=10)return false;
        if(!h.slotRead)return h.pc==0x1606e4u;
        return h.index128==1&&h.pool==scan.publication.pool&&h.publication138==scan.publication.publication138&&
            (!h.stored||h.record!=scan.publication.record);
    }
    bool sourceFogSuccessorPositionCurrentNative(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& h)const noexcept {
        if(!sourceFogSuccessorPositionHistoryNative(scan,h)||
           !sourceFogSuccessorCountObservationCurrentNative(1,scan,scan.readerEmitter,scan.readerOccurrence))return false;
        const auto& actual=sourceFogSuccessorPosition_;
        if(!h.slotRead)return !actual.pc&&!actual.slotRead&&!sourceFogSuccessorEmitter_&&!sourceFogSuccessorOccurrence_;
        if(sourceFogSuccessorEmitter_!=scan.readerEmitter||sourceFogSuccessorOccurrence_!=scan.readerOccurrence||
           !sourceFogSuccessorPositionHistoryNative(scan,actual)||!actual.slotRead||h.pool!=actual.pool||
           h.record!=actual.record||h.publication138!=actual.publication138||h.index128!=actual.index128||
           h.before!=actual.before||h.after!=actual.after||h.stored!=actual.stored||
           (h.pc!=actual.pc&&!(h.pc==0x1606fcu&&actual.pc==0x1606e4u&&!h.stored)))return false;
        //An attempted SQ changes only the caller's pending PC until the real
        //write succeeds. The exact issued slot observation remains immutable.
        return particles_->sourcePositionSlotCurrentNative(h.publication138,1,h.record,h.stored?&h.after:nullptr);
    }
    bool readSourceFogSuccessorPositionSlot1606E4(const SourceFogRetirement160570& scan,
        std::int32_t reachedIndex128,const void* reachedPublication138,SourceFogPosition1606FC& h)noexcept {
        if(sourceFogSuccessorCountPublication_.stored||sourceActiveCount128_!=1)return false;
        if(!sourceFogPositionHistoryNative(h)||h.pc||h.slotRead)return false;
        h.pc=0x1606e4;
        if(!sourceFogSuccessorPositionCurrentNative(scan,h)||reachedIndex128!=1||
           reachedPublication138!=sourceParticlePointers138_)return false;
        const OriginalGcParticle50KG* record=nullptr;
        if(!particles_->readSourcePositionSlot1606E4(reachedPublication138,reachedIndex128,record))return false;
        h.pool=particles_.get();h.publication138=reachedPublication138;h.index128=reachedIndex128;
        h.record=record;h.slotRead=true; //NULL or alias is retained before any record-store admission.
        sourceFogSuccessorPosition_=h;sourceFogSuccessorEmitter_=scan.readerEmitter;
        sourceFogSuccessorOccurrence_=scan.readerOccurrence;return true;
    }
    bool storeSourceFogSuccessorPosition1606FC(const SourceFogRetirement160570& scan,
        const std::array<std::uint32_t,4>& after,SourceFogPosition1606FC& h)noexcept {
        if(sourceFogSuccessorCountPublication_.stored||sourceActiveCount128_!=1)return false;
        if(!sourceFogSuccessorPositionHistoryNative(scan,h)||!h.slotRead||h.stored||h.pc!=0x1606e4u)return false;
        h.pc=0x1606fc;
        if(!sourceFogSuccessorPositionCurrentNative(scan,h)||!h.record||h.record==scan.publication.record)return false;
        std::array<std::uint32_t,4> before{};
        if(!particles_->storeSourcePosition1606FC(h.publication138,1,h.record,after,before))return false;
        h.before=before;h.after=after;h.stored=true;sourceFogSuccessorPosition_=h;
        return true; //Only inactive slot1 position00. Count1, slot0 and all source48 lanes are untouched.
    }
    //One16088C SQ into the same already-positioned first Fog record. There
    //is no separate slot identity or replacement record in this receipt.
    struct SourceFogVelocity16088C {
        std::array<std::uint32_t,4> before{},after{};
        bool stored=false;
        std::uint32_t pc=0;
    };
    static bool sourceFogVelocityHistoryNative(const SourceFogVelocity16088C& h)noexcept {
        const std::array<std::uint32_t,4> empty{};
        //Historical validation never dereferences the retired position owner.
        if(!h.stored)return h.before==empty&&h.after==empty&&(h.pc==0||h.pc==0x16088cu);
        return h.pc==0x16088cu&&h.before[3]==0x3f800000u&&
            detail::sourceParticleFogVelocityVector50LA(h.after);
    }
    bool sourceFogVelocityCurrentNative(const SourceFogPosition1606FC& position,
        const SourceFogVelocity16088C& h)const noexcept {
        if(!sourceFogVelocityHistoryNative(h)||!position.stored||!position.index128||
           *position.index128!=0||!sourceFogPositionCurrentNative(position))return false;
        return particles_->sourceVelocitySlotCurrentNative(position.publication138,*position.index128,
            position.record,h.stored?&h.after:nullptr);
    }
    bool storeSourceFogVelocity16088C(const SourceFogPosition1606FC& position,
        const std::array<std::uint32_t,4>& after,SourceFogVelocity16088C& h)noexcept {
        if(!sourceFogVelocityHistoryNative(h)||h.pc||h.stored)return false;
        h.pc=0x16088c; //One attempt: every refusal retains this exact pending SQ.
        if(!sourceFogVelocityCurrentNative(position,h))return false;
        std::array<std::uint32_t,4> before{};
        if(!particles_->storeSourceVelocity16088C(position.publication138,*position.index128,
           position.record,after,before))return false;
        h.before=before;h.after=after;h.stored=true;
        return true; //No scalar/owner field or count128 publication here.
    }
    static bool sourceFogSuccessorVelocityHistoryNative(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,const SourceFogVelocity16088C& h)noexcept {
        return sourceFogVelocityHistoryNative(h)&&sourceFogSuccessorPositionHistoryNative(scan,position)&&
            ((!h.pc&&!h.stored)||(scan.step==10&&position.stored));
    }
    bool sourceFogSuccessorVelocityCurrentNative(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,const SourceFogVelocity16088C& h)const noexcept {
        if(!sourceFogSuccessorVelocityHistoryNative(scan,position,h)||!position.stored||
           !sourceFogSuccessorPositionCurrentNative(scan,position))return false;
        const auto& actual=sourceFogSuccessorVelocity_;
        if(!sourceFogVelocityHistoryNative(actual)||h.stored!=actual.stored||h.before!=actual.before||h.after!=actual.after||
           (h.pc!=actual.pc&&!(h.pc==0x16088cu&&!actual.pc&&!h.stored)))return false;
        return particles_->sourceSuccessorVelocitySlotCurrentNative(position.publication138,1,position.record,
            h.stored?&h.after:nullptr);
    }
    bool storeSourceFogSuccessorVelocity16088C(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,const std::array<std::uint32_t,4>& after,
        SourceFogVelocity16088C& h)noexcept {
        if(sourceFogSuccessorCountPublication_.stored||sourceActiveCount128_!=1)return false;
        if(!sourceFogSuccessorVelocityHistoryNative(scan,position,h)||h.pc||h.stored)return false;
        h.pc=0x16088c;
        if(!sourceFogSuccessorVelocityCurrentNative(scan,position,h))return false;
        std::array<std::uint32_t,4> before{};
        if(!particles_->storeSourceSuccessorVelocity16088C(position.publication138,1,position.record,after,before))return false;
        h.before=before;h.after=after;h.stored=true;sourceFogSuccessorVelocity_=h;
        return true; //One actual slot1 SQ; no count or source48 effect.
    }
    //One partial first Fog record after1606FC. emitter is native provenance
    //from the first successful store; source48 is known only at count>=3.
    struct SourceFogFields1608AC {
        const void* emitter=nullptr;
        std::array<std::uint32_t,5> values{}; //38,3C,30,2C,28 raw source inputs.
        std::uint8_t storedCount=0;
        std::uint32_t pc=0;
        std::optional<std::uint32_t> lifeRead30;
    };
    static bool sourceFogFieldsHistoryNative(const SourceFogFields1608AC& h)noexcept {
        constexpr std::uint32_t stores[]={0x1608ac,0x1608b8,0x1608bc,0x1608c4,0x1608cc,0x1608d4};
        constexpr std::uint8_t knownAt[]={1,2,4,5,6};
        if(h.storedCount>6||bool(h.emitter)!=(h.storedCount!=0))return false;
        for(std::size_t i=0;i<h.values.size();++i)if(h.storedCount<knownAt[i]&&h.values[i])return false;
        //Exact completed-store PC or the one attempted next operation. No
        //historical pool, particle or emitter pointer is dereferenced here.
        const auto last=h.storedCount?stores[h.storedCount-1]:0u;
        const auto next=h.storedCount<6?stores[h.storedCount]:0x1608d8u;
        if(h.pc!=last&&h.pc!=next)return false;
        return !h.lifeRead30||(h.storedCount==6&&h.pc==0x1608d8u&&*h.lifeRead30==h.values[2]);
    }
    bool sourceFogFieldsCurrentNative(const SourceFogPosition1606FC& position,
        const SourceFogFields1608AC& h,const void* exactEmitter)const noexcept {
        if(!sourceFogFieldsHistoryNative(h)||!position.stored||!position.index128||
           *position.index128!=0||!exactEmitter||(h.emitter&&h.emitter!=exactEmitter)||
           (sourceFogCountPublication_.stored&&sourceFogCountPublication_.emitter!=exactEmitter)||
           !sourceFogPositionCurrentNative(position))return false;
        return particles_->sourceFogFieldsCurrentNative(position.publication138,*position.index128,
            position.record,exactEmitter,h.storedCount,h.values);
    }
    bool storeSourceFogField1608AC(const SourceFogPosition1606FC& position,const void* exactEmitter,
        std::uint32_t sourcePC,std::uint32_t inputBits,SourceFogFields1608AC& h)noexcept {
        constexpr std::uint32_t stores[]={0x1608ac,0x1608b8,0x1608bc,0x1608c4,0x1608cc,0x1608d4};
        if(!sourceFogFieldsHistoryNative(h)||h.storedCount>=6||h.lifeRead30||
           sourcePC!=stores[h.storedCount]||h.pc!=(h.storedCount?stores[h.storedCount-1]:0u))return false;
        h.pc=sourcePC; //One-attempt boundary. Earlier successful stores stay.
        if(!sourceFogFieldsCurrentNative(position,h,exactEmitter))return false;
        const auto prior=h.storedCount;
        if(!particles_->storeSourceFogField1608AC(position.publication138,*position.index128,
            position.record,exactEmitter,prior,sourcePC,inputBits))return false;
        if(prior<2)h.values[prior]=inputBits;
        else if(prior>2)h.values[prior-1]=inputBits;
        h.emitter=exactEmitter;++h.storedCount;
        return true; //Only this one original store; no source128 change.
    }
    bool readSourceFogLife1608D8(const SourceFogPosition1606FC& position,const void* exactEmitter,
        SourceFogFields1608AC& h)noexcept {
        if(!sourceFogFieldsHistoryNative(h)||h.storedCount!=6||h.pc!=0x1608d4u||h.lifeRead30)return false;
        h.pc=0x1608d8;
        if(!sourceFogFieldsCurrentNative(position,h,exactEmitter))return false;
        std::uint32_t bits=0;
        if(!particles_->readSourceFogLife1608D8(position.publication138,*position.index128,
            position.record,exactEmitter,bits))return false;
        h.lifeRead30=bits;
        return true; //Authentic operand for the separately bounded DIV.S1608DC.
    }
    static bool sourceFogSuccessorFieldsHistoryNative(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,const SourceFogFields1608AC& h,const void* exactEmitter)noexcept {
        if(!sourceFogFieldsHistoryNative(h)||!sourceFogSuccessorPositionHistoryNative(scan,position))return false;
        if(!h.pc&&!h.storedCount)return true;
        return scan.step==10&&position.stored&&exactEmitter&&exactEmitter==scan.readerEmitter&&
            (!h.emitter||h.emitter==exactEmitter);
    }
    bool sourceFogSuccessorFieldsCurrentNative(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,const SourceFogFields1608AC& h,const void* exactEmitter)const noexcept {
        if(!sourceFogSuccessorFieldsHistoryNative(scan,position,h,exactEmitter)||!position.stored||
           !exactEmitter||exactEmitter!=scan.readerEmitter||h.lifeRead30!=sourceFogSuccessorLifeRead30_||
           !sourceFogSuccessorPositionCurrentNative(scan,position))return false;
        return particles_->sourceSuccessorFogFieldsCurrentNative(position.publication138,1,position.record,
            exactEmitter,h.storedCount,h.values);
    }
    bool storeSourceFogSuccessorField1608AC(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,const void* exactEmitter,std::uint32_t sourcePC,
        std::uint32_t inputBits,SourceFogFields1608AC& h)noexcept {
        if(sourceFogSuccessorCountPublication_.stored||sourceActiveCount128_!=1)return false;
        constexpr std::uint32_t stores[]={0x1608ac,0x1608b8,0x1608bc,0x1608c4,0x1608cc,0x1608d4};
        if(!sourceFogSuccessorFieldsHistoryNative(scan,position,h,exactEmitter)||h.storedCount>=6||
           sourcePC!=stores[h.storedCount]||h.pc!=(h.storedCount?stores[h.storedCount-1]:0u))return false;
        h.pc=sourcePC;
        if(!sourceFogSuccessorFieldsCurrentNative(scan,position,h,exactEmitter))return false;
        const auto prior=h.storedCount;
        if(!particles_->storeSourceSuccessorFogField1608AC(position.publication138,1,position.record,
            exactEmitter,prior,sourcePC,inputBits))return false;
        if(prior<2)h.values[prior]=inputBits;
        else if(prior>2)h.values[prior-1]=inputBits;
        h.emitter=exactEmitter;++h.storedCount;
        return true; //One source store only. The new+48 lane is known after store3, never before.
    }
    bool readSourceFogSuccessorLife1608D8(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,const void* exactEmitter,SourceFogFields1608AC& h)noexcept {
        if(!sourceFogSuccessorFieldsHistoryNative(scan,position,h,exactEmitter)||
           h.storedCount!=6||h.pc!=0x1608d4u||h.lifeRead30)return false;
        h.pc=0x1608d8;
        if(sourceFogSuccessorCountPublication_.stored||sourceActiveCount128_!=1||
           !sourceFogSuccessorFieldsCurrentNative(scan,position,h,exactEmitter))return false;
        std::uint32_t bits=0;
        if(!particles_->readSourceSuccessorFogLife1608D8(position.publication138,1,position.record,exactEmitter,bits))return false;
        sourceFogSuccessorLifeRead30_=bits;h.lifeRead30=bits;
        return true; //Actual slot1 read, separately issued before the bounded DIV.
    }
    //Post-DIV custody is separate from the six completed1608AC..1608D4
    //stores and authentic1608D8 load. pcs/values record only completed source
    //operations; pc records the last operation or a failed next attempt.
    struct SourceFogTail1608E0 {
        std::array<std::uint32_t,7> pcs{},values{};
        std::uint8_t completedOps=0;
        std::uint32_t pc=0;
    };
    static bool sourceFogTailOperationPCNative(const SourceFogTail1608E0& h,
        std::uint8_t operation,std::uint32_t pc)noexcept {
        switch(operation){
            case 0:return pc==0x1608e0u;
            case 1:return pc==0x1608e4u;
            case 2:return pc==0x1608f0u;
            case 3:return pc==0x160930u||pc==0x160940u;
            case 4:return (h.pcs[3]==0x160930u&&pc==0x16093cu)||
                          (h.pcs[3]==0x160940u&&pc==0x16094cu);
            case 5:return pc==0x160964u||pc==0x160974u;
            case 6:return (h.pcs[5]==0x160964u&&pc==0x160970u)||
                          (h.pcs[5]==0x160974u&&pc==0x160984u);
            default:return false;
        }
    }
    static bool sourceFogTailHistoryNative(const SourceFogTail1608E0& h)noexcept {
        if(h.completedOps>7)return false;
        for(std::uint8_t operation=0;operation<7;++operation){
            if(operation<h.completedOps){
                if(!sourceFogTailOperationPCNative(h,operation,h.pcs[operation]))return false;
            }else if(h.pcs[operation]||h.values[operation])return false;
        }
        const auto last=h.completedOps?h.pcs[h.completedOps-1]:0u;
        if(h.pc!=last&&!sourceFogTailOperationPCNative(h,h.completedOps,h.pc))return false;
        //All checks are structural. No historical emitter/pool/record pointer
        //is dereferenced, including after native borrow withdrawal.
        if(h.completedOps>=1&&!detail::sourceParticleFogReciprocalWord50LA(h.values[0]))return false;
        if(h.completedOps>=2&&h.values[1])return false;
        if(h.completedOps>=4&&h.values[3]!=h.values[2])return false;
        if(h.completedOps>=5&&h.values[4]!=(h.pcs[3]==0x160930u?
           (h.values[3]|0x4000u):(h.values[3]&0xffffbfffu)))return false;
        if(h.completedOps>=6&&h.values[5]!=h.values[4])return false;
        if(h.completedOps>=7&&h.values[6]!=(h.pcs[5]==0x160964u?
           (h.values[5]|0x8000u):(h.values[5]&0xffff7fffu)))return false;
        return true;
    }
    bool sourceFogTailCurrentNative(const SourceFogPosition1606FC& position,
        const SourceFogFields1608AC& fields,const SourceFogTail1608E0& h,
        const void* exactEmitter)const noexcept {
        if(!sourceFogTailHistoryNative(h)||fields.storedCount!=6||fields.pc!=0x1608d8u||
           !fields.lifeRead30||!sourceFogFieldsCurrentNative(position,fields,exactEmitter))return false;
        //Authenticate every attempted/completed reciprocal against the real
        //retained record30 read. An empty receipt still permits an unchanged
        //unsupported-DIV stop; no new source read or flag effect is invented.
        if(h.pc||h.completedOps){
            std::uint32_t inverse=0;bool assertedDivideByZero=false;
            if(!detail::sourceParticleFogReciprocal50LA(*fields.lifeRead30,inverse,assertedDivideByZero))return false;
            if(h.completedOps&&h.values[0]!=inverse)return false;
        }
        return particles_->sourceFogTailCurrentNative(position.publication138,*position.index128,
            position.record,exactEmitter,h.completedOps,h.pcs,h.values);
    }
    bool storeSourceFogTail1608E0(const SourceFogPosition1606FC& position,
        const SourceFogFields1608AC& fields,const void* exactEmitter,std::uint32_t sourcePC,
        std::uint32_t inputBits,SourceFogTail1608E0& h)noexcept {
        if(!sourceFogTailHistoryNative(h)||h.completedOps>=7||h.completedOps==3||h.completedOps==5||
           !sourceFogTailOperationPCNative(h,h.completedOps,sourcePC)||
           h.pc!=(h.completedOps?h.pcs[h.completedOps-1]:0u))return false;
        h.pc=sourcePC; //Failed attempt leaves this exact source store pending.
        if(!sourceFogTailCurrentNative(position,fields,h,exactEmitter))return false;
        const auto prior=h.completedOps;
        if(!prior){std::uint32_t inverse=0;bool assertedDivideByZero=false;
            if(!detail::sourceParticleFogReciprocal50LA(*fields.lifeRead30,inverse,assertedDivideByZero)||inputBits!=inverse)return false;}
        if(!particles_->storeSourceFogTail1608E0(position.publication138,*position.index128,
           position.record,exactEmitter,prior,sourcePC,inputBits))return false;
        h.pcs[prior]=sourcePC;h.values[prior]=inputBits;++h.completedOps;
        return true; //No source128 change or owner48 dereference.
    }
    bool readSourceFogFlags160930(const SourceFogPosition1606FC& position,
        const SourceFogFields1608AC& fields,const void* exactEmitter,std::uint32_t sourcePC,
        SourceFogTail1608E0& h)noexcept {
        if(!sourceFogTailHistoryNative(h)||(h.completedOps!=3&&h.completedOps!=5)||
           !sourceFogTailOperationPCNative(h,h.completedOps,sourcePC)||
           h.pc!=h.pcs[h.completedOps-1])return false;
        h.pc=sourcePC; //Each failed native admission retains the reached LW.
        if(!sourceFogTailCurrentNative(position,fields,h,exactEmitter))return false;
        const auto prior=h.completedOps;std::uint32_t bits=0;
        if(!particles_->readSourceFogFlags160930(position.publication138,*position.index128,
           position.record,exactEmitter,prior,sourcePC,bits))return false;
        h.pcs[prior]=sourcePC;h.values[prior]=bits;++h.completedOps;
        return true; //Actual record44 read; values[3]/[5] are its source result.
    }
    static bool sourceFogSuccessorTailHistoryNative(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,const SourceFogFields1608AC& fields,
        const SourceFogTail1608E0& h,const void* exactEmitter)noexcept {
        if(!sourceFogTailHistoryNative(h)||!sourceFogSuccessorFieldsHistoryNative(scan,position,fields,exactEmitter))return false;
        if(!h.pc&&!h.completedOps)return true;
        if(!fields.lifeRead30)return false;
        std::uint32_t inverse=0;bool assertedDivideByZero=false;
        if(!detail::sourceParticleFogReciprocal50LA(*fields.lifeRead30,inverse,assertedDivideByZero))return false;
        return !h.completedOps||h.values[0]==inverse;
    }
    bool sourceFogSuccessorTailCurrentNative(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,
        const SourceFogFields1608AC& fields,const SourceFogTail1608E0& h,
        const void* exactEmitter)const noexcept {
        if(!sourceFogSuccessorTailHistoryNative(scan,position,fields,h,exactEmitter)||fields.storedCount!=6||fields.pc!=0x1608d8u||
           !fields.lifeRead30||!sourceFogSuccessorFieldsCurrentNative(scan,position,fields,exactEmitter))return false;
        //Authenticate every attempted/completed reciprocal against the real
        //retained record30 read. An empty receipt still permits an unchanged
        //unsupported-DIV stop; no new source read or flag effect is invented.
        if(h.pc||h.completedOps){
            std::uint32_t inverse=0;bool assertedDivideByZero=false;
            if(!detail::sourceParticleFogReciprocal50LA(*fields.lifeRead30,inverse,assertedDivideByZero))return false;
            if(h.completedOps&&h.values[0]!=inverse)return false;
        }
        return particles_->sourceSuccessorFogTailCurrentNative(position.publication138,*position.index128,
            position.record,exactEmitter,h.completedOps,h.pcs,h.values);
    }
    bool storeSourceFogSuccessorTail1608E0(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,
        const SourceFogFields1608AC& fields,const void* exactEmitter,std::uint32_t sourcePC,
        std::uint32_t inputBits,SourceFogTail1608E0& h)noexcept {
        if(!sourceFogTailHistoryNative(h)||h.completedOps>=7||h.completedOps==3||h.completedOps==5||
           !sourceFogTailOperationPCNative(h,h.completedOps,sourcePC)||
           h.pc!=(h.completedOps?h.pcs[h.completedOps-1]:0u))return false;
        h.pc=sourcePC; //Failed attempt leaves this exact source store pending.
        if(sourceFogSuccessorCountPublication_.stored||sourceActiveCount128_!=1||
           !sourceFogSuccessorTailCurrentNative(scan,position,fields,h,exactEmitter))return false;
        const auto prior=h.completedOps;
        if(!prior){std::uint32_t inverse=0;bool assertedDivideByZero=false;
            if(!detail::sourceParticleFogReciprocal50LA(*fields.lifeRead30,inverse,assertedDivideByZero)||inputBits!=inverse)return false;}
        if(!particles_->storeSourceSuccessorFogTail1608E0(position.publication138,*position.index128,
           position.record,exactEmitter,prior,sourcePC,inputBits))return false;
        h.pcs[prior]=sourcePC;h.values[prior]=inputBits;++h.completedOps;
        return true; //No source128 change or owner48 dereference.
    }
    bool readSourceFogSuccessorFlags160930(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,
        const SourceFogFields1608AC& fields,const void* exactEmitter,std::uint32_t sourcePC,
        SourceFogTail1608E0& h)noexcept {
        if(!sourceFogTailHistoryNative(h)||(h.completedOps!=3&&h.completedOps!=5)||
           !sourceFogTailOperationPCNative(h,h.completedOps,sourcePC)||
           h.pc!=h.pcs[h.completedOps-1])return false;
        h.pc=sourcePC; //Each failed native admission retains the reached LW.
        if(sourceFogSuccessorCountPublication_.stored||sourceActiveCount128_!=1||
           !sourceFogSuccessorTailCurrentNative(scan,position,fields,h,exactEmitter))return false;
        const auto prior=h.completedOps;std::uint32_t bits=0;
        if(!particles_->readSourceSuccessorFogFlags160930(position.publication138,*position.index128,
           position.record,exactEmitter,prior,sourcePC,bits))return false;
        h.pcs[prior]=sourcePC;h.values[prior]=bits;++h.completedOps;
        return true; //Actual record44 read; values[3]/[5] are its source result.
    }
    bool storeSourceFogCount160994(const SourceFogPosition1606FC& position,
        const SourceFogFields1608AC& fields,const SourceFogTail1608E0& tail,
        const void* exactEmitter,const void* occurrence,const OriginalTrackParticleCold50LA* reachedManager70,
        std::int32_t reachedBefore128,std::int32_t reachedAfter128,SourceFogCount160994& h)noexcept {
        if(!sourceFogCountHistoryNative(h)||h.pc||h.stored)return false;
        h.pc=0x160994; //One attempt; refusal leaves the real SW pending.
        if(!exactEmitter||!occurrence||reachedManager70!=this||reachedBefore128!=0||reachedAfter128!=1||
           sourceFogCountPublication_.stored||!sourceActiveCountAuthorityNative()||
           !sourceActiveCount128Known_||sourceActiveCount128_!=reachedBefore128||
           tail.completedOps!=7||tail.pc!=tail.pcs[6]||
           !sourceFogTailCurrentNative(position,fields,tail,exactEmitter))return false;
        SourceFogCount160994 committed;
        committed.manager=this;committed.pool=position.pool;committed.record=position.record;
        committed.publication138=position.publication138;committed.emitter=exactEmitter;
        committed.occurrence=occurrence;committed.generation=generation_;
        committed.index128=*position.index128;committed.before128=reachedBefore128;
        committed.after128=reachedAfter128;committed.pc=0x160994;committed.stored=true;
        //All admission precedes the original SW. These fixed-size copies
        //cannot allocate or fail. Never roll this count back on later refusal
        //or native cancellation, and never clear the source48 identity.
        sourceActiveCount128_=reachedAfter128;
        sourceFogCountPublication_=committed;h=committed;
        return true; //Before160998: no lazy guard/clock/RNG or helper return.
    }
    bool sourceFogSuccessorCountReceiptCurrentNative(const SourceFogCount160994& h,
        const void* exactEmitter,const void* occurrence)const noexcept {
        if(!sourceFogSuccessorCountHistoryNative(h)||!exactEmitter||!occurrence||
           exactEmitter!=sourceFogSuccessorEmitter_||occurrence!=sourceFogSuccessorOccurrence_||
           !sourceActiveCount128Known_||!sourceActiveCountAuthorityNative())return false;
        const auto& actual=sourceFogSuccessorCountPublication_;
        if(h.stored!=actual.stored)return false;
        return !h.stored||sameSourceFogCountNative(h,actual);
    }
    bool storeSourceFogSuccessorCount160994(const SourceFogRetirement160570& scan,
        const SourceFogPosition1606FC& position,const SourceFogFields1608AC& fields,const SourceFogTail1608E0& tail,
        const void* exactEmitter,const void* occurrence,const OriginalTrackParticleCold50LA* reachedManager70,
        std::int32_t reachedBefore128,std::int32_t reachedAfter128,SourceFogCount160994& h)noexcept {
        if(!sourceFogSuccessorCountHistoryNative(h)||h.pc||h.stored)return false;
        h.pc=0x160994;
        if(!exactEmitter||!occurrence||scan.readerEmitter!=exactEmitter||scan.readerOccurrence!=occurrence||
           reachedManager70!=this||reachedBefore128!=1||reachedAfter128!=2||sourceFogSuccessorCountPublication_.stored||
           !sourceFogSuccessorCountReceiptCurrentNative(h,exactEmitter,occurrence)||sourceActiveCount128_!=1||
           !sourceFogSuccessorCountObservationCurrentNative(reachedBefore128,scan,exactEmitter,occurrence)||
           tail.completedOps!=7||tail.pc!=tail.pcs[6]||
           !sourceFogSuccessorTailCurrentNative(scan,position,fields,tail,exactEmitter))return false;
        SourceFogCount160994 committed;
        committed.manager=this;committed.pool=position.pool;committed.record=position.record;
        committed.publication138=position.publication138;committed.emitter=exactEmitter;
        committed.occurrence=occurrence;committed.generation=generation_;
        committed.index128=1;committed.before128=reachedBefore128;committed.after128=reachedAfter128;
        committed.pc=0x160994;committed.stored=true;
        //Original SW and fixed-size custody publication cannot fail. Keep
        //the original0->1 event, both source48 lanes, and all earlier effects.
        sourceActiveCount128_=reachedAfter128;sourceFogSuccessorCountPublication_=committed;h=committed;
        return true; //Before160998; no lazy guard, clock, extra RNG or return.
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
        //160BB4 is SW zero,+128: an active signed count, never a pointer.
        sourceActiveCount128_=0;sourceActiveCount128Known_=true;field12C_=0;capacity130_=0;
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
    const void *sourceAtlas110_=nullptr,*sourceParticlePointers138_=nullptr,
        *sourceGroups13C_=nullptr;
    //160BEC/160C14 zero stores;161870/161888 pass these SAME integer lanes
    //to158B30. They are signed counts, not constructor-NULL pointer owners.
    std::int32_t sourceNumFrames154_=0,sourceNumGroups140_=0;
    std::int32_t sourceActiveCount128_=0;
    bool sourceActiveCount128Known_=false; //Actual160BB4, not legacy particleCount().
    SourceFogCount160994 sourceFogCountPublication_; //First irrevocable source0->1 event.
    SourceFogCount160994 sourceFogSuccessorCountPublication_; //Separate source1->2 event, never epoch0 replay.
    std::optional<std::uint32_t> sourceFogSuccessorLifeRead30_; //Actual issued slot1 read, not an inferred value.
    SourceFogRetirement160570 sourceFogRetirementObservation_; //One distinct read-only successor scan.
    SourceFogPosition1606FC sourceFogSuccessorPosition_; //Separate one-shot inactive slot1 write custody.
    SourceFogVelocity16088C sourceFogSuccessorVelocity_; //Issued SQ receipt, even when identical to constructor VF0.
    const void* sourceFogSuccessorEmitter_=nullptr;
    const void* sourceFogSuccessorOccurrence_=nullptr; //Native identity only, never source48 or a live capability.
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
    std::vector<std::uint8_t> particlePsm_,particlePst_; // area E: retainAtlasMaterialSources
    bool atlasSourcesRetained_=false;
};
}
