#include "original_ui_garage_ui_lights50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
namespace rr::platform {
namespace {
using Vec=std::array<float,4>;
float fp(std::uint32_t u)noexcept{float f;std::memcpy(&f,&u,4);return f;}
std::uint32_t word(float f)noexcept{std::uint32_t u;std::memcpy(&u,&f,4);return u;}
float add(float a,float b)noexcept{volatile float r=a+b;return r;}
float sub(float a,float b)noexcept{volatile float r=a-b;return r;}
float mul(float a,float b)noexcept{volatile float r=a*b;return r;}
float div(float a,float b)noexcept{volatile float r=a/b;return r;}
bool finite(const Vec& v)noexcept{for(float f:v)if(!std::isfinite(f))return false;return true;}
bool vuNormalize(Vec& v)noexcept {
    // Same bounded finite VU convention as ChariotRoot49U: rounded binary32
    // sqrt followed by binary32 division, not scalar EE RSQRT's single round.
    if(!finite(v))return false;
    const float x=mul(v[0],v[0]),y=mul(v[1],v[1]),z=mul(v[2],v[2]);
    const float norm=add(add(x,y),mul(1.f,z));
    if(!std::isfinite(norm)||(norm!=0&&(word(norm)&0x7f800000u)==0))return false;
    float q=fp(0x7f7fffffu);
    if(norm!=0){volatile float root=std::sqrt(norm);q=div(1.f,root);}
    for(std::size_t i=0;i<3;++i)v[i]=mul(v[i],q);
    return finite(v);
}
bool infinityPosition(const Vec& in,Vec& out)noexcept {
    const float x=mul(in[0],in[0]),y=mul(in[1],in[1]),z=mul(in[2],in[2]);
    const float norm=add(add(x,y),z);
    if(!std::isfinite(norm)||norm<=0||(word(norm)&0x7f800000u)==0)return false;
    //107110 scalar RSQRT.S, then1CE200 reciprocal, then1CE214 division.
    volatile float reciprocalRoot=static_cast<float>(1.0/std::sqrt(static_cast<double>(norm)));
    const float length=div(1.f,reciprocalRoot),factor=div(80000.f,length);
    out={{mul(in[0],factor),mul(in[1],factor),mul(in[2],factor),1.f}};return finite(out);
}
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
    return a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
}
OriginalUiGarageUiLights50KG::~OriginalUiGarageUiLights50KG()noexcept {
    if((bound_&&!payloadReleased_)||scopeBorrowed()||(manager_&&manager_->uiLightsOwner_==this))std::terminate();
}
bool OriginalUiGarageUiLights50KG::identityCurrent()const noexcept {
    return bound_&&!payloadReleased_&&scopeBorrowed()&&manager_&&manager_->current()&&
        manager_->uiLightsOwner_==this&&manager_->issuer_==issuer_&&sameOwner(application_,manager_->entities_)&&
        provider_&&provider_->sealed()&&provider_->revision()==providerRevision_;
}
bool OriginalUiGarageUiLights50KG::current()const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    if(!complete_||!identityCurrent()||!scopeCurrent()||!descriptors29C_)return false;
    for(std::size_t i=0;i<2;++i){if(!lights_[i])return false;const auto& l=*lights_[i];
        if(!lightIdentity(l)||!l.typed||!l.configured||!l.registered||!l.attached||l.retiring||l.retired||l.entity.reference00!=1||
           l.entity.parent1C!=grant_->manager()||l.entity.mask20||descriptors29C_[i].light38!=&l)return false;
    }
    return true;
}
bool OriginalUiGarageUiLights50KG::prepare1CDB10(std::shared_ptr<Grant> grant,std::shared_ptr<Search> search,
    std::shared_ptr<Manager> manager,OriginalUiGarageModelResources50KG& resources,
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,Diagnostic& d)noexcept {
    d={};if(attempted_||bound_||payloadReleased_)return fail(d,Status::phase,0x1CDB10);
    try {
    if(!grant||!grant->current()||!search||!search->current()||!sameOwner(grant,search->grant_)||
       search->node_.next14||!manager||!sameOwner(manager,manager->issuedOwner_.lock())||!manager->current()||!manager->zoneReleasePreflight()||
       (manager->zonesAttempted_&&!manager->zonesComplete_)||manager->uiLightsOwner_||manager->count94_||manager->registryFailed_||
       manager->issuer_!=grant->issuer()||!sameOwner(manager->entities_,grant->application())||
       resources.issuer_!=grant->issuer()||!sameOwner(resources.application_,grant->application())||
       resources.generation_!=grant->generation_||!resources.domain_||!resources.domain_->prepared()||
       !assets||!assets->loaded()||!assets->archive()||!assets->selectedPod()||!search->selected_||
       !search->selected_->model()||search->selected_->model()->source()!=assets->selectedPod()->model.get()||
       std::fegetround()!=FE_TONEAREST)return fail(d,Status::input,0x1CDB10);
    bool actualResource=false;for(const auto& resource:resources.population_)if(resource.get()==search->selected_->model())actualResource=true;
    if(!actualResource)return fail(d,Status::input,0x132820);
    const auto archive=assets->archive();const auto provider=resources.domain_->archiveProvider3E1A70();
    const auto member=archive->find("UI/UIGarage/ui.dat");assets::OriginalStandardArchiveProvider50LA::MemberView found;
    if(archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::garage)||
       !member.identity||!member.data||!provider||!provider->sealed()||
       provider->lookup132820(member.identity->name,found)!=assets::OriginalStandardArchiveProvider50LA::Lookup::present||
       !found.data||found.size!=member.size||std::memcmp(found.data,member.data,member.size))return fail(d,Status::source,0x132820);
    // Authentic immutable parsing is a cold resource stage. It creates no
    // source29C descriptor or Entity and cannot certify manager population.
    assets::OriginalUiDat50KG ui;
    if(!assets::readOriginalUiDat50KG(*archive,"UI/UIGarage/ui.dat",ui,d.parse,
        assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,Status::source,0x1CDB10);
    const auto* section=ui.section("Lights");std::int32_t count=0;
    if(!section||!assets::originalUiInteger50KG(*section,"NumberOfLights",2,count,d.parse,0x1CDB4C)||count!=2)
        return fail(d,Status::source,0x1CDB4C);
    const auto step=grant->bindGarageUiLightsPayload(this,search.get());
    if(!step.okay())return fail(d,Status::scope,step.sourcePC);
    attempted_=bound_=true;grant_=std::move(grant);search_=std::move(search);manager_=std::move(manager);
    application_=grant_->application();issuer_=grant_->issuer();assets_=std::move(assets);ui_=std::move(ui);
    provider_=provider;providerRevision_=provider->revision();Manager::Diagnostic md;
    if(!manager_->bindUiLights(*this,md))std::terminate(); // all exact preflight above, no callback
    // Actual reached1CCD88, after the genuine Search predecessor. A prepared
    // zone shape is never accepted as a replacement for this reset/rebuild.
    if(!manager_->setupOneZone148B60(md))return fail(d,Status::registry,md.sourcePC);
    try{descriptors29C_.reset(new Descriptor[2]);}
    catch(...){return fail(d,Status::allocation,0x1CDB6C);}
    for(std::size_t i=0;i<2;++i){auto& row=descriptors29C_[i];row.position00=row.infinity10=row.look20={{0,0,0,1}};}
    for(std::size_t i=0;i<2;++i){d.ordinal=i;auto& row=descriptors29C_[i];row.light38=nullptr;row.light38Known=true;
        if(!parseDescriptor1CDCF0(i,d)||!constructLight146720(i,d)||!configureLight146F60(i,d))return false;
        row.light38=lights_[i].get(); //1CDC50 only after146F60 returned.
        if(!manager_->appendUiLight148F30(*this,i,md))return fail(d,Status::registry,md.sourcePC);
        // Actual registration is verified before the source attachment. No
        // caller-supplied ready flag and no retain added for the borrowed row.
        if(!lights_[i]->registered||manager_->count94_!=i+1||manager_->registry9C_[i]!=&lights_[i]->entity)
            std::terminate();
        const auto attached=grant_->attachGarageUiLight1CDC8C(this,i,lights_[i]->entity);
        if(!attached.okay())return fail(d,Status::scope,attached.sourcePC);
        lights_[i]->attached=true;
    }
    complete_=true;d.sourcePC=0x1CDCB0;return true;
    }catch(...){return fail(d,Status::allocation,0x1CDB10);}
}
bool OriginalUiGarageUiLights50KG::parseDescriptor1CDCF0(std::size_t ordinal,Diagnostic& d)noexcept {
    auto& row=descriptors29C_[ordinal];
    try {
        const auto* section=ui_.section(ordinal?"Light2":"Light1");
        if(!section)return fail(d,Status::source,0x1CDCF0); // only authenticated authored branches
        const auto* type=section->find("Type");
        if(!type||type->text!=(ordinal?"Directional":"Ambient"))return fail(d,Status::source,0x1CE014);
        row.typeA0=ordinal?2u:4u;
        std::array<float,3> rgb{};
        if(!section->find("ColorRGB")||!assets::originalUiTriple50KG(*section,"ColorRGB",{},rgb,d.parse,0x1CE094))
            return fail(d,Status::source,0x1CE094);
        for(std::size_t i=0;i<3;++i){if(!std::isfinite(rgb[i])||rgb[i]<0||rgb[i]>255)return fail(d,Status::arithmetic,0x1CEA20);row.color3C[i]=rgb[i];}
        row.knownColor=7; //1CEA20 NEVER writes authored alpha48.
        std::array<float,3> position{};
        if(!assets::originalUiTriple50KG(*section,"Position",{{-2000,4000,1252}},position,d.parse,0x1CE0D4))return fail(d,Status::source,0x1CE0D4);
        row.position00={{position[0],position[1],position[2],1}};row.position30=&row.position00;
        if(!section->find("Position")&&row.typeA0==4)row.position30=nullptr;
        const auto flag=[&](const char* key,bool fallback,unsigned bit,std::uint32_t pc){bool value=false;
            if(!assets::originalUiBoolean50KG(*section,key,fallback,value,d.parse,pc))return false;
            const auto mask=std::uint8_t(1u<<bit);row.flagsA4=std::uint8_t((row.flagsA4&~mask)|(value?mask:0));row.knownFlagsA4|=mask;return true;};
        if(!flag("PlaceLightAtInfinity",false,1,0x1CE154))return fail(d,Status::source,0x1CE154);
        if(row.flagsA4&2){if(!infinityPosition(row.position00,row.infinity10))return fail(d,Status::arithmetic,0x1CE1EC);row.position30=&row.infinity10;}
        row.look34=&row.look20;
        if(section->find("LookVector"))return fail(d,Status::source,0x1CE280); // not authored here
        if(section->find("TargetPosition")){
            std::array<float,3> target{};if(!assets::originalUiTriple50KG(*section,"TargetPosition",{},target,d.parse,0x1CE2C0))return fail(d,Status::source,0x1CE2C0);
            row.look20={{sub(target[0],row.position00[0]),sub(target[1],row.position00[1]),sub(target[2],row.position00[2]),1}};
            if(!vuNormalize(row.look20))return fail(d,Status::arithmetic,0x1CE310);
        }else{row.look20={{0,0,0,1}};if(row.typeA0==4)row.look34=nullptr;else return fail(d,Status::source,0x1CE368);}
        if(!assets::originalUiNumber50KG(*section,"Range",-1,row.range4C,d.parse,0x1CE3A0))return fail(d,Status::source,0x1CE3A0);
        if(row.range4C==-1)row.range4C=0;
        if(!std::isfinite(row.range4C)||row.range4C<0)return fail(d,Status::arithmetic,0x147018);
        if(!flag("ShowDebugSphere",false,0,0x1CE424)||!flag("CastsShadows",false,3,0x1CE474)||
           !flag("EmitsLight",true,2,0x1CE4CC)||!flag("HasLensFlare",false,4,0x1CE524))return fail(d,Status::source,d.parse.sourcePC);
        if(!(row.flagsA4&4))return fail(d,Status::source,0x1CDBD0);
        if(row.flagsA4&16){
            // Authenticated original integer tokens only. This bounded reader
            // does not widen native acceptance to arbitrary source atoi text.
            const auto* color=section->find("LensFlareColorRGB");
            if(!color||color->text!="255,255,240")return fail(d,Status::source,0x1CE598);
            row.flareColor58=0x00fffff0u;
            if(!assets::originalUiNumber50KG(*section,"LensFlareBrightness",-1,row.brightness5C,d.parse,0x1CE5B4))return fail(d,Status::source,0x1CE5B4);
            if(row.brightness5C==-1)row.brightness5C=1;
            const auto* layers=section->find("LensFlareLayersVisible");
            if(!layers||layers->text!="T,T,T,T,T")return fail(d,Status::source,0x1CE640);
            row.layers50={{1,1,1,1,1}};
            const auto* name=section->find("LensFlareTextureMapName");
            const std::string_view text=name?std::string_view(name->text):std::string_view("overlay\\lensflare.tga");
            if(text.size()>=63)return fail(d,Status::source,0x1CE728);
            std::memcpy(row.name60.data(),text.data(),text.size());row.name60[text.size()]='\0';row.flareKnown=true;
        }
        row.parsed=true;return true;
    }catch(...){return fail(d,Status::allocation,0x1CDCF0);}
}
bool OriginalUiGarageUiLights50KG::constructLight146720(std::size_t ordinal,Diagnostic& d)noexcept {
    try{lights_[ordinal]=std::make_unique<Light>();}
    catch(...){return fail(d,Status::allocation,0x1CDBF8);}
    auto& l=*lights_[ordinal];const auto step=grant_->constructGarageUiLight146720(this,ordinal,l.entity);
    if(!step.okay())return fail(d,Status::scope,step.sourcePC);
    l.constructed=true;l.query30=0;l.query32=0xff;l.flags33=0;l.knownFlags33=7;
    l.type34=0;l.mode38=3;l.diffuse3C=l.specular4C=l.ambient5C={{128,128,128,0}};
    l.diffuseAlphaKnown=l.specularAlphaKnown=l.ambientAlphaKnown=true;
    l.position6C={{0,0,0}};l.look78={{1,0,0}};
    l.range84=fp(game::SourceUiLightMath50KG::range146898());l.field88=1;l.attenuation8C={{1,0,0}};
    l.cone9C=fp(0x3fc90fdbu);l.cone98=div(l.cone9C,2.f);l.luminanceCC=0xbf800000u;l.ordinalC8=-1;
    l.positionA0={{0,0,0,1}};l.lookB0={{0,0,0,1}};
    // C0/C4/D0/D4/D8 are real constructor null writes, not skipped objects.
    const auto type=application_->types383408().lightEmitter146938();
    if(!type.okay())return fail(d,Status::types,type.sourcePC);
    l.query32=type.type;l.typed=true;return true;
}
bool OriginalUiGarageUiLights50KG::configureLight146F60(std::size_t ordinal,Diagnostic& d)noexcept {
    auto& l=*lights_[ordinal];const auto& row=descriptors29C_[ordinal];Manager::Diagnostic md;
    if(!l.typed||!row.parsed||l.observerD0||l.fieldC0||l.fieldC4||l.fieldD8||!identityCurrent()||std::fegetround()!=FE_TONEAREST)return fail(d,Status::phase,0x146F60);
    l.type34=row.typeA0;l.ordinalC8=static_cast<std::int32_t>(ordinal);
    Vec half{};for(std::size_t i=0;i<3;++i)half[i]=div(row.color3C[i],2.f);
    if(row.typeA0==4){l.ambient5C=half;l.ambientAlphaKnown=false;l.diffuse3C=l.specular4C={{0,0,0,0}};
        l.diffuseAlphaKnown=l.specularAlphaKnown=true;
    }else{l.diffuse3C=l.specular4C=half;l.diffuseAlphaKnown=l.specularAlphaKnown=false;
        l.ambient5C={{0,0,0,0}};l.ambientAlphaKnown=true;}
    if(!manager_->incrementUiRevision149300(*this,md))return fail(d,Status::registry,md.sourcePC);
    if(!game::SourceUiLightMath50KG::luminance146E60(word(l.diffuse3C[0]),word(l.diffuse3C[1]),word(l.diffuse3C[2]),l.luminanceCC))
        return fail(d,Status::arithmetic,0x146E60);
    float range=row.range4C;const float maximum=fp(game::SourceUiLightMath50KG::range146898());
    if(range==0||range>maximum)range=maximum;
    if(range!=l.range84){l.range84=range;if(!manager_->incrementUiRevision149300(*this,md))return fail(d,Status::registry,md.sourcePC);}
    if(row.position00[0]!=l.positionA0[0]||row.position00[1]!=l.positionA0[1]||row.position00[2]!=l.positionA0[2]){
        l.positionA0=row.position00; //1CDC24 passes BASE, NOT descriptor30.
        //147600 has no observer/flare work with actual D0/C0/C4 null.
        l.position6C={{l.positionA0[0],l.positionA0[1],l.positionA0[2]}};
        if(!manager_->incrementUiRevision149300(*this,md))return fail(d,Status::registry,md.sourcePC);
    }
    if(row.look34&&(row.look20[0]!=l.lookB0[0]||row.look20[1]!=l.lookB0[1]||row.look20[2]!=l.lookB0[2])){
        l.lookB0=row.look20;if(!vuNormalize(l.lookB0))return fail(d,Status::arithmetic,0x147200);
        l.look78={{l.lookB0[0],l.lookB0[1],l.lookB0[2]}};
        if(!manager_->incrementUiRevision149300(*this,md))return fail(d,Status::registry,md.sourcePC);
    }
    l.attenuation8C={{1,0,0}};l.mode38=3;l.configured=true;return true;
}
bool OriginalUiGarageUiLights50KG::retireLight146980(std::size_t ordinal,Diagnostic& d)noexcept {
    d={};d.ordinal=ordinal;if(ordinal>=2)return fail(d,Status::input,0x146980);
    if(!lights_[ordinal]||!lights_[ordinal]->constructed||lights_[ordinal]->retired)return true;
    auto& l=*lights_[ordinal];
    if(!identityCurrent()||!scopeCurrent()||!manager_->preflightRemoveUiLight1491F0(*this,ordinal))return fail(d,Status::registry,0x1491F0);
    if(!l.retiring){const auto step=grant_->preflightGarageUiLightRetirement(this,ordinal,l.entity);
        if(!step.okay())return fail(d,Status::scope,step.sourcePC);
        const auto begun=grant_->beginGarageUiLightRetirement(this,ordinal,l.entity);
        if(!begun.okay())return fail(d,Status::scope,begun.sourcePC);
        l.retiring=true;complete_=false;
    }
    Manager::Diagnostic md;if(!manager_->removeUiLight1491F0(*this,ordinal,md))std::terminate();
    const auto finished=grant_->finishGarageUiLightRetirement(this,ordinal,l.entity);
    if(!finished.okay())std::terminate();
    l.retiring=false;l.retired=true;l.attached=false;d.sourcePC=0x1469EC;return true;
}
bool OriginalUiGarageUiLights50KG::retireManagerDescriptors1CCB20(Diagnostic& d)noexcept {
    d={};if(payloadReleased_)return true;
    if(!identityCurrent()||!payloadCurrent())return fail(d,Status::phase,0x1CCB04);
    for(const auto& l:lights_)if(l&&l->constructed&&!l->retired)return fail(d,Status::phase,0x1CCB04);
    Manager::Diagnostic md;if(!manager_->releaseZones148110(md))return fail(d,Status::registry,md.sourcePC);
    descriptors29C_.reset(); //1CCB20, AFTER actual shared148110 and manager2A0 release.
    if(!manager_->releaseUiLightsBorrow(*this,md))std::terminate();
    const auto noted=grant_->noteGarageUiLightsPayloadReleased(this);if(!noted.okay())std::terminate();
    payloadReleased_=true;complete_=false;d.sourcePC=0x1CCB20;return true;
}
OriginalUiGarageUiLights50KG::Observation OriginalUiGarageUiLights50KG::inspect()const noexcept {
    Observation out;out.attempted=attempted_;out.descriptorsPublished=bool(descriptors29C_);out.complete=complete_;out.payloadReleased=payloadReleased_;
    for(std::size_t i=0;i<2;++i){
        if(lights_[i]){const auto& l=*lights_[i];auto& o=out.lights[i];o.entity=l.constructed?&l.entity:nullptr;
            o.query32=l.query32;o.flags33=l.flags33;o.knownFlags33=l.knownFlags33;o.type34=l.type34;o.mode38=l.mode38;
            o.range84=word(l.range84);o.luminanceCC=l.luminanceCC;o.diffuse3C=l.diffuse3C;o.specular4C=l.specular4C;o.ambient5C=l.ambient5C;
            o.positionA0=l.positionA0;o.lookB0=l.lookB0;o.constructed=l.constructed;o.typed=l.typed;o.configured=l.configured;
            o.registered=l.registered;o.attached=l.attached;o.retired=l.retired;o.diffuseAlphaKnown=l.diffuseAlphaKnown;
            o.specularAlphaKnown=l.specularAlphaKnown;o.ambientAlphaKnown=l.ambientAlphaKnown;
        }
        if(descriptors29C_){const auto& r=descriptors29C_[i];auto& o=out.descriptors[i];o.position00=r.position00;o.infinity10=r.infinity10;o.look20=r.look20;
            o.color3C=r.color3C;o.knownColor=r.knownColor;o.flagsA4=r.flagsA4;o.knownFlagsA4=r.knownFlagsA4;o.typeA0=r.typeA0;
            o.flareColor58=r.flareColor58;o.range4C=r.range4C;o.brightness5C=r.brightness5C;o.layers50=r.layers50;o.name60=r.name60;
            o.vectorsConstructed=true;o.light38Known=r.light38Known;o.parsed=r.parsed;o.flareKnown=r.flareKnown;
            o.light38Identity=reinterpret_cast<std::uintptr_t>(r.light38);o.position30Present=r.position30!=nullptr;o.look34Present=r.look34!=nullptr;
        }
    }return out;
}
}
