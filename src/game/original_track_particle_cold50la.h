#pragma once
#include "original_application_entities50la.h"
#include "original_gc_particle_manager50kg.h"
#include "../assets/original_standard_particle_material_reference50la.h"
#include <exception>
#include <memory>
#include <new>
#include <utility>

namespace rr::game {
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
class OriginalTrackParticleCold50LA final {
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
        prepared_=false;
        // Actual142654->104970 consumes this owner's single reference
        // before the concrete160C70 destructor, not inside1413E0 itself.
        if(entityConstructed_)--component_.reference00;
        particles_.reset();
        material150_.reset();
        if(entityConstructed_){
            --application_->objectCount423F60();
            //160DBC..DC4->1413E0->104910 performs the actual global store
            // after storage/material destruction. No second reference release.
        }
        closed_=true;return true;
    }
private:
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
