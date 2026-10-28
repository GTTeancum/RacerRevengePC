#pragma once
#include <cstdint>
#include <memory>
namespace original_race_hud_scope50LA { class Registration; }
namespace native_ai50IV { struct Session; }
namespace native_racer50HN { class Bank; }
namespace rr::game {
class OriginalApplicationEntities50LA;
class OriginalRaceConstructor50LA;
struct TrackNavigation50EU;
struct AiTrackMetadata50IU;
//One actual counted1AF7F0 manager. Its time30/flags58 and native list/scalar
//backing are the existing Setup Session, not another timing/finish machine.
//Original guest arrays and racer virtual74 are an explicit native adapter;
//neither that adapter nor publication certifies1AFAE0/full Race load/attach.
class OriginalRaceTimingManager50LA final {
    friend class OriginalRaceConstructor50LA;
    friend class ::original_race_hud_scope50LA::Registration;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    OriginalRaceTimingManager50LA(std::shared_ptr<OriginalApplicationEntities50LA>,
        std::uint64_t,native_ai50IV::Session&,const native_racer50HN::Bank&);
    bool constructed()const noexcept;
    bool continueNativeAssociation(OriginalRaceConstructor50LA&,native_ai50IV::Session&,
        const native_racer50HN::Bank&,const TrackNavigation50EU&,const AiTrackMetadata50IU&,std::uint32_t&)noexcept;
    bool readNativeFlags(const OriginalRaceConstructor50LA&,const native_ai50IV::Session&,
        const native_racer50HN::Bank&,std::uint8_t&)const noexcept;
    bool readQueuedEventEnable58(const OriginalRaceConstructor50LA&,const native_ai50IV::Session&,
        const native_racer50HN::Bank&,bool&)const noexcept;
    bool readEndCallerMode(const OriginalRaceConstructor50LA&,const native_ai50IV::Session&,
        const native_racer50HN::Bank&,std::uint8_t&)const noexcept;
    bool readAssociationHeadCD8(const OriginalRaceConstructor50LA&,const native_ai50IV::Session&,
        const native_racer50HN::Bank&,const std::uint8_t& actualArenaCD8,bool&)const noexcept;
    bool readAssociatedCount30(const OriginalRaceConstructor50LA&,const native_ai50IV::Session&,
        const native_racer50HN::Bank&,std::int32_t&)const noexcept;
    bool retireNativeDetached(std::uint32_t&)noexcept;
public:
    OriginalRaceTimingManager50LA(const OriginalRaceTimingManager50LA&)=delete;
    OriginalRaceTimingManager50LA& operator=(const OriginalRaceTimingManager50LA&)=delete;
    ~OriginalRaceTimingManager50LA()noexcept;
};
}
