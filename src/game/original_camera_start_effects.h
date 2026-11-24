#pragma once
#include "camera_view49f.h"
#include "original_camera_scene_tick.h"
#include "lap_host_clock50id.h"
#include "lap_timing50id.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

namespace original_main_entry50KG { struct Application; }
namespace original_camera_session50KX { class AsyncRetainedFade; }
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
//115510/237000 only: preserve the existing view/projection X/Y and screen.
//Ordered native stores are retained even if a later arithmetic step rejects.
OriginalCameraStartResult setOriginalCameraRange237000(float nearPlane,float farPlane,
    const std::array<float,16>& view,const std::array<float,16>& screen,float guardX,float guardY,
    std::array<float,16>& projection,std::array<float,16>& combined,
    std::array<float,16>& combinedScreen,std::array<float,16>& guardProjection,
    std::array<float,16>& guardCombined)noexcept;
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
// One actual2AED60(500,0,0,*) request prefix. Camera T0=0 returns after
// this prefix; TA skip/Results T0=1 must still own the actual260850 wait.
// Retain separately for each real caller; never retry through a fresh cursor.
class OriginalCameraExitFadeRequest2AED60 final {
    OriginalVideoFadeState* video_=nullptr;
    HostMonotonicClock50ID* clock_=nullptr;
    std::chrono::steady_clock::time_point epoch_{};
    OriginalVideoFadeState expected_;
    std::int32_t manager_=0,mode_=0;
    OriginalCameraStartResult result_{};
public:
    OriginalCameraExitFadeRequest2AED60()=default;
    OriginalCameraExitFadeRequest2AED60(const OriginalCameraExitFadeRequest2AED60&)=delete;
    OriginalCameraExitFadeRequest2AED60& operator=(const OriginalCameraExitFadeRequest2AED60&)=delete;
    bool pending()const noexcept{return video_&&result_.status!=OriginalCameraStartStatus::complete;}
    // Same Video, clock/epoch and actual parent arguments. Failed calls retain
    // partial effects and repeat only their cached failure, never the prefix.
    OriginalCameraStartResult invoke(std::int32_t actualFadeManagerA8,std::int32_t actualApplicationMode22C,
        OriginalVideoFadeState&,HostMonotonicClock50ID&)noexcept;
};
enum class OriginalVideoFadeDirection : std::uint8_t { out260850,in261630 };
// One retained ASYNC request: OUT260320(true,false) or IN260610(true).
// Source target/direction/400 stores survive later failure. A retry returns
// the same terminal failure instead of re-entering the newly set no-op bit.
// Pair each instance with one real caller invocation/consumer; never reset it
// to retry failed source work or reuse successful custody for another request.
class OriginalVideoAsyncRequest50LA final {
    OriginalVideoFadeState* video_=nullptr;
    HostMonotonicClock50ID* clock_=nullptr;
    std::chrono::steady_clock::time_point epoch_{};
    OriginalVideoFadeState expected_;
    OriginalVideoFadeDirection direction_=OriginalVideoFadeDirection::out260850;
    std::int32_t mode_=0,duration_=0;
    OriginalCameraStartResult result_{};
public:
    OriginalVideoAsyncRequest50LA()=default;
    OriginalVideoAsyncRequest50LA(const OriginalVideoAsyncRequest50LA&)=delete;
    OriginalVideoAsyncRequest50LA& operator=(const OriginalVideoAsyncRequest50LA&)=delete;
    bool attempted()const noexcept{return video_!=nullptr;}
    bool pending()const noexcept{return attempted()&&result_.status!=OriginalCameraStartStatus::complete;}
    // Borrows the actual Video and one application clock through the entire
    // caller continuation. Shared lastNanoseconds may advance elsewhere.
    // Re-invoking a successful request never returns complete a second time.
    OriginalCameraStartResult invoke(OriginalVideoFadeDirection,OriginalVideoFadeState&,
        HostMonotonicClock50ID&,std::int32_t applicationMode22C,std::int32_t durationMilliseconds)noexcept;
};
// Exact17C5C8 zero-duration/nonasync/no-redraw prefix. Preserves source
// stores and the real400 clock sample before the unresolved3E8 read. Never
// certifies the request's return or invents initial alpha/interval arithmetic.
class OriginalLoadingFadePrefix17C5C8 final {
    friend struct ::original_main_entry50KG::Application;
    // Only a retained one-shot Application continuation may use this helper.
    // It is currently uncalled: the flags9 UI retirement remains unresolved.
    static OriginalCameraStartResult advance(
        OriginalVideoFadeState&,HostMonotonicClock50ID&,std::int32_t applicationMode22C)noexcept;
};
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
    // Same actual application clock and epoch for every pending phase. This
    // is a borrow, not a private time origin; all owners must outlive the
    // entire pending continuation and its required backend retirement.
    HostMonotonicClock50ID* clock_=nullptr;
    std::chrono::steady_clock::time_point clockEpoch_{};
    OriginalVideoFadeDirection direction_=OriginalVideoFadeDirection::out260850;
    unsigned phase_=0;std::uint64_t ordinal_=0;
    std::int32_t startAlpha_=0,alpha_=0,step_=1,distance_=0;
    float startMilliseconds_=0;
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
    // The same borrowed clock identity/epoch is sealed before backend begin.
    // Legitimate shared lastNanoseconds advances remain allowed. A changed
    // epoch/clock or represented Video field rejects before further backend
    // work or sampling, including after a successful synchronous backend return.
    // Completed phases stay advanced; restoring admission cannot replay them.
    // This guard does not cancel or release a pending renderer transaction.
    OriginalCameraStartResult advance(OriginalVideoFadeState&,HostMonotonicClock50ID&,
        OriginalVideoBlockingBackend*);
};
// Numeric continuation of Results1CA778 -> FC2AED60(500,0,0,1), NOT
// admission to Results input or a receipt for its parent call. The lexical
// caller must retain Application/Video/clock/backend and its original input
// batch, hold simulation, and retire/cancel the real backend before releasing
// those owners. A separate instance is required for every source invocation;
// the completed initial-Camera cursor is never reused. The actual retained
// Results input continuation owns this separate cursor and real backend.
class OriginalResultsBlockingFade2AED60 final {
    OriginalCameraExitFadeRequest2AED60 request_;
    OriginalVideoBlockingFade fade_;
    OriginalVideoFadeState* video_=nullptr;
    HostMonotonicClock50ID* clock_=nullptr;
    OriginalVideoBlockingBackend* backend_=nullptr;
    std::chrono::steady_clock::time_point epoch_{};
    bool requested_=false,finished_=false;
    OriginalCameraStartResult requestResult_{};
public:
    OriginalResultsBlockingFade2AED60()=default;
    OriginalResultsBlockingFade2AED60(const OriginalResultsBlockingFade2AED60&)=delete;
    OriginalResultsBlockingFade2AED60& operator=(const OriginalResultsBlockingFade2AED60&)=delete;
    // Source A8 suppression/zero adjusted duration really return without
    //260850. Otherwise complete is possible only after actual restoration.
    OriginalCameraStartResult advance(std::int32_t actualManagerA8,
        std::int32_t actualApplicationMode22C,OriginalVideoFadeState&,
        HostMonotonicClock50ID&,OriginalVideoBlockingBackend*);
    bool complete()const noexcept{return finished_;}
};
// Numeric25E610 arguments only, never a draw/completion receipt.
struct OriginalVideoFadeDraw {
    bool reached25E610=false;
    std::uint32_t argb=0;
};
// One actual25B5D0 invocation. Preserves pre-draw alpha/time stores and waits
// at25E610; completion flags are private post-return effects, not preparation.
// The native retained-image owner alone may join its actual backend return.
class OriginalVideoFadeUpdate25B5D0 final {
    friend class ::original_camera_session50KX::AsyncRetainedFade;
    enum class Phase : std::uint8_t {fresh,clock,draw,complete,failed};
    enum class DrawReturn : std::uint8_t {covered,increasing,decreasing};
    OriginalVideoFadeState* video_=nullptr;
    HostMonotonicClock50ID* clock_=nullptr;
    std::chrono::steady_clock::time_point epoch_{};
    OriginalVideoFadeState expected_;
    std::int32_t view_=0,count_=0;
    Phase phase_=Phase::fresh;
    DrawReturn return_=DrawReturn::covered;
    OriginalVideoFadeDraw draw_{};
    OriginalCameraStartResult result_{};
    static bool sameFields(const OriginalVideoFadeState&,const OriginalVideoFadeState&)noexcept;
    bool current(const OriginalVideoFadeState&,const HostMonotonicClock50ID&)const noexcept;
    bool complete()const noexcept{return phase_==Phase::complete;}
    // No caller-provided boolean/token may acknowledge25E610. Only the
    // retained owner calls this after its real copyDrawAndComplete succeeds.
    OriginalCameraStartResult afterNativeDrawComplete(OriginalVideoFadeState&,HostMonotonicClock50ID&)noexcept;
public:
    OriginalVideoFadeUpdate25B5D0()=default;
    OriginalVideoFadeUpdate25B5D0(const OriginalVideoFadeUpdate25B5D0&)=delete;
    OriginalVideoFadeUpdate25B5D0& operator=(const OriginalVideoFadeUpdate25B5D0&)=delete;
    // Same actual Video, clock/epoch and camera arguments across retries.
    // videoBackend/25E610 means a cached draw boundary, not completed drawing.
    // Numeric/clock failure is cached: no repeated sample or prefix mutation.
    OriginalCameraStartResult advance(OriginalVideoFadeState&,HostMonotonicClock50ID&,
        std::int32_t actualApplicationViewIndex0C,std::int32_t actualCameraCount264,
        OriginalVideoFadeDraw&)noexcept;
};
// One-shot prefix for callers with no draw owner. Complete is possible only
// for no-draw paths. A required25E610 returns videoBackend and preserves only
// its reached prefix. Caller must treat that boundary/failure as terminal;
// use the retained cursor above when a real native draw can later return.
OriginalCameraStartResult advanceOriginalVideoFade25B5D0(
    OriginalVideoFadeState&,HostMonotonicClock50ID&,
    std::int32_t actualApplicationViewIndex0C,std::int32_t actualCameraCount264,
    OriginalVideoFadeDraw&) noexcept;
