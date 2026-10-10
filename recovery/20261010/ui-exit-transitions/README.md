# Shared UI exit ScreenText and native lifecycle reconstruction

Status: source-only staged candidate. No active repository files were edited.
No build, test, syntax check, sanitizer, native project parser/importer, gameplay,
guest CPU/VU execution, or GPU execution was run. Independent bounded Python
standard-library reads/decompression of original data and static instruction
text inspection were performed. Parent owns integration and the next permitted
centralized validation window.

## Delivered

- New `src/game/original_ui_exit_text50kg.{h,cpp}`: concrete shared page+440
  ScreenText setup, authenticated original localization/font association,
  immutable native glyph geometry, and actual RenderText preparation state.
- `src/assets/original_ui_screen_assets50kg.{h,cpp}`: retains immutable original
  CEL/PST member identities/ranges from the same successful decode. No source
  byte pointer is retained. `sourceProvenance()` exposes this real association.
- `platform/linux/native/original_ui_main_draw50kg.{h,cpp}`: loads the actual
  +440 owner using existing denmrk16 resources and queues its actual text packet
  at the source endpoint, before aperture rectangles, through denmrk16Top and
  the existing original registry drain.
- `platform/linux/native/original_ui_main_session50kg.{h,cpp}`: concrete Main
  B4 prompt/widget/music effects, source-no-op B8/BC hooks, full aperture bind,
  and the optional external lifecycle/proposal boundaries described below.
- `src/game/original_ui_transition_owner50kg.{h,cpp}`: shared
  `bindInitialPage(screen,page,callbacks)`; `bindInitialMain` delegates to it.
  Destination admission remains mandatory. Terminal queue behavior is unchanged.
- `platform/linux/native/original_ui_main_audio50kg.{h,cpp}`: narrow concrete-idle
  fade no-change support, from source234480. Fresh preimages include parent's
  Options event5 addition. Event allowlists, six voices, fade interpolation,
  playback, source PCM and output-device behavior otherwise remain unchanged.

Add new translation unit `src/game/original_ui_exit_text50kg.cpp` to the real
native source list. There are no new dependencies, host paths, asset substitutes,
readiness callbacks, or executable guest addresses. CMake/host changes are owned
by the parent and are not staged here.

## Exact original +440 behavior

The source call at1D75A4 is **RenderText**, not object deletion, hiding, a timer,
or an arbitrary completion effect. The original function2E0170 asserts that
it runs during Render3D, prepares actual ScreenText geometry/packets via2E0F50,
then submits each active packet through246760 at2E02B0. It is not a trace sink.

Shared constructor1D5EA0 calls ScreenText constructor2E02F0 on page+440 at1D5EEC.
Shared page setup1D63B0 constructs it at1D6BB0 with:

- Font lookup literal `denmrk16` at3AEDC0, called1D6AFC
- Material literal `denmrk16Top` at3AEE18, passed1D6BA4
- Original localized entry89, loaded from the text table at offset2C8
- Capacity10 at1D6BAC, one allocated packet of at most36 glyphs
- Color0x00CCCCCC at1D6C38, source alpha255 from2E00D4
- Position481,384 at1D6C84..90, no justification/centering override
- Source scale1, angle0, assigned depth1 from2E00DC..EC

Independent authenticated LOC data inspection found entry89 is `STR_LOADING`,
text `LOADING...`, exactly10 bytes and source maximum10. The native source does
not contain the label string: it copies `english.at(89)->text` from the already
authenticated localization owner. See `evidence/exit-localization.json`.

`prepare()` admits only proven shared-constructor users Main4/1, Options12/1,
Rider5/1 or5/2, and Course6/1. It authenticates the correct page archive,
follows its actual ui.dat PageFile, requires PageInfo and the exact material
member, and checks font resource provenance against UIFONTS ordinals2/3
(denmrk16 CEL/PST). Merely matching atlas dimensions or caller image IDs fails.
Supporting the shared exit object does not admit any destination page.

`projectRender2E0170()` accepts only the aperture's exact closing endpoint
notification1D75A4->2E0170, while its pre-clamp top>=224/bottom<=224 state remains
visible and input268 is false. It publishes real glyph/image/material/color
geometry and updates concrete source packet/position state. Parent collectors
must place it at that actual draw boundary, preflight capacity, and enqueue it
before either aperture rectangle. The Main collector now does so. The ten
original glyphs fit one actual ScreenText packet. Packet RGB is102/102/102,
alpha63, assigned depth1; source registry order determines the eventual pass.

The source aperture emits this call once on the closing endpoint frame when
manager lifecycle is3. It then clamps and changes to closed. The text is not
invented as a persistent loading screen on later frames. Page exits/lifecycle4
and Course exits/lifecycle5 do not emit it. The next real manager update polls
closed and only a subsequent update queues the deferred application request.

Original embedded-object destruction is represented by ordinary native owner
lifetime: its pinned text/geometry/images are released when the real page
resource owner is retired. RenderText is never mistaken for that teardown.

## Main effects and source gates

Main B4 clears promptMask42C, performs the existing real closingB4 widget phase,
then calls the concrete music fade1.0s, in source order1E696C..1E6A34. Main B8
1E6A50 and shared BC1D7A00 are actual return-only functions. Their no-effects
implementations are source-backed, unlike +440 rendering.

Source234480 returns with no effects when no stream handle is active; B4
ignores its return value. The native audio helper now handles only its concrete
idle state (`!ownsMusic_ && !mixer_->active(musicVoice)`) as a successful
no-change command. Foreign active/stale owned voice states still reject. This
covers the genuine Main music-start source gates44==4 or48==2 without inventing
music. Native true means a handled command, never a fabricated original return1.
The admitted native fade policy remains distinct from original IOP timing.

