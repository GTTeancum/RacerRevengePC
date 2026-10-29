#include "original_standard_texture_table50la.h"
#include "original_standard_texture_resource50la.h"
#include "original_loading_pst_file50la.h"
#include "original_standard_material_context50la.h"
#include <exception>
#include <limits>
#include <utility>
#include <vector>

namespace rr::assets {
namespace {
using E=OriginalStandardTextureStatus50LA;
using D=OriginalStandardTextureDiagnostic50LA;
bool fail(D& d,E status,std::uint32_t pc,std::int32_t row=-1) noexcept {
    d.status=status;d.sourcePC=pc;d.row=row;return false;
}
unsigned char fold(unsigned char c) noexcept {
    return c>='A'&&c<='Z'?static_cast<unsigned char>(c+('a'-'A')):c;
}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)
        if(fold(static_cast<unsigned char>(a[i]))!=fold(static_cast<unsigned char>(b[i])))return false;
    return true;
}
bool cString(const std::string& name) noexcept {
    return name.find('\0')==std::string::npos;
}
}
struct OriginalStandardTextureTable50LA::Storage {
    struct Row {
        std::string name00;
        std::shared_ptr<OriginalStandardTextureResource50LA> object04;
        std::int32_t references08=0;
    };
    std::vector<Row> rows;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> construction;
    std::shared_ptr<OriginalStandardTextureResource50LA> partial;
    // Native pending load-attempt exists before original Resource allocation.
    std::shared_ptr<OriginalLoadingPstFile50LA> loadingPstPending;
    D terminalDiagnostic{};
    bool populationBound=false,populationActive=false,populationTerminal=false;
};
OriginalStandardTextureTable50LA::OriginalStandardTextureTable50LA() noexcept=default;
OriginalStandardTextureTable50LA::~OriginalStandardTextureTable50LA() {
    // Exact pending source files require explicit fallible native abort first.
    if(storage_&&storage_->loadingPstPending)std::terminate();
    if(storage_)for(const auto& row:storage_->rows)
        if(row.references08!=0||row.object04)std::terminate();
    if(storage_&&storage_->partial){
        if(!storage_->partial->release104970())std::terminate();
        storage_->partial.reset();
    }
}
bool OriginalStandardTextureTable50LA::prepareFresh(D& d) noexcept {
    d={};
    if(storage_)return fail(d,E::alreadyPrepared,0x3caff0);
    try {
        // 299150..174 establishes actual zero count/capacity and null buffers.
        storage_=std::make_unique<Storage>();return true;
    }catch(...){return fail(d,E::allocation,0x299130);}
}
bool OriginalStandardTextureTable50LA::prepared() const noexcept {return bool(storage_);}
std::size_t OriginalStandardTextureTable50LA::rowCount() const noexcept {
    return storage_?storage_->rows.size():0;
}
bool OriginalStandardTextureTable50LA::row(std::int16_t index,
    OriginalStandardTextureRowView50LA& out) const noexcept {
    out={};
    if(!storage_||index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return false;
    const auto& source=storage_->rows[static_cast<std::size_t>(index)];
    out.name00=&source.name00;out.object04=source.object04.get();
    out.references08=&source.references08;return true;
}
bool OriginalStandardTextureTable50LA::find(const std::string& name,std::int16_t& index) const noexcept {
    index=-1;
    if(!storage_||!cString(name))return false;
    for(std::size_t i=0;i<storage_->rows.size();++i)if(same(name,storage_->rows[i].name00)){
        index=static_cast<std::int16_t>(i);return true;
    }
    return false;
}
bool OriginalStandardTextureTable50LA::bindMaterialContext(
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context,D& d) noexcept {
    if(!storage_)return fail(d,E::unprepared,0x2472F0);
    auto& s=*storage_;
    if(s.populationActive||s.populationTerminal)return fail(d,E::phase,0x2472F0);
    if(!context||!context->live())return fail(d,E::video,0x2472F0);
    if(s.construction&&s.construction!=context)return fail(d,E::phase,0x2472F0);
    if(!s.construction)s.construction=std::move(context);
    return true;
}
bool OriginalStandardTextureTable50LA::acquire299330AndRetain298A30(
    const std::string& name,std::int16_t& index,D& d) noexcept {
    d={};index=-1;
    if(!storage_)return fail(d,E::unprepared,0x299330);
    if(storage_->populationActive||storage_->populationTerminal)return fail(d,E::phase,0x299330);
    if(!cString(name))return fail(d,E::input,0x299330);
    std::int16_t found=-1;
    if(find(name,found)){
        auto& row=storage_->rows[static_cast<std::size_t>(found)];
        if(row.references08<0)return fail(d,E::reference,0x298a30,found);
        if(row.references08==std::numeric_limits<std::int32_t>::max())
            return fail(d,E::overflow,0x298ae8,found);
        ++row.references08;index=found;return true;
    }
    // 2472F8 narrows the row to16 bits; 247308 sign-extends that token.
    const auto next=storage_->rows.size();
    if(next>static_cast<std::size_t>(std::numeric_limits<std::int16_t>::max()))
        return fail(d,E::range,0x2472f8);
    try {
        Storage::Row row;
        row.name00=name;
        // Source298900 appends null object/zero refs;299330 owns its name.
        // 247304 immediately takes this pass's one actual298A30 reference.
        row.references08=1;
        storage_->rows.push_back(std::move(row));
        index=static_cast<std::int16_t>(next);return true;
    }catch(...){return fail(d,E::allocation,0x2993bc);}
}
// Loading Font stores its actual408 between299330 and298A30. Keep that
// source interval distinct from the existing native combined helper.
bool OriginalStandardTextureTable50LA::lookupLoading299330(const std::string& name,std::int16_t& index,D& d)noexcept {
    d={};index=-1;
    if(!storage_||storage_->populationActive||storage_->populationTerminal||!cString(name))return fail(d,E::phase,0x299330);
    if(find(name,index))return true;
    const auto next=storage_->rows.size();
    if(next>static_cast<std::size_t>(std::numeric_limits<std::int16_t>::max()))return fail(d,E::range,0x299330);
    try{Storage::Row row;row.name00=name;storage_->rows.push_back(std::move(row));
        index=static_cast<std::int16_t>(next);return true;
    }catch(...){return fail(d,E::allocation,0x2993BC);}
}
bool OriginalStandardTextureTable50LA::retainLoading298A30(std::int16_t index,D& d)noexcept {
    d={};if(!storage_||storage_->populationActive||storage_->populationTerminal||index<0||
       static_cast<std::size_t>(index)>=storage_->rows.size())return fail(d,E::phase,0x298A30,index);
    auto& row=storage_->rows[static_cast<std::size_t>(index)];
    if(row.references08<0)return fail(d,E::reference,0x298A30,index);
    if(row.references08==std::numeric_limits<std::int32_t>::max())return fail(d,E::overflow,0x298AE8,index);
    ++row.references08;return true;
}
bool OriginalStandardTextureTable50LA::abortLoadingPstNative(
    const std::shared_ptr<OriginalLoadingFontSource50LA>& root,D& d)noexcept {
    d={};if(!storage_||!storage_->loadingPstPending)return true;
    auto& s=*storage_;auto& p=*s.loadingPstPending;
    if(!root||s.populationActive||!s.populationTerminal||!s.resolver||p.root_!=root)
        return fail(d,E::phase,0);
    if(s.partial){
        if(s.partial->references00_!=1)return fail(d,E::reference,0);
        if(s.partial->loadingPstFile_){
            if(s.partial->loadingPstFile_!=s.loadingPstPending||!s.partial->preflightLoadingPstNative())
                return fail(d,E::phase,0);
        }else if(p.boundResource_||s.partial->registered_||s.partial->resolver_||s.partial->construction_)
            return fail(d,E::phase,0);
    }else if(p.boundResource_)return fail(d,E::phase,0);
    // Every reference/backing refusal is checked before closing the exact file.
    // Closing this native partial never certifies24E880/24E0A4 completion.
    if(!s.resolver->closeLoadingPst131040(p,true,d))return false;
    if(s.partial){
        if(s.partial->loadingPstFile_&&!s.partial->closeLoadingPstNative(d))return false;
        if(!s.partial->release104970())return fail(d,E::reference,0);
        s.partial.reset();
    }
    s.loadingPstPending.reset();d={};
    return true; // Terminal diagnostic/history intentionally remain sticky.
}
bool OriginalStandardTextureTable50LA::release29E400(std::int16_t index) noexcept {
    if(!storage_||index<0||static_cast<std::size_t>(index)>=storage_->rows.size())return false;
    auto& row=storage_->rows[static_cast<std::size_t>(index)];
    if(storage_->populationActive||row.references08<=0)return false;
    --row.references08;
    if(row.references08==0&&row.object04){
        if(!row.object04->release104970())return false;
        row.object04.reset(); //29E450 then29E460; aliases retain their own refs.
    }
    //Null resource skips29E450. Zero refs do not erase this row or its name.
    return true;
}
bool OriginalStandardTextureTable50LA::resolve298E60(std::int16_t index,
    OriginalStandardTextureResource50LA*& out,D& d) const noexcept {
    d={};out=nullptr;d.row=index;
    if(!storage_)return fail(d,E::unprepared,0x298E60,index);
    if(index<0||static_cast<std::size_t>(index)>=storage_->rows.size())
        return fail(d,E::range,0x298E60,index);
    if(storage_->populationActive||storage_->populationTerminal)
        return fail(d,E::phase,0x298E60,index);
    out=storage_->rows[static_cast<std::size_t>(index)].object04.get();return true;
}
bool OriginalStandardTextureTable50LA::populationTerminal() const noexcept {
    return storage_&&storage_->populationTerminal;
}
const OriginalStandardTextureResource50LA* OriginalStandardTextureTable50LA::partialConstruction() const noexcept {
    return storage_?storage_->partial.get():nullptr;
}
bool OriginalStandardTextureTable50LA::preflightPopulationOwners(
    const std::shared_ptr<OriginalStandardTextureResolver50LA>& resolver,
    const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
    const std::shared_ptr<const OriginalStandardArchiveProvider50LA>& provider,D& d)const noexcept {
    d={};if(!storage_||storage_->populationActive||storage_->populationTerminal||storage_->partial)
        return fail(d,E::phase,0x29E340);
    if(!resolver||!context||!context->live())return fail(d,E::video,0x29C768);
    if((storage_->construction&&storage_->construction!=context)||
       (storage_->populationBound&&(storage_->resolver!=resolver||storage_->construction!=context)))
        return fail(d,E::phase,0x29E340);
    return resolver->preflightPopulationOwners(context,provider,d);
}
bool OriginalStandardTextureTable50LA::populate29E340(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<const OriginalStandardMaterialContext50LA> construction,D& d) noexcept {
    d={};
    if(!storage_)return fail(d,E::unprepared,0x29E340);
    auto& s=*storage_;
    if(s.populationTerminal){d=s.terminalDiagnostic;return false;}
    if(s.populationActive)return fail(d,E::phase,0x29E340);
    if(!resolver)return fail(d,E::source,0x24DAF8);
    if(!construction||!construction->live())return fail(d,E::video,0x29C768);
    if(s.construction&&s.construction!=construction)return fail(d,E::phase,0x29E340);
    if(s.populationBound&&(s.resolver!=resolver||s.construction!=construction))
        return fail(d,E::phase,0x29E340);
    // The source table scan with actual count zero performs no texture load,
    // so it never reaches24DAF8's resolver lookup. Keep the corpus unprepared
    // and unbound; a later genuine positive-row scan retains all normal gates.
    if(s.rows.empty()){d.status=E::complete;return true;}
    if(!resolver->prepared())return fail(d,E::source,0x24DAF8);
    if(!resolver->bind(construction,d))return false;
    if(!s.populationBound){
        s.resolver=std::move(resolver);s.construction=std::move(construction);s.populationBound=true;
    }
    struct Guard {bool& active;explicit Guard(bool& a):active(a){active=true;}~Guard(){active=false;}} guard(s.populationActive);
    const auto terminal=[&]() noexcept {s.populationTerminal=true;s.terminalDiagnostic=d;return false;};
    try {
        for(std::size_t i=0;i<s.rows.size();++i){
            ++d.populationVisited;auto& row=s.rows[i];
            if(row.references08<=0||row.object04)continue;
            d.row=static_cast<std::int32_t>(i);
            std::size_t source=0;std::string resolvedName;
            OriginalStandardTextureResolver50LA::UiSelection selected;
            if(!s.resolver->resolve24DADC(row.name00,source,resolvedName,d,&selected))return terminal();
            std::shared_ptr<OriginalStandardTextureResource50LA> cached;
            const bool loadingPst=bool(s.resolver->loadingPstFontEntry_);
            if(loadingPst){
                if(!s.resolver->captureLoadingPst132820(resolvedName,source,s.loadingPstPending,d))return terminal();
                if(s.loadingPstPending->hitReturned_)cached=s.loadingPstPending->hit_.lock();
            }else if(!s.resolver->cached132820(source,cached,d))return terminal();
            if(cached){
                // Preserve the resource's actual first-source receipt. This
                // later lookup grants neither replacement nor backfilling.
                if(!s.construction->live()){fail(d,E::video,0x24DCE4,d.row);return terminal();}
                //29E388 supplies retainCached=1; this is a RESOURCE reference,
                //independent of the row's one-reference-per-material-map count.
                if(!cached->retain104960(d))return terminal();
                row.object04=std::move(cached);d.sourceEffectsCommitted=true;
            }else {
                //A control-block allocation failure before native construction
                //is cleaned without pretending to have registered/published.
                s.partial=OriginalStandardTextureResource50LA::allocate();
                d.sourceEffectsCommitted=true;
                if(loadingPst){
                    if(!s.resolver->bindLoadingPstResource(*s.loadingPstPending,*s.partial,d))return terminal();
                    s.partial->loadingPstFile_=s.loadingPstPending;
                }
                if(!s.partial->construct24DA30(s.resolver,source,resolvedName,s.construction,d,static_cast<std::size_t>(-1),&selected))return terminal();
                if(loadingPst){
                    if(!s.resolver->associateLoadingPst1326A0(*s.loadingPstPending,s.partial,d)||
                       !s.resolver->closeLoadingPst131040(*s.loadingPstPending,false,d))return terminal();
                }else if(!s.resolver->cache1326A0(source,s.partial)){
                    fail(d,E::phase,0x1326A0,d.row);return terminal();
                }
                row.object04=std::move(s.partial); //29E3C0 only after actual load.
            }
            if(loadingPst)s.loadingPstPending.reset(); // Actual row now owns resource; no pending handle survives.
            ++d.populationPublished;
        }
        d.status=E::complete;return true;
    }catch(...){fail(d,E::allocation,0x24DEE8,d.row);return terminal();}
}
}
