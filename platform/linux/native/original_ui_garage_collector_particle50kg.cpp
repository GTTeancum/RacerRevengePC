#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include <cstdio>
namespace {
using D=rr::assets::OriginalStandardMaterialDiagnostic50LA;
using S=rr::assets::OriginalStandardMaterialStatus50LA;
bool reject(D& d,S status,std::uint32_t pc,const char* field)noexcept {
    d.status=status;d.sourcePC=pc;d.field=field;return false;
}
}
namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::collectParticlePrimitive24B9B0(
    OriginalUiGarageSceneParticles50KG& pool,D& d)noexcept {
    d={};const auto fifo=issuer_?issuer_->nativeCollectorFifoOwner_:nullptr;
    if(!identityCurrent()||phase_!=Phase::thirdPass||sourcePC_!=0x162E50||
       !fifo||!page_||!page_->scene4C0_||page_->sceneBacking_.get()!=page_->scene4C0_||
       page_->scene4C0_->sceneParticles_.get()!=&pool||callback_!=&pool.entity||
       page_->scenePageChildren_[9]!=&pool.entity||!page_->grant_||
       !page_->grant_->garageScenePageChildCurrent(*page_,*page_->scene4C0_,9,pool.entity)||
       pool.drawPublication!=publication_||pool.drawSourcePC!=0x162E50)
        return reject(d,S::phase,0x162E50,"actual ParticleManager +48 source append cursor");
    return fifo->collectGarageParticle24B9B0(*this,pool,d);
}
}
namespace original_main_entry50KG {
bool NativeCollectorFifo50LA::collectGarageParticle24B9B0(
    rr::platform::OriginalUiGarageDrawContribution50KG& draw,
    rr::platform::OriginalUiGarageSceneParticles50KG& pool,D& d)noexcept {
    d={};if(!application_||!retainGarageRegistry(*application_,d))return false;
    auto self=draw.retainSelfForFifo();
    rr::platform::OriginalUiGarageSceneParticles50KG::PacketBorrow packet;
    if(!self||draw.issuer_!=application_||pool.nativeGarageFifoPins_||
       !pool.borrowPacket162E50(packet)||packet.publication!=draw.publication_||
       !packet.revision||!packet.particleCount||pool.leaf14COwner.get()!=this||
       pool.leaf14CRegistration<0||!table_->coldCollectorCurrent(pool.leaf14CReceipt)||
       table_->nativeCollectorEntry29C0B0(pool.leaf14CReceipt,
           static_cast<std::size_t>(pool.leaf14CRegistration))!=pool.field14C||
       !packet.material150||packet.material150->table382B38()!=table_.get())
        return reject(d,S::phase,0x162E50,"same retained particle cache/packet owner");
    rr::assets::OriginalStandardMaterialCollectorEntryView50LA origin;
    rr::assets::OriginalStandardMaterialPassView50LA pass;
    if(!table_->coldCollectorEntry(registry_,static_cast<std::size_t>(pool.leaf14CRegistration),origin)||
       !origin.material||origin.material!=pool.projectionMaterial||origin.pass!=0||
       origin.materialRow!=packet.material150->material00()||
       !origin.material->pass4C(0,pass)||!pass.parameterCount28||!pass.parameterMask24||
       *pass.parameterCount28||*pass.parameterMask24||
       (pass.parameters34&&!pass.parameters34->empty())){
        std::fprintf(stderr,"garage_particle_registration_rejected stage=append publication=%llu "
            "registration=%d material_row=%d entry_row=%d entry_pass=%d entry_present=%d "
            "same_material=%d count_present=%d count=%d mask_present=%d mask=%u parameters_present=%d\n",
            static_cast<unsigned long long>(draw.publication_),static_cast<int>(pool.leaf14CRegistration),
            static_cast<int>(packet.material150->material00()),static_cast<int>(origin.materialRow),
            static_cast<int>(origin.pass),origin.material?1:0,origin.material==pool.projectionMaterial?1:0,
            pass.parameterCount28?1:0,pass.parameterCount28?static_cast<int>(*pass.parameterCount28):-1,
            pass.parameterMask24?1:0,pass.parameterMask24?static_cast<unsigned>(*pass.parameterMask24):0u,
            pass.parameters34?1:0);
        return reject(d,S::collector,0x1628D8,"actual source first-pass registration");
    }
    //247484/249420 leaves parameters34 null for a zero-count pass. Preserve
    //that actual empty overlay, as the particle24B4E4 lowerer requires; do not
    //manufacture a parameter owner or substitute the redirected final leaf.
    //162E50 supplies a1=Material150, a2=&Primitive160, a3=t0=null.
    //29C150 retains a1 at record04;24B9E4 stores a2 at08. The scratch
    //packet remains owned by Material150 allocation10, not by a fake Surface.
    Record record;record.garageDraw=std::move(self);record.garagePublication=draw.publication_;
    record.garageParticles=&pool;record.particlePacketRevision=packet.revision;
    record.primitive04=packet.material150;record.attributes08=packet.primitive160;
    record.originParameters=pass.parameters34;record.vertexBase=nullptr;record.textureBase=nullptr;
    return appendGarage24B9B0(std::move(record),pool.leaf14CRegistration,d);
}
}
