#include "original_results104_storage50la.h"
#include "../../platform/linux/native/original_application_aural50kg.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/original_race_hud_scope50la.h"
#include "../../tests/native/original_position_stats_adapter50la.h"
#include <array>
#include <optional>
#include <thread>

namespace original_position_stats_adapter50LA {
bool Owner::captureEndObservation1AB520(const rr::game::OriginalApplicationEntities50LA* application,
    const rr::assets::OriginalStandardMaterialTable50LA* materials,std::uint64_t generation,
    EndObservation& output,Diagnostic& d)const noexcept {
    d={};if(!camera_||!manager_)return false;
    try {
        const auto& c=*camera_;const auto publication=c.publicationCount_;
        if(!resultsRowsCurrent(application,materials,generation,publication)){
            d.status=Status::identity;return false;
        }
        //Same native manager30 extent as existing PositionStats observation.
        //The supported actual Bank is bounded1..8, not an invented guest list.
        EndObservation next{publication,static_cast<std::int32_t>(c.actual_.bank.count())};
        if(!resultsRowsCurrent(application,materials,generation,publication)){
            d.status=Status::phase;return false;
        }
        output=next;d.status=Status::complete;return true;
    }catch(...){d.status=Status::read;return false;}
}
bool Owner::readEndRetirementCD4(const ResultsRecordIdentity& row,
    std::uint32_t& output,Diagnostic& d)const noexcept {
    d={};
    try {
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::identity;return false;}
        const auto value=camera_->actual_.bank.read(row.managerIndex,camera_->reads_.retirementCD4);
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::phase;return false;}
        output=value;d.status=Status::complete;return true;
    }catch(...){d.status=Status::read;return false;}
}
bool Owner::readEndPlaceCCC(const ResultsRecordIdentity& row,
    std::int32_t& output,Diagnostic& d)const noexcept {
    d={};
    try {
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::identity;return false;}
        const auto& m=*manager_;const auto i=row.managerIndex;
        //Read the actual manager-index CCC, never a sorted display row. No
        //positive/range/unique-place gate:1ABE70 compares signed words as-is.
        if(!m.fields[i].bound||!m.observed[i].bound||m.observed[i].managerIndex!=i||
           m.fields[i].placeCCC!=m.rank[i].placeCCC||m.observed[i].placeCCC!=m.rank[i].placeCCC){
            d.status=Status::identity;return false;
        }
        const auto value=m.rank[i].placeCCC;
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::phase;return false;}
        output=value;d.status=Status::complete;return true;
    }catch(...){d.status=Status::read;return false;}
}
}

