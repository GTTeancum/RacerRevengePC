#include "vehicle_window_gl50ds_storage.h"
#include "../../../tests/native/ra_static_fire_visual50la.h"
#include "../../../tests/native/rc_crystal50rc.h"
#include "../../../tests/native/tb_original_world50lm.h"
#include "../../../tests/native/rc_particle_visual50rc.h"
#include "../../../tests/native/rc_particle_render50rc.h"
#include "../../../tests/native/gb_particle_visual50gb.h"
#include "../../../tests/native/mc_particle_visual50mc.h"
#include "../../../tests/native/gb_particle_render50gb.h"
#include "../../../tests/native/mc_particle_render50mc.h"
#include "../../../tests/native/ra_cold_aggregate50la.h"
#include "original_video_blocking_native.h"

namespace rr::platform {
namespace {
// Local observation only: no GL calls, logger, test dependency or simulation clock.
class GameplayDisplayWallScope50DS final {
    using Clock = std::chrono::steady_clock;
    VehicleWindowWallSample50DS* sample_ = nullptr;
    Clock::time_point before_{};
public:
    GameplayDisplayWallScope50DS(bool enabled,VehicleWindowWallSample50DS& sample) noexcept {
        if(enabled){sample_=&sample;sample.attempted=true;before_=Clock::now();}
    }
    ~GameplayDisplayWallScope50DS() noexcept {
        if(!sample_)return;
        const auto after=Clock::now();
        if(after<before_)return;
        // Bound before integer conversion, including implementations with a
        // wider steady-clock duration. An invalid sample never changes success.
        const auto ns=std::chrono::duration<long double,std::nano>(after-before_).count();
        if(!(ns>=0 && ns<18446744073709551616.0L))return;
        sample_->wallNs=static_cast<std::uint64_t>(ns);sample_->valid=true;
    }
    GameplayDisplayWallScope50DS(const GameplayDisplayWallScope50DS&)=delete;
    GameplayDisplayWallScope50DS& operator=(const GameplayDisplayWallScope50DS&)=delete;
};
} // namespace

VehicleWindowGl50DS::VehicleWindowGl50DS() noexcept = default;
VehicleWindowGl50DS::~VehicleWindowGl50DS() noexcept {
    VehicleWindowDiagnostic50DS d{};
    close(d);
    if (d.cleanupIncomplete) std::terminate();
}

bool VehicleWindowGl50DS::create(const VehicleWindowRequest50DS& request,
    const assets::VehicleDrawAssets50CR& assets, VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    if(sessionDisplayFrontendCreate50KG_||(sessionDisplayOwner50LA_&&!sessionDisplayCreate50LA_))
        return fail(d,VehicleWindowError50DS::request,"session renderer creation requires its actual display owner");
    if(request.gameplayBinderTextures){
        // Binder uses the shared cable quad buffers and the requested
        // world material shader. RA has its own authenticated world geometry,
        // so it must not supply a fabricated legacy track mesh here.
        if(!request.cables.textures||(!request.raStaticWorld&&
            (request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures||!request.track.mesh))||
            request.gameplayBinderTextures->textureCount()!=10||
            !request.gameplayBinderTextures->validBorrow(request.context,request.isCurrent,
                request.gameplayBinderTextures->publicationToken()))
            return fail(d,VehicleWindowError50DS::request,"Binder needs exact cold bank, world material program and shared geometry storage");
        for(U i=0;i<10;++i)if(!request.gameplayBinderTextures->texture(i))
            return fail(d,VehicleWindowError50DS::material,"Binder stored frame missing");
    }

    if (storage_) return fail(d, VehicleWindowError50DS::alreadyOwned, "renderer already owns resources");
    if (!request.procAddress || !request.isCurrent ||
        (request.mode != VehicleWindowMode50DS::viewerNeutralUntextured &&
            request.mode != VehicleWindowMode50DS::callerMaterialTextures) ||
        (request.output != VehicleWindowOutput50DS::defaultBackBuffer &&
            request.output != VehicleWindowOutput50DS::headless) ||
        request.maximumWidth <= 0 || request.maximumHeight <= 0 ||
        std::uint64_t(request.maximumWidth) * std::uint64_t(request.maximumHeight) >
            std::numeric_limits<std::size_t>::max() /
                (16u + (request.enableHostHud ? 8u : 0u) + (request.enableOriginalUi ? 16u : 0u) +
                 (request.enableOriginalGameplayBlur ? 12u : 0u)))
        return fail(d, VehicleWindowError50DS::request,
            "context callbacks, valid output/vehicle shading modes and positive target bounds required");
    if (!assets.loaded()) return fail(d, VehicleWindowError50DS::assets, "50CR assets are not loaded");
    if (!validateTrack(request, d) || !validateMaterials(request, assets, d) ||
        !validateCables(request, d) || !validateSky(request, d) || !validateDynamicWorld(request, d) ||
        !validateTrackAnimations(request, d)) return false;
    if(request.track.rcParticleWindow && (request.raStaticWorld || request.track.raStaticFire || request.track.gbParticleWindow || request.track.mcParticleWindow ||
        request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures ||
        !request.track.rcParticleWindow->validateTrack(request.track)))
        return fail(d,VehicleWindowError50DS::material,"RC particle completion requires its exact actual world/image/span owner");
    if(request.track.gbParticleWindow && (request.track.mcParticleWindow || request.raStaticWorld || request.track.raStaticFire || request.track.rcParticleWindow || request.track.rcCrystal || request.track.tbWorld ||
        request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures ||
        !request.track.gbParticleWindow->validateTrack(request.track)))
        return fail(d,VehicleWindowError50DS::material,"GB particle completion requires its exact actual world/image/span owner");
    if(request.track.mcParticleWindow && (request.track.gbParticleWindow || request.raStaticWorld || request.track.raStaticFire || request.track.rcParticleWindow || request.track.rcCrystal || request.track.tbWorld ||
        request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures ||
        !request.track.mcParticleWindow->validateTrack(request.track)))
        return fail(d,VehicleWindowError50DS::material,"MC particle completion requires its exact actual world/image/span owner");
    if (request.racerArchetypeCount > maxVehicleWindowRacerArchetypes50DS ||
        bool(request.racerArchetypeCount) != bool(request.racerArchetypes))
        return fail(d, VehicleWindowError50DS::request, "racer archetypes require an explicit bounded cold array");
    for (U i = 0; i < request.racerArchetypeCount; ++i) {
        const auto& archetype = request.racerArchetypes[i];
        if (!archetype.assets || !archetype.assets->loaded())
            return fail(d, VehicleWindowError50DS::assets, "racer archetype assets are not loaded");
        auto selected = request;
        selected.materials = archetype.materials;
        selected.cables = archetype.cables;
        if (!validateMaterials(selected, *archetype.assets, d) || !validateCables(selected, d)) return false;
    }
    std::unique_ptr<Storage> next(new (std::nothrow) Storage);
    if (!next) return fail(d, VehicleWindowError50DS::allocation, "allocate caller-context renderer owner");
    if(request.raStaticWorld){
        if(request.track.mesh || request.track.raStaticFire || !request.raStaticWorldTextures ||
           !request.raStaticWorld->world_ || request.raStaticWorld->terminal_ || request.raStaticWorld->closed_ ||
           request.raStaticWorldTextures->raStaticWorldOwner()!=request.raStaticWorld->world_.get() ||
           !request.raStaticWorld->world_->publishRenderer(this,request.raStaticWorldTextures))
            return fail(d,VehicleWindowError50DS::material,"actual complete RA aggregate/image publication required without legacy track");
        next->raWorld=request.raStaticWorld->world_;next->raWorldRenderer=this;
        next->raWorldImagePublication=request.raStaticWorldTextures->publicationToken();
    }else if(request.raStaticWorldTextures)
        return fail(d,VehicleWindowError50DS::material,"RA image bank requires actual aggregate owner");
    if(request.track.raStaticFire){
        if(!request.track.raStaticFire->publishRenderer(this,request.track.textures))
            return fail(d,VehicleWindowError50DS::material,"FireP0t renderer publication already leased");
        next->raStaticFireOwner=request.track.raStaticFire;next->raStaticFireRenderer=this;
        next->raStaticFirePublicationSerial=request.track.raStaticFire->rendererPublicationSerial_;
    }
    if(request.track.rcParticleWindow){
        if(!request.track.rcParticleWindow->attachRenderer(this))
            return fail(d,VehicleWindowError50DS::material,"RC particle Window already belongs to a renderer");
        next->rcParticleWindow=request.track.rcParticleWindow;next->rcParticleRenderer=this;
        next->rcParticleAttempt=request.track.rcParticleWindow->lastAttempt_;
    }
    if(request.track.gbParticleWindow){
        if(!request.track.gbParticleWindow->attachRenderer(this))
            return fail(d,VehicleWindowError50DS::material,"GB particle Window already belongs to a renderer");
        next->gbParticleWindow=request.track.gbParticleWindow;next->gbParticleRenderer=this;
        next->gbParticleAttempt=request.track.gbParticleWindow->lastAttempt_;
    }
    if(request.track.mcParticleWindow){
        if(!request.track.mcParticleWindow->attachRenderer(this))
            return fail(d,VehicleWindowError50DS::material,"MC particle Window already belongs to a renderer");
        next->mcParticleWindow=request.track.mcParticleWindow;next->mcParticleRenderer=this;
        next->mcParticleAttempt=request.track.mcParticleWindow->lastAttempt_;
    }
    next->diagnostic = &d;
    const bool result = next->initialize(request, assets);
    next->diagnostic = nullptr;
    if (!result) {
        next->cleanup(d);
        if (d.cleanupIncomplete) storage_ = std::move(next);
        return false;
    }
    storage_ = std::move(next);
    return true;
}

bool VehicleWindowGl50DS::createOriginalFrontend(const OriginalFrontendWindowRequest50LA& input,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    if(sessionDisplayCreate50LA_||(sessionDisplayOwner50LA_&&!sessionDisplayFrontendCreate50KG_)){
        d={};return fail(d,VehicleWindowError50DS::request,
            "frontend renderer creation requires its actual frontend display scope");}
    d={};
    if(storage_)return fail(d,VehicleWindowError50DS::alreadyOwned,"renderer already owns resources");
    if(!input.procAddress||!input.isCurrent||!input.cpuTextures||!input.textures||
        (input.output!=VehicleWindowOutput50DS::defaultBackBuffer&&input.output!=VehicleWindowOutput50DS::headless)||
        input.maximumWidth<=0||input.maximumHeight<=0||
        std::uint64_t(input.maximumWidth)*std::uint64_t(input.maximumHeight)>
            std::numeric_limits<std::size_t>::max()/16u)
        return fail(d,VehicleWindowError50DS::request,"frontend requires exact context, authenticated UI bank and bounded output");
    VehicleWindowRequest50DS request;
    request.context=input.context;request.procAddress=input.procAddress;request.isCurrent=input.isCurrent;
    request.maximumWidth=input.maximumWidth;request.maximumHeight=input.maximumHeight;request.output=input.output;
    request.enableOriginalUi=true;request.enableOriginalUiSourceRgba=input.enableSourceRgba;
    request.originalUiCpuTextures=input.cpuTextures;request.originalUiTextures=input.textures;
    std::unique_ptr<Storage> next(new(std::nothrow) Storage);
    if(!next)return fail(d,VehicleWindowError50DS::allocation,"allocate dedicated frontend renderer owner");
    next->diagnostic=&d;
    const bool result=next->initializeOriginalFrontend(request);
    next->diagnostic=nullptr;
    if(!result){
        next->cleanup(d);
        if(d.cleanupIncomplete)storage_=std::move(next);
        return false;
    }
    storage_=std::move(next);return true;
}

bool VehicleWindowGl50DS::updateWorldFog(const VehicleWindowWorldFog50DS& fog,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"world-fog update requires initialized renderer");
    auto& s=*storage_;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"world-fog update requires idle renderer");
    if(!std::isfinite(fog.start) || !std::isfinite(fog.end) || fog.start<0.f || fog.end<=fog.start)
        return fail(d,VehicleWindowError50DS::request,"world-fog update requires finite ordered nonnegative range");
    for(float value:fog.color)
        if(!std::isfinite(value) || value<0.f || value>1.f)
            return fail(d,VehicleWindowError50DS::request,"world-fog update color outside finite [0,1]");
    if(fog.enabled && (s.request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures ||
       s.trackIndexCount<=0 || !s.trackProgram || !s.trackVertexArray || !s.trackSpanCount))
        return fail(d,VehicleWindowError50DS::request,"world-fog enable requires actual retained textured track");
    s.request.track.worldFog=fog;
    return true;
}

