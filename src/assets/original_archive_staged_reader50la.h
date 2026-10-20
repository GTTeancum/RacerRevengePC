#pragma once
#include "original_standard_archive_provider50la.h"
#include <algorithm>
#include <array>
#include <zlib.h>

namespace rr::assets {
// Staging storage, NOT retail FRead or a live3E1A70 service. No disk handles,
// global row IDs, activation history, attached-object state or page authority.
class OriginalArchiveStagedReader50LA final {
    friend class OriginalStandardArchiveProvider50LA;
    using Provider=OriginalStandardArchiveProvider50LA;
public:
    enum class Status {complete,identity,phase,input,allocation,decode,pending};
    struct Diagnostic {Status status=Status::complete;std::size_t copied=0,splitBlocks=0,decodedBlocks=0;
        std::size_t firstSplitBlock=static_cast<std::size_t>(-1),firstSpan=0,secondSpan=0;};
    static constexpr std::size_t blockBytes=24576, bufferBytes=655360;
private:
    struct Row {std::string name;std::size_t offset=0,size=0,cursor=0;bool open=false;};
    const original_main_entry50KG::Application* const issuer_;
    const std::shared_ptr<Provider> provider_;
    OriginalUiEncodedArchiveView50KG input_;
    std::vector<Row> rows_;
    std::array<std::unique_ptr<std::uint8_t[]>,8> inputBuffers_;
    std::unique_ptr<std::uint8_t[]> decoded_;
    std::size_t payload_=0,payloadBytes_=0,cachedBlock_=static_cast<std::size_t>(-1);
    bool staged_=false,pending_=false,released_=false;
    OriginalArchiveStagedReader50LA(const original_main_entry50KG::Application& app,
        std::shared_ptr<Provider> provider) noexcept:issuer_(&app),provider_(std::move(provider)){}
    bool identity(const original_main_entry50KG::Application& app,Diagnostic& d)const noexcept {
        if(&app!=issuer_){d.status=Status::identity;return false;}return true;
    }
    static bool fail(Diagnostic& d,Status status)noexcept {d.status=status;return false;}
    void clear()noexcept {
        std::vector<Row>().swap(rows_);input_={};for(auto& p:inputBuffers_)p.reset();decoded_.reset();
        payload_=payloadBytes_=0;cachedBlock_=static_cast<std::size_t>(-1);staged_=pending_=false;
    }
    bool decodeBlock(std::size_t index,Diagnostic& d)noexcept {
        if(cachedBlock_==index)return true;
        ResourceBlock48C b;
        if(!input_.block(index,b)||!b.compressed_bytes||b.compressed_bytes>blockBytes||
           b.compressed_offset<payload_)return fail(d,Status::input);
        const std::size_t relative=b.compressed_offset-payload_,n=b.compressed_bytes;
        if(relative>=payloadBytes_||n>=payloadBytes_-relative)return fail(d,Status::input);
        // Gather only from staged encoded input buffers, never canonical decoded bytes.
        std::array<std::uint8_t,blockBytes> encoded{};
        std::size_t done=0;
        while(done<n){const auto p=relative+done;const auto chunk=std::min(n-done,bufferBytes-p%bufferBytes);
            std::memcpy(encoded.data()+done,inputBuffers_[p/bufferBytes].get()+p%bufferBytes,chunk);done+=chunk;}
        const auto marker=relative+n;
        if(inputBuffers_[marker/bufferBytes][marker%bufferBytes]!=255)return fail(d,Status::input);
        const bool split=relative/bufferBytes!=(relative+n-1)/bufferBytes;
        // Strict authenticated-block decoding. Original2B0780 ignores errors;
        // this deliberately stronger staging policy is not retail failure parity.
        std::array<std::uint8_t,blockBytes+1> next{};
        z_stream z{};if(inflateInit(&z)!=Z_OK)return fail(d,Status::allocation);
        z.next_in=encoded.data();z.avail_in=static_cast<uInt>(n);
        z.next_out=next.data();z.avail_out=static_cast<uInt>(next.size());
        const int result=inflate(&z,Z_FINISH);
        const bool okay=result==Z_STREAM_END&&z.total_out==blockBytes&&z.total_in==n&&z.avail_in==0;
        inflateEnd(&z);
        if(!okay)return fail(d,result==Z_MEM_ERROR?Status::allocation:Status::decode);
        std::memcpy(decoded_.get(),next.data(),blockBytes);cachedBlock_=index;
        ++d.decodedBlocks;
        if(split){if(!d.splitBlocks){d.firstSplitBlock=index;d.firstSpan=bufferBytes-relative%bufferBytes;d.secondSpan=n-d.firstSpan;}++d.splitBlocks;}
        return true;
    }
public:
    OriginalArchiveStagedReader50LA(const OriginalArchiveStagedReader50LA&)=delete;
    OriginalArchiveStagedReader50LA& operator=(const OriginalArchiveStagedReader50LA&)=delete;
    // RAII storage only; destruction does not call retail named-release or detach
    // actual canonical objects. Immutable provider backing remains independently owned.
    ~OriginalArchiveStagedReader50LA()=default;
    bool stage(const original_main_entry50KG::Application& app,std::size_t mount,Diagnostic& d)noexcept {
        d={};if(!identity(app,d))return false;
        if(released_||staged_)return fail(d,Status::phase);
        OriginalUiEncodedArchiveView50KG input;
        if(!provider_->encodedUiInput(mount,input)||input.providerIdentity()!=provider_.get()||
           !input.identity()||input.identity()->id!=OriginalUiArchiveId50KG::tournament)return fail(d,Status::input);
        ResourceBlock48C first;if(!input.block(0,first)||first.compressed_offset>=input.size())return fail(d,Status::input);
        const std::size_t payload=first.compressed_offset,bytes=input.size()-payload;
        if(bytes>8*bufferBytes)return fail(d,Status::input);
        try {
            std::vector<Row> rows;rows.reserve(input.identity()->memberCount);
            for(std::size_t i=0;i<input.identity()->memberCount;++i){
                const auto* m=originalUiMemberIdentity50KG(input.identity()->id,i);
                if(!m||std::size_t(m->logicalOffset)+m->bytes>input.blockCount()*blockBytes)return fail(d,Status::input);
                rows.push_back({m->name,m->logicalOffset,m->bytes,0,false});
            }
            std::array<std::unique_ptr<std::uint8_t[]>,8> buffers;
            for(auto& p:buffers)p=std::make_unique<std::uint8_t[]>(bufferBytes);
            auto decoded=std::make_unique<std::uint8_t[]>(blockBytes);
            for(std::size_t p=0;p<bytes;){const auto n=std::min(bufferBytes,bytes-p);
                std::memcpy(buffers[p/bufferBytes].get(),input.data()+payload+p,n);p+=n;}
            // All fallible work precedes staging publication. No canonical mutation.
            rows_.swap(rows);inputBuffers_.swap(buffers);decoded_.swap(decoded);input_=std::move(input);
            payload_=payload;payloadBytes_=bytes;staged_=true;return true;
        }catch(...){return fail(d,Status::allocation);}
    }
    bool open(const original_main_entry50KG::Application& app,std::size_t row,Diagnostic& d)noexcept {
        d={};if(!identity(app,d))return false;
        if(released_||!staged_||pending_)return fail(d,Status::phase);
        if(row>=rows_.size()||rows_[row].open)return fail(d,Status::input);
        rows_[row].open=true;rows_[row].cursor=0;return true;
    }
    bool read(const original_main_entry50KG::Application& app,std::size_t row,
        std::uint8_t* destination,std::size_t count,Diagnostic& d)noexcept {
        d={};if(!identity(app,d))return false;
        if(released_||!staged_)return fail(d,Status::phase);
        if(row>=rows_.size()||!rows_[row].open||(!destination&&count))return fail(d,Status::input);
        auto& r=rows_[row];count=std::min(count,r.size-r.cursor);
        while(d.copied<count){const auto position=r.offset+r.cursor;
            if(!decodeBlock(position/blockBytes,d))return false;
            const auto n=std::min(count-d.copied,blockBytes-position%blockBytes);
            std::memcpy(destination+d.copied,decoded_.get()+position%blockBytes,n);
            r.cursor+=n;d.copied+=n;
        }return true;
    }
    bool close(const original_main_entry50KG::Application& app,std::size_t row,Diagnostic& d)noexcept {
        d={};if(!identity(app,d))return false;
        if(released_||!staged_)return fail(d,Status::phase);
        if(row>=rows_.size()||!rows_[row].open)return fail(d,Status::input);
        rows_[row].open=false;rows_[row].cursor=0;return true;
    }
    // This pending bit belongs ONLY to staging. It cannot attest12E6D0 completion:
    // actual cached-object associations remain in the canonical resolver.
    bool requestRetire(const original_main_entry50KG::Application& app,Diagnostic& d)noexcept {
        d={};if(!identity(app,d))return false;
        if(released_||!staged_)return fail(d,Status::phase);
        pending_=true;d.status=Status::pending;return true;
    }
    bool finishRetire(const original_main_entry50KG::Application& app,Diagnostic& d)noexcept {
        d={};if(!identity(app,d))return false;
        if(released_||!staged_||!pending_)return fail(d,Status::phase);
        for(const auto& r:rows_)if(r.open)return fail(d,Status::pending);
        clear();return true;
    }
    bool release(const original_main_entry50KG::Application& app,Diagnostic& d)noexcept {
        d={};if(!identity(app,d))return false;
        if(!released_){clear();released_=true;}return true;
    }
    std::size_t memberCount()const noexcept{return rows_.size();}
    bool pending()const noexcept{return pending_;}
    bool released()const noexcept{return released_;}
    const Provider* providerIdentity()const noexcept{return provider_.get();}
};
inline bool OriginalStandardArchiveProvider50LA::issueStagedReader(
    const original_main_entry50KG::Application& app,const std::shared_ptr<OriginalStandardArchiveProvider50LA>& owner,
    std::shared_ptr<OriginalArchiveStagedReader50LA>& out)noexcept {
    if(owner.get()!=this||out)return false;
    try {out=std::shared_ptr<OriginalArchiveStagedReader50LA>(new OriginalArchiveStagedReader50LA(app,owner));return true;}
    catch(...){return false;}
}
}
