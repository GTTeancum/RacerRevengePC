#pragma once
#include <memory>
namespace rr::platform {class VehicleWindowGl50DS;struct VehicleWindowFrame50DS;}
namespace original_main_entry50KG {
struct Application;
// Scoped accepted native Race render-tail contribution. Not source mode5 or
// App0C==264-1. The real Frame must remain unmoved and outlive this scope.
class NativeResultsFrame50LA final {
    friend struct Application;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    NativeResultsFrame50LA();
public:
    NativeResultsFrame50LA(const NativeResultsFrame50LA&)=delete;
    NativeResultsFrame50LA& operator=(const NativeResultsFrame50LA&)=delete;
    ~NativeResultsFrame50LA()noexcept;
    bool current(const rr::platform::VehicleWindowGl50DS&,
        const rr::platform::VehicleWindowFrame50DS&)const noexcept;
};
}
