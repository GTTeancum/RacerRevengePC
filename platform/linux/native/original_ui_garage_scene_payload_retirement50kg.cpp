#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_static_models50kg.h"
#include "original_ui_garage_scene_collision50kg.h"
#include "original_ui_main_session50kg.h"
#include "original_ui_light_manager_owner50kg.h"
#include "original_application_aural50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_garage_scene_emitters50kg.h"
#include "original_ui_garage_scene_lighting50kg.h"
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool payloadStop(D& d,std::uint32_t pc)noexcept {
    d={};d.status=S::phase;d.sourcePC=pc;return false;
}
}
bool OriginalUiGaragePodScene50KG::sourceCollisionListRetiredForPayload()const noexcept {
    return sceneCollision_&&(failedPageRetirementOwner_?
        sceneCollision_->failedRawListRetired27BB80(*failedPageRetirementOwner_):
        sceneCollision_->rawListRetired27BB80());
}
bool OriginalUiGaragePodScene50KG::scenePayloadRetirementCurrent()const noexcept {
    //The real Page1FA1B4 path already proved Page ref0, no children and no
    //callback/Actor borrow. This is the actual still-counted Scene ref0
    //interval, not a detached-construction cleanup or public completion flag.
    return constructed_&&sceneLive_&&grant_&&grant_->current()&&
        grant_->scenePodRetirementCurrent(podScene_)&&
        retirementStage_>=RetirementStage::groupReleased&&
        finalLoadStage_==FinalLoadStage::complete&&loadStage_==LoadStage::audioClosed&&
        !sourceConstructionFailed_&&!configFailure_&&!actorOwner3C8_&&!worldList500_&&
        !count50C_&&!capacity504_&&!config34_&&!config38_&&retiredAudioConfig38_&&
        nativeApplication_&&nativeApplication_->known&&
        soundGroupSourceRetired235150();
}
bool OriginalUiGaragePodScene50KG::retireStartData26FDEC(D& d)noexcept {
    d={};
    if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::groupReleased||
       !sceneSetup_||sceneSetup_->closed_||!sceneSetup_->complete_||!sceneSetup_->startData348_)
        return payloadStop(d,0x26FDEC);
    sceneSetup_->startData348_.reset(); //2E8B00 then actual null348 at26FDF4.
    retirementStage_=RetirementStage::startDataReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireZones26FE08(D& d)noexcept {
    d={};
    if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::startDataReleased||
       !sceneSetup_||sceneSetup_->closed_||!sceneSetup_->zones350_||
       numZones34C_!=1||sceneSetup_->zoneCount_!=1)
        return payloadStop(d,0x26FE08);
    //2E8750 calls275DC0 in reverse array order. Release each actual nested
    //exclusions120 allocation before the outer array, never a copied Zone.
    for(std::size_t end=sceneSetup_->zoneCount_;end>0;--end)
        sceneSetup_->zones350_[end-1].exclusions120.reset();
    sceneSetup_->zones350_.reset(); //26FE10 null350; source34C count survives.
    sceneSetup_->zoneCount_=0; //Native allocation extent, NOT source numZones34C.
    retirementStage_=RetirementStage::zonesReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retirePortalArray26FE14(D& d)noexcept {
    d={};
    if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::zonesReleased)
        return payloadStop(d,0x26FE14);
    //270594 supplied the actual null. The admitted live reader has zero
    //portals and never reaches270C78/272C8C. Positive owner remains unavailable.
    if(numPortals358_||portals3A0_)return payloadStop(d,0x26FE28);
    retirementStage_=RetirementStage::portalArrayReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireEnvironment276B30(D& d)noexcept {
    d={};
    if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::portalArrayReleased||
       !sceneSetup_||sceneSetup_->closed_||!sceneSetup_->environment3A4_)
        return payloadStop(d,0x26FE40);
    auto& environment=*sceneSetup_->environment3A4_;
    if(environment.cubeCount00<0||
       static_cast<std::size_t>(environment.cubeCount00)>environment.allocatedCubeRows||
       (environment.allocatedCubeRows&&!environment.cubeFiles04))return payloadStop(d,0x276B90);
    //Read276C00 may have converted a sole 'none' cube into count0 after
    //freeing its row, retaining the outer/zone arrays. Do not leak or invent
    //a second row free by using allocation extent instead of source count.
    if(environment.cubeFiles04)
        for(std::size_t i=static_cast<std::size_t>(environment.cubeCount00);i<environment.allocatedCubeRows;++i)
            if(environment.cubeFiles04[i])return payloadStop(d,0x276BAC);
    for(std::int32_t i=0;i<environment.cubeCount00;++i)
        environment.cubeFiles04[static_cast<std::size_t>(i)].reset(); //276B80 ascending.
    environment.cubeFiles04.reset(); //276BAC.
    environment.zoneCube08.reset(); //276BC0, after cube pointer array.
    sceneSetup_->environment3A4_.reset(); //276BD8 then26FE48 null3A4.
    //Do NOT clear Application443108 here: its distinct actual Visibility
    //environment node remains until the later reached26FF54 source store.
    retirementStage_=RetirementStage::environmentReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireFogDescriptor26FE58(D& d)noexcept {
    d={};
    if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::environmentReleased||
       !sceneSetup_||sceneSetup_->closed_)
        return payloadStop(d,0x26FE58);
    if(!sceneSetup_->fog3AC_){
        //273F70/273F7C already consumed this descriptor after all three
        //initializers.26FE50 takes its real null branch, not a second free.
        if(!sceneLighting_||!sceneLighting_->identityCurrent()||
           sceneLighting_->fogStage_!=OriginalUiGarageSceneLighting50KG::FogStage::complete||
           !sceneLighting_->rendererWritePerformed_||sceneLighting_->renderProxyActive_)
            return payloadStop(d,0x26FE58);
        for(std::size_t i=0;i<3;++i){const auto* entity=sceneLighting_->fogEntity(i);
            if(!sceneLighting_->fogConstructed_[i]||!sceneLighting_->fogRetired_[i]||
               sceneLighting_->fogRetiring_[i]||!entity||entity->reference00)
                return payloadStop(d,0x26FE58);
        }
    }else sceneSetup_->fog3AC_.reset(); //26FE58/26FE60, plain descriptor only.
    retirementStage_=RetirementStage::fogDescriptorReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireTerrainInfo26FE70(D& d)noexcept {
    d={};
    if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::fogDescriptorReleased||
       !sceneDispatch_||sceneDispatch_->closed||!sceneDispatch_->terrainComplete||
       !sceneDispatch_->terrain3B0)return payloadStop(d,0x26FE70);
    sceneDispatch_->terrain3B0.reset(); //Plain native TerrainInfo payload.
    retirementStage_=RetirementStage::terrainInfoReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireStaticPointerArray26FE88(D& d)noexcept {
    d={};
    if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::terrainInfoReleased||
       numZones34C_!=1||!staticZoneLists3C4_||!staticModels_||
       staticZoneLists3C4_[0]!=&staticModels_->list3C4_)
        return payloadStop(d,0x26FE88);
    const auto& list=*staticModels_;const auto& entity=list.list3C4_;
    //The source frees ONLY the pointer array. Page142590 must already have
    //performed real children and27B650 destruction; never invoke closeCold
    //here against an attached Page list or infer that count30 means retired.
    if(!list.closed_||list.listLive_||list.retiring_||list.entries34_||
       entity.reference00||entity.firstChild18)
        return payloadStop(d,0x27B650);
    for(std::size_t i=0;i<list.count;++i)
        if(list.wrappers_[i]||(list.models_[i]&&!list.models_[i]->closed_)||
           list.enrolled_[i]||list.attached_[i])return payloadStop(d,0x240450);
    //142590 intentionally retains the dying node's parent/sibling pointers.
    //Do not demand invented detach/null stores after its genuine destructor.
    staticZoneLists3C4_.reset(); //26FE90 null3C4; retained backing is a tombstone.
    retirementStage_=RetirementStage::staticPointerArrayReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retirePhysicsPointerArray26FEA0(D& d)noexcept {
    d={};
    if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::staticPointerArrayReleased||
       !toyFields_||!toyFields_->complete_||toyFields_->closed_)
        return payloadStop(d,0x26FE94);
    //Actual live2718F8 reader produced zero PhysicsObjects. Positive raw
    //wrapper/descriptors are unavailable; do not silently free imagined rows.
    if(toyFields_->physicsCount4B0_||toyFields_->physics4B4_)return payloadStop(d,0x26FEA0);
    retirementStage_=RetirementStage::physicsPointerArrayReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retirePhysicsManager26FEC8(D& d)noexcept {
    d={};
    if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::physicsPointerArrayReleased)
        return payloadStop(d,0x26FEAC);
    //Actual2705FC null4B8. Garage Page never invokes slot148=274ED0;
    //the distinct144=274E90 is TriggerList only. Do not invent a manager.
    if(physicsManager4B8_)return payloadStop(d,0x26FEC8);
    retirementStage_=RetirementStage::physicsManagerReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireZoneLists27C030(D& d)noexcept {
    d={};using S=assets::OriginalStandardMaterialStatus50LA;
    const auto stop=[&](std::uint32_t pc){d.status=S::phase;d.sourcePC=pc;return false;};
    if(!scenePayloadRetirementCurrent()||!sceneSetup_||sceneSetup_->closed_)
        return stop(0x26FED0);
    auto& setup=*sceneSetup_;
    if(retirementStage_==RetirementStage::zoneListsReleased)
        return !setup.lists3D4_||stop(0x26FEE4);
    if(retirementStage_!=RetirementStage::physicsManagerReleased)
        return stop(0x26FED0);

    // The admitted270900 path really constructed27C110 and its three tables,
    // even though no row buffer was populated. A missing owner at this reached
    // stage is not proof of the source null branch or successful destruction.
    if(!setup.lists3D4_)return stop(0x26FEDC);
    auto& lists=*setup.lists3D4_;
    if(numZones34C_!=1||lists.count00!=numZones34C_||!lists.rows0C||
       !lists.capacities04||!lists.used08)return stop(0x27C054);
    // Only the actual initialized cold lists are admitted. Positive live rows
    // need their own proven lifetime/borrow handling before this source free;
    // neither their contents nor owner release may be invented from count0.
    for(std::int32_t i=0;i<lists.count00;++i)
        if(lists.used08[i]!=0||lists.capacities04[i]!=0||lists.rows0C[i])
            return stop(0x27C06C);

    //27C074 frees the per-zone pointer buffers in index order, without calling
    //any pointed-to Entity destructor. Their nulls do not skip the later table
    //releases. Explicit resets preserve source order instead of C++ reverse
    //member destruction (which would release used08 before capacities04).
    for(std::int32_t i=0;i<lists.count00;++i)lists.rows0C[i].reset();
    lists.rows0C.reset();       //27C098/27C0A0.
    lists.capacities04.reset(); //27C0B0/27C0B8.
    lists.used08.reset();       //27C0C8/27C0D0.
    setup.lists3D4_.reset();    //27C0E4(flags1), then26FEE4.
    retirementStage_=RetirementStage::zoneListsReleased;
    return true;
}

