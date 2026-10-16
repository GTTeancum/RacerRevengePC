#include "original_ui_tournament_career_resources50kg.h"
#include <utility>
#include "original_ui_main_draw50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include "../../../src/assets/original_standard_texture_resource50la.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include <cmath>
#include "../../../src/game/original_ui_text_position50kg.h"

namespace rr::platform {
namespace {
using Error=OriginalUiTournamentCareerResourceError50KG;
using Diagnostic=OriginalUiTournamentCareerResourceDiagnostic50KG;
using Archive=assets::OriginalUiArchiveId50KG;
constexpr auto missing=assets::unresolvedMaterialTexture50EA;
bool fail(Diagnostic& d,Error e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
bool authentic(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,Archive id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
bool sameStamp(OriginalUiRiderProfileStamp50KG a,OriginalUiRiderProfileStamp50KG b) noexcept {
    return a.generation==b.generation&&a.mode34==b.mode34;
}
// Source sprintf("%d") consumes a signed32 bit pattern. Avoid implementation-
// defined unsigned-to-signed narrowing while preserving negative source values.
std::string decimal(std::uint32_t value) {
    const auto signedValue=value<=0x7fffffffu?std::int64_t(value):std::int64_t(value)-0x100000000LL;
    return std::to_string(signedValue);
}
// Pure original187C20: no profile reads, win counting or unlock inference.
unsigned category(unsigned oneBasedCourse) noexcept {
    return oneBasedCourse==4?1:oneBasedCourse==8?2:oneBasedCourse==13?3:0;
}
}
struct OriginalUiTournamentCareerResources50KG::Storage {
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile;
    std::shared_ptr<const assets::OriginalUiArchive50KG> tournament,fonts,appStart,backgrounds;
    assets::OriginalUiMaterialTemplates50KG templates;
    std::array<OriginalUiTournamentCareerMaterial50KG,24> materials;
    assets::OriginalUiImageBankInput50KG bankInput;
    std::vector<OriginalUiTournamentMedalPacket50KG> medalPackets;
    std::vector<OriginalUiTournamentStatPacket50KG> statPackets;
    std::array<OriginalUiTournamentMedalPacket50KG,4> backgroundPackets{};
    OriginalUiTournamentMedalPacket50KG portraitPacket{};
    std::array<OriginalUiTournamentMedalPacket50KG,9> markerPackets{};
    game::OriginalUiTextureAnimation50KG markerAnimation{};
    const assets::OriginalUiMemberIdentity50KG* markerMember=nullptr;
    bool canonicalMarkerTerminal=false;
    assets::OriginalStandardMaterialDiagnostic50LA canonicalMarkerFailure{};
    std::array<std::uint32_t,6> statCounts{};
    OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot career{};
    assets::OriginalUiTournamentPageData50KG page;
    assets::OriginalUiRiderPodAssets50KG pod;
    std::shared_ptr<assets::OriginalUiScreenAssets50KG> images;
    std::array<OriginalUiTournamentNumber50KG,3> numbers{};
    std::array<OriginalUiTournamentMedal50KG,13> medals{};
    std::array<std::uint32_t,3> trophyVisibility{};
    std::array<std::uint32_t,3> trophyImages{{missing,missing,missing}};
};
OriginalUiTournamentCareerResources50KG::OriginalUiTournamentCareerResources50KG() noexcept=default;
OriginalUiTournamentCareerResources50KG::~OriginalUiTournamentCareerResources50KG()=default;
bool OriginalUiTournamentCareerResources50KG::load(
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile,std::uint32_t selected,
    std::shared_ptr<const assets::OriginalUiArchive50KG> tournament,
    std::shared_ptr<const assets::OriginalUiArchive50KG> fonts,
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,
    std::shared_ptr<const assets::OriginalUiArchive50KG> backgrounds,
    const assets::OriginalUiMaterialRuntime50KG& runtime,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyLoaded,0x1EEC40);
    if(!profile||!authentic(tournament,Archive::tournament)||!authentic(fonts,Archive::fonts)||
        !authentic(appStart,Archive::appStart)||!authentic(backgrounds,Archive::backgrounds)||!runtime.overrideKnown)
        return fail(d,Error::owner,0x1EE088);
    try {
        auto s=std::make_unique<Storage>();
        s->profile=std::move(profile);s->tournament=std::move(tournament);s->fonts=std::move(fonts);s->appStart=std::move(appStart);s->backgrounds=std::move(backgrounds);
        if(!s->profile->tournamentCareerSnapshot(selected,s->career)||s->career.stamp.mode34!=2)
            return fail(d,Error::profile,0x1EE088);
        OriginalUiRiderProfileColors50KG appearance;
        if(!s->profile->colors(selected,appearance))return fail(d,Error::profile,0x1F1DB8);
        const auto progress=unsigned(s->career.progress219C&31u);
        // Native admission is restricted to progress0..13. Original masks31
        // and still iterates13 entries; rejecting larger progress is stricter.
        if(progress>13)return fail(d,Error::unsupportedProgress,0x1EF88C);
        if(!s->page.load(s->tournament,d.page))return fail(d,Error::page,0x1EDAB8);
        if(!s->pod.loadTournament(s->tournament,selected,d.pod))return fail(d,Error::pod,0x1EEC40);
        std::vector<assets::OriginalUiImageSource50KG> sources;
        const auto add=[&](const assets::OriginalUiArchive50KG& archive,const std::string& path,
            const std::string& celPath)->bool {
            const auto member=archive.find(path);
            const auto cel=celPath.empty()?assets::OriginalUiMemberView50KG{}:archive.find(celPath);
            if(!member.identity||!member.data||(!celPath.empty()&&(!cel.identity||!cel.data)))return false;
            const auto* identity=archive.identity();std::size_t ordinal=0;
            for(;ordinal<identity->memberCount;++ordinal)
                if(assets::originalUiMemberIdentity50KG(identity->id,ordinal)==member.identity)break;
            if(ordinal==identity->memberCount)return false;
            const auto imageIdentity=(std::uint64_t(static_cast<std::uint8_t>(identity->id)+1u)<<56)|
                (std::uint64_t(ordinal+1u)<<32);
            sources.push_back({imageIdentity,member,0,member.size,cel});return true;
        };
        // Bank0 is the genuine denmrk14 CEL/PST.1EDC38 resolves that font;
        // 1EDC68/1EDDC8/1EDF28 construct the three40-character text objects.
        if(!add(*s->fonts,"font/denmrk14.pst","font/denmrk14.cel"))return fail(d,Error::image,0x1EDC38);
        constexpr const char* names[]{"medlgout","medlggld","medlgslv","medlgbrz",
            "trphgldi","trphslvi","trphbrzi","trphglda","trphslva","trphbrza","medlgicn","notch"};
        for(const auto* name:names)
            if(!add(*s->tournament,std::string("ui/textures/")+name+".pst",""))
                return fail(d,Error::image,0x2DF0B0);
        std::string portrait="00_flrg",background="00_bkg";
        portrait[0]=background[0]=char('0'+selected/10);
        portrait[1]=background[1]=char('0'+selected%10);
        if(!add(*s->tournament,"ui/textures/"+portrait+".pst","")||
            !add(*s->backgrounds,"ui/textures/"+background+".pst",""))
            return fail(d,Error::image,0x1F17AC);
        const auto marker=s->tournament->find("ui/textures/medstar.atm");
        assets::OriginalUiAtm50KG markerAtm;assets::DecodeError48C markerError;
        if(!marker.identity||!marker.data||!assets::splitOriginalUiAtm50KG(marker.data,marker.size,markerAtm,markerError)||
            markerAtm.frames.size()!=9)return fail(d,Error::animation,0x1EFEDC);
        std::size_t markerOrdinal=0;
        for(;markerOrdinal<s->tournament->identity()->memberCount;++markerOrdinal)
            if(assets::originalUiMemberIdentity50KG(Archive::tournament,markerOrdinal)==marker.identity)break;
        if(markerOrdinal==s->tournament->identity()->memberCount)return fail(d,Error::animation,0x1EFEDC);
        s->markerMember=marker.identity;
        for(const auto& frame:markerAtm.frames){
            if(frame.pstOffset>0xffffffffu)return fail(d,Error::animation,0x102230);
            const auto identity=(std::uint64_t(static_cast<std::uint8_t>(Archive::tournament)+1u)<<56)|
                (std::uint64_t(markerOrdinal+1u)<<32)|std::uint64_t(frame.pstOffset);
            sources.push_back({identity,marker,frame.pstOffset,frame.pstBytes,{}});
        }
        s->images=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!s->images->load(sources,d.image))return fail(d,Error::image,0x2DF0B0);
        if(!s->templates.load(*s->appStart,d.material))return fail(d,Error::material,0x2DF10C);
        for(unsigned i=0;i<24;++i){
            auto& binding=s->materials[i];
            binding.registeredName=i==0?"font/denmrk14":std::string("ui/textures/")+(i>=15?std::string("medstar"):i==13?portrait:i==14?background:std::string(names[i-1]));
            if(i>=5&&i<=10){
                if(!assets::readOriginalUiTournamentTrophyFallbackMaterial50KG(*s->tournament,
                    static_cast<assets::OriginalUiTournamentTrophy50KG>(i-5),s->templates,
                    assets::OriginalUiTemplate50KG::screenMt,runtime,binding.source,d.material))
                    return fail(d,Error::material,d.material.sourcePC);
            }else if(i==12){
                if(!assets::readOriginalUiTournamentNotchFallbackMaterial50KG(*s->tournament,s->templates,
                    assets::OriginalUiTemplate50KG::screenMt,runtime,binding.source,d.material))
                    return fail(d,Error::material,d.material.sourcePC);
            }else if(i==13){
                if(!assets::readOriginalUiTournamentPortraitFallbackMaterial50KG(*s->tournament,
                    static_cast<std::uint8_t>(selected),s->templates,assets::OriginalUiTemplate50KG::screenMt,
                    runtime,binding.source,d.material))return fail(d,Error::material,d.material.sourcePC);
            }else{
                const auto psm=i==0?std::string("font/denmrk14.psm"):
                    binding.registeredName+".psm";
                if(!assets::readOriginalUiMaterial50KG(*s->tournament,psm,s->templates,
                    assets::OriginalUiTemplate50KG::screenMt,runtime,binding.source,d.material))
                    return fail(d,Error::material,d.material.sourcePC);
            }
            const auto* decoded=s->images->decodedTexture(i);
            if(binding.source.passes.size()!=1||!decoded)return fail(d,Error::material,0x2476A0);
            const auto& pass=binding.source.passes[0];
            // Images0..14 are standalone PSTs;15..23 are the authenticated
            // medstar ATM frames governed by one retained unique controller.
            const std::string expected=i==0?"denmrk14":i>=15?"medstar":i==13?portrait:i==14?background:names[i-1];
            if(pass.textureCount!=1||pass.textureName!=expected||(i<15&&pass.authored.find("TextureAnimation"))||
                !assets::prepareOriginalUiNativeMaterial50KG(pass,*decoded,binding.policy,d.material))
                return fail(d,Error::material,0x2476A0);
            OriginalUiMaterialSpan50KG converted;
            if(!convertOriginalUiMaterialSpan50KG(binding.policy,{0,1,1,i,24},converted))
                return fail(d,Error::material,0x2476A0);
            binding.sampler=converted.native.sampler;
        }
        const auto& markerPass=s->materials[15].source.passes[0];
        const auto* animationMode=markerPass.authored.find("TextureAnimation");
        assets::OriginalUiPageDiagnostic50KG animationDiagnostic;
        game::OriginalUiTextureAnimationConfig50KG markerConfig;
        std::int32_t looping=1,reverse=0,loopTo=-1;
        if(!animationMode||animationMode->text!="unique"||
            !assets::originalUiNumber50KG(markerPass.authored,"animatedMapInitialFrame",0,markerConfig.initialFrame,animationDiagnostic,0x29B950)||
            !assets::originalUiNumber50KG(markerPass.authored,"animatedMapFramesPerSecond",1,markerConfig.framesPerSecond,animationDiagnostic,0x29B950)||
            !assets::originalUiInteger50KG(markerPass.authored,"animatedMapLooping",1,looping,animationDiagnostic,0x29B950)||
            !assets::originalUiInteger50KG(markerPass.authored,"animatedMapReverseLoopOnEnd",0,reverse,animationDiagnostic,0x29B950)||
            !assets::originalUiInteger50KG(markerPass.authored,"animatedMapLoopOnEndToFrame",-1,loopTo,animationDiagnostic,0x29B950))
            return fail(d,Error::animation,0x29B950);
        // Exact authenticated medstar overrides, with omitted source defaults.
        if(markerConfig.initialFrame!=0||markerConfig.framesPerSecond!=15||looping!=1||reverse!=0||loopTo!=0)
            return fail(d,Error::animation,0x29B950);
        markerConfig.looping=true;markerConfig.reverseLoopOnEnd=false;markerConfig.loopOnEndToFrame=0;
        if(!game::initializeOriginalUiTextureAnimation50KG(s->markerAnimation,markerConfig,9))
            return fail(d,Error::animation,0x29B950);
        const float markerX=progress==13?641.f:progress<4?41.f+37.f*float(progress):
            progress<8?215.f+37.f*float(progress-4):386.f+37.f*float(progress-8);
        const float markerY=progress==13?449.f:301.f;
        for(unsigned frame=0;frame<9;++frame){
            const auto* atlas=s->images->textures()->texture(15+frame);
            if(!atlas||atlas->texture.width!=64||atlas->texture.height!=64)
                return fail(d,Error::animation,0x102230);
            auto& packet=s->markerPackets[frame];packet.objectOffset=0xFA0;packet.image=15+frame;
            auto& q=packet.sprite;q.materialName=s->materials[15].registeredName.c_str();q.sourcePC=0x1F2884;
            q.vertices={{{markerX,markerY,0,0},{markerX+64,markerY,1,0},
                {markerX,markerY+64,0,1},{markerX+64,markerY+64,1,1}}};
        }
        // Original1F1934..1F1D38 uses319/223 sample endpoints divided
        // by the actual texture dimensions, with mirrored lower/right quads.
        const auto* backgroundAtlas=s->images->textures()->texture(14);
        const auto* portraitAtlas=s->images->textures()->texture(13);
        if(!backgroundAtlas||!portraitAtlas||!backgroundAtlas->texture.width||
            !backgroundAtlas->texture.height||!portraitAtlas->texture.width||!portraitAtlas->texture.height)
            return fail(d,Error::image,0x1F17AC);
        const float u=319.f*(1.f/float(backgroundAtlas->texture.width));
        const float v=223.f*(1.f/float(backgroundAtlas->texture.height));
        for(unsigned i=0;i<4;++i){
            auto& packet=s->backgroundPackets[i];packet.objectOffset=0x3D80+i*0x60;packet.image=14;
            auto& q=packet.sprite;q.materialName=s->materials[14].registeredName.c_str();
            q.sourcePC=0x1F273C;q.authoredDepth=15;
            const float x=(i&1)?320.f:0.f,y=(i&2)?224.f:0.f;
            const float u0=(i&1)?u:0.f,u1=(i&1)?0.f:u,v0=(i&2)?v:0.f,v1=(i&2)?0.f:v;
            q.vertices={{{x,y,u0,v0},{x+320,y,u1,v0},{x,y+224,u0,v1},{x+320,y+224,u1,v1}}};
        }
        auto& portraitPacket=s->portraitPacket;portraitPacket.objectOffset=0x1000;portraitPacket.image=13;
        auto& portraitQuad=portraitPacket.sprite;portraitQuad.materialName=s->materials[13].registeredName.c_str();
        portraitQuad.sourcePC=0x1F2758;
        const float right=296.f+float(portraitAtlas->texture.width),bottom=100.f+float(portraitAtlas->texture.height);
        portraitQuad.vertices={{{296,100,0,0},{right,100,1,0},{296,bottom,0,1},{right,bottom,1,1}}};
        const auto* font=s->images->font(0);const auto* atlas=s->images->textures()->texture(0);
        if(!font||!atlas)return fail(d,Error::image,0x1EDC38);
        const std::uint32_t values[]{s->career.unspent2194,s->career.earned2190,s->career.kills2198};
        constexpr std::uint32_t offsets[]{0x3F00,0x3F70,0x3FE0};
        constexpr std::uint32_t pcs[]{0x1EF108,0x1EF5FC,0x1EF734};
        constexpr float x[]{550,529,473},y[]{176,206,236};
        for(unsigned i=0;i<3;++i){
            auto& n=s->numbers[i];n.objectOffset=offsets[i];n.formatPC=pcs[i];n.text=decimal(values[i]);
            const game::OriginalUiTextInput50KG input{font,&atlas->texture,
                reinterpret_cast<const std::uint8_t*>(n.text.data()),n.text.size(),x[i],y[i]};
            // Source justification0 at3C4738, scale1/angle0. No DAT widget
            // identity is forged for these explicitly constructed ScreenTexts.
            if(!game::layoutOriginalUiText50KG(input,n.glyphs,d.text))
                return fail(d,Error::text,pcs[i]);
        }
        for(unsigned i=0;i<13;++i){
            auto& medal=s->medals[i];medal.objectOffset=0x9A0+i*0x60;
            medal.category=static_cast<std::uint8_t>(category(i+1));
            if(i>=progress){medal.image=11;continue;}
            const auto place=s->career.medals1938[i];
            // Original asserts for unexpected completed-place values.
            if(place<1||place>3){d.courseOrder=i;return fail(d,Error::unsupportedMedal,0x1EF8D4);}
            medal.image=medal.category?4u+place:1u+place;
            if(medal.category){
                s->trophyVisibility[medal.category-1]=1;
                s->trophyImages[medal.category-1]=7u+place;
            }
        }

        game::OriginalUiRiderStatInput50KG stats;
        if(!s->profile->statSnapshot(selected,0,stats)||!stats.profileReady||
            stats.mode34!=2||stats.selectedRider!=selected||stats.observedRider!=selected)
            return fail(d,Error::profile,0x1F13DC);
        const auto count=[](float value,std::int32_t& output)->bool{
            //188D70/188DE0: distinct binary32 MUL/ADD, CVT.W.S, cap20.
            if(!std::isfinite(value))return false;
            volatile float product=20.f*value;
            volatile float rounded=product+.5f;
            if(!game::roundOriginalUiTextMetric50KG(rounded,1.f,output))return false;
            if(output>20)output=20;
            return output>=0;
        };
        for(unsigned row=0;row<6;++row){
            std::int32_t maximum=0,current=0;
            if(!count(stats.maximum0408[row],maximum)||!count(stats.current1920[row],current)||current>maximum)
                return fail(d,Error::unsupportedStat,0x188D70);
            s->statCounts[row]=static_cast<std::uint32_t>(maximum);
            for(std::int32_t n=0;n<maximum;++n){
                OriginalUiTournamentStatPacket50KG packet;
                packet.objectOffset=0x1060+row*0x780+std::uint32_t(n)*0x60;packet.image=12;
                auto& quad=packet.sprite;quad.materialName=s->materials[12].registeredName.c_str();
                quad.sourcePC=0x1F2778;
                quad.sourcePacketColor=game::originalUiTextPacketColor50KG(
                    n<current?appearance.accent17FA00:0x00333333,255);
                //1F13E0 rowY;1F1690 x. Explicit source5x10 dimensions.
                const float x=156.f+7.f*float(n),y=164.f+18.f*float(row);
                quad.vertices={{{x,y,0,0},{x+5,y,1,0},{x,y+10,0,1},{x+5,y+10,1,1}}};
                s->statPackets.push_back(packet);
            }
        }

        // Complete medal subsection1F27AC..1F2880 in source packet order:
        // outline, optional large trophy, course icon, repeated13 times.
        // This is only that subsection, never a sealed complete-screen frame.
        constexpr float outlineX[]{54,91,128,165,228,265,302,339,399,436,473,510,547};
        const auto append=[&](std::uint32_t offset,std::uint32_t image,float x,float y,std::uint32_t pc)->bool{
            const auto* atlas=s->images->textures()->texture(image);
            if(!atlas||!atlas->texture.width||!atlas->texture.height)return false;
            OriginalUiTournamentMedalPacket50KG packet;packet.objectOffset=offset;packet.image=image;
            auto& quad=packet.sprite;quad.materialName=s->materials[image].registeredName.c_str();
            quad.sourcePC=pc;
            // Appearance1F1D80: every outline, unearned icon and visible
            // large trophy takes17FA00; completed icons retain source white.
            if(image==1||image==11||(image>=8&&image<=10))
                quad.sourcePacketColor=game::originalUiTextPacketColor50KG(appearance.accent17FA00,255);
            const float right=x+float(atlas->texture.width),bottom=y+float(atlas->texture.height);
            quad.vertices={{{x,y,0,0},{right,y,1,0},{x,bottom,0,1},{right,bottom,1,1}}};
            s->medalPackets.push_back(packet);return true;
        };
        s->medalPackets.reserve(29);
        for(unsigned i=0;i<13;++i){
            if(!append(0x4C0+i*0x60,1,outlineX[i],314,0x1F27B4))return fail(d,Error::image,0x1F27B4);
            const auto category=s->medals[i].category;
            const bool trophy=category&&s->trophyVisibility[category-1]!=0;
            const float x=trophy?outlineX[i]-13:outlineX[i]+4;
            const float y=trophy?298:318;
            if(trophy){
                constexpr std::uint32_t drawPC[]{0x1F27E8,0x1F2824,0x1F2860};
                if(!append(0xE80+(category-1)*0x60,s->trophyImages[category-1],x,y,drawPC[category-1]))
                    return fail(d,Error::image,drawPC[category-1]);
            }
            if(!append(s->medals[i].objectOffset,s->medals[i].image,x,y,0x1F286C))
                return fail(d,Error::image,0x1F286C);
        }
        if(!sameStamp(s->profile->stamp(),s->career.stamp))return fail(d,Error::profile,0x1EEC40);
        s->bankInput.archives={s->appStart,s->tournament,s->fonts,s->backgrounds};
        s->bankInput.owners={s->images};
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,Error::allocation,0x1EEC40);}
}
bool OriginalUiTournamentCareerResources50KG::loaded()const noexcept{return bool(storage_);}
bool OriginalUiTournamentCareerResources50KG::current()const noexcept {
    return storage_&&!storage_->canonicalMarkerTerminal&&sameStamp(storage_->profile->stamp(),storage_->career.stamp);
}
const OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot* OriginalUiTournamentCareerResources50KG::career()const noexcept{return storage_?&storage_->career:nullptr;}
const assets::OriginalUiTournamentPageData50KG* OriginalUiTournamentCareerResources50KG::page()const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiRiderPodAssets50KG* OriginalUiTournamentCareerResources50KG::pod()const noexcept{return storage_?&storage_->pod:nullptr;}
std::shared_ptr<const assets::OriginalUiScreenAssets50KG> OriginalUiTournamentCareerResources50KG::images()const noexcept{return storage_?storage_->images:nullptr;}
const std::array<OriginalUiTournamentNumber50KG,3>* OriginalUiTournamentCareerResources50KG::numbers()const noexcept{return storage_?&storage_->numbers:nullptr;}
const std::array<OriginalUiTournamentMedal50KG,13>* OriginalUiTournamentCareerResources50KG::medals()const noexcept{return storage_?&storage_->medals:nullptr;}
const std::array<std::uint32_t,3>* OriginalUiTournamentCareerResources50KG::trophyImages()const noexcept{return storage_?&storage_->trophyImages:nullptr;}

