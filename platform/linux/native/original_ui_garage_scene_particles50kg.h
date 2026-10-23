#pragma once
#include "../../../src/game/original_ui_garage_particles50kg.h"
#include "../../../src/game/original_wreck_components50la.h"
#include "original_ui_garage_particle_packet_geometry50kg.h"
#include "../../../src/assets/original_standard_particle_material_reference50la.h"
#include <array>
#include <memory>
#include <optional>
#include <vector>
namespace original_main_entry50KG {class NativeCollectorFifo50LA;}
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
// Actual Scene4A0 native owner. Numeric emitter construction is deliberately
//later than this160E00 manager prefix. No renderer readiness is inferred.
class OriginalUiGarageSceneParticles50KG final {
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    using C=game::OriginalWreckComponent50LA;
    using Vec=std::array<float,4>;
    struct Primitive final {
        struct alignas(16) Buffer {std::array<std::uint32_t,128> words;};
        std::unique_ptr<Buffer> ownedBuffer20;
        bool released133FA0=false;
        const std::uint32_t* nativePayload04=nullptr; //Native pointer-width DMA relocation.
        std::array<std::optional<std::uint32_t>,8> unwritten00To1C;
        void* buffer20=nullptr;std::uint32_t count24=0,count28=0;std::int32_t field2C=-1;
    };
    struct Auxiliary final {
        std::uint64_t reference00=1;
        //The actual live enabled18 is the numeric manager's retained child,
        //bound only after its single pool exists. Other constructor state is
        //retained here, not a second enabled flag or a fake null helper.
        const game::OriginalGcParticleAuxiliaryChild50KG* numericChild=nullptr;
        std::uint32_t field10=0,field14=0,field1C=0,field24=0,field12C=0,field134=0,field138=0;
        float field20=0,minimum28=0,maximum2C=190;
        Vec position30{{0,-20,0,1}},color40{{128,128,128,128}};
        Vec position50{{0,0,0,1}},scale60{{1,1,0,1}};
        Vec inverseDimensions70{},field90{{-100000,-100000,-100000,1}};
        std::uint32_t fieldA0=0,fieldA4=0,fieldAC=0;
        Primitive primitiveB0,primitiveE0;
        std::array<std::optional<std::uint32_t>,4> unwritten110To11C;
        std::uint32_t field120=0,field124=0,field128=0;
    };
    struct DepthBucket {
        std::uint32_t epoch00=0;
        const game::OriginalGcParticle50KG* head04=nullptr;
    };
    struct ScratchDelete {void operator()(std::uint8_t*)const noexcept;};
    struct PacketOrigin {
        std::uint16_t poolSlot=0,frame=0,depthBucket=0;
    };
    struct PacketBorrow {
        const Primitive* primitive160=nullptr;
        const assets::OriginalStandardParticleMaterialReference50LA* material150=nullptr;
        const std::uint32_t* constants=nullptr;
        std::uint32_t constantWordCount=0,constantQwordCount=0;
        const std::uint8_t* scratch15C=nullptr;
        const std::uint8_t* scratchPayload04=nullptr;
        std::size_t scratchCapacity=0,scratchUsedBytes=0;
        const PacketOrigin* origins=nullptr;
        std::uint32_t particleCount=0;
        std::uint64_t publication=0,revision=0;
    };
    bool borrowPacket162E50(PacketBorrow&)const noexcept;
    bool lowerPacket3212C8(const game::OriginalUiMainParticleCamera50KG&,
        const game::OriginalUiMainParticleRenderConstants50KG&,
        const std::array<float,3>& actualCameraPosition180,
        OriginalUiGarageParticlePacketGeometry50KG&,
        assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    C entity;
    std::unique_ptr<Auxiliary> auxiliary1C0;
    std::vector<DepthBucket> buckets11C;
    std::unique_ptr<std::uint8_t,ScratchDelete> scratch15C;
    std::size_t scratchBytes=0,scratchUsedBytes=0;
    const std::uint8_t* scratchPayload04=nullptr; //Pointer source16281C, never a guest integer.
    std::array<PacketOrigin,game::originalGcParticleCapacity50KG> packetOrigins{};
    std::uint32_t nativeGarageFifoPins_=0;
    std::uint64_t packetRevision_=0;
    std::uint32_t packetTerminalPC=0;
    std::size_t scratchPacketCount=0;
    std::unique_ptr<game::OriginalUiGarageParticles50KG> numeric;
    std::shared_ptr<const game::OriginalUiGarageParticleAssets50KG> assetsOwner;
    std::unique_ptr<assets::OriginalStandardParticleMaterialReference50LA> material150;
    Vec position30{{0,0,0,1}},gravity40{{0,-64,0,1}};
    std::array<Vec,4> normals50{};
    //12ACC0 writes nothing.160E00 writes only XYZ of these source records.
    std::array<std::array<float,3>,4> quadPositions90{};
    // Source+0C/+10/+14/+18/+1C are individually unwritten.149310 writes
    // ONLY+10, so a whole optional five-word record would fabricate its peers.
    std::array<std::array<std::optional<std::uint32_t>,5>,4> quadRemainder;
    Primitive primitive160,primitive190;
    std::int32_t capacity130=0,depthCount118=0;
    std::uint32_t used128=0,field12C=0,field134=0,field158=0;
    const void* field14C=nullptr; //Actual same-FIFO native29C0B0 entry, not a material row.
    std::shared_ptr<::original_main_entry50KG::NativeCollectorFifo50LA> leaf14COwner;
    assets::OriginalStandardMaterialCollectorReceipt50LA leaf14CReceipt;
    std::int32_t leaf14CRegistration=-1;
    const assets::OriginalStandardMaterialObject50LA* projectionMaterial=nullptr;
    std::uint32_t epoch114=0; //Actual160C34 constructor store.
    std::optional<std::int32_t> minimum120,maximum124; //Not written by160B10.
    // Native pointer-width backing for the SAME pool's source20 link word.
    // The actual source24 depth remains in the original retained pool itself.
    std::array<const game::OriginalGcParticle50KG*,game::originalGcParticleCapacity50KG> next20{};
    std::array<bool,game::originalGcParticleCapacity50KG> next20Written{};
    std::array<float,16> projectionCamera250{};
    float depthScale=0,inverseDepthScale=0;
    std::size_t projectionRow=0,projectionCount=0;
    std::uint32_t projectionTerminalPC=0;
    std::array<float,16> facingMatrix162CA8{};
    std::array<float,3> particleLightDirection{},particleLightDiffuse{},particleLightAmbient{};
    std::size_t lightingRow=0,lightingAlphaRow=0;
    std::int32_t field144=1;float glowScalar148=255;
    bool entityLive=false,depthWritten=false,prepared=false,retiring=false,retired=false;
    //Invocation custody; never Source fields or render/frame readiness.
    std::uint64_t updatePublication=0;std::uint32_t updateSourcePC=0;
    bool updateReturned=false,updateFailed=false;
    // Exact callback invocation custody, not an empty-pool/renderer flag.
    //162870 may commit14C=0 and then stop on its unsupported positive branch.
    std::uint64_t projectionPublication=0,drawPublication=0;
    std::uint32_t projectionSourcePC=0,drawSourcePC=0;
public:
    ~OriginalUiGarageSceneParticles50KG()noexcept;
};
}
