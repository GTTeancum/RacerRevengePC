#pragma once
#include "original_ui_menu_warning_packet50kg.h"
#include "original_ui_garage_screen_text50kg.h"
#include "../../../src/assets/original_standard_material_domain50la.h"
#include <cstddef>
#include "../../../src/assets/native_asset48b.h"
namespace original_main_entry50KG {struct Video;class NativeCollectorFifo50LA;}
namespace rr::platform {
class OriginalUiGaragePageOwner50KG;
// Canonical embedded2DEF40, real2461E0 material reference and aligned2DF0B0
// packet backing. No counted Entity, native collector or draw-ready claim.
class OriginalUiGarageQuad50KG final {
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class OriginalUiSharedDialogs50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    using Root=::original_main_entry50KG::Application;
    using Video=::original_main_entry50KG::Video;
    using Domain=assets::OriginalStandardMaterialDomain50LA;
    using Context=assets::OriginalStandardMaterialContext50LA;
    using Archive=assets::OriginalUiArchive50KG;
public:
    enum class Status:std::uint8_t {complete,input,phase,allocation,material,missingPsmFallbackPending,texture,arithmetic,rendererRetirementPending};
    enum class Stage:std::uint8_t {empty,constructed,primitivePublished,materialRetained,populated,packetPublished,loaded,failed,retired};
    struct Diagnostic {Status status=Status::complete;std::uint32_t sourcePC=0;bool sourceEffectsCommitted=false;assets::OriginalStandardMaterialDiagnostic50LA material{};};
    OriginalUiGarageQuad50KG()noexcept=default;
    ~OriginalUiGarageQuad50KG()noexcept;
    OriginalUiGarageQuad50KG(const OriginalUiGarageQuad50KG&)=delete;
    OriginalUiGarageQuad50KG& operator=(const OriginalUiGarageQuad50KG&)=delete;
private:
    struct alignas(16) Qword {std::array<std::uint32_t,4> words{};};
    struct PacketDelete {void operator()(Qword*)const noexcept;};
    struct Primitive {
        std::int32_t material00=-1;void* optional04=nullptr;
        std::uint8_t type0C=0,field0D=0,field0E=255,flags0F=0;
        Qword* packet10=nullptr;void* optional14=nullptr;
        float scale18=1,scale1C=1;std::uint16_t field20=0,field22=0;
        bool materialStored=false,materialRetained=false;
        std::shared_ptr<Domain> domain;std::shared_ptr<const Context> context;
    };
    class RootInputs final {
        friend class OriginalUiGaragePageOwner50KG;
        friend class OriginalUiSharedDialogs50KG;
        friend class OriginalUiGarageQuad50KG;
        const Root* issuer_;std::shared_ptr<Domain> domain_;std::shared_ptr<const Context> context_;
        std::shared_ptr<Video> video_;std::shared_ptr<const Archive> garage_;
        const std::string* directory_;
        RootInputs(const Root* root,std::shared_ptr<Domain> domain,std::shared_ptr<const Context> context,
            std::shared_ptr<Video> video,std::shared_ptr<const Archive> garage,const std::string* directory)noexcept
            :issuer_(root),domain_(std::move(domain)),context_(std::move(context)),video_(std::move(video)),
             garage_(std::move(garage)),directory_(directory){}
    };
    std::array<float,4> position10{},size20{},uv30{};
    float field40=0,scale4C=0,scale50=0;std::uint32_t color44=0;
    std::uint16_t alpha48=0;std::uint8_t flags5CValue=0,flags5CKnownMask=0;
    Primitive* material54=nullptr;Qword* resource58=nullptr;
    std::uint32_t offset=0;bool constructed=false;
    std::unique_ptr<Primitive> primitiveOwner_;
    std::unique_ptr<Qword[],PacketDelete> packetOwner_;
    Qword* dmaData4_=nullptr; //Actual native pointer, never a guessed32-bit guest word.
    std::shared_ptr<Domain> domain_;std::shared_ptr<const Context> context_;
    std::shared_ptr<Video> video_;std::shared_ptr<const Archive> garage_;
    Stage stage_=Stage::empty;std::uint32_t pendingPC_=0;
    static bool inputsCurrent(const RootInputs&)noexcept;
    static bool retainPrimitive2465B0(const RootInputs&,Primitive&,const char*,Diagnostic&)noexcept;
    static bool loadAperturePrimitive2E20F0(const RootInputs&,std::unique_ptr<Primitive>&,Diagnostic&)noexcept;
    bool construct2DEF40(std::uint32_t)noexcept;
    bool load2DF0B0(const RootInputs&,Diagnostic&)noexcept;
    // Queued records borrow the actual Primitive and packet. Mutation and
    // source retirement wait for genuine105990 release of these same pins.
    std::uint32_t nativeGarageFifoPins_=0;
    std::uint32_t nativeMenuWarningPins_=0; // Private Shared menu receipts, not FIFO.
    std::uint64_t packetRevision_=0;
    bool packetCurrent2DF550()const noexcept;
    bool canMutatePacket2DF550()const noexcept;
    bool draw2DF550(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool drawWarning4702DF550(OriginalUiGarageDrawContribution50KG&,OriginalUiSharedDialogs50KG&,Diagnostic&)noexcept;
    bool drawMenuWarning4702DF550(OriginalUiSharedDialogs50KG&,Diagnostic&)noexcept;
    static void initializeDrawAssertion2DF550()noexcept;
    bool regeneratePacket2DF550(std::uint32_t,std::uint32_t,Diagnostic&)noexcept;
    struct LoweredPacket327E90 {
        std::array<assets::Vertex48B,8> vertices{};
        std::array<std::array<float,3>,8> stq{};
        std::array<float,8> rasterW{},fog{};
        std::uint32_t quadCount=0;
    };
    bool lowerPacket327E90(std::uint64_t expectedRevision,
        const std::array<std::uint32_t,2>& actualFixed4XyOffset,
        LoweredPacket327E90&,Diagnostic&)const noexcept;
    bool retainMenuWarningPacket(OriginalUiSharedDialogs50KG&,
        OriginalUiMenuWarningPacket50KG&,Diagnostic&)noexcept;
    bool releaseMenuWarningPacket(OriginalUiMenuWarningPacket50KG&)noexcept;
    bool lowerMenuWarningPacket(const OriginalUiSharedDialogs50KG&,
        const OriginalUiMenuWarningPacket50KG&,const std::array<std::uint32_t,2>&,
        OriginalUiMenuWarningGeometry50KG&,Diagnostic&)const noexcept;
    bool menuWarningPacketMatches(const OriginalUiSharedDialogs50KG&,
        const OriginalUiMenuWarningPacket50KG&)const noexcept;
    bool lowerRetainedPacketBody327E90(std::uint64_t,const std::array<std::uint32_t,2>&,
        LoweredPacket327E90&,Diagnostic&)const noexcept;
    std::uint8_t retirementDrainPhase_=0;
    bool demand1FDD60(Diagnostic&)noexcept;
    bool preflightRetirement2DEFE0(Diagnostic&)const noexcept;
    static bool preflightPrimitive246260(const std::unique_ptr<Primitive>&,Diagnostic&)noexcept;
    static void releasePrimitive246260(std::unique_ptr<Primitive>&)noexcept;
    bool retireResources2DF038(Diagnostic&)noexcept;
    bool fail(Diagnostic&,Status,std::uint32_t)noexcept;
    static const char* authoredName(std::uint32_t)noexcept;
};
}
