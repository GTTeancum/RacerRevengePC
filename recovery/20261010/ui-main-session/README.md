# Original Main native session reconstruction

Status: staged, source-reviewed, UNTESTED. No build, syntax check, test, importer,
project parser, gameplay, or guest execution was run by this worker.

## Delivered owner

`files/platform/linux/native/original_ui_main_session50kg.{h,cpp}` joins the
actual authenticated shared49-widget manager, original initial manager lifecycle,
Main entry/opening callbacks, shared aperture primitives, immutable manager camera,
actual Main scene and fire subscribers, concrete authored packet collector, native
material registry drain, and optional concrete native Main audio transport.

Cold resources are produced by `OriginalUiMainDrawResources50KG::load` from actual
appStart, mainMenu, fonts, controllerPopup, overlays, resolved English, the source
material-runtime descriptor and a genuinely owned native video mode. Upload its
exact immutable texture/sampler bank with the native GL owner; pass those same
owners into `OriginalUiMainSession50KG::bind`. Scene, camera and lighting are
prepared from authenticated source bytes inside the session. No GUI labels,
images, shader policies or route readiness are synthesized.

The profile and application subsets are shared with their sole native process
owners. Existing imported profiles supply real field values. A genuinely fresh
native startup can use the narrow source Main reset helper, with an explicitly
native initial backing state; this is not an imported original save. Source reset
sets saved Main slot1 and `(availability11018 & 0x80) | 4`, preserving the high bit.
Entry changes mode34 to4, retaining its old value in previous38 only if changed.
No save-file write or gameplay launch exists. Actual application RNG308030 and
process fireSerial433970 are mutable references, never a per-emitter private RNG.
An initial host-clock seed is an explicit native startup policy if used by the
application; original game seeding has not been claimed.

Enabled audio requires a ready `OriginalUiMainAudio50KG`. Muted mode requires a
null audio owner and increments suppressed-audio receipts at source effect points.
A muted render is allowed, never represented as playback. The enabled frontend
transport uses its own actual frontend mixer and must be closed before any future
race audio session; no route in this owner can start such a session.

## Actual lifecycle and tree order

- Cold2A3C10: requestedPage90 then Main entry74. Entry owns profile mode, prompt
  mask0, exact widget effects, empty debug-message state and native UI field3D8.
- Manager ctor+17C begins1. First real update clears it and sets lifecycle2;
  second runs the source state2 entry sequence. It does not call entry74 twice.
- State2: managerEntry84 reaches the proven empty Main function2A4330, then
  beginTransition7C(entry), then recursively hides the page input container.
- OpeningAC: actual widget/group effects, then source-gated loop music9.
- Shared aperture geometry advances only at render48 using Main's previously
  saved update delta. Crossing an endpoint does not settle until the next real
  render observes it. OpenedB0 sets prompt mask1 and actual group/widget state.
- The next manager update polls the real aperture, shows the input container,
  and becomes idle. No timer, render count, GPU fence or fake completion opens it.
- Tree update: manager state switch; ordinary widget updates; current Main update
  and saved delta; page children; OnTop updates; actual scene particle advance;
  stationary camera point; actual fire emit. Scene children attach AFTER the
  entire Pod creation1CCBA0 returns, at1CF410 ->1CFAF0.
- Tree render: ordinary widget projections; Main background/arrows; shared two
  aperture quads; real base-controller prompts; OnTop; actual scene fire packets;
  authored material registry drain. The scene fog/reset/proxy nodes occur
  before the particle leaf and are represented by the concrete source-prepared
  render-state boundary, including their camera clip/color writes. The collector
  retains source FIFO per leaf. This is not UI-first/particles-last GPU submission:
  drain resolves real groups.

Pause contract: Main1E6290 has no pause branch and always saves the supplied
source delta. A native paused update must therefore supply delta0; nonzero paused
delta is rejected before mutation. The real source manager switch can still run
at zero delta, but aperture geometry cannot consume paused elapsed time. Paused
renders must not advance material animations, and the application source clock
must exclude paused duration on resume. Particle/audio pause gates remain real.

