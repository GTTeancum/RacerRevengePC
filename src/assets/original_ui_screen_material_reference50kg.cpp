#include "original_ui_screen_material_reference50kg.h"
#include "original_ui_material_application50la.h"
#include "original_standard_texture_resource50la.h"
#include <cstring>
#include <exception>
#include <utility>
namespace rr::assets {
namespace {
using D=OriginalStandardMaterialDiagnostic50LA;
using S=OriginalStandardMaterialStatus50LA;
bool fail(D& d,S status,std::uint32_t pc) noexcept {d.status=status;d.sourcePC=pc;return false;}
bool directory(const char* value,std::size_t capacity) noexcept {
    if(!value||!capacity)return false;
    std::size_t n=0;while(n<capacity&&n<256&&value[n])++n;
    if(n==capacity||n==256)return false;
    if(n&&value[n-1]=='\\')--n;
    constexpr char expected[]="ui\\textures";
    if(n!=sizeof(expected)-1)return false;
    for(std::size_t i=0;i<n;++i){
        char c=value[i];if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
        if(c!=expected[i])return false;
    }
    return true;
}
}
OriginalUiScreenMaterialReference50KG::~OriginalUiScreenMaterialReference50KG(){release299AC0();}
bool OriginalUiScreenMaterialReference50KG::acquireTournamentOutline1EF884(
    std::uint8_t outline,std::shared_ptr<const OriginalUiArchive50KG> tournament,
    std::shared_ptr<const OriginalUiMaterialApplication50LA> application,
    std::shared_ptr<OriginalStandardMaterialTable50LA> materials,
    std::shared_ptr<OriginalStandardTextureTable50LA> textures,
    const char* actualDirectory,std::size_t capacity,D& d) noexcept {
    d={};
    if(started_)return fail(d,S::alreadyPrepared,0x1EF884);
    if(outline>=13||!tournament||!tournament->loaded()||
        tournament->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament))
        return fail(d,S::archive,0x1EF884);
    if(!application||!materials||!materials->screenApplicationMatches(*application)||
        !textures||!textures->prepared()||!directory(actualDirectory,capacity))
        return fail(d,S::input,0x2DF11C);
    if(materials->populationTerminal()||materials->demandTerminal()||textures->populationTerminal())
        return fail(d,S::phase,0x2DF144);
    // No absence fallback or generated PSM. Only the genuine authored pair.
    const auto psm=tournament->find("ui\\textures\\medlgout.psm");
    const auto pst=tournament->find("ui\\textures\\medlgout.pst");
    if(!psm.identity||!psm.data||psm.size!=psm.identity->bytes||
        !pst.identity||!pst.data||pst.size!=pst.identity->bytes)
        return fail(d,S::source,0x1EF884);
    try {
        std::string requested=actualDirectory;if(requested.back()!='\\')requested+='\\';
        requested+="medlgout"; // original3AF0A8; no suffix stripping needed.
        tournament_=std::move(tournament);application_=std::move(application);
        materials_=std::move(materials);textures_=std::move(textures);objectOffset_=0x4C0u+0x60u*outline;started_=true;
        std::int32_t row=-1;const bool found=materials_->find298C80(requested,row,d);
        material00_=row;d.row=row;d.storedSlots=1;d.sourceEffectsCommitted=true;
        if(!found)return false;
        if(!materials_->retainScreen14D590(row,d))return false;
        retained_=true;d.retainedSlots=1;return true;
    }catch(...){return fail(d,S::allocation,0x299BE0);}
}
bool OriginalUiScreenMaterialReference50KG::acquireTournamentMarker1EFEDC(
    std::shared_ptr<const OriginalUiArchive50KG> tournament,
    std::shared_ptr<const OriginalUiMaterialApplication50LA> application,
    std::shared_ptr<OriginalStandardMaterialTable50LA> materials,
    std::shared_ptr<OriginalStandardTextureTable50LA> textures,
    const char* actualDirectory,std::size_t capacity,D& d) noexcept {
    d={};
    if(started_)return fail(d,S::alreadyPrepared,0x1EFEDC);
    if(!tournament||!tournament->loaded()||
        tournament->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament))
        return fail(d,S::archive,0x1EFEDC);
    if(!application||!materials||!materials->screenApplicationMatches(*application)||
        !textures||!textures->prepared()||!directory(actualDirectory,capacity))
        return fail(d,S::input,0x2DF11C);
    if(materials->populationTerminal()||materials->demandTerminal()||textures->populationTerminal())
        return fail(d,S::phase,0x2DF144);
    try {
    // No absence fallback or generated PSM. Only the genuine authored pair.
    const auto psm=tournament->find("ui\\textures\\medstar.psm");
    const auto pst=tournament->find("ui\\textures\\medstar.atm");
    if(!psm.identity||!psm.data||psm.size!=psm.identity->bytes||
        !pst.identity||!pst.data||pst.size!=pst.identity->bytes)
        return fail(d,S::source,0x1EFEDC);
        std::string requested=actualDirectory;if(requested.back()!='\\')requested+='\\';
        requested+="medstar"; //Original1EFEDC name; no generated per-frame names.
        tournament_=std::move(tournament);application_=std::move(application);
        materials_=std::move(materials);textures_=std::move(textures);objectOffset_=0xFA0u;marker_=true;started_=true;
        std::int32_t row=-1;const bool found=materials_->find298C80(requested,row,d);
        material00_=row;d.row=row;d.storedSlots=1;d.sourceEffectsCommitted=true;
        if(!found)return false;
        if(!materials_->retainScreen14D590(row,d))return false;
        retained_=true;d.retainedSlots=1;return true;
    }catch(...){return fail(d,S::allocation,0x299BE0);}
}
bool OriginalUiScreenMaterialReference50KG::acquireTournamentFallback2DF0B0(
    OriginalUiTournamentFallback50KG kind,std::uint32_t objectIndex,std::uint8_t career,
    std::shared_ptr<const OriginalUiArchive50KG> tournament,
    std::shared_ptr<const OriginalUiMaterialApplication50LA> application,
    std::shared_ptr<OriginalStandardMaterialTable50LA> materials,
    std::shared_ptr<OriginalStandardTextureTable50LA> textures,
    const char* actualDirectory,std::size_t capacity,D& d) noexcept {
    d={};if(started_)return fail(d,S::alreadyPrepared,0x2DF0B0);
    const auto k=static_cast<unsigned>(kind);
    if(k>7||(k!=7&&career!=0)||(k<3&&objectIndex>=13)||
        (k>=3&&k<6&&objectIndex>=3)||(k==6&&objectIndex>=120)||
        (k==7&&(objectIndex!=0||career<1||career>26||career==5||career==6||career==13)))
        return fail(d,S::input,0x2DF0B0);
    if(!tournament||!tournament->loaded()||
        tournament->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament))
        return fail(d,S::archive,0x2DF0B0);
    if(!application||!materials||!materials->screenApplicationMatches(*application)||
        !textures||!textures->prepared()||!directory(actualDirectory,capacity))
        return fail(d,S::input,0x2DF11C);
    if(materials->populationTerminal()||materials->demandTerminal()||textures->populationTerminal())
        return fail(d,S::phase,0x2DF144);
    try {
        constexpr const char* names[]{"trphgldi","trphslvi","trphbrzi","trphglda","trphslva","trphbrza","notch"};
        std::string name=k<7?names[k]:"00_flrg";
        if(k==7){name[0]=char('0'+career/10);name[1]=char('0'+career%10);}
        const auto path=std::string("ui\\textures\\")+name;
        const auto pst=tournament->find(path+".pst"),page=tournament->find("UI\\UITourn\\standing.dat");
        if(tournament->find(path+".psm").identity||!pst.identity||!pst.data||
            pst.size!=pst.identity->bytes||!page.identity||!page.data)
            return fail(d,S::source,0x29B638);
        std::string requested=actualDirectory;if(requested.back()!='\\')requested+='\\';
        requested+=name;
        tournament_=std::move(tournament);application_=std::move(application);
        materials_=std::move(materials);textures_=std::move(textures);
        objectOffset_=k<3?0x9A0+objectIndex*0x60:k<6?0xE80+objectIndex*0x60:
            k==6?0x1060+objectIndex*0x60:0x1000;
        constexpr std::uint32_t trophyStores[]{0x1F25AC,0x1F25E4,0x1F261C};
        fallbackDemandStorePC_=k<3?0x1F2564:k<6?trophyStores[objectIndex]:
            k==6?0x1F2690:0x1F24FC;
        missingPsm_=true;started_=true;
        std::int32_t row=-1;const bool found=materials_->find298C80(requested,row,d);
        material00_=row;d.row=row;d.storedSlots=1;d.sourceEffectsCommitted=true;
        if(!found)return false;
        if(!materials_->retainTournamentFallback14D590(row,tournament_,textures_,d))return false;
        retained_=true;d.retainedSlots=1;return true;
    }catch(...){return fail(d,S::allocation,0x299BE0);}
}
bool OriginalUiScreenMaterialReference50KG::raiseTournamentOutlineDemand1F2508(D& d) const noexcept {
    d={};
    if(marker_||missingPsm_||!retained_||!materials_||!application_||!textures_||!tournament_||
        !materials_->screenApplicationMatches(*application_))
        return fail(d,S::reference,0x1F2508);
    return materials_->demandTournamentOutline2DF8A0(material00_,*textures_,
        tournament_->find("ui\\textures\\medlgout.psm"),
        tournament_->find("ui\\textures\\medlgout.pst"),d);
}
bool OriginalUiScreenMaterialReference50KG::raiseTournamentFallbackDemand1F24B0(D& d) const noexcept {
    d={};
    if(!missingPsm_||!retained_||!materials_||!application_||!textures_||!tournament_||
        !materials_->screenApplicationMatches(*application_))
        return fail(d,S::reference,0x1F24B0);
    return materials_->demandTournamentFallback2DF8A0(material00_,*textures_,*tournament_,
        fallbackDemandStorePC_,d);
}
bool OriginalUiScreenMaterialReference50KG::raiseTournamentMarkerDemand1F2624(D& d) const noexcept {
    d={};
    if(!marker_||!retained_||!materials_||!application_||!textures_||!tournament_||
       !materials_->screenApplicationMatches(*application_))return fail(d,S::reference,0x1F2624);
    try {
        return materials_->demandTournamentMarker2DF8A0(material00_,*textures_,
            tournament_->find("ui\\textures\\medstar.psm"),tournament_->find("ui\\textures\\medstar.atm"),d);
    }catch(...){return fail(d,S::allocation,0x2DF8A0);}
}
bool OriginalUiScreenMaterialReference50KG::serviceTournamentMarker24A740(
    const rr::game::OriginalApplicationAnimationClockView50KG& clock,D& d) noexcept {
    d={};
    if(!marker_||!retained_||!materials_||!application_||!textures_||!tournament_||
       !materials_->screenApplicationMatches(*application_))return fail(d,S::reference,0x2DF864);
    try {
        return materials_->serviceTournamentMarker24A740(material00_,*textures_,
            tournament_->find("ui\\textures\\medstar.psm"),tournament_->find("ui\\textures\\medstar.atm"),clock,d);
    }catch(...){return fail(d,S::allocation,0x24A740);}
}
bool OriginalUiScreenMaterialReference50KG::prepareTournamentMarkerFrame24B278(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,
    OriginalStandardMaterialMapBindingView50LA& output,D& d) const noexcept {
    d={};
    try {
    if(!marker_||!retained_||!materials_||!textures_||!application_||!tournament_||
       !materials_->screenApplicationMatches(*application_))return fail(d,S::reference,0x2DF864);
    OriginalStandardMaterialRowView50LA row;
    if(!materials_->row(material00_,row)||!row.object04||!row.references08||*row.references08<=0||
       row.object04->textures383400()!=textures_.get())return fail(d,S::reference,0x24A740);
    const auto psm=tournament_->find("ui\\textures\\medstar.psm");
    const auto atm=tournament_->find("ui\\textures\\medstar.atm");
    const auto* source=row.object04->source();
    const auto* sourceBytes=source?source->originalBytes():nullptr;
    if(!psm.identity||!atm.identity||!psm.data||!atm.data||!sourceBytes||
       sourceBytes->size()!=psm.size||std::memcmp(sourceBytes->data(),psm.data,psm.size))
        return fail(d,S::source,0x24A740);
    OriginalStandardMaterialMapBindingView50LA next;
    if(!materials_->prepareColdScreenPrototypeMap249610(receipt,material00_,0,0,next,d))return false;
    const auto* bytes=next.texture?next.texture->originalBytes():nullptr;
    std::size_t offset=0,size=0;
    if(next.preparation!=OriginalStandardMaterialMapPreparation50LA::ready||!bytes||
       bytes->size()!=atm.size||std::memcmp(bytes->data(),atm.data,atm.size)||
       !next.texture->pstExtent24E880(offset,size)||offset<12||offset>atm.size||size>atm.size-offset)
        return fail(d,S::texture,0x102050);
    output=next;return true;
    }catch(...){return fail(d,S::allocation,0x24B278);}
}
void OriginalUiScreenMaterialReference50KG::release299AC0() noexcept {
    if(!retained_)return;
    if(!materials_||!materials_->release14D500(material00_))std::terminate();
    retained_=false;
    // Retain manager/source pins for inspection until native destruction.
}
}
