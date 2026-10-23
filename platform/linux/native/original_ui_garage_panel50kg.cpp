#include "original_ui_garage_panel50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include <algorithm>
#include <cmath>
#include <utility>
namespace rr::platform {
namespace {
using D=OriginalUiGaragePanelDiagnostic50KG;using E=OriginalUiGaragePanelError50KG;
using A=assets::OriginalUiArchiveId50KG;using T=game::OriginalUiTextWidgetStatus50KG;
constexpr auto missing=assets::unresolvedMaterialTexture50EA;
bool fail(D& d,E e,std::uint32_t pc=0)noexcept{d.error=e;d.sourcePC=pc;return false;}
bool textId(std::uint16_t id)noexcept{return id==1||id==2||id==3||id==7||id==8||(id>=11&&id<=22);}
bool authentic(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,A id)noexcept{
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);}
char fold(char c)noexcept{return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const std::string& b)noexcept{
    if(a.size()!=b.size())return false;for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;return true;}
bool sameSampler(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b)noexcept{
    return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;}
bool registry(const assets::OriginalUiMemberView50KG& table,
    std::array<OriginalUiGaragePanelMaterial50KG,9>& bindings) {
    if(!table.identity||!table.data||table.size!=70962)return false;
    std::array<bool,9> found{};std::size_t p=0;std::uint32_t token=0;
    const auto space=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    while(p<table.size){while(p<table.size&&space(table.data[p]))++p;if(p==table.size)break;
        const auto b=p;while(p<table.size&&!space(table.data[p]))++p;
        const std::string name(reinterpret_cast<const char*>(table.data+b),p-b);
        for(std::size_t i=0;i<bindings.size();++i)if(same(bindings[i].registeredName,name)){
            if(found[i])return false;found[i]=true;bindings[i].registryToken=token;bindings[i].registryIdentity=table.identity;}
        ++token;
    }
    return token==3235&&std::all_of(found.begin(),found.end(),[](bool b){return b;});
}
game::OriginalUiOverlayQuad50KG quad(const char* name,float x,float y,float w,float h,
    float u,float v,float depth,std::uint32_t pc,std::array<std::uint8_t,4> color)noexcept{
    game::OriginalUiOverlayQuad50KG q;q.materialName=name;q.authoredDepth=depth;q.sourcePC=pc;q.sourcePacketColor=color;
    q.vertices={{{x,y,0,0},{x+w,y,u,0},{x,y+h,0,v},{x+w,y+h,u,v}}};return q;
}
bool split(const std::string& s,std::size_t count,std::size_t capacity,std::vector<std::string>& out){
    std::vector<std::string> result(count);std::size_t start=0,line=0;
    do {if(line>=count)return false;const auto end=s.find('@',start);const auto n=end==std::string::npos?s.size()-start:end-start;
        // All admitted authentic fresh strings fit. Do not silently replace
        // unknown source truncation/wrapping with a host layout policy.
        if(n>capacity)return false;result[line++]=s.substr(start,n);if(end==std::string::npos)break;start=end+1;
    }while(true);out=std::move(result);return true;
}
}
struct OriginalUiGaragePanel50KG::Storage {
    OriginalUiGaragePanelColdInput50KG owners;
    assets::OriginalUiMaterialTemplates50KG templates;
    std::array<assets::OriginalUiOverlayText50KG,23> descriptors,live;
    std::array<game::OriginalUiTextWidget50KG,23> widgets;
    std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,23> geometry;
    std::array<std::uint32_t,23> widgetMaterial{};
    std::vector<std::uint16_t> ordinary;
    std::array<OriginalUiGaragePanelMaterial50KG,9> materials;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;
    assets::OriginalUiImageBankInput50KG bankInput;assets::OriginalUiImageBank50KG bank;
    std::vector<MaterialTextureSampler50EB> samplers;
    std::array<std::string,6> description;
    std::string cashLabel,currency,maximum,cost;
    std::array<game::OriginalUiTextureAnimation50KG,2> arrows;
    std::array<std::uint32_t,2> frames{};
    game::OriginalApplicationAnimationClockView50KG arrowClock;
    bool hasClock=false,themeKnown=false;std::uint32_t primary=0,secondary=0;
    std::array<game::OriginalUiOverlayQuad50KG,125> quads;
    game::OriginalUiTextLayout50KG costGeometry;
    std::array<OriginalUiGaragePanelPacket50KG,126> packets{};std::size_t count=0;
};
OriginalUiGaragePanel50KG::OriginalUiGaragePanel50KG()noexcept=default;
OriginalUiGaragePanel50KG::~OriginalUiGaragePanel50KG()=default;
bool OriginalUiGaragePanel50KG::load(const OriginalUiGaragePanelColdInput50KG& in,D& d)noexcept{
    d={};if(storage_)return fail(d,E::alreadyLoaded,0x1FA2D0);
    if(!in.page||!in.page->loaded()||!authentic(in.page->archive(),A::garage)||
       !authentic(in.appStart,A::appStart)||!authentic(in.fonts,A::fonts)||!in.english||!in.english->loaded()||
       !in.runtime.overrideKnown||in.runtime.override435670||!in.video.lifetime||
       in.video.width!=640||in.video.height!=448||in.video.colorBits!=32||
       in.video.packetZMaximum!=0xffffff||in.video.zBufferFormat!=0x31)return fail(d,E::owner,0x1FA2D0);
    try {
        auto s=std::make_unique<Storage>();s->owners=in;s->owners.english=nullptr;s->widgetMaterial.fill(missing);
        const auto garage=in.page->archive();const auto& dat=*in.page->widgetsDat();const auto& page=*in.page->page();
        if(page.widgetCount!=22||page.controls.size()!=8||page.groups.size()!=3||page.hasPopup||
           page.controls[0].widgets!=std::vector<std::uint16_t>{8,10}||
           page.controls[1].widgets!=std::vector<std::uint16_t>{1,2,3,7})return fail(d,E::population,0x1FA2D0);
        for(std::uint16_t i=2;i<8;++i)if(page.controls[i].widgets!=std::vector<std::uint16_t>{std::uint16_t(i+9),std::uint16_t(i+15)})return fail(d,E::population,0x1D7138);
        for(std::uint16_t id=1;id<=22;++id){d.widget=id;
            const auto* section=dat.section("Widget "+std::to_string(id));const auto* type=section?section->find("Type"):nullptr;
            const char* expected=textId(id)?"Overlay_Text":id==6?"Movement_Control":id==10?"Watto":"Empty";
            if(!type||type->text!=expected)return fail(d,E::population,0x1CD160);
            if(id==4||id==5||id==9)continue;
            bool top=false;if(!assets::originalUiBoolean50KG(*section,"OnTop",false,top,d.descriptor,0x1CD890)||top)return fail(d,E::descriptor,0x1CD890);
            s->ordinary.push_back(id);if(!textId(id))continue;
            auto& t=s->descriptors[id];
            if(!assets::readOriginalUiGarageOverlayText50KG(dat,id,in.english,t,d.descriptor)||
               t.wantsJoystickInput[0]||t.wantsJoystickInput[1]||t.depth!=1)return fail(d,E::descriptor,0x2A9000);
            const bool dynamic=id==8||(id>=11&&id<=16);
            if((t.textSource==assets::OriginalUiTextSource50KG::dynamic)!=dynamic||
               (dynamic&&(t.dynamicLineLength!=40||t.lines.size()!=(id==8?2u:4u))))return fail(d,E::descriptor,0x2A9000);
            s->live[id]=t;d.callback=s->widgets[id].bind(t);if(d.callback!=T::ok)return fail(d,E::text,0x2A9000);
        }
        if(s->ordinary.size()!=19)return fail(d,E::population,0x1CD9F8);
        if(!s->templates.load(*in.appStart,d.materialDetail))return fail(d,E::material,0x2DF10C);
        constexpr const char* names[]{"font/denmrk16top","font/venus16top","font/denmrk16","font/denmrk14",
            "ui/textures/podstat2","ui/textures/garagetab","ui/textures/notch","ui/textures/arrwleftsm","ui/textures/arrwrghtsm"};
        constexpr const char* textures[]{"denmrk16","venus16","denmrk16","denmrk14","podstat2","garagetab","notch","arrwleftsm","arrwrghtsm"};
        std::vector<assets::OriginalUiImageSource50KG> sources;
        const auto add=[&](assets::OriginalUiImageSource50KG source){
            for(std::size_t i=0;i<sources.size();++i){const auto& x=sources[i];if(x.member.identity==source.member.identity&&x.cel.identity==source.cel.identity&&x.pstOffset==source.pstOffset&&x.pstBytes==source.pstBytes)return std::uint32_t(i);}
            source.identity=0x4750000000000000ull+sources.size()+1;sources.push_back(source);return std::uint32_t(sources.size()-1);};
        for(std::size_t i=0;i<9;++i){d.material=std::uint32_t(i);auto& b=s->materials[i];b.registeredName=names[i];
            if(i!=4&&i!=6){
                if(!assets::readOriginalUiMaterial50KG(*garage,b.registeredName+".psm",s->templates,
                   assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.authored,d.materialDetail))return fail(d,E::material,d.materialDetail.sourcePC);
                if(b.authored.passes.size()!=1||b.authored.passes[0].textureCount!=1||!same(b.authored.passes[0].textureName,textures[i]))return fail(d,E::material,0x2476A0);
            }
            const auto& archive=i<4?*in.fonts:*garage;const auto prefix=i<4?"font/":"ui/textures/";
            const auto member=archive.find(std::string(prefix)+textures[i]+(i>=7?".atm":".pst"));
            if(!member.identity||!member.data||member.size!=member.identity->bytes)return fail(d,E::image,0x2DF180);
            if(i>=7){assets::OriginalUiAtm50KG atm;assets::DecodeError48C ad;
                if(!assets::splitOriginalUiAtm50KG(member.data,member.size,atm,ad)||atm.frames.size()!=4)return fail(d,E::image,0x2DF180);
                for(const auto& f:atm.frames){if(f.width!=16||f.height!=16||f.kind||f.mipCount!=1)return fail(d,E::image,0x2DF180);
                    b.images.push_back(add({0,member,f.pstOffset,f.pstBytes,{}}));}
                const auto& p=b.authored.passes[0];const auto* animation=p.authored.find("TextureAnimation");
                if(!animation||!same(animation->text,"unique")||
                   !assets::originalUiNumber50KG(p.authored,"animatedMapInitialFrame",0,b.animation.initialFrame,d.descriptor,0x29B950)||
                   !assets::originalUiNumber50KG(p.authored,"animatedMapFramesPerSecond",1,b.animation.framesPerSecond,d.descriptor,0x29B950)||
                   !assets::originalUiBoolean50KG(p.authored,"animatedMapLooping",true,b.animation.looping,d.descriptor,0x29B950)||
                   !assets::originalUiBoolean50KG(p.authored,"animatedMapReverseLoopOnEnd",false,b.animation.reverseLoopOnEnd,d.descriptor,0x29B950)||
                   !assets::originalUiInteger50KG(p.authored,"animatedMapLoopOnEndToFrame",-1,b.animation.loopOnEndToFrame,d.descriptor,0x29B950)||
                   b.animation.initialFrame!=0||b.animation.framesPerSecond!=16||!b.animation.looping||!b.animation.reverseLoopOnEnd||b.animation.loopOnEndToFrame!=0||
                   !game::initializeOriginalUiTextureAnimation50KG(s->arrows[i-7],b.animation,4))return fail(d,E::material,0x29B950);
                b.animated=true;
            }else{const auto cel=i<4?archive.find(std::string(prefix)+textures[i]+".cel"):assets::OriginalUiMemberView50KG{};
                if(i<4&&(!cel.identity||!cel.data))return fail(d,E::image,0x2DFD90);
                b.images.push_back(add({0,member,0,member.size,cel}));
                if(!b.authored.passes.empty()&&b.authored.passes[0].authored.find("TextureAnimation"))return fail(d,E::material,0x29B950);
            }
        }
        if(!registry(in.appStart->find("AppStart/Mat_Stbl.txt"),s->materials))return fail(d,E::registry,0x298C80);
        auto decoded=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!decoded->load(sources,d.imageDetail))return fail(d,E::image,0x2DF180);
        s->bankInput.archives={in.appStart,garage,in.fonts};s->bankInput.owners={decoded};
        if(!s->bank.load(s->bankInput,d.bank))return fail(d,E::image,0x2DF180);
        s->images=s->bank.images();const auto* bank=s->images?s->images->textures():nullptr;
        if(!bank||bank->textureCount()!=14)return fail(d,E::image,0x2DF180);
        for(std::uint32_t i=0;i<sources.size();++i){const auto* m=s->bank.mapping(0,i);if(!m||m->combinedIndex!=i)return fail(d,E::image,0x2DF180);}
        s->bankInput.owners={s->images};s->samplers.resize(bank->textureCount());std::vector<bool> assigned(bank->textureCount());
        for(std::size_t i=0;i<9;++i){d.material=std::uint32_t(i);auto& b=s->materials[i];
            for(auto image:b.images){d.image=image;assets::OriginalUiNativeMaterial50KG policy;MaterialTextureSampler50EB sampler;
                if(i==4||i==6){b.fallback=std::make_unique<assets::OriginalUiGaragePanelFallback50KG>();
                    if(!b.fallback->load(garage,in.appStart,i==4?assets::OriginalUiGaragePanelFallbackKind50KG::podstat2:assets::OriginalUiGaragePanelFallbackKind50KG::notch,
                       in.runtime,s->images,image,d.fallback))return fail(d,E::material,d.fallback.sourcePC);
                    policy=*b.fallback->policy();sampler.minification=sampler.magnification=MaterialTextureFilter50EB::linear;
                    sampler.s=sampler.t=MaterialTextureWrap50EB::clampToEdge;
                }else{const auto* pixels=s->images->decodedTexture(image);
                    if(!pixels||!assets::prepareOriginalUiNativeMaterial50KG(b.authored.passes[0],*pixels,policy,d.materialDetail))return fail(d,E::material,0x2476A0);
                    OriginalUiMaterialSpan50KG span;if(!convertOriginalUiMaterialSpan50KG(policy,{0,1,1,image,bank->textureCount()},span))return fail(d,E::material,0x2476A0);sampler=span.native.sampler;
                }
                if(assigned[image]&&!sameSampler(s->samplers[image],sampler))return fail(d,E::sampler,0x2476A0);
                assigned[image]=true;s->samplers[image]=sampler;b.policies.push_back(policy);
            }
        }
        for(bool set:assigned)if(!set)return fail(d,E::sampler);
        const auto* tab=bank->texture(s->materials[5].images[0]);if(!tab||tab->texture.width!=32||tab->texture.height!=32)return fail(d,E::image,0x1FA5B4);
        for(std::uint16_t id=1;id<=22;++id)if(textId(id)){d.widget=id;const auto& t=s->descriptors[id];
            std::size_t m=0;for(;m<4;++m)if(same("font/"+t.materialName,s->materials[m].registeredName))break;
            if(m==4)return fail(d,E::material,0x2DFD90);s->widgetMaterial[id]=std::uint32_t(m);
            const auto image=s->materials[m].images[0];const auto* font=s->images->font(image);const auto* atlas=bank->texture(image);
            if(!font||!atlas||!same(t.fontName,s->materials[m].authored.passes[0].textureName))return fail(d,E::image,0x2DFD90);
            auto g=std::make_unique<game::OriginalUiTextGeometry50KG>();
            if(!(t.textSource==assets::OriginalUiTextSource50KG::dynamic?g->prepareDynamic(t,t.lines,*font,atlas->texture,d.geometry):g->prepare(t,*font,atlas->texture,d.geometry)))return fail(d,E::text,0x2E0A30);
            s->geometry[id]=std::move(g);
        }
        constexpr const char* stems[]{"ACCELERATION","TOP_SPEED","HANDLING","DEFENSE","REPAIR","COOLING"};
        for(std::size_t i=0;i<6;++i){const auto* prefix=in.english->at(211+i);const auto* item=in.english->find(std::string("STR_GARAGE_PAGE_")+stems[i]+"_1");
            if(!prefix||!item||prefix->name!=std::string("STR_GARAGE_PAGE_")+stems[i]+"_DESC")return fail(d,E::text,0x1FCDAC);
            s->description[i]=prefix->text+"@"+item->text;std::vector<std::string> check;
            if(!split(s->description[i],4,40,check))return fail(d,E::text,0x1FD7CC);
        }
        const auto* cash=in.english->at(208);const auto* currency=in.english->at(209);const auto* maximum=in.english->at(210);
        if(!cash||cash->name!="STR_GARAGE_PAGE_AVAILABLE_TRUGUTS"||!currency||currency->name!="STR_GARAGE_PAGE_TRUGUTS_ABBREVIATION"||
           !maximum||maximum->name!="STR_GARAGE_PAGE_MAX")return fail(d,E::text,0x1FD85C);
        s->cashLabel=cash->text;s->currency=currency->text;s->maximum=maximum->text;
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,E::allocation,0x1FA2D0);}
}
bool OriginalUiGaragePanel50KG::setEntryTheme(std::uint32_t p,std::uint32_t q,D& d)noexcept{
    d={};if(!storage_)return fail(d,E::notLoaded,0x1FBAB0);auto& s=*storage_;
    //1FBAB0 colors3,17..22,11..16 and panel/tabs with A8;1FBDDC/DF0
    // colors1/2 with AC. Cash8 keeps its authored color.
    for(std::uint16_t id=1;id<=22;++id)if(id==1||id==2||id==3||id>=11){d.widget=id;
        d.callback=s.widgets[id].setWidgetColor(id<=2?q:p);if(d.callback!=T::ok)return fail(d,E::text,0x2AA7F0);}
    s.primary=p;s.secondary=q;s.themeKnown=true;d={};return true;
}
bool OriginalUiGaragePanel50KG::projectFreshPanel(const OriginalUiGaragePanelInput50KG& in,D& d)noexcept{
    d={};if(!storage_)return fail(d,E::notLoaded,0x1FBE10);auto& s=*storage_;
    if(!in.state||!in.colorsKnown||!in.panelOffsetKnown||!std::isfinite(in.panelOffset4D8)||
       in.panelOffset4D8<0||in.panelOffset4D8>140||!s.themeKnown||in.primaryA8!=s.primary||in.secondaryAC!=s.secondary)
        return fail(d,E::input,0x1FBE10);
    std::array<game::OriginalUiGarageRowNumbers50KG,6> rows;
    for(std::uint32_t r=0;r<6;++r)if(!game::describeOriginalUiGarageFreshRow50KG(*in.state,r,rows[r]))return fail(d,E::input,0x1FC2C4);
    const auto selected=in.state->selected4E4;const auto& selectedRow=rows[selected];
    // This owner is deliberately fresh-only: original upgrade name ordinal1.
    if(selectedRow.showUpgradeDescription&&selectedRow.upgradeNameOrdinal!=1)return fail(d,E::input,0x1FCCC4);
    try {
        const float y=in.panelOffset4D8;auto live=s.live;
        for(std::uint16_t r=0;r<6;++r){live[17+r].x=170;volatile float position=float(268+18*r)+y;live[17+r].y=std::int32_t(position);}
        live[8].x=385;volatile float cashY=245.f+y;live[8].y=std::int32_t(cashY);
        if(!split(s.cashLabel+" "+std::to_string(in.state->available34B0),2,40,live[8].lines))return fail(d,E::text,0x1FD8AC);
        const auto desc=std::uint16_t(11+selected);
        if(selectedRow.showUpgradeDescription){live[desc].x=440;volatile float descriptionY=290.f+y;live[desc].y=std::int32_t(descriptionY);
            if(!split(s.description[selected],4,40,live[desc].lines))return fail(d,E::text,0x1FD7CC);}
        std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,23> geometry;
        const auto* bank=s.images->textures();
        for(std::uint16_t id=1;id<=22;++id)if(id==8||id==desc||id>=17){d.widget=id;
            const auto image=s.materials[s.widgetMaterial[id]].images[0];const auto* font=s.images->font(image);const auto* atlas=bank->texture(image);
            if(!font||!atlas)return fail(d,E::image,0x2DFD90);
            auto g=std::make_unique<game::OriginalUiTextGeometry50KG>();const auto& t=live[id];
            if(!(t.textSource==assets::OriginalUiTextSource50KG::dynamic?g->prepareDynamic(t,t.lines,*font,atlas->texture,d.geometry):g->prepare(t,*font,atlas->texture,d.geometry)))return fail(d,E::text,0x2E0A30);
            geometry[id]=std::move(g);
        }
        std::string cost=selectedRow.maximumReached?s.maximum:std::to_string(selectedRow.nextPrice)+" "+s.currency;
        //1FDA80..98 clamps against exact capacity339B=6, not five bytes.
        if(cost.size()>6)cost.resize(6);
        game::OriginalUiTextLayout50KG costGeometry;const auto costImage=s.materials[2].images[0];
        const auto* font=s.images->font(costImage);const auto* atlas=bank->texture(costImage);
        if(!font||!atlas)return fail(d,E::image,0x1FA360);
        volatile float rowY=18.f*float(selected);volatile float costBase=267.f+rowY;volatile float costY=costBase+y;
        game::OriginalUiTextInput50KG costInput{font,&atlas->texture,reinterpret_cast<const std::uint8_t*>(cost.data()),cost.size(),365.f,costY};
        if(!game::layoutOriginalUiText50KG(costInput,costGeometry,d.glyph))return fail(d,E::text,0x1FDB88);
        auto quads=s.quads;const auto themed=game::originalUiTextPacketColor50KG(s.primary,255);
        quads[0]=quad("ui/textures/podstat2",0,251.f+y,640,128,1,1,15,0x1FBECC,themed);
        quads[1]=quad("ui/textures/garagetab",374,243.f+y,32,18,1,18.f/32.f,1,0x1FBF6C,themed);
        quads[2]=quad("ui/textures/garagetab",406,243.f+y,234,18,20.f/32.f,18.f/32.f,1,0x1FC00C,themed);
        for(std::size_t r=0;r<6;++r)for(std::size_t j=0;j<20;++j){
            auto color=std::array<std::uint8_t,4>{{25,25,25,50}};
            if(std::int32_t(j)<rows[r].current)color=r==selected?std::array<std::uint8_t,4>{{102,102,102,63}}:themed;
            volatile float row=18.f*float(r);volatile float base=271.f+row;volatile float yy=base+y;
            // ScreenTexture SetWidth/Height alone leaves initial0..1 UVs.
            quads[3+r*20+j]=quad("ui/textures/notch",190.f+7.f*float(j),yy,5,10,1,1,1,0x1FCA98,color);
        }
        //1FDB90..1FDD24: x191 - actual left width16 -3; right191+133+5.
        volatile float arrowBase=270.f+rowY;volatile float arrowY=y+arrowBase;
        quads[123]=quad("ui/textures/arrwleftsm",172,arrowY,16,16,1,1,1,0x1FDC5C,{{127,127,127,63}});
        quads[124]=quad("ui/textures/arrwrghtsm",329,arrowY,16,16,1,1,1,0x1FDD24,{{127,127,127,63}});
        // Publish only after all allocating/projection operations succeeded.
        s.live=std::move(live);for(std::uint16_t id=1;id<=22;++id)if(geometry[id])s.geometry[id]=std::move(geometry[id]);
        s.quads=quads;s.cost=std::move(cost);s.costGeometry=std::move(costGeometry);
        for(std::uint16_t id=11;id<=16;++id)s.widgets[id].setControlEnabled(false);
        if(selectedRow.showUpgradeDescription)s.widgets[desc].setControlEnabled(true);
        s.count=0;
        const auto append=[&](std::size_t q,std::uint32_t m,std::uint32_t object){auto& p=s.packets[s.count++];p={};
            p.quad=&s.quads[q];p.color=p.quad->sourcePacketColor;p.material=m;p.frame=m>=7?s.frames[m-7]:0;
            p.image=s.materials[m].images[p.frame];p.sourcePC=p.quad->sourcePC;p.objectOffset=object;
            p.packetZ=float(s.owners.video.packetZMaximum)-p.quad->authoredDepth;};
        append(0,4,0x5E0);append(1,5,0x520);append(2,5,0x580);
        for(std::size_t r=0;r<6;++r)for(std::int32_t j=0;j<rows[r].maximum;++j)
            append(3+r*20+std::size_t(j),6,0x640+std::uint32_t(r*20+std::size_t(j))*0x60);
        auto& text=s.packets[s.count++];text={};text.text=&s.costGeometry;text.color={{102,102,102,63}};text.material=2;text.image=costImage;
        text.objectOffset=0x3340;text.sourcePC=0x1FDB88;text.packetZ=float(s.owners.video.packetZMaximum)-12.f;
        append(123,7,0x33F0);append(124,8,0x3450);d={};return true;
    }catch(...){return fail(d,E::allocation,0x1FBE10);}
}
bool OriginalUiGaragePanel50KG::advanceArrows(const game::OriginalApplicationAnimationClockView50KG& clock,D& d)noexcept{
    d={};if(!storage_)return fail(d,E::notLoaded);auto& s=*storage_;
    game::OriginalApplicationAnimationClockSample50KG sample;
    if(!clock.read(sample)||(s.hasClock&&!s.arrowClock.sameOwner(clock)))return fail(d,E::clock,0x29AC90);
    // Actual non-animatesWhilePaused materials do not tick with pause/pending.
    if(sample.pause11C||sample.pending120)return true;
    auto arrows=s.arrows;auto frames=s.frames;
    for(std::size_t i=0;i<2;++i)if(!game::advanceOriginalUiTextureAnimation50KG(arrows[i],sample.current230,frames[i]))return fail(d,E::clock,0x29B028);
    s.arrows=arrows;s.frames=frames;s.arrowClock=clock;s.hasClock=true;return true;
}
bool OriginalUiGaragePanel50KG::applyTextCallback(std::uint16_t id,game::OriginalUiControlOperation50KG op,bool value,D& d)noexcept{
    d={};d.widget=id;if(!storage_)return fail(d,E::notLoaded);if(!textId(id))return fail(d,E::requiredJoin,0x1CD160);
    d.callback=storage_->widgets[id].apply(op,value);return d.callback==T::ok||fail(d,E::text,0x2AA37C);
}
bool OriginalUiGaragePanel50KG::updateText(std::uint16_t id,bool shown,D& d)noexcept{
    d={};d.widget=id;if(!storage_)return fail(d,E::notLoaded);if(!textId(id))return fail(d,E::requiredJoin,0x1CD160);
    d.callback=storage_->widgets[id].updateStatic(shown);return d.callback==T::ok||d.callback==T::hidden||fail(d,E::text,0x29DA40);
}
bool OriginalUiGaragePanel50KG::projectText(std::uint16_t id,std::size_t line,bool shown,OriginalUiGarageWidgetTextProjection50KG& out,D& d)noexcept{
    d={};d.widget=id;if(!storage_)return fail(d,E::notLoaded);if(!textId(id))return fail(d,E::requiredJoin,0x1CD160);
    auto& s=*storage_;const auto* geometry=s.geometry[id].get();
    if(!geometry||line>=geometry->lines().size()||line>=s.live[id].lines.size())return fail(d,E::input,0x2E0A30);
    OriginalUiGarageWidgetTextProjection50KG result;result.geometry=&geometry->lines()[line];
    d.callback=s.widgets[id].projectLine(line,result.line,shown);if(d.callback!=T::ok)return fail(d,E::text,0x29DA40);
    // Runtime strings/geometry belong to this separate SetText owner; the
    // immutable descriptor borrowed by TextWidget is never rewritten.
    result.line.text=&s.live[id].lines[line];result.material=s.widgetMaterial[id];result.image=s.materials[result.material].images[0];
    result.packetZ=float(s.owners.video.packetZMaximum)-s.descriptors[id].depth;out=result;return true;
}
bool OriginalUiGaragePanel50KG::loaded()const noexcept{return bool(storage_);}
const assets::OriginalUiGaragePageData50KG* OriginalUiGaragePanel50KG::pageData()const noexcept{return storage_?storage_->owners.page.get():nullptr;}
const assets::OriginalUiOverlayText50KG* OriginalUiGaragePanel50KG::descriptor(std::uint16_t id)const noexcept{return storage_&&textId(id)?&storage_->descriptors[id]:nullptr;}
const game::OriginalUiTextWidget50KG* OriginalUiGaragePanel50KG::widget(std::uint16_t id)const noexcept{return storage_&&textId(id)?&storage_->widgets[id]:nullptr;}
const std::vector<std::uint16_t>* OriginalUiGaragePanel50KG::ordinaryOrder()const noexcept{return storage_?&storage_->ordinary:nullptr;}
const OriginalUiGaragePanelPacket50KG* OriginalUiGaragePanel50KG::packets()const noexcept{return storage_&&storage_->count?storage_->packets.data():nullptr;}
std::size_t OriginalUiGaragePanel50KG::packetCount()const noexcept{return storage_?storage_->count:0;}
const game::OriginalUiOverlayQuad50KG* OriginalUiGaragePanel50KG::notch(std::size_t r,std::size_t j)const noexcept{return storage_&&storage_->count&&r<6&&j<20?&storage_->quads[3+r*20+j]:nullptr;}
const std::string* OriginalUiGaragePanel50KG::costText()const noexcept{return storage_?&storage_->cost:nullptr;}
const std::vector<std::string>* OriginalUiGaragePanel50KG::liveText(std::uint16_t id)const noexcept{return storage_&&textId(id)?&storage_->live[id].lines:nullptr;}
const std::array<OriginalUiGaragePanelMaterial50KG,9>* OriginalUiGaragePanel50KG::materials()const noexcept{return storage_?&storage_->materials:nullptr;}
std::shared_ptr<const assets::OriginalUiScreenAssets50KG> OriginalUiGaragePanel50KG::images()const noexcept{return storage_?storage_->images:nullptr;}
const assets::OriginalUiImageBankInput50KG* OriginalUiGaragePanel50KG::bankInput()const noexcept{return storage_?&storage_->bankInput:nullptr;}
const assets::OriginalUiImageBank50KG* OriginalUiGaragePanel50KG::sourceImageBank()const noexcept{return storage_?&storage_->bank:nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiGaragePanel50KG::samplers()const noexcept{return storage_?&storage_->samplers:nullptr;}
const OriginalUiGaragePanelVideo50KG* OriginalUiGaragePanel50KG::video()const noexcept{return storage_?&storage_->owners.video:nullptr;}
}