bool OriginalUiGaragePodScene50KG::retireCurrentInstanceMaster26FEF8(D& d)noexcept {
    d={};using S=assets::OriginalStandardMaterialStatus50LA;
    const auto stop=[&](std::uint32_t pc){d.status=S::phase;d.sourcePC=pc;return false;};
    if(!scenePayloadRetirementCurrent()||!sceneSetup_||sceneSetup_->closed_||
       !nativeApplication_)return stop(0x26FEE8);
    auto& setup=*sceneSetup_;
    if(retirementStage_==RetirementStage::instanceMasterReleased)
        return !setup.lists3D4_||stop(0x26FEE4);
    if(retirementStage_!=RetirementStage::zoneListsReleased||setup.lists3D4_)
        return stop(0x26FEE8);
    // The real scene population admitted zero descriptors, not absence of the
    // reached global owner. A positive live instance branch remains unsupported.
    if(setup.instanceCount3E0_!=0||setup.instanceDescriptors3DC_||!setup.instanceMaster_)
        return stop(0x26FEF8);

    //26FEEC reads CURRENT absolute444078. Never compare this with the stale
    //per-Scene native alias and substitute or reuse that alias as the target.
    //A different Scene may have overwritten444078 since this one was loaded.
    auto current=nativeApplication_->instanceMaster444078;
    if(current){
        //Only the native constructor's genuine raw10*0x108 allocation with
        //no populated rows is represented here. Check the CURRENT owner, not
        //merely this Scene's descriptor count, before any irreversible effect.
        if(!current->records00_||current->capacity04_!=10||current->growth08_!=100000||
           current->used0C_!=0||current->count10_!=0)return stop(0x2BFD40);
        //2BFD68 clears444078 in the delay slot BEFORE2BFF00 frees records00.
        //The local pin only keeps native storage valid across that source
        //operation. No stale Scene alias or displaced allocation is closed.
        nativeApplication_->instanceMaster444078.reset();
        current->close2BFD40(); //Reached2BFF28 releases the real raw capacity.
        current.reset();       //2BFD7C(flags1): drop this destructor's native pin.
        nativeApplication_->instanceMaster444078.reset(); //26FF04 caller clear.
    }
    //If the actual global was null,26FEF0 skips its destructor and caller clear.
    //Leave setup.instanceMaster_ and overwrittenInstanceMasters untouched:
    //the former is only a native lifetime alias, the latter preserves genuine
    //270FD0 displaced allocations. Final setup release belongs to the parent.
    retirementStage_=RetirementStage::instanceMasterReleased;
    return true;
}

