// Include once after the actual Application/Display definitions in the future
// Course lifecycle implementation. No public or legacy Course caller exists.
#include "original_ui_course_page_payload50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <exception>
#include <new>
namespace rr::platform {
OriginalUiCoursePagePayload50KG::~OriginalUiCoursePagePayload50KG()noexcept {
    if(pinned_||quadCount_||textCount_||apertures_[0]||apertures_[1])std::terminate();
}
bool OriginalUiCoursePagePayload50KG::stop(Diagnostic& d,std::uint32_t pc)const noexcept {
    d.sourcePC=pc;
    d.sourceEffectsCommitted=phase_!=Phase::empty&&phase_!=Phase::bound;
    return false;
}
bool OriginalUiCoursePagePayload50KG::displayCurrent()const noexcept {
    // Existing pinned identity check also permits retirement at pin capacity.
    // This method confers no MemoryCard construction or source authority.
    return thread_==std::this_thread::get_id()&&root_&&display_&&pinned_&&
        display_->application_==root_&&display_->memoryCardConstructorDrainCurrent50KG();
}
bool OriginalUiCoursePagePayload50KG::bindNativeDisplay(const Root& root,
    std::shared_ptr<Display> display,Diagnostic& d)noexcept {
    d={};
    if(thread_!=std::this_thread::get_id()||phase_!=Phase::empty||pinned_||
       !display||display->application_!=&root||!display->frontendOwnerCurrent50KG()||!display->canRetain())
        return stop(d,0x1D01B4);
    root_=&root;display_=std::move(display);display_->pin();pinned_=true;phase_=Phase::bound;
    return true;
}
void OriginalUiCoursePagePayload50KG::constructQuad(std::uint32_t offset)noexcept {
    if(quadCount_==quads_.size())std::terminate();
    auto& q=quads_[quadCount_];
    if(!q.initializeColdPrefix2DEF40(offset,false))std::terminate();
    q.coursePrefixOnly_=true; // No Garage authoredName/load role is inherited.
    ++quadCount_;
}
void OriginalUiCoursePagePayload50KG::constructText()noexcept {
    if(textCount_==texts_.size()||texts_[textCount_].value)std::terminate();
    auto& storage=texts_[textCount_];
    storage.value=::new(static_cast<void*>(storage.bytes.data())) Text;
    ++textCount_;
}
bool OriginalUiCoursePagePayload50KG::constructShared1D5EA0(Diagnostic& d)noexcept {
    d={};if(phase_!=Phase::bound||quadCount_||textCount_||!displayCurrent()||display_->retiring_)return stop(d,0x1D5EB4);
    vtable_=0x3D16C0;
    for(std::uint32_t i=0;i<4;++i)constructQuad(0x2A0+i*0x60); //1D5EE0.
    constructText(); //1D5EEC: actual2E02F0 at440.
    shared_.openSize274=224;shared_.time4B0=0;shared_.top27C=0;
    shared_.left288=0;shared_.left278=0;shared_.right290=640;shared_.right280=640;
    shared_.bottom294=448;shared_.savedDelta298=0;shared_.field420=0;shared_.field42C=3;
    shared_.field434=0;shared_.popup438=nullptr;shared_.input268=0;shared_.camera4BC=nullptr;
    phase_=Phase::shared;d.sourceEffectsCommitted=true;d.sourcePC=0x1D5F7C;return true;
}
bool OriginalUiCoursePagePayload50KG::constructDerived1D8D70(Diagnostic& d)noexcept {
    d={};if(phase_!=Phase::shared||quadCount_!=4||textCount_!=1||!displayCurrent()||display_->retiring_)return stop(d,0x1D8D84);
    vtable_=0x3D1450;
    for(std::uint32_t i=0;i<4;++i)constructQuad(0x4C0+i*0x60);
    constructQuad(0x640);
    for(std::uint32_t i=0;i<5;++i)constructQuad(0x6A0+i*0x60);
    for(std::uint32_t i=0;i<3;++i)constructQuad(0x880+i*0x60);
    for(std::uint32_t i=0;i<3;++i)constructQuad(0x9A0+i*0x60);
    //AC0,B30,BA0,C10,C80,CF0,D60, then DD0[8],1150[8].
    for(std::size_t i=0;i<23;++i)constructText();
    zero14D0_.fill(0);field14FC_=-3;
    phase_=Phase::derived;d.sourceEffectsCommitted=true;d.sourcePC=0x1D8F00;return true;
}
bool OriginalUiCoursePagePayload50KG::retireLastText(Diagnostic& d)noexcept {
    if(!textCount_||!displayCurrent())return stop(d,0x2E1340);
    auto& storage=texts_[textCount_-1];
    if(!storage.value||!storage.value->retire2E1340(d.text))return stop(d,d.text.sourcePC?d.text.sourcePC:0x2E1340);
    storage.value->~Text();storage.value=nullptr;--textCount_;return true;
}
bool OriginalUiCoursePagePayload50KG::retireLastQuad(Diagnostic& d)noexcept {
    if(!quadCount_||!displayCurrent())return stop(d,0x2DF018);
    auto& q=quads_[quadCount_-1];
    const auto index=quadCount_-1;
    if(index<4&&failedPromptRetainPrimitive_[index]){
        const auto* p=q.primitiveOwner_.get();
        if(phase_!=Phase::nativeAborting||!failedPromptRetainRows_[index]||!p||
           p!=failedPromptRetainPrimitive_[index]||p!=q.material54||!p->materialStored||
           p->material00!=*failedPromptRetainRows_[index]||p->material00<0||p->materialRetained||
           p->optional04||p->optional14||(p->flags0F&1u)||p->packet10||q.resource58||q.packetOwner_||
           q.nativeGarageFifoPins_||q.nativeMenuWarningPins_)return stop(d,0x246288);
        // No acquired material reference exists. Preserve material54 as
        // comparison-only history; do NOT issue246260/release14D500.
        q.primitiveOwner_.reset();q.domain_.reset();q.context_.reset();q.video_.reset();q.garage_.reset();
        q.constructed=false;q.stage_=Quad::Stage::retired;
        failedPromptRetainPrimitive_[index]=nullptr;failedPromptRetainRows_[index].reset();
        --quadCount_;d.sourcePC=0;return true;
    }
    if(index<4&&failedPromptRetainRows_[index])return stop(d,0x246288);
    if(!q.coursePrefixOnly_||!q.preflightRetirement2DEFE0(d.quad))return stop(d,0x2DEFE0);
    if(!q.retirementDrainPhase_){std::int32_t actualV0=0;
        const auto r=game::serviceOriginalRenderer244D60(display_->dispatch_,true,display_->renderer_,d.renderer,
            actualV0,game::OriginalCameraDrainPolicy::initializedContext);
        if(r.status!=game::OriginalCameraStartStatus::complete)return stop(d,r.sourcePC);
        q.retirementDrainPhase_=1;
    }
    if(q.retirementDrainPhase_==1){
        if(display_->dispatch_.pendingCallbackDC)return stop(d,0x242F78);
        if(display_->dispatch_.count54||display_->dispatch_.count60)return stop(d,0x245090);
        if(!display_->renderer_.finishInitializedContextWork(d.renderer)||!d.renderer.contextWorkCompleted)
            return stop(d,0x2DF030);
        q.retirementDrainPhase_=2;
    }
    if(!q.retireResources2DF038(d.quad))return stop(d,d.quad.sourcePC);
    --quadCount_;return true;
}
bool OriginalUiCoursePagePayload50KG::retireDerivedPayload(Diagnostic& d)noexcept {
    // Source2E87F0 visits array elements in reverse constructor order.
    while(textCount_>1)if(!retireLastText(d))return false;
    while(quadCount_>4)if(!retireLastQuad(d))return false;
    return true;
}
bool OriginalUiCoursePagePayload50KG::retireDerived20E990(Diagnostic& d)noexcept {
    d={};if(!displayCurrent()||(phase_!=Phase::derived&&phase_!=Phase::retiringDerived&&
        phase_!=Phase::beforeUnknownApertures&&phase_!=Phase::beforeBase))return stop(d,0x20E990);
    if(phase_==Phase::derived){vtable_=0x3D1450;phase_=Phase::retiringDerived;}
    if(phase_==Phase::retiringDerived){
        if(!retireDerivedPayload(d))return false;
        vtable_=0x3D16C0;phase_=Phase::beforeUnknownApertures;
    }
    if(phase_==Phase::beforeUnknownApertures){
        // Only actual1D66E0 publications can satisfy these source reads.
        while(retiredApertures_<2){
            if(!shared_.apertures424[retiredApertures_])return stop(d,0x20EACC);
            if(*shared_.apertures424[retiredApertures_]==nullptr){
                // Only an actually reached1D66E0 failed-allocation store can
                // establish NULL. Original20EAD0 skips that known-null slot.
                if(apertures_[retiredApertures_]||aperturePhases_[retiredApertures_]!=AperturePhase::failed)
                    return stop(d,0x20EACC);
                aperturePhases_[retiredApertures_]=AperturePhase::retired;
            }else if(!retireAperture2E1980(retiredApertures_,d))return false;
            ++retiredApertures_;
        }
        while(textCount_)if(!retireLastText(d))return false; //20EAF4 Text440.
        while(quadCount_)if(!retireLastQuad(d))return false; //20EB0C shared prompts.
        phase_=Phase::beforeBase;
    }
    // The containing counted Page still owns2A5CF0 and1413E0. No success or
    // source count drop follows solely from payload disposal.
    return stop(d,0x2A5CF0);
}
bool OriginalUiCoursePagePayload50KG::nativeAbortCold(Diagnostic& d)noexcept {
    d={};if(phase_==Phase::nativeRetired)return true;
    if(phase_==Phase::empty)return true;
    if(!displayCurrent())return stop(d,0x2DF018);
    phase_=Phase::nativeAborting;
    // Release only our genuinely constructed/published component backing.
    // Unknown424/428 slots are never read, deleted, or rewritten.
    if(!retireDerivedPayload(d))return false;
    for(std::size_t i=0;i<2;++i)if(apertures_[i]&&!retireAperture2E1980(i,d))return false;
    while(textCount_)if(!retireLastText(d))return false;
    while(quadCount_)if(!retireLastQuad(d))return false;
    display_->unpin();pinned_=false;display_.reset();root_=nullptr;
    apertureDomain_.reset();apertureContext_.reset();
    phase_=Phase::nativeRetired;return true;
}
}

