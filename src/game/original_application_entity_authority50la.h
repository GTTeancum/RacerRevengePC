#pragma once
namespace rr::game { class OriginalLoadingManager33050LA; }
namespace original_racer_manager_entity { class Owner; }
namespace original_camera_session50KX { class Session; }
namespace original_main_entry50KG { struct Application; }
#include "original_entity_chains50la.h"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <vector>
#include <limits>
#include <functional>
#include <utility>

namespace ra_pod_scene50LA { class Owner; }
namespace ra_cold_aggregate50LA { class Owner; }
namespace ra_source_camera50LA { class OwnershipBridge; }
namespace rr::platform { class RaceAudioGroups50IC;class OriginalApplicationAural50KG;class OriginalUiLightManagerOwner50KG; }
namespace rr::game {
class OriginalTunnelTrack50LA;
class OriginalApplicationEntities50LA;
class OriginalPauseStorage50LA;
// One native Application's lifetime membership. Session generations remain the
// owning constructor's original values. This does not certify the unrepresented
// original forest, dispatch targets, or a completed Application phase.
class OriginalApplicationEntityAuthority50LA final {
    friend class OriginalApplicationEntities50LA;
    friend class OriginalWorldCollisionObjects50LA; //Exact1B577C callback enrollment/attachment custody.
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalLoadingManager33050LA;
    friend class OriginalLoadingScreen40Mode0LA;
    friend class OriginalFrontendCameraCold50LA;
    friend class OriginalUiEntityScope50KG;
    friend class OriginalPauseStorage50LA;
    friend class OriginalResults10450LA;
    friend class OriginalRaceF850LA;
    friend class OriginalSimpleVCR50LA;
    friend class OriginalTrackParticleCold50LA;
    friend class OriginalRaceMusicManager50LA;
    friend class OriginalRaceTimingManager50LA;
    friend class OriginalRaceRacerManager50LA;
    friend class OriginalTunnelTrack50LA;
    friend class OriginalRaceConstructor50LA;
    friend class ::original_camera_session50KX::Session;
    friend class ::original_racer_manager_entity::Owner;
    friend class rr::platform::RaceAudioGroups50IC;
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class rr::platform::OriginalApplicationAural50KG;
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    // The same-session handling join checks its freshly constructed speech
    // Entity's retirement authority before publishing the player's D48 alias.
    friend class ::ra_source_camera50LA::OwnershipBridge;
    friend class OriginalPlayerConstructorPrefix50LA;
    using C=OriginalWreckComponent50LA;
    struct Entry { C* node=nullptr;std::uint64_t generation=0,key=0; };
    // Keep original allocation-free64-node construction. Only an actual cold
    // owner may reserve more, before any of its counted constructors run.
    template<class T> struct Storage {
        std::array<T,maxOriginalWreckComponents50LA> initial{};
        std::vector<T> expanded;
        std::size_t size()const noexcept{return expanded.empty()?initial.size():expanded.size();}
        T* data()noexcept{return expanded.empty()?initial.data():expanded.data();}
        const T* data()const noexcept{return expanded.empty()?initial.data():expanded.data();}
        T& operator[](std::size_t i)noexcept{return data()[i];}
        const T& operator[](std::size_t i)const noexcept{return data()[i];}
    };
    // Validation scratch maps exact pointer identity to the ORIGINAL row.
    // It never owns/copies an Entity or certifies a previous forest traversal.
    struct VisitIndex { const C* node=nullptr;std::size_t row=0; };
    Storage<Entry> entries_;
    Storage<C*> nodes_;
    mutable Storage<unsigned char> visitScratch_;
    mutable Storage<VisitIndex> visitIndexScratch_;
    std::size_t count_=0;
    // Host-side lookup index only (never source state): exact enrolled node
    // pointer -> its CURRENT entries_/nodes_ row, maintained by enroll/retire/
    // reserveAdditional, the only writers of those rows. Open addressing over
    // a power-of-two table at least twice the row capacity; slot value is row+1
    // (0 = empty). Allocation happens only where row storage itself grows.
    template<class T,std::size_t N> struct IndexStorage {
        std::array<T,N> initial{};
        std::vector<T> expanded;
        std::size_t size()const noexcept{return expanded.empty()?initial.size():expanded.size();}
        T* data()noexcept{return expanded.empty()?initial.data():expanded.data();}
        const T* data()const noexcept{return expanded.empty()?initial.data():expanded.data();}
    };
    IndexStorage<std::uint32_t,2*maxOriginalWreckComponents50LA> rowIndex_;
    static std::size_t indexHash(const C* p,std::size_t mask)noexcept {
        auto v=reinterpret_cast<std::uintptr_t>(p);
        v^=v>>17;v*=static_cast<std::uintptr_t>(0x9E3779B97F4A7C15ull);v^=v>>29;
        return static_cast<std::size_t>(v)&mask;
    }
    static void indexInsert(std::uint32_t* table,std::size_t size,const C* p,std::size_t row)noexcept {
        const std::size_t mask=size-1;
        for(std::size_t i=indexHash(p,mask);;i=(i+1)&mask)
            if(!table[i]){table[i]=static_cast<std::uint32_t>(row+1);return;}
    }
    void indexRebuild()noexcept {
        auto* table=rowIndex_.data();const std::size_t size=rowIndex_.size();
        for(std::size_t i=0;i<size;++i)table[i]=0;
        for(std::size_t i=0;i<count_;++i)indexInsert(table,size,nodes_[i],i);
    }
    static bool indexLegacy()noexcept {
        static const bool on=[]{const char* v=std::getenv("RR_GARAGE_LEGACY_INDEX");return v&&v[0]=='1'&&!v[1];}();
        return on;
    }
    static bool indexShadow()noexcept {
        static const bool on=[]{const char* v=std::getenv("RR_GARAGE_PROOF_SHADOW");return v&&v[0]=='1'&&!v[1];}();
        return on;
    }
    std::size_t linearRow(const C* p)const noexcept {
        for(std::size_t i=0;i<count_;++i)if(entries_[i].node==p)return i;
        return count_;
    }
    // Row of p among the first count_ rows, or count_ when not enrolled.
    std::size_t rowOf(const C* p)const noexcept {
        if(indexLegacy())return linearRow(p);
        std::size_t result=count_;
        if(p){
            const auto* table=rowIndex_.data();const std::size_t mask=rowIndex_.size()-1;
            for(std::size_t i=indexHash(p,mask);table[i];i=(i+1)&mask){
                const std::size_t row=table[i]-1u;
                if(row<count_&&nodes_[row]==p&&entries_[row].node==p){result=row;break;}
            }
        }
        if(indexShadow()&&result!=linearRow(p))std::terminate();
        return result;
    }
    // Native cold-factory provenance only, not a guest field. Never reset by
    // retirement: a currently empty forest cannot prove no prior dispatcher.
    bool nonRootEnrollmentObserved_=false;
    // Monotonic native provenance. Even retirement/address reuse cannot make a
    // later forest look like the exact source13FE24 constructor prefix.
    std::uint64_t enrollmentSerial_=0;
    C* auralStartupPrefix_=nullptr;
    bool enrollAuralStartupPrefix(C& node,const C& outer,const C& inner)noexcept {
        if(count_!=2||enrollmentSerial_!=2||nonRootEnrollmentObserved_||auralStartupPrefix_||
           entries_[0].node!=&outer||entries_[1].node!=&inner||
           node.kind!=OriginalWreckComponentKind50LA::auralScape102BE0)return false;
        if(!enroll(node))return false; // The existing non-root latch STILL becomes true.
        auralStartupPrefix_=&node;return true;
    }
    // Private, synchronous view. No caller can retain an address snapshot.
    struct View {
        const OriginalApplicationEntityAuthority50LA* authority;
        C* const* objects;
        std::size_t count;
        bool valid()const noexcept {
            if(!authority||!count||count!=authority->count_||objects!=authority->nodes_.data()||
               count>authority->entries_.size()||count>authority->nodes_.size()||
               count>authority->visitScratch_.size()||count>authority->visitIndexScratch_.size())return false;
            for(std::size_t i=0;i<count;++i){
                const auto& e=authority->entries_[i];const auto* p=objects[i];
                if(!p||p!=e.node||!p->reference00||p->sessionGeneration!=e.generation||
                   p->objectKey!=e.key||p->kind==OriginalWreckComponentKind50LA::unbound)return false;
            }
            return true;
        }
        unsigned char* scratch()const noexcept {
            // forest() has just performed the full domain/Entry validation.
            // Rebuild on EVERY traversal; no cache crosses a mutation or call.
            auto* index=authority->visitIndexScratch_.data();
            for(std::size_t i=0;i<count;++i)index[i]={objects[i],i};
            const std::less<const C*> less;
            // In-place heap sort: bounded native scratch, no per-frame heap
            // allocation and no pointer-order assumption for unrelated nodes.
            const auto sift=[&](std::size_t root,std::size_t end) noexcept {
                while(root<end/2){
                    std::size_t child=root*2+1;
                    if(child+1<end&&less(index[child].node,index[child+1].node))++child;
                    if(!less(index[root].node,index[child].node))return;
                    std::swap(index[root],index[child]);root=child;
                }
            };
            for(std::size_t start=count/2;start;)sift(--start,count);
            for(std::size_t end=count;end>1;){
                --end;std::swap(index[0],index[end]);sift(0,end);
            }
            return authority->visitScratch_.data();
        }
        bool visitRow(const C* p,std::size_t& row)const noexcept {
            // Only the same synchronous forest walk consumes scratch().
            // Search uses the total pointer order, never a foreign dereference.
            const auto* index=authority->visitIndexScratch_.data();
            const std::less<const C*> less;
            std::size_t first=0,last=count;
            while(first<last){
                const auto middle=first+(last-first)/2;
                if(less(index[middle].node,p))first=middle+1;else last=middle;
            }
            if(first==count||index[first].node!=p)return false;
            row=index[first].row;
            return row<count&&objects[row]==p;
        }
        friend bool walk(const View& v,const OriginalWreckComponent50LA* first,
                         const OriginalWreckComponent50LA* parent,unsigned char* seen)noexcept {
            const OriginalWreckComponent50LA* previous=nullptr;
            for(auto* p=first;p;p=p->next14){
                std::size_t i=0;
                if(!v.visitRow(p,i))return false; // Exact enrolled-pointer membership.
                if(seen[i]||p->parent1C!=parent||p->previous10!=previous)return false;
                seen[i]=true;
                if(!walk(v,p->firstChild18,p,seen))return false;
                previous=p;
            }
            return true;
        }
        friend unsigned char* entityVisitScratch(const View& v)noexcept{return v.scratch();}
        friend bool domain(const View& v)noexcept{return v.valid();}
        bool forestProvenMemo()const noexcept{return authority&&authority->forestProven(*this);}
        void forestRecordMemo()const noexcept{if(authority)authority->forestRecord(*this);}
        friend bool entityForestProven(const View& v)noexcept{return v.forestProvenMemo();}
        friend void entityForestRecord(const View& v)noexcept{v.forestRecordMemo();}
        bool containsMember(const C* p)const noexcept {
            // Same answer as the linear scan: a current View's objects ARE the
            // authority's live rows, whose exact index rowOf() maintains.
            if(authority&&objects==authority->nodes_.data()&&count==authority->count_)
                return authority->rowOf(p)<count;
            for(std::size_t i=0;i<count;++i)if(objects[i]==p)return true;
            return false;
        }
        friend bool contains(const View& v,const C* p)noexcept{return v.containsMember(p);}
    };
    // Exact host-side memo of the last PROVEN-VALID forest() result. Every
    // field the full validation reads (Entry rows, node array/count/extents and
    // each enrolled node's identity and link words) is retained; a later View
    // is accepted without re-walking ONLY if all of them are bit-identical.
    // Rejections are never memoized. Not a guest field or readiness flag.
    struct ForestRow {
        const C* node=nullptr;std::uint64_t entryGeneration=0,entryKey=0;
        std::uint64_t reference=0,generation=0,key=0;
        OriginalWreckComponentKind50LA kind=OriginalWreckComponentKind50LA::unbound;
        const C* previous=nullptr;const C* next=nullptr;const C* first=nullptr;const C* parent=nullptr;
    };
    mutable std::vector<ForestRow> forestRows_;
    mutable bool forestRecorded_=false;
    mutable C* const* forestObjects_=nullptr;
    mutable std::size_t forestCount_=0;
    mutable std::array<std::size_t,4> forestExtents_{};
    std::array<std::size_t,4> forestExtentsNow()const noexcept {
        return {entries_.size(),nodes_.size(),visitScratch_.size(),visitIndexScratch_.size()};
    }
    bool forestRowsMatch(const View& v)const noexcept {
        if(!forestRecorded_||v.authority!=this||v.objects!=forestObjects_||v.count!=forestCount_||
           v.count!=count_||v.objects!=nodes_.data()||forestRows_.size()!=v.count||
           forestExtents_!=forestExtentsNow())return false;
        for(std::size_t i=0;i<v.count;++i){
            const auto& r=forestRows_[i];const auto& e=entries_[i];const C* p=v.objects[i];
            if(p!=r.node||e.node!=r.node||e.generation!=r.entryGeneration||e.key!=r.entryKey||
               p->reference00!=r.reference||p->sessionGeneration!=r.generation||p->objectKey!=r.key||
               p->kind!=r.kind||p->previous10!=r.previous||p->next14!=r.next||
               p->firstChild18!=r.first||p->parent1C!=r.parent)return false;
        }
        return true;
    }
    bool forestProven(const View& v)const noexcept {
        using Scope=EntityForestReadScope50LA;
        if(Scope::depth&&Scope::provenAuthority==this&&Scope::provenObjects==v.objects&&
           Scope::provenCount==v.count&&Scope::provenSerial==enrollmentSerial_&&
           v.count==count_&&v.objects==nodes_.data())return true;
        if(!forestRowsMatch(v))return false;
        if(Scope::depth){Scope::provenAuthority=this;Scope::provenObjects=v.objects;
            Scope::provenCount=v.count;Scope::provenSerial=enrollmentSerial_;}
        return true;
    }
    void forestRecord(const View& v)const noexcept {
        // Called only after a complete successful validation of this View.
        forestRecorded_=false;
        if(v.authority!=this||v.count!=count_||v.objects!=nodes_.data())return;
        try{forestRows_.resize(v.count);}catch(...){return;}
        for(std::size_t i=0;i<v.count;++i){
            const auto& e=entries_[i];const C* p=v.objects[i];auto& r=forestRows_[i];
            r.node=p;r.entryGeneration=e.generation;r.entryKey=e.key;r.reference=p->reference00;
            r.generation=p->sessionGeneration;r.key=p->objectKey;r.kind=p->kind;
            r.previous=p->previous10;r.next=p->next14;r.first=p->firstChild18;r.parent=p->parent1C;
        }
        forestObjects_=v.objects;forestCount_=v.count;forestExtents_=forestExtentsNow();forestRecorded_=true;
        using Scope=EntityForestReadScope50LA;
        if(Scope::depth){Scope::provenAuthority=this;Scope::provenObjects=v.objects;
            Scope::provenCount=v.count;Scope::provenSerial=enrollmentSerial_;}
    }
    View view()const noexcept{return {this,nodes_.data(),count_};}
    bool reserveAdditional(std::size_t additional)noexcept {
        // Source object counts are32-bit. No transient View survives this
        // synchronous cold call; only registry backing moves, never Entities.
        if(count_>std::numeric_limits<std::uint32_t>::max()||
           additional>std::numeric_limits<std::uint32_t>::max()-count_)return false;
        const auto required=count_+additional;
        if(required<=entries_.size())return true;
        try {
            std::vector<Entry> entries;
            std::vector<C*> nodes;
            std::vector<unsigned char> scratch;
            std::vector<VisitIndex> index;
            std::vector<std::uint32_t> rows;
            if(required>entries.max_size()||required>nodes.max_size()||required>scratch.max_size()||
               required>index.max_size()||required>std::numeric_limits<std::uint32_t>::max()/4u)return false;
            std::size_t indexSize=rowIndex_.size();while(indexSize<2*required)indexSize*=2;
            entries.resize(required);nodes.resize(required);scratch.resize(required);index.resize(required);
            if(indexSize!=rowIndex_.size())rows.resize(indexSize);
            for(std::size_t i=0;i<count_;++i){entries[i]=entries_[i];nodes[i]=nodes_[i];}
            // All allocations succeeded before the first mutation.
            entries_.expanded.swap(entries);nodes_.expanded.swap(nodes);visitScratch_.expanded.swap(scratch);
            visitIndexScratch_.expanded.swap(index);
            if(!rows.empty())rowIndex_.expanded.swap(rows);
            indexRebuild();
            return true;
        }catch(...){return false;}
    }
    bool canEnroll(const C& node,std::uint64_t key)const noexcept {
        if(count_==entries_.size()||!key||enrollmentSerial_==std::numeric_limits<std::uint64_t>::max())return false;
        for(std::size_t i=0;i<count_;++i)
            if(entries_[i].node==&node||entries_[i].key==key)return false;
        return true;
    }
    bool enroll(C& node)noexcept {
        if(!canEnroll(node,node.objectKey)||!node.reference00||!node.sessionGeneration||
           node.kind==OriginalWreckComponentKind50LA::unbound)return false;
        if(count_>=2)nonRootEnrollmentObserved_=true;
        entries_[count_]={&node,node.sessionGeneration,node.objectKey};nodes_[count_]=&node;
        indexInsert(rowIndex_.data(),rowIndex_.size(),&node,count_);++count_;++enrollmentSerial_;
        return true;
    }
    bool canRetire(const C& node)const noexcept {
        if(node.reference00!=1)return false;
        const std::size_t i=rowOf(&node);
        if(i<count_)return node.sessionGeneration==entries_[i].generation&&node.objectKey==entries_[i].key;
        return false;
    }
    bool retire(C& node)noexcept {
        // Called immediately before the genuine last-reference release. No
        // stale enrollment may survive storage destruction or address reuse.
        // The concrete owner checks its exact source teardown topology first.
        // Application142590 root release still retains dying-node parent links.
        if(node.reference00!=1)return false;
        const std::size_t i=rowOf(&node);
        if(i<count_){
            if(node.sessionGeneration!=entries_[i].generation||node.objectKey!=entries_[i].key)return false;
            for(std::size_t j=i+1;j<count_;++j){entries_[j-1]=entries_[j];nodes_[j-1]=nodes_[j];}
            entries_[--count_]={};nodes_[count_]=nullptr;indexRebuild();return true;
        }
        return false;
    }
    OriginalWreckComponentResult50LA appendSibling(C& anchor,C& head,std::uint32_t mask,std::uint32_t lock)noexcept {
        return original_entity_chain_detail50LA::appendOriginalWreckSiblingChain14186050LA(view(),anchor,head,mask,lock);
    }
    OriginalWreckComponentResult50LA attachChildren(C& parent,C& head,std::uint32_t mask,std::uint32_t lock)noexcept {
        return original_entity_chain_detail50LA::attachOriginalWreckChildChain14196050LA(view(),parent,head,mask,lock);
    }
    OriginalWreckComponentResult50LA detach(C& node,std::uint32_t lock)noexcept {
        return original_entity_chain_detail50LA::detachOriginalWreckComponent141A7050LA(view(),node,lock);
    }
    OriginalApplicationEntityAuthority50LA()noexcept=default;
    ~OriginalApplicationEntityAuthority50LA(){if(count_)std::terminate();}
    OriginalApplicationEntityAuthority50LA(const OriginalApplicationEntityAuthority50LA&)=delete;
    OriginalApplicationEntityAuthority50LA& operator=(const OriginalApplicationEntityAuthority50LA&)=delete;
};
}
