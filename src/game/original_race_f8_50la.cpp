#include "original_race_f8_storage50la.h"
#include <exception>
#include <limits>
#include <utility>
#include <thread>
namespace rr::game {
OriginalRaceF850LA::Impl::Impl(std::shared_ptr<OriginalApplicationEntities50LA> a,std::uint64_t session)
    :application(std::move(a)) {
    if(!application||!application->application10()||!session||application->attachmentLock423F58()||
       application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return;
    const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&component));
    if(!application->entityAuthority_.canEnroll(component,key))return;
    const auto made=initializeOriginalWreckComponent50LA(component,OriginalWreckComponentKind50LA::entity141300,
        session,key,true,application->objectCount423F60()); //1983DC, actual1A9128 A1=1.
    if(made.status!=OriginalWreckComponentStatus50LA::complete)return;
    counted=true;if(!application->entityAuthority_.enroll(component))std::terminate();
    //3CE6A0 installation, embedded2DEF40[2], then2E02F0[2], then the known
    //four null polygon slots and low5 flag/transition stores. No heap work here.
    textures40.emplace();texts100.emplace();fields.emplace();constructed=true;
}
OriginalRaceF850LA::OriginalRaceF850LA(std::shared_ptr<OriginalApplicationEntities50LA> a,std::uint64_t session)
    :impl_(new Impl(std::move(a),session)){}