namespace rr::game {

void OriginalRaceConstructor50LA::continuePreAck1AB520(
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalRaceEndEntryDiagnostic50LA& d)noexcept {
    using S=OriginalRaceEndEntryDiagnostic50LA::Status;
    if(resultsFeedbackThread_!=std::this_thread::get_id()){
        d.reachedStateKnown=false;d.status=S::owner;d.sourcePC=0x1ab520;
        d.operation="Race pre-ack continuation requires its construction thread";return;
    }
    const auto reached=[&]()noexcept {
        d.reachedStateKnown=true;d.hudStopCommitted=endEntryStep_>=EndEntryStep::dispatch;
        d.recordBit7E4Committed=endEntryStep_>=EndEntryStep::recordBit;
        d.rivalWord7E8Committed=endEntryStep_>=EndEntryStep::rivalWord;
        d.acknowledgement20CRead=endEntryStep_>=EndEntryStep::ackRead&&bool(endAcknowledgement20C_);
        d.waitingBit5Committed=endEntryStep_>=EndEntryStep::waitingBit;
        d.resultsModeScalarReached=endEntryStep_>=EndEntryStep::waitingModeScalar;
        d.resultsModeFeedbackReturned=endEntryStep_>=EndEntryStep::waitingPlaces;
        d.dispatchReturned1AB520=endDispatchReturned_;
        if(d.acknowledgement20CRead)d.observedAcknowledgement20C=*endAcknowledgement20C_;
    };
    const auto stop=[&](S status,std::uint32_t pc,const char* operation)noexcept {
        reached();d.status=status;d.sourcePC=pc;d.operation=operation;
    };
    if(endEntryStep_<EndEntryStep::dispatch||!endEntryProfile_||!profile_||!counted_||rolledBack_){
        stop(S::owner,0x1ab520,"Actual suspended Race/HUD prefix absent");return;
    }
    //Original first tests104, then actual Profile11258. Non-results/subtype
    //branches stop at their real target; no generic virtual-return success.
    if(!results104_){stop(S::incomplete,0x1abbc0,"Source null104 branch reached; separate tail remains unimplemented");return;}
    platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selection;
    if(!profile_->courseRaceSnapshot(selection)){
        stop(S::profile,0x1ab55c,"Actual Profile11258 read unavailable");return;
    }
    if(selection.subtype!=1u){stop(S::incomplete,0x1abbc0,"Source non-subtype1 branch reached; separate tail remains unimplemented");return;}
    const auto profileCurrent=[&]()noexcept {
        const auto now=profile_->stamp();
        return endEntryProfile_&&now.generation==endEntryProfile_->generation&&now.mode34==endEntryProfile_->mode34;
    };
    if(!profileCurrent()||!results104_->endDispatchOwnerCurrent1AB520(*this,observer)){
        stop(S::owner,0x1ab568,"Actual same Profile/Results loaded-Operation/Bank ownership unavailable");return;
    }
    using Observation=::original_position_stats_adapter50LA::Owner;
    Observation::EndObservation current;Observation::Diagnostic observed;
    const auto* materials=results104_->impl_->domain->materials382B38().get();
    if(!observer.captureEndObservation1AB520(application_.get(),materials,entity_.sessionGeneration,current,observed)){
        stop(S::owner,0x1ab568,"Actual published synchronous Camera/manager read unavailable");return;
    }
    if(endDispatchPublication_&&*endDispatchPublication_!=current.publication){
        stop(S::owner,0x1ab568,"Suspended invocation publication changed; preceding stores retained, never replayed");return;
    }
    endDispatchPublication_=current.publication;
    const auto currentOwners=[&]()noexcept {
        return profileCurrent()&&results104_->endDispatchOwnerCurrent1AB520(*this,observer)&&
            observer.resultsRowsCurrent(application_.get(),materials,entity_.sessionGeneration,current.publication);
    };
    const auto identity=[&](std::size_t i,Observation::ResultsRecordIdentity& row)noexcept {
        return currentOwners()&&observer.captureResultsRecordIdentity(application_.get(),materials,
            entity_.sessionGeneration,i,row,observed)&&row.publication==current.publication;
    };
    if(endEntryStep_==EndEntryStep::dispatch){
        using Query=OriginalRaceRecordQualification50KG;
        std::array<OriginalRaceRecordQualificationRacerObservation50KG,2> rows{};
        std::size_t available=0;std::optional<Query> query;Query::Diagnostic qd;
        //Only the canonical Race-owned lanes may supply1AC100. Constructor
        //known-maskFC deliberately leaves low2 absent until actual1A6C10/50
        //after the retained268300 reset. Session's numeric1 and arena human
        //count never fill a missing lane or authorize this source read.
        const std::optional<std::uint8_t> low2=(flags1B0KnownMask_&3u)==3u?
            std::optional<std::uint8_t>{std::uint8_t(flags1B0KnownValue_&3u)}:std::nullopt;
        //Ask the existing exact predicate first: source early exclusions must
        //not consume racer timing operands. Missing-racer asks for the next
        //actual manager index; retired rows likewise never read timing/CD0.
        while(!profile_->prepareRaceRecordQualification1ABFC0(0,low2,
            rows.data(),available,query,qd)){
            if(qd.status==Query::Status::missingRaceFlags){
                stop(S::owner,0x1ac100,"Canonical Race1B0 low2 producer after268300 absent; no7E4 store");return;
            }
            if(qd.status!=Query::Status::missingRacer||qd.managerIndex!=available||available>=rows.size()){
                stop(S::source,qd.sourcePC,"Original readonly1ABFC0 unsupported; no7E4 store");return;
            }
            Observation::ResultsRecordIdentity row;
            auto& value=rows[available];
            if(!identity(available,row)||!observer.readEndRetirementCD4(row,value.retirementCD4,observed)){
                stop(S::owner,qd.sourcePC,"Actual1AF330 qualification racer/CD4 read unavailable");return;
            }
            if(!value.retirementCD4&&
               (!observer.readResultsBestLapCB4(row,value.records.bestLapCB4,observed)||
                !observer.readResultsRaceTimeCB8(row,value.records.timeCB8,observed)||
                !observer.readResultsObservedTimingCD0(row,value.records.countCD0,observed))){
                stop(S::owner,qd.sourcePC,"Actual qualification timing lanes unavailable; CD0 is observed, never credited");return;
            }
            ++available;
        }
        platform::OriginalUiRiderProfileStamp50KG after;
        if(!query||!currentOwners()||!profile_->storeEndRecordBit1AB598(*query,*endEntryProfile_,after)){
            stop(S::profile,0x1ab598,"Actual same-Profile7E4 store rejected before mutation");return;
        }
        //No throw/yield/callback between store, expected-stamp refresh and
        //cursor advance. This source delay-slot effect survives later failure.
        endEntryProfile_=after;endEntryStep_=EndEntryStep::recordBit;
    }
    if(endEntryStep_==EndEntryStep::recordBit){
        using Query=OriginalRaceRivalQualification50KG;
        std::array<OriginalRaceRivalRacerObservation50KG,8> rows{};
        std::size_t available=0;std::optional<Query> query;Query::Diagnostic qd;
        while(!profile_->prepareRaceRivalQualification1ABE70(rows.data(),available,
            current.managerCount30,query,qd)){
            if(qd.status!=Query::Status::missingRacer||qd.managerIndex!=available||available>=rows.size()){
                stop(S::source,qd.sourcePC,"Original readonly1ABE70 unsupported; prior7E4 retained");return;
            }
            Observation::ResultsRecordIdentity row;auto& value=rows[available];
            if(!identity(available,row)){
                stop(S::owner,qd.sourcePC,"Actual rival getter-index identity unavailable; prior7E4 retained");return;
            }
            value.racer60=row.racer60;
            //1ABEE4 first reads row0 CCC. Later scanned rows read60 first
            //and only the first matching row consumes CCC (1ABF50/5C).
            if(available!=0&&!qd.scanRival11280){
                stop(S::source,qd.sourcePC,"Original rival scan request lacks its descriptor; prior7E4 retained");return;
            }
            if((available==0||row.racer60==static_cast<std::uint32_t>(*qd.scanRival11280))&&
               !observer.readEndPlaceCCC(row,value.placeCCC,observed)){
                stop(S::owner,qd.sourcePC,"Actual reached rival CCC read unavailable; prior7E4 retained");return;
            }
            //Only row0 CD4 is read by1ABE70; later retirement fields and
            //nonmatching CCC remain unconsumed, not invented known facts.
            if(available==0&&!observer.readEndRetirementCD4(row,value.retirementCD4,observed)){
                stop(S::owner,0x1abee8,"Actual row0 rival-retirement read unavailable; prior7E4 retained");return;
            }
            ++available;
        }
        platform::OriginalUiRiderProfileStamp50KG after;
        if(!query||!currentOwners()||!profile_->storeEndRivalWord1AB5A8(*query,*endEntryProfile_,after)){
            stop(S::profile,0x1ab5a8,"Actual same-Profile7E8 store rejected; prior7E4 retained");return;
        }
        endEntryProfile_=after;endEntryStep_=EndEntryStep::rivalWord;
    }
    if(endEntryStep_==EndEntryStep::rivalWord){
        //The actual20C read occurs ONLY AFTER both original stores. No ack
        //argument, Enter mapping, mode setter or inferred completion is used.
        if(!currentOwners()||!results104_->impl_||!results104_->impl_->derived){
            stop(S::owner,0x1ab5b0,"Actual Results20C read unavailable; both Profile stores retained");return;
        }
        endAcknowledgement20C_=results104_->impl_->derived->acknowledgement20C;
        endEntryStep_=EndEntryStep::ackRead;
    }
    if(endEntryStep_<EndEntryStep::ackRead||!endAcknowledgement20C_){
        stop(S::owner,0x1ab5b0,"Suspended pre-ack cursor inconsistent");return;
    }
    if(*endAcknowledgement20C_){
        stop(S::incomplete,0x1ab5bc,"Actual nonzero20C branch; input/mode/reward path remains unimplemented");return;
    }
    if(endDispatchReturned_){
        stop(S::incomplete,0x1abe5c,"Actual1AB520 epilogue already returned; caller tail has not returned");return;
    }
    if(endEntryStep_==EndEntryStep::ackRead){
        //1AB918/91C/920 test ONLY bit5 (dsll32 by26; dsrl32 by31).
        //Never demand unknown low2 before the source reaches its later read.
        if((flags1B0KnownMask_&0x20u)!=0x20u){
            stop(S::owner,0x1ab918,"Canonical Race1B0 bit5 unknown; no waiting-bit store");return;
        }
        if(flags1B0KnownValue_&0x20u){
            //1AB924 jumps to the real register-restore epilogue1ABE48..60.
            //Only THIS callee returned: no mode call, scan, new invocation,
            //caller tail or root-complete claim follows from the set bit.
            endDispatchReturned_=true;
            stop(S::incomplete,0x1abe5c,"Actual already-set bit5 branch returned1AB520; caller tail remains");return;
        }
        //1AB93C: update the actual represented Race byte, preserving every
        //other lane AND its known-mask, including genuinely loaded low2. Cursor
        //moves with the source store, before the fallible later Results call.
        flags1B0KnownValue_=std::uint8_t((flags1B0KnownValue_&~0x20u)|0x20u);
        endEntryStep_=EndEntryStep::waitingBit;
    }
    using Mode=OriginalResults10450LA::ModeCpuPrefix;
    if(endEntryStep_==EndEntryStep::waitingBit||endEntryStep_==EndEntryStep::waitingModeScalar){
        OriginalResults104Diagnostic50LA modeDiagnostic;
        //1AB944 A1=1;1AB94C delay slot A2=0. Scalar changes precede the
        //mandatory feedback tail even when its actual device borrow is absent.
        const auto mode=endEntryStep_==EndEntryStep::waitingBit?
            results104_->setModeCpuPrefix1CA8E0(*this,observer,1u,0u,modeDiagnostic):
            results104_->continueModeFeedbackCpu1AF770(*this,observer,modeDiagnostic);
        if(mode==Mode::none){
            stop(S::owner,modeDiagnostic.sourcePC,modeDiagnostic.operation?modeDiagnostic.operation:
                "Actual Results mode continuation unavailable; prior waiting bit retained");return;
        }
        endEntryStep_=EndEntryStep::waitingModeScalar;
        if(mode==Mode::scalarPrefix){
            stop(S::owner,modeDiagnostic.sourcePC,modeDiagnostic.operation?modeDiagnostic.operation:
                "Actual Results feedback tail unavailable; scalar effects retained without replay");return;
        }
        //Both genuine tail variants return through1AF7E0/7E4 to1AB950.
        //The lexical clear is infallible once admitted, with no yield/callback.
        //Retain the next original locals before the first low2 read.
        endWaitingPlaceScan_.emplace();
        endEntryStep_=EndEntryStep::waitingPlaces;
    }
    if(endEntryStep_!=EndEntryStep::waitingPlaces||!endWaitingPlaceScan_){
        stop(S::owner,0x1ab950,"Suspended zero20C cursor inconsistent; prior effects retained");return;
    }
    auto& scan=*endWaitingPlaceScan_;
    using Cut=EndWaitingPlaceScan::Cut;
    const auto low2=[&](std::uint32_t pc,std::int32_t& count)noexcept {
        //Each source loop condition reloads the same canonical byte. Never
        //cache arena humanCount or consume low2 before the reached condition.
        if(!currentOwners()||(flags1B0KnownMask_&3u)!=3u){
            stop(S::owner,pc,"Actual canonical low2 read unavailable; prior source locals/effects retained");
            return false;
        }
        count=static_cast<std::int32_t>(flags1B0KnownValue_&3u);return true;
    };
    const auto place=[&](std::uint32_t pc,std::int32_t& value)noexcept {
        Observation::ResultsRecordIdentity row;
        if(scan.indexS0<0||!identity(static_cast<std::size_t>(scan.indexS0),row)||
           !observer.readEndPlaceCCC(row,value,observed)){
            stop(S::owner,pc,"Actual manager-index1AF330/signedCCC read unavailable; scan resumes at this getter");
            return false;
        }
        return true;
    };
    for(;;){
        std::int32_t count=0,value=0;
        switch(scan.cut){
        case Cut::minimumLow2:
            if(!low2(0x1ab988,count))return;
            if(scan.indexS0<count){scan.cut=Cut::minimumGetter;break;}
            //The first minimum is deliberately discarded by original99C.
            scan.valueS1=-1;scan.minimumS2=12;scan.indexS0=0;
            scan.cut=Cut::selectionLow2;break;
        case Cut::minimumGetter:
            if(!place(0x1ab960,value))return;
            if(value<scan.valueS1)scan.valueS1=value; //SignedSLT, no place gate.
            ++scan.indexS0;scan.cut=Cut::minimumLow2;break;
        case Cut::selectionLow2:
            if(!low2(0x1ab9f8,count))return;
            if(scan.indexS0<count){scan.cut=Cut::selectionGetter;break;}
            scan.indexS0=0;scan.cut=Cut::controllerLow2;break;
        case Cut::selectionGetter:
            if(!scan.minimumS2){
                stop(S::source,0x1ab9bc,"Suspended scan lacks reached sourceS2");return;
            }
            if(!place(0x1ab9b0,value))return;
            if(value<*scan.minimumS2){
                //1AB9D4 is the SECOND getter's delay-slot write. Preserve
                //it on rejection there; never reuse the firstCCC operand.
                scan.valueS1=scan.indexS0;scan.cut=Cut::selectedGetter;break;
            }
            if(scan.valueS1==-1)scan.valueS1=0; //1AB9EC, even CCC>=12.
            ++scan.indexS0;scan.cut=Cut::selectionLow2;break;
        case Cut::selectedGetter:
            if(!place(0x1ab9d0,value))return;
            scan.minimumS2=value; //The second actual read at1AB9D8.
            if(scan.valueS1==-1)scan.valueS1=0;
            ++scan.indexS0;scan.cut=Cut::selectionLow2;break;
        case Cut::controllerLow2:
            if(!low2(0x1aba40,count))return;
            if(scan.indexS0>=count){scan.cut=Cut::owner78;break;}
            //BLEZL at1ABA14 takes its increment delay slot only at index0.
            if(scan.indexS0<=0){++scan.indexS0;break;}
            scan.cut=Cut::controllerSlot;break;
        case Cut::controllerSlot:
            stop(S::incomplete,0x1aba24,
                "Actual nonzero-index Race34 controller/virtual18 owner remains unrepresented; scans retained");return;
        case Cut::owner78: {
            //Only actual1A6E48/7C publication may select this positive branch.
            //Constructor-null/native absence is NOT the source loaded-null path.
            if(!vcr78Owner_||vcr78Source_!=vcr78Owner_.get()||
               !vcr78Source_->belongsToRace(*this)){
                stop(S::owner,0x1aba54,"Actual published same-Race78 unavailable; no null fallback");return;
            }
            scan.savedVcr=vcr78Source_;scan.indexS0=0; //1ABA68 delay slot.
            scan.cut=Cut::vcrStopped;scan.stoppedPC=0x1abaa0;
            OriginalSimpleVCRDiagnostic50LA vd;
            if(!scan.savedVcr->enableEndChannelsAndBackup1ABA68(*this,scan.indexS0,vd)){
                scan.stoppedPC=vd.sourcePC;
                stop(S::incomplete,vd.sourcePC,vd.operation);return;
            }
            scan.cut=Cut::vcrFlag;break;
        }
        case Cut::vcrStopped:
        case Cut::worldResetStopped:
        case Cut::pauseStopped:
        case Cut::auralStopped:
        case Cut::musicFadeStopped:
        case Cut::particleStopped:
        case Cut::worldRestartStopped:
        case Cut::racerRestartStopped:
            stop(S::incomplete,scan.stoppedPC,
                "Waiting-results restart prefix stopped; actual writes retained without retry or replay");return;
        case Cut::vcrFlag:
            if(vcr78Source_!=scan.savedVcr||!scan.savedVcr||
               !scan.savedVcr->belongsToRace(*this)){
                stop(S::owner,0x1ababc,"Saved Race78 replaced after channel stores/backup");return;
            }
            //1ABABC read and1ABADC JAL delay store: set only bit4. Preserve
            //all other represented lanes AND their unknown mask.
            flags1B0KnownValue_=std::uint8_t(flags1B0KnownValue_|0x10u);
            flags1B0KnownMask_=std::uint8_t(flags1B0KnownMask_|0x10u);
            scan.cut=Cut::worldReset;break;
        case Cut::worldReset: {
            //Actual1AC8E0(A1=0), first mandatory call: World74+3D0,
            //TriggerList2DC320 table3D5230 slot80=2DCC70. No generic virtual
            //return or use of a copied trigger descriptor is admitted.
            scan.cut=Cut::worldResetStopped;scan.stoppedPC=0x1ac8f8;
            if(!endWorldCurrentNative()){
                stop(S::owner,scan.stoppedPC,"Actual same-Race restart World74 unavailable");return;
            }
            if(!endResetNativeTriggerList2DCC70(
                scan.stoppedPC)){
                stop(S::incomplete,scan.stoppedPC,"Actual World3D0 reset rejected; VCR and Race bit4 retained");return;
            }
            scan.cut=Cut::pause;break;
        }
        case Cut::pause: {
            //1AC914..938 reads the SAME App11C owner. Source nonzero becomes
            //zero; zero performs no store. Existing native pause writer touches
            //only that shared word, not transport/input/timestamp/single-step.
            scan.cut=Cut::pauseStopped;scan.stoppedPC=0x1ac920;
            const auto scope=endHud_.lock();std::uint32_t pause=0;
            if(!scope||!scope->application_||scope->application_->entities()!=application_||
               scope->application_->race28C()!=this||
               !scope->application_->animationClock().readPause11C(pause)){
                stop(S::owner,scan.stoppedPC,"Actual same-App11C owner unavailable after trigger reset");return;
            }
            scan.pause11C=pause;
            if(pause)scope->application_->setExplicitAnimationPause(false); //1AC938.
            scan.cut=Cut::auralStop;break;
        }
        case Cut::auralStop: {
            scan.cut=Cut::auralStopped;scan.stoppedPC=0x1ac944;
            const auto scope=endHud_.lock();
            if(!scope||!scope->application_||scope->application_->entities()!=application_||
               scope->application_->race28C()!=this){
                stop(S::owner,scan.stoppedPC,"Actual same-App restart Aural read unavailable");return;
            }
            platform::OriginalApplicationAural50KG::Diagnostic ad;
            if(!platform::OriginalApplicationAural50KG::restartOwnedList103210(*scope->application_,
                application_,scan.auralIndexS0,scan.auralCount5C,scan.auralReachedEmitter,ad)){
                scan.stoppedPC=ad.sourcePC;
                stop(S::incomplete,scan.stoppedPC,
                    "Actual Aural103210 owner/positive child boundary; retained iterator and prior restart effects");return;
            }
            scan.cut=Cut::musicFade;break;
        }
        case Cut::musicFade: {
            scan.cut=Cut::musicFadeStopped;scan.stoppedPC=0x1ac964;
            const auto scope=endHud_.lock();OriginalRaceMusicManagerDiagnostic50LA md;
            if(!scope||!music10C_){
                stop(S::owner,scan.stoppedPC,"Actual restart music10C owner unavailable");return;
            }
            if(!music10C_->restartNativeFade1AC960(*this,*scope,md)){
                scan.stoppedPC=md.sourcePC;stop(S::incomplete,scan.stoppedPC,md.operation);return;
            }
            scan.cut=Cut::particleBoundary;break;
        }
        case Cut::particleBoundary:
            scan.cut=Cut::particleStopped;scan.stoppedPC=0x1ac96c;
            if(!endWorldCurrentNative()){
                stop(S::owner,scan.stoppedPC,"Actual restart World4A0 owner unavailable");return;
            }
            if(!endResetNativeParticles161F60(scan.stoppedPC)){
                stop(S::incomplete,scan.stoppedPC,
                    "Actual particle pointer/auxiliary reset stopped; ordered effects retained without replay");return;
            }
            scan.cut=Cut::worldRestartBoundary;break;
        case Cut::worldRestartBoundary:
            scan.cut=Cut::worldRestartStopped;scan.stoppedPC=0x1ac98c;
            if(!endWorldCurrentNative()){
                stop(S::owner,scan.stoppedPC,"Actual World2702E0 owner unavailable");return;
            }
            if(!endResetLiveWorldPhysics2702E0(scan.stoppedPC)){
                stop(S::incomplete,scan.stoppedPC,
                    "Actual World restart stopped; ordered owner effects retained without replay");return;
            }
            scan.stoppedPC=0x1ac994;field90_=0; //Actual SAME Race scalar after World return.
            scan.cut=Cut::racerRestart;break;
        case Cut::racerRestart: {
            //1AC998 loads the actual B0.1AC9A4 passes the saved original
            //1AC8E0 A1, which is zero for THIS1ABAD8 Results call (delay1ABADC).
            // Claim before any16E830 mutation; never replay a reached AI row.
            scan.cut=Cut::racerRestartStopped;scan.stoppedPC=0x1ac998;
            const auto scope=endHud_.lock();
            if(!endWorldCurrentNative()||!scope||!racerB0_){
                stop(S::owner,scan.stoppedPC,"Actual same-Race native B0/World/Scope unavailable");return;
            }
            if(!scope->restartRacerScalars1AEA70(*this,0u,scan.stoppedPC)){
                stop(S::incomplete,scan.stoppedPC,
                    "Actual B0 restart stopped; reached AI scalar resets retained before placement dependency");return;
            }
            // The current bounded callee never reaches1AEA70's return. A
            // future implementation must join the real1AED30 successor here.
            scan.stoppedPC=0x1ac9a8;
            stop(S::incomplete,scan.stoppedPC,"Actual B0 returned; separate1AED30 body reset remains unjoined");return;
        }
        }
    }
}
}
