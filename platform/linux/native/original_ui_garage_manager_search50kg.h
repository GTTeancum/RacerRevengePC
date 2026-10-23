#pragma once
#include "original_ui_garage_selected_model50kg.h"
#include "original_ui_search_list_cold50kg.h"
#include "../../../src/game/original_frontend_camera_cold50la.h"
namespace rr::platform {
// Genuine manager2A0/2A4 ownership with exact Page Camera mode2 membership.
// List history survives Camera removal until the actual manager payload tail.
class OriginalUiGarageManagerSearch50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageUiLights50KG;
    using C=game::OriginalWreckComponent50LA;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using List=OriginalUiSearchListCold50KG;
public:
    enum class Status : std::uint8_t {complete,input,phase,allocation,selected,list,scope};
    struct Diagnostic {Status status=Status::complete;std::uint32_t sourcePC=0;};
    struct Observation {
        const C* node2A4=nullptr;const C* selectedPredecessor=nullptr;
        const List* list2A0=nullptr;const List* first34=nullptr;const List* second38=nullptr;
        List::Snapshot list{};
        bool attempted=false,nodeLive=false,nodeRetired=false,payloadReleased=false,complete=false;
    };
    OriginalUiGarageManagerSearch50KG()noexcept=default;
    ~OriginalUiGarageManagerSearch50KG()noexcept;
    OriginalUiGarageManagerSearch50KG(const OriginalUiGarageManagerSearch50KG&)=delete;
    OriginalUiGarageManagerSearch50KG& operator=(const OriginalUiGarageManagerSearch50KG&)=delete;
    bool current()const noexcept;
    Observation inspect()const noexcept;
private:
    std::shared_ptr<Grant> grant_;
    std::shared_ptr<game::OriginalApplicationEntities50LA> application_;
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::shared_ptr<OriginalUiGarageSelectedModel50KG> selected_;
    const C* selectedEntity_=nullptr;
    std::unique_ptr<List> list_;
    List* manager2A0_=nullptr;
    C node_;
    // Only explicit27F140..15C writes. Untouched vectors/matrices do not exist
    // in this representation and cannot supply bounds/collection readiness.
    const void* field30_=nullptr;
    List* first34_=nullptr;List* second38_=nullptr;
    const void* field3C_=nullptr;const void* field40_=nullptr;
    const void* field44_=nullptr;const void* field48_=nullptr;const void* fieldA0_=nullptr;
    bool attempted_=false,bound_=false,selectedBorrow_=false,nodeLive_=false,nodeRetiring_=false;
    bool nodeRetired_=false,complete_=false,payloadReleased_=false;
    static bool fail(Diagnostic& d,Status s,std::uint32_t pc)noexcept{d={s,pc};return false;}
    struct CameraObserver {
        const void* next00=nullptr;
        const C* child04=nullptr;
        const List* method08=nullptr;
        std::uint32_t zero0C=0,zero10=0,slot14=0;
        std::array<std::uint32_t,4> previousPosition20{};
        bool sharedConstructed=false,added=false,complete=false;
    };
    std::shared_ptr<game::OriginalFrontendCameraCold50LA> camera_;
    std::unique_ptr<CameraObserver> cameraObserver_;
    bool cameraInputCurrent(const std::shared_ptr<game::OriginalFrontendCameraCold50LA>&)const noexcept;
    bool cameraRegistrationCurrent()const noexcept;
    bool registerCamera114770(std::shared_ptr<game::OriginalFrontendCameraCold50LA>,Diagnostic&)noexcept;
    bool unregisterCamera1148D0(const std::shared_ptr<game::OriginalFrontendCameraCold50LA>&,Diagnostic&)noexcept;
    bool emptyFields()const noexcept;
    bool prepare1CCD14(std::shared_ptr<Grant>,std::shared_ptr<OriginalUiGarageSelectedModel50KG>,Diagnostic&)noexcept;
    bool retireNode27F180(Diagnostic&)noexcept;
    // Session must begin the actual manager ref0 stage first, then call this
    // actual payload release, then finish its Entity count. No full-close API.
    bool retireManagerList1CCADC(Diagnostic&)noexcept;
};
}
