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
#include <memory>
#include <exception>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace gb_animation_binding50GB { class Binding; }
namespace native_racer_contact50HW { class SnapshotAccess; }
namespace mc_animation_binding50MC { class Binding; }
namespace mc_collision_world50MC { class Owner; }
namespace gb_query50GB { class Binding; }
namespace mc_query50MC { class Binding; }
namespace gb_collision_world50GB { class Owner; }
namespace tc_cold_visibility50TC { class Retirement; }
namespace tb_cold_visibility50TB { class Retirement; }
namespace gb_cold_visibility50GB { class Retirement; }
namespace mc_cold_visibility50MC { class Retirement; }
namespace ra_source_camera50LA { class OwnershipBridge; }
namespace original_main_entry50KG {class NativeCollectorFifo50LA;}

namespace native_racer_model50LA {class Owner;}
namespace native_racer50HN {class Bank;}
namespace original_manager_fields {class Access;}
namespace mb_source_trigger50MB {
class Owner;
// Minted only by the actual aggregate after all partial source owners retire.
// Identity is necessary but does not substitute for that owner-specific cleanup.
class RetirementReceipt final {
    friend class Owner;friend class native_racer50HN::Bank;
    const native_racer50HN::Bank* bank_;
    const Owner* issuer_;
    std::uint64_t generation_,serial_;std::size_t slot_;
    RetirementReceipt(const native_racer50HN::Bank& bank,const Owner& issuer,
        std::uint64_t generation,std::uint64_t serial,std::size_t slot)noexcept
        :bank_(&bank),issuer_(&issuer),generation_(generation),serial_(serial),slot_(slot){}
public:
    RetirementReceipt(const RetirementReceipt&)=delete;
    RetirementReceipt& operator=(const RetirementReceipt&)=delete;
};
}
namespace native_racer50HN {
inline constexpr std::size_t maximumRacers=rr::game::maxHostRaceRacers50HT;
class Bank {
    friend class ::original_manager_fields::Access;
public:
    class QueryReservation final {
        friend class Bank;friend class mb_source_trigger50MB::Owner;
        Bank* bank_=nullptr;const mb_source_trigger50MB::Owner* issuer_=nullptr;
        std::uint64_t generation_=0,serial_=0;std::size_t slot_=0;
        QueryReservation()=default;
    public:
        QueryReservation(const QueryReservation&)=delete;
        QueryReservation& operator=(const QueryReservation&)=delete;
        ~QueryReservation()noexcept{if(bank_)std::terminate();}
    };
    bool sourceQueryPending()const noexcept{return pendingToken_!=nullptr;}
    std::size_t sourceQuerySlot()const noexcept{return pendingToken_?pendingToken_->slot_:maximumRacers;}
    Bank()=default;
    ~Bank() noexcept;
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
        if(retired_||!sealed_||visiting_||readBlocked_||pendingToken_||mcAnimationBlocked_||gbAnimationBlocked_||mcQueryActive_||gbQueryActive_||mcTerminalBlocked_||gbTerminalBlocked_)
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
        if(retired_||!sealed_||visiting_||readBlocked_||pendingToken_||mcAnimationBlocked_||gbAnimationBlocked_||mcQueryActive_||gbQueryActive_||mcTerminalBlocked_||gbTerminalBlocked_||index>=count_||handle.owner_!=this||!handle.live_)
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
        if(retired_)throw std::runtime_error("retired native racer bank");
        if(count>std::numeric_limits<std::size_t>::max()/sizeof(T))
            throw std::runtime_error("racer owner extent overflow");
        if(count)add(values,count*sizeof(T));
    }
    template<class T,class A> void contents(std::vector<T,A>& values) {
        elements(values.data(),values.size());
    }
    // Call after all cold owners/pointers are established, before player input.
    // A one-entrant race owns only the real player slot; no opponent is synthesized.
    // Slot zero is the player. Opponents begin from identical cold native state;
    // initialize each slot at its separate host grid position before stepping.
    void seal(std::size_t count) {
        if(retired_||sealed_||spans_.empty()||count<1||count>maximumRacers)
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
        if(retired_||!sealed_||index>=count_||visiting_||pendingToken_||
           mcQueryActive_||gbQueryActive_||(mcTerminalBlocked_&&!mcTerminalVisit_)||(gbTerminalBlocked_&&!gbTerminalVisit_)||
           ((gbAnimationBlocked_&&!gbAnimationVisit_)||(mcAnimationBlocked_&&!mcAnimationVisit_)))
            throw std::runtime_error("invalid or nested native racer visit");
        if(!index){
            // Legacy slot0 visits do not take the swapping lease. Keep their
            // existing nesting semantics, but reject scalar observations from
            // inside the callback just as for a nonzero/batched lease.
            const bool previous=readBlocked_;readBlocked_=true;
            const auto priorSlot=currentSlot_;currentSlot_=0;++visitDepth_;
            struct ReadBlock {
                Bank& bank;bool previous;std::size_t priorSlot;
                ~ReadBlock(){bank.readBlocked_=previous;bank.currentSlot_=priorSlot;--bank.visitDepth_;}
            } readBlock{*this,previous,priorSlot};
            return invokeSourceChecked(std::forward<F>(operation));
        }
        save(0);restore(index);visiting_=true;
        const auto priorSlot=currentSlot_;currentSlot_=index;++visitDepth_;
        struct Restore {
            Bank& bank;std::size_t index,priorSlot;
            ~Restore(){bank.save(index);bank.restore(0);bank.visiting_=false;bank.currentSlot_=priorSlot;--bank.visitDepth_;}
        } restorePlayer{*this,index,priorSlot};
        return invokeSourceChecked(std::forward<F>(operation));
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
        if(retired_||!sealed_||visiting_||pendingToken_||visitCount>count_||
           mcQueryActive_||gbQueryActive_||(mcTerminalBlocked_&&!mcTerminalVisit_)||(gbTerminalBlocked_&&!gbTerminalVisit_)||
           ((gbAnimationBlocked_&&!gbAnimationVisit_)||(mcAnimationBlocked_&&!mcAnimationVisit_)))
            throw std::runtime_error("invalid or nested native racer batch visit");
        for(std::size_t position=0;position<visitCount;++position)
            if(order&&order[position]>=count_)
                throw std::runtime_error("invalid native racer batch slot");
        if(!visitCount)return true;
        visiting_=true;
        const auto priorSlot=currentSlot_;currentSlot_=0;++visitDepth_;
        std::size_t current=0;
        struct Restore {
            Bank& bank;std::size_t& current;std::size_t priorSlot;
            ~Restore(){
                bank.save(current);
                if(current)bank.restore(0);
                bank.visiting_=false;bank.currentSlot_=priorSlot;--bank.visitDepth_;
            }
        } restorePlayer{*this,current,priorSlot};
        for(std::size_t position=0;position<visitCount;++position){
            const auto index=order?order[position]:position;
            if(index!=current){save(current);restore(index);current=index;}
            currentSlot_=index;
            const bool accepted=operation(index);
            if(pendingToken_){
                if(accepted)throw std::runtime_error("source query remains pending after batch callback");
                return false;
            }
            if(!accepted)return false;
        }
        return true;
    }
    template<class F> bool visitAll(F&& operation) {
        return visitMany(nullptr,count_,std::forward<F>(operation));
    }
    // Race gate is supplied by the host countdown/finish owner. A stopped slot
    // remains inspectable/renderable; it is never silently reset or teleported.
    template<class Step> bool step(std::size_t index,bool racing,Step&& operation) {
        if(retired_||pendingToken_||index==0||index>=count_)throw std::runtime_error("invalid AI racer slot");
        if(!racing||!slots_[index].active)return false;
        try {
            const bool advanced=visit(index,std::forward<Step>(operation));
            if(advanced)++slots_[index].completedSteps;
            else slots_[index].active=false;
            return advanced;
        } catch (...) {slots_[index].active=false;throw;}
    }
    void stop(std::size_t index) noexcept {if(!retired_&&index<count_)slots_[index].active=false;}
    bool active(std::size_t index)const noexcept{return !retired_&&index<count_&&slots_[index].active;}
    std::uint64_t completedSteps(std::size_t index)const noexcept{
        return index<count_?slots_[index].completedSteps:0;
    }
    // Explicit native terminal teardown, after all real camera/roster/GPU users.
    // Keep scalar reporting counters, but destroy all borrowed-pointer snapshots
    // and registration spans before releasing the actual RA lifetime owner.
    bool retireNativeBacking()noexcept {
        if(retired_)return true;
        if(visiting_||readBlocked_||visitDepth_||retirementVisit_||mcAnimationVisit_||gbAnimationVisit_||mcTerminalVisit_||gbTerminalVisit_||pendingToken_||sourceIssuer_||nativeFifoPins_||mcAnimationIssuer_||gbAnimationIssuer_||mcQueryIssuer_||gbQueryIssuer_||mcQueryActive_||gbQueryActive_||
           (raWorldLifetime_&&!raWorldLifetime_->canRetire()))return false;
        retired_=true;sealed_=false;
        for(auto& slot:slots_)std::vector<unsigned char>().swap(slot.bytes);
        std::vector<Span>().swap(spans_);
        raWorldLifetime_.reset();gbAnimationBacking_.reset();mcAnimationBacking_.reset();return true;
    }
    std::size_t count()const noexcept{return count_;}
    std::size_t bytesPerRacer()const noexcept{return bytes_;}
private:
    friend class ::gb_animation_binding50GB::Binding;
    friend class ::mc_animation_binding50MC::Binding;
    friend class ::mc_query50MC::Binding;
    const ::mc_query50MC::Binding* mcQueryIssuer_=nullptr;
    bool mcQueryActive_=false,mcTerminalBlocked_=false,mcTerminalVisit_=false;
    friend class ::gb_query50GB::Binding;
    const ::gb_query50GB::Binding* gbQueryIssuer_=nullptr;
    bool gbQueryActive_=false,gbTerminalBlocked_=false,gbTerminalVisit_=false;
    // Only the concrete GB Binding may gate a global pose publication and
    // temporarily enter visitAll while blocked. No generic readiness setter or
    // selected-slot receipt is added. A retained issuer pins backing lifetime
    // until its four moving memberships have retired in every actual arena.
    //Declared before span/snapshot owners so backing retires after them even
    //during normal destruction. Binding close clears only the issuer; later
    //actual Bank retirement releases the concrete aggregate retain.
    // Distinct MC global publication fence. Actual animation graph/motion/RNG
    // memory remains explicitly excluded from per-racer mutable Bank spans.
    std::shared_ptr<::mc_collision_world50MC::Owner> mcAnimationBacking_;
    const ::mc_animation_binding50MC::Binding* mcAnimationIssuer_=nullptr;
    bool mcAnimationBlocked_=false,mcAnimationVisit_=false;
    std::shared_ptr<::gb_collision_world50GB::Owner> gbAnimationBacking_;
    const ::gb_animation_binding50GB::Binding* gbAnimationIssuer_=nullptr;
    bool gbAnimationBlocked_=false,gbAnimationVisit_=false;
    friend class mb_source_trigger50MB::Owner;
    friend class ::tc_cold_visibility50TC::Retirement;
    friend class ::tb_cold_visibility50TB::Retirement;
    friend class ::gb_cold_visibility50GB::Retirement;
    friend class ::mc_cold_visibility50MC::Retirement;
    enum class QueryState:std::uint8_t {idle,active,suspended};
    const mb_source_trigger50MB::Owner* sourceIssuer_=nullptr;
    std::uint64_t sourceGeneration_=0,lastSourceSerial_=0;
    QueryReservation* pendingToken_=nullptr;
    QueryState queryState_=QueryState::idle;
    std::size_t currentSlot_=0,visitDepth_=0;
    bool retirementVisit_=false;
    bool matchesSourceQuery(const QueryReservation& token)const noexcept {
        return pendingToken_==&token&&token.bank_==this&&token.issuer_==sourceIssuer_&&sourceIssuer_&&
            token.generation_==sourceGeneration_&&token.serial_==lastSourceSerial_&&token.slot_<count_;
    }
    bool bindSourceQueryOwner(const mb_source_trigger50MB::Owner& issuer,std::uint64_t generation)noexcept {
        if(retired_||!sealed_||visiting_||readBlocked_||visitDepth_||pendingToken_||sourceIssuer_||!generation)return false;
        sourceIssuer_=&issuer;sourceGeneration_=generation;lastSourceSerial_=0;return true;
    }
    bool unbindSourceQueryOwner(const mb_source_trigger50MB::Owner& issuer,std::uint64_t generation)noexcept {
        if(retired_||visiting_||readBlocked_||visitDepth_||pendingToken_||retirementVisit_||
            sourceIssuer_!=&issuer||sourceGeneration_!=generation)return false;
        sourceIssuer_=nullptr;sourceGeneration_=lastSourceSerial_=0;return true;
    }
    bool beginSourceQuery(const mb_source_trigger50MB::Owner& issuer,
        std::uint64_t generation,std::uint64_t serial,QueryReservation& token)noexcept {
        if(managerFieldsVisit_||retired_||!sealed_||pendingToken_||token.bank_||sourceIssuer_!=&issuer||
            generation!=sourceGeneration_||!serial||serial<=lastSourceSerial_||
            visitDepth_!=1||currentSlot_>=count_||retirementVisit_)return false;
        token.bank_=this;token.issuer_=&issuer;token.generation_=generation;
        token.serial_=serial;token.slot_=currentSlot_;pendingToken_=&token;lastSourceSerial_=serial;
        queryState_=QueryState::active;return true;
    }
    bool suspendSourceQuery(const mb_source_trigger50MB::Owner& issuer,QueryReservation& token)noexcept {
        if(!matchesSourceQuery(token)||token.issuer_!=&issuer||queryState_!=QueryState::active||
            visitDepth_!=1||currentSlot_!=token.slot_||retirementVisit_)return false;
        queryState_=QueryState::suspended;return true;
    }
    void clearSourceQuery(QueryReservation& token)noexcept {
        token.bank_=nullptr;token.issuer_=nullptr;token.generation_=token.serial_=0;
        token.slot_=0;pendingToken_=nullptr;queryState_=QueryState::idle;
    }
    bool completeSourceQuery(const mb_source_trigger50MB::Owner& issuer,QueryReservation& token)noexcept {
        if(!matchesSourceQuery(token)||token.issuer_!=&issuer||queryState_!=QueryState::active||
            visitDepth_!=1||currentSlot_!=token.slot_||retirementVisit_)return false;
        clearSourceQuery(token);return true;
    }
    template<class F> bool visitSuspendedForRetirement(const mb_source_trigger50MB::Owner& issuer,QueryReservation& token,F&& cleanup) {
        if(retired_||!sealed_||!matchesSourceQuery(token)||token.issuer_!=&issuer||queryState_!=QueryState::suspended||
            visiting_||readBlocked_||visitDepth_||retirementVisit_||nativeFifoPins_)
            throw std::runtime_error("invalid suspended source cleanup visit");
        const auto slot=token.slot_;if(slot){save(0);restore(slot);}
        visiting_=true;readBlocked_=true;retirementVisit_=true;currentSlot_=slot;visitDepth_=1;
        struct Restore {
            Bank& bank;std::size_t slot;
            ~Restore(){bank.save(slot);if(slot)bank.restore(0);bank.currentSlot_=0;
                bank.visitDepth_=0;bank.visiting_=bank.readBlocked_=bank.retirementVisit_=false;}
        } restorePlayer{*this,slot};
        const bool accepted=std::forward<F>(cleanup)();
        if(accepted&&pendingToken_)throw std::runtime_error("source cleanup did not retire pending owners");
        return accepted;
    }
    bool retireSuspendedSourceQuery(const mb_source_trigger50MB::Owner& issuer,QueryReservation& token,
        const mb_source_trigger50MB::RetirementReceipt& receipt)noexcept {
        if(!matchesSourceQuery(token)||token.issuer_!=&issuer||queryState_!=QueryState::suspended||
            !retirementVisit_||visitDepth_!=1||currentSlot_!=token.slot_||
            receipt.bank_!=this||receipt.issuer_!=token.issuer_||
            receipt.generation_!=token.generation_||receipt.serial_!=token.serial_||receipt.slot_!=token.slot_)return false;
        clearSourceQuery(token);return true;
    }
    template<class F> decltype(auto) invokeSourceChecked(F&& operation) {
        using Result=std::invoke_result_t<F&&>;
        if constexpr(std::is_same_v<std::remove_cvref_t<Result>,bool>){
            // Preserve bool references as well as ordinary bool values.
            Result result=std::forward<F>(operation)();
            if(pendingToken_&&result)
                throw std::runtime_error("source query remains pending after successful callback");
            return static_cast<Result>(result);
        }else{
            // Direct return preserves references and guaranteed copy elision,
            // including callbacks returning a nonmovable prvalue. The check
            // runs after result construction, but never while unwinding it.
            struct NormalReturnCheck {
                Bank& bank;int exceptions;
                ~NormalReturnCheck() noexcept(false) {
                    if(std::uncaught_exceptions()==exceptions&&bank.pendingToken_)
                        throw std::runtime_error("source query remains pending after callback");
                }
            } check{*this,std::uncaught_exceptions()};
            return std::forward<F>(operation)();
        }
    }
    friend class ra_source_camera50LA::OwnershipBridge;
    friend class ::native_racer_model50LA::Owner; //Private source handling requires exact live slot.
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class ::native_racer_contact50HW::SnapshotAccess;
    std::uint32_t nativeFifoPins_=0;
    // Only the concrete RA bridge can construct/install the derived pin. No
    // public replacement or generic keepalive. Only terminal backing teardown
    // may release this pin early, after its concrete retirement preflight.
    struct RaWorldLifetime {
        const Bank* const identity;
        explicit RaWorldLifetime(const Bank& bank)noexcept:identity(&bank){}
        virtual ~RaWorldLifetime()=default;
        virtual bool canRetire()const noexcept=0;
    };
    // Declared before every span/snapshot: destroyed last. Bank alone strongly
    // owns the pin; its concrete owner retains the actual same-source backing.
    std::shared_ptr<RaWorldLifetime> raWorldLifetime_;
    struct Span {void* address;std::size_t bytes,offset;};
    struct Slot {std::vector<unsigned char> bytes;std::uint64_t completedSteps=0;bool active=false;};
    std::vector<Span> spans_;
    std::array<Slot,maximumRacers> slots_{};
    std::size_t bytes_=0,count_=0;
    bool sealed_=false,visiting_=false,readBlocked_=false,retired_=false;
    bool managerFieldsVisit_=false; // A narrow manager lease cannot start a source query.
    void add(void* address,std::size_t bytes) {
        if(retired_||sealed_||!address||!bytes||bytes>std::numeric_limits<std::size_t>::max()-bytes_)
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
