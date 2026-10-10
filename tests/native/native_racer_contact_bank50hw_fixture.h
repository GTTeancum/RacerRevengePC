#pragma once
// Heterogeneous bank adapter. Per-racer parameters/bindings are restored by
// Bank::visit in the shared arena; contact settings/resources are cold-bound
// independently by native_racer_contact50HW::Owner.
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_contact50hw_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "../../src/game/vehicle_frame49w.h"

namespace native_racer_contact50HW {
// Native admission diagnostic, not a fabricated original return/status.
// Source223100..223110 requires the real body20C/210 partner list and222F00
// dynamic-damage owners. Legacy HOST mode cannot publish those owners; the
// optional50IW join requires the retained registry and every participant bound.
struct AiContactBoundary50IV {
    bool unsupported=false;
    std::size_t racerSlot=0;
    std::uint32_t sourcePC=0;
    std::int32_t contactCount20C=0;
};
// One HOST world-frame boundary only: call after ALL participating independent
// native frames finished at Ready, outside Bank::visit(), before another racer
// begins its next frame. No renderer, source query, or event consumer can run
// concurrently. The bank must contain the full frame state, as50HN already does.
//
// The source game's all-body scheduling is not reconstructed here. Impulses
// enter each native body now; accumulated source position/rotation corrections
// are consumed by its NEXT ordinary integration. Never move the scenes here.
// The secondary/chariot body and its graph/joint caches remain owned by that
// racer's subsequent normal native coupled-body step; no artificial translation
// or matching-velocity overwrite of the secondary body is performed.
//
// Each racer has its own original geometry and authored body/spring parameters.
// The bank MUST snapshot live bindings and parameters as well as frame state.
// identity[i] is a unique opaque main-body id. The optional50IW join requires
// these same tokens in its retained source-manager registry for the session.
// material[i] is explicit; emptyPhysicalRootMaterial is the documented HOST
// policy for the authenticated fresh empty root, not unknown-prefix recovery. Pair order indexes bank slots directly, with no
// sorting/deduplication. A stalled/finished racer should appear only if the host
// deliberately wants it to participate; pair order is the eligibility boundary.
// All requested pairs still run into local scratch. In legacy mode an actual
// accepted AI contact returns ResponseRejected before ANY racer commit; mere
// eligibility/broadphase miss does not reject. Joined50IW mode instead requires
// the same retained registry as self338/globalB0 and canonical dynamic owners.
// It appends both sides' actual ordered receipts to their persistent20C/210
// through165430 semantics and commits those lists with the pair response.
// Optional aiBoundary identifies missing ownership/count boundaries. On failure
// all bank states and receipt remain unchanged; scratch/diagnostic may change.
inline Status applyAtWorldBoundary(
    native_racer50HN::Bank& bank,Owner& contacts,
    VehicleFrameState49W& live,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,
    const std::array<std::uint32_t,maxRacers>& identity,
    const std::array<std::uint32_t,maxRacers>& material,
    const Pair* order,std::size_t pairCount,
    Receipt& receipt,AiContactBoundary50IV* aiBoundary=nullptr,
    bool dynamicJoined=false,
    const native_dynamic_contact50IW::Registry* dynamicRegistry=nullptr) {
    if(aiBoundary)*aiBoundary={};
    const auto count=bank.count();
    if(count<2||count>maxRacers||count!=contacts.count())return Status::InvalidInput;
    if(dynamicJoined&&(!dynamicRegistry||
       !dynamicRegistry->matchesContactIdentities(identity.data(),count))){
        if(aiBoundary)*aiBoundary={true,0,0x22a280u,0};
        return Status::ResponseRejected;
    }
    if(pairCount>maxRacers*(maxRacers-1)||(pairCount&&!order))return Status::InvalidInput;
    std::array<bool,maxRacers> participates{};
    for(std::size_t p=0;p<pairCount;++p){
        if(order[p].primary>=count||order[p].other>=count||order[p].primary==order[p].other)return Status::InvalidInput;
        participates[order[p].primary]=participates[order[p].other]=true;
    }
    std::array<Body,maxRacers> before{},after{};
    std::array<bool,maxRacers> actualAi{},dynamicBound{};
    std::array<BodyContactListState48R,maxRacers> contactBefore{},contactAfter{};
    for(std::size_t i=0;i<count;++i){
        const bool valid=bank.visit(i,[&](){
            const auto node=static_cast<int>(bindings.lifecycle.main190);
            if(!validVehicleSceneCount49W(live,bindings)||node<0||
               node>=static_cast<int>(live.sceneNodeCount))return false;
            const auto& root=live.bodies.main.components.root;
            actualAi[i]=live.fields.vehicle58==1u;
            if(dynamicJoined){
                dynamicBound[i]=live.fields.dynamicOwnersBound50IW;
                // Preserve the signed source count bits, including unsupported
                // negative states, without an unsigned-to-signed conversion.
                std::memcpy(&contactBefore[i].count20C,&live.bodies.main.components.body20C,
                    sizeof(contactBefore[i].count20C));
                std::memcpy(contactBefore[i].slots210.data(),live.mainContactPartners210,
                    sizeof(live.mainContactPartners210));
            }
            if(participates[i]&&(live.phase!=VehicleFramePhase49W::Ready||
               std::memcmp(&root.scene,&live.nodes[node].world100,sizeof(BodyMatrix48G))))return false;
            before[i].physical.identity=identity[i];before[i].physical.body.state=root;
            before[i].physical.body.parameters=parameters.bodies.main.body;
            before[i].physical.body.localCenter=parameters.bodies.main.centerOffset;
            before[i].material=material[i];return true;
        });
        if(!valid)return Status::InvalidInput;
    }
    // Joined ownership admission precedes all response/list mutations. Every
    // participant remains a real active Ready racer, even when no pair hits.
    if(dynamicJoined)for(std::size_t i=0;i<count;++i)if(participates[i]){
        if(!bank.active(i)||!dynamicBound[i]){
            if(aiBoundary)*aiBoundary={true,i,0x223110u,contactBefore[i].count20C};
            return Status::ResponseRejected;
        }
        if(contactBefore[i].count20C<0||
           contactBefore[i].count20C>static_cast<std::int32_t>(BodyContactCapacity48R)){
            if(aiBoundary)*aiBoundary={true,i,0x22a2a8u,contactBefore[i].count20C};
            return Status::ResponseRejected;
        }
    }
    Receipt staged{};
    const auto status=contacts.run(before.data(),count,order,pairCount,after.data(),staged);
    if(status!=Status::Complete)return status;
    // Owner::run appends both sides' real contact identities only after an
    // accepted geometry hit and the original processed-body suppression gate.
    // Detect the source-required partner branch from that receipt, not from a
    // guessed distance or a changed impulse (valid contacts may have zero force).
    for(std::size_t i=0;i<count;++i)
        if(!dynamicJoined&&actualAi[i]&&staged.contacts[i].count20C>0){
            if(aiBoundary)*aiBoundary={true,i,0x223110u,staged.contacts[i].count20C};
            return Status::ResponseRejected;
        }
    if(dynamicJoined){
        contactAfter=contactBefore;
        for(std::size_t i=0;i<count;++i){
            const auto& appended=staged.contacts[i];
            // Owner::run starts its receipt empty and records only successful
            // geometry/processed-gate contacts, in actual per-body append order.
            // Merge into the still-pending canonical list; never clear20C here.
            if(appended.count20C<0||
               appended.count20C>static_cast<std::int32_t>(BodyContactCapacity48R))
                return Status::InvalidInput;
            for(std::int32_t j=0;j<appended.count20C;++j)
                if(applyBodyContactList48R(appended.slots210[static_cast<std::size_t>(j)],
                    contactAfter[i])==BodyContactListStatus48R::InvalidInput)
                    return Status::InvalidInput;
        }
    }
    // Check every participant BEFORE writing any participant. This catches a
    // caller that changed a racer while collecting snapshots. No retained body
    // or scene pointer crosses a visit: only copied POD snapshots do.
    for(std::size_t i=0;i<count;++i)
        if(participates[i]&&!bank.visit(i,[&](){
            if(live.phase!=VehicleFramePhase49W::Ready||
               std::memcmp(&live.bodies.main.components.root,&before[i].physical.body.state,sizeof(RootBodyState48H)))
                return false;
            if(dynamicJoined&&(!bank.active(i)||!live.fields.dynamicOwnersBound50IW||
               std::memcmp(&live.bodies.main.components.body20C,&contactBefore[i].count20C,sizeof(contactBefore[i].count20C))||
               std::memcmp(live.mainContactPartners210,contactBefore[i].slots210.data(),sizeof(live.mainContactPartners210))))
                return false;
            return true;
        }))
            return Status::InvalidInput;
    // All fallible work is over. Commit the exact two-body numerical response
    // fields and, only when joined, canonical20C/210; all ten210 slots preserve
    // their old bits unless a source append overwrites that slot. Transform,
    // position, quaternion, history and terrain stay intact.
    const auto commit=[&](std::size_t i){bank.visit(i,[&](){
        auto& dst=live.bodies.main.components.root;const auto& src=after[i].physical.body.state;
        dst.body.velocity=src.body.velocity;dst.body.angularVelocity=src.body.angularVelocity;
        dst.loads=src.loads;dst.positionCorrection1a0=src.positionCorrection1a0;
        dst.rotationCorrection1b0=src.rotationCorrection1b0;
        dst.accumulator1c0=src.accumulator1c0;dst.accumulator1d0=src.accumulator1d0;
        if(dynamicJoined){
            std::memcpy(&live.bodies.main.components.body20C,&contactAfter[i].count20C,
                sizeof(contactAfter[i].count20C));
            std::memcpy(live.mainContactPartners210,contactAfter[i].slots210.data(),
                sizeof(live.mainContactPartners210));
        }
    });};
    for(std::size_t i=1;i<count;++i)if(participates[i])commit(i);if(participates[0])commit(0);
    receipt=staged;return Status::Complete;
}
}
