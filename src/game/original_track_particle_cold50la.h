#pragma once
#include "original_application_entities50la.h"
#include "original_gc_particle_manager50kg.h"
#include "../assets/original_standard_particle_material_reference50la.h"
#include <exception>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <optional>
#include <memory>
#include <new>
#include <utility>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__)
#error "Track particle bounded numeric adaptation requires strict floating point"
#endif
namespace ra_pod_scene50LA {class Owner;}
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
            d.component=initializeOriginalWreckComponent50LA(component_,
                OriginalWreckComponentKind50LA::particleManager160B10,generation_,
                key(&component_),true,application_->objectCount423F60());
            if(d.component.status!=OriginalWreckComponentStatus50LA::complete)
                return fail(d,S::component,d.component.sourcePC);
            entityConstructed_=true;d.entityConstructed=true;
            // Source130 receives the caller's5000;118 is signed division
            // toward zero.160E78 writes that value into the ONE application
            // global. Do not roll it back when later atlas/material work fails.
            capacity130_=static_cast<std::int32_t>(originalGcParticleCapacity50KG);
            depthBucketCount118_=capacity130_/2;
            application_->particleDepth382BD8_=depthBucketCount118_;
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
        if(entityConstructed_){
            if(!application_||application_->attachmentLock423F58()||
               component_.kind!=OriginalWreckComponentKind50LA::particleManager160B10||
               component_.reference00!=1u||component_.sessionGeneration!=generation_||
               component_.objectKey!=key(&component_)||component_.parent1C||component_.previous10||
               component_.next14||component_.firstChild18||component_.mask20||component_.descendants24)
                return false;
        }
        if(!canRetireNativeResources160E00())return false;
        prepared_=false;resourcesComplete_=false;
        // Actual142654->104970 consumes this owner's single reference
        // before the concrete160C70 destructor, not inside1413E0 itself.
        if(entityConstructed_)--component_.reference00;
        // Withdraw the raw auxiliary borrow before the numeric domain dies;
        // retain its Reference metadata until source160D9C after buckets.
        if(auxiliary1C0_)auxiliary1C0_->numericChild=nullptr;
        particles_.reset();
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
        // The ONE existing enabled18 remains in the actual numeric owner.
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
    std::int32_t capacity130_=0,depthBucketCount118_=0;
    bool attempted_=false,entityConstructed_=false,depthGlobalWritten_=false;
    bool prepared_=false,closed_=false;
};
}
