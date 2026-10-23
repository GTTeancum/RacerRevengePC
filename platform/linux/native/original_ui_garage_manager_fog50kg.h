#pragma once
#include "original_ui_garage_ui_lights50kg.h"
#include "original_ui_garage_fog_color_lease50kg.h"
#include "original_ui_garage_fog_packet50kg.h"
namespace rr::platform {
// Source1CCFA0 owns three actual counted Entities and helper5C. Source1CD080
// appends Fog0, On2, Off1 only after the real22-slot widget loop returns.
class OriginalUiGarageManagerFog50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageWidget1Owner50KG;
    friend class OriginalUiGarageFogColorLease50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    using C=game::OriginalWreckComponent50LA;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using Lights=OriginalUiGarageUiLights50KG;
    using Lease=OriginalUiGarageFogColorLease50KG;
    using Widget=OriginalUiGarageWidget1Owner50KG;
public:
    enum class Status:std::uint8_t {complete,input,source,phase,allocation,scope,renderer,arithmetic};
    enum class Stage:std::uint8_t {empty,bound,descriptor,fog,helper,awaitingRenderer,fogPublished,offPublished,complete,retired,attaching,attached};
    struct Diagnostic {Status status=Status::complete;std::uint32_t sourcePC=0;assets::OriginalUiPageDiagnostic50KG parse{};};
    struct Observation {
        Stage stage=Stage::empty;std::array<const C*,3> live{};
        std::array<bool,3> attached{},retired{};std::size_t attachmentCursor=0;
        std::array<std::uintptr_t,3> managerFieldIdentities{};
        bool descriptorAllocated=false,descriptorParsed=false,constructorKnown=false,rangeKnown=false;
        bool helperAllocated=false,helperEmpty=false,rendererWritePerformed=false,payloadReleased=false;
        std::uint32_t color30=0,field44=0,field48=0,field4C=0,dirty60=0;
        float start34=0,end38=0,visibility3C=0,haziness40=0,far50=0,near54=0,gap58=0;
        std::uintptr_t fogOnBorrow30=0;bool fogOnBorrowKnown=false;
    };
    OriginalUiGarageManagerFog50KG()noexcept=default;
    ~OriginalUiGarageManagerFog50KG()noexcept;
    OriginalUiGarageManagerFog50KG(const OriginalUiGarageManagerFog50KG&)=delete;
    OriginalUiGarageManagerFog50KG& operator=(const OriginalUiGarageManagerFog50KG&)=delete;
    bool current()const noexcept;
    // Actual1CD080 return, not Page/Scene readiness or Garage admission.
    bool currentAttached()const noexcept;
    Observation inspect()const noexcept;
private:
    struct Descriptor {std::uint32_t rgb00=0;float visibility04=0,haziness08=0;};
    //133F80 writes only20/24/28/2C. No source value is assigned to00..1F.
    struct Primitive {void* buffer20=nullptr;std::uint32_t count24=0,count28=0;std::int32_t field2C=-1;
        std::unique_ptr<OriginalUiGarageFogPacket50KG> packet;
        bool current(std::uint32_t color)const noexcept {
            return packet?buffer20==packet->words.data()&&count24==128&&count28==13&&field2C==1&&packet->current(color):
                !buffer20&&!count24&&!count28&&field2C==-1;
        }
    };
    struct Fog {C entity;std::uint32_t color30=0;float start34=0,end38=0,visibility3C=0,haziness40=0;
        std::uint32_t field44=0,field48=0,field4C=0;float far50=0,near54=0,gap58=0;
        Primitive* helper5C=nullptr;std::uint32_t dirty60=0;};
    struct Off {C entity;};
    struct On {C entity;const C* fog30=nullptr;}; //34 is unwritten, not observed.
    std::shared_ptr<Grant> grant_;std::shared_ptr<Lights> lights_;
    std::shared_ptr<Lease> renderer_;const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::unique_ptr<Descriptor> descriptor_;std::unique_ptr<Fog> fog_;
    std::unique_ptr<Primitive> helper_;std::unique_ptr<Off> off_;std::unique_ptr<On> on_;
    std::array<C*,3> managerFields_{};std::array<bool,3> constructed_{},attached_{},retiring_{},retired_{};
    std::array<const C*,3> previousAtRetire_{};
    const Widget* widgetOwner_=nullptr;std::size_t attachmentCursor_=0;
    bool renderProxyActive_=false;
    Stage stage_=Stage::empty;bool bound_=false,descriptorParsed_=false,constructorKnown_=false;
    bool helperReleased_=false,rangeKnown_=false,rendererWritePerformed_=false,onBorrowKnown_=false,payloadReleased_=false;
    static bool fail(Diagnostic& d,Status s,std::uint32_t pc)noexcept{d.status=s;d.sourcePC=pc;return false;}
    C* entity(std::size_t)const noexcept;
    bool identityCurrent()const noexcept;
    bool fieldsCurrent()const noexcept;
    bool colorWriteCurrentFor(const ::original_main_entry50KG::Application*)const noexcept;
    bool prepare1CCFA0(std::shared_ptr<Grant>,std::shared_ptr<Lights>,std::shared_ptr<Lease>,Diagnostic&)noexcept;
    bool continue1CCFA0(std::shared_ptr<Lease>,Diagnostic&)noexcept;
    bool attachAfterWidgets1CD080(const Widget&,Diagnostic&)noexcept;
    bool widgetCustodyCurrentFor(const Widget*)const noexcept;
    bool retireOne142590(std::size_t,Diagnostic&)noexcept;
    bool retireAttached1CD080(Diagnostic&)noexcept;
    bool retireDetached1CCFA0(Diagnostic&)noexcept;
};
}
