#pragma once
#include "original_ui_garage_scene_setup50kg.h"
#include "original_ui_light_manager_owner50kg.h"
#include "original_ui_garage_fog_color_lease50kg.h"
#include "original_ui_garage_fog_packet50kg.h"
#include "../../../src/game/source_acos49t.h"
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalUiGarageVisibility50KG;
// Actual273AA0/273C30 payload. These owners do not certify a rendered Scene,
// Page attachment, runtime scheduling, camera projection or material setup.
class OriginalUiGarageSceneLighting50KG final {
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiLightManagerOwner50KG;
    friend class OriginalUiGarageVisibility50KG;
    friend class OriginalUiGarageFogColorLease50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    using C=game::OriginalWreckComponent50LA;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using Manager=OriginalUiLightManagerOwner50KG;
    using Lease=OriginalUiGarageFogColorLease50KG;
    using Vec=std::array<float,4>;
    struct Light final {
        C entity;
        std::uint16_t query30=0;
        std::uint8_t query32=0xff,flags33=0,knownFlags33=7;
        std::uint32_t type34=0,mode38=3;
        Vec diffuse3C{{128,128,128,0}},specular4C{{128,128,128,0}},ambient5C{{128,128,128,0}};
        std::array<float,3> position6C{},look78{{1,0,0}};
        float range84=0,field88=1;
        std::array<float,3> attenuation8C{{1,0,0}};
        float cone98=0,cone9C=0;
        Vec positionA0{{0,0,0,1}},lookB0{{0,0,0,1}};
        const void* fieldC0=nullptr;const void* fieldC4=nullptr;
        std::int32_t ordinalC8=-1;std::uint32_t luminanceCC=0xbf800000u;
        const void* observerD0=nullptr;const void* fieldD4=nullptr;const void* fieldD8=nullptr;
        bool constructed=false,typed=false,configured=false,registered=false,attached=false,retiring=false,derivedRetired=false,retired=false;
        std::uint64_t updatePublication=0;bool updateReturned=false,updateFailed=false;
    };
    struct Primitive final {
        //133F80 leaves00..1F undefined. No packet buffer exists before render.
        std::array<std::optional<std::uint32_t>,8> unwritten00To1C;
        void* buffer20=nullptr;std::uint32_t count24=0,count28=0;std::int32_t field2C=-1;
        std::unique_ptr<OriginalUiGarageFogPacket50KG> packet;
        bool current(std::uint32_t color)const noexcept {
            return packet?buffer20==packet->words.data()&&count24==128&&count28==13&&field2C==1&&packet->current(color):
                !buffer20&&!count24&&!count28&&field2C==-1;
        }
    };
    struct Fog final {
        C entity;
        std::optional<std::uint32_t> color30;
        std::optional<float> start34,end38,visibility3C,haziness40;
        std::uint32_t field44=0,field48=1,field4C=0;
        std::optional<float> far50,near54,gap58;
        std::unique_ptr<Primitive> helper5C;
        std::uint32_t dirty60=1;
    };
    struct Reset final {C entity;};
    struct Proxy final {C entity;std::optional<const C*> fog30;}; //34 untouched.
    enum class FogStage {empty,constructing,awaitingRenderer,complete};
    OriginalUiGarageSceneLighting50KG()noexcept=default;
    OriginalUiGaragePodScene50KG* owner_=nullptr;
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::shared_ptr<game::OriginalApplicationEntities50LA> application_;
    std::shared_ptr<Manager> manager_;
    std::shared_ptr<Lease> renderer_;
    std::uint64_t generation_=0;
    std::array<std::unique_ptr<Light>,2> lights_;
    std::unique_ptr<Fog> fog3EC_;
    std::unique_ptr<Reset> reset3F4_;
    std::unique_ptr<Proxy> proxy3F0_;
    std::array<bool,3> fogConstructed_{},fogRetiring_{},fogRetired_{};
    FogStage fogStage_=FogStage::empty;
    bool renderProxyActive_=false;
    bool managerBound_=false,lightsComplete_=false,rendererWritePerformed_=false,closed_=false;
    bool identityCurrent()const noexcept;
    bool lightIdentity(const Light&)const noexcept;
    bool updateLight147650(const OriginalUiGaragePageOwner50KG&,C&,float,std::uint64_t,D&)noexcept;
    void initializeLight146720(std::size_t)noexcept;
    bool configureLight146F60(std::size_t,const OriginalUiGarageSceneSetup50KG::LightTemp&,D&)noexcept;
    bool colorWriteCurrentFor(const ::original_main_entry50KG::Application*)const noexcept;
    C* fogEntity(std::size_t)const noexcept;
public:
    ~OriginalUiGarageSceneLighting50KG()noexcept;
    OriginalUiGarageSceneLighting50KG(const OriginalUiGarageSceneLighting50KG&)=delete;
    OriginalUiGarageSceneLighting50KG& operator=(const OriginalUiGarageSceneLighting50KG&)=delete;
};
}