const std::array<std::uint32_t,3>* OriginalUiTournamentCareerResources50KG::trophyVisibility4050()const noexcept{return storage_?&storage_->trophyVisibility:nullptr;}
const std::array<OriginalUiTournamentCareerMaterial50KG,24>* OriginalUiTournamentCareerResources50KG::materials()const noexcept{return storage_?&storage_->materials:nullptr;}
const assets::OriginalUiImageBankInput50KG* OriginalUiTournamentCareerResources50KG::bankInput()const noexcept{return storage_?&storage_->bankInput:nullptr;}

const std::vector<OriginalUiTournamentMedalPacket50KG>* OriginalUiTournamentCareerResources50KG::medalPackets(std::uint8_t popup)const noexcept {
    return current()&&!(popup&1u)?&storage_->medalPackets:nullptr;
}

const std::vector<OriginalUiTournamentStatPacket50KG>* OriginalUiTournamentCareerResources50KG::statPackets(std::uint8_t popup)const noexcept {
    return current()&&!(popup&1u)?&storage_->statPackets:nullptr;
}
const std::array<std::uint32_t,6>* OriginalUiTournamentCareerResources50KG::statCounts3D60()const noexcept {
    return storage_?&storage_->statCounts:nullptr;
}
const std::array<OriginalUiTournamentMedalPacket50KG,4>* OriginalUiTournamentCareerResources50KG::backgroundPackets(std::uint8_t popup)const noexcept {
    return current()&&!(popup&1u)?&storage_->backgroundPackets:nullptr;
}
const OriginalUiTournamentMedalPacket50KG* OriginalUiTournamentCareerResources50KG::portraitPacket(std::uint8_t popup)const noexcept {
    return current()&&!(popup&1u)?&storage_->portraitPacket:nullptr;
}

