#pragma once
#include "original_phy_cursor50la.h"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <limits>
#include <memory>
#include <new>
#include <thread>
#include <utility>
#include <vector>

namespace rr::assets {
// One forward native PHY-load occurrence. This owns real lazy reader work,
// not original Config file00, source157320/1577D0 execution, or guest heap.
// Operation must authenticate its live Bank visit and later Session/racer
// joins; this class authenticates the exact supplied occurrence and backing.
// It is deliberately outside banked snapshots, noncopyable and nonmovable.
class NativePhyLoad50LA final {
public:
    struct Context {
        const void* operation=nullptr;
        const void* bank=nullptr;
        std::uint64_t generation=0;
        std::size_t slot=0;
    };
    enum class Failure {
        none, invalidContext, differentContext, differentThread, invalidBacking,
        differentBacking, notOpened, alreadyAttempted, openFailed, alreadyClosed,
        aborted, activeReaders, readerLimit, inputRejected, hostAllocation,
        hostLength, hostException
    };
private:
    struct NativeState {
        OriginalPhyCursor50LA::NativeCache cache;
        OriginalPhyCursor50LA cursor;
        explicit NativeState(const std::vector<std::uint8_t>& bytes)noexcept
            :cache{},cursor(bytes,cache){}
    };
    Context context_{};
    std::thread::id thread_{};
    // An aliasing pin to the SAME Rig PHY vector, never another equal vector.
    // It outlives cursor/cache and is retained even after explicit close/abort.
    std::shared_ptr<const std::vector<std::uint8_t>> bytes_;
    const std::uint8_t* data_=nullptr;
    std::size_t size_=0;
    std::unique_ptr<NativeState> state_;
    std::size_t readers_=0;
    bool attempted_=false,closed_=false,aborted_=false;
    Failure failure_=Failure::none;

    static bool valid(Context context)noexcept {return context.operation&&context.bank;}
    static bool same(Context a,Context b)noexcept {
        return a.operation==b.operation&&a.bank==b.bank&&
            a.generation==b.generation&&a.slot==b.slot;
    }
    bool fail(Failure failure)noexcept {failure_=failure;return false;}
    Failure liveFailure(Context context,const std::vector<std::uint8_t>& bytes)const noexcept {
        if(!valid(context))return Failure::invalidContext;
        if(!attempted_&&!aborted_)return Failure::notOpened;
        if(!same(context_,context))return Failure::differentContext;
        if(thread_!=std::this_thread::get_id())return Failure::differentThread;
        if(aborted_)return Failure::aborted;
        if(closed_)return Failure::alreadyClosed;
        if(!state_)return Failure::openFailed;
        if(!bytes_||bytes_.get()!=&bytes||bytes.data()!=data_||bytes.size()!=size_)
            return Failure::differentBacking;
        return Failure::none;
    }
    bool acquire(Context context,const std::vector<std::uint8_t>& bytes)noexcept {
        const auto rejected=liveFailure(context,bytes);
        if(rejected!=Failure::none)return fail(rejected);
        if(readers_==std::numeric_limits<std::size_t>::max())return fail(Failure::readerLimit);
        ++readers_;failure_=Failure::none;return true;
    }
    void release()noexcept {
        // Borrowers may not escape their thread or their owner lifetime.
        if(!readers_||thread_!=std::this_thread::get_id())std::terminate();
        --readers_;
    }
    bool leaseCurrent()const noexcept {
        return readers_&&bytes_&&liveFailure(context_,*bytes_)==Failure::none;
    }
    Failure readerFailure()const noexcept {
        if(!state_)return Failure::none;
        switch(state_->cache.failure){
        case OriginalPhyCursor50LA::NativeFailure::none:return Failure::none;
        case OriginalPhyCursor50LA::NativeFailure::inputRejected:return Failure::inputRejected;
        case OriginalPhyCursor50LA::NativeFailure::hostAllocation:return Failure::hostAllocation;
        case OriginalPhyCursor50LA::NativeFailure::hostLength:return Failure::hostLength;
        case OriginalPhyCursor50LA::NativeFailure::hostException:return Failure::hostException;
        }
        return Failure::hostException;
    }
public:
    class Reader final {
        friend class NativePhyLoad50LA;
        NativePhyLoad50LA* owner_=nullptr;
        Reader(NativePhyLoad50LA& owner,Context context,const std::vector<std::uint8_t>& bytes)noexcept {
            if(owner.acquire(context,bytes))owner_=&owner;
        }
        void requireCurrent()const noexcept {
            if(!owner_||!owner_->leaseCurrent())std::terminate();
        }
    public:
        Reader(const Reader&)=delete;
        Reader& operator=(const Reader&)=delete;
        Reader(Reader&&)=delete;
        Reader& operator=(Reader&&)=delete;
        ~Reader(){if(owner_)owner_->release();}
        explicit operator bool()const noexcept {return owner_&&owner_->leaseCurrent();}
        const NativePhyLoad50LA* owner()const noexcept {return *this?owner_:nullptr;}
        // All references, including the cursor itself, are stack borrows only.
        // Do not copy/retain the cursor or selected workspace beyond this Reader.
        // Returned token views alone may survive: they refer to immutable Rig
        // bytes, which stay pinned after the replaceable workspace is released.
        OriginalPhyCursor50LA& cursor()noexcept {requireCurrent();return owner_->state_->cursor;}
        const std::vector<std::uint8_t>& bytes()const noexcept {requireCurrent();return *owner_->bytes_;}
    };

