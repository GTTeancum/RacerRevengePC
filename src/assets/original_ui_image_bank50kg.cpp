#include "original_ui_image_bank50kg.h"
#include <limits>
#include <utility>
namespace rr::assets {
namespace {
using E=OriginalUiImageBankError50KG;using D=OriginalUiImageBankDiagnostic50KG;
bool fail(D& d,E e) noexcept {d.error=e;return false;}
struct Located {OriginalUiMemberView50KG view;std::size_t globalOrdinal=0;bool loose=false;bool addon=false;std::size_t addonArchive=0;};
bool locate(const OriginalUiImageBankInput50KG& input,const OriginalUiMemberIdentity50KG* identity,
    Located& out) noexcept {
    if(!identity)return false;
    for(const auto& archive:input.archives)
        for(std::size_t i=0;i<archive->memberCount();++i){
            const auto view=archive->member(i);
            if(view.identity==identity){out={view,archive->identity()->firstMember+i};return true;}
        }
    // PORT EXTENSION (DLC add-on courses): a port-owned loose package file.
    for(std::size_t i=0;i<input.looseMembers.size();++i)
        if(input.looseMembers[i].identity==identity){out={input.looseMembers[i],i,true,false,0};return true;}
    // PORT EXTENSION (DLC): an add-on archive member; its ordinal is local to its archive.
    for(std::size_t a=0;a<input.addonArchives.size();++a){const auto& archive=input.addonArchives[a];
        for(std::size_t i=0;i<archive->memberCount();++i){
            const auto view=archive->member(i);
            if(view.identity==identity){out={view,i,false,true,a};return true;}
        }
    }
    return false;
}
bool same(const OriginalUiImageSource50KG& a,const OriginalUiImageSource50KG& b) noexcept {
    return a.member.identity==b.member.identity&&a.pstOffset==b.pstOffset&&
        a.pstBytes==b.pstBytes&&a.cel.identity==b.cel.identity;
}
bool samePixels(const Texture48E& a,const Texture48E& b) noexcept {
    if(a.revision!=b.revision||a.flags!=b.flags||a.kind!=b.kind||
        a.original_palette!=b.original_palette||a.base_rrtex!=b.base_rrtex||a.mips.size()!=b.mips.size())return false;
    for(std::size_t i=0;i<a.mips.size();++i){const auto& x=a.mips[i];const auto& y=b.mips[i];
        if(x.width!=y.width||x.height!=y.height||x.source_header_offset!=y.source_header_offset||
            x.source_indices_offset!=y.source_indices_offset||x.rgba!=y.rgba)return false;
    }
    return true;
}
}
struct OriginalUiImageBank50KG::Storage {
    std::vector<std::shared_ptr<const OriginalUiArchive50KG>> archives,addonArchives;
    std::vector<std::shared_ptr<const OriginalUiScreenAssets50KG>> owners;
    std::shared_ptr<OriginalUiScreenAssets50KG> images;
    std::vector<std::vector<OriginalUiImageBankMapping50KG>> mappings;
    std::vector<OriginalUiMemberView50KG> looseMembers; // PORT EXTENSION (DLC), empty for stock
    std::shared_ptr<const void> looseOwner;
};
OriginalUiImageBank50KG::OriginalUiImageBank50KG() noexcept=default;
OriginalUiImageBank50KG::~OriginalUiImageBank50KG()=default;
bool OriginalUiImageBank50KG::load(const OriginalUiImageBankInput50KG& input,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyLoaded);
    using AddonLimits=OriginalUiImageBankAddonLimits50DL;
    if(input.archives.empty()||input.archives.size()>11||input.owners.empty()||
       input.addonArchives.size()>AddonLimits::archives||
       input.owners.size()>128+3*input.addonArchives.size()) // pod, character and page owners per add-on archive on top of the stock 128
        return fail(d,E::input);
    try {
        for(std::size_t i=0;i<input.addonArchives.size();++i){const auto& a=input.addonArchives[i];
            if(!a||!a->loaded()||!a->addon()||!a->identity()||a->memberCount()>AddonLimits::members)return fail(d,E::input);
            for(std::size_t j=0;j<i;++j)if(input.addonArchives[j]->identity()==a->identity())return fail(d,E::input);
        }
        std::size_t stockSources=0,addonSources=0;
        for(std::size_t i=0;i<input.archives.size();++i){const auto& a=input.archives[i];
            if(!a||!a->loaded()||!a->identity()||
                a->identity()!=originalUiArchiveIdentity50KG(a->identity()->id))return fail(d,E::input);
            for(std::size_t j=0;j<i;++j)if(input.archives[j]->identity()==a->identity())return fail(d,E::input);
        }
        // PORT EXTENSION (DLC add-on courses): loose members are complete views
        // with port-owned identities, pinned by looseOwner. Bounded count.
        if(input.looseMembers.size()>64||(!input.looseMembers.empty()&&!input.looseOwner))return fail(d,E::input);
        for(const auto& m:input.looseMembers){
            if(!m.identity||!m.data||!m.size||m.size!=m.identity->bytes)return fail(d,E::input);
            for(const auto& a:input.archives)for(std::size_t i=0;i<a->memberCount();++i)
                if(a->member(i).identity==m.identity)return fail(d,E::input); // never a catalog identity
        }
        auto next=std::make_unique<Storage>();next->archives=input.archives;next->owners=input.owners;
        next->looseMembers=input.looseMembers;next->looseOwner=input.looseOwner;
        next->addonArchives=input.addonArchives;
        next->mappings.resize(input.owners.size());std::vector<OriginalUiImageSource50KG> sources;
        for(std::size_t owner=0;owner<input.owners.size();++owner){
            d.owner=static_cast<std::uint32_t>(owner);const auto& bank=input.owners[owner];
            if(!bank||!bank->loaded()||!bank->textures()||bank->textures()->textureCount()>256)
                return fail(d,E::input);
            auto& mappings=next->mappings[owner];mappings.resize(bank->textures()->textureCount());
            for(std::uint32_t image=0;image<mappings.size();++image){
                d.image=image;const auto* provenance=bank->sourceProvenance(image);
                const auto* texture=bank->textures()->texture(image);Located member{},cel{};
                if(!provenance||!texture||!bank->decodedTexture(image)||
                    !locate(input,provenance->memberIdentity,member)||
                    (provenance->celIdentity&&!locate(input,provenance->celIdentity,cel)))return fail(d,E::provenance);
                if(provenance->pstOffset>std::numeric_limits<std::uint32_t>::max())return fail(d,E::capacity);
                if(member.addon){
                    // PORT EXTENSION (DLC): add-on images never carry a CEL here.
                    if(cel.view.identity||member.addonArchive>=AddonLimits::archives||
                       member.globalOrdinal>=AddonLimits::members)return fail(d,E::capacity);
                }else if(member.globalOrdinal>=32767||(cel.view.identity&&cel.addon))return fail(d,E::capacity);
                OriginalUiImageSource50KG source{};source.member=member.view;source.cel=cel.view;
                source.pstOffset=provenance->pstOffset;source.pstBytes=provenance->pstBytes;
                // Native namespace0x5549. 15-bit globally unique member ordinal,
                // one CEL metadata bit, then original stored-PST byte offset.
                // PORT EXTENSION: add-on namespace 0x41 in the top byte, 7-bit
                // add-on archive index (bits 49..55), 10-bit member index (39..48).
                source.identity=member.addon?
                    (0x4100000000000000ull|((member.addonArchive+1ull)<<49)|((member.globalOrdinal+1ull)<<39)|source.pstOffset):
                    (0x5549000000000000ull|((member.globalOrdinal+1ull)<<33)|
                    (std::uint64_t(bool(source.cel.identity))<<32)|source.pstOffset);
                // PORT EXTENSION (DLC add-on courses): loose package files take namespace 0x554C.
                if(member.loose){
                    if(cel.view.identity)return fail(d,E::provenance);
                    source.identity=0x554C000000000000ull|((member.globalOrdinal+1ull)<<33)|source.pstOffset;
                }
                std::size_t index=0;for(;index<sources.size();++index){
                    if(same(source,sources[index]))break;
                    if(source.identity==sources[index].identity)return fail(d,E::identityConflict);
                }
                if(index==sources.size()){
                    // Stock keeps its own 256; add-on images have their own capacity.
                    if(member.addon?addonSources==AddonLimits::sources:stockSources==256)return fail(d,E::capacity);
                    ++(member.addon?addonSources:stockSources);
                    sources.push_back(source);
                }
                mappings[image]={static_cast<std::uint32_t>(index),texture->identity,source.identity};
            }
        }
        if(sources.empty())return fail(d,E::input);
        next->images=std::make_shared<OriginalUiScreenAssets50KG>();
        if(!next->images->load(sources,d.images,input.ipumCache))return fail(d,E::decode);
        for(std::size_t owner=0;owner<input.owners.size();++owner){
            d.owner=static_cast<std::uint32_t>(owner);
            for(std::uint32_t i=0;i<next->mappings[owner].size();++i){d.image=i;
                const auto& mapping=next->mappings[owner][i];
                const auto* a=input.owners[owner]->decodedTexture(i);
                const auto* b=next->images->decodedTexture(mapping.combinedIndex);
                if(!a||!b||!samePixels(*a,*b))return fail(d,E::pixels);
            }
        }
        storage_=std::move(next);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiImageBank50KG::loaded()const noexcept{return bool(storage_);}
std::shared_ptr<const OriginalUiScreenAssets50KG> OriginalUiImageBank50KG::images()const noexcept {
    return storage_?storage_->images:nullptr;
}
const OriginalUiImageBankMapping50KG* OriginalUiImageBank50KG::mapping(std::uint32_t owner,std::uint32_t image)const noexcept {
    return storage_&&owner<storage_->mappings.size()&&image<storage_->mappings[owner].size()?
        &storage_->mappings[owner][image]:nullptr;
}
OriginalUiMemberView50KG OriginalUiImageBank50KG::sourceMember(std::uint32_t index)const noexcept {
    const auto* source=storage_&&storage_->images?storage_->images->sourceProvenance(index):nullptr;
    if(!source||!source->memberIdentity)return {};
    for(const auto& archive:storage_->archives)for(std::size_t i=0;i<archive->memberCount();++i){
        const auto member=archive->member(i);if(member.identity==source->memberIdentity)return member;
    }
    for(const auto& member:storage_->looseMembers)if(member.identity==source->memberIdentity)return member;
    for(const auto& archive:storage_->addonArchives)for(std::size_t i=0;i<archive->memberCount();++i){
        const auto member=archive->member(i);if(member.identity==source->memberIdentity)return member;
    }
    return {};
}

}
