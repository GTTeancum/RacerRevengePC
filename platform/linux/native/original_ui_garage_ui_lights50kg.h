#pragma once
#include "original_ui_garage_manager_search50kg.h"
#include "original_ui_light_manager_owner50kg.h"
#include "../../../src/game/source_acos49t.h"
namespace rr::platform {
// Actual reached1CDB10 descriptor/Light146720/borrowed148F30 registry prefix.
// Unknown authored alpha remains unknown. No renderer, Scene or Page admission.
class OriginalUiGarageUiLights50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGarageWidget1Owner50KG;
    friend class OriginalUiLightManagerOwner50KG;
    friend class OriginalUiGarageManagerFog50KG;
    using C=game::OriginalWreckComponent50LA;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using Search=OriginalUiGarageManagerSearch50KG;
    using Manager=OriginalUiLightManagerOwner50KG;
    using Vec=std::array<float,4>;
public:
    enum class Status : std::uint8_t {complete,input,source,phase,allocation,scope,types,arithmetic,registry};
    struct Diagnostic {Status status=Status::complete;std::uint32_t sourcePC=0;std::size_t ordinal=0;
        assets::OriginalUiPageDiagnostic50KG parse{};};
    // Color alpha and descriptor lanes are readable only when their known
    // masks/flags say so. Native zero backing never initializes source alpha.
    struct LightObservation {
        const C* entity=nullptr;std::uint8_t query32=0xff,flags33=0,knownFlags33=0;
        std::uint32_t type34=0,mode38=0,range84=0,luminanceCC=0;
        Vec diffuse3C{},specular4C{},ambient5C{},positionA0{},lookB0{};
        bool constructed=false,typed=false,configured=false,registered=false,attached=false,retired=false;
        bool diffuseAlphaKnown=false,specularAlphaKnown=false,ambientAlphaKnown=false;
    };
    struct DescriptorObservation {
        Vec position00{},infinity10{},look20{},color3C{};std::uint8_t knownColor=0,flagsA4=0,knownFlagsA4=0;
        std::uint32_t typeA0=0,flareColor58=0;float range4C=0,brightness5C=0;
        std::array<std::uint8_t,5> layers50{};std::array<char,64> name60{};
        bool vectorsConstructed=false,light38Known=false,parsed=false,flareKnown=false;
        std::uintptr_t light38Identity=0;bool position30Present=false,look34Present=false;
    };
    struct Observation {std::array<LightObservation,2> lights{};std::array<DescriptorObservation,2> descriptors{};
        bool attempted=false,descriptorsPublished=false,complete=false,payloadReleased=false;};
    OriginalUiGarageUiLights50KG()noexcept=default;
    ~OriginalUiGarageUiLights50KG()noexcept;
    OriginalUiGarageUiLights50KG(const OriginalUiGarageUiLights50KG&)=delete;
    OriginalUiGarageUiLights50KG& operator=(const OriginalUiGarageUiLights50KG&)=delete;
    bool current()const noexcept;
    Observation inspect()const noexcept;
private:
    struct Light {
        C entity;std::uint16_t query30=0;std::uint8_t query32=0xff,flags33=0,knownFlags33=0;
        std::uint32_t type34=0,mode38=0;Vec diffuse3C{},specular4C{},ambient5C{};
        std::array<float,3> position6C{},look78{};float range84=0,field88=0;
        std::array<float,3> attenuation8C{};float cone98=0,cone9C=0;Vec positionA0{},lookB0{};
        const void* fieldC0=nullptr;const void* fieldC4=nullptr;std::int32_t ordinalC8=-1;std::uint32_t luminanceCC=0;
        const void* observerD0=nullptr;const void* fieldD4=nullptr;const void* fieldD8=nullptr;
        bool constructed=false,typed=false,configured=false,registered=false,attached=false,retiring=false,retired=false;
        bool diffuseAlphaKnown=false,specularAlphaKnown=false,ambientAlphaKnown=false;
    };
    struct Descriptor {
        Vec position00{},infinity10{},look20{};const Vec* position30=nullptr;const Vec* look34=nullptr;
        Light* light38=nullptr;Vec color3C{};std::uint8_t knownColor=0;float range4C=0;
        std::array<std::uint8_t,5> layers50{};std::uint32_t flareColor58=0;float brightness5C=0;
        std::array<char,64> name60{};std::uint32_t typeA0=0;std::uint8_t flagsA4=0,knownFlagsA4=0;
        bool light38Known=false,parsed=false,flareKnown=false;
    };
    std::shared_ptr<Grant> grant_;std::shared_ptr<Search> search_;std::shared_ptr<Manager> manager_;
    std::shared_ptr<game::OriginalApplicationEntities50LA> application_;
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets_;
    std::shared_ptr<assets::OriginalStandardArchiveProvider50LA> provider_;
    std::uint64_t providerRevision_=0;assets::OriginalUiDat50KG ui_;
    std::unique_ptr<Descriptor[]> descriptors29C_;std::array<std::unique_ptr<Light>,2> lights_{};
    bool attempted_=false,bound_=false,complete_=false,payloadReleased_=false;
    static bool fail(Diagnostic& d,Status s,std::uint32_t pc)noexcept{d.status=s;d.sourcePC=pc;return false;}
    bool identityCurrent()const noexcept;
    bool lightIdentity(const Light& l)const noexcept {
        return grant_&&l.entity.kind==game::OriginalWreckComponentKind50LA::light146720&&
            l.entity.sessionGeneration==grant_->generation_&&l.entity.objectKey==
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&l.entity));
    }
    bool scopeCurrent()const noexcept{return grant_&&grant_->current();}
    bool scopeBorrowed()const noexcept{return grant_&&grant_->stillBorrowingGarageUiLightsOwner(this);}
    bool payloadCurrent()const noexcept{return grant_&&grant_->managerUiLightsPayloadRetirementCurrent(this);}
    bool parseDescriptor1CDCF0(std::size_t,Diagnostic&)noexcept;
    bool constructLight146720(std::size_t,Diagnostic&)noexcept;
    bool configureLight146F60(std::size_t,Diagnostic&)noexcept;
    bool prepare1CDB10(std::shared_ptr<Grant>,std::shared_ptr<Search>,std::shared_ptr<Manager>,
        OriginalUiGarageModelResources50KG&,std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,Diagnostic&)noexcept;
    bool retireLight146980(std::size_t,Diagnostic&)noexcept;
    bool retireManagerDescriptors1CCB20(Diagnostic&)noexcept;
};
}