    NativePhyLoad50LA()noexcept=default;
    NativePhyLoad50LA(const NativePhyLoad50LA&)=delete;
    NativePhyLoad50LA& operator=(const NativePhyLoad50LA&)=delete;
    NativePhyLoad50LA(NativePhyLoad50LA&&)=delete;
    NativePhyLoad50LA& operator=(NativePhyLoad50LA&&)=delete;
    ~NativePhyLoad50LA(){
        if(readers_)std::terminate();
        // Abort cleanup only, never a successful native outer-close receipt.
        state_.reset();
    }

    bool open(Context context,std::shared_ptr<const std::vector<std::uint8_t>> bytes)noexcept {
        if(attempted_||aborted_)return fail(Failure::alreadyAttempted);
        attempted_=true;context_=context;thread_=std::this_thread::get_id();
        if(!valid(context))return fail(Failure::invalidContext);
        if(!bytes||bytes.use_count()==0)return fail(Failure::invalidBacking);
        bytes_=std::move(bytes);data_=bytes_->data();size_=bytes_->size();
        // No all-document parse, selection seed, guest open, or previous-work
        // receipt. Existing bulk readers and future cursor calls do actual work.
        try {state_=std::make_unique<NativeState>(*bytes_);}
        catch(const std::bad_alloc&){return fail(Failure::hostAllocation);}
        catch(const std::length_error&){return fail(Failure::hostLength);}
        catch(...){return fail(Failure::hostException);}
        failure_=Failure::none;return true;
    }
    bool current(Context context,const std::vector<std::uint8_t>& bytes)const noexcept {
        return liveFailure(context,bytes)==Failure::none;
    }
    Reader borrow(Context context,const std::vector<std::uint8_t>& bytes)noexcept {
        return Reader(*this,context,bytes); // C++17 guaranteed elision; no heap.
    }
    bool close(Context context,const std::vector<std::uint8_t>& bytes)noexcept {
        const auto rejected=liveFailure(context,bytes);
        if(rejected!=Failure::none)return fail(rejected);
        if(readers_)return fail(Failure::activeReaders);
        failure_=readerFailure();
        state_.reset();closed_=true;return true;
    }
    // Terminal native cleanup, including failed open and never-opened slots.
    // Repeated cleanup is allowed; no retry/reopen and no successful close is
    // manufactured. Existing attempted/closed history and failures survive.
    bool abort(Context context)noexcept {
        if(!valid(context))return fail(Failure::invalidContext);
        if(attempted_||aborted_||closed_){
            if(!same(context_,context))return fail(Failure::differentContext);
            if(thread_!=std::this_thread::get_id())return fail(Failure::differentThread);
        }else{context_=context;thread_=std::this_thread::get_id();}
        if(readers_)return fail(Failure::activeReaders);
        if(failure_==Failure::none)failure_=readerFailure();
        state_.reset();
        if(!closed_)aborted_=true;
        return true;
    }
    bool attempted()const noexcept {return attempted_;}
    bool opened()const noexcept {return state_&&!closed_&&!aborted_;}
    bool closed()const noexcept {return closed_;}
    bool aborted()const noexcept {return aborted_;}
    std::size_t activeReaders()const noexcept {return readers_;}
    Failure failure()const noexcept {return failure_==Failure::none?readerFailure():failure_;}
};
}
