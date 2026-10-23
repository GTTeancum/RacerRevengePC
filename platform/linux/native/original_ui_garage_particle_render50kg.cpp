#include "vehicle_window_gl50ds.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <limits>

namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::particleRender162D30(C& node,Diagnostic& d)noexcept {
    if(!identityCurrent()||phase_!=Phase::thirdPass||callback_!=&node||
       !page_||page_->scenePageChildren_[9]!=&node||!page_->grant_||
       !page_->scene4C0_||page_->sceneBacking_.get()!=page_->scene4C0_)
        return fail(d,0x162D30);
    auto& scene=*page_->scene4C0_;
    if(scene.grant_!=page_->grant_||!scene.sceneParticles_||
       !page_->grant_->garageScenePageChildCurrent(*page_,scene,9,node))
        return fail(d,0x162D30);
    auto& pool=*scene.sceneParticles_;
    const auto* manager=pool.numeric?pool.numeric->manager():nullptr;
    if(&node!=&pool.entity||node.parent1C!=&page_->entity_||!node.reference00||
       !pool.entityLive||!pool.prepared||pool.retiring||pool.retired||pool.nativeGarageFifoPins_||
       !manager||!pool.numeric->managerPrepared160E00()||
       !pool.updateReturned||pool.updateFailed||pool.updatePublication!=publication_||
       pool.projectionPublication!=publication_||
       (pool.projectionSourcePC!=0x162D1Cu&&pool.projectionSourcePC!=0x162D20u)||
       !publication_||pool.drawPublication==publication_)
        return fail(d,0x162D30);
    // The actual earlier162870 invocation is retained separately from this
    // third-pass invocation. A failed positive sort cannot impersonate empty.
    pool.drawPublication=publication_;pool.drawSourcePC=sourcePC_=0x162D40;
    //160E24 allocated this real auxiliary; do not take a fake null branch.
    if(!pool.auxiliary1C0||!pool.auxiliary1C0->reference00||
       !pool.auxiliary1C0->numericChild||
       pool.auxiliary1C0->numericChild!=manager->auxiliaryChild()||
       !pool.auxiliary1C0->numericChild->constructed)
        return fail(d,0x162D40);
    pool.drawSourcePC=sourcePC_=0x162D4C;
    if(pool.auxiliary1C0->numericChild->enabled18){
        pool.drawSourcePC=0x164710;return fail(d,0x164710);
    }
    pool.drawSourcePC=sourcePC_=0x162D64;
    const auto application=issuer_->entities();
    if(!application||page_->authority_!=application||page_->grant_->application()!=application)
        return fail(d,0x162D64);
    const auto returned=[&]()noexcept {
        pool.drawSourcePC=sourcePC_=0x162E68;d.sourcePC=0x162E68;return true;
    };
    // Source reads this real process word BEFORE the current particle count.
    if(!application->particleRenderEnabled382BC8_)return returned();
    pool.drawSourcePC=sourcePC_=0x162D78;
    const auto count=manager->particleCount();
    if(count>game::originalGcParticleCapacity50KG)return fail(d,0x162D78);
    if(!count)return returned(); //Actual128==0 branch. No packet/collector call.
    pool.drawSourcePC=sourcePC_=0x162D84;
    // This is source12C, not the old constructor-only outer used128 mirror.
    // A future populated162870 may genuinely produce a zero visible count.
    if(!pool.field12C)return returned();
    //Actual BSS globals433A08/10/18/20 and gp84F0/F4/F8/FC belong
    //to the SAME retained process authority. Reentry does not reinitialize them.
    const auto profile=[&](std::size_t index,std::uint32_t pc)noexcept {
        pool.drawSourcePC=sourcePC_=pc;
        if(!application->particleDrawProfileGuards84F0_[index]){
            application->particleDrawProfile433A08_[index]=-1;
            application->particleDrawProfileGuards84F0_[index]=1;
        }
    };
    profile(0,0x162D9C);profile(1,0x162DBC);
    if(pool.packetTerminalPC||pool.packetRevision_==std::numeric_limits<std::uint64_t>::max())
        return fail(d,pool.packetTerminalPC?pool.packetTerminalPC:0x162DE0);
    ++pool.packetRevision_; //Native lifetime revision BEFORE first target mutation.
    pool.drawSourcePC=sourcePC_=0x162010;
    if(!buildParticlePacket162010(pool,d))return false;
    profile(2,0x162DE8);
    pool.drawSourcePC=sourcePC_=0x162230;
    if(!buildParticleScratch162230(pool,d))return false;
    profile(3,0x162E14);
    pool.drawSourcePC=sourcePC_=0x162E44;
    assets::OriginalStandardMaterialDiagnostic50LA material;
    if(!pool.material150||!pool.material150->bindGarageScratch162E44(pool,pool.scratch15C.get(),material)){
        pool.packetTerminalPC=material.sourcePC?material.sourcePC:0x162E44;
        return fail(d,pool.packetTerminalPC);
    }
    pool.drawSourcePC=sourcePC_=0x162E50;
    if(!collectParticlePrimitive24B9B0(pool,material)){
        //The typed FIFO owns any already appended records. Never replay a
        //possibly post-target append or erase its real packet/pins here.
        pool.packetTerminalPC=material.sourcePC?material.sourcePC:0x24B9B0;
        return fail(d,pool.packetTerminalPC);
    }
    return returned();
}
}
