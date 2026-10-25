#include "original_standard_material_table50la.h"
#include "../../platform/linux/native/original_ui_memory_card_constructor50kg.h"
#include "original_shader_pass_groups50la.h"
#include "original_standard_texture_resource50la.h"
#include "original_standard_material_animation50la.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <exception>
#include <limits>
#include <new>
#include <utility>
#include <type_traits>

namespace rr::assets {
namespace {
using D=OriginalStandardMaterialDiagnostic50LA;
using S=OriginalStandardMaterialStatus50LA;
constexpr std::size_t tableRows=3235,tableBytes=70962;
constexpr std::size_t collectorLimit=4096; //29C4E8 registration slots, NOT FIFO packet capacity.
bool fail(D& d,S s,std::uint32_t pc) noexcept {d.status=s;d.sourcePC=pc;return false;}
char lower(char c) noexcept {return c>='A'&&c<='Z'?char(c+('a'-'A')):c;}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)if(lower(a[i])!=lower(b[i]))return false;
    return true;
}
bool ascii(const std::string& value,std::size_t maximum,bool empty) noexcept {
    if(value.size()>maximum||(!empty&&value.empty()))return false;
    for(unsigned char c:value)if(c<32||c>=127)return false;
    return true;
}
const OriginalUiMemberIdentity50KG* appMember(const char* name) noexcept {
    const auto* app=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart);
    if(!app)return nullptr;
    for(std::size_t i=0;i<app->memberCount;++i){const auto* m=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::appStart,i);
        if(m&&std::strcmp(m->name,name)==0)return m;}
    return nullptr;
}
bool collectorPass(const OriginalStandardMaterialObject50LA& material,std::size_t slot,
    const OriginalStandardMaterialContext50LA& construction,
    OriginalStandardMaterialPassView50LA& pass,D& d) noexcept {
    if(!material.pass4C(slot,pass)||!pass.references04||*pass.references04<=0||
       !pass.program18||*pass.program18<0||!pass.texture1A||!pass.uv1E||
       !pass.mapCount20||*pass.mapCount20>2||!pass.flags21||(*pass.flags21&~7u)||
       !pass.group22||(*pass.group22!=-1&&*pass.group22!=-2&&
           !OriginalShaderPassGroups50LA::contains(*pass.group22))||!pass.state30||!pass.nativeBindings||
       bool(*pass.flags21&1u)!=(*pass.mapCount20==2)||
       pass.state30->contextCount()!=(*pass.mapCount20==2?2u:1u))
        return fail(d,S::collector,0x249BA0u);
    //Unique vertex leaves retain their existing merge behavior. Unique
    //texture leaves are excluded separately by249C9C..249CF0 below.
    for(std::size_t i=0;i<pass.state30->contextCount();++i){
        const auto& state=*pass.state30->context(i);
        if(state.runtime435670||state.videoDepthBase2E0!=construction.depthBufferBase2E0()||
           state.videoDepthFormat5C!=static_cast<std::int32_t>(construction.depthBufferFormat5C()))
            return fail(d,S::collector,0x249C84u);
    }
    return true;
}
bool equalCollectorState(const OriginalStandardMaterialStateContext50LA& a,
    const OriginalStandardMaterialStateContext50LA& b) noexcept {
    // Bounded constructor fields occupy disjoint payload bits. Compare actual
    // values, including inactive ones, never provenance/template/source PCs.
    for(std::size_t i=0;i<a.fields.size();++i)if(a.fields[i].value!=b.fields[i].value)return false;
    for(std::size_t i=0;i<a.sampler.size();++i)if(a.sampler[i].value!=b.sampler[i].value)return false;
    return a.videoDepthBase2E0==b.videoDepthBase2E0&&a.videoDepthFormat5C==b.videoDepthFormat5C;
}
// Exact host state, including provenance. Collector comparison intentionally
// ignores provenance; the UI transaction must not erase or invent it.
bool equalUiInteger(const OriginalStandardMaterialInteger50LA& a,
    const OriginalStandardMaterialInteger50LA& b)noexcept {
    return a.value==b.value&&a.origin==b.origin&&a.templateMember==b.templateMember&&
        a.byteOffset==b.byteOffset&&a.sourcePC==b.sourcePC;
}
bool equalUiContext(const OriginalStandardMaterialStateContext50LA& a,
    const OriginalStandardMaterialStateContext50LA& b)noexcept {
    for(std::size_t i=0;i<a.fields.size();++i)if(!equalUiInteger(a.fields[i],b.fields[i]))return false;
    for(std::size_t i=0;i<a.sampler.size();++i)if(!equalUiInteger(a.sampler[i],b.sampler[i]))return false;
    return a.videoDepthBase2E0==b.videoDepthBase2E0&&a.videoDepthFormat5C==b.videoDepthFormat5C&&
        a.runtime435670==b.runtime435670;
}
bool equalUiBindings(const std::array<OriginalStandardMaterialNativeBinding50KG,2>& a,
    const std::array<OriginalStandardMaterialNativeBinding50KG,2>& b)noexcept {
    return a[0].equivalent(b[0])&&a[1].equivalent(b[1]);
}
bool sameUiPassPointers(const OriginalStandardMaterialPassView50LA& a,
    const OriginalStandardMaterialPassView50LA& b)noexcept {
    return a.references04==b.references04&&a.program18==b.program18&&a.texture1A==b.texture1A&&
        a.uv1E==b.uv1E&&a.mapCount20==b.mapCount20&&a.flags21==b.flags21&&a.group22==b.group22&&
        a.fillRgba2C==b.fillRgba2C&&a.constructionTemplate==b.constructionTemplate&&a.state30==b.state30&&
        a.parameterMask24==b.parameterMask24&&a.parameterCount28==b.parameterCount28&&
        a.parameters34==b.parameters34&&a.animation10==b.animation10&&
        a.textureAnimation10==b.textureAnimation10&&a.nativeBindings==b.nativeBindings;
}
bool sameUiSlotBefore(const OriginalStandardMaterialUiSharedHistorySlot50LA& a,
    const OriginalStandardMaterialUiSharedHistorySlot50LA& b)noexcept {
    return a.row==b.row&&a.pass==b.pass&&a.object==b.object&&a.state30==b.state30&&
        a.nativeBindings==b.nativeBindings&&equalUiContext(a.beforeContext,b.beforeContext)&&
        equalUiBindings(a.beforeNativeBindings,b.beforeNativeBindings);
}
bool sameUiSourceName(const std::string& a,const char* b)noexcept {
    const auto fold=[](char c){return c=='\\'?'/':lower(c);};
    std::size_t i=0;
    for(;i<a.size()&&b[i];++i)if(fold(a[i])!=fold(b[i]))return false;
    return i==a.size()&&!b[i];
}
bool equalCollectorPass249BA0(const OriginalStandardMaterialPassView50LA& a,
    const OriginalStandardMaterialPassView50LA& b) noexcept {
    //249CF4..249D08: unique(-2) never merges, even with itself. This is per
    //canonical material/pass registration; its primitive submissions still
    //share that one registration and preserve their source FIFO order.
    if(*a.group22==-2||*b.group22==-2)return false;
    if(*a.program18!=*b.program18||*a.mapCount20!=*b.mapCount20||
       *a.texture1A!=*b.texture1A||*a.uv1E!=*b.uv1E||
       (*a.flags21&6u)!=(*b.flags21&6u)||*a.group22!=*b.group22||
       a.state30->contextCount()!=b.state30->contextCount())return false;
    if((a.textureAnimation10&&a.textureAnimation10->unique18())||
       (b.textureAnimation10&&b.textureAnimation10->unique18()))return false;
    // Single2476A0 has15 payload qwords; dual2480D0 has27. Context count
    // distinguishes those complete shapes; both dual contexts are compared.
    for(std::size_t i=0;i<a.state30->contextCount();++i)
        if(!equalCollectorState(*a.state30->context(i),*b.state30->context(i))||
           !(*a.nativeBindings)[i].equivalent((*b.nativeBindings)[i]))return false;
    // Material name/pass ordinal, fill RGBA, parameters and vertex animation
    // are deliberately absent from the original comparison. A future FIFO
    // must retain each originating submission's own parameter/animation data.
    return true;
}
}
struct OriginalStandardMaterialTable50LA::Storage {
    struct Row {
        std::string name00;
        std::unique_ptr<OriginalStandardMaterialObject50LA> object04;
        std::int32_t references08=0;
        const OriginalUiMaterialTemplateState50KG* associated30=nullptr;
        std::uint32_t byteOffset=0;
        std::shared_ptr<const OriginalUiArchive50KG> tournamentMissingPsm;
        std::shared_ptr<OriginalStandardTextureTable50LA> tournamentTextures;
        std::shared_ptr<const OriginalUiArchive50KG> speedBoostMissingPsm;
        std::shared_ptr<OriginalStandardTextureTable50LA> speedBoostTextures;
        std::shared_ptr<const OriginalUiArchive50KG> damageMissingPsm;
        std::shared_ptr<OriginalStandardTextureTable50LA> damageTextures;
        std::shared_ptr<const OriginalUiArchive50KG> pauseBackdropMissingPsm;
        std::shared_ptr<OriginalStandardTextureTable50LA> pauseBackdropTextures;
        std::shared_ptr<const OriginalGarageScreenFallback50KG> garageFallback;
        std::shared_ptr<const OriginalMemoryCardScreenFallback50KG> memoryCardPending;
    };
    std::shared_ptr<const OriginalUiArchive50KG> appStart;
    std::shared_ptr<const OriginalUiMaterialTemplates50KG> templates;
    std::vector<Row> rows;
    // Both the legacy complete preparation and early source consumer use
    // this one parser against their supplied authenticated decoded bytes.
    // It constructs these exact rows once; template attachment never calls it.
    bool loadFreshNames(const std::uint8_t* sourceData,std::size_t sourceSize,D& d) {
        rows.reserve(tableRows);
        const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
        std::size_t cursor=0;
        while(cursor<sourceSize){
            while(cursor<sourceSize&&delimiter(sourceData[cursor]))++cursor;
            if(cursor==sourceSize)break;
            const auto start=cursor;
            while(cursor<sourceSize&&!delimiter(sourceData[cursor]))++cursor;
            Storage::Row row;row.byteOffset=static_cast<std::uint32_t>(start);
            row.name00.assign(reinterpret_cast<const char*>(sourceData+start),cursor-start);
            if(!ascii(row.name00,255,false)||rows.size()>=tableRows)return fail(d,S::table,0x298788);
            for(char& c:row.name00)c=lower(c); //3094E0. No separator conversion.
            //29888C/2988B0 and14D344 are the real fresh null/zero stores.
            row.object04=nullptr;row.references08=0;row.associated30=nullptr;
            rows.push_back(std::move(row));
        }
        if(rows.size()!=tableRows)return fail(d,S::table,0x2988D8);
        return true;
    }
    std::unique_ptr<OriginalStandardMaterialObject50LA> partial;
    std::shared_ptr<const OriginalStandardProgramTable50LA> programs;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> construction;
    struct CollectorEntry {
        const OriginalStandardMaterialObject50LA* material=nullptr;
        std::int32_t row=-1,pass=-1,redirect=-1;
        std::int32_t limit=std::numeric_limits<std::int32_t>::max(); //29C0E4..F0.
        void* nativeHead0C=nullptr;void* nativeTail10=nullptr;std::uint32_t nativeEpoch14=0;
    };
    std::vector<CollectorEntry> collector;
    std::uint64_t collectorGeneration=0; // Native seal, not source frame epoch04.
    bool collectorActive=false,collectorSealed=false;
    std::uint32_t nativeFifoPins=0;
    bool nativeBindingPoisoned=false,collectorBindingDirty=false;
    // Distinct serial UI lease, not a FIFO pin. The actual bridge owns the
    // receipt lifetime and must release it before ordered table retirement.
    const UiSharedHistoryOwner* uiSharedHistoryOwner=nullptr;
    const UiSharedHistoryReceipt* uiSharedHistoryReceipt=nullptr;
    std::uint64_t uiSharedHistoryLeaseRevision=0,nativeBindingRevision=0;
    const ::original_main_entry50KG::NativeCollectorFifo50LA* uiNativeCollectorOwner=nullptr;
    void invalidateCollector() noexcept {
        collectorSealed=false;
        collector.clear(); // Borrowed node pointers retire before any actual pass destruction.
    }
    D terminalDiagnostic{};
    bool populationBound=false,populationActive=false,populationTerminal=false;
    // Fresh application BSS guards24A460/24A4E0. One owner, never per surface,
    // material, model or scene. Stored values are not exposed until source use.
    bool firstDemand24A47C=false,firstTexture24A4EC=false;
    float demandScale4356E0=0;
    std::uint32_t demandUvAssertion4356E8=0;
    bool demandActive=false,demandTerminal=false;
    D demandTerminalDiagnostic{};
    ~Storage(){
        if(nativeFifoPins||uiSharedHistoryOwner||uiSharedHistoryReceipt)std::terminate();
        invalidateCollector();
        //Terminal partial ownership is native cleanup, never a source success
        //or an automatic rollback of already-published table rows.
        partial.reset();
        for(auto& row:rows)row.object04.reset(); //14D240 source ascending order.
    }
};
OriginalStandardMaterialTable50LA::OriginalStandardMaterialTable50LA() noexcept=default;
OriginalStandardMaterialTable50LA::~OriginalStandardMaterialTable50LA()=default;
bool OriginalStandardMaterialTable50LA::prepareFresh(
    std::shared_ptr<const OriginalUiArchive50KG> app,
    std::shared_ptr<const OriginalUiMaterialTemplates50KG> templates,D& d) noexcept {
    d={};if(storage_||appStartNames_)return fail(d,S::alreadyPrepared,0x14D2F0);
    if(!app||!app->loaded()||app->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart))
        return fail(d,S::archive,0x2985B0);
    const auto* defaults=templates?templates->get(OriginalUiTemplate50KG::defaultMt):nullptr;
    const auto* screen=templates?templates->get(OriginalUiTemplate50KG::screenMt):nullptr;
    if(!defaults||!screen||defaults->identity!=appMember("AppStart\\DefaultMT.psm")||
       screen->identity!=appMember("AppStart\\ScreenMT.psm"))return fail(d,S::templates,0x14D590);
    try {
        const auto source=app->find("AppStart\\Mat_Stbl.txt");
        if(!source.identity||source.identity!=appMember("AppStart\\Mat_Stbl.txt")||
           !source.data||source.size!=tableBytes)return fail(d,S::table,0x298724);
        auto next=std::make_unique<Storage>();next->appStart=std::move(app);next->templates=std::move(templates);
        if(!next->loadFreshNames(source.data,source.size,d))return false;
        storage_=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x2987DC);}
    catch(...){return fail(d,S::input,0x2985B0);}
}
bool OriginalStandardMaterialTable50LA::prepareFresh(
    std::shared_ptr<const OriginalUiMaterialApplication50LA> application,D& d) noexcept {
    d={};if(!application||!application->prepared())return fail(d,S::templates,0x14D2F0);
    const auto* templates=&application->templates();
    std::shared_ptr<const OriginalUiMaterialTemplates50KG> retained(application,templates);
    return prepareFresh(application->appStart(),std::move(retained),d);
}
bool OriginalStandardMaterialTable50LA::prepareAppStartNames17CAD4(
    std::shared_ptr<const OriginalUiArchive50KG> app,
    const std::vector<std::uint8_t>& sourceBytes,D& d) noexcept {
    d={};if(storage_||appStartNames_)return fail(d,S::alreadyPrepared,0x14D2F0);
    if(!app||!app->loaded()||app->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart))
        return fail(d,S::archive,0x2985B0);
    try {
        const auto source=app->find("AppStart\\Mat_Stbl.txt");
        if(!source.identity||source.identity!=appMember("AppStart\\Mat_Stbl.txt")||
           !source.data||source.size!=tableBytes)return fail(d,S::table,0x298724);
        // The canonical member authenticates the actual service result; its
        // resident view is NOT substituted for the just-completed source read.
        // The domain alone proves the131230/12EC90/12FB30/131040 chronology.
        if(sourceBytes.size()!=source.size||
           std::memcmp(sourceBytes.data(),source.data,source.size)!=0)return fail(d,S::source,0x298724);
        auto next=std::make_unique<Storage>();next->appStart=std::move(app);
        if(!next->loadFreshNames(sourceBytes.data(),sourceBytes.size(),d))return false;
        // Remains publicly unprepared: no references, objects or templates can
        // be observed/mutated through the existing storage_-gated consumers.
        appStartNames_=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x2987DC);}
    catch(...){return fail(d,S::input,0x2985B0);}
}
bool OriginalStandardMaterialTable50LA::attachAppStartTemplates(
    std::shared_ptr<const OriginalUiMaterialApplication50LA> application,D& d) noexcept {
    d={};if(storage_)return fail(d,S::alreadyPrepared,0x14D2F0);
    if(!appStartNames_)return fail(d,S::unprepared,0x14D2F0);
    if(!application||!application->prepared())return fail(d,S::templates,0x14D590);
    if(application->appStart()!=appStartNames_->appStart)return fail(d,S::archive,0x14D590);
    const auto& templates=application->templates();
    const auto* defaults=templates.get(OriginalUiTemplate50KG::defaultMt);
    const auto* screen=templates.get(OriginalUiTemplate50KG::screenMt);
    if(!defaults||!screen||defaults->identity!=appMember("AppStart\\DefaultMT.psm")||
       screen->identity!=appMember("AppStart\\ScreenMT.psm"))return fail(d,S::templates,0x14D590);
    // The alias pins the actual application. Neither its templates nor the
    // early canonical rows are copied; all null associations remain null.
    appStartNames_->templates=std::shared_ptr<const OriginalUiMaterialTemplates50KG>(application,&templates);
    storage_=std::move(appStartNames_);return true;
}
bool OriginalStandardMaterialTable50LA::prepared() const noexcept {return bool(storage_);}
std::size_t OriginalStandardMaterialTable50LA::rowCount() const noexcept {return storage_?storage_->rows.size():0;}
bool OriginalStandardMaterialTable50LA::row(std::int32_t index,OriginalStandardMaterialRowView50LA& out) const noexcept {
    if(!storage_||index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return false;
    const auto& r=storage_->rows[static_cast<std::size_t>(index)];
    out={&r.name00,r.object04.get(),&r.references08,r.associated30,r.byteOffset};return true;
}
bool OriginalStandardMaterialTable50LA::find298C80(const std::string& name,std::int32_t& out,D& d) const noexcept {
    d={};if(!storage_)return fail(d,S::unprepared,0x298C80);
    if(!ascii(name,255,false))return fail(d,S::name,0x298C80);
    for(std::size_t i=0;i<storage_->rows.size();++i)if(same(name,storage_->rows[i].name00)){
        out=static_cast<std::int32_t>(i);d.row=out;return true;
    }
    return fail(d,S::missingName,0x298D64);
}
bool OriginalStandardMaterialTable50LA::resolve298E60(std::int32_t index,
    const OriginalStandardMaterialObject50LA*& out,D& d) const noexcept {
    d={};d.row=index;if(!storage_)return fail(d,S::unprepared,0x298E60);
    // The source count-inclusive assertion is unsafe at count. The native
    // admitted domain is the actual allocated rows; reject that bad index.
    if(index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return fail(d,S::range,0x298E60);
    if(storage_->populationTerminal)return fail(d,S::phase,storage_->terminalDiagnostic.sourcePC);
    out=storage_->rows[static_cast<std::size_t>(index)].object04.get();return true;
}
bool OriginalStandardMaterialTable50LA::screenApplicationMatches(
    const OriginalUiMaterialApplication50LA& application) const noexcept {
    return storage_&&application.prepared()&&storage_->appStart==application.appStart()&&
        storage_->templates.get()==&application.templates();
}
bool OriginalStandardMaterialTable50LA::serviceTournamentMarker24A740(std::int32_t index,
    const OriginalStandardTextureTable50LA& textures,const OriginalUiMemberView50KG& psm,
    const OriginalUiMemberView50KG& atm,const rr::game::OriginalApplicationAnimationClockView50KG& clock,D& d) noexcept {
    d={};d.row=index;
    if(!storage_)return fail(d,S::unprepared,0x24A740);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationTerminal||s.populationActive||s.demandActive||s.collectorActive||s.demandTerminal)
        return fail(d,S::phase,0x24A740);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return fail(d,S::range,0x24A740);
    const auto& row=s.rows[static_cast<std::size_t>(index)];
    if(row.references08<=0||row.associated30!=s.templates->get(OriginalUiTemplate50KG::screenMt)||
       s.textures.get()!=&textures)return fail(d,S::reference,0x24A740);
    if(!row.object04)return fail(d,S::missingMaterialObject,0x24A740);
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(s.demandActive);
    if(row.object04->serviceTournamentMarker24A740(textures,psm,atm,clock,d))return true;
    //Native failure after an actual leaf write is terminal; preserve effects
    //and require ordered cleanup, rather than rolling the controller back.
    if(d.sourceEffectsCommitted){s.demandTerminal=true;s.demandTerminalDiagnostic=d;}
    return false;
}
bool OriginalStandardMaterialTable50LA::demandTournamentMarker2DF8A0(
    std::int32_t index,const OriginalStandardTextureTable50LA& textures,
    const OriginalUiMemberView50KG& psm,const OriginalUiMemberView50KG& atm,D& d) const noexcept {
    d={};d.row=index;
    if(!storage_)return fail(d,S::unprepared,0x2DF8A0);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationTerminal||s.populationActive||s.demandActive||s.collectorActive||s.demandTerminal)
        return fail(d,S::phase,0x2DF8A0);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return fail(d,S::range,0x2DF8B4);
    const auto& row=s.rows[static_cast<std::size_t>(index)];
    if(row.references08<=0||row.associated30!=s.templates->get(OriginalUiTemplate50KG::screenMt)||
        s.textures.get()!=&textures)return fail(d,S::reference,0x2DF8B4);
    if(!row.object04)return fail(d,S::missingMaterialObject,0x2DF8BC);
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(s.demandActive);
    return row.object04->demandTournamentMarker2DF8A0(textures,psm,atm,d);
}
bool OriginalStandardMaterialTable50LA::demandTournamentOutline2DF8A0(
    std::int32_t index,const OriginalStandardTextureTable50LA& textures,
    const OriginalUiMemberView50KG& psm,const OriginalUiMemberView50KG& pst,D& d) const noexcept {
    d={};d.row=index;
    if(!storage_)return fail(d,S::unprepared,0x2DF8A0);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationTerminal||s.populationActive||s.demandActive||s.collectorActive||s.demandTerminal)
        return fail(d,S::phase,0x2DF8A0);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return fail(d,S::range,0x2DF8B4);
    const auto& row=s.rows[static_cast<std::size_t>(index)];
    if(row.references08<=0||row.associated30!=s.templates->get(OriginalUiTemplate50KG::screenMt)||
        s.textures.get()!=&textures)return fail(d,S::reference,0x2DF8B4);
    if(!row.object04)return fail(d,S::missingMaterialObject,0x2DF8BC);
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(s.demandActive);
    return row.object04->demandTournamentOutline2DF8A0(textures,psm,pst,d);
}
bool OriginalStandardMaterialTable50LA::demandSharedWarning2DF8A0(
    std::int32_t index,const OriginalStandardTextureTable50LA& textures,
    const OriginalUiMemberView50KG& psm,const OriginalUiMemberView50KG& pst,D& d) const noexcept {
    d={};d.row=index;
    if(!storage_)return fail(d,S::unprepared,0x2DF8A0);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationTerminal||s.populationActive||s.demandActive||s.collectorActive||s.demandTerminal)
        return fail(d,S::phase,0x2DF8A0);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return fail(d,S::range,0x2DF8B4);
    const auto& row=s.rows[static_cast<std::size_t>(index)];
    if(row.references08<=0||row.associated30!=s.templates->get(OriginalUiTemplate50KG::screenMt)||
       s.textures.get()!=&textures)return fail(d,S::reference,0x2DF8B4);
    if(!row.object04)return fail(d,S::missingMaterialObject,0x2DF8BC);
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(s.demandActive);
    return row.object04->demandSharedWarning2DF8A0(textures,psm,pst,d);
}
bool OriginalStandardMaterialTable50LA::demandGarage2DF8A0(
    std::int32_t index,const OriginalStandardTextureTable50LA& textures,
    std::uint32_t pc,D& d) const noexcept {
    d={};d.row=index;
    if(!storage_)return fail(d,S::unprepared,0x2DF8A0);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationTerminal||s.populationActive||s.demandActive||s.collectorActive||s.demandTerminal)
        return fail(d,S::phase,0x2DF8A0);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return fail(d,S::range,0x2DF8B4);
    const auto& row=s.rows[static_cast<std::size_t>(index)];
    if(row.references08<=0||row.associated30!=s.templates->get(OriginalUiTemplate50KG::screenMt)||
       s.textures.get()!=&textures)return fail(d,S::reference,0x2DF8B4);
    if(!row.object04)return fail(d,S::missingMaterialObject,0x2DF8BC);
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(s.demandActive);
    return row.object04->demandGarage2DF8A0(textures,pc,d);
}
bool OriginalStandardMaterialTable50LA::demandTournamentFallback2DF8A0(
    std::int32_t index,const OriginalStandardTextureTable50LA& textures,
    const OriginalUiArchive50KG& archive,std::uint32_t storePC,D& d) const noexcept {
    d={};d.row=index;
    if(!storage_)return fail(d,S::unprepared,0x2DF8A0);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationTerminal||s.populationActive||s.demandActive||s.collectorActive||s.demandTerminal)
        return fail(d,S::phase,0x2DF8A0);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return fail(d,S::range,0x2DF8B4);
    const auto& row=s.rows[static_cast<std::size_t>(index)];
    if(row.references08<=0||row.associated30!=s.templates->get(OriginalUiTemplate50KG::screenMt)||
        s.textures.get()!=&textures||row.tournamentTextures.get()!=&textures||
        row.tournamentMissingPsm.get()!=&archive)return fail(d,S::reference,0x2DF8B4);
    if(!row.object04)return fail(d,S::missingMaterialObject,0x2DF8BC);
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(s.demandActive);
    return row.object04->demandTournamentFallback2DF8A0(textures,archive,storePC,d);
}
bool OriginalStandardMaterialTable50LA::populationTerminal() const noexcept {
    return storage_&&storage_->populationTerminal;
}
bool OriginalStandardMaterialTable50LA::rebuildColdCollector14D440(
    OriginalStandardMaterialCollectorReceipt50LA& output,D& d) noexcept {
    d={};if(!storage_)return fail(d,S::unprepared,0x14D440u);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.nativeBindingPoisoned||s.nativeFifoPins||s.populationTerminal||s.populationActive||s.demandActive||s.collectorActive||demandTerminal())
        return fail(d,S::phase,0x14D440u);
    if(s.populationBound&&(!s.construction||!s.construction->live()))return fail(d,S::video,0x14D440u);
    if(s.collectorGeneration==std::numeric_limits<std::uint64_t>::max())return fail(d,S::overflow,0x14D440u);
    struct Guard {bool& active;explicit Guard(bool& a):active(a){active=true;}~Guard(){active=false;}} guard(s.collectorActive);
    //29C3F0 clears all old nodes;29BEA0 clears every nonnull material's active
    // mapping before ANY registration. No pass is retained/released here.
    // A failed prefix remains private/unsealed and cannot revive an old seal.
    s.invalidateCollector();d.sourceEffectsCommitted=true;
    for(auto& row:s.rows)if(row.object04)row.object04->clearCollectors29BEA0();
    try {
        s.collector.reserve(collectorLimit);
        for(std::size_t r=0;r<s.rows.size();++r){
            auto* material=s.rows[r].object04.get();
            if(!material)continue; //14D490: pending positive refs are not constructed passes.
            d.row=static_cast<std::int32_t>(r);
            const auto* count=material->passCount48();
            if(!s.construction||!material->constructed29B550()||!material->row40()||
               *material->row40()!=d.row||!count||*count<1||*count>4)
                return fail(d,S::collector,0x29BED0u);
            std::int32_t previous=-1;
            for(std::int32_t p=0;p<*count;++p){
                d.pass=static_cast<std::size_t>(p);d.slot=s.collector.size();
                if(s.collector.size()>=collectorLimit)return fail(d,S::range,0x29C4E8u);
                OriginalStandardMaterialPassView50LA pass;
                if(!collectorPass(*material,d.pass,*s.construction,pass,d))return false;
                const auto current=static_cast<std::int32_t>(s.collector.size());
                //29C490 compares ALL prior registrations, even redirected ones.
                // Later representatives can redirect earlier leaves only before
                // their own later-pass barrier. Do not sort a racer-only subset.
                for(auto& prior:s.collector){
                    OriginalStandardMaterialPassView50LA earlier;
                    if(!collectorPass(*prior.material,static_cast<std::size_t>(prior.pass),
                        *s.construction,earlier,d))return false;
                    if(equalCollectorPass249BA0(pass,earlier)&&current<prior.limit)prior.redirect=current;
                }
                Storage::CollectorEntry entry;
                entry.material=material;entry.row=d.row;entry.pass=p;
                s.collector.push_back(entry);
                material->setCollector29BED0(d.pass,current);
                //29BF58..74 writes this barrier AFTER the above redirect scan.
                if(previous>=0)s.collector[static_cast<std::size_t>(previous)].limit=current;
                previous=current;
            }
        }
        ++s.collectorGeneration;s.collectorSealed=true;
        OriginalStandardMaterialCollectorReceipt50LA next;
        next.owner_=this;next.generation_=s.collectorGeneration;output=next;
        d.status=S::complete;return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x29C558u);}
    catch(...){return fail(d,S::collector,0x14D440u);}
}
bool OriginalStandardMaterialTable50LA::bindUiNativeCollector1B73BC(
    const ::original_main_entry50KG::NativeCollectorFifo50LA& owner)noexcept {
    //Construction may precede14D440. This retains only actual native collector
    //lifetime; every later append still requires the separately sealed registry.
    if(!prepared()||storage_->uiSharedHistoryOwner||storage_->uiNativeCollectorOwner||storage_->nativeFifoPins||storage_->collectorActive)return false;
    storage_->uiNativeCollectorOwner=&owner;return true;
}
bool OriginalStandardMaterialTable50LA::canRetireNativeCollector29C300()const noexcept {
    return storage_&&!storage_->uiSharedHistoryOwner&&!storage_->nativeFifoPins&&!storage_->collectorActive&&
        !storage_->populationActive&&!storage_->demandActive;
}
void OriginalStandardMaterialTable50LA::retireNativeCollector29C300()noexcept {
    if(!canRetireNativeCollector29C300())std::terminate();
    //29C370..3A8 destroys registration nodes in ascending order. Concrete
    //24AED0->29C100 frees only each node, with no material/pass release.
    // Invalidate external receipts before freeing the same native records.
    storage_->collectorSealed=false;
    for(auto& entry:storage_->collector)entry=Storage::CollectorEntry{};
    std::vector<Storage::CollectorEntry>().swap(storage_->collector);
    // Unlike14D440 rebuild, source29C300 does NOT call29BEA0 on materials.
    storage_->uiNativeCollectorOwner=nullptr;storage_->collectorBindingDirty=false;
}
bool OriginalStandardMaterialTable50LA::pinNativeCollector(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt) noexcept {
    if(!coldCollectorCurrent(receipt)||storage_->nativeFifoPins==std::numeric_limits<std::uint32_t>::max())return false;
    ++storage_->nativeFifoPins;return true;
}
void OriginalStandardMaterialTable50LA::unpinNativeCollector() noexcept {
    if(!storage_||storage_->uiSharedHistoryOwner||!storage_->nativeFifoPins)std::terminate();
    --storage_->nativeFifoPins;
    //Source registration leaves are fixed throughout the actual FIFO drain.
    //Accepted249610 history is real immediately, but14D440 cannot rebuild
    //while pins exist. Expire the old comparison only at its last real pin.
    if(!storage_->nativeFifoPins&&storage_->collectorBindingDirty&&!storage_->uiNativeCollectorOwner){
        storage_->collectorSealed=false;storage_->collectorBindingDirty=false;
    }
}
bool OriginalStandardMaterialTable50LA::coldCollectorCurrent(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt) const noexcept {
    return storage_&&!storage_->uiSharedHistoryOwner&&!storage_->nativeBindingPoisoned&&receipt.owner_==this&&receipt.generation_&&
        receipt.generation_==storage_->collectorGeneration&&storage_->collectorSealed&&
        !storage_->collectorActive&&!storage_->populationActive&&!storage_->populationTerminal&&
        !demandTerminal()&&(!storage_->populationBound||
            (storage_->construction&&storage_->construction->live()));
}
std::size_t OriginalStandardMaterialTable50LA::coldCollectorCount(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt) const noexcept {
    return coldCollectorCurrent(receipt)?storage_->collector.size():0;
}
bool OriginalStandardMaterialTable50LA::coldCollectorEntry(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::size_t index,
    OriginalStandardMaterialCollectorEntryView50LA& output) const noexcept {
    if(!coldCollectorCurrent(receipt)||index>=storage_->collector.size())return false;
    const auto& entry=storage_->collector[index];
    auto leaf=index;
    while(storage_->collector[leaf].redirect!=-1){
        const auto next=storage_->collector[leaf].redirect;
        if(next<=static_cast<std::int32_t>(leaf)||static_cast<std::size_t>(next)>=storage_->collector.size())return false;
        leaf=static_cast<std::size_t>(next);
    }
    OriginalStandardMaterialCollectorEntryView50LA view;
    view.materialRow=entry.row;view.pass=entry.pass;view.registration=static_cast<std::int32_t>(index);
    view.redirect18=entry.redirect;view.limit1C=entry.limit;view.finalLeaf=static_cast<std::int32_t>(leaf);
    view.material=entry.material;output=view;return true;
}
const void* OriginalStandardMaterialTable50LA::nativeCollectorEntry29C0B0(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::size_t index)const noexcept {
    return coldCollectorCurrent(receipt)&&index<storage_->collector.size()?&storage_->collector[index]:nullptr;
}
bool OriginalStandardMaterialTable50LA::appendNativeCollector29C150(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::size_t index,
    std::uint32_t epoch,void* record,void*& previousTail)noexcept {
    OriginalStandardMaterialCollectorEntryView50LA view;
    if(!record||!coldCollectorEntry(receipt,index,view)||!storage_->nativeFifoPins||view.finalLeaf<0)return false;
    auto& leaf=storage_->collector[static_cast<std::size_t>(view.finalLeaf)];
    if(leaf.nativeEpoch14==epoch&&!leaf.nativeTail10)return false;
    previousTail=leaf.nativeEpoch14==epoch?leaf.nativeTail10:nullptr;
    if(leaf.nativeEpoch14!=epoch){leaf.nativeEpoch14=epoch;leaf.nativeHead0C=record;}
    leaf.nativeTail10=record;return true;
}
bool OriginalStandardMaterialTable50LA::coldCollectorPass(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t row,std::size_t pass,
    OriginalStandardMaterialCollectorEntryView50LA& output) const noexcept {
    if(!coldCollectorCurrent(receipt)||row<0||static_cast<std::size_t>(row)>=storage_->rows.size())return false;
    const auto* material=storage_->rows[static_cast<std::size_t>(row)].object04.get();
    if(!material||!material->passCount48()||pass>=static_cast<std::size_t>(*material->passCount48()))return false;
    const auto* index=material->collector5C(pass);
    OriginalStandardMaterialCollectorEntryView50LA next;
    if(!index||*index<0||!coldCollectorEntry(receipt,static_cast<std::size_t>(*index),next)||
       next.material!=material||next.materialRow!=row||next.pass!=static_cast<std::int32_t>(pass))return false;
    output=next;return true;
}
bool OriginalStandardMaterialTable50LA::prepareColdRepresentativeMap249610(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t row,
    std::size_t slot,std::size_t map,OriginalStandardMaterialMapBindingView50LA& output,D& d) const noexcept {
    return prepareColdMap249610(receipt,row,slot,map,PrototypeRoute::ordinary,output,d);
}
bool OriginalStandardMaterialTable50LA::prepareGarageParticleMap249610(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t row,std::size_t map,
    OriginalStandardMaterialMapBindingView50LA& out,D& d)const noexcept {
    OriginalStandardMaterialMapBindingView50LA next;
    if(!prepareColdMap249610(receipt,row,0,map,PrototypeRoute::ordinary,next,d))return false;
    OriginalStandardMaterialPassView50LA pass;OriginalStandardProgramRowView50LA program;
    if(map>1||!next.representative.material||next.representative.pass!=0||
       !next.representative.material->pass4C(0,pass)||!pass.program18||!pass.mapCount20||*pass.mapCount20!=2||
       !pass.constructionTemplate||pass.constructionTemplate!=storage_->templates->get(OriginalUiTemplate50KG::defaultMt)||
       pass.animation10||pass.textureAnimation10||!pass.uv1E||(*pass.uv1E)[0]||(*pass.uv1E)[1]||
       !storage_->programs->row(*pass.program18,program)||!program.canonicalName||
       std::strcmp(program.canonicalName,"ParticleCode"))return fail(d,S::unsupportedPass,0x24B1C0);
    next.nativeParticleRoute=true;out=next;return true;
}
bool OriginalStandardMaterialTable50LA::prepareColdScreenPrototypeMap249610(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t row,
    std::size_t slot,std::size_t map,OriginalStandardMaterialMapBindingView50LA& output,D& d) const noexcept {
    if(!prepareColdMap249610(receipt,row,slot,map,PrototypeRoute::screenNullKey,output,d))return false;
    output.nativeScreenPrototypeRoute=true;return true;
}
bool OriginalStandardMaterialTable50LA::selectRacerMaterial29BFF0(std::int32_t index,D& d) noexcept {
    d={};d.row=index;
    if(!storage_)return fail(d,S::unprepared,0x298e60u);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationActive||s.demandActive||s.collectorActive||s.populationTerminal||s.demandTerminal)
        return fail(d,S::phase,0x29bff0u);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return fail(d,S::range,0x298e60u);
    auto* material=s.rows[static_cast<std::size_t>(index)].object04.get();
    // Unlike a texture-null skip,229B40 calls material29BFF0 unconditionally.
    if(!material)return fail(d,S::missingMaterialObject,0x229b40u);
    s.demandActive=true;
    const bool completed=material->select29BFF0(d);s.demandActive=false;
    if(!completed&&d.sourceEffectsCommitted){s.demandTerminal=true;s.demandTerminalDiagnostic=d;}
    return completed;
}
bool OriginalStandardMaterialTable50LA::inspectCableEmission24B9B0(
    std::int32_t index,std::size_t slot,OriginalCableEmissionBinding50LA& out,D& d) const noexcept {
    d={};d.row=index;d.pass=slot;
    if(!storage_)return fail(d,S::unprepared,0x24a798u);
    const auto& s=*storage_;
    if(s.populationActive||s.demandActive||s.collectorActive||s.populationTerminal||demandTerminal()||
       !s.construction||!s.construction->live())return fail(d,S::phase,0x24a798u);
    // This is an observation of the actual sealed construction registry, NOT
    // source collector frame04, head0C/tail10 or currentScene4014 authority.
    OriginalStandardMaterialCollectorReceipt50LA seal;
    seal.owner_=this;seal.generation_=s.collectorGeneration;
    if(!coldCollectorCurrent(seal)||index<0||static_cast<std::size_t>(index)>=s.rows.size())
        return fail(d,S::collector,0x24a798u);
    const auto& row=s.rows[static_cast<std::size_t>(index)];
    if(row.references08<=0||row.associated30!=s.templates->get(OriginalUiTemplate50KG::defaultMt))
        return fail(d,S::reference,0x24a790u);
    if(!coldCollectorPass(seal,index,slot,out.origin_)||out.origin_.finalLeaf<0||
       !coldCollectorEntry(seal,static_cast<std::size_t>(out.origin_.finalLeaf),out.representative_)||
       out.representative_.redirect18!=-1)
        return fail(d,S::collector,0x24a7b0u);
    if(!collectorPass(*out.origin_.material,slot,*s.construction,out.originatingPass_,d)||
       !collectorPass(*out.representative_.material,static_cast<std::size_t>(out.representative_.pass),
           *s.construction,out.effectivePass_,d))return false;
    const auto& origin=out.originatingPass_;const auto& effective=out.effectivePass_;
    if(!(*origin.flags21&2u)||!(*effective.flags21&2u)||!origin.parameters34)
        return fail(d,S::source,0x24ba10u);
    // All earlier passes must be disabled at this unconsumed first-pass stop.
    // There is no externally supplied continuation token that can skip one.
    for(std::size_t previous=0;previous<slot;++previous){
        OriginalStandardMaterialPassView50LA pass;
        if(!row.object04->pass4C(previous,pass)||!pass.flags21||(*pass.flags21&2u))
            return fail(d,S::phase,0x24a728u);
    }
    const auto& runtime=s.construction->materialRuntime();
    if(!runtime.callbackOverrideKnown||runtime.callbackOverride435678)
        return fail(d,S::runtime,0x24a674u);
    out.originatingParameters_=origin.parameters34;
    if(origin.animation10){
        out.vertexBase_=origin.animation10->prototype10();
        if(!origin.animation10->constructed29B890()||!out.vertexBase_||
           out.vertexBase_->prototype()!=origin.animation10||out.vertexBase_->model()||out.vertexBase_->nativeModelId()||
           out.vertexBase_->callback00()||out.vertexBase_->callbackOwner04())
            return fail(d,S::unsupportedPass,0x24a760u);
    }
    out.textureBase_=origin.textureAnimation10;
    if(out.textureBase_&&(!out.textureBase_->unique18()||out.vertexBase_||
       out.textureBase_->materialOwner()!=out.origin_.material||
       out.textureBase_->passOrdinal()!=slot))return fail(d,S::unsupportedPass,0x24a750u);
    out.context_=s.construction;out.registry_=seal;out.mapCount_=*effective.mapCount20;
    for(std::size_t map=0;map<out.mapCount_;++map){
        // Read-only prospective binding only. Uses the FINAL representative's
        // actual map token/animation/resource and effective per-context TEXA.
        // It never dispatches that representative's animation or marks a bind.
        if(!prepareColdMap249610(seal,index,slot,map,PrototypeRoute::cableNullKey,out.maps_[map],d))return false;
    }
    d.sourcePC=0x24b9dcu;return true; // Inspection only;29C150 is not invoked.
}
OriginalCableMaterialCallbackStage50LA OriginalStandardMaterialTable50LA::serviceCableNullKey24A620(
    std::int32_t index,D& d) noexcept {
    using Stage=OriginalCableMaterialCallbackStage50LA;
    d={};d.row=index;
    const auto reject=[&](S status,std::uint32_t pc){fail(d,status,pc);return Stage::blocked;};
    if(!storage_)return reject(S::unprepared,0x2467c8u);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationActive||s.demandActive||s.collectorActive||s.populationTerminal||s.demandTerminal)
        return reject(S::phase,0x2467c8u);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return reject(S::range,0x2467c8u);
    auto& row=s.rows[static_cast<std::size_t>(index)];
    if(row.references08<=0||!s.templates||
       row.associated30!=s.templates->get(OriginalUiTemplate50KG::defaultMt))
        return reject(S::reference,0x2467c8u);
    if(!row.object04)return reject(S::missingMaterialObject,0x2467e0u);
    if(!s.construction||!s.construction->live())return reject(S::video,0x24a620u);
    const auto& runtime=s.construction->materialRuntime();
    if(!runtime.callbackOverrideKnown||runtime.callbackOverride435678!=0u)
        return reject(S::runtime,0x24a674u); // Alternate435678 branch unsupported.
    const auto* count=row.object04->passCount48();
    if(!count||*count<0||*count>4)return reject(S::passCount,0x24a64cu);
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(s.demandActive);
    for(std::size_t passIndex=0;passIndex<static_cast<std::size_t>(*count);++passIndex){
        d.pass=passIndex;OriginalStandardMaterialPassView50LA pass;
        if(!row.object04->pass4C(passIndex,pass)||!pass.flags21)return reject(S::source,0x24a728u);
        if(!(*pass.flags21&2u))continue; // No animation/stamp effects for disabled passes.
        if(!row.object04->serviceCableBasePrototype29AC90(passIndex,d)){
            if(d.sourceEffectsCommitted){s.demandTerminal=true;s.demandTerminalDiagnostic=d;}
            return Stage::blocked;
        }
        // The real443848 collector/renderer lifetime and24B9B0 are not joined.
        // Do not resolve a substitute renderer, bind a decoded image, issue a
        // draw receipt, continue later passes, or run strip children here.
        d.sourcePC=0x24a790u;return Stage::rendererSubmissionRequired;
    }
    d.sourcePC=0x24a7d0u;return Stage::materialReturned;
}
bool OriginalStandardMaterialTable50LA::serviceRetainedScreenPrototype24A740(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t index,
    std::size_t passIndex,D& d) noexcept {
    d={};d.row=index;d.pass=passIndex;
    if(!storage_||!coldCollectorCurrent(receipt))return fail(d,S::collector,0x24A620);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationActive||s.demandActive||s.collectorActive||s.populationTerminal||s.demandTerminal)
        return fail(d,S::phase,0x24A740);
    if(!s.construction||!s.construction->live())return fail(d,S::video,0x24A620);
    const auto& runtime=s.construction->materialRuntime();
    if(!runtime.callbackOverrideKnown||runtime.callbackOverride435678!=0u)
        return fail(d,S::runtime,0x24A674);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return fail(d,S::range,0x24A740);
    auto& row=s.rows[static_cast<std::size_t>(index)];OriginalStandardMaterialPassView50LA pass;
    OriginalStandardMaterialCollectorEntryView50LA collector;
    if(row.references08<=0||!s.templates||row.associated30!=s.templates->get(OriginalUiTemplate50KG::screenMt)||
       !row.object04||!row.object04->pass4C(passIndex,pass)||!pass.flags21||
       !coldCollectorPass(receipt,index,passIndex,collector))return fail(d,S::reference,0x24A740);
    if(!(*pass.flags21&2u))return true;
    // Current ScreenObject path has no model/clone key. Unique controllers
    // retain their actual origin leaf, never a representative's foreign base.
    if(pass.animation10||(pass.textureAnimation10&&
       (!pass.textureAnimation10->unique18()||collector.finalLeaf!=collector.registration)))
        return fail(d,S::unsupportedPass,0x29B1D0);
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(s.demandActive);
    if(row.object04->serviceCableBasePrototype29AC90(passIndex,d))return true;
    if(d.sourceEffectsCommitted){s.demandTerminal=true;s.demandTerminalDiagnostic=d;}
    return false;
}
bool OriginalStandardMaterialTable50LA::serviceRetainedModelPrototype24A740(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t index,std::size_t passIndex,
    const rr::game::OriginalApplicationAnimationClockView50KG& clock,D& d) noexcept {
    d={};d.row=index;d.pass=passIndex;
    if(!storage_||!coldCollectorCurrent(receipt))return fail(d,S::collector,0x24A620);
    auto& s=*storage_;
    if(s.uiSharedHistoryOwner||s.populationActive||s.demandActive||s.collectorActive||s.populationTerminal||s.demandTerminal)
        return fail(d,S::phase,0x24A740);
    if(index<0||static_cast<std::size_t>(index)>=s.rows.size())return fail(d,S::range,0x24A740);
    auto& row=s.rows[static_cast<std::size_t>(index)];OriginalStandardMaterialPassView50LA pass;
    OriginalStandardMaterialCollectorEntryView50LA collector;
    if(!s.textures||row.references08<=0||row.associated30!=s.templates->get(OriginalUiTemplate50KG::defaultMt)||
       !row.object04||!row.object04->pass4C(passIndex,pass)||!pass.flags21||
       !coldCollectorPass(receipt,index,passIndex,collector))return fail(d,S::reference,0x24A740);
    if(!(*pass.flags21&2u))return true; //24A728 disabled pass, no controller effects.
    if(!pass.textureAnimation10||!pass.textureAnimation10->unique18()||pass.animation10||
       collector.finalLeaf!=collector.registration)return fail(d,S::unsupportedPass,0x29B1D0);
    struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(s.demandActive);
    if(row.object04->serviceTexturePrototype29AC90(passIndex,*s.textures,clock,d))return true;
    if(d.sourceEffectsCommitted){s.demandTerminal=true;s.demandTerminalDiagnostic=d;}
    return false;
}
bool OriginalStandardMaterialTable50LA::prepareRetainedModelMap24B26C(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t index,std::size_t passIndex,
    std::size_t map,OriginalStandardMaterialMapBindingView50LA& output,D& d) const noexcept {
    d={};d.row=index;d.pass=passIndex;d.map=map;
    if(!storage_||!coldCollectorCurrent(receipt)||storage_->demandActive)return fail(d,S::collector,0x24B26C);
    if(index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return fail(d,S::range,0x24B26C);
    const auto& row=storage_->rows[static_cast<std::size_t>(index)];OriginalStandardMaterialPassView50LA pass;
    if(row.references08<=0||row.associated30!=storage_->templates->get(OriginalUiTemplate50KG::defaultMt)||
       !row.object04||!row.object04->pass4C(passIndex,pass)||pass.animation10||
       !pass.textureAnimation10||!pass.textureAnimation10->unique18())return fail(d,S::reference,0x24B26C);
    // Binding29AF90 is distinct from29B1D0 service selection. With the proven
    // absent table14, both independently resolve the same retained prototype.
    return prepareColdMap249610(receipt,index,passIndex,map,PrototypeRoute::retainedModel,output,d);
}
bool OriginalStandardMaterialTable50LA::prepareColdMap249610(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t row,
    std::size_t slot,std::size_t map,PrototypeRoute route,
    OriginalStandardMaterialMapBindingView50LA& output,D& d) const noexcept {
    d={};d.row=row;d.pass=slot;d.map=map;
    OriginalStandardMaterialMapBindingView50LA next;
    if(!coldCollectorPass(receipt,row,slot,next.origin)||next.origin.finalLeaf<0||
       !coldCollectorEntry(receipt,static_cast<std::size_t>(next.origin.finalLeaf),next.representative)||
       next.representative.redirect18!=-1||!next.representative.material||
       !storage_->construction||!storage_->textures||storage_->textures->populationTerminal())
        return fail(d,S::collector,0x24B1A0u);
    OriginalStandardMaterialPassView50LA pass;
    if(!collectorPass(*next.representative.material,static_cast<std::size_t>(next.representative.pass),
        *storage_->construction,pass,d))return false;
    if(map>=*pass.mapCount20||map>=pass.state30->contextCount())return fail(d,S::range,0x24B2C0u);
    next.map=map;next.textureToken=(*pass.texture1A)[map];
    next.contextAfter249610=*pass.state30->context(map);
    //24B1B0 skips the entire disabled representative, including map lookup.
    if(!(*pass.flags21&2u)){output=next;return true;}
    if(next.representative.material->textures383400()!=storage_->textures.get())
        return fail(d,S::texture,0x24B250u);
    //24B244's genuine -1 skip, distinct from a pending canonical resource.
    // Only the private cable inspection adds this domain; -2 still requires
    // the unjoined actual render-target249920 owner and is rejected below.
    if(route==PrototypeRoute::cableNullKey&&next.textureToken==-1){
        next.preparation=OriginalStandardMaterialMapPreparation50LA::absentMap;output=next;return true;
    }
    OriginalStandardTextureRowView50LA textureRow;
    if(next.textureToken<0||!storage_->textures->row(next.textureToken,textureRow)||
       !textureRow.references08||*textureRow.references08<=0)return fail(d,S::texture,0x24B254u);
    //24B260 skips pending resource04. Do not mark this map bound or select a
    //different image or fabricate an animated child for an unpopulated row.
    if(!textureRow.object04){
        next.preparation=OriginalStandardMaterialMapPreparation50LA::nullTexture;output=next;return true;
    }
    if(pass.textureAnimation10&&route==PrototypeRoute::ordinary)return fail(d,S::unsupportedPass,0x29B1D0);
    const OriginalStandardTextureResource50LA* selected=nullptr;
    const bool selection=route==PrototypeRoute::retainedModel?
        next.representative.material->selectModelPrototype24B26C(
            static_cast<std::size_t>(next.representative.pass),map,*textureRow.object04,selected,d):
        route==PrototypeRoute::cableNullKey?
        next.representative.material->selectTexturePrototype29AF90(
            static_cast<std::size_t>(next.representative.pass),map,*textureRow.object04,selected,d):
        next.representative.material->selectScreenPrototype24B278(
            static_cast<std::size_t>(next.representative.pass),map,*textureRow.object04,selected,d);
    if(!selection||!selected)return false;
    const auto& texture=*selected;
    const auto* flags=texture.flagsF8();const auto* known=texture.flagsF8KnownMask();
    if(!texture.constructed24DA30()||!texture.references00()||*texture.references00()<=0||
       !flags||!known||!(*known&1u))return fail(d,S::texture,0x249818u);
    //24B278 selected an actual102050 child for ATM, or ordinary29C6D0 self.
    //The aggregate itself never supplies ordinary249610 storage fields.
    next.texture=&texture;
    if(*flags&1u){
        using F=OriginalUiGsField50KG;
        const std::uint32_t pc=*pass.mapCount20==1?0x24983Cu:(map==0?0x249884u:0x2498BCu);
        const auto write=[&](F field,std::int32_t value){
            next.contextAfter249610.fields[static_cast<std::size_t>(field)]={
                value,OriginalStandardMaterialValueOrigin50LA::runtime,nullptr,0,pc};
        };
        write(F::sourceAlphaValueWhenAis0,255);write(F::sourceAlphaValueWhenAis1,255);
        write(F::methodOfExpandingTextureAlpha,1);
    }
    next.preparation=OriginalStandardMaterialMapPreparation50LA::ready;
    output=next;return true;
}
bool OriginalStandardMaterialTable50LA::prepareGarageNativeBinding249610(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t row,std::size_t pass,
    OriginalStandardMaterialMapBindingView50LA& output,OriginalUiGarageProgram50KG& family,D& d)const noexcept {
    d={};OriginalStandardMaterialMapBindingView50LA next;
    if(!prepareColdRepresentativeMap249610(receipt,row,pass,0,next,d))return false;
    if(next.preparation!=OriginalStandardMaterialMapPreparation50LA::ready||!next.texture)
        return fail(d,S::texture,0x24B254);
    OriginalStandardMaterialPassView50LA view;
    OriginalStandardProgramRowView50LA program;
    if(!next.representative.material->pass4C(static_cast<std::size_t>(next.representative.pass),view)||
       !view.mapCount20||*view.mapCount20!=1||!view.program18||!view.nativeBindings||
       !storage_->programs->row(*view.program18,program)||!program.canonicalName||
       view.animation10||view.textureAnimation10)
        return fail(d,S::unsupportedPass,0x24B278);
    OriginalUiGarageProgram50KG actual;
    if(std::strcmp(program.canonicalName,"PrelitCompressed")==0)actual=OriginalUiGarageProgram50KG::prelitCompressed;
    else if(std::strcmp(program.canonicalName,"Diffuse")==0)actual=OriginalUiGarageProgram50KG::diffuse;
    else if(std::strcmp(program.canonicalName,"DiffuseCompressed")==0)actual=OriginalUiGarageProgram50KG::diffuseCompressed;
    else if(std::strcmp(program.canonicalName,"SpecularMapCompressed")==0)actual=OriginalUiGarageProgram50KG::specularMapCompressed;
    else return fail(d,S::unsupportedPass,0x24B278);
    // Distinct source identities. No program, pass or required contribution is
    // skipped; unsupported animation/dual-map/foreign programs remain closed.
    output=next;family=actual;return true;
}

bool OriginalStandardMaterialTable50LA::prepareGarageUntexturedState249BA0(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,std::int32_t row,std::size_t slot,
    OriginalStandardMaterialMapBindingView50LA& output,D& d)const noexcept {
    d={};d.row=row;d.pass=slot;OriginalStandardMaterialMapBindingView50LA next;
    if(!coldCollectorPass(receipt,row,slot,next.origin)||next.origin.finalLeaf<0||
       !coldCollectorEntry(receipt,static_cast<std::size_t>(next.origin.finalLeaf),next.representative)||
       next.representative.redirect18!=-1||!next.representative.material||!storage_->construction)
        return fail(d,S::collector,0x24B1A0);
    OriginalStandardMaterialPassView50LA pass;OriginalStandardProgramRowView50LA program;
    if(!collectorPass(*next.representative.material,static_cast<std::size_t>(next.representative.pass),*storage_->construction,pass,d))return false;
    if(!pass.flags21||!(*pass.flags21&2u)||!pass.mapCount20||*pass.mapCount20||!pass.program18||
       !storage_->programs->row(*pass.program18,program)||!program.canonicalName||
       std::strcmp(program.canonicalName,"ScreenObject")||pass.animation10||pass.textureAnimation10||
       !pass.state30||pass.state30->contextCount()!=1u||!pass.nativeBindings||
       (*pass.nativeBindings)[0].bound()||(*pass.nativeBindings)[1].bound())return fail(d,S::unsupportedPass,0x249BA0);
    next.contextAfter249610=*pass.state30->context(0);
    using F=OriginalUiGsField50KG;
    if(next.contextAfter249610.fields[static_cast<std::size_t>(F::textureMappingOnOff)].value!=0)
        return fail(d,S::unsupportedPass,0x249BA0);
    next.nativeUntexturedRoute=true;next.preparation=OriginalStandardMaterialMapPreparation50LA::absentMap;
    output=next;return true;
}
bool OriginalStandardMaterialTable50LA::garageNativeBindingsCurrent(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,
    const std::vector<OriginalStandardMaterialMapBindingView50LA>& maps)const noexcept {
    if(!coldCollectorCurrent(receipt)||!storage_->nativeFifoPins||maps.empty()||
        storage_->nativeBindingRevision==std::numeric_limits<std::uint64_t>::max())return false;
    for(const auto& expected:maps){
        OriginalStandardMaterialMapBindingView50LA now;D d;
        const bool prepared=expected.nativeParticleRoute?
            prepareGarageParticleMap249610(receipt,expected.origin.materialRow,expected.map,now,d):
            expected.nativeUntexturedRoute?
            prepareGarageUntexturedState249BA0(receipt,expected.origin.materialRow,static_cast<std::size_t>(expected.origin.pass),now,d):
            prepareColdMap249610(receipt,expected.origin.materialRow,static_cast<std::size_t>(expected.origin.pass),0,
                expected.nativeScreenPrototypeRoute?PrototypeRoute::screenNullKey:PrototypeRoute::ordinary,now,d);
        if(!prepared||now.nativeParticleRoute!=expected.nativeParticleRoute||now.map!=expected.map||
           now.nativeUntexturedRoute!=expected.nativeUntexturedRoute||
           now.preparation!=(expected.nativeUntexturedRoute?OriginalStandardMaterialMapPreparation50LA::absentMap:OriginalStandardMaterialMapPreparation50LA::ready)||
           now.origin.material!=expected.origin.material||now.origin.registration!=expected.origin.registration||
           now.representative.material!=expected.representative.material||
           now.representative.registration!=expected.representative.registration||now.texture!=expected.texture||
           !equalCollectorState(now.contextAfter249610,expected.contextAfter249610))return false;
    }
    return true;
}
void OriginalStandardMaterialTable50LA::commitGarageNativeBindings249610(
    const OriginalStandardMaterialCollectorReceipt50LA& receipt,
    const std::vector<OriginalStandardMaterialMapBindingView50LA>& maps,
    const std::vector<OriginalStandardMaterialNativeBinding50KG>& images)noexcept {
    if(maps.size()!=images.size()||!garageNativeBindingsCurrent(receipt,maps))std::terminate();
    for(std::size_t i=0;i<images.size();++i)if(images[i].bound()==maps[i].nativeUntexturedRoute)std::terminate();
    // All validation precedes any mutation. Store only genuine final leaves.
    for(std::size_t i=0;i<maps.size();++i){const auto& map=maps[i];
        auto& object=storage_->rows[static_cast<std::size_t>(map.representative.materialRow)].object04;
        if(map.nativeUntexturedRoute)object->commitNativeUntexturedState249BA0(static_cast<std::size_t>(map.representative.pass),map.contextAfter249610);
        else object->commitNativeBinding249610(static_cast<std::size_t>(map.representative.pass),map.nativeParticleRoute?map.map:0,map.contextAfter249610,images[i]);
    }
    // The next14D440 must compare the retained effective history. A previous
    // construction/redirect receipt cannot be reused as that new comparison.
    ++storage_->nativeBindingRevision;storage_->collectorBindingDirty=true;
}
void OriginalStandardMaterialTable50LA::poisonGarageNativeBindings()noexcept {
    if(storage_&&storage_->uiSharedHistoryOwner)std::terminate();
    if(storage_){storage_->nativeBindingPoisoned=true;storage_->collectorSealed=false;}
}
OriginalStandardMaterialTable50LA::UiSharedHistoryReceipt::~UiSharedHistoryReceipt()noexcept {
    if(table_||owner_)std::terminate();
}
bool OriginalStandardMaterialTable50LA::nativeBindingHistoryUsable()const noexcept {
    // A failed unrelated canonical batch can poison the SAME table after the
    // UI lease has ended. Earlier accepted shared histories are not rolled
    // back, but cannot be reused. Real in-flight canonical FIFO pins remain
    // legitimate; this observation neither acquires nor substitutes for one.
    return storage_&&storage_->populationBound&&storage_->construction&&storage_->construction->live()&&
        !storage_->nativeBindingPoisoned&&!storage_->populationTerminal&&!demandTerminal()&&
        !storage_->populationActive&&!storage_->demandActive&&!storage_->collectorActive&&!storage_->partial;
}
bool OriginalStandardMaterialTable50LA::uiSharedHistoryQuiescent()const noexcept {
    if(!storage_)return false;
    const auto& s=*storage_;
    if(!s.populationBound||!s.construction||!s.construction->live()||!s.programs||!s.textures||
       !s.templates||s.nativeBindingPoisoned||s.nativeFifoPins||s.populationActive||s.populationTerminal||
       s.demandActive||s.collectorActive||demandTerminal()||s.partial)return false;
    // No material or texture population is skipped to borrow the three rows.
    // A pending foreign row is a pending real source operation, not a UI-only
    // manager. All subsequent table mutations are excluded by this lease.
    for(const auto& row:s.rows)if(row.references08>0&&!row.object04)return false;
    for(std::size_t i=0;i<s.textures->rowCount();++i){
        OriginalStandardTextureRowView50LA row;
        if(i>static_cast<std::size_t>(std::numeric_limits<std::int16_t>::max())||
           !s.textures->row(static_cast<std::int16_t>(i),row)||!row.references08||
           (*row.references08>0&&!row.object04))return false;
    }
    return true;
}
bool OriginalStandardMaterialTable50LA::captureUiSharedHistorySlot(std::size_t index,
    OriginalStandardMaterialUiSharedHistorySlot50LA& out,
    OriginalStandardMaterialPassView50LA& pass,std::int32_t& references)const noexcept {
    static constexpr const char* names[]={"ui/textures/dimpopup","ui/textures/overlay1popup","font/venus16popup"};
    static constexpr const char* sources[]={"ui/textures/dimpopup.psm","ui/textures/overlay1popup.psm","font/venus16popup.psm"};
    static constexpr std::uint32_t offsets[]={70804,70826,70853};
    if(!storage_||index>=3||storage_->rows.size()!=tableRows||!storage_->construction||!storage_->templates||
       !storage_->programs||!storage_->textures)return false;
    const auto& s=*storage_;const auto number=static_cast<std::int32_t>(3228+index);
    const auto& row=s.rows[static_cast<std::size_t>(number)];
    const auto* object=row.object04.get();
    const auto* screen=s.templates->get(OriginalUiTemplate50KG::screenMt);
    if(row.references08<=0||row.associated30!=screen||row.byteOffset!=offsets[index]||
       !sameUiSourceName(row.name00,names[index])||!object||!object->constructed29B550()||
       !object->row40()||*object->row40()!=number||!object->passCount48()||*object->passCount48()!=1||
       !object->source()||!object->source()->prepared()||!object->source()->member()||
       !sameUiSourceName(*object->source()->member(),sources[index])||
       object->textures383400()!=s.textures.get()||!object->pass4C(0,pass))return false;
    OriginalStandardProgramRowView50LA program;
    if(!pass.references04||*pass.references04<=0||!pass.program18||
       !s.programs->row(*pass.program18,program)||!program.canonicalName||
       std::strcmp(program.canonicalName,"ScreenObject")||!pass.texture1A||!pass.uv1E||
       !pass.mapCount20||*pass.mapCount20!=(index?1u:0u)||!pass.flags21||
       !(*pass.flags21&2u)||(*pass.flags21&~6u)||!pass.group22||
       pass.constructionTemplate!=screen||pass.animation10||pass.textureAnimation10||
       !pass.state30||pass.state30->contextCount()!=1u||!pass.nativeBindings||
       (*pass.nativeBindings)[1].bound()||(!index&&(*pass.nativeBindings)[0].bound()))return false;
    const auto* context=pass.state30->context(0);
    if(!context||context->runtime435670||
       context->videoDepthBase2E0!=s.construction->depthBufferBase2E0()||
       context->videoDepthFormat5C!=static_cast<std::int32_t>(s.construction->depthBufferFormat5C())||
       context->fields[static_cast<std::size_t>(OriginalUiGsField50KG::textureMappingOnOff)].value!=(index?1:0))return false;
    OriginalStandardMaterialUiSharedHistorySlot50LA next;
    next.row=number;next.object=object;next.state30=pass.state30;next.nativeBindings=pass.nativeBindings;
    next.beforeContext=*context;next.afterContext=*context;
    next.beforeNativeBindings=*pass.nativeBindings;next.afterNativeBindings=*pass.nativeBindings;
    next.writeKind=index?OriginalStandardMaterialUiSharedHistoryWrite50LA::bound:
        OriginalStandardMaterialUiSharedHistoryWrite50LA::untextured;
    out=next;references=row.references08;return true;
}
bool OriginalStandardMaterialTable50LA::beginUiSharedHistory(const UiSharedHistoryOwner* owner,
    const OriginalStandardMaterialContext50LA* context,UiSharedHistoryReceipt& receipt,
    UiSharedHistorySlots& output)noexcept {
    static_assert(std::is_nothrow_copy_assignable_v<UiSharedHistorySlots>);
    if(!owner||!context||!uiSharedHistoryQuiescent()||storage_->construction.get()!=context||
       storage_->uiSharedHistoryOwner||storage_->uiSharedHistoryReceipt||receipt.table_||receipt.owner_||
       storage_->uiSharedHistoryLeaseRevision==std::numeric_limits<std::uint64_t>::max()||
       storage_->nativeBindingRevision==std::numeric_limits<std::uint64_t>::max())return false;
    UiSharedHistorySlots slots;
    std::array<OriginalStandardMaterialPassView50LA,3> views;
    std::array<std::int32_t,3> references;
    for(std::size_t i=0;i<slots.size();++i)
        if(!captureUiSharedHistorySlot(i,slots[i],views[i],references[i]))return false;
    // Complete preflight before issuing the address-pinned private receipt.
    auto& s=*storage_;
    receipt.table_=this;receipt.owner_=owner;receipt.context_=context;
    receipt.leaseRevision_=++s.uiSharedHistoryLeaseRevision;receipt.bindingRevision_=s.nativeBindingRevision;
    receipt.snapshots_=slots;receipt.passViews_=views;receipt.retainedReferences_=references;
    s.uiSharedHistoryOwner=owner;s.uiSharedHistoryReceipt=&receipt;output=slots;return true;
}
bool OriginalStandardMaterialTable50LA::ownsUiSharedHistoryLease(const UiSharedHistoryOwner* owner,
    const UiSharedHistoryReceipt& receipt)const noexcept {
    // Deliberately independent of poison/live context: authentic cancellation
    // and retirement still have to release a lease after a failed callback.
    return storage_&&owner&&receipt.table_==this&&receipt.owner_==owner&&
        storage_->uiSharedHistoryOwner==owner&&storage_->uiSharedHistoryReceipt==&receipt&&
        receipt.leaseRevision_&&receipt.leaseRevision_==storage_->uiSharedHistoryLeaseRevision;
}
bool OriginalStandardMaterialTable50LA::tableOwnerUiSharedHistory(const UiSharedHistoryOwner* owner,
    const OriginalStandardMaterialContext50LA* context,const UiSharedHistoryReceipt& receipt)const noexcept {
    return ownsUiSharedHistoryLease(owner,receipt)&&context&&receipt.context_==context&&
        storage_->construction.get()==context&&context->live();
}
bool OriginalStandardMaterialTable50LA::bindingRevisionUiSharedHistory(const UiSharedHistoryOwner* owner,
    const UiSharedHistoryReceipt& receipt,std::uint64_t& output)const noexcept {
    if(!uiSharedHistoryCurrent(owner,receipt))return false;
    output=receipt.bindingRevision_;return true;
}
bool OriginalStandardMaterialTable50LA::uiSharedHistoryCurrent(const UiSharedHistoryOwner* owner,
    const UiSharedHistoryReceipt& receipt)const noexcept {
    if(!tableOwnerUiSharedHistory(owner,receipt.context_,receipt)||!uiSharedHistoryQuiescent()||
       receipt.bindingRevision_!=storage_->nativeBindingRevision)return false;
    for(std::size_t i=0;i<receipt.snapshots_.size();++i){
        OriginalStandardMaterialUiSharedHistorySlot50LA now;
        OriginalStandardMaterialPassView50LA pass;std::int32_t references=0;
        if(!captureUiSharedHistorySlot(i,now,pass,references)||
           references!=receipt.retainedReferences_[i]||!sameUiPassPointers(pass,receipt.passViews_[i])||
           !sameUiSlotBefore(now,receipt.snapshots_[i]))return false;
    }
    return true;
}
bool OriginalStandardMaterialTable50LA::commitUiSharedHistory(const UiSharedHistoryOwner* owner,
    UiSharedHistoryReceipt& receipt,const UiSharedHistorySlots& slots)noexcept {
    if(!uiSharedHistoryCurrent(owner,receipt)||
       storage_->nativeBindingRevision==std::numeric_limits<std::uint64_t>::max())return false;
    // Source ordering/accepted callback/history synchronization are the common
    // owner's responsibility. This primitive cannot create their authority.
    // Every check for all three slots precedes the first nonthrowing store.
    for(std::size_t i=0;i<slots.size();++i){
        const auto& slot=slots[i];const auto& before=receipt.snapshots_[i];
        if(!sameUiSlotBefore(slot,before)||slot.writeKind!=before.writeKind)return false;
        if(!slot.write){
            if(!equalUiContext(slot.afterContext,before.beforeContext)||
               !equalUiBindings(slot.afterNativeBindings,before.beforeNativeBindings))return false;
            continue;
        }
        if(!i){
            if(!equalUiContext(slot.afterContext,before.beforeContext)||
               !equalUiBindings(slot.afterNativeBindings,before.beforeNativeBindings)||
               slot.afterNativeBindings[0].bound()||slot.afterNativeBindings[1].bound())return false;
            continue;
        }
        const auto& image=slot.afterNativeBindings[0];
        if(!image.bound()||slot.afterNativeBindings[1].bound()||
           !slot.afterNativeBindings[1].equivalent(before.beforeNativeBindings[1]))return false;
        // Rejoin the SAME actual canonical map; no representative, redirect,
        // null-key animation, synthetic registration or FIFO pin is involved.
        OriginalStandardTextureRowView50LA textureRow;
        const auto token=(*receipt.passViews_[i].texture1A)[0];
        if(token<0||!storage_->textures->row(token,textureRow)||!textureRow.references08||
           *textureRow.references08<=0||!textureRow.object04)return false;
        const auto& texture=*textureRow.object04;
        const auto* flags=texture.flagsF8();const auto* known=texture.flagsF8KnownMask();
        if(!texture.constructed24DA30()||!texture.references00()||*texture.references00()<=0||
           !flags||!known||!(*known&1u)||!texture.originalBytes()||!texture.sourceMember()||
           !texture.sourceKind()||!texture.levelCountB0()||*texture.levelCountB0()<=0||
           image.pstOffset_||image.pstBytes_!=texture.originalBytes()->size()||image.kind_!=*texture.sourceKind())return false;
        const auto archive=i==1?OriginalUiArchiveId50KG::controllerPopup:OriginalUiArchiveId50KG::fonts;
        const char* name=i==1?"ui/textures/overlay1.pst":"font/venus16.pst";
        const auto* identity=originalUiArchiveIdentity50KG(archive);bool exactMember=false;
        if(!identity)return false;
        for(std::size_t member=0;member<identity->memberCount;++member){
            const auto* candidate=originalUiMemberIdentity50KG(archive,member);
            if(candidate==image.member_&&candidate&&sameUiSourceName(*texture.sourceMember(),candidate->name)&&
               sameUiSourceName(*texture.sourceMember(),name)&&candidate->bytes==image.pstBytes_)exactMember=true;
        }
        if(!exactMember)return false;
        bool exactLevel=false;
        for(std::int32_t mip=0;mip<*texture.levelCountB0();++mip){
            OriginalStandardTextureLevelView50LA level;
            if(!texture.level100(static_cast<std::size_t>(mip),level))return false;
            if(level.sourceMipOrdinal==image.storedMip_&&level.sourceHeaderOffset==image.header_&&
               level.sourceIndicesOffset==image.payload_&&level.width104>0&&level.height110>0&&
               static_cast<std::uint32_t>(level.width104)==image.width_&&
               static_cast<std::uint32_t>(level.height110)==image.height_)exactLevel=true;
        }
        if(!exactLevel)return false;
        auto expected=before.beforeContext;
        if(*flags&1u){
            using F=OriginalUiGsField50KG;
            expected.fields[static_cast<std::size_t>(F::sourceAlphaValueWhenAis0)]={255,OriginalStandardMaterialValueOrigin50LA::runtime,nullptr,0,0x24983Cu};
            expected.fields[static_cast<std::size_t>(F::sourceAlphaValueWhenAis1)]={255,OriginalStandardMaterialValueOrigin50LA::runtime,nullptr,0,0x24983Cu};
            expected.fields[static_cast<std::size_t>(F::methodOfExpandingTextureAlpha)]={1,OriginalStandardMaterialValueOrigin50LA::runtime,nullptr,0,0x24983Cu};
        }
        if(!equalUiContext(expected,slot.afterContext))return false;
    }
    // The objects retain their own original context metadata except the exact
    // source TEXA branch above. No allocation, dispatch, callback or fallible
    // lookup occurs from this point until the transaction is fully published.
    static_assert(std::is_nothrow_copy_assignable_v<OriginalStandardMaterialStateContext50LA>);
    static_assert(std::is_nothrow_copy_assignable_v<OriginalStandardMaterialNativeBinding50KG>);
    for(std::size_t i=0;i<slots.size();++i){const auto& slot=slots[i];if(!slot.write)continue;
        auto& object=storage_->rows[3228+i].object04;
        if(i)object->commitNativeBinding249610(0,0,slot.afterContext,slot.afterNativeBindings[0]);
        else object->commitNativeUntexturedState249BA0(0,slot.afterContext);
    }
    ++storage_->nativeBindingRevision;receipt.bindingRevision_=storage_->nativeBindingRevision;
    for(std::size_t i=0;i<slots.size();++i){
        auto& snapshot=receipt.snapshots_[i];
        snapshot.beforeContext=slots[i].afterContext;snapshot.afterContext=slots[i].afterContext;
        snapshot.beforeNativeBindings=slots[i].afterNativeBindings;snapshot.afterNativeBindings=slots[i].afterNativeBindings;
    }
    // Unlike an in-flight canonical FIFO, UI has no registration that can
    // remain current. Expire the comparison immediately, without rebuilding,
    // clearing/forging registrations or issuing any collector receipt.
    storage_->collectorSealed=false;storage_->collectorBindingDirty=true;return true;
}
bool OriginalStandardMaterialTable50LA::cancelUiSharedHistory(const UiSharedHistoryOwner* owner,
    UiSharedHistoryReceipt& receipt)noexcept {
    // Cancellation is lease retirement, never rollback of an accepted draw.
    return releaseUiSharedHistory(owner,receipt);
}
bool OriginalStandardMaterialTable50LA::releaseUiSharedHistory(const UiSharedHistoryOwner* owner,
    UiSharedHistoryReceipt& receipt)noexcept {
    if(!ownsUiSharedHistoryLease(owner,receipt))return false;
    storage_->uiSharedHistoryOwner=nullptr;storage_->uiSharedHistoryReceipt=nullptr;
    receipt.table_=nullptr;receipt.owner_=nullptr;receipt.context_=nullptr;
    receipt.leaseRevision_=0;receipt.bindingRevision_=0;
    receipt.snapshots_={};receipt.passViews_={};receipt.retainedReferences_={};return true;
}
bool OriginalStandardMaterialTable50LA::poisonUiSharedHistory(const UiSharedHistoryOwner* owner,
    const UiSharedHistoryReceipt& receipt)noexcept {
    if(!ownsUiSharedHistoryLease(owner,receipt))return false;
    storage_->nativeBindingPoisoned=true;storage_->collectorSealed=false;return true;
}
bool OriginalStandardMaterialTable50LA::demandTerminal() const noexcept {
    return storage_&&(storage_->demandTerminal||
        (storage_->textures&&storage_->textures->populationTerminal()));
}
const float* OriginalStandardMaterialTable50LA::demandScale4356E0() const noexcept {
    return storage_&&storage_->firstDemand24A47C?&storage_->demandScale4356E0:nullptr;
}
const std::uint32_t* OriginalStandardMaterialTable50LA::demandUvAssertion4356E8() const noexcept {
    return storage_&&storage_->firstTexture24A4EC?&storage_->demandUvAssertion4356E8:nullptr;
}
bool OriginalStandardMaterialTable50LA::demandSurface246810(
    const OriginalStandardModelResource50LA& resource,std::size_t lod,std::size_t ordinal,
    std::int32_t overrideRow,float projectedRadius,float scale,D& d) const noexcept {
    d={};d.slot=ordinal;
    if(!storage_)return fail(d,S::unprepared,0x246810);
    auto& s=*storage_;
    if(s.populationTerminal){d=s.terminalDiagnostic;return false;}
    if(s.demandTerminal){d=s.demandTerminalDiagnostic;return false;}
    if(s.textures&&s.textures->populationTerminal())return fail(d,S::demandTerminal,0x29E340);
    if(s.uiSharedHistoryOwner||s.populationActive||s.demandActive||s.collectorActive)return fail(d,S::phase,0x246810);
    const auto* scene=resource.scene();
    if(!resource.prepared()||!scene||lod>=scene->lods.size()||ordinal>=scene->lods[lod].surfaces.size())
        return fail(d,S::input,0x246810);
    const auto& surface=scene->lods[lod].surfaces[ordinal];
    if(overrideRow==-1){
        if(surface.material_index>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))
            return fail(d,S::range,0x246898);
        d.row=static_cast<std::int32_t>(surface.material_index);
    }else d.row=overrideRow;
    //246838/24689C reads the same full382B38 row, never a recipe projection.
    const OriginalStandardMaterialObject50LA* material=nullptr;D lookup;
    if(!resolve298E60(d.row,material,lookup))return fail(d,lookup.status,lookup.sourcePC);
    if(!material)return fail(d,S::missingMaterialObject,0x246884);
    //246844/2468A4 consumes entry0D. The current immutable decoder represents
    //exactly one source UV-size lane; reject broader lanes rather than pad.
    const auto count=static_cast<std::uint8_t>((surface.component_word>>8u)&255u);
    const float uvSize=static_cast<float>(surface.uv_size_uninterpreted);
    if(count!=1||!std::isfinite(uvSize)||static_cast<double>(uvSize)!=surface.uv_size_uninterpreted)
        return fail(d,S::input,0x246844);
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559,"source demand binary32");
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
    return fail(d,S::arithmetic,0x24A430);
