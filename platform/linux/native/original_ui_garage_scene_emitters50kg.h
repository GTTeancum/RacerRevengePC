#pragma once
#include "original_ui_garage_model_resource50kg.h"
#include "../../../src/game/original_ui_garage_particles50kg.h"
#include <array>
#include <memory>
#include <optional>
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
// Concrete27DFD0/27C7B0 lifetime and genuine27DFC0 update prefix. Existing
//ToyFields owns ParticleList/wrappers; SceneParticles owns the ONE pool/atlas.
//Private Page dispatch borrows the actual renderer, never a ready callback.
//No public complete-Scene or runtime admission capability is issued.
class OriginalUiGarageSceneEmitters50KG final {
    friend class OriginalUiGaragePodScene50KG;
    using C=game::OriginalWreckComponent50LA;
    struct Row final {
        C entity; // The actual typed15B860/15DE00/15E800/15F100 subobject.
        C* manager70=nullptr; // Borrow of actual Scene4A0 Entity, no AddRef.
        const game::OriginalRacerSceneObject50LA* attachment74=nullptr;
        // Native pin/view of the ACTUAL actor model. No copied graph, new
        //source reference, guessed PSG index or detached test fixture.
        std::shared_ptr<OriginalUiGarageModelResource50KG> modelPin;
        game::SceneGraph49P graphView{};
        game::SceneNodeIndex49P attachmentNode=game::SceneNodeIndex49P::none;
        std::uint32_t authenticatedSourceRow=0;
        std::array<char,260> filename78; // Only source-produced bytes through NUL.
        std::size_t filenameKnown=0;
        bool entityLive=false,numericConstructed=false,loaded=false,attached=false;
        bool retiring=false,retired=false;
    };
    std::array<std::unique_ptr<Row>,7> rows;
    //27C8F8 etc allocate a local array BEFORE derived construction, then
    //publish wrapper00 only after the constructor loop. Retain failed prefix.
    std::unique_ptr<C*[]> pendingPointerArray;
    std::size_t completed=0;
    bool complete=false,failed=false,retired=false;
    bool listBound=false,listRetiring=false,listRetired=false,payloadReleased=false;
    std::uint32_t stoppedPC=0;
    //Native custody of the actual first27DFC0 invocation, not Source fields.
    std::uint64_t updatePublication=0;
    C* updateChild=nullptr;std::size_t updatedRows=0;
    std::uint32_t updateSourcePC=0;
    bool updateReturned=false,updateFailed=false;
public:
    ~OriginalUiGarageSceneEmitters50KG()noexcept;
};
}
