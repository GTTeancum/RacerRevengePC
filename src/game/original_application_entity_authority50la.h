#pragma once
#include "original_entity_chains50la.h"
#include <array>
#include <exception>
#include <vector>
#include <limits>

namespace ra_pod_scene50LA { class Owner; }
namespace rr::platform { class RaceAudioGroups50IC; }
namespace rr::game {
class OriginalTunnelTrack50LA;
class OriginalApplicationEntities50LA;
class OriginalPauseStorage50LA;
// One native Application's lifetime membership. Session generations remain the
// owning constructor's original values. This does not certify the unrepresented
// original forest, dispatch targets, or a completed Application phase.
class OriginalApplicationEntityAuthority50LA final {
    friend class OriginalApplicationEntities50LA;
    friend class OriginalPauseStorage50LA;
    friend class OriginalTunnelTrack50LA;
    friend class OriginalRaceConstructor50LA;
    friend class rr::platform::RaceAudioGroups50IC;
    friend class ::ra_pod_scene50LA::Owner;
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
    Storage<Entry> entries_;
    Storage<C*> nodes_;
    mutable Storage<unsigned char> visitScratch_;
    std::size_t count_=0;
    // Private, synchronous view. No caller can retain an address snapshot.
    struct View {
        const OriginalApplicationEntityAuthority50LA* authority;
        C* const* objects;
        std::size_t count;
        bool valid()const noexcept {
            if(!authority||!count||count!=authority->count_||objects!=authority->nodes_.data()||
               count>authority->entries_.size()||count>authority->nodes_.size()||
               count>authority->visitScratch_.size())return false;
            for(std::size_t i=0;i<count;++i){
                const auto& e=authority->entries_[i];const auto* p=objects[i];
                if(!p||p!=e.node||!p->reference00||p->sessionGeneration!=e.generation||
                   p->objectKey!=e.key||p->kind==OriginalWreckComponentKind50LA::unbound)return false;
            }
            return true;
        }
        unsigned char* scratch()const noexcept{return authority->visitScratch_.data();}
        friend unsigned char* entityVisitScratch(const View& v)noexcept{return v.scratch();}
        friend bool domain(const View& v)noexcept{return v.valid();}
        friend bool contains(const View& v,const C* p)noexcept {
            for(std::size_t i=0;i<v.count;++i)if(v.objects[i]==p)return true;
            return false;
        }
    };
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
            if(required>entries.max_size()||required>nodes.max_size()||required>scratch.max_size())return false;
            entries.resize(required);nodes.resize(required);scratch.resize(required);
            for(std::size_t i=0;i<count_;++i){entries[i]=entries_[i];nodes[i]=nodes_[i];}
            // All three allocations succeeded before the first mutation.
            entries_.expanded.swap(entries);nodes_.expanded.swap(nodes);visitScratch_.expanded.swap(scratch);
            return true;
        }catch(...){return false;}
    }
    bool canEnroll(const C& node,std::uint64_t key)const noexcept {
        if(count_==entries_.size()||!key)return false;
        for(std::size_t i=0;i<count_;++i)
            if(entries_[i].node==&node||entries_[i].key==key)return false;
        return true;
    }
    bool enroll(C& node)noexcept {
        if(!canEnroll(node,node.objectKey)||!node.reference00||!node.sessionGeneration||
           node.kind==OriginalWreckComponentKind50LA::unbound)return false;
        entries_[count_]={&node,node.sessionGeneration,node.objectKey};nodes_[count_++]=&node;
        return true;
    }
    bool canRetire(const C& node)const noexcept {
        if(node.reference00!=1)return false;
        for(std::size_t i=0;i<count_;++i)if(entries_[i].node==&node)
            return node.sessionGeneration==entries_[i].generation&&node.objectKey==entries_[i].key;
        return false;
    }
    bool retire(C& node)noexcept {
        // Called immediately before the genuine last-reference release. No
        // stale enrollment may survive storage destruction or address reuse.
        // The concrete owner checks its exact source teardown topology first.
        // Application142590 root release still retains dying-node parent links.
        if(node.reference00!=1)return false;
        for(std::size_t i=0;i<count_;++i)if(entries_[i].node==&node){
            if(node.sessionGeneration!=entries_[i].generation||node.objectKey!=entries_[i].key)return false;
            for(std::size_t j=i+1;j<count_;++j){entries_[j-1]=entries_[j];nodes_[j-1]=nodes_[j];}
            entries_[--count_]={};nodes_[count_]=nullptr;return true;
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