Main's default standalone session now binds full aperture hooks, but still
admits no destinations. Original labels remain visible and unsupported confirm
leaves route/profile unchanged. A complete close owner alone is not a completed
Main-to-Options or Main-to-Rider frontend route.

## Optional external lifecycle contract

`OriginalUiMainSessionCold50KG::externalTransition` is an optional shared_ptr
to the actual coordinator-owned lifecycle owner for this native source manager.
Without it, standalone Main bind/tick/input behavior remains as before. With it:

1. Cold bind rejects an already-bound external owner before page/resource side
   effects. It constructs resources/page owners but does not bind, reconstruct,
   clone, restart, or tick the manager. The unused internal owner stays unbound.
2. After concrete source/destination owners exist, the coordinator binds the
   actual owner with `bindInitialPage(4,1,callbacks)` and forwards its real
   synchronous notifications to `notifyTransition(event,diagnostic)`.
3. The wrapper requires event route/phase/lifecycle to match the real retained
   owner's current snapshot. `pollCompletion(route,phase)` reads the actual
   aperture and validates the same owner; it never accepts a supplied completion.
4. Per active frame call `prepareUpdateAudio(update,diagnostic)`, then the
   actual owner's `tick()`, then `update(same update,diagnostic)`, then render.
   This preserves existing audio-before-manager-before-child ordering. Audio
   preparation is one-use; child update does not advance it again. Duplicate
   preparation, consumed-argument mismatch, or rendering before consumption
   faults the session rather than retrying already-consumed audio. The update
   argument includes actual dt, pause state, and held debug input snapshot.
5. Render obtains lifecycle from that same retained owner. No arbitrary caller
   snapshot, readiness mask, destination certificate or callback can replace it.
6. Each real source manager gets one authoritative owner. After queue acceptance
   the old owner remains terminal. The parent activates a fresh destination
   manager/owner only at the deferred application safe boundary, then retires
   old owners. There is no immortal lifecycle snapshot rebound or session cycle.

External-owner identity/lifetime review: the cold shared pointer is copied into
private session storage and never reassigned. `externalLifecycleOwner()` returns
that exact retained object; all input/notification/poll/update/render snapshots
come from it. Both manager and session are noncopyable and have no reset/rebind
API, and the external path never binds or ticks its dormant internal manager.
Resetting the caller's shared pointer does not destroy the retained manager.
Manager callbacks borrow the coordinator context: that context and its concrete
page owners must outlive all manager calls, and no callbacks may run after the
coordinator/session teardown begins. A shared pointer to the manager does not
itself retain its borrowed callback context. The coordinator must create one
fresh owner per source manager and preserve serialized access; the cold guard
cannot certify future callback-context lifetime or prevent a caller from
sharing one still-unbound owner across unrelated cold sessions.

The parent must perform actual source child update after every admitted manager
switch case, including the initial restart tick and queued exit case. No frame
may publish after any source callback/child/audio failure. Bank/session teardown
must remain deferred until callback/tick/frame consumers have returned.

## Input proposals and commit ordering

In external mode, a valid original confirm can return `routeProposed` with a
`routeProposal`. It contains the exact original decision plus source session,
input serial, and an observed lifecycle for diagnostics. It is unaccepted and
writes no source mode/player bits/saved slot, request state, or SELECT audio.
Second-controller-required and popup/input gates do not become proposals.

`routeProposalCurrent(proposal)` rejects a foreign/stale proposal, changed
selection/control enable/block state, non-idle phase, non-open aperture, missing
input visibility, or lost source application/profile state. Another input,
audio/update/render boundary, or lifecycle notification invalidates it. The
proposal's snapshot is not used to certify readiness.

The coordinator alone checks the concrete completed destination, then calls the
real manager `requestMain(proposal.decision)`. Its existing commit callback owns
mode/player-bit writes for slots1..3. After request acceptance, the coordinator
writes the actual savedSlot6E0 (source1E61A4) and immediately calls
`notifyRouteAccepted(proposal,diagnostic)` before another tick. That method
checks the actual request/current selection/profile writes and emits SELECT1
exactly once. It commits neither route nor profile. In particular Main slot4
produces screen12/page1 without a mode/player-bit write. `pageAccepted` still
means the original Main callback's unconditional input return1; it is not route
admission and remains separate from the proposal result.

## Evidence and integration

Primary evidence is in `evidence/`: shared construction/setup; ScreenText
allocation/render/chunk/position/release; verified Rider/Course/Options shared
constructor calls; the inactive-stream fade branch; authenticated entry89.
Existing sibling manager-lifecycle and Main-aperture-hook static evidence
supplies the unchanged manager and B4 ordering. Original ELF SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

`files/` holds complete candidates. `preimages/` holds exact copied existing
files and `.absent` markers for new files. `manifest.json` and
`integration.patch` record candidate/preimage identities and exact replacements.
Reconcile changed parent files by preimage before integration; never blindly
overwrite later host/session/audio changes. Staging scripts are text construction
records, not executable validation and not required for integration.

Parent validation still needs compilation and real standalone/external frame
coverage, both muted/enabled, source entry and exit, same-frame proposal expiry,
foreign/repeated proposals, changed selections, duplicate/mismatched audio
preparation, exact single +440 packet/registry ordering, failed/lost GPU owners,
and repeated Main-to-Options-to-Main with fresh per-manager ownership. None of
those outcomes is claimed here.
