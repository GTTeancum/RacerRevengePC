#pragma once
// Staged252C70 allocation only. No2542F0 reset or source13C publication.
#include "original_standard_texture_lifetime50la.h"
#include <array>
#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <utility>
namespace rr::assets {
struct OriginalStandardTextureTransferNode50LA {
    std::byte* data00=nullptr;
    std::uint16_t side04=0;
    std::uint8_t flags06=0;
    OriginalStandardTextureTransferNode50LA* child[4]{};
};
enum class OriginalStandardTextureTransferColdStatus50LA {complete,input,identity,conflict,allocation,phase};
struct OriginalStandardTextureTransferColdDiagnostic50LA {
    OriginalStandardTextureTransferColdStatus50LA status=OriginalStandardTextureTransferColdStatus50LA::complete;
    std::uint32_t sourcePC=0;
    bool effects=false;
};
class OriginalStandardTextureTransferCold50LA final {
    friend struct ::original_main_entry50KG::Application;
    using S=OriginalStandardTextureTransferColdStatus50LA;
    using D=OriginalStandardTextureTransferColdDiagnostic50LA;
    using Node=OriginalStandardTextureTransferNode50LA;
    struct Row {
        Node root;
        std::array<std::uint16_t,5> available{1,4,16,64,256}; //2520D0
    };
    const original_main_entry50KG::Application* const issuer_;
    const std::shared_ptr<OriginalStandardTextureLifetime50LA> manager_;
    // Source first256 records: opaque lanes stay unread until final memset.
    std::array<std::byte,0x1800> records_;
    std::unique_ptr<std::byte[]> scratch1800_;
    std::unique_ptr<Row[]> rows1804_;
    std::unique_ptr<Node[]> arena1814_;
    std::array<std::unique_ptr<std::byte[]>,12> backing_{};
    std::uint32_t rowCount1808_=0,counter180C_=0,budget1810_=0;
    std::size_t cursor_=0,rowsBuilt_=0;
    std::array<std::byte,11> tail1818_;
    bool attempted_=false,claimed_=false,complete_=false,released_=false;
    OriginalStandardTextureTransferCold50LA(const original_main_entry50KG::Application& issuer,
        std::shared_ptr<OriginalStandardTextureLifetime50LA> manager)noexcept
        :issuer_(&issuer),manager_(std::move(manager)){}
    bool same(const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<OriginalStandardTextureLifetime50LA>& manager)const noexcept {
        return issuer==issuer_&&manager&&manager==manager_;
    }
    bool fail(D& d,S s,std::uint32_t pc)const noexcept {d={s,pc,claimed_};return false;}
    bool build252150(Node& node,std::uint16_t side,std::byte* base,unsigned quadrant)noexcept {
        node.data00=base+std::size_t(side)*side*2*quadrant;
        node.side04=side;node.flags06=1;
        if(side==16){for(auto& c:node.child)c=nullptr;return true;}
        for(unsigned q=0;q<4;++q){
            if(cursor_>=12*340||manager_->treeCursor442B60_!=arena1814_.get()+cursor_)return false;
            auto* next=manager_->treeCursor442B60_;
            node.child[q]=next;++cursor_;
            manager_->treeCursor442B60_=arena1814_.get()+cursor_;
            if(!build252150(*next,side/2,node.data00,q))return false;
        }
        return true;
    }
    bool construct252C70(const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<OriginalStandardTextureLifetime50LA>& manager,D& d)noexcept {
        d={};if(!same(issuer,manager))return fail(d,S::identity,0x252C70);
        if(attempted_||released_)return fail(d,S::phase,0x252C70);
        // Do not reset, steal or overwrite another real owner/cursor/transfer.
        if(manager_->publishedTransfer13C_||manager_->coldTransferOwner_||manager_->treeCursor442B60_||manager_->nativeUploads_)
            return fail(d,S::conflict,0x252C70);
        attempted_=true;claimed_=true;manager_->coldTransferOwner_=this;
        //2E8860/252E70:256 source24-byte records; untouched lanes are opaque.
        for(std::size_t i=0;i<256;++i){
            std::memset(records_.data()+i*24,0,12);
            std::memset(records_.data()+i*24+20,0,4);
        }
        counter180C_=0;
        scratch1800_.reset(new(std::nothrow) std::byte[0x20000]);
        if(!scratch1800_)return fail(d,S::allocation,0x252CA8);
        rowCount1808_=12;budget1810_=8u<<17; //reached1MiB→8, source min12.
        rows1804_.reset(new(std::nothrow) Row[12]);
        if(!rows1804_)return fail(d,S::allocation,0x252CE4);
        // Native-pointer tree storage represents exactly12*340 source nodes;
        // guest byte request is12*8160=97920, NOT sizeof(native Node)*count.
        arena1814_.reset(new(std::nothrow) Node[12*340]);
        if(!arena1814_)return fail(d,S::allocation,0x252D34);
        manager_->treeCursor442B60_=arena1814_.get();
        for(std::size_t i=0;i<12;++i){
            rows1804_[i].root={}; //252D80 clears only source first24 bytes.
            backing_[i].reset(new(std::nothrow) std::byte[0x20000]);
            if(!backing_[i])return fail(d,S::allocation,0x252D88);
            if(!build252150(rows1804_[i].root,256,backing_[i].get(),0))
                return fail(d,S::phase,0x252E10);
            ++rowsBuilt_;
        }
        std::memset(records_.data(),0,records_.size()); //252E38
        tail1818_.fill(std::byte{0}); //252E48/252E50
        complete_=true;d={S::complete,0x252E68,true};return true;
    }
    bool releaseCold(const original_main_entry50KG::Application* issuer,
        const std::shared_ptr<OriginalStandardTextureLifetime50LA>& manager,D& d)noexcept {
        d={};if(!same(issuer,manager))return fail(d,S::identity,0x252EA0);
        if(released_)return true;
        if(!claimed_){released_=true;return true;}
        if(manager_->nativeUploads_)return fail(d,S::conflict,0x252EA0);
        if(manager_->coldTransferOwner_!=this||
           manager_->treeCursor442B60_!=(arena1814_?arena1814_.get()+cursor_:nullptr))
            return fail(d,S::phase,0x252EA0);
        //252EA0 order; partial native failure retires only reached allocations.
        for(std::size_t i=0;i<12;++i){backing_[i].reset();if(rows1804_)rows1804_[i].root.data00=nullptr;}
        rows1804_.reset();arena1814_.reset();manager_->treeCursor442B60_=nullptr;
        scratch1800_.reset();manager_->coldTransferOwner_=nullptr;
        claimed_=false;complete_=false;released_=true;d={S::complete,0x252F70,true};return true;
    }
public:
    struct Snapshot {
        bool attempted,claimed,complete,released;
        std::uint32_t rows,budget;
        std::size_t nodes,rowsBuilt,backings;
        bool scratch,descriptors,arena;
    };
    Snapshot snapshot()const noexcept {
        std::size_t n=0;for(const auto& p:backing_)if(p)++n;
        return {attempted_,claimed_,complete_,released_,rowCount1808_,budget1810_,
            cursor_,rowsBuilt_,n,bool(scratch1800_),bool(rows1804_),bool(arena1814_)};
    }
    ~OriginalStandardTextureTransferCold50LA(){if(claimed_&&!released_)std::terminate();}
    OriginalStandardTextureTransferCold50LA(const OriginalStandardTextureTransferCold50LA&)=delete;
    OriginalStandardTextureTransferCold50LA& operator=(const OriginalStandardTextureTransferCold50LA&)=delete;
};
}