bool OriginalUiGaragePodScene50KG::retirePrimarySearchPointers26FF14(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::instanceMasterReleased||
       !visibility_)return payloadStop(d,0x26FF14);
    if(!visibility_->retirePrimaryNodePointers26FF14(*this,d))return false;
    retirementStage_=RetirementStage::primarySearchPointersReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireSecondarySearchPointers26FF2C(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::primarySearchPointersReleased||
       !visibility_)return payloadStop(d,0x26FF2C);
    if(!visibility_->retireSecondaryNodePointers26FF2C(*this,d))return false;
    retirementStage_=RetirementStage::secondarySearchPointersReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireCollisionPointers26FF44(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::secondarySearchPointersReleased||
       !visibility_||!visibility_->sourceNodePointersReleased26FF34()||
       !sceneDispatch_||!sceneDispatch_->collisionLists3CC||!sceneDispatch_->collisionZone||
       !sceneCollision_||!sourceCollisionListRetiredForPayload()||!sceneCollision_->pageQueriesRetired_||
       numZones34C_!=1||sceneDispatch_->collisionLists3CC[0]!=sceneDispatch_->collisionZone.get())
        return payloadStop(d,0x26FF44);
    //Only the raw array. Source List27BB80 and Query11B4A0 already ran
    //under actual Page child callbacks; their native tombstones stay pinned.
    sceneDispatch_->collisionLists3CC.reset();
    retirementStage_=RetirementStage::collisionPointersReleased;return true;
}
bool OriginalUiGaragePodScene50KG::clearEnvironmentGlobal26FF54(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::collisionPointersReleased||
       !visibility_||!sceneDispatch_||sceneDispatch_->collisionLists3CC)return payloadStop(d,0x26FF54);
    if(!visibility_->clearSourceEnvironmentGlobal26FF54(*this,d))return false;
    retirementStage_=RetirementStage::environmentGlobalCleared;return true;
}
bool OriginalUiGaragePodScene50KG::retireSkyPointerArray26FF64(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::environmentGlobalCleared||
       !visibility_||!visibility_->sourceEnvironmentGlobalCleared26FF54()||
       !attachmentReset270550Known_||cubeModels3F8_)return payloadStop(d,0x26FF64);
    //2705C8 initialized NULL; this Page never calls the cube producer slot.
    //26FF5C therefore skips the free. A populated branch is not omitted.
    retirementStage_=RetirementStage::skyPointerArrayReleased;return true;
}
bool OriginalUiGaragePodScene50KG::searchListPayloadRetirementCurrent26FF6C()const noexcept {
    return scenePayloadRetirementCurrent()&&retirementStage_==RetirementStage::skyPointerArrayReleased&&
        visibility_&&visibility_->sourceSceneOwner_==this&&visibility_->sourceNodePointersReleased26FF34()&&
        visibility_->sourceEnvironmentGlobalCleared26FF54()&&sceneDispatch_&&
        !sceneDispatch_->collisionLists3CC&&attachmentReset270550Known_&&!cubeModels3F8_;
}
bool OriginalUiGaragePodScene50KG::retireSearchListPayloads26FF6C(D& d)noexcept {
    d={};if(!searchListPayloadRetirementCurrent26FF6C())return payloadStop(d,0x26FF6C);
    if(!visibility_->retirePageSceneSearchLists26FF6C(*this,d))return false;
    if(!visibility_->sceneSearchListsRetired2700E8(*this))return payloadStop(d,0x2700E8);
    retirementStage_=RetirementStage::searchListPayloadsReleased;return true;
}

