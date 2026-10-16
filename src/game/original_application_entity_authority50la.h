#pragma once
#include "original_entity_chains50la.h"
#include <array>
#include <exception>

namespace rr::game {
class OriginalApplicationEntities50LA;
class OriginalPauseStorage50LA;
// One native Application's lifetime membership. Session generations remain the
// owning constructor's original values. This does not certify the unrepresented
// original forest, dispatch targets, or a completed Application phase.
class OriginalApplicationEntityAuthority50LA final {
    friend class OriginalApplicationEntities50LA;
    friend class OriginalPauseStorage50LA;
    using C=OriginalWreckComponent50LA;
    struct Entry { C* node=nullptr;std::uint64_t generation=0,key=0; };
    std::array<Entry,maxOriginalWreckComponents50LA> entries_{};
    std::array<C*,maxOriginalWreckComponents50LA> nodes_{};
    std::size_t count_=0;
    // Private, synchronous view. No caller can retain an address snapshot.
    struct View {
        const OriginalApplicationEntityAuthority50LA* authority;
        C* const* objects;
        std::size_t count;
        bool valid()const noexcept {
            if(!authority||!count||count!=authority->count_||objects!=authority->nodes_.data())return false;
            for(std::size_t i=0;i<count;++i){
                const auto& e=authority->entries_[i];const auto* p=objects[i];
                if(!p||p!=e.node||!p->reference00||p->sessionGeneration!=e.generation||
                   p->objectKey!=e.key||p->kind==OriginalWreckComponentKind50LA::unbound)return false;
            }
            return true;
        }
        friend bool domain(const View& v)noexcept{return v.valid();}
        friend bool contains(const View& v,const C* p)noexcept {
            for(std::size_t i=0;i<v.count;++i)if(v.objects[i]==p)return true;
            return false;
        }
    };
    View view()const noexcept{return {this,nodes_.data(),count_};}
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
