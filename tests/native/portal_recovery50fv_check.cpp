// ONE controlled component check, not an end-to-end TA gameplay failure replay.
// A finite synthetic world produces a real native50EN failure receipt. That
// receipt then exercises the HOST policy with native50FL state and actual TA
// checkpoint/ground assets. No guest Machine, PC client, or ee.bin is used.
#include "checkpoint_recovery50fi_fixture.h"
#include "live_portals50ep_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include <iostream>
#include <memory>
#include <type_traits>

namespace recovery=checkpoint_recovery50fi_fixture;
namespace startup=runtime_startup50fl;
using namespace rr::game;
static unsigned checks=0;
static void check(bool ok,const char* message) {
    ++checks;if(!ok)throw std::runtime_error(message);
}
template<class T>static void same(const T& a,const T& b,const char* message) {
    static_assert(std::is_trivially_copyable_v<T>);
    check(!std::memcmp(&a,&b,sizeof(T)),message);
}
static LiveZoneBounds50EN boundsAt(float x) {
    LiveZoneBounds50EN b{};b.bounds.shapeTag=1;b.bounds.shapeWorld=startup::identity();
    b.bounds.shapeWorld.v[12]=x;
    b.bounds.localCenter={{0,0,0,1}};b.bounds.localHalf={{.6f,.6f,.6f,1}};
    b.bounds.cachedCenter={{x,0,0,1}};b.bounds.cachedRadius=.6f;return b;
}
static LiveZoneResult50EN realCounterexample() {
    // 50FT's analytical three-zone/two-plane construction. Requested radius
    // 0.6f has the same strict 0.5 < radius < 0.75 relation as its 0.625f.
    LiveZonePortalInput50EN authored[2]{};
    for(unsigned i=0;i<2;++i) {
        const float x=float(i);authored[i].zoneA=1;authored[i].zoneB=i+2;
        authored[i].vertices[0]={{x,-10,-10}};authored[i].vertices[1]={{x,10,-10}};
        authored[i].vertices[2]={{x,10,10}};authored[i].vertices[3]={{x,-10,10}};
    }
    CollectionRef50BX heads[3]{};LiveZonePortalLink50EN portals[4]{};
    PortalGeometry50EO geometry[2]{};LiveZoneWorld50EN world{};
    check(prepareLiveZoneWorld50EN(authored,2,3,{heads,3,portals,4,geometry,2},world)==
          LiveZoneStatus50EN::Complete,"counterexample world preparation");
    check(portals[heads[0]-1].portal==2,"authored B-then-A incident order");
    SearchStorage50BS registries[6]{};ZoneRegistration50CJ registrations[1]{};
    CollectionTree50BX methods[6]{};CollectionZone50BX zones[3]{};
    SearchListBlock50BR blocks[12]{};ZoneRegistrationScratch50CJ scratch{};
    ZoneLifecycle50CL life{};
    check(initializeZoneLifecycle50CL(life,{registries,6,registrations,1,
          {methods,6,zones,3,blocks,12}},3,1,16,scratch)==SearchStorageStatus50BS::Complete,
          "counterexample lifecycle preparation");
    CollectionZoneLink50BX links[3]{},scratchLinks[3]{};
    LiveZoneEntry50EN entries[3]{},scratchEntries[3]{};
    SearchStorage50BS scratchRegistries[6]{};LiveZonePortalLink50EN scratchPortals[4]{};
    LiveZoneWorkspace50EN work{scratchRegistries,6,scratchLinks,scratchEntries,3,scratchPortals,4,{}};
    const LiveZoneStorage50EN storage{links,entries,3};LiveZoneMembership50EN membership{};
    const auto start=boundsAt(-.5f);
    const auto attached=attachLiveZoneMembership50EN(life,world,membership,storage,
          1,2,ZoneMethod50CL::Second,0.f,start,work);
    check(attached.status==LiveZoneStatus50EN::Complete,"zero-radius attachment");
    check(links[0].zone==2&&!links[0].next,"attach has only zone2 primary");
    const auto middle=boundsAt(.5f);
    const auto crossed=updateLiveZoneMembership50EN(life,world,membership,middle,work);
    check(crossed.status==LiveZoneStatus50EN::Complete&&crossed.crossedPortals==1,
          "cross portal A once into zone1");
    check(links[0].zone==1&&links[0].next,"new zone1 primary and secondaries");
    const auto a=links[0].next;const auto b=links[a-1].next;
    check(links[a-1].zone==2&&b&&links[b-1].zone==3&&!links[b-1].next,
          "genuine A(zone2) -> B(zone3) secondary chain");
    // Every published owner/view/cache is preserved by the actual failure.
    const auto savedMembership=membership;const auto savedLife=life;const auto savedWorld=world;
    decltype(registries) savedRegistries;std::memcpy(savedRegistries,registries,sizeof registries);
    decltype(registrations) savedRegistrations;std::memcpy(savedRegistrations,registrations,sizeof registrations);
    decltype(methods) savedMethods;std::memcpy(savedMethods,methods,sizeof methods);
    decltype(zones) savedZones;std::memcpy(savedZones,zones,sizeof zones);
    decltype(blocks) savedBlocks;std::memcpy(savedBlocks,blocks,sizeof blocks);
    decltype(links) savedLinks;std::memcpy(savedLinks,links,sizeof links);
    decltype(entries) savedEntries;std::memcpy(savedEntries,entries,sizeof entries);
    decltype(portals) savedPortals;std::memcpy(savedPortals,portals,sizeof portals);
    decltype(heads) savedHeads;std::memcpy(savedHeads,heads,sizeof heads);
    decltype(geometry) savedGeometry;std::memcpy(savedGeometry,geometry,sizeof geometry);
    const auto end=boundsAt(.25f);
    const auto receipt=updateLiveZoneMembership50EN(life,world,membership,end,work);
    check(receipt.status==LiveZoneStatus50EN::UnsupportedLinkRemoval,
          "native50EN did not reproduce UnsupportedLinkRemoval; do not fabricate receipt");
    check(receipt.boundsStatus==CollectionBoundsStatus48S::Complete&&
          receipt.geometryStatus==PortalGeometryStatus50EO::Complete&&
          receipt.registryStatus==SearchStorageStatus50BS::Complete&&receipt.crossedPortals==0,
          "real native failure has completed suboperations and no crossing");
    same(membership,savedMembership,"failure preserves membership");
    same(life,savedLife,"failure preserves lifecycle");same(world,savedWorld,"failure preserves world");
    same(registries,savedRegistries,"failure preserves registry words");
    same(registrations,savedRegistrations,"failure preserves registration owner");
    same(methods,savedMethods,"failure preserves method views");same(zones,savedZones,"failure preserves zone views");
    same(blocks,savedBlocks,"failure preserves block views");same(links,savedLinks,"failure preserves link chain");
    same(entries,savedEntries,"failure preserves live entries");same(portals,savedPortals,"failure preserves portal caches");
    same(heads,savedHeads,"failure preserves incident heads");same(geometry,savedGeometry,"failure preserves geometry");
    std::cout<<"Controlled native50EN counterexample: attach x=-0.5 radius=0; update x=0.5 radius=0.6; "
             <<"update x=0.25 -> UnsupportedLinkRemoval, transactional state preserved.\n";
    return receipt;
}
int main() {try {
    check(std::fesetround(FE_TONEAREST)==0,"round-to-nearest");
    const auto receipt=realCounterexample();
    auto owner=std::make_unique<startup::Fixture>(0);
    auto scene=authored_runtime_ground50cp_fixture::load({4,16,32,64});
    const auto authored=loadLivePortalInputs50EP(scene.world);
    std::vector<CollectionRef50BX> heads(scene.world.zones.size());
    std::vector<LiveZonePortalLink50EN> links(authored.size()*2);
    std::vector<PortalGeometry50EO> portals(authored.size());LiveZoneWorld50EN world{};
    check(prepareLiveZoneWorld50EN(authored.data(),authored.size(),heads.size(),
          {heads.data(),heads.size(),links.data(),links.size(),portals.data(),portals.size()},world)==
          LiveZoneStatus50EN::Complete,"retained TA portal preparation");
    rr::assets::ResArchive48C archive;rr::assets::DecodeError48C error;
    check(rr::assets::decodeResource48C(startup::readBytes("work/native/project/disc/TRACKS/TA.RES"),
          archive,error),"retained TA archive");
    std::vector<std::uint8_t> spline;
    check(rr::assets::extractResMember48C(archive,"tracks\\mta.spl",spline,error),"retained MTA spline");
    auto map=std::make_unique<TrackNavigation50EU>();
    auto scratch=std::make_unique<TrackNavigationScratch50EU>();TrackNavigationDiagnostic50EU nd{};
    check(readTrackNavigation50EU(reinterpret_cast<const char*>(spline.data()),spline.size(),
          owner->state.animationShared.libm,*scratch,*map,nd),"retained TA source gate preparation");
    check(bindTrackNavigationZones50EU(world,*map,nd),"retained TA source zone index binding");
    TrackNavigationProgress50EU progress{};initializeTrackNavigationProgress50EU(progress);
    // Controlled canonical inputs, explicitly not captured gameplay damage or
    // a reproduced frame: nonzero damage detects an accidental fresh-vehicle policy.
    auto& state=owner->state;state.fields.damage7D0=.125f;state.fields.damage7D4=.25f;
    state.fields.damage7D8=.5f;state.fields.damage7DC=.75f;
    state.phase=VehicleFramePhase49W::PostWorld;
    const auto originalState=std::make_unique<VehicleFrameState49W>(state);
    const auto originalProgress=progress;recovery::Plan plan{};recovery::Diagnostic diagnostic{};
    check(recovery::preparePortalRecovery(*map,progress,state,owner->bindings,receipt,
          scene.lift,world,plan,diagnostic),"real receipt prepares HOST portal recovery at PostWorld");
    check(plan.policy==recovery::Policy::PortalLinkRemoval&&plan.prepared,"portal policy selected");
    same(plan.portalDecision,receipt,"actual receipt retained");
    same(plan.progress,progress,"navigation retained");
    check(plan.damage.damage7D0==state.fields.damage7D0&&plan.damage.damage7D4==state.fields.damage7D4&&
          plan.damage.damage7D8==state.fields.damage7D8&&plan.damage.damage7DC==state.fields.damage7DC,
          "four actual canonical damage lanes retained");
    check(plan.selection.anchorNode==progress.reset.vehicleC8C&&!plan.selection.writesC80&&
          plan.primaryZone==CollectionRef50BX(map->nodeIndex78[progress.reset.vehicleC8C])+1u,
          "genuine retained checkpoint and produced zone selected");
    for(unsigned i=0;i<3;++i)check(plan.selection.anchorPosition[i]==
          map->track.nodes[progress.reset.vehicleC8C].position[i],"genuine authored checkpoint coordinates");
    check(plan.sourcePose.v[12]!=state.nodes[static_cast<unsigned>(owner->bindings.lifecycle.main190)].localC0.v[12],
          "checkpoint selection is not current-position fallback");
    same(state,*originalState,"successful preparation preserves original native frame");
    same(progress,originalProgress,"successful preparation preserves original navigation");
    std::vector<std::uint32_t> stack(scene.stackCount);BodyMatrix48G grounded{};
    check(recovery::resolveGround(scene,plan,4,0,stack.data(),stack.size(),grounded,diagnostic),
          "actual retained TA ground hit");
    check(diagnostic.groundPhase==ResetProbePhase50AG::Hit&&diagnostic.attempts>0&&diagnostic.candidates>0,
          "ground receipt is genuine query hit");
    const auto groundAttempts=diagnostic.attempts;const auto groundCandidates=diagnostic.candidates;
    auto fresh=std::make_unique<startup::Fixture>(0);TrackNavigationProgress50EU restored{};
    initializeTrackNavigationProgress50EU(restored);float ca0[4]{};
    check(recovery::restoreContinuity(plan,fresh->state.fields,restored,ca0,grounded,diagnostic),
          "fresh native owner restores continuity after ground");
    same(restored,progress,"fresh owner preserves navigation");
    check(fresh->state.fields.damage7D0==state.fields.damage7D0&&fresh->state.fields.damage7D4==state.fields.damage7D4&&
          fresh->state.fields.damage7D8==state.fields.damage7D8&&fresh->state.fields.damage7DC==state.fields.damage7DC,
          "fresh owner preserves all four damage lanes");
    check(!std::memcmp(ca0,grounded.v+12,sizeof ca0),"fresh CA0 rebased to actual grounded pose");
    auto reject=[&](const VehicleFrameState49W& input,const LiveZoneResult50EN& decision,recovery::Error expected) {
        recovery::Plan untouched=plan;const auto before=untouched;
        check(!recovery::preparePortalRecovery(*map,progress,input,owner->bindings,decision,
              scene.lift,world,untouched,diagnostic),"negative portal admission rejects");
        check(diagnostic.error==expected,"negative portal diagnostic");same(untouched,before,"rejected preparation preserves output");
    };
    // Each enum phase other than PostWorld must reject, not just Ready.
    for(unsigned phase=0;phase<=static_cast<unsigned>(VehicleFramePhase49W::PostEngineBase50X);++phase) {
        if(phase==static_cast<unsigned>(VehicleFramePhase49W::PostWorld))continue;
        auto wrong=std::make_unique<VehicleFrameState49W>(state);wrong->phase=static_cast<VehicleFramePhase49W>(phase);
        reject(*wrong,receipt,recovery::Error::PortalCut);
    }
    for(unsigned status=0;status<=static_cast<unsigned>(LiveZoneStatus50EN::UnsupportedLinkRemoval);++status) {
        if(status==static_cast<unsigned>(LiveZoneStatus50EN::UnsupportedLinkRemoval))continue;
        auto wrong=receipt;wrong.status=static_cast<LiveZoneStatus50EN>(status);
        reject(state,wrong,recovery::Error::PortalDecision);
    }
    auto wrong=receipt;wrong.boundsStatus=CollectionBoundsStatus48S::InvalidInput;
    reject(state,wrong,recovery::Error::PortalDecision);
    wrong=receipt;wrong.geometryStatus=PortalGeometryStatus50EO::DegenerateGeometry;
    reject(state,wrong,recovery::Error::PortalDecision);
    wrong=receipt;wrong.registryStatus=SearchStorageStatus50BS::GrowthBoundary;
    reject(state,wrong,recovery::Error::PortalDecision);
    same(state,*originalState,"all policy checks preserve old frame");same(progress,originalProgress,"all policy checks preserve old navigation");
    std::cout<<"TA checkpoint anchor="<<plan.selection.anchorNode<<" next="<<plan.selection.selectedNextNode
             <<" zone="<<plan.primaryZone<<" grounded="<<grounded.v[12]<<','<<grounded.v[13]<<','<<grounded.v[14]
             <<" ground_attempts="<<groundAttempts<<" candidates="<<groundCandidates<<'\n';
    std::cout<<"portal_recovery50fv PASS "<<checks<<" controlled component assertions; "
             <<"NOT an end-to-end gameplay failure reproduction; no game/device/guest launch.\n";return 0;
} catch(const std::exception& error) {
    std::cerr<<"portal_recovery50fv FAIL after "<<checks<<" assertions: "<<error.what()<<'\n';return 1;
}}
