#include "original_results_primitive_references50la.h"
#include "original_results_screen_fallback50la.h"
#include "original_standard_material_domain50la.h"
#include "original_ui_screen_assets50kg.h"
#include "dlc_registry50dl.h"
#include "dlc_files50dl.h"
#include "original_ui_addon_material50dl.h"
#include "original_standard_material_table50la.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <cstdio>
#include <cstring>
#include <bit>
#include <exception>
#include <limits>
#include <initializer_list>
#include <utility>
namespace rr::assets {
namespace {
using D=OriginalStandardMaterialDiagnostic50LA;
using S=OriginalStandardMaterialStatus50LA;
using Provider=OriginalStandardArchiveProvider50LA;
bool fail(D& d,S status,std::uint32_t pc)noexcept {d.status=status;d.sourcePC=pc;return false;}
bool same(const std::string& a,const std::string& b)noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i){auto x=a[i],y=b[i];
        if(x>='A'&&x<='Z')x=char(x+32);if(y>='A'&&y<='Z')y=char(y+32);if(x!=y)return false;
    }return true;
}
bool sourceDirectory(const std::string& actual,const char* expected)noexcept {
    auto n=actual.size();if(n&&actual[n-1]=='\\')--n;
    if(n!=std::strlen(expected))return false;
    for(std::size_t i=0;i<n;++i){auto c=actual[i];if(c>='A'&&c<='Z')c=char(c+32);if(c!=expected[i])return false;}return true;
}
std::string portraitName(std::uint32_t rawRacer60) {
    // Actual3ACDA0 is "%02d_fsm": signed32, minimum width2, zero padding.
    // Native bridge retains the original32 bits; no catalogue-positive gate
    // may run before the genuine name lookup/handle store/ref retain.
    const auto value=std::bit_cast<std::int32_t>(rawRacer60);
    return (value>=0&&value<10?std::string("0"):std::string{})+std::to_string(value)+"_fsm";
}
bool originalPortrait(std::uint32_t id)noexcept {
    // Exact OVERLAYS catalogue; no missing racer is replaced by an available image.
    return id>=1&&id<=26&&id!=5&&id!=6&&id!=13;
}
// Preserve the real resolver's PST/IPU/TGA/ATM priority. ATM bytes retain all
// authored frames for the existing decoder; no first-frame image substitute.
bool appendTexture(const std::shared_ptr<const Provider>& provider,const std::string& name,
    std::vector<OriginalStandardTextureSourceInput50LA>& textures,D& d) {
    for(const char* suffix:{".pst",".ipu",".tga",".atm"}){
        Provider::MemberView member;const auto lookup=provider->lookup132820(name+suffix,member);
        if(lookup==Provider::Lookup::unknown)return fail(d,S::source,0x132820);
        if(lookup==Provider::Lookup::absent)continue;
        if(std::strcmp(suffix,".ipu")==0||!member.member||!member.data||!member.size)
            return fail(d,S::texture,0x24DB60);
        textures.push_back({*member.member,{member.data,member.data+member.size}});return true;
    }
    return fail(d,S::source,0x24DC30);
}
bool appendAuthored(const std::shared_ptr<const Provider>& provider,const std::string& requested,
    const Provider::MemberView& psm,std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>>& materials,
    std::vector<OriginalStandardTextureSourceInput50LA>& textures,D& d) {
    if(!psm.member||!psm.data||!psm.size)return fail(d,S::source,0x29B628);
    auto source=std::make_shared<OriginalStandardMaterialSource50LA>();
    if(!source->prepare(*psm.member,{psm.data,psm.data+psm.size},d.document))return fail(d,S::document,0x29B628);
    const auto* general=source->section("General");std::int32_t passes=0;
    if(!general||!originalUiInteger50KG(*general,"TotalShaderPasses",0,passes,d.document,0x29B718)||passes<1||passes>4)
        return fail(d,S::document,0x29B718);
    const auto slash=requested.find_last_of('\\');
    const auto prefix=slash==std::string::npos?std::string{}:requested.substr(0,slash+1);
    const auto fallback=slash==std::string::npos?requested:requested.substr(slash+1);
    for(std::int32_t pass=1;pass<=passes;++pass){
        const auto* section=source->section("ShaderPass_"+std::to_string(pass));std::int32_t maps=1;
        if(!section||!originalUiInteger50KG(*section,"totalTextureMaps",1,maps,d.document,0x247214)||maps<0||maps>2)
            return fail(d,S::document,0x247214);
        for(std::int32_t map=0;map<maps;++map){
            const auto* field=section->find(map?"TextureMap_2":"TextureMap_1");
            const auto name=field&&!field->text.empty()?field->text:fallback;
            if(name.empty()||name.size()>255||same(name,"RenderTarget")||name.find_first_of("\\/.:\"")!=std::string::npos)
                return fail(d,S::name,0x2472A8);
            if(!appendTexture(provider,prefix+name,textures,d))return false;
        }
    }
    materials.push_back(std::move(source));return true;
}
}
OriginalResultsPrimitiveReferences50LA::~OriginalResultsPrimitiveReferences50LA(){
    for(const auto& reference:references_)if(reference.retained)std::terminate();
}
bool OriginalResultsPrimitiveReferences50LA::prepareCold(
    std::shared_ptr<OriginalStandardMaterialDomain50LA> domain,
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context,
    std::shared_ptr<const OriginalUiArchive50KG> overlays,std::size_t count,D& d)noexcept {
    d={};
    if(prepared_||failed_||closing_)return fail(d,S::phase,0x1BEEB0);
    if(!domain||!domain->prepared()||!context||!context->live()||domain->application_!=context->materialApplication()||
       !overlays||!overlays->loaded()||overlays->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)||
       !domain->archiveProvider3E1A70()||!domain->archiveProvider3E1A70()->sealed())return fail(d,S::source,0x1BEEB0);
    if(count>(std::numeric_limits<std::size_t>::max()-126)/7)return fail(d,S::overflow,0x1BF6D4);
    const auto refs=126+7*count;
    if(refs>references_.max_size())return fail(d,S::overflow,0x1BF6D4);
    try {references_.resize(refs);domain_=std::move(domain);context_=std::move(context);
        overlays_=std::move(overlays);racers_=count;prepared_=true;return true;
    }catch(...){failed_=true;return fail(d,S::allocation,0x1BEEB0);}
}
OriginalResultsPrimitiveReferences50LA::Source OriginalResultsPrimitiveReferences50LA::source(std::size_t i)const noexcept {
    using R=Role;Source out{};std::size_t cursor=0;
    const auto take=[&](R role,std::uint32_t pc,std::uint32_t offset,std::size_t ordinal,std::size_t batches,
        const char* name,const char* directory="font",bool missing=false)noexcept {
        const bool selected=i>=cursor&&i-cursor<batches;
        if(selected)out={role,pc,offset,ordinal,directory,name,missing};cursor+=batches;return selected;
    };
    if(take(R::baseResults1,0x1B8840,0xA0,0,1,"results1","ui\\textures",true)||
       take(R::baseResults2,0x1B898C,0x100,0,1,"results2","ui\\textures",true)||
       take(R::baseResults2,0x1B8AD8,0x40,0,1,"results2","ui\\textures",true)||
       take(R::letterBox,0x1B8E10,0x38,0,1,"LetterBoxPause","ui\\textures")||
       take(R::letterBox,0x1B8E10,0x3C,0,1,"LetterBoxPause","ui\\textures")||
       take(R::baseTextPause,0x1B8C08,0x180,0,2,"denmarkbold16pause"))return out;
    constexpr R iconRoles[]{R::iconSquare,R::iconTriangle,R::iconCross,R::iconCircle};
    constexpr std::uint32_t iconPCs[]{0x1BEF38,0x1BEFE8,0x1BF0A0,0x1BF158},iconOffsets[]{0x2D0,0x270,0x210,0x330};
    constexpr const char* icons[]{"icon_sqr","icon_tri","icon_x","icon_cir"};
    for(unsigned n=0;n<4;++n)if(take(iconRoles[n],iconPCs[n],iconOffsets[n],0,1,icons[n],"ui\\textures",true))return out;
    for(unsigned n=0;n<3;++n)if(take(R::denmark16,0x1BF23C,0x490+0x70*n,0,2,"denmarkbold16ingame","font",true))return out;
    if(take(R::denmark16,0x1BF440,0x3A0,0,2,"denmarkbold16ingame","font",true)||
       take(R::denmark16,0x1BF5A0,0x410,0,2,"denmarkbold16ingame","font",true))return out;
    const auto dynamicText=6*racers_;
    if(i>=cursor&&i-cursor<dynamicText)return {R::denmark16,0x1BF758,0x5E0,(i-cursor)/2,"font","denmarkbold16ingame",true};
    cursor+=dynamicText;
    if(i>=cursor&&i-cursor<racers_)return {R::portrait,0x1BF804,0x480,i-cursor,"ui\\textures",nullptr,true};
    cursor+=racers_;
    constexpr R medals[]{R::medalGold,R::medalSilver,R::medalBronze};
    constexpr const char* medalNames[]{"medasmgld","medasmslv","medasmbrz"};
    for(unsigned n=0;n<3;++n)if(take(medals[n],0x1BF848+0x18*n,0x5F0+0x60*n,0,1,medalNames[n],"ui\\textures"))return out;
    constexpr std::uint32_t textOffsets[]{0x710,0x780,0x7F0,0x940,0x860,0x9B0,0x8D0,0xA20};
    constexpr std::uint32_t textPCs[]{0x1BF898,0x1BF9B0,0x1BFAC8,0x1BFB88,0x1BFC48,0x1BFD08,0x1BFDC8,0x1BFE88};
    for(unsigned n=0;n<8;++n)if(take(R::denmark16,textPCs[n],textOffsets[n],0,n==6?2:1,"denmarkbold16ingame","font",true))return out;
    if(take(R::denmark16,0x1BFF68,0xA90,0,1,"denmarkbold16ingame","font",true)||
       take(R::denmark16,0x1C00C4,0xA98,0,1,"denmarkbold16ingame","font",true))return out;
    for(unsigned n=0;n<42;++n)if(take(R::denmark16,n<39?0x1C02A8:0x1C027C,0xA94,n,1,"denmarkbold16ingame","font",true))return out;
    if(take(R::denmark16,0x1C0330,0x1150,0,2,"denmarkbold16ingame","font",true)||
       take(R::denmark16,0x1C0444,0x11C0,0,2,"denmarkbold16ingame","font",true))return out;
    constexpr std::uint32_t euroPCs[]{0x1C052C,0x1C05F0,0x1C06B0,0x1C0770,0x1C0830,0x1C08F0,0x1C09B0,0x1C0A70};
    for(unsigned n=0;n<8;++n)if(take(R::eurostile13,euroPCs[n],0x1230+0x70*n,0,2,"eurostileboldbold13ingame"))return out;
    constexpr std::uint32_t fourPCs[]{0x1C0B54,0x1C0C18,0x1C0CD8,0x1C0D98};
    for(unsigned n=0;n<4;++n)if(take(R::denmark16,fourPCs[n],0x15B0+0x70*n,0,2,"denmarkbold16ingame","font",true))return out;
    for(unsigned n=0;n<3;++n)if(take(R::denmark16,0x1C0E58+0xC0*n,0x1770+0x70*n,0,2,"denmarkbold16ingame","font",true))return out;
    constexpr std::uint32_t triplePCs[]{0x1C109C,0x1C1164,0x1C122C};
    for(unsigned n=0;n<12;++n)if(take(R::denmark16,triplePCs[n%3],0xAF0+0x70*n,0,1,"denmarkbold16ingame","font",true))return out;
    for(unsigned n=0;n<3;++n)if(take(R::separator,0x1C140C+0x18*n,0x1030+0x60*n,0,1,"MenuSeparator","ui\\textures"))return out;
    return {};
}
bool OriginalResultsScreenFallback50LA::current(const std::string& materialName,
    const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
    const std::shared_ptr<OriginalStandardTextureTable50LA>& textures,
    const std::shared_ptr<const Provider>& provider)const noexcept {
    if(row_<0||!caller_||!offset_||!context_||context!=context_||!context_->live()||
       !textures_||textures!=textures_||!provider_||provider!=provider_||!provider_->sealed()||
       provider_->revision()!=revision_||!overlays_||!overlays_->loaded()||
       overlays_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)||
       !selectedArchive_.identity()||selectedArchive_.providerIdentity()!=provider_.get()||pstBytes_.empty())return false;
    try {
        if(!same(materialName,name_+".psm")||overlays_->find(name_+".psm").identity)return false;
        const bool square=name_=="ui\\textures\\icon_sqr",circle=name_=="ui\\textures\\icon_cir";
        const bool triangle=name_=="ui\\textures\\icon_tri",cross=name_=="ui\\textures\\icon_x";
        const bool font=name_=="font\\denmarkbold16ingame";
        const bool portrait=caller_==0x1BF804&&offset_==0x480&&originalPortrait(racer60_)&&
            name_=="ui\\textures\\"+portraitName(racer60_);
        if(!((square&&caller_==0x1BEF38&&offset_==0x2D0)||
             (triangle&&caller_==0x1BEFE8&&offset_==0x270)||
             (cross&&caller_==0x1BF0A0&&offset_==0x210)||
             (circle&&caller_==0x1BF158&&offset_==0x330)||font||portrait))return false;
        const auto* selected=selectedArchive_.identity();
        const bool sourceAllowed=selected==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)||
            ((cross||triangle)&&selected==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::controllerPopup))||
            ((square||circle)&&selected==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garage));
        if(!sourceAllowed)return false;
        const auto original=overlays_->find(name_+".pst");
        if(!original.identity||!original.data||original.size!=original.identity->bytes||
           original.size!=pstBytes_.size()||std::memcmp(original.data,pstBytes_.data(),original.size))return false;
        Provider::MemberView psm,pst;
        if(provider_->lookup132820(name_+".psm",psm)!=Provider::Lookup::absent||
           provider_->lookup132820(name_+".pst",pst)!=Provider::Lookup::present||
           !pst.member||*pst.member!=pstMember_||!pst.data||pst.size!=pstBytes_.size()||
           std::memcmp(pst.data,pstBytes_.data(),pst.size)||pst.mount!=mount_||pst.ordinal!=member_||
           pst.revision!=revision_||pst.bankGeneration!=bankGeneration_||psm.revision!=pst.revision)return false;
        Provider::MountView mount;
        return provider_->mount(mount_,mount)&&mount.archiveSha256&&
            *mount.archiveSha256==selected->sha256&&mount.archiveBytes==selected->bytes;
    }catch(...){return false;}
}
bool OriginalResultsPrimitiveReferences50LA::acquireNext(Role role,std::uint32_t caller,
    std::uint32_t offset,std::size_t ordinal,std::uint32_t racer60,
    const std::string& directory,D& d)noexcept {
    d={};
    const auto stop=[&](S status,std::uint32_t pc)noexcept {
        failed_=true;d.status=status;d.sourcePC=pc;
        if(next_<references_.size()){const auto& r=references_[next_];d.row=r.material00;
            d.storedSlots=r.started?1:0;d.retainedSlots=r.retained?1:0;
            d.sourceEffectsCommitted=d.sourceEffectsCommitted||r.started||r.retained;
        }return false;
    };
    if(!prepared_||failed_||closing_||next_>=references_.size())return stop(S::phase,0x299BE0);
    const auto expected=source(next_);
    if(!expected.directory||expected.role!=role||expected.caller!=caller||expected.offset!=offset||expected.ordinal!=ordinal||
       (role!=Role::portrait&&racer60!=0)||!sourceDirectory(directory,expected.directory))
        return stop(S::input,caller);
    if(!domain_||!domain_->prepared()||!context_||!context_->live()||
       domain_->application_!=context_->materialApplication()||
       domain_->materials382B38()->populationTerminal()||domain_->materials382B38()->demandTerminal()||
       !domain_->textures383400()||domain_->textures383400()->populationTerminal())return stop(S::phase,0x14D370);
    if(role==Role::portrait&&dlc50DL::addonNumberRange(racer60)&&dlc50DL::addonPod(racer60)){
        // PORT EXTENSION, NOT ORIGINAL: see Reference::absent. No standard-table
        // name lookup, store or retain happens; the source order of later
        // occurrences is kept. The pod's own icon loads into this owner's own
        // add-on bank (addonIcons_); a refusal leaves the portrait undrawn.
        auto& reference=references_[next_];reference=Reference{};reference.absent=true;
        const auto* pod=dlc50DL::addonPod(racer60);std::size_t index=noAddonIcon;std::string why;
        if(loadAddonIcon50DL(racer60,index,why))reference.addonIcon=index;
        else std::fprintf(stderr,"dlc_results_portrait racer=%u key=%s icon=%s status=absent reason=\"%s\"\n",
            racer60,pod->key.c_str(),pod->ui.resultsIcon.c_str(),why.c_str());
        ++next_;return true;
    }
    try {
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return stop(S::source,0x132820);
        const auto revision=provider->revision();const auto table=domain_->materials382B38();
        if(!table||!table->screenApplicationMatches(*context_->materialApplication()))return stop(S::input,0x14D590);
        const auto name=role==Role::portrait?portraitName(racer60):std::string(expected.name);
        std::string requested=directory;if(requested.back()!='\\')requested+='\\';requested+=name;
        auto& reference=references_[next_];reference.started=true;
        std::int32_t row=-1;const bool found=table->find298C80(requested,row,d);
        reference.material00=row;d.row=row;d.storedSlots=1;d.sourceEffectsCommitted=true;
        if(!found)return stop(d.status,d.sourcePC);
        //299BE0 stores the real numeric handle before14D590 retains it. Keep
        //that prefix on any later source/corpus/population failure.
        if(!table->retainScreen14D590(row,d))return stop(d.status,d.sourcePC);
        reference.retained=true;d.retainedSlots=1;
        OriginalStandardMaterialRowView50LA view;
        if(!table->row(row,view))return stop(S::range,0x298E60);
        if(view.object04){
            if(!view.object04->constructed29B550()||view.object04->textures383400()!=domain_->textures383400().get())
                return stop(S::phase,0x298E60);
            // Loaded rows do not replay29B638 or acquire new family history.
            ++next_;return true;
        }
        Provider::MemberView psm;const auto lookup=provider->lookup132820(requested+".psm",psm);
        std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> materials;
        std::vector<OriginalStandardTextureSourceInput50LA> textures;
        if(lookup==Provider::Lookup::present){
            if(!appendAuthored(provider,requested,psm,materials,textures,d))return stop(d.status,d.sourcePC);
        }else if(lookup==Provider::Lookup::absent&&expected.missingPsm){
            // The actual requested row is already stored/retained. Catalogue
            // absence is now a reached unsupported resource cut, not authority
            // to skip the original numeric acquisition or substitute an ID.
            if(role==Role::portrait&&!originalPortrait(racer60))return stop(S::source,0x29B638);
            const auto original=overlays_->find(requested+".pst");Provider::MemberView pst;
            if(overlays_->find(requested+".psm").identity||!original.identity||!original.data||original.size!=original.identity->bytes||
               provider->lookup132820(requested+".pst",pst)!=Provider::Lookup::present||!pst.member||!pst.data||
               pst.size!=original.size||std::memcmp(pst.data,original.data,pst.size))return stop(S::source,0x29B638);
            if(role==Role::baseResults1||role==Role::baseResults2){
                // SAME shared-base1B8810 occurrence, not a Pause100 owner or
                // derived-Pause certificate. The numeric retain already ran.
                if(!table->bindResultsBaseBackdropAfterRetain(row,overlays_,domain_->textures383400(),d))
                    return stop(d.status,d.sourcePC);
            }else{
                std::shared_ptr<OriginalResultsScreenFallback50LA> proof(new OriginalResultsScreenFallback50LA);
                proof->row_=row;proof->caller_=caller;proof->offset_=offset;proof->ordinal_=ordinal;
                proof->racer60_=racer60;proof->occurrence_=next_;proof->name_=requested;
                for(char& c:proof->name_)if(c>='A'&&c<='Z')c=char(c+32);
                proof->pstMember_=*pst.member;proof->pstBytes_.assign(pst.data,pst.data+pst.size);
                proof->overlays_=overlays_;proof->provider_=provider;proof->context_=context_;proof->textures_=domain_->textures383400();
                proof->mount_=pst.mount;proof->member_=pst.ordinal;proof->revision_=pst.revision;proof->bankGeneration_=pst.bankGeneration;
                if(!provider->encodedUiInput(pst.mount,proof->selectedArchive_))return stop(S::source,0x29B638);
                if(!table->bindResultsPendingAfterRetain(row,std::move(proof),d))return stop(d.status,d.sourcePC);
            }
            textures.push_back({*pst.member,{pst.data,pst.data+pst.size}});
        }else return stop(S::source,0x29B638);
        if(provider->revision()!=revision||!provider->sealed())return stop(S::phase,0x132820);
        D append;
        if(!domain_->appendOriginalSources(materials,textures,append)){
            d=append;return stop(d.status,d.sourcePC);
        }
        d.row=row;d.storedSlots=1;d.retainedSlots=1;d.sourceEffectsCommitted=true;
        ++next_;return true;
    }catch(...){return stop(S::allocation,0x299BE0);}
}
bool OriginalResultsPrimitiveReferences50LA::loadAddonIcon50DL(std::uint32_t racer60,
    std::size_t& index,std::string& why)noexcept {
    // PORT EXTENSION, NOT ORIGINAL: see AddonIcon in the header.
    index=noAddonIcon;
    try {
        const auto* pod=dlc50DL::addonPod(racer60);
        if(!pod){why="registry row absent";return false;}
        for(std::size_t i=0;i<addonIcons_.size();++i)if(addonIcons_[i].racer60==racer60){index=i;return true;}
        if(addonIcons_.size()>=racers_){why="outside the Results add-on icon capacity (one per racer)";return false;}
        const auto& icon=pod->ui.resultsIcon;
        if(icon.size()<5||!same(icon.substr(icon.size()-4),".pst")){why="results_icon is not a .pst member";return false;}
        std::vector<std::uint8_t> bytes;
        {
            const auto path=dlc50DL::host(std::filesystem::path(pod->ui.archive));
            std::error_code e;const auto size=std::filesystem::file_size(path,e);
            if(e||!size||size>dlc50DL::RegistryLimits::uiArchiveBytes){why="menu archive outside the add-on UI archive capacity";return false;}
            std::ifstream in(path,std::ios::binary);
            if(!in){why="menu archive cannot be opened";return false;}
            bytes.resize(static_cast<std::size_t>(size));
            if(!in.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()))){why="menu archive cannot be read";return false;}
        }
        auto archive=std::make_shared<OriginalUiArchive50KG>();OriginalUiArchiveDiagnostic50KG ad;
        if(!archive->loadAddon(pod->ui.archive,std::move(bytes),dlc50DL::RegistryLimits::uiArchiveBytes,
            dlc50DL::RegistryLimits::uiDecodedBytes,dlc50DL::RegistryLimits::uiEntries,ad)||!archive->addon()){
            why="menu archive does not decode within its capacity (status "+std::to_string(unsigned(ad.status))+")";return false;}
        const auto member=archive->find(icon);
        if(!member.identity||!member.data||!member.size||member.size!=member.identity->bytes||
           originalUiAddonMemberArchive50DL(member.identity)!=archive->identity()){why="results_icon member absent from the menu archive";return false;}
        // The original "%02d_fsm" portraits have no PSM (2475C0 fallback); an
        // add-on icon with an authored PSM has no matching draw policy here.
        if(archive->find(icon.substr(0,icon.size()-4)+".psm").identity){why="results_icon has an authored PSM (not supported)";return false;}
        auto images=std::make_shared<OriginalUiScreenAssets50KG>();OriginalUiScreenAssetDiagnostic50KG decoded;
        if(!images->load({OriginalUiImageSource50KG{0,member,0,member.size,{}}},decoded)){
            why="results_icon does not decode";return false;}
        const auto* image=images->decodedTexture(0);
        if(!image||image->mips.empty()||!image->mips.back().width||!image->mips.back().height){why="results_icon decoded image empty";return false;}
        AddonIcon next;next.racer60=racer60;next.key=pod->key;next.member=icon;next.archive=std::move(archive);
        next.images=std::move(images);next.identity=member.identity;next.bytes=member.size;
        addonIcons_.push_back(std::move(next));index=addonIcons_.size()-1;return true;
    }catch(const std::exception& x){try{why=x.what();}catch(...){}}
     catch(...){}
    try{if(why.empty())why="results_icon load failed";}catch(...){}
    return false;
}
bool OriginalResultsPrimitiveReferences50LA::prepareAddonPortrait50DL(std::size_t reference,
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,NativePrimitive& output,D& d)noexcept {
    // PORT EXTENSION, NOT ORIGINAL: see AddonIcon / PortraitPolicy in the header.
    d={};
    if(nativeImageThread_!=std::this_thread::get_id())return fail(d,S::phase,0x246760);
    if(!prepared_||failed_||closing_||next_!=references_.size()||reference>=references_.size()||
       source(reference).role!=Role::portrait||!domain_||!context_||!context_->live())return fail(d,S::reference,0x246760);
    const auto* icon=addonIcon(&references_[reference]);
    if(!icon){d.field="add-on Results icon not loaded";return fail(d,S::reference,0x246760);}
    const auto& policy=portraitPolicy_;
    if(!policy.known||!policy.final||policy.leaf<0||policy.reference>=references_.size()||
       !references_[policy.reference].retained||references_[policy.reference].material00!=policy.row){
        d.field="no stock Results portrait policy in this capture";return fail(d,S::reference,0x2475C0);}
    const auto table=domain_->materials382B38();
    if(!table||!table->pinNativeCollector(receipt))return fail(d,S::collector,0x249610);
    const auto unpin=[&](OriginalStandardMaterialTable50LA*)noexcept{table->unpinNativeCollector();};
    std::unique_ptr<OriginalStandardMaterialTable50LA,decltype(unpin)> pin(table.get(),unpin);
    try {
        // The borrowed stock portrait is still the same live FINAL leaf.
        OriginalStandardMaterialCollectorEntryView50LA origin,final;
        if(!table->coldCollectorPass(receipt,policy.row,0,origin)||origin.finalLeaf!=policy.leaf||
           !table->coldCollectorEntry(receipt,static_cast<std::size_t>(origin.finalLeaf),final)||
           final.redirect18!=-1||final.material!=policy.final||final.pass!=0||!final.material->constructed29B550()){
            d.field="stock Results portrait policy changed";return fail(d,S::collector,0x24B1A0);}
        const auto* image=icon->images->decodedTexture(0);
        if(!image)return fail(d,S::texture,0x24E880);
        NativePrimitive next;OriginalUiMaterialDiagnostic50KG numeric;
        if(!prepareOriginalResultsAddonPortraitMaterial50DL(policy.pass,*policy.final,*image,next.textured,numeric)){
            d.document=numeric.document;d.field="add-on Results icon policy";
            return fail(d,S::unsupportedPass,numeric.sourcePC?numeric.sourcePC:0x2475C0);}
        if(next.textured.blend==OriginalUiMaterialBlend50KG::destinationAlphaAdditive)return fail(d,S::unsupportedPass,0x249610);
        next.image.images=icon->images;next.image.member=icon->identity;next.image.image=0;
        next.image.offset=0;next.image.bytes=icon->bytes;next.hasTexture=true;next.finalLeaf=policy.leaf;
        output=std::move(next);return true;
    }catch(...){return fail(d,S::allocation,0x2475C0);}
}
void OriginalResultsPrimitiveReferences50LA::release299AC0(std::size_t i)noexcept {
    if(i>=references_.size())std::terminate();closing_=true;
    // Cache keys borrow canonical objects only until the first real release.
    // Copied immutable pixels may survive in a native renderer's own custody.
    nativeImages_.clear();
    auto& reference=references_[i];if(!reference.retained)return;
    if(!domain_||!domain_->materials382B38()->release14D500(reference.material00))std::terminate();
    reference.retained=false;
}
bool OriginalResultsPrimitiveReferences50LA::decodeCurrentNativeImage50LA(std::size_t reference,
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,NativeImage& output,D& d)noexcept {
    d={};
    if(nativeImageThread_!=std::this_thread::get_id())return fail(d,S::phase,0x246760);
    if(!prepared_||failed_||closing_||next_!=references_.size()||reference>=references_.size()||
       !references_[reference].retained||!domain_||!context_||!context_->live())return fail(d,S::reference,0x246760);
    const auto table=domain_->materials382B38();
    if(!table||!table->pinNativeCollector(receipt))return fail(d,S::collector,0x249610);
    const auto unpin=[&](OriginalStandardMaterialTable50LA*)noexcept{table->unpinNativeCollector();};
    std::unique_ptr<OriginalStandardMaterialTable50LA,decltype(unpin)> pin(table.get(),unpin);
    try {
        OriginalStandardMaterialMapBindingView50LA map;
        const auto currentMap=[&](OriginalStandardMaterialMapBindingView50LA& out){
            return !closing_&&context_->live()&&references_[reference].retained&&
                table->prepareColdScreenPrototypeMap249610(receipt,references_[reference].material00,0,0,out,d)&&
                out.preparation==OriginalStandardMaterialMapPreparation50LA::ready&&out.texture&&
                out.representative.material&&out.representative.material->textures383400()==domain_->textures383400().get();
        };
        if(!currentMap(map))return fail(d,S::texture,d.sourcePC?d.sourcePC:0x249610);
        OriginalStandardTextureRowView50LA row;
        if(!domain_->textures383400()->row(map.textureToken,row)||!row.references08||*row.references08<=0||!row.object04)
            return fail(d,S::texture,0x298E60);
        const auto* root=row.object04;
        if(root->construction_!=context_||root->resolver_!=domain_->resolver3E1A70()){
            d.field=root->construction_!=context_?"Results primitive texture: other construction context":"Results primitive texture: other resolver";
            return fail(d,S::texture,0x24E880);}
        std::shared_ptr<const OriginalUiArchive50KG> archive;OriginalUiMemberView50KG member;
        std::size_t offset=0,extent=0;
        bool resourceSource=false;
        if(!root->borrowResultsUiPstImage50LA(*map.texture,archive,member,offset,extent)){
            // This standalone PST's row was first created through a non-UI
            // registration (e.g. the Application font owner), so it carries no
            // UI first source. Use the SAME member of the overlays archive when
            // its bytes are identical (the Results material proof uses that
            // member too); otherwise decode the resource's own original bytes.
            const auto* name=root->sourceMember();const auto* bytes=root->originalBytes();
            std::size_t begin=0,length=0;
            if(map.texture!=root||root->aggregate_||root->atmChild_||root->animated102070()||
               root->sourceFormat_!=OriginalStandardTextureSourceFormat50LA::pstIndexed||
               !name||!bytes||bytes->empty()||!root->pstExtent24E880(begin,length)||
               !length||begin>bytes->size()||length>bytes->size()-begin||
               bytes->size()>std::numeric_limits<std::uint32_t>::max()){
                d.field="Results primitive texture: not a borrowed UI PST image";return fail(d,S::texture,0x24E880);}
            const auto overlaid=overlays_&&overlays_->loaded()?overlays_->find(*name):OriginalUiMemberView50KG{};
            if(overlaid.identity&&overlaid.data&&overlaid.size==bytes->size()&&
               !std::memcmp(overlaid.data,bytes->data(),bytes->size())){
                archive=overlays_;member=overlaid;
            }else{
                const OriginalUiMemberIdentity50KG* identityOf=nullptr;
                for(const auto& held:nativeResourceMembers_)
                    if(held->name==name->c_str()&&held->bytes==bytes->size()){identityOf=held.get();break;}
                if(!identityOf){
                    nativeResourceMembers_.push_back(std::make_unique<OriginalUiMemberIdentity50KG>(
                        OriginalUiMemberIdentity50KG{name->c_str(),0u,static_cast<std::uint32_t>(bytes->size()),nullptr}));
                    identityOf=nativeResourceMembers_.back().get();
                }
                member={identityOf,bytes->data(),bytes->size()};
            }
            offset=begin;extent=length;resourceSource=true;
        }
        const bool animated=root->animated102070();
        const auto* count=animated?root->frameCount20():nullptr;
        if(animated&&(!count||*count<=0))return fail(d,S::texture,0x102050);
        const auto imageCount=animated?static_cast<std::size_t>(*count):std::size_t(1);
        NativeImageCache identity;identity.tableObject=root;identity.archive=archive;identity.member=member.identity;
        identity.children.reserve(imageCount);identity.offsets.reserve(imageCount);identity.extents.reserve(imageCount);
        std::uint32_t selectedIndex=0;bool selectedFound=false;
        for(std::size_t i=0;i<imageCount;++i){
            const auto* child=animated?root->frame102050(static_cast<std::int32_t>(i)):root;
            std::shared_ptr<const OriginalUiArchive50KG> childArchive;OriginalUiMemberView50KG childMember;
            std::size_t begin=0,length=0;
            if(resourceSource&&child==root){childArchive=archive;childMember=member;begin=offset;length=extent;}
            else if(!child||!root->borrowResultsUiPstImage50LA(*child,childArchive,childMember,begin,length)||
               childArchive!=archive||childArchive.owner_before(archive)||archive.owner_before(childArchive)||
               childMember.identity!=member.identity||childMember.data!=member.data||childMember.size!=member.size)
                return fail(d,S::texture,0x102050);
            identity.children.push_back(child);identity.offsets.push_back(begin);identity.extents.push_back(length);
            if(child==map.texture){selectedFound=true;selectedIndex=static_cast<std::uint32_t>(i);}
        }
        if(!selectedFound)return fail(d,S::texture,0x102050);
        const auto hit=std::find_if(nativeImages_.begin(),nativeImages_.end(),[&](const auto& cache){
            return cache.tableObject==root&&cache.archive==archive&&!cache.archive.owner_before(archive)&&
                !archive.owner_before(cache.archive)&&cache.member==member.identity&&cache.children==identity.children&&
                cache.offsets==identity.offsets&&cache.extents==identity.extents&&cache.images&&cache.images->loaded();
        });
        std::shared_ptr<const OriginalUiScreenAssets50KG> bank;
        if(hit!=nativeImages_.end())bank=hit->images;
        else {
            std::vector<OriginalUiImageSource50KG> sources;sources.reserve(imageCount);
            for(std::size_t i=0;i<imageCount;++i)sources.push_back({i,member,identity.offsets[i],identity.extents[i],{}});
            auto fresh=std::make_shared<OriginalUiScreenAssets50KG>();OriginalUiScreenAssetDiagnostic50KG decoded;
            if(!fresh->load(sources,decoded)){d.field="Results primitive texture: decode";return fail(d,S::texture,0x24E880);}
            // All actual ATM frames are retained; frame selection still comes
            // from the real current prototype, never native time or frame0.
            for(std::size_t i=0;i<imageCount;++i){
                const auto* image=fresh->decodedTexture(static_cast<std::uint32_t>(i));const auto* child=identity.children[i];
                if(!image||image->mips.empty()||!child->sourceWidth14()||!child->sourceHeight18()||
                   image->mips.back().width!=static_cast<std::uint32_t>(*child->sourceWidth14())||
                   image->mips.back().height!=static_cast<std::uint32_t>(*child->sourceHeight18())){
                    d.field="Results primitive texture: decoded size";return fail(d,S::texture,0x24E880);}
            }
            identity.images=fresh;bank=fresh;
            OriginalStandardMaterialMapBindingView50LA after;
            if(!currentMap(after)||after.texture!=map.texture||after.textureToken!=map.textureToken||
               after.origin.material!=map.origin.material||after.representative.material!=map.representative.material||
               after.representative.pass!=map.representative.pass)return fail(d,S::phase,0x249610);
            nativeImages_.push_back(std::move(identity));
        }
        OriginalStandardMaterialMapBindingView50LA finalMap;
        if(!currentMap(finalMap)||finalMap.texture!=map.texture||finalMap.textureToken!=map.textureToken||
           finalMap.origin.material!=map.origin.material||finalMap.representative.material!=map.representative.material||
           finalMap.representative.pass!=map.representative.pass)return fail(d,S::phase,0x249610);
        NativeImage next;next.images=std::move(bank);next.tableObject=root;next.selected=map.texture;
        next.member=member.identity;next.image=selectedIndex;next.offset=offset;next.bytes=extent;
        output=std::move(next);return true;
    }catch(...){return fail(d,S::allocation,0x24E880);}
}

