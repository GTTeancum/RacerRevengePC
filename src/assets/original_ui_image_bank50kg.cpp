#include "original_ui_image_bank50kg.h"
#include <limits>
#include <utility>
namespace rr::assets {
namespace {
using E=OriginalUiImageBankError50KG;using D=OriginalUiImageBankDiagnostic50KG;
bool fail(D& d,E e) noexcept {d.error=e;return false;}
struct Located {OriginalUiMemberView50KG view;std::size_t globalOrdinal=0;};
bool locate(const OriginalUiImageBankInput50KG& input,const OriginalUiMemberIdentity50KG* identity,
    Located& out) noexcept {
    if(!identity)return false;
    for(const auto& archive:input.archives)
        for(std::size_t i=0;i<archive->memberCount();++i){
            const auto view=archive->member(i);
            if(view.identity==identity){out={view,archive->identity()->firstMember+i};return true;}
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
    std::vector<std::shared_ptr<const OriginalUiArchive50KG>> archives;
    std::vector<std::shared_ptr<const OriginalUiScreenAssets50KG>> owners;
    std::shared_ptr<OriginalUiScreenAssets50KG> images;
    std::vector<std::vector<OriginalUiImageBankMapping50KG>> mappings;
};
OriginalUiImageBank50KG::OriginalUiImageBank50KG() noexcept=default;
OriginalUiImageBank50KG::~OriginalUiImageBank50KG()=default;
bool OriginalUiImageBank50KG::load(const OriginalUiImageBankInput50KG& input,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyLoaded);
    if(input.archives.empty()||input.archives.size()>11||input.owners.empty()||input.owners.size()>128)
        return fail(d,E::input);
    try {
        for(std::size_t i=0;i<input.archives.size();++i){const auto& a=input.archives[i];
            if(!a||!a->loaded()||!a->identity()||
                a->identity()!=originalUiArchiveIdentity50KG(a->identity()->id))return fail(d,E::input);
            for(std::size_t j=0;j<i;++j)if(input.archives[j]->identity()==a->identity())return fail(d,E::input);
        }
        auto next=std::make_unique<Storage>();next->archives=input.archives;next->owners=input.owners;
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
                if(member.globalOrdinal>=32767||provenance->pstOffset>std::numeric_limits<std::uint32_t>::max())
                    return fail(d,E::capacity);
                OriginalUiImageSource50KG source{};source.member=member.view;source.cel=cel.view;
                source.pstOffset=provenance->pstOffset;source.pstBytes=provenance->pstBytes;
                // Native namespace0x5549. 15-bit globally unique member ordinal,
                // one CEL metadata bit, then original stored-PST byte offset.
                source.identity=0x5549000000000000ull|((member.globalOrdinal+1ull)<<33)|
                    (std::uint64_t(bool(source.cel.identity))<<32)|source.pstOffset;
                std::size_t index=0;for(;index<sources.size();++index){
                    if(same(source,sources[index]))break;
                    if(source.identity==sources[index].identity)return fail(d,E::identityConflict);
                }
                if(index==sources.size()){
                    if(sources.size()==256)return fail(d,E::capacity);
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
}