bool OriginalUiTournamentCareerResources50KG::sampleMarker(
    const OriginalUiMainFrameClock50KG& clock,std::uint8_t popup,OriginalUiTournamentMedalPacket50KG& output) noexcept {
    if(!current()||!clock.known||(popup&1u))return false;
    auto next=storage_->markerAnimation;std::uint32_t frame=0;
    if(clock.advanceAnimations){
        if(!game::advanceOriginalUiTextureAnimation50KG(next,clock.milliseconds,frame))return false;
    }else if(!game::selectOriginalUiTextureAnimationFrame50KG(next,frame))return false;
    if(frame>=storage_->markerPackets.size())return false;
    output=storage_->markerPackets[frame];storage_->markerAnimation=next;return true;
}

bool OriginalUiTournamentCareerResources50KG::canonicalMarkerTerminal() const noexcept {
    return storage_&&storage_->canonicalMarkerTerminal;
}
bool OriginalUiTournamentCareerResources50KG::sampleCanonicalMarker(
    const OriginalUiMainFrameClock50KG& clock,assets::OriginalUiScreenMaterialReference50KG& reference,
    const assets::OriginalStandardMaterialCollectorReceipt50LA& receipt,std::uint8_t popup,
    OriginalUiTournamentMedalPacket50KG& output,assets::OriginalStandardMaterialDiagnostic50LA& d) noexcept {
    d={};
    //The concrete composite owner remembers a failed post-effect projection.
    //Retry cannot dispatch again, even with a new clock/reference/receipt.
    if(storage_&&storage_->canonicalMarkerTerminal){d=storage_->canonicalMarkerFailure;return false;}
    const auto failure=[&]() noexcept {
        if(storage_&&d.sourceEffectsCommitted){
            storage_->canonicalMarkerFailure=d;
            storage_->canonicalMarkerTerminal=true;
        }
        return false;
    };
    const auto reject=[&](std::uint32_t pc){
        d.status=assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=pc;return failure();
    };
    if(!current()||(popup&1u))return reject(0x1F2884);
    //No legacy known/milliseconds/advanceAnimations inference. Caller schedules
    //the real24A740 service; canonical controller owns duplicate/pause gates.
    if(!reference.serviceTournamentMarker24A740(clock.applicationAnimation,d))return failure();
    const bool effects=d.sourceEffectsCommitted;
    assets::OriginalStandardMaterialMapBindingView50LA binding;
    if(!reference.prepareTournamentMarkerFrame24B278(receipt,binding,d)){
        d.sourceEffectsCommitted=d.sourceEffectsCommitted||effects;return failure();
    }
    d.sourceEffectsCommitted=d.sourceEffectsCommitted||effects;
    std::size_t offset=0,bytes=0;
    if(!binding.texture||!binding.texture->pstExtent24E880(offset,bytes))return reject(0x102050);
    const auto* width=binding.texture->sourceWidth14();const auto* height=binding.texture->sourceHeight18();
    if(!width||!height)return reject(0x102508);
    //Each candidate is an already-authenticated decode owner. Matching the
    //exact member-relative PST extent joins the actual child, not a guessed9
    //or parallel arithmetic phase. The copied packet borrows this resource bank.
    for(std::size_t i=0;i<storage_->markerPackets.size();++i){
        const auto image=storage_->markerPackets[i].image;
        const auto* provenance=storage_->images->sourceProvenance(image);
        const auto* decoded=storage_->images->decodedTexture(image);
        if(!provenance||provenance->memberIdentity!=storage_->markerMember||provenance->celIdentity||
           provenance->pstOffset!=offset||provenance->pstBytes!=bytes)continue;
        if(!decoded||decoded->mips.empty()||decoded->mips.back().width!=static_cast<std::uint32_t>(*width)||
           decoded->mips.back().height!=static_cast<std::uint32_t>(*height))return reject(0x102508);
        output=storage_->markerPackets[i];return true;
    }
    return reject(0x102050);
}

}
