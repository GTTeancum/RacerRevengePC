#include "original_save_profile_owner50kg.h"
#include <utility>

namespace rr::platform {
OriginalSaveProfileOwner50KG::OriginalSaveProfileOwner50KG() noexcept=default;
OriginalSaveProfileOwner50KG::Observation OriginalSaveProfileOwner50KG::observation() const noexcept {
    Observation out;out.selectedServiceSlot=selectedServiceSlot_;out.seenCard3C8=seenCard3C8_;
    out.cache1914=cache1914_;out.directoryCache1918=directoryCache1918_;
    out.identity191C=identity191C_;out.flags=cardFlags_;out.hallOrigin=hallOrigin_;return out;
}
void OriginalSaveProfileOwner50KG::markDirty1AB648() noexcept {
    cardFlags_.value=static_cast<std::uint8_t>(cardFlags_.value|1u);
    cardFlags_.knownMask=static_cast<std::uint8_t>(cardFlags_.knownMask|1u);
}
void OriginalSaveProfileOwner50KG::recordFreshHall(const game::OriginalUiHallRecords50KG& records) noexcept {
    hallOrigin_=HallOrigin::localizedFreshRecords;
    localizedNames_=records.namesSource;localizedText_=records.textSource;
    payloadRecords_.reset();
}
bool OriginalSaveProfileOwner50KG::preparePayloadMetadata(const game::OriginalSaveProfileStage50KG& stage,
    std::unique_ptr<OriginalSaveProfileOwner50KG>& out) const noexcept {
    try {
        // Retain the EXACT same envelope as all decoded fields, not a copied
        // or borrowed surrogate. Shared ownership adds no storage-read authority.
        const auto payload=stage.retainedEnvelope();
        if(!payload)return false;
        std::unique_ptr<OriginalSaveProfileOwner50KG> next(new OriginalSaveProfileOwner50KG(*this));
        next->identity191C_=payload->identity191C();
        //186D94..DE0 after actual reads: known bit0 clear/bit1 set; no invented
        // value is assigned to the constructor-unknown upper six bits.
        next->cardFlags_.value=static_cast<std::uint8_t>((next->cardFlags_.value&0xfcu)|2u);
        next->cardFlags_.knownMask=static_cast<std::uint8_t>(next->cardFlags_.knownMask|3u);
        next->hallOrigin_=HallOrigin::originalPayload;
        next->localizedNames_=nullptr;next->localizedText_=nullptr;
        next->payloadRecords_=payload;
        out=std::move(next);return true;
    }catch(...){return false;}
}
}
