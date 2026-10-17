# Reuse adjacent material setup in remaining original HUD passes

UNTESTED source change. No builds, syntax checks, smokes or FPS measurements.
Base publication: 88edf271153d2a4cd507fe049d15be30eb109e66.

## Scope

Damage, TimeStats and SpeedBoost now repeat depthMask, source-blend uniform and
texture binding only for the first packet or a change in material ID.
PositionStats similarly guards its material-derived depth function/write mask,
source-blend/texturing uniforms and texture binding. All four passes still draw
every packet and triangle in original order, with every barrier and error gate.

## Admission and state lifetime

Each preparation rejects zero packet counts and invalid material indices/ranges.
Damage admits only material0; TimeStats admits two materials; SpeedBoost admits
12 and separately validates triangle versus quad shape. PositionStats admits
four materials and authenticates radar policy when material3 is used.

In both coherent-fetch and barrier-based rendering, each admitted packet reaches
a draw error gate before a later packet can reuse state. No shader/program,
texture selector/binding or depth-policy write occurs between these setup sites
except the proposed same-material setup itself. Texture lookup returns retained
metadata without callbacks. Existing entry borrowing/identity validation stays.
Each pass starts with packet0 and establishes its own state; no cache spans passes.

PositionStats material3 retains its distinct zero-map radar, ALWAYS/no-write
policy. Moving between radar and textured materials still executes the complete
setup because material IDs differ. SpeedBoost's indexCount computation and
three-real-vertex versus quad handling remain outside the guard and unchanged.
Owner order, geometry, alpha equations and final restoration are unchanged.

Three setup API calls are omitted per adjacent equal-material packet in Damage,
TimeStats and SpeedBoost; five in PositionStats. Actual adjacent incidence and
FPS impact remain unmeasured. A successor may validate radar/text transitions,
triangle needles, all HUD material paths, both fetch modes and failure cleanup.

## Source-reachable repeated packets

These guards are not justified solely by hypothetical owner inputs. Current
owner construction does not coalesce the following separate source submissions:
Damage's leaf emits objects0/1/2 as separate material0 packets; each visible
TimeStats row appends both side objects as material0; SpeedBoost's heat sequence
emits multiple objects below7 that map to material0, then stable-sorts by leaf;
PositionStats with position preference enabled emits five fixed material0
objects separately. These establish source-reachable equal-material adjacency,
not measured occurrence or timing for the latest smoke frame.