bool OriginalUiGaragePodScene50KG::retireCameraZonePointers2700F8(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::searchListPayloadsReleased||
       !visibility_||!visibility_->sceneSearchListsRetired2700E8(*this)||
       !toyFields_||!toyFields_->complete_||toyFields_->closed_||
       toyFields_->cameraCount3E4_!=0||toyFields_->cameraZones3E8_)
        return payloadStop(d,0x2700F8);
    //Live TOY2723B8 produced count0 and skipped2723CC allocation.2700F0
    //reaches the corresponding null branch. No nonexistent array is freed.
    retirementStage_=RetirementStage::cameraZonePointersReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireSunFlarePointers270110(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::cameraZonePointersReleased||
       !toyFields_||!toyFields_->complete_||toyFields_->closed_||!toyFields_->sunFlareModeA1_||
       toyFields_->sunFlareCount4A8_!=0||toyFields_->sunFlares4A4_)
        return payloadStop(d,0x270110);
    //Actual retained profile-controlled read and count0 branch, not forced
    //disabled SunFlares. Source270108 skips only the proven null pointer.
    retirementStage_=RetirementStage::sunFlarePointersReleased;return true;
}
bool OriginalUiGaragePodScene50KG::retireLightManagerZones270120(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::sunFlarePointersReleased||
       !retainedLightManager426F60_)
        return payloadStop(d,0x270120);
    auto& manager=*retainedLightManager426F60_;const auto issued=manager.issuedOwner_.lock();
    if(!issued||issued.get()!=&manager||issued.owner_before(retainedLightManager426F60_)||
       retainedLightManager426F60_.owner_before(issued)||manager.issuer_!=grant_->issuer()||
       manager.entities_!=grant_->application()||!manager.current())return payloadStop(d,0x148110);
    if(sceneLighting_){
        //All source Light/Fog destructors already returned under Page142590.
        //End only their native backing/manager borrow now, before the actual
        //270120 zone release. The earlier26FE50 proof used this same owner.
        const auto& lighting=*sceneLighting_;
        if(!lighting.identityCurrent()||lighting.manager_!=retainedLightManager426F60_||
           !lighting.managerBound_||manager.sceneLightsOwner_!=&lighting||lighting.renderProxyActive_)
            return payloadStop(d,0x270120);
        if(!finishSceneLightingPayload273AA0(d))return false;
    }
    //A retry after native backing release still needs the exact live manager
    //and unchanged source zone preflight; no Light/Fog destructor is replayed.
    if(manager.sceneLightsOwner_||!manager.zoneReleasePreflight())return payloadStop(d,0x148110);
    //The actual current singleton identity is part of manager.current(). Its
    //real node/List refs and arrays are released by the existing148110 owner.
    //Flat UI registry, global426F60 and source defaults remain alive.
    OriginalUiLightManagerOwner50KG::Diagnostic md;
    if(!manager.releaseZones148110(md))return payloadStop(d,md.sourcePC?md.sourcePC:0x148110);
    retirementStage_=RetirementStage::lightManagerZonesReleased;
    retainedLightManager426F60_.reset();return true;
}
bool OriginalUiGaragePodScene50KG::retireDefaultZoneObserver270138(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::lightManagerZonesReleased||
       retainedLightManager426F60_||!visibility_)return payloadStop(d,0x270138);
    if(!visibility_->retireDefaultZoneObserver270138(*this,d))return false;
    if(!visibility_->defaultZoneObserverRetired270144(*this))return payloadStop(d,0x270144);
    retirementStage_=RetirementStage::defaultZoneObserverReleased;return true;
}

