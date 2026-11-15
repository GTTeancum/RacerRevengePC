#pragma once
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/assets/original_standard_material_table50la.h"
#include "../../src/assets/original_standard_texture_resource50la.h"
#include <cstring>
#include <utility>

namespace original_racer_material_sources50LA {
namespace a=rr::assets;
using Provider=a::OriginalStandardArchiveProvider50LA;
using Materials=std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>>;
using Textures=std::vector<a::OriginalStandardTextureSourceInput50LA>;
enum class Status : std::uint8_t {complete,input,unsealed,model,reference,missingMaterial,
    document,unsupported,missingTexture,conflict,allocation};
struct Diagnostic {
    Status status=Status::complete;
    std::uint32_t sourcePC=0;
    std::size_t modelSlot=0,pass=0,map=0;
    std::string member;
    a::OriginalUiPageDiagnostic50KG document{};
};
namespace detail {
inline bool fail(Diagnostic& d,Status s,std::uint32_t pc){d.status=s;d.sourcePC=pc;return false;}
inline bool ascii(const std::string& s,std::size_t cap,bool empty=false){
    if(s.size()>cap||(!empty&&s.empty()))return false;
    for(unsigned char c:s)if(c<32||c>=127)return false;
    return true;
}
inline bool sameBytes(const Provider::MemberView& view,const std::vector<std::uint8_t>& bytes){
    return view.data&&view.size==bytes.size()&&(!view.size||std::memcmp(view.data,bytes.data(),view.size)==0);
}
inline bool integer(const a::OriginalUiDatSection50KG& s,const std::string& key,
    std::int32_t fallback,std::int32_t& value,Diagnostic& d,std::uint32_t pc){
    return a::originalUiInteger50KG(s,key,fallback,value,d.document,pc)||fail(d,Status::document,pc);
}
inline std::string withExtension(std::string member,const char* extension){
    const auto dot=member.find_last_of('.');if(dot!=std::string::npos)member.resize(dot);
    member+=extension;return member;
}
inline bool texture(const Provider& provider,const std::string& requested,Textures& textures,Diagnostic& d){
    // Exact source24DA30 priority. Only the full sealed native mount provider
    // may return absent; an omitted input vector entry never does so.
    Provider::MemberView view;
    // 24DADC..24DC30: .pst, .ipu (refused: no IPU decoder), .tga, then .atm.
    // The one place that defines the order (racers, pending track rows).
    for(const char* extension:{".pst",".ipu",".tga",".atm"}){
        const auto candidate=withExtension(requested,extension);d.member=candidate;
        const auto found=provider.lookup132820(candidate,view);
        if(found==Provider::Lookup::unknown)return fail(d,Status::unsealed,0x132820);
        if(found==Provider::Lookup::absent)continue;
        if(std::strcmp(extension,".ipu")==0)return fail(d,Status::unsupported,0x24DB70);
        if(!view.member||!view.data||!view.size)return fail(d,Status::input,0x132820);
        for(const auto& old:textures)if(Provider::sameName(old.member,*view.member))
            return sameBytes(view,old.bytes)||fail(d,Status::conflict,0x132820);
        textures.push_back({*view.member,{view.data,view.data+view.size}});return true;
    }
    return fail(d,Status::missingTexture,0x24DC48);
}
inline bool material(const Provider& provider,const std::string& canonical,
    Materials& materials,Textures& textures,Diagnostic& d){
    const auto requested=withExtension(canonical,".psm");d.member=requested;
    Provider::MemberView view;
    const auto found=provider.lookup132820(requested,view);
    if(found==Provider::Lookup::unknown)return fail(d,Status::unsealed,0x132820);
    if(found!=Provider::Lookup::present||!view.member||!view.data||!view.size)
        return fail(d,Status::missingMaterial,0x29B628);
    std::shared_ptr<const a::OriginalStandardMaterialSource50LA> source;
    for(const auto& old:materials)if(old&&old->member()&&Provider::sameName(*old->member(),*view.member)){
        if(source||!old->prepared()||!old->originalBytes()||!sameBytes(view,*old->originalBytes()))
            return fail(d,Status::conflict,0x29B628);
        source=old;
    }
    if(!source){
        auto prepared=std::make_shared<a::OriginalStandardMaterialSource50LA>();
        const std::vector<std::uint8_t> bytes(view.data,view.data+view.size);
        if(!prepared->prepare(*view.member,bytes,d.document))return fail(d,Status::document,0x29B628);
        source=std::move(prepared);materials.push_back(source);
    }
    const auto* general=source->section("General");std::int32_t passes=0;
    if(!general||!integer(*general,"TotalShaderPasses",0,passes,d,0x29B718))
        return fail(d,Status::document,0x29B718);
    if(passes<1||passes>4)return fail(d,Status::unsupported,0x29B718);
    const auto slash=source->member()->find_last_of('\\');
    const auto prefix=slash==std::string::npos?std::string{}:source->member()->substr(0,slash+1);
    for(std::int32_t p=0;p<passes;++p){
        d.pass=static_cast<std::size_t>(p);d.map=0;
        const auto* section=source->section("ShaderPass_"+std::to_string(p+1));std::int32_t maps=1;
        if(!section||!integer(*section,"totalTextureMaps",1,maps,d,0x247214))
            return fail(d,Status::document,0x247214);
        if(maps<0||maps>2)return fail(d,Status::unsupported,0x247214);
        // Disabled passes and duplicate slots still own their actual maps.
        // This assembler deduplicates immutable payloads only, never references.
        for(std::int32_t m=0;m<maps;++m){
            d.map=static_cast<std::size_t>(m);const auto* field=section->find("TextureMap_"+std::to_string(m+1));
            std::string name=field?field->text:std::string{};
            if(name.empty()){
                auto begin=canonical.find_last_of("\\/");begin=begin==std::string::npos?0:begin+1;
                if(begin==0&&canonical.size()>1&&canonical[1]==':')begin=2;
                auto end=canonical.find_last_of('.');if(end==std::string::npos||end<begin)end=canonical.size();
                name=canonical.substr(begin,end-begin);
            }
            if(!ascii(name,255)||Provider::sameName(name,"RenderTarget")||
               name.find_first_of("\\/.:\"")!=std::string::npos||prefix.size()+name.size()>255)
                return fail(d,Status::unsupported,0x2472A8);
            if(!texture(provider,prefix+name,textures,d))return false;
        }
    }
    return true;
}
}

// A whole actual model resource and its genuine26E640 handles are the
// input. No manifest material list, LOD0 projection, new model, numeric retain,
// or guessed readiness is created. Output append is transactional and preserves
// existing source order/bytes. The actual table/population owns all behavior.
inline bool appendModelSources(const a::OriginalStandardModelResource50LA& resource,
    const a::OriginalStandardMaterialReferences50LA& references,const std::string& actualDirectory36B970,
    const Provider& provider,Materials& materials,Textures& textures,Diagnostic& d)noexcept {
    d={};
    if(!provider.sealed())return detail::fail(d,Status::unsealed,0x132820);
    const auto revision=provider.revision();const auto* scene=resource.scene();
    const auto* count=references.count1C0();const auto* table=references.table382B38();
    if(!resource.prepared()||!resource.member()||!resource.originalBytes()||!scene||
       !references.complete()||references.released26E4C0()||!count||*count!=scene->materials.size()||
       !table||!table->prepared()||!detail::ascii(actualDirectory36B970,255,true))
        return detail::fail(d,Status::input,0x26E640);
    if(table->populationTerminal()||table->demandTerminal())return detail::fail(d,Status::reference,0x14D370);
    try {
        Provider::MemberView model;
        if(provider.lookup132820(*resource.member(),model)!=Provider::Lookup::present||
           !detail::sameBytes(model,*resource.originalBytes()))return detail::fail(d,Status::model,0x132820);
        auto nextMaterials=materials;auto nextTextures=textures;
        for(std::size_t i=0;i<scene->materials.size();++i){
            d.modelSlot=i;const auto& name=scene->materials[i];
            if(!detail::ascii(name,63))return detail::fail(d,Status::input,0x26E6A4);
            std::string canonical=actualDirectory36B970;
            if(!canonical.empty()&&canonical.back()!='\\')canonical+='\\';
            canonical+=name;
            if(canonical.size()>255)return detail::fail(d,Status::input,0x26E70C);
            const auto* handle=references.handle1C4(i);a::OriginalStandardMaterialRowView50LA row;
            if(!handle||!table->row(*handle,row)||!row.name00||!row.references08||*row.references08<=0||
               !Provider::sameName(canonical,*row.name00))return detail::fail(d,Status::reference,0x26E7C8);
            if(!detail::material(provider,*row.name00,nextMaterials,nextTextures,d))return false;
        }
        if(!provider.sealed()||provider.revision()!=revision)return detail::fail(d,Status::unsealed,0x132820);
        materials.swap(nextMaterials);textures.swap(nextTextures);d.status=Status::complete;return true;
    }catch(...){return detail::fail(d,Status::allocation,0x29B628);}
}
// Existing right callers retain the identical whole-resource closure.
inline bool appendRightModelSources(const a::OriginalStandardModelResource50LA& resource,
    const a::OriginalStandardMaterialReferences50LA& references,const std::string& actualDirectory36B970,
    const Provider& provider,Materials& materials,Textures& textures,Diagnostic& d)noexcept {
    return appendModelSources(resource,references,actualDirectory36B970,provider,materials,textures,d);
}
}
