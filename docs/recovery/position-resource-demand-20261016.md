# PositionStats191AF0 canonical texture demand

This bounded native display join invokes the exact two demand-floor operations
from retained source words in `position-draw-source-20261015.txt:136–146`.
It changes no geometry, preference, visibility, ALL8 control, or submission order.

1. 191B00 calls ScreenTexture+270 getter2DF8A0. The adapter resolves its cold
   texture-table token and requires the same fixed_[0].atlas canonical object.
   191B0C loads10; C.LE.S10,current skips191B28 if already at least10.
2. 191B3C calls282210 on the actual Application328 font registry with the
   eurostileboldbold13ingame name (3A8E60). A null result skips the second
   floor. A present entry must be the same retained text font; its canonical
   texture token is resolved and compared with texture281F90. 191B88 performs
   the same conditional floor10 store.
3. 191B90 calls141C30. This detached native resource Entity has no children;
   no invented parent/global traversal is performed or claimed.

The texture method is private, friended only to the concrete retained HUD
adapter alongside the existing TimeStats/SpeedBoost/Damage accessors. It admits
only the two original store PCs, live referenced nonaggregate textures, and the
same finite ordinary demand domain as the existing native HUD demand methods.
Earlier writes remain committed if a later dependency rejects. It never resets
shared demand, manufactures residency, or treats uploaded pixels as a source
resource-demand receipt.

## Scheduling and lifetime

Cold initialization resolves the two real tokens after the canonical shared
material population; it adds no resources, references, archive mounts or new
population. Existing fixed texture references and Application font Entry pins
retain both resources. At native eligible-display preparation the adapter first
validates the current canonical material map, then services stats followed by
font, before appending any PositionStats packets. This matches the existing
native TimeStats194E20 scheduling convention. It deliberately does not claim
original global resource-demand traversal before predraw: the native path still
prepares CPU text/radar before this eligible-display service, and hidden frames
remain outside that service. Full retail demand/reset/upload scheduling would
require its own authenticated parent/global traversal owner.

The Application registry already exists through genuine190E64 staging and
cannot close while the retained ScreenTexts hold their Entry. fonts328() thus
borrows the existing process registry. Per-frame lookup and token resolution
perform no texture acquisition or decoder operation. The HUD adapter remains a
weak borrower of the resource owner; no new display lifetime cycle is added.

## Damage18F470 inspection

The active Damage adapter already resolves the genuine +50 atlas lease and
calls demandDamage18F470 at its original18F4A8 floor store. It runs after native
race/UI visibility and canonical map admission, before the state40 early exit
and complete18F680 health/proximity leaf. No missing demand call was found.
Its stale comment saying there was no caller is corrected; behavior is unchanged.
As above, this is explicit native eligible-display scheduling, not the full
original application demand traversal or proof of GPU residency.

Base a02afd8. Source inspection only: no builds, tests, syntax/sanitizer checks,
gameplay, decoder execution or guest execution. Root owns integration/validation.
