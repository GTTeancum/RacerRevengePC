# Native Main host-loop wiring

The concrete entry change is owned by the native entry worker. The adjacent
examples/main_session_frame50kg.h is an actual bounded call-order adapter using
native session and window APIs, not a software test or a substitute frontend.
Its render(clock,diagnostic) signature owns the internal authenticated fog join.

Cold order:

1. Read original archive bytes under the user-selected game-files root. Load
   OriginalUiArchive50KG with exact IDs appStart, localization, overlays, fonts,
   mainMenu, controllerPopup. Each authenticates exact expected bytes. Resolve
   OriginalUiEnglish50KG from localization using its existing authenticated API.
2. Establish the actual native video/GL context owner. Fill OriginalUiMainColdInput
   with owning appStart/mainMenu/fonts/controllerPopup/overlays pointers, English,
   actual material-runtime descriptor and original640x448 video-mode values plus
   genuine shared video lifetime. resources.load is a cold CPU call.
3. Fill MaterialTextureGlRequest50EB from the actual current GL context/functions
   and resources.samplers(); create the exact resources.textures() bank. Retain
   the owning GL publication, not raw GLuints or a no-op shared-pointer alias.
4. Allocate the sole process profile/application subsets. For an imported save,
   use actual fields. For deliberate new native startup, initialize its backing
   state, call resetOriginalUiMainProfileFields50KG, and mark that owner's known
   state only after setup. Keep the application RNG and fire serial shared with
   every eventual consumer. Any clock seed is an explicit native startup policy.
5. Explicit mute: audioMode=muted, audio=nullptr. Enabled: create one real dedicated
   frontend mixer/voice bank and concrete OriginalUiMainAudio50KG, prepare from
   the original AUDIO root, retain its mixer/device lifetime, and use its own
   mixStereo16 gate. Never run a competing race music writer on that mixer.
6. Build OriginalUiMainSessionCold50KG from those same owners; session.bind reads
   the actual manager camera and Main scene, creates real fire state and executes
   source cold page entry. No Rider/Course/Options/Gallery capability is granted.

Steady loop:

- Poll real host events. Map Up to2, Down to3, Cross/confirm to4, Triangle/back to6
  using the original controller enum. Do not synthesize navigation for captures.
  Keep controller index and real second-controller presence. Input0..15 passes
  through the actual source page/container/Movement3 delivery rules.
- SDL/window close or explicit application quit calls session.requestQuit(); this
  is distinct from original back6, which does nothing in Main.
- Pass a real source update delta, in[0,1]. Do not clamp, manufacture fixed ticks
  or catch up hidden renders. A rejected long sample remains a disclosed pause/
  scheduling issue. For deliberate pause, supply delta0 and paused=true; freeze
  animation advancement and source milliseconds so resume cannot jump time.
- session.update executes one real manager switch plus the actual tree order.
  session.render prepares all Main UI and real scene contributions, then drains
  authored material groups. No world/AI/vehicle/gameplay update belongs here.
- Only on success fetch session.draw and call the typed VehicleWindowGl50DS
  submitOriginalUi(*draw,actualDrawableWidth,actualDrawableHeight,diagnostic).
  Existing host swaps normally after success. Never call submitHostMenu as a
  failure fallback or claim a failed frame was presented.
- Log unavailableDestination/secondControllerRequired receipts. They remain real
  Main selections; do not fabricate original popup artwork or start a native race.
- Pump enabled frontend audio through its concrete mixStereo16 gate. In muted
  mode there is no PCM transport; keep suppressed receipts in capture provenance.

Shutdown:

Stop loop and audio-device pumping. requestQuit pauses frontend PCM consumption;
reset the concrete audio owner before its mixer/storage/device disappear. Destroy
session/draw borrowers before closing its MaterialTextureGl50EB publication on
its original current context/thread. Check close diagnostics; do not destroy the
host context while unresolved GL resources remain. Release resources/archive/
profile/application owners only when no borrower remains.
