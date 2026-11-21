#include <cstdio>
#include "original_application_aural50kg.h"
#include "original_audio_pending_waves50kg.h"
#include "original_ui_main_audio50kg.h"
#include "original_ui_garage_audio_backing50kg.h"
#include "native_source_reverb50kg.h"
#include "race_audio_groups50ic.h"
#include "../../../tests/native/race_music50ie_fixture.h"
#include "../../../tests/native/original_camera_cycle_audio50kx.h"
#include "../../../tests/native/source_announcer50ie_fixture.h"
#include "../../../tests/native/results_engine_stop26d070.inl"
#include "../../../tests/native/race_audio50hq_fixture.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <limits>
#include <string>

namespace rr::platform {
bool OriginalApplicationAural50KG::finishResultsNative267BE0(
    const Application& root,const RaceAudioGroups50IC& groups,Diagnostic& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id())return fail(d,Status::affinity,0x267be0);
    if(issuer_!=&root||root.sourceAural00Owner_.get()!=this||!current()||
       !backendCurrent()||!nativeStreamSlotsCurrent()||!groups.current()||
       groups.coldApplication_!=entities_||!groups.coldStarted_||!groups.mixer_)
        return fail(d,Status::owner,0x267be0);
    auto& i=**interfaceA0_;auto& main=*i.main;
    const bool closed=main.ordinaryOutputRetired_;
    // Ordinary Main.current() deliberately rejects retired frontend output.
    // This distinct Results input scope instead requires its genuine same
    // Root closed-output receipt; never weaken Main or generic Root admission.
    if(i.controlBoundaryCount==std::numeric_limits<std::uint64_t>::max()||
       !main.precedingGroupControlsCurrent()||
       (closed?(!root.frontendAudioClosedReceiptCurrent()||main.pendingNativeGroupControls()):!main.current()))
        return fail(d,Status::backend,0x267be0);
    const auto frame=i.mixer->mixedFrames();const auto raceFrame=groups.mixer_->mixedFrames();
    const auto raceRevision=groups.mixer_->controlRevision();
    std::array<std::uint64_t,8> refs{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,refs))
        return fail(d,Status::backend,0x267be0);
    for(std::size_t n=0;n<refs.size();++n)
        if(refs[n]&&(!i.streamSlots[n].occupied||refs[n]!=i.streamSlots[n].generation))
            return fail(d,Status::owner,0x267be0);
    // SAME genuine native control consumer as267BE0. Actual play/stop/fade
    // controls already published synchronously; only owned command25 routing
    // may remain pending. Closed frontend requires none and consumes its real
    // empty pending set, not an invented EE packet or hardware completion.
    std::string detail;
    if(!main.serviceColdGarageRouting(detail))return fail(d,Status::backend,0x266a10);
    if(main.pendingNativeGroupControls()||!main.precedingGroupControlsCurrent()||
       !nativeStreamSlotsCurrent()||!groups.current()||
       i.mixer->mixedFrames()!=frame||i.wetProcessor->nextFrame()!=frame||
       groups.mixer_->mixedFrames()!=raceFrame||groups.mixer_->controlRevision()!=raceRevision||
       (closed&&!root.frontendAudioClosedReceiptCurrent()))
        return fail(d,Status::backend,0x267c3c);
    ++i.controlBoundaryCount;i.controlBoundaryFrame=frame;
    i.controlBoundaryMainRevision=main.controlRevision_;
    i.controlBoundaryMixerRevision=i.mixer->controlRevision();
    i.controlBoundaryGroupSequence=main.nativeGroupControlSequence_;
    // No audio mix, stream aging, device reopen/drain, DSP clear or guest RPC.
    d={Status::complete,0x267ca4};return true;
}
bool AudioMixer50HD::canStopNativeSource26D070(
    const OriginalApplicationAural50KG& owner,bool introOwnsPool)const noexcept {
    if(!nativeSourceWetProcessor_||!nativeSourceWetProcessorCurrent(owner,*nativeSourceWetProcessor_)||
       !voices_||!canRecordControlChanges(1))return false;
    std::array<bool,48> claimed{};
    for(const auto& cell:nativeSourceRoutes_.cells_)
        if(cell.kind==NativeVoiceKind::unknown)return false; // Actual A30 bit0 must be known.
    for(std::size_t n=0;n<count_;++n){const auto& route=nativeSourceVoiceRoutes_[n];
        if(introOwnsPool){if(voices_[n].active||route.count)return false;continue;}
        if(voices_[n].active&&!route.count)return false;
        if(!route.count)continue;
        if(!nativeSourceRoutes_.current(route)||!validClip(voices_[n].clip)||
           route.count!=voices_[n].clip.channels)return false;
        for(unsigned j=0;j<route.count;++j){
            const auto index=route.cells[j];if(claimed[index])return false;claimed[index]=true;
        }
    }
    // An intro lease owns its own routes in THIS same pool but a different
    // actual mixer; Aural independently proves every one via current().
    if(!introOwnsPool)for(std::size_t n=0;n<claimed.size();++n)
        if(nativeSourceRoutes_.cells_[n].reserved!=claimed[n])return false;
    return true;
}
void AudioMixer50HD::stopNativeSourcePreflighted26D070()noexcept {
    // Native atomic transport adaptation: acknowledge real source-owned PCM
    // stop before publishing EE free reservations. No callback/mix in between.
    // Generic stopAll is forbidden: it also touches unrelated logical voices.
    for(std::size_t n=0;n<count_;++n)if(nativeSourceVoiceRoutes_[n].count){
        if(!nativeSourceRoutes_.release(nativeSourceVoiceRoutes_[n]))std::terminate();
        voices_[n].active=false;
    }
    // Source2 cores x24 channels ascending:340 store then A30 test/A40 store.
    // Existing release preserved generation/bindings/elapsed/group/DSP/PCM.
    for(auto& cell:nativeSourceRoutes_.cells_){
        if(cell.reserved)std::terminate(); // Every live route was actually stopped.
        cell.reserved=false;cell.age=cell.kind==NativeVoiceKind::stream?0.f:3.f;
    }
    noteControlChange();
}
bool OriginalApplicationAural50KG::stopResultsNative26D070(
    const Application& root,const RaceAudioGroups50IC& groups,
    ::race_music50IE::Owner& music,::original_camera_cycle_audio50KX::Owner& feedback,
    ::source_announcer50IE::Owner& announcer,::live_audio50HF::Owner& gameplay,
    ::native_original_ai_audio::Owner& engines,::race_audio50HQ::Owner* countdown,
    std::uint64_t session,Diagnostic& d)noexcept {
    // Every refusal below keeps its original Status and pc 26D070; the step number names
    // which preflight gate refused, in source order (cr15: Results input stopped after the
    // Results replay with no way to tell which owner refused). Log only, no store.
    const auto refuse26D070=[](Diagnostic& out,Status status,unsigned step)noexcept {
        std::fprintf(stderr,"aural_stop26D070_refused step=%u status=%u\n",step,static_cast<unsigned>(status));
        return fail(out,status,0x26d070);
    };
    d={};if(thread_!=std::this_thread::get_id())return refuse26D070(d,Status::affinity,1);
    if(issuer_!=&root||root.sourceAural00Owner_.get()!=this||!current()||
       !backendCurrent()||!nativeStreamSlotsCurrent())return refuse26D070(d,Status::owner,2);
    auto& i=**interfaceA0_;auto& main=*i.main;auto* intro=i.raceIntroLease;
    // Real race Music118, Announcer120, engines and UI124 are native
    // representations of ORIGINAL audio even where their logical voices do
    // not use the source48 pool. Never advance DONE while one survives.
    // Caller passes only its exact retained Race.raceAudio_, not supplied IDs.
    // Typed Music/UI/Announcer and the separately admitted human Course or
    // zero-human ALL8 owner layout cover actual retained instances. This is
    // the shared native26D070 transport, not an input/End/Pause authorization.
    // Other providers require inactivity; no generic stopAll or relabeling.
    if(!groups.current()||groups.coldApplication_!=entities_||!groups.coldStarted_||
       groups.taIntroTransport_.get()!=intro||!groups.mixer_||
       (intro&&(intro->mixer_!=groups.mixer_||!intro->current())))
        return refuse26D070(d,Status::owner,3);
    const bool all8=engines.firstAiSlot_==0u; //Actual prepared owner, never a caller policy flag.
    if(groups.mixer_==i.mixer||!music.canStopResultsNative26D070(*groups.mixer_,groups)||
       !feedback.canStopResultsNative26D070(*groups.mixer_,groups)||
       !announcer.canStopResultsNative26D070(*groups.mixer_,groups))
        return refuse26D070(d,Status::backend,4);
    if(all8){
        if(groups.coldSession_!=session||
           !engines.canStopAll8Engines26D070(*groups.mixer_,groups,session)||
           !gameplay.all8NonHumanMixerOwner26D070(*groups.mixer_,groups,
               engines.racers_[0].engine.identity.pod60))
            return refuse26D070(d,Status::backend,5);
    }else if(!gameplay.canStopResultsGameplay26D070(*groups.mixer_,groups,session)||
       !engines.canStopResultsEngines26D070(*groups.mixer_,groups,session)||
       engines.humanResetOwner_!=&gameplay.humanController_)
        return refuse26D070(d,Status::backend,6);
    // These SAME actual registered owners carry separate native transports.
    // A failed optional typed admission grants no slots: the unchanged final
    // residual-active fence then requires every such voice to be inactive.
    // Thus unsupported/unprepared quiet providers preserve the old domain;
    // active ownership is never inferred from a slot number or filename.
    const bool auxiliary=all8?engines.canStopAll8Auxiliary26D070(*groups.mixer_,groups,session):
        engines.canStopResultsAuxiliary26D070(*groups.mixer_,groups,session);
    // The selected human Provider/lap owner is not reclassified as ALL8 row0.
    // Any active voice outside the genuine AI/shared claims still rejects below.
    const bool lap=!all8&&gameplay.canStopResultsLap26D070(*groups.mixer_,groups,session);
    const bool countdownAdmitted=countdown&&countdown->announcer_==announcer.episode_&&
        countdown->canStopResultsCountdown26D070(*groups.mixer_,groups);
    const auto auxiliaryCount=auxiliary?engines.resultsAuxiliaryStopCount26D070():0u;
    const auto lapCount=lap?gameplay.resultsLapStopCount26D070():0u;
    const auto countdownCount=countdownAdmitted?countdown->resultsCountdownStopCount26D070():0u;
    // Course world-sound owners (fixed-position World4D8 loops). Optional like
    // the auxiliary rows: unadmitted owners get no claims, so any of their
    // voices still active is refused by the residual-active fence below.
    const bool world=engines.canStopResultsWorld26D070(*groups.mixer_,groups);
    const auto worldCount=world?engines.resultsWorldStopCount26D070():0u;
    if(auxiliaryCount>audioMaximumVoices50HD||lapCount>audioMaximumVoices50HD||worldCount>audioMaximumVoices50HD||
       !groups.mixer_->canRecordControlChanges(3u+(intro?intro->count_:0u)+
           (all8?0u: ::gameplay_audio50HE::cueCount)+engines.resultsEngineStopCount26D070()+auxiliaryCount+lapCount+countdownCount+worldCount))
        return refuse26D070(d,Status::backend,7);
    const auto announcerSlot=announcer.resultsStopVoice26D070();
    if(announcerSlot>=groups.count_||announcerSlot==::race_music50IE::voice||
       announcerSlot==::original_camera_cycle_audio50KX::Owner::voice)
        return refuse26D070(d,Status::owner,8);
    std::array<bool,audioMaximumVoices50HD> typed{};
    const auto claim=[&](std::size_t slot)noexcept {
        if(slot>=groups.count_||typed[slot]||
           (intro&&slot>=intro->first_&&slot-intro->first_<intro->count_))return false;
        typed[slot]=true;return true;
    };
    if(!claim(::race_music50IE::voice)||!claim(::original_camera_cycle_audio50KX::Owner::voice)||
       !claim(announcerSlot))return refuse26D070(d,Status::owner,9);
    if(!all8)for(std::size_t n=0;n<::gameplay_audio50HE::cueCount;++n){std::size_t slot;
        if(gameplay.resultsGameplayStopVoice26D070(n,slot)&&!claim(slot))
            return refuse26D070(d,Status::owner,10);
    }
    for(std::size_t n=0;n<engines.resultsEngineStopCount26D070();++n)
        if(!claim(engines.resultsEngineStopVoice26D070(n)))return refuse26D070(d,Status::owner,11);
    for(std::size_t n=0;n<auxiliaryCount;++n)
        if(!claim(engines.resultsAuxiliaryStopVoice26D070(n)))return refuse26D070(d,Status::owner,12);
    for(std::size_t n=0;n<lapCount;++n)
        if(!claim(gameplay.resultsLapStopVoice26D070(n)))return refuse26D070(d,Status::owner,13);
    for(std::size_t n=0;n<countdownCount;++n)
        if(!claim(countdown->resultsCountdownStopVoice26D070(n)))return refuse26D070(d,Status::owner,14);
    for(std::size_t n=0;n<worldCount;++n)
        if(!claim(engines.resultsWorldStopVoice26D070(n)))return refuse26D070(d,Status::owner,15);
    for(std::size_t slot=0;slot<groups.count_;++slot){
        const bool typedIntro=intro&&slot>=intro->first_&&slot-intro->first_<intro->count_;
        const bool typedProvider=typed[slot];
        if(typedIntro&&typedProvider)return refuse26D070(d,Status::owner,16);
        if(!typedIntro&&!typedProvider&&groups.mixer_->active(slot)){
            std::fprintf(stderr,"aural_stop26D070_unclaimed_active_voice slot=%zu\n",slot);
            return refuse26D070(d,Status::backend,17);}
    }
    const bool closed=main.ordinaryOutputRetired_;
    if(!main.canRecordControlChange(1)||main.pendingNativeGroupControls()||
       !main.precedingGroupControlsCurrent()||
       (closed?!root.frontendAudioClosedReceiptCurrent():!main.current()))
        return refuse26D070(d,Status::backend,18);
    if(i.streamRetirementAuthorized||!i.mixer->canStopNativeSource26D070(*this,intro!=nullptr)||
       (intro&&(!intro->current()||intro->aural_.get()!=this||
                !intro->mixer_->canRecordControlChanges(intro->count_))))
        return refuse26D070(d,Status::backend,19);
    std::array<std::uint64_t,8> refs{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,refs))
        return refuse26D070(d,Status::backend,20);
    for(std::size_t n=0;n<refs.size();++n)
        if(refs[n]&&(!i.streamSlots[n].occupied||refs[n]!=i.streamSlots[n].generation))
            return refuse26D070(d,Status::owner,21);
    // All potentially failing observations precede mutation. Existing drain
    // uses exact Playback revisions, includes older overlaps, stops PCM and
    // retires source routes. Retain the lease, assets and stream grace slots.
    // Preflight above makes each synchronous stop infallible; do not replay a
    // partially committed native operation as an unselected source branch.
    if(intro&&!intro->drain())std::terminate();
    music.stopResultsNativePreflighted26D070();
    feedback.stopResultsNativePreflighted26D070();
    announcer.stopResultsNativePreflighted26D070();
    if(!all8)gameplay.stopResultsGameplayPreflighted26D070();
    engines.stopResultsEnginesPreflighted26D070();
    if(auxiliary)engines.stopResultsAuxiliaryPreflighted26D070();
    if(lap)gameplay.stopResultsLapPreflighted26D070();
    if(countdownAdmitted)countdown->stopResultsCountdownPreflighted26D070();
    if(world)engines.stopResultsWorldPreflighted26D070();
    i.mixer->stopNativeSourcePreflighted26D070();
    main.noteControlChange();
    if(closed){
        auto& backing=*root.garageAudioBackingOwner_;
        backing.frontendClosedStop26D070MainRevision_=main.controlRevision_;
        backing.frontendClosedStop26D070MixerRevision_=i.mixer->controlRevision();
    }
    // Real synchronous stop returned. This is NOT E00/E04 history, command
    //20/21 submission, stream-slot free,267BE0, DSP reset or output drain.
    d={Status::complete,0x26d1f0};return true;
}
bool AudioMixer50HD::canResetNativeSourceMemory268300(
    const OriginalApplicationAural50KG& owner,bool releasingStoppedGarageClips)const noexcept {
    // Caller has already fenced its immutable owner thread and actual Main.
    if(!nativeSourceWetProcessor_||
       !nativeSourceWetProcessorCurrent(owner,*nativeSourceWetProcessor_)||
       nativeRaceIntroTransport_||nativeWattoRouting_.command||nativeSceneRouting_.command||
       !voices_||count_!=OriginalUiMainAudio50KG::garageSceneRequiredVoices||
       !canRecordControlChanges(1))return false;
    // Route is private to this Mixer and the explicitly rejected intro lease.
    // No stale retained Route can become valid when uint16 generations reset.
    for(std::size_t n=0;n<count_;++n)
        if(voices_[n].active||nativeSourceVoiceRoutes_[n].count)return false;
    for(const auto& cell:nativeSourceRoutes_.cells_)if(cell.reserved)return false;
    // A stopped Garage PCM borrow is never reclaimed as if it were a source
    // stop. Only Main's exact stopped-history release in this same transaction
    // (already preflighted by the caller) may leave such clips here.
    if(!releasingStoppedGarageClips)for(std::size_t n=OriginalUiMainAudio50KG::courseRequiredVoices;
        n<OriginalUiMainAudio50KG::garageSceneVoiceEnd;++n){const auto& clip=voices_[n].clip;
        if(clip.samples||clip.frames||clip.sampleRate||clip.channels||clip.loopStart||clip.loopEnd)return false;
    }
    return true;
}
void AudioMixer50HD::resetNativeSourceMemoryPreflighted268300()noexcept {
    // Existing SAME48 source-policy cells, not native logical voice IDs and
    // not another descriptor bank. Source340 reservations precede9D0 stamps.
    for(auto& cell:nativeSourceRoutes_.cells_)cell.reserved=false;
    for(auto& cell:nativeSourceRoutes_.cells_)cell.generation=0;
    for(auto& cell:nativeSourceRoutes_.cells_){
        //268380..3C4 clears A30 low bits0..3 in order. The native model owns
        // stream/loop/pause semantics only; bit2 and upper nibble are not
        // represented and no fabricated zero-byte/unknown-bit claim is made.
        cell.kind=NativeVoiceKind::sample;cell.loop=false;cell.paused=false;
        // A34/A38=-1 withdraw source bindings, NOT the9D0 generation. Group
        // claims are absent by preflight; no guest sample/Group ID is invented.
        cell.sourceBound=false;cell.duration=0.f;
        cell.streamSlot=0;cell.streamGeneration=0;
        cell.elapsed=0.f;cell.age=180.f;
    }
    // All per-logical-voice routes were already empty; no playback/PCM cursor,
    // control revision, dry/wet gain or persistent routing byte changes here.
}
bool OriginalApplicationAural50KG::resetNativeMemory268300(
    const Application& root,Diagnostic& d)noexcept {
    d={};
    // Independent immutable fence precedes interface, weak and root reads.
    if(thread_!=std::this_thread::get_id())return fail(d,Status::affinity,0x268300);
    if(issuer_!=&root||!current()||root.sourceAural00Owner_.get()!=this||
       !backendCurrent()||!nativeStreamSlotsCurrent())return fail(d,Status::owner,0x268300);
    auto& i=**interfaceA0_;auto& main=*i.main;
    const bool closedOutput=main.ordinaryOutputRetired_;
    const auto pending=entities_->pendingWaves98A4_;
    if(!pending||!pending->current()||pending->issuer_!=issuer_||
       pending->authority_.lock().get()!=entities_.get()||pending->nativeUnresolvedSceneAbortRevision_)
        return fail(d,Status::owner,0x268300);
    auto* const cache=pending->nativeSamples_.get();
    // Original268300 ClearMemory discards every loaded sample, including a
    // finished Garage's. Native: its stopped voices release their clips and
    // PCM bindings here, but only from the exact closed-output retired state.
    const bool releaseGarage=main.stoppedGarageHistoryPresent();
    if(releaseGarage&&(!closedOutput||!cache||!main.canReleaseStoppedGarageHistory268300(*cache))){
        std::fprintf(stderr,"aural_reset_268300_rejected garage_history_unreleasable closed_output=%d cache=%d\n",
            int(closedOutput),int(cache!=nullptr));
        return fail(d,Status::unsupportedNativeReset,0x268300);}
    // This is explicitly a bounded native supported domain, NOT an original
    // condition or evidence that268300/its caller tests a positive RPC return.
    if(i.raceIntroLease||i.streamRetirementAuthorized||
       (closedOutput?(!main.retiredOrdinaryFrontendOutputCurrent()||
            !root.frontendAudioClosedReceiptCurrent()):!main.current())||
       !main.ordinaryVoiceOwnershipCurrent()||!main.garageEntryCustody_.expired()||
       (main.garageSourceLifetime_!=OriginalUiMainAudio50KG::GarageSourceLifetime::initialPrefix&&
        main.garageSourceLifetime_!=OriginalUiMainAudio50KG::GarageSourceLifetime::retired)||
       main.finalNativePcmRetired_||main.nativeGroupControlCount_||main.pendingNativeGroupControls()||
       !main.canRecordControlChange(1)||!i.mixer->canRecordControlChanges(2)||!i.mixer->canResetNativeSourceMemory268300(*this,releaseGarage)){
        std::fprintf(stderr,"aural_reset_268300_rejected garage_lifetime=%d garage_custody_expired=%d mixer_ready=%d\n",
            int(main.garageSourceLifetime_),int(main.garageEntryCustody_.expired()),int(i.mixer->canResetNativeSourceMemory268300(*this,releaseGarage)));
        return fail(d,Status::unsupportedNativeReset,0x268300);}
    //268300 does not release source Group/listener/emitter registrations or
    // pending98A4 rows. In particular1A6BF0 is AFTER the six Race groups and
    // World's seventh2350D0 group, all with actual count48=0 at this cut.
    // Do not reuse output-final-retirement's all-groups-empty condition here.
    // Keep enrolled registrations intact; real PCM leases/routes, not an
    // unrelated registration count, are the hazards checked by this reset.
    const auto authority=entities_->entityAuthority_.view();
    for(const auto* group:entities_->soundGroups434A10_)
        if(group&&(!contains(authority,group)||!group->reference00))
            return fail(d,Status::owner,0x268300);
    for(const auto& list:lists_)for(std::uint32_t n=0;n<list.count;++n){
        const auto* entry=list.at104750(n);
        if(!entry||!contains(authority,entry)||!entry->reference00)
            return fail(d,Status::owner,0x268300);
    }
    //268300 preserves exact ready typed listeners and their source counters.
    // Incomplete construction remains terminal and cannot be repaired by reset.
    if(!listenerCustodyCurrent(false))return fail(d,Status::unsupportedNativeReset,0x268300);
    std::array<std::uint64_t,8> references{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,references))
        return fail(d,Status::owner,0x268300);
    for(std::size_t n=0;n<references.size();++n)
        if(references[n]&&(!i.streamSlots[n].occupied||references[n]!=i.streamSlots[n].generation))
            return fail(d,Status::owner,0x268300);
    // With a Garage release, Main's preflight already took the exact census:
    // the canonical backing plus each stopped voice's one binding, no other.
    if(cache&&!releaseGarage&&!cache->canRetireNativeBacking()){
        std::fprintf(stderr,"aural_reset_268300_rejected outstanding_pcm_lease=1\n");
        return fail(d,Status::unsupportedNativeReset,0x268300);}
    if(!cache)for(const auto& row:pending->rows_)if(row.resolvedNativeSample)
        return fail(d,Status::unsupportedNativeReset,0x268300);

    // TRANSACTION BOUNDARY: every rejecting check is above. No allocation,
    // external callback, yield or fallible operation occurs below. This
    // preflight is a native lifetime adaptation; original268300 instead makes
    // its source stores directly and subsequently calls synchronous RPC35.
    // Native cache lifetime replaces DF0/380 sample-index metadata, not a
    // fabricated200-row integer/SPU table. Invalidate handles before channels;
    // retain backing until the later source988C free/clear position.
    // Raw Mixer clips leave before their strong Garage bindings are dropped.
    if(releaseGarage)main.releaseStoppedGarageHistory268300Preflighted();
    if(cache)++cache->generation_; //268310..324, generation cannot wrap.
    i.mixer->resetNativeSourceMemoryPreflighted268300(); //268328..3EC.
    for(std::size_t n=0;n<i.streamSlots.size();++n){
        i.handles9C0[n]=0;i.reuseAge9C8[n]=0x43340000u; //2683F0..42C.
        i.streamSlots[n].occupied=false;i.streamSlots[n].age=180.f;
    }
    //9C4/9CC and retained source metadata/PCM are NOT reset by this source
    // loop. Keep native stream generation monotonic, not reset to guest0.
    if(cache){cache->wing_.reset();for(auto& backing:cache->scene_)backing.reset();}
    //268430..478 native counterpart. Pending98A4 row/count/name/weak-handle
    // storage is deliberately untouched: only those weak handles turn stale.

    //26847C..4BC -> IRX RpcRouteNormal(function35) -> ClearMemory BF60..C04C:
    // rewind sample SPU allocation; each stream Available=1, WantsLoaded=0,
    // WantsPlayed=0, WantsPlayedOnVoiceCore=-1, WantsStopped=0, IsPlaying=0.
    // Native allocation uses the invalidated cache, and synchronous controls
    // have no pending IOP queues/core-address cursor. Empty source routes and
    // occupied=false above are its real state replacement. No StopAllVoices,
    // file close, DSP reset, device flush or positive-result receipt is implied.
    // Always publish this native control operation, even on an empty reset.
    i.mixer->noteControlChange();main.noteControlChange();
    if(closedOutput){
        // Actual Course handoff already joined/closed the output callback.
        // Preserve that original close/frame/playback history. This exact
        // nonthrowing commit alone publishes its latest reset descendant;
        // no new close, output activation or generic revision waiver occurs.
        auto& backing=*root.garageAudioBackingOwner_;
        backing.frontendClosedReset268300MainRevision_=main.controlRevision_;
        backing.frontendClosedReset268300MixerRevision_=i.mixer->controlRevision();
        backing.frontendClosedStop26D070MainRevision_=0;
        backing.frontendClosedStop26D070MixerRevision_=0;
    }
    d={Status::complete,0x2684D4};return true;
}
}
