# Rider mixed widget manager, source-only candidate

Two NEW files at files/src/game; no shared modifications. Requires the separately
frozen text-empty-default shared reader correction for authentic text65/66/73.
No builds, syntax checks, project parsers/importers, tests or gameplay were run.
Root owns all integration and validation; CPU binding is not render readiness.

## Actual population and shared page ownership

OriginalUiRiderWidgets50KG pins authenticated Rider archive and uses the existing
OriginalUiRiderSelectionData50KG to bind rider.dat. Both factory page1/player0
and page2/player1 use that same authored PageNo1 file. The manager owns TWO
navigation states with one shared119-widget population, matching a screen's
shared widgets rather than cloning119 resources for each page.

It reads real Widgets.Dat through the shared DAT reader (preserve repeated keys),
then shared text/icon/movement readers. Actual Type decides each slot. Empty
means original null1CD70C: no fabricated UIWidget, no child attachment or image.
All nonempty siblings attach ordinary ascending then OnTop descending, preserving
existing source manager order. Newly created states start recursively hidden
(2A44E0), not guessed visible because a control references them. Every real page
control membership is checked; Empty membership rejects. Movement8 is registered
exactly once. Unused Overlay_Text65/66/73 remain genuine empty-text widgets, not
Empty type and not guessed racer names.

Text callbacks and source state use OriginalUiTextWidget50KG directly. Icon
geometry uses layoutOriginalUiIcon50KG; fonts/glyphs/materials/image animations
remain common resource/draw owners. Manager binds the initial genuine image for
icon rect defaults and preserves its cold size, source depth and packet layout.
Any animated icon's selected frame must still be resolved by its actual material
animation owner at draw; this manager does not declare its initial frame static.

## Ordered callbacks and dynamic scenes

commitNavigation(player,next,commands,hooks) expands each original control's
widget list in authored order, retaining duplicates and intermediate skipped
riders' deselect/select sequences. It preflights the COMPLETE expanded list
before effects. The external synchronous adapter receives that full list, and
must establish the actual cold/model/traversal resources for every dynamic call.
The manager then executes text/icon/movement effects and dynamic callbacks in
EXACT mixed order. No dynamic calls are collapsed, deferred or acknowledged by
a readiness flag. Only after every callback succeeds is next navigation stored.

For a User_Rotate callback, the adapter must join the actual pod scene owner,
loading its authenticated asset/graph, performing observer+attachment sequence,
selection/deselection and any source cleanup. For Soultree_Character it must join
actual character graph/playback/IK/observer lifetime. The in-progress scene API
is ../scene/files/src/game/original_ui_rider_model_scene50kg.h; no unstable scene
header is copied or modified here. Adapter apply returns observed selected/shown
state only AFTER the real callback completes. Its kind/onTop cannot change.

If preflight rejects, no widget effect runs. If a synchronous callback fails
after earlier effects, navigation is NOT published and manager becomes poisoned.
This is fail-stop, NOT a false rollback of scene loads, RNG, observer or GPU
side effects. Parent must drain/discard/rebuild via actual lifecycle recovery.
Never retry the same partly executed batch or claim destination readiness.

apply() also accepts explicit direct source widget calls (e.g. appearance's
nonvirtual2A7A00). setEnabled goes to text.setControlEnabled, NOT its virtual
text enable/disable methods. Icons keep their authored selection visibility
flags; reset only shows; clearSelection only clears selected. Movement's own
select/deselect always show/hide (2AB4B0/2AB530), and has no drawable.

The manager does NOT conflate navigation publication with completion of page
entry/opening/closing. Parent controller owns exact timing of transform capture,
appearance lights/camera tint, dynamic labels, prompt mask42C, resetting selected
control, transform restore, deferred4EC, popups and sound. The existing Rider
selection plan aggregates some later callbacks; do not execute it prematurely
at an earlier hook merely because this manager can commit its commands.

## Separate input paths

Rider page raw events0/1 already invoke its specialized previous/next routines
1EB370/1EB1F0. Movement8 subscribes both controllers with left=-2,right=-1. Its
widget path is generic attached-group move through2AB400/410, followed by actual
manager virtual+84 request at2AB490. planMovement therefore uses shared
moveOriginalUiControl50KG on a private page navigation copy, returning ordered
commands and explicit notification request. It DOES NOT remap into another
Rider page event, apply page appearance, or guess manager callback completion.
The container supplies reached/pageEnabled/pageAccepted; selected is sampled
from widget8 at the call point, AFTER page handling. Parent must commit returned
callbacks before manager notification and resolve the real notification target.

IMPORTANT: rider.dat says NumberOfAssociations=0 although it has unused
AssociationID1=24 text. Shared page parser correctly honors the zero count. Do
not synthesize association24, enable Movement8, or route both paths to force
left/right behavior. A hidden widget isn't synonymous with unselected, so the
gate checks actual selected state, not a UI visibility heuristic.

## Dynamic text and live appearance

Text6,87,117,118,119 have actual dynamic capacities from Widgets.Dat (1x20,
1x20,2x40,2x40,1x40). Cold descriptors and their strings remain immutable as
required by the shared text state leaf. A separate live-string vector accepts
only original caller-produced lines within those capacities. All same source
text callbacks/state remain shared; projectText substitutes the live text pointer
and uses shared source positioning with the live descriptor view. No fallback
words, labels, UTF8 conversion or invented profile formatting is supplied.

Drain/discard borrowed text projections before replacing live strings. Measure
CURRENT bytes using the authentic font atlas and layoutOriginalUiText50KG, then
call projectText with actual font height/measured width/height, then generate
final glyph geometry at its returned original position. Do not retain cold empty
line glyphs or merely offset pixel-rounded geometry. Missing direct Text65/66/73
uses shared source empty-string correction, never local descriptor invention.

setTextPosition provides the source integer widget-position update, including
locked label4's CVT.W.S(320+panelY). setColor sends text through source shared
line-color/lock logic and icons through original RGB>>1 packet conversion while
preserving alpha. The caller must invoke actual1EB640 color changes and page
label formatting at their source phase; binding doesn't imply those happened.

## Remaining ownership boundary

Manager has real static text/icon/movement population and bounded synchronous
scene callback orchestration. It still requires actual dynamic adapter, page
controller lifecycle, dynamic label producer, cold font/material/image binding,
animated-icon clock, source projection/depth/scene render and sound/popup owners.
No destination admission or fabricated renderer is included. Parent centrally
validates the candidate and its exact integration order before marking any
portion of the original Rider screen complete.
