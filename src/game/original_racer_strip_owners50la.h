#pragma once
#include "original_pod_cable_allocation50la.h"
#include "original_racer_model_scene50la.h"
#include <string>

namespace rr::game {
// Inactive native213A40/213E90 owner. No packet builder or renderer receipt.
class OriginalRacerStripOwners50LA final {
    friend class ::native_racer50HW::Cables;
    friend class ::original_cable_selection50LA::Binding;
    using C=OriginalWreckComponent50LA;
    using O=OriginalRacerSceneObject50LA;
    using Strip=OriginalPodCableStripCold50LA;
    using R=OriginalPodCableAllocationResult50LA;
    using S=OriginalPodCableAllocationStatus50LA;
    OriginalRacerEntityCold50LA* racer_=nullptr;
    std::shared_ptr<OriginalApplicationEntities50LA> app_;
    std::array<std::unique_ptr<Strip>,2> binders_;
    std::unique_ptr<Strip> ring_;
    std::array<const O*,4> anchors_{}; //230,234,238,23C; genuine lookup null is retained.
    std::uint32_t count248_=0;
    bool binderReturned_=false,ringReturned_=false,closed_=false;
    static std::uint64_t key(const void* p)noexcept{return std::uint64_t(reinterpret_cast<std::uintptr_t>(p));}
    static bool equalName(const std::string& a,const char* b)noexcept {
        std::size_t i=0;for(;i<a.size()&&b[i];++i){auto x=a[i],y=b[i];
            if(x>='A'&&x<='Z')x=char(x+32);if(y>='A'&&y<='Z')y=char(y+32);
            if(x!=y)return false;}return i==a.size()&&!b[i];
    }
    bool find28C590(const OriginalRacerModelSceneOwner50LA* model,
        const SceneGraph49P& graph,const char* name,const O*& found)noexcept {
        found=nullptr;if(!model||!model->sourceRowCount()||model->released()||!model->model())return false;
        // Actual rowsLoaded table includes genuine warm childless row0=model.
        // No fresh PSG re-load or synthetic child is needed for name lookup.
        const std::array<const OriginalRacerModelSceneOwner50LA*,5> owners{{racer_->rightModel1B8,
            racer_->leftModel1BC,racer_->bottomRightModel1C0,racer_->bottomLeftModel1C4,racer_->chariotModel1B4}};
        std::size_t remaining=0;for(const auto* owner:owners)if(owner)remaining+=owner->sourceRowCount()+1u;
        auto walk=[&](auto&& self,SceneNodeIndex49P index)->bool {
            if(index==SceneNodeIndex49P::none)return true;
            const auto n=static_cast<std::uint32_t>(index);
            if(!remaining--||!graph.nodes||n>=graph.count)return false;
            const O* object=nullptr;
            for(const auto* owner:owners)if(owner){
                if(owner->model()&&owner->model()->sceneIndex==index)object=owner->model();
                for(std::uint32_t j=0;!object&&j<owner->sourceRowCount();++j){
                    const auto* row=owner->sourceRow(j);if(row&&row->sceneIndex==index)object=row;}
                if(object)break;
            }
            if(!object||!object->constructed||object->released)return false;
            if(equalName(object->name38,name)){found=object;return true;}
            //28C5CC..28C6B8 searches188 before184, including nested calls.
            if(!self(self,graph.nodes[n].nextSibling188)||found)return found!=nullptr;
            return self(self,graph.nodes[n].firstChild184);
        };
        return walk(walk,model->model()->sceneIndex);
    }
    // Exact current detached forest identities; never trust a caller sibling list.
    OriginalWreckComponentDomain50LA detachedDomain(std::array<C*,12>& objects)const noexcept {
        std::size_t n=0;objects[n++]=&racer_->detached7F8;
        for(const auto* m:{racer_->rightModel1B8,racer_->leftModel1BC,racer_->bottomRightModel1C0,
                          racer_->bottomLeftModel1C4,racer_->chariotModel1B4})
            if(m&&m->modelEntity8())objects[n++]=const_cast<C*>(m->modelEntity8());
        for(const auto& s:binders_)if(s)objects[n++]=&s->entity_;
        if(ring_)objects[n++]=&ring_->entity_;
        return {racer_->racer.sessionGeneration,objects.data(),n};
    }
    bool owns(const C* entity)const noexcept {
        for(const auto& s:binders_)if(s&&entity==&s->entity_)return true;
        return ring_&&entity==&ring_->entity_;
    }
    R setup(std::unique_ptr<Strip>& target,const std::string& name,std::uint8_t format,
        std::uint32_t color,const std::string& directory,std::uint32_t& pc) {
        target.reset(new Strip);
        auto& s=*target;
        const auto c=initializeOriginalWreckComponent50LA(s.entity_,OriginalWreckComponentKind50LA::strip23BCA0,
            racer_->racer.sessionGeneration,key(&s.entity_),bool(racer_->racer.flags29&1u),app_->objectCount423F60());
        if(c.status!=OriginalWreckComponentStatus50LA::complete){target.reset();return {S::component,c.sourcePC};}
        s.flags48_=1; //150378/390 known low bits
        s.format3C_=format;s.count34_=1;
        pc=0x150498u;s.groups38_.reset(new std::uint32_t[1]);s.field40_=1; //real a9
        s.color44_=color;s.flags48_|=1;
        s.color70_=std::array<float,4>{{float(((color>>16)&255u)>>1),float(((color>>8)&255u)>>1),
                                      float((color&255u)>>1),64.f}}; //23D750
        s.groups38_[0]=4; //original382E90/382E98 integer words copied by1504EC
        pc=0x1504f0u;s.primitive30_.reset(new Strip::Primitive);
        rr::assets::OriginalStandardMaterialDiagnostic50LA d;
        if(!s.primitive30_->reference.acquireNamed299BE0(racer_->selectionMaterialTable_,directory,name,d))
            return {S::material,d.sourcePC};
        s.primitive30_->byte0C=format;
        s.attributes_.constructQuad23BEB0(format,pc);
        s.primitive30_->attributes10=&s.attributes_;s.primitive30_->flags0F&=std::uint8_t(~1u);
        s.primitive30_->vertices20=4;s.primitive30_->groups22=1;
        return {S::selectionAttachmentComplete,0x23c028u};
    }
    void vertices(Strip& strip,bool ghost)noexcept {
        auto& a=strip.attributes_;
        //213C48..213D2C /213F70..214050: all positions, then all triples.
        for(std::uint32_t i=0;i<4;++i)for(unsigned k=0;k<3;++k)
            a.vertices_->positions[i].xyz[k]={app_->cableForward434660_[k],0xffu};
        for(std::uint32_t i=0;i<4;++i)for(unsigned k=0;k<3;++k)
            a.vertices_->triples[i].components[k]={app_->binderTriple434650_[k],0xffu};
        a.setTexcoord23C800(0,0.f,0.f);a.setTexcoord23C800(1,0.f,1.f);
        a.setTexcoord23C800(2,1.f,0.f);a.setTexcoord23C800(3,1.f,1.f);
        if(ghost)for(std::uint32_t i=0;i<4;++i)
            a.vertices_->colors[i]={{255u,255u,255u,127u}}; //23C620, packed7FFFFFFF
    }
    R attach(Strip& strip,std::uint32_t sourcePC)noexcept {
        std::array<C*,12> objects{};const auto domain=detachedDomain(objects);
        const auto r=appendOriginalWreckComponent50LA(domain,racer_->detached7F8,strip.entity_,8u,
            app_->attachmentLock423F58());
        return {r.status==OriginalWreckComponentStatus50LA::complete?S::selectionAttachmentComplete:S::component,
                r.status==OriginalWreckComponentStatus50LA::complete?sourcePC:r.sourcePC};
    }
    R construct(OriginalRacerEntityCold50LA& racer,std::shared_ptr<OriginalApplicationEntities50LA> app,
        const SceneGraph49P& graph,std::uint32_t racer60,const std::string& directory)noexcept {
        if(racer_||closed_||racer.lateStrips_)return {S::alreadyStarted,0x213a40u};
        if(!racer.prepared()||!app||racer.cables254||!racer.color260_||!racer.selectionMaterialTable_||
           racer.actualCounter423F60()!=&app->objectCount423F60()||
           racer.actualLock423F58()!=&app->attachmentLock423F58()||app->attachmentLock423F58()||
           racer.detached7F8.parent1C||racer.detached7F8.reference00!=1u||!racer60||racer60>99u)
            return {S::owner,0x213a40u};
        racer_=&racer;app_=std::move(app);racer.lateStrips_=this;
        count248_=unsigned(bool(racer.leftModel1BC||racer.rightModel1B8))+
                  unsigned(bool(racer.bottomLeftModel1C4||racer.bottomRightModel1C0));
        if(count248_==1u){
            if(!find28C590(racer.leftModel1BC,graph,"binderleft1",anchors_[0])||
               !find28C590(racer.rightModel1B8,graph,"binderright1",anchors_[2]))return {S::owner,0x213ab8u};
        }else if(count248_==2u){
            if(!find28C590(racer.bottomLeftModel1C4,graph,"binderleft1",anchors_[0])||
               !find28C590(racer.leftModel1BC,graph,"binderleft2",anchors_[1])||
               !find28C590(racer.rightModel1B8,graph,"binderright1",anchors_[2])||
               !find28C590(racer.bottomRightModel1C0,graph,"binderright2",anchors_[3]))return {S::owner,0x213af0u};
        }
        std::uint32_t pc=0x213b64u;
        try {
            rr::assets::OriginalStandardMaterialDiagnostic50LA d;
            const bool ghost=racer.mode3C_!=0;
            for(std::uint32_t i=0;i<count248_;++i){
                auto result=setup(binders_[i],ghost?"binderGhost":"binder",ghost?15u:7u,*racer.color260_,directory,pc);
                if(result.status!=S::selectionAttachmentComplete)return result;
                if(!racer.selectionMaterialTable_->find298C80(ghost?"textures\\binderGhost":"textures\\binder",
                    racer.selectionMaterial244_,d))return {S::material,d.sourcePC}; //213BE0/213C28, no extra retain
                vertices(*binders_[i],ghost);
                result=attach(*binders_[i],0x213e58u);if(result.status!=S::selectionAttachmentComplete)return result;
            }
            binderReturned_=true; //Only the actual loop and attachments returned.
            const std::string name="concring"+std::string(racer60<10u?"0":"")+std::to_string(racer60);
            pc=0x213ed0u;auto result=setup(ring_,name,15u,*racer.color260_,directory,pc);
            if(result.status!=S::selectionAttachmentComplete)return result;
            if(!racer.selectionMaterialTable_->find298C80(directory+name,racer.selectionMaterial26C_,d))
                return {S::material,d.sourcePC}; //213F54 uses literal "%s%s%02d", no extra slash.
            vertices(*ring_,false); //Ring color stream remains genuinely unwritten.
            result=attach(*ring_,0x2140f4u);if(result.status!=S::selectionAttachmentComplete)return result;
            ringReturned_=true;return result;
        }catch(...){return {S::allocation,pc};}
    }
    R close()noexcept {
        if(closed_)return {S::closed,0};
        if(!racer_){closed_=true;return {S::closed,0};}
        if(!app_||racer_->lateStrips_!=this||racer_->cables254||app_->attachmentLock423F58())
            return {S::topology,0x2105d8u};
        // Validate the whole known chain and every owned strip before mutation.
        std::array<C*,12> objects{};const auto domain=detachedDomain(objects);C* previous=nullptr;
        std::size_t left=domain.count;
        for(auto* n=racer_->detached7F8.firstChild18;n;n=n->next14){
            if(!left--||!original_wreck_component_detail50LA::contains(domain,n)||
               n->parent1C!=&racer_->detached7F8||n->previous10!=previous)return {S::topology,0x142590u};
            previous=n;
        }
        for(const auto* s:{ring_.get(),binders_[1].get(),binders_[0].get()})if(s&&
           (s->entity_.firstChild18||s->entity_.reference00!=1u||
            (s->entity_.parent1C&&s->entity_.parent1C!=&racer_->detached7F8)))return {S::topology,0x23bcf0u};
        for(auto* slot:{&ring_,&binders_[1],&binders_[0]})if(*slot){
            auto& s=**slot;auto& e=s.entity_;
            if(e.parent1C){
                if(e.next14)return {S::topology,0x142590u}; //Never erase an unreleased later child.
                if(e.previous10)e.previous10->next14=nullptr;else racer_->detached7F8.firstChild18=nullptr;
            }else if(e.previous10||e.next14)return {S::topology,0x142590u};
            --e.reference00;s.attributes_.release23BCF0();s.groups38_.reset();s.primitive30_.reset();
            --app_->objectCount423F60();slot->reset();
        }
        racer_->lateStrips_=nullptr;racer_=nullptr;app_.reset();closed_=true;return {S::closed,0x23bd90u};
    }
public:
    ~OriginalRacerStripOwners50LA(){if(close().status!=S::closed)std::terminate();}
};
}
