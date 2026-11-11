// Included by the production Aural TU. No extra clock, output or DSP pass.
#include <climits>
#include <cstring>
#include "../../../src/assets/original_race_intro_pcm_identity50la.h"
namespace rr::platform {
std::shared_ptr<const assets::OriginalStreamMetadata50KZ>
OriginalApplicationAural50KG::retainedStreamMetadata()const noexcept {
    return current()&&backendCurrent()?(**interfaceA0_).main->nativeSourceMetadata_:nullptr;
}
namespace native_race_intro_detail50LA {
inline bool sameClip(const AudioClip50HD& a,const AudioClip50HD& b)noexcept {
    return a.samples==b.samples&&a.frames==b.frames&&a.sampleRate==b.sampleRate&&
        a.channels==b.channels&&a.loopStart==b.loopStart&&a.loopEnd==b.loopEnd;
}
inline float pitch(const assets::OriginalStreamMetadataRow50KZ& row)noexcept {
    return static_cast<float>((double(row.originalPitch)*48000.0/4096.0)/double(row.originalSampleRate));
}
}
const assets::OriginalStreamMetadataRow50KZ*
NativeRaceIntroTransport50LA::Assets::source(unsigned id,const AudioClip50HD& clip)const noexcept {
    namespace d=native_race_intro_detail50LA;
    if(!authoring||!bank||id<1||id>authoring->eventCount()||authoring->eventCount()>pcm.size())return nullptr;
    const auto index=id-1;const auto* event=authoring->event(id);const auto* p=pcm[index];
    const auto* identity=event?assets::originalRaceIntroPcmIdentity50LA(event->filename):nullptr;
    if(!identity||!event||!event->metadata||!event->isStream||event->isStereo||event->isLoop||event->factoryFlags!=2||
       !p||p->request.name!=event->filename||p->state!=assets::AudioAssetState50HC::ready||!p->request.stream||p->request.stereo||p->request.loop||
       p->sourceSha256!=identity->sha256||p->pcm.sourceSha256!=identity->sha256||
       p->sourceBytes!=identity->bytes||p->pcm.sampleRate!=identity->sampleRate||p->pcm.channels!=1||
       !p->pcm.frames||p->pcm.samples.size()!=p->pcm.frames||
       p->pcm.frames>(identity->bytes-48)/16*28||
       clip.samples!=p->pcm.samples.data()||clip.frames!=p->pcm.frames||clip.sampleRate!=identity->sampleRate||
       clip.channels!=1||clip.loopStart||clip.loopEnd||!d::sameClip(clip,clips[index])||
       !AudioMixer50HD::validClip(clip)||!event->metadata->originalPitch||event->metadata->originalPitch>=16384u||
       event->metadata->originalSampleRate!=identity->sampleRate||event->metadata->originalSize+48!=identity->bytes||
       !(event->metadata->admittedDurationSeconds>0.f)||
       authoring->metadataOwner()->findForStreamRequest(true,event->filename)!=event->metadata)return nullptr;
    return event->metadata;
}
bool OriginalApplicationAural50KG::raceIntroTransportCurrent(
    const NativeRaceIntroTransport50LA& t,bool acquiring)const noexcept {
    if(!current()||!backendCurrent()||!nativeStreamSlotsCurrent()||!attached_||
       entity_.parent1C!=&entities_->root10_||!t.assets_||!t.assets_->authoring||!t.assets_->bank)return false;
    const auto& i=**interfaceA0_;const auto& b=issuer_->garageAudioBackingOwner_;
    if((acquiring?i.raceIntroLease!=nullptr:i.raceIntroLease!=&t)||!b||
       !b->frontendOutputAttempted_||!b->frontendOutputClosed_||b->frontendOutputCloseFailed_||
       b->frontendOutputAccountingFailed_||!i.main->ordinaryOutputRetired_||
       !issuer_->sourceRace28COwner_||t.mixer_==i.mixer||
       !same(i.main->nativeSourceMetadata_,t.assets_->authoring->metadataOwner()))return false;
    const auto canonical=issuer_->standardMaterialDomainOwner_?
        issuer_->standardMaterialDomainOwner_->archiveProvider3E1A70():nullptr;
    if(!canonical||canonical.get()!=t.assets_->authoring->providerIdentity())return false;
    // Output close already stopped all frontend producers. No source route
    // or stream history is discarded: retired source cells remain in pool.
    for(std::size_t n=0;n<i.nativeCount;++n)
        if(i.mixer->active(n)||i.mixer->nativeSourceVoiceRoutes_[n].count)return false;
    return true;
}
bool OriginalApplicationAural50KG::raceIntroPublicationServiced(
    const original_ui_source_timing50LH::PublicationView& timing,std::uint64_t serial)const noexcept {
    if(!current()||!backendCurrent()||!(**interfaceA0_).raceIntroLease)return false;
    const auto& t=*(**interfaceA0_).raceIntroLease;
    return t.current()&&t.lastPublication_==serial&&timing.sameOwner(t.timing_);
}
bool OriginalApplicationAural50KG::raceIntroFactorCurrent(
    const NativeRaceIntroTransport50LA& t,
    const original_ui_source_timing50LH::PublicationView& timing,float& factor,std::uint64_t& serial)const noexcept {
    original_ui_source_timing50LH::Sample sample;
    if(!raceIntroTransportCurrent(t)||!timing.readPrepared(sample,serial)||
       !issuer_->nativeTaRootEntered_||issuer_->nativeTaRootAborted_||
       issuer_->nativeTaRootRace_!=issuer_->sourceRace28COwner_.get()||
       !issuer_->nativeTaRootController_||issuer_->nativeTaRootPublication_!=serial||
       !timing.sameOwner(issuer_->nativeTaRootTiming_)||
       issuer_->sourceApplicationMode22C_!=2||!issuer_->sourceApplicationElapsed23C_||
       !std::isfinite(sample.updateSeconds)||sample.updateSeconds<0.f||
       std::bit_cast<std::uint32_t>(*issuer_->sourceApplicationElapsed23C_)!=
           std::bit_cast<std::uint32_t>(sample.updateSeconds)||
       (t.lastPublication_&&(!timing.sameOwner(t.timing_)||t.lastPublication_==UINT64_MAX||
                            serial!=t.lastPublication_+1)))return false;
    factor=sample.updateSeconds;return true;
}
NativeRaceIntroTransport50LA::~NativeRaceIntroTransport50LA()noexcept {
    if(leased_&&!released_)std::terminate();
}
std::unique_ptr<NativeRaceIntroTransport50LA> NativeRaceIntroTransport50LA::create(
    std::shared_ptr<const OriginalApplicationAural50KG> aural,AudioMixer50HD& mixer,
    std::size_t first,std::size_t count,
    std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate> authoring,
    std::shared_ptr<const assets::AudioBank50HC> bank)noexcept {
    if(!aural||!authoring||!authoring->eventCount()||authoring->eventCount()>3||(authoring->trackId()!="TA"&&authoring->trackId()!="RA"&&authoring->trackId()!="GC"&&authoring->trackId()!="SA"&&authoring->trackId()!="MA"&&authoring->trackId()!="GA"&&authoring->trackId()!="MB"&&authoring->trackId()!="SC"&&authoring->trackId()!="GB")||!bank||
       bank->clips.size()!=authoring->eventCount()||bank->ready!=authoring->eventCount()||bank->missing||bank->invalid||bank->unsupported||first!=originalTaIntroFirstVoice50HD||count!=originalTaIntroVoices50HD||
       first>=mixer.count()||count>mixer.count()-first||
       !mixer.requiresVoiceGroupGains50IC()||mixer.nativeRaceIntroTransport_||mixer.nativeSourceWetProcessor_||
       mixer.auralMasterOwner_||!mixer.originalSoundMode50KG()||!mixer.canRecordControlChanges())return {};
    try {
        auto t=std::unique_ptr<NativeRaceIntroTransport50LA>(new NativeRaceIntroTransport50LA);
        auto a=std::make_shared<Assets>();a->authoring=std::move(authoring);a->bank=std::move(bank);
        for(unsigned id=1;id<=a->authoring->eventCount();++id){const auto* event=a->authoring->event(id);if(!event)return {};
            const auto* p=a->bank->find(std::string(event->filename));if(!p)return {};
            a->pcm[id-1]=p;a->clips[id-1]={p->pcm.samples.data(),p->pcm.frames,p->pcm.sampleRate,p->pcm.channels,0,0};
            if(!a->source(id,a->clips[id-1]))return {};
        }
        t->aural_=std::move(aural);t->assets_=std::move(a);t->mixer_=&mixer;t->first_=first;t->count_=count;
        t->epoch_=mixer.groupEpoch50IC();t->rate_=mixer.sampleRate();t->instances_.resize(count);
        if(!t->aural_->raceIntroTransportCurrent(*t,true))return {};
        for(std::size_t n=0;n<count;++n){const auto slot=first+n;
            if(mixer.active(slot)||mixer.playbackRevision(slot)||!mixer.hasVoiceGroupGain50IC(slot)||
               mixer.nativeSourceVoiceRoutes_[slot].count||mixer.groupPaused50IC_[slot])return {};
        }
        auto& i=**t->aural_->interfaceA0_;
        for(const auto& cell:i.mixer->nativeSourceRoutes_.cells_)
            if(cell.reserved)return {};
        i.raceIntroLease=t.get();mixer.nativeRaceIntroTransport_=t.get();
        mixer.nativeRaceIntroRenderCurrent_=&NativeRaceIntroTransport50LA::renderCurrent;
        mixer.nativeRaceIntroFirst_=first;mixer.nativeRaceIntroCount_=count;t->leased_=true;
        return t;
    }catch(...){return {};}
}
bool NativeRaceIntroTransport50LA::current()const noexcept {return currentWithMixerPause(paused_);}
bool NativeRaceIntroTransport50LA::currentWithMixerPause(bool mixerPause)const noexcept {
    if(!leased_||released_||!aural_||!mixer_||!assets_||!count_||instances_.size()!=count_||
       mixer_->nativeRaceIntroTransport_!=this||mixer_->nativeRaceIntroRenderCurrent_!=&renderCurrent||
       mixer_->nativeRaceIntroFirst_!=first_||mixer_->nativeRaceIntroCount_!=count_||
       mixer_->groupEpoch50IC()!=epoch_||mixer_->sampleRate()!=rate_||first_>=mixer_->count()||
       count_>mixer_->count()-first_||!mixer_->requiresVoiceGroupGains50IC()||
       !aural_->raceIntroTransportCurrent(*this))return false;
    const auto& pool=(**aural_->interfaceA0_).mixer->nativeSourceRoutes_;
    // Source26C180 negative269650 errors decode an otherwise out-of-domain
    // generation read atA3E. Its native equivalent is safe only while EVERY
    // retained A3C value is finite/nonnegative, including stopped histories.
    if(!pool.canServiceChannels267CB0(0.f))return false;
    for(const auto& cell:pool.cells_)
        if(!std::isfinite(cell.elapsed)||cell.elapsed<0.f||cell.generation>500)return false;
    std::array<bool,48> claimed{};
    for(unsigned id=1;id<=assets_->authoring->eventCount();++id)if(!assets_->source(id,assets_->clips[id-1]))return false;
    for(std::size_t n=0;n<count_;++n){const auto slot=first_+n;const auto& in=instances_[n];
        if(!mixer_->hasVoiceGroupGain50IC(slot)||mixer_->groupPaused50IC_[slot]!=mixerPause||
           mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
        if(!in.playback.serial){if(mixer_->active(slot)||mixer_->playbackRevision(slot)||in.route.count)return false;continue;}
        const auto& v=mixer_->voices_[slot];
        if(in.playback.voice!=slot||in.playback.serial>nextSerial_||!in.eventId||in.eventId>assets_->authoring->eventCount()||
           mixer_->playbackRevision(slot)!=in.playback.revision||
           !native_race_intro_detail50LA::sameClip(v.clip,assets_->clips[in.eventId-1])||
           v.gain!=1.f||v.pitch!=native_race_intro_detail50LA::pitch(*assets_->authoring->event(in.eventId)->metadata)||v.pan!=0.f||v.loop||
           !std::isfinite(v.sourceFrame)||v.sourceFrame<0.0||
           (v.active&&v.sourceFrame>=static_cast<double>(v.clip.frames))||
           (in.route.count&&!pool.current(in.route)))return false;
        if(in.route.count)for(unsigned j=0;j<in.route.count;++j){
            const auto index=in.route.cells[j];
            if(claimed[index]||pool.cells_[index].paused!=paused_)return false;
            claimed[index]=true;
        }
    }
    for(std::size_t n=0;n<claimed.size();++n)if(pool.cells_[n].reserved!=claimed[n])return false;
    return true;
}
bool NativeRaceIntroTransport50LA::renderCurrent(const void* owner,std::size_t slot)noexcept {
    const auto* t=static_cast<const NativeRaceIntroTransport50LA*>(owner);
    if(!t||!t->current()||slot<t->first_||slot-t->first_>=t->count_)return false;
    const auto& in=t->instances_[slot-t->first_];
    return in.playback.serial&&in.route.count&&
        (**t->aural_->interfaceA0_).mixer->nativeSourceRoutes_.current(in.route);
}
bool NativeRaceIntroTransport50LA::canPlay(unsigned event)const noexcept {
    if(!current()||event<1||event>assets_->authoring->eventCount()||nextSerial_>=INT_MAX||!mixer_->canRecordControlChanges(1))return false;
    float left=0,right=0;if(!mixer_->originalRouting50KG(0,left,right,100))return false;
    for(std::size_t n=0;n<count_;++n)
        if(!instances_[n].route.count&&!mixer_->active(first_+n))return true;
    return false;
}
NativeRaceIntroTransport50LA::Dispatch NativeRaceIntroTransport50LA::play(unsigned event,Playback& out)noexcept {
    if(!canPlay(event))return Dispatch::unavailable;
    auto& i=**aural_->interfaceA0_;auto slots=i.streamSlots;
    std::size_t selected=slots.size();float best=0.f;
    for(std::size_t n=0;n<slots.size();++n)if(!slots[n].occupied)
        if(best==0.f||slots[n].age>best){selected=n;best=slots[n].age;}
    if(selected==slots.size()){lastSourceResult_=-107;return Dispatch::sourceFalse;} //26973C, no allocation.
    if(i.streamGeneration==UINT64_MAX)return Dispatch::unavailable;
    std::size_t native=0;while(native<count_&&(instances_[native].route.count||mixer_->active(first_+native)))++native;
    if(native==count_)return Dispatch::unavailable;
    const auto& clip=assets_->clips[event-1];const auto* metadata=assets_->source(event,clip);
    if(!metadata)return Dispatch::unavailable;
    auto& source=slots[selected];source.kind=OriginalApplicationAural50KG::NativeInterface::StreamKind::raceIntro;
    source.sourceOrdinal=event;source.generation=i.streamGeneration+1;source.metadata=metadata;
    source.clip=clip;source.raceAssets=assets_;source.occupied=true;source.age=0.f;
    auto pool=i.mixer->nativeSourceRoutes_;NativeSourceVoiceRoutes50KG::Route route;
    if(!pool.reserve(NativeSourceVoiceRoutes50KG::Kind::stream,1,route)){
        //269C9C channel-allocation failure clears occupancy, but retains the
        //already-written stream row and zero reuse age. No fabricated handle.
        source.occupied=false;aural_->commitNativeStreamSlots(slots);lastSourceResult_=-100;return Dispatch::sourceFalse;
    }
    std::array<std::uint8_t,2> indices{{static_cast<std::uint8_t>(selected),0}};
    std::array<std::uint64_t,2> generations{{source.generation,0}};
    if(!pool.bindStreamPublication266DA0(route,metadata->admittedDurationSeconds,false,indices,generations,1)||
       !pool.setSourcePause26A9D0(route,paused_))return Dispatch::unavailable;
    float left=0,right=0;if(!mixer_->originalRouting50KG(0,left,right,100))return Dispatch::unavailable;
    const auto voice=first_+native;
    // All potentially failing/native allocation checks precede publication.
    i.mixer->nativeSourceRoutes_=pool;aural_->commitNativeStreamSlots(slots);
    mixer_->noteControlChange();mixer_->playbackRevisions_[voice]=mixer_->controlRevision_;
    mixer_->voices_[voice]={clip,0.0,1.f,native_race_intro_detail50LA::pitch(*metadata),0.f,true,false};
    mixer_->originalLeft50KG_[voice]=left;mixer_->originalRight50KG_[voice]=right;
    out={voice,mixer_->playbackRevisions_[voice],++nextSerial_};
    instances_[native]={out,route,event};lastSourceResult_=1;return Dispatch::sourceTrue;
}
bool NativeRaceIntroTransport50LA::active(const Playback& p,bool& result)const noexcept {
    result=false;if(!current()||!p.serial||p.serial>nextSerial_||p.voice<first_||p.voice-first_>=count_||!p.revision)return false;
    const auto& in=instances_[p.voice-first_];
    if(in.playback.serial!=p.serial||in.playback.revision!=p.revision)return true;
    result=in.route.count&&(**aural_->interfaceA0_).mixer->nativeSourceRoutes_.current(in.route);return true;
}
bool NativeRaceIntroTransport50LA::stop(const Playback& p,bool& wasActive)noexcept {
    if(!active(p,wasActive))return false;
    if(!wasActive)return true;
    if(!mixer_->canRecordControlChanges(1))return false;
    auto& in=instances_[p.voice-first_];auto& pool=(**aural_->interfaceA0_).mixer->nativeSourceRoutes_;
    if(!pool.release(in.route))return false;
    mixer_->noteControlChange();mixer_->voices_[p.voice].active=false;return true;
}
bool NativeRaceIntroTransport50LA::canSetPaused(bool)const noexcept {
    return current();
}
bool NativeRaceIntroTransport50LA::setPaused(bool paused)noexcept {
    if(!currentWithMixerPause(paused))return false; // Caller already completed actual Group suffix.
    auto& pool=(**aural_->interfaceA0_).mixer->nativeSourceRoutes_;
    for(auto& in:instances_)if(in.route.count&&!pool.setSourcePause26A9D0(in.route,paused))std::terminate();
    // Source Group caller owns its ordered2356EC mixer group-pause writes.
    paused_=paused;return true;
}
bool NativeRaceIntroTransport50LA::service(
    const original_ui_source_timing50LH::PublicationView& timing,float& factor)noexcept {
    factor=0.f;if(!current())return false;
    std::uint64_t serial=0;float dt=0.f;
    if(!aural_->raceIntroFactorCurrent(*this,timing,dt,serial))return false;
    auto& i=**aural_->interfaceA0_;auto next=i.mixer->nativeSourceRoutes_;
    if(!next.serviceChannels267CB0(dt))return false;
    for(std::size_t n=0;n<count_;++n)if(instances_[n].route.count&&!next.current(instances_[n].route)&&
        mixer_->active(first_+n))return false; // Never silently drop still-rendering PCM.
    std::array<OriginalApplicationAural50KG::NativeInterface::StreamSlot,8> slots;
    if(!aural_->previewNativeStreamSuffix(dt,slots))return false;
    i.mixer->nativeSourceRoutes_=next;aural_->commitNativeStreamSlots(slots);
    for(auto& in:instances_)if(in.route.count&&!next.current(in.route))in.route={};
    timing_=timing;lastPublication_=serial;factor=dt;return true;
}
bool NativeRaceIntroTransport50LA::drain()noexcept {
    if(!current()||!mixer_->canRecordControlChanges(count_))return false;
    // Native memory-safety drain includes older overlaps, unlike2924A0.
    for(auto& in:instances_)if(in.playback.serial){bool active=false;if(!stop(in.playback,active))return false;}
    return true;
}
bool NativeRaceIntroTransport50LA::release()noexcept {
    if(released_)return true;
    if(!current())return false;
    for(std::size_t n=0;n<count_;++n)if(instances_[n].route.count||mixer_->active(first_+n))return false;
    auto& i=**aural_->interfaceA0_;i.raceIntroLease=nullptr;
    mixer_->nativeRaceIntroTransport_=nullptr;mixer_->nativeRaceIntroRenderCurrent_=nullptr;
    mixer_->nativeRaceIntroFirst_=0;mixer_->nativeRaceIntroCount_=0;
    // Stop/grace histories remain in the same Aural pool/stream rows. Clearing
    // only native raw clip borrows permits safe bank retirement after release.
    for(std::size_t n=0;n<count_;++n)mixer_->voices_[first_+n].clip={};
    released_=true;leased_=false;return true;
}
}
