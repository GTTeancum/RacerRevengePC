#include "original_application_aural50kg.h"
#include "original_ui_garage_audio_backing50kg.h"
#include "original_ui_light_manager_owner50kg.h"
#include "native_source_reverb50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <bit>
#include <cmath>
#include <limits>
namespace rr::platform {
using K=game::OriginalWreckComponentKind50LA;
namespace {
template<class T> bool same(const std::shared_ptr<T>& a,const std::shared_ptr<T>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
}
OriginalApplicationAural50KG::NativeInterface::~NativeInterface()noexcept {
    if(streamGeneration&&!streamRetirementAuthorized)std::terminate();
    // Only actual interface destruction discharges remaining stopped stream
    // slots. No ordinary stop, clear103DD0, Group retirement or reentry does.
    streamSlots={};
}
bool OriginalApplicationAural50KG::List::initialize104410()noexcept {
    if(!constructed||count||capacity||storage||allocatedBit0)return false;
    //104510..530 publish growth/capacity/bit even when allocation returned null.
    try{storage.reset(new C*[8]);}catch(...){storage.reset();}
    growth=16;capacity=8;allocatedBit0=true;return bool(storage);
}
bool OriginalApplicationAural50KG::List::valid()const noexcept {
    return constructed&&count<=capacity&&(!count||storage)&&
        (allocatedBit0?(growth==16&&capacity>=8):(!count&&!growth&&!capacity&&!storage));
}
OriginalApplicationAural50KG::C* OriginalApplicationAural50KG::List::at104750(std::uint32_t i)const noexcept {
    return count&&storage&&i<count?storage[i]:nullptr;
}
bool OriginalApplicationAural50KG::List::remove1046D0(const C* node)noexcept {
    for(std::uint32_t i=0;i<count;++i)if(storage[i]==node){
        storage[i]=storage[count-1];--count;return true; // No stable erase/null tail store.
    }
    return false;
}
OriginalApplicationAural50KG::OriginalApplicationAural50KG(const Application& root,std::shared_ptr<E> e)noexcept
    :issuer_(&root),entities_(std::move(e)),lifetime_(root.animationClock()),generation_(key(entities_.get())){}
OriginalApplicationAural50KG::~OriginalApplicationAural50KG()noexcept {
    if(constructed_&&!retired_)std::terminate();
}
bool OriginalApplicationAural50KG::sourceIdentity()const noexcept {
    if(thread_!=std::this_thread::get_id()||!entities_||entities_->nativeOwnerThread_!=thread_)return false;
    std::uint32_t pause=0;
    if(!issuer_||!lifetime_.readPause11C(pause))return false; // Before raw issuer dereference.
    return constructed_&&!retired_&&same(issuer_->entities(),entities_)&&
        lifetime_.sameOwner(issuer_->animationClock())&&entities_->constructed_&&!entities_->released_&&
        entities_->auralNativeOwner_==this&&entities_->aural00Live_&&entities_->aural00_==&entity_&&
        entities_->auralPrefixIssued_&&entity_.kind==K::auralScape102BE0&&
        entity_.sessionGeneration==generation_&&entity_.objectKey==key(&entity_)&&entity_.reference00==1&&
        contains(entities_->entityAuthority_.view(),&entity_);
}
bool OriginalApplicationAural50KG::current()const noexcept {
    if(!sourceIdentity()||entities_->attachmentLock423F58_)return false;
    for(const auto& list:lists_)if(!list.valid())return false;
    return game::original_entity_chain_detail50LA::forest(entities_->entityAuthority_.view());
}
std::shared_ptr<const OriginalApplicationAural50KG>
OriginalApplicationAural50KG::retainColdStreamManager232DB0(
    const std::shared_ptr<E>& app,
    const std::shared_ptr<const assets::OriginalStreamMetadata50KZ>& metadata,
    const std::shared_ptr<const assets::OriginalStandardArchiveProvider50LA>& provider)const noexcept {
    if(!current()||!nativeInitialized_||!backendCurrent()||!same(entities_,app)||
       !issuer_->sourceAural00Owner_||issuer_->sourceAural00Owner_.get()!=this||
       !issuer_->standardMaterialDomainOwner_||!issuer_->standardMaterialDomainOwner_->prepared())return {};
    const auto& main=(**interfaceA0_).main;
    const std::shared_ptr<const assets::OriginalStandardArchiveProvider50LA> canonicalProvider=
        issuer_->standardMaterialDomainOwner_->archiveProvider3E1A70();
    if(!main||!same(main->nativeSourceMetadata_,metadata)||!same(canonicalProvider,provider))return {};
    // current checked issuer lifetime before raw issuer access. Return the
    // actual Application's control block, never shared_ptr(this).
    return issuer_->sourceAural00Owner_;
}
bool OriginalApplicationAural50KG::exactAuralOnlyStartupPrefix()const noexcept {
    if(!current()||attached_||entity_.parent1C||entity_.previous10||entity_.next14||entity_.firstChild18||
       entity_.mask20||entity_.descendants24||(entity_.flags29&15u)||entities_->root10_.firstChild18||
       entities_->objectCount423F60_!=3)return false;
    const auto& a=entities_->entityAuthority_;
    return a.count_==3&&a.enrollmentSerial_==3&&a.nonRootEnrollmentObserved_&&a.auralStartupPrefix_==&entity_&&
        a.entries_[0].node==&entities_->outer26C_&&a.entries_[1].node==&entities_->root10_&&a.entries_[2].node==&entity_;
}
bool OriginalApplicationAural50KG::firstStartupInputsCurrent(const Application& root)noexcept {
    const auto& e=root.entities();std::uint32_t pause=0;
    if(!e||e->nativeOwnerThread_!=std::this_thread::get_id()||!root.animationClock().readPause11C(pause))return false;
    const auto& a=e->entityAuthority_;
    return e->constructed_&&!e->released_&&!e->attachmentLock423F58_&&!e->auralPrefixIssued_&&
        !e->aural00_&&!e->auralNativeOwner_&&e->objectCount423F60_==2&&a.count_==2&&
        a.enrollmentSerial_==2&&!a.nonRootEnrollmentObserved_&&!e->root10_.firstChild18&&
        game::original_entity_chain_detail50LA::forest(a.view());
}
bool OriginalApplicationAural50KG::construct13FE24(const Application& root,
    std::shared_ptr<OriginalApplicationAural50KG>& out,Diagnostic& d)noexcept {
    d={};if(out)return fail(d,Status::phase,0x13FE24);
    const auto e=root.entities();std::uint32_t pause=0;
    if(!e||!root.animationClock().readPause11C(pause))return fail(d,Status::owner,0x13FE24);
    if(e->nativeOwnerThread_!=std::this_thread::get_id())return fail(d,Status::affinity,0x13FE24);
    const auto& a=e->entityAuthority_;
    if(!e->constructed_||e->released_||e->attachmentLock423F58_||e->auralPrefixIssued_||e->aural00_||e->auralNativeOwner_||
       e->objectCount423F60_!=2||a.count_!=2||a.enrollmentSerial_!=2||a.nonRootEnrollmentObserved_||
       e->root10_.firstChild18||!game::original_entity_chain_detail50LA::forest(a.view()))return fail(d,Status::phase,0x13FE24);
    std::shared_ptr<OriginalApplicationAural50KG> next;
    try{next=std::shared_ptr<OriginalApplicationAural50KG>(new OriginalApplicationAural50KG(root,e));}
    catch(...){return fail(d,Status::allocation,0x13FE10);}
    if(!e->entityAuthority_.reserveAdditional(1)||!e->entityAuthority_.canEnroll(next->entity_,key(&next->entity_)))return fail(d,Status::allocation,0x141300);
    const auto init=game::initializeOriginalWreckComponent50LA(next->entity_,K::auralScape102BE0,
        next->generation_,key(&next->entity_),false,e->objectCount423F60_);
    if(init.status!=game::OriginalWreckComponentStatus50LA::complete)std::terminate();
    for(auto& list:next->lists_)list.construct1043E0();
    next->constructed_=true;
    if(!e->entityAuthority_.enrollAuralStartupPrefix(next->entity_,e->outer26C_,e->root10_))std::terminate();
    e->aural00_=&next->entity_;e->auralNativeOwner_=next.get();e->aural00Live_=true;e->auralPrefixIssued_=true;
    out=std::move(next);d={Status::complete,0x13FE2C};return true;
}
bool OriginalApplicationAural50KG::nativeOutputSourceQuiescent()const noexcept {
    // Also used before device-close stops owned ordinary voices. Validate
    // their exact stream custody here, but leave final stopped-slot discharge
    // exclusively to retireNativeOrEmpty102CD0 after real output retirement.
    if(!current()||!backendCurrent()||!nativeStreamSlotsCurrent())return false;
    const auto& interface=**interfaceA0_;std::array<std::uint64_t,8> references{};
    if(!interface.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,references))return false;
    for(std::size_t n=0;n<references.size();++n)
        if(references[n]&&(!interface.streamSlots[n].occupied||references[n]!=interface.streamSlots[n].generation))return false;
    for(const auto& list:lists_)if(list.count)return false;
    for(const auto* listener:listeners4C_)if(listener)return false;
    for(const auto* group:entities_->soundGroups434A10_)if(group)return false;
    return true;
}
std::shared_ptr<OriginalAudioPendingWaves50KG> OriginalApplicationAural50KG::nativeOutputPendingNamespace()const noexcept {
    return current()?entities_->pendingWaves98A4_:nullptr;
}
bool OriginalApplicationAural50KG::nativeOutputSourceRetired(const Application& root,const C* historical)noexcept {
    const auto entities=root.entities();
    if(!historical||!entities||root.sourceAural00Owner_||!entities->auralPrefixIssued_||
       entities->auralNativeOwner_||entities->aural00Live_||entities->aural00_!=historical||entities->pendingWaves98A4_)
        return false;
    for(const auto* group:entities->soundGroups434A10_)if(group)return false;
    // Historical pointer comparison only. Actual102CD0 already released the
    // native interface and counted Entity; no freed source object is read.
    return true;
}
bool OriginalApplicationAural50KG::backendCurrent(bool permitFailedTransport)const noexcept {
    if(!sourceIdentity()||!interfaceA0_||!*interfaceA0_)return false;
    const auto& i=**interfaceA0_;const auto& b=issuer_->garageAudioBackingOwner_;
    if(!b||!b->inputsCurrent(*issuer_)||(!permitFailedTransport&&b->failed_)||!i.main||i.main.get()!=&b->transport_||
       i.main.owner_before(b)||b.owner_before(i.main)||i.mixer!=&b->mixer_||
       i.mixerEpoch!=b->mixer_.groupEpoch50IC()||i.nativeRate!=b->mixer_.sampleRate()||
       i.nativeCount!=b->mixer_.count()||i.main->mixer_!=i.mixer||!i.main->prepared_||
       i.main->mixerEpoch_!=i.mixerEpoch||i.main->preparedSoundMode_!=b->soundMode_.get()||
       (!permitFailedTransport&&i.main->finalNativePcmRetired_)||
       (i.main->finalNativePcmRetired_&&!i.main->finalNativePcmRetirementCurrent()))return false;
    return i.wetProcessor&&i.wetProcessorBound&&i.wetProcessor->configured()&&
        i.main->nativeWetOwner_==this&&i.main->nativeWetProcessor_==i.wetProcessor.get()&&
        i.main->nativeWetEpoch_==i.mixerEpoch&&i.main->nativeWetInterfaceCurrent()&&
        i.main->nativeWetPlaySong_==&OriginalApplicationAural50KG::playNativeMainSong234170&&
        i.main->nativeWetPlayEffect_==&OriginalApplicationAural50KG::playNativeMainEffect233410&&
        i.main->nativeWetPlayRider_==&OriginalApplicationAural50KG::playNativeMainRider233410&&
        i.main->nativeWetPlayWatto_==&OriginalApplicationAural50KG::playNativeWattoStream233410&&
        i.main->nativeWetCanPublish_==&OriginalApplicationAural50KG::canPublishNativeGarageVoices&&
        i.mixer->nativeSourceWetProcessorCurrent(*this,*i.wetProcessor)&&
        i.mastersApplied&&i.mixer->auralMasterOwner_==this&&i.masterPercent[0]==100&&i.masterPercent[1]==100&&
        i.mixer->auralMasterGain_[0]==1.f&&i.mixer->auralMasterGain_[1]==1.f&&
        entities_->soundMasterSlotsInitialized_&&entities_->soundGroupReserved434A10_[0]&&entities_->soundGroupReserved434A10_[1];
}
bool OriginalApplicationAural50KG::initializeNative102DE0(Diagnostic& d)noexcept {
    d={};if(!exactAuralOnlyStartupPrefix()||initializationAttempted_||interfaceA0_)
        return fail(d,Status::phase,0x102DE0);
    // Concrete native endpoint replaces only267910's platform bootstrap. The
    // source register/list/master effects below still execute on real owners.
    const auto b=issuer_->garageAudioBackingOwner_;
    if(!b||!b->inputsCurrent(*issuer_)||b->failed_||b->stage_!=OriginalUiGarageAudioBacking50KG::Stage::transportPrepared||
       b->mixer_.sampleRate()!=NativeSourceReverb50KG::nativeRate||
       b->mixer_.mixedFrames()||b->transport_.ownsMusic_||b->transport_.paused_||
       b->transport_.nativeGroupControlCount_||b->mixer_.auralMasterOwner_||b->mixer_.masterGain()!=1.f||
       b->transport_.nativeWetOwner_||b->transport_.nativeWetProcessor_||b->transport_.nativeWetEpoch_||
       b->transport_.nativeWetSetRawMasters_||b->transport_.nativeWetPlaySong_||b->transport_.nativeWetPlayEffect_||b->transport_.nativeWetPlayRider_||b->transport_.nativeWetPlayWatto_||b->transport_.nativeWetCanPublish_||b->transport_.nativeWetInterfaceRetired_||
       !b->soundMode_||!b->soundMode_->ready()||b->soundMode_->fields().outputMode9900!=1||
       b->soundMode_->fields().reverseAngle9918||b->soundMode_->revision()==std::numeric_limits<std::uint64_t>::max()||
       !b->mixer_.canRecordControlChanges(2)||!b->transport_.prepared_||!b->transport_.nativeSourceMetadata_||
       b->transport_.mixer_!=&b->mixer_||
       b->transport_.mixerEpoch_!=b->mixer_.groupEpoch50IC()||
       b->transport_.preparedSoundMode_!=b->soundMode_.get())
        return fail(d,Status::backend,0x232AA0);
    if(entities_->soundMasterSlotsInitialized_)return fail(d,Status::phase,0x232B18);
    for(std::size_t n=0;n<25;++n)if(entities_->soundGroups434A10_[n]||entities_->soundGroupReserved434A10_[n])
        return fail(d,Status::phase,0x232B18);
    for(std::size_t n=0;n<b->mixer_.count();++n)if(b->mixer_.active(n)||b->mixer_.playbackRevision(n))
        return fail(d,Status::backend,0x267910);
    initializationAttempted_=true;interfaceA0_.emplace(); //102E30 is now known, initially allocation failure/null.
    try{*interfaceA0_=std::make_unique<NativeInterface>();}
    catch(...){return fail(d,Status::allocation,0x102E18);}
    auto& i=**interfaceA0_;
    i.main=std::shared_ptr<OriginalUiMainAudio50KG>(b,&b->transport_);i.mixer=&b->mixer_;
    i.mixerEpoch=b->mixer_.groupEpoch50IC();i.nativeRate=b->mixer_.sampleRate();i.nativeCount=static_cast<std::uint32_t>(b->mixer_.count());
    i.channelCount37C=8; // Incoming13FE40 value, NOT the native mixer voice count.
    try{
        i.handles9C0.reset(new std::uint32_t[8]);i.reuseAge9C8.reset(new std::uint32_t[8]);i.rows9CC.reset(new std::uint64_t[8]);
    }catch(...){return fail(d,Status::allocation,0x26792C);}
    for(unsigned n=0;n<8;++n){i.handles9C0[n]=0;i.reuseAge9C8[n]=0x43340000u;}
    // Actual267910 platform initialization precedes232BD0/232C20 masters.
    // The supplied PS2SOUND startup installs mode107/delay48/feedback24,
    // raw7fff depths and real cleared work RAM on both cores. Retain that
    // numerical producer here, not per SoundGroup or per Garage visit.
    try{i.wetProcessor=std::make_unique<NativeSourceReverb50KG>();}
    catch(...){return fail(d,Status::allocation,0x267910);}
    NativeSourceReverb50KG::Startup wetStartup{};
    wetStartup.outputRate=i.nativeRate;wetStartup.firstFrame=i.mixer->mixedFrames();
    if(!i.wetProcessor->initialize(wetStartup))return fail(d,Status::backend,0x267910);
    // Publish only the identity needed for the private same-mixer/epoch bind.
    // Dry master writes have not happened. A rejected cold bind withdraws this
    // provisional pointer and leaves the real partial interface for cleanup.
    i.mixer->auralMasterOwner_=this;
    if(!i.mixer->bindNativeSourceWetProcessor(*this,*i.wetProcessor,
        &NativeSourceReverb50KG::preflight,&NativeSourceReverb50KG::process)){
        i.mixer->auralMasterOwner_=nullptr;return fail(d,Status::backend,0x267910);
    }
    i.wetProcessorBound=true;
    // Aural strongly retains both Main and the DSP; publish only an exact
    // non-owning control borrow into that same Main after the successful bind.
    // The typed member is issued here, so ordinary Main targets do not acquire
    // an unresolved direct dependency on the numerical implementation TU.
    i.main->nativeWetOwner_=this;i.main->nativeWetProcessor_=i.wetProcessor.get();
    i.main->nativeWetEpoch_=i.mixerEpoch;i.main->nativeWetSetRawMasters_=&NativeSourceReverb50KG::setRawMasters;
    i.main->nativeWetPlaySong_=&OriginalApplicationAural50KG::playNativeMainSong234170;
    i.main->nativeWetPlayEffect_=&OriginalApplicationAural50KG::playNativeMainEffect233410;
    i.main->nativeWetPlayRider_=&OriginalApplicationAural50KG::playNativeMainRider233410;
    i.main->nativeWetPlayWatto_=&OriginalApplicationAural50KG::playNativeWattoStream233410;
    i.main->nativeWetCanPublish_=&OriginalApplicationAural50KG::canPublishNativeGarageVoices;
    // Native synchronous dry master operations. No queue item or wet-output
    // receipt is fabricated. Their two source stores and PCM consumers agree.
    for(std::size_t n=0;n<i.nativeCount;++n)i.mixer->auralMasterBus_[n]=n==OriginalUiMainAudio50KG::musicVoice?1u:0u;
    for(unsigned bus=0;bus<2;++bus){i.masterPercent[bus]=100;i.mixer->auralMasterGain_[bus]=1.f;i.mixer->noteControlChange();}
    i.mastersApplied=true;
    //232B60(1) writes the actual retained mode consumed by native routing. It
    // does not pretend to apply a profile transaction or replace the owner.
    b->soundMode_->fields_.outputMode9900=1;b->soundMode_->fields_.reverseAngle9918=0;
    ++b->soundMode_->revision_;i.initialized28Bit0=true;
    entities_->soundGroupReserved434A10_[0]=entities_->soundGroupReserved434A10_[1]=true;
    entities_->soundMasterSlotsInitialized_=true;
    word38_=0;word3C_=48; //232D40 is literally return48, not host channel availability.
    for(unsigned n=0;n<3;++n)if(!lists_[n].initialize104410()){
        //102EC4 calls actual virtual10 after a list allocation fails. Retire
        // this actual empty owner and its known A0, retaining Main00 identity.
        Diagnostic release;
        if(!retireNativeOrEmpty102CD0(release))std::terminate();
        return fail(d,Status::allocation,0x102E7C+0x18u*n);
    }
    nativeInitialized_=true;flags9CBit0_=false; //13FE5C follows successful setup.
    d={Status::complete,0x13FE5C};return true;
}
bool OriginalApplicationAural50KG::attachNativePrefix140108(Diagnostic& d)noexcept {
    d={};if(!current()||!nativeInitialized_||!backendCurrent()||attached_||entity_.parent1C||entity_.previous10||entity_.next14)
        return fail(d,Status::phase,0x140108);
    // Only this exact bounded native Light predecessor is admitted. Missing
    // retail virtual94/texture-manager Entity are NOT inferred complete.
    const auto* light=entities_->lightManager426F60_;
    const auto& a=entities_->entityAuthority_;
    if(!light||entities_->root10_.firstChild18!=light||light->next14||light->previous10||light->firstChild18||
       light->parent1C!=&entities_->root10_||light->kind!=K::lightManager147ED0||
       a.count_!=4||a.enrollmentSerial_!=4||entities_->objectCount423F60_!=4)
        return fail(d,Status::topology,0x140108);
    const auto r=entities_->entityAuthority_.attachChildren(entities_->root10_,entity_,0xffffffffu,0);
    if(r.status!=game::OriginalWreckComponentStatus50LA::complete)return fail(d,Status::topology,r.sourcePC);
    attached_=true;d={Status::complete,0x140108};return true;
}
bool OriginalApplicationAural50KG::releaseRegistered103D00(C& emitter,Diagnostic& d)noexcept {
    // Native gameplay's private AI list has not yet transferred these real
    // Entity owners here. Refuse BEFORE1046D0/detach/release rather than erase
    // a registration without its actual virtual10 destructor. A future typed
    // producer must add its concrete preflight/release branch here first.
    (void)emitter;return fail(d,Status::registeredEmitter,0x103DA8);
}
bool OriginalApplicationAural50KG::clear103DD0(Diagnostic& d)noexcept {
    d={};if(!current()||clearCalls_==std::numeric_limits<std::uint64_t>::max())return fail(d,Status::owner,0x103DD0);
    // Every iteration uses the REAL retained list, not a temporary empty list
    // or native_original_ai_audio::Owner's shutdown. No A0/backend mutation.
    while(lists_[0].count){
        auto* emitter=lists_[0].at104750(0);
        if(!emitter)return fail(d,Status::topology,0x103DF0);
        if(!releaseRegistered103D00(*emitter,d))return false;
    }
    ++clearCalls_;d={Status::complete,0x103E14};return true;
}
bool OriginalApplicationAural50KG::canActivateUiInitializer1B7340(Diagnostic& d)const noexcept {
    d={};if(!current()||!nativeInitialized_||!backendCurrent()||!attached_||
       entity_.parent1C!=&entities_->root10_)return fail(d,Status::owner,0x1B7340);
    // Actual Aural table3CC900+20 is BASE141560, including1414C0 notification.
    // This reached native owner is childless; never replace a nonempty real
    // notification tree with an activity bit or an assumed no-op.
    if(entity_.firstChild18)return fail(d,Status::topology,0x1414CC);
    d={Status::complete,0x1B7340};return true;
}
bool OriginalApplicationAural50KG::activateUiInitializer1B7340(Diagnostic& d)noexcept {
    if(!canActivateUiInitializer1B7340(d))return false;
    game::OriginalWreckComponent50LA* const node=&entity_;
    const game::OriginalWreckComponentDomain50LA reached{entity_.sessionGeneration,&node,1};
    const auto activated=game::setOriginalWreckBaseActivity50LA(reached,entity_,true);
    if(activated.status!=game::OriginalWreckComponentStatus50LA::complete)
        return fail(d,Status::topology,activated.sourcePC);
    d={Status::complete,0x141598};return true;
}
bool OriginalApplicationAural50KG::canInitializeUiReverb232C70(Diagnostic& d)const noexcept {
    d={};
    // Source1B7364 is a separate reached interface command, after activation
    // but before any UI sound. Preflight is also legal before that activation.
    if(!current()||!nativeInitialized_||!backendCurrent()||!attached_||
       entity_.parent1C!=&entities_->root10_||entity_.firstChild18||entity_.descendants24)
        return fail(d,Status::owner,0x1B7364);
    const auto& i=**interfaceA0_;const auto& main=*i.main;
    if(i.mixer->mixedFrames()||i.wetProcessor->nextFrame()||i.streamGeneration||
       main.nativeGroupControlCount_||main.nativeGroupControlSequence_||main.nativeWetMasterCount_||
       main.nativeWetMasterSequence_||main.nativeWetMasterFrame_||main.nativeWetMasterPercent_||main.nativeWetMasterRaw_||
       !main.canRecordControlChange()||!i.mixer->canRecordControlChanges())
        return fail(d,Status::phase,0x26A820);
    for(std::size_t n=0;n<i.nativeCount;++n)
        if(i.mixer->active(n)||i.mixer->playbackRevision(n))return fail(d,Status::phase,0x26A820);
    d={Status::complete,0x1B7364};return true;
}
bool OriginalApplicationAural50KG::initializeUiReverb232C70(Diagnostic& d)noexcept {
    if(!canInitializeUiReverb232C70(d))return false;
    if((entity_.flags29&0x0du)!=1u)return fail(d,Status::phase,0x1B7340);
    auto& i=**interfaceA0_;auto& main=*i.main;
    // Exact232C70(AuralA0,0,0,-1,-1): absolute zero, optional parameters
    // untouched. Native synchronous command2 shares Main's real ordered
    // control sequence with later command25/Page command2 publications.
    const std::array<NativeSourceReverb50KG::RawMaster,2> zero{{{0,0},{0,0}}};
    if(!i.wetProcessor->setRawMasters(zero,i.mixer->mixedFrames()))
        return fail(d,Status::backend,0x26A820);
    ++main.nativeWetMasterCount_;main.nativeWetMasterSequence_=++main.nativeGroupControlSequence_;
    main.nativeWetMasterFrame_=i.mixer->mixedFrames();main.nativeWetMasterPercent_=0;main.nativeWetMasterRaw_=0;
    i.mixer->noteControlChange();main.noteControlChange();
    // No fallible work after the numerical write. Startup history, allpass/
    // FIR phase, clock and dry gains remain intact; this is not DSP completion.
    d={Status::complete,0x232D0C};return true;
}
bool OriginalApplicationAural50KG::canInitializeUiReentryReverb232C70(Diagnostic& d)const noexcept {
    d={};
    // Application alone issues this branch from a new1B7300, after proving
    // the prior dispatcher/dialog retirement. It may preflight before the
    // new counted dispatcher exists; that constructor owns the no-replay
    // boundary, not a persistent readiness bit on Aural or its PCM backing.
    if(!current()||!nativeInitialized_||!backendCurrent()||!attached_||
       entity_.parent1C!=&entities_->root10_||entity_.firstChild18||entity_.descendants24||
       issuer_->sourceAural00Owner_.get()!=this||issuer_->frontendAudioShutdownComplete_||
       issuer_->garageCustodyOwner_||!nativeOutputSourceQuiescent())
        return fail(d,Status::owner,0x1B7364);
    const auto& b=issuer_->garageAudioBackingOwner_;
    const auto& i=**interfaceA0_;const auto& main=*i.main;
    if(b->stage_!=OriginalUiGarageAudioBacking50KG::Stage::transportPrepared||
       !b->frontendOutputAttempted_||b->frontendOutputClosed_||b->frontendOutputCloseFailed_||
       b->frontendOutputAccountingFailed_||b->frontendOutputSerial_<=1||
       b->frontendOutputAuralEntity_!=&entity_||!b->frontendOutputOwner_)
        return fail(d,Status::owner,0x1B7364);
    const auto& output=*b->frontendOutputOwner_;
    const auto transport=output.transport();const auto info=output.info();
    const auto frame=i.mixer->mixedFrames();
    // prepareFrontendAudioOutput already consumed the genuine device-close
    // receipt before rebinding this same transport. Rebind changes control
    // revisions, so do not demand the now-consumed old revision snapshot.
    // Its stopped playback/PCM cursors and cumulative frame remain exact.
    if(!output.enabled()||!transport||transport.get()!=i.main.get()||
       transport.owner_before(b)||b.owner_before(transport)||
       output.retainedOutputSerial()!=b->frontendOutputSerial_||
       output.sourceFrameOrigin()!=frame||output.sourceMixedFrames()!=frame||
       b->frontendClosedSourceFrames_!=frame||output.acceptedNanoseconds()||
       output.remainderNanoseconds()||output.lastPcmDrain().prepared||
       info.sampleRate!=i.nativeRate||info.simulationMicroseconds||info.mixedFrames||info.capturedFrames||
       info.deviceQueuedFrames||info.deviceConsumedFrames||info.deviceOverflowFrames)
        return fail(d,Status::phase,0x26A820);
    // Only normal initial/retired Garage source ownership can cross a real
    // frontend output rebind. Terminal failed-entry families remain closed.
    if(!main.current()||!main.reconstructibleGarageRowsCurrent()||
       (main.garageSourceLifetime_!=OriginalUiMainAudio50KG::GarageSourceLifetime::initialPrefix&&
        main.garageSourceLifetime_!=OriginalUiMainAudio50KG::GarageSourceLifetime::retired)||
       main.nativeGroupControlCount_||i.mixer->nativeWattoRouting_.command||i.mixer->nativeSceneRouting_.command||
       main.ownsMusic_||main.ownsRiderConfirm_||main.musicSong_||main.fading_||
       !entities_->pendingWaves98A4_||!entities_->pendingWaves98A4_->current()||
       i.wetProcessor->nextFrame()!=frame||!main.nativeWetMasterCount_||
       !main.nativeWetMasterSequence_||main.nativeWetMasterSequence_>main.nativeGroupControlSequence_||
       main.nativeWetMasterFrame_>frame||
       main.nativeWetMasterCount_==std::numeric_limits<std::uint64_t>::max()||
       main.nativeGroupControlSequence_==std::numeric_limits<std::uint64_t>::max()||
       !main.canRecordControlChange(1)||!i.mixer->canRecordControlChanges(1))
        return fail(d,Status::phase,0x26A820);
    for(const auto& command:main.nativeGroupControls_)
        if(command)return fail(d,Status::phase,0x26A820);
    for(bool owned:main.ownsEffect_)if(owned)return fail(d,Status::phase,0x26A820);
    for(std::size_t n=0;n<i.nativeCount;++n){
        AudioVoiceObservation50HD voice;
        if(!i.mixer->inspectVoice(n,voice)||voice.voice.active)
            return fail(d,Status::phase,0x26A820);
        if(n<OriginalUiMainAudio50KG::courseRequiredVoices&&
           (voice.playbackRevision!=b->frontendClosedPlaybackRevisions_[n]||
            voice.voice.sourceFrame!=b->frontendClosedSourceFramesByVoice_[n]))
            return fail(d,Status::phase,0x26A820);
    }
    d={Status::complete,0x1B7364};return true;
}
bool OriginalApplicationAural50KG::initializeUiReentryReverb232C70(Diagnostic& d)noexcept {
    if(!canInitializeUiReentryReverb232C70(d))return false;
    if((entity_.flags29&0x0du)!=1u)return fail(d,Status::phase,0x1B7340);
    auto& i=**interfaceA0_;auto& main=*i.main;
    // The real repeated232C70(AuralA0,0,0,-1,-1), after1B7340. Apply at
    // the unchanged cumulative sample boundary, never rebuild/configure or
    // clear the processor, advance voices, rewind clocks or erase history.
    const std::array<NativeSourceReverb50KG::RawMaster,2> zero{{{0,0},{0,0}}};
    if(!i.wetProcessor->setRawMasters(zero,i.mixer->mixedFrames()))
        return fail(d,Status::backend,0x26A820);
    ++main.nativeWetMasterCount_;main.nativeWetMasterSequence_=++main.nativeGroupControlSequence_;
    main.nativeWetMasterFrame_=i.mixer->mixedFrames();main.nativeWetMasterPercent_=0;main.nativeWetMasterRaw_=0;
    i.mixer->noteControlChange();main.noteControlChange();
    // Numerical write is the last fallible operation. Existing work/FIR,
    // stream ages/generations, clips, cursors and output accounting survive.
    d={Status::complete,0x232D0C};return true;
}
bool OriginalApplicationAural50KG::finishNativeControls267BE0(Diagnostic& d)noexcept {
    d={};
    if(!current()||issuer_->sourceApplicationMode22C_!=0)return fail(d,Status::owner,0x267BE0);
    return submitNativeControls267BE0(d);
}
bool OriginalApplicationAural50KG::canFinishGaragePageInput267BE0(
    const OriginalUiGaragePageOwner50KG& page,Diagnostic& d)const noexcept {
    d={};
    if(!current()||!backendCurrent()||!nativeStreamSlotsCurrent()||
       !attached_||entity_.parent1C!=&entities_->root10_)return fail(d,Status::owner,0x267BE0);
    const auto& i=**interfaceA0_;const auto& main=*i.main;
    if(!main.nativeGaragePageInputAudioCurrent(page)||main.nativeWetOwner_!=this||
       i.controlBoundaryCount==std::numeric_limits<std::uint64_t>::max()||
       !main.precedingGroupControlsCurrent()||main.pendingNativeGroupControls()||
       !main.canRecordControlChange(0u)||!i.mixer->canRecordControlChanges(0u))
        return fail(d,Status::backend,0x267BE0);
    std::array<std::uint64_t,8> references{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,references))
        return fail(d,Status::backend,0x267BE0);
    for(std::size_t n=0;n<references.size();++n)
        if(references[n]&&(!i.streamSlots[n].occupied||references[n]!=i.streamSlots[n].generation))
            return fail(d,Status::backend,0x267BE0);
    return true;
}
bool OriginalApplicationAural50KG::finishGaragePageInput267BE0(
    const OriginalUiGaragePageOwner50KG& page,std::uint64_t& completedBoundary,Diagnostic& d)noexcept {
    completedBoundary=0;
    if(!canFinishGaragePageInput267BE0(page,d)||!submitNativeControls267BE0(d))return false;
    // Caller records this only after the real Page effect returned. The same
    // boundary count may never be used to replay its preceding input effects.
    completedBoundary=(**interfaceA0_).controlBoundaryCount;return true;
}
bool OriginalApplicationAural50KG::submitNativeControls267BE0(Diagnostic& d)noexcept {
    d={};
    if(!current()||!backendCurrent()||!nativeStreamSlotsCurrent()||
       !attached_||entity_.parent1C!=&entities_->root10_)
        return fail(d,Status::owner,0x267BE0);
    auto& i=**interfaceA0_;auto& main=*i.main;
    if(i.controlBoundaryCount==std::numeric_limits<std::uint64_t>::max()||
       !main.current()||!main.precedingGroupControlsCurrent())
        return fail(d,Status::backend,0x267BE0);
    const auto frame=i.mixer->mixedFrames();
    std::array<std::uint64_t,8> references{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,references))
        return fail(d,Status::backend,0x267BE0);
    for(std::size_t n=0;n<references.size();++n)
        if(references[n]&&(!i.streamSlots[n].occupied||references[n]!=i.streamSlots[n].generation))
            return fail(d,Status::backend,0x267BE0);
    // Retail267BE0 submits its active EE command buffer via266A10 and clears
    // the newly selected packet. The native implementation has no guest RPC
    // buffers: actual play/stop/master calls already publish synchronously.
    // Retained command25 objects are strong owner custody, not pending work.
    // Only their real unapplied routing writes need this ordered consumer.
    std::string detail;
    if(!main.serviceColdGarageRouting(detail))return fail(d,Status::backend,0x266A10);
    if(main.pendingNativeGroupControls()||!main.precedingGroupControlsCurrent()||
       !nativeStreamSlotsCurrent()||i.mixer->mixedFrames()!=frame||i.wetProcessor->nextFrame()!=frame)
        return fail(d,Status::backend,0x267C3C);
    ++i.controlBoundaryCount;i.controlBoundaryFrame=frame;
    i.controlBoundaryMainRevision=main.controlRevision_;
    i.controlBoundaryMixerRevision=i.mixer->controlRevision();
    i.controlBoundaryGroupSequence=main.nativeGroupControlSequence_;
    // No output/device drain, SourceSound233E00,267CB0 or FIR processing is
    // part of267BE0. The enclosing caller retains this exact native receipt.
    d={Status::complete,0x267CA4};return true;
}
bool OriginalApplicationAural50KG::nativeStreamSlotsCurrent(bool permitRetired)const noexcept {
    if(!backendCurrent(permitRetired))return false;
    const auto& i=**interfaceA0_;
    if(i.channelCount37C!=8||!i.handles9C0||!i.reuseAge9C8||!i.rows9CC||i.streamSelector9C4!=-1)return false;
    for(std::size_t n=0;n<i.streamSlots.size();++n){const auto& slot=i.streamSlots[n];
        if(i.handles9C0[n]!=std::uint32_t(slot.occupied)||
           i.reuseAge9C8[n]!=std::bit_cast<std::uint32_t>(slot.age)||
           !std::isfinite(slot.age)||slot.age<0.f)return false;
        if(!slot.generation){if(slot.occupied||slot.metadata||slot.age!=180.f)return false;continue;}
        if(slot.generation>i.streamGeneration||!slot.metadata)return false;
        // Strong Main custody outlives replaced/explicitly stopped native PCM.
        // Do not compare against the most recent Jukebox handle or revision.
        const auto* row=slot.kind==NativeInterface::StreamKind::song?
            i.main->nativeSongSourceCurrent(slot.sourceOrdinal,slot.clip):
            slot.kind==NativeInterface::StreamKind::riderConfirm?
            i.main->nativeRiderSourceCurrent(slot.sourceOrdinal,slot.clip):
            slot.kind==NativeInterface::StreamKind::watto?
            i.main->nativeWattoStreamSourceCurrent(slot.sourceOrdinal,slot.clip):nullptr;
        if(row!=slot.metadata)return false;
    }
    return true;
}
bool OriginalApplicationAural50KG::nativeStreamFinalRetirementCurrent()const noexcept {
    if(!nativeStreamSlotsCurrent(true))return false;
    const auto& i=**interfaceA0_;
    if(i.streamRetirementAuthorized||(i.streamGeneration&&!i.main->ordinaryOutputRetired_))return false;
    // Root's real device-close/ordinary retirement precedes Pending teardown.
    // EOF or a quiet decoder alone never permits final stream-slot discharge.
    for(std::size_t n=0;n<i.nativeCount;++n)
        if(i.mixer->active(n)||i.mixer->nativeSourceVoiceRoutes_[n].count)return false;
    std::array<std::uint64_t,8> references{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,0.f,false,references))return false;
    for(std::size_t n=0;n<references.size();++n)
        if(references[n]&&(!i.streamSlots[n].occupied||references[n]!=i.streamSlots[n].generation))return false;
    // Mixer release independently checks every reserved pool cell and claim.
    // Unreserved stream-bit cells/occupied slots may persist until this actual
    // interface dies; neither its final guard nor this observation ages them.
    return true;
}
bool OriginalApplicationAural50KG::prepareNativeStreamStart(OriginalUiMainAudio50KG& main,
    NativeInterface::StreamKind kind,std::uint32_t ordinal,const AudioClip50HD& clip,bool loop,
    std::array<NativeInterface::StreamSlot,8>& next,AudioMixer50HD::NativeSourceStreamStart50KG& start)const noexcept {
    if(!nativeStreamSlotsCurrent()||(**interfaceA0_).main.get()!=&main)return false;
    const bool music=kind==NativeInterface::StreamKind::song;
    if(loop!=music||clip.channels!=(music?2u:1u))return false;
    const auto* row=music?main.nativeSongSourceCurrent(ordinal,clip):
        kind==NativeInterface::StreamKind::riderConfirm?main.nativeRiderSourceCurrent(ordinal,clip):
        kind==NativeInterface::StreamKind::watto?main.nativeWattoStreamSourceCurrent(ordinal,clip):nullptr;
    if(!row)return false;
    const auto& i=**interfaceA0_;
    if(i.streamGeneration>std::numeric_limits<std::uint64_t>::max()-clip.channels)return false;
    next=i.streamSlots;start={};start.count=static_cast<std::uint8_t>(clip.channels);
    start.duration=row->admittedDurationSeconds;start.loop=loop;
    for(std::size_t channel=0;channel<clip.channels;++channel){
        std::size_t selected=next.size();float best=0.f;
        for(std::size_t n=0;n<next.size();++n)if(!next[n].occupied){
            // Exact266B70: zero best selects every free candidate; positive
            // equal ages retain the earlier candidate. Never steal a slot.
            if(best==0.f||next[n].age>best){selected=n;best=next[n].age;}
        }
        if(selected==next.size())return false;
        auto& slot=next[selected];slot.kind=kind;slot.sourceOrdinal=ordinal;
        slot.generation=i.streamGeneration+channel+1;slot.metadata=row;slot.clip=clip;
        slot.occupied=true;slot.age=0.f;
        start.slots[channel]=static_cast<std::uint8_t>(selected);start.generations[channel]=slot.generation;
    }
    return true;
}
void OriginalApplicationAural50KG::commitNativeStreamSlots(
    const std::array<NativeInterface::StreamSlot,8>& next)const noexcept {
    auto& i=**interfaceA0_;i.streamSlots=next;
    for(std::size_t n=0;n<next.size();++n){
        i.handles9C0[n]=std::uint32_t(next[n].occupied);
        i.reuseAge9C8[n]=std::bit_cast<std::uint32_t>(next[n].age);
        if(next[n].generation>i.streamGeneration)i.streamGeneration=next[n].generation;
    }
    // Native typed rows above retain pitch/duration authoring. Do not fill
    // unknown raw9CC group fields with fabricated source group IDs.
}
bool OriginalApplicationAural50KG::playNativeMainSong234170(OriginalUiMainAudio50KG& main,
    std::uint32_t song,const AudioClip50HD& clip,float gain,float pitch)const noexcept {
    std::array<NativeInterface::StreamSlot,8> next;AudioMixer50HD::NativeSourceStreamStart50KG start;
    if(!prepareNativeStreamStart(main,NativeInterface::StreamKind::song,song,clip,true,next,start))return false;
    auto& i=**interfaceA0_;
    // Existing actual Main native bus policy: Song gain40% on dry master1.
    // This does not invent original UI Group+50/registry3 or a command25.
    const float expectedPitch=song==6?OriginalUiGarageWattoAudio50KG::nativeRendererPitch:1.f;
    if(gain!=.4f||pitch!=expectedPitch||i.mixer->auralMasterBus_[0]!=1u||
       i.mixer->hasVoiceGroupGain50IC(0))return false;
    // Native atomic admission: failed preflight leaves the outgoing native
    // voice/slots intact; this is NOT a fabricated original success receipt.
    if(!i.mixer->playNativeSourceOrdinaryStream(*this,OriginalUiMainAudio50KG::musicVoice,clip,gain,pitch,start))return false;
    commitNativeStreamSlots(next);return true;
}
bool OriginalApplicationAural50KG::playNativeMainEffect233410(OriginalUiMainAudio50KG& main,
    std::size_t index,const AudioClip50HD& clip,float gain,float pitch)const noexcept {
    if(!nativeStreamSlotsCurrent()||(**interfaceA0_).main.get()!=&main)return false;
    AudioMixer50HD::NativeSourceSampleStart50KG start;
    if(!main.nativeEffectSourceCurrent(index,clip,start))return false;
    auto& i=**interfaceA0_;const auto voice=OriginalUiMainAudio50KG::firstEffectVoice+index-1;
    if(gain!=1.f||pitch!=1.f||i.mixer->auralMasterBus_[voice]!=0u||i.mixer->hasVoiceGroupGain50IC(voice))return false;
    return i.mixer->playNativeSourceOrdinarySample(*this,voice,clip,gain,pitch,start);
}
bool OriginalApplicationAural50KG::playNativeMainRider233410(OriginalUiMainAudio50KG& main,
    std::size_t index,const AudioClip50HD& clip,float gain,float pitch)const noexcept {
    if(index>std::numeric_limits<std::uint32_t>::max())return false;
    std::array<NativeInterface::StreamSlot,8> next;AudioMixer50HD::NativeSourceStreamStart50KG start;
    if(!prepareNativeStreamStart(main,NativeInterface::StreamKind::riderConfirm,
        static_cast<std::uint32_t>(index),clip,false,next,start))return false;
    auto& i=**interfaceA0_;
    constexpr auto voice=OriginalUiMainAudio50KG::riderConfirmVoice;
    if(gain!=1.f||pitch!=1.f||i.mixer->auralMasterBus_[voice]!=0u||i.mixer->hasVoiceGroupGain50IC(voice))return false;
    if(!i.mixer->playNativeSourceOrdinaryStream(*this,voice,clip,gain,pitch,start))return false;
    commitNativeStreamSlots(next);return true;
}
bool OriginalApplicationAural50KG::playNativeWattoStream233410(OriginalUiMainAudio50KG& main,
    const void* command,std::size_t index,std::size_t voice,const AudioClip50HD& clip,
    std::int32_t volume,float pitch,std::int32_t angle)const noexcept {
    if(index>std::numeric_limits<std::uint32_t>::max()||volume!=100||angle||
       pitch!=OriginalUiGarageWattoAudio50KG::nativeRendererPitch||
       !main.nativeWattoStreamStartCurrent(index,voice,clip,command))return false;
    std::array<NativeInterface::StreamSlot,8> next;AudioMixer50HD::NativeSourceStreamStart50KG start;
    if(!prepareNativeStreamStart(main,NativeInterface::StreamKind::watto,
        static_cast<std::uint32_t>(index),clip,false,next,start))return false;
    auto& i=**interfaceA0_;
    if(!i.mixer->playNativeSourceStreamVolume(command,*this,voice,clip,volume,pitch,angle,start))return false;
    commitNativeStreamSlots(next);return true;
}
bool OriginalApplicationAural50KG::previewNativeStreamSuffix(float factor,
    std::array<NativeInterface::StreamSlot,8>& next)const noexcept {
    if(!nativeStreamSlotsCurrent()||!std::isfinite(factor)||factor<0.f)return false;
    const auto& i=**interfaceA0_;std::array<std::uint64_t,8> before{},after{};
    if(!i.mixer->nativeSourceStreamReferences267CB0(*this,factor,false,before)||
       !i.mixer->nativeSourceStreamReferences267CB0(*this,factor,true,after))return false;
    next=i.streamSlots;
    for(std::size_t n=0;n<next.size();++n){auto& slot=next[n];
        if((before[n]&&(!slot.occupied||before[n]!=slot.generation))||
           (after[n]&&(!slot.occupied||after[n]!=slot.generation)))return false;
        // Channel cleanup267F7C..267FA8 precedes the suffix. A newly released
        // slot takes free-age268144 this visit; a later orphan2682BC does not.
        if(before[n]&&!after[n])slot.occupied=false;
        if(!slot.occupied&&slot.age<180.f){
            const float advanced=slot.age+factor;if(!std::isfinite(advanced))return false;slot.age=advanced;
        }
        if(slot.occupied&&!after[n])slot.occupied=false; //9C4 is proven -1.
    }
    return true;
}
bool OriginalApplicationAural50KG::boundedSourceBodyCurrent()const noexcept {
    if(word30_||word34_||word38_||word3C_!=48||word40_||word44_||word48_||
       word98_!=-1||entities_->diagnostic14_||entity_.firstChild18||entity_.descendants24)return false;
    for(const auto& list:lists_)if(!list.valid()||list.count)return false;
    for(const auto* listener:listeners4C_)if(listener)return false;
    return true;
}
bool OriginalApplicationAural50KG::canPublishNativeGarageVoices(const OriginalUiMainAudio50KG& main,
    std::size_t maximumMonoPlays)const noexcept {
    if(!current()||!backendCurrent()||!nativeStreamSlotsCurrent()||(**interfaceA0_).main.get()!=&main||
       !attached_||entity_.parent1C!=&entities_->root10_||(entity_.flags29&0x0du)!=1u||
       !(entity_.mask20&2u)||!boundedSourceBodyCurrent()||
       !issuer_->nativeSourceAudioDispatcherCurrent(*this))return false;
    const auto& i=**interfaceA0_;std::array<NativeInterface::StreamSlot,8> next;
    if(maximumMonoPlays>i.streamSlots.size()||
       !i.mixer->canServiceNativeSourceVoices267CB0(*this,0.f)||!previewNativeStreamSuffix(0.f,next)||
       !i.mixer->canReserveNativeSourceMonoVoices(maximumMonoPlays))return false;
    std::size_t free=0;for(const auto& slot:i.streamSlots)if(!slot.occupied)++free;
    // Conservative canExecute bound before shared RNG/source mutations: every
    // requested play could be a mono response. A later actual wing is a sample.
    // No speculative stop or preview aging can manufacture extra free slots.
    return free>=maximumMonoPlays;
}
bool OriginalApplicationAural50KG::canUpdate103720(float factor,Diagnostic& d)const noexcept {
    d={};if(!current()||!backendCurrent()||!attached_||entity_.parent1C!=&entities_->root10_||
       (entity_.flags29&0x0du)!=1u||!(entity_.mask20&2u))return fail(d,Status::owner,0x103720);
    if(!std::isfinite(factor)||factor<0.f||issuer_->sourceApplicationMode22C_!=2||
       !issuer_->sourceApplicationElapsed23C_||std::bit_cast<std::uint32_t>(factor)!=
       std::bit_cast<std::uint32_t>(*issuer_->sourceApplicationElapsed23C_))
        return fail(d,Status::phase,0x103758);
    // Preflight the whole supported body before its first source effect.
    // No invented listener/emitter traversal, capacity/eviction or debug draw.
    if(word30_||word34_||word38_||word3C_!=48||word40_||word44_||word48_||
       word98_!=-1||entities_->diagnostic14_||entity_.firstChild18||entity_.descendants24)
        return fail(d,Status::phase,0x103280);
    for(const auto& list:lists_)if(list.count)return fail(d,Status::registeredEmitter,0x103280);
    for(const auto* listener:listeners4C_)if(listener)return fail(d,Status::phase,0x103790);
    const auto& i=**interfaceA0_;
    std::array<NativeInterface::StreamSlot,8> next;
    if(!i.mixer->canServiceNativeSourceVoices267CB0(*this,factor)||!previewNativeStreamSuffix(factor,next))
        return fail(d,Status::backend,0x267CB0);
    d={Status::complete,0x103758};return true;
}
bool OriginalApplicationAural50KG::update103720(float factor,Diagnostic& d)noexcept {
    if(!canUpdate103720(factor,d))return false;
    auto& i=**interfaceA0_;
    std::array<NativeInterface::StreamSlot,8> next;
    if(!previewNativeStreamSuffix(factor,next)||!i.mixer->serviceNativeSourceVoices267CB0(*this,factor))
        return fail(d,Status::backend,0x267CB0);
    commitNativeStreamSlots(next); // All suffix failure checks preceded channel mutation.
    //103760 ->103280: empty5C skips both passes. Actual1047C0 writes the
    // two empty list counts without discarding allocated backing/capacity.
    lists_[1].count=0;lists_[2].count=0; //1032E8/1032F0
    // Exact0+48-0 capacity admits no eviction; empty84, zero48, null14 and
    // childless inherited141ED0 take their real empty paths. This is not a
    // general active-source service or playback-readiness receipt.
    d={Status::complete,0x103B04};return true;
}
bool OriginalApplicationAural50KG::retireNativeOrEmpty102CD0(Diagnostic& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id())return fail(d,Status::affinity,0x102CD0);
    if(retired_)return true;
    if(!current()||entity_.firstChild18||entity_.next14||!entities_->entityAuthority_.canRetire(entity_))return fail(d,Status::phase,0x102CD0);
    if(attached_?(entity_.parent1C!=&entities_->root10_):(entity_.parent1C||entity_.previous10))
        return fail(d,Status::topology,0x142590);
    for(const auto* listener:listeners4C_)if(listener)return fail(d,Status::phase,0x102D14);
    for(const auto& list:lists_)if(list.count)return fail(d,Status::registeredEmitter,0x102D60);
    if(interfaceA0_&&*interfaceA0_){auto& i=**interfaceA0_;
        if(i.mastersApplied){
            if(!backendCurrent(true)||!nativeStreamFinalRetirementCurrent()||
               i.main->nativeGroupControlCount_||entities_->pendingWaves98A4_)
                return fail(d,Status::backend,0x232A38);
            for(const auto* group:entities_->soundGroups434A10_)if(group)return fail(d,Status::phase,0x232A38);
            for(std::size_t n=0;n<i.nativeCount;++n)if(i.mixer->active(n))return fail(d,Status::backend,0x232A38);
            // Release the borrowed numerical producer while the exact Aural
            // identity, interface and strong processor owner are still alive.
            // Mixer release independently requires all voice/route claims gone.
            // A rejection changes neither master ownership nor stored history.
            if(!i.wetProcessor||!i.wetProcessorBound||
               !i.mixer->releaseNativeSourceWetProcessor(*this,*i.wetProcessor))
                return fail(d,Status::backend,0x232A38);
            i.wetProcessorBound=false;i.streamRetirementAuthorized=true;
            // Withdraw the Main control borrow after Mixer has stopped using
            // the DSP, before destroying its owning interface or Main alias.
            i.main->nativeWetPlaySong_=nullptr;i.main->nativeWetPlayEffect_=nullptr;
            i.main->nativeWetPlayRider_=nullptr;i.main->nativeWetPlayWatto_=nullptr;i.main->nativeWetCanPublish_=nullptr;
            i.main->nativeWetSetRawMasters_=nullptr;i.main->nativeWetProcessor_=nullptr;
            i.main->nativeWetOwner_=nullptr;i.main->nativeWetEpoch_=0;i.main->nativeWetInterfaceRetired_=true;
            i.mixer->auralMasterOwner_=nullptr;i.mastersApplied=false;
        }
        // Earlier allocation/startup/bind failures may retain an unbound
        // processor, but a live borrower can never be silently destroyed.
        if(i.wetProcessorBound||(i.main&&(i.main->nativeWetOwner_||i.main->nativeWetProcessor_||
           i.main->nativeWetEpoch_||i.main->nativeWetSetRawMasters_||i.main->nativeWetPlaySong_||i.main->nativeWetPlayEffect_||i.main->nativeWetPlayRider_||i.main->nativeWetPlayWatto_||i.main->nativeWetCanPublish_)))return fail(d,Status::backend,0x232A38);
        // Release actual native interface arrays/transport borrow, never reset
        // Main while another source owner still pins it. This is the actual
        // interface retirement where retained wet history is finally destroyed.
        // Root retires the separately owned backing later.
        interfaceA0_->reset();
    }
    for(std::size_t n=3;n;--n)lists_[n-1].storage.reset(); //102D60/74/88 order.
    if(attached_){
        if(entity_.parent1C!=&entities_->root10_)return fail(d,Status::topology,0x142590);
        if(entity_.previous10)entity_.previous10->next14=entity_.next14;
        else entities_->root10_.firstChild18=entity_.next14;
    }else if(entity_.parent1C||entity_.previous10)return fail(d,Status::topology,0x142590);
    if(!entities_->entityAuthority_.retire(entity_))std::terminate();
    --entity_.reference00;--entities_->objectCount423F60_;
    entities_->auralNativeOwner_=nullptr;entities_->aural00Live_=false;retired_=true;
    //Source Application00 remains its stale identity, never forged null.
    d={Status::complete,nativeInitialized_?0x102D94u:0u};return true;
}
OriginalApplicationAural50KG::Observation OriginalApplicationAural50KG::inspect()const noexcept {
    Observation o;if(thread_!=std::this_thread::get_id())return o;
    o.entity=constructed_?&entity_:nullptr;o.constructed=constructed_;o.attached=attached_;o.retired=retired_;
    o.interfaceKnown=interfaceA0_.has_value();o.nativeInitialized=nativeInitialized_;o.sourceCapacity3C=word3C_;o.clearCalls=clearCalls_;
    for(std::size_t n=0;n<3;++n){const auto& l=lists_[n];o.lists[n]={l.count,l.growth,l.capacity,l.storage.get(),l.constructed,l.allocatedBit0};}
    if(interfaceA0_&&*interfaceA0_){const auto& i=**interfaceA0_;o.interfaceA0=&i;o.nativeMain=i.main.get();o.mixer=i.mixer;
        o.nativeVoiceCount=i.nativeCount;o.masterPercent=i.masterPercent;o.masterControlsApplied=i.mastersApplied;
        o.wetProcessor=i.wetProcessor.get();
        if(i.wetProcessor){o.wetProcessorConfigured=i.wetProcessor->configured();o.wetProcessorNextFrame=i.wetProcessor->nextFrame();
            o.wetProcessorBound=i.wetProcessorBound&&i.mixer&&i.mixer->nativeSourceWetProcessorCurrent(*this,*i.wetProcessor);}}
    // Numerical ownership is not actual source-routed wet playback support.
    // wetOutputSupported remains false until the real route/producer is joined.
    return o;
}
}

