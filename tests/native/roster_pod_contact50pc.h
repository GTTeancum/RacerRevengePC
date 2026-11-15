#pragma once
// Step R (work\scratch\pod-contact\PLAN.md): the original in-query pod-versus-
// pod response, one common path for every course and every pod.
//
// The original has no separate pod pass. Another pod's 694 row is one more
// collected candidate; the visited racer's own 294BB0 / 219420 / 2964C0 write
// BOTH bodies inside that racer's frame (1A83D0 manager order). In this port the
// other racer's mutable state lives in its saved Bank slot while another racer
// is visited, so the ONLY way this file writes another racer is SlotAccess:
//  * otherWritable below is the one place that names the fields of ANOTHER
//    racer the original writes (165430 +20C/+210, 1654C0 +238/+23C, 296C10 /
//    297190 / 297390 / 297610 root loads, velocities and accumulators);
//  * every write is resolved to that slot's saved bytes and bounds-checked
//    against the field's bound span AND that slot's saved size before it
//    happens; a write that does not fit is refused, not done and reported;
//  * the visited racer's own state is never written here (it is the live arena).
// Immutable pod geometry (694 row type0 resource, membership bounds) is the one
// shared world's, read from track storage, never from a Bank slot.
//
// Inert until step R5: while the PORT STAND-IN other-pod694 exclusion list
// (track_runtime_storage50IG::writeRosterPodExclusionStandIn) is in force, no
// collected list contains a pod and nothing here writes anything.
#include "native_racer_bank50hn_fixture.h"
#include "track_runtime_storage50ig_fixture.h"
#include "../../src/game/vehicle_frame49w.h"
#include "../../src/game/body_contact_list48r.h"
#include "../../src/game/vehicle_contact48r.h"
#include "../../src/game/spring_records48n.h"
#include "../../src/game/spring_callback48n.h"
#include "../../src/game/type0_bounds50ew.h"
#include "../../src/game/original_racer_main_body_owner50la.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <type_traits>