bool OriginalUiGaragePodScene50KG::retireAuralRegistrations27015C(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::defaultZoneObserverReleased||
       !visibility_||!visibility_->defaultZoneObserverRetired270144(*this)||!grant_->issuer())
        return payloadStop(d,0x27015C);
    //Main00 is a real process-lifetime constructor owner, not the Scene's
    //SoundGroup4D8, AI/Droid backend, or a null inferred from absent TOY rows.
    //103DD0 clears its actual registrations; it neither destroys nor nulls it.
    OriginalApplicationAural50KG::Diagnostic ad;
    if(!grant_->issuer()->clearAuralRegistrations103DD0(ad))
        return payloadStop(d,ad.sourcePC?ad.sourcePC:0x103DD0);
    retirementStage_=RetirementStage::auralRegistrationsCleared;return true;
}
bool OriginalUiGaragePodScene50KG::retireFogInterpolationSlots270164(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::auralRegistrationsCleared||
       !fogInterpolationReset27061C_||!fogInterpolationBranch270D24_||!sceneSetup_||
       sceneSetup_->perZoneFog4F0_)return payloadStop(d,0x270164);
    constexpr std::array<std::uint32_t,5> callPC{{0x270170,0x270184,0x270198,0x2701AC,0x2701C0}};
    for(std::size_t i=0;i<fogInterpolation4DC_.size();++i){
        //These are the actual five reached null branches produced by27061C
        //and the live275E30/276300 flag at270D24. A positive allocation is not
        //supported by the bounded read path and cannot be silently discarded.
        if(fogInterpolation4DC_[i])return payloadStop(d,callPC[i]);
    }
    retirementStage_=RetirementStage::fogInterpolationSlotsReleased;return true;
}
bool OriginalUiGaragePodScene50KG::finishSceneRetirement2701CC(D& d)noexcept {
    d={};if(!scenePayloadRetirementCurrent()||retirementStage_!=RetirementStage::fogInterpolationSlotsReleased||
       !visibility_||!visibility_->closed_||!visibility_->defaultZoneObserverRetired270144(*this)||
       !visibility_->sceneSearchListsRetired2700E8(*this)||sceneLighting_||retainedLightManager426F60_||
       !sceneSetup_||!sceneSetup_->complete_||sceneSetup_->closed_||sceneSetup_->startData348_||
       sceneSetup_->environment3A4_||sceneSetup_->fog3AC_||sceneSetup_->zones350_||sceneSetup_->lists3D4_||
       sceneSetup_->instanceCount3E0_||sceneSetup_->instanceDescriptors3DC_||
       !sceneDispatch_||sceneDispatch_->terrain3B0||sceneDispatch_->collisionLists3CC||
       !sceneDispatch_->collisionZone||!sceneDispatch_->collisionZone->retired||
       sceneDispatch_->collisionZone->live||sceneDispatch_->collisionZone->retiring||
       sceneDispatch_->collisionZone->records34||!staticModels_||!staticModels_->closed_||
       staticModels_->pageRetirement_!=OriginalUiGarageStaticModels50KG::PageRetirement::complete||
       !sceneCollision_||!sceneCollision_->pageQueriesRetired_||!sourceCollisionListRetiredForPayload()||
       !sceneParticles_||!sceneParticles_->retired||sceneParticles_->entityLive||sceneParticles_->retiring||
       !sceneEmitters_||!sceneEmitters_->retired||!sceneEmitters_->listRetired||!sceneEmitters_->payloadReleased||
       !toyFields_||!toyFields_->complete_||toyFields_->triggers3D0_||!toyFields_->particles3D8_||
       toyFields_->particles3D8_->live||toyFields_->particles3D8_->entity.reference00||
       toyFields_->particles3D8_->entity.firstChild18||
       toyFields_->particles3D8_->records34||toyFields_->physics4B4_||toyFields_->cameraZones3E8_||
       toyFields_->sunFlares4A4_||toyFields_->buoys53C_||!audioFields_||!audioFields_->complete||
       audioFields_->ambient3B4||audioFields_->soultree3B8||!audioFields_->collision3BC||!audioFields_->driveOn3C0||
       worldList500_||interpolation510_||interpolation514_||animationRows52C_||animationRows530_||
       water400_||waterRenderer404_||ecosystem408_||cubeModels3F8_||podScene_.firstChild18)
        return payloadStop(d,0x2701CC);
    if(const auto* lights=sceneSetup_->lights3A8_.get()){
        if(!lights->pageRetired||lights->entityLive||lights->pageRetiring||lights->entity.reference00||
           lights->entity.firstChild18||lights->lights38||lights->zoneColors3C)return payloadStop(d,0x2781B0);
    }
    for(const auto* list:{audioFields_->collision3BC.get(),audioFields_->driveOn3C0.get()})
        if(!list->retired||list->live||list->retiring||list->entity.reference00||list->entity.firstChild18||list->rows34)
            return payloadStop(d,0x278B90);
    for(const auto& row:sceneEmitters_->rows)
        if(row&&(!row->retired||row->entityLive||row->retiring||row->entity.reference00))
            return payloadStop(d,0x15A720);
    const auto released=grant_->finishScenePodRetirement1413E0(*this,podScene_);
    if(!released.okay())return payloadStop(d,released.sourcePC?released.sourcePC:0x1413E0);
    //Only native envelope custody closes here. Every guest allocation/ref,
    //observer, list and payload above already has its ordered source release.
    //Keep native dead-field tombstones and Main's Page alias; no unregister,
    //global replacement, source-word clearing, or detached cleanup is invoked.
    sceneSetup_->closed_=true;sceneDispatch_->closed=true;toyFields_->closed_=true;
    audioFields_->closed=true;sceneCollision_->closed_=true;
    sceneLive_=false;retirementStage_=RetirementStage::complete;return true;
}

}