#endif
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(projectedRadius)||!std::isfinite(scale))
        return fail(d,S::arithmetic,0x24A430);
    if(!s.construction||!s.construction->live())return fail(d,S::video,0x24A430);
    struct Guard {bool& active;explicit Guard(bool& a):active(a){active=true;}~Guard(){active=false;}} guard(s.demandActive);
    if(!s.firstDemand24A47C){
        s.demandScale4356E0=1.5f;s.firstDemand24A47C=true;d.sourceEffectsCommitted=true;
    }
    if(!material->demand24A430(&uvSize,count,projectedRadius,scale,s.demandScale4356E0,
        s.firstTexture24A4EC,s.demandUvAssertion4356E8,d)){
        // A failed source call may already have raised a shared texture demand.
        // Freeze this actual manager, not merely the model that observed it.
        if(d.sourceEffectsCommitted){s.demandTerminal=true;s.demandTerminalDiagnostic=d;}
        return false;
    }
    d.status=S::complete;return true;
}
const OriginalStandardMaterialObject50LA* OriginalStandardMaterialTable50LA::partialConstruction() const noexcept {
    return storage_?storage_->partial.get():nullptr;
}
bool OriginalStandardMaterialTable50LA::populate14D370(
    const OriginalStandardMaterialPopulationInput50LA& input,D& d) noexcept {
    d={};if(!storage_)return fail(d,S::unprepared,0x14D370);
    auto& s=*storage_;
    if(s.populationTerminal){d=s.terminalDiagnostic;return false;}
    if(s.uiSharedHistoryOwner||s.nativeFifoPins||s.populationActive||s.demandActive||s.collectorActive||demandTerminal())return fail(d,S::phase,0x14D370);
    if(s.populationBound&&(s.programs!=input.actualPrograms382B70||s.textures!=input.actualTextures383400||
       s.construction!=input.construction))return fail(d,S::phase,0x14D370);
    // Only the concrete native Video factory can mint this context. Verify
    // actual shared APPSTART and the EXACT associated template backing, not
    // equal-looking recipes or an independently reconstructed application.
    if(!input.construction||!input.construction->live())return fail(d,S::video,0x248060);
    const auto& application=input.construction->materialApplication();
    if(!application||application->appStart()!=s.appStart||&application->templates()!=s.templates.get())
        return fail(d,S::templates,0x24714C);
    struct Guard {bool& active;explicit Guard(bool& a):active(a){active=true;}~Guard(){active=false;}} guard(s.populationActive);
    const auto terminal=[&]() noexcept {s.populationTerminal=true;s.terminalDiagnostic=d;return false;};
    try {
        for(std::size_t i=0;i<s.rows.size();++i){
            ++d.populationVisited;auto& row=s.rows[i];
            //14D3A0..3B8: source scan order and actual numeric ref/object gates.
            if(row.references08<=0||row.object04)continue;
            d.row=static_cast<std::int32_t>(i);d.pass=0;
            if(!s.populationBound){
                s.programs=input.actualPrograms382B70;s.textures=input.actualTextures383400;
                s.construction=input.construction;s.populationBound=true;
            }
            s.partial=std::unique_ptr<OriginalStandardMaterialObject50LA>(new OriginalStandardMaterialObject50LA);
            d.sourceEffectsCommitted=true; //Actual native24A390/29B410 constructor owner.
            D lookup;std::int32_t canonical=-1;
            if(!find298C80(row.name00,canonical,lookup)){
                fail(d,lookup.status,lookup.sourcePC);return terminal();
            }
            //29B854 publishes by first-name match, then14D3FC by scan index.
            //Fresh26E640 and AIDamage299BE0 retain only first matches, so these
            //are the SAME row.
            //No API can create refs on a later duplicate. If that scope changes,
            //stop rather than invent cross-row aliases/double destruction.
            if(canonical!=static_cast<std::int32_t>(i)){
                fail(d,S::duplicatePublication,0x29B854);return terminal();
            }
            if(row.pauseBackdropMissingPsm&&row.pauseBackdropTextures!=input.actualTextures383400){
                fail(d,S::texture,0x247638);return terminal();
            }
            if(row.damageMissingPsm&&row.damageTextures!=input.actualTextures383400){
                fail(d,S::texture,0x247638);return terminal();
            }
            if(row.speedBoostMissingPsm&&row.speedBoostTextures!=input.actualTextures383400){
                fail(d,S::texture,0x247638);return terminal();
            }
            if(row.tournamentMissingPsm&&row.tournamentTextures!=input.actualTextures383400){
                fail(d,S::texture,0x247638);return terminal();
            }
            if(!s.partial->construct29B550(canonical,row.name00,s.templates,row.associated30,input,row.tournamentMissingPsm,row.speedBoostMissingPsm,row.damageMissingPsm,row.pauseBackdropMissingPsm,row.garageFallback,row.memoryCardPending,d))return terminal();
            if(!s.partial->constructed29B550()){
                fail(d,S::phase,0x29B854);return terminal();
            }
            if(!s.construction->live()){
                fail(d,S::video,0x29B854);return terminal();
            }
            s.invalidateCollector(); //A newly published pass set requires a complete rebuild.
            row.object04=std::move(s.partial); //29B854 real loaded owner publication.
            row.memoryCardPending.reset(); //Consumed capability; history is now object-owned.
            //14D3FC repeats298F80 with the identical object and row. It creates
            //no second ownership reference and does not rebuild collector14D440.
            ++d.populationPublished;
        }
        d.status=S::complete;return true;
    }catch(const std::bad_alloc&){fail(d,S::allocation,0x14D3C0);return terminal();}
    catch(...){fail(d,S::input,0x14D370);return terminal();}
}
bool OriginalStandardMaterialTable50LA::firstPending14D370(std::int32_t& index,
    const std::string*& name,D& d) const noexcept {
    d={};if(!storage_)return fail(d,S::unprepared,0x14D370);
    index=-1;name=nullptr;
    for(std::size_t i=0;i<storage_->rows.size();++i){const auto& r=storage_->rows[i];
        if(r.references08>0&&!r.object04){
            index=static_cast<std::int32_t>(i);name=&r.name00;d.row=index;return true;
        }
    }
    return true;
}
bool OriginalStandardMaterialTable50LA::retainDefault14D590(std::int32_t index,D& d) noexcept {
    return retain14D590(index,OriginalUiTemplate50KG::defaultMt,d);
}
bool OriginalStandardMaterialTable50LA::retainTournamentFallback14D590(
    std::int32_t index,std::shared_ptr<const OriginalUiArchive50KG> archive,
    std::shared_ptr<OriginalStandardTextureTable50LA> textures,D& d) noexcept {
    if(!storage_||index<0||static_cast<std::size_t>(index)>=storage_->rows.size()||!archive||
        !archive->loaded()||archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament))
        return fail(d,S::source,0x29B638);
    auto& row=storage_->rows[static_cast<std::size_t>(index)];
    if(row.pauseBackdropMissingPsm||row.damageMissingPsm||!textures||!textures->prepared()||(row.tournamentTextures&&row.tournamentTextures!=textures)||
        (row.tournamentMissingPsm&&row.tournamentMissingPsm!=archive)||
        (row.object04&&!row.tournamentMissingPsm))return fail(d,S::phase,0x29B638);
    if(!retainScreen14D590(index,d))return false;
    row.tournamentMissingPsm=std::move(archive);row.tournamentTextures=std::move(textures);return true;
}
bool OriginalStandardMaterialTable50LA::retainSpeedBoostFallback14D590(
    std::int32_t index,std::shared_ptr<const OriginalUiArchive50KG> archive,
    std::shared_ptr<OriginalStandardTextureTable50LA> textures,D& d) noexcept {
    if(!storage_||index<0||static_cast<std::size_t>(index)>=storage_->rows.size()||!archive||
        !archive->loaded()||archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
        return fail(d,S::source,0x29B638);
    auto& row=storage_->rows[static_cast<std::size_t>(index)];
    if(row.pauseBackdropMissingPsm||row.tournamentMissingPsm||row.damageMissingPsm||!textures||!textures->prepared()||
        (row.speedBoostTextures&&row.speedBoostTextures!=textures)||
        (row.object04&&!row.speedBoostMissingPsm))return fail(d,S::phase,0x29B638);
    // Serial host sessions may independently authenticate the SAME original
    // OVERLAYS. Preserve the first retained proof object (including while the
    // canonical row is loaded/pending); never replace its active provenance.
    if(row.speedBoostMissingPsm&&row.speedBoostMissingPsm!=archive){
        const auto& prior=*row.speedBoostMissingPsm;
        if(!prior.loaded()||prior.identity()!=archive->identity()||
            prior.memberCount()!=archive->identity()->memberCount||
            archive->memberCount()!=prior.memberCount())return fail(d,S::source,0x29B638);
        for(std::size_t i=0;i<prior.memberCount();++i){
            const auto oldMember=prior.member(i),newMember=archive->member(i);
            if(!oldMember.identity||oldMember.identity!=newMember.identity||
                !oldMember.data||!newMember.data||oldMember.size!=oldMember.identity->bytes||
                newMember.size!=oldMember.size||
                (oldMember.size&&std::memcmp(oldMember.data,newMember.data,oldMember.size)!=0))
                return fail(d,S::source,0x29B638);
        }
    }
    if(!retainScreen14D590(index,d))return false;
    if(!row.speedBoostMissingPsm)row.speedBoostMissingPsm=std::move(archive);
    if(!row.speedBoostTextures)row.speedBoostTextures=std::move(textures);
    return true;
}
bool OriginalStandardMaterialTable50LA::bindGarageScreenFallbackAfterRetain(std::int32_t index,
    std::shared_ptr<const OriginalGarageScreenFallback50KG> proof,D& d)noexcept {
    if(!storage_||storage_->uiSharedHistoryOwner||index<0||static_cast<std::size_t>(index)>=storage_->rows.size()||!proof||proof->row_!=index)
        return fail(d,S::source,0x29B638);
    auto& row=storage_->rows[static_cast<std::size_t>(index)];
    const auto rejected=[&](bool value,const char* guard)noexcept {
        if(value){std::fprintf(stderr,"original_garage_fallback_binding_failed guard=%s row=%d\n",guard,index);std::fflush(stderr);}
        return value;
    };
    if(rejected(row.references08<=0,"references")||rejected(!storage_->templates,"templates")||
       rejected(row.associated30!=storage_->templates->get(OriginalUiTemplate50KG::screenMt),"template_identity")||
       rejected(bool(row.tournamentMissingPsm),"tournament_owner")||rejected(bool(row.speedBoostMissingPsm),"speedboost_owner")||
       rejected(bool(row.damageMissingPsm),"damage_owner")||rejected(bool(row.pauseBackdropMissingPsm),"pause_owner")||
       rejected(bool(row.memoryCardPending),"memorycard_pending")||
       rejected(row.object04&&!row.garageFallback,"object_without_garage_proof")||
       rejected(!proof->current(row.name00+".psm",proof->context_,proof->textures_),"new_proof_current"))
        return fail(d,S::phase,0x29B638);
    //Numeric104960 retain already happened at2465B0. This is native source
    //admission only and MUST NOT add a second source reference.
    if(row.garageFallback){
        if(rejected(!row.garageFallback->current(row.name00+".psm",proof->context_,proof->textures_),"retained_proof_current"))
            return fail(d,S::source,0x29B638);
    }else row.garageFallback=std::move(proof);
    return true;
}
bool OriginalStandardMaterialTable50LA::retainDamageFallback14D590(
    std::int32_t index,std::shared_ptr<const OriginalUiArchive50KG> archive,
    std::shared_ptr<OriginalStandardTextureTable50LA> textures,D& d) noexcept {
    if(!storage_||index<0||static_cast<std::size_t>(index)>=storage_->rows.size()||!archive||
        !archive->loaded()||archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
        return fail(d,S::source,0x29B638);
    auto& row=storage_->rows[static_cast<std::size_t>(index)];
    if(row.pauseBackdropMissingPsm||row.tournamentMissingPsm||row.speedBoostMissingPsm||!textures||!textures->prepared()||
        (row.damageTextures&&row.damageTextures!=textures)||
        (row.object04&&!row.damageMissingPsm))return fail(d,S::phase,0x29B638);
    // Serial host sessions may independently authenticate the SAME original
    // OVERLAYS. Preserve the first retained proof object (including while the
    // canonical row is loaded/pending); never replace its active provenance.
    if(row.damageMissingPsm&&row.damageMissingPsm!=archive){
        const auto& prior=*row.damageMissingPsm;
        if(!prior.loaded()||prior.identity()!=archive->identity()||
            prior.memberCount()!=archive->identity()->memberCount||
            archive->memberCount()!=prior.memberCount())return fail(d,S::source,0x29B638);
        for(std::size_t i=0;i<prior.memberCount();++i){
            const auto oldMember=prior.member(i),newMember=archive->member(i);
            if(!oldMember.identity||oldMember.identity!=newMember.identity||
                !oldMember.data||!newMember.data||oldMember.size!=oldMember.identity->bytes||
                newMember.size!=oldMember.size||
                (oldMember.size&&std::memcmp(oldMember.data,newMember.data,oldMember.size)!=0))
                return fail(d,S::source,0x29B638);
        }
    }
    if(!retainScreen14D590(index,d))return false;
    if(!row.damageMissingPsm)row.damageMissingPsm=std::move(archive);
    if(!row.damageTextures)row.damageTextures=std::move(textures);
    return true;
}
bool OriginalStandardMaterialTable50LA::retainPauseBackdropFallback14D590(
    std::int32_t index,std::shared_ptr<const OriginalUiArchive50KG> archive,
    std::shared_ptr<OriginalStandardTextureTable50LA> textures,D& d) noexcept {
    if(!storage_||index<0||static_cast<std::size_t>(index)>=storage_->rows.size()||!archive||
        !archive->loaded()||archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
        return fail(d,S::source,0x29B638);
    auto& row=storage_->rows[static_cast<std::size_t>(index)];
    if(row.tournamentMissingPsm||row.speedBoostMissingPsm||row.damageMissingPsm||!textures||!textures->prepared()||
        (row.pauseBackdropTextures&&row.pauseBackdropTextures!=textures)||
        (row.object04&&!row.pauseBackdropMissingPsm))return fail(d,S::phase,0x29B638);
    if(row.name00!="ui\\textures\\results1"&&row.name00!="ui\\textures\\results2")
        return fail(d,S::source,0x29B638);
    // Serial host sessions may independently authenticate the SAME original
    // OVERLAYS. Preserve the first retained proof object (including while the
    // canonical row is loaded/pending); never replace its active provenance.
    if(row.pauseBackdropMissingPsm&&row.pauseBackdropMissingPsm!=archive){
        const auto& prior=*row.pauseBackdropMissingPsm;
        if(!prior.loaded()||prior.identity()!=archive->identity()||
            prior.memberCount()!=archive->identity()->memberCount||
            archive->memberCount()!=prior.memberCount())return fail(d,S::source,0x29B638);
        for(std::size_t i=0;i<prior.memberCount();++i){
            const auto oldMember=prior.member(i),newMember=archive->member(i);
            if(!oldMember.identity||oldMember.identity!=newMember.identity||
                !oldMember.data||!newMember.data||oldMember.size!=oldMember.identity->bytes||
                newMember.size!=oldMember.size||
                (oldMember.size&&std::memcmp(oldMember.data,newMember.data,oldMember.size)!=0))
                return fail(d,S::source,0x29B638);
        }
    }
    if(!retainScreen14D590(index,d))return false;
    if(!row.pauseBackdropMissingPsm)row.pauseBackdropMissingPsm=std::move(archive);
    if(!row.pauseBackdropTextures)row.pauseBackdropTextures=std::move(textures);
    return true;
}
bool OriginalStandardMaterialTable50LA::bindMemoryCardPendingAfterRetain(
    const rr::platform::OriginalUiMemoryCardConstructor50KG& owner,
    std::shared_ptr<const OriginalMemoryCardScreenFallback50KG> proof,D& d)noexcept {
    d={};std::int32_t index=-1;
    if(!storage_||storage_->uiSharedHistoryOwner||storage_->nativeFifoPins||storage_->nativeBindingPoisoned||
       storage_->populationTerminal||storage_->populationActive||storage_->demandActive||storage_->collectorActive||demandTerminal()||
       !proof||!owner.iconOccurrenceCurrent(proof->roleIndex_,*this,index)||proof->table_!=this||proof->row_!=index||
       proof->issuer_.lock().get()!=&owner||!proof->occurrenceCurrent())return fail(d,S::phase,0x29B638);
    if(index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return fail(d,S::range,0x29B638);
    auto& row=storage_->rows[static_cast<std::size_t>(index)];d.row=index;
    if(row.references08<=0||row.object04||!storage_->templates||
       row.associated30!=storage_->templates->get(OriginalUiTemplate50KG::screenMt)||
       !OriginalStandardArchiveProvider50LA::sameName(row.name00,proof->name_)||
       row.garageFallback||row.tournamentMissingPsm||row.speedBoostMissingPsm||row.damageMissingPsm||row.pauseBackdropMissingPsm)
        return fail(d,S::source,0x29B638);
    if(row.memoryCardPending){
        if(row.memoryCardPending->issuer_.lock().get()!=&owner||!row.memoryCardPending->occurrenceCurrent()||
           row.memoryCardPending->roleIndex_!=proof->roleIndex_||row.memoryCardPending->row_!=index||
           row.memoryCardPending->table_!=this||row.memoryCardPending->primitive_!=proof->primitive_)
            return fail(d,S::phase,0x29B638);
    }else row.memoryCardPending=std::move(proof);
    return true; //No source reference or population effect.
}
bool OriginalStandardMaterialTable50LA::clearMemoryCardPendingForRetirement(
    const rr::platform::OriginalUiMemoryCardConstructor50KG& owner,D& d)noexcept {
    d={};std::array<std::int32_t,4> indices{{-1,-1,-1,-1}};
    if(!storage_||!canRetireNativeCollector29C300()||!owner.retainedLoadRoles_||owner.retainedLoadRoles_>7)
        return fail(d,S::phase,0x14D500);
    // Validate EVERY actual pending capability before removing any. No source
    // ref/packet is released here, and no allocation/callback follows validation.
    for(std::size_t role=0;role<owner.retainedLoadRoles_&&role<4;++role){auto& index=indices[role];
        if(!owner.retainedRoleRetirementIdentity(role,*this,index))return fail(d,S::phase,0x14D500);
        if(index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return fail(d,S::range,0x14D500);
        const auto& row=storage_->rows[static_cast<std::size_t>(index)];
        if(!OriginalStandardArchiveProvider50LA::sameName(row.name00,owner.loadRoles_[role].qualifiedName))
            return fail(d,S::source,0x14D500);
        if(row.memoryCardPending&&(row.memoryCardPending->issuer_.lock().get()!=&owner||
           row.memoryCardPending->roleIndex_!=role||row.memoryCardPending->table_!=this||
           row.memoryCardPending->row_!=index||row.memoryCardPending->primitive_!=
                owner.quads_[owner.loadRoles_[role].constructionIndex].material54))return fail(d,S::phase,0x14D500);
    }
    for(std::size_t role=4;role<owner.retainedLoadRoles_;++role){std::int32_t strip=-1;
        if(!owner.retainedRoleRetirementIdentity(role,*this,strip)||strip<0||
           static_cast<std::size_t>(strip)>=storage_->rows.size())return fail(d,S::phase,0x14D500);
        const auto& row=storage_->rows[static_cast<std::size_t>(strip)];
        if(row.memoryCardPending||!OriginalStandardArchiveProvider50LA::sameName(row.name00,owner.loadRoles_[role].qualifiedName))
            return fail(d,S::source,0x14D500);
    }
    for(std::size_t role=0;role<owner.retainedLoadRoles_&&role<4;++role)
        storage_->rows[static_cast<std::size_t>(indices[role])].memoryCardPending.reset();
    return true;
}
bool OriginalStandardMaterialTable50LA::preflightMemoryCardStripBinding(
    const rr::platform::OriginalUiMemoryCardConstructor50KG& owner,D& d,std::size_t role)const noexcept {
    d={};std::int32_t index=-1;
    if(role<4||role>6||!storage_||storage_->uiSharedHistoryOwner||storage_->nativeFifoPins||storage_->nativeBindingPoisoned||
       storage_->populationTerminal||storage_->populationActive||storage_->demandActive||storage_->collectorActive||demandTerminal()||
       !owner.stripOccurrenceCurrent(*this,index,role))return fail(d,S::phase,0x2DF144);
    if(index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return fail(d,S::range,0x298E60);
    for(std::size_t role=0;role<4;++role)
        if(!owner.completedIconStateCurrent(role)||!preflightMemoryCardIconBinding(owner,role,d))return false;
    for(std::size_t prior=4;prior<role;++prior){std::int32_t preceding=-1;
        if(!owner.completedStripStateCurrent(prior)||!owner.stripOccurrenceCurrent(*this,preceding,prior)||preceding!=index)
            return fail(d,S::phase,0x2DF144);
        if(!preflightMemoryCardStripBinding(owner,d,prior))return false;
    }
    const auto& row=storage_->rows[static_cast<std::size_t>(index)];d.row=index;
    if(row.references08<=0||!row.object04||row.memoryCardPending||row.garageFallback||
       !storage_->templates||!storage_->populationBound||storage_->construction!=owner.context_||
       storage_->programs!=owner.domain_->programs382B70()||storage_->textures!=owner.domain_->textures383400()||
       row.associated30!=storage_->templates->get(OriginalUiTemplate50KG::screenMt)||
       !OriginalStandardArchiveProvider50LA::sameName(row.name00,owner.loadRoles_[role].qualifiedName))
        return fail(d,S::reference,0x2DF144);
    return row.object04->preflightCapturedOverlayBinding(index,owner.context_,owner.domain_->textures383400(),
        storage_->programs,owner.domain_->archiveProvider3E1A70(),owner.domain_->resolver3E1A70(),d);
}
bool OriginalStandardMaterialTable50LA::preflightMemoryCardFirstIconBinding(
    const rr::platform::OriginalUiMemoryCardConstructor50KG& occurrence,D& d)const noexcept {
    return preflightMemoryCardIconBinding(occurrence,0,d);
}
bool OriginalStandardMaterialTable50LA::preflightMemoryCardIconBinding(
    const rr::platform::OriginalUiMemoryCardConstructor50KG& occurrence,std::size_t role,D& d)const noexcept {
    d={};std::int32_t index=-1;
    if(!storage_||storage_->uiSharedHistoryOwner||storage_->nativeFifoPins||storage_->nativeBindingPoisoned||
       storage_->populationTerminal||storage_->populationActive||storage_->demandActive||storage_->collectorActive||demandTerminal()||
       !occurrence.iconOccurrenceCurrent(role,*this,index))return fail(d,S::phase,0x2DF158);
    if(index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return fail(d,S::range,0x298E60);
    const auto& row=storage_->rows[static_cast<std::size_t>(index)];d.row=index;
    // Current actual occurrence is MemoryCard; the object's captured first
    // construction family remains unchanged and is validated separately.
    if(row.references08<=0||!row.object04||!storage_->templates||!storage_->populationBound||
       storage_->construction!=occurrence.context_||storage_->programs!=occurrence.domain_->programs382B70()||
       storage_->textures!=occurrence.domain_->textures383400()||
       row.associated30!=storage_->templates->get(OriginalUiTemplate50KG::screenMt)||
       !OriginalStandardArchiveProvider50LA::sameName(row.name00,occurrence.loadRoles_[role].qualifiedName))
        return fail(d,S::reference,0x2DF158);
    return row.object04->preflightCapturedSharedIconBinding(index,occurrence.context_,
        occurrence.domain_->textures383400(),storage_->programs,occurrence.domain_->archiveProvider3E1A70(),
        occurrence.domain_->resolver3E1A70(),d);
}
bool OriginalStandardMaterialTable50LA::preflightMemoryCardPopulation(
    const rr::platform::OriginalUiMemoryCardConstructor50KG& owner,
    bool& firstPending,bool& noTexturePending,D& d,std::size_t role)const noexcept {
    d={};firstPending=noTexturePending=false;std::int32_t first=-1;
    if(!storage_||storage_->uiSharedHistoryOwner||storage_->nativeFifoPins||storage_->nativeBindingPoisoned||
       storage_->populationTerminal||storage_->populationActive||storage_->demandActive||storage_->collectorActive||demandTerminal()||
       !owner.iconOccurrenceCurrent(role,*this,first)||!owner.optionsSource_||!storage_->templates||
       !owner.domain_->textures383400()||owner.domain_->textures383400()->populationTerminal()||
       first<0||static_cast<std::size_t>(first)>=storage_->rows.size())return fail(d,S::phase,0x2DF144);
    for(std::size_t prior=0;prior<role;++prior)
        if(!owner.completedIconStateCurrent(prior)||!preflightMemoryCardIconBinding(owner,prior,d))return false;
    if(!owner.domain_->textures383400()->preflightPopulationOwners(owner.domain_->resolver3E1A70(),
        owner.context_,owner.domain_->archiveProvider3E1A70(),d.texture))return fail(d,S::texture,d.texture.sourcePC);
    const auto& actual=storage_->rows[static_cast<std::size_t>(first)];
    if(actual.references08<=0||actual.associated30!=storage_->templates->get(OriginalUiTemplate50KG::screenMt)||
       !OriginalStandardArchiveProvider50LA::sameName(actual.name00,owner.loadRoles_[role].qualifiedName))return fail(d,S::reference,0x2DF144);
    if(storage_->populationBound&&(storage_->construction!=owner.context_||
       storage_->programs!=owner.domain_->programs382B70()||storage_->textures!=owner.domain_->textures383400()))
        return fail(d,S::phase,0x2DF144);
    // Enumerate the WHOLE actual pending material set. These bounds reject
    // unsupported obligations before admission; they do not shorten14D370.
    for(std::size_t i=0;i<storage_->rows.size();++i){const auto& row=storage_->rows[i];
        if(row.references08<=0||row.object04)continue;d.row=static_cast<std::int32_t>(i);
        if(row.associated30!=storage_->templates->get(OriginalUiTemplate50KG::screenMt)||
           row.garageFallback||row.tournamentMissingPsm||row.speedBoostMissingPsm||row.damageMissingPsm||row.pauseBackdropMissingPsm)
            return fail(d,S::source,0x29B638);
        if(static_cast<std::int32_t>(i)==first){
            if(row.memoryCardPending&&(!row.memoryCardPending->occurrenceCurrent()||
               row.memoryCardPending->issuer_.lock().get()!=&owner||row.memoryCardPending->roleIndex_!=role||
               row.memoryCardPending->row_!=first||row.memoryCardPending->table_!=this))return fail(d,S::source,0x29B638);
            firstPending=true;
        }else if(OriginalStandardArchiveProvider50LA::sameName(row.name00,"ui\\textures\\NoTexture")&&!row.memoryCardPending){
            if(noTexturePending)return fail(d,S::duplicatePublication,0x29B854);
            noTexturePending=true;
        }else return fail(d,S::source,0x29B628);
    }
    if(actual.object04&&!preflightMemoryCardIconBinding(owner,role,d))return false;
    const auto textures=owner.domain_->textures383400();
    for(std::size_t i=0;i<textures->rowCount();++i){
        if(i>static_cast<std::size_t>(std::numeric_limits<std::int16_t>::max()))return fail(d,S::range,0x29E340);
        OriginalStandardTextureRowView50LA row;
        if(!textures->row(static_cast<std::int16_t>(i),row)||!row.references08||!row.name00)return fail(d,S::texture,0x29E340);
        if(*row.references08<=0||row.object04)continue;
        // A loaded icon with unresolved texture requires a different material-only
        // receipt join, not v29's completed-resource preflight. Keep it closed.
        if(!firstPending||!OriginalStandardArchiveProvider50LA::sameName(*row.name00,owner.loadRoles_[role].qualifiedName))
            return fail(d,S::texture,0x29E340);
    }
    d.sourcePC=0x2DF144;return true;
}
bool OriginalStandardMaterialTable50LA::preflightMemoryCardStripPopulation(
    const rr::platform::OriginalUiMemoryCardConstructor50KG& owner,
    bool& firstPending,bool& noTexturePending,D& d,std::size_t role)const noexcept {
    d={};firstPending=noTexturePending=false;std::int32_t first=-1;
    if(role<4||role>6||!storage_||storage_->uiSharedHistoryOwner||storage_->nativeFifoPins||storage_->nativeBindingPoisoned||
       storage_->populationTerminal||storage_->populationActive||storage_->demandActive||storage_->collectorActive||demandTerminal()||
       !owner.stripOccurrenceCurrent(*this,first,role)||!owner.optionsSource_||!storage_->templates||
       !owner.domain_->textures383400()||owner.domain_->textures383400()->populationTerminal()||
       first<0||static_cast<std::size_t>(first)>=storage_->rows.size())return fail(d,S::phase,0x2DF144);
    for(std::size_t prior=0;prior<4;++prior)
        if(!owner.completedIconStateCurrent(prior)||!preflightMemoryCardIconBinding(owner,prior,d))return false;
    if(!owner.domain_->textures383400()->preflightPopulationOwners(owner.domain_->resolver3E1A70(),
        owner.context_,owner.domain_->archiveProvider3E1A70(),d.texture))return fail(d,S::texture,d.texture.sourcePC);
    const auto& actual=storage_->rows[static_cast<std::size_t>(first)];
    if(actual.references08<=0||actual.memoryCardPending||actual.associated30!=storage_->templates->get(OriginalUiTemplate50KG::screenMt)||
       !OriginalStandardArchiveProvider50LA::sameName(actual.name00,owner.loadRoles_[role].qualifiedName))return fail(d,S::reference,0x2DF144);
    // Later references must reuse the actual first strip's completed object;
    // never reconstruct missing history or admit another positive source.
    if(role>4){
        if(!actual.object04)return fail(d,S::source,0x29B628);
        if(!preflightMemoryCardStripBinding(owner,d,role))return false;
    }
    if(storage_->populationBound&&(storage_->construction!=owner.context_||
       storage_->programs!=owner.domain_->programs382B70()||storage_->textures!=owner.domain_->textures383400()))
        return fail(d,S::phase,0x2DF144);
    // Enumerate the WHOLE actual pending material set. These bounds reject
    // unsupported obligations before admission; they do not shorten14D370.
    for(std::size_t i=0;i<storage_->rows.size();++i){const auto& row=storage_->rows[i];
        if(row.references08<=0||row.object04)continue;d.row=static_cast<std::int32_t>(i);
        if(row.associated30!=storage_->templates->get(OriginalUiTemplate50KG::screenMt)||
           row.garageFallback||row.tournamentMissingPsm||row.speedBoostMissingPsm||row.damageMissingPsm||row.pauseBackdropMissingPsm)
            return fail(d,S::source,0x29B628);
        if(row.memoryCardPending)return fail(d,S::source,0x29B628);
        if(static_cast<std::int32_t>(i)==first){
            firstPending=true; // Only the exact positive authored-role row.
        }else if(OriginalStandardArchiveProvider50LA::sameName(row.name00,"ui\\textures\\NoTexture")&&!row.memoryCardPending){
            if(noTexturePending)return fail(d,S::duplicatePublication,0x29B854);
            noTexturePending=true;
        }else return fail(d,S::source,0x29B628);
    }
    if(actual.object04&&!preflightMemoryCardStripBinding(owner,d,role))return false;
    const auto textures=owner.domain_->textures383400();
    for(std::size_t i=0;i<textures->rowCount();++i){
        if(i>static_cast<std::size_t>(std::numeric_limits<std::int16_t>::max()))return fail(d,S::range,0x29E340);
        OriginalStandardTextureRowView50LA row;
        if(!textures->row(static_cast<std::int16_t>(i),row)||!row.references08||!row.name00)return fail(d,S::texture,0x29E340);
        if(*row.references08<=0||row.object04)continue;
        // A loaded authored material with unresolved texture requires a material-only
        // receipt join, not the existing complete-resource join. Keep it closed.
        if(!firstPending||!OriginalStandardArchiveProvider50LA::sameName(*row.name00,"ui\\textures\\overlay1"))
            return fail(d,S::texture,0x29E340);
    }
    d.sourcePC=0x2DF144;return true;
}
bool OriginalStandardMaterialTable50LA::preflightGarageLoadedSharedIcon(
    const rr::platform::OriginalUiGarageQuad50KG& occurrence,D& d)const noexcept {
    d={};using Quad=rr::platform::OriginalUiGarageQuad50KG;
    const auto& q=occurrence;const auto* p=q.material54;
    if(!storage_||storage_->uiSharedHistoryOwner||storage_->nativeFifoPins||storage_->nativeBindingPoisoned||
       storage_->populationTerminal||storage_->populationActive||storage_->demandActive||storage_->collectorActive||demandTerminal()||
       q.memoryCardPrefixOnly_||!q.constructed||q.stage_!=Quad::Stage::materialRetained||
       !q.domain_||!q.domain_->prepared()||q.domain_->materials382B38().get()!=this||
       !q.context_||!q.context_->live()||!q.video_||!q.garage_||!q.garage_->loaded()||
       q.garage_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garage)||
       (q.offset!=0x2A0&&q.offset!=0x300&&q.offset!=0x360&&q.offset!=0x3C0)||
       !p||p!=q.primitiveOwner_.get()||!p->materialStored||!p->materialRetained||p->type0C!=15||
       (p->flags0F&1u)||p->domain!=q.domain_||p->context!=q.context_||p->packet10||q.resource58||q.packetOwner_||
       q.nativeGarageFifoPins_||q.nativeMenuWarningPins_)
        return fail(d,S::phase,0x2DF144);
    const auto index=p->material00;d.row=index;
    if(index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return fail(d,S::range,0x298E60);
    const auto& row=storage_->rows[static_cast<std::size_t>(index)];
    const char* expected=q.offset==0x2A0?"ui\\textures\\icon_x":q.offset==0x300?"ui\\textures\\icon_tri":
        q.offset==0x360?"ui\\textures\\icon_sqr":"ui\\textures\\icon_cir";
    if(row.references08<=0||!row.object04||row.memoryCardPending||!storage_->templates||!storage_->populationBound||
       storage_->construction!=q.context_||storage_->programs!=q.domain_->programs382B70()||
       storage_->textures!=q.domain_->textures383400()||
       row.associated30!=storage_->templates->get(OriginalUiTemplate50KG::screenMt)||
       !OriginalStandardArchiveProvider50LA::sameName(row.name00,expected))return fail(d,S::reference,0x2DF144);
    // No proof is minted/stored for this occurrence. The object retains its
    // own first family, and the real global population call still follows.
    return row.object04->preflightCapturedSharedIconBinding(index,q.context_,q.domain_->textures383400(),
        storage_->programs,q.domain_->archiveProvider3E1A70(),q.domain_->resolver3E1A70(),d);
}
bool OriginalStandardMaterialTable50LA::preflightRetain14D590(std::int32_t index,D& d)const noexcept {
    d.row=index;if(!storage_)return fail(d,S::unprepared,0x14D590);
    if(storage_->uiSharedHistoryOwner||storage_->nativeFifoPins||storage_->populationTerminal||storage_->populationActive||storage_->demandActive||
       storage_->collectorActive||demandTerminal())
        return fail(d,S::phase,0x14D590);
    if(index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return fail(d,S::range,0x298A30);
    const auto& r=storage_->rows[static_cast<std::size_t>(index)];
    if(r.references08==std::numeric_limits<std::int32_t>::max())return fail(d,S::overflow,0x298AE8);
    return true;
}
bool OriginalStandardMaterialTable50LA::retainScreen14D590(std::int32_t index,D& d) noexcept {
    return retain14D590(index,OriginalUiTemplate50KG::screenMt,d);
}
bool OriginalStandardMaterialTable50LA::retain14D590(std::int32_t index,
    OriginalUiTemplate50KG associated,D& d) noexcept {
    if(!preflightRetain14D590(index,d))return false;
    auto& r=storage_->rows[static_cast<std::size_t>(index)];
    ++r.references08; //298AE8/EC, before the unconditional associated30 store.
    r.associated30=storage_->templates->get(associated); //14D5C0, even if already loaded.
    return true;
}
bool OriginalStandardMaterialTable50LA::release14D500(std::int32_t index) noexcept {
    if(!storage_||index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return false;
    auto& r=storage_->rows[static_cast<std::size_t>(index)];
    if(r.references08<=0||storage_->uiSharedHistoryOwner||storage_->nativeFifoPins||storage_->populationActive||storage_->demandActive||storage_->collectorActive)return false;
    --r.references08; //298C64/68, before testing the result at14D534.
    if(r.references08==0&&r.object04){
        // Source nodes borrow pass pointers. Never preserve a usable seal or
        // dangling internal node across14D55C..578's real pass/texture cleanup.
        storage_->invalidateCollector();
        r.object04.reset();
    }
    if(r.references08==0){
        r.memoryCardPending.reset();
        // Native construction-family admission cannot govern a later fresh
        // null-row lifetime. Loaded-object history was destroyed above.
        r.garageFallback.reset();
    }
    //Zero + pending null still takes14D54C's genuine no-destroy branch.
    //14D500 does NOT clear associated30, even on the final release.
    return true;
}
struct OriginalStandardMaterialReferences50LA::Storage {
    std::shared_ptr<OriginalStandardMaterialTable50LA> table;
    std::uint32_t count1C0=0;
    std::vector<std::int32_t> handles;
    std::size_t stored=0,retained=0;
    bool complete=false,released=false,allocationFreed=false;
};
OriginalStandardMaterialReferences50LA::OriginalStandardMaterialReferences50LA() noexcept=default;
OriginalStandardMaterialReferences50LA::~OriginalStandardMaterialReferences50LA(){
    D diagnostic;
    if(!release26E4C0(diagnostic))std::terminate();
}
bool OriginalStandardMaterialReferences50LA::acquire26E640(const OriginalStandardModelResource50LA& resource,
    std::shared_ptr<OriginalStandardMaterialTable50LA> table,const OriginalStandardMaterialPath50LA& path,D& d) noexcept {
    d={};if(storage_)return fail(d,S::alreadyPrepared,0x26E640);
    const auto* scene=resource.scene();
    if(!resource.prepared()||!scene||scene->materials.empty()||
       scene->materials.size()>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())||
       !table||!table->prepared()||!path.actualPrefix36B970||
       !ascii(*path.actualPrefix36B970,255,true))return fail(d,S::input,0x26E640);
    if(table->populationTerminal())return fail(d,S::phase,0x14D370);
    if(table->demandTerminal())return fail(d,S::phase,0x24A430);
    try {
        auto next=std::make_unique<Storage>();next->table=std::move(table);
        next->count1C0=static_cast<std::uint32_t>(scene->materials.size());
        next->handles.resize(next->count1C0); //Backing only; unwritten slots are never exposed.
        storage_=std::move(next);d.sourceEffectsCommitted=true; //26E670,26E684.
        auto& s=*storage_;
        for(std::size_t i=0;i<scene->materials.size();++i){
            d.slot=i;const auto& name=scene->materials[i];
            if(!ascii(name,63,false))return fail(d,S::name,0x26E6A4);
            std::string requested=*path.actualPrefix36B970;
            if(!requested.empty()&&requested.back()!='\\')requested+='\\';
            requested+=name;
            if(requested.size()>255)return fail(d,S::name,0x26E70C); //Source256-byte scratch admission.
            D lookup;std::int32_t index=-1;
            if(!s.table->find298C80(requested,index,lookup)){
                d.row=lookup.row;return fail(d,lookup.status,lookup.sourcePC);
            }
            s.handles[i]=index;++s.stored;d.storedSlots=s.stored;d.row=index; //26E7C8.
            if(!s.table->retainDefault14D590(index,d))return false;
            ++s.retained;d.retainedSlots=s.retained;
        }
        s.complete=true;return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x26E67C);}
    catch(...){return fail(d,S::input,0x26E640);}
}
bool OriginalStandardMaterialReferences50LA::clone26EB44(
    const OriginalStandardMaterialReferences50LA& prototype,D& d) noexcept {
    d={};if(storage_)return fail(d,S::alreadyPrepared,0x26EB44);
    if(!prototype.storage_||!prototype.storage_->complete||prototype.storage_->released)
        return fail(d,S::input,0x26EB44);
    const auto& source=*prototype.storage_;
    if(!source.table||!source.table->prepared()||source.stored!=source.count1C0||
       source.retained!=source.count1C0||source.handles.size()!=source.count1C0)
        return fail(d,S::input,0x26EB44);
    if(source.table->populationTerminal()||source.table->demandTerminal())
        return fail(d,S::phase,0x26EB44);
    try {
        auto next=std::make_unique<Storage>();next->table=source.table;
        next->count1C0=source.count1C0;next->handles=source.handles;
        next->stored=next->handles.size();
        storage_=std::move(next);d.sourceEffectsCommitted=true;
        auto& s=*storage_;d.storedSlots=s.stored; //26EB44/54/64.
        for(std::size_t i=0;i<s.stored;++i){
            d.slot=i;d.row=s.handles[i];
            if(!s.table->retainDefault14D590(s.handles[i],d))return false;
            ++s.retained;d.retainedSlots=s.retained; //26EB90.
        }
        s.complete=true;return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x26EB4C);}
    catch(...){return fail(d,S::input,0x26EB44);}
}
bool OriginalStandardMaterialReferences50LA::canRelease26E4C0() const noexcept {
    if(!storage_||storage_->released)return true;
    const auto& s=*storage_;
    if(!s.table||!s.table->storage_||s.retained>s.handles.size())return false;
    const auto& table=*s.table->storage_;
    if(table.uiSharedHistoryOwner||table.populationActive||table.demandActive||table.collectorActive)return false;
    for(std::size_t i=0;i<s.retained;++i){
        const auto row=s.handles[i];
        if(row<0||static_cast<std::size_t>(row)>=table.rows.size())return false;
        std::size_t required=0;
        for(std::size_t j=0;j<s.retained;++j)if(s.handles[j]==row)++required;
        if(table.rows[static_cast<std::size_t>(row)].references08<0||
           static_cast<std::size_t>(table.rows[static_cast<std::size_t>(row)].references08)<required)
            return false;
    }
    return true;
}
bool OriginalStandardMaterialReferences50LA::release26E4C0(D& d) noexcept {
    d={};if(!storage_||storage_->released)return true;
    if(!canRelease26E4C0())return fail(d,S::phase,0x26E4C0);
    auto& s=*storage_;
    //Original slot order, including repeated handles. The preflight above
    //means this serial loop cannot fail after an earlier row was released.
    for(std::size_t i=0;i<s.retained;++i){
        d.slot=i;d.row=s.handles[i];
        if(!s.table->release14D500(s.handles[i]))std::terminate();
        d.sourceEffectsCommitted=true;
    }
    s.retained=0;s.released=true;return true;
}
bool OriginalStandardMaterialReferences50LA::released26E4C0() const noexcept {
    return storage_&&storage_->released;
}
bool OriginalStandardMaterialReferences50LA::freeAllocation26E5EC() noexcept {
    if(!storage_||!storage_->released)return false;
    if(storage_->allocationFreed)return true;
    std::vector<std::int32_t>().swap(storage_->handles);
    storage_->allocationFreed=true;return true;
}
bool OriginalStandardMaterialReferences50LA::started() const noexcept {return bool(storage_);}
bool OriginalStandardMaterialReferences50LA::complete() const noexcept {return storage_&&storage_->complete&&!storage_->released;}
const std::uint32_t* OriginalStandardMaterialReferences50LA::count1C0() const noexcept {return storage_?&storage_->count1C0:nullptr;}
const std::int32_t* OriginalStandardMaterialReferences50LA::handle1C4(std::size_t ordinal) const noexcept {
    return storage_&&!storage_->allocationFreed&&ordinal<storage_->stored?&storage_->handles[ordinal]:nullptr;
}
const std::int32_t* OriginalStandardMaterialReferences50LA::allocationBase1C4() const noexcept {
    return storage_&&!storage_->allocationFreed?storage_->handles.data():nullptr;
}
const OriginalStandardMaterialTable50LA* OriginalStandardMaterialReferences50LA::table382B38() const noexcept {
    return storage_?storage_->table.get():nullptr;
}
}
