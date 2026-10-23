#include "vehicle_window_gl50ds.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_light_manager_owner50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include <algorithm>
#include <cmath>
#include <thread>

namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::captureModelContext24BA50(
    OriginalUiGarageModelLowering50KG::Context& out,
    assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    using S=assets::OriginalStandardMaterialStatus50LA;
    d={};const auto fail=[&](std::uint32_t pc){d.status=S::phase;d.sourcePC=pc;return false;};
    // The final FIFO owner sets this phase/PC only at its actual24B890 call.
    // Generic Camera current, a fabricated public context or an accepted CPU
    // record is not authority to execute this producer.
    if(!identityCurrent()||phase_!=Phase::collectorDrain24B7B0||sourcePC_!=0x24ba50u||
       !publication_||!renderer_||!renderer_->currentFor(issuer_)||
       !renderer_->ownsRenderPrelude(*this,publication_)||!cameraFrame_.current())
        return fail(0x24b890u);
    const auto fifo=issuer_->nativeCollectorFifoOwner_;
    if(!fifo||fifo->application_!=issuer_||fifo->closed_||!fifo->draining_||!fifo->uiConstructor1B73BC_||
       fifo->ownerThread_!=std::this_thread::get_id())return fail(0x24b890u);
    // Empty is the real24B8D4 branch, not a successful empty frame. Nonempty
    // rows must all belong to this same actual publication; racing records or
    // another Garage cursor cannot borrow its context. No pins are released.
    for(const auto& record:fifo->records_)
        if(record.garageDraw.get()!=this||record.garagePublication!=publication_||record.epoch!=fifo->epoch04_)
            return fail(0x24b890u);
    OriginalUiGarageCameraFrame50KG::Fields camera;std::uint32_t pending=0;
    if(!cameraFrame_.read(camera,pending))return fail(pending?pending:0x24bbacu);
    const auto& display=*renderer_->display_;
    if(!display.sourceCurrentCamera0Known_||display.sourceSelectedCamera0_!=camera_||
       !display.sourceFogRangeKnown_||!std::isfinite(display.sourceFogEndB4_)||
       !std::isfinite(display.sourceFogScaleB8_))return fail(0x24be38u);
    if((sourceXyOffset_[0]&15u)||(sourceXyOffset_[1]&15u))return fail(0x243d40u);
    OriginalUiGarageModelLowering50KG::Context next;
    next.sourceXyOffset={{sourceXyOffset_[0]>>4u,sourceXyOffset_[1]>>4u}};
    // Existing native packet transport is specifically source640x448. Do not
    // silently clamp/offset an unsupported source viewport into that target.
    if(next.sourceXyOffset!=std::array<std::uint32_t,2>{{1728u,1824u}})
        return fail(0x243d40u);
    next.camera.viewC0=camera.viewC0;next.camera.projection80=camera.projection80;
    next.camera.clipProjection290=camera.guardProjection290;next.camera.screen140=camera.screen140;
    std::copy_n(camera.position180.begin(),3,next.camera.cameraPosition180.begin());
    next.guardX334=camera.guardX334;next.guardY338=camera.guardY338;
    next.camera.canvasOffset={{static_cast<float>(next.sourceXyOffset[0]),static_cast<float>(next.sourceXyOffset[1])}};
    //24BE38/68 copy B4/B8 directly. Never replace this with B0, a new slope
    // calculation, authored Fog fields or a second global fog owner.
    next.camera.fogEnd33Z=display.sourceFogEndB4_;next.camera.fogSlope33W=display.sourceFogScaleB8_;
    const auto& manager=issuer_->sourceLightManager426F60Owner_;
    if(!manager)return fail(0x24bd54u);
    game::original_racer_lighting50KM::LightMatrices lightRows;
    OriginalUiLightManagerOwner50KG::Diagnostic light;
    if(!manager->readGarageGlobalLightRows1499A0(*issuer_,lightRows,light))
        return fail(light.sourcePC?light.sourcePC:0x24bd7cu);
    next.globalLights1499A0=lightRows;
    out=next;d.status=S::complete;d.sourcePC=0x24be78u;return true;
}
}
