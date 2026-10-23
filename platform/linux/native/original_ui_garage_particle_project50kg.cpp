#include "vehicle_window_gl50ds.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <limits>

namespace {
using MD=rr::assets::OriginalStandardMaterialDiagnostic50LA;
using MS=rr::assets::OriginalStandardMaterialStatus50LA;
bool rejected(MD& d,MS status,std::uint32_t pc)noexcept {d.status=status;d.sourcePC=pc;return false;}
float add32(float a,float b)noexcept {volatile float v=a+b;return v;}
float sub32(float a,float b)noexcept {volatile float v=a-b;return v;}
float mul32(float a,float b)noexcept {volatile float v=a*b;return v;}
float div32(float a,float b)noexcept {volatile float v=a/b;return v;}
bool cvt(float f,std::int32_t& i)noexcept {
    return rr::game::convertOriginalUiApertureCvtWS50KG(f,i);
}
}
namespace original_main_entry50KG {
bool NativeCollectorFifo50LA::selectGarageParticleLeaf1628EC(
    rr::platform::OriginalUiGarageDrawContribution50KG& draw,
    rr::platform::OriginalUiGarageSceneParticles50KG& pool,MD& d)noexcept {
    d={};
    if(!application_||!retainGarageRegistry(*application_,d))return false;
    if(draw.issuer_!=application_||!draw.particleProjectionInvocation1628ECCurrent(pool)||
       !pool.material150||!pool.material150->retained()||
       pool.material150->table382B38()!=table_.get()||!pool.projectionMaterial||pool.field14C)
        return rejected(d,MS::phase,0x1628D4);
    const rr::assets::OriginalStandardMaterialObject50LA* material=nullptr;
    if(!table_->resolve298E60(pool.material150->material00(),material,d)||
       material!=pool.projectionMaterial||!material)
        return rejected(d,MS::reference,0x1628D4);
    const auto* registration=material->collector5C(0);
    rr::assets::OriginalStandardMaterialCollectorEntryView50LA entry;
    if(!registration||*registration<0||
       !table_->coldCollectorEntry(registry_,static_cast<std::size_t>(*registration),entry)||
       entry.material!=material||entry.materialRow!=pool.material150->material00()||entry.pass!=0){
        std::fprintf(stderr,"garage_particle_registration_rejected stage=select publication=%llu "
            "registration_present=%d registration=%d material_row=%d entry_row=%d entry_pass=%d "
            "entry_present=%d same_material=%d\n",
            static_cast<unsigned long long>(draw.publication_),registration?1:0,
            registration?static_cast<int>(*registration):-1,static_cast<int>(pool.material150->material00()),
            static_cast<int>(entry.materialRow),static_cast<int>(entry.pass),entry.material?1:0,
            entry.material==material?1:0);
        return rejected(d,MS::collector,0x1628D8);
    }
    const auto* actual=table_->nativeCollectorEntry29C0B0(registry_,static_cast<std::size_t>(*registration));
    if(!actual)return rejected(d,MS::collector,0x1628E8);
    // This is the same live native registry allocation used by model1C8 and
    // queued24B9B0. It is NOT a canonical row/parameter pointer or new FIFO.
    pool.leaf14COwner=application_->nativeCollectorFifoOwner_;
    pool.leaf14CReceipt=registry_;pool.leaf14CRegistration=*registration;
    pool.field14C=actual;d.sourcePC=0x1628EC;d.sourceEffectsCommitted=true;
    return true;
}
}
namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::particleProjectionInvocation1628ECCurrent(
    const OriginalUiGarageSceneParticles50KG& pool)const noexcept {
    return identityCurrent()&&phase_==Phase::firstPass&&sourcePC_==0x1628EC&&
        page_&&page_->scene4C0_&&page_->sceneBacking_.get()==page_->scene4C0_&&
        page_->scene4C0_->sceneParticles_.get()==&pool&&callback_==&pool.entity&&
        page_->scenePageChildren_[9]==&pool.entity&&pool.projectionPublication==publication_&&
        pool.projectionSourcePC==0x1628EC&&publication_!=0;
}
bool OriginalUiGarageDrawContribution50KG::particleProject162870(C& node,Diagnostic& d)noexcept {
    if(!identityCurrent()||phase_!=Phase::firstPass||callback_!=&node||!page_||
       page_->scenePageChildren_[9]!=&node||!page_->grant_||!page_->scene4C0_||
       page_->sceneBacking_.get()!=page_->scene4C0_)return fail(d,0x162870);
    auto& scene=*page_->scene4C0_;
    if(!scene.sceneParticles_||scene.grant_!=page_->grant_||
       !page_->grant_->garageScenePageChildCurrent(*page_,scene,9,node))return fail(d,0x162870);
    auto& pool=*scene.sceneParticles_;if(pool.nativeGarageFifoPins_)return fail(d,0x162870);
    const auto* manager=pool.numeric?pool.numeric->manager():nullptr;
    const auto app=issuer_->entities();
    if(&node!=&pool.entity||node.parent1C!=&page_->entity_||!node.reference00||
       !pool.entityLive||!pool.prepared||pool.retiring||pool.retired||!manager||
       !pool.numeric->managerPrepared160E00()||!pool.updateReturned||pool.updateFailed||
       pool.updatePublication!=publication_||!publication_||!app||page_->authority_!=app||
       page_->grant_->application()!=app)return fail(d,0x162870);
    if(pool.projectionPublication!=publication_){
        if(pool.projectionPublication&&pool.projectionSourcePC!=0x162D1C&&pool.projectionSourcePC!=0x162D20)
            return fail(d,pool.projectionTerminalPC?pool.projectionTerminalPC:pool.projectionSourcePC);
        // Release only native cache lifetime. The real162880 store still
        // occurs in the existing Page-owned exact empty/positive prefix.
        pool.leaf14COwner.reset();pool.leaf14CReceipt={};pool.leaf14CRegistration=-1;
        pool.projectionMaterial=nullptr;pool.projectionTerminalPC=0;
        pool.projectionRow=0;pool.projectionCount=manager->particleCount();
        std::uint32_t pc=0;
        if(page_->renderEmptyPool162870(node,publication_,pc)){sourcePC_=pc;d.sourcePC=pc;return true;}
        if(pc!=0x162898)return fail(d,pc);
    }
    if(pool.projectionTerminalPC)return fail(d,pool.projectionTerminalPC);
    if(pool.projectionSourcePC==0x162D1C||pool.projectionSourcePC==0x162D20){
        sourcePC_=pool.projectionSourcePC;d.sourcePC=sourcePC_;return true;
    }
    if(!pool.projectionCount||pool.projectionCount>game::originalGcParticleCapacity50KG||
       manager->particleCount()!=pool.projectionCount)return fail(d,0x162884);
    const auto terminal=[&](std::uint32_t pc)noexcept {
        pool.projectionTerminalPC=pc;pool.projectionSourcePC=sourcePC_=pc;return fail(d,pc);
    };
    if(pool.projectionSourcePC==0x162898){
        if(!pool.material150)return terminal(0x1628B4);
        MD md;
        if(!pool.material150->selectGarage1628CC(pool.projectionMaterial,md))
            return terminal(md.sourcePC?md.sourcePC:0x1628CC);
        pool.projectionSourcePC=0x1628D4;
    }
    if(pool.projectionSourcePC==0x1628D4||pool.projectionSourcePC==0x1628EC){
        pool.projectionSourcePC=sourcePC_=0x1628EC;MD md;
        if(!issuer_->nativeCollectorFifoOwner_||
           !issuer_->nativeCollectorFifoOwner_->selectGarageParticleLeaf1628EC(*this,pool,md))
            return fail(d,md.sourcePC?md.sourcePC:0x1628DC);
        pool.projectionSourcePC=0x16293C;
    }
    if(pool.projectionSourcePC==0x16293C){
        OriginalUiGarageCameraFrame50KG::Fields camera;std::uint32_t pc=0;
        if(!cameraFrame_.read(camera,pc))return fail(d,pc?pc:0x162914);
        pool.projectionCamera250=camera.combinedScreen250;
        pool.field12C=0; //16293C, before conversion and scale arithmetic.
        const auto depth=app->particleDepth382BD8();
        if(std::fegetround()!=FE_TONEAREST||depth<=0||depth!=pool.depthCount118||
           pool.buckets11C.size()!=static_cast<std::size_t>(depth))
            return terminal(0x162944);
        const auto maximumDepth=std::bit_cast<float>(0x4B7FFF28u);
        pool.depthScale=div32(static_cast<float>(depth),maximumDepth);
        pool.inverseDepthScale=div32(1.f,pool.depthScale);
        if(!std::isfinite(pool.depthScale)||pool.depthScale<=0||!std::isfinite(pool.inverseDepthScale))
            return terminal(0x162968);
        pool.minimum120=std::numeric_limits<std::int32_t>::max();pool.maximum124=0;
        ++pool.epoch114; //Exact uint32 wrap; bucket compare162A4C is SLTU.
        pool.projectionSourcePC=0x16299C;
    }
    if(pool.projectionSourcePC==0x16299C){
        const auto& matrix=pool.projectionCamera250;
        const auto maximumDepth=std::bit_cast<float>(0x4B7FFF28u);
        while(pool.projectionRow<pool.projectionCount){
            const auto* actual=manager->particleAt(pool.projectionRow);
            if(!actual)return terminal(0x1629B8);
            //1629C8/CC multiply separately; ADDA then MADD and translation
            //retain source ordering instead of a reassociated host dot product.
            const auto xy=add32(mul32(actual->position[0],matrix[2]),mul32(actual->position[1],matrix[6]));
            const auto depth=add32(matrix[14],add32(xy,mul32(actual->position[2],matrix[10])));
            if(!std::isfinite(depth))return terminal(0x1629D8);
            if(depth<maximumDepth){
                std::int32_t integral=0,bucket=0,quantum=0;
                if(!cvt(depth,integral)||
                   !cvt(sub32(static_cast<float>(app->particleDepth382BD8()),
                       mul32(static_cast<float>(integral),pool.depthScale)),bucket)||
                   !cvt(mul32(depth,pool.depthScale),quantum))return terminal(0x162A18);
                const auto quantized=mul32(pool.inverseDepthScale,static_cast<float>(quantum));
                std::size_t slot=0;
                if(!manager->publishGarageProjection162A24(pool.projectionRow,actual,quantized,slot)||
                   slot>=pool.next20.size())return terminal(0x162A24);
                //Source24 write precedes upper bucket rejection, including
                //particles that are not added to the actual visible chain.
                if(bucket<app->particleDepth382BD8()){
                    if(bucket<0||static_cast<std::size_t>(bucket)>=pool.buckets11C.size()||
                       !pool.minimum120||!pool.maximum124)return terminal(0x162A38);
                    auto& row=pool.buckets11C[static_cast<std::size_t>(bucket)];
                    if(row.epoch00<pool.epoch114){
                        row.head04=actual;row.epoch00=pool.epoch114;pool.next20[slot]=nullptr;
                    }else{pool.next20[slot]=row.head04;row.head04=actual;}
                    pool.next20Written[slot]=true;
                    ++pool.field12C;
                    if(bucket<*pool.minimum120)pool.minimum120=bucket;
                    if(*pool.maximum124<bucket)pool.maximum124=bucket;
                }
            }
            ++pool.projectionRow;
        }
        pool.projectionSourcePC=0x162AD8;
    }
    if(pool.projectionSourcePC==0x162AD8){
        if(!pool.field12C){
            pool.projectionSourcePC=sourcePC_=0x162D20;d.sourcePC=0x162D20;return true;
        }
        pool.projectionSourcePC=0x162AEC;
    }
    if(pool.projectionSourcePC==0x162AEC||pool.projectionSourcePC==0x14949C||
       pool.projectionSourcePC==0x162CD8)return lightParticleCorners149310(pool,d);
    sourcePC_=pool.projectionSourcePC;return fail(d,sourcePC_);
}
}
