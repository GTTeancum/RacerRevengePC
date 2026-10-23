#include "original_ui_rider_material_lifetime50kg.h"
#include "../assets/original_ui_garage_scene_assets50kg.h"
#include "../assets/original_ui_garage_watto_assets50kg.h"
#include <cstring>
#include <limits>
#include <utility>

namespace rr::game {
namespace {
using D=OriginalUiRiderMaterialLifetimeDiagnostic50KG;
using E=OriginalUiRiderMaterialLifetimeError50KG;
using Kind=OriginalUiRiderMaterialOwnerKind50KG;
using Event=OriginalUiRiderMaterialEvent50KG;
using EventKind=OriginalUiRiderMaterialEventKind50KG;
constexpr std::uint32_t tableRows=3235;
constexpr std::size_t tableBytes=70962;
constexpr auto maximum=std::numeric_limits<std::uint64_t>::max();
bool fail(D& d,E e,std::uint32_t pc=0) noexcept {d.error=e;d.sourcePC=pc;return false;}
char fold(char c) noexcept{return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool sameName(const std::string& a,const char* b) noexcept {
    std::size_t i=0;for(;i<a.size()&&b[i];++i)if(fold(a[i])!=fold(b[i]))return false;
    return i==a.size()&&!b[i];
}
const assets::OriginalUiMemberIdentity50KG* member(assets::OriginalUiArchiveId50KG archive,const char* name) noexcept {
    const auto* a=assets::originalUiArchiveIdentity50KG(archive);if(!a)return nullptr;
    for(std::size_t i=0;i<a->memberCount;++i){const auto* m=assets::originalUiMemberIdentity50KG(archive,i);
        if(m&&std::strcmp(m->name,name)==0)return m;}
    return nullptr;
}
bool riderMember(const assets::OriginalUiMemberIdentity50KG* identity) noexcept {
    const auto* archive=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider);
    if(!identity||!archive)return false;
    for(std::size_t i=0;i<archive->memberCount;++i)
        if(identity==assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::rider,i))return true;
    return false;
}
bool table(const assets::OriginalUiArchive50KG& app,assets::OriginalUiMemberView50KG& out) noexcept {
    if(!app.loaded()||app.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::appStart))return false;
    const auto* identity=member(assets::OriginalUiArchiveId50KG::appStart,"AppStart\\Mat_Stbl.txt");
    if(!identity)return false;
    for(std::size_t i=0;i<app.memberCount();++i){const auto v=app.member(i);
        if(v.identity==identity){if(!v.data||v.size!=tableBytes)return false;out=v;return true;}}
    return false;
}
bool basename(const std::string& name) noexcept {
    if(name.empty()||name.size()>127||name=="."||name=="..")return false;
    for(unsigned char c:name)if(c<32||c>=127||c=='/'||c=='\\'||c==':')return false;
    return true;
}
}
bool OriginalUiRiderMaterialRow50KG::same(const OriginalUiRiderMaterialRow50KG& other)const noexcept {
    return prepared()&&other.prepared()&&table_==other.table_&&rank_==other.rank_&&offset_==other.offset_;
}
bool OriginalUiRiderMaterialRow50KG::prepare(const assets::OriginalUiArchive50KG& app,
    const std::string& name,OriginalUiRiderMaterialRow50KG& out,D& d) noexcept {
    d={};assets::OriginalUiMemberView50KG source{};
    if(!table(app,source))return fail(d,E::archive,0x298724);
    if(name.empty()||name.size()>255)return fail(d,E::row,0x298C80);
    OriginalUiRiderMaterialRow50KG next;bool found=false;
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    std::size_t cursor=0;std::uint32_t rank=0;
    while(cursor<source.size){
        while(cursor<source.size&&delimiter(source.data[cursor]))++cursor;
        if(cursor==source.size)break;
        const auto start=cursor;while(cursor<source.size&&!delimiter(source.data[cursor]))++cursor;
        if(name.size()==cursor-start){std::size_t j=0;
            for(;j<name.size()&&fold(name[j])==fold(static_cast<char>(source.data[start+j]));++j){}
            if(j==name.size()){
                if(found){
                    // Original298C80 returns the first matching token. Admit
                    // only the two exact authenticated later occurrences;
                    // neither replaces its first row nor creates an owner.
                    const bool window=sameName(name,"ui/textures/pod_wndw")&&next.rank_==2490&&
                        rank==3221&&start==70650;
                    const bool mars=sameName(name,"ui/textures/08c_3")&&next.rank_==2386&&
                        next.offset_==55255&&rank==2387&&start==55274;
                    if(!window&&!mars)return fail(d,E::table,0x298C80);
                }else{next.table_=source.identity;next.rank_=rank;next.offset_=static_cast<std::uint32_t>(start);found=true;}
            }
        }
        if(rank>=tableRows)return fail(d,E::table,0x298724);++rank;
    }
    if(rank!=tableRows)return fail(d,E::table,0x298724);
    if(!found)return fail(d,E::row,0x298C80);
    out=next;return true;
}
bool OriginalUiRiderMaterialFootprint50KG::model(const assets::OriginalUiArchive50KG& app,
    const assets::Scene48D& scene,D& d) noexcept {
    if(scene.materials.empty()||scene.materials.size()>keys_.size())return fail(d,E::footprint,0x26E640);
    try {
        for(const auto& name:scene.materials){
            if(!basename(name))return fail(d,E::footprint,0x26E640);
            OriginalUiRiderMaterialRow50KG key;
            if(!OriginalUiRiderMaterialRow50KG::prepare(app,"ui/textures/"+name,key,d))return false;
            std::size_t i=0;for(;i<rows_;++i)if(keys_[i].same(key))break;
            if(i==rows_){keys_[rows_]=key;++rows_;}
            ++occurrences_[i];++slots_;
        }
        return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderMaterialFootprint50KG::garageStatic(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiGarageSceneAssets50KG& a,std::size_t i,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto* m=a.staticModel(i);
    if(!a.loaded()||!m||m->ordinal!=i+1||!m->model||!m->model->scene()||!m->model->source.identity)
        return fail(d,E::footprint,0x26E640);
    OriginalUiRiderMaterialFootprint50KG n;if(!n.model(app,*m->model->scene(),d))return false;
    n.kind_=Kind::garageStaticModel;n.garageOrdinal_=static_cast<std::uint16_t>(i+1);
    n.source_=m->model->source.identity;out=n;return true;
}
bool OriginalUiRiderMaterialFootprint50KG::garageActor(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiGarageSceneAssets50KG& a,std::size_t i,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto* m=a.actor(i);
    if(!a.loaded()||!m||m->ordinal!=i+1||!m->model||!m->model->scene()||!m->mcf.identity())
        return fail(d,E::footprint,0x1538B0);
    OriginalUiRiderMaterialFootprint50KG n;if(!n.model(app,*m->model->scene(),d))return false;
    n.kind_=Kind::garageActorModel;n.garageOrdinal_=static_cast<std::uint16_t>(i+1);
    n.source_=m->mcf.identity();out=n;return true;
}
bool OriginalUiRiderMaterialFootprint50KG::garageSelected(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiGarageSceneAssets50KG& a,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto* m=a.selectedPod();
    if(!a.loaded()||!m||!m->model||!m->model->scene()||!m->model->source.identity)
        return fail(d,E::footprint,0x1D1CD0);
    OriginalUiRiderMaterialFootprint50KG n;if(!n.model(app,*m->model->scene(),d))return false;
    n.kind_=Kind::garageSelectedModel;n.source_=m->model->source.identity;out=n;return true;
}
bool OriginalUiRiderMaterialFootprint50KG::garageWatto(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiGarageWattoAssets50KG& a,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};if(!a.loaded()||!a.scene()||!a.geometrySource())return fail(d,E::footprint,0x1D4430);
    OriginalUiRiderMaterialFootprint50KG n;if(!n.model(app,*a.scene(),d))return false;
    n.kind_=Kind::garageWattoModel;n.widget_=10;n.source_=a.geometrySource();out=n;return true;
}
bool OriginalUiRiderMaterialFootprint50KG::pod(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiRiderPodAssets50KG& asset,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto id=asset.widgetId();
    const auto* widget=member(assets::OriginalUiArchiveId50KG::rider,"UI\\UIRider\\Widgets.Dat");
    // The immutable asset owner authenticated the widget's actual SLT reference.
    if(!widget||!asset.loaded()||!asset.scene()||!id||id>=120||
       asset.widgetSource()!=widget||!riderMember(asset.modelSource().identity))return fail(d,E::footprint,0x26E640);
    OriginalUiRiderMaterialFootprint50KG next;
    if(!next.model(app,*asset.scene(),d))return false;
    next.kind_=Kind::podGraph;next.widget_=id;next.source_=asset.modelSource().identity;out=next;return true;
}
bool OriginalUiRiderMaterialFootprint50KG::tournamentPod(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiRiderPodAssets50KG& asset,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto id=asset.widgetId();const auto archive=asset.archive();
    const auto* widget=member(assets::OriginalUiArchiveId50KG::tournament,"UI\\UITourn\\Widgets.Dat");
    if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::tournament)||
       !widget||!asset.loaded()||!asset.scene()||id<39||id>61||asset.widgetSource()!=widget)
        return fail(d,E::footprint,0x26E640);
    bool modelMember=false;
    for(std::size_t i=0;i<archive->memberCount();++i)if(archive->member(i).identity==asset.modelSource().identity)modelMember=true;
    if(!modelMember||!asset.modelSource().identity)return fail(d,E::footprint,0x26E640);
    OriginalUiRiderMaterialFootprint50KG next;if(!next.model(app,*asset.scene(),d))return false;
    next.kind_=Kind::podGraph;next.widget_=id;next.source_=asset.modelSource().identity;out=next;return true;
}
bool OriginalUiRiderMaterialFootprint50KG::tournamentBinder(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiRiderBinderAssets50KG& asset,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto* pass=asset.pass();
    const auto* psm=member(assets::OriginalUiArchiveId50KG::tournament,"ui\\textures\\binder.psm");
    const auto* atm=member(assets::OriginalUiArchiveId50KG::tournament,"ui\\textures\\binder.atm");
    if(!psm||!atm||!asset.loaded()||!pass||pass->identity!=psm||pass->pass!=1||asset.atmMember().identity!=atm)
        return fail(d,E::footprint,0x150450);
    try {OriginalUiRiderMaterialFootprint50KG next;
        if(!OriginalUiRiderMaterialRow50KG::prepare(app,"ui/textures/binder",next.keys_[0],d))return false;
        next.kind_=Kind::binderHelper;next.source_=pass->identity;next.rows_=1;next.slots_=1;next.occurrences_[0]=1;
        out=next;return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderMaterialFootprint50KG::character(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiRiderCharacterAssets50KG& asset,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto id=asset.widgetId();
    const auto* widget=member(assets::OriginalUiArchiveId50KG::rider,"UI\\UIRider\\Widgets.Dat");
    // The immutable asset owner authenticated the widget's actual MCF reference.
    if(!widget||!asset.loaded()||!asset.scene()||!id||id>=120||!asset.baseName()||
       asset.baseName()->empty()||asset.widgetSource()!=widget||!riderMember(asset.mcfSource()))
        return fail(d,E::footprint,0x1538B0);
    OriginalUiRiderMaterialFootprint50KG next;
    if(!next.model(app,*asset.scene(),d))return false;
    next.kind_=Kind::characterGraph;next.widget_=id;next.source_=asset.mcfSource();out=next;return true;
}
bool OriginalUiRiderMaterialFootprint50KG::binder(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiRiderBinderAssets50KG& asset,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto* pass=asset.pass();
    const auto* psm=member(assets::OriginalUiArchiveId50KG::rider,"ui\\textures\\binder.psm");
    const auto* atm=member(assets::OriginalUiArchiveId50KG::rider,"ui\\textures\\binder.atm");
    if(!psm||!atm||!asset.loaded()||!pass||pass->identity!=psm||pass->pass!=1||asset.atmMember().identity!=atm)
        return fail(d,E::footprint,0x150450);
    try {
        OriginalUiRiderMaterialFootprint50KG next;
        if(!OriginalUiRiderMaterialRow50KG::prepare(app,"ui/textures/binder",next.keys_[0],d))return false;
        next.kind_=Kind::binderHelper;next.source_=pass->identity;next.rows_=1;next.slots_=1;next.occurrences_[0]=1;
        out=next;return true;
    }catch(...){return fail(d,E::allocation);}
}
struct OriginalUiRiderMaterialLifetime50KG::Storage {
    struct Row {OriginalUiRiderMaterialRow50KG key{};std::uint32_t owners=0;std::uint64_t epoch=0;};
    struct Owner {
        bool active=false;std::uint64_t serial=0;std::size_t count=0;
        std::array<std::uint16_t,originalUiRiderMaterialRows50KG> rows{};
        std::array<std::uint32_t,originalUiRiderMaterialRows50KG> occurrences{};
    };
    struct Acquire {
        std::size_t owner=0,added=0,count=0;
        std::array<std::uint16_t,originalUiRiderMaterialRows50KG> rows{};
    };
    struct Population {
        enum class Phase : std::uint8_t {idle,scanning,awaitingGroup};
        Phase phase=Phase::idle;
        std::size_t count=0,next=0,reserved=0;
        std::uint64_t firstSequence=0;
        std::array<std::uint16_t,originalUiRiderMaterialRows50KG> rows{};
    };
    std::shared_ptr<const assets::OriginalUiArchive50KG> app;
    const assets::OriginalUiMemberIdentity50KG* table=nullptr;
    std::array<Row,originalUiRiderMaterialRows50KG> rows{};
    std::array<Owner,originalUiRiderMaterialOwners50KG> owners{};
    std::array<Event,originalUiRiderMaterialEvents50KG> journal{};
    std::size_t rowCount=0,head=0,eventCount=0,cleanupReserved=0;
    std::uint64_t sequence=0,ownerSerial=0,epochSerial=0,acknowledged=0;
    Population population{};
    bool aborted=false;
    bool expected(std::uint64_t prior,D& d)const noexcept {
        return prior==sequence||fail(d,E::staleSequence);
    }
    bool populationActive()const noexcept {return population.phase!=Population::Phase::idle;}
    // Reserve every future cleanup event in both bounded spaces. Acknowledging
    // journal entries frees slots but does not restore consumed sequence values.
    bool room(std::size_t count,std::size_t reservedAfter,D& d)const noexcept {
        if(eventCount>journal.size())return fail(d,E::invariant);
        const auto slots=journal.size()-eventCount;
        if(count>slots||reservedAfter>slots-count)return fail(d,E::journalCapacity);
        const auto sequences=maximum-sequence;
        return (count<=sequences&&reservedAfter<=sequences-count)||fail(d,E::overflow);
    }
    void append(Event e) noexcept {
        e.sequence=++sequence;journal[(head+eventCount)%journal.size()]=e;++eventCount;
    }
    bool acquire(const OriginalUiRiderMaterialFootprint50KG& f,std::uint64_t prior,Acquire& plan,D& d)const noexcept {
        if(!expected(prior,d))return false;
        if(aborted)return fail(d,E::aborted);
        if(populationActive())return fail(d,E::populationActive);
        if(!f.prepared()||!f.source()||!f.rowCount()||f.rowCount()>rows.size()||!f.materialSlotCount()||
           f.materialSlotCount()>rows.size())return fail(d,E::footprint);
        plan={};plan.count=f.rowCount();plan.owner=owners.size();
        for(std::size_t i=0;i<owners.size();++i)if(!owners[i].active){plan.owner=i;break;}
        if(plan.owner==owners.size())return fail(d,E::ownerCapacity);
        if(ownerSerial==maximum)return fail(d,E::overflow);
        std::uint32_t slots=0;
        for(std::size_t i=0;i<f.rowCount();++i){const auto* key=f.row(i);const auto occurrence=f.slotOccurrences(i);
            if(!key||!key->prepared()||key->tableIdentity()!=table||!occurrence||occurrence>f.materialSlotCount()-slots)
                return fail(d,E::footprint);
            slots+=occurrence;d.row=key->rank();
            for(std::size_t j=0;j<i;++j)if(key->same(*f.row(j)))return fail(d,E::footprint);
            std::size_t row=0;for(;row<rowCount;++row)if(rows[row].key.same(*key))break;
            if(row==rowCount){
                if(rowCount+plan.added==rows.size())return fail(d,E::rowCapacity);
                row=rowCount+plan.added++;
            }else if(rows[row].owners==std::numeric_limits<std::uint32_t>::max())return fail(d,E::overflow);
            plan.rows[i]=static_cast<std::uint16_t>(row);
        }
        if(slots!=f.materialSlotCount())return fail(d,E::footprint);
        return room(plan.count,cleanupReserved+2*plan.count,d);
    }
    bool release(std::size_t owner,std::uint64_t prior,std::size_t& events,D& d)const noexcept {
        if(!expected(prior,d))return false;const auto& token=owners[owner];
        if(populationActive())return fail(d,E::populationActive);
        if(!token.active||!token.count||token.count>rows.size())return fail(d,E::invariant);
        if(cleanupReserved<2*token.count)return fail(d,E::invariant);
        events=token.count;
        for(std::size_t i=0;i<token.count;++i){const auto row=token.rows[i];
            if(row>=rowCount||!rows[row].owners)return fail(d,E::invariant);
            if(rows[row].owners==1&&rows[row].epoch)++events;
        }
        // This domain spends only its own already-guaranteed reservation.
        // An aborted population cannot take away any other owner's cleanup.
        return room(events,cleanupReserved-2*token.count,d);
    }
    bool populate(std::uint64_t prior,Population& plan,D& d)const noexcept {
        if(!expected(prior,d))return false;
        if(aborted)return fail(d,E::aborted);
        if(populationActive())return fail(d,E::populationActive);
        plan={};
        for(std::size_t i=0;i<rowCount;++i){
            if(!rows[i].owners&&rows[i].epoch)return fail(d,E::invariant);
            if(rows[i].owners&&!rows[i].epoch)plan.rows[plan.count++]=static_cast<std::uint16_t>(i);
        }
        if(plan.count>maximum-epochSerial)return fail(d,E::overflow);
        // Lock capacity for each exact row plus distinct14D370 and14D440
        // boundaries. The caller constructs nothing until this succeeds.
        if(!room(0,cleanupReserved+plan.count+2,d))return false;
        // Bounded allocation-free table-order population, independent of
        // native footprint/owner acquisition order.
        for(std::size_t i=1;i<plan.count;++i){const auto row=plan.rows[i];std::size_t j=i;
            while(j&&rows[plan.rows[j-1]].key.rank()>rows[row].key.rank()){
                plan.rows[j]=plan.rows[j-1];--j;
            }
            plan.rows[j]=row;
        }
        plan.phase=Population::Phase::scanning;plan.reserved=plan.count+2;
        plan.firstSequence=sequence+1;
        return true;
    }
};
OriginalUiRiderMaterialLifetime50KG::OriginalUiRiderMaterialLifetime50KG() noexcept=default;
OriginalUiRiderMaterialLifetime50KG::~OriginalUiRiderMaterialLifetime50KG()=default;
bool OriginalUiRiderMaterialLifetime50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> app,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyBound);assets::OriginalUiMemberView50KG source{};
    if(!app||!table(*app,source))return fail(d,E::archive,0x298724);
    try {auto next=std::make_unique<Storage>();next->app=std::move(app);next->table=source.identity;
        storage_=std::move(next);return true;
    }catch(...){return fail(d,E::allocation);}
}
std::uint64_t OriginalUiRiderMaterialLifetime50KG::sequence()const noexcept{return storage_?storage_->sequence:0;}
std::uint64_t OriginalUiRiderMaterialLifetime50KG::acknowledgedSequence()const noexcept{return storage_?storage_->acknowledged:0;}
std::size_t OriginalUiRiderMaterialLifetime50KG::pendingEvents()const noexcept{return storage_?storage_->eventCount:0;}
bool OriginalUiRiderMaterialLifetime50KG::hasOwners()const noexcept {
    if(storage_)for(const auto& owner:storage_->owners)if(owner.active)return true;
    return false;
}
bool OriginalUiRiderMaterialLifetime50KG::populationActive()const noexcept{return storage_&&storage_->populationActive();}
bool OriginalUiRiderMaterialLifetime50KG::aborted()const noexcept{return storage_&&storage_->aborted;}
bool OriginalUiRiderMaterialLifetime50KG::canAcquire(const OriginalUiRiderMaterialFootprint50KG& f,
    std::uint64_t prior,D& d)const noexcept {
    d={};if(!storage_)return fail(d,E::notBound);Storage::Acquire plan;
    return storage_->acquire(f,prior,plan,d);
}
bool OriginalUiRiderMaterialLifetime50KG::owns(const OriginalUiRiderMaterialOwnerToken50KG& token)const noexcept {
    return storage_&&token.ledger_==this&&token.slot_<storage_->owners.size()&&token.serial_&&
        storage_->owners[token.slot_].active&&storage_->owners[token.slot_].serial==token.serial_;
}
bool OriginalUiRiderMaterialLifetime50KG::canRelease(const OriginalUiRiderMaterialOwnerToken50KG& token,
    std::uint64_t prior,D& d)const noexcept {
    d={};if(!storage_)return fail(d,E::notBound);if(!owns(token))return fail(d,E::staleOwner);
    std::size_t events=0;return storage_->release(token.slot_,prior,events,d);
}
bool OriginalUiRiderMaterialLifetime50KG::canPopulate(std::uint64_t prior,D& d)const noexcept {
    d={};if(!storage_)return fail(d,E::notBound);Storage::Population plan;
    return storage_->populate(prior,plan,d);
}
bool OriginalUiRiderMaterialLifetime50KG::acquire(const OriginalUiRiderMaterialFootprint50KG& f,
    std::uint64_t prior,OriginalUiRiderMaterialOwnerToken50KG& out,OriginalUiRiderMaterialMutation50KG& receipt,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);if(out.issued())return fail(d,E::input);
    auto& s=*storage_;Storage::Acquire plan;if(!s.acquire(f,prior,plan,d))return false;
    const auto first=s.sequence+1;const auto serial=s.ownerSerial+1;auto& owner=s.owners[plan.owner];
    owner={};owner.serial=serial;owner.count=plan.count;owner.active=true;
    s.cleanupReserved+=2*plan.count;
    for(std::size_t i=0;i<plan.count;++i){const auto index=plan.rows[i];auto& row=s.rows[index];
        if(index>=s.rowCount)row.key=*f.row(i);
        ++row.owners;owner.rows[i]=index;owner.occurrences[i]=f.slotOccurrences(i);
        Event e;e.kind=EventKind::ownerAcquired;e.row=row.key;e.ownerSerial=serial;e.epoch=row.epoch;
        e.ownerPresenceAfter=row.owners;e.sourceSlotOccurrences=owner.occurrences[i];s.append(e);
    }
    s.rowCount+=plan.added;s.ownerSerial=serial;
    OriginalUiRiderMaterialOwnerToken50KG token;token.ledger_=this;token.serial_=serial;token.slot_=static_cast<std::uint16_t>(plan.owner);
    out=token;receipt={first,s.sequence,static_cast<std::uint32_t>(plan.count)};d={};return true;
}
bool OriginalUiRiderMaterialLifetime50KG::release(const OriginalUiRiderMaterialOwnerToken50KG& token,
    std::uint64_t prior,OriginalUiRiderMaterialMutation50KG& receipt,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);if(!owns(token))return fail(d,E::staleOwner);
    auto& s=*storage_;std::size_t events=0;if(!s.release(token.slot_,prior,events,d))return false;
    const auto first=s.sequence+1;auto& owner=s.owners[token.slot_];
    s.cleanupReserved-=2*owner.count;
    for(std::size_t i=0;i<owner.count;++i){auto& row=s.rows[owner.rows[i]];
        Event e;e.kind=EventKind::ownerReleased;e.row=row.key;e.ownerSerial=owner.serial;e.epoch=row.epoch;
        e.ownerPresenceAfter=--row.owners;e.sourceSlotOccurrences=owner.occurrences[i];s.append(e);
        if(!row.owners&&row.epoch){
            Event retired;retired.kind=EventKind::rowRetired;retired.row=row.key;retired.epoch=row.epoch;
            retired.retirement.ledger_=this;retired.retirement.row_=row.key;
            retired.retirement.epoch_=row.epoch;retired.retirement.sequence_=s.sequence+1;s.append(retired);
            row.epoch=0;
        }
    }
    owner.active=false;receipt={first,s.sequence,static_cast<std::uint32_t>(events)};d={};return true;
}
bool OriginalUiRiderMaterialLifetime50KG::beginPopulation(std::uint64_t prior,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);auto& s=*storage_;Storage::Population plan;
    if(!s.populate(prior,plan,d))return false;
    s.population=plan;return true;
}
const OriginalUiRiderMaterialRow50KG* OriginalUiRiderMaterialLifetime50KG::nextPopulationRow()const noexcept {
    if(!storage_)return nullptr;const auto& s=*storage_;const auto& p=s.population;
    return p.phase==Storage::Population::Phase::scanning&&p.next<p.count?
        &s.rows[p.rows[p.next]].key:nullptr;
}
bool OriginalUiRiderMaterialLifetime50KG::commitConstructedRow(const OriginalUiRiderMaterialRow50KG& key,
    std::uint64_t prior,OriginalUiRiderMaterialRowState50KG& out,
    OriginalUiRiderMaterialMutation50KG& receipt,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);auto& s=*storage_;auto& p=s.population;
    if(!s.expected(prior,d))return false;
    if(s.aborted)return fail(d,E::aborted);
    if(p.phase!=Storage::Population::Phase::scanning)return fail(d,E::populationInactive);
    if(p.next>=p.count)return fail(d,E::populationOrder);
    const auto index=p.rows[p.next];
    if(index>=s.rowCount)return fail(d,E::invariant);
    auto& row=s.rows[index];d.row=row.key.rank();
    if(!row.key.same(key))return fail(d,E::populationOrder);
    if(!row.owners||row.epoch||p.reserved!=p.count-p.next+2)return fail(d,E::invariant);
    if(s.epochSerial==maximum)return fail(d,E::overflow);
    if(!s.room(1,s.cleanupReserved+p.reserved-1,d))return false;
    row.epoch=++s.epochSerial;
    Event e;e.kind=EventKind::rowPopulated;e.row=row.key;e.epoch=row.epoch;e.ownerPresenceAfter=row.owners;
    s.append(e);++p.next;--p.reserved;
    out={row.key,row.owners,row.epoch};receipt={s.sequence,s.sequence,1};d={};return true;
}
bool OriginalUiRiderMaterialLifetime50KG::finishPopulation(std::uint64_t prior,
    OriginalUiRiderMaterialMutation50KG& receipt,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);auto& s=*storage_;auto& p=s.population;
    if(!s.expected(prior,d))return false;
    if(s.aborted)return fail(d,E::aborted);
    if(p.phase!=Storage::Population::Phase::scanning)return fail(d,E::populationInactive);
    if(p.next!=p.count)return fail(d,E::populationIncomplete);
    if(p.reserved!=2)return fail(d,E::invariant);
    if(!s.room(1,s.cleanupReserved+1,d))return false;
    Event boundary;boundary.kind=EventKind::populationBoundary;s.append(boundary);
    --p.reserved;p.phase=Storage::Population::Phase::awaitingGroup;
    receipt={p.firstSequence,s.sequence,static_cast<std::uint32_t>(p.count+1)};return true;
}
bool OriginalUiRiderMaterialLifetime50KG::recordGroupRebuilt(std::uint64_t prior,
    OriginalUiRiderMaterialMutation50KG& receipt,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);auto& s=*storage_;auto& p=s.population;
    if(!s.expected(prior,d))return false;
    if(s.aborted)return fail(d,E::aborted);
    if(p.phase!=Storage::Population::Phase::awaitingGroup)return fail(d,E::populationInactive);
    if(p.next!=p.count||p.reserved!=1)return fail(d,E::invariant);
    if(!s.room(1,s.cleanupReserved,d))return false;
    Event boundary;boundary.kind=EventKind::groupRebuilt;s.append(boundary);
    p={};receipt={s.sequence,s.sequence,1};return true;
}
bool OriginalUiRiderMaterialLifetime50KG::abortPopulation(std::uint64_t prior,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);auto& s=*storage_;
    if(!s.expected(prior,d))return false;
    if(s.aborted&&!s.populationActive())return true;
    if(!s.populationActive())return fail(d,E::populationInactive);
    // Successful prefix epochs/events remain authoritative. The only released
    // capacity is this scan's still-unused reservation; owner cleanup survives.
    s.population={};s.aborted=true;return true;
}
bool OriginalUiRiderMaterialLifetime50KG::populate(std::uint64_t prior,
    OriginalUiRiderMaterialMutation50KG& receipt,D& d) noexcept {
    if(!beginPopulation(prior,d))return false;
    while(const auto* row=nextPopulationRow()){
        OriginalUiRiderMaterialRowState50KG state;OriginalUiRiderMaterialMutation50KG committed;
        if(!commitConstructedRow(*row,sequence(),state,committed,d)){
            D ignored;abortPopulation(sequence(),ignored);return false;
        }
    }
    if(finishPopulation(sequence(),receipt,d))return true;
    D ignored;abortPopulation(sequence(),ignored);return false;
}
bool OriginalUiRiderMaterialLifetime50KG::rowState(const OriginalUiRiderMaterialRow50KG& key,
    OriginalUiRiderMaterialRowState50KG& out)const noexcept {
    if(!storage_||!key.prepared()||key.tableIdentity()!=storage_->table)return false;
    for(std::size_t i=0;i<storage_->rowCount;++i){const auto& r=storage_->rows[i];
        if(r.key.same(key)){out={r.key,r.owners,r.epoch};return true;}}
    return false;
}
bool OriginalUiRiderMaterialLifetime50KG::event(std::uint64_t sequence,Event& out)const noexcept {
    if(!storage_||sequence<=storage_->acknowledged||sequence>storage_->sequence)return false;
    const auto offset=sequence-storage_->acknowledged-1;
    if(offset>=storage_->eventCount)return false;
    out=storage_->journal[(storage_->head+static_cast<std::size_t>(offset))%storage_->journal.size()];
    return true;
}
bool OriginalUiRiderMaterialLifetime50KG::retirementCurrent(const OriginalUiRiderMaterialRetirement50KG& token)const noexcept {
    if(token.ledger_!=this||!token.epoch_)return false;Event e;
    return event(token.sequence_,e)&&e.kind==EventKind::rowRetired&&e.sequence==token.sequence_&&
        e.retirement.ledger_==this&&e.retirement.epoch_==token.epoch_&&e.retirement.row_.same(token.row_);
}
bool OriginalUiRiderMaterialLifetime50KG::acknowledgeThrough(std::uint64_t sequence,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);auto& s=*storage_;
    if(sequence<s.acknowledged||sequence>s.sequence)return fail(d,E::staleSequence);
    const auto count=sequence-s.acknowledged;
    if(count>s.eventCount)return fail(d,E::invariant);
    s.head=(s.head+static_cast<std::size_t>(count))%s.journal.size();s.eventCount-=static_cast<std::size_t>(count);
    s.acknowledged=sequence;return true;
}
}
