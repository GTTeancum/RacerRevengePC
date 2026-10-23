#pragma once
#include "../../../src/game/original_wreck_components50la.h"
#include "../../../src/assets/original_ui_pages50kg.h"
#include <array>
#include <memory>
#include <string>
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
class OriginalUiGarageSceneDispatch50KG final {
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGarageSceneCollision50KG;
    using C=game::OriginalWreckComponent50LA;
    struct CollisionDescriptor final {
        // Native representation of the source27A224/27A270 inherited fields.
        std::string stem00;
        std::array<float,3> position110{},look120{0,0,1},up130{0,1,0};
        std::array<float,3> smokeOffset150{},smokeVelocity160{};
        bool useLighting140=true,useDynamicLighting144=false,isSmoker14C=false;
        std::uint32_t field148=1;
        std::int32_t field180=0,material184=1;
    };
    struct CollisionWrapper final {
        std::uint32_t type00=1;
        // Source live collision+4 remains absent until later1B5590.
        const void* liveCollision04=nullptr;
        std::unique_ptr<CollisionDescriptor> temporary08;
    };
    struct CollisionList final {
        C entity;
        std::uint32_t count30=0;
        std::unique_ptr<CollisionWrapper*[]> records34;
        std::array<std::unique_ptr<CollisionWrapper>,30> records;
        // Native retirement receipts only.27BB80 preserves source count30
        // and dying Entity links; its wrapper payload is not a query owner.
        bool live=false,complete=false,retiring=false,retired=false;
    };
    std::unique_ptr<CollisionList*[]> collisionLists3CC;
    std::unique_ptr<CollisionList> collisionZone;
    struct TerrainRow final {
        float friction00=1,drag04=1,traction08=1;
        std::int32_t dust0C=1,dirt10=1;
    };
    struct TerrainInfo final {
        std::uint32_t count00=0;
        std::array<TerrainRow,8> rows04{};
    };
    std::unique_ptr<TerrainInfo> terrain3B0;
    float ambientMinIdle368=0;
    bool ambientFlash390=false;
    bool collisionAttempted=false,sceneComplete=false,terrainComplete=false;
    bool closed=false;
    OriginalUiGarageSceneDispatch50KG()noexcept=default;
public:
    ~OriginalUiGarageSceneDispatch50KG()noexcept;
    OriginalUiGarageSceneDispatch50KG(const OriginalUiGarageSceneDispatch50KG&)=delete;
    OriginalUiGarageSceneDispatch50KG& operator=(const OriginalUiGarageSceneDispatch50KG&)=delete;
};
}
