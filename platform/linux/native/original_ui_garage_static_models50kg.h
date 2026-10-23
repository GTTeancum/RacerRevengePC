#pragma once
// Concrete Garage Scene3C4 owner. Descriptor publication and live model
// population are separate source stages, neither is Page/render admission.
#include "original_ui_garage_visibility50kg.h"
#include <array>
#include <memory>
namespace rr::platform {
class OriginalUiGaragePageOwner50KG;
class OriginalUiGarageStaticModels50KG final {
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGarageSceneCollision50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageActors50GN;
    friend class OriginalUiGarageVisibility50KG;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using C=game::OriginalWreckComponent50LA;
    using K=game::OriginalWreckComponentKind50LA;
    using R=OriginalUiGarageModelResource50KG;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using S=assets::OriginalStandardMaterialStatus50LA;
    static constexpr std::size_t count=assets::originalUiGarageStaticModelCount50KG;
    struct Descriptor {
        const assets::OriginalUiGarageStaticModel50KG* source=nullptr;
        std::array<float,3> position110{},look120{0,0,1},up130{0,1,0};
        std::array<float,3> smokeOffset150{},smokeVelocity160{};
        bool useLighting140=true,useDynamicLighting144=false,isSmoker14C=false;
        std::uint32_t field148=1;
        std::string modelName180;
    };
    struct Wrapper {
        std::uint32_t type00=1; //Standard3D26F0, not PhysicsObject.
        R* model8=nullptr;
        std::unique_ptr<Descriptor> descriptor10;
    };
    OriginalUiGarageStaticModels50KG()noexcept=default;
    std::shared_ptr<Grant> grant_;
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets_;
    std::shared_ptr<OriginalUiGarageModelResources50KG> resources_;
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility_;
    OriginalUiGaragePodScene50KG* sceneOwner_=nullptr;
    C list3C4_;
    std::uint32_t count30_=0;
    std::unique_ptr<Wrapper*[]> entries34_;
    std::array<std::unique_ptr<Wrapper>,count> wrappers_{};
    std::array<std::shared_ptr<R>,count> models_{};
    std::array<bool,count> enrolled_{},attached_{};
    std::size_t populated_=0;
    bool attempted_=false,listLive_=false,descriptorsComplete_=false;
    bool populationAttempted_=false,populationComplete_=false,retiring_=false,closed_=false;
    bool environmentNodeVisible_=true; //275EF4 default, not caller readiness.
    bool prepare2710F0(OriginalUiGaragePodScene50KG&,std::shared_ptr<Grant>,
        std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,
        std::shared_ptr<OriginalUiGarageModelResources50KG>,D&)noexcept;
    bool populate274160(std::shared_ptr<OriginalUiGarageVisibility50KG>,D&)noexcept;
    bool descriptorIdentityCurrent()const noexcept;
    bool populationCurrent()const noexcept;
    // Borrow of the actual counted3C4 Entity only after all30 real children.
    // It deliberately remains detached until the genuine Page1B48A0 stage.
    C* pageEntity3C4()noexcept;
    bool belongsTo(const OriginalUiGaragePodScene50KG&,const std::shared_ptr<Grant>&,
        const std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>&)const noexcept;
    // Detached-custody cleanup only. Page attachment requires its own typed
    // retirement predecessor and cannot silently enter this private path.
    bool closeCold142590(D&)noexcept;
    enum class PageRetirement {none,queries,models,listReference,wrappers,base,complete};
    PageRetirement pageRetirement_=PageRetirement::none;
    const OriginalUiGaragePageOwner50KG* pageRetirementOwner_=nullptr;
    // Actual ordinal5 callback after raw3CC/ordinal6. No detached close and
    //no observer281490 removal substitute for the normal destructor.
    bool retirePage27B650(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool failedPageRetirement_=false;
    bool retireFailedPageStatic27B650(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retirePageBody27B650(const OriginalUiGaragePageOwner50KG&,bool failedPrefix,D&)noexcept;


public:
    ~OriginalUiGarageStaticModels50KG()noexcept;
    OriginalUiGarageStaticModels50KG(const OriginalUiGarageStaticModels50KG&)=delete;
    OriginalUiGarageStaticModels50KG& operator=(const OriginalUiGarageStaticModels50KG&)=delete;
};
}