// Source state prefix1AFD80, through1AD330. The represented manager is the
// actual shared ALL8 manager, not race_mode1_host50ID's one-human host owner.
struct OriginalRaceStartCountdown {
    float countdown34=0; //1AF87C constructor; caller must bind the actual field.
};
// Common rewrite (R2): no course enum. The course's own TOB is the authority for
// every course: 174DF0 reads World+3E4 = TOB [CSCameras].NumberOfCameras (1B28EC).
class OriginalCourseStartActions final {
    bool authenticated_=false;
    std::array<char,3> code_{}; // course code as data (diagnostics / same-course check only)
    // Immutable track authoring. Controller174DF0 applies its own real
    // launch/configuration policy (174EA8); the authored count stays intact.
    std::int32_t sourceSceneCutCount3E4_=-1;
public:
    bool authenticated() const noexcept {return authenticated_;}
    std::string_view code() const noexcept {return {code_.data(),code_[0]?(code_[1]?2u:1u):0u};}
    std::int32_t sourceSceneCutCount3E4() const noexcept {return sourceSceneCutCount3E4_;}
    // One rule for every course: decode the course TOB and read its unique
    // [CSCameras].NumberOfCameras (0..7, the native cut storage). The stock
    // values are receipts only (stock_course_receipts50cr.h role "cuts").
    bool prepare(std::string_view code,const std::uint8_t*,std::size_t) noexcept;
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
    OriginalRaceFogFields&,std::uint32_t& actualRendererFogColor28,bool split=false) noexcept;
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
inline bool matchesRaceHudViewport(const OriginalCameraViewportFields& viewport,unsigned view,
    unsigned count,std::int32_t mode,bool hidden) noexcept {
    if(view>=count||((mode==0)?count!=1u:((mode!=1&&mode!=2)||count!=2u)))return false;
    const bool full=view==0&&viewport.x1E0==0&&viewport.y1E4==0&&
        viewport.width1E8==640&&viewport.height1EC==448;
    if(full)return mode==0||hidden;
    if(mode==1)return viewport.x1E0==0&&viewport.y1E4==224*int(view)&&
        viewport.width1E8==640&&viewport.height1EC==224;
    return mode==2&&viewport.x1E0==320*int(view)&&viewport.y1E4==0&&
        viewport.width1E8==320&&viewport.height1EC==448;
}

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
    std::uint32_t actualRendererBufferCount0C,std::int32_t mode=0,unsigned view=0) noexcept;
//2458C0(a2=0) post-drain numeric zoom writes, then236DD0 matrix/guard fields.
// Renderer current scene0 must be passed separately: source2458C0 updates
// that scene's zoom, which need not be the scene whose viewport was supplied.
OriginalCameraStartResult updateOriginalViewportZoom2458C0(
    const OriginalCameraViewportFields& supplied,OriginalCameraViewportFields& rendererCurrent) noexcept;
//115350's complete numeric core, independent of game-camera bounds318..324.
//Used by the real frontend Camera initializer before those bounds are written.
//No target setter, normalization, renderer call, or lifecycle readiness.
OriginalCameraStartResult rebuildOriginalSceneCore115350(const CameraSceneState49F&,
    const assets::TVCameraVector49E& position,std::int32_t width1E8,
    CameraViewOutput49F&,float& focalPixels1D8) noexcept;
OriginalCameraStartResult computeOriginalGuardAngle2371D0(float focalPixels1D8,
    float& guardAngle330) noexcept;
OriginalCameraStartResult rebuildOriginalRaceScene236DD0(
    OriginalCameraViewportFields&,const CameraSceneState49F&,
    const assets::TVCameraVector49E& actualPosition180,CameraViewOutput49F&) noexcept;
}
