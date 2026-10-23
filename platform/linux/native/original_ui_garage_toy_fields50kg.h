#pragma once
// Native271CF0 tail storage. Descriptors are not emitter instances, a complete
// Scene, Page ancestry, rendering, physics registration or gameplay readiness.
#include "../../../src/game/original_wreck_components50la.h"
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
class OriginalUiGarageToyFields50KG final {
    friend class OriginalUiGaragePodScene50KG;
    using C=game::OriginalWreckComponent50LA;
    struct Text260 final {
        std::array<char,260> bytes;
        void set(const std::string&);
        const char* get()const noexcept{return bytes.data();}
    };
    struct EmitterTemp final {
        std::int32_t startingZone00=0,typeID04=0;
        Text260 attachToSoultree08;
        //27C430 does NOT initialize10C. The unattached read leaves it untouched.
        std::optional<std::string> childObject10C;
        Text260 emitterFilename210;
        std::array<float,4> position320{{0,0,0,1}};
        std::optional<std::int32_t> numInstances330;
        std::optional<bool> startOn334;
        EmitterTemp();
    };
    struct EmitterWrapper final {
        std::unique_ptr<C*[]> live00; //Absent until real27C7B0 construction.
        std::unique_ptr<EmitterTemp> temp04;
        //27DF24 initializes0/4 and vtable14 only;8/C/10 are not zero fields.
        std::array<std::optional<std::uint32_t>,3> unread08To10;
    };
    struct TriggerList final {
        C entity;
        bool live=false;
        std::int32_t count30=0;
        //No trigger descriptors in the admitted Garage; nonzero rejects.
        std::unique_ptr<C*[]> records34;
    };
    struct ParticleList final {
        C entity;
        bool live=false;
        std::int32_t count30=0;
        std::unique_ptr<EmitterWrapper*[]> records34;
        std::array<std::unique_ptr<EmitterWrapper>,7> records;
    };
    OriginalUiGarageToyFields50KG()noexcept=default;
    std::unique_ptr<TriggerList> triggers3D0_;
    std::unique_ptr<ParticleList> particles3D8_;
    std::int32_t physicsCount4B0_=0,cameraCount3E4_=0,sunFlareCount4A8_=0,buoyCount538_=0;
    std::unique_ptr<C*[]> physics4B4_,sunFlares4A4_,buoys53C_;
    std::unique_ptr<std::int32_t[]> cameraZones3E8_;
    std::optional<bool> sunFlareModeA1_;
    std::int32_t staticParameterReadCount_=0;
    bool complete_=false,closed_=false;
public:
    ~OriginalUiGarageToyFields50KG()noexcept;
    OriginalUiGarageToyFields50KG(const OriginalUiGarageToyFields50KG&)=delete;
    OriginalUiGarageToyFields50KG& operator=(const OriginalUiGarageToyFields50KG&)=delete;
};
}
