#pragma once
#include "material_texture_gl50eb.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include <array>
#include <vector>
namespace rr::platform {
class OriginalUiRiderSession50KG;
class OriginalUiRiderDrawContribution50KG;
class OriginalUiFrontend50KG;
class VehicleWindowGl50DS;
// Private source-owner material closure. No public mask constructor or setter.
class OriginalUiRiderResidencyDemand50KG final {
    friend class OriginalUiRiderSession50KG;
    friend class OriginalUiRiderDrawContribution50KG;
    friend class OriginalUiRiderResidentPool50KG;
    friend class VehicleWindowGl50DS;
    const OriginalUiRiderSession50KG* session_=nullptr;
    std::uint64_t serial_=0;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank_;
    std::vector<std::uint32_t> indices_;
    bool entry_=false,consumed_=false;
    bool requiresCoverage_=false; // Derived only from validated closure prototypes.
public:
    OriginalUiRiderResidencyDemand50KG()=default;
    const OriginalUiRiderSession50KG* session()const noexcept{return session_;}
    std::uint64_t serial()const noexcept{return serial_;}
    const std::shared_ptr<const assets::OriginalUiImageBank50KG>& bank()const noexcept{return bank_;}
    bool includes(std::uint32_t i)const noexcept {
        for(auto value:indices_)if(value==i)return true;return false;
    }
    std::uint32_t logicalCount()const noexcept {
        return bank_&&bank_->images()&&bank_->images()->textures()?bank_->images()->textures()->textureCount():0;
    }
};
// Session-scoped native GL storage. Every live name is charged, including names
// retained after failed staging/deletion. Destruction never silently deletes GL.
class OriginalUiRiderResidentPool50KG final {
    friend class OriginalUiRiderSession50KG;
    friend class OriginalUiFrontend50KG;
    friend class MaterialTextureGl50EB;
    struct Storage;std::unique_ptr<Storage> storage_;
    explicit OriginalUiRiderResidentPool50KG(const OriginalUiRiderSession50KG&,
        std::shared_ptr<const assets::OriginalUiImageBank50KG>);
    bool stage(const std::shared_ptr<OriginalUiRiderResidentPool50KG>& self,
        const OriginalUiRiderResidencyDemand50KG&,const MaterialTextureGlRequest50EB&,
        MaterialTextureGl50EB&,MaterialTextureGlDiagnostic50EB&) noexcept;
    bool collect(MaterialTextureGlDiagnostic50EB&) noexcept;
    bool close(MaterialTextureGlDiagnostic50EB&) noexcept;
    bool dropPublication(MaterialTextureGl50EB&,MaterialTextureGlDiagnostic50EB&) noexcept;
public:
    ~OriginalUiRiderResidentPool50KG() noexcept;
    OriginalUiRiderResidentPool50KG(const OriginalUiRiderResidentPool50KG&)=delete;
    OriginalUiRiderResidentPool50KG& operator=(const OriginalUiRiderResidentPool50KG&)=delete;
    std::size_t liveGpuBytes()const noexcept;
    std::uint32_t liveImageCount()const noexcept;
    bool faulted()const noexcept;
};
// Staging may allocate private renderer resources required by this source
// closure, but cannot publish a live binding, frame, or source receipt. Only
// the same frontend requests validation/commit; session validates its plan.
class OriginalUiRiderTextureValidation50KG final {
    friend class VehicleWindowGl50DS;
    const VehicleWindowGl50DS* renderer_=nullptr;
    const OriginalUiRiderSession50KG* session_=nullptr;
    const assets::OriginalUiImageBank50KG* bank_=nullptr;
    const MaterialTextureGl50EB* gpu_=nullptr;
    std::uint64_t serial_=0,token_=0,rendererEpoch_=0;
    std::array<std::uint8_t,assets::maxMaterialTextureAssets50EA> maximumAlpha_{};
    bool ready_=false;
    bool requiresCoverage_=false;
public:
    OriginalUiRiderTextureValidation50KG()=default;
};
}
