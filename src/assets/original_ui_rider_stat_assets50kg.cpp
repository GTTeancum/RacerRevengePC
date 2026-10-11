#include "original_ui_rider_stat_assets50kg.h"
#include <utility>
namespace rr::assets {
namespace {
using E=OriginalUiRiderStatAssetError50KG;using D=OriginalUiRiderStatAssetDiagnostic50KG;
bool fail(D& d,E e) noexcept {d.error=e;return false;}
bool authentic(const std::shared_ptr<const OriginalUiArchive50KG>& a,OriginalUiArchiveId50KG id) noexcept {
    return a&&a->loaded()&&a->identity()==originalUiArchiveIdentity50KG(id);
}
char fold(char c) noexcept {return c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;return true;
}
bool portraitIndex(unsigned r) noexcept {return r>=1&&r<=26&&r!=5&&r!=6&&r!=13;}
std::string portraitName(unsigned r){std::string n="00_flrg";n[0]=char('0'+r/10);n[1]=char('0'+r%10);return n;}
constexpr const char* names[]={"podstat","podstat2","notch","medsmicn","medsmgld","medsmslv","medsmbrz","medstrsm","denmrk14"};
}
struct OriginalUiRiderStatAssets50KG::Storage {
    std::shared_ptr<const OriginalUiArchive50KG> rider,fonts,appStart;
    std::shared_ptr<OriginalUiScreenAssets50KG> images;
    std::array<OriginalUiRiderStatResource50KG,9> resources;
    std::array<OriginalUiRiderStatResource50KG,27> portraits;
    std::array<std::string,6> labels;
    const OriginalUiMemberIdentity50KG* localizedText=nullptr;
};
OriginalUiRiderStatAssets50KG::OriginalUiRiderStatAssets50KG() noexcept=default;
OriginalUiRiderStatAssets50KG::~OriginalUiRiderStatAssets50KG()=default;
bool OriginalUiRiderStatAssets50KG::load(std::shared_ptr<const OriginalUiArchive50KG> rider,
    std::shared_ptr<const OriginalUiArchive50KG> fonts,std::shared_ptr<const OriginalUiArchive50KG> app,
    const OriginalUiEnglish50KG& english,const OriginalUiMaterialRuntime50KG& runtime,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyLoaded);
    if(!authentic(rider,OriginalUiArchiveId50KG::rider)||!authentic(fonts,OriginalUiArchiveId50KG::fonts)||
        !authentic(app,OriginalUiArchiveId50KG::appStart))return fail(d,E::archive);
    if(!english.loaded()||!runtime.overrideKnown)return fail(d,E::localization);
    try {
        auto next=std::make_unique<Storage>();next->rider=std::move(rider);next->fonts=std::move(fonts);next->appStart=std::move(app);
        next->localizedText=english.textIdentity();
        for(std::size_t i=0;i<6;++i){const auto* entry=english.at(200+i);
            if(!entry||entry->text.size()>20)return fail(d,E::localization);next->labels[i]=entry->text;}
        OriginalUiMaterialTemplates50KG templates;
        if(!templates.load(*next->appStart,d.material))return fail(d,E::material);
        std::vector<OriginalUiImageSource50KG> sources;
        const auto populate=[&](OriginalUiRiderStatResource50KG& resource,const std::string& name,
            bool font,bool animated)->bool {
            resource.registeredName=(font?"font/":"ui/textures/")+name;resource.animated=animated;
            // Every original stat constructor resolves this very same named map.
            // All declared passes remain available to the eventual packet owner.
            if(resource.material.passes.empty())return fail(d,E::material);
            for(const auto& pass:resource.material.passes)
                if(pass.textureCount!=1||!same(pass.textureName,name))return fail(d,E::material);
            const auto& archive=font?*next->fonts:*next->rider;
            const auto member=archive.find(resource.registeredName+(animated?".atm":".pst"));
            if(!member.identity||!member.data)return fail(d,E::member);
            const auto append=[&](std::size_t offset,std::size_t bytes,OriginalUiMemberView50KG cel)->bool {
                if(sources.size()>=256)return fail(d,E::images);
                const auto i=static_cast<std::uint32_t>(sources.size());
                // Native owner-local ordinal, never a globally interchangeable ID.
                sources.push_back({0x5253000000000000ull|std::uint64_t(i+1),member,offset,bytes,cel});
                resource.images.push_back(i);return true;
            };
            if(animated){OriginalUiAtm50KG atm;
                if(!splitOriginalUiAtm50KG(member.data,member.size,atm,d.atm)||atm.frames.empty())return fail(d,E::animation);
                for(const auto& frame:atm.frames)if(!append(frame.pstOffset,frame.pstBytes,{}))return false;
            }else{const auto cel=font?archive.find(resource.registeredName+".cel"):OriginalUiMemberView50KG{};
                if(font&&!cel.identity)return fail(d,E::member);if(!append(0,member.size,cel))return false;}
            return true;
        };
        for(std::size_t i=0;i<9;++i){d.object=static_cast<std::uint32_t>(i);auto& r=next->resources[i];
            if(i<3){if(!readOriginalUiRiderStatFallbackMaterial50KG(*next->rider,static_cast<OriginalUiRiderStatTexture50KG>(i),
                templates,OriginalUiTemplate50KG::screenMt,runtime,r.material,d.material))return fail(d,E::material);
            }else if(!readOriginalUiMaterial50KG(*next->rider,std::string(i==8?"font/":"ui/textures/")+names[i]+".psm",
                templates,OriginalUiTemplate50KG::screenMt,runtime,r.material,d.material))return fail(d,E::material);
            if(!populate(r,names[i],i==8,i==7))return false;
        }
        for(std::uint8_t riderIndex=1;riderIndex<=26;++riderIndex){if(!portraitIndex(riderIndex))continue;
            d.object=0x100u+riderIndex;auto& r=next->portraits[riderIndex];
            if(!readOriginalUiRiderPortraitFallbackMaterial50KG(*next->rider,riderIndex,templates,OriginalUiTemplate50KG::screenMt,
                runtime,r.material,d.material))return fail(d,E::material);
            if(!populate(r,portraitName(riderIndex),false,false))return false;
        }
        next->images=std::make_shared<OriginalUiScreenAssets50KG>();
        if(!next->images->load(sources,d.images))return fail(d,E::images);
        const auto fontIndex=next->resources[8].images[0];
        if(!next->images->font(fontIndex))return fail(d,E::images);
        storage_=std::move(next);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderStatAssets50KG::loaded()const noexcept{return bool(storage_);}
const OriginalUiRiderStatResource50KG* OriginalUiRiderStatAssets50KG::resource(OriginalUiRiderStatAsset50KG id)const noexcept {
    const auto i=static_cast<std::size_t>(id);return storage_&&i<9?&storage_->resources[i]:nullptr;
}
const OriginalUiRiderStatResource50KG* OriginalUiRiderStatAssets50KG::portrait(std::uint8_t r)const noexcept {
    return storage_&&portraitIndex(r)?&storage_->portraits[r]:nullptr;
}
const std::string* OriginalUiRiderStatAssets50KG::label(std::uint8_t stat)const noexcept {
    return storage_&&stat<6?&storage_->labels[stat]:nullptr;
}
const OriginalUiMemberIdentity50KG* OriginalUiRiderStatAssets50KG::localizedTextSource()const noexcept {
    return storage_?storage_->localizedText:nullptr;
}
std::shared_ptr<const OriginalUiScreenAssets50KG> OriginalUiRiderStatAssets50KG::images()const noexcept {
    return storage_?storage_->images:nullptr;
}
}
