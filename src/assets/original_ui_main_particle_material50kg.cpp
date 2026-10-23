#include "original_ui_main_particle_material50kg.h"
#include <string>
#include <utility>

namespace rr::assets {
namespace {
using Field=OriginalUiGsField50KG;
using Error=OriginalUiMaterialError50KG;
struct Spec {Field field;std::uint32_t pc[2];bool global=false;};
// Actual2480D0 getter PCs. Shared COLCLAMP/PABE are unsuffixed; PRMODE CTXT
// is forced0/1 by the builder, not inherited or read from the authored PSM.
constexpr Spec specs[]={
    {Field::textureColorComponent,{0x248494,0x2484f8}},
    {Field::textureFunction,{0x2484ac,0x248510}},
    {Field::sourceAlphaValueWhenAis0,{0x24876c,0x2487f8}},
    {Field::sourceAlphaValueWhenAis1,{0x248784,0x248810}},
    {Field::methodOfExpandingTextureAlpha,{0x24879c,0x248828}},
    {Field::inputColorA,{0x248894,0x2489b0}},
    {Field::inputColorB,{0x2488ac,0x2489c8}},
    {Field::inputAlphaC,{0x2488c4,0x2489e0}},
    {Field::inputColorD,{0x2488f4,0x248a10}},
    {Field::fixedAlphaValue,{0x24890c,0x248a28}},
    {Field::wrapModeHorizontal,{0x248ad4,0x248be4}},
    {Field::wrapModeVertical,{0x248aec,0x248bfc}},
    {Field::minClampParameterInUDirection,{0x248b04,0x248c14}},
    {Field::maxClampParameterInUDirection,{0x248b1c,0x248c2c}},
    {Field::minClampParameterInVDirection,{0x248b34,0x248c44}},
    {Field::maxClampParameterInVDirection,{0x248b4c,0x248c5c}},
    {Field::colorClampingMethod,{0x248424,0x248424},true},
    {Field::alphaCorrectionValue,{0x248ccc,0x248d00}},
    {Field::alphaBlendingControlInUnitsOfPixels,{0x248458,0x248458},true},
    {Field::gouraudShadingOnOff,{0x248d64,0x248ec0}},
    {Field::textureMappingOnOff,{0x248d7c,0x248ed8}},
    {Field::foggingOnOff,{0x248d94,0x248ef0}},
    {Field::alphaBlendingOnOff,{0x248dac,0x248f08}},
    {Field::onePassAntiAliasingOnOff,{0x248dc4,0x248f20}},
    {Field::textureCoordinatesSTQorUV,{0x248ddc,0x248f38}},
    {Field::fragmentValueControlFixedOrNot,{0x248df4,0x248f50}},
    {Field::alphaTestOnOff,{0x249020,0x24917c}},
    {Field::alphaTestMethod,{0x249038,0x249194}},
    {Field::alphaReferenceValue,{0x249050,0x2491ac}},
    {Field::processingMethodWhenFailedInAlphaTest,{0x249068,0x2491c4}},
    {Field::destinationAlphaTestOnOff,{0x249080,0x2491dc}},
    {Field::destinationAlphaTestMode,{0x249098,0x2491f4}},
    {Field::depthTestMethod,{0x2490b0,0x24920c}},
    {Field::zWriteDisabled,{0x2492a8,0x249314}}
};
bool fail(OriginalUiMaterialDiagnostic50KG& d,Error e,const char* field,std::uint32_t pc=0){
    d.error=e;d.field=field;d.sourcePC=pc;return false;
}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i){
        const auto fold=[](char c){return c>='A'&&c<='Z'?char(c+32):c;};
        if(fold(a[i])!=fold(b[i]))return false;
    }
    return true;
}
std::string key(const Spec& spec,unsigned state){
    const auto* name=originalUiGsFieldName50KG(spec.field);
    return spec.global?std::string(name):std::string(name)+"_"+std::to_string(state+1);
}
bool known(const std::string& field){
    const char* common[]={"microProgramName","EnableThisShader","totalTextureMaps","ShaderPassGroup",
        "TextureMap_1","TextureMap_2","TextureCoordinateIndex_1","TextureCoordinateIndex_2"};
    for(auto* name:common)if(same(field,name))return true;
    for(unsigned state=0;state<2;++state){
        for(const auto& spec:specs)if(same(field,key(spec,state)))return true;
        const char* local[]={"inputAlphaD","lodCalculationMethod","maximumMipLevel","magFilter","minFilter","L","K"};
        for(auto* name:local)if(same(field,std::string(name)+"_"+std::to_string(state+1)))return true;
    }
    return false;
}
}
static bool prepareParticleMaterial(const OriginalUiArchive50KG& archive,
    const OriginalUiMaterialTemplates50KG& templates,const OriginalUiMaterialRuntime50KG& runtime,
    const Texture48E& texture,OriginalUiMainParticleMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d,OriginalUiArchiveId50KG expected) noexcept {
    d={};d.pass=1;
    try{
        if(!archive.loaded()||archive.identity()!=originalUiArchiveIdentity50KG(expected))
            return fail(d,Error::archive,"authenticated Main/Options source owner");
        const auto* inherited=templates.get(OriginalUiTemplate50KG::defaultMt);
        if(!inherited||!inherited->identity||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"actual particle DefaultMT",0x160ff4);
        if(runtime.override435670)return fail(d,Error::unsupportedValue,"435670 override",0x248924);
        const bool garage=expected==OriginalUiArchiveId50KG::garage;
        const char* textureName=garage?"garageprt":"mmprt";
        const std::string stem=std::string("particle\\")+textureName;
        OriginalUiDat50KG data;
        if(!readOriginalUiDat50KG(archive,stem+".psm",data,d.document,
            OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,Error::document,"mmprt.psm");
        const auto* general=data.section("General");const auto* section=data.section("ShaderPass_1");
        if(!general||!section)return fail(d,Error::section,"Main ParticleCode sections");
        const auto integer=[&](const char* name,std::int32_t fallback,std::int32_t& value,std::uint32_t pc=0){
            return originalUiInteger50KG(*section,name,fallback,value,d.document,pc)||fail(d,Error::value,name,pc);
        };
        std::int32_t passes=0,enabled=0,maps=0;
        if(!originalUiInteger50KG(*general,"TotalShaderPasses",0,passes,d.document,0x29b718)||
            !integer("EnableThisShader",1,enabled,0x2471dc)||
            !integer("totalTextureMaps",1,maps,0x247214))return fail(d,Error::value,"ParticleCode pass count");
        const auto* program=section->find("microProgramName");const auto* group=section->find("ShaderPassGroup");
        if(passes!=1||enabled!=1||maps!=2||!program||program->text!="ParticleCode"||
            (group&&!same(group->text,"none")))return fail(d,Error::profile,"actual Main ParticleCode",0x2474a0);
        for(const auto& field:section->values)
            if(field.sourceValuePresent&&!known(field.key))return fail(d,Error::unsupportedField,field.key.c_str());
        if(texture.revision!=1||texture.flags||texture.kind>1||texture.mips.size()!=1)
            return fail(d,Error::texture,garage?"actual single indexed garageprt mip":"actual single indexed mmprt mip");
        const auto& mip=texture.mips[0];
        if((!garage&&(mip.width!=128||mip.height!=256))||
           (garage&&(mip.width<16||mip.height<16||mip.width>1024||mip.height>1024||
              (mip.width&(mip.width-1))||(mip.height&(mip.height-1))))||
           mip.rgba.size()!=std::uint64_t(mip.width)*mip.height*4u)
            return fail(d,Error::texture,garage?"actual bounded garageprt atlas":"mmprt128x256");
        // Real mmprt contains alpha255. Preserve full authored bytes; native
        // draw admission bounds textureAlpha*vertexAlpha, not each separately.
        OriginalUiMainParticleMaterial50KG next;
        next.identity=data.identity();next.templateIdentity=inherited->identity;
        next.textureIdentity=archive.find(stem+".pst").identity;
        if(!next.identity||!next.textureIdentity)return fail(d,Error::archive,"mmprt identities");
        for(unsigned state=0;state<2;++state){
            const auto suffix="_"+std::to_string(state+1);
            const auto* map=section->find("TextureMap"+suffix);std::int32_t uv=0;
            if(!map||!same(map->text,textureName)||
                !integer(("TextureCoordinateIndex"+suffix).c_str(),0,uv,0x247338)||uv!=0)
                return fail(d,Error::textureName,"actual mmprt map/UV0");
            auto gs=inherited->gs;
            for(const auto& spec:specs){
                auto& v=gs.fields[static_cast<std::size_t>(spec.field)];
                const auto name=key(spec,state);
                if(spec.field==Field::inputColorD){
                    const auto alias="inputAlphaD"+suffix;
                    if(!integer(alias.c_str(),v.value,v.value,state?0x2489f8:0x2488dc))return false;
                }
                if(!integer(name.c_str(),v.value,v.value,spec.pc[state]))return false;
                if(const auto* field=section->find(name))v={v.value,data.identity(),field->valueOffset,spec.pc[state]};
            }
            const auto expect=[&](Field field,std::int32_t value){
                return gs[field].value==value||fail(d,Error::unsupportedValue,originalUiGsFieldName50KG(field),gs[field].sourcePC);
            };
            // Actual Main fixed subset. Tests/fog are implemented by the
            // separate native particle mode, rather than discarded as UI state.
            if(!expect(Field::textureColorComponent,1)||!expect(Field::textureFunction,0)||
                !expect(Field::colorClampingMethod,1)||!expect(Field::alphaCorrectionValue,0)||
                !expect(Field::alphaBlendingControlInUnitsOfPixels,0)||!expect(Field::gouraudShadingOnOff,1)||
                !expect(Field::textureMappingOnOff,1)||!expect(Field::foggingOnOff,1)||
                !expect(Field::alphaBlendingOnOff,1)||!expect(Field::onePassAntiAliasingOnOff,0)||
                !expect(Field::textureCoordinatesSTQorUV,0)||!expect(Field::fragmentValueControlFixedOrNot,0)||
                !expect(Field::alphaTestOnOff,1)||!expect(Field::alphaTestMethod,6)||
                !expect(Field::alphaReferenceValue,0)||!expect(Field::processingMethodWhenFailedInAlphaTest,0)||
                !expect(Field::destinationAlphaTestOnOff,0)||!expect(Field::depthTestMethod,2)||
                !expect(Field::zWriteDisabled,1)||!expect(Field::inputColorA,0)||
                !expect(Field::inputColorB,state?2:1)||!expect(Field::inputAlphaC,0)||
                !expect(Field::inputColorD,1)||!expect(Field::wrapModeHorizontal,0)||!expect(Field::wrapModeVertical,0))
                return false;
            const char* names[]={"lodCalculationMethod","maximumMipLevel","magFilter","minFilter","L","K"};
            constexpr std::int32_t defaults[]={0,0,1,5,0,-75};
            for(unsigned n=0;n<6;++n){std::int32_t value=defaults[n];
                if(!integer((std::string(names[n])+suffix).c_str(),defaults[n],value)||value!=defaults[n])
                    return fail(d,Error::unsupportedValue,"actual Main TEX1 sampler",state?0x24863c:0x24853c);
            }
            gs.fields[static_cast<std::size_t>(Field::environmentContext0or1)]={
                static_cast<std::int32_t>(state),nullptr,0,state?0x248FCCu:0x248E6Cu};
            next.sourceStates[state]=gs;
            for(unsigned i=0;i<6;++i)next.sourceSamplers[state][i]=defaults[i];
            auto& policy=next.states[state];
            policy.blend=state?OriginalUiMaterialBlend50KG::additive:OriginalUiMaterialBlend50KG::sourceAlpha;
            policy.minification=policy.magnification=OriginalUiMaterialFilter50KG::linear;
            policy.u=policy.v=OriginalUiMaterialWrap50KG::repeat;
            policy.depthMethod=2;policy.depthWrite=false;policy.environmentContext=static_cast<std::uint8_t>(state);
            policy.requiresOriginalDepthOwner=true;policy.requiresSeparateAlphaStore=true;
        }
        next.fogEnabled=true;next.alphaTestGreaterZero=true;next.noDestinationAlphaConsumer=true;
        output=next;d={};return true;
    }catch(...){return fail(d,Error::allocation,"Main particle material");}
}
bool prepareOriginalUiMainParticleMaterial50KG(const OriginalUiArchive50KG& archive,
    const OriginalUiMaterialTemplates50KG& templates,const OriginalUiMaterialRuntime50KG& runtime,
    const Texture48E& texture,OriginalUiMainParticleMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    return prepareParticleMaterial(archive,templates,runtime,texture,output,d,OriginalUiArchiveId50KG::mainMenu);
}
bool prepareOriginalUiOptionsParticleMaterial50KG(const OriginalUiArchive50KG& archive,
    const OriginalUiMaterialTemplates50KG& templates,const OriginalUiMaterialRuntime50KG& runtime,
    const Texture48E& texture,OriginalUiMainParticleMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    return prepareParticleMaterial(archive,templates,runtime,texture,output,d,OriginalUiArchiveId50KG::options);
}
bool prepareOriginalUiGarageParticleMaterial50KG(const OriginalUiArchive50KG& archive,
    const OriginalUiMaterialTemplates50KG& templates,const OriginalUiMaterialRuntime50KG& runtime,
    const Texture48E& texture,OriginalUiMainParticleMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    return prepareParticleMaterial(archive,templates,runtime,texture,output,d,OriginalUiArchiveId50KG::garage);
}

}
