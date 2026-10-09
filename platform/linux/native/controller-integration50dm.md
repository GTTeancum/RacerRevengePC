# Explicit Linux controller path50DM

Source-only integration wiring; no joystick device has been opened, polled or
validated by this work. Enable RR_BUILD_LINUX_JOYSTICK to compile the50DK/50DL
library. It has no device-discovery or permission-changing behavior.

Order for the owning single-threaded client:

1. Caller initializes original49D preset state and selects the desired preset.
   Resolve action row and configuration slot separately. Never use the old
   diagnostic's all-minus-one flag-button placeholders with the real provider.
2. Caller explicitly opens the intended joydev path through50DK. Supply all
   four axis calibrations and sixteen numeric button bindings for50DL; mappings
   may reuse raw fields. Disabled bindings are explicit, not missing data.
3. Poll a bounded event batch. Inspect terminal/partial progress. A closed or
   faulted transport cannot supply a current native packet. Do not pretend
   consumed kernel events were rolled back or replace unobserved fields by zero.
4. Map the resulting connected snapshot with50DL using a caller-supplied packet
   template. On failure keep the last published packet untouched and follow the
   application's explicit unavailable-input policy; do not advance from stale
   data simply because an output object still exists.
5. Feed a successful mapped packet to50CX with exactly the initial48Y descriptor
   layout and16 active codes. Consume/copy its entire ordered transition batch
   through48Z before the next sample overwrites it. If queue insertion fails,
   resolve the pending batch before sampling again. Bind manager record views
   to ingress.buttons().records, not a copy.
6. The application must execute the existing event dispatcher with real effect
   bindings. Missing callbacks are a visible boundary, never no-op success.
   After that, prepare one FrameReader49K with the already-ready49I factor and
   real resolved action codes. Consume it while the nonmoving ingress owner is
   alive, before another sample/prepare call, through readVehicleFrame49W.
7. Carry the same resolved configuration bit into the later binding phase. Do
   not run the composed48Y player-control step as well; that would duplicate
   conditioning/filtering already performed by the canonical frame pipeline.

50DK is raw Linux transport,50DL is documented host conversion policy,50CX
retains original native sampling/held-query behavior. These are separate
contracts. No physical button labels, calibration, clock units, camera/menu
policy, original hardware transport or successful live controller claim is
inferred from having this source code.
