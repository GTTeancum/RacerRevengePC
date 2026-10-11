#include "original_ui_rider_material_lifetime50kg.h"
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
                    // Exact later duplicate in original Mat_Stbl;298C80 keeps
                    // the first2490 token. No other collision is admitted.
                    if(!sameName(name,"ui/textures/pod_wndw")||next.rank_!=2490||
                       rank!=3221||start!=70650)return fail(d,E::table,0x298C80);
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
bool OriginalUiRiderMaterialFootprint50KG::pod(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiRiderPodAssets50KG& asset,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto id=asset.widgetId();
    const auto* widget=member(assets::OriginalUiArchiveId50KG::rider,"UI\\UIRider\\Widgets.Dat");
    const auto* modelSource=member(assets::OriginalUiArchiveId50KG::rider,
        id==27?"ui\\geometry\\19.psg":"ui\\geometry\\20.psg");
    if(!widget||!modelSource||!asset.loaded()||!asset.scene()||(id!=27&&id!=28)||
       asset.widgetSource()!=widget||asset.modelSource().identity!=modelSource)return fail(d,E::footprint,0x26E640);
    OriginalUiRiderMaterialFootprint50KG next;
    if(!next.model(app,*asset.scene(),d))return false;
    next.kind_=Kind::podGraph;next.widget_=id;next.source_=asset.modelSource().identity;out=next;return true;
}
bool OriginalUiRiderMaterialFootprint50KG::character(const assets::OriginalUiArchive50KG& app,
    const assets::OriginalUiRiderCharacterAssets50KG& asset,OriginalUiRiderMaterialFootprint50KG& out,D& d) noexcept {
    d={};const auto id=asset.widgetId();
    const auto* widget=member(assets::OriginalUiArchiveId50KG::rider,"UI\\UIRider\\Widgets.Dat");
    const auto* mcf=member(assets::OriginalUiArchiveId50KG::rider,
        id==53?"UI\\Anim\\19c.mcf":"ui\\anim\\20c.mcf");
    if(!widget||!mcf||!asset.loaded()||!asset.scene()||(id!=53&&id!=54)||!asset.baseName()||
       *asset.baseName()!=(id==53?"19c":"20c")||asset.widgetSource()!=widget||asset.mcfSource()!=mcf)
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
        std::size_t count=0;
        std::array<std::uint16_t,originalUiRiderMaterialRows50KG> rows{};
    };
    std::shared_ptr<const assets::OriginalUiArchive50KG> app;
    const assets::OriginalUiMemberIdentity50KG* table=nullptr;
    std::array<Row,originalUiRiderMaterialRows50KG> rows{};
    std::array<Owner,originalUiRiderMaterialOwners50KG> owners{};
    std::array<Event,originalUiRiderMaterialEvents50KG> journal{};
    std::size_t rowCount=0,head=0,eventCount=0;
    std::uint64_t sequence=0,ownerSerial=0,epochSerial=0,acknowledged=0;
    bool expected(std::uint64_t prior,D& d)const noexcept {
        return prior==sequence||fail(d,E::staleSequence);
    }
    bool room(std::size_t count,D& d)const noexcept {
        if(count>journal.size()-eventCount)return fail(d,E::journalCapacity);
        return count<=maximum-sequence||fail(d,E::overflow);
    }
    void append(Event e) noexcept {
        e.sequence=++sequence;journal[(head+eventCount)%journal.size()]=e;++eventCount;
    }
    bool acquire(const OriginalUiRiderMaterialFootprint50KG& f,std::uint64_t prior,Acquire& plan,D& d)const noexcept {
        if(!expected(prior,d))return false;
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
        return room(plan.count,d);
    }
    bool release(std::size_t owner,std::uint64_t prior,std::size_t& events,D& d)const noexcept {
        if(!expected(prior,d))return false;const auto& token=owners[owner];
        if(!token.active||!token.count||token.count>rows.size())return fail(d,E::invariant);
        events=token.count;
        for(std::size_t i=0;i<token.count;++i){const auto row=token.rows[i];
            if(row>=rowCount||!rows[row].owners)return fail(d,E::invariant);
            if(rows[row].owners==1&&rows[row].epoch)++events;
        }
        return room(events,d);
    }
    bool populate(std::uint64_t prior,Population& plan,D& d)const noexcept {
        if(!expected(prior,d))return false;plan={};
        for(std::size_t i=0;i<rowCount;++i){
            if(!rows[i].owners&&rows[i].epoch)return fail(d,E::invariant);
            if(rows[i].owners&&!rows[i].epoch)plan.rows[plan.count++]=static_cast<std::uint16_t>(i);
        }
        if(plan.count>maximum-epochSerial)return fail(d,E::overflow);
        if(!room(plan.count+1,d))return false;
        // Bounded allocation-free table-order population, independent of
        // native footprint/owner acquisition order.
        for(std::size_t i=1;i<plan.count;++i){const auto row=plan.rows[i];std::size_t j=i;
            while(j&&rows[plan.rows[j-1]].key.rank()>rows[row].key.rank()){
                plan.rows[j]=plan.rows[j-1];--j;
            }
            plan.rows[j]=row;
        }
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
std::size_t OriginalUiRiderMaterialLifetime50KG::pendingEvents()const noexcept{return storage_?storage_->eventCount:0;}
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
bool OriginalUiRiderMaterialLifetime50KG::populate(std::uint64_t prior,
    OriginalUiRiderMaterialMutation50KG& receipt,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);auto& s=*storage_;Storage::Population plan;
    if(!s.populate(prior,plan,d))return false;const auto first=s.sequence+1;
    for(std::size_t i=0;i<plan.count;++i){auto& row=s.rows[plan.rows[i]];row.epoch=++s.epochSerial;
        Event e;e.kind=EventKind::rowPopulated;e.row=row.key;e.epoch=row.epoch;e.ownerPresenceAfter=row.owners;s.append(e);
    }
    Event boundary;boundary.kind=EventKind::populationBoundary;s.append(boundary);
    receipt={first,s.sequence,static_cast<std::uint32_t>(plan.count+1)};d={};return true;
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