The sole camera has immutable authored pose, viewport640x448, zoom1, no source
scene camera animations and no debug override. Static source2363E0/114AE0/236DD0
confirms these conditions leave pose unchanged. Render-time fog DOES change its
clipping planes. The typed scene render-state helper proves later scene fog/proxy
precedence and prepares the same camera at its particle boundary: near1/far4000,
fog start3936/end4000, constant33.z=4000 and33.w=-3.984375. The cold pose is retained
without re-normalizing; projection Z and combined matrices are rebuilt through
source equations for that boundary. These are snapshots of one camera, not two
camera entities. Source lighting RGB49 and canvas offsets1728,1824 are internal.
Unconsumed camera motion history, audio-listener state and spatial visibility
handles are not claimed recovered. Global228bit2 is rejected; no pose/viewport/
zoom mutator is exposed.

## Input and unavailable routes

Host input edges map to original enum2(previous),3(next),4(confirm),6(back).
No generated inputs are used. Original input indices0..15 can reach Movement3
through the actual container, but its real selection snapshot remains decisive.
Actual Main Group1 association count0 does not select Movement3.

Page1E5E00 returns1 whenever called, including popup/debug/controller gates and
unhandled inputs. The session preserves this before snapshot/leaf delivery. Main
back6 is genuinely unhandled, so it does not exit, start driving or open a host UI.
Host window quit calls the separate `requestQuit()` method.

Previous/next mutate real controls and call source sound3/4 when enabled. Confirm
produces the real requested slot/mode/route shape, then returns unavailable while
Rider/Options/Gallery owners are absent. A missing second controller returns that
specific status without inventing a popup. Neither unavailable result changes
profile selection, mode/player bits, lifecycle or application queue. Accepted
confirm audio/save-slot/route commit is deliberately not claimed.

Source267BE0 is NOT an established idle reset. Its exact early return is admitted
only when shared source transportE00 is-1. An active pending operation remains
unsupported. Static source shows opcode20/21 and a0x4508-byte transport buffer;
its broader owner is not guessed. This is a bounded cold-start admission.

## Other staged source changes

1. `original_ui_transition_owner50kg`: adds separate `bindInitialMain`, pending
   restart flag and lifecycle2 case; legacy `bind` keeps already-entered semantics.
2. `original_ui_aperture_owner50kg`: fixes exact Pod prefix construction for the
   authored PageFile(main.dat -> UI/UIMain/main.dat). Adds explicit entry-only
   admission, whose exit request and exit-completion predicate reject before any
   exit mutation. Existing full `bindMain` still requires actual exit object+440.

No missing+440 ScreenText is silently certified. A future destination owner must
supply its real source exit object and effects before full exiting is admitted.

## Current integration boundary

The native entry worker owns real host input events, application clock and dt,
window/device lifetime, audio-device pumping and final GL submission. `render`
publishes a draw only after the complete collector join succeeds; `draw()` returns
null after an incomplete attempt. Original page packed clear color is retained:
RGB70,10,10 ->0x00460A0A with source truncation and no halving. Presentation alpha
is the bounded native renderer policy, not recovered source framebuffer alpha.

`render(clock,diagnostic)` takes no caller-supplied fog/color/camera certification.
Cold bind invokes `prepareOriginalUiMainParticleRenderState50KG` from the exact
scene, camera and lighting owners, and passes its complete typed result into the
collector. This includes source-packed normalized fog RGB; the scene authoring
color is not substituted independently. The final helper dependency must be
integrated alongside the session and collector before any central verification.

Failure after a real callback does not claim transaction rollback; the faulted
owner stays unavailable. Initial callback failures retain the owner so a caller
cannot retry against partially mutated shared resources. Recreate the actual
resource/profile owner deliberately instead. Fire receipts retain source RNG
draws, first-update skip and explicit native undefined-carry-policy information.

## Source evidence

The adjacent evidence directory contains static word-level decoding of original
SLUS_202.68, SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
The decoder only reads bytes; it does not interpret/execute guest instructions.
Key ranges:2A3C10 initial selection,2A3C80..2A41B0 manager lifecycle,
1CCBA0..1CCF80 child creation,1E5D10..1E6410 Main entry/input/update,
1E68A0..1E6A60 music/prompt hooks,1864B0/186EF0 profile reset,267BE0 pending
transport. Source provenance addresses never become native executable pointers.
