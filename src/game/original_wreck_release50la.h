#pragma once
#include "original_wreck_activation50la.h"
#include "original_wreck_launch50la.h"

namespace rr::game {
// Borrow ONE application-owned pair of original lazy fields. Guard bytes are
// gp-780C / gp-7808; values are434870 /434878. The caller retains these across
// collections/racers and supplies the existing shared308030 random state.
struct OriginalWreckReleaseGlobals50LA {
    std::uint8_t* angularGuard=nullptr;
    float* angularScale434870=nullptr;
    std::uint8_t* velocityGuard=nullptr;
    float* velocityScale434878=nullptr;
    std::uint32_t* sharedRawRandom=nullptr;
};
enum class OriginalWreckReleaseStatus50LA : std::uint8_t {
    complete,binding,activation,groupEffects,component,body,launch
};
struct OriginalWreckReleaseResult50LA {
    OriginalWreckReleaseStatus50LA status=OriginalWreckReleaseStatus50LA::binding;
    std::uint32_t sourcePC=0x22ca60u;
    bool lazyFieldsVisited=false,groupDeactivated=false,primaryFlightSet=false;
    bool pieceReactivated=false,independentMaskSet=false,bodyEnabled=false,bodyReset=false;
    OriginalWreckActivationResult50LA activation{};
    OriginalWreckEffectsReceipt50LA effects{};
    OriginalWreckComponentResult50LA component{};
    OriginalWreckLaunchResult50LA launch{};
};
// Full22CA60 for the actual authored ONE-piece collection only. This is a
// concrete caller of the existing activation and launch leaves, not a death
// handler receipt. It neither publishes racer state nor awards credit.
//
// Fresh/detached memberships and a complete cold owner are required by22C6A0.
// All earlier source writes survive any reached boundary. Never replay entry
// after failure. No generic callbacks, private RNG, borrowed-body replacement,
// or unowned collection entries are accepted. Source/global storage must be
// live, mutually disjoint and exclusively borrowed for this serial call.
inline OriginalWreckReleaseResult50LA releaseOriginalOnePieceWreck22CA60(
    OriginalOnePieceWreckOwner50LA& owner,const OriginalWreckActivationSource50LA& source,
    const OriginalWreckReleaseGlobals50LA& globals)noexcept {
    using S=OriginalWreckReleaseStatus50LA;
    OriginalWreckReleaseResult50LA r;
    auto fail=[&](S status,std::uint32_t pc){r.status=status;r.sourcePC=pc;return r;};
    const auto* reset=owner.bodyResetGlobals();
    if(!owner.prepared()||!reset||owner.assets.selected().pieces.size()!=1u||
       owner.group.capacity34||owner.group.attachedCount30||!source.body||
       source.body==&owner.physical.body.root||!globals.angularGuard||
       !globals.angularScale434870||!globals.velocityGuard||!globals.velocityScale434878||
       !globals.sharedRawRandom)return fail(S::binding,0x22ca60u);
    //22CA74..22CABC occurs before22C6A0. Preserve already-initialized values.
    if(!*globals.angularGuard){
        *globals.angularScale434870=original_wreck_launch_detail50LA::word(0x41a00000u);
        *globals.angularGuard=1;
    }
    if(!*globals.velocityGuard){
        *globals.velocityScale434878=original_wreck_launch_detail50LA::word(0x4392aa7fu);
        *globals.velocityGuard=1;
    }
    r.lazyFieldsVisited=true;
    r.activation=activateOriginalOnePieceWreck50LA(owner,source); //22CAD0
    if(r.activation.status!=OriginalWreckActivationStatus50LA::complete)
        return fail(S::activation,r.activation.sourcePC);
    //22CAE4 invokes the genuine group override2CC9C0, including external
    //effect controls BEFORE base141520. Source-null effects skip that prefix.
    if(owner.hasGroupEffects()){
        r.effects=owner.effectOwners.deactivateGroupEffects();
        if(!r.effects.okay())return fail(S::groupEffects,r.effects.sourcePC);
    }
    r.component=setOriginalWreckBaseActivity50LA(owner.domain,owner.groupComponent(),false);
    if(r.component.status!=OriginalWreckComponentStatus50LA::complete)
        return fail(S::component,0x2cca54u);
    r.groupDeactivated=true;
    owner.piece.flight34=1; //22CB00, actual collection38[0]
    r.primaryFlightSet=true;
    //22CB14 ->2CA2F0 scans ONLY group34 nonprimary slots. Genuine capacity0
    //branches2CA320->2CA440->2CA458, with no detach/recompute/query writes.
    r.component=activateOriginalWreckPiece50LA(owner.domain,owner.pieceComponent(),owner.piece.contactTime12C);
    if(r.component.status!=OriginalWreckComponentStatus50LA::complete)
        return fail(S::component,0x22cb34u);
    r.pieceReactivated=true;
    owner.pieceComponent().mask20|=2u; //22CB54, no ancestor OR propagation
    r.independentMaskSet=true;
    if(setOriginalWreckBodyEnabled50LA(owner.physical,true)!=OriginalWreckBodyControlStatus50LA::complete)
        return fail(S::body,0x22cb70u);
    r.bodyEnabled=true;
    resetSecondaryActivation49V(owner.physical.body,owner.physical.supplement,*reset); //22CB94
    r.bodyReset=true;
    r.launch=applyOriginalWreckLaunchTail50LA(source.body->body,owner.physical.body.root.body,
        owner.physical.body.velocity70W,owner.physical.body.angularVelocityB0W,
        *globals.angularScale434870,*globals.velocityScale434878,*globals.sharedRawRandom);
    if(r.launch.status!=OriginalWreckLaunchStatus50LA::complete)
        return fail(S::launch,r.launch.sourcePC);
    r.status=S::complete;r.sourcePC=0x22ce08u;return r;
}
} // namespace rr::game
