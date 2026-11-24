# Oovo IV anti-gravity exit

Vengeance uses the original N64 racer’s recovery state when a pod crosses a ZeroGravity Off surface. The following addresses are from the USA N64 ROM. The roll calculation is shared by human and computer-controlled pods.

| N64 address | Behavior |
| --- | --- |
| 0x80066C78 | A pod already in flight clears the flight flag and enters recovery. |
| 0x8006601C–0x80066034 | Surface alignment takes one eighth of the bank error. The recovery flag bypasses the height-based attenuation. |
| 0x8006B0D8–0x8006B0EC | The recovery branch multiplies the surface-roll correction by ten and stores the roll rate. |
| 0x8006B2A0–0x8006B2CC | The orientation step multiplies that rate by elapsed seconds. |
| 0x80067270–0x80067294 | A ground-ray miss returns the upright world normal. |
| 0x8006C648–0x8006C67C | Ground distance below 12 source units clears recovery. |

For a constant road bank, the combined gain is 1.25 per second. That removes half the angular error in approximately 0.55 seconds and 90 percent in approximately 1.84 seconds. Track curvature, collisions and landing can change the measured interval. The port keeps hover contact and applies the recovery bank correction during the same physics step.

## The first grounded frames

The USA ROM follows two different paths on consecutive updates at touchdown.

| N64 address | Behavior |
| --- | --- |
| 0x8006B63C–0x8006B694 | Outside flight, refresh world gravity, measure the current orientation against it, and save the measured angles before computing corrections. |
| 0x800670DC–0x8006713C, 0x8006729C–0x800672BC | The ordinary ground ray follows gravity from two units above the racer. It returns the hit normal and subtracts those two units from the hit distance. Magnet surfaces have a separate direction rule. |
| 0x8006C4D4, 0x80067B3C | The full orientation path calls surface alignment. Its bank increment is one eighth of the error at full ground influence. At intermediate hover heights the original attenuates it, and above the hover range it eases toward world level. Steering and manual lean contribute separately. |
| 0x8006C4DC–0x8006C508 | Subtract the saved initial angles to obtain this update's angular increments. |
| 0x8006C50C–0x8006C548 | Recovery dispatches to the spline orientation routine. Once flight and recovery are clear, the normal turn routine at 0x80067C70 receives those increments. |
| 0x80067D90–0x80067DB8 | The full grounded path constructs a rotation around the forward axis using the roll increment directly. The recovery-only factor of ten and seconds multiplier no longer apply. |

The distance test runs **after** the orientation dispatch. Crossing it changes the next update's routine. An unfinished bank continues to converge on subsequent updates. At full ground influence and a steady target, a fixed one-eighth step halves the remaining error in just over five updates.

The port uses the N64 ground-ray direction and touchdown threshold on the ordinary Oovo IV exit. During the handoff to Revenge ground handling it applies the full-ground roll fraction at the port's existing 30 Hz reference cadence, then releases the small residual to normal terrain following. That cadence conversion and the residual cutoff are port integration choices.

In the Vengeance Time Trial replay, the first grounded update measured 35.89 degrees of remaining roll. At updates 16 and 32 it measured 13.71 and 4.92 degrees. Update 43 measured 0.23 degrees at 16 ms per update. Native framebuffer sequences at map nodes 194 through 197 and 199 show the exit and the subsequent straight.
