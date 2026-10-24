#include "original_save_profile_stage50kg.h"
#include <utility>

namespace rr::game {
namespace {
using Stage=OriginalSaveProfileStage50KG;
constexpr std::size_t payloadOffset(std::size_t profile) noexcept {
    return OriginalSavePayloadEnvelope50KG::recordsPayloadOffset+
        profile-OriginalSavePayloadEnvelope50KG::recordsProfileOffset;
}
std::uint32_t little32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
        (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
}
Stage::Status result(Stage::Diagnostic& d,Stage::Status s,std::size_t offset=0) noexcept {
    d.status=s;d.payloadByteOffset=offset;return s;
}
}
OriginalSaveProfileStage50KG::OriginalSaveProfileStage50KG(
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> envelope,
    const OriginalUiGameSettingsProfile50KG& settings,const ControlPresets49D& controls,
    std::uint8_t flags) noexcept
    :envelope_(std::move(envelope)),settingsBefore_(settings),settingsAfter_(settings),
     controlsBefore_(controls),controlsAfter_(controls),flagsBefore_(flags),flagsAfter_(flags) {}
OriginalSaveProfileStage50KG::Status OriginalSaveProfileStage50KG::prepare(
    std::shared_ptr<const OriginalSavePayloadEnvelope50KG> envelope,
    const OriginalUiGameSettingsProfile50KG& currentSettings,const ControlPresets49D& currentControls,
    std::uint8_t currentFlags,std::unique_ptr<OriginalSaveProfileStage50KG>& out,Diagnostic& d) noexcept {
    d={};if(!envelope)return result(d,Status::missingEnvelope);
    if(!validOriginalUiGameSettingsProfile50KG(currentSettings))return result(d,Status::invalidCurrentSettings);
    try {
        std::unique_ptr<OriginalSaveProfileStage50KG> next(
            new OriginalSaveProfileStage50KG(std::move(envelope),currentSettings,currentControls,currentFlags));
        for(std::uint32_t career=0;career<next->careers_.size();++career){
            std::unique_ptr<OriginalSaveCareerProjection50KG> decoded;
            if(OriginalSaveCareerProjection50KG::retainCareer(next->envelope_,career,decoded,d.career)!=
                OriginalSaveCareerProjection50KG::Status::ok){
                d.careerIndex=career;return result(d,Status::careerFields,d.career.payloadByteOffset);
            }
            next->careers_[career]=decoded->fields();
            next->careerNames_[career]=decoded->name219D();
        }
        if(OriginalSaveRecordsProjection50KG::retainRecords(next->envelope_,next->records_,d.records)!=
            OriginalSaveRecordsProjection50KG::Status::ok)
            return result(d,Status::recordsFields,d.records.payloadByteOffset);
        const auto& saved=next->records_->sharedFields();
        next->settingsAfter_.soundMode1101C=saved.soundMode1101C;
        next->settingsAfter_.volume11020=saved.volume11020;
        next->settingsAfter_.flags11050=saved.flags11050;
        const auto* bytes=next->envelope_->originalBytes().data();
        for(std::size_t slot=0;slot<controlPresetSlots49D;++slot){
            auto& target=next->controlsAfter_.slots[slot];const auto lane=slot*4;
            target.configuration11040=little32(bytes+payloadOffset(0x11040)+lane);
            if(target.configuration11040>1)
                return result(d,Status::unsupportedControlConfiguration,payloadOffset(0x11040)+lane);
            target.preset11048=little32(bytes+payloadOffset(0x11048)+lane);
            if(target.preset11048>7)
                return result(d,Status::unsupportedControlPreset,payloadOffset(0x11048)+lane);
            target.field10F94=little32(bytes+payloadOffset(0x10f94)+lane);
            target.field10F9C=little32(bytes+payloadOffset(0x10f9c)+lane);
            target.field10FA4=little32(bytes+payloadOffset(0x10fa4)+lane);
            target.field11030=little32(bytes+payloadOffset(0x11030)+lane);
            target.field11038=little32(bytes+payloadOffset(0x11038)+lane);
            // Source pause labels switch on0..7; decrement/increment wrap this
            // same domain. Unsupported values leave source text unspecified.
            if(target.field10F94>7)
                return result(d,Status::unsupportedHudPreference,payloadOffset(0x10f94)+lane);
            if(target.field10F9C>7)
                return result(d,Status::unsupportedHudPreference,payloadOffset(0x10f9c)+lane);
            //177AB0 new-mode jump table3A8080: index3/default asserts. Other
            // entries have genuine branches, not necessarily native owners.
            if(target.field11030>7||target.field11030==3)
                return result(d,Status::unsupportedCameraMode,payloadOffset(0x11030)+lane);
            //10FA4 is tested as nonzero/toggled with !value; retain all bits.
            //11038 is matched by ID in2842F0's linear bank search. No guessed
            // small numeric range: missing ID leaves the prior choice intact.
            // No selector/preview: actions690..6B4 are not in the save payload.
            // Keep current action rows even when saved preset/config changes.
        }
        // This represents only the final186DE4..186E18 merge, not permission
        // to perform it before genuine188740 or before complete owner preflight.
        next->flagsAfter_=static_cast<std::uint8_t>((next->flagsBefore_&0xfeu)|
            next->envelope_->packedFlag11054Bit0());
        out=std::move(next);return result(d,Status::ok);
    } catch(...) {return result(d,Status::allocationFailure);}
}
}