bool VehicleWindowGl50DS::bindOriginalSpeedBoost(std::shared_ptr<game::OriginalSpeedBoost50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original SpeedBoost binding requires idle gameplay renderer");
    if(s.nativeF8BarPreparationFailed||s.speedBoost||s.speedBoostUiCreationFailed||s.timeStatsUiCreationFailed||s.positionStatsUiCreationFailed||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.raceHudUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original SpeedBoost already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.prepareSpeedBoostResources(owner);
    if(!accepted&&s.hasSpeedBoostResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deleteSpeedBoostNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasSpeedBoostResources())s.speedBoost.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original SpeedBoost optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::bindOriginalTimeStats(std::shared_ptr<game::OriginalTimeStats50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original TimeStats binding requires idle gameplay renderer");
    if(s.nativeF8BarPreparationFailed||s.timeStats||s.timeStatsUiCreationFailed||s.positionStatsUiCreationFailed||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.raceHudUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original TimeStats already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.prepareTimeStatsResources(owner);
    if(!accepted&&s.hasTimeStatsResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deleteTimeStatsNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasTimeStatsResources())s.timeStats.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original TimeStats optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::bindOriginalDamage(std::shared_ptr<game::OriginalDamage50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original Damage binding requires idle gameplay renderer");
    if(s.nativeF8BarPreparationFailed||s.damage||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.positionStatsUiCreationFailed||s.timeStatsUiCreationFailed||s.raceHudUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original Damage already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.prepareDamageResources(owner);
    if(!accepted&&s.hasDamageResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deleteDamageNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasDamageResources())s.damage.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original Damage optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::retireOriginalPauseDerived(VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!storage_||!storage_->pauseDerived)return true;
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Pause retirement requires its live renderer");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"Pause retirement requires idle renderer");
    s.diagnostic=&d;
    bool okay=s.check("before original Pause binding retirement");
    if(okay){s.gl.finish();okay=s.check("drain original Pause binding work");}
    if(okay)okay=s.deletePauseDerivedNames(d);
    s.diagnostic=nullptr;
    return okay;
}
bool VehicleWindowGl50DS::bindOriginalPauseDerived(std::shared_ptr<game::OriginalPauseNativeDraw50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original PauseDerived binding requires idle gameplay renderer");
    if(s.nativeF8BarPreparationFailed||s.pauseDerived||s.pauseDerivedUiCreationFailed||s.damageUiCreationFailed||s.positionStatsUiCreationFailed||s.timeStatsUiCreationFailed||s.raceHudUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original PauseDerived already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.preparePauseDerivedResources(owner);
    if(!accepted&&s.hasPauseDerivedResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deletePauseDerivedNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasPauseDerivedResources())s.pauseDerived.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original PauseDerived optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::bindOriginalPositionStats(std::shared_ptr<game::OriginalPositionStatsNativeOwner50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original PositionStats binding requires idle gameplay renderer");
    if(s.nativeF8BarPreparationFailed||s.positionStats||s.positionStatsUiCreationFailed||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.timeStatsUiCreationFailed||s.raceHudUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original PositionStats already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.preparePositionStatsResources(owner);
    if(!accepted&&s.hasPositionStatsResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deletePositionStatsNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasPositionStatsResources())s.positionStats.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original PositionStats optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::bindOriginalRaceHud(const game::OriginalRaceHud50KG& owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original HUD binding requires idle gameplay renderer");
    if(s.nativeF8BarPreparationFailed||s.raceHud||s.raceHudUiCreationFailed||s.positionStatsUiCreationFailed||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.timeStatsUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original HUD already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.prepareRaceHudResources(owner);
    if(!accepted&&s.hasRaceHudResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deleteRaceHudNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasRaceHudResources())s.raceHud.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original HUD optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::submit(const game::VehicleDrawBatch50CO& batch,
    const VehicleWindowFrame50DS& frame, VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    Storage::RcFrameObservation rcObservation(s,d);
    if(s.frameNativeResults||s.nativeResultsFrame||s.nativeResultsRenderer||
       (frame.nativeResults&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic)))
        return fail(d,VehicleWindowError50DS::frame,"Results native frame requires idle owning renderer");
    if(s.frameNativeF8Bars||s.nativeF8BarFrame||s.nativeF8BarRenderer||
       (frame.nativeF8BarPrefix&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic)))
        return fail(d,VehicleWindowError50DS::frame,"F8 native frame requires the idle owning renderer");
    if(s.nativeF8BarPreparationFailed||s.nativeResultsPreparationFailed)
        return fail(d,VehicleWindowError50DS::frame,"failed native F8/Results resource preparation remains cleanup-owned");
    auto submissionStageStart=s.rendererStageNow();
    if(s.request.enableGameplayDisplayWallTiming)s.playerTiming={};
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    // Only this real serial gameplay draw entry advances the native attempt.
    // Fixed updates and completion/readback methods never manufacture one.
    if(s.rcParticleWindow&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"RC particle draw entry requires the idle creating thread");
    if((s.gbParticleWindow||s.mcParticleWindow)&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"GB particle transaction requires idle creating thread");
    Storage::RcParticleAttemptScope rcParticleAttemptScope{s,d};
    Storage::GbParticleAttemptScope gbParticleAttemptScope{s,d};
    Storage::McParticleAttemptScope mcParticleAttemptScope{s,d};
    if(!s.beginRcParticleAttempt(d)||!s.beginGbParticleAttempt(d)||!s.beginMcParticleAttempt(d))return false;
    if (!s.current(d)) return false;
    // After affinity/current-context admission, clear borrowed input pointers
    // on EVERY exit, including validation failures before common frame cleanup.
    struct NativeF8BorrowReset {
        Storage& owner;
        ~NativeF8BorrowReset(){owner.frameNativeF8Bars=nullptr;
            owner.nativeF8BarFrame=nullptr;owner.nativeF8BarRenderer=nullptr;}
    } nativeF8BorrowReset{s};
    struct ResultsBorrowReset {
        Storage& owner;
        ~ResultsBorrowReset(){owner.frameNativeResults=nullptr;owner.nativeResultsFrame=nullptr;owner.nativeResultsRenderer=nullptr;}
    } resultsBorrowReset{s};
    if(frame.nativeResults){
        if(!frame.nativeResults->current(*this,frame))
            return fail(d,VehicleWindowError50DS::frame,"Results exact accepted native Frame ownership changed");
        s.frameNativeResults=frame.nativeResults;s.nativeResultsFrame=&frame;s.nativeResultsRenderer=this;
    }
    // Any new gameplay attempt supersedes an uncompleted candidate. Failure
    // anywhere below cannot authorize its partial draws through a later finish.
    s.trackVisibilityPendingTarget=0;
    for(U i=0;i<s.trackSpanCount;++i)s.trackSpans[i].submittedGameplayCandidate=false;
    if(!s.prepareNativeF8BarFrame(*this,frame,d))return false;
    if(!s.texturesCurrent(d))return false;
    struct TrackUvBorrowReset {
        VehicleWindowTrackUvFrame50DS& borrowed;
        ~TrackUvBorrowReset() { borrowed={}; }
    } trackUvBorrowReset{s.frameTrackUv};
    if(!s.validateGameplayBlurFrame(frame.gameplayBlur,d) || !s.validateGcParticleFrame(frame,d) ||
       !s.validateRcParticleFrame(frame,d) || !s.validateGbParticleFrame(frame,d) || !s.validateMcParticleFrame(frame,d))return false;
    for(U i=0;i<s.trackSpanCount;++i)if(s.trackSpans[i].sourceUvAnimation &&
        (!frame.trackUvAnimation || !frame.trackUvAnimation->owner || !frame.trackUvAnimation->service))
        return fail(d,VehicleWindowError50DS::frame,"animated static material has no real frame controller");
    // A cold bank may remain resident in Main. Only active race frames supply
    // meshes; absence never requests an animation clock or a lighting owner.
    if((frame.gameplayBinderCount&&!s.request.gameplayBinderTextures)||
        bool(frame.gameplayBinders)!=bool(frame.gameplayBinderCount))
        return fail(d,VehicleWindowError50DS::frame,"Binder cold/frame presence differs");
    if(frame.gameplayBinderCount){
        if(frame.gameplayBinderCount<frame.opponentCount+1u||frame.gameplayBinderCount>2u*(frame.opponentCount+1u)||frame.gameplayBinderCount>16u||frame.gameplayBinderFrame>=10u)
            return fail(d,VehicleWindowError50DS::frame,"Binder racer/frame extent differs");
        constexpr float uv[4][2]={{0,0},{0,1},{1,0},{1,1}};
        for(U i=0;i<frame.gameplayBinderCount;++i)for(U v=0;v<4;++v){
            const auto& vertex=frame.gameplayBinders[i].vertices[v];
            for(float x:vertex.position)if(!std::isfinite(x))return fail(d,VehicleWindowError50DS::frame,"Nonfinite Binder position");
            for(U c=0;c<2;++c)if(vertex.uv[c]!=uv[v][c])return fail(d,VehicleWindowError50DS::frame,"Binder source UV order differs");
            if(vertex.color[3]!=64)return fail(d,VehicleWindowError50DS::frame,"Binder source strip alpha differs");
        }
    }
    if (frame.width <= 0 || frame.height <= 0 ||
        frame.width > s.maximumWidth || frame.height > s.maximumHeight ||
        !frame.worldViewProjection || !frame.clearRgba)
        return fail(d, VehicleWindowError50DS::frame,
            "explicit matrix/color and nonzero viewport within cold bounds required");
    for (U i = 0; i < 16; ++i)
        if (!std::isfinite(frame.worldViewProjection[i]))
            return fail(d, VehicleWindowError50DS::frame, "nonfinite world-to-clip matrix");
    for (U i = 0; i < 4; ++i)
        if (!std::isfinite(frame.clearRgba[i]) || frame.clearRgba[i] < 0 || frame.clearRgba[i] > 1)
            return fail(d, VehicleWindowError50DS::frame, "clear color outside finite [0,1]");
    // From the first reached flare service through every backend/preflight
    // exit, partial source effects require retirement, never next-frame replay.
    bool flareEntered=false,flareSubmissionAccepted=false;
    struct FlareSubmissionScope {Storage& owner;bool& entered;bool& accepted;
        ~FlareSubmissionScope(){if(entered&&!accepted)owner.raLocalFlareFailed=true;}
    } flareSubmissionScope{s,flareEntered,flareSubmissionAccepted};
    if(!s.prepareRaWorldFrame(frame,d,flareEntered))return false;
    if (s.request.track.worldFog.enabled) {
        if (!frame.worldFogCamera)
            return fail(d, VehicleWindowError50DS::frame, "world fog requires explicit camera position");
        for (U i = 0; i < 3; ++i)
            if (!std::isfinite(frame.worldFogCamera[i]))
                return fail(d, VehicleWindowError50DS::frame, "nonfinite world fog camera");
    }
    if (bool(s.skyIndexCount) != bool(frame.skyWorldViewProjection))
        return fail(d, VehicleWindowError50DS::frame, "sky matrix presence must match authored sky request");
    if (frame.skyWorldViewProjection)
        for (U i = 0; i < 16; ++i)
            if (!std::isfinite(frame.skyWorldViewProjection[i]))
                return fail(d, VehicleWindowError50DS::frame, "nonfinite authored sky matrix");
    // Selected-light validation precedes draw submission. Scoped RA flare
    // resource preparation above restores its bindings; any subsequent refusal
    // retires reached flare effects. Missing packets never use generic light.
    game::NativeGcSelectedLightsCandidate playerLights{};
    game::NativeGcSelectedLightsCandidate opponentLights[maxVehicleWindowOpponents50DS]{};
    {
        // These owned validators neither invoke callbacks nor mutate the live
        // vertex/normal streams. Reuse only a completed, identity-matched scan;
        // ready() still performs its original scan after currentness callbacks.
        Storage::RacerNormalReceipt normalReceipts[1 + maxVehicleWindowOpponents50DS]{};
        if (!s.validateMaterialUvs(frame.materialUvs, frame.materialUvCount, d) ||
            !s.validateOpponents(batch, frame, d, normalReceipts) ||
            !s.validateDynamicWorldFrame(frame.dynamicWorld, d) ||
            !s.validateTrackAnimationFrames(frame, d)) return false;
        std::uintptr_t fleetLightOwner = 0;
        if (!s.copyNativeGcLights(frame.lightingPolicy, frame.selectedLights, 0,
            batch, 0, fleetLightOwner, playerLights, d, normalReceipts[0])) return false;
        for (U i = 0; i < frame.opponentCount; ++i) {
            d.opponentIndex = i;
            const auto& opponent = frame.opponents[i];
            if (!s.copyNativeGcLights(opponent.lightingPolicy, opponent.selectedLights, i + 1,
                *opponent.batch, opponent.racerArchetype, fleetLightOwner, opponentLights[i], d,
                normalReceipts[i + 1])) return false;
        }
        d.opponentIndex = maxVehicleWindowOpponents50DS;
        if (!s.prepareSpecularFrame50LA(batch, frame, d, normalReceipts) || !s.prepareRcCrystalFrame(frame,d)) return false;
    }
    if (bool(s.request.cables.textures) != bool(frame.cables))
        return fail(d, VehicleWindowError50DS::frame, "cable frame presence must match explicit cold cable request");
    if (frame.cables && !validateCableFrame(*frame.cables, d)) return false;
    d.originalSpeedBoostRejected=!s.prepareSpeedBoostFrame(frame.originalSpeedBoost);
    if(d.originalSpeedBoostRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original Speed/Boost frame rejected");
    d.originalTimeStatsRejected=!s.prepareTimeStatsFrame(frame.originalTimeStats);
    if(d.originalTimeStatsRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original TimeStats frame rejected");
    d.originalPositionStatsRejected=!s.preparePositionStatsFrame(frame.originalPositionStats);
    if(d.originalPositionStatsRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original PositionStats frame rejected");
    d.originalPauseDerivedRejected=!s.preparePauseDerivedFrame(frame.originalPauseDerived);
    if(d.originalPauseDerivedRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original Pause derived frame rejected");
    d.originalDamageRejected=!s.prepareDamageFrame(frame.originalDamage);
    if(d.originalDamageRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original Damage frame rejected");
    float admittedOriginalBottom=0.f;
    if(s.timeStats&&s.timeStats->frame){
        // Match drawTimeStats' actual640x448 aspect transform, including
        // letterboxing. Use admitted copied geometry, never a caller flag.
        const double sourceScale=std::min(double(frame.width)/640.0,double(frame.height)/448.0);
        const double oy=(frame.height-448.0*sourceScale)*.5;
        float bottom=0.f;
        for(U i=0;i<s.timeStats->quads*4;++i)
            bottom=std::max(bottom,s.timeStats->vertices[i].position[1]);
        admittedOriginalBottom=static_cast<float>(oy+double(bottom)*sourceScale);
    }
    if (frame.hostHud) {
        if (!s.request.enableHostHud || !s.hostHudGeometry)
            return fail(d, VehicleWindowError50DS::frame, "host HUD frame requires explicit cold enableHostHud");
        const VehicleWindowHostHud50DS hud = *frame.hostHud;
        const float values[] = {hud.heat01, hud.leftDamage01, hud.rightDamage01};
        for (float value : values)
            if (!std::isfinite(value) || value < 0.f || value > 1.f)
                return fail(d, VehicleWindowError50DS::frame, "host HUD live values must be finite [0,1]");
        if (hud.routeValid &&
            (!std::isfinite(hud.routeX) || !std::isfinite(hud.routeY) ||
             std::abs(hud.routeX) > 1.f || std::abs(hud.routeY) > 1.f ||
             std::abs(hud.routeX * hud.routeX + hud.routeY * hud.routeY - 1.f) > 0.001f))
            return fail(d, VehicleWindowError50DS::frame,
                "host HUD enabled route bearing must be finite normalized screen-right/ahead");
        if (hud.lapCountValid && hud.completedLaps > maxVehicleWindowCompletedLaps50DS)
            return fail(d, VehicleWindowError50DS::frame,
                "host HUD enabled completed-lap count must be in [0,999]");
        // Native 1AFD80 installs 0x40A66666 (5.2 seconds) for the ordinary
        // course start. The existing one-way HUD adapter displays ceil of
        // the real remaining time, so its admitted maximum is 6, not the
        // earlier HOST-only three-second start. This changes no race clock.
        constexpr U maxRaceCountdown = 6;
        if (hud.raceEnabled && (!hud.raceLapTarget || hud.raceLapTarget > 999 ||
            !hud.raceLap || hud.raceLap > hud.raceLapTarget || !hud.raceRacerCount ||
            hud.raceRacerCount > 16 || !hud.racePlace || hud.racePlace > hud.raceRacerCount ||
            hud.raceCountdown > maxRaceCountdown ||
            unsigned(hud.raceCountdown != 0) + unsigned(hud.raceGo) + unsigned(hud.raceFinished) +
                unsigned(!hud.raceFinished && hud.raceRetirementCD4) > 1)) {
            // Failure-only payload; keep the public diagnostic ABI intact.
            if (d.error == VehicleWindowError50DS::none)
                std::fprintf(stderr,
                    "native race HUD rejection lap=%u target=%u place=%u racers=%u countdown=%u go=%u finished=%u retirementCD4=%u\n",
                    static_cast<unsigned>(hud.raceLap), static_cast<unsigned>(hud.raceLapTarget),
                    static_cast<unsigned>(hud.racePlace), static_cast<unsigned>(hud.raceRacerCount),
                    static_cast<unsigned>(hud.raceCountdown), static_cast<unsigned>(hud.raceGo),
                    static_cast<unsigned>(hud.raceFinished), static_cast<unsigned>(hud.raceRetirementCD4));
            return fail(d, VehicleWindowError50DS::frame, "native race HUD counters/status outside admitted bounds");
        }
        if (hud.raceResultsReady && (!hud.raceEnabled || (!hud.raceFinished && !hud.raceRetirementCD4) ||
            hud.raceCountdown || hud.raceGo))
            return fail(d, VehicleWindowError50DS::frame, "host results presentation requires a sealed finish or retirement");
        if (hud.resultsPanelVisible && (!hud.raceResultsReady || !hud.standingsCount ||
            hud.resultsSelectedAction >= 3 ||
            (!hud.resultsConfirming && hud.resultsConfirmAccept)))
            return fail(d, VehicleWindowError50DS::frame, "live results panel requires a sealed outcome and bounded action");
        if (hud.resultsPanelVisible && hud.raceFinished &&
            (!hud.resultsFinishPlace || hud.resultsFinishPlace > hud.raceRacerCount ||
             (!hud.standingsSourceRanks && hud.resultsFinishPlace != hud.racePlace) ||
             !std::isfinite(hud.resultsFinishSeconds) || hud.resultsFinishSeconds < 0.0 ||
             hud.resultsFinishSeconds > double(maxVehicleWindowLapSeconds50DS)))
            return fail(d, VehicleWindowError50DS::frame, "finished results require an explicit bounded finish place/time");
        if (hud.resultsPanelVisible && !hud.raceFinished &&
            (hud.resultsFinishPlace || hud.resultsFinishSeconds != 0.0))
            return fail(d, VehicleWindowError50DS::frame, "DNF results cannot supply a finish place or finish time");
        if (hud.raceClocksValid) {
            const auto validSeconds = [](float value) noexcept {
                return std::isfinite(value) && value >= 0.f && value <= maxVehicleWindowLapSeconds50DS;
            };
            if (!hud.raceEnabled || !validSeconds(hud.raceElapsedSeconds) ||
                (!hud.raceFinished && !hud.raceRetirementCD4 && !validSeconds(hud.currentLapSeconds)))
                return fail(d, VehicleWindowError50DS::frame, "native race clocks outside admitted presentation bounds");
        }
        if (hud.completedLapTimesValid) {
            if (!hud.raceEnabled || !hud.lapCountValid || !hud.standingsCount ||
                hud.raceLapTarget > maxVehicleWindowLapTimes50DS ||
                hud.completedLapTimeCount != std::min(hud.completedLaps, hud.raceLapTarget))
                return fail(d, VehicleWindowError50DS::frame, "completed lap times require the bounded committed race");
            const auto validSeconds = [](float value) noexcept {
                return std::isfinite(value) && value >= 0.f && value <= maxVehicleWindowLapSeconds50DS;
            };
            if (hud.completedLapTimeCount && !validSeconds(hud.bestLapSeconds))
                return fail(d, VehicleWindowError50DS::frame, "native best lap seconds outside admitted bounds");
            for (U i = 0; i < hud.completedLapTimeCount; ++i)
                if (!validSeconds(hud.completedLapSeconds[i]))
                    return fail(d, VehicleWindowError50DS::frame, "native completed lap seconds outside admitted bounds");
        }
        if (hud.standingsCount) {
            if (!hud.raceEnabled || hud.standingsCount > maxVehicleWindowStandings50DS ||
                hud.standingsCount != hud.raceRacerCount || hud.standingsPlayerIndex >= hud.standingsCount)
                return fail(d, VehicleWindowError50DS::frame, "named standings require the complete bounded race field");
            bool seen[maxVehicleWindowStandings50DS]{}, unfinishedSeen = false;
            for (U i = 0; i < hud.standingsCount; ++i) {
                const auto& row = hud.standings[i];
                const bool rankValid = hud.standingsSourceRanks ?
                    (row.place >= 1 && row.place <= hud.standingsCount &&
                     (!i || hud.standings[i - 1].place <= row.place)) : row.place == i + 1;
                if (row.racerIndex >= hud.standingsCount || seen[row.racerIndex] || !rankValid ||
                    !validHostHudName(row.displayName) || row.completedLaps > maxVehicleWindowCompletedLaps50DS ||
                    row.finished != (row.completedLaps >= hud.raceLapTarget) ||
                    (!hud.standingsSourceRanks && row.finished && unfinishedSeen) || (hud.raceCountdown && row.completedLaps))
                    return fail(d, VehicleWindowError50DS::frame, "named standings order/name/lap/finish state rejected");
                seen[row.racerIndex] = true;
                unfinishedSeen = unfinishedSeen || !row.finished;
                if (row.racerIndex == hud.standingsPlayerIndex &&
                    (row.place != hud.racePlace || row.finished != hud.raceFinished ||
                     row.retirementCD4 != hud.raceRetirementCD4 ||
                     std::min(row.completedLaps + 1, hud.raceLapTarget) != hud.raceLap))
                    return fail(d, VehicleWindowError50DS::frame, "named standings player differs from race HUD");
            }
        }
        if (!s.hostHudGeometry->build(hud, frame.width, frame.height,admittedOriginalBottom))
            return fail(d, VehicleWindowError50DS::limits, "host HUD exceeds fixed glyph geometry capacity");
    }
    // Reset every cold material to identity so absent overrides never retain
    // a previous frame's animation. Validation above precedes all GL work.
    for (U i = 0; i < s.materialCount; ++i) {
        auto& uv = s.materials[i].uv;
        uv[0] = uv[1] = 1.f; uv[2] = uv[3] = 0.f;
    }
    for (U i = 0; i < frame.materialUvCount; ++i) {
        const auto& input = frame.materialUvs[i];
        auto& uv = s.materials[s.materialOffsets[componentIndex(input.component)] + input.materialIndex].uv;
        uv[0] = input.uScale; uv[1] = input.vScale;
        uv[2] = input.uOffset; uv[3] = input.vOffset;
    }
    // Snapshot bounded opponent descriptors and UV values before any GL work.
    // Their independent source vertex/cable arrays remain borrowed only through
    // this synchronous transaction; no per-frame allocation or index upload.
    // Reuse the admitted light values above instead of copying every component
    // matrix again. Reset on every exit, including a failed backend transaction.
    struct FrameLightBorrowReset {
        Storage& state;
        U opponentCount;
        ~FrameLightBorrowReset() noexcept {
            state.framePlayerLights = nullptr;
            for (U i = 0; i < opponentCount; ++i) state.frameOpponentLights[i] = nullptr;
        }
    } frameLightBorrowReset{s, frame.opponentCount};
    s.frameOpponentCount = frame.opponentCount;
    s.framePlayerBatch = batch;
    s.framePlayerLights = &playerLights;
    for (U i = 0; i < frame.opponentCount; ++i) {
        const auto& opponent = frame.opponents[i];
        s.frameOpponentBatches[i] = *opponent.batch;
        s.frameOpponentLights[i] = &opponentLights[i];
        s.frameOpponentCables[i] = opponent.cables;
        s.frameOpponentArchetypes[i] = opponent.racerArchetype;
        const auto selected = s.racerView(opponent.racerArchetype);
        for (U m = 0; m < selected.materialCount; ++m)
            s.opponentMaterialUvs[i * s.opponentMaterialStride + m] = {};
        for (U u = 0; u < opponent.materialUvCount; ++u) {
            const auto& input = opponent.materialUvs[u];
            auto& uv = s.opponentMaterialUvs[i * s.opponentMaterialStride +
                selected.materialOffsets[componentIndex(input.component)] + input.materialIndex].value;
            uv[0] = input.uScale; uv[1] = input.vScale;
            uv[2] = input.uOffset; uv[3] = input.vOffset;
        }
    }
    s.frameTrackUv=frame.trackUvAnimation?*frame.trackUvAnimation:VehicleWindowTrackUvFrame50DS{};
    s.frameSkyEnabled=frame.skyEnabled;
    if (frame.skyWorldViewProjection)
        std::memcpy(s.skyMatrix, frame.skyWorldViewProjection, sizeof(s.skyMatrix));
    if (s.request.track.worldFog.enabled)
        std::memcpy(s.worldFogCamera, frame.worldFogCamera, sizeof(s.worldFogCamera));
    std::memcpy(s.matrix, frame.worldViewProjection, sizeof(s.matrix));
    std::memcpy(s.clearRgba, frame.clearRgba, sizeof(s.clearRgba));
    s.pendingRaceCameraPublication=frame.sourceBodyCameraPublication50LA;
    s.width = frame.width; s.height = frame.height;
    s.frameCables = frame.cables;
    s.frameGameplayBinders=frame.gameplayBinders;s.frameGameplayBinderCount=frame.gameplayBinderCount;
    s.frameGameplayBinderImage=frame.gameplayBinderFrame;
    s.frameGcParticles=frame.mcParticles?frame.mcParticles->rendererDraw():frame.gbParticles?frame.gbParticles->rendererDraw():(frame.rcParticles?frame.rcParticles->rendererDraw():frame.gcParticles);
    s.frameRcParticles=bool(frame.rcParticles);s.frameGbParticles=bool(frame.gbParticles);s.frameMcParticles=bool(frame.mcParticles);
    s.frameDynamicWorld = frame.dynamicWorld;
    for (U i = 0; i < s.trackAnimationCount; ++i)
        s.trackAnimations[i].selectedFrame = frame.trackAnimationFrames[i];
    s.frameHostHud = frame.hostHud != nullptr;
    // Prepare authentic geometry before any gameplay target writes. A bounded
    // rejection leaves the existing native HUD and world frame unchanged.
    d.originalRaceHudRejected=!s.prepareRaceHudFrame(frame.originalRaceHud);
    if(d.originalRaceHudRejected&&s.raceHud)s.raceHud->frame=false;
    s.frameGameplayBlurEnabled=frame.gameplayBlur!=nullptr;
    s.frameGameplayBlur=frame.gameplayBlur?*frame.gameplayBlur:GameplayBlurFrame50DS{};
    s.frameSet = true; s.diagnostic = &d;
    s.prepareTrackVisibility();
    s.traceRaStage("validation_prepare",submissionStageStart,false);
    bool accepted = s.check("before private submission") &&
        (s.request.output == VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    if (accepted) accepted = s.prepareCoverageExtent();
    s.traceRaStage("target_preflight",submissionStageStart,false);
    if (accepted) {
        accepted = game::submitVehicleDrawBatch50CS(batch, s.adapter->backend(), d.submission);
        s.traceRaStage("backend_transaction",submissionStageStart,false);
        if (!accepted) {
            // Structural rejection precedes adapter error reset: never copy an
            // old GL error into a new structural-rejection diagnostic.
            if (!d.glError && d.submission.error >= game::VehicleDrawSubmitError50CS::backendPreflight)
                d.glError = s.adapter->glError();
            std::fprintf(stderr,"vehicle_submission_failure stage=%u component=%u span=%u gl=%u\n",
                unsigned(d.submission.error),d.submission.component,d.submission.span,s.adapter->glError());
            // Keep the first concrete world/effect rejection from callbacks;
            // the outer submission status must not erase its cause.
            if(d.error==VehicleWindowError50DS::none)
                fail(d, VehicleWindowError50DS::submission, "50CS/50CV submission rejected");
        } else {
            d.batchAccepted = true;
            if(s.request.output==VehicleWindowOutput50DS::headless)
                s.noteGameplayDisplayPending(s.displayedTarget,s.committedWidth,s.committedHeight);
            if (s.request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
                auto copyStageStart=s.rendererStageNow();
                accepted = s.copyToWindow();
                s.traceRaStage("copy_to_window",copyStageStart,false);
                if (!accepted && d.error == VehicleWindowError50DS::gl)
                    d.error = VehicleWindowError50DS::presentation;
            }
        }
    }
    s.frameSet = false; s.frameCables = nullptr; s.frameHostHud = false; s.diagnostic = nullptr;
    if(s.raceHud)s.raceHud->frame=false;
    if(s.timeStats)s.timeStats->frame=false;
    if(s.positionStats)s.positionStats->frame=false;
    if(s.damage)s.damage->frame=false;
    if(s.pauseDerived)s.pauseDerived->frame=false;
    if(s.speedBoost)s.speedBoost->frame=false;
    s.frameGameplayBlurEnabled=false;s.frameGameplayBlur={};
    s.frameGameplayBinders=nullptr;s.frameGameplayBinderCount=0;s.frameGameplayBinderImage=0;
    s.frameGcParticles=nullptr;s.frameRcParticles=false;s.frameGbParticles=false;s.frameMcParticles=false;
    s.framePlayerBatch = game::VehicleDrawBatch50CO{};
    s.frameDynamicWorld = {};
    for (U i = 0; i < s.frameOpponentCount; ++i) {
        s.frameOpponentBatches[i] = game::VehicleDrawBatch50CO{};
        s.frameOpponentCables[i] = nullptr;
        s.frameOpponentArchetypes[i] = 0;
    }
    s.frameOpponentCount = 0;
    if(accepted && s.displayedTarget==s.targets[s.published])
        s.trackVisibilityPendingTarget=s.displayedTarget;
    if(s.rcParticleWindow){
        if(accepted&&!s.trackVisibilityPendingTarget)
            accepted=fail(d,VehicleWindowError50DS::frame,"RC accepted draw has no actual pending gameplay target");
        rcParticleAttemptScope.retainPending=accepted;
    }
    if(s.gbParticleWindow){
        if(accepted&&!s.trackVisibilityPendingTarget)
            accepted=fail(d,VehicleWindowError50DS::frame,"GB accepted draw has no actual pending gameplay target");
        gbParticleAttemptScope.retainPending=accepted;
    }
    if(s.mcParticleWindow){
        if(accepted&&!s.trackVisibilityPendingTarget)
            accepted=fail(d,VehicleWindowError50DS::frame,"MC accepted draw has no actual pending gameplay target");
        mcParticleAttemptScope.retainPending=accepted;
    }
    s.reportPlayerTiming(accepted);
    flareSubmissionAccepted=accepted;
    return accepted;
}

bool VehicleWindowGl50DS::submitHostMenu(const VehicleWindowHostMenu50DS& input,
    std::int32_t width, std::int32_t height, VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if (!s.current(d)) return false;
    if (s.active || s.frameSet || !s.request.enableHostHud || !s.hostHudGeometry ||
        !s.hostHudProgram || !s.hostHudVertexBuffer || !s.hostHudIndexBuffer || !s.hostHudVertexArray ||
        !s.menuTargets[0] || !s.menuTargets[1] ||
        width <= 0 || height <= 0 || width > s.maximumWidth || height > s.maximumHeight ||
        !input.rowCount || input.rowCount > maxVehicleWindowMenuRows50DS || input.selectedRow >= input.rowCount)
        return fail(d, VehicleWindowError50DS::frame, "host menu requires idle opt-in renderer and bounded viewport/rows");
    if (input.retainRaceImage && s.raceWidth > 0 && s.raceHeight > 0 && !s.gl.blitFramebuffer)
        return fail(d, VehicleWindowError50DS::entryPoint, "retained host menu backdrop requires glBlitFramebuffer");
    const auto validText = [](const char* value) noexcept {
        if (!value) return false;
        for (U i = 0; i <= maxVehicleWindowMenuText50DS; ++i) {
            if (!value[i]) return true;
            if (i == maxVehicleWindowMenuText50DS || (value[i] != ' ' && !hostHudGlyph(value[i]))) return false;
        }
        return false;
    };
    if (!validText(input.title) || !validText(input.subtitle) || !validText(input.footer))
        return fail(d, VehicleWindowError50DS::frame, "host menu text is not bounded supported ASCII");
    for (U i = 0; i < input.rowCount; ++i)
        if (!validText(input.rows[i]))
            return fail(d, VehicleWindowError50DS::frame, "host menu row is not bounded supported ASCII");
    if (!s.hostHudGeometry->buildMenu(input, width, height))
        return fail(d, VehicleWindowError50DS::frame, "host menu exceeds cold glyph capacity");

    s.diagnostic = &d;
    bool accepted = s.check("before private host menu submission") &&
        (s.request.output == VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    const auto pending = static_cast<std::uint8_t>(s.menuPublished ^ 1u);
    if (accepted) {
        // Separate color-only targets leave the native adapter's publication
        // bank, both GPU vertex banks, and its last complete race image intact.
        s.gl.bindFramebuffer(framebuffer, s.menuTargets[pending]);
        s.gl.drawBuffer(colorAttachment); s.gl.readBuffer(colorAttachment);
        s.gl.viewport(0, 0, width, height);
        constexpr U disabled[] = {0x0BE2, 0x0B44, 0x0C11, 0x0B90, 0x8037, 0x0B20,
            0x8F9D, 0x0BD0, 0x809D, 0x80A0, 0x809E, 0x8C89, 0x8DB9, 0x864F, 0x0B71};
        for (U capability : disabled) s.gl.disable(capability);
        for (I i = 0; i < s.clipDistances; ++i) s.gl.disable(0x3000 + static_cast<U>(i));
        s.gl.colorMask(1, 1, 1, 1); s.gl.polygonMode(0x0408, 0x1B02);
        s.gl.clearColor(.015f, .025f, .045f, 1.f); s.gl.clear(colorBufferBit);
        if (input.retainRaceImage && s.raceWidth > 0 && s.raceHeight > 0) {
            s.gl.bindFramebuffer(readFramebuffer, s.targets[s.published]);
            s.gl.readBuffer(colorAttachment);
            s.gl.blitFramebuffer(0, 0, s.raceWidth, s.raceHeight,
                0, 0, width, height, colorBufferBit, 0x2600);
        }
        accepted = s.check("prepare independent host menu backdrop");
        s.frameHostHud = true;
        if (accepted) accepted = s.drawHostHud() && s.check("accept private host menu frame");
        s.frameHostHud = false;
        if (accepted) {
            s.menuPublished = pending; s.displayedTarget = s.menuTargets[pending];
            s.uiPrivatePublicationGeneration=0;
            s.uiSourcePublishedTargetGeneration=0;
            s.width = s.committedWidth = width; s.height = s.committedHeight = height;
            s.imageCommitted = true; d.hostMenuAccepted = true;
            if(s.request.output==VehicleWindowOutput50DS::headless)
                s.noteGameplayDisplayPending(s.displayedTarget,width,height);
            if (s.request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
                accepted = s.copyToWindow();
                if (!accepted && d.error == VehicleWindowError50DS::gl)
                    d.error = VehicleWindowError50DS::presentation;
            }
        }
    }
    s.diagnostic = nullptr;
    return accepted;
}

bool VehicleWindowGl50DS::submitRetainedOriginalPause(
    const game::OriginalPauseNativeDraw50LA& owner,I width,I height,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Pause requires initialized renderer");
    auto& s=*storage_;
    if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.uiBlendOldSnapshot||s.hasUiByteBlendNames()||
       s.pauseDerivedUiCreationFailed||!s.request.enableOriginalGameplayBlur||
       !s.retainedRaceDisplayCompleted||s.retainedRaceContainsPause||!s.imageCommitted||
       !s.retainedRaceCameraPublication||s.gameplayAwaitingTarget||
       width<=0||height<=0||width!=s.raceWidth||height!=s.raceHeight||
       width>s.maximumWidth||height>s.maximumHeight||!s.targets[s.published]||
       !s.menuTargets[0]||!s.menuTargets[1]||!s.gl.blitFramebuffer||
       !owner.frame()||owner.frame()->cameraPublication!=s.retainedRaceCameraPublication)
        return fail(d,VehicleWindowError50DS::frame,"Pause requires completed unchanged race extent and camera publication");
    if(!s.preparePauseDerivedFrame(&owner)||!s.pauseDerived||!s.pauseDerived->frame){
        d.originalPauseDerivedRejected=true;
        return fail(d,VehicleWindowError50DS::frame,"authentic bound Pause frame rejected");
    }
    s.diagnostic=&d;
    const auto oldWidth=s.width,oldHeight=s.height;
    s.width=width;s.height=height;
    const auto pending=static_cast<std::uint8_t>(s.menuPublished^1u);
    bool accepted=s.check("before retained original Pause")&&
        (s.request.output==VehicleWindowOutput50DS::headless||s.checkDefaultFramebuffer());
    if(accepted){
        // Normalize raster state without touching world/material/flame preparation.
        constexpr U disabled[]={0x0BE2,0x0B44,0x0C11,0x0B90,0x8037,0x0B20,
            0x8F9D,0x0BD0,0x809D,0x80A0,0x809E,0x8C89,0x8DB9,0x864F};
        for(U capability:disabled)s.gl.disable(capability);
        for(I i=0;i<s.clipDistances;++i)s.gl.disable(0x3000+static_cast<U>(i));
        s.gl.polygonMode(0x0408,0x1B02);
        accepted=s.drawPauseDerivedTargets(s.targets[s.published],s.menuTargets[pending])&&
            s.check("accept retained original Pause");
    }
    s.pauseDerived->frame=false;
    if(accepted){
        s.menuPublished=pending;s.displayedTarget=s.menuTargets[pending];
        s.uiPrivatePublicationGeneration=0;s.uiSourcePublishedTargetGeneration=0;
        s.committedWidth=width;s.committedHeight=height;s.imageCommitted=true;
        if(s.request.output==VehicleWindowOutput50DS::headless)
            s.noteGameplayDisplayPending(s.displayedTarget,width,height);
        else {
            accepted=s.copyToWindow();
            if(!accepted&&d.error==VehicleWindowError50DS::gl)d.error=VehicleWindowError50DS::presentation;
        }
    }else{s.width=oldWidth;s.height=oldHeight;}
    s.diagnostic=nullptr;return accepted;
}

bool VehicleWindowGl50DS::readbackRgba(std::uint8_t* destination, std::size_t capacityBytes,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.current(d)) return false;
    if (!s.imageCommitted)
        return fail(d, VehicleWindowError50DS::noImage, "no privately committed viewport to read");
    d.readbackWidth = s.committedWidth; d.readbackHeight = s.committedHeight;
    // Cold bounds already prove this product fits size_t; pending/failed frames
    // cannot replace these dimensions. Still validate before any readback GL call.
    d.readbackBytes = std::size_t(s.committedWidth) * std::size_t(s.committedHeight) * 4;
    if (!destination || capacityBytes < d.readbackBytes)
        return fail(d, VehicleWindowError50DS::readback, "RGBA destination is null or too small for committed viewport");
    s.diagnostic = &d;
    const bool result = s.copyRgba(destination);
    if (!result && d.error == VehicleWindowError50DS::gl) d.error = VehicleWindowError50DS::readback;
    s.diagnostic = nullptr;
    return result;
}

bool VehicleWindowGl50DS::readbackWindowFrontRgba(std::uint8_t* destination,
    std::size_t capacityBytes,I width,I height,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    return readbackWindowBufferRgba(destination,capacityBytes,width,height,true,nullptr,d);
}

bool VehicleWindowGl50DS::readbackWindowBackRgba(GameHost50ER& host,std::uint8_t* destination,
    std::size_t capacityBytes,I width,I height,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    return readbackWindowBufferRgba(destination,capacityBytes,width,height,false,&host,d);
}

bool VehicleWindowGl50DS::readbackWindowBufferRgba(std::uint8_t* destination,
    std::size_t capacityBytes,I width,I height,bool front,const GameHost50ER* expectedHost,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"window readback requires initialized renderer");
    auto& s=*storage_;
    if(!s.current(d))return false;
    if((expectedHost && s.request.context!=static_cast<const void*>(expectedHost)) ||
        s.request.output!=VehicleWindowOutput50DS::defaultBackBuffer || s.active || s.frameSet || s.diagnostic ||
        !s.imageCommitted || width<=0 || height<=0 || width!=s.committedWidth || height!=s.committedHeight ||
        std::uint64_t(width)*std::uint64_t(height)>std::numeric_limits<std::size_t>::max()/4u)
        return fail(d,VehicleWindowError50DS::readback,"window-buffer readback requires idle real window owner and matching committed drawable");
    d.readbackWidth=width;d.readbackHeight=height;
    d.readbackBytes=std::size_t(width)*std::size_t(height)*4u; // Existing cold capacity bounds.
    if(!destination || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"window RGBA destination is null or too small");
    s.diagnostic=&d;
    bool accepted=s.check("before actual window framebuffer readback");
    I previousRead=0,defaultRead=0,previousPackBuffer=0;
    constexpr U packNames[]={0x0D05,0x0D02,0x0D03,0x0D04,0x0D00,0x0D01,0x806C,0x806B};
    I previousPack[8]{};
    bool restore=false;
    if(accepted){
        s.gl.getIntegerv(0x8CAA,&previousRead); // GL_READ_FRAMEBUFFER_BINDING
        s.gl.getIntegerv(0x88ED,&previousPackBuffer); // GL_PIXEL_PACK_BUFFER_BINDING
        for(unsigned i=0;i<8;++i)s.gl.getIntegerv(packNames[i],&previousPack[i]);
        accepted=s.check("query prior window readback binding");
    }
    if(accepted){
        s.gl.bindFramebuffer(readFramebuffer,0);s.gl.getIntegerv(0x0C02,&defaultRead);
        restore=true;accepted=s.check("query default framebuffer read buffer");
    }
    if(accepted){
        s.gl.readBuffer(front?0x0404:0x0405); // GL_FRONT after swap / GL_BACK strictly before swap.
        s.drawGl.bindBuffer(pixelPackBuffer,0);
        s.gl.pixelStorei(0x0D05,1);s.gl.pixelStorei(0x0D02,0);
        s.gl.pixelStorei(0x0D03,0);s.gl.pixelStorei(0x0D04,0);
        s.gl.pixelStorei(0x0D00,0);s.gl.pixelStorei(0x0D01,0);
        s.gl.pixelStorei(0x806C,0);s.gl.pixelStorei(0x806B,0);
        accepted=s.check("configure actual window RGBA readback");
    }
    if(accepted){
        const U status=s.gl.checkFramebufferStatus(readFramebuffer);
        accepted=s.check("check actual window readback framebuffer");
        if(accepted&&status!=framebufferComplete){d.framebufferStatus=status;
            accepted=fail(d,VehicleWindowError50DS::readback,"actual window framebuffer is incomplete");}
    }
    if(accepted){d.readbackBufferMayBeModified=true;
        s.gl.readPixels(0,0,width,height,rgba,unsignedByte,destination);
        accepted=s.check(front?"read actual swapped window front-buffer RGBA bytes":
            "read actual pre-swap window back-buffer RGBA bytes");}
    if(restore){
        s.gl.readBuffer(static_cast<U>(defaultRead));s.gl.bindFramebuffer(readFramebuffer,static_cast<U>(previousRead));
        for(unsigned i=0;i<8;++i)s.gl.pixelStorei(packNames[i],previousPack[i]);
        s.drawGl.bindBuffer(pixelPackBuffer,static_cast<U>(previousPackBuffer));
        const bool restored=s.check("restore window readback binding");accepted=accepted&&restored;
    }
    if(!accepted&&d.error==VehicleWindowError50DS::gl)d.error=VehicleWindowError50DS::readback;
    s.diagnostic=nullptr;return accepted;
}

bool VehicleWindowGl50DS::finishSubmittedWork(VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if(s.rcParticleWindow&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"RC completion requires the idle creating thread");
    if((s.gbParticleWindow||s.mcParticleWindow)&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"GB particle transaction requires idle creating thread");
    Storage::RcParticleAttemptScope rcParticleAttemptScope{s,d};
    Storage::GbParticleAttemptScope gbParticleAttemptScope{s,d};
    Storage::McParticleAttemptScope mcParticleAttemptScope{s,d};
    if (!s.current(d)) return false;
    if (!s.imageCommitted)
        return fail(d, VehicleWindowError50DS::noImage, "no privately committed frame to finish");
    s.diagnostic = &d;
    bool result = s.check("before explicit submitted-work completion");
    if (result) {
        s.gl.finish();
        result = s.check("complete prior submitted GL work");
    }
    d.submittedWorkCompleted = result;
    if(result)s.acceptCompletedTrackVisibility();
    else s.trackVisibilityPendingTarget=0;
    s.diagnostic = nullptr;
    if(result)result=s.completeRcParticleAttempt(d);
    if(result)result=s.completeGbParticleAttempt(d);
    if(result)result=s.completeMcParticleAttempt(d);
    return result;
}

bool VehicleWindowGl50DS::completedRaStaticFire(
    ra_static_fire_visual50LA::CompletedDrawReceipt& out,VehicleWindowDiagnostic50DS& d) const noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    out={};d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"FireP0t completion requires live renderer");
    const auto& s=*storage_;
    if(!s.raStaticFireOwner||s.raStaticFireRenderer!=this||
       s.raStaticFireOwner->rendererPublication_!=this||
       s.raStaticFirePublicationSerial!=s.raStaticFireOwner->rendererPublicationSerial_)
        return fail(d,VehicleWindowError50DS::material,"FireP0t publication identity differs");
    // Reuse the real idle/current/GPU-completion boundary. These outputs are
    // private temporaries, never caller-supplied claims of submitted geometry.
    const std::uint32_t indices[4]={0,1,2,3};std::uint8_t submitted[4]{};
    VehicleWindowTrackVisibilityReceipt50DS receipt;
    if(!completedTrackSpanVisibility(indices,submitted,4,receipt,d)||!s.texturesCurrent(d))return false;
    if(!s.raStaticFireOwner->nativeCompletionLive_||receipt.sourceSpanCount!=4||receipt.completedGameplayGeneration!=s.raStaticFireOwner->nativeCompletedGeneration_)
        return fail(d,VehicleWindowError50DS::frame,"FireP0t native completed generation differs");
    for(U i=0;i<4;++i)if(bool(submitted[i])!=s.raStaticFireOwner->nativeSubmitted_[i])
        return fail(d,VehicleWindowError50DS::frame,"FireP0t completed span evidence differs");
    out.owner_=s.raStaticFireOwner;out.publication_=s.raStaticFirePublicationSerial;
    out.generation_=receipt.completedGameplayGeneration;return true;
}

bool VehicleWindowGl50DS::completedTrackSpanVisibility(const std::uint32_t* indices,
    std::uint8_t* submitted,std::uint32_t count,VehicleWindowTrackVisibilityReceipt50DS& receipt,
    VehicleWindowDiagnostic50DS& d) const noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};receipt={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"track visibility requires initialized renderer");
    const auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if(s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"track visibility requires idle renderer");
    if(!s.trackVisibilityCompletedValid)
        return fail(d,VehicleWindowError50DS::noImage,"no completed gameplay track visibility receipt");
    if(count>s.trackSpanCount || (count && (!indices || !submitted)))
        return fail(d,VehicleWindowError50DS::frame,"track visibility output or count is invalid");
    for(U i=0;i<count;++i)if(indices[i]>=s.trackSpanCount)
        return fail(d,VehicleWindowError50DS::frame,"track visibility source span index is outside the cold table");
    for(U i=0;i<count;++i)submitted[i]=s.trackSpans[indices[i]].submittedCompletedGameplay?1u:0u;
    receipt.completedGameplayGeneration=s.trackVisibilityCompletedGeneration;
    receipt.sourceSpanCount=s.trackSpanCount;return true;
}

namespace {
bool sameDisplayFadeSample(const game::OriginalVideoBlockingSample& a,
    const game::OriginalVideoBlockingSample& b) noexcept {
    return a.direction==b.direction && a.ordinal==b.ordinal && a.gsRgbaq==b.gsRgbaq &&
        a.alpha==b.alpha && a.sourceCopyPC==b.sourceCopyPC;
}
}
OriginalVideoBlockingNativeBackend::~OriginalVideoBlockingNativeBackend() noexcept {
    // Teardown is cancellation, never successful completion of the fade.
    if(!cancelAndRetire())std::terminate();
}
game::OriginalCameraStartResult OriginalVideoBlockingNativeBackend::failure(std::uint32_t pc) const noexcept {
    return {diagnostic_.error==VehicleWindowError50DS::noImage?
        game::OriginalCameraStartStatus::rendererNoImage:game::OriginalCameraStartStatus::videoBackend,pc};
}
bool OriginalVideoBlockingNativeBackend::ownersCurrent() noexcept {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return false;
    if(!renderer_.initialized())return fail(diagnostic_,VehicleWindowError50DS::notInitialized,"display fade requires live renderer");
    auto& s=*renderer_.storage_;
    if(s.frontendOnly)return fail(diagnostic_,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay fade capability");
    if(!s.current(diagnostic_,this))return false;
    if(s.request.context!=&host_ || ((s.request.output==VehicleWindowOutput50DS::defaultBackBuffer)!=window_))
        return fail(diagnostic_,VehicleWindowError50DS::context,"display fade requires renderer's exact current GameHost50ER");
    if(s.active || s.frameSet || s.diagnostic)
        return fail(diagnostic_,VehicleWindowError50DS::frame,"display fade requires idle non-reentrant renderer");
    return checkHost_(host_,window_,width_,height_,diagnostic_);
}
game::OriginalCameraStartResult OriginalVideoBlockingNativeBackend::begin(
    game::OriginalVideoFadeDirection direction,const game::OriginalVideoFadeState& video) {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return {game::OriginalCameraStartStatus::owner,0};
    using S=game::OriginalCameraStartStatus;
    const bool async=use_==OriginalVideoNativeUse::asyncFrozenUnderlay25B5D0;
    const auto pc=async?0x25e610u:(direction==game::OriginalVideoFadeDirection::out260850?0x260850u:0x261630u);
    if(phase_==OriginalVideoNativePhase::failed)return failure(pc);
    diagnostic_={};hostDiagnostic_={};
    if(phase_==OriginalVideoNativePhase::sourceReady && direction==direction_)
        return ownersCurrent()?game::OriginalCameraStartResult{S::complete,pc}:failure(pc);
    if((phase_!=OriginalVideoNativePhase::idle && phase_!=OriginalVideoNativePhase::retired) ||
       (use_!=OriginalVideoNativeUse::blockingSourceLoops && !async) ||
       (direction!=game::OriginalVideoFadeDirection::out260850 && direction!=game::OriginalVideoFadeDirection::in261630) ||
       (async?((video.flags404Low6&7u)!=(direction==game::OriginalVideoFadeDirection::out260850?5u:6u)):
           bool(video.flags404Low6&4u)) ||
       !(video.flags404Low6&(direction==game::OriginalVideoFadeDirection::out260850?1u:2u)) ||
       (direction==game::OriginalVideoFadeDirection::out260850 && (video.flags404Low6&32u)))
        return {S::sourcePath,pc};
    if(phase_==OriginalVideoNativePhase::retired)width_=height_=0;
    if(!ownersCurrent())return failure(pc);
    auto& s=*renderer_.storage_;
    if(!s.imageCommitted || !s.raceWidth || !s.raceHeight ||
       s.displayedTarget!=s.targets[s.published] || s.committedWidth!=s.raceWidth || s.committedHeight!=s.raceHeight){
        fail(diagnostic_,VehicleWindowError50DS::noImage,"display fade requires current real race image, never UI or invented cold image");
        return failure(pc);
    }
    width_=s.raceWidth;height_=s.raceHeight;
    if(!ownersCurrent()){width_=height_=0;return failure(pc);}
    s.diagnostic=&diagnostic_;
    bool accepted=s.check("before native display-fade begin") && s.loadDisplayFadeFunctions();
    if(!accepted){s.diagnostic=nullptr;width_=height_=0;return failure(pc);}
    // Install the exclusive borrow before every fallible allocation/copy.
    // Failed work retains it; only real retirement/cancellation unlocks it.
    direction_=direction;nextOrdinal_=windowSwaps_=headlessCompletions_=0;last_={};pending_={};pendingBank_=0;
    s.displayFadeOwner=this;phase_=OriginalVideoNativePhase::failed;
    s.gl.finish();accepted=s.check("complete race source before display-fade snapshot");
    if(accepted && !s.deleteDisplayFadeNames(diagnostic_))
        accepted=fail(diagnostic_,VehicleWindowError50DS::cleanup,"display-fade cached name deletion remains pending");
    I textureLimit=0,textureUnits=0;
    if(accepted){
        s.gl.getIntegerv(0x0D33,&textureLimit);s.gl.getIntegerv(0x8872,&textureUnits);
        accepted=s.check("query native display-fade texture limits");
        if(accepted && (width_>textureLimit || height_>textureLimit || textureUnits<1))
            accepted=fail(diagnostic_,VehicleWindowError50DS::limits,"display-fade texture dimensions or unit unavailable");
    }
    if(accepted)accepted=s.createDisplayFadeTarget(s.displayFadeSourceTarget,s.displayFadeSourceColor,width_,height_) &&
        s.createDisplayFadeTarget(s.displayFadeTargets[0],s.displayFadeColors[0],width_,height_) &&
        s.createDisplayFadeTarget(s.displayFadeTargets[1],s.displayFadeColors[1],width_,height_) && s.createDisplayFadeProgram();
    if(accepted){
        s.gl.disable(0x0C11);s.gl.disable(0x8DB9);
        s.gl.bindFramebuffer(readFramebuffer,s.targets[s.published]);s.gl.readBuffer(colorAttachment);
        s.gl.bindFramebuffer(drawFramebuffer,s.displayFadeSourceTarget);s.gl.drawBuffer(colorAttachment);
        s.gl.blitFramebuffer(0,0,width_,height_,0,0,width_,height_,colorBufferBit,0x2600);
        accepted=s.check("snapshot actual committed race into distinct immutable display-fade source");
    }
    const bool restored=s.restoreDisplayFadeBindings(width_,height_);accepted=accepted && restored;
    if(accepted){s.gl.finish();accepted=s.check("complete immutable display-fade source snapshot");}
    if(accepted){diagnostic_.submittedWorkCompleted=true;phase_=OriginalVideoNativePhase::sourceReady;}
    s.diagnostic=nullptr;
    return accepted?game::OriginalCameraStartResult{S::complete,pc}:failure(pc);
}
game::OriginalCameraStartResult OriginalVideoBlockingNativeBackend::copyDrawAndComplete(
    const game::OriginalVideoBlockingSample& sample) {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return {game::OriginalCameraStartStatus::owner,0};
    using S=game::OriginalCameraStartStatus;
    const bool async=use_==OriginalVideoNativeUse::asyncFrozenUnderlay25B5D0;
    const auto pc=async?0x25e610u:(direction_==game::OriginalVideoFadeDirection::out260850?0x262c70u:0x262d80u);
    if(phase_==OriginalVideoNativePhase::failed)return failure(pc);
    diagnostic_={};hostDiagnostic_={};
    if(sample.direction!=direction_ || sample.sourceCopyPC!=(async?0u:pc) ||
       (phase_!=OriginalVideoNativePhase::sourceReady && phase_!=OriginalVideoNativePhase::presentationPending))
        return {S::owner,pc};
    // An acknowledged exact retry never recopies, redraws, or swaps twice.
    if(nextOrdinal_ && sameDisplayFadeSample(sample,last_))return {S::complete,pc};
    if(sample.ordinal!=nextOrdinal_ || nextOrdinal_==std::numeric_limits<std::uint64_t>::max() ||
       (phase_==OriginalVideoNativePhase::presentationPending && !sameDisplayFadeSample(sample,pending_)))
        return {S::owner,pc};
    if(!ownersCurrent())return failure(pc);
    auto& s=*renderer_.storage_;
    if(s.displayFadeOwner!=this)return {S::owner,pc};
    if(phase_==OriginalVideoNativePhase::sourceReady){
        pending_=sample;pendingBank_=static_cast<std::uint8_t>(nextOrdinal_&1u);
        s.diagnostic=&diagnostic_;
        bool accepted=s.check("before display-fade source recopy");
        if(accepted){
            // Blocking262C70/262D80 or explicit native async retained-image
            // recopy EVERY sample. Async sourceCopyPC=0 makes no blocking-PC
            // attribution. Never sample an effect or previous sample.
            s.gl.disable(0x0C11);s.gl.disable(0x8DB9);
            s.gl.bindFramebuffer(readFramebuffer,s.displayFadeSourceTarget);s.gl.readBuffer(colorAttachment);
            s.gl.bindFramebuffer(drawFramebuffer,s.displayFadeTargets[pendingBank_]);s.gl.drawBuffer(colorAttachment);
            s.gl.blitFramebuffer(0,0,width_,height_,0,0,width_,height_,colorBufferBit,0x2600);
            accepted=s.check("recopy immutable race source for display-fade sample");
        }
        if(accepted){
            s.gl.bindFramebuffer(framebuffer,s.displayFadeTargets[pendingBank_]);
            s.gl.drawBuffer(colorAttachment);s.gl.readBuffer(colorAttachment);
            if(s.gl.clipControl)s.gl.clipControl(0x8CA1,0x935E);
            s.gl.viewport(0,0,width_,height_);
            constexpr U disabled[]={0x0BE2,0x0B44,0x0C11,0x0B90,0x8037,0x0B20,
                0x8F9D,0x0BD0,0x809D,0x80A0,0x809E,0x809F,0x8E51,0x8C89,0x8DB9,0x864F,0x0BF2,0x0B71};
            for(U capability:disabled)s.gl.disable(capability);
            for(I i=0;i<s.clipDistances;++i)s.gl.disable(0x3000+static_cast<U>(i));
            s.gl.colorMask(1,1,1,1);s.gl.depthMask(0);s.gl.polygonMode(0x0408,0x1B02);
            s.gl.useProgram(s.displayFadeProgram);s.drawGl.bindVertexArray(s.displayFadeVertexArray);
            s.gl.activeTexture(textureUnit0);s.gl.bindTexture(texture2D,s.displayFadeSourceColor);
            if(s.gl.bindSampler)s.gl.bindSampler(0,0);
            s.gl.uniform1i(s.displayFadeImageUniform,0);
            s.gl.uniform4f(s.displayFadeColorUniform,float(sample.gsRgbaq&255u),float((sample.gsRgbaq>>8)&255u),
                float((sample.gsRgbaq>>16)&255u),float((sample.gsRgbaq>>24)&255u));
            accepted=s.check("bind immutable race source and actual packed RGBAQ bytes");
            if(accepted){s.gl.drawArrays(0x0004,0,3);accepted=s.check("draw native full-race display-fade sample");}
        }
        // Exact final effect image, staged before its real finish/swap.
        if(accepted)accepted=s.stageGameplayDisplayedHistory(s.displayFadeTargets[pendingBank_],width_,height_);
        const bool restored=s.restoreDisplayFadeBindings(width_,height_);accepted=accepted && restored;
        if(accepted){s.gl.finish();accepted=s.check("complete private display-fade sample");}
        if(accepted && window_){
            accepted=s.checkDefaultFramebuffer();
            if(accepted){
                s.gl.bindFramebuffer(readFramebuffer,s.displayFadeTargets[pendingBank_]);s.gl.readBuffer(colorAttachment);
                s.gl.bindFramebuffer(drawFramebuffer,0);s.gl.drawBuffer(backBuffer);
                accepted=s.check("bind completed display-fade sample for window copy");
            }
            if(accepted){
                diagnostic_.defaultFramebufferMayBeModified=true;
                s.gl.blitFramebuffer(0,0,width_,height_,0,0,width_,height_,colorBufferBit,0x2600);
                accepted=s.check("copy completed display-fade sample to real window back buffer");
            }
            const bool windowRestored=s.restoreDisplayFadeBindings(width_,height_);accepted=accepted && windowRestored;
            if(accepted){s.gl.finish();accepted=s.check("complete display-fade back-buffer copy before swap");}
        }
        s.diagnostic=nullptr;
        if(!accepted){phase_=OriginalVideoNativePhase::failed;return failure(pc);}
        diagnostic_.submittedWorkCompleted=true;phase_=OriginalVideoNativePhase::presentationPending;
    }
    if(window_){
        // Actual50DQ swap returns false only before SDL_GL_SwapWindow is
        // called. A false result therefore leaves this SAME sample pending;
        // it may retry the swap, without repeating draw or back-buffer copy.
        if(!swapHost_(host_,hostDiagnostic_)){
            fail(diagnostic_,VehicleWindowError50DS::presentation,"GameHost50ER display-fade swap not performed");
            return failure(pc);
        }
        ++windowSwaps_;
    }else ++headlessCompletions_;
    // CPU-only acceptance after actual display completion, never another swap.
    s.acceptGameplayDisplayedHistory();
    // No fallible operation follows a successful swap. Publish only a fully
    // completed image; alternating surfaces preserve the previous committed
    // sample across every failed write. Source race banks remain untouched.
    s.displayedTarget=s.displayFadeTargets[pendingBank_];s.imageCommitted=true;
    s.width=s.committedWidth=width_;s.height=s.committedHeight=height_;
    s.uiPrivatePublicationGeneration=0;s.uiSourcePublishedTargetGeneration=0;
    last_=sample;++nextOrdinal_;phase_=OriginalVideoNativePhase::sourceReady;
    return {S::complete,pc};
}
bool OriginalVideoBlockingNativeBackend::retire(bool cancelled) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return false;
    if(!renderer_.storage_ || renderer_.storage_->displayFadeOwner!=this){
        if(phase_==OriginalVideoNativePhase::idle || phase_==OriginalVideoNativePhase::retired ||
           phase_==OriginalVideoNativePhase::cancelled)return true;
        return fail(diagnostic_,VehicleWindowError50DS::frame,"display-fade retirement lost its actual renderer owner");
    }
    auto& s=*renderer_.storage_;
    // Cancellation must remain possible after a resize. It changes no window
    // or private publication; it only drains work and restores native state.
    if(!renderer_.initialized() || !s.current(diagnostic_,this) || s.active || s.frameSet || s.diagnostic)return false;
    s.diagnostic=&diagnostic_;
    bool accepted=s.check("before display-fade retirement");
    if(accepted)accepted=s.restoreDisplayFadeBindings(width_,height_);
    if(accepted){s.gl.finish();accepted=s.check("complete display-fade state restoration and retirement");}
    if(accepted && s.displayFadeDeletionPending && !s.deleteDisplayFadeNames(diagnostic_))
        accepted=fail(diagnostic_,VehicleWindowError50DS::cleanup,"display-fade retirement still owns unresolved deleted names");
    s.diagnostic=nullptr;
    if(!accepted)return false;
    diagnostic_.submittedWorkCompleted=true;s.displayFadeOwner=nullptr;
    phase_=cancelled?OriginalVideoNativePhase::cancelled:OriginalVideoNativePhase::retired;
    return true;
}
game::OriginalCameraStartResult OriginalVideoBlockingNativeBackend::restoreAndComplete(
    game::OriginalVideoFadeDirection direction) {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return {game::OriginalCameraStartStatus::owner,0};
    using S=game::OriginalCameraStartStatus;
    const auto pc=use_==OriginalVideoNativeUse::asyncFrozenUnderlay25B5D0?0x25e610u:
        (direction_==game::OriginalVideoFadeDirection::out260850?0x261594u:0x261df0u);
    if(direction!=direction_)return {S::owner,pc};
    if(phase_==OriginalVideoNativePhase::retired)return {S::complete,pc};
    // Failed/unpublished samples must never let the numeric owner publish
    // final alpha/flags. Only the separate cancellation method can retire it.
    if(phase_==OriginalVideoNativePhase::failed)return failure(pc);
    if(phase_!=OriginalVideoNativePhase::sourceReady && phase_!=OriginalVideoNativePhase::restoring)
        return {S::owner,pc};
    diagnostic_={};phase_=OriginalVideoNativePhase::restoring;
    return retire(false)?game::OriginalCameraStartResult{S::complete,pc}:failure(pc);
}
bool OriginalVideoBlockingNativeBackend::cancelAndRetire() noexcept {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return false;
    diagnostic_={};return retire(true);
}

bool VehicleWindowGl50DS::idleForRaceReset50LA(VehicleWindowDiagnostic50DS& d) const noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"race reset requires initialized renderer");
    const auto& s=*storage_;
    if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"race reset requires idle native submission ownership");
    return true;
}

bool VehicleWindowGl50DS::frontendCameraViewportIdle50KG(VehicleWindowDiagnostic50DS& d) const noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    if(!idleForRaceReset50LA(d))return false;
    const auto& s=*storage_;
    // Sealed Main/Rider/Career RGBA frames own pixels, not source Scene0.
    // They do not execute26F940 and must not invalidate canonical selection.
    // Gameplay uses a different creation path and is not admitted here.
    if(!s.frontendOnly||s.nativeGlFaultObserved50KG||s.frameSet)
        return fail(d,VehicleWindowError50DS::frame,"First Camera requires untouched native frontend scene selection");
    return true;
}

bool VehicleWindowGl50DS::garageFirstCameraViewportIdle50KG(VehicleWindowDiagnostic50DS& d) const noexcept {
    return frontendCameraViewportIdle50KG(d); // Same historical Garage predicate.
}

bool VehicleWindowGl50DS::frontendLocalizationIdle50KG(VehicleWindowDiagnostic50DS& d) const noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    if(!idleForRaceReset50LA(d))return false;
    const auto& s=*storage_;
    if(!s.frontendOnly||s.nativeGlFaultObserved50KG)
        return fail(d,VehicleWindowError50DS::frame,"localization requires the retained no-error frontend renderer");
    // Do not accept closed/replaced UI banks merely because the GL context is
    // still current. Their real context/publication borrows must remain valid.
    const auto* cpu=s.request.originalUiCpuTextures;
    const auto* gpu=s.request.originalUiTextures;
    if(!cpu||!cpu->loaded()||!gpu||!cpu->textureCount()||cpu->textureCount()!=gpu->textureCount()||
       !gpu->validBorrow(s.request.context,s.request.isCurrent,s.uiTexturePublication))
        return fail(d,VehicleWindowError50DS::material,"localization requires the exact live UI bank publication");
    if(gpu->riderSparse()){
        const auto* bank=gpu->riderBank();const auto images=bank?bank->images():nullptr;
        if(s.riderTextureBorrow!=gpu||!gpu->riderSession()||!images||images->textures()!=cpu)
            return fail(d,VehicleWindowError50DS::material,"localization requires the actual retained sparse Rider bank");
        for(U i=0;i<cpu->textureCount();++i){
            const auto* source=cpu->texture(i);const auto* image=gpu->asset(i);
            if(!source||(image&&(!image->name||image->identity!=source->identity)))
                return fail(d,VehicleWindowError50DS::material,"localization sparse UI identity changed");
        }
    }else if(!gpu->matchesSourceBank(*cpu,s.uiTexturePublication))
        return fail(d,VehicleWindowError50DS::material,"localization CPU/UI upload identity changed");
    return true;
}

bool VehicleWindowGl50DS::optionsViewportReservedCurrent50KG(const OriginalOptionsMutationInterlock50KG& owner)const noexcept {
    if(!storage_||!initialized())return false;
    const auto& s=*storage_;
    return owner.current(s.request.context)&&s.thread==std::this_thread::get_id()&&
        s.frontendOnly&&!s.nativeGlFaultObserved50KG&&!s.active&&!s.frameSet&&!s.diagnostic&&
        !s.displayFadeOwner&&!s.menuRasterShared50KG&&!s.garagePollFence50KG;
}
bool VehicleWindowGl50DS::optionsViewportIdle50KG(VehicleWindowDiagnostic50DS& d)const noexcept {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted()||!idleForRaceReset50LA(d))return false;
    return storage_->frontendOnly&&!storage_->nativeGlFaultObserved50KG&&!storage_->frameSet;
}
bool VehicleWindowGl50DS::finishOptionsViewportWork50KG(
    const OriginalOptionsMutationInterlock50KG& owner,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation operation(owner);
    d={};if(!operation.admitted()||!initialized())return fail(d,VehicleWindowError50DS::frame,"actual Options reservation required");
    auto& s=*storage_;
    if(!owner.current(s.request.context)||!s.current(d,nullptr,nullptr,&owner)||
       s.active||s.frameSet||s.diagnostic||!s.frontendOnly||s.nativeGlFaultObserved50KG||
       s.contextDrainGeneration==std::numeric_limits<std::uint64_t>::max())return false;
    s.diagnostic=&d;
    bool okay=s.check("before Options initialized-context completion");
    if(okay){s.gl.finish();okay=s.check("complete Options initialized-context work");}
    if(okay){++s.contextDrainGeneration;d.contextWorkCompleted=true;d.contextDrainGeneration=s.contextDrainGeneration;}
    s.diagnostic=nullptr;return okay;
}
bool VehicleWindowGl50DS::finishInitializedContextWork(VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.current(d)) return false;
    if (s.contextDrainGeneration == std::numeric_limits<std::uint64_t>::max())
        return fail(d, VehicleWindowError50DS::submission, "context drain generation exhausted");
    s.diagnostic = &d;
    bool result = s.check("before explicit initialized-context completion");
    if (result) {
        s.gl.finish();
        result = s.check("complete prior initialized-context GL work");
    }
    if (result) {
        ++s.contextDrainGeneration;
        d.contextWorkCompleted = true;
        d.contextDrainGeneration = s.contextDrainGeneration;
    }
    s.diagnostic = nullptr;
    return result;
}

bool VehicleWindowGl50DS::completeGameplayDisplayImpl(GameHost50ER& host,
    GameHostDiagnostic50ER& hostDiagnostic,
    bool (*checkHost)(GameHost50ER&,bool,I,I,VehicleWindowDiagnostic50DS&) noexcept,
    bool (*swapHost)(GameHost50ER&,GameHostDiagnostic50ER&) noexcept,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"gameplay display requires initialized renderer");
    auto& s=*storage_;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if(s.rcParticleWindow&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"RC display completion requires the idle creating thread");
    if((s.gbParticleWindow||s.mcParticleWindow)&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"GB particle transaction requires idle creating thread");
    Storage::RcParticleAttemptScope rcParticleAttemptScope{s,d};
    Storage::GbParticleAttemptScope gbParticleAttemptScope{s,d};
    Storage::McParticleAttemptScope mcParticleAttemptScope{s,d};
    if(!s.current(d))return false;
    const bool window=s.request.output==VehicleWindowOutput50DS::defaultBackBuffer;
    if(!s.request.enableOriginalGameplayBlur || s.request.context!=static_cast<void*>(&host) ||
        !checkHost || !swapHost)
        return fail(d,VehicleWindowError50DS::context,"gameplay display requires opted-in renderer's exact current GameHost");
    if(s.active || s.frameSet || s.diagnostic || !s.imageCommitted ||
        !s.gameplayAwaitingTarget || s.gameplayAwaitingTarget!=s.displayedTarget ||
        s.gameplayAwaitingWidth!=s.committedWidth || s.gameplayAwaitingHeight!=s.committedHeight ||
        (s.displayedTarget!=s.targets[s.published] && s.displayedTarget!=s.menuTargets[s.menuPublished]))
        return fail(d,VehicleWindowError50DS::frame,"gameplay display needs new successful race or host-menu publication");
    if(!checkHost(host,window,s.committedWidth,s.committedHeight,d))return false;
    s.diagnostic=&d;
    const auto stageHistory=[&]() noexcept {
        GameplayDisplayWallScope50DS timing(s.request.enableGameplayDisplayWallTiming,d.gameplayHistoryStageWall);
        return s.stageGameplayDisplayedHistory(s.displayedTarget,s.committedWidth,s.committedHeight);
    };
    bool accepted=s.check("before actual gameplay display completion") && stageHistory();
    // All fallible GPU work precedes the real swap. The staged image is an
    // independent copy, so no subsequent frame can mutate accepted history.
    s.gl.bindFramebuffer(framebuffer,0);
    const bool restored=s.check("restore framebuffer before gameplay display completion");
    accepted=accepted && restored;
    if(accepted){
        {
            GameplayDisplayWallScope50DS timing(s.request.enableGameplayDisplayWallTiming,d.gameplayFinishWall);
            s.gl.finish();
        }
        accepted=s.check("complete final gameplay image and staged history before display");
    }
    d.submittedWorkCompleted=accepted;
    s.diagnostic=nullptr;
    if(!accepted){s.trackVisibilityPendingTarget=0;return false;}
    const auto swapWindow=[&]() noexcept {
        GameplayDisplayWallScope50DS timing(s.request.enableGameplayDisplayWallTiming,d.gameplaySwapWall);
        return swapHost(host,hostDiagnostic);
    };
    if(window && !swapWindow()) {
        s.trackVisibilityPendingTarget=0;
        return fail(d,VehicleWindowError50DS::presentation,"GameHost gameplay window swap was not submitted");
    }
    if(s.displayedTarget==s.targets[s.published])s.retainedRaceDisplayCompleted=true;
    s.acceptGameplayDisplayedHistory();
    s.acceptCompletedTrackVisibility();
    if(!s.completeRcParticleAttempt(d)||!s.completeGbParticleAttempt(d)||!s.completeMcParticleAttempt(d))return false;
    d.gameplayDisplayHistoryAccepted=true;
    d.gameplayDisplayHistoryGeneration=s.gameplayHistoryGeneration;
    d.gameplayWindowSwapSubmitted=window;d.gameplayHeadlessDisplayCompleted=!window;
    return true;
}

bool VehicleWindowGl50DS::clearGameplayBlurStrengths(VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"gameplay blur clear requires initialized renderer");
    auto& s=*storage_;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if(!s.current(d))return false;
    if(!s.request.enableOriginalGameplayBlur || s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"gameplay blur clear requires idle opted-in renderer");
    game::clearGameplayBlurVideoStrengths(s.gameplayBlurVideo);
    return true;
}