bool OriginalResultsPrimitiveReferences50LA::prepareNativePrimitive50LA(std::size_t reference,
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,NativePrimitive& output,D& d)noexcept {
    d={};
    if(nativeImageThread_!=std::this_thread::get_id())return fail(d,S::phase,0x246760);
    if(!prepared_||failed_||closing_||next_!=references_.size()||reference>=references_.size()||
       !references_[reference].retained||!domain_||!context_||!context_->live()||!overlays_)
        return fail(d,S::reference,0x246760);
    const auto role=source(reference).role;
    const bool authored=role==Role::letterBox||role==Role::baseTextPause||role==Role::eurostile13||
        role==Role::medalGold||role==Role::medalSilver||role==Role::medalBronze||role==Role::separator;
    const bool fallback=role==Role::baseResults1||role==Role::baseResults2||role==Role::iconSquare||
        role==Role::iconTriangle||role==Role::iconCross||role==Role::iconCircle||role==Role::denmark16||role==Role::portrait;
    if(!authored&&!fallback)return fail(d,S::unsupportedPass,0x247110);
    const auto table=domain_->materials382B38();
    if(!table||!table->pinNativeCollector(receipt))return fail(d,S::collector,0x249610);
    const auto unpin=[&](OriginalStandardMaterialTable50LA*)noexcept{table->unpinNativeCollector();};
    std::unique_ptr<OriginalStandardMaterialTable50LA,decltype(unpin)> pin(table.get(),unpin);
    const auto row=references_[reference].material00;
    const auto sameContext=[](const OriginalStandardMaterialStateContext50LA& a,
        const OriginalStandardMaterialStateContext50LA& b)noexcept {
        const auto word=[](const auto& x,const auto& y)noexcept {
            return x.value==y.value&&x.origin==y.origin&&x.templateMember==y.templateMember&&
                x.byteOffset==y.byteOffset&&x.sourcePC==y.sourcePC;
        };
        for(std::size_t i=0;i<a.fields.size();++i)if(!word(a.fields[i],b.fields[i]))return false;
        for(std::size_t i=0;i<a.sampler.size();++i)if(!word(a.sampler[i],b.sampler[i]))return false;
        return a.videoDepthBase2E0==b.videoDepthBase2E0&&a.videoDepthFormat5C==b.videoDepthFormat5C&&
            a.runtime435670==b.runtime435670;
    };
    // Exactly this originating primitive, after its caller's geometry. No
    // FINAL representative is substituted for a unique origin controller.
    if(!table->serviceRetainedScreenPrototype24A740(receipt,row,0,d))return false;
    const bool serviced=d.sourceEffectsCommitted;
    const auto stop=[&](S status,std::uint32_t pc){d.sourceEffectsCommitted|=serviced;return fail(d,status,pc);};
    try {
        OriginalStandardMaterialCollectorEntryView50LA origin,final;
        if(!table->coldCollectorPass(receipt,row,0,origin)||origin.finalLeaf<0||
           !table->coldCollectorEntry(receipt,static_cast<std::size_t>(origin.finalLeaf),final)||
           final.redirect18!=-1||!final.material||final.pass!=0)return stop(S::collector,0x24B1A0);
        OriginalUiMaterial50KG parsed;D actual;
        if(authored&&!final.material->borrowResultsAuthored247110(*overlays_,parsed,actual)){
            d=actual;return stop(d.status,d.sourcePC);
        }
        OriginalStandardMaterialPassView50LA pass;
        if(!final.material->pass4C(0,pass)||!pass.state30||pass.state30->contextCount()!=1||!pass.mapCount20)
            return stop(S::unsupportedPass,0x249BA0);
        NativePrimitive next;
        OriginalStandardMaterialStateContext50LA state=*pass.state30;
        OriginalStandardMaterialMapBindingView50LA map;
        if(*pass.mapCount20){
            if(!table->prepareColdScreenPrototypeMap249610(receipt,row,0,0,map,d)||
               map.representative.material!=final.material||map.representative.pass!=final.pass||
               map.preparation!=OriginalStandardMaterialMapPreparation50LA::ready||!map.texture)
                return stop(S::texture,d.sourcePC?d.sourcePC:0x249610);
            if(fallback&&!final.material->borrowResultsScreenFallback2475C0(*map.texture,parsed,actual)){
                d=actual;return stop(d.status,d.sourcePC);
            }
            D image;
            if(!decodeCurrentNativeImage50LA(reference,receipt,next.image,image)){
                d=image;return stop(d.status,d.sourcePC);
            }
            if(next.image.selected!=map.texture)return stop(S::texture,0x249610);
            OriginalStandardMaterialMapBindingView50LA afterDecode;
            if(!table->prepareColdScreenPrototypeMap249610(receipt,row,0,0,afterDecode,d)||
               afterDecode.texture!=map.texture||afterDecode.textureToken!=map.textureToken||
               afterDecode.representative.material!=final.material||afterDecode.representative.pass!=final.pass||
               !sameContext(afterDecode.contextAfter249610,map.contextAfter249610))
                return stop(S::phase,0x249610);
            state=afterDecode.contextAfter249610;next.hasTexture=true;
        }
        if((fallback&&!next.hasTexture)||parsed.passes.size()!=1)return stop(S::unsupportedPass,0x2475C0);
        auto effective=parsed.passes[0];
        // Preserve the actual authored or missing-PSM first construction;
        // project only CURRENT FINAL fields and reached249610 TEXA.
        for(std::size_t i=0;i<effective.gs.fields.size();++i){const auto& v=state.fields[i];
            effective.gs.fields[i]={v.value,v.templateMember,v.byteOffset,v.sourcePC};}
        effective.lodCalculationMethod=state.sampler[0].value;effective.maximumMipLevel=state.sampler[1].value;
        effective.magFilter=state.sampler[2].value;effective.minFilter=state.sampler[3].value;
        effective.lodL=state.sampler[4].value;effective.lodK=state.sampler[5].value;
        OriginalUiMaterialDiagnostic50KG numeric;
        if(next.hasTexture){
            const auto* image=next.image.images?next.image.images->decodedTexture(next.image.image):nullptr;
            if(!image)return stop(S::texture,0x24E880);
            if(fallback){
                // Only the actual FINAL object can enter its private native
                // fallback numeric adapter. Public parsed recipes stay closed.
                OriginalUiMaterial50KG observed;D lowered;
                if(!final.material->prepareResultsScreenFallback2475C0(*table,receipt,map,
                    *next.image.images,next.image.image,observed,next.textured,lowered)){
                    d=lowered;return stop(d.status,d.sourcePC);
                }
            }else if(!prepareOriginalUiNativeMaterial50KG(effective,*image,next.textured,numeric)){
                d.document=numeric.document;return stop(S::unsupportedPass,numeric.sourcePC?numeric.sourcePC:0x249610);
            }
            if(next.textured.blend==OriginalUiMaterialBlend50KG::destinationAlphaAdditive)
                return stop(S::unsupportedPass,0x249610); // No inherited gameplay alpha claim.
        }else if(!prepareOriginalUiNativeUntexturedMaterial50KG(effective,next.untextured,numeric)){
            d.document=numeric.document;return stop(S::unsupportedPass,numeric.sourcePC?numeric.sourcePC:0x249BA0);
        }
        OriginalStandardMaterialCollectorEntryView50LA now;
        if(!context_->live()||closing_||!table->coldCollectorPass(receipt,row,0,now)||
           now.material!=origin.material||now.registration!=origin.registration||now.finalLeaf!=origin.finalLeaf)
            return stop(S::phase,0x24B1A0);
        if(next.hasTexture){
            OriginalStandardMaterialMapBindingView50LA current;
            if(!table->prepareColdScreenPrototypeMap249610(receipt,row,0,0,current,d)||
               current.texture!=next.image.selected||current.textureToken!=map.textureToken||
               current.representative.material!=final.material||current.representative.pass!=final.pass||
               !sameContext(current.contextAfter249610,state))return stop(S::phase,0x249610);
        }else {
            OriginalStandardMaterialPassView50LA current;
            if(!final.material->pass4C(0,current)||!current.mapCount20||*current.mapCount20||
               !current.state30||current.state30->contextCount()!=1||!sameContext(*current.state30,state))
                return stop(S::phase,0x249BA0);
        }
        next.finalLeaf=origin.finalLeaf; //Rechecked against the live registry just above.
        if(role==Role::portrait&&fallback&&next.hasTexture&&!portraitPolicy_.known&&!addonIcons_.empty()){
            // PORT EXTENSION, NOT ORIGINAL: kept for an add-on icon of this
            // same capture (PortraitPolicy). No stock output changes; a failed
            // copy only leaves the add-on icon undrawn.
            try{portraitPolicy_.pass=effective;portraitPolicy_.final=final.material;portraitPolicy_.row=row;
                portraitPolicy_.leaf=origin.finalLeaf;portraitPolicy_.reference=reference;portraitPolicy_.known=true;}
            catch(...){portraitPolicy_.known=false;portraitPolicy_.final=nullptr;}
        }
        output=std::move(next);d.sourceEffectsCommitted|=serviced;return true;
    }catch(...){return stop(S::allocation,0x247110);}
}

}
