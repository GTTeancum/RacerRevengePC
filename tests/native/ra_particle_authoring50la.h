#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/game/original_gc_fire_emission50kg.h"
#include "../../src/game/original_application_entities50la.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include <optional>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <array>
#include <exception>
#include <memory>
#include <limits>
#include <stdexcept>
#include <utility>

namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_particle_authoring50LA {
namespace a=rr::assets;namespace g=rr::game;
// Authenticated source closure plus four existing counted identities. The
// private reached loader adds same-owner cold fields; runtime emission stays unjoined.
class Owner final {
    friend class ::ra_cold_aggregate50LA::Owner;
public:
    struct AttachmentSource {
        std::string soulTree;
        std::size_t worldInstance=0;
        std::uint32_t zone=1,ordinal=0,type=8,instances=1;
        std::array<float,4> offset50{{0,15,0,1}};
    };
    struct Definition {
        g::OriginalGcFireEmissionDefinition50KG emission{};
        g::OriginalGcCommonEmitterTiming50KG timing{};
        std::array<float,3> authoredVelocity{};
        float ageAtBirth=0,groundLevel=0,gravityMultiplier=3,restitution=.55f;
        std::int32_t forcedSets=0;
    };
    struct Diagnostic {
        const char* failure=nullptr;std::size_t constructed=0;
        a::DecodeError48C decode{};a::OriginalUiPageDiagnostic50KG text{};
        g::OriginalWreckComponentResult50LA component{};
    };
private:
    struct Emitter {
        g::OriginalWreckComponent50LA entity;
        g::EffectControlState50BC controls{};
        // Source15DE48,15DE44; these are distinct per-emitter mutable fields.
        float fractional224=0;
        std::uint32_t field228=1;
        // Reached15DEF0 raw borrows; no source retain or manager registration.
        g::OriginalWreckComponent50LA* manager70=nullptr;
        g::OriginalGcParticleManager50KG* numericManager70=nullptr;
        const g::OriginalRacerSceneObject50LA* attachment74=nullptr;
        std::array<float,3> offset50XYZ{};
        std::optional<std::uint32_t> sourceOffsetW;
        std::string filename78; // Only reached bytes through NUL; not ctor-zero.
        Definition liveDefinition{}; // Only loaderPC-proven lanes are source-known.
        std::optional<float> inverseFrames1C0,delay1DC;
        std::optional<std::int32_t> currentFrame190;
        bool loaderAttempted=false,loaderComplete=false;
        bool enrolled=false,retired=false; // Native custody; no source field claim.
        std::uint32_t loaderPC=0;
        // Source range200/210 W is constructorVF0.W1, unlike offset50 W.
        // No new constructor/reset receipt for existing controls or224/228.
    };
    class NativeEmitterConfig157320;
    bool loadEmitter15DEF0(std::size_t,g::OriginalGcParticleManager50KG&,
        g::OriginalWreckComponent50LA&,const g::OriginalRacerSceneObject50LA&,
        std::shared_ptr<a::OriginalStandardArchiveProvider50LA>,const std::string&,
        const std::string&,const std::array<float,3>&,std::optional<std::uint32_t>,
        Diagnostic&,std::uint32_t&)noexcept;
    bool readLiveEmitter15E6D0(Emitter&,NativeEmitterConfig157320&,Diagnostic&,
        std::uint32_t&)noexcept;
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::shared_ptr<g::OriginalApplicationEntities50LA> application_;
    std::array<std::vector<std::uint8_t>,5> bytes_{};
    a::ParameterDocument48F tob_;
    std::array<AttachmentSource,4> attachments_{};
    Definition definition_{};
    std::array<Emitter,4> emitters_{};
    std::uint64_t generation_=0;
    std::size_t constructed_=0;
    bool attempted_=false,decoded_=false,closed_=false;
    static void need(bool value,const char* why){if(!value)throw std::runtime_error(why);}
    static std::uint64_t key(const void* p) noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    static const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
        const a::ParameterBlock48F* result=nullptr;
        for(const auto& b:p.blocks)if(b.name==name){need(!result,"duplicate RA block");result=&b;}
        need(result,"missing RA block");return *result;
    }
    static const std::string* optional(const a::ParameterBlock48F& b,const char* name){
        const std::string* result=nullptr;
        for(const auto& f:b.fields)if(f.name==name){need(!result,"duplicate RA field");result=&f.value;}
        return result;
    }
    static const std::string& field(const a::ParameterBlock48F& b,const char* name){
        const auto* result=optional(b,name);need(result,"missing RA field");return *result;
    }
    static const a::OriginalUiDatSection50KG& section(
        const std::vector<a::OriginalUiDatSection50KG>& doc,const char* name){
        const a::OriginalUiDatSection50KG* found=nullptr;
        for(const auto& s:doc)if(s.name==name){need(!found,"duplicate RA EMT section");found=&s;}
        need(found,"missing RA EMT section");return *found;
    }
    bool readDefinition(Diagnostic& d){
        std::vector<a::OriginalUiDatSection50KG> doc;
        if(!g::readOriginalGcParticleText50KG(bytes_[1],801,
            "839be25d68132969c3e7b5772ba1a5a5e66f280d48d188d76bc04ab8c3689d41",doc))return false;
        const auto& general=section(doc,"General Info");
        const auto& properties=section(doc,"Properties");const auto& animation=section(doc,"Animation");
        auto& f=definition_;auto& e=f.emission;
        //15E6D0 reads derived range/spread BEFORE common15AEA0.
        if(!a::originalUiTriple50KG(general,"MinRange",{},e.minRange,d.text,0x15e710)||
           !a::originalUiTriple50KG(general,"MaxRange",{},e.maxRange,d.text,0x15e72c)||
           !a::originalUiBoolean50KG(general,"ApplySpreadToVelocity",true,e.applySpreadToVelocity,d.text,0x15e744)||
           !a::originalUiInteger50KG(general,"ParticleGroup",-1,e.particleGroup,d.text,0x15aee0)||
           !a::originalUiNumber50KG(general,"ParticleLife",1000,e.particleLife,d.text,0x15af70)||
           !a::originalUiNumber50KG(general,"InitialSize",1,e.initialSize,d.text,0x15af8c)||
           !a::originalUiNumber50KG(general,"Growth",0,e.growth,d.text,0x15afa4)||
           !a::originalUiNumber50KG(general,"EmitRate",500,e.emitRate,d.text,0x15afc0)||
           !a::originalUiNumber50KG(general,"AgeAtBirth",0,f.ageAtBirth,d.text,0x15afd8)||
           !a::originalUiTriple50KG(general,"Velocity",{},f.authoredVelocity,d.text,0x15b004)||
           !a::originalUiNumber50KG(general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,d.text,0x15b064)||
           !a::originalUiNumber50KG(general,"GravityMultiplier",3,f.gravityMultiplier,d.text,0x15b080)||
           !a::originalUiNumber50KG(general,"Restitution",.55f,f.restitution,d.text,0x15b0a0))return false;
        bool fades=false,additive=false,velocity=false,grows=false,nonlinear=false,glows=false,gravity=false,lit=true;
        if(!a::originalUiBoolean50KG(properties,"Fades",false,fades,d.text,0)||
           !a::originalUiBoolean50KG(properties,"Additive",false,additive,d.text,0)||
           !a::originalUiBoolean50KG(properties,"HasVelocity",false,velocity,d.text,0)||
           !a::originalUiBoolean50KG(properties,"Grows",false,grows,d.text,0)||
           !a::originalUiBoolean50KG(properties,"FadeNonLinear",false,nonlinear,d.text,0)||
           !a::originalUiBoolean50KG(properties,"Glows",false,glows,d.text,0)||
           !a::originalUiBoolean50KG(properties,"Gravity",false,gravity,d.text,0)||
           !a::originalUiBoolean50KG(properties,"Lit",true,lit,d.text,0)||
           !a::originalUiBoolean50KG(properties,"RandomMirrorAboutX",false,e.randomMirrorX,d.text,0)||
           !a::originalUiBoolean50KG(properties,"RandomMirrorAboutY",false,e.randomMirrorY,d.text,0)||
           !a::originalUiNumber50KG(animation,"FramesPerSet",-1,e.framesPerSet,d.text,0x15b490)||
           !a::originalUiInteger50KG(animation,"NumberOfForcedSets",0,f.forcedSets,d.text,0x15b4c0))return false;
        e.particleFlags=(additive?1u:0u)|(fades?2u:0u)|(velocity?4u:0u)|(grows?8u:0u)|
            (gravity?0x20u:0u)|(!lit?0x800u:0u)|(nonlinear?0x1000u:0u)|(glows?0x2000u:0u)|
            (e.framesPerSet>1?0x400u:0u)|(f.groundLevel!=std::numeric_limits<float>::max()?0x200u:0u);
        if(g::readOriginalGcCommonEmitterTiming50KG(bytes_[1],f.timing,d.text)!=g::OriginalGcSprayStatus50KG::complete)return false;
        e.authoredVelocityY220=f.authoredVelocity[1];
        // These atlas endpoints are verified from the authentic PRT below;
        // no manager registration or source-loaded flag is implied.
        e.firstFrame=52;e.lastFrame=75;
        return e.particleGroup==4&&e.particleLife==1000&&e.initialSize==10&&e.growth==-6.5f&&e.emitRate==250&&
            e.framesPerSet==8&&e.particleFlags==0x1405u&&e.randomMirrorX&&e.randomMirrorY&&e.applySpreadToVelocity&&
            e.minRange==std::array<float,3>{{-20.5f,-16,-20.5f}}&&e.maxRange==std::array<float,3>{{20.5f,16,20.5f}}&&
            f.authoredVelocity==std::array<float,3>{{0,80,0}}&&f.ageAtBirth==0&&f.forcedSets==0&&
            f.timing.mode1D8==0&&f.timing.delayTime1E0==0;
    }
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    ~Owner(){if(!close())std::terminate();}
    bool decode(std::shared_ptr<const ra_cold_sources50LA::Owner> source,Diagnostic& d) noexcept {
        d={};if(attempted_||closed_){d.failure="RA particle closure already attempted";return false;}attempted_=true;
        try {
            const auto* s=source?source->sourcesDecoded():nullptr;
            need(s&&s->world.zones.size()==10,"actual RA source owner required");source_=std::move(source);
            need(field(block(s->world.parameters,"environment"),"particletexturemapname")=="demoprt.tga","RA actual SCB particle atlas selection");
            struct Pin{const char* name;std::size_t bytes;const char* sha;};
            constexpr Pin pins[5]={
                {"tracks\\ra.tob",95558,"7336c6bab40265adb5e82aec8ed3ee66669c19b26a597cfacedddfca4be48524"},
                {"particle\\raFire.emt",801,"839be25d68132969c3e7b5772ba1a5a5e66f280d48d188d76bc04ab8c3689d41"},
                {"particle\\demoprt.prt",9210,"d4604f2cfd8f40b9704fb63527fdb7574323cbebc9ac34b255b166e9f6be0a5f"},
                {"particle\\demoprt.psm",5022,"ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1"},
                {"particle\\demoprt.pst",132124,"e01d9c314d4c8c31e9d1e56cbbf4573e722c3ce1965e1daddfbeb33844b82bb6"}};
            for(std::size_t i=0;i<5;++i)need(a::extractResMember48C(s->archive,pins[i].name,bytes_[i],d.decode,pins[i].bytes)&&
                bytes_[i].size()==pins[i].bytes&&pod_materials50ed_fixture::detail::sha256(bytes_[i])==pins[i].sha,"RA particle exact member identity");
            need(a::decodeParameters48F(bytes_[0],tob_,d.decode),"RA original TOB syntax");
            need(field(block(tob_,"particleemitters"),"numberofemitters")=="4","RA emitter count");
            for(std::size_t i=0;i<4;++i){
                auto& out=attachments_[i];out.soulTree="FireP0t0"+std::to_string(i+1)+".slt";
                const auto& b=block(tob_,"particleemitter"+std::to_string(i+1));
                need(field(b,"attachtosoultree")==out.soulTree&&field(b,"emitterfilename")=="rafire.emt"&&
                     field(b,"numberofinstances")=="1"&&field(b,"offsetfromsoultree")=="0.0,15.0,0.0"&&
                     field(b,"startingzone")=="1"&&field(b,"typeid")=="8","RA attachment authoring");
                std::size_t matches=0;
                for(std::size_t j=0;j<s->world.instances.size();++j){const auto& instance=s->world.instances[j];
                    need(instance.parameter_block<s->world.parameters.blocks.size(),"RA SCB block bounds");
                    const auto* slt=optional(s->world.parameters.blocks[instance.parameter_block],"slt");
                    if(slt&&*slt==out.soulTree){++matches;out.worldInstance=j;out.ordinal=instance.ordinal;
                        need(instance.zone==1&&instance.ordinal==366+i&&
                            instance.geometry_member=="geometry\\firep0t0"+std::to_string(i+1)+".psg","RA exact SoulTree placement");}}
                need(matches==1,"RA unique authored SoulTree match");
            }
            need(readDefinition(d),"RA original EMT fields/defaults");
            // Reuse only the existing authenticated text reader; do not create
            // another numeric particle manager merely to inspect this atlas.
            std::vector<a::OriginalUiDatSection50KG> prt;
            need(g::readOriginalGcParticleText50KG(bytes_[2],pins[2].bytes,pins[2].sha,prt),"RA PRT text");
            const auto& group=section(prt,"Group4");std::int32_t type=0,first=0,last=0;
            need(a::originalUiInteger50KG(group,"TypeID",-1,type,d.text,0)&&
                 a::originalUiInteger50KG(group,"FirstFrame",-1,first,d.text,0)&&
                 a::originalUiInteger50KG(group,"LastFrame",-1,last,d.text,0)&&type==8&&first==52&&last==75,"RA actual fire atlas group");
            decoded_=true;return true;
        }catch(...){d.failure="RA particle closure rejected; private prefix retained";return false;}
    }
    // Explicit detached construction stage only. Source sourceA1=1 at27CA70.
    // Four separate Entity counters and constructor control words; no fake
    // scene parent, manager ownerId, render-stamp owner or attachment74.
    bool constructDetached(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::uint64_t sessionGeneration,Diagnostic& d) noexcept {
        d={};if(!decoded_||closed_||application_||!application||!application->application10()||
            !application->application26C()||!sessionGeneration){d.failure="RA detached constructor inputs";return false;}
        application_=std::move(application);generation_=sessionGeneration;
        for(auto& e:emitters_){
            d.component=g::initializeOriginalWreckComponent50LA(e.entity,g::OriginalWreckComponentKind50LA::effect15DE00,
                generation_,key(&e.entity),true,application_->objectCount423F60());
            if(d.component.status!=g::OriginalWreckComponentStatus50LA::complete){d.failure="RA Entity constructor prefix";return false;}
            ++constructed_;d.constructed=constructed_;
            e.controls.position30[3]=e.controls.previous40[3]=e.controls.velocity60[3]=0x3f800000u;
            e.controls.flags1D4=1; // represented low bits15A684,15A6B0,15A6CC
            e.controls.word19C=0;e.fractional224=0;e.field228=1;
        }
        return true;
    }
    const std::array<AttachmentSource,4>* attachmentSources()const noexcept{return decoded_&&!closed_?&attachments_:nullptr;}
    const Definition* definition()const noexcept{return decoded_&&!closed_?&definition_:nullptr;}
    const std::vector<std::uint8_t>* originalMember(std::size_t i)const noexcept{return decoded_&&!closed_&&i<5?&bytes_[i]:nullptr;}
    const g::EffectControlState50BC* detachedControls(std::size_t i)const noexcept{return !closed_&&i<constructed_?&emitters_[i].controls:nullptr;}
    std::size_t detachedConstructedCount()const noexcept{return closed_?0:constructed_;}
    bool close() noexcept {
        if(closed_)return true;
        if(constructed_&&(!application_||application_->attachmentLock423F58()))return false;
        std::size_t live=0;
        for(std::size_t i=0;i<constructed_;++i){const auto& owner=emitters_[i];const auto& c=owner.entity;
            if(owner.enrolled||c.sessionGeneration!=generation_||c.objectKey!=key(&c)||
               c.kind!=g::OriginalWreckComponentKind50LA::effect15DE00||c.parent1C||c.firstChild18||
               c.previous10||c.next14||c.descendants24)return false;
            if(owner.retired){
                if(c.reference00||owner.manager70||owner.numericManager70||owner.attachment74)return false;
            }else {
                if(c.reference00!=1||c.mask20)return false;
                ++live;
            }
        }
        if(live&&application_->objectCount423F60()<live)return false;
        // Actual list children were already finally released by142590 before
        // the List metadata destructor. Unenrolled detached/failed prefixes
        // remain THIS owner's responsibility; never decrement either twice.
        for(std::size_t i=0;i<constructed_;++i){auto& owner=emitters_[i];
            if(owner.retired)continue;
            // Native raw-borrow withdrawal before the real manager/model dies;
            // source15DE60 does not retain/release either70 or74. Actual EMT
            // forcedSets0 has no owned1C4 resource or RNG/service registration.
            owner.manager70=nullptr;owner.numericManager70=nullptr;owner.attachment74=nullptr;
            --owner.entity.reference00;--application_->objectCount423F60();owner.retired=true;
        }
        closed_=true;return true;
    }
};
}

#include "ra_particle_loader50la.inl"