namespace original_main_entry50KG {
bool Application::nativeSourceAudioDispatcherCurrent(const rr::platform::OriginalApplicationAural50KG& aural)const noexcept {
    const auto& scope=sharedUiScopeOwner_;
    if(!scope||sourceAural00Owner_.get()!=&aural||aural.issuer_!=this||
       (aural.entities_!=entitiesOwner_||aural.entities_.owner_before(entitiesOwner_)||entitiesOwner_.owner_before(aural.entities_))||scope->issuer_!=this||
       (scope->application_!=entitiesOwner_||scope->application_.owner_before(entitiesOwner_)||entitiesOwner_.owner_before(scope->application_))||!scope->live().okay()||!scope->dispatcherLive_)return false;
    const auto& dispatcher=scope->dispatcher_;
    return dispatcher.parent1C==&entitiesOwner_->root10_&&dispatcher.previous10==&aural.entity_&&
        aural.entity_.next14==&dispatcher&&scope->linked(dispatcher,&entitiesOwner_->root10_);
}
bool Application::stageAuralCold13FE24(std::shared_ptr<rr::platform::OriginalApplicationAural50KG>& out,
    rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(out||sourceAural00Owner_){d={O::Status::phase,0x13FE24};return false;}
    if(!O::construct13FE24(*this,out,d))return false;
    sourceAural00Owner_=out;return true;
}
bool Application::initializeAuralNative102DE0(rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(!sourceAural00Owner_||sourceAural00Owner_->issuer_!=this){d={O::Status::owner,0x102DE0};return false;}
    return sourceAural00Owner_->initializeNative102DE0(d);
}
bool Application::attachAuralNativePrefix140108(rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(!sourceAural00Owner_||sourceAural00Owner_->issuer_!=this){d={O::Status::owner,0x140108};return false;}
    return sourceAural00Owner_->attachNativePrefix140108(d);
}
bool Application::clearAuralRegistrations103DD0(rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(!sourceAural00Owner_||sourceAural00Owner_->issuer_!=this){d={O::Status::owner,0x27015C};return false;}
    return sourceAural00Owner_->clear103DD0(d);
}
bool Application::releaseAuralCold102CD0(rr::platform::OriginalApplicationAural50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalApplicationAural50KG;
    if(!sourceAural00Owner_||sourceAural00Owner_->issuer_!=this){d={O::Status::owner,0x102CD0};return false;}
    if(!sourceAural00Owner_->retireNativeOrEmpty102CD0(d))return false;
    sourceAural00Owner_.reset();return true;
}
}

namespace original_main_entry50KG {
bool Application::stageAuralLightNativePrefix13FE24(std::uint32_t nativeOutputRate,
    rr::platform::OriginalApplicationAural50KG::Diagnostic& d,std::string& error)const noexcept {
    using A=rr::platform::OriginalApplicationAural50KG;
    d={};error.clear();
    // This is a coherent private native join, not original main(argc) startup
    // or ordinary Garage admission. Every failed published prefix stays Root
    // owned for explicit cleanup; no destructor silently invents a rollback.
    std::shared_ptr<A> aural;
    if(!stageAuralCold13FE24(aural,d))return false;
    if(!prepareGarageAudioBackingCold(nativeOutputRate,error)){
        d={A::Status::backend,0x102E4C};return false;
    }
    if(!initializeAuralNative102DE0(d))return false;
    std::shared_ptr<rr::platform::OriginalUiLightManagerOwner50KG> light;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic ld;
    if(!stageLightManagerAfterAuralNative140074(light,ld)){
        d={A::Status::phase,ld.sourcePC};return false;
    }
    return attachAuralNativePrefix140108(d);
}
}
