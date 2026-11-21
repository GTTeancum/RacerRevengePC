#include "original_ui_tournament_career_resources50kg.h"
#include "original_ui_rider_session50kg.h"
#include "original_ui_course_session50kg.h"
#include "original_ui_garage_session50kg.h"
#include <variant>
#include <utility>
#include <cstring>
#include <limits>
#include "original_ui_main_draw50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include "../../../src/assets/original_standard_texture_resource50la.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include <cmath>
#include "../../../src/game/original_ui_text_position50kg.h"
#include "../../../src/game/dlc_career50dl.h"
#include "../../../src/game/dlc_addon_ui_archive50dl.h"
#include "../../../src/assets/dlc_registry50dl.h"
#include "../../../src/assets/original_ui_addon_material50dl.h"
#include <cstdio>

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
bool sameCommon(const OriginalUiMainProfile50KG& a,const OriginalUiMainProfile50KG& b) noexcept {
    return a.known==b.known&&a.mode34==b.mode34&&a.previousMode38==b.previousMode38&&
        a.savedSlot6E0==b.savedSlot6E0&&a.availability11018==b.availability11018&&
        a.flags11054==b.flags11054&&a.playerBits1125C==b.playerBits1125C;
}
bool sameSelection(const game::OriginalUiRiderSelectionProfile50KG& a,
    const game::OriginalUiRiderSelectionProfile50KG& b) noexcept {
    if(a.ownerReady!=b.ownerReady||a.mode34!=b.mode34||a.remembered6F0!=b.remembered6F0||
       a.remembered6F4!=b.remembered6F4||a.remembered6F8!=b.remembered6F8||a.remembered6FC!=b.remembered6FC)return false;
    for(std::size_t i=0;i<a.riders.size();++i){const auto& x=a.riders[i];const auto& y=b.riders[i];
        if(x.ownerReady!=y.ownerReady||x.unlock10FAC!=y.unlock10FAC||x.hidden730!=y.hidden730||
           x.careerByteOwnerReady!=y.careerByteOwnerReady||x.career1938!=y.career1938)return false;}
    return true;
}
bool sameCourse(const game::OriginalUiCourseRememberedProfile50KG& a,
    const game::OriginalUiCourseRememberedProfile50KG& b) noexcept {
    return a.ownerReady==b.ownerReady&&a.mode34==b.mode34&&a.course700==b.course700&&a.laps704==b.laps704&&
        a.entrants708==b.entrants708&&a.subtype70C==b.subtype70C&&a.course710==b.course710&&
        a.entrants714==b.entrants714&&a.laps718==b.laps718&&a.packed1125E==b.packed1125E&&
        a.cardFlag100BC==b.cardFlag100BC&&a.availability11018==b.availability11018&&
        a.planet0Primary79C==b.planet0Primary79C&&a.planet0Secondary7A0==b.planet0Secondary7A0&&
        a.courseOrder100C4==b.courseOrder100C4;
}
// Compare actual binary32 payloads, including signed zero. Padding is not data.
bool sameFloat(float a,float b) noexcept {
    static_assert(sizeof(float)==sizeof(std::uint32_t));
    std::uint32_t x=0,y=0;std::memcpy(&x,&a,sizeof(x));std::memcpy(&y,&b,sizeof(y));return x==y;
}
bool sameStats(const game::OriginalUiRiderStatInput50KG& a,const game::OriginalUiRiderStatInput50KG& b) noexcept {
    if(a.profileReady!=b.profileReady||a.selectedRider!=b.selectedRider||a.observedRider!=b.observedRider||
       a.mode34!=b.mode34||a.unlocked10FAC!=b.unlocked10FAC||!sameFloat(a.panelY4DC,b.panelY4DC)||
       a.accentColor17FA00!=b.accentColor17FA00||a.medal1938!=b.medal1938||a.careerTrack219C!=b.careerTrack219C)return false;
    for(std::size_t i=0;i<a.current1920.size();++i)
        if(!sameFloat(a.current1920[i],b.current1920[i])||!sameFloat(a.maximum0408[i],b.maximum0408[i]))return false;
    return true;
}
bool samePodCareer(const game::OriginalTournamentPodCareer50KG& a,const game::OriginalTournamentPodCareer50KG& b) noexcept {
    if(a.medals1938!=b.medals1938||a.remainingPrizes1946!=b.remainingPrizes1946)return false;
    for(std::size_t i=0;i<a.abilities1920.size();++i)if(!sameFloat(a.abilities1920[i],b.abilities1920[i]))return false;
    return true;
}
// Stamp equality is deliberately separate: attach projects only the observed
// exact Confirm generation, while every actual career field must stay equal.
bool sameCareerValues(const OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot& a,
    const OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot& b) noexcept {
    return a.career100C0==b.career100C0&&a.earned2190==b.earned2190&&a.unspent2194==b.unspent2194&&
        a.kills2198==b.kills2198&&a.progress219C==b.progress219C&&a.availability11018==b.availability11018&&
        a.cardFlag100BC==b.cardFlag100BC&&a.nativeBetaStorage==b.nativeBetaStorage&&a.medals1938==b.medals1938&&a.remainingPrizes1946==b.remainingPrizes1946;
}
bool sameTextMetadata(const OriginalUiRiderProfileOwner50KG::TournamentTextMetadataSnapshot& a,
    const OriginalUiRiderProfileOwner50KG::TournamentTextMetadataSnapshot& b) noexcept {
    return a.career100C0==b.career100C0&&a.racerName3C==b.racerName3C&&a.courseTitle1127C==b.courseTitle1127C&&
        a.planetTitle11281==b.planetTitle11281&&a.progressIndex==b.progressIndex&&a.course100C4==b.course100C4&&
        sameStamp(a.stamp,b.stamp);
}
bool sameDecision(const game::OriginalUiRouteDecision50KG& a,const game::OriginalUiRouteDecision50KG& b) noexcept {
    const auto& x=a.request;const auto& y=b.request;
    if(a.action!=b.action||a.blockers!=b.blockers||a.admitted!=b.admitted||a.profileProposalComplete!=b.profileProposalComplete||
       a.selectedSlot!=b.selectedSlot||a.selectedRider!=b.selectedRider||a.inputPC!=b.inputPC||x.kind!=y.kind||
       x.sourceScreen!=y.sourceScreen||x.sourcePage!=y.sourcePage||x.targetScreen!=y.targetScreen||
       x.targetPage!=y.targetPage||x.sourcePC!=y.sourcePC||a.profileChanges.count!=b.profileChanges.count||
       a.profileChanges.count>a.profileChanges.values.size())return false;
    for(std::size_t i=0;i<a.profileChanges.count;++i){const auto& p=a.profileChanges.values[i];const auto& q=b.profileChanges.values[i];
        if(p.field!=q.field||p.operation!=q.operation||p.value!=q.value||p.replaceMask!=q.replaceMask||p.sourcePC!=q.sourcePC)return false;}
    return true;
}
bool sameProposal(const OriginalUiRiderRouteProposal50KG& a,const OriginalUiRiderRouteProposal50KG& b) noexcept {
    return a.source&&a.source==b.source&&a.serial&&a.serial==b.serial&&sameStamp(a.profileStamp,b.profileStamp)&&sameDecision(a.decision,b.decision);
}
using EntryProposal=std::variant<std::monostate,OriginalUiRiderRouteProposal50KG,OriginalUiCourseRouteProposal50KG,std::shared_ptr<const OriginalUiGarageSession50KG>>;
bool sameProposal(const OriginalUiRiderRouteProposal50KG& a,const EntryProposal& b) noexcept {
    const auto* p=std::get_if<OriginalUiRiderRouteProposal50KG>(&b);return p&&sameProposal(a,*p);
}
bool sameProposal(const OriginalUiCourseRouteProposal50KG& a,const EntryProposal& b) noexcept {
    const auto* p=std::get_if<OriginalUiCourseRouteProposal50KG>(&b);
    return p&&a.source&&a.source==p->source&&a.serial&&a.serial==p->serial&&
        sameStamp(a.profileStamp,p->profileStamp)&&sameDecision(a.decision,p->decision);
}
struct ConfirmDependencies {
    OriginalUiMainProfile50KG common{};
    game::OriginalUiRiderSelectionProfile50KG selection{};
    game::OriginalUiCourseRememberedProfile50KG course{};
    OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot career{};
    OriginalUiRiderProfileOwner50KG::TournamentTextMetadataSnapshot text{};
    game::OriginalTournamentPodCareer50KG pod{};
    game::OriginalUiRiderStatInput50KG stats{};
    OriginalUiRiderProfileColors50KG colors{};
    std::uint8_t packed1125D=0;
    std::uint32_t comparison100C0=0;
};
bool captureConfirmDependencies(const OriginalUiRiderProfileOwner50KG& profile,
    std::uint32_t selected,ConfirmDependencies& out) noexcept {
    const auto* common=profile.commonProfile();const auto before=profile.stamp();ConfirmDependencies next;
    if(!common||!common->known||before.mode34!=2||common->mode34!=2||selected>=27)return false;
    next.common=*common;
    if(!profile.selectionSnapshot(next.selection)||!profile.courseSnapshot(next.course)||
       !profile.tournamentCareerSnapshot(selected,next.career)||!profile.tournamentTextMetadataSnapshot(selected,next.text)||
       !profile.tournamentPodCareerSnapshot(selected,selected,next.pod)||
       !profile.statSnapshot(selected,0,next.stats)||!profile.colors(selected,next.colors)||
       !profile.riderConfirmSnapshot(next.packed1125D,next.comparison100C0)||
       !sameStamp(before,next.career.stamp)||!sameStamp(before,next.text.stamp)||
       !sameStamp(before,profile.stamp())||!sameCommon(*common,next.common))return false;
    out=next;return true;
}
bool sameDependencies(const ConfirmDependencies& a,const ConfirmDependencies& b) noexcept {
    return sameCommon(a.common,b.common)&&sameSelection(a.selection,b.selection)&&sameCourse(a.course,b.course)&&
        sameCareerValues(a.career,b.career)&&sameStamp(a.career.stamp,b.career.stamp)&&sameTextMetadata(a.text,b.text)&&
        samePodCareer(a.pod,b.pod)&&
        sameStats(a.stats,b.stats)&&a.colors.accent17FA00==b.colors.accent17FA00&&
        a.colors.secondary17FA10==b.colors.secondary17FA10&&a.packed1125D==b.packed1125D&&a.comparison100C0==b.comparison100C0;
}
bool exactTournamentProposal(const OriginalUiRiderRouteProposal50KG& proposal,const ConfirmDependencies& before) noexcept {
    const auto selected=before.career.career100C0;
    if(!proposal.source||!proposal.serial||!sameStamp(proposal.profileStamp,before.career.stamp)||
       before.common.mode34!=2||(before.common.playerBits1125C&0x60)!=0x20||(before.career.cardFlag100BC&&!before.career.nativeBetaStorage)||
       (before.course.cardFlag100BC&&!before.career.nativeBetaStorage)||selected>=before.selection.riders.size()||
       !before.selection.riders[selected].ownerReady||!before.selection.riders[selected].unlock10FAC)return false;
    game::OriginalUiRiderContext50KG context;context.playerIndex=0;context.selectedRider=selected;context.mode34=2;
    context.packed1125C=before.common.playerBits1125C;context.unlockOwnerReady=true;
    context.comparisonRider100C0=before.comparison100C0;
    context.selectedUnlockWord10FAC=before.selection.riders[selected].unlock10FAC;
    context.admission.sourceControllerReady=true;context.admission.selectionOwnerReady=true;context.admission.profileOwnerReady=true;
    game::OriginalUiRouteDecision50KG exact;
    return game::dispatchOriginalUiRider50KG(0,game::OriginalUiRouteInput50KG::activate,context,exact)==game::OriginalUiRouteStatus50KG::unavailable&&
        exact.request.targetScreen==9&&exact.request.targetPage==1&&exact.request.sourcePC==0x1EA5E4&&
        exact.profileChanges.count==3&&exact.blockers==static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)&&
        sameDecision(exact,proposal.decision);
}
bool exactCourseBackProposal(const OriginalUiCourseRouteProposal50KG& proposal,const ConfirmDependencies& before) noexcept {
    if(!proposal.source||!proposal.serial||!sameStamp(proposal.profileStamp,before.career.stamp)||
       before.common.mode34!=2||(before.common.playerBits1125C&0x60)!=0x20||(before.career.cardFlag100BC&&!before.career.nativeBetaStorage)||
       (before.course.cardFlag100BC&&!before.career.nativeBetaStorage)||(before.career.progress219C&31u)>=13||
       before.comparison100C0!=before.career.career100C0||(before.packed1125D>>3)!=before.career.career100C0)return false;
    game::OriginalUiCourseContext50KG context;game::OriginalUiRouteDecision50KG exact;
    return proposal.source->routeContext(proposal,context)&&context.mode34==2&&context.courseSlot14E4>=1&&context.courseSlot14E4<=13&&
        game::dispatchOriginalUiCourse50KG(0,game::OriginalUiRouteInput50KG::back,context,exact)==game::OriginalUiRouteStatus50KG::unavailable&&
        exact.request.targetScreen==9&&exact.request.targetPage==1&&exact.request.sourcePC==0x1DF0B4&&
        exact.profileChanges.count==0&&exact.blockers==static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner)&&
        sameDecision(exact,proposal.decision);
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
    const OriginalUiTournamentDrawContribution50KG* nativeMarkerOwner=nullptr;
    bool nonNativeMarkerUsed=false;
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile;
    std::shared_ptr<const assets::OriginalUiArchive50KG> tournament,fonts,appStart,backgrounds;
    // PORT EXTENSION (DLC career): the add-on pod's own menu archive when the
    // selected career is an add-on's (its portrait and background); else null.
    std::shared_ptr<const assets::OriginalUiArchive50KG> addonArt;
    bool addonCareer=false;
    std::shared_ptr<assets::OriginalUiRiderPodAssets50KG> addonPod; // null = the model area stays empty
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
    game::OriginalUiRiderStatInput50KG resourceStats{};
    OriginalUiRiderProfileColors50KG resourceColors{};
    ConfirmDependencies confirmBefore{};
    EntryProposal confirmProposal{};
    bool confirmPrepared=false,confirmAttachAttempted=false,confirmAttachmentTerminal=false;
    Diagnostic confirmAttachmentFailure{};
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
        s->resourceColors=appearance;
        const auto progress=unsigned(s->career.progress219C&31u);
        // Native admission is restricted to progress0..13. Original masks31
        // and still iterates13 entries; rejecting larger progress is stricter.
        if(progress>13)return fail(d,Error::unsupportedProgress,0x1EF88C);
        if(!s->page.load(s->tournament,d.page))return fail(d,Error::page,0x1EDAB8);
        if(!s->pod.loadTournament(s->tournament,selected,d.pod))return fail(d,Error::pod,0x1EEC40);
        // DLC log (only with add-on pods admitted): which archive the pod model came from.
        if(rr::assets::dlc50DL::addonPodCount()){const auto m=s->pod.modelSource();
            std::fprintf(stderr,"dlc_model_source screen=tournament career=%u source=stock_archive archive=UITOURN member=%s%s\n",
                selected,m.identity&&m.identity->name?m.identity->name:"-",
                game::dlc50DL::activeAddonCareer(selected)?" role=anchor_layout_only drawn=never":"");}
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
        // PORT EXTENSION (DLC career, dlc_career50dl.h). The original rule names
        // the SELECTED career's %02d_flrg / %02d_bkg; for an add-on career (100C0
        // holds its anchor) those are the package's own portrait and background
        // (ui.portraits / ui.background), read from its own archive. The anchor's
        // art is never used: a missing member refuses the screen.
        game::dlc50DL::AddonCareerArt50DL art;std::string portraitPath;
        if(const auto* addon=game::dlc50DL::activeAddonCareer(selected)){
            std::string why,backgroundPath;
            if(!game::dlc50DL::loadAddonCareerArt50DL(addon->number,art,why)||art.portraitPst.empty()||
               art.backgroundPst.empty()||art.backgroundPsm.empty()){
                std::fprintf(stderr,"dlc_career tournament_art key=%s status=refused reason=\"%s\"\n",addon->key.c_str(),
                    why.empty()?"package lists no portrait/background":why.c_str());
                return fail(d,Error::image,0x1F17AC);
            }
            game::dlc50DL::addonMemberStem50DL(art.portraitPst,portraitPath,portrait);
            game::dlc50DL::addonMemberStem50DL(art.backgroundPst,backgroundPath,background);
            s->addonArt=art.archive;s->addonCareer=true;
            // The add-on's own pod model (ui.rider_model) in the anchor career's
            // UITourn widget layout. A failure leaves the model area empty
            // (logged); the anchor's pod is never drawn for this career.
            if(const auto* pod=rr::assets::dlc50DL::addonPod(addon->number)){
                auto model=std::make_shared<assets::OriginalUiRiderPodAssets50KG>();assets::OriginalUiRiderPodDiagnostic50KG pd;
                const bool ok=model->loadTournamentAddon(s->tournament,selected,art.archive,pod->ui.riderModel,pd);
                if(ok)s->addonPod=std::move(model);
                std::fprintf(stderr,"dlc_model_source screen=tournament career=%u source=package key=%s member=%s status=%s\n",
                    selected,addon->key.c_str(),pod->ui.riderModel.c_str(),ok?"loaded":"refused");
            }
        }
        const auto addAddon=[&](const std::string& name)->bool {
            // Port-owned image identity (namespace 0x5447, never a catalog row);
            // the bank finds the member in its addonArchives.
            const auto member=s->addonArt->find(name);
            if(!member.identity||!member.data)return false;
            sources.push_back({0x5447000000000000ull|(std::uint64_t(sources.size()+1)<<32),member,0,member.size,{}});return true;
        };
        if(s->addonArt?(!addAddon(art.portraitPst)||!addAddon(art.backgroundPst)):
           (!add(*s->tournament,"ui/textures/"+portrait+".pst","")||
            !add(*s->backgrounds,"ui/textures/"+background+".pst","")))
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
            }else if(i==13&&s->addonArt){
                // DLC career: the same ScreenTexture missing-PSM fallback on the package portrait.
                if(!assets::readOriginalUiAddonPortraitFallbackMaterial50DL(*s->addonArt,portraitPath,s->templates,
                    assets::OriginalUiTemplate50KG::screenMt,runtime,binding.source,d.material))
                    return fail(d,Error::material,d.material.sourcePC);
            }else if(i==14&&s->addonArt){
                // DLC career: the package's own %02d_bkg-rule PSM (ui.background).
                if(!assets::readOriginalUiMaterial50KG(*s->addonArt,art.backgroundPsm,s->templates,
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
        s->resourceStats=stats;
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
        if(s->addonArt)s->bankInput.addonArchives={s->addonArt}; // DLC career art (bank add-on namespace)
        s->bankInput.owners={s->images};
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,Error::allocation,0x1EEC40);}
}
bool OriginalUiTournamentCareerResources50KG::sameProfileOwner(const OriginalUiRiderProfileOwner50KG& p)const noexcept {
    return storage_&&storage_->profile.get()==&p;
}
bool OriginalUiTournamentCareerResources50KG::prepareRiderConfirm(
    const OriginalUiRiderRouteProposal50KG& proposal,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();
    if(!s||!current()||s->confirmPrepared||s->confirmAttachAttempted||!proposal.source||
       !proposal.source->ownsProfile(*s->profile)||!proposal.source->routeProposalCurrent(proposal)||
       !s->profile->canPrepareRiderConfirmToTournament(proposal.decision,proposal.profileStamp))
        return fail(d,Error::profile,0x1EA5E4);
    ConfirmDependencies before;
    if(!captureConfirmDependencies(*s->profile,s->career.career100C0,before)||
       before.career.stamp.generation==std::numeric_limits<std::uint64_t>::max()||
       !sameStamp(before.career.stamp,s->career.stamp)||!sameCareerValues(before.career,s->career)||
       !sameStats(before.stats,s->resourceStats)||
       before.colors.accent17FA00!=s->resourceColors.accent17FA00||
       before.colors.secondary17FA10!=s->resourceColors.secondary17FA10||
       !exactTournamentProposal(proposal,before))return fail(d,Error::profile,0x1EEA64);
    // Freeze only observations of the same live profile and the actual pending
    // source proposal. In particular, do not manufacture a committed profile
    // or acquire any canonical material/animation history here.
    s->confirmBefore=before;s->confirmProposal=proposal;s->confirmPrepared=true;return true;
}
bool OriginalUiTournamentCareerResources50KG::riderConfirmPreparationCurrent(
    const OriginalUiRiderRouteProposal50KG& proposal) const noexcept {
    const auto* s=storage_.get();ConfirmDependencies actual;
    return s&&s->confirmPrepared&&!s->confirmAttachAttempted&&current()&&
        sameProposal(proposal,s->confirmProposal)&&proposal.source->ownsProfile(*s->profile)&&
        proposal.source->routeProposalCurrent(proposal)&&
        s->profile->canPrepareRiderConfirmToTournament(proposal.decision,proposal.profileStamp)&&
        captureConfirmDependencies(*s->profile,s->career.career100C0,actual)&&
        sameDependencies(actual,s->confirmBefore);
}
bool OriginalUiTournamentCareerResources50KG::acceptRiderConfirmCommit(
    const OriginalUiRiderRouteProposal50KG& proposal,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();
    if(!s||!s->confirmPrepared)return fail(d,Error::profile,0x1EA58C);
    if(s->confirmAttachAttempted){
        if(s->confirmAttachmentTerminal)d=s->confirmAttachmentFailure;
        else fail(d,Error::profile,0x1EA58C);
        return false;
    }
    // This is a one-use post-commit observation. A mismatch is terminal, never
    // a reason to undo source writes or to try another stamp/profile/route.
    s->confirmAttachAttempted=true;
    const auto reject=[&]() noexcept {
        fail(d,Error::profile,0x1EA58C);s->confirmAttachmentFailure=d;
        s->confirmAttachmentTerminal=true;return false;
    };
    if(s->canonicalMarkerTerminal||!sameProposal(proposal,s->confirmProposal)||
       !proposal.source->ownsProfile(*s->profile)||
       !sameStamp(s->career.stamp,s->confirmBefore.career.stamp)||
       !sameCareerValues(s->career,s->confirmBefore.career)||
       !exactTournamentProposal(proposal,s->confirmBefore)||
       s->confirmBefore.career.stamp.generation==std::numeric_limits<std::uint64_t>::max())return reject();
    auto expected=s->confirmBefore;const auto selected=expected.career.career100C0;
    expected.selection.remembered6FC=selected; //1EA54C
    expected.packed1125D=static_cast<std::uint8_t>((expected.packed1125D&7u)|(selected<<3)); //1EA58C
    expected.comparison100C0=selected; //1EA5A8
    ++expected.career.stamp.generation;
    ++expected.text.stamp.generation;
    ConfirmDependencies actual;
    if(!captureConfirmDependencies(*s->profile,selected,actual)||!sameDependencies(actual,expected))return reject();
    // Equality above proves all consumed resource values remained unchanged.
    // Reconcile this owner, not an arbitrary stamp supplied by the caller.
    s->career.stamp=actual.career.stamp;s->confirmProposal=std::monostate{};return true;
}
bool OriginalUiTournamentCareerResources50KG::prepareCourseBack(
    const OriginalUiCourseRouteProposal50KG& proposal,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();ConfirmDependencies before;
    if(!s||!current()||s->confirmPrepared||s->confirmAttachAttempted||!proposal.source||
       !proposal.source->ownsProfile(*s->profile)||!proposal.source->routeProposalCurrent(proposal)||
       !captureConfirmDependencies(*s->profile,s->career.career100C0,before)||
       !sameStamp(before.career.stamp,s->career.stamp)||!sameCareerValues(before.career,s->career)||
       !sameStats(before.stats,s->resourceStats)||
       before.colors.accent17FA00!=s->resourceColors.accent17FA00||
       before.colors.secondary17FA10!=s->resourceColors.secondary17FA10||
       !exactCourseBackProposal(proposal,before))return fail(d,Error::profile,0x1DF0B4);
    s->confirmBefore=before;s->confirmProposal=proposal;s->confirmPrepared=true;return true;
}
bool OriginalUiTournamentCareerResources50KG::courseBackPreparationCurrent(
    const OriginalUiCourseRouteProposal50KG& proposal) const noexcept {
    const auto* s=storage_.get();ConfirmDependencies actual;
    return s&&s->confirmPrepared&&!s->confirmAttachAttempted&&current()&&
        sameProposal(proposal,s->confirmProposal)&&proposal.source->ownsProfile(*s->profile)&&
        proposal.source->routeProposalCurrent(proposal)&&
        captureConfirmDependencies(*s->profile,s->career.career100C0,actual)&&
        sameDependencies(actual,s->confirmBefore)&&exactCourseBackProposal(proposal,actual);
}
bool OriginalUiTournamentCareerResources50KG::acceptCourseBackCommit(
    const OriginalUiCourseRouteProposal50KG& proposal,Diagnostic& d) noexcept {
    d={};auto* s=storage_.get();
    if(!s||!s->confirmPrepared)return fail(d,Error::profile,0x1DF0B4);
    if(s->confirmAttachAttempted){
        if(s->confirmAttachmentTerminal)d=s->confirmAttachmentFailure;
        else fail(d,Error::profile,0x1DF0B4);return false;
    }
    s->confirmAttachAttempted=true;
    const auto reject=[&]() noexcept {
        fail(d,Error::profile,0x1DF0B4);s->confirmAttachmentFailure=d;
        s->confirmAttachmentTerminal=true;return false;
    };
    ConfirmDependencies actual;
    if(s->canonicalMarkerTerminal||!sameProposal(proposal,s->confirmProposal)||
       !proposal.source->ownsProfile(*s->profile)||!proposal.source->routeProposalCurrent(proposal)||
       !sameStamp(s->career.stamp,s->confirmBefore.career.stamp)||
       !sameCareerValues(s->career,s->confirmBefore.career)||
       !exactCourseBackProposal(proposal,s->confirmBefore)||
       !captureConfirmDependencies(*s->profile,s->career.career100C0,actual)||
       !sameDependencies(actual,s->confirmBefore))return reject();
    // The route itself writes nothing: retain the genuine unchanged stamp.
    s->confirmProposal=std::monostate{};return true;
}
bool OriginalUiTournamentCareerResources50KG::prepareGarageBack(
    const std::shared_ptr<const OriginalUiGarageSession50KG>& source,Diagnostic& d)noexcept {
    d={};auto* s=storage_.get();ConfirmDependencies before;
    if(!s||!source||!current()||s->confirmPrepared||s->confirmAttachAttempted||
       source->profileOwnerForReturn17DE10()!=s->profile||
       !captureConfirmDependencies(*s->profile,s->career.career100C0,before)||
       !sameStamp(before.career.stamp,s->career.stamp)||!sameCareerValues(before.career,s->career)||
       !sameStats(before.stats,s->resourceStats)||before.colors.accent17FA00!=s->resourceColors.accent17FA00||
       before.colors.secondary17FA10!=s->resourceColors.secondary17FA10)return fail(d,Error::profile,0x17DE10);
    s->confirmBefore=before;s->confirmProposal=source;s->confirmPrepared=true;return true;
}
bool OriginalUiTournamentCareerResources50KG::garageBackPreparationCurrent(
    const std::shared_ptr<const OriginalUiGarageSession50KG>& source)const noexcept {
    const auto* s=storage_.get();ConfirmDependencies actual;
    const auto* held=s?std::get_if<std::shared_ptr<const OriginalUiGarageSession50KG>>(&s->confirmProposal):nullptr;
    return s&&source&&held&&*held==source&&s->confirmPrepared&&!s->confirmAttachAttempted&&current()&&
        source->profileOwnerForReturn17DE10()==s->profile&&
        captureConfirmDependencies(*s->profile,s->career.career100C0,actual)&&sameDependencies(actual,s->confirmBefore);
}
bool OriginalUiTournamentCareerResources50KG::acceptGarageBackCommit(
    const std::shared_ptr<const OriginalUiGarageSession50KG>& source,Diagnostic& d)noexcept {
    d={};auto* s=storage_.get();
    if(!s||!s->confirmPrepared||s->confirmAttachAttempted)return fail(d,Error::profile,0x17DE10);
    const bool current=garageBackPreparationCurrent(source);
    s->confirmAttachAttempted=true;
    if(!current){fail(d,Error::profile,0x17DE10);s->confirmAttachmentFailure=d;s->confirmAttachmentTerminal=true;return false;}
    // No source profile mutation or generation increment on Garage Back.
    // Frontend invokes this only after actual manager2A3510 retirement.
    s->confirmProposal=std::monostate{};return true;
}
bool OriginalUiTournamentCareerResources50KG::loaded()const noexcept{return bool(storage_);}
bool OriginalUiTournamentCareerResources50KG::current()const noexcept {
    return storage_&&!storage_->canonicalMarkerTerminal&&!storage_->confirmAttachmentTerminal&&
        sameStamp(storage_->profile->stamp(),storage_->career.stamp);
}
const OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot* OriginalUiTournamentCareerResources50KG::career()const noexcept{return storage_?&storage_->career:nullptr;}
const assets::OriginalUiTournamentPageData50KG* OriginalUiTournamentCareerResources50KG::page()const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiRiderPodAssets50KG* OriginalUiTournamentCareerResources50KG::pod()const noexcept{return storage_?&storage_->pod:nullptr;}
bool OriginalUiTournamentCareerResources50KG::addonCareer()const noexcept{return storage_&&storage_->addonCareer;}
std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> OriginalUiTournamentCareerResources50KG::addonPod()const noexcept{
    return storage_?storage_->addonPod:nullptr;}
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
    if(!current()||!clock.known||(popup&1u)||storage_->nativeMarkerOwner)return false;
    storage_->nonNativeMarkerUsed=true;
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
    if(!current()||(popup&1u)||storage_->nativeMarkerOwner)return reject(0x1F2884);
    storage_->nonNativeMarkerUsed=true;
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

bool OriginalUiTournamentCareerResources50KG::claimNativeMarker(const OriginalUiTournamentDrawContribution50KG* owner) noexcept {
    if(!storage_||!owner||storage_->nativeMarkerOwner||storage_->nonNativeMarkerUsed||storage_->canonicalMarkerTerminal)return false;
    storage_->nativeMarkerOwner=owner;return true;
}
const game::OriginalUiTextureAnimationConfig50KG* OriginalUiTournamentCareerResources50KG::nativeMarkerConfig(const OriginalUiTournamentDrawContribution50KG* owner) const noexcept {
    return storage_&&owner&&storage_->nativeMarkerOwner==owner?&storage_->markerAnimation.config:nullptr;
}
const OriginalUiTournamentMedalPacket50KG* OriginalUiTournamentCareerResources50KG::nativeMarkerPacket(const OriginalUiTournamentDrawContribution50KG* owner,std::uint32_t frame) const noexcept {
    return current()&&owner&&storage_->nativeMarkerOwner==owner&&frame<storage_->markerPackets.size()?&storage_->markerPackets[frame]:nullptr;
}

}
