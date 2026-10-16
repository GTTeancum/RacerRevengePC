#pragma once
// Inactive private selection and draw adapters.229AE0 selection is separate
// from23CA50 draw; only draw may acquire the actual display inspection pin.
// No runtime registration or public callback factory exists.
#include "ra_source_camera_ownership50la.h"
#include "native_racer_cables50hw_fixture.h"
#include "original_race_display_lifetime50la.h"
#include "original_native_collector_fifo50la.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace original_cable_selection50LA {
namespace g=rr::game;
using I=g::SceneNodeIndex49P;
using V=std::array<float,4>;
enum class Status : std::uint8_t {
    owner,phase,graph,arithmetic,allocationIncomplete,visibility,
    detachedField48Unsupported,sourceNull254,disabledEarlyReturn,completeAdmitted,
    callbackMaterialBlocked,callbackAllocation,callbackChildrenUnsupported,
    callbackRendererSubmissionRequired,callbackReturned,callbackEmissionBlocked,callbackNativeFifoRequired,
    callbackBarrier,callbackKindUnsupported,callbackSelectedLightingRequired,callbackMaterialOwnerRequired
};
enum class EmissionBlock : std::uint8_t {none,material,display,attributes,collectorFrameSceneAndFifo};
struct EmissionDiagnostic {
    EmissionBlock block=EmissionBlock::none;
    std::uint32_t sourcePC=0,batch=0,vertex=0;
    std::int32_t registration=-1,representative=-1;
    std::size_t maps=0;
};
struct Result {
    Status status=Status::owner;std::uint32_t sourcePC=0;
    bool sourceEffectsCommitted=false;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material{};
    EmissionDiagnostic emission{};
};
class Binding final {
    friend class ra_source_camera50LA::OwnershipBridge;
    using Bridge=ra_source_camera50LA::OwnershipBridge;
    using Session=original_camera_session50KX::Session;
    using Visibility=g::original_gc_gameplay_lighting_candidate::CameraVisibilityLease;
    // Stack-only lifetime: created INSIDE the canonical Bank visit and never
    // returned. The enclosing shared bridge pin keeps this camera/session and
    // its actual Application alive. No caller graph, matrix or ready flag.
    struct RenderBorrow {
        Session& session;
        native_racer50HW::Cables& cables;
        g::OriginalRacerEntityCold50LA& racer;
        g::OriginalApplicationEntities50LA& application;
        g::VehicleFrameState49W& frame;
        const g::VehicleFrameBindings49W& bindings;
        const g::VehicleFrameParameters49W& parameters;
        g::CoupledNoiseContext49M& noise;
        const Visibility& visibility;
        std::uint32_t callbackRacer;
    };
    static float number(std::uint32_t bits)noexcept {float x;std::memcpy(&x,&bits,4);return x;}
    struct Math {
        bool good=true;
        float keep(float x)noexcept{good=good&&std::isfinite(x);return x;}
        float add(float a,float b)noexcept{keep(a);keep(b);volatile float x=a+b;return keep(x);}
        float sub(float a,float b)noexcept{keep(a);keep(b);volatile float x=a-b;return keep(x);}
        float mul(float a,float b)noexcept{keep(a);keep(b);volatile float x=a*b;return keep(x);}
        float div(float a,float b)noexcept{keep(a);keep(b);volatile float x=a/b;return keep(x);}
        V normalize(V v)noexcept {
            const float n=add(add(mul(v[0],v[0]),mul(v[1],v[1])),mul(v[2],v[2]));
            if(n>0.f&&n<std::numeric_limits<float>::min()){good=false;return v;}
            float inverse=std::numeric_limits<float>::max();
            if(n>0.f){volatile float root=std::sqrt(n);inverse=div(1.f,root);}
            for(unsigned k=0;k<3;++k)v[k]=mul(v[k],inverse);
            v[3]=1.f;return v;
        }
        V scaled(const V& v,float scale)noexcept {
            return {{mul(scale,v[0]),mul(scale,v[1]),mul(scale,v[2]),1.f}}; //106790
        }
    };
    //28B9B0: rotate XYZ through current main190 world axes, no translation.
    // Only XYZ is consumed below; this native temporary W is not a claim
    // about preserved VU scratch W. Source endpoint/scaled stores set W=1.
    static V rotate(const g::BodyMatrix48G& m,const V& v,Math& f)noexcept {
        V out{{0.f,0.f,0.f,1.f}};
        for(unsigned k=0;k<3;++k)
            out[k]=f.add(f.add(f.mul(m.v[k],v[0]),f.mul(m.v[4+k],v[1])),f.mul(m.v[8+k],v[2]));
        return out;
    }
    static Result detached(RenderBorrow& b,unsigned index,const g::SceneGraph49P& graph,
        V& scratch40,const V& scratch50,V& tangent60,Math& f)noexcept {
        constexpr std::uint32_t calls[]={0x224bc0u,0x224e2cu,0x225098u,0x225304u};
        auto& globals=b.application.cableSelectionGlobals_;
        const float time=b.frame.bodies.secondary.time864;
        const float length=b.parameters.lifecycle.restLength854;
        const float phase=f.mul(number(globals[4].word),time);
        const float coordinate=index==3u?f.add(2.f,phase):f.sub(phase,index==2u?17.f:11.f);
        const float z=f.keep(-length);float first=0.f,second=0.f;
        if(!f.good||!g::evaluateGradientNoise49M(b.noise.noise,b.noise.sharedRandomState,coordinate,first))
            return {Status::arithmetic,calls[index]};
        const float y=f.mul(number(globals[3].word),first);
        const float secondPhase=f.mul(number(globals[4].word),b.frame.bodies.secondary.time864);
        const float secondCoordinate=f.add(index==3u?5.f:index==2u?2.f:7.f,secondPhase);
        if(!f.good||!g::evaluateGradientNoise49M(b.noise.noise,b.noise.sharedRandomState,secondCoordinate,second))
            return {Status::arithmetic,calls[index]+0x2cu};
        scratch40={{f.mul(number(globals[3].word),second),y,z,1.f}};
        if(!f.good)return {Status::arithmetic,calls[index]+0x44u};
        const auto main=b.bindings.visual.main190;
        if(!valid(main,graph)||!g::refreshSceneWorld49P(graph,main))return {Status::graph,calls[index]+0x64u};
        const auto& world=graph.nodes[static_cast<unsigned>(main)].world100;
        const auto displacement=rotate(world,scratch40,f);
        for(unsigned k=0;k<3;++k)scratch40[k]=f.add(scratch50[k],displacement[k]);
        scratch40[3]=1.f;
        tangent60=rotate(world,b.application.cableForward434660_,f);
        if(!f.good)return {Status::arithmetic,calls[index]+0xc0u};
        return {Status::completeAdmitted,0};
    }
    static bool valid(I node,const g::SceneGraph49P& graph)noexcept {
        return static_cast<std::int32_t>(node)>=0&&static_cast<std::uint32_t>(node)<graph.count;
    }
    static bool origin(const g::SceneGraph49P& graph,I node,V& out)noexcept {
        if(!valid(node,graph)||!g::refreshSceneWorld49P(graph,node))return false;
        const auto& m=graph.nodes[static_cast<unsigned>(node)].world100;
        for(unsigned k=0;k<3;++k)out[k]=m.v[12+k];
        return true; //28B1A0(relative=null) preserves destination W.
    }
    static Result perform(RenderBorrow& b)noexcept {
        // Source initialization precedes even the genuine null254 return.
        constexpr std::uint32_t initial[]={0x4392aa7fu,0x3f800000u,0u,0x40a00000u,0x40400000u};
        auto& globals=b.application.cableSelectionGlobals_;
        for(unsigned i=0;i<5;++i)if(!globals[i].guard){globals[i].word=initial[i];globals[i].guard=1;}
        if(!b.racer.cables254)return {Status::sourceNull254,0x225590u};
        auto* allocation=b.cables.selectionState_.allocation_.get();
        if(!allocation||b.racer.cables254!=allocation||allocation->racer_!=&b.racer||
           allocation->application_.get()!=&b.application||allocation->closed_||
           allocation->entity_.parent1C!=&b.racer.detached7F8||
           allocation->ownedStrips_!=allocation->count3C_||!allocation->count3C_||
           !allocation->positions58_||!allocation->positions5C_||!allocation->tangents60_||
           !allocation->tangents64_||!allocation->enabled6C_||!allocation->scale54_)
            return {Status::allocationIncomplete,0x2247c8u};
        auto& a=*allocation;
        if(std::fegetround()!=FE_TONEAREST)return {Status::arithmetic,0x2247d4u};
        const g::SceneGraph49P graph{b.frame.nodes,b.frame.sceneNodeCount};
        if(!g::validVehicleSceneCount49W(b.frame,b.bindings)||!g::validateSceneGraph49P(graph))
            return {Status::graph,0x2247ecu};
        V scratch40{{0.f,0.f,0.f,1.f}},scratch50=scratch40;
        if(!origin(graph,b.bindings.visual.main190,scratch40))return {Status::graph,0x2247ecu};
        Math f;const auto& camera=b.session.spatial_.position180;
        const float dx=f.sub(camera.v[0],scratch40[0]);
        const float dy=f.sub(camera.v[1],scratch40[1]);
        const float dz=f.sub(camera.v[2],scratch40[2]);
        const float distance=f.add(f.add(f.mul(dx,dx),f.mul(dy,dy)),f.mul(dz,dz));
        const auto flags=b.session.actual_.all8Manager.flags58;
        float threshold=number(0x4745c100u);
        if(!(flags&2u)||(flags&1u))threshold=number(0x481c4000u);
        else if((b.session.raceFlags1B0_&3u)>=2u)threshold=number(0x461c4000u);
        const auto state=b.frame.bodies.main.vehicleState40;
        if(!f.good)return {Status::arithmetic,0x224848u};
        if(state==3u||state==4u||!(distance<=threshold)){
            for(unsigned i=0;i<4;++i)a.setEnabled174370(i,0);
            return {Status::disabledEarlyReturn,0x2248f8u}; //No174360 or endpoint clearing.
        }
        V tangent60{{0.f,0.f,0.f,1.f}},tangent70=tangent60;
        if(b.racer.chariotModel1B4){
            const auto node=b.bindings.lifecycle.model1B4;
            if(!valid(node,graph)||!g::refreshSceneWorld49P(graph,node))return {Status::graph,0x224920u};
            const auto& model=graph.nodes[static_cast<unsigned>(node)].world100;
            for(unsigned k=0;k<3;++k){tangent60[k]=f.keep(-model.v[8+k]);
                tangent70[k]=f.add(model.v[4+k],tangent60[k]);}
            float yScale=number(globals[2].word); //224A78 nonzero-state path.
            if(state==0u){
                const float low=number(globals[2].word),high=number(globals[1].word),speedEnd=number(globals[0].word);
                yScale=0.f; //2249DC equal-denominator branch keeps f3=0.
                if(speedEnd!=0.f){
                    yScale=f.add(high,f.div(f.mul(f.sub(b.frame.bodies.main.cachedSpeed34,0.f),f.sub(low,high)),f.sub(speedEnd,0.f)));
                    if(low<=high){if(yScale<low)yScale=low;else if(high<yScale)yScale=high;}
                    else {if(yScale<high)yScale=high;else if(low<yScale)yScale=low;}
                }
            }
            tangent60[1]=f.mul(tangent60[1],yScale);tangent70[1]=f.mul(tangent70[1],yScale);
            tangent60=f.normalize(tangent60);tangent70=f.normalize(tangent70);
        }
        if(!f.good)return {Status::arithmetic,0x224a98u};
        constexpr std::uint32_t starts[]={0x224af0u,0x224d5cu,0x224fc8u,0x225234u};
        constexpr std::uint32_t detachedReads[]={0x224b94u,0x224e00u,0x22506cu,0x2252d8u};
        for(unsigned index=0;index<4;++index){
            const auto first=b.cables.selectionFirst_[index],second=b.cables.selectionSecond_[index];
            if(first==I::none||second==I::none)continue; //Genuine authored null pair only.
            if(!origin(graph,first,scratch40)||!origin(graph,second,scratch50))return {Status::graph,starts[index]};
            if(index<a.count3C_){
                a.positions5C_[index]=scratch50;
                const auto value=f.scaled(tangent70,*a.scale54_);
                if(!f.good)return {Status::arithmetic,starts[index]+0x80u};
                a.tangents64_[index]=value;
            }
            // Read the canonical scalar only on the actual matching-state branch.
            if(state==(index%2u?2u:1u)){
                const auto& field=b.frame.fields.cableMode48;
                if(!field.constructed_)return {Status::detachedField48Unsupported,detachedReads[index]};
                if(field.value_){const auto result=detached(b,index,graph,scratch40,scratch50,tangent60,f);
                    if(result.status!=Status::completeAdmitted)return result;}
            }
            if(index<a.count3C_){
                // Source third pair writes60 before58 on BOTH zero/nonzero48
                // routes (225168..22522C); the other three write58 first.
                if(index!=2u)a.positions58_[index]=scratch40;
                const auto value=f.scaled(tangent60,*a.scale54_);
                if(!f.good)return {Status::arithmetic,starts[index]+0x230u};
                a.tangents60_[index]=value;
                if(index==2u)a.positions58_[index]=scratch40;
            }
        }
        for(unsigned index=0;index<4;++index){
            const int value=state==1u?int(index%2u):state==2u?int(!(index%2u)):1;
            a.setEnabled174370(index,value);
        }
        // Borrow the authentic already-active lease; never nest camera leases
        // or manufacture a racer DrawLease for a source membership read.
        const auto* actual7F0=b.session.actual_.visibility.cableCallbackVisibility7F0(
            b.visibility,b.callbackRacer,b.frame,b.bindings,b.cables.selectionState_.membership50_);
        if(!actual7F0)return {Status::visibility,0x225584u};
        //174360: SAME scalar observed by existing visibility/reset owners,
        //then all four lanes of the reached live endpoint scratch, in order.
        b.cables.selectionState_.membership50_=actual7F0->object;
        b.cables.selectionState_.position40_=scratch40;
        return {Status::completeAdmitted,0x2255a4u};
    }
    // Concrete forest, not a caller-authored list. Resolve identity BEFORE
    // dereferencing a sibling pointer; unknown outside/model/effect owners stop.
    static bool forestIdentity(RenderBorrow& b,const g::OriginalWreckComponent50LA* node)noexcept {
        if(!node)return false;
        const auto& domain=b.racer.domain();
        for(std::size_t i=0;i<domain.count;++i)if(domain.objects[i]==node)return true;
        for(const auto* model:{b.racer.chariotModel1B4,b.racer.leftModel1BC,b.racer.rightModel1B8,
                              b.racer.bottomLeftModel1C4,b.racer.bottomRightModel1C0}){
            if(!model)continue;
            if(model->modelEntity8()==node)return true;
            for(std::uint32_t i=0;i<model->sourceRowCount();++i){
                const auto* row=model->sourceRow(i);if(row && &row->entity()==node)return true;
            }
        }
        if(b.racer.lateStrips_&&b.racer.lateStrips_->owns(node))return true;
        auto* a=b.cables.selectionState_.allocation_.get();
        if(!a||a->closed_||a->racer_!=&b.racer)return false;
        if(node==&a->entity_)return true;
        for(std::uint32_t i=0;i<a->ownedStrips_;++i)
            if(a->strips30_[i]&&node==&a->strips30_[i]->entity_)return true;
        return false;
    }
    enum class ForestSlot {selection40,draw48};
    static Result children(RenderBorrow& b,const g::OriginalWreckComponent50LA& parent,
        ForestSlot slot,Result result,std::size_t depth=0)noexcept {
        const bool select=slot==ForestSlot::selection40;
        const std::uint32_t mask=select?0x1000u:8u;
        const std::uint32_t entry=select?0x141c30u:0x141d70u;
        if(depth>=g::maxOriginalWreckComponents50LA){result.status=Status::graph;result.sourcePC=entry;return result;}
        if(parent.descendants24&mask){
            const g::OriginalWreckComponent50LA* previous=nullptr;
            auto* node=parent.firstChild18;std::size_t walked=0;
            while(node){
                if(++walked>g::maxOriginalWreckComponents50LA||!forestIdentity(b,node)||
                   node->parent1C!=&parent||node->previous10!=previous||
                   !node->reference00||node->sessionGeneration!=parent.sessionGeneration){
                    result.status=Status::graph;result.sourcePC=entry+0x28u;return result;
                }
                if((node->flags29&1u)&&!(node->flags29&8u)&&(node->mask20&mask)){
                    auto* display=b.session.displayInputs_;
                    if(!display){result.status=Status::callbackBarrier;result.sourcePC=entry+0x68u;return result;}
                    std::int32_t actualReturn=0;
                    const auto barrier=g::serviceOriginalRenderer244D60(display->rendererDispatch,false,
                        display->renderer,display->diagnostic,actualReturn);
                    if(barrier.status!=g::OriginalCameraStartStatus::complete){
                        result.status=Status::callbackBarrier;result.sourcePC=barrier.sourcePC;return result;
                    }
                    auto next=dispatchComponent(b,*node,slot,depth+1u);
                    next.sourceEffectsCommitted=next.sourceEffectsCommitted||result.sourceEffectsCommitted;
                    result=next;
                    if(result.status!=Status::callbackReturned)return result;
                }
                // Read next AFTER the real callback, matching the source.
                previous=node;node=node->next14;
            }
        }
        result.status=Status::callbackReturned;result.sourcePC=select?0x141cc8u:0x141e08u;return result;
    }
    static Result selectCables1743E0(RenderBorrow& b,std::size_t depth)noexcept {
        auto* a=b.cables.selectionState_.allocation_.get();
        if(!a||a->closed_||a->racer_!=&b.racer||b.racer.cables254!=a||
           a->application_.get()!=&b.application||a->ownedStrips_!=a->count3C_||
           !a->count3C_||a->count3C_>4u||a->samples38_!=9u||!a->enabled6C_||
           !a->positions58_||!a->positions5C_||!a->tangents60_||!a->tangents64_||
           !a->basis70_||!a->basis74_||!a->basis78_||!a->basis7C_||!a->vertices68_||
           std::fegetround()!=FE_TONEAREST||!b.session.viewport_.matricesReady||
           b.session.display_.rendererCurrentScene0!=&b.session.viewport_)
            return {Status::allocationIncomplete,0x1743e0u};
        //133730(row,2) from the actual selected scene's C0 view, no caller axis.
        const auto& view=b.session.matrices_.view;
        const V forward{{view[2],view[6],view[10],1.f}};
        Math f;Result result;
        for(unsigned i=0;i<3;++i)f.keep(forward[i]);
        if(!f.good)return {Status::arithmetic,0x174494u};
        auto sample=[&](std::uint32_t cable,std::uint32_t n)->V {
            V out{{0.f,0.f,0.f,1.f}};
            for(unsigned k=0;k<3;++k){
                const float p0=f.mul(a->basis70_[n],a->positions5C_[cable][k]);
                const float p1=f.mul(a->basis74_[n],a->positions58_[cable][k]);
                const float t0=f.mul(a->basis78_[n],a->tangents64_[cable][k]);
                const float t1=f.mul(a->basis7C_[n],a->tangents60_[cable][k]);
                out[k]=f.add(f.add(f.add(p0,p1),t0),t1);
            }return out;
        };
        for(std::uint32_t cable=0;cable<a->count3C_;++cable){
            if(!a->enabled6C_[cable])continue;
            V next=sample(cable,0),segment{{0.f,0.f,0.f,1.f}};
            for(std::uint32_t n=1;n<=a->samples38_;++n){
                const V current=next;
                if(n<a->samples38_){next=sample(cable,n);
                    for(unsigned k=0;k<3;++k)segment[k]=f.sub(current[k],next[k]);}
                // Last sample reuses the previous segment, including zero.
                V cross{{f.sub(f.mul(forward[1],segment[2]),f.mul(forward[2],segment[1])),
                    f.sub(f.mul(forward[2],segment[0]),f.mul(forward[0],segment[2])),
                    f.sub(f.mul(forward[0],segment[1]),f.mul(forward[1],segment[0])),1.f}};
                cross=f.normalize(cross);V edge=current;
                for(unsigned k=0;k<3;++k)edge[k]=f.add(current[k],f.mul(.25f,cross[k]));
                if(!f.good){result.status=Status::arithmetic;result.sourcePC=0x174894u;return result;}
                const auto first=2u*(cable*a->samples38_+n-1u);
                a->vertices68_[first]=current;a->vertices68_[first+1u]=edge;
                result.sourceEffectsCommitted=true;
            }
        }
        return children(b,a->entity_,ForestSlot::selection40,result,depth);
    }
    static Result selectModel26EFC0(RenderBorrow& b,const g::OriginalWreckComponent50LA& node)noexcept {
        for(const auto* owners:{b.session.actual_.actualRightModels,b.session.actual_.actualLeftModels,
            b.session.actual_.actualBottomRightModels,b.session.actual_.actualBottomLeftModels,
            b.session.actual_.actualChariotModels}){
            auto* model=owners?(*owners)[b.callbackRacer].get():nullptr;
            if(!model||model->scene_.modelEntity8()!=&node)continue;
            if(!model->finishedFor(b.racer)||model->bank_!=&b.session.actual_.bank||
               model->slot_!=b.callbackRacer||model->arena_!=&b.frame||
               model->application_.get()!=&b.application||!b.session.actual_.applicationMaterials||
               !model->materialReferences()||model->materialReferences()->table382B38()!=
                   b.session.actual_.applicationMaterials.get())return {Status::owner,0x2418f0u};
            g::original_gc_gameplay_lighting_candidate::Diagnostic d;
            const bool complete=b.session.actual_.visibility.selectRaModel26EFC0(b.visibility,b.application,
                model->scene_,model->graph(),nullptr,b.session.matrices_,b.session.viewport_,
                *b.session.actual_.applicationMaterials,d);
            return {complete?Status::callbackReturned:Status::callbackMaterialBlocked,
                    complete?0x26f054u:d.sourcePC,true};
        }
        return {Status::owner,0x2418f0u};
    }
    static Result dispatchComponent(RenderBorrow& b,const g::OriginalWreckComponent50LA& node,
        ForestSlot slot,std::size_t depth)noexcept {
        using Kind=g::OriginalWreckComponentKind50LA;
        if(node.kind==Kind::entity141300)return children(b,node,slot,{},depth);
        if(slot==ForestSlot::selection40&&node.kind==Kind::model2403A0)return selectModel26EFC0(b,node);
        if(slot==ForestSlot::selection40&&node.kind==Kind::scene2893A0){
            //3D3198 slot40=141C30 on the actual embedded Entity8.
            if(!forestIdentity(b,&node))return {Status::graph,0x141c30u};
            return children(b,node,slot,{},depth);
        }
        auto* a=b.cables.selectionState_.allocation_.get();
        if(a && &node==&a->entity_&&node.kind==Kind::podCables173CC0){
            if(slot==ForestSlot::selection40)return selectCables1743E0(b,depth);
            //174900 consumes14A0A0's three selected-light matrices FIRST.
            // No caller matrices, uninitialized stack matrices, or white fallback.
            return {Status::callbackSelectedLightingRequired,0x174948u};
        }
        if(b.racer.lateStrips_&&b.racer.lateStrips_->owns(&node)&&node.kind==Kind::strip23BCA0){
            if(slot==ForestSlot::selection40)return children(b,node,slot,{},depth);
            return {Status::callbackKindUnsupported,0x23ca50u}; //Late-strip draw remains unjoined.
        }
        if(a&&node.kind==Kind::strip23BCA0){
            for(std::uint32_t i=0;i<a->ownedStrips_;++i)if(&a->strips30_[i]->entity_==&node){
                if(slot==ForestSlot::selection40)return children(b,node,slot,{},depth);
                return performStripCallback(b,i);
            }
        }
        return {Status::callbackKindUnsupported,slot==ForestSlot::selection40?0x141ca8u:0x141de8u};
    }
    static Result inspectEmission(RenderBorrow& b,g::OriginalPodCableStripCold50LA& strip,
        Result result)noexcept {
        namespace a=rr::assets;
        result.status=Status::callbackEmissionBlocked;
        a::OriginalCableEmissionBinding50LA binding;
        a::OriginalStandardMaterialDiagnostic50LA diagnostic;
        if(!b.session.actual_.applicationMaterials||!strip.primitive30_||
           !strip.primitive30_->reference.inspectEmission24B9B0(*b.session.actual_.applicationMaterials,
               result.material.pass,binding,diagnostic)){
            result.emission={EmissionBlock::material,diagnostic.sourcePC};
            result.sourcePC=diagnostic.sourcePC;
            const bool effects=result.material.sourceEffectsCommitted;
            result.material=diagnostic;result.material.sourceEffectsCommitted|=effects;return result;
        }
        result.emission.registration=binding.origin_.registration;
        result.emission.representative=binding.representative_.registration;
        result.emission.maps=binding.mapCount_;
        auto* display=b.session.displayInputs_;
        if(!display||!binding.context_){
            result.emission.block=EmissionBlock::display;result.emission.sourcePC=0x24a798u;return result;
        }
        try {
            // The pin ties the canonical material's sealed Video to the SAME
            // actual session442F70 dispatch and host renderer. Selection never
            // calls this draw-only acquisition (229AE0 ->224720 is separate).
            auto pin=original_main_entry50KG::RaceDisplayLifetime50LA::retainCableInspection(
                display->renderer,display->rendererDispatch,binding.context_->videoIdentity());
            if(!pin){result.emission.block=EmissionBlock::display;result.emission.sourcePC=0x24a798u;return result;}
            const auto& attributes=strip.attributes_;const auto& primitive=*strip.primitive30_;
            const auto* common=attributes.common_.get();
            const auto* group=attributes.group_.get();
            if(primitive.attributes10!=&attributes||!common||!common->layout||
               *common->layout!=g::OriginalCableStripAttributes50LA::CommonData::Layout::cableFormat7||
               strip.format3C_!=7u||primitive.byte0C!=7u||!group||!group->batches||!attributes.vertices_||
               group->vertexCount!=primitive.vertices20||strip.count34_!=primitive.groups22||
               group->batchCount!=2u||group->vertexCount!=18u){
                result.emission.block=EmissionBlock::attributes;result.emission.sourcePC=0x24b9e4u;return result;
            }
            // Validate the real numeric streams without filling unknown lanes,
            // compacting source batch duplicates or changing strip controls.
            for(std::uint32_t batch=0;batch<group->batchCount;++batch){
                const auto& stream=group->batches[batch];
                result.emission.batch=batch;
                const std::uint32_t count=batch?4u:16u,offset=batch*16u;
                if(stream.count!=count||stream.positions!=attributes.vertices_->positions.data()+offset||
                   stream.triples!=attributes.vertices_->triples.data()+offset||
                   stream.texcoords!=attributes.vertices_->texcoords.data()+offset){
                    result.emission.block=EmissionBlock::attributes;result.emission.sourcePC=0x24b9e4u;return result;
                }
                for(std::uint32_t vertex=0;vertex<count;++vertex){
                    result.emission.vertex=vertex;
                    const auto& position=stream.positions[vertex];
                    bool known=position.stripControl==(vertex<2?-14:14);
                    for(const auto& lane:position.xyz)known=known&&lane.value&&std::isfinite(*lane.value);
                    for(const auto& lane:stream.triples[vertex].components)known=known&&lane.value&&std::isfinite(*lane.value);
                    for(const auto& lane:stream.texcoords[vertex].uv)known=known&&lane.value&&std::isfinite(*lane.value);
                    if(!known){result.emission.block=EmissionBlock::attributes;result.emission.sourcePC=0x24b9e4u;return result;}
                }
            }
            for(const auto* matrix:{&common->matrix40,&common->matrix90,&common->matrixE0})
                for(const auto& row:*matrix)for(float lane:row)if(!std::isfinite(lane)){
                    result.emission.block=EmissionBlock::attributes;result.emission.sourcePC=0x24b9e4u;return result;
                }
            for(float lane:common->vector130)if(!std::isfinite(lane)){
                result.emission.block=EmissionBlock::attributes;result.emission.sourcePC=0x24b9e4u;return result;
            }
            // All retained numeric/material/display inputs are inspectable.
            // The private FIFO owner exists, but actual ordered280118 zone
            // publication and callback/pass continuation are still unavailable.
            // Never map session frame/viewport to collector state or append here.
            result.status=Status::callbackNativeFifoRequired;
            result.sourcePC=0x24b9dcu;
            result.emission.block=EmissionBlock::collectorFrameSceneAndFifo;
            result.emission.sourcePC=0x29c150u;
            return result; // pin releases; no borrowed pointer escapes.
        }catch(...){
            result.emission.block=EmissionBlock::display;result.emission.sourcePC=0x24a798u;
            return result; // Prior common/dirty/animation effects remain recorded.
        }
    }
    static Result performStripCallback(RenderBorrow& b,std::uint32_t index)noexcept {
        auto* a=b.cables.selectionState_.allocation_.get();
        if(!a||b.racer.cables254!=a||a->racer_!=&b.racer||a->application_.get()!=&b.application||
           a->closed_||a->entity_.parent1C!=&b.racer.detached7F8||
           a->ownedStrips_!=a->count3C_||!a->strips30_||index>=a->ownedStrips_||!a->strips30_[index])
            return {Status::allocationIncomplete,0x23ca50u};
        auto& strip=*a->strips30_[index];
        if(strip.entity_.parent1C!=&a->entity_||
           strip.entity_.kind!=g::OriginalWreckComponentKind50LA::strip23BCA0)
            return {Status::graph,0x23ca50u};
        Result result;
        if(strip.flags48_&2u)return children(b,strip.entity_,ForestSlot::draw48,result);
        auto& attributes=strip.attributes_;
        // Complete150450 stores40=0 and constructs the native common owner.
        // Bare23BCA0 alone is insufficient. The authentic callback can also
        // recreate common50 after its genuine null test; preserve that order.
        if(!strip.field40_||!strip.primitive30_||strip.primitive30_->attributes10!=&attributes)
            return {Status::allocationIncomplete,0x23ca70u};
        if(!attributes.common_){
            try {attributes.common_=std::make_unique<g::OriginalCableStripAttributes50LA::CommonData>();}
            catch(...){return {Status::callbackAllocation,0x23cb34u};}
            result.sourceEffectsCommitted=true;
        }
        auto& common=*attributes.common_;
        common.layout=g::OriginalCableStripAttributes50LA::CommonData::Layout::cableFormat7;
        result.sourceEffectsCommitted=true;
        if(*strip.field40_==0u){
            V actualAmbient;
            if(!b.session.actual_.visibility.currentSourceGlobalAmbient50(actualAmbient)){
                result.status=Status::visibility;result.sourcePC=0x23caa8u;return result;
            }
            common.colorC0()=actualAmbient; // All four authentic lanes; no clamp.
            // Source zero40 branch does NOT clear the strip dirty bit.
        }else if(strip.flags48_&1u){
            if(!strip.color70_){result.status=Status::allocationIncomplete;result.sourcePC=0x23cac4u;return result;}
            common.colorC0()=*strip.color70_;
            strip.flags48_=static_cast<std::uint8_t>(strip.flags48_&~1u);
        }
        if(!b.session.actual_.applicationMaterials){
            result.status=Status::callbackMaterialBlocked;result.sourcePC=0x2467ccu;return result;
        }
        const auto stage=strip.primitive30_->reference.service23CA50(
            *b.session.actual_.applicationMaterials,result.material);
        result.sourceEffectsCommitted=result.sourceEffectsCommitted||result.material.sourceEffectsCommitted;
        result.sourcePC=result.material.sourcePC;
        using Stage=rr::assets::OriginalCableMaterialCallbackStage50LA;
        if(stage==Stage::blocked){result.status=Status::callbackMaterialBlocked;return result;}
        if(stage==Stage::rendererSubmissionRequired){
            result.status=Status::callbackRendererSubmissionRequired;
            return inspectEmission(b,strip,result);
        }
        return children(b,strip.entity_,ForestSlot::draw48,result);
    }
    static Result performRacer229AE0(RenderBorrow& b)noexcept {
        b.racer.selectionMarker38_=1; //229AF0 before shared lazy and224720.
        if(!b.application.racerSelectionGuard87F0_){
            b.application.racerSelectionGuard87F0_=1;
            b.application.racerSelectionAssert434820_=-1;
        }
        auto result=perform(b);result.sourceEffectsCommitted=true;
        if(result.status!=Status::completeAdmitted&&result.status!=Status::disabledEarlyReturn&&
           result.status!=Status::sourceNull254)return result;
        const auto state=b.frame.bodies.main.vehicleState40;
        const std::int32_t* handle=nullptr;
        if(state==0u){
            if((!b.racer.lateStrips_||!b.racer.lateStrips_->binderReturned_)){result.status=Status::callbackMaterialOwnerRequired;
                result.sourcePC=0x229b24u;return result;}
            handle=&b.racer.selectionMaterial244_;
        }else if(state==4u){
            if((!b.racer.lateStrips_||!b.racer.lateStrips_->ringReturned_)){result.status=Status::callbackMaterialOwnerRequired;
                result.sourcePC=0x229b5cu;return result;}
            handle=&b.racer.selectionMaterial26C_;
        }
        if(handle&&*handle!=-1){
            if(!b.session.actual_.applicationMaterials||
               b.racer.selectionMaterialTable_!=b.session.actual_.applicationMaterials){
                result.status=Status::callbackMaterialBlocked;result.sourcePC=0x298e60u;return result;}
            if(!b.session.actual_.applicationMaterials->selectRacerMaterial29BFF0(*handle,result.material)){
                result.status=Status::callbackMaterialBlocked;result.sourcePC=result.material.sourcePC;return result;}
        }
        // Material stage has actually returned (or the actual state has no
        // material branch). Detached7F8 slot40 precedes racer children141C30.
        auto next=dispatchComponent(b,b.racer.detached7F8,ForestSlot::selection40,0);
        next.sourceEffectsCommitted=true;
        if(next.status!=Status::callbackReturned)return next;
        return children(b,b.racer.racer,ForestSlot::selection40,next);
    }
    enum class Operation {selection,stripCallback,racerSelection};
    static Result run(std::shared_ptr<Bridge> bridge,std::size_t slot,
        native_racer50HW::Cables& cables,const Visibility& visibility)noexcept {
        //229AE0 ->224720 precedes29BFF0's pass virtual20 callbacks. Neither
        //is23CA50/24A620 draw dispatch; keep all emission/Video pins out here.
        return visit(std::move(bridge),slot,cables,visibility,Operation::selection,0);
    }
    // Still private and unregistered. A future actual traversal must call it
    // with this owner's existing view lease and the concrete child ordinal.
    // It never replays224720 merely to acquire a banked borrow.
    static Result runStripCallback(std::shared_ptr<Bridge> bridge,std::size_t slot,
        native_racer50HW::Cables& cables,const Visibility& visibility,std::uint32_t strip)noexcept {
        return visit(std::move(bridge),slot,cables,visibility,Operation::stripCallback,strip);
    }
    static bool selectFromRegistry(const std::shared_ptr<Bridge>& bridge,std::uint32_t slot,
        const Visibility& lease,g::original_gc_gameplay_lighting_candidate::Diagnostic& d)noexcept {
        const auto result=runRacerSelection(bridge,slot,lease);
        d.racer=slot;d.sourcePC=result.sourcePC;
        if(result.status!=Status::callbackReturned){
            d.status=g::original_gc_gameplay_lighting_candidate::Status::phase;return false;
        }
        return true;
    }
    static bool runZoneSelection(std::shared_ptr<Bridge> bridge,ra_source_camera50LA::Diagnostic& d)noexcept {
        if(!bridge)return false;
        const g::original_gc_gameplay_lighting_candidate::RaRacerSelectionDispatch50LA dispatch(
            bridge,&selectFromRegistry);
        return bridge->prepareZonePrefix114D00(dispatch,d);
    }
    static Result runRacerSelection(std::shared_ptr<Bridge> bridge,std::size_t slot,
        const Visibility& visibility)noexcept {
        if(!bridge||!bridge->camera_||!bridge->camera_->session||slot>=8u||
           !bridge->camera_->cablePins[slot]||!bridge->camera_->cables[slot]||
           bridge->camera_->session->actual_.actualCables!=&bridge->camera_->cables)return {};
        auto& cables=*bridge->camera_->cables[slot];
        return visit(std::move(bridge),slot,cables,visibility,Operation::racerSelection,0);
    }
    static Result visit(std::shared_ptr<Bridge> bridge,std::size_t slot,
        native_racer50HW::Cables& cables,const Visibility& visibility,
        Operation operation,std::uint32_t strip)noexcept {
        const auto entryPC=operation==Operation::selection?0x224720u:
            operation==Operation::racerSelection?0x229ae0u:0x23ca50u;
        if(!bridge||slot>=8u)return {};
        Bridge::StaticLightPublication publication;
        if(!bridge->readStaticLightPublication(publication)||!bridge->actual_||!bridge->gridArena_)
            return {Status::phase,entryPC};
        auto& s=*bridge->camera_->session;
        if(!s.actual_.applicationEntities||!s.actual_.actualRigs||
           !(*s.actual_.actualRigs)[slot]||cables.sourceRig_!=(*s.actual_.actualRigs)[slot].get()||
           &s.actual_.bank!=bridge->bankIdentity_||&s.actual_.arena!=&bridge->actual_->frame||
           &s.actual_.bindings!=&bridge->gridArena_->bindings||
           &s.actual_.visibility!=bridge->freshLighting_.get()||
           s.display_.rendererCurrentScene0!=&s.viewport_)return {};
        if(visibility.owner_!=&s.actual_.visibility||
           !visibility.racerVisibility7F0())return {Status::visibility,entryPC};
        auto* actualNoise=&s.actual_.arena.noiseOwner();
        Result result;
        try {
            s.actual_.bank.visit(slot,[&](){
                if(s.frameStage_!=Session::FrameStage::published||s.publicationCount_!=publication.publication||
                   s.generation_!=publication.generation||s.completedFrame_!=publication.frame){
                    result={Status::phase,entryPC};return;}
                auto& frame=s.actual_.arena;const auto& bindings=s.actual_.bindings;
                if((frame.phase!=g::VehicleFramePhase49W::Ready&&frame.phase!=g::VehicleFramePhase49W::PostGraphics)||
                   bindings.lifecycle.model1B4!=cables.sourceRig_->roots[native_racer50HW::Rig::chariot]||
                   bindings.lifecycle.left1BC!=cables.sourceRig_->roots[native_racer50HW::Rig::left]||
                   bindings.lifecycle.right1B8!=cables.sourceRig_->roots[native_racer50HW::Rig::right]||
                   bindings.visual.main190!=bindings.lifecycle.main190){result={Status::graph,entryPC};return;}
                auto& racer=s.racerEntities_[slot];
                if(!racer.prepared()||racer.actualCounter423F60()!=&s.actual_.applicationEntities->objectCount423F60()||
                   racer.actualLock423F58()!=&s.actual_.applicationEntities->attachmentLock423F58())return;
                if(&frame.noiseOwner()!=actualNoise)return;
                RenderBorrow borrow{s,cables,racer,*s.actual_.applicationEntities,frame,bindings,
                    bridge->gridArena_->parameters,*actualNoise,visibility,static_cast<std::uint32_t>(slot)};
                if(!s.actual_.visibility.cableCallbackVisibility7F0(visibility,
                    static_cast<std::uint32_t>(slot),frame,bindings,cables.selectionState_.membership50_)){
                    result={Status::visibility,entryPC};return;}
                if(operation==Operation::selection)result=perform(borrow);
                else if(operation==Operation::racerSelection)result=performRacer229AE0(borrow);
                else result=performStripCallback(borrow,strip);
            });
        }catch(...){result.status=Status::owner;result.sourcePC=entryPC;return result;}
        return result;
    }
};
}