namespace rr::platform {
bool OriginalUiCoursePagePayload50KG::apertureComponentCurrent(std::size_t ordinal)const noexcept {
    return ordinal<2&&phase_==Phase::derived&&displayCurrent()&&!display_->retiring_&&
        root_->standardMaterialDomainOwner_&&root_->standardMaterialDomainOwner_==apertureDomain_&&
        apertureDomain_->prepared()&&root_->materialContextOwner_==apertureContext_&&
        apertureContext_&&apertureContext_->live()&&root_->materialApplication&&
        apertureContext_->materialApplication()==root_->materialApplication&&
        apertureContext_->applicationAnimationClock().sameOwner(root_->animationClock())&&
        apertureDomain_->materials382B38()->screenApplicationMatches(*root_->materialApplication)&&
        (root_->materialDirectory36B970_=="ui\\textures\\"||root_->materialDirectory36B970_=="ui\\textures"||
         root_->materialDirectory36B970_=="UI\\Textures\\")&&
        apertures_[ordinal]&&shared_.apertures424[ordinal]&&
        *shared_.apertures424[ordinal]==apertures_[ordinal].get()&&
        (ordinal==0?aperturePhases_[1]==AperturePhase::absent:aperturePhases_[0]==AperturePhase::loaded);
}
bool OriginalUiCoursePagePayload50KG::loadApertureComponent1D66C0(std::size_t ordinal,Diagnostic& d)noexcept {
    d={};
    if(ordinal>=2||phase_!=Phase::derived||!displayCurrent()||display_->retiring_)
        return stop(d,0x1D66C0);
    if(aperturePhases_[ordinal]!=AperturePhase::absent)return stop(d,apertureStopPC_[ordinal]);
    if((ordinal==0&&aperturePhases_[1]!=AperturePhase::absent)||
       (ordinal==1&&aperturePhases_[0]!=AperturePhase::loaded)||shared_.apertures424[ordinal])
        return stop(d,0x1D66C0);
    apertureDomain_=root_->standardMaterialDomainOwner_;apertureContext_=root_->materialContextOwner_;
    // Native admission before actual allocation. No forced directory setter,
    // fallback/population, or synthesized predecessor-loader receipt.
    if(!apertureDomain_||!apertureDomain_->prepared()||!apertureContext_||!apertureContext_->live()||
       !root_->materialApplication||apertureContext_->materialApplication()!=root_->materialApplication||
       !apertureContext_->applicationAnimationClock().sameOwner(root_->animationClock())||
       !apertureDomain_->materials382B38()->screenApplicationMatches(*root_->materialApplication)||
       (root_->materialDirectory36B970_!="ui\\textures\\"&&root_->materialDirectory36B970_!="ui\\textures"&&
        root_->materialDirectory36B970_!="UI\\Textures\\"))
        return stop(d,0x1D66C0);
    aperturePhases_[ordinal]=AperturePhase::failed; //One attempt, including native allocation failure.
    try {apertures_[ordinal]=std::make_unique<Aperture>();}
    catch(...){
        //1D66C8 skips only the constructor on failed allocation;1D66E0
        //still publishes its genuine null v0 before1D66F8 dereferences it.
        shared_.apertures424[ordinal]=nullptr;
        apertureStopPC_[ordinal]=0x1D66F8;
        return stop(d,apertureStopPC_[ordinal]);
    }
    shared_.apertures424[ordinal]=apertures_[ordinal].get(); //Actual1D66E0 pointer publication.
    aperturePhases_[ordinal]=AperturePhase::published;apertureStopPC_[ordinal]=0x1D66F8;
    if(!loadAperture2E1AE0(ordinal,d)){
        aperturePhases_[ordinal]=AperturePhase::failed;apertureStopPC_[ordinal]=d.sourcePC;return false;
    }
    auto& p=*apertures_[ordinal];
    for(std::size_t i=0;i<4;++i){auto& vertex=p.vertices00[i];
        if(vertex.aliases10!=1||!vertex.rgba1C[0]||vertex.rgba1C[1]){
            aperturePhases_[ordinal]=AperturePhase::failed;apertureStopPC_[ordinal]=0x1D6750;
            return stop(d,0x1D6750);
        }
        vertex.rgba1C[0]->words[3]=63; //1D6758.
        vertex.rgba1C[0]->words[0]=vertex.rgba1C[0]->words[1]=vertex.rgba1C[0]->words[2]=0; //133F20.
    }
    aperturePhases_[ordinal]=AperturePhase::loaded;
    apertureStopPC_[ordinal]=ordinal?0x1D6804:0x1D66C0;
    d.sourceEffectsCommitted=true;d.sourcePC=apertureStopPC_[ordinal];return true;
}
bool OriginalUiCoursePagePayload50KG::loadAperture2E1AE0(std::size_t ordinal,Diagnostic& d)noexcept {
    if(!apertureComponentCurrent(ordinal)||aperturePhases_[ordinal]!=AperturePhase::published)
        return stop(d,0x2E1AE0);
    auto& p=*apertures_[ordinal];aperturePhases_[ordinal]=AperturePhase::loading;
    p.type0C=1;p.count04=p.count08=4; //2E1B04/20/28. vertices00 is constructor NULL.
    const auto firstUse=[](PolygonOwner::PolygonAssertion& s)noexcept {
        if(!s.guard.load(std::memory_order_acquire)){
            s.word.store(1,std::memory_order_relaxed);s.guard.store(1,std::memory_order_release);
        }
    };
    // SAME process source assertion storage, not per-Course substitutes.
    firstUse(PolygonOwner::polygonCount8F38());firstUse(PolygonOwner::polygonLimit8F3C());
    firstUse(PolygonOwner::polygonQuads8F44());
    try {
        apertureStopPC_[ordinal]=0x2E1E60;
        bool vertexAllocationFailed=false;
        try {p.vertices00=std::make_unique<Aperture::Vertex[]>(4);} //Actual typed2E2190 backing.
        catch(...){vertexAllocationFailed=true;p.vertices00.reset();}
        //2E8990 returns NULL on NULL input;2E1E88 publishes that result.
        // The following reached scalar stores do not depend on its success.
        p.owns38=1;p.dirty3C=1;p.count34=p.count08;
        p.count48=1;p.capacity4C=1;p.count34=1;
        apertureStopPC_[ordinal]=0x2E1F64;
        if(vertexAllocationFailed)return stop(d,0x2E1F64); //Before next allocation, not loader return.
        try {p.packets44=std::make_unique<Aperture::Descriptor[]>(1);}
        catch(...){
            p.packets44.reset(); //Actual NULL publication2E1F6C.
            return stop(d,0x2E1FC8); //Later aligned allocation/store through44 remains unjoined.
        }
        apertureStopPC_[ordinal]=0x2E1FC8;
        Quad::Qword* packet=nullptr;
        try {packet=static_cast<Quad::Qword*>(::operator new[](18*sizeof(Quad::Qword),std::align_val_t{64}));}
        catch(...){
            p.packets44[0].allocation00.reset(); //Actual NULL descriptor store2E1FE8.
            return stop(d,0x2E1FF8); //Before memset306478 on NULL; no headers/aliases invented.
        }
        p.packets44[0].allocation00.reset(packet);
        for(std::size_t i=0;i<18;++i)std::construct_at(packet+i);
        auto& descriptor=p.packets44[0];
        packet[1].words={0x11000000,0x03000000,0x02000200,0x6C0E8000};descriptor.vif04=packet+1;
        packet[2].words={0x8004,0x30064000,0x512,0};descriptor.gif08=packet+2;descriptor.tag0C=packet+3;
        descriptor.sourcePrimitive=12;descriptor.sourceVertices=4;
        std::size_t cursor=4;
        for(std::size_t i=0;i<4;++i){auto& vertex=p.vertices00[i];
            firstUse(PolygonOwner::polygonAliases8F64());
            if(vertex.aliases10>=2)return stop(d,0x2E2C68);
            const auto slot=vertex.aliases10;
            vertex.rgba1C[slot]=packet+cursor++;vertex.stq14[slot]=packet+cursor++;
            packet[cursor].words={0,0,0x3F800000,0};vertex.xyz24[slot]=packet+cursor++;
            ++vertex.aliases10;
        }
        packet[cursor].words={0x140000B0,0x10000000,0,0};
        packet[0].words={0x30000011,0,0,0};descriptor.dmaData4=packet+1;descriptor.pointersKnown=true;
        apertureStopPC_[ordinal]=0x2E2094;
        try {p.primitives30=std::make_unique<std::unique_ptr<Quad::Primitive>[]>(1);}
        catch(...){
            p.primitives30.reset(); //Actual NULL publication2E209C.
            return stop(d,0x2E20A8); //Before next Primitive allocation/store through30.
        }
        if(!Quad::loadCourseAperturePrimitive2E20F0(*this,ordinal,p.primitives30[0],d.quad))
            return stop(d,d.quad.sourcePC);
        p.primitives30[0]->packet10=packet; //2E2144, after real canonical material retain.
        d.sourceEffectsCommitted=true;d.sourcePC=0x2E2180;return true;
    }catch(...){return stop(d,apertureStopPC_[ordinal]);}
}
bool OriginalUiGarageQuad50KG::loadCourseAperturePrimitive2E20F0(
    OriginalUiCoursePagePayload50KG& owner,std::size_t ordinal,std::unique_ptr<Primitive>& p,Diagnostic& d)noexcept {
    d={};
    if(!owner.apertureComponentCurrent(ordinal)||
       owner.aperturePhases_[ordinal]!=OriginalUiCoursePagePayload50KG::AperturePhase::loading||
       !owner.apertures_[ordinal]->primitives30||&p!=&owner.apertures_[ordinal]->primitives30[0]||p){
        d.status=Status::input;d.sourcePC=0x2E20A8;return false;
    }
    try {
        try {p=std::make_unique<Primitive>();} //Actual2461E0 payload, published2E20D4.
        catch(...){
            p.reset(); //2E20B0 skips ctor,2E20D4 still publishes genuine NULL.
            d.status=Status::allocation;d.sourcePC=0x2E20E8;return false; //Before NULL vtable read.
        }
        p->domain=owner.apertureDomain_;p->context=owner.apertureContext_;
        std::int32_t row=-1;const auto table=p->domain->materials382B38();
        const bool found=table->find298C80("ui\\textures\\NoTexture",row,d.material);
        p->material00=row;p->materialStored=true;
        auto& guard=OriginalUiGarageScreenText50KG::guard8CD8();
        auto& assertion=OriginalUiGarageScreenText50KG::assertion443640();
        if(!guard.load(std::memory_order_acquire)){
            assertion.store(1,std::memory_order_relaxed);guard.store(1,std::memory_order_release);
        }
        if((assertion.load()&&row==-1)||!found){d.status=Status::material;d.sourcePC=0x299CF8;return false;}
        if(!table->retainScreen14D590(row,d.material)){
            // Preserve the real material00 store and assertion first-use
            // writes. This terminal receipt owns ONLY this failed Primitive,
            // not an acquired table reference or ordinary246260 completion.
            owner.failedRetainPrimitive_[ordinal]=p.get();
            owner.failedRetainRows_[ordinal]=row;
            d.status=Status::material;d.sourcePC=d.material.sourcePC;return false;
        }
        p->materialRetained=true;p->type0C=15;p->flags0F&=std::uint8_t(~1u);
        d.sourceEffectsCommitted=true;d.sourcePC=0x2E2144;return true;
    }catch(...){d.status=Status::allocation;d.sourcePC=p?0x299BE0:0x2E20A8;return false;}
}
bool OriginalUiCoursePagePayload50KG::drainPolygon(std::uint8_t& phase,Diagnostic& d)noexcept {
    if(!displayCurrent())return stop(d,0x2E19B4);
    if(!phase){std::int32_t returned=0;
        const auto r=game::serviceOriginalRenderer244D60(display_->dispatch_,true,display_->renderer_,d.renderer,
            returned,game::OriginalCameraDrainPolicy::initializedContext);
        if(r.status!=game::OriginalCameraStartStatus::complete)return stop(d,r.sourcePC);
        phase=1;
    }
    if(phase==1){
        if(display_->dispatch_.pendingCallbackDC)return stop(d,0x242F78);
        if(display_->dispatch_.count54||display_->dispatch_.count60)return stop(d,0x245090);
        if(!display_->renderer_.finishInitializedContextWork(d.renderer)||!d.renderer.contextWorkCompleted)
            return stop(d,0x2E19CC);
        phase=2;
    }
    return phase==2;
}
bool OriginalUiCoursePagePayload50KG::retireAperture2E1980(std::size_t ordinal,Diagnostic& d)noexcept {
    if(ordinal>=2||!displayCurrent()||
       (phase_!=Phase::beforeUnknownApertures&&phase_!=Phase::nativeAborting))return stop(d,0x2E1980);
    if(aperturePhases_[ordinal]==AperturePhase::retired)return true;
    if(!shared_.apertures424[ordinal]||!apertures_[ordinal]||
       *shared_.apertures424[ordinal]!=apertures_[ordinal].get())return stop(d,0x20EACC);
    auto& p=*apertures_[ordinal];
    if(p.nativeGarageFifoPins_||p.nativeMenuWarningPins_||
       (p.primitives30&&p.count34!=1)||(p.packets44&&p.count48!=1)||
       (p.vertices00&&(!p.owns38||p.count04!=4||p.count08!=4)))return stop(d,0x2E19D4);
    const bool failedRetain=failedRetainRows_[ordinal].has_value();
    if(failedRetain){
        // No source destructor can waive its positive stored reference. This
        // is solely native disposal of the exact unpublished failed owner.
        if(phase_!=Phase::nativeAborting||!p.primitives30||!p.primitives30[0]||
           !failedRetainPrimitive_[ordinal]||p.primitives30[0].get()!=failedRetainPrimitive_[ordinal])return stop(d,0x246288);
        const auto& primitive=*p.primitives30[0];
        if(!primitive.materialStored||primitive.materialRetained||
           primitive.material00!=*failedRetainRows_[ordinal]||primitive.material00<0||
           primitive.domain!=apertureDomain_||primitive.context!=apertureContext_||
           primitive.optional04||primitive.optional14||primitive.packet10||(primitive.flags0F&1u))
            return stop(d,0x246288);
    }else if(failedRetainPrimitive_[ordinal])return stop(d,0x246288);
    else if(p.primitives30&&!Quad::preflightPrimitive246260(p.primitives30[0],d.quad))
        return stop(d,d.quad.sourcePC);
    if(!drainPolygon(apertureDrains_[ordinal],d))return false;
    if(p.primitives30){
        if(failedRetain){
            // Never release14D500: no reference was acquired. Stored source
            // row remains untouched until this actual backing is disposed.
            p.primitives30[0].reset();failedRetainPrimitive_[ordinal]=nullptr;
            failedRetainRows_[ordinal].reset();
        }else Quad::releasePrimitive246260(p.primitives30[0]); //2E1A08.
        p.primitives30.reset(); //2E1A28, or native failed-prefix withdrawal.
    }
    if(p.packets44){p.packets44[0].allocation00.reset();p.packets44.reset();} //2E1A5C/80.
    if(p.owns38)p.vertices00.reset(); //2E1A9C, vertex destructor has no payload effect.
    apertures_[ordinal].reset();aperturePhases_[ordinal]=AperturePhase::retired;
    //Source424/428 remains its historical dangling pointer, never dereferenced.
    d.sourceEffectsCommitted=true;d.sourcePC=failedRetain?0:0x2E1AB4;return true;
}
}