namespace roster_pod_contact50PC {
using namespace rr::game;
inline constexpr std::size_t maximumRacers=native_racer50HN::maximumRacers;
inline constexpr std::size_t maximumOthers=maximumRacers-1;
using Storage=track_runtime_storage50IG::Storage;
using RosterObject=std::remove_pointer_t<decltype(Storage::RowRef::row)>;

// THE ONE PLACE: fields of ANOTHER racer an in-query pod response may write.
// Main body only: the target of a pod694 candidate is that pod's body A0
// (main340), for the visited racer's main AND secondary lanes.
enum class OtherField : std::uint8_t {
    count20C,partners210,              //165430 (vtbl3CE000+98) on the other body
    count238,partners23C,              //1654C0 (vtbl+A0) on the other body
    velocity70,angularVelocityB0,loads,//296C10 / 297610 recipient, other side
    positionCorrection1A0,rotationCorrection1B0,accumulator1C0,accumulator1D0
};
inline constexpr std::size_t otherFieldCount=11;
inline const char* otherFieldName(OtherField f) noexcept {
    constexpr const char* names[otherFieldCount]={"count20C","partners210","count238","partners23C",
        "velocity70","angularVelocityB0","loads","positionCorrection1A0","rotationCorrection1B0",
        "accumulator1C0","accumulator1D0"};
    const auto i=static_cast<std::size_t>(f);return i<otherFieldCount?names[i]:"unknown";
}

struct Totals {
    std::uint64_t classified=0,mergedPods=0,mergedOtherRows=0,firstPods=0;
    std::uint64_t dynamicHits=0,suppressedReverse=0,recordsOther44=0,contact219420=0;
    std::uint64_t twoBodyResponses=0,otherWrites=0,refusedWrites=0,staticPodCategory219420=0;
    std::uint64_t pairLines=0,writeLines=0,refusalLines=0;
};
inline Totals& totals() noexcept {static Totals t;return t;}
inline void reportSummary(std::ostream& out,std::uint64_t frame) {
    const auto& t=totals();
    out<<"pod_contact_summary frame="<<frame<<" classified="<<t.classified<<" merged_pods="<<t.mergedPods
       <<" merged_other_rows="<<t.mergedOtherRows<<" first_pods="<<t.firstPods
       <<" dynamic_hits="<<t.dynamicHits<<" suppressed_reverse="<<t.suppressedReverse
       <<" records_other44="<<t.recordsOther44<<" contact219420="<<t.contact219420
       <<" static_pod_category219420="<<t.staticPodCategory219420
       <<" two_body_responses="<<t.twoBodyResponses<<" other_writes="<<t.otherWrites
       <<" refused_writes="<<t.refusedWrites<<'\n';
}
// First 20 per process, then every 200th.
inline void reportPair(const char* path,const char* lane,std::size_t primary,std::size_t other,
    std::size_t records,std::uint32_t material) {
    auto& t=totals();const auto n=t.pairLines++;
    if(n>=20&&n%200)return;
    std::cerr<<"pod_contact_pair n="<<n<<" path="<<path<<" lane="<<lane<<" primary="<<primary
        <<" other="<<other<<" records="<<records<<" material="<<material<<'\n';
}

class SlotAccess final {
    struct Field {const unsigned char* live=nullptr;std::size_t bytes=0,offset=0;bool bound=false;};
    native_racer50HN::Bank& bank_;
    std::size_t current_=0;
    bool ready_=false;
    std::array<Field,otherFieldCount> writable_{};
    Field root_,bodyParameters_,centerOffset_,phase_;
    template<class T> Field bind(const T& value) const noexcept {
        static_assert(std::is_trivially_copyable_v<T>);
        const auto address=reinterpret_cast<std::uintptr_t>(&value);
        for(const auto& span:bank_.spans_){
            const auto begin=reinterpret_cast<std::uintptr_t>(span.address);
            if(address<begin)continue;
            const auto within=address-begin;
            if(within<=span.bytes&&sizeof value<=span.bytes-within)
                return {reinterpret_cast<const unsigned char*>(&value),sizeof value,span.offset+within,true};
        }
        return {};
    }
    // The visited racer is current_; every other slot's mutable bytes are its
    // saved Bank bytes (slot0 is saved before a nonzero visit swaps it out).
    bool leaseCurrent() const noexcept {
        const auto& b=bank_;
        return ready_&&!b.retired_&&b.sealed_&&current_<b.count_&&b.currentSlot_==current_&&
            b.visitDepth_<=1&&!b.pendingToken_&&!b.retirementVisit_&&(current_==0||b.visiting_);
    }
    unsigned char* at(std::size_t slot,const Field& f,std::size_t within,std::size_t bytes) noexcept {
        if(!leaseCurrent()||slot>=bank_.count_||slot==current_||!f.bound)return nullptr;
        if(within>f.bytes||bytes>f.bytes-within)return nullptr;          //inside the bound field
        auto& saved=bank_.slots_[slot].bytes;
        if(f.offset>saved.size()||within>saved.size()-f.offset||
           bytes>saved.size()-f.offset-within)return nullptr;            //inside that slot's saved bytes
        return saved.data()+f.offset+within;
    }
    void refuse(std::size_t slot,OtherField f,std::size_t within,std::size_t bytes) noexcept {
        auto& t=totals();++t.refusedWrites;
        if(t.refusalLines++<20)std::cerr<<"pod_contact_write_refused visited="<<current_<<" slot="<<slot
            <<" field="<<otherFieldName(f)<<" within="<<within<<" bytes="<<bytes<<'\n';
    }
public:
    // Binds field offsets only (spans are fixed once sealed); allowed inside
    // the visited racer's lease. live/parameters are the ONE execution arena.
    SlotAccess(native_racer50HN::Bank& bank,const VehicleFrameState49W& live,
        const VehicleFrameParameters49W& parameters,std::size_t visited) noexcept
        :bank_(bank),current_(visited) {
        static_assert(sizeof(live.bodies.main.components.body20C)==sizeof(BodyContactListState48R::count20C));
        static_assert(sizeof(live.mainContactPartners210)==sizeof(BodyContactListState48R::slots210));
        static_assert(sizeof(live.bodies.main.components.root.callbackState238)==sizeof(BodyContactListState48R::count20C));
        static_assert(sizeof(live.bodies.main.components.root.processedContactPartners23C)==sizeof(BodyContactListState48R::slots210));
        if(bank.retired_||!bank.sealed_)return;
        const auto& root=live.bodies.main.components.root;
        writable_={bind(live.bodies.main.components.body20C),bind(live.mainContactPartners210),
            bind(root.callbackState238),bind(root.processedContactPartners23C),
            bind(root.body.velocity),bind(root.body.angularVelocity),bind(root.loads),
            bind(root.positionCorrection1a0),bind(root.rotationCorrection1b0),
            bind(root.accumulator1c0),bind(root.accumulator1d0)};
        root_=bind(root);bodyParameters_=bind(parameters.bodies.main.body);
        centerOffset_=bind(parameters.bodies.main.centerOffset);phase_=bind(live.phase);
        bool all=root_.bound&&bodyParameters_.bound&&centerOffset_.bound&&phase_.bound;
        for(const auto& f:writable_)all=all&&f.bound;
        ready_=all;
    }
    SlotAccess(const SlotAccess&)=delete;SlotAccess& operator=(const SlotAccess&)=delete;
    bool ready() const noexcept{return leaseCurrent();}
    std::size_t visited() const noexcept{return current_;}
    bool fits(std::size_t slot,OtherField f,std::size_t within,std::size_t bytes) noexcept {
        const auto i=static_cast<std::size_t>(f);
        return i<otherFieldCount&&at(slot,writable_[i],within,bytes)!=nullptr;
    }
    bool readField(std::size_t slot,OtherField f,std::size_t within,void* out,std::size_t bytes) noexcept {
        const auto i=static_cast<std::size_t>(f);if(i>=otherFieldCount)return false;
        const auto* p=at(slot,writable_[i],within,bytes);if(!p)return false;
        std::memcpy(out,p,bytes);return true;
    }
    // The ONLY write into another racer. Refused (nothing written) unless it fits.
    bool write(std::size_t slot,OtherField f,std::size_t within,const void* in,std::size_t bytes) noexcept {
        const auto i=static_cast<std::size_t>(f);
        auto* p=i<otherFieldCount?at(slot,writable_[i],within,bytes):nullptr;
        if(!p||!in){refuse(slot,f,within,bytes);return false;}
        std::memcpy(p,in,bytes);auto& t=totals();++t.otherWrites;
        if(t.writeLines++<20)std::cerr<<"pod_contact_cross_write visited="<<current_<<" slot="<<slot
            <<" field="<<otherFieldName(f)<<" within="<<within<<" bytes="<<bytes<<'\n';
        return true;
    }
    // Other body for 2964C0, read-only. Earlier manager slots are Ready, later
    // ones AfterBodyChildren (two-phase schedule); both are frame-boundary states.
    bool gatherBody(std::size_t slot,std::uint32_t identity,SpringModelBody48K& out) noexcept {
        VehicleFramePhase49W phase{};
        const auto* p=at(slot,phase_,0,sizeof phase);if(!p)return false;std::memcpy(&phase,p,sizeof phase);
        if(phase!=VehicleFramePhase49W::Ready&&phase!=VehicleFramePhase49W::AfterBodyChildren)return false;
        const auto* r=at(slot,root_,0,sizeof out.body.state);
        const auto* m=at(slot,bodyParameters_,0,sizeof out.body.parameters);
        const auto* c=at(slot,centerOffset_,0,sizeof out.body.localCenter);
        if(!identity||!r||!m||!c)return false;
        out.identity=identity;std::memcpy(&out.body.state,r,sizeof out.body.state);
        std::memcpy(&out.body.parameters,m,sizeof out.body.parameters);
        std::memcpy(&out.body.localCenter,c,sizeof out.body.localCenter);return true;
    }
    // The 2964C0 response fields of one other body (296C10 loads, 297610
    // impulses/accumulators): the same members the host join committed.
    struct BodyItem {OtherField f;const void* p;std::size_t n;};
    static std::array<BodyItem,7> bodyItems(const RootBodyState48H& s) noexcept {
        return {{{OtherField::velocity70,&s.body.velocity,sizeof s.body.velocity},
            {OtherField::angularVelocityB0,&s.body.angularVelocity,sizeof s.body.angularVelocity},
            {OtherField::loads,&s.loads,sizeof s.loads},
            {OtherField::positionCorrection1A0,&s.positionCorrection1a0,sizeof s.positionCorrection1a0},
            {OtherField::rotationCorrection1B0,&s.rotationCorrection1b0,sizeof s.rotationCorrection1b0},
            {OtherField::accumulator1C0,&s.accumulator1c0,sizeof s.accumulator1c0},
            {OtherField::accumulator1D0,&s.accumulator1d0,sizeof s.accumulator1d0}}};
    }
    // Call BEFORE the primary's response commits: proves every later write fits.
    bool fitsBody(std::size_t slot) noexcept {
        const RootBodyState48H shape{};
        for(const auto& i:bodyItems(shape))if(!fits(slot,i.f,0,i.n)){refuse(slot,i.f,0,i.n);return false;}
        return true;
    }
    // Every field is checked before the first write, so no other body is
    // ever partially committed.
    bool commitBody(std::size_t slot,const SpringModelBody48K& after) noexcept {
        const auto items=bodyItems(after.body.state);
        if(!fitsBody(slot))return false;
        for(const auto& i:items)if(!write(slot,i.f,0,i.p,i.n))return false;
        return true;
    }
};

// Signed 165430/1654C0 list on ANOTHER body: append `identity` if absent and
// not full. Only the new slot and the count are written (all other slots keep
// their bits), both checked before either write.
inline bool appendOtherList(SlotAccess& a,std::size_t slot,OtherField countField,OtherField slotsField,
    BodyContactIdentity48R identity,BodyContactListStatus48R& status) noexcept {
    BodyContactListState48R list;
    if(!a.readField(slot,countField,0,&list.count20C,sizeof list.count20C)||
       !a.readField(slot,slotsField,0,list.slots210.data(),sizeof list.slots210))return false;
    const auto before=list.count20C;
    status=applyBodyContactList48R(identity,list);
    if(status==BodyContactListStatus48R::InvalidInput)return false;
    if(status!=BodyContactListStatus48R::Appended)return true;
    const auto within=static_cast<std::size_t>(before)*sizeof(BodyContactIdentity48R);
    if(!a.fits(slot,slotsField,within,sizeof(BodyContactIdentity48R))||
       !a.fits(slot,countField,0,sizeof list.count20C))return false;
    return a.write(slot,slotsField,within,&list.slots210[static_cast<std::size_t>(before)],sizeof(BodyContactIdentity48R))&&
        a.write(slot,countField,0,&list.count20C,sizeof list.count20C);
}
// The visited racer's own body lists (live arena). Main lane: main root + main
// 20C/210; secondary lane: secondary root + secondary 20C/210.
struct OwnBody {
    BodyContactIdentity48R identity{};
    RootBodyState48H* root=nullptr;
    std::uint32_t* count20C=nullptr;
    BodyContactIdentity48R* partners210=nullptr;
};
inline bool appendOwnContact165430(OwnBody& own,BodyContactIdentity48R other) noexcept {
    if(!own.count20C||!own.partners210)return false;
    BodyContactListState48R list;std::memcpy(&list.count20C,own.count20C,sizeof list.count20C);
    std::memcpy(list.slots210.data(),own.partners210,sizeof list.slots210);
    const auto status=applyBodyContactList48R(other,list);
    if(status==BodyContactListStatus48R::InvalidInput)return false;
    std::memcpy(own.count20C,&list.count20C,sizeof list.count20C);
    std::memcpy(own.partners210,list.slots210.data(),sizeof list.slots210);return true;
}
// 165540 (vtbl+A4): scans every entry for signed count>0, no early exit.
// Above ten is not bounded backing: refused.
inline bool ownProcessedContains165540(const OwnBody& own,BodyContactIdentity48R other,bool& found) noexcept {
    if(!own.root)return false;
    std::int32_t count;std::memcpy(&count,&own.root->callbackState238,sizeof count);
    if(count>static_cast<std::int32_t>(BodyContactCapacity48R))return false;
    bool match=false;
    for(std::int32_t i=0;i<count;++i)if(own.root->processedContactPartners23C[static_cast<std::size_t>(i)].value==other.value)match=true;
    found=match;return true;
}

// Typed view of one collected list. Static ordinals are already proven by the
// caller's own resolve (registeredIndices); a roster row is matched by its
// actual identity in that slot's own row. No sort, merge or deduplication.
enum class SelectedKind : std::uint8_t {staticCandidate,pod694,otherRoster};
struct Selected {SelectedKind kind=SelectedKind::staticCandidate;std::uint32_t identity=0;std::size_t index=0,slot=0;};
struct SelectedList {
    std::array<Selected,track_runtime_storage50IG::sourceSelectionCapacity> entries{};
    std::size_t count=0,pods=0,otherRows=0;
};
inline bool classify(Storage& storage,std::size_t roster,const Type2StaticCandidate49X* candidates,
    std::size_t candidateCount,const std::uint32_t* ids,std::size_t count,const std::size_t* staticIndices,
    std::size_t staticCount,SelectedList& out) noexcept {
    out={};if(!ids||count>out.entries.size()||roster>maximumRacers)return false;
    std::size_t nextStatic=0;
    for(std::size_t n=0;n<count;++n){
        const auto id=ids[n];auto& e=out.entries[n];e.identity=id;
        if(nextStatic<staticCount&&staticIndices&&staticIndices[nextStatic]<candidateCount&&candidates&&
           candidates[staticIndices[nextStatic]].candidate.collision.filter.identity==id){
            e.kind=SelectedKind::staticCandidate;e.index=staticIndices[nextStatic++];continue;
        }
        bool found=false;
        for(std::size_t slot=0;slot<roster&&!found;++slot)
            for(std::size_t r=0;r<Storage::rosterRoles&&!found;++r){
                const auto role=static_cast<Storage::RosterRole>(r);const auto row=storage.row(slot,role);
                if(!row||!row.row||row.row->identity!=id)continue;
                e.kind=role==Storage::RosterRole::pod694?SelectedKind::pod694:SelectedKind::otherRoster;
                e.index=slot*Storage::rosterRoles+r;e.slot=slot;found=true;
            }
        if(!found)return false;
        if(e.kind==SelectedKind::pod694)++out.pods;else ++out.otherRows;
    }
    out.count=count;return true;
}

// 294D04..294D2C: OTHER body+288 scene name, three bytes before the first '.'
// (or end); "fr<d>" gives d-'0' (signed byte), anything else leaves 0. A name
// shorter than three reaches an unsafe original read: refused, as on RA.
inline bool podMaterial294D04(const OriginalRacerMainBodyOwner50LA* body,std::uint32_t& material) noexcept {
    material=0;if(!body)return false;
    const auto* scene=body->scene288();
    if(!scene||!scene->constructed||scene->released)return false;
    const auto& name=scene->name38;std::size_t end=0;
    while(end<name.size()&&name[end]!='\0'&&name[end]!='.')++end;
    if(end<3u)return false;
    if(name[end-3u]=='f'&&name[end-2u]=='r'){
        const auto byte=static_cast<unsigned char>(name[end-1u]);
        const std::int32_t signedByte=byte<128u?byte:static_cast<std::int32_t>(byte)-256;
        material=static_cast<std::uint32_t>(signedByte-48);
    }
    return true;
}

// 294C2C..294D2C body phase of 294BB0 for a dynamic (pod) target, AFTER the
// caller's actual geometry hit, 294BD8 skip mask and 294C18 current130 store.
// 294C44 own+A4 contains -> suppress records; else 294C68 other+A0 (1654C0),
// 294C7C own+98, 294C90 other+98 (165430). Material is parsed either way.
struct DynamicPodPhase {bool suppressPairRecords=false;std::uint32_t material=0;std::uint32_t stoppedPC=0x294c44;};
inline bool dynamicPodBodyPhase294C34(SlotAccess& a,std::size_t otherSlot,BodyContactIdentity48R other,
    const OriginalRacerMainBodyOwner50LA* otherBody,OwnBody& own,DynamicPodPhase& out) noexcept {
    out={};bool found=false;
    if(!a.ready()||!other.value||!own.identity.value||!ownProcessedContains165540(own,other,found))return false;
    out.suppressPairRecords=found;
    if(!found){
        BodyContactListStatus48R status{};
        out.stoppedPC=0x294c68;
        if(!appendOtherList(a,otherSlot,OtherField::count238,OtherField::partners23C,own.identity,status))return false;
        out.stoppedPC=0x294c7c;if(!appendOwnContact165430(own,other))return false;
        out.stoppedPC=0x294c90;
        if(!appendOtherList(a,otherSlot,OtherField::count20C,OtherField::partners210,own.identity,status))return false;
    }else ++totals().suppressedReverse;
    out.stoppedPC=0x294d04;
    if(!podMaterial294D04(otherBody,out.material))return false;
    out.stoppedPC=0x294dd4;++totals().dynamicHits;return true;
}
// 294E0C..294E74 (owner+88 mode 2): 2963E0 for ALL cumulative pairs, query
// scene, other body, parsed material. Modes 3 and 5 are not represented.
inline bool appendDynamicPodRecords294E0C(const SpringCumulativePair48N* pairs,std::size_t pairCount,
    SpringSceneId48N scene,BodyContactIdentity48R other,std::uint32_t material,
    SpringRecord48N* records,std::size_t capacity,SpringRecordState48N& state,std::size_t& appended) noexcept {
    appended=0;if(pairCount&&!pairs)return false;
    for(std::size_t i=0;i<pairCount;++i){
        SpringRecordAppend48N r{};
        std::memcpy(&r.field00,&pairs[i].first,sizeof r.field00);
        std::memcpy(&r.localPoint10,&pairs[i].second,sizeof r.localPoint10);
        std::memcpy(&r.normal20,&pairs[i].normal,sizeof r.normal20);
        std::memcpy(&r.surfacePoint30,&pairs[i].surfacePoint,sizeof r.surfacePoint30);
        r.scene40=scene;r.other44={other.value};r.material4C=material;
        const auto status=appendSpringRecord48N(r,records,capacity,state);
        if(status!=SpringRecordStatus48N::Appended&&status!=SpringRecordStatus48N::Saturated)return false;
        if(status==SpringRecordStatus48N::Appended){++appended;++totals().recordsOther44;}
    }
    return true;
}

// 219440..21947C prologue: one process-wide pair of lazily initialized words
// (gp-7894/434718 = 0.5, gp-7890/434720 = 1.0). Not per racer, never banked.
inline VehicleContactStatics48R& vehicleContactStatics219420() noexcept {static VehicleContactStatics48R s{};return s;}
inline void enterVehicleContact219440(VehicleContactStatics48R& s) noexcept {
    if(!s.flags[0]){s.words[0]=0x3f000000u;s.flags[0]=1;}
    if(!s.flags[1]){s.words[1]=0x3f800000u;s.flags[1]=1;}
}
// Visited racer's 219420 inputs: model1B4 position/up (28B1A0/28A700 XYZ),
// 364+80 radius, and main190 world for the two 28BD50 local points.
struct VehicleContactSelf219420 {
    SpringVector4_48N position{},up{};
    float vehicleRadius364=0;
    BodyMatrix48G localScene190{};
    bool localSceneClean=false;
};
// 219500..2197C8 for ONE first-list entry that is another pod's 694 row (pod
// mask 3B14B0 already matched by the caller). 1202E0 type0 bounds, contact
// geometry, local points, candidate surface, own+98 then other+98, 2963E0
// append (other44, scene 0, material 0). Miss returns true with hit=false.
inline bool vehicleContactPod219500(SlotAccess& a,std::size_t otherSlot,const RosterObject& row,
    BodyContactIdentity48R other,OwnBody& own,const VehicleContactSelf219420& self,
    const VehicleContactStatics48R& statics,SpringRecord48N* records,std::size_t capacity,
    SpringRecordState48N& state,bool& hit) noexcept {
    hit=false;
    if(!a.ready()||!other.value||!row.type0Resource||!row.membership.state.attached||!statics.flags[0])return false;
    Type0BoundsInput50EW boundsInput{};boundsInput.resource=row.type0Resource;
    boundsInput.composed00=row.membership.state.lastShapeWorld;
    const auto& cached=row.membership.state.bounds;
    std::memcpy(boundsInput.cachedCenterWords,cached.cachedCenter.v,sizeof(boundsInput.cachedCenterWords));
    std::memcpy(&boundsInput.cachedRadiusWord,&cached.cachedRadius,sizeof(boundsInput.cachedRadiusWord));
    CollectionBoundsResult48S bounds{};
    if(prepareType0Bounds50EW(boundsInput,bounds)!=CollectionBoundsStatus48S::Complete)return false;
    VehicleContactGeometryInput48R input{};input.position=self.position;input.up=self.up;
    input.boundsHalf=bounds.half;input.cachedCenter=cached.cachedCenter;
    input.vehicleRadius=self.vehicleRadius364;input.radiusScaleWord=statics.words[0];
    VehicleContactGeometryPrefix48R geometry{};
    if(!prepareVehicleContactGeometry48R(input,geometry))return false;
    if(!geometry.hit)return true;
    if(!self.localSceneClean)return false; //Dirty190 getter not represented here.
    SpringRecordAppend48N record{};record.normal20=geometry.normal;
    if(!prepareVehicleContactLocalPoint48R(self.localScene190,input.position,record.field00)||
       !prepareVehicleContactLocalPoint48R(self.localScene190,geometry.vehicleSurface,record.localPoint10)||
       !prepareVehicleContactCandidateSurface48R(input.cachedCenter,geometry.normal,
           geometry.candidateRadius,record.surfacePoint30))return false;
    BodyContactListStatus48R status{};
    if(!a.fits(otherSlot,OtherField::count20C,0,sizeof(std::int32_t)))return false;
    if(!appendOwnContact165430(own,other))return false;                                         //219788
    if(!appendOtherList(a,otherSlot,OtherField::count20C,OtherField::partners210,own.identity,status))return false; //21979C
    record.other44={other.value};record.scene40={0};record.material4C=0;                         //2197C0
    const auto appended=appendSpringRecord48N(record,records,capacity,state);
    if(appended!=SpringRecordStatus48N::Appended&&appended!=SpringRecordStatus48N::Saturated)return false;
    if(appended==SpringRecordStatus48N::Appended)++totals().recordsOther44;
    hit=true;++totals().contact219420;return true;
}
}
