#include "original_save_profile_owner50kg.h"
#include "original_save_native_read_stage50kg.h"
#include <cstring>
#include <limits>
#include <utility>

namespace rr::platform {
namespace {
bool sameHallCourse(const game::OriginalUiHallCourseRecords50KG& a,
    const game::OriginalUiHallCourseRecords50KG& b) noexcept {
    if(a.knockouts10388!=b.knockouts10388||a.lapNames102FC!=b.lapNames102FC||
       a.raceNames10348!=b.raceNames10348||a.knockoutNames10394!=b.knockoutNames10394)return false;
    for(std::size_t rank=0;rank<3;++rank)
        if(std::memcmp(&a.lap102F0[rank],&b.lap102F0[rank],sizeof(float))||
           std::memcmp(&a.race1033C[rank],&b.race1033C[rank],sizeof(float)))return false;
    return true;
}
}
OriginalSaveProfileOwner50KG::OriginalSaveProfileOwner50KG() noexcept=default;
OriginalSaveProfileOwner50KG::Observation OriginalSaveProfileOwner50KG::observation() const noexcept {
    Observation out;out.selectedServiceSlot=selectedServiceSlot_;out.seenCard3C8=seenCard3C8_;
    out.cache1914=cache1914_;out.directoryCache1918=directoryCache1918_;
    out.identity191C=identity191C_;out.flags=cardFlags_;out.hallOrigin=hallOrigin_;
    out.hallRecordEpoch=hallRecordDescendant_?hallRecordDescendant_->epoch:0;return out;
}
void OriginalSaveProfileOwner50KG::recordFreshNames() noexcept {
    // Source reset is strcpy(empty), not a21-byte clear. If this ever gains a
    // genuine repeated-reset caller, already-known suffix bytes must survive.
    for(auto& name:careerNames_)name.replaceCString({});
    for(auto& name:playerNames_)name.replaceCString({});
    namesInitialized_=true;
}
bool OriginalSaveProfileOwner50KG::namesReady()const noexcept {
    if(!namesInitialized_)return false;
    for(const auto& name:careerNames_)if(!name.valid())return false;
    for(const auto& name:playerNames_)if(!name.valid())return false;
    return true;
}
bool OriginalSaveProfileOwner50KG::persistedName(bool career,std::uint32_t index,
    OriginalPersistedProfileName50KG& out)const noexcept {
    if(!namesReady()||index>=(career?careerNames_.size():playerNames_.size()))return false;
    out=career?careerNames_[index]:playerNames_[index];return true;
}
void OriginalSaveProfileOwner50KG::markDirty1AB648() noexcept {
    cardFlags_.value=static_cast<std::uint8_t>(cardFlags_.value|1u);
    cardFlags_.knownMask=static_cast<std::uint8_t>(cardFlags_.knownMask|1u);
}
bool OriginalSaveProfileOwner50KG::recordFreshHall(const game::OriginalUiHallRecords50KG& records) noexcept {
    if(!records.namesSource||!records.textSource)return false;
    try {
        auto root=std::make_shared<const game::OriginalUiHallRecords50KG>(records);
        hallOrigin_=HallOrigin::localizedFreshRecords;
        localizedNames_=records.namesSource;localizedText_=records.textSource;
        freshHall_=std::move(root);payloadRecords_.reset();importedRead_.reset();
        nativeBetaHall_.reset();hallRecordDescendant_.reset();return true;
    }catch(...){return false;}
}
bool OriginalSaveProfileOwner50KG::preparePayloadMetadata(
    std::shared_ptr<const OriginalSaveNativeReadStage50KG> read,
    std::unique_ptr<OriginalSaveProfileOwner50KG>& out) const noexcept {
    try {
        // Retain the EXACT same envelope as all decoded fields, not a copied
        // or borrowed surrogate. Shared ownership adds no storage-read authority.
        if(!read||read->receipt().kind()!=OriginalSaveNativeReader50KG::Kind::payload186C70)return false;
        const auto payload=read->projection().retainedEnvelope();
        if(!payload)return false;
        std::unique_ptr<OriginalSaveProfileOwner50KG> next(new OriginalSaveProfileOwner50KG(*this));
        next->namesInitialized_=true;
        for(std::size_t i=0;i<next->careerNames_.size();++i)
            if(!next->careerNames_[i].retainExact(read->projection().careerNames219D()[i]))return false;
        for(std::size_t i=0;i<next->playerNames_.size();++i)
            if(!next->playerNames_[i].retainExact(read->projection().records().sharedFields().playerNames10F68[i]))return false;
        if(!next->namesReady())return false;
        next->identity191C_=payload->identity191C();
        //186D94..DE0 after actual reads: known bit0 clear/bit1 set; no invented
        // value is assigned to the constructor-unknown upper six bits.
        next->cardFlags_.value=static_cast<std::uint8_t>((next->cardFlags_.value&0xfcu)|2u);
        next->cardFlags_.knownMask=static_cast<std::uint8_t>(next->cardFlags_.knownMask|3u);
        next->hallOrigin_=HallOrigin::originalPayload;
        next->localizedNames_=nullptr;next->localizedText_=nullptr;
        next->payloadRecords_=payload;next->importedRead_=std::move(read);
        next->nativeBetaHall_.reset();next->freshHall_.reset();next->hallRecordDescendant_.reset();
        out=std::move(next);return true;
    }catch(...){return false;}
}
bool OriginalSaveProfileOwner50KG::sameHallRoot(const OriginalSaveProfileOwner50KG& other)const noexcept {
    return hallOrigin_==other.hallOrigin_&&localizedNames_==other.localizedNames_&&
        localizedText_==other.localizedText_&&payloadRecords_.get()==other.payloadRecords_.get()&&
        importedRead_.get()==other.importedRead_.get()&&nativeBetaHall_.get()==other.nativeBetaHall_.get()&&
        freshHall_.get()==other.freshHall_.get();
}
bool OriginalSaveProfileOwner50KG::hallRecordsCurrent(const game::OriginalUiHallRecords50KG& records,
    const std::shared_ptr<const game::OriginalHallProfileIdentity50KG>& identity,
    std::uint64_t generation)const noexcept {
    if(!identity||!generation)return false;
    const std::array<game::OriginalUiHallCourseRecords50KG,13>* savedRows=nullptr;
    if(hallOrigin_==HallOrigin::localizedFreshRecords){
        if(!freshHall_||!localizedNames_||!localizedText_||payloadRecords_||importedRead_||nativeBetaHall_||
           records.namesSource!=localizedNames_||records.textSource!=localizedText_||
           freshHall_->namesSource!=localizedNames_||freshHall_->textSource!=localizedText_)return false;
        savedRows=&freshHall_->courses;
    }else if(hallOrigin_==HallOrigin::nativeBeta){
        if(!nativeBetaHall_||freshHall_||payloadRecords_||importedRead_||records.namesSource||
           records.textSource||localizedNames_||localizedText_||nativeBetaHall_->namesSource||
           nativeBetaHall_->textSource)return false;
        savedRows=&nativeBetaHall_->courses;
    }else{
        if(hallOrigin_!=HallOrigin::originalPayload||!payloadRecords_||!importedRead_||freshHall_||nativeBetaHall_||
           records.namesSource||records.textSource||localizedNames_||localizedText_||
           importedRead_->projection().retainedEnvelope().get()!=payloadRecords_.get()||
           importedRead_->receipt().kind()!=OriginalSaveNativeReader50KG::Kind::payload186C70)return false;
        savedRows=&importedRead_->projection().records().courses();
    }
    if(hallRecordDescendant_){
        const auto& next=*hallRecordDescendant_;
        if(!next.epoch||next.profileIdentity.get()!=identity.get()||next.origin!=hallOrigin_||
           next.names!=localizedNames_||next.text!=localizedText_||
           next.payload.get()!=payloadRecords_.get()||next.read.get()!=importedRead_.get()||
           next.nativeBeta.get()!=nativeBetaHall_.get()||next.fresh.get()!=freshHall_.get()||
           next.generationBefore==std::numeric_limits<std::uint64_t>::max()||
           next.generationAfter!=next.generationBefore+1u||next.generationAfter>generation||
           next.records.namesSource!=records.namesSource||next.records.textSource!=records.textSource)return false;
        savedRows=&next.records.courses;
    }
    for(std::size_t i=0;i<records.courses.size();++i)
        if(!sameHallCourse(records.courses[i],(*savedRows)[i]))return false;
    return true;
}
bool OriginalSaveProfileOwner50KG::prepareHallRecordDescendant(
    const game::OriginalUiHallRecords50KG& current,const game::OriginalRaceRecordInsertionStage50KG& stage,
    const std::shared_ptr<const game::OriginalHallProfileIdentity50KG>& identity,
    std::uint64_t generation,std::unique_ptr<OriginalSaveProfileOwner50KG>& out)const noexcept {
    if(generation==std::numeric_limits<std::uint64_t>::max()||stage.profileGeneration()!=generation||
       stage.course()>=current.courses.size()||!hallRecordsCurrent(current,identity,generation)||
       !sameHallCourse(current.courses[stage.course()],stage.before()))return false;
    const bool changed=!sameHallCourse(stage.before(),stage.after());
    const auto epoch=hallRecordDescendant_?hallRecordDescendant_->epoch:0u;
    if(changed&&epoch==std::numeric_limits<std::uint64_t>::max())return false;
    try {
        std::unique_ptr<OriginalSaveProfileOwner50KG> next(new OriginalSaveProfileOwner50KG(*this));
        if(changed){
            auto descendant=std::make_shared<HallRecordDescendant>();
            descendant->records=current;descendant->records.courses[stage.course()]=stage.after();
            descendant->profileIdentity=identity;descendant->epoch=epoch+1u;
            descendant->generationBefore=generation;descendant->generationAfter=generation+1u;
            descendant->origin=hallOrigin_;descendant->names=localizedNames_;descendant->text=localizedText_;
            descendant->payload=payloadRecords_;descendant->read=importedRead_;
            descendant->nativeBeta=nativeBetaHall_;descendant->fresh=freshHall_;
            next->hallRecordDescendant_=std::move(descendant);
        }
        // Root/read identity, names, card flags and other metadata stay exact.
        // This copied owner is a private preparation container, NOT live state.
        // Recheck after allocating: a native allocation/new-handler boundary
        // must not silently turn a displaced root/preimage into a current one.
        if(!hallRecordPreparationMatches(*next,current,stage,identity,generation))return false;
        out=std::move(next);return true;
    }catch(...){return false;}
}
bool OriginalSaveProfileOwner50KG::hallRecordPreparationMatches(
    const OriginalSaveProfileOwner50KG& prepared,const game::OriginalUiHallRecords50KG& current,
    const game::OriginalRaceRecordInsertionStage50KG& stage,
    const std::shared_ptr<const game::OriginalHallProfileIdentity50KG>& identity,
    std::uint64_t generation)const noexcept {
    if(generation==std::numeric_limits<std::uint64_t>::max()||stage.profileGeneration()!=generation||
       stage.course()>=current.courses.size()||!sameHallRoot(prepared)||
       !hallRecordsCurrent(current,identity,generation)||
       !sameHallCourse(current.courses[stage.course()],stage.before()))return false;
    if(sameHallCourse(stage.before(),stage.after()))
        return prepared.hallRecordDescendant_.get()==hallRecordDescendant_.get();
    const auto epoch=hallRecordDescendant_?hallRecordDescendant_->epoch:0u;
    if(epoch==std::numeric_limits<std::uint64_t>::max()||!prepared.hallRecordDescendant_)return false;
    const auto& next=*prepared.hallRecordDescendant_;
    if(next.epoch!=epoch+1u||next.generationBefore!=generation||next.generationAfter!=generation+1u||
       !prepared.hallRecordsCurrent(next.records,identity,generation+1u))return false;
    for(std::size_t course=0;course<current.courses.size();++course){
        const auto& expected=course==stage.course()?stage.after():current.courses[course];
        if(!sameHallCourse(next.records.courses[course],expected))return false;
    }
    return true;
}

}
