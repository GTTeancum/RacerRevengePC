#pragma once
// Native scalar trigger entry/lifecycle recovered from original 2A00A0,
// 2A0E20, 2A0B30,124010/124100 and141520/141560. No query geometry,
// guest memory, RNG generator, allocation, fabricated contact or sound.
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rr::game {
inline constexpr std::size_t triggerContactCapacity50IN=32;
inline constexpr std::size_t triggerActionCapacity50IN=32;
// Opaque native identities. Zero is the source null slot. Each live collider
// has a unique nonzero identity and its own reciprocal CC/D0 list.
struct TriggerColliderContacts50IN {
    std::uint32_t identity=0;
    std::array<std::uint32_t,triggerContactCapacity50IN> membersCC{};
    std::size_t highWaterD0=0;
};
// Fixed allocation with a cold-selected live prefix. Tail capacity is not a
// fabricated racer or collider, and cannot participate in membership walks.
template<std::size_t Capacity> class TriggerColliderStorage50IN final {
    std::array<TriggerColliderContacts50IN,Capacity> storage_{};
    std::size_t count_=Capacity;
    bool selected_=false;
public:
    bool setCount(std::size_t count) noexcept {
        if(selected_||!count||count>Capacity)return false;
        for(const auto& c:storage_){
            if(c.identity||c.highWaterD0)return false;
            for(const auto id:c.membersCC)if(id)return false;
        }
        count_=count;selected_=true;return true;
    }
    std::size_t size()const noexcept{return count_;}
    bool empty()const noexcept{return count_==0;}
    auto data()noexcept{return storage_.data();}
    auto data()const noexcept{return storage_.data();}
    auto begin()noexcept{return storage_.begin();}
    auto end()noexcept{return storage_.begin()+count_;}
    auto begin()const noexcept{return storage_.begin();}
    auto end()const noexcept{return storage_.begin()+count_;}
    TriggerColliderContacts50IN& operator[](std::size_t i)noexcept{return storage_[i];}
    const TriggerColliderContacts50IN& operator[](std::size_t i)const noexcept{return storage_[i];}
    TriggerColliderContacts50IN& at(std::size_t i){if(i>=count_)throw std::out_of_range("trigger collider index");return storage_[i];}
    const TriggerColliderContacts50IN& at(std::size_t i)const {if(i>=count_)throw std::out_of_range("trigger collider index");return storage_[i];}
};
struct TriggerEntryState50IN {
    std::uint8_t flags29=3;
    // bit0 exits,bit1 enters,bit2 OneTimeOnly,bit3 fired,bit4 touched,
    // bit5 previous service interval's touched. Higher bits preserved.
    std::uint8_t flags1B8=3;
    std::int32_t randomTriggerPercent90=100;
    std::size_t callbackCount98=0;
    std::uint32_t lastCollider1B4=0;
    TriggerColliderContacts50IN collider;
};
struct TriggerActionCall50IN {
    std::uint32_t triggerCollider=0,otherCollider=0;
    std::size_t actionIndex=0;
    bool entering=false;
    std::uint32_t sourceSite=0;
};
struct TriggerActionHooks50IN {
    void* context=nullptr;
    // Preflight is pure and must not depend on effects of an earlier action.
    // The caller proves all action destinations and provider capacity here.
    bool (*preflight)(void*,const TriggerActionCall50IN&) noexcept=nullptr;
    // After successful preflight apply is infallible. It must not mutate this
    // trigger, its memberships, hooks, or lifecycle observation storage.
    void (*apply)(void*,const TriggerActionCall50IN&) noexcept=nullptr;
};
enum class TriggerEntryStatus50IN:std::uint8_t { Complete,Input,Capacity,RandomInput,ActionInput };
struct TriggerEntryReceipt50IN {
    TriggerEntryStatus50IN status=TriggerEntryStatus50IN::Input;
    std::size_t callbacks=0,addedPairs=0,removedPairs=0;
    bool randomConsumed=false,serviceRan=false;
};
struct TriggerOverlap50IN {
    // One observation per slot below D0, preserving null holes and slot order.
    // Null slot requires collider=nullptr; overlap is ignored in a null slot.
    TriggerColliderContacts50IN* collider=nullptr;
    bool overlap=false;
};
namespace trigger_entry50IN_detail {
inline bool valid(const TriggerColliderContacts50IN&s)noexcept {
    if(!s.identity||s.highWaterD0>s.membersCC.size())return false;
    for(std::size_t i=0;i<s.highWaterD0;++i){
        const auto id=s.membersCC[i];if(id==s.identity)return false;
        if(id)for(std::size_t j=0;j<i;++j)if(s.membersCC[j]==id)return false;
    }
    // Slots beyond the original high-water count are unallocated, not inputs.
    return true;
}
inline bool valid(const TriggerEntryState50IN&s)noexcept {
    return valid(s.collider)&&s.callbackCount98<=triggerActionCapacity50IN&&
        s.randomTriggerPercent90>=0&&s.randomTriggerPercent90<=100;
}
inline std::size_t find(const TriggerColliderContacts50IN&s,std::uint32_t id)noexcept {
    for(std::size_t i=0;i<s.highWaterD0;++i)if(s.membersCC[i]==id)return i;
    return s.highWaterD0;
}
inline bool room(const TriggerColliderContacts50IN&s,std::uint32_t id)noexcept {
    return find(s,id)<s.highWaterD0||find(s,0)<s.highWaterD0||s.highWaterD0<s.membersCC.size();
}
inline void add(TriggerColliderContacts50IN&s,std::uint32_t id)noexcept {
    if(find(s,id)<s.highWaterD0)return;
    const auto hole=find(s,0);
    if(hole<s.highWaterD0)s.membersCC[hole]=id;
    else s.membersCC[s.highWaterD0++]=id;
}
inline void remove(TriggerColliderContacts50IN&s,std::uint32_t id)noexcept {
    const auto slot=find(s,id);if(slot<s.highWaterD0)s.membersCC[slot]=0;
}
inline bool preflight(const TriggerEntryState50IN&s,std::uint32_t other,bool enter,
    const TriggerActionHooks50IN&hooks)noexcept {
    if(!s.callbackCount98)return true;
    if(!hooks.preflight||!hooks.apply)return false;
    for(std::size_t i=0;i<s.callbackCount98;++i)
        if(!hooks.preflight(hooks.context,{s.collider.identity,other,i,enter,enter?0x2a0eccu:0x2a0d20u}))return false;
    return true;
}
inline std::size_t apply(const TriggerEntryState50IN&s,std::uint32_t other,bool enter,
    const TriggerActionHooks50IN&hooks)noexcept {
    for(std::size_t i=0;i<s.callbackCount98;++i)
        hooks.apply(hooks.context,{s.collider.identity,other,i,enter,enter?0x2a0eccu:0x2a0d20u});
    return s.callbackCount98;
}
}
// Call only for a backend-admitted geometric hit after appropriate filters.
// Source parity requires the actual native narrow-query hit route; an explicit
// HOST adapter using this scalar helper remains HOST-owned and must say so.
// randomPercent supplies the source provider's already-converted integer1..100
// only when required. With percent100 it is not inspected or consumed. Missing
// RNG/provider/capacity produces no state writes or callback effects; retry the
// same contact with the required input. Original successful write/call order is
// maintained, including callbacks BEFORE both reciprocal membership additions.
inline TriggerEntryReceipt50IN triggerContact50IN(TriggerEntryState50IN&s,
    TriggerColliderContacts50IN&other,const TriggerActionHooks50IN&hooks,
    std::int32_t randomPercent=0)noexcept {
    namespace d=trigger_entry50IN_detail;TriggerEntryReceipt50IN r;
    if(!d::valid(s)||!d::valid(other)||&s.collider==&other||s.collider.identity==other.identity)return r;
    if(!(s.flags29&1)){r.status=TriggerEntryStatus50IN::Complete;return r;}
    const auto touched=static_cast<std::uint8_t>(s.flags1B8|0x10u);
    bool admitted=!((touched&4)&&(touched&8));
    if(admitted&&s.randomTriggerPercent90!=100){
        if(touched&0x20)admitted=false;
        else {
            if(randomPercent<1||randomPercent>100){r.status=TriggerEntryStatus50IN::RandomInput;return r;}
            r.randomConsumed=true;admitted=randomPercent<=s.randomTriggerPercent90;
        }
    }
    if(!admitted){s.flags1B8=touched;r.status=TriggerEntryStatus50IN::Complete;return r;}
    const bool fresh=d::find(s.collider,other.identity)==s.collider.highWaterD0;
    const bool callbacks=fresh&&(touched&2);
    if(fresh&&(!d::room(s.collider,other.identity)||!d::room(other,s.collider.identity))){
        r.randomConsumed=false;r.status=TriggerEntryStatus50IN::Capacity;return r;
    }
    if(callbacks&&!d::preflight(s,other.identity,true,hooks)){
        r.randomConsumed=false;r.status=TriggerEntryStatus50IN::ActionInput;return r;
    }
    s.flags1B8=static_cast<std::uint8_t>(touched|8u);
    s.lastCollider1B4=other.identity;
    if(fresh){
        if(callbacks)r.callbacks=d::apply(s,other.identity,true,hooks);
        d::add(s.collider,other.identity);d::add(other,s.collider.identity);r.addedPairs=1;
    }
    r.status=TriggerEntryStatus50IN::Complete;return r;
}
// Scalar portion of original2A0B30. The caller supplies exact122BA0 coarse
// overlaps at the original world-service boundary, AFTER racer queries and
// BEFORE this trigger's child service. Parent transforms/child updates remain
// the caller's responsibility. active29 bit0 and bits2/3 model141ED0's child
// scheduling gates. serviceEligible supplies the parent service's mask0x2
// registration (2DCC20 uses0x402); it is NOT collision membership method1/2.
// All observations/reciprocal lists and every exit callback are preflighted
// before writes. Successful prune order is reciprocal remove,own remove,
// explicit own-slot null, then ordered exit callbacks. D0 never decreases.
inline TriggerEntryReceipt50IN serviceTrigger50IN(TriggerEntryState50IN&s,
    const TriggerOverlap50IN*observations,std::size_t observationCount,
    const TriggerActionHooks50IN&hooks,bool serviceEligible=true)noexcept {
    namespace d=trigger_entry50IN_detail;TriggerEntryReceipt50IN r;
    if(!d::valid(s))return r;
    if(!serviceEligible||!(s.flags29&1)||(s.flags29&0x0c)){
        r.status=TriggerEntryStatus50IN::Complete;return r;
    }
    if(observationCount!=s.collider.highWaterD0||(observationCount&&!observations))return r;
    for(std::size_t i=0;i<observationCount;++i){
        const auto id=s.collider.membersCC[i];const auto*other=observations[i].collider;
        if(!id){if(other)return r;continue;}
        if(!other||other==&s.collider||!d::valid(*other)||other->identity!=id)return r;
        if(!observations[i].overlap&&(s.flags1B8&1)&&!d::preflight(s,id,false,hooks)){
            r.status=TriggerEntryStatus50IN::ActionInput;return r;
        }
    }
    s.flags1B8=static_cast<std::uint8_t>((s.flags1B8&~0x30u)|((s.flags1B8&0x10u)<<1));
    r.serviceRan=true;
    for(std::size_t i=0;i<observationCount;++i){
        auto*other=observations[i].collider;if(!other||observations[i].overlap)continue;
        const auto id=other->identity;
        d::remove(*other,s.collider.identity);d::remove(s.collider,id);s.collider.membersCC[i]=0;
        ++r.removedPairs;
        if(s.flags1B8&1)r.callbacks+=d::apply(s,id,false,hooks);
    }
    r.status=TriggerEntryStatus50IN::Complete;return r;
}
// Scalar own-object effects of141520/141560. Descendant enable/disable calls
// remain the enclosing owner's responsibility. Neither clears membership,
// lastCollider,OneTimeOnly,fired or touched interval state.
inline void setTriggerEnabled50IN(TriggerEntryState50IN&s,bool enabled)noexcept {
    s.flags29=static_cast<std::uint8_t>((s.flags29&~3u)|(enabled?3u:0u));
}
}
