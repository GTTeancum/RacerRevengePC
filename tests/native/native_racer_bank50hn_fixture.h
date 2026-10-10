#pragma once
// HOST race-session owner, not a recovered original AI/race constructor.
//
// The existing native vehicle/contact pipeline is serial and has many borrowed
// stable-address views. This bank multiplexes that ONE execution arena at whole
// frame boundaries. Each slot stores all registered persistent mutable owner
// bytes; immutable geometry and throwaway scratch remain shared. Restoring into
// the SAME living objects preserves their original internal pointer bindings.
// No captured guest output, generated trajectory, animated marker or guessed
// physics is used: an opponent advances only by executing the native frame.
//
// Do not register a std::vector/string/container object: register its POD backing
// contents or explicitly mutable scalar fields. All registered addresses and
// vector capacities must remain unchanged for the entire bank lifetime. There
// must be no borrowed outstanding reader, list cursor, pending event batch or
// concurrent/reentrant native call when visiting another slot. The caller owns
// the exhaustive registration list in native_racer_owners50hn.inc.
#include "host_race_limits50ht.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace native_racer50HN {
inline constexpr std::size_t maximumRacers=rr::game::maxHostRaceRacers50HT;
class Bank {
public:
    Bank()=default;
    Bank(const Bank&)=delete;
    Bank& operator=(const Bank&)=delete;
    // Cold-bound scalar subrange of one registered owner span. No caller may
    // construct an offset, borrow a snapshot pointer, or read shared noise.
    template<class T> class ReadHandle {
        static_assert(std::is_arithmetic_v<T>||std::is_enum_v<T>,
            "Read handles copy only scalar/enum values, never pointers or owner records");
        friend class Bank;
        const Bank* owner_=nullptr;
        const unsigned char* live_=nullptr;
        std::size_t offset_=0;
    public:
        ReadHandle()=default;
    };
    template<class T> ReadHandle<T> bindRead(const T& value) const {
        if(!sealed_||visiting_||readBlocked_)
            throw std::runtime_error("racer read binding requires a sealed idle bank");
        const auto address=reinterpret_cast<std::uintptr_t>(&value);
        for(const auto& span:spans_){
            const auto first=reinterpret_cast<std::uintptr_t>(span.address);
            if(address<first)continue;
            const auto within=address-first;
            if(within>span.bytes||sizeof(T)>span.bytes-within)continue;
            ReadHandle<T> handle;
            handle.owner_=this;
            handle.live_=reinterpret_cast<const unsigned char*>(&value);
            handle.offset_=span.offset+within;
            return handle;
        }
        throw std::runtime_error("racer read field is not within one registered owner span");
    }
    template<class T> T read(std::size_t index,const ReadHandle<T>& handle) const {
        if(!sealed_||visiting_||readBlocked_||index>=count_||handle.owner_!=this||!handle.live_)
            throw std::runtime_error("invalid or leased native racer scalar read");
        T value;
        // Slot0 may have advanced since its last save. Every nonzero slot is
        // saved at visit/step exit; reads never run inside either arena lease.
        const auto* source=index?slots_[index].bytes.data()+handle.offset_:handle.live_;
        std::memcpy(&value,source,sizeof value);
        return value;
    }
    // Cold registration only. The rejection of overlapping spans catches an
    // accidental duplicate registration of a parent record and one of its fields.
    template<class T> void owner(T& value) {
        static_assert(std::is_trivially_copyable_v<T>,
            "Register POD owner fields/backing spans, never dynamic containers");
        add(&value,sizeof value);
    }
    template<class T> void elements(T* values,std::size_t count) {
        static_assert(std::is_trivially_copyable_v<T>,"Backing elements must be POD");
        if(count>std::numeric_limits<std::size_t>::max()/sizeof(T))
            throw std::runtime_error("racer owner extent overflow");
        if(count)add(values,count*sizeof(T));
    }
    template<class T,class A> void contents(std::vector<T,A>& values) {
        elements(values.data(),values.size());
    }
    // Call after all cold owners/pointers are established, before player input.
    // Slot zero is the player. Opponents begin from identical cold native state;
    // initialize each slot at its separate host grid position before stepping.
    void seal(std::size_t count) {
        if(sealed_||spans_.empty()||count<2||count>maximumRacers)
            throw std::runtime_error("invalid native racer bank seal");
        for(std::size_t i=0;i<count;++i){slots_[i].bytes.resize(bytes_);slots_[i].active=true;}
        count_=count;sealed_=true;
        for(std::size_t i=0;i<count_;++i)save(i);
    }
    // The live arena always belongs to player zero outside visit(). Player may
    // advance normally between visits. Save the latest player before swapping;
    // preserve an opponent's exact partial state on failure and restore player
    // even if the callback throws. No allocation occurs after seal().
    template<class F> decltype(auto) visit(std::size_t index,F&& operation) {
        if(!sealed_||index>=count_||visiting_)
            throw std::runtime_error("invalid or nested native racer visit");
        if(!index){
            // Legacy slot0 visits do not take the swapping lease. Keep their
            // existing nesting semantics, but reject scalar observations from
            // inside the callback just as for a nonzero/batched lease.
            const bool previous=readBlocked_;readBlocked_=true;
            struct ReadBlock {
                Bank& bank;bool previous;
                ~ReadBlock(){bank.readBlocked_=previous;}
            } readBlock{*this,previous};
            return std::forward<F>(operation)();
        }
        save(0);restore(index);visiting_=true;
        struct Restore {
            Bank& bank;std::size_t index;
            ~Restore(){bank.save(index);bank.restore(0);bank.visiting_=false;}
        } restorePlayer{*this,index};
        return std::forward<F>(operation)();
    }
    // One bounded serial lease for an existing whole-fleet observation sweep.
    // A null order means slots0..visitCount-1; otherwise preserve the supplied
    // order exactly. Callback false stops immediately; exceptions propagate.
    // Like visit(), save the current slot even on failure and restore player0
    // before returning. Only the redundant between-peer player swaps vanish.
    // Callbacks may copy observations out but must not retain arena references,
    // visit/step another slot, or depend on a player restore between callbacks.
    // This does not advance any slot's completedSteps or change eligibility.
    template<class F> bool visitMany(const std::size_t* order,
        std::size_t visitCount,F&& operation) {
        if(!sealed_||visiting_||visitCount>count_)
            throw std::runtime_error("invalid or nested native racer batch visit");
        for(std::size_t position=0;position<visitCount;++position)
            if(order&&order[position]>=count_)
                throw std::runtime_error("invalid native racer batch slot");
        if(!visitCount)return true;
        visiting_=true;
        std::size_t current=0;
        struct Restore {
            Bank& bank;std::size_t& current;
            ~Restore(){
                bank.save(current);
                if(current)bank.restore(0);
                bank.visiting_=false;
            }
        } restorePlayer{*this,current};
        for(std::size_t position=0;position<visitCount;++position){
            const auto index=order?order[position]:position;
            if(index!=current){save(current);restore(index);current=index;}
            if(!operation(index))return false;
        }
        return true;
    }
    template<class F> bool visitAll(F&& operation) {
        return visitMany(nullptr,count_,std::forward<F>(operation));
    }
    // Race gate is supplied by the host countdown/finish owner. A stopped slot
    // remains inspectable/renderable; it is never silently reset or teleported.
    template<class Step> bool step(std::size_t index,bool racing,Step&& operation) {
        if(index==0||index>=count_)throw std::runtime_error("invalid AI racer slot");
        if(!racing||!slots_[index].active)return false;
        try {
            const bool advanced=visit(index,std::forward<Step>(operation));
            if(advanced)++slots_[index].completedSteps;
            else slots_[index].active=false;
            return advanced;
        } catch (...) {slots_[index].active=false;throw;}
    }
    void stop(std::size_t index) noexcept {if(index<count_)slots_[index].active=false;}
    bool active(std::size_t index)const noexcept{return index<count_&&slots_[index].active;}
    std::uint64_t completedSteps(std::size_t index)const noexcept{
        return index<count_?slots_[index].completedSteps:0;
    }
    std::size_t count()const noexcept{return count_;}
    std::size_t bytesPerRacer()const noexcept{return bytes_;}
private:
    struct Span {void* address;std::size_t bytes,offset;};
    struct Slot {std::vector<unsigned char> bytes;std::uint64_t completedSteps=0;bool active=false;};
    std::vector<Span> spans_;
    std::array<Slot,maximumRacers> slots_{};
    std::size_t bytes_=0,count_=0;
    bool sealed_=false,visiting_=false,readBlocked_=false;
    void add(void* address,std::size_t bytes) {
        if(sealed_||!address||!bytes||bytes>std::numeric_limits<std::size_t>::max()-bytes_)
            throw std::runtime_error("invalid native racer owner span");
        const auto first=reinterpret_cast<std::uintptr_t>(address);
        if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)
            throw std::runtime_error("native racer owner address overflow");
        for(const auto& span:spans_){const auto other=reinterpret_cast<std::uintptr_t>(span.address);
            if(first<other+span.bytes&&other<first+bytes)
                throw std::runtime_error("overlapping native racer owner spans");}
        spans_.push_back({address,bytes,bytes_});bytes_+=bytes;
    }
    void save(std::size_t index)noexcept {
        for(const auto& span:spans_)std::memcpy(slots_[index].bytes.data()+span.offset,span.address,span.bytes);
    }
    void restore(std::size_t index)noexcept {
        for(const auto& span:spans_)std::memcpy(span.address,slots_[index].bytes.data()+span.offset,span.bytes);
    }
};
}
