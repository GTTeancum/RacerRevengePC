#include "original_ui_tournament_widget_resources50kg.h"
#include <cstring>
#include <utility>

namespace rr::platform {
namespace {
using D=OriginalUiTournamentWidgetResourceDiagnostic50KG;
using E=OriginalUiTournamentWidgetResourceError50KG;
using K=OriginalUiTournamentWidgetKind50KG;
using A=assets::OriginalUiArchiveId50KG;
constexpr auto missing=assets::unresolvedMaterialTexture50EA;
constexpr std::array<std::uint8_t,23> careers{{1,2,3,4,7,8,9,10,11,12,14,15,16,17,18,19,20,21,22,23,24,25,26}};
constexpr std::array<std::int32_t,9> popupIds{{16,24,22,23,17,18,21,25,26}};
bool fail(D& d,E error,std::uint32_t pc) noexcept {d.error=error;d.sourcePC=pc;return false;}
bool validId(std::uint16_t id) noexcept {return id&&id<originalUiTournamentWidgetSlots50KG;}
bool authentic(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,A id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
char fold(char c) noexcept {return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;
    return true;
}
bool sameSampler(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b) noexcept {
    return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
}
std::uint64_t imageIdentity(const assets::OriginalUiArchive50KG& archive,
    const assets::OriginalUiMemberView50KG& member) noexcept {
    const auto* a=archive.identity();if(!a||!member.identity)return 0;
    for(std::size_t i=0;i<a->memberCount;++i)
        if(assets::originalUiMemberIdentity50KG(a->id,i)==member.identity)
            return(std::uint64_t(static_cast<std::uint8_t>(a->id)+1u)<<56)|(std::uint64_t(i+1u)<<32);
    return 0;
}
K expectedKind(std::uint16_t id) noexcept {
    if(!validId(id))return K::absent;
    if(id==5||id==12||id==19||id==62)return K::empty;
    if(id==6)return K::movement;
    if(id>=27&&id<=30)return K::icon;
    if(id>=39&&id<=61)return K::userRotate;
    return K::text;
}
const char* typeName(K kind) noexcept {
    switch(kind){case K::empty:return "Empty";case K::movement:return "Movement_Control";
        case K::icon:return "Icon";case K::userRotate:return "User_Rotate";case K::text:return "Overlay_Text";
        default:return "";}
}
bool podDescriptor(const assets::OriginalUiDat50KG& dat,const assets::OriginalUiArchive50KG& archive,
    std::uint16_t id,OriginalUiTournamentPodDescriptor50KG& out,D& d) {
    const auto* section=dat.section("Widget "+std::to_string(id));
    if(!section||id<39||id>61)return fail(d,E::descriptor,0x1D31D0);
    const auto career=careers[id-39];std::string stem="00";
    stem[0]=char('0'+career/10);stem[1]=char('0'+career%10);
    const auto* slt=section->find("SLT");bool dynamic=false,onTop=false;
    if(!slt||slt->text!=stem+".slt"||
       !assets::originalUiBoolean50KG(*section,"DynamicLoad",false,dynamic,d.descriptor,0x1CD5C0)||!dynamic||
       !assets::originalUiBoolean50KG(*section,"OnTop",false,onTop,d.descriptor,0x1CD890)||onTop)
        return fail(d,E::descriptor,0x1CD600);
    OriginalUiTournamentPodDescriptor50KG p;p.identity=dat.identity();p.widget=id;p.career=career;p.slt=slt->text;
    auto& c=p.config;
    if(!section->find("Position")||!section->find("LookVector")||!section->find("UpVector")||
       !assets::originalUiTriple50KG(*section,"Position",{},c.position,d.descriptor,0x1D3224)||
       !assets::originalUiTriple50KG(*section,"LookVector",{},c.look,d.descriptor,0x1D3254)||
       !assets::originalUiTriple50KG(*section,"UpVector",{},c.up,d.descriptor,0x1D3290)||
       !assets::originalUiBoolean50KG(*section,"IsLit",false,c.lit,d.descriptor,0x1D32BC)||
       !assets::originalUiBoolean50KG(*section,"StartPaused",true,c.startPaused,d.descriptor,0x1D331C)||
       !assets::originalUiBoolean50KG(*section,"CleanFromMemory",false,c.cleanFromMemory,d.descriptor,0x1D337C)||
       !assets::originalUiBoolean50KG(*section,"EnableOnSelect",false,p.enableOnSelect,d.descriptor,0x2A77E0)||
       !assets::originalUiBoolean50KG(*section,"DisableOnSelect",false,p.disableOnSelect,d.descriptor,0x2A77E0)||
       !assets::originalUiBoolean50KG(*section,"EnableOnDeSelect",false,p.enableOnDeselect,d.descriptor,0x2A77E0)||
       !assets::originalUiBoolean50KG(*section,"DisableOnDeSelect",false,p.disableOnDeselect,d.descriptor,0x2A77E0))
        return fail(d,E::descriptor,0x1D31D0);
    constexpr const char* flags[]{"XRotate","YRotate","ZRotate"};
    constexpr const char* speeds[]{"XVelocity","YVelocity","ZVelocity"};
    for(unsigned i=0;i<3;++i)
        if(!assets::originalUiBoolean50KG(*section,flags[i],false,c.rotate[i],d.descriptor,0x1D32D4+0x18*i)||
           !assets::originalUiNumber50KG(*section,speeds[i],0,c.velocity[i],d.descriptor,0x1D3334+0x18*i))
            return fail(d,E::descriptor,0x1D31D0);
    for(unsigned i=0;i<2;++i)
        if(!assets::originalUiBoolean50KG(*section,i?"WantsJoystickInput2":"WantsJoystickInput1",false,
            p.wantsJoystickInput[i],d.descriptor,0x1CD8E0)||p.wantsJoystickInput[i])
            return fail(d,E::descriptor,0x1CD8E0);
    p.modelSource=archive.find("UI/Geometry/"+stem+".psg");
    if(!p.modelSource.identity||!p.modelSource.data||p.modelSource.size!=p.modelSource.identity->bytes)
        return fail(d,E::image,0x1D35C4);
    out=std::move(p);return true;
}
bool registry(const assets::OriginalUiMemberView50KG& table,
    std::array<OriginalUiTournamentWidgetMaterial50KG,originalUiTournamentWidgetMaterialCount50KG>& bindings) {
    if(!table.identity||!table.data||table.size!=70962)return false;
    std::array<bool,originalUiTournamentWidgetMaterialCount50KG> found{};
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    std::size_t cursor=0;std::uint32_t token=0;
    while(cursor<table.size){
        while(cursor<table.size&&delimiter(table.data[cursor]))++cursor;
        if(cursor==table.size)break;
        const auto start=cursor;while(cursor<table.size&&!delimiter(table.data[cursor]))++cursor;
        for(std::size_t i=0;i<bindings.size();++i){auto& b=bindings[i];
            if(b.registeredName.size()!=cursor-start)continue;
            std::size_t j=0;for(;j<b.registeredName.size()&&fold(b.registeredName[j])==fold(char(table.data[start+j]));++j){}
            if(j!=b.registeredName.size())continue;
            // These genuine rows occur once. Do not create a second token.
            if(found[i])return false;
            found[i]=true;b.registryToken=token;b.registryIdentity=table.identity;
        }
        ++token;
    }
    if(token!=3235)return false;
    for(bool f:found)if(!f)return false;
    return true;
}
}
struct OriginalUiTournamentWidgetResources50KG::Storage {
    OriginalUiTournamentWidgetColdInput50KG owners;
    assets::OriginalUiTournamentPageData50KG page;
    assets::OriginalUiMaterialTemplates50KG templates;
    std::array<K,originalUiTournamentWidgetSlots50KG> kinds{};
    std::array<assets::OriginalUiOverlayText50KG,originalUiTournamentWidgetSlots50KG> texts;
    std::array<assets::OriginalUiIcon50KG,originalUiTournamentWidgetSlots50KG> icons;
    std::array<OriginalUiTournamentPodDescriptor50KG,23> pods;
    assets::OriginalUiMovementControl50KG movement;
    std::vector<std::uint16_t> ordinary,top;
    std::array<OriginalUiTournamentWidgetMaterial50KG,originalUiTournamentWidgetMaterialCount50KG> materials;
    std::array<std::uint32_t,originalUiTournamentWidgetSlots50KG> widgetMaterials{};
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;
    assets::OriginalUiImageBankInput50KG bankInput;
    assets::OriginalUiImageBank50KG imageBank;
    std::vector<MaterialTextureSampler50EB> samplers;
    std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,originalUiTournamentWidgetSlots50KG> geometry;
    std::array<game::OriginalUiIconLayout50KG,4> layouts;
    std::array<std::vector<OriginalUiTournamentPopupTextClone50KG>,9> popupClones;
    std::array<OriginalUiTournamentPopupTexture50KG,7> popupTextures{};
    game::OriginalUiExitText50KG exitText;
};
OriginalUiTournamentWidgetResources50KG::OriginalUiTournamentWidgetResources50KG() noexcept=default;
OriginalUiTournamentWidgetResources50KG::~OriginalUiTournamentWidgetResources50KG()=default;
bool OriginalUiTournamentWidgetResources50KG::load(const OriginalUiTournamentWidgetColdInput50KG& in,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyLoaded,0x1CD160);
    if(!authentic(in.appStart,A::appStart)||!authentic(in.tournament,A::tournament)||
       !authentic(in.fonts,A::fonts)||!authentic(in.controllerPopup,A::controllerPopup)||!authentic(in.overlays,A::overlays)||
       !in.english||!in.english->loaded()||!in.runtime.overrideKnown||!in.video.lifetime||
       in.video.width!=640||in.video.height!=448||in.video.colorBits!=32||
       in.video.packetZMaximum!=0xffffff||in.video.zBufferFormat!=0x31)
        return fail(d,E::owner,0x1CD160);
    try {
        auto s=std::make_unique<Storage>();s->owners=in;s->owners.english=nullptr;s->widgetMaterials.fill(missing);
        if(!s->page.load(in.tournament,d.page))return fail(d,E::page,0x1EDAB8);
        const auto& page=*s->page.page();const auto& dat=*s->page.widgetsDat();
        if(page.widgetCount!=63||page.popupWidgets!=popupIds||page.controls.size()!=3||page.groups.size()!=3||
           !page.controls[0].widgets.empty()||
           page.controls[1].widgets!=std::vector<std::uint16_t>{1,2,3,7,37,30}||
           page.controls[2].widgets!=std::vector<std::uint16_t>{27,28,29,38,31,32,33,34,35,36,62,11,9,10})
            return fail(d,E::population,0x1D6DEC);
        for(std::size_t i=0;i<3;++i)
            if(page.groups[i].controls!=std::vector<std::int16_t>{static_cast<std::int16_t>(i)}||
               !page.groups[i].associations.empty()||!page.groups[i].keepEnabled)
                return fail(d,E::population,0x1D7180);
        for(std::uint16_t id=1;id<originalUiTournamentWidgetSlots50KG;++id){
            d.widget=id;const auto* section=dat.section("Widget "+std::to_string(id));
            const auto* type=section?section->find("Type"):nullptr;
            const auto kind=expectedKind(id);s->kinds[id]=kind;
            if(!type||type->text!=typeName(kind))return fail(d,E::population,0x1CD160);
            // Empty is a real null factory slot, not a disabled synthetic object.
            if(kind==K::empty)continue;
            bool top=false;if(!assets::originalUiBoolean50KG(*section,"OnTop",false,top,d.descriptor,0x1CD890)||top)
                return fail(d,E::descriptor,0x1CD890);
            if(kind==K::text){
                auto& text=s->texts[id];
                if(!assets::readOriginalUiOverlayText50KG(dat,id,in.english,text,d.descriptor)||
                   text.wantsJoystickInput[0]||text.wantsJoystickInput[1])return fail(d,E::descriptor,0x2A9000);
                const bool dynamic=id==9||id==10||id==11||id==37||id==38;
                if((text.textSource==assets::OriginalUiTextSource50KG::dynamic)!=dynamic)
                    return fail(d,E::descriptor,0x2A9000);
                if(dynamic){const auto capacity=id==11?80:id==37?40:35;
                    if(text.lines.size()!=1||!text.lines[0].empty()||text.dynamicLineLength!=capacity)
                        return fail(d,E::descriptor,0x2A9000);}
            }else if(kind==K::icon){
                auto& icon=s->icons[id];
                if(!assets::readOriginalUiIcon50KG(dat,id,icon,d.descriptor)||
                   icon.wantsJoystickInput[0]||icon.wantsJoystickInput[1])return fail(d,E::descriptor,0x2AC590);
            }else if(kind==K::movement){
                if(!assets::readOriginalUiMovementControl50KG(dat,id,s->movement,d.descriptor)||
                   s->movement.gridSize||!s->movement.wantsJoystickInput[0]||!s->movement.wantsJoystickInput[1])
                    return fail(d,E::descriptor,0x2AB010);
                for(std::size_t i=0;i<s->movement.mappings.size();++i)
                    if(s->movement.mappings[i]!=(i==0?-2:i==1?-1:0))return fail(d,E::descriptor,0x2AB010);
            }else if(!podDescriptor(dat,*in.tournament,id,s->pods[id-39],d))return false;
            s->ordinary.push_back(id);
        }
        if(s->ordinary.size()!=59)return fail(d,E::population,0x1CD9F8);
        // In particular Control2 retains62, while no descriptor or draw object
        // is manufactured for it. The eventual callback loop must skip nulls.
        for(const auto& control:page.controls)for(auto id:control.widgets)
            if(!validId(id)||(expectedKind(id)==K::empty&&id!=62))return fail(d,E::population,0x1D7138);
        if(!s->templates.load(*in.appStart,d.material))return fail(d,E::material,0x2DF10C);
        constexpr const char* names[]{"font/denmrk16top","font/venus16top","font/denmrk16",
            "font/denmrk14","ui/textures/overlay1","ui/textures/nameblur",
            "ui/textures/overlay1top","ui/textures/icon_x","ui/textures/icon_tri","ui/textures/icon_sqr","ui/textures/icon_cir","ui/textures/notexture"};
        constexpr const char* textures[]{"denmrk16","venus16","denmrk16","denmrk14","overlay1","nameblur",
            "overlay1","icon_x","icon_tri","icon_sqr","icon_cir",""};
        std::vector<assets::OriginalUiImageSource50KG> sources;
        for(std::size_t i=0;i<s->materials.size();++i){
            d.materialIndex=static_cast<std::uint32_t>(i);auto& b=s->materials[i];b.registeredName=names[i];
            if(i==5){if(!assets::readOriginalUiTournamentNameBlurFallbackMaterial50KG(*in.tournament,s->templates,
                    assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.source,d.material))
                    return fail(d,E::material,d.material.sourcePC);
            }else if(i>=7&&i<=10){if(!assets::readOriginalUiTournamentPromptFallbackMaterial50KG(*in.tournament,*in.overlays,
                    static_cast<assets::OriginalUiMainPrompt50KG>(i-7),s->templates,
                    assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.source,d.material))
                    return fail(d,E::material,d.material.sourcePC);
            }else if(!assets::readOriginalUiMaterial50KG(*in.tournament,b.registeredName+".psm",s->templates,
                assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.source,d.material))
                return fail(d,E::material,d.material.sourcePC);
            if(i==11){if(b.source.passes.size()!=1||b.source.passes[0].textureCount!=0||
                    !assets::prepareOriginalUiNativeUntexturedMaterial50KG(b.source.passes[0],b.untextured,d.material))
                    return fail(d,E::material,0x2476A0);continue;}
            if(b.source.passes.size()!=1||b.source.passes[0].textureCount!=1||
               !same(b.source.passes[0].textureName,textures[i])||b.source.passes[0].authored.find("TextureAnimation"))
                return fail(d,E::material,0x2476A0);
            const auto& archive=i<4?*in.fonts:(i==4||i==6)?*in.controllerPopup:(i==7||i==8)?*in.overlays:*in.tournament;
            const std::string prefix=i<4?"font/":"ui/textures/";
            const auto member=archive.find(prefix+textures[i]+".pst");
            const auto cel=i<4?archive.find(prefix+textures[i]+".cel"):assets::OriginalUiMemberView50KG{};
            const auto identity=imageIdentity(archive,member);
            if(!identity||!member.data||member.size!=member.identity->bytes||
               (i<4&&(!cel.identity||!cel.data||cel.size!=cel.identity->bytes)))return fail(d,E::image,0x2DF180);
            std::size_t image=0;for(;image<sources.size();++image)
                if(sources[image].member.identity==member.identity&&sources[image].cel.identity==cel.identity)break;
            if(image==sources.size())sources.push_back({identity,member,0,member.size,cel});
            b.image=static_cast<std::uint32_t>(image);
        }
        if(!registry(in.appStart->find("AppStart/Mat_Stbl.txt"),s->materials))return fail(d,E::registry,0x298C80);
        auto decoded=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!decoded->load(sources,d.image))return fail(d,E::image,0x2DF180);
        s->bankInput.archives={in.appStart,in.tournament,in.fonts,in.controllerPopup,in.overlays};s->bankInput.owners={decoded};
        if(!s->imageBank.load(s->bankInput,d.bank))return fail(d,E::image,0x2DF180);
        s->images=s->imageBank.images();const auto* bank=s->images->textures();
        if(!bank||bank->textureCount()!=sources.size())return fail(d,E::image,0x2DF180);
        // No cross-owner remap is assumed, even for this one cold bank.
        for(std::size_t i=0;i<sources.size();++i){const auto* m=s->imageBank.mapping(0,static_cast<std::uint32_t>(i));
            if(!m||m->combinedIndex!=i)return fail(d,E::image,0x2DF180);}
        // Export the SAME canonical owner used by every widget/material view.
        // The local bank retains its original decoded inputs independently;
        // outer banks must map these exposed identities, not the pre-bank IDs.
        s->bankInput.owners={s->images};
        s->samplers.resize(bank->textureCount());std::vector<bool> assigned(bank->textureCount(),false);
        for(std::size_t i=0;i<s->materials.size();++i){
            d.materialIndex=static_cast<std::uint32_t>(i);auto& b=s->materials[i];d.imageIndex=b.image;
            OriginalUiMaterialSpan50KG converted;
            if(b.image==missing){if(!convertOriginalUiUntexturedMaterialSpan50KG(b.untextured,{0,1,1},converted))
                    return fail(d,E::material,0x2476A0);continue;}
            const auto* pixels=s->images->decodedTexture(b.image);
            if(!pixels||!assets::prepareOriginalUiNativeMaterial50KG(b.source.passes[0],*pixels,b.policy,d.material)||
               !convertOriginalUiMaterialSpan50KG(b.policy,{0,1,1,b.image,bank->textureCount()},converted))
                return fail(d,E::material,0x2476A0);
            b.sampler=converted.native.sampler;
            if(assigned[b.image]&&!sameSampler(s->samplers[b.image],b.sampler))return fail(d,E::sampler,0x2476A0);
            assigned[b.image]=true;s->samplers[b.image]=b.sampler;
        }
        for(bool set:assigned)if(!set)return fail(d,E::sampler,0x2476A0);
        for(std::uint16_t id=1;id<originalUiTournamentWidgetSlots50KG;++id){
            d.widget=id;const auto kind=s->kinds[id];if(kind!=K::text&&kind!=K::icon)continue;
            const auto registered=kind==K::text?"font/"+s->texts[id].materialName:"ui/textures/"+s->icons[id].materialName;
            std::size_t m=0;for(;m<s->materials.size();++m)if(same(registered,s->materials[m].registeredName))break;
            if(m==s->materials.size())return fail(d,E::material,0x299CAC);
            s->widgetMaterials[id]=static_cast<std::uint32_t>(m);const auto& b=s->materials[m];
            const auto* atlas=bank->texture(b.image);if(!atlas)return fail(d,E::image,0x2DF180);
            if(kind==K::text){const auto& text=s->texts[id];const auto* font=s->images->font(b.image);
                if(!font||!same(text.fontName,b.source.passes[0].textureName)||text.depth!=1)
                    return fail(d,E::geometry,0x2DFD90);
                if(text.textSource!=assets::OriginalUiTextSource50KG::dynamic){
                    auto geometry=std::make_unique<game::OriginalUiTextGeometry50KG>();
                    if(!geometry->prepare(text,*font,atlas->texture,d.text))return fail(d,E::geometry,0x2E0A30);
                    s->geometry[id]=std::move(geometry);
                }
            }else if(game::layoutOriginalUiIcon50KG(s->icons[id],atlas->texture,in.video.packetZMaximum,s->layouts[id-27])!=
                game::OriginalUiIconLayoutStatus50KG::complete)return fail(d,E::geometry,0x2AC590);
        }
        for(std::size_t i=0;i<popupIds.size();++i){
            const auto id=static_cast<std::uint16_t>(popupIds[i]);d.widget=id;const auto& text=s->texts[id];
            for(const auto& control:page.controls)for(auto member:control.widgets)
                if(member==id)return fail(d,E::population,0x1FF020);
            const auto m=s->widgetMaterials[id];if(m>=s->materials.size())return fail(d,E::material,0x1FF020);
            const auto& b=s->materials[m];const auto* font=s->images->font(b.image);const auto* atlas=bank->texture(b.image);
            const auto* provenance=s->images->sourceProvenance(b.image);const auto* geometry=s->geometry[id].get();
            if(!font||!atlas||!geometry||!b.source.identity||text.fontName!="denmrk16"||
               !provenance||provenance->memberIdentity!=in.fonts->find("font/denmrk16.pst").identity||
               provenance->celIdentity!=in.fonts->find("font/denmrk16.cel").identity||
               geometry->lines().size()!=text.lines.size())return fail(d,E::image,0x2037D0);
            for(std::size_t line=0;line<text.lines.size();++line){const auto& bytes=text.lines[line];
                if(bytes.size()>=255||bytes.find('\0')!=std::string::npos)return fail(d,E::geometry,0x2037D0);
                OriginalUiTournamentPopupTextClone50KG clone;clone.sourceWidget=id;
                clone.materialIdentity=b.source.identity;clone.font=font;clone.atlas=&atlas->texture;clone.text=bytes;
                //203974..203994 replaces the template widget's leading with
                // exactly21 for copied ScreenTexts. This is not an authored
                // descriptor edit, and preserves the pre-justification origin.
                clone.x10=float(text.x);clone.y14=float(text.y)+21.f*float(line);
                clone.justification50=text.justification;clone.color54=text.color;
                clone.capacity5B=static_cast<std::uint8_t>(bytes.size()+1u);
                clone.characterCount5C=static_cast<std::uint8_t>(bytes.size());
                clone.allocatedPackets5D=static_cast<std::uint8_t>((clone.capacity5B+35u)/36u);
                clone.activePackets5E=clone.allocatedPackets5D;s->popupClones[i].push_back(std::move(clone));
            }
        }
        constexpr std::uint32_t popupOffsets[]{0x160,0x1C0,0x220,0x280,0x40,0xA0,0x100};
        constexpr std::uint32_t popupCalls[]{0x1FF058,0x1FF110,0x1FF1C8,0x1FF280,0x1FF338,0x1FF4AC,0x1FF68C};
        for(std::size_t i=0;i<s->popupTextures.size();++i){const auto m=static_cast<std::uint32_t>(i<4?7+i:6);
            auto& occurrence=s->popupTextures[i];occurrence.objectOffset=popupOffsets[i];
            occurrence.constructorPC=popupCalls[i];occurrence.materialIndex=m;occurrence.imageIndex=s->materials[m].image;
            const auto* atlas=bank->texture(occurrence.imageIndex);
            if(!atlas||!atlas->texture.width||!atlas->texture.height)return fail(d,E::image,popupCalls[i]);
            auto& q=occurrence.initialQuad;q.materialName=s->materials[m].registeredName.c_str();
            q.sourcePC=popupCalls[i];float x=0,y=0,width=0,height=0,u1=1,v0=0,v1=1;
            if(i<4){constexpr float origins[]{514,427,305,305};
                x=origins[i];y=378;width=float(atlas->texture.width);height=float(atlas->texture.height);
            }else{
                if(atlas->texture.width!=32||atlas->texture.height!=32)return fail(d,E::geometry,popupCalls[i]);
                width=640;height=i==6?14.f:16.f;q.sourcePacketColor={{121,66,13,63}};
                // First strip keeps its default0..1 UV. Other strips use the
                // constructor's actual reciprocal atlas dimensions.
                if(i!=4){u1=32.f*(1.f/float(atlas->texture.width));
                    v0=(i==5?32.f:18.f)*(1.f/float(atlas->texture.height));
                    v1=i==5?0.f:32.f*(1.f/float(atlas->texture.height));}
            }
            q.vertices={{{x,y,0,v0},{x+width,y,u1,v0},{x,y+height,0,v1},{x+width,y+height,u1,v1}}};
        }
        game::OriginalUiExitTextDiagnostic50KG exitDiagnostic;
        if(s->exitText.prepare({9,1},in.tournament,*in.english,s->images,s->materials[0].image,exitDiagnostic)!=
            game::OriginalUiExitTextStatus50KG::ok)return fail(d,E::geometry,exitDiagnostic.sourcePC);
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,E::allocation,0x1CD160);}
}
game::OriginalUiExitText50KG* OriginalUiTournamentWidgetResources50KG::exitText() noexcept{return storage_?&storage_->exitText:nullptr;}
bool OriginalUiTournamentWidgetResources50KG::loaded() const noexcept{return bool(storage_);}
const assets::OriginalUiTournamentPageData50KG* OriginalUiTournamentWidgetResources50KG::pageData() const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiPage50KG* OriginalUiTournamentWidgetResources50KG::page() const noexcept{return storage_?storage_->page.page():nullptr;}
const assets::OriginalUiDat50KG* OriginalUiTournamentWidgetResources50KG::widgetsDat() const noexcept{return storage_?storage_->page.widgetsDat():nullptr;}
K OriginalUiTournamentWidgetResources50KG::kind(std::uint16_t id) const noexcept{return storage_&&validId(id)?storage_->kinds[id]:K::absent;}
bool OriginalUiTournamentWidgetResources50KG::hasWidget(std::uint16_t id) const noexcept{const auto k=kind(id);return k!=K::absent&&k!=K::empty;}
const std::vector<std::uint16_t>* OriginalUiTournamentWidgetResources50KG::order(bool top) const noexcept{return storage_?(top?&storage_->top:&storage_->ordinary):nullptr;}
const assets::OriginalUiOverlayText50KG* OriginalUiTournamentWidgetResources50KG::text(std::uint16_t id) const noexcept{return kind(id)==K::text?&storage_->texts[id]:nullptr;}
const assets::OriginalUiIcon50KG* OriginalUiTournamentWidgetResources50KG::icon(std::uint16_t id) const noexcept{return kind(id)==K::icon?&storage_->icons[id]:nullptr;}
const OriginalUiTournamentPodDescriptor50KG* OriginalUiTournamentWidgetResources50KG::pod(std::uint16_t id) const noexcept{return kind(id)==K::userRotate?&storage_->pods[id-39]:nullptr;}
const assets::OriginalUiMovementControl50KG* OriginalUiTournamentWidgetResources50KG::movement() const noexcept{return storage_?&storage_->movement:nullptr;}
const game::OriginalUiIconLayout50KG* OriginalUiTournamentWidgetResources50KG::iconLayout(std::uint16_t id) const noexcept{return kind(id)==K::icon?&storage_->layouts[id-27]:nullptr;}
const game::OriginalUiTextGeometry50KG* OriginalUiTournamentWidgetResources50KG::staticTextGeometry(std::uint16_t id) const noexcept{return kind(id)==K::text?storage_->geometry[id].get():nullptr;}
bool OriginalUiTournamentWidgetResources50KG::prepareDynamicText(std::uint16_t id,const std::vector<std::string>& lines,
    game::OriginalUiTextGeometry50KG& output,D& d) const noexcept {
    d={};d.widget=id;const auto* descriptor=text(id);
    if(!descriptor||descriptor->textSource!=assets::OriginalUiTextSource50KG::dynamic)return fail(d,E::descriptor,0x2A9000);
    const auto m=storage_->widgetMaterials[id];if(m>=storage_->materials.size())return fail(d,E::material,0x2DFD90);
    const auto image=storage_->materials[m].image;const auto* font=storage_->images->font(image);
    const auto* atlas=storage_->images->textures()->texture(image);
    if(!font||!atlas)return fail(d,E::image,0x2DFD90);
    if(!output.prepareDynamic(*descriptor,lines,*font,atlas->texture,d.text))return fail(d,E::geometry,0x2E0A30);
    return true;
}
const std::array<OriginalUiTournamentWidgetMaterial50KG,originalUiTournamentWidgetMaterialCount50KG>* OriginalUiTournamentWidgetResources50KG::materials() const noexcept{return storage_?&storage_->materials:nullptr;}
std::uint32_t OriginalUiTournamentWidgetResources50KG::widgetMaterial(std::uint16_t id) const noexcept{return storage_&&validId(id)?storage_->widgetMaterials[id]:missing;}
std::shared_ptr<const assets::OriginalUiScreenAssets50KG> OriginalUiTournamentWidgetResources50KG::images() const noexcept{return storage_?storage_->images:nullptr;}
const assets::OriginalUiImageBankInput50KG* OriginalUiTournamentWidgetResources50KG::bankInput() const noexcept{return storage_?&storage_->bankInput:nullptr;}
const assets::OriginalUiImageBank50KG* OriginalUiTournamentWidgetResources50KG::sourceImageBank() const noexcept{return storage_?&storage_->imageBank:nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiTournamentWidgetResources50KG::samplers() const noexcept{return storage_?&storage_->samplers:nullptr;}
const std::array<std::vector<OriginalUiTournamentPopupTextClone50KG>,9>* OriginalUiTournamentWidgetResources50KG::popupClones438() const noexcept{return storage_?&storage_->popupClones:nullptr;}
const std::array<OriginalUiTournamentPopupTexture50KG,7>* OriginalUiTournamentWidgetResources50KG::popupTextures438() const noexcept{return storage_?&storage_->popupTextures:nullptr;}
}
