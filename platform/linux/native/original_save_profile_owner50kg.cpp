#include "original_save_profile_owner50kg.h"
#include "original_save_native_read_stage50kg.h"
#include <cstring>
#include <utility>

namespace rr::platform {
OriginalSaveProfileOwner50KG::OriginalSaveProfileOwner50KG() noexcept=default;
OriginalSaveProfileOwner50KG::Observation OriginalSaveProfileOwner50KG::observation() const noexcept {
    Observation out;out.selectedServiceSlot=selectedServiceSlot_;out.seenCard3C8=seenCard3C8_;
    out.cache1914=cache1914_;out.directoryCache1918=directoryCache1918_;
    out.identity191C=identity191C_;out.flags=cardFlags_;out.hallOrigin=hallOrigin_;return out;
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
void OriginalSaveProfileOwner50KG::recordFreshHall(const game::OriginalUiHallRecords50KG& records) noexcept {
    hallOrigin_=HallOrigin::localizedFreshRecords;
    localizedNames_=records.namesSource;localizedText_=records.textSource;
    payloadRecords_.reset();importedRead_.reset();
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
        out=std::move(next);return true;
    }catch(...){return false;}
}
bool OriginalSaveProfileOwner50KG::hallRecordsCurrent(const game::OriginalUiHallRecords50KG& records)const noexcept {
    if(hallOrigin_==HallOrigin::localizedFreshRecords)
        return localizedNames_&&localizedText_&&!payloadRecords_&&!importedRead_&&
            records.namesSource==localizedNames_&&records.textSource==localizedText_;
    const std::array<game::OriginalUiHallCourseRecords50KG,13>* savedRows=nullptr;
    if(hallOrigin_==HallOrigin::nativeBeta){
        if(!nativeBetaHall_||records.namesSource||records.textSource||localizedNames_||localizedText_)return false;
        savedRows=&nativeBetaHall_->courses;
    }else{
        if(hallOrigin_!=HallOrigin::originalPayload||!payloadRecords_||!importedRead_||
           records.namesSource||records.textSource||localizedNames_||localizedText_||
           importedRead_->projection().retainedEnvelope().get()!=payloadRecords_.get()||
           importedRead_->receipt().kind()!=OriginalSaveNativeReader50KG::Kind::payload186C70)return false;
        savedRows=&importedRead_->projection().records().courses();
    }
    const auto& saved=*savedRows;
    for(std::size_t i=0;i<records.courses.size();++i){
        const auto& a=records.courses[i];const auto& b=saved[i];
        if(a.knockouts10388!=b.knockouts10388||a.lapNames102FC!=b.lapNames102FC||
           a.raceNames10348!=b.raceNames10348||a.knockoutNames10394!=b.knockoutNames10394)return false;
        for(std::size_t rank=0;rank<3;++rank)
            if(std::memcmp(&a.lap102F0[rank],&b.lap102F0[rank],sizeof(float))||
               std::memcmp(&a.race1033C[rank],&b.race1033C[rank],sizeof(float)))return false;
    }
    return true;
}

}
