#pragma once
#include "camera_view49f.h"
#include "original_camera_scene_tick.h"
#include "lap_host_clock50id.h"
#include "lap_timing50id.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace rr::game {
// These are additional fields of the ACTUAL application/video/race owners.
// Borrow them across camera calls; do not construct private shadow copies.
enum class OriginalCameraStartStatus : std::uint8_t {
    complete,owner,sourcePath,arithmetic,clock,unknownVideoAlpha,
    rendererDrain,rendererNoImage,rendererCallback,sceneChildren,alreadyComplete,
    videoBackend,videoClockWait,rendererQueue
};
struct OriginalCameraStartResult {
    OriginalCameraStartStatus status=OriginalCameraStartStatus::owner;
    std::uint32_t sourcePC=0;
};
struct OriginalVideoFadeState {
    //259E00 initializes every represented value below EXCEPT3E8. Its first
    //actual assignment is blocking260850/261630 or a reached25B5D0 update.
    std::uint8_t covered3DC=0,flags404Low6=0;
    std::uint32_t toColor3E0=0x80000000u,fromColor3E4=0;
    std::optional<std::int32_t> alpha3E8;
    std::int32_t targetAlpha3EC=128,millisecondsPerStep3F0=0;
    float redSlope3F4=0,greenSlope3F8=0,blueSlope3FC=0,lastMilliseconds400=0;
};
// Reached2AED60(500,0,0,0), actual video virtual+40=260300 and+34=260320.
// A8 suppression and source no-op returns are complete synchronous calls.
// Source260320's nonzero source-render callback and application mode6 assert
// are NOT converted into success. Clock is the same application-wide host
// backend used by101A50;400 retains49J FLOAT milliseconds, not uint32 time.
OriginalCameraStartResult requestOriginalCameraExitFade2AED60(
    std::int32_t actualFadeManagerA8,std::int32_t actualApplicationMode22C,
    OriginalVideoFadeState&,HostMonotonicClock50ID&) noexcept;
enum class OriginalVideoFadeDirection : std::uint8_t { out260850,in261630 };
// The two real video virtual request methods. They do not execute a fade or
// manufacture an alpha value. Unknown constructor3E8 remains explicit.
OriginalCameraStartResult requestOriginalVideoFade260320(
    OriginalVideoFadeState&,HostMonotonicClock50ID&,std::int32_t applicationMode22C,
    std::int32_t durationMilliseconds,bool asynchronous,bool redrawSource);
OriginalCameraStartResult requestOriginalVideoFade260610(
    OriginalVideoFadeState&,HostMonotonicClock50ID&,std::int32_t applicationMode22C,
    std::int32_t durationMilliseconds,bool asynchronous);
//1B7D50->2AEE90(500,1,0). Fresh uncovered video genuinely skips this call.
//When covered, it writes the real manager A8=8/98=500; its later2AE7D0
//consumer requests asynchronous260610. These manager fields are borrowed.
OriginalCameraStartResult requestOriginalFirstMainFade1B7D50(
    const OriginalVideoFadeState&,std::int32_t& actualManagerA8,std::int32_t& actualDuration98) noexcept;

struct OriginalVideoBlockingSample {
    OriginalVideoFadeDirection direction=OriginalVideoFadeDirection::out260850;
    std::uint64_t ordinal=0,gsRgbaq=0;
    std::int32_t alpha=0;
    //Every sample copies the genuine source framebuffer via262C70/262D80.
    //It is not a repeated blend over the previous sample.
    std::uint32_t sourceCopyPC=0;
};
// Explicit platform boundary. There is deliberately no success/no-op backend.
// Implementations must own the actual initialized renderer, retained source
// image, source draw-state restore and completion/presentation synchronization.
// complete means those operations really finished; pending/failure may be
// retried only by the SAME backend transaction, without duplicate publication.
// Existing finishSubmittedWork alone cannot implement a fade sample.
class OriginalVideoBlockingBackend {
public:
    virtual ~OriginalVideoBlockingBackend()=default;
    virtual OriginalCameraStartResult begin(OriginalVideoFadeDirection,const OriginalVideoFadeState&)=0;
    virtual OriginalCameraStartResult copyDrawAndComplete(const OriginalVideoBlockingSample&)=0;
    virtual OriginalCameraStartResult restoreAndComplete(OriginalVideoFadeDirection)=0;
};
class OriginalVideoBlockingFade final {
    OriginalVideoFadeState* video_=nullptr;
    OriginalVideoBlockingBackend* backend_=nullptr;
    OriginalVideoFadeDirection direction_=OriginalVideoFadeDirection::out260850;
    unsigned phase_=0;std::uint64_t ordinal_=0;
    std::int32_t startAlpha_=0,alpha_=0,step_=1,distance_=0;
    float startMilliseconds_=0,waitUntilDelta_=0;
    OriginalVideoFadeState parameters_;
    bool completed_=false;
public:
    OriginalVideoBlockingFade()=default;
    OriginalVideoBlockingFade(const OriginalVideoBlockingFade&)=delete;
    OriginalVideoBlockingFade& operator=(const OriginalVideoBlockingFade&)=delete;
    bool complete()const noexcept{return completed_;}
    //Call at25A9C0's reached25AA4C/25AA78, after its real frame drain and
    //261E80 registered update callbacks. No simulation advance while pending.
    //nullptr is a concrete missing-backend result, never completed rendering.
    OriginalCameraStartResult advance(OriginalVideoFadeState&,HostMonotonicClock50ID&,
        OriginalVideoBlockingBackend*);
};
// Bounded numeric25B5D0 active-fade update. It owns no renderer draw primitive:
// actual25E610 color is returned for the same shared video presentation owner.
struct OriginalVideoFadeDraw {
    bool reached25E610=false;
    std::uint32_t argb=0;
};
OriginalCameraStartResult advanceOriginalVideoFade25B5D0(
    OriginalVideoFadeState&,HostMonotonicClock50ID&,
    std::int32_t actualApplicationViewIndex0C,std::int32_t actualCameraCount264,
    OriginalVideoFadeDraw&) noexcept;