OriginalRaceF850LA::~OriginalRaceF850LA()noexcept {
    OriginalRaceF8Diagnostic50LA d;if(impl_&&!closeDetached(d))std::terminate();
}
bool OriginalRaceF850LA::constructed()const noexcept{return impl_&&impl_->constructed&&!impl_->closed;}
bool OriginalRaceF850LA::Impl::detached()const noexcept {
    return counted&&application&&application->application10()&&!application->attachmentLock423F58()&&
        component.reference00==1&&component.kind==OriginalWreckComponentKind50LA::entity141300&&
        component.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&component))&&component.sessionGeneration&&
        !component.parent1C&&!component.previous10&&!component.next14&&!component.firstChild18&&
        !component.descendants24&&component.mask20==mask&&component.category28==0xff&&
        (component.flags29&15u)==3u&&application->entityAuthority_.canRetire(component);
}
bool OriginalRaceF850LA::Impl::stop(D& d,std::uint32_t pc,const char* operation)noexcept {
    d.sourcePC=pc;d.operation=operation;d.retainedPrimitives=0;
    for(const auto& r:references)if(r.retained)++d.retainedPrimitives;
    return false;
}
bool OriginalRaceF850LA::loadedCold()const noexcept {
    return constructed()&&impl_->loadComplete&&impl_->initialVisibility;
}
bool OriginalRaceF850LA::initialVisibility1A9170()noexcept {
    if(!constructed()||!impl_->loadComplete||impl_->initialVisibility||!impl_->detached())return false;
    //Actual199770(1,1,1,0,1), AFTER1A9144 publication; final1 is
    //the1A9174 delay slot. Reuse the actual owner's complete state leaf.
    if(!visibility199770(1,1,1,0,1))return false;
    impl_->initialVisibility=true;return true;
}
bool OriginalRaceF850LA::belongsTo(const std::shared_ptr<OriginalApplicationEntities50LA>& a,
    std::uint64_t generation)const noexcept {
    return constructed()&&impl_->application==a&&impl_->component.sessionGeneration==generation;
}
OriginalWreckComponent50LA* OriginalRaceF850LA::entity()noexcept{return constructed()?&impl_->component:nullptr;}
bool OriginalRaceF850LA::attachedMask(std::uint32_t mask)noexcept {
    if(!loadedCold()||impl_->component.mask20!=mask)return false;impl_->mask=mask;return true;
}
void OriginalRaceF850LA::Impl::release(std::size_t i)noexcept {
    if(i>=references.size())std::terminate();auto& r=references[i];if(!r.retained)return;
    if(!domain||!domain->materials382B38()->release14D500(r.row))std::terminate();r.retained=false;
}
void OriginalRaceF850LA::Impl::closeText(Text& t)noexcept {
    t.string8.reset();t.glyphsC.reset(); //2DFC60 before primitive traversal.
    if(t.primitives4)for(std::size_t i=0;i<t.count0;++i)if(t.primitives4[i]){
        release(t.primitives4[i]->reference);t.primitives4[i].reset();
    }
    t.primitives4.reset();
    //2DFD24..60 frees the actual packet buffers ASCENDING before the array.
    //Do not let delete[] choose reverse TextPacket member destruction order.
    if(t.packets64)for(std::size_t i=0;i<t.packetCount5D;++i)t.packets64[i].vertices.reset();
    t.packets64.reset();t.font4C.reset();
}
void OriginalRaceF850LA::Impl::closeTexture(Texture& t)noexcept {
    //Resource suffix only. A bound F8 lease has performed this object's real
    //renderer20(1)/24 prelude; the unbound cold path certifies no such prelude.
    t.buffer58.reset(); //2DF038..48 precedes primitive/material destruction.
    if(t.primitive54){
        t.primitive54->packet10=nullptr; //Actual2DF058, after backing release.
        release(t.primitive54->reference);t.primitive54.reset(); //2DF070.
    }
    t.atlas=nullptr; //Native borrow invalidation only.
}
void OriginalRaceF850LA::Impl::closeResources()noexcept {
    //198548..56C: four source pointer slots ASCENDING, then2E87F0 text and
    //texture arrays descending. HOST partial cleanup visits only acquired data.
    //Unbound path only: no F8 submission exists here. This native known-acquired
    //cold disposal certifies neither2E1980 nor2DEFE0 renderer20(1)/24 prelude.
    //Bound resources use the lease's per-object resumable retirement instead.
    for(auto& p:polygons30)if(p){
        if(p->primitives30)for(std::size_t n=0;n<p->count34;++n)if(p->primitives30[n]){
            release(p->primitives30[n]->reference);p->primitives30[n].reset();
        }
        p->primitives30.reset();p->packets44.reset();p->vertices00.reset();p.reset();
    }
    if(texts100){for(std::size_t n=2;n;--n)closeText((*texts100)[n-1]);texts100.reset();}
    if(textures40){for(std::size_t n=2;n;--n)closeTexture((*textures40)[n-1]);textures40.reset();}
    nativeImageCache.reset();fonts.reset();sourceData.reset();context.reset();domain.reset();
}
bool OriginalRaceF850LA::nativeBarBorrowed()const noexcept{return impl_&&impl_->nativeBarBorrows!=0;}
bool OriginalRaceF850LA::closeDetached(OriginalRaceF8Diagnostic50LA& d)noexcept {
    d={};if(!impl_||impl_->closed)return true;auto& p=*impl_;
    if(p.application&&p.application->nativeOwnerThread_!=std::this_thread::get_id())
        return p.stop(d,0x198510,"F8 retirement requires its actual owner thread");
    if(p.nativeBarBorrows)return p.stop(d,0x198510,"F8 actual native Frame borrow remains live");
    if(!p.retirementStarted){
        if(p.counted&&!p.detached())return p.stop(d,0x198510,"F8 still attached or altered");
        if(p.counted){
            if(!p.application->objectCount423F60()||!p.application->entityAuthority_.retire(p.component))
                return p.stop(d,0x104970,"F8 actual Entity authority");
            --p.component.reference00;d.sourceEffectsCommitted=true;
        }
        p.retirementStarted=true;
    }else if(p.counted&&(!p.application||!p.application->application10()||
        p.application->attachmentLock423F58()||p.component.reference00||p.component.parent1C||
        p.component.previous10||p.component.next14||p.component.firstChild18||p.component.descendants24||
        !p.application->objectCount423F60()))
        return p.stop(d,0x198510,"F8 actual detached in-progress retirement");
    if(p.rendererLease){
        if(!retireRendererResources198510(d))return false;
    }else p.closeResources(); //Existing known-acquired CPU-only cold disposal.
    if(p.counted)--p.application->objectCount423F60();
    p.closed=true;return true;
}
}
