#pragma once
#include "original_ui_pages50kg.h"
#include "import_texture48e.h"
#include <array>
#include <memory>

namespace rr::game {
struct OriginalUiParticleFrame50KG {
    // Raw source coordinates, TL,TR,BR,BL; no added texel or atlas clamping.
    std::array<std::array<float,2>,4> sourceUv{};
};
struct OriginalUiParticleGroup50KG { std::int32_t type=0,first=0,last=0; };
struct OriginalUiMainFireDefinition50KG {
    std::int32_t startingZone=0,type=0,instances=0,particleGroup=0;
    std::array<float,3> position{},velocity{},minRange{},maxRange{};
    float lifeMilliseconds=0,initialSize=0,growth=0,ratePerSecond=0,ageAtBirth=0;
    float framesPerSet=0;
    bool startOn=false,authoredToyAdditive=false,fades=false,additive=false;
    bool hasVelocity=false,grows=false,fadeNonLinear=false,glows=false;
    bool randomMirrorX=false,randomMirrorY=false;
    // Runtime defaults actually read by 15AEA0/15E6D0 when omitted in mmfire:
    bool gravity=false,lit=true,applySpreadToVelocity=true;
    std::int32_t emitMode=0,forcedSetCount=0;
    float delaySeconds=0;
    std::uint32_t sourceParticleFlags=0;
};
struct OriginalUiMainSceneDefinition50KG {
    std::array<float,3> fogColor{};
    float fogVisibility=0,fogHaziness=0;
    std::array<float,3> ambientColor{},directionalColor{},lightPosition{},lightTarget{};
    std::int32_t staticModelCount=0,sceneCameraCount=0,animationCount=0;
    OriginalUiMainFireDefinition50KG fire;
    std::array<OriginalUiParticleGroup50KG,8> groups{};
    std::array<OriginalUiParticleFrame50KG,104> frames{};
};
enum class OriginalUiMainSceneStatus50KG : std::uint8_t {
    none,alreadyPrepared,archive,authoredData,unsupported,texture,allocation
};
struct OriginalUiMainSceneDiagnostic50KG {
    OriginalUiMainSceneStatus50KG status=OriginalUiMainSceneStatus50KG::none;
    const char* member=nullptr;
    assets::OriginalUiPageDiagnostic50KG page{};
    assets::DecodeError48C texture{};
};
// Cold immutable owner of the actual Main SCN/TOY/PRT/EMT/PSM and decoded PST.
// There are ZERO authored static meshes, CSCameras, or animations in this scene.
// This is preparation, not a scene scheduler or proof of completed rendering.
// Retained DATs preserve all authoring, including inactive PRT groups whose UVs
// legitimately lie outside this small Main-only128x256 atlas. Never clamp them.
class OriginalUiMainSceneOwner50KG final {
public:
    OriginalUiMainSceneOwner50KG() noexcept;
    ~OriginalUiMainSceneOwner50KG();
    OriginalUiMainSceneOwner50KG(const OriginalUiMainSceneOwner50KG&)=delete;
    OriginalUiMainSceneOwner50KG& operator=(const OriginalUiMainSceneOwner50KG&)=delete;
    bool prepare(const assets::OriginalUiArchive50KG&,
        OriginalUiMainSceneDiagnostic50KG&) noexcept;
    bool prepared() const noexcept;
    const OriginalUiMainSceneDefinition50KG* definition() const noexcept;
    const assets::Texture48E* particleTexture() const noexcept;
    const assets::OriginalUiDat50KG* particleMaterial() const noexcept;
    const assets::OriginalUiDat50KG* sceneAuthoring() const noexcept;
    const assets::OriginalUiDat50KG* toyAuthoring() const noexcept;
    const assets::OriginalUiDat50KG* emitterAuthoring() const noexcept;
    const assets::OriginalUiDat50KG* atlasAuthoring() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