void VehicleWindowGl50DS::logRaCapturePixelProvenance() noexcept {
    OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return;
    if(!initialized())return;
    auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic||
       !s.imageCommitted||!s.raWorld||!s.raWorldFramePrepared)return;
    VehicleWindowDiagnostic50DS observation;
    if(!s.current(observation)||!s.raWorldFrameCurrent(observation))return;
    s.traceRaPixelProvenance();
}

bool VehicleWindowGl50DS::close(VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){d.cleanupIncomplete=bool(storage_);return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    // An admitted close attempt ends this source-only geometry observation,
    // even when dependent-owner or cleanup preflight subsequently rejects.
    // An Options-interlock rejection above never enters close/mutates state.
    if(storage_)storage_->nativeSkyMeshIdentity129280=nullptr;
    if(sessionDependentPins50LA_){
        d.cleanupIncomplete=true;
        return fail(d,VehicleWindowError50DS::request,"dependent primitive retirement must precede renderer close");
    }
    if (!storage_) return true;
    if(storage_->menuRasterShared50KG){d.cleanupIncomplete=true;
        return fail(d,VehicleWindowError50DS::cleanup,"native menu raster still owns renderer resources");}
    storage_->uiDirectVideoLifetime.reset();
    const bool result = storage_->cleanup(d);
    if (!d.cleanupIncomplete) {
        if(storage_->riderTextureBorrow&&!storage_->riderTextureBorrow->releaseRiderBorrow(storage_.get())){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"Rider renderer close borrower mismatch");}
        storage_->riderTextureBorrow=nullptr;storage_.reset();
    }
    return result;
}
bool VehicleWindowGl50DS::initialized() const noexcept { return storage_ && storage_->initialized; }
}