// Source state prefix1AFD80, through1AD330. The represented manager is the
// actual shared ALL8 manager, not race_mode1_host50ID's one-human host owner.
struct OriginalRaceStartCountdown {
    float countdown34=0; //1AF87C constructor; caller must bind the actual field.
};
enum class OriginalCameraStartCourse : std::uint8_t { unspecified,gc,ga };
class OriginalCourseStartActions final {
    bool authenticated_=false;OriginalCameraStartCourse course_=OriginalCameraStartCourse::unspecified;
public:
    bool authenticated() const noexcept {return authenticated_;}
    OriginalCameraStartCourse course() const noexcept {return course_;}
    // Exact GC/GA TOB members. Both lack[321_Anims];1B28EC's0 survives.
    // No arbitrary parsed count/success receipt can claim this source path.
    bool prepare(OriginalCameraStartCourse,const std::uint8_t*,std::size_t) noexcept;
};
OriginalCameraStartResult beginOriginalRaceCountdown1AFD80(
    OriginalRaceStartCountdown&,LapTimingManager50ID& actualManager,
    std::int32_t actualApplication33C,std::int32_t actualLaunchKind11258,
    const OriginalCourseStartActions&) noexcept;
// Source1AFA84..1AFAB4 before1AFE30. Does not invent countdown completion:
// old<=0 requires the actual1AFE30 race/event/sound owner on the NEXT call.
enum class OriginalCountdownStep : std::uint8_t {inactive,advanced,needs1AFE30,arithmetic};
OriginalCountdownStep advanceOriginalRaceCountdown1AFA84(
    OriginalRaceStartCountdown&,const LapTimingManager50ID&,float actualDelta) noexcept;

struct OriginalRaceFogFields {
    std::uint32_t color30=0;
    float start34=0,end38=0,authoredFraction3C=0,authoredFraction40=0;
    float far50=0,near54=0,minimum58=0;
    std::uint32_t dirty60=0;
};
//13E920(4000,bits44A6A99A,64), preserving actual authored fractions3C/40.
OriginalCameraStartResult restoreOriginalRaceFog1A98DC(
    OriginalRaceFogFields&,std::uint32_t& actualRendererFogColor28) noexcept;
//199770(0,1,1,1,1). Source1E0 is deliberately not touched.
void revealOriginalRaceUi199770(std::uint8_t& actualFlags1E4) noexcept;

// Extra numeric fields of the same real scene as CameraSceneState49F and
// OriginalCameraSceneTick. Core/look/up/position are BORROWED, never copied
// into a second independently advancing scene owner.
struct OriginalCameraViewportFields {
    std::uint64_t sessionGeneration=0,sceneKey=0;
    bool prepared=false,matricesReady=false;
    std::int32_t fullWidth204=0,fullHeight208=0;
    float aspect1DC=0;
    std::int32_t x1E0=0,y1E4=0,width1E8=0,height1EC=0;
    float depthNear1F0=0,depthFar1F4=1;
    float left318=0,right31C=0,top320=0,bottom324=0,zoom328=1,zoom32C=1;
    float depth310=1,depth314=0,focalPixels1D8=0,guardAngle330=0,guardX334=0,guardY338=0;
    std::array<float,16> screen140{},combinedScreen250{},guardProjection290{},guardCombined2D0{};
};
// Actual1A939C..1A93B4:640,448,bits3FAAAAAB. Establishes genuine1F8 BEFORE
// any intro composition. This projects the defined post2361C0 bounds/screen
// fields, not its earlier1146B0/114A00 renderer calls. Platform drain/selection
// belongs to the owning cold call. Core/cache fields must not be consumed
// before matricesReady is set by the reached236DD0 below; zero-filled native
// scratch is not evidence that the source wrote those caches in this cut.
OriginalCameraStartResult prepareOriginalRaceViewport2361C0(
    OriginalCameraViewportFields&,std::uint64_t generation,std::uint64_t sceneKey,
    CameraSceneState49F& actualScene) noexcept;
//1158A0 numeric pre-barrier writes; full-window route reached by BOTH fresh
//1A9750(true) and ordinary8C=0 false. Epochs are the same scene-tick owner.
OriginalCameraStartResult setOriginalFullRaceViewport1158A0(
    OriginalCameraViewportFields&,CameraSceneState49F&,OriginalCameraSceneTick&,
    std::uint32_t actualRendererBufferCount0C) noexcept;
//2458C0(a2=0) post-drain numeric zoom writes, then236DD0 matrix/guard fields.
// Renderer current scene0 must be passed separately: source2458C0 updates
// that scene's zoom, which need not be the scene whose viewport was supplied.
OriginalCameraStartResult updateOriginalViewportZoom2458C0(
    const OriginalCameraViewportFields& supplied,OriginalCameraViewportFields& rendererCurrent) noexcept;
OriginalCameraStartResult rebuildOriginalRaceScene236DD0(
    OriginalCameraViewportFields&,const CameraSceneState49F&,
    const assets::TVCameraVector49E& actualPosition180,CameraViewOutput49F&) noexcept;
}
